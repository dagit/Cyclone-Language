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
short* last_plus_one; } ; struct _tagged_ptr2{ struct _xtunion_struct** curr;
struct _xtunion_struct** base; struct _xtunion_struct** last_plus_one; } ;
struct _tagged_ptr3{ struct Cyc_Yyltype* curr; struct Cyc_Yyltype* base; struct
Cyc_Yyltype* last_plus_one; } ; struct _tuple18{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u]; struct
Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ; extern char
Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char* tag; } ;
extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
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
typedef void* Cyc_Position_Error_kind; static const unsigned int Cyc_Position_Lex=
0; static const unsigned int Cyc_Position_Parse= 1; static const unsigned int
Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{
struct _tagged_string source; struct Cyc_Position_Segment* seg; void* kind;
struct _tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse( struct Cyc_Position_Segment*,
struct _tagged_string); extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; extern void Cyc_Position_post_error( struct Cyc_Position_Error*);
typedef struct _tagged_string* Cyc_Absyn_field_name_t; typedef struct
_tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t; typedef
struct _tuple1* Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1* Cyc_Absyn_typedef_name_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; typedef void* Cyc_Absyn_Scope; struct Cyc_Absyn_Tqual;
typedef void* Cyc_Absyn_Size_of; typedef void* Cyc_Absyn_Kind; typedef void* Cyc_Absyn_Array_kind;
typedef void* Cyc_Absyn_Sign; struct Cyc_Absyn_Conref; typedef void* Cyc_Absyn_Constraint;
typedef void* Cyc_Absyn_Bounds; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_FnInfo;
struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_XTunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_XTunionFieldInfo; typedef void* Cyc_Absyn_Type; typedef void*
Cyc_Absyn_Funcparams; typedef void* Cyc_Absyn_Type_modifier; typedef void* Cyc_Absyn_Cnst;
typedef void* Cyc_Absyn_Primop; typedef void* Cyc_Absyn_Incrementor; typedef
void* Cyc_Absyn_Raw_exp; struct Cyc_Absyn_Exp; typedef void* Cyc_Absyn_Raw_stmt;
struct Cyc_Absyn_Stmt; typedef void* Cyc_Absyn_Raw_pat; struct Cyc_Absyn_Pat;
typedef void* Cyc_Absyn_Binding; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_Fndecl;
struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl;
struct Cyc_Absyn_XTuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
typedef void* Cyc_Absyn_Raw_decl; struct Cyc_Absyn_Decl; typedef void* Cyc_Absyn_Designator;
typedef struct _xtunion_struct* Cyc_Absyn_StmtAnnot; typedef void* Cyc_Absyn_Attribute;
struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t; typedef struct
Cyc_Absyn_Tqual* Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t; typedef
void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t; typedef
void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t;
typedef struct Cyc_Absyn_XTunionInfo Cyc_Absyn_xtunion_info_t; typedef struct
Cyc_Absyn_TunionFieldInfo Cyc_Absyn_tunion_field_info_t; typedef struct Cyc_Absyn_XTunionFieldInfo
Cyc_Absyn_xtunion_field_info_t; typedef void* Cyc_Absyn_type_t; typedef void*
Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier_t; typedef void*
Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop_t; typedef void* Cyc_Absyn_incrementor_t;
typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_t; typedef struct Cyc_Absyn_Stmt*
Cyc_Absyn_stmt_opt_t; typedef void* Cyc_Absyn_raw_pat_t; typedef struct Cyc_Absyn_Pat*
Cyc_Absyn_pat_t; typedef void* Cyc_Absyn_binding_t; typedef struct Cyc_Absyn_Switch_clause*
Cyc_Absyn_switch_clause_t; typedef struct Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl_t; typedef struct Cyc_Absyn_Uniondecl*
Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Tunionfield* Cyc_Absyn_tunionfield_t;
typedef struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_tuniondecl_t; typedef struct Cyc_Absyn_XTuniondecl*
Cyc_Absyn_xtuniondecl_t; typedef struct Cyc_Absyn_Typedefdecl* Cyc_Absyn_typedefdecl_t;
typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield_t; typedef struct Cyc_Absyn_Enumdecl*
Cyc_Absyn_enumdecl_t; typedef struct Cyc_Absyn_Vardecl* Cyc_Absyn_vardecl_t;
typedef void* Cyc_Absyn_raw_decl_t; typedef struct Cyc_Absyn_Decl* Cyc_Absyn_decl_t;
typedef void* Cyc_Absyn_designator_t; typedef struct _xtunion_struct* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; static const
unsigned int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n_tag= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n_tag= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const unsigned int Cyc_Absyn_Static= 0; static const unsigned int
Cyc_Absyn_Abstract= 1; static const unsigned int Cyc_Absyn_Public= 2; static
const unsigned int Cyc_Absyn_Extern= 3; static const unsigned int Cyc_Absyn_ExternC=
4; struct Cyc_Absyn_Tqual{ int q_const: 1; int q_volatile: 1; int q_restrict: 1;
} ; static const unsigned int Cyc_Absyn_B1= 0; static const unsigned int Cyc_Absyn_B2=
1; static const unsigned int Cyc_Absyn_B4= 2; static const unsigned int Cyc_Absyn_B8=
3; static const unsigned int Cyc_Absyn_AnyKind= 0; static const unsigned int Cyc_Absyn_MemKind=
1; static const unsigned int Cyc_Absyn_BoxKind= 2; static const unsigned int Cyc_Absyn_RgnKind=
3; static const unsigned int Cyc_Absyn_EffKind= 4; static const unsigned int Cyc_Absyn_Signed=
0; static const unsigned int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{
void* v; } ; static const int Cyc_Absyn_Eq_constr_tag= 0; struct Cyc_Absyn_Eq_constr_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr_tag= 1; struct
Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1; } ;
static const unsigned int Cyc_Absyn_No_constr= 0; struct Cyc_Absyn_Tvar{ struct
_tagged_string* name; struct Cyc_Absyn_Conref* kind; } ; static const
unsigned int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b_tag= 0;
struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual* tq; struct Cyc_Absyn_Conref* bounds; } ;
struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt*
effect; void* ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_TunionInfo{ struct _tuple1* name; struct Cyc_List_List*
targs; void* rgn; struct Cyc_Absyn_Tuniondecl* tud; } ; struct Cyc_Absyn_XTunionInfo{
struct _tuple1* name; void* rgn; struct Cyc_Absyn_XTuniondecl* xtud; } ; struct
Cyc_Absyn_TunionFieldInfo{ struct _tuple1* name; struct Cyc_List_List* targs;
struct _tuple1* fname; struct Cyc_Absyn_Tuniondecl* tud; struct Cyc_Absyn_Tunionfield*
tufd; } ; struct Cyc_Absyn_XTunionFieldInfo{ struct _tuple1* name; struct
_tuple1* fname; struct Cyc_Absyn_XTuniondecl* xtud; struct Cyc_Absyn_Tunionfield*
xtufd; } ; static const unsigned int Cyc_Absyn_VoidType= 0; static const int Cyc_Absyn_Evar_tag=
0; struct Cyc_Absyn_Evar_struct{ int tag; void* f1; struct Cyc_Core_Opt* f2; int
f3; } ; static const int Cyc_Absyn_VarType_tag= 1; struct Cyc_Absyn_VarType_struct{
int tag; struct Cyc_Absyn_Tvar* f1; } ; static const int Cyc_Absyn_TunionType_tag=
2; struct Cyc_Absyn_TunionType_struct{ int tag; struct Cyc_Absyn_TunionInfo f1;
} ; static const int Cyc_Absyn_XTunionType_tag= 3; struct Cyc_Absyn_XTunionType_struct{
int tag; struct Cyc_Absyn_XTunionInfo f1; } ; static const int Cyc_Absyn_TunionFieldType_tag=
4; struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; static const int Cyc_Absyn_XTunionFieldType_tag= 5; struct Cyc_Absyn_XTunionFieldType_struct{
int tag; struct Cyc_Absyn_XTunionFieldInfo f1; } ; static const int Cyc_Absyn_PointerType_tag=
6; struct Cyc_Absyn_PointerType_struct{ int tag; struct Cyc_Absyn_PtrInfo f1; }
; static const int Cyc_Absyn_IntType_tag= 7; struct Cyc_Absyn_IntType_struct{
int tag; void* f1; void* f2; } ; static const unsigned int Cyc_Absyn_FloatType=
1; static const unsigned int Cyc_Absyn_DoubleType= 2; static const int Cyc_Absyn_ArrayType_tag=
8; struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual*
f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_FnType_tag= 9;
struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo f1; } ; static
const int Cyc_Absyn_TupleType_tag= 10; struct Cyc_Absyn_TupleType_struct{ int
tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_StructType_tag= 11;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType_tag=
12; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType_tag=
13; struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType_tag= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType_tag= 15; struct
Cyc_Absyn_TypedefType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Core_Opt* f3; } ; static const unsigned int Cyc_Absyn_HeapRgn= 3;
static const int Cyc_Absyn_AccessEff_tag= 16; struct Cyc_Absyn_AccessEff_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff_tag= 17; struct Cyc_Absyn_JoinEff_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_NoTypes_tag= 0;
struct Cyc_Absyn_NoTypes_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment*
f2; } ; static const int Cyc_Absyn_WithTypes_tag= 1; struct Cyc_Absyn_WithTypes_struct{
int tag; struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ; typedef
void* Cyc_Absyn_Pointer_Sort; static const int Cyc_Absyn_NonNullable_ps_tag= 0;
struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Nullable_ps_tag= 1; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const unsigned int Cyc_Absyn_TaggedArray_ps=
0; static const int Cyc_Absyn_Regparm_att_tag= 0; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; static const unsigned int Cyc_Absyn_Stdcall_att= 0; static
const unsigned int Cyc_Absyn_Cdecl_att= 1; static const unsigned int Cyc_Absyn_Noreturn_att=
2; static const unsigned int Cyc_Absyn_Const_att= 3; static const int Cyc_Absyn_Aligned_att_tag=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const
unsigned int Cyc_Absyn_Packed_att= 4; static const int Cyc_Absyn_Section_att_tag=
2; struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_string f1; } ;
static const unsigned int Cyc_Absyn_Nocommon_att= 5; static const unsigned int
Cyc_Absyn_Shared_att= 6; static const unsigned int Cyc_Absyn_Unused_att= 7;
static const unsigned int Cyc_Absyn_Weak_att= 8; static const unsigned int Cyc_Absyn_Dllimport_att=
9; static const unsigned int Cyc_Absyn_Dllexport_att= 10; static const
unsigned int Cyc_Absyn_No_instrument_function_att= 11; static const unsigned int
Cyc_Absyn_Constructor_att= 12; static const unsigned int Cyc_Absyn_Destructor_att=
13; static const unsigned int Cyc_Absyn_No_check_memory_usage_att= 14; static
const unsigned int Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod_tag=
0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; }
; static const int Cyc_Absyn_Pointer_mod_tag= 1; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual* f3; } ; static const int
Cyc_Absyn_Function_mod_tag= 2; struct Cyc_Absyn_Function_mod_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_TypeParams_mod_tag= 3; struct Cyc_Absyn_TypeParams_mod_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ;
static const int Cyc_Absyn_Attributes_mod_tag= 4; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; static
const int Cyc_Absyn_Char_c_tag= 0; struct Cyc_Absyn_Char_c_struct{ int tag; void*
f1; char f2; } ; static const int Cyc_Absyn_Short_c_tag= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; static const int Cyc_Absyn_Int_c_tag= 2; struct
Cyc_Absyn_Int_c_struct{ int tag; void* f1; int f2; } ; static const int Cyc_Absyn_LongLong_c_tag=
3; struct Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ;
static const int Cyc_Absyn_Float_c_tag= 4; struct Cyc_Absyn_Float_c_struct{ int
tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_String_c_tag= 5;
struct Cyc_Absyn_String_c_struct{ int tag; struct _tagged_string f1; } ; static
const unsigned int Cyc_Absyn_Null_c= 0; static const unsigned int Cyc_Absyn_Plus=
0; static const unsigned int Cyc_Absyn_Times= 1; static const unsigned int Cyc_Absyn_Minus=
2; static const unsigned int Cyc_Absyn_Div= 3; static const unsigned int Cyc_Absyn_Mod=
4; static const unsigned int Cyc_Absyn_Eq= 5; static const unsigned int Cyc_Absyn_Neq=
6; static const unsigned int Cyc_Absyn_Gt= 7; static const unsigned int Cyc_Absyn_Lt=
8; static const unsigned int Cyc_Absyn_Gte= 9; static const unsigned int Cyc_Absyn_Lte=
10; static const unsigned int Cyc_Absyn_Not= 11; static const unsigned int Cyc_Absyn_Bitnot=
12; static const unsigned int Cyc_Absyn_Bitand= 13; static const unsigned int
Cyc_Absyn_Bitor= 14; static const unsigned int Cyc_Absyn_Bitxor= 15; static
const unsigned int Cyc_Absyn_Bitlshift= 16; static const unsigned int Cyc_Absyn_Bitlrshift=
17; static const unsigned int Cyc_Absyn_Bitarshift= 18; static const
unsigned int Cyc_Absyn_Size= 19; static const unsigned int Cyc_Absyn_Printf= 20;
static const unsigned int Cyc_Absyn_Fprintf= 21; static const unsigned int Cyc_Absyn_Xprintf=
22; static const unsigned int Cyc_Absyn_Scanf= 23; static const unsigned int Cyc_Absyn_Fscanf=
24; static const unsigned int Cyc_Absyn_Sscanf= 25; static const unsigned int
Cyc_Absyn_PreInc= 0; static const unsigned int Cyc_Absyn_PostInc= 1; static
const unsigned int Cyc_Absyn_PreDec= 2; static const unsigned int Cyc_Absyn_PostDec=
3; static const int Cyc_Absyn_Const_e_tag= 0; struct Cyc_Absyn_Const_e_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Var_e_tag= 1; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple1* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e_tag=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_Primop_e_tag= 3; struct Cyc_Absyn_Primop_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_AssignOp_e_tag=
4; struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Increment_e_tag=
5; struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2; } ; static const int Cyc_Absyn_Conditional_e_tag= 6; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_SeqExp_e_tag= 7; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_UnknownCall_e_tag= 8; struct Cyc_Absyn_UnknownCall_e_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Absyn_FnCall_e_tag= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Throw_e_tag= 10;
struct Cyc_Absyn_Throw_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_NoInstantiate_e_tag= 11; struct Cyc_Absyn_NoInstantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Instantiate_e_tag=
12; struct Cyc_Absyn_Instantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Cast_e_tag= 13; struct
Cyc_Absyn_Cast_e_struct{ int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; static
const int Cyc_Absyn_Address_e_tag= 14; struct Cyc_Absyn_Address_e_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_New_e_tag= 15;
struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Sizeoftyp_e_tag= 16; struct Cyc_Absyn_Sizeoftyp_e_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e_tag= 17; struct
Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Deref_e_tag= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e_tag= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e_tag= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e_tag= 21; struct
Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp*
f2; } ; static const int Cyc_Absyn_Tuple_e_tag= 22; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e_tag=
23; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e_tag= 24; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e_tag=
25; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e_tag=
26; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_Tunion_e_tag= 27; struct Cyc_Absyn_Tunion_e_struct{ int tag;
struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ; static
const int Cyc_Absyn_XTunion_e_tag= 28; struct Cyc_Absyn_XTunion_e_struct{ int
tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_XTuniondecl*
f3; struct Cyc_Absyn_Tunionfield* f4; } ; static const int Cyc_Absyn_Enum_e_tag=
29; struct Cyc_Absyn_Enum_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; struct Cyc_Absyn_Enumfield* f3; } ; static const int Cyc_Absyn_Malloc_e_tag=
30; struct Cyc_Absyn_Malloc_e_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_UnresolvedMem_e_tag= 31; struct Cyc_Absyn_UnresolvedMem_e_struct{ int
tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e_tag=
32; struct Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Codegen_e_tag= 33; struct Cyc_Absyn_Codegen_e_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e_tag=
34; struct Cyc_Absyn_Fill_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt* topt; void* r; struct Cyc_Position_Segment*
loc; } ; static const unsigned int Cyc_Absyn_Skip_s= 0; static const int Cyc_Absyn_Exp_s_tag=
0; struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Seq_s_tag= 1; struct Cyc_Absyn_Seq_s_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Return_s_tag=
2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_IfThenElse_s_tag= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s_tag= 4; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s_tag=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s_tag= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s_tag= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s_tag= 8; struct Cyc_Absyn_For_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct
Cyc_Absyn_Stmt* f4; } ; static const int Cyc_Absyn_Switch_s_tag= 9; struct Cyc_Absyn_Switch_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Fallthru_s_tag= 10; struct Cyc_Absyn_Fallthru_s_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2; } ; static const
int Cyc_Absyn_Decl_s_tag= 11; struct Cyc_Absyn_Decl_s_struct{ int tag; struct
Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Cut_s_tag=
12; struct Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Splice_s_tag= 13; struct Cyc_Absyn_Splice_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Label_s_tag=
14; struct Cyc_Absyn_Label_s_struct{ int tag; struct _tagged_string* f1; struct
Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s_tag= 15; struct Cyc_Absyn_Do_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct _tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s_tag=
16; struct Cyc_Absyn_TryCatch_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;
struct Cyc_List_List* f2; } ; struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment*
loc; struct Cyc_List_List* non_local_preds; int try_depth; struct
_xtunion_struct* annot; } ; static const unsigned int Cyc_Absyn_Wild_p= 0;
static const int Cyc_Absyn_Var_p_tag= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const unsigned int Cyc_Absyn_Null_p= 1;
static const int Cyc_Absyn_Int_p_tag= 1; struct Cyc_Absyn_Int_p_struct{ int tag;
void* f1; int f2; } ; static const int Cyc_Absyn_Char_p_tag= 2; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; static const int Cyc_Absyn_Float_p_tag= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Tuple_p_tag= 4;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Pointer_p_tag= 5; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; static const int Cyc_Absyn_Reference_p_tag= 6;
struct Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Struct_p_tag= 7; struct Cyc_Absyn_Struct_p_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Tunion_p_tag= 8;
struct Cyc_Absyn_Tunion_p_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Tuniondecl*
f5; struct Cyc_Absyn_Tunionfield* f6; } ; static const int Cyc_Absyn_XTunion_p_tag=
9; struct Cyc_Absyn_XTunion_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_XTuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_p_tag= 10; struct Cyc_Absyn_Enum_p_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_UnknownId_p_tag= 11; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple1* f1; } ; static const int Cyc_Absyn_UnknownCall_p_tag=
12; struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; static const int Cyc_Absyn_UnknownFields_p_tag=
13; struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_Absyn_Pat{ void* r;
struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp*
where_clause; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment* loc; } ;
static const unsigned int Cyc_Absyn_Unresolved_b= 0; static const int Cyc_Absyn_Global_b_tag=
0; struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Funname_b_tag= 1; struct Cyc_Absyn_Funname_b_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Param_b_tag=
2; struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Local_b_tag= 3; struct Cyc_Absyn_Local_b_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Pat_b_tag= 4;
struct Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct
Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual* tq;
void* type; struct Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt*
region; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc;
int is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Core_Opt*
width; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structdecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Uniondecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Tunionfield{
struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_List_List* typs;
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_XTuniondecl{ void* sc; struct _tuple1* name; struct
Cyc_List_List* fields; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d_tag= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d_tag= 1; struct Cyc_Absyn_Fn_d_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d_tag=
2; struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Struct_d_tag= 3; struct Cyc_Absyn_Struct_d_struct{ int tag; struct
Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Absyn_Union_d_tag= 4; struct
Cyc_Absyn_Union_d_struct{ int tag; struct Cyc_Absyn_Uniondecl* f1; } ; static
const int Cyc_Absyn_Tunion_d_tag= 5; struct Cyc_Absyn_Tunion_d_struct{ int tag;
struct Cyc_Absyn_Tuniondecl* f1; } ; static const int Cyc_Absyn_XTunion_d_tag= 6;
struct Cyc_Absyn_XTunion_d_struct{ int tag; struct Cyc_Absyn_XTuniondecl* f1; }
; static const int Cyc_Absyn_Enum_d_tag= 7; struct Cyc_Absyn_Enum_d_struct{ int
tag; struct Cyc_Absyn_Enumdecl* f1; } ; static const int Cyc_Absyn_Typedef_d_tag=
8; struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1;
} ; static const int Cyc_Absyn_Namespace_d_tag= 9; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d_tag= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d_tag=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement_tag= 0; struct Cyc_Absyn_ArrayElement_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName_tag=
1; struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot_tag[ 11u]; struct Cyc_Absyn_EmptyAnnot_struct{
char* tag; } ; extern struct Cyc_Absyn_Tqual* Cyc_Absyn_combine_tqual( struct
Cyc_Absyn_Tqual* x, struct Cyc_Absyn_Tqual* y); extern struct Cyc_Absyn_Tqual*
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern void*
Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t;
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
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_xtunion_decl(
void* s, struct _tuple1* n, struct Cyc_List_List* fs, struct Cyc_Position_Segment*
loc); extern void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct Cyc_Core_Opt*
eff_typ, void* ret_typ, struct Cyc_List_List* args, int varargs, struct Cyc_List_List*);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern void Cyc_Lex_register_typedef( struct _tuple1* s); extern
void Cyc_Lex_enter_namespace( struct _tagged_string*); extern void Cyc_Lex_leave_namespace();
extern void Cyc_Lex_enter_using( struct _tuple1*); extern void Cyc_Lex_leave_using();
typedef void* Cyc_Parse_Struct_or_union; static const unsigned int Cyc_Parse_Struct_su=
0; static const unsigned int Cyc_Parse_Union_su= 1; typedef void* Cyc_Parse_struct_or_union_t;
typedef void* Cyc_Parse_Blockitem; static const int Cyc_Parse_TopDecls_bl_tag= 0;
struct Cyc_Parse_TopDecls_bl_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Parse_Stmt_bl_tag= 1; struct Cyc_Parse_Stmt_bl_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Parse_FnDecl_bl_tag= 2;
struct Cyc_Parse_FnDecl_bl_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ;
typedef void* Cyc_Parse_blockitem_t; typedef void* Cyc_Parse_Type_specifier;
static const int Cyc_Parse_Signed_spec_tag= 0; struct Cyc_Parse_Signed_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Unsigned_spec_tag=
1; struct Cyc_Parse_Unsigned_spec_struct{ int tag; struct Cyc_Position_Segment*
f1; } ; static const int Cyc_Parse_Short_spec_tag= 2; struct Cyc_Parse_Short_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Long_spec_tag=
3; struct Cyc_Parse_Long_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Type_spec_tag= 4; struct Cyc_Parse_Type_spec_struct{
int tag; void* f1; struct Cyc_Position_Segment* f2; } ; static const int Cyc_Parse_Decl_spec_tag=
5; struct Cyc_Parse_Decl_spec_struct{ int tag; struct Cyc_Absyn_Decl* f1; } ;
typedef void* Cyc_Parse_type_specifier_t; typedef void* Cyc_Parse_Storage_class;
static const unsigned int Cyc_Parse_Typedef_sc= 0; static const unsigned int Cyc_Parse_Extern_sc=
1; static const unsigned int Cyc_Parse_ExternC_sc= 2; static const unsigned int
Cyc_Parse_Static_sc= 3; static const unsigned int Cyc_Parse_Auto_sc= 4; static
const unsigned int Cyc_Parse_Register_sc= 5; static const unsigned int Cyc_Parse_Abstract_sc=
6; typedef void* Cyc_Parse_storage_class_t; struct Cyc_Parse_Declaration_spec{
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
void) _throw(( struct _xtunion_struct*)({ struct Cyc_Position_Exit_struct*
_temp0=( struct Cyc_Position_Exit_struct*) GC_malloc( sizeof( struct Cyc_Position_Exit_struct));
_temp0[ 0]=({ struct Cyc_Position_Exit_struct _temp1; _temp1.tag= Cyc_Position_Exit_tag;
_temp1;}); _temp0;}));} static void Cyc_Parse_warn( struct _tagged_string msg,
struct Cyc_Position_Segment* sg){({ struct _tagged_string _temp2= Cyc_Position_string_of_segment(
sg); struct _tagged_string _temp3= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: %.*s\n",
_temp2.last_plus_one - _temp2.curr, _temp2.curr, _temp3.last_plus_one - _temp3.curr,
_temp3.curr);}); return;} static void* Cyc_Parse_unimp( struct _tagged_string
msg, struct Cyc_Position_Segment* sg){ return(( void*(*)( struct _tagged_string
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string
_temp4= msg; xprintf("%.*s unimplemented", _temp4.last_plus_one - _temp4.curr,
_temp4.curr);}), sg);} static void Cyc_Parse_unimp2( struct _tagged_string msg,
struct Cyc_Position_Segment* sg){({ struct _tagged_string _temp5= Cyc_Position_string_of_segment(
sg); struct _tagged_string _temp6= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: Cyclone does not yet support %.*s\n",
_temp5.last_plus_one - _temp5.curr, _temp5.curr, _temp6.last_plus_one - _temp6.curr,
_temp6.curr);}); return;} static struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field(
struct Cyc_Position_Segment* loc, struct _tuple8* field_info){ struct Cyc_Core_Opt*
_temp9; struct _tuple7* _temp11; struct _tuple8 _temp7=* field_info; _LL12:
_temp11= _temp7.f1; goto _LL10; _LL10: _temp9= _temp7.f2; goto _LL8; _LL8: if((*
_temp11).f4 != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp13=( char*)"bad type params in struct field";
struct _tagged_string _temp14; _temp14.curr= _temp13; _temp14.base= _temp13;
_temp14.last_plus_one= _temp13 + 32; _temp14;}), loc);}{ struct _tuple1* qid=(*
_temp11).f1;{ void* _temp15=(* qid).f1; struct Cyc_List_List* _temp25; struct
Cyc_List_List* _temp27; _LL17: if(( unsigned int) _temp15 > 1u?(( struct
_tunion_struct*) _temp15)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL26: _temp25=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp15)->f1; if(
_temp25 == 0){ goto _LL18;} else{ goto _LL19;}} else{ goto _LL19;} _LL19: if((
unsigned int) _temp15 > 1u?(( struct _tunion_struct*) _temp15)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL28: _temp27=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp15)->f1; if( _temp27 == 0){ goto _LL20;} else{ goto _LL21;}} else{ goto
_LL21;} _LL21: if(( int) _temp15 == Cyc_Absyn_Loc_n){ goto _LL22;} else{ goto
_LL23;} _LL23: goto _LL24; _LL18: goto _LL16; _LL20: goto _LL16; _LL22: goto
_LL16; _LL24: Cyc_Parse_err(( struct _tagged_string)({ char* _temp29=( char*)"struct field cannot be qualified with a module name";
struct _tagged_string _temp30; _temp30.curr= _temp29; _temp30.base= _temp29;
_temp30.last_plus_one= _temp29 + 52; _temp30;}), loc); goto _LL16; _LL16:;}{
struct Cyc_List_List* atts=(* _temp11).f5; return({ struct Cyc_Absyn_Structfield*
_temp31=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp31->name=(* qid).f2; _temp31->tq=(* _temp11).f2; _temp31->type=( void*)(*
_temp11).f3; _temp31->width= _temp9; _temp31->attributes= atts; _temp31;});}}}
static struct _tuple2* Cyc_Parse_make_param( struct Cyc_Position_Segment* loc,
struct _tuple9* field){ struct _tuple9 _temp34; struct Cyc_List_List* _temp35;
void* _temp37; struct Cyc_Absyn_Tqual* _temp39; struct Cyc_Core_Opt* _temp41;
struct _tuple9* _temp32= field; _temp34=* _temp32; _LL42: _temp41= _temp34.f1;
goto _LL40; _LL40: _temp39= _temp34.f2; goto _LL38; _LL38: _temp37= _temp34.f3;
goto _LL36; _LL36: _temp35= _temp34.f4; goto _LL33; _LL33: { struct Cyc_Core_Opt*
idopt= 0; if( _temp41 != 0){ idopt=({ struct Cyc_Core_Opt* _temp43=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp43->v=( void*)(*(( struct _tuple1*)
_temp41->v)).f2; _temp43;});{ void* _temp44=(*(( struct _tuple1*) _temp41->v)).f1;
struct Cyc_List_List* _temp54; struct Cyc_List_List* _temp56; _LL46: if((
unsigned int) _temp44 > 1u?(( struct _tunion_struct*) _temp44)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL55: _temp54=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp44)->f1; if( _temp54 == 0){ goto _LL47;} else{ goto _LL48;}} else{ goto
_LL48;} _LL48: if(( unsigned int) _temp44 > 1u?(( struct _tunion_struct*)
_temp44)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL57: _temp56=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp44)->f1; if( _temp56 == 0){ goto _LL49;}
else{ goto _LL50;}} else{ goto _LL50;} _LL50: if(( int) _temp44 == Cyc_Absyn_Loc_n){
goto _LL51;} else{ goto _LL52;} _LL52: goto _LL53; _LL47: goto _LL45; _LL49:
goto _LL45; _LL51: goto _LL45; _LL53: Cyc_Parse_err(( struct _tagged_string)({
char* _temp58=( char*)"parameter cannot be qualified with a module name"; struct
_tagged_string _temp59; _temp59.curr= _temp58; _temp59.base= _temp58; _temp59.last_plus_one=
_temp58 + 49; _temp59;}), loc); goto _LL45; _LL45:;}} if( _temp35 != 0){(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp60=( char*)"parameter should have no type parameters";
struct _tagged_string _temp61; _temp61.curr= _temp60; _temp61.base= _temp60;
_temp61.last_plus_one= _temp60 + 41; _temp61;}), loc);} return({ struct _tuple2*
_temp62=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp62->f1=
idopt; _temp62->f2= _temp39; _temp62->f3= _temp37; _temp62;});}} static void*
Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Parse_Type_spec_struct* _temp63=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp63[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp64; _temp64.tag= Cyc_Parse_Type_spec_tag;
_temp64.f1=( void*) t; _temp64.f2= loc; _temp64;}); _temp63;});} static void*
Cyc_Parse_array2ptr( void* t){ void* _temp65= t; struct Cyc_Absyn_Exp* _temp71;
struct Cyc_Absyn_Tqual* _temp73; void* _temp75; _LL67: if(( unsigned int)
_temp65 > 4u?(( struct _tunion_struct*) _temp65)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL76: _temp75=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp65)->f1;
goto _LL74; _LL74: _temp73=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp65)->f2; goto _LL72; _LL72: _temp71=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp65)->f3; goto _LL68;} else{ goto _LL69;} _LL69: goto _LL70; _LL68: return
Cyc_Absyn_starb_typ( _temp75,( void*) Cyc_Absyn_HeapRgn, _temp73, _temp71 == 0?(
void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp77=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp77[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp78; _temp78.tag= Cyc_Absyn_Upper_b_tag;
_temp78.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp79= _temp71;
if( _temp79 == 0){ _throw( Null_Exception);} _temp79;}); _temp78;}); _temp77;}));
_LL70: return t; _LL66:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} static struct _tuple10* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple10*
_temp80=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp80->f1=(*
t).f2; _temp80->f2=(* t).f3; _temp80;});} static void Cyc_Parse_only_vardecl(
struct Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_string
decl_kind;{ void* _temp81=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp107; int
_temp109; struct Cyc_Absyn_Exp* _temp111; struct Cyc_Core_Opt* _temp113; struct
Cyc_Core_Opt* _temp115; struct Cyc_Absyn_Pat* _temp117; struct Cyc_Absyn_Fndecl*
_temp119; struct Cyc_Absyn_Structdecl* _temp121; struct Cyc_Absyn_Uniondecl*
_temp123; struct Cyc_Absyn_Tuniondecl* _temp125; struct Cyc_Absyn_Typedefdecl*
_temp127; struct Cyc_Absyn_XTuniondecl* _temp129; struct Cyc_Absyn_Enumdecl*
_temp131; struct Cyc_List_List* _temp133; struct _tagged_string* _temp135;
struct Cyc_List_List* _temp137; struct _tuple1* _temp139; struct Cyc_List_List*
_temp141; _LL83: if((( struct _tunion_struct*) _temp81)->tag == Cyc_Absyn_Var_d_tag){
_LL108: _temp107=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp81)->f1; goto _LL84;} else{ goto _LL85;} _LL85: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_Let_d_tag){ _LL118: _temp117=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp81)->f1; goto _LL116; _LL116: _temp115=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp81)->f2; goto
_LL114; _LL114: _temp113=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp81)->f3; goto _LL112; _LL112: _temp111=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Let_d_struct*) _temp81)->f4; goto _LL110; _LL110: _temp109=( int)((
struct Cyc_Absyn_Let_d_struct*) _temp81)->f5; goto _LL86;} else{ goto _LL87;}
_LL87: if((( struct _tunion_struct*) _temp81)->tag == Cyc_Absyn_Fn_d_tag){
_LL120: _temp119=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp81)->f1; goto _LL88;} else{ goto _LL89;} _LL89: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_Struct_d_tag){ _LL122: _temp121=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp81)->f1; goto _LL90;} else{ goto _LL91;}
_LL91: if((( struct _tunion_struct*) _temp81)->tag == Cyc_Absyn_Union_d_tag){
_LL124: _temp123=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp81)->f1; goto _LL92;} else{ goto _LL93;} _LL93: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_Tunion_d_tag){ _LL126: _temp125=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp81)->f1; goto _LL94;} else{ goto _LL95;}
_LL95: if((( struct _tunion_struct*) _temp81)->tag == Cyc_Absyn_Typedef_d_tag){
_LL128: _temp127=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp81)->f1; goto _LL96;} else{ goto _LL97;} _LL97: if((( struct _tunion_struct*)
_temp81)->tag == Cyc_Absyn_XTunion_d_tag){ _LL130: _temp129=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp81)->f1; goto _LL98;} else{ goto _LL99;}
_LL99: if((( struct _tunion_struct*) _temp81)->tag == Cyc_Absyn_Enum_d_tag){
_LL132: _temp131=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp81)->f1; goto _LL100;} else{ goto _LL101;} _LL101: if((( struct
_tunion_struct*) _temp81)->tag == Cyc_Absyn_Namespace_d_tag){ _LL136: _temp135=(
struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp81)->f1;
goto _LL134; _LL134: _temp133=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp81)->f2; goto _LL102;} else{ goto _LL103;} _LL103: if((( struct
_tunion_struct*) _temp81)->tag == Cyc_Absyn_Using_d_tag){ _LL140: _temp139=(
struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp81)->f1; goto _LL138;
_LL138: _temp137=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp81)->f2; goto _LL104;} else{ goto _LL105;} _LL105: if((( struct
_tunion_struct*) _temp81)->tag == Cyc_Absyn_ExternC_d_tag){ _LL142: _temp141=(
struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*) _temp81)->f1; goto
_LL106;} else{ goto _LL82;} _LL84: if( _temp107->initializer != 0){(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp143=( char*)"initializers are not allowed in parameter declarations";
struct _tagged_string _temp144; _temp144.curr= _temp143; _temp144.base= _temp143;
_temp144.last_plus_one= _temp143 + 55; _temp144;}), x->loc);}{ void* _temp145=(*
_temp107->name).f1; struct Cyc_List_List* _temp155; struct Cyc_List_List*
_temp157; _LL147: if(( int) _temp145 == Cyc_Absyn_Loc_n){ goto _LL148;} else{
goto _LL149;} _LL149: if(( unsigned int) _temp145 > 1u?(( struct _tunion_struct*)
_temp145)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL156: _temp155=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp145)->f1; if( _temp155 == 0){ goto _LL150;}
else{ goto _LL151;}} else{ goto _LL151;} _LL151: if(( unsigned int) _temp145 > 1u?((
struct _tunion_struct*) _temp145)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL158:
_temp157=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp145)->f1;
if( _temp157 == 0){ goto _LL152;} else{ goto _LL153;}} else{ goto _LL153;}
_LL153: goto _LL154; _LL148: goto _LL146; _LL150: goto _LL146; _LL152: goto
_LL146; _LL154: Cyc_Parse_err(( struct _tagged_string)({ char* _temp159=( char*)"module names not allowed on parameter declarations";
struct _tagged_string _temp160; _temp160.curr= _temp159; _temp160.base= _temp159;
_temp160.last_plus_one= _temp159 + 51; _temp160;}), x->loc); goto _LL146; _LL146:;}{
int found= 0; for( 0; params != 0; params= params->tl){ if( Cyc_String_zstrptrcmp((*
_temp107->name).f2,( struct _tagged_string*) params->hd) == 0){ found= 1; break;}}
if( ! found){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp161=*(* _temp107->name).f2;
xprintf("%.*s is not listed as a parameter", _temp161.last_plus_one - _temp161.curr,
_temp161.curr);}), x->loc);} return;} _LL86: decl_kind=( struct _tagged_string)({
char* _temp162=( char*)"let declaration"; struct _tagged_string _temp163;
_temp163.curr= _temp162; _temp163.base= _temp162; _temp163.last_plus_one=
_temp162 + 16; _temp163;}); goto _LL82; _LL88: decl_kind=( struct _tagged_string)({
char* _temp164=( char*)"function declaration"; struct _tagged_string _temp165;
_temp165.curr= _temp164; _temp165.base= _temp164; _temp165.last_plus_one=
_temp164 + 21; _temp165;}); goto _LL82; _LL90: decl_kind=( struct _tagged_string)({
char* _temp166=( char*)"struct declaration"; struct _tagged_string _temp167;
_temp167.curr= _temp166; _temp167.base= _temp166; _temp167.last_plus_one=
_temp166 + 19; _temp167;}); goto _LL82; _LL92: decl_kind=( struct _tagged_string)({
char* _temp168=( char*)"union declaration"; struct _tagged_string _temp169;
_temp169.curr= _temp168; _temp169.base= _temp168; _temp169.last_plus_one=
_temp168 + 18; _temp169;}); goto _LL82; _LL94: decl_kind=( struct _tagged_string)({
char* _temp170=( char*)"tunion declaration"; struct _tagged_string _temp171;
_temp171.curr= _temp170; _temp171.base= _temp170; _temp171.last_plus_one=
_temp170 + 19; _temp171;}); goto _LL82; _LL96: decl_kind=( struct _tagged_string)({
char* _temp172=( char*)"typedef"; struct _tagged_string _temp173; _temp173.curr=
_temp172; _temp173.base= _temp172; _temp173.last_plus_one= _temp172 + 8;
_temp173;}); goto _LL82; _LL98: decl_kind=( struct _tagged_string)({ char*
_temp174=( char*)"xtunion declaration"; struct _tagged_string _temp175; _temp175.curr=
_temp174; _temp175.base= _temp174; _temp175.last_plus_one= _temp174 + 20;
_temp175;}); goto _LL82; _LL100: decl_kind=( struct _tagged_string)({ char*
_temp176=( char*)"enum declaration"; struct _tagged_string _temp177; _temp177.curr=
_temp176; _temp177.base= _temp176; _temp177.last_plus_one= _temp176 + 17;
_temp177;}); goto _LL82; _LL102: decl_kind=( struct _tagged_string)({ char*
_temp178=( char*)"namespace declaration"; struct _tagged_string _temp179;
_temp179.curr= _temp178; _temp179.base= _temp178; _temp179.last_plus_one=
_temp178 + 22; _temp179;}); goto _LL82; _LL104: decl_kind=( struct
_tagged_string)({ char* _temp180=( char*)"using declaration"; struct
_tagged_string _temp181; _temp181.curr= _temp180; _temp181.base= _temp180;
_temp181.last_plus_one= _temp180 + 18; _temp181;}); goto _LL82; _LL106:
decl_kind=( struct _tagged_string)({ char* _temp182=( char*)"extern C declaration";
struct _tagged_string _temp183; _temp183.curr= _temp182; _temp183.base= _temp182;
_temp183.last_plus_one= _temp182 + 21; _temp183;}); goto _LL82; _LL82:;}(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp184= decl_kind; xprintf("%.*s appears in parameter type",
_temp184.last_plus_one - _temp184.curr, _temp184.curr);}), x->loc); return;}
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple11* env, struct
_tagged_string* x){ struct _tuple11 _temp187; struct Cyc_Position_Segment*
_temp188; struct Cyc_List_List* _temp190; struct _tuple11* _temp185= env;
_temp187=* _temp185; _LL191: _temp190= _temp187.f1; goto _LL189; _LL189:
_temp188= _temp187.f2; goto _LL186; _LL186: if( _temp190 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp192=* x; xprintf("missing type for parameter %.*s",
_temp192.last_plus_one - _temp192.curr, _temp192.curr);}), _temp188);}{ void*
_temp193=( void*)(( struct Cyc_Absyn_Decl*) _temp190->hd)->r; struct Cyc_Absyn_Vardecl*
_temp199; _LL195: if((( struct _tunion_struct*) _temp193)->tag == Cyc_Absyn_Var_d_tag){
_LL200: _temp199=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp193)->f1; goto _LL196;} else{ goto _LL197;} _LL197: goto _LL198; _LL196:{
void* _temp201=(* _temp199->name).f1; struct Cyc_List_List* _temp211; struct Cyc_List_List*
_temp213; _LL203: if(( int) _temp201 == Cyc_Absyn_Loc_n){ goto _LL204;} else{
goto _LL205;} _LL205: if(( unsigned int) _temp201 > 1u?(( struct _tunion_struct*)
_temp201)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL212: _temp211=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp201)->f1; if( _temp211 == 0){ goto _LL206;}
else{ goto _LL207;}} else{ goto _LL207;} _LL207: if(( unsigned int) _temp201 > 1u?((
struct _tunion_struct*) _temp201)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL214:
_temp213=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp201)->f1;
if( _temp213 == 0){ goto _LL208;} else{ goto _LL209;}} else{ goto _LL209;}
_LL209: goto _LL210; _LL204: goto _LL202; _LL206: goto _LL202; _LL208: goto
_LL202; _LL210: Cyc_Parse_err(( struct _tagged_string)({ char* _temp215=( char*)"module name not allowed on parameter";
struct _tagged_string _temp216; _temp216.curr= _temp215; _temp216.base= _temp215;
_temp216.last_plus_one= _temp215 + 37; _temp216;}), _temp188); goto _LL202;
_LL202:;} if( Cyc_String_zstrptrcmp((* _temp199->name).f2, x) == 0){ return({
struct _tuple2* _temp217=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp217[ 0]=({ struct _tuple2 _temp218; _temp218.f1=({ struct Cyc_Core_Opt*
_temp219=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp219[ 0]=({ struct Cyc_Core_Opt _temp220; _temp220.v=( void*)(* _temp199->name).f2;
_temp220;}); _temp219;}); _temp218.f2= _temp199->tq; _temp218.f3=( void*)
_temp199->type; _temp218;}); _temp217;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple11* _temp221=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp221->f1= _temp190->tl; _temp221->f2= _temp188; _temp221;}), x);} _LL198:
return(( struct _tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp222=( char*)"non-variable declaration";
struct _tagged_string _temp223; _temp223.curr= _temp222; _temp223.base= _temp222;
_temp223.last_plus_one= _temp222 + 25; _temp223;}),(( struct Cyc_Absyn_Decl*)
_temp190->hd)->loc); _LL194:;}} static int Cyc_Parse_is_typeparam( void* tm){
void* _temp224= tm; int _temp230; struct Cyc_Position_Segment* _temp232; struct
Cyc_List_List* _temp234; _LL226: if(( unsigned int) _temp224 > 1u?(( struct
_tunion_struct*) _temp224)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL235:
_temp234=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp224)->f1; goto _LL233; _LL233: _temp232=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp224)->f2; goto _LL231; _LL231:
_temp230=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp224)->f3; goto
_LL227;} else{ goto _LL228;} _LL228: goto _LL229; _LL227: return 1; _LL229:
return 0; _LL225:;} static void* Cyc_Parse_id2type( struct _tagged_string s,
struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp236=( char*)"`H"; struct _tagged_string _temp237; _temp237.curr=
_temp236; _temp237.base= _temp236; _temp237.last_plus_one= _temp236 + 3;
_temp237;})) == 0){ return( void*) Cyc_Absyn_HeapRgn;} else{ return( void*)({
struct Cyc_Absyn_VarType_struct* _temp238=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp238[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp239; _temp239.tag= Cyc_Absyn_VarType_tag; _temp239.f1=({ struct Cyc_Absyn_Tvar*
_temp240=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp240->name=({ struct _tagged_string* _temp241=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp241[ 0]= s; _temp241;});
_temp240->kind= k; _temp240;}); _temp239;}); _temp238;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp242=
t; struct Cyc_Absyn_Tvar* _temp248; _LL244: if(( unsigned int) _temp242 > 4u?((
struct _tunion_struct*) _temp242)->tag == Cyc_Absyn_VarType_tag: 0){ _LL249:
_temp248=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp242)->f1;
goto _LL245;} else{ goto _LL246;} _LL246: goto _LL247; _LL245: return _temp248;
_LL247: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char*
_temp250=( char*)"expecting a list of type variables, not types"; struct
_tagged_string _temp251; _temp251.curr= _temp250; _temp251.base= _temp250;
_temp251.last_plus_one= _temp250 + 46; _temp251;}), loc); _LL243:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp252=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp252[ 0]=({ struct Cyc_Absyn_VarType_struct _temp253; _temp253.tag= Cyc_Absyn_VarType_tag;
_temp253.f1= pr; _temp253;}); _temp252;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp254=(
void*) tms->hd; void* _temp260; _LL256: if(( unsigned int) _temp254 > 1u?((
struct _tunion_struct*) _temp254)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL261:
_temp260=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp254)->f1; goto
_LL257;} else{ goto _LL258;} _LL258: goto _LL259; _LL257: if( tms->tl == 0? 1:(
Cyc_Parse_is_typeparam(( void*)( tms->tl)->hd)?( tms->tl)->tl == 0: 0)){ void*
_temp262= _temp260; struct Cyc_Core_Opt* _temp268; int _temp270; struct Cyc_List_List*
_temp272; struct Cyc_Position_Segment* _temp274; struct Cyc_List_List* _temp276;
_LL264: if((( struct _tunion_struct*) _temp262)->tag == Cyc_Absyn_WithTypes_tag){
_LL273: _temp272=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp262)->f1; goto _LL271; _LL271: _temp270=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp262)->f2; goto _LL269; _LL269: _temp268=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp262)->f3; goto _LL265;} else{ goto _LL266;}
_LL266: if((( struct _tunion_struct*) _temp262)->tag == Cyc_Absyn_NoTypes_tag){
_LL277: _temp276=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp262)->f1; goto _LL275; _LL275: _temp274=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp262)->f2; goto _LL267;} else{ goto _LL263;}
_LL265: Cyc_Parse_warn(( struct _tagged_string)({ char* _temp278=( char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp279; _temp279.curr= _temp278; _temp279.base= _temp278;
_temp279.last_plus_one= _temp278 + 92; _temp279;}), loc); return tms; _LL267:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp276, tds);{ struct _tuple11* env=({ struct _tuple11* _temp280=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp280->f1= tds; _temp280->f2=
loc; _temp280;}); return({ struct Cyc_List_List* _temp281=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp281->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp282=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp282[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp283; _temp283.tag= Cyc_Absyn_Function_mod_tag;
_temp283.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp284=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp284[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp285; _temp285.tag= Cyc_Absyn_WithTypes_tag;
_temp285.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple11*,
struct _tagged_string*), struct _tuple11* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, env, _temp276); _temp285.f2= 0; _temp285.f3= 0;
_temp285;}); _temp284;})); _temp283;}); _temp282;})); _temp281->tl= 0; _temp281;});}
_LL263:;} else{ return({ struct Cyc_List_List* _temp286=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp286->hd=( void*)(( void*) tms->hd);
_temp286->tl= Cyc_Parse_oldstyle2newstyle( tms->tl, tds, loc); _temp286;});}
_LL259: return({ struct Cyc_List_List* _temp287=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp287->hd=( void*)(( void*) tms->hd);
_temp287->tl= Cyc_Parse_oldstyle2newstyle( tms->tl, tds, loc); _temp287;});
_LL255:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function( struct Cyc_Core_Opt*
dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt*
body, struct Cyc_Position_Segment* loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator*
_temp288=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp288->id= d->id; _temp288->tms= Cyc_Parse_oldstyle2newstyle( d->tms, tds,
loc); _temp288;});}{ void* sc=( void*) Cyc_Absyn_Public; struct Cyc_List_List*
tss= 0; struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual(); int is_inline= 0;
struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*) dso->v)->tq;
is_inline=(( struct Cyc_Parse_Declaration_spec*) dso->v)->is_inline; atts=((
struct Cyc_Parse_Declaration_spec*) dso->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){ void* _temp289=( void*)((( struct Cyc_Parse_Declaration_spec*)
dso->v)->sc)->v; _LL291: if(( int) _temp289 == Cyc_Parse_Extern_sc){ goto _LL292;}
else{ goto _LL293;} _LL293: if(( int) _temp289 == Cyc_Parse_ExternC_sc){ goto
_LL294;} else{ goto _LL295;} _LL295: if(( int) _temp289 == Cyc_Parse_Static_sc){
goto _LL296;} else{ goto _LL297;} _LL297: goto _LL298; _LL292: sc=( void*) Cyc_Absyn_Extern;
goto _LL290; _LL294: sc=( void*) Cyc_Absyn_ExternC; goto _LL290; _LL296: sc=(
void*) Cyc_Absyn_Static; goto _LL290; _LL298: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp299=( char*)"bad storage class on function"; struct
_tagged_string _temp300; _temp300.curr= _temp299; _temp300.base= _temp299;
_temp300.last_plus_one= _temp299 + 30; _temp300;}), loc); goto _LL290; _LL290:;}}{
struct Cyc_Core_Opt* _temp303; void* _temp305; struct _tuple5 _temp301= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL306: _temp305= _temp301.f1; goto _LL304; _LL304: _temp303=
_temp301.f2; goto _LL302; _LL302: { struct Cyc_List_List* _temp309; struct Cyc_List_List*
_temp311; void* _temp313; struct Cyc_Absyn_Tqual* _temp315; struct _tuple6
_temp307= Cyc_Parse_apply_tms( tq, _temp305, atts, d->tms); _LL316: _temp315=
_temp307.f1; goto _LL314; _LL314: _temp313= _temp307.f2; goto _LL312; _LL312:
_temp311= _temp307.f3; goto _LL310; _LL310: _temp309= _temp307.f4; goto _LL308;
_LL308: if( _temp303 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp317=( char*)"nested type declaration within function prototype"; struct
_tagged_string _temp318; _temp318.curr= _temp317; _temp318.base= _temp317;
_temp318.last_plus_one= _temp317 + 50; _temp318;}), loc);} if( _temp311 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp319=( char*)"bad type params, ignoring";
struct _tagged_string _temp320; _temp320.curr= _temp319; _temp320.base= _temp319;
_temp320.last_plus_one= _temp319 + 26; _temp320;}), loc);}{ void* _temp321=
_temp313; struct Cyc_Absyn_FnInfo _temp327; struct Cyc_List_List* _temp329; int
_temp331; struct Cyc_List_List* _temp333; void* _temp335; struct Cyc_Core_Opt*
_temp337; struct Cyc_List_List* _temp339; _LL323: if(( unsigned int) _temp321 >
4u?(( struct _tunion_struct*) _temp321)->tag == Cyc_Absyn_FnType_tag: 0){ _LL328:
_temp327=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp321)->f1;
_LL340: _temp339=( struct Cyc_List_List*) _temp327.tvars; goto _LL338; _LL338:
_temp337=( struct Cyc_Core_Opt*) _temp327.effect; goto _LL336; _LL336: _temp335=(
void*) _temp327.ret_typ; goto _LL334; _LL334: _temp333=( struct Cyc_List_List*)
_temp327.args; goto _LL332; _LL332: _temp331=( int) _temp327.varargs; goto
_LL330; _LL330: _temp329=( struct Cyc_List_List*) _temp327.attributes; goto
_LL324;} else{ goto _LL325;} _LL325: goto _LL326; _LL324: { struct Cyc_List_List*
args2=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp333); return({ struct Cyc_Absyn_Fndecl*
_temp341=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp341->sc=( void*) sc; _temp341->name= d->id; _temp341->tvs= _temp339;
_temp341->is_inline= is_inline; _temp341->effect= _temp337; _temp341->ret_type=(
void*) _temp335; _temp341->args= args2; _temp341->varargs= _temp331; _temp341->body=
body; _temp341->cached_typ= 0; _temp341->param_vardecls= 0; _temp341->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp329, _temp309); _temp341;});} _LL326: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp342=( char*)"declarator is not a function prototype";
struct _tagged_string _temp343; _temp343.curr= _temp342; _temp343.base= _temp342;
_temp343.last_plus_one= _temp342 + 39; _temp343;}), loc); _LL322:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp344=( char*)"missing argument variable in function prototype"; struct
_tagged_string _temp345; _temp345.curr= _temp344; _temp345.base= _temp344;
_temp345.last_plus_one= _temp344 + 48; _temp345;}), loc); return({ struct
_tuple4* _temp346=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp346->f1=({ struct _tagged_string* _temp347=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp347[ 0]=( struct
_tagged_string)({ char* _temp348=( char*)"?"; struct _tagged_string _temp349;
_temp349.curr= _temp348; _temp349.base= _temp348; _temp349.last_plus_one=
_temp348 + 2; _temp349;}); _temp347;}); _temp346->f2=(* t).f2; _temp346->f3=(* t).f3;
_temp346;});} else{ return({ struct _tuple4* _temp350=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp350->f1=( struct _tagged_string*)((* t).f1)->v;
_temp350->f2=(* t).f2; _temp350->f3=(* t).f3; _temp350;});}} static char
_temp353[ 52u]="at most one type may appear within a type specifier"; static
struct _tagged_string Cyc_Parse_msg1=( struct _tagged_string){ _temp353,
_temp353, _temp353 + 52u}; static char _temp356[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2=( struct _tagged_string){ _temp356,
_temp356, _temp356 + 63u}; static char _temp359[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3=( struct _tagged_string){ _temp359,
_temp359, _temp359 + 50u}; static char _temp362[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4=( struct _tagged_string){ _temp362,
_temp362, _temp362 + 60u}; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
declopt= 0; int seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*)
Cyc_Absyn_VoidType; void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed;
struct Cyc_Position_Segment* last_loc= loc; for( 0; ts != 0; ts= ts->tl){ void*
_temp363=( void*) ts->hd; struct Cyc_Position_Segment* _temp377; void* _temp379;
struct Cyc_Position_Segment* _temp381; struct Cyc_Position_Segment* _temp383;
struct Cyc_Position_Segment* _temp385; struct Cyc_Position_Segment* _temp387;
struct Cyc_Absyn_Decl* _temp389; _LL365: if((( struct _tunion_struct*) _temp363)->tag
== Cyc_Parse_Type_spec_tag){ _LL380: _temp379=( void*)(( struct Cyc_Parse_Type_spec_struct*)
_temp363)->f1; goto _LL378; _LL378: _temp377=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Type_spec_struct*) _temp363)->f2; goto _LL366;} else{ goto
_LL367;} _LL367: if((( struct _tunion_struct*) _temp363)->tag == Cyc_Parse_Signed_spec_tag){
_LL382: _temp381=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Signed_spec_struct*)
_temp363)->f1; goto _LL368;} else{ goto _LL369;} _LL369: if((( struct
_tunion_struct*) _temp363)->tag == Cyc_Parse_Unsigned_spec_tag){ _LL384:
_temp383=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Unsigned_spec_struct*)
_temp363)->f1; goto _LL370;} else{ goto _LL371;} _LL371: if((( struct
_tunion_struct*) _temp363)->tag == Cyc_Parse_Short_spec_tag){ _LL386: _temp385=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Short_spec_struct*) _temp363)->f1;
goto _LL372;} else{ goto _LL373;} _LL373: if((( struct _tunion_struct*) _temp363)->tag
== Cyc_Parse_Long_spec_tag){ _LL388: _temp387=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Long_spec_struct*) _temp363)->f1; goto _LL374;} else{ goto
_LL375;} _LL375: if((( struct _tunion_struct*) _temp363)->tag == Cyc_Parse_Decl_spec_tag){
_LL390: _temp389=( struct Cyc_Absyn_Decl*)(( struct Cyc_Parse_Decl_spec_struct*)
_temp363)->f1; goto _LL376;} else{ goto _LL364;} _LL366: if( seen_type){ Cyc_Parse_err(
Cyc_Parse_msg1, _temp377);} last_loc= _temp377; seen_type= 1; t= _temp379; goto
_LL364; _LL368: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp381);} if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp391=( char*)"signed qualifier must come before type";
struct _tagged_string _temp392; _temp392.curr= _temp391; _temp392.base= _temp391;
_temp392.last_plus_one= _temp391 + 39; _temp392;}), _temp381);} last_loc=
_temp381; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed; goto _LL364; _LL370: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp383);} if( seen_type){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp393=( char*)"signed qualifier must come before type";
struct _tagged_string _temp394; _temp394.curr= _temp393; _temp394.base= _temp393;
_temp394.last_plus_one= _temp393 + 39; _temp394;}), _temp383);} last_loc=
_temp383; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL364; _LL372:
if( seen_size){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp395=( char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp396; _temp396.curr= _temp395; _temp396.base= _temp395;
_temp396.last_plus_one= _temp395 + 59; _temp396;}), _temp385);} if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp397=( char*)"short modifier must come before base type";
struct _tagged_string _temp398; _temp398.curr= _temp397; _temp398.base= _temp397;
_temp398.last_plus_one= _temp397 + 42; _temp398;}), _temp385);} last_loc=
_temp385; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL364; _LL374: if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp399=( char*)"long modifier must come before base type";
struct _tagged_string _temp400; _temp400.curr= _temp399; _temp400.base= _temp399;
_temp400.last_plus_one= _temp399 + 41; _temp400;}), _temp387);} if( seen_size){
void* _temp401= sz; _LL403: if(( int) _temp401 == Cyc_Absyn_B4){ goto _LL404;}
else{ goto _LL405;} _LL405: goto _LL406; _LL404: sz=( void*) Cyc_Absyn_B8; goto
_LL402; _LL406: Cyc_Parse_err(( struct _tagged_string)({ char* _temp407=( char*)"extra long in type specifier";
struct _tagged_string _temp408; _temp408.curr= _temp407; _temp408.base= _temp407;
_temp408.last_plus_one= _temp407 + 29; _temp408;}), _temp387); goto _LL402;
_LL402:;} last_loc= _temp387; seen_size= 1; goto _LL364; _LL376: last_loc=
_temp389->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp409=(
void*) _temp389->r; struct Cyc_Absyn_Structdecl* _temp423; struct Cyc_Absyn_Tuniondecl*
_temp425; struct Cyc_Absyn_XTuniondecl* _temp427; struct Cyc_Absyn_Uniondecl*
_temp429; struct Cyc_Absyn_Enumdecl* _temp431; _LL411: if((( struct
_tunion_struct*) _temp409)->tag == Cyc_Absyn_Struct_d_tag){ _LL424: _temp423=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp409)->f1;
goto _LL412;} else{ goto _LL413;} _LL413: if((( struct _tunion_struct*) _temp409)->tag
== Cyc_Absyn_Tunion_d_tag){ _LL426: _temp425=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp409)->f1; goto _LL414;} else{ goto
_LL415;} _LL415: if((( struct _tunion_struct*) _temp409)->tag == Cyc_Absyn_XTunion_d_tag){
_LL428: _temp427=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp409)->f1; goto _LL416;} else{ goto _LL417;} _LL417: if((( struct
_tunion_struct*) _temp409)->tag == Cyc_Absyn_Union_d_tag){ _LL430: _temp429=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp409)->f1;
goto _LL418;} else{ goto _LL419;} _LL419: if((( struct _tunion_struct*) _temp409)->tag
== Cyc_Absyn_Enum_d_tag){ _LL432: _temp431=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp409)->f1; goto _LL420;} else{ goto _LL421;}
_LL421: goto _LL422; _LL412: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp423->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp433=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp433[ 0]=({ struct Cyc_Absyn_StructType_struct _temp434; _temp434.tag= Cyc_Absyn_StructType_tag;
_temp434.f1=( struct _tuple1*)(( struct _tuple1*)( _temp423->name)->v); _temp434.f2=
args; _temp434.f3= 0; _temp434;}); _temp433;}); if( _temp423->fields != 0){
declopt=({ struct Cyc_Core_Opt* _temp435=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp435->v=( void*) _temp389; _temp435;});} goto
_LL410;} _LL414: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)( void*(*
f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ,
_temp425->tvs); t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp436=(
struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp436[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp437; _temp437.tag= Cyc_Absyn_TunionType_tag;
_temp437.f1=({ struct Cyc_Absyn_TunionInfo _temp438; _temp438.name=( struct
_tuple1*)( _temp425->name)->v; _temp438.targs= args; _temp438.rgn=( void*)((
void*) Cyc_Absyn_HeapRgn); _temp438.tud= 0; _temp438;}); _temp437;}); _temp436;});
if( _temp425->fields != 0){ declopt=({ struct Cyc_Core_Opt* _temp439=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp439->v=( void*)
_temp389; _temp439;});} goto _LL410;} _LL416: t=( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp440=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp440[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp441; _temp441.tag= Cyc_Absyn_XTunionType_tag;
_temp441.f1=({ struct Cyc_Absyn_XTunionInfo _temp442; _temp442.name= _temp427->name;
_temp442.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp442.xtud= 0; _temp442;});
_temp441;}); _temp440;}); if( _temp427->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp443=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp443->v=( void*) _temp389; _temp443;});} goto _LL410; _LL418: { struct Cyc_List_List*
args=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp429->tvs); t=( void*)({
struct Cyc_Absyn_UnionType_struct* _temp444=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp444[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp445; _temp445.tag= Cyc_Absyn_UnionType_tag;
_temp445.f1=( struct _tuple1*)(( struct _tuple1*)( _temp429->name)->v); _temp445.f2=
args; _temp445.f3= 0; _temp445;}); _temp444;}); if( _temp429->fields != 0){
declopt=({ struct Cyc_Core_Opt* _temp446=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp446->v=( void*) _temp389; _temp446;});} goto
_LL410;} _LL420: t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp447=(
struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp447[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp448; _temp448.tag= Cyc_Absyn_EnumType_tag;
_temp448.f1= _temp431->name; _temp448.f2= 0; _temp448;}); _temp447;}); declopt=({
struct Cyc_Core_Opt* _temp449=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp449->v=( void*) _temp389; _temp449;}); goto _LL410; _LL422:((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp450=( char*)"bad declaration within type specifier";
struct _tagged_string _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 38; _temp451;}), _temp389->loc); goto _LL410;
_LL410:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp389->loc);} goto _LL364;
_LL364:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp452=( char*)"missing type withing specifier";
struct _tagged_string _temp453; _temp453.curr= _temp452; _temp453.base= _temp452;
_temp453.last_plus_one= _temp452 + 31; _temp453;}), last_loc);} t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp454=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp454[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp455; _temp455.tag= Cyc_Absyn_IntType_tag; _temp455.f1=( void*) sgn;
_temp455.f2=( void*) sz; _temp455;}); _temp454;});} else{ if( seen_sign){ void*
_temp456= t; void* _temp462; void* _temp464; _LL458: if(( unsigned int) _temp456
> 4u?(( struct _tunion_struct*) _temp456)->tag == Cyc_Absyn_IntType_tag: 0){
_LL465: _temp464=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp456)->f1;
goto _LL463; _LL463: _temp462=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp456)->f2; goto _LL459;} else{ goto _LL460;} _LL460: goto _LL461; _LL459: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp466=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp466[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp467; _temp467.tag= Cyc_Absyn_IntType_tag; _temp467.f1=( void*) sgn;
_temp467.f2=( void*) _temp462; _temp467;}); _temp466;}); goto _LL457; _LL461:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp468=( char*)"sign specification on non-integral type";
struct _tagged_string _temp469; _temp469.curr= _temp468; _temp469.base= _temp468;
_temp469.last_plus_one= _temp468 + 40; _temp469;}), last_loc); goto _LL457;
_LL457:;} if( seen_size){ void* _temp470= t; void* _temp476; void* _temp478;
_LL472: if(( unsigned int) _temp470 > 4u?(( struct _tunion_struct*) _temp470)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL479: _temp478=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp470)->f1; goto _LL477; _LL477: _temp476=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp470)->f2; goto _LL473;} else{ goto _LL474;} _LL474: goto _LL475; _LL473: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp480=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp480[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp481; _temp481.tag= Cyc_Absyn_IntType_tag; _temp481.f1=( void*) _temp478;
_temp481.f2=( void*) sz; _temp481;}); _temp480;}); goto _LL471; _LL475: Cyc_Parse_err((
struct _tagged_string)({ char* _temp482=( char*)"size qualifier on non-integral type";
struct _tagged_string _temp483; _temp483.curr= _temp482; _temp483.base= _temp482;
_temp483.last_plus_one= _temp482 + 36; _temp483;}), last_loc); goto _LL471;
_LL471:;}} return({ struct _tuple5 _temp484; _temp484.f1= t; _temp484.f2=
declopt; _temp484;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* d=( struct
Cyc_Parse_Declarator*) ds->hd; struct _tuple1* q= d->id; struct Cyc_List_List*
_temp487; struct Cyc_List_List* _temp489; void* _temp491; struct Cyc_Absyn_Tqual*
_temp493; struct _tuple6 _temp485= Cyc_Parse_apply_tms( tq, t, shared_atts, d->tms);
_LL494: _temp493= _temp485.f1; goto _LL492; _LL492: _temp491= _temp485.f2; goto
_LL490; _LL490: _temp489= _temp485.f3; goto _LL488; _LL488: _temp487= _temp485.f4;
goto _LL486; _LL486: return({ struct Cyc_List_List* _temp495=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp495->hd=( void*)({ struct
_tuple7* _temp496=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp496->f1= q; _temp496->f2= _temp493; _temp496->f3= _temp491; _temp496->f4=
_temp489; _temp496->f5= _temp487; _temp496;}); _temp495->tl= Cyc_Parse_apply_tmss(
_temp493, t, ds->tl, shared_atts); _temp495;});}} static int Cyc_Parse_fn_type_att(
void* a){ void* _temp497= a; int _temp511; _LL499: if(( unsigned int) _temp497 >
15u?(( struct _tunion_struct*) _temp497)->tag == Cyc_Absyn_Regparm_att_tag: 0){
_LL512: _temp511=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp497)->f1;
goto _LL500;} else{ goto _LL501;} _LL501: if(( int) _temp497 == Cyc_Absyn_Stdcall_att){
goto _LL502;} else{ goto _LL503;} _LL503: if(( int) _temp497 == Cyc_Absyn_Cdecl_att){
goto _LL504;} else{ goto _LL505;} _LL505: if(( int) _temp497 == Cyc_Absyn_Noreturn_att){
goto _LL506;} else{ goto _LL507;} _LL507: if(( int) _temp497 == Cyc_Absyn_Const_att){
goto _LL508;} else{ goto _LL509;} _LL509: goto _LL510; _LL500: goto _LL502;
_LL502: goto _LL504; _LL504: goto _LL506; _LL506: goto _LL508; _LL508: return 1;
_LL510: return 0; _LL498:;} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp513; _temp513.f1= tq; _temp513.f2=
t; _temp513.f3= 0; _temp513.f4= atts; _temp513;});}{ void* _temp514=( void*) tms->hd;
struct Cyc_Absyn_Exp* _temp528; void* _temp530; int _temp532; struct Cyc_Position_Segment*
_temp534; struct Cyc_List_List* _temp536; struct Cyc_Absyn_Tqual* _temp538; void*
_temp540; void* _temp542; struct Cyc_List_List* _temp544; struct Cyc_Position_Segment*
_temp546; _LL516: if(( int) _temp514 == Cyc_Absyn_Carray_mod){ goto _LL517;}
else{ goto _LL518;} _LL518: if(( unsigned int) _temp514 > 1u?(( struct
_tunion_struct*) _temp514)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){ _LL529:
_temp528=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp514)->f1; goto _LL519;} else{ goto _LL520;} _LL520: if(( unsigned int)
_temp514 > 1u?(( struct _tunion_struct*) _temp514)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL531: _temp530=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp514)->f1;
goto _LL521;} else{ goto _LL522;} _LL522: if(( unsigned int) _temp514 > 1u?((
struct _tunion_struct*) _temp514)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL537: _temp536=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp514)->f1; goto _LL535; _LL535: _temp534=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp514)->f2; goto _LL533; _LL533:
_temp532=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp514)->f3; goto
_LL523;} else{ goto _LL524;} _LL524: if(( unsigned int) _temp514 > 1u?(( struct
_tunion_struct*) _temp514)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL543:
_temp542=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp514)->f1; goto
_LL541; _LL541: _temp540=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp514)->f2; goto _LL539; _LL539: _temp538=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp514)->f3; goto _LL525;} else{ goto _LL526;}
_LL526: if(( unsigned int) _temp514 > 1u?(( struct _tunion_struct*) _temp514)->tag
== Cyc_Absyn_Attributes_mod_tag: 0){ _LL547: _temp546=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_Attributes_mod_struct*) _temp514)->f1; goto _LL545; _LL545:
_temp544=( struct Cyc_List_List*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp514)->f2; goto _LL527;} else{ goto _LL515;} _LL517: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp548=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp548[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp549; _temp549.tag= Cyc_Absyn_ArrayType_tag;
_temp549.f1=( void*) t; _temp549.f2= tq; _temp549.f3= 0; _temp549;}); _temp548;}),
atts, tms->tl); _LL519: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp550=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp550[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp551; _temp551.tag= Cyc_Absyn_ArrayType_tag;
_temp551.f1=( void*) t; _temp551.f2= tq; _temp551.f3=( struct Cyc_Absyn_Exp*)
_temp528; _temp551;}); _temp550;}), atts, tms->tl); _LL521: { void* _temp552=
_temp530; struct Cyc_Core_Opt* _temp558; int _temp560; struct Cyc_List_List*
_temp562; struct Cyc_Position_Segment* _temp564; struct Cyc_List_List* _temp566;
_LL554: if((( struct _tunion_struct*) _temp552)->tag == Cyc_Absyn_WithTypes_tag){
_LL563: _temp562=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp552)->f1; goto _LL561; _LL561: _temp560=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp552)->f2; goto _LL559; _LL559: _temp558=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp552)->f3; goto _LL555;} else{ goto _LL556;}
_LL556: if((( struct _tunion_struct*) _temp552)->tag == Cyc_Absyn_NoTypes_tag){
_LL567: _temp566=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp552)->f1; goto _LL565; _LL565: _temp564=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp552)->f2; goto _LL557;} else{ goto _LL553;}
_LL555: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as= as->tl){ if( Cyc_Parse_fn_type_att(( void*) as->hd)){ fn_atts=({
struct Cyc_List_List* _temp568=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp568->hd=( void*)(( void*) as->hd); _temp568->tl=
fn_atts; _temp568;});} else{ new_atts=({ struct Cyc_List_List* _temp569=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp569->hd=( void*)((
void*) as->hd); _temp569->tl= new_atts; _temp569;});}}} if( tms->tl != 0){ void*
_temp570=( void*)( tms->tl)->hd; int _temp576; struct Cyc_Position_Segment*
_temp578; struct Cyc_List_List* _temp580; _LL572: if(( unsigned int) _temp570 >
1u?(( struct _tunion_struct*) _temp570)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL581: _temp580=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp570)->f1; goto _LL579; _LL579: _temp578=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp570)->f2; goto _LL577; _LL577:
_temp576=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp570)->f3; goto
_LL573;} else{ goto _LL574;} _LL574: goto _LL575; _LL573: typvars= _temp580; tms=
tms->tl; goto _LL571; _LL575: goto _LL571; _LL571:;} if(((( ! _temp560? _temp562
!= 0: 0)? _temp562->tl == 0: 0)?(*(( struct _tuple2*) _temp562->hd)).f1 == 0: 0)?(*((
struct _tuple2*) _temp562->hd)).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp562=
0; _temp560= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct
_tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp562); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp558, t, _temp562, _temp560, fn_atts), new_atts, tms->tl);} _LL557:(
void) _throw((( struct _xtunion_struct*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char*
_temp582=( char*)"function declaration without parameter types"; struct
_tagged_string _temp583; _temp583.curr= _temp582; _temp583.base= _temp582;
_temp583.last_plus_one= _temp582 + 45; _temp583;}), _temp564)); _LL553:;} _LL523:
if( tms->tl == 0){ return({ struct _tuple6 _temp584; _temp584.f1= tq; _temp584.f2=
t; _temp584.f3= _temp536; _temp584.f4= atts; _temp584;});}( void) _throw(((
struct _xtunion_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp585=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp586; _temp586.curr= _temp585; _temp586.base= _temp585;
_temp586.last_plus_one= _temp585 + 68; _temp586;}), _temp534)); _LL525: { void*
_temp587= _temp542; struct Cyc_Absyn_Exp* _temp595; struct Cyc_Absyn_Exp*
_temp597; _LL589: if(( unsigned int) _temp587 > 1u?(( struct _tunion_struct*)
_temp587)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL596: _temp595=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp587)->f1; goto
_LL590;} else{ goto _LL591;} _LL591: if(( unsigned int) _temp587 > 1u?(( struct
_tunion_struct*) _temp587)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL598:
_temp597=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp587)->f1; goto _LL592;} else{ goto _LL593;} _LL593: if(( int) _temp587 ==
Cyc_Absyn_TaggedArray_ps){ goto _LL594;} else{ goto _LL588;} _LL590: return Cyc_Parse_apply_tms(
_temp538, Cyc_Absyn_atb_typ( t, _temp540, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp599=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp599[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp600; _temp600.tag= Cyc_Absyn_Upper_b_tag;
_temp600.f1= _temp595; _temp600;}); _temp599;})), atts, tms->tl); _LL592: return
Cyc_Parse_apply_tms( _temp538, Cyc_Absyn_starb_typ( t, _temp540, tq,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp601=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp601[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp602; _temp602.tag= Cyc_Absyn_Upper_b_tag; _temp602.f1= _temp597; _temp602;});
_temp601;})), atts, tms->tl); _LL594: return Cyc_Parse_apply_tms( _temp538, Cyc_Absyn_tagged_typ(
t, _temp540, tq), atts, tms->tl); _LL588:;} _LL527: return Cyc_Parse_apply_tms(
tq, t,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( atts, _temp544), tms->tl); _LL515:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp605; void* _temp607; struct _tuple5 _temp603= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL608: _temp607= _temp603.f1; goto _LL606; _LL606: _temp605=
_temp603.f2; goto _LL604; _LL604: if( _temp605 != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp609=( char*)"ignoring nested type declaration(s) in specifier list";
struct _tagged_string _temp610; _temp610.curr= _temp609; _temp610.base= _temp609;
_temp610.last_plus_one= _temp609 + 54; _temp610;}), loc);} return _temp607;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp611=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp611[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp612; _temp612.tag= Cyc_Absyn_Decl_s_tag;
_temp612.f1= d; _temp612.f2= s; _temp612;}); _temp611;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual* tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp613=( char*)"inline is only allowed on function definitions"; struct
_tagged_string _temp614; _temp614.curr= _temp613; _temp614.base= _temp613;
_temp614.last_plus_one= _temp613 + 47; _temp614;}), loc);} if( tss == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp615=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp616; _temp616.curr= _temp615; _temp616.base= _temp615;
_temp616.last_plus_one= _temp615 + 39; _temp616;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp617=( void*)( ds->sc)->v; _LL619: if(( int) _temp617 == Cyc_Parse_Typedef_sc){
goto _LL620;} else{ goto _LL621;} _LL621: if(( int) _temp617 == Cyc_Parse_Extern_sc){
goto _LL622;} else{ goto _LL623;} _LL623: if(( int) _temp617 == Cyc_Parse_ExternC_sc){
goto _LL624;} else{ goto _LL625;} _LL625: if(( int) _temp617 == Cyc_Parse_Static_sc){
goto _LL626;} else{ goto _LL627;} _LL627: if(( int) _temp617 == Cyc_Parse_Auto_sc){
goto _LL628;} else{ goto _LL629;} _LL629: if(( int) _temp617 == Cyc_Parse_Register_sc){
goto _LL630;} else{ goto _LL631;} _LL631: if(( int) _temp617 == Cyc_Parse_Abstract_sc){
goto _LL632;} else{ goto _LL618;} _LL620: istypedef= 1; goto _LL618; _LL622: s=(
void*) Cyc_Absyn_Extern; goto _LL618; _LL624: s=( void*) Cyc_Absyn_ExternC; goto
_LL618; _LL626: s=( void*) Cyc_Absyn_Static; goto _LL618; _LL628: s=( void*) Cyc_Absyn_Public;
goto _LL618; _LL630: s=( void*) Cyc_Absyn_Public; goto _LL618; _LL632: s=( void*)
Cyc_Absyn_Abstract; goto _LL618; _LL618:;}{ struct Cyc_List_List* _temp635;
struct Cyc_List_List* _temp637; struct _tuple0 _temp633=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL638: _temp637= _temp633.f1;
goto _LL636; _LL636: _temp635= _temp633.f2; goto _LL634; _LL634: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp635; for( 0; es != 0; es= es->tl){
if(( struct Cyc_Absyn_Exp*) es->hd != 0){ exps_empty= 0; break;}}}{ struct
_tuple5 ts_info= Cyc_Parse_collapse_type_specifiers( tss, loc); if( _temp637 ==
0){ struct Cyc_Core_Opt* _temp641; void* _temp643; struct _tuple5 _temp639=
ts_info; _LL644: _temp643= _temp639.f1; goto _LL642; _LL642: _temp641= _temp639.f2;
goto _LL640; _LL640: if( _temp641 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
_temp641->v;{ void* _temp645=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp659;
struct Cyc_Absyn_Structdecl* _temp661; struct Cyc_Absyn_Uniondecl* _temp663;
struct Cyc_Absyn_Tuniondecl* _temp665; struct Cyc_Absyn_XTuniondecl* _temp667;
_LL647: if((( struct _tunion_struct*) _temp645)->tag == Cyc_Absyn_Enum_d_tag){
_LL660: _temp659=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp645)->f1; goto _LL648;} else{ goto _LL649;} _LL649: if((( struct
_tunion_struct*) _temp645)->tag == Cyc_Absyn_Struct_d_tag){ _LL662: _temp661=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp645)->f1;
goto _LL650;} else{ goto _LL651;} _LL651: if((( struct _tunion_struct*) _temp645)->tag
== Cyc_Absyn_Union_d_tag){ _LL664: _temp663=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp645)->f1; goto _LL652;} else{ goto _LL653;}
_LL653: if((( struct _tunion_struct*) _temp645)->tag == Cyc_Absyn_Tunion_d_tag){
_LL666: _temp665=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp645)->f1; goto _LL654;} else{ goto _LL655;} _LL655: if((( struct
_tunion_struct*) _temp645)->tag == Cyc_Absyn_XTunion_d_tag){ _LL668: _temp667=(
struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*) _temp645)->f1;
goto _LL656;} else{ goto _LL657;} _LL657: goto _LL658; _LL648:( void*)( _temp659->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp669=( char*)"bad attributes on enum"; struct _tagged_string _temp670;
_temp670.curr= _temp669; _temp670.base= _temp669; _temp670.last_plus_one=
_temp669 + 23; _temp670;}), loc);} goto _LL646; _LL650:( void*)( _temp661->sc=(
void*) s); _temp661->attributes= atts; goto _LL646; _LL652:( void*)( _temp663->sc=(
void*) s); _temp663->attributes= atts; goto _LL646; _LL654:( void*)( _temp665->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp671=( char*)"bad attributes on tunion"; struct _tagged_string _temp672;
_temp672.curr= _temp671; _temp672.base= _temp671; _temp672.last_plus_one=
_temp671 + 25; _temp672;}), loc);} goto _LL646; _LL656:( void*)( _temp667->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp673=( char*)"bad attributes on xtunion"; struct _tagged_string _temp674;
_temp674.curr= _temp673; _temp674.base= _temp673; _temp674.last_plus_one=
_temp673 + 26; _temp674;}), loc);} goto _LL646; _LL658: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp675=( char*)"bad declaration"; struct
_tagged_string _temp676; _temp676.curr= _temp675; _temp676.base= _temp675;
_temp676.last_plus_one= _temp675 + 16; _temp676;}), loc); return 0; _LL646:;}
return({ struct Cyc_List_List* _temp677=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp677->hd=( void*) d; _temp677->tl= 0;
_temp677;});} else{ void* _temp678= _temp643; struct Cyc_Absyn_Structdecl**
_temp692; struct Cyc_List_List* _temp694; struct _tuple1* _temp696; struct Cyc_Absyn_TunionInfo
_temp698; struct Cyc_Absyn_Tuniondecl* _temp700; void* _temp702; struct Cyc_List_List*
_temp704; struct _tuple1* _temp706; struct Cyc_Absyn_XTunionInfo _temp708;
struct Cyc_Absyn_XTuniondecl* _temp710; void* _temp712; struct _tuple1* _temp714;
struct Cyc_Absyn_Uniondecl** _temp716; struct Cyc_List_List* _temp718; struct
_tuple1* _temp720; struct Cyc_Absyn_Enumdecl* _temp722; struct _tuple1* _temp724;
_LL680: if(( unsigned int) _temp678 > 4u?(( struct _tunion_struct*) _temp678)->tag
== Cyc_Absyn_StructType_tag: 0){ _LL697: _temp696=( struct _tuple1*)(( struct
Cyc_Absyn_StructType_struct*) _temp678)->f1; goto _LL695; _LL695: _temp694=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp678)->f2;
goto _LL693; _LL693: _temp692=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp678)->f3; goto _LL681;} else{ goto _LL682;} _LL682: if(( unsigned int)
_temp678 > 4u?(( struct _tunion_struct*) _temp678)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL699: _temp698=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp678)->f1; _LL707: _temp706=( struct _tuple1*) _temp698.name; goto _LL705;
_LL705: _temp704=( struct Cyc_List_List*) _temp698.targs; goto _LL703; _LL703:
_temp702=( void*) _temp698.rgn; goto _LL701; _LL701: _temp700=( struct Cyc_Absyn_Tuniondecl*)
_temp698.tud; goto _LL683;} else{ goto _LL684;} _LL684: if(( unsigned int)
_temp678 > 4u?(( struct _tunion_struct*) _temp678)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL709: _temp708=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp678)->f1; _LL715: _temp714=( struct _tuple1*) _temp708.name; goto _LL713;
_LL713: _temp712=( void*) _temp708.rgn; goto _LL711; _LL711: _temp710=( struct
Cyc_Absyn_XTuniondecl*) _temp708.xtud; goto _LL685;} else{ goto _LL686;} _LL686:
if(( unsigned int) _temp678 > 4u?(( struct _tunion_struct*) _temp678)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL721: _temp720=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp678)->f1; goto _LL719; _LL719: _temp718=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp678)->f2; goto _LL717; _LL717: _temp716=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp678)->f3;
goto _LL687;} else{ goto _LL688;} _LL688: if(( unsigned int) _temp678 > 4u?((
struct _tunion_struct*) _temp678)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL725:
_temp724=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp678)->f1;
goto _LL723; _LL723: _temp722=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp678)->f2; goto _LL689;} else{ goto _LL690;} _LL690: goto _LL691; _LL681: {
struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp694);
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp726=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp726->sc=( void*) s; _temp726->name=({ struct Cyc_Core_Opt* _temp727=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp727->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp728= _temp696; if( _temp728 ==
0){ _throw( Null_Exception);} _temp728;})); _temp727;}); _temp726->tvs= ts2;
_temp726->fields= 0; _temp726->attributes= 0; _temp726;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp729=( char*)"bad attributes on struct";
struct _tagged_string _temp730; _temp730.curr= _temp729; _temp730.base= _temp729;
_temp730.last_plus_one= _temp729 + 25; _temp730;}), loc);} return({ struct Cyc_List_List*
_temp731=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp731->hd=( void*)({ struct Cyc_Absyn_Decl* _temp732=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp732->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp733=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp733[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp734; _temp734.tag= Cyc_Absyn_Struct_d_tag;
_temp734.f1= sd; _temp734;}); _temp733;})); _temp732->loc= loc; _temp732;});
_temp731->tl= 0; _temp731;});} _LL683: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp704); struct Cyc_Absyn_Decl* tud= Cyc_Absyn_tunion_decl( s,({ struct Cyc_Core_Opt*
_temp735=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp735->v=( void*)(( struct _tuple1*) _temp706); _temp735;}), ts2, 0, loc);
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp736=( char*)"bad attributes on tunion";
struct _tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 25; _temp737;}), loc);} return({ struct Cyc_List_List*
_temp738=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp738->hd=( void*) tud; _temp738->tl= 0; _temp738;});} _LL685: { struct Cyc_Absyn_XTuniondecl*
xtud=({ struct Cyc_Absyn_XTuniondecl* _temp739=( struct Cyc_Absyn_XTuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl)); _temp739->sc=( void*) s;
_temp739->name= _temp714; _temp739->fields= 0; _temp739;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp740=( char*)"bad attributes on xtunion";
struct _tagged_string _temp741; _temp741.curr= _temp740; _temp741.base= _temp740;
_temp741.last_plus_one= _temp740 + 26; _temp741;}), loc);} return({ struct Cyc_List_List*
_temp742=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp742->hd=( void*)({ struct Cyc_Absyn_Decl* _temp743=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp743->r=( void*)(( void*)({
struct Cyc_Absyn_XTunion_d_struct* _temp744=( struct Cyc_Absyn_XTunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_d_struct)); _temp744[ 0]=({ struct
Cyc_Absyn_XTunion_d_struct _temp745; _temp745.tag= Cyc_Absyn_XTunion_d_tag;
_temp745.f1= xtud; _temp745;}); _temp744;})); _temp743->loc= loc; _temp743;});
_temp742->tl= 0; _temp742;});} _LL687: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp718); struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl*
_temp746=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp746->sc=( void*) s; _temp746->name=({ struct Cyc_Core_Opt* _temp747=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp747->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp748= _temp720; if( _temp748 ==
0){ _throw( Null_Exception);} _temp748;})); _temp747;}); _temp746->tvs= ts2;
_temp746->fields= 0; _temp746->attributes= 0; _temp746;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp749=( char*)"bad attributes on union";
struct _tagged_string _temp750; _temp750.curr= _temp749; _temp750.base= _temp749;
_temp750.last_plus_one= _temp749 + 24; _temp750;}), loc);} return({ struct Cyc_List_List*
_temp751=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp751->hd=( void*)({ struct Cyc_Absyn_Decl* _temp752=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp752->r=( void*)(( void*)({
struct Cyc_Absyn_Union_d_struct* _temp753=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp753[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp754; _temp754.tag= Cyc_Absyn_Union_d_tag; _temp754.f1= ud; _temp754;});
_temp753;})); _temp752->loc= loc; _temp752;}); _temp751->tl= 0; _temp751;});}
_LL689: { struct Cyc_Absyn_Enumdecl* ed=({ struct Cyc_Absyn_Enumdecl* _temp755=(
struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp755->sc=( void*) s; _temp755->name= _temp724; _temp755->fields= 0; _temp755;});
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp756=( char*)"bad attributes on enum";
struct _tagged_string _temp757; _temp757.curr= _temp756; _temp757.base= _temp756;
_temp757.last_plus_one= _temp756 + 23; _temp757;}), loc);} return({ struct Cyc_List_List*
_temp758=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp758->hd=( void*)({ struct Cyc_Absyn_Decl* _temp759=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp759->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp760=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp760[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp761; _temp761.tag= Cyc_Absyn_Enum_d_tag; _temp761.f1= ed; _temp761;});
_temp760;})); _temp759->loc= loc; _temp759;}); _temp758->tl= 0; _temp758;});}
_LL691: Cyc_Parse_err(( struct _tagged_string)({ char* _temp762=( char*)"missing declarator";
struct _tagged_string _temp763; _temp763.curr= _temp762; _temp763.base= _temp762;
_temp763.last_plus_one= _temp762 + 19; _temp763;}), loc); return 0; _LL679:;}}
else{ void* t= ts_info.f1; struct Cyc_List_List* fields= Cyc_Parse_apply_tmss(
tq, t, _temp637, atts); if( istypedef){ if( ! exps_empty){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp764=( char*)"initializer in typedef declaration";
struct _tagged_string _temp765; _temp765.curr= _temp764; _temp765.base= _temp764;
_temp765.last_plus_one= _temp764 + 35; _temp765;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, fields); if( ts_info.f2 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)( ts_info.f2)->v;{ void*
_temp766=( void*) d->r; struct Cyc_Absyn_Structdecl* _temp780; struct Cyc_Absyn_Tuniondecl*
_temp782; struct Cyc_Absyn_XTuniondecl* _temp784; struct Cyc_Absyn_Uniondecl*
_temp786; struct Cyc_Absyn_Enumdecl* _temp788; _LL768: if((( struct
_tunion_struct*) _temp766)->tag == Cyc_Absyn_Struct_d_tag){ _LL781: _temp780=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp766)->f1;
goto _LL769;} else{ goto _LL770;} _LL770: if((( struct _tunion_struct*) _temp766)->tag
== Cyc_Absyn_Tunion_d_tag){ _LL783: _temp782=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp766)->f1; goto _LL771;} else{ goto
_LL772;} _LL772: if((( struct _tunion_struct*) _temp766)->tag == Cyc_Absyn_XTunion_d_tag){
_LL785: _temp784=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp766)->f1; goto _LL773;} else{ goto _LL774;} _LL774: if((( struct
_tunion_struct*) _temp766)->tag == Cyc_Absyn_Union_d_tag){ _LL787: _temp786=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp766)->f1;
goto _LL775;} else{ goto _LL776;} _LL776: if((( struct _tunion_struct*) _temp766)->tag
== Cyc_Absyn_Enum_d_tag){ _LL789: _temp788=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp766)->f1; goto _LL777;} else{ goto _LL778;}
_LL778: goto _LL779; _LL769:( void*)( _temp780->sc=( void*) s); _temp780->attributes=
atts; atts= 0; goto _LL767; _LL771:( void*)( _temp782->sc=( void*) s); goto
_LL767; _LL773:( void*)( _temp784->sc=( void*) s); goto _LL767; _LL775:( void*)(
_temp786->sc=( void*) s); goto _LL767; _LL777:( void*)( _temp788->sc=( void*) s);
goto _LL767; _LL779: Cyc_Parse_err(( struct _tagged_string)({ char* _temp790=(
char*)"declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp791; _temp791.curr= _temp790; _temp791.base= _temp790;
_temp791.last_plus_one= _temp790 + 70; _temp791;}), loc); goto _LL767; _LL767:;}
decls=({ struct Cyc_List_List* _temp792=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp792->hd=( void*) d; _temp792->tl= decls;
_temp792;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp793=
Cyc_Absyn_attribute2string(( void*) atts->hd); xprintf("bad attribute %.*s in typedef",
_temp793.last_plus_one - _temp793.curr, _temp793.curr);}), loc);} return decls;}}
else{ if( ts_info.f2 != 0){ Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp794=( char*)"nested type declaration within declarator"; struct
_tagged_string _temp795; _temp795.curr= _temp794; _temp795.base= _temp794;
_temp795.last_plus_one= _temp794 + 42; _temp795;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* ds= fields; for( 0; ds != 0; ds= ds->tl,
_temp635= _temp635->tl){ struct _tuple7 _temp798; struct Cyc_List_List* _temp799;
struct Cyc_List_List* _temp801; void* _temp803; struct Cyc_Absyn_Tqual* _temp805;
struct _tuple1* _temp807; struct _tuple7* _temp796=( struct _tuple7*) ds->hd;
_temp798=* _temp796; _LL808: _temp807= _temp798.f1; goto _LL806; _LL806:
_temp805= _temp798.f2; goto _LL804; _LL804: _temp803= _temp798.f3; goto _LL802;
_LL802: _temp801= _temp798.f4; goto _LL800; _LL800: _temp799= _temp798.f5; goto
_LL797; _LL797: if( _temp801 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp809=( char*)"bad type params, ignoring"; struct _tagged_string
_temp810; _temp810.curr= _temp809; _temp810.base= _temp809; _temp810.last_plus_one=
_temp809 + 26; _temp810;}), loc);} if( _temp635 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp811=( char*)"unexpected null in parse!"; struct
_tagged_string _temp812; _temp812.curr= _temp811; _temp812.base= _temp811;
_temp812.last_plus_one= _temp811 + 26; _temp812;}), loc);}{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*) _temp635->hd; struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl(
_temp807, _temp803, eopt); vd->tq= _temp805;( void*)( vd->sc=( void*) s); vd->attributes=
_temp799;{ struct Cyc_Absyn_Decl* d=({ struct Cyc_Absyn_Decl* _temp813=( struct
Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp813->r=( void*)((
void*)({ struct Cyc_Absyn_Var_d_struct* _temp814=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp814[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp815; _temp815.tag= Cyc_Absyn_Var_d_tag; _temp815.f1= vd; _temp815;});
_temp814;})); _temp813->loc= loc; _temp813;}); decls=({ struct Cyc_List_List*
_temp816=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp816->hd=( void*) d; _temp816->tl= decls; _temp816;});}}}} return(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}}
static void* Cyc_Parse_id_to_kind( struct _tagged_string s, struct Cyc_Position_Segment*
loc){ if( Cyc_String_strlen( s) !=( unsigned int) 1){ Cyc_Parse_err(({ struct
_tagged_string _temp817= s; xprintf("bad kind: %.*s", _temp817.last_plus_one -
_temp817.curr, _temp817.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;} else{
switch(({ struct _tagged_string _temp818= s; char* _temp820= _temp818.curr + 0;
if( _temp820 < _temp818.base? 1: _temp820 >= _temp818.last_plus_one){ _throw(
Null_Exception);}* _temp820;})){ case 'A': _LL821: return( void*) Cyc_Absyn_AnyKind;
case 'M': _LL822: return( void*) Cyc_Absyn_MemKind; case 'B': _LL823: return(
void*) Cyc_Absyn_BoxKind; case 'R': _LL824: return( void*) Cyc_Absyn_RgnKind;
case 'E': _LL825: return( void*) Cyc_Absyn_EffKind; default: _LL826: Cyc_Parse_err(({
struct _tagged_string _temp828= s; xprintf("bad kind: %.*s", _temp828.last_plus_one
- _temp828.curr, _temp828.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment*
loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1; Cyc_Lex_register_typedef( x);
if((* t).f5 != 0){ Cyc_Parse_err(({ struct _tagged_string _temp829= Cyc_Absyn_attribute2string((
void*)((* t).f5)->hd); xprintf("bad attribute %.*s within typedef", _temp829.last_plus_one
- _temp829.curr, _temp829.curr);}), loc);} return Cyc_Absyn_new_decl(( void*)({
struct Cyc_Absyn_Typedef_d_struct* _temp830=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp830[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp831; _temp831.tag= Cyc_Absyn_Typedef_d_tag;
_temp831.f1=({ struct Cyc_Absyn_Typedefdecl* _temp832=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp832->name= x; _temp832->tvs=(*
t).f4; _temp832->defn=( void*)(* t).f3; _temp832;}); _temp831;}); _temp830;}),
loc);} typedef struct _xtunion_struct* Cyc_YYSTYPE; char Cyc_Okay_tok_tag[ 9u]="Okay_tok";
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
f1; } ; char Cyc_TunionField_tok_tag[ 16u]="TunionField_tok"; struct Cyc_TunionField_tok_struct{
char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; char Cyc_TunionFieldList_tok_tag[
20u]="TunionFieldList_tok"; struct Cyc_TunionFieldList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; char Cyc_ParamDecl_tok_tag[ 14u]="ParamDecl_tok";
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
char* tag; struct Cyc_List_List* f1; } ; char Cyc_Enumfield_tok_tag[ 14u]="Enumfield_tok";
struct Cyc_Enumfield_tok_struct{ char* tag; struct Cyc_Absyn_Enumfield* f1; } ;
char Cyc_EnumfieldList_tok_tag[ 18u]="EnumfieldList_tok"; struct Cyc_EnumfieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; static char _temp835[ 8u]="parse.y";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp835, _temp835, _temp835 + 8u}}; struct _tuple12*
Cyc_yyget_Int_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp836= yy1; struct _tuple12* _temp842; _LL838: if((*(( struct _xtunion_struct*)
_temp836)).tag == Cyc_Int_tok_tag){ _LL843: _temp842=(( struct Cyc_Int_tok_struct*)
_temp836)->f1; goto _LL839;} else{ goto _LL840;} _LL840: goto _LL841; _LL839:
return _temp842; _LL841:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Int_tok);
_LL837:;} static char _temp846[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp846,
_temp846, _temp846 + 8u}}; struct _tagged_string Cyc_yyget_String_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp847= yy1; struct
_tagged_string _temp853; _LL849: if((*(( struct _xtunion_struct*) _temp847)).tag
== Cyc_String_tok_tag){ _LL854: _temp853=(( struct Cyc_String_tok_struct*)
_temp847)->f1; goto _LL850;} else{ goto _LL851;} _LL851: goto _LL852; _LL850:
return _temp853; _LL852:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_String_tok);
_LL848:;} static char _temp857[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp857,
_temp857, _temp857 + 8u}}; char Cyc_yyget_Char_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp858= yy1; char _temp864; _LL860: if((*(( struct
_xtunion_struct*) _temp858)).tag == Cyc_Char_tok_tag){ _LL865: _temp864=((
struct Cyc_Char_tok_struct*) _temp858)->f1; goto _LL861;} else{ goto _LL862;}
_LL862: goto _LL863; _LL861: return _temp864; _LL863:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Char_tok); _LL859:;} static char _temp868[ 8u]="parse.y";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp868, _temp868, _temp868 + 8u}}; void* Cyc_yyget_Pointer_Sort_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp869= yy1; void*
_temp875; _LL871: if((*(( struct _xtunion_struct*) _temp869)).tag == Cyc_Pointer_Sort_tok_tag){
_LL876: _temp875=(( struct Cyc_Pointer_Sort_tok_struct*) _temp869)->f1; goto
_LL872;} else{ goto _LL873;} _LL873: goto _LL874; _LL872: return _temp875;
_LL874:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Pointer_Sort_tok);
_LL870:;} static char _temp879[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp879,
_temp879, _temp879 + 8u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp880= yy1; struct Cyc_Absyn_Exp*
_temp886; _LL882: if((*(( struct _xtunion_struct*) _temp880)).tag == Cyc_Exp_tok_tag){
_LL887: _temp886=(( struct Cyc_Exp_tok_struct*) _temp880)->f1; goto _LL883;}
else{ goto _LL884;} _LL884: goto _LL885; _LL883: return _temp886; _LL885:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_Exp_tok); _LL881:;} static char
_temp890[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp890, _temp890, _temp890 + 8u}};
struct Cyc_List_List* Cyc_yyget_ExpList_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp891= yy1; struct Cyc_List_List* _temp897; _LL893:
if((*(( struct _xtunion_struct*) _temp891)).tag == Cyc_ExpList_tok_tag){ _LL898:
_temp897=(( struct Cyc_ExpList_tok_struct*) _temp891)->f1; goto _LL894;} else{
goto _LL895;} _LL895: goto _LL896; _LL894: return _temp897; _LL896:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_ExpList_tok); _LL892:;} static
char _temp901[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp901, _temp901, _temp901 + 8u}};
struct Cyc_List_List* Cyc_yyget_InitializerList_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp902= yy1; struct Cyc_List_List* _temp908; _LL904:
if((*(( struct _xtunion_struct*) _temp902)).tag == Cyc_InitializerList_tok_tag){
_LL909: _temp908=(( struct Cyc_InitializerList_tok_struct*) _temp902)->f1; goto
_LL905;} else{ goto _LL906;} _LL906: goto _LL907; _LL905: return _temp908;
_LL907:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_InitializerList_tok);
_LL903:;} static char _temp912[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp912,
_temp912, _temp912 + 8u}}; void* Cyc_yyget_Primop_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp913= yy1; void* _temp919; _LL915: if((*((
struct _xtunion_struct*) _temp913)).tag == Cyc_Primop_tok_tag){ _LL920: _temp919=((
struct Cyc_Primop_tok_struct*) _temp913)->f1; goto _LL916;} else{ goto _LL917;}
_LL917: goto _LL918; _LL916: return _temp919; _LL918:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Primop_tok); _LL914:;} static char _temp923[ 8u]="parse.y";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp923, _temp923, _temp923 + 8u}}; struct Cyc_Core_Opt*
Cyc_yyget_Primopopt_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp924= yy1; struct Cyc_Core_Opt* _temp930; _LL926: if((*(( struct
_xtunion_struct*) _temp924)).tag == Cyc_Primopopt_tok_tag){ _LL931: _temp930=((
struct Cyc_Primopopt_tok_struct*) _temp924)->f1; goto _LL927;} else{ goto _LL928;}
_LL928: goto _LL929; _LL927: return _temp930; _LL929:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Primopopt_tok); _LL925:;} static char _temp934[ 8u]="parse.y";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp934, _temp934, _temp934 + 8u}}; struct _tuple1* Cyc_yyget_QualId_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp935= yy1; struct
_tuple1* _temp941; _LL937: if((*(( struct _xtunion_struct*) _temp935)).tag ==
Cyc_QualId_tok_tag){ _LL942: _temp941=(( struct Cyc_QualId_tok_struct*) _temp935)->f1;
goto _LL938;} else{ goto _LL939;} _LL939: goto _LL940; _LL938: return _temp941;
_LL940:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_QualId_tok); _LL936:;}
static char _temp945[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp945, _temp945, _temp945 + 8u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp946= yy1; struct Cyc_Absyn_Stmt* _temp952; _LL948: if((*((
struct _xtunion_struct*) _temp946)).tag == Cyc_Stmt_tok_tag){ _LL953: _temp952=((
struct Cyc_Stmt_tok_struct*) _temp946)->f1; goto _LL949;} else{ goto _LL950;}
_LL950: goto _LL951; _LL949: return _temp952; _LL951:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Stmt_tok); _LL947:;} static char _temp956[ 8u]="parse.y";
static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp956, _temp956, _temp956 + 8u}}; void* Cyc_yyget_BlockItem_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp957= yy1; void*
_temp963; _LL959: if((*(( struct _xtunion_struct*) _temp957)).tag == Cyc_BlockItem_tok_tag){
_LL964: _temp963=(( struct Cyc_BlockItem_tok_struct*) _temp957)->f1; goto _LL960;}
else{ goto _LL961;} _LL961: goto _LL962; _LL960: return _temp963; _LL962:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_BlockItem_tok); _LL958:;} static
char _temp967[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_SwitchClauseList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp967, _temp967, _temp967 + 8u}};
struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp968= yy1; struct Cyc_List_List* _temp974;
_LL970: if((*(( struct _xtunion_struct*) _temp968)).tag == Cyc_SwitchClauseList_tok_tag){
_LL975: _temp974=(( struct Cyc_SwitchClauseList_tok_struct*) _temp968)->f1; goto
_LL971;} else{ goto _LL972;} _LL972: goto _LL973; _LL971: return _temp974;
_LL973:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_SwitchClauseList_tok);
_LL969:;} static char _temp978[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp978,
_temp978, _temp978 + 8u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp979= yy1; struct Cyc_Absyn_Pat*
_temp985; _LL981: if((*(( struct _xtunion_struct*) _temp979)).tag == Cyc_Pattern_tok_tag){
_LL986: _temp985=(( struct Cyc_Pattern_tok_struct*) _temp979)->f1; goto _LL982;}
else{ goto _LL983;} _LL983: goto _LL984; _LL982: return _temp985; _LL984:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_Pattern_tok); _LL980:;} static
char _temp989[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_PatternList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp989, _temp989, _temp989 + 8u}};
struct Cyc_List_List* Cyc_yyget_PatternList_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp990= yy1; struct Cyc_List_List* _temp996; _LL992:
if((*(( struct _xtunion_struct*) _temp990)).tag == Cyc_PatternList_tok_tag){
_LL997: _temp996=(( struct Cyc_PatternList_tok_struct*) _temp990)->f1; goto
_LL993;} else{ goto _LL994;} _LL994: goto _LL995; _LL993: return _temp996;
_LL995:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_PatternList_tok);
_LL991:;} static char _temp1000[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1000, _temp1000, _temp1000 + 8u}}; struct _tuple17* Cyc_yyget_FieldPattern_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1001= yy1; struct
_tuple17* _temp1007; _LL1003: if((*(( struct _xtunion_struct*) _temp1001)).tag
== Cyc_FieldPattern_tok_tag){ _LL1008: _temp1007=(( struct Cyc_FieldPattern_tok_struct*)
_temp1001)->f1; goto _LL1004;} else{ goto _LL1005;} _LL1005: goto _LL1006;
_LL1004: return _temp1007; _LL1006:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_FieldPattern_tok); _LL1002:;} static char _temp1011[ 8u]="parse.y";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1011, _temp1011, _temp1011 + 8u}}; struct Cyc_List_List*
Cyc_yyget_FieldPatternList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1012= yy1; struct Cyc_List_List* _temp1018; _LL1014: if((*((
struct _xtunion_struct*) _temp1012)).tag == Cyc_FieldPatternList_tok_tag){
_LL1019: _temp1018=(( struct Cyc_FieldPatternList_tok_struct*) _temp1012)->f1;
goto _LL1015;} else{ goto _LL1016;} _LL1016: goto _LL1017; _LL1015: return
_temp1018; _LL1017:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_FieldPatternList_tok);
_LL1013:;} static char _temp1022[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1022,
_temp1022, _temp1022 + 8u}}; struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1023= yy1; struct Cyc_Absyn_Fndecl*
_temp1029; _LL1025: if((*(( struct _xtunion_struct*) _temp1023)).tag == Cyc_FnDecl_tok_tag){
_LL1030: _temp1029=(( struct Cyc_FnDecl_tok_struct*) _temp1023)->f1; goto
_LL1026;} else{ goto _LL1027;} _LL1027: goto _LL1028; _LL1026: return _temp1029;
_LL1028:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_FnDecl_tok);
_LL1024:;} static char _temp1033[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1033, _temp1033, _temp1033 + 8u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1034= yy1; struct Cyc_List_List*
_temp1040; _LL1036: if((*(( struct _xtunion_struct*) _temp1034)).tag == Cyc_DeclList_tok_tag){
_LL1041: _temp1040=(( struct Cyc_DeclList_tok_struct*) _temp1034)->f1; goto
_LL1037;} else{ goto _LL1038;} _LL1038: goto _LL1039; _LL1037: return _temp1040;
_LL1039:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DeclList_tok);
_LL1035:;} static char _temp1044[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1044, _temp1044, _temp1044 + 8u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1045= yy1; struct Cyc_Parse_Declaration_spec*
_temp1051; _LL1047: if((*(( struct _xtunion_struct*) _temp1045)).tag == Cyc_DeclSpec_tok_tag){
_LL1052: _temp1051=(( struct Cyc_DeclSpec_tok_struct*) _temp1045)->f1; goto
_LL1048;} else{ goto _LL1049;} _LL1049: goto _LL1050; _LL1048: return _temp1051;
_LL1050:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DeclSpec_tok);
_LL1046:;} static char _temp1055[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1055, _temp1055, _temp1055 + 8u}}; struct _tuple13* Cyc_yyget_InitDecl_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1056= yy1; struct
_tuple13* _temp1062; _LL1058: if((*(( struct _xtunion_struct*) _temp1056)).tag
== Cyc_InitDecl_tok_tag){ _LL1063: _temp1062=(( struct Cyc_InitDecl_tok_struct*)
_temp1056)->f1; goto _LL1059;} else{ goto _LL1060;} _LL1060: goto _LL1061;
_LL1059: return _temp1062; _LL1061:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_InitDecl_tok); _LL1057:;} static char _temp1066[ 8u]="parse.y";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1066, _temp1066, _temp1066 + 8u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp1067= yy1; struct Cyc_List_List* _temp1073; _LL1069: if((*(( struct
_xtunion_struct*) _temp1067)).tag == Cyc_InitDeclList_tok_tag){ _LL1074:
_temp1073=(( struct Cyc_InitDeclList_tok_struct*) _temp1067)->f1; goto _LL1070;}
else{ goto _LL1071;} _LL1071: goto _LL1072; _LL1070: return _temp1073; _LL1072:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_InitDeclList_tok); _LL1068:;}
static char _temp1077[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_StorageClass_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1077, _temp1077, _temp1077 +
8u}}; void* Cyc_yyget_StorageClass_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1078= yy1; void* _temp1084; _LL1080: if((*(( struct
_xtunion_struct*) _temp1078)).tag == Cyc_StorageClass_tok_tag){ _LL1085:
_temp1084=(( struct Cyc_StorageClass_tok_struct*) _temp1078)->f1; goto _LL1081;}
else{ goto _LL1082;} _LL1082: goto _LL1083; _LL1081: return _temp1084; _LL1083:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StorageClass_tok); _LL1079:;}
static char _temp1088[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeSpecifier_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1088, _temp1088, _temp1088 +
8u}}; void* Cyc_yyget_TypeSpecifier_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1089= yy1; void* _temp1095; _LL1091: if((*(( struct
_xtunion_struct*) _temp1089)).tag == Cyc_TypeSpecifier_tok_tag){ _LL1096:
_temp1095=(( struct Cyc_TypeSpecifier_tok_struct*) _temp1089)->f1; goto _LL1092;}
else{ goto _LL1093;} _LL1093: goto _LL1094; _LL1092: return _temp1095; _LL1094:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeSpecifier_tok); _LL1090:;}
static char _temp1099[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_StructOrUnion_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1099, _temp1099, _temp1099 +
8u}}; void* Cyc_yyget_StructOrUnion_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1100= yy1; void* _temp1106; _LL1102: if((*(( struct
_xtunion_struct*) _temp1100)).tag == Cyc_StructOrUnion_tok_tag){ _LL1107:
_temp1106=(( struct Cyc_StructOrUnion_tok_struct*) _temp1100)->f1; goto _LL1103;}
else{ goto _LL1104;} _LL1104: goto _LL1105; _LL1103: return _temp1106; _LL1105:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StructOrUnion_tok); _LL1101:;}
static char _temp1110[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeQual_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1110, _temp1110, _temp1110 +
8u}}; struct Cyc_Absyn_Tqual* Cyc_yyget_TypeQual_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1111= yy1; struct Cyc_Absyn_Tqual* _temp1117;
_LL1113: if((*(( struct _xtunion_struct*) _temp1111)).tag == Cyc_TypeQual_tok_tag){
_LL1118: _temp1117=(( struct Cyc_TypeQual_tok_struct*) _temp1111)->f1; goto
_LL1114;} else{ goto _LL1115;} _LL1115: goto _LL1116; _LL1114: return _temp1117;
_LL1116:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeQual_tok);
_LL1112:;} static char _temp1121[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure_tag,( struct
_tagged_string){ _temp1121, _temp1121, _temp1121 + 8u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1122= yy1; struct Cyc_List_List* _temp1128; _LL1124: if((*((
struct _xtunion_struct*) _temp1122)).tag == Cyc_StructFieldDeclList_tok_tag){
_LL1129: _temp1128=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp1122)->f1;
goto _LL1125;} else{ goto _LL1126;} _LL1126: goto _LL1127; _LL1125: return
_temp1128; _LL1127:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL1123:;} static char _temp1132[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure_tag,( struct
_tagged_string){ _temp1132, _temp1132, _temp1132 + 8u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1133= yy1; struct Cyc_List_List* _temp1139; _LL1135: if((*((
struct _xtunion_struct*) _temp1133)).tag == Cyc_StructFieldDeclListList_tok_tag){
_LL1140: _temp1139=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp1133)->f1;
goto _LL1136;} else{ goto _LL1137;} _LL1137: goto _LL1138; _LL1136: return
_temp1139; _LL1138:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL1134:;} static char _temp1143[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1143, _temp1143, _temp1143 + 8u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1144= yy1; struct Cyc_List_List*
_temp1150; _LL1146: if((*(( struct _xtunion_struct*) _temp1144)).tag == Cyc_TypeModifierList_tok_tag){
_LL1151: _temp1150=(( struct Cyc_TypeModifierList_tok_struct*) _temp1144)->f1;
goto _LL1147;} else{ goto _LL1148;} _LL1148: goto _LL1149; _LL1147: return
_temp1150; _LL1149:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeModifierList_tok);
_LL1145:;} static char _temp1154[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1154,
_temp1154, _temp1154 + 8u}}; void* Cyc_yyget_Rgn_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1155= yy1; void* _temp1161; _LL1157: if((*((
struct _xtunion_struct*) _temp1155)).tag == Cyc_Rgn_tok_tag){ _LL1162: _temp1161=((
struct Cyc_Rgn_tok_struct*) _temp1155)->f1; goto _LL1158;} else{ goto _LL1159;}
_LL1159: goto _LL1160; _LL1158: return _temp1161; _LL1160:( void) _throw((
struct _xtunion_struct*)& Cyc_yyfail_Rgn_tok); _LL1156:;} static char _temp1165[
8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1165, _temp1165, _temp1165 +
8u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp1166= yy1; struct Cyc_Parse_Declarator*
_temp1172; _LL1168: if((*(( struct _xtunion_struct*) _temp1166)).tag == Cyc_Declarator_tok_tag){
_LL1173: _temp1172=(( struct Cyc_Declarator_tok_struct*) _temp1166)->f1; goto
_LL1169;} else{ goto _LL1170;} _LL1170: goto _LL1171; _LL1169: return _temp1172;
_LL1171:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Declarator_tok);
_LL1167:;} static char _temp1176[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1176, _temp1176, _temp1176 + 8u}}; struct _tuple15* Cyc_yyget_DeclaratorExpopt_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1177= yy1; struct
_tuple15* _temp1183; _LL1179: if((*(( struct _xtunion_struct*) _temp1177)).tag
== Cyc_DeclaratorExpopt_tok_tag){ _LL1184: _temp1183=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1177)->f1; goto _LL1180;} else{ goto _LL1181;} _LL1181: goto _LL1182;
_LL1180: return _temp1183; _LL1182:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_DeclaratorExpopt_tok); _LL1178:;} static char _temp1187[ 8u]="parse.y";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1187, _temp1187, _temp1187 + 8u}}; struct Cyc_List_List*
Cyc_yyget_DeclaratorExpoptList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1188= yy1; struct Cyc_List_List* _temp1194; _LL1190: if((*((
struct _xtunion_struct*) _temp1188)).tag == Cyc_DeclaratorExpoptList_tok_tag){
_LL1195: _temp1194=(( struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1188)->f1;
goto _LL1191;} else{ goto _LL1192;} _LL1192: goto _LL1193; _LL1191: return
_temp1194; _LL1193:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DeclaratorExpoptList_tok);
_LL1189:;} static char _temp1198[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1198, _temp1198, _temp1198 + 8u}}; struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1199= yy1; struct Cyc_Parse_Abstractdeclarator*
_temp1205; _LL1201: if((*(( struct _xtunion_struct*) _temp1199)).tag == Cyc_AbstractDeclarator_tok_tag){
_LL1206: _temp1205=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1199)->f1;
goto _LL1202;} else{ goto _LL1203;} _LL1203: goto _LL1204; _LL1202: return
_temp1205; _LL1204:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL1200:;} static char _temp1209[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1209, _temp1209, _temp1209 + 8u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1210= yy1; struct Cyc_Absyn_Tunionfield*
_temp1216; _LL1212: if((*(( struct _xtunion_struct*) _temp1210)).tag == Cyc_TunionField_tok_tag){
_LL1217: _temp1216=(( struct Cyc_TunionField_tok_struct*) _temp1210)->f1; goto
_LL1213;} else{ goto _LL1214;} _LL1214: goto _LL1215; _LL1213: return _temp1216;
_LL1215:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TunionField_tok);
_LL1211:;} static char _temp1220[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1220, _temp1220, _temp1220 + 8u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1221= yy1; struct Cyc_List_List*
_temp1227; _LL1223: if((*(( struct _xtunion_struct*) _temp1221)).tag == Cyc_TunionFieldList_tok_tag){
_LL1228: _temp1227=(( struct Cyc_TunionFieldList_tok_struct*) _temp1221)->f1;
goto _LL1224;} else{ goto _LL1225;} _LL1225: goto _LL1226; _LL1224: return
_temp1227; _LL1226:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TunionFieldList_tok);
_LL1222:;} static char _temp1231[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1231, _temp1231, _temp1231 + 8u}}; struct _tuple14* Cyc_yyget_QualSpecList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1232= yy1; struct
_tuple14* _temp1238; _LL1234: if((*(( struct _xtunion_struct*) _temp1232)).tag
== Cyc_QualSpecList_tok_tag){ _LL1239: _temp1238=(( struct Cyc_QualSpecList_tok_struct*)
_temp1232)->f1; goto _LL1235;} else{ goto _LL1236;} _LL1236: goto _LL1237;
_LL1235: return _temp1238; _LL1237:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_QualSpecList_tok); _LL1233:;} static char _temp1242[ 8u]="parse.y";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1242, _temp1242, _temp1242 + 8u}}; struct Cyc_List_List*
Cyc_yyget_IdList_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp1243= yy1; struct Cyc_List_List* _temp1249; _LL1245: if((*(( struct
_xtunion_struct*) _temp1243)).tag == Cyc_IdList_tok_tag){ _LL1250: _temp1249=((
struct Cyc_IdList_tok_struct*) _temp1243)->f1; goto _LL1246;} else{ goto _LL1247;}
_LL1247: goto _LL1248; _LL1246: return _temp1249; _LL1248:( void) _throw((
struct _xtunion_struct*)& Cyc_yyfail_IdList_tok); _LL1244:;} static char
_temp1253[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1253, _temp1253, _temp1253 +
8u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp1254= yy1; struct _tuple2* _temp1260; _LL1256: if((*((
struct _xtunion_struct*) _temp1254)).tag == Cyc_ParamDecl_tok_tag){ _LL1261:
_temp1260=(( struct Cyc_ParamDecl_tok_struct*) _temp1254)->f1; goto _LL1257;}
else{ goto _LL1258;} _LL1258: goto _LL1259; _LL1257: return _temp1260; _LL1259:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_ParamDecl_tok); _LL1255:;}
static char _temp1264[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1264, _temp1264, _temp1264 +
8u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1265= yy1; struct Cyc_List_List* _temp1271;
_LL1267: if((*(( struct _xtunion_struct*) _temp1265)).tag == Cyc_ParamDeclList_tok_tag){
_LL1272: _temp1271=(( struct Cyc_ParamDeclList_tok_struct*) _temp1265)->f1; goto
_LL1268;} else{ goto _LL1269;} _LL1269: goto _LL1270; _LL1268: return _temp1271;
_LL1270:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_ParamDeclList_tok);
_LL1266:;} static char _temp1275[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1275, _temp1275, _temp1275 + 8u}}; struct _tuple16* Cyc_yyget_ParamDeclListBool_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1276= yy1; struct
_tuple16* _temp1282; _LL1278: if((*(( struct _xtunion_struct*) _temp1276)).tag
== Cyc_ParamDeclListBool_tok_tag){ _LL1283: _temp1282=(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp1276)->f1; goto _LL1279;} else{ goto _LL1280;} _LL1280: goto _LL1281;
_LL1279: return _temp1282; _LL1281:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_ParamDeclListBool_tok); _LL1277:;} static char _temp1286[ 8u]="parse.y";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1286, _temp1286, _temp1286 + 8u}}; struct Cyc_List_List*
Cyc_yyget_TypeList_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp1287= yy1; struct Cyc_List_List* _temp1293; _LL1289: if((*(( struct
_xtunion_struct*) _temp1287)).tag == Cyc_TypeList_tok_tag){ _LL1294: _temp1293=((
struct Cyc_TypeList_tok_struct*) _temp1287)->f1; goto _LL1290;} else{ goto
_LL1291;} _LL1291: goto _LL1292; _LL1290: return _temp1293; _LL1292:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeList_tok); _LL1288:;} static
char _temp1297[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1297, _temp1297, _temp1297 +
8u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1298= yy1; struct Cyc_List_List* _temp1304;
_LL1300: if((*(( struct _xtunion_struct*) _temp1298)).tag == Cyc_DesignatorList_tok_tag){
_LL1305: _temp1304=(( struct Cyc_DesignatorList_tok_struct*) _temp1298)->f1;
goto _LL1301;} else{ goto _LL1302;} _LL1302: goto _LL1303; _LL1301: return
_temp1304; _LL1303:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DesignatorList_tok);
_LL1299:;} static char _temp1308[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1308, _temp1308, _temp1308 + 8u}}; void* Cyc_yyget_Designator_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp1309= yy1; void* _temp1315;
_LL1311: if((*(( struct _xtunion_struct*) _temp1309)).tag == Cyc_Designator_tok_tag){
_LL1316: _temp1315=(( struct Cyc_Designator_tok_struct*) _temp1309)->f1; goto
_LL1312;} else{ goto _LL1313;} _LL1313: goto _LL1314; _LL1312: return _temp1315;
_LL1314:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Designator_tok);
_LL1310:;} static char _temp1319[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1319,
_temp1319, _temp1319 + 8u}}; void* Cyc_yyget_Kind_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1320= yy1; void* _temp1326; _LL1322: if((*((
struct _xtunion_struct*) _temp1320)).tag == Cyc_Kind_tok_tag){ _LL1327:
_temp1326=(( struct Cyc_Kind_tok_struct*) _temp1320)->f1; goto _LL1323;} else{
goto _LL1324;} _LL1324: goto _LL1325; _LL1323: return _temp1326; _LL1325:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_Kind_tok); _LL1321:;} static char
_temp1330[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1330, _temp1330, _temp1330 +
8u}}; void* Cyc_yyget_Type_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1331= yy1; void* _temp1337; _LL1333: if((*(( struct
_xtunion_struct*) _temp1331)).tag == Cyc_Type_tok_tag){ _LL1338: _temp1337=((
struct Cyc_Type_tok_struct*) _temp1331)->f1; goto _LL1334;} else{ goto _LL1335;}
_LL1335: goto _LL1336; _LL1334: return _temp1337; _LL1336:( void) _throw((
struct _xtunion_struct*)& Cyc_yyfail_Type_tok); _LL1332:;} static char _temp1341[
8u]="parse.y"; static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1341, _temp1341, _temp1341 +
8u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1342= yy1; struct Cyc_List_List* _temp1348;
_LL1344: if((*(( struct _xtunion_struct*) _temp1342)).tag == Cyc_AttributeList_tok_tag){
_LL1349: _temp1348=(( struct Cyc_AttributeList_tok_struct*) _temp1342)->f1; goto
_LL1345;} else{ goto _LL1346;} _LL1346: goto _LL1347; _LL1345: return _temp1348;
_LL1347:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_AttributeList_tok);
_LL1343:;} static char _temp1352[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1352, _temp1352, _temp1352 + 8u}}; void* Cyc_yyget_Attribute_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp1353= yy1; void* _temp1359;
_LL1355: if((*(( struct _xtunion_struct*) _temp1353)).tag == Cyc_Attribute_tok_tag){
_LL1360: _temp1359=(( struct Cyc_Attribute_tok_struct*) _temp1353)->f1; goto
_LL1356;} else{ goto _LL1357;} _LL1357: goto _LL1358; _LL1356: return _temp1359;
_LL1358:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Attribute_tok);
_LL1354:;} static char _temp1363[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1363, _temp1363, _temp1363 + 8u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1364= yy1; struct Cyc_Absyn_Enumfield*
_temp1370; _LL1366: if((*(( struct _xtunion_struct*) _temp1364)).tag == Cyc_Enumfield_tok_tag){
_LL1371: _temp1370=(( struct Cyc_Enumfield_tok_struct*) _temp1364)->f1; goto
_LL1367;} else{ goto _LL1368;} _LL1368: goto _LL1369; _LL1367: return _temp1370;
_LL1369:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Enumfield_tok);
_LL1365:;} static char _temp1374[ 8u]="parse.y"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1374, _temp1374, _temp1374 + 8u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1375= yy1; struct Cyc_List_List*
_temp1381; _LL1377: if((*(( struct _xtunion_struct*) _temp1375)).tag == Cyc_EnumfieldList_tok_tag){
_LL1382: _temp1381=(( struct Cyc_EnumfieldList_tok_struct*) _temp1375)->f1; goto
_LL1378;} else{ goto _LL1379;} _LL1379: goto _LL1380; _LL1378: return _temp1381;
_LL1380:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_EnumfieldList_tok);
_LL1376:;} struct Cyc_Yyltype{ int timestamp; int first_line; int first_column;
int last_line; int last_column; struct _tagged_string text; } ; typedef struct
Cyc_Yyltype Cyc_yyltype; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype
_temp1383; _temp1383.timestamp= 0; _temp1383.first_line= 0; _temp1383.first_column=
0; _temp1383.last_line= 0; _temp1383.last_column= 0; _temp1383.text=( struct
_tagged_string)({ char* _temp1384=( char*)""; struct _tagged_string _temp1385;
_temp1385.curr= _temp1384; _temp1385.base= _temp1384; _temp1385.last_plus_one=
_temp1384 + 1; _temp1385;}); _temp1383;});} static char _temp1388[ 1u]="";
struct Cyc_Yyltype Cyc_yylloc=( struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column=
0,.last_line= 0,.last_column= 0,.text=( struct _tagged_string){ _temp1388,
_temp1388, _temp1388 + 1u}}; static short Cyc_yytranslate[ 348u]={ (short)0,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)119, (short)2, (short)2, (short)101, (short)117, (short)113, (short)2,
(short)97, (short)98, (short)108, (short)111, (short)99, (short)112, (short)105,
(short)116, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)104, (short)93, (short)102, (short)96,
(short)103, (short)110, (short)109, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)106, (short)2,
(short)107, (short)115, (short)100, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)94,
(short)114, (short)95, (short)118, (short)2, (short)2, (short)2, (short)2,
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
(short)2, (short)2, (short)2, (short)2, (short)2, (short)1, (short)2, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, (short)22, (short)23, (short)24, (short)25,
(short)26, (short)27, (short)28, (short)29, (short)30, (short)31, (short)32,
(short)33, (short)34, (short)35, (short)36, (short)37, (short)38, (short)39,
(short)40, (short)41, (short)42, (short)43, (short)44, (short)45, (short)46,
(short)47, (short)48, (short)49, (short)50, (short)51, (short)52, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, (short)60,
(short)61, (short)62, (short)63, (short)64, (short)65, (short)66, (short)67,
(short)68, (short)69, (short)70, (short)71, (short)72, (short)73, (short)74,
(short)75, (short)76, (short)77, (short)78, (short)79, (short)80, (short)81,
(short)82, (short)83, (short)84, (short)85, (short)86, (short)87, (short)88,
(short)89, (short)90, (short)91, (short)92}; static char _temp1391[ 2u]="$";
static char _temp1394[ 6u]="error"; static char _temp1397[ 12u]="$undefined.";
static char _temp1400[ 5u]="AUTO"; static char _temp1403[ 9u]="REGISTER"; static
char _temp1406[ 7u]="STATIC"; static char _temp1409[ 7u]="EXTERN"; static char
_temp1412[ 8u]="TYPEDEF"; static char _temp1415[ 5u]="VOID"; static char
_temp1418[ 5u]="CHAR"; static char _temp1421[ 6u]="SHORT"; static char _temp1424[
4u]="INT"; static char _temp1427[ 5u]="LONG"; static char _temp1430[ 6u]="FLOAT";
static char _temp1433[ 7u]="DOUBLE"; static char _temp1436[ 7u]="SIGNED"; static
char _temp1439[ 9u]="UNSIGNED"; static char _temp1442[ 6u]="CONST"; static char
_temp1445[ 9u]="VOLATILE"; static char _temp1448[ 9u]="RESTRICT"; static char
_temp1451[ 7u]="STRUCT"; static char _temp1454[ 6u]="UNION"; static char
_temp1457[ 5u]="CASE"; static char _temp1460[ 8u]="DEFAULT"; static char
_temp1463[ 7u]="INLINE"; static char _temp1466[ 3u]="IF"; static char _temp1469[
5u]="ELSE"; static char _temp1472[ 7u]="SWITCH"; static char _temp1475[ 6u]="WHILE";
static char _temp1478[ 3u]="DO"; static char _temp1481[ 4u]="FOR"; static char
_temp1484[ 5u]="GOTO"; static char _temp1487[ 9u]="CONTINUE"; static char
_temp1490[ 6u]="BREAK"; static char _temp1493[ 7u]="RETURN"; static char
_temp1496[ 7u]="SIZEOF"; static char _temp1499[ 5u]="ENUM"; static char
_temp1502[ 8u]="NULL_kw"; static char _temp1505[ 4u]="LET"; static char
_temp1508[ 6u]="THROW"; static char _temp1511[ 4u]="TRY"; static char _temp1514[
6u]="CATCH"; static char _temp1517[ 4u]="NEW"; static char _temp1520[ 9u]="ABSTRACT";
static char _temp1523[ 9u]="FALLTHRU"; static char _temp1526[ 6u]="USING";
static char _temp1529[ 10u]="NAMESPACE"; static char _temp1532[ 7u]="TUNION";
static char _temp1535[ 8u]="XTUNION"; static char _temp1538[ 5u]="FILL"; static
char _temp1541[ 8u]="CODEGEN"; static char _temp1544[ 4u]="CUT"; static char
_temp1547[ 7u]="SPLICE"; static char _temp1550[ 7u]="PRINTF"; static char
_temp1553[ 8u]="FPRINTF"; static char _temp1556[ 8u]="XPRINTF"; static char
_temp1559[ 6u]="SCANF"; static char _temp1562[ 7u]="FSCANF"; static char
_temp1565[ 7u]="SSCANF"; static char _temp1568[ 7u]="MALLOC"; static char
_temp1571[ 7u]="PTR_OP"; static char _temp1574[ 7u]="INC_OP"; static char
_temp1577[ 7u]="DEC_OP"; static char _temp1580[ 8u]="LEFT_OP"; static char
_temp1583[ 9u]="RIGHT_OP"; static char _temp1586[ 6u]="LE_OP"; static char
_temp1589[ 6u]="GE_OP"; static char _temp1592[ 6u]="EQ_OP"; static char
_temp1595[ 6u]="NE_OP"; static char _temp1598[ 7u]="AND_OP"; static char
_temp1601[ 6u]="OR_OP"; static char _temp1604[ 11u]="MUL_ASSIGN"; static char
_temp1607[ 11u]="DIV_ASSIGN"; static char _temp1610[ 11u]="MOD_ASSIGN"; static
char _temp1613[ 11u]="ADD_ASSIGN"; static char _temp1616[ 11u]="SUB_ASSIGN";
static char _temp1619[ 12u]="LEFT_ASSIGN"; static char _temp1622[ 13u]="RIGHT_ASSIGN";
static char _temp1625[ 11u]="AND_ASSIGN"; static char _temp1628[ 11u]="XOR_ASSIGN";
static char _temp1631[ 10u]="OR_ASSIGN"; static char _temp1634[ 9u]="ELLIPSIS";
static char _temp1637[ 11u]="LEFT_RIGHT"; static char _temp1640[ 12u]="COLON_COLON";
static char _temp1643[ 11u]="IDENTIFIER"; static char _temp1646[ 17u]="INTEGER_CONSTANT";
static char _temp1649[ 7u]="STRING"; static char _temp1652[ 19u]="CHARACTER_CONSTANT";
static char _temp1655[ 18u]="FLOATING_CONSTANT"; static char _temp1658[ 9u]="TYPE_VAR";
static char _temp1661[ 16u]="QUAL_IDENTIFIER"; static char _temp1664[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1667[ 10u]="ATTRIBUTE"; static char _temp1670[ 4u]="';'";
static char _temp1673[ 4u]="'{'"; static char _temp1676[ 4u]="'}'"; static char
_temp1679[ 4u]="'='"; static char _temp1682[ 4u]="'('"; static char _temp1685[ 4u]="')'";
static char _temp1688[ 4u]="','"; static char _temp1691[ 4u]="'_'"; static char
_temp1694[ 4u]="'$'"; static char _temp1697[ 4u]="'<'"; static char _temp1700[ 4u]="'>'";
static char _temp1703[ 4u]="':'"; static char _temp1706[ 4u]="'.'"; static char
_temp1709[ 4u]="'['"; static char _temp1712[ 4u]="']'"; static char _temp1715[ 4u]="'*'";
static char _temp1718[ 4u]="'@'"; static char _temp1721[ 4u]="'?'"; static char
_temp1724[ 4u]="'+'"; static char _temp1727[ 4u]="'-'"; static char _temp1730[ 4u]="'&'";
static char _temp1733[ 4u]="'|'"; static char _temp1736[ 4u]="'^'"; static char
_temp1739[ 4u]="'/'"; static char _temp1742[ 4u]="'%'"; static char _temp1745[ 4u]="'~'";
static char _temp1748[ 4u]="'!'"; static char _temp1751[ 5u]="prog"; static char
_temp1754[ 17u]="translation_unit"; static char _temp1757[ 21u]="translation_unit_opt";
static char _temp1760[ 21u]="external_declaration"; static char _temp1763[ 20u]="function_definition";
static char _temp1766[ 21u]="function_definition2"; static char _temp1769[ 13u]="using_action";
static char _temp1772[ 15u]="unusing_action"; static char _temp1775[ 17u]="namespace_action";
static char _temp1778[ 19u]="unnamespace_action"; static char _temp1781[ 12u]="declaration";
static char _temp1784[ 17u]="declaration_list"; static char _temp1787[ 23u]="declaration_specifiers";
static char _temp1790[ 24u]="storage_class_specifier"; static char _temp1793[ 15u]="attributes_opt";
static char _temp1796[ 11u]="attributes"; static char _temp1799[ 15u]="attribute_list";
static char _temp1802[ 10u]="attribute"; static char _temp1805[ 15u]="type_specifier";
static char _temp1808[ 5u]="kind"; static char _temp1811[ 15u]="type_qualifier";
static char _temp1814[ 15u]="enum_specifier"; static char _temp1817[ 11u]="enum_field";
static char _temp1820[ 22u]="enum_declaration_list"; static char _temp1823[ 26u]="struct_or_union_specifier";
static char _temp1826[ 16u]="type_params_opt"; static char _temp1829[ 16u]="struct_or_union";
static char _temp1832[ 24u]="struct_declaration_list"; static char _temp1835[ 25u]="struct_declaration_list0";
static char _temp1838[ 21u]="init_declarator_list"; static char _temp1841[ 22u]="init_declarator_list0";
static char _temp1844[ 16u]="init_declarator"; static char _temp1847[ 19u]="struct_declaration";
static char _temp1850[ 25u]="specifier_qualifier_list"; static char _temp1853[
23u]="struct_declarator_list"; static char _temp1856[ 24u]="struct_declarator_list0";
static char _temp1859[ 18u]="struct_declarator"; static char _temp1862[ 17u]="tunion_specifier";
static char _temp1865[ 17u]="tunionfield_list"; static char _temp1868[ 12u]="tunionfield";
static char _temp1871[ 11u]="declarator"; static char _temp1874[ 18u]="direct_declarator";
static char _temp1877[ 8u]="pointer"; static char _temp1880[ 13u]="pointer_char";
static char _temp1883[ 8u]="rgn_opt"; static char _temp1886[ 4u]="rgn"; static
char _temp1889[ 20u]="type_qualifier_list"; static char _temp1892[ 20u]="parameter_type_list";
static char _temp1895[ 11u]="effect_set"; static char _temp1898[ 14u]="atomic_effect";
static char _temp1901[ 11u]="region_set"; static char _temp1904[ 15u]="parameter_list";
static char _temp1907[ 22u]="parameter_declaration"; static char _temp1910[ 16u]="identifier_list";
static char _temp1913[ 17u]="identifier_list0"; static char _temp1916[ 12u]="initializer";
static char _temp1919[ 18u]="array_initializer"; static char _temp1922[ 17u]="initializer_list";
static char _temp1925[ 12u]="designation"; static char _temp1928[ 16u]="designator_list";
static char _temp1931[ 11u]="designator"; static char _temp1934[ 10u]="type_name";
static char _temp1937[ 14u]="any_type_name"; static char _temp1940[ 15u]="type_name_list";
static char _temp1943[ 20u]="abstract_declarator"; static char _temp1946[ 27u]="direct_abstract_declarator";
static char _temp1949[ 10u]="statement"; static char _temp1952[ 18u]="labeled_statement";
static char _temp1955[ 21u]="expression_statement"; static char _temp1958[ 19u]="compound_statement";
static char _temp1961[ 16u]="block_item_list"; static char _temp1964[ 11u]="block_item";
static char _temp1967[ 20u]="selection_statement"; static char _temp1970[ 15u]="switch_clauses";
static char _temp1973[ 20u]="iteration_statement"; static char _temp1976[ 15u]="jump_statement";
static char _temp1979[ 8u]="pattern"; static char _temp1982[ 19u]="tuple_pattern_list";
static char _temp1985[ 20u]="tuple_pattern_list0"; static char _temp1988[ 14u]="field_pattern";
static char _temp1991[ 19u]="field_pattern_list"; static char _temp1994[ 20u]="field_pattern_list0";
static char _temp1997[ 11u]="expression"; static char _temp2000[ 22u]="assignment_expression";
static char _temp2003[ 20u]="assignment_operator"; static char _temp2006[ 23u]="conditional_expression";
static char _temp2009[ 20u]="constant_expression"; static char _temp2012[ 22u]="logical_or_expression";
static char _temp2015[ 23u]="logical_and_expression"; static char _temp2018[ 24u]="inclusive_or_expression";
static char _temp2021[ 24u]="exclusive_or_expression"; static char _temp2024[ 15u]="and_expression";
static char _temp2027[ 20u]="equality_expression"; static char _temp2030[ 22u]="relational_expression";
static char _temp2033[ 17u]="shift_expression"; static char _temp2036[ 20u]="additive_expression";
static char _temp2039[ 26u]="multiplicative_expression"; static char _temp2042[
16u]="cast_expression"; static char _temp2045[ 17u]="unary_expression"; static
char _temp2048[ 14u]="format_primop"; static char _temp2051[ 15u]="unary_operator";
static char _temp2054[ 19u]="postfix_expression"; static char _temp2057[ 19u]="primary_expression";
static char _temp2060[ 25u]="argument_expression_list"; static char _temp2063[
26u]="argument_expression_list0"; static char _temp2066[ 9u]="constant"; static
char _temp2069[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
227u]={( struct _tagged_string){ _temp1391, _temp1391, _temp1391 + 2u},( struct
_tagged_string){ _temp1394, _temp1394, _temp1394 + 6u},( struct _tagged_string){
_temp1397, _temp1397, _temp1397 + 12u},( struct _tagged_string){ _temp1400,
_temp1400, _temp1400 + 5u},( struct _tagged_string){ _temp1403, _temp1403,
_temp1403 + 9u},( struct _tagged_string){ _temp1406, _temp1406, _temp1406 + 7u},(
struct _tagged_string){ _temp1409, _temp1409, _temp1409 + 7u},( struct
_tagged_string){ _temp1412, _temp1412, _temp1412 + 8u},( struct _tagged_string){
_temp1415, _temp1415, _temp1415 + 5u},( struct _tagged_string){ _temp1418,
_temp1418, _temp1418 + 5u},( struct _tagged_string){ _temp1421, _temp1421,
_temp1421 + 6u},( struct _tagged_string){ _temp1424, _temp1424, _temp1424 + 4u},(
struct _tagged_string){ _temp1427, _temp1427, _temp1427 + 5u},( struct
_tagged_string){ _temp1430, _temp1430, _temp1430 + 6u},( struct _tagged_string){
_temp1433, _temp1433, _temp1433 + 7u},( struct _tagged_string){ _temp1436,
_temp1436, _temp1436 + 7u},( struct _tagged_string){ _temp1439, _temp1439,
_temp1439 + 9u},( struct _tagged_string){ _temp1442, _temp1442, _temp1442 + 6u},(
struct _tagged_string){ _temp1445, _temp1445, _temp1445 + 9u},( struct
_tagged_string){ _temp1448, _temp1448, _temp1448 + 9u},( struct _tagged_string){
_temp1451, _temp1451, _temp1451 + 7u},( struct _tagged_string){ _temp1454,
_temp1454, _temp1454 + 6u},( struct _tagged_string){ _temp1457, _temp1457,
_temp1457 + 5u},( struct _tagged_string){ _temp1460, _temp1460, _temp1460 + 8u},(
struct _tagged_string){ _temp1463, _temp1463, _temp1463 + 7u},( struct
_tagged_string){ _temp1466, _temp1466, _temp1466 + 3u},( struct _tagged_string){
_temp1469, _temp1469, _temp1469 + 5u},( struct _tagged_string){ _temp1472,
_temp1472, _temp1472 + 7u},( struct _tagged_string){ _temp1475, _temp1475,
_temp1475 + 6u},( struct _tagged_string){ _temp1478, _temp1478, _temp1478 + 3u},(
struct _tagged_string){ _temp1481, _temp1481, _temp1481 + 4u},( struct
_tagged_string){ _temp1484, _temp1484, _temp1484 + 5u},( struct _tagged_string){
_temp1487, _temp1487, _temp1487 + 9u},( struct _tagged_string){ _temp1490,
_temp1490, _temp1490 + 6u},( struct _tagged_string){ _temp1493, _temp1493,
_temp1493 + 7u},( struct _tagged_string){ _temp1496, _temp1496, _temp1496 + 7u},(
struct _tagged_string){ _temp1499, _temp1499, _temp1499 + 5u},( struct
_tagged_string){ _temp1502, _temp1502, _temp1502 + 8u},( struct _tagged_string){
_temp1505, _temp1505, _temp1505 + 4u},( struct _tagged_string){ _temp1508,
_temp1508, _temp1508 + 6u},( struct _tagged_string){ _temp1511, _temp1511,
_temp1511 + 4u},( struct _tagged_string){ _temp1514, _temp1514, _temp1514 + 6u},(
struct _tagged_string){ _temp1517, _temp1517, _temp1517 + 4u},( struct
_tagged_string){ _temp1520, _temp1520, _temp1520 + 9u},( struct _tagged_string){
_temp1523, _temp1523, _temp1523 + 9u},( struct _tagged_string){ _temp1526,
_temp1526, _temp1526 + 6u},( struct _tagged_string){ _temp1529, _temp1529,
_temp1529 + 10u},( struct _tagged_string){ _temp1532, _temp1532, _temp1532 + 7u},(
struct _tagged_string){ _temp1535, _temp1535, _temp1535 + 8u},( struct
_tagged_string){ _temp1538, _temp1538, _temp1538 + 5u},( struct _tagged_string){
_temp1541, _temp1541, _temp1541 + 8u},( struct _tagged_string){ _temp1544,
_temp1544, _temp1544 + 4u},( struct _tagged_string){ _temp1547, _temp1547,
_temp1547 + 7u},( struct _tagged_string){ _temp1550, _temp1550, _temp1550 + 7u},(
struct _tagged_string){ _temp1553, _temp1553, _temp1553 + 8u},( struct
_tagged_string){ _temp1556, _temp1556, _temp1556 + 8u},( struct _tagged_string){
_temp1559, _temp1559, _temp1559 + 6u},( struct _tagged_string){ _temp1562,
_temp1562, _temp1562 + 7u},( struct _tagged_string){ _temp1565, _temp1565,
_temp1565 + 7u},( struct _tagged_string){ _temp1568, _temp1568, _temp1568 + 7u},(
struct _tagged_string){ _temp1571, _temp1571, _temp1571 + 7u},( struct
_tagged_string){ _temp1574, _temp1574, _temp1574 + 7u},( struct _tagged_string){
_temp1577, _temp1577, _temp1577 + 7u},( struct _tagged_string){ _temp1580,
_temp1580, _temp1580 + 8u},( struct _tagged_string){ _temp1583, _temp1583,
_temp1583 + 9u},( struct _tagged_string){ _temp1586, _temp1586, _temp1586 + 6u},(
struct _tagged_string){ _temp1589, _temp1589, _temp1589 + 6u},( struct
_tagged_string){ _temp1592, _temp1592, _temp1592 + 6u},( struct _tagged_string){
_temp1595, _temp1595, _temp1595 + 6u},( struct _tagged_string){ _temp1598,
_temp1598, _temp1598 + 7u},( struct _tagged_string){ _temp1601, _temp1601,
_temp1601 + 6u},( struct _tagged_string){ _temp1604, _temp1604, _temp1604 + 11u},(
struct _tagged_string){ _temp1607, _temp1607, _temp1607 + 11u},( struct
_tagged_string){ _temp1610, _temp1610, _temp1610 + 11u},( struct _tagged_string){
_temp1613, _temp1613, _temp1613 + 11u},( struct _tagged_string){ _temp1616,
_temp1616, _temp1616 + 11u},( struct _tagged_string){ _temp1619, _temp1619,
_temp1619 + 12u},( struct _tagged_string){ _temp1622, _temp1622, _temp1622 + 13u},(
struct _tagged_string){ _temp1625, _temp1625, _temp1625 + 11u},( struct
_tagged_string){ _temp1628, _temp1628, _temp1628 + 11u},( struct _tagged_string){
_temp1631, _temp1631, _temp1631 + 10u},( struct _tagged_string){ _temp1634,
_temp1634, _temp1634 + 9u},( struct _tagged_string){ _temp1637, _temp1637,
_temp1637 + 11u},( struct _tagged_string){ _temp1640, _temp1640, _temp1640 + 12u},(
struct _tagged_string){ _temp1643, _temp1643, _temp1643 + 11u},( struct
_tagged_string){ _temp1646, _temp1646, _temp1646 + 17u},( struct _tagged_string){
_temp1649, _temp1649, _temp1649 + 7u},( struct _tagged_string){ _temp1652,
_temp1652, _temp1652 + 19u},( struct _tagged_string){ _temp1655, _temp1655,
_temp1655 + 18u},( struct _tagged_string){ _temp1658, _temp1658, _temp1658 + 9u},(
struct _tagged_string){ _temp1661, _temp1661, _temp1661 + 16u},( struct
_tagged_string){ _temp1664, _temp1664, _temp1664 + 18u},( struct _tagged_string){
_temp1667, _temp1667, _temp1667 + 10u},( struct _tagged_string){ _temp1670,
_temp1670, _temp1670 + 4u},( struct _tagged_string){ _temp1673, _temp1673,
_temp1673 + 4u},( struct _tagged_string){ _temp1676, _temp1676, _temp1676 + 4u},(
struct _tagged_string){ _temp1679, _temp1679, _temp1679 + 4u},( struct
_tagged_string){ _temp1682, _temp1682, _temp1682 + 4u},( struct _tagged_string){
_temp1685, _temp1685, _temp1685 + 4u},( struct _tagged_string){ _temp1688,
_temp1688, _temp1688 + 4u},( struct _tagged_string){ _temp1691, _temp1691,
_temp1691 + 4u},( struct _tagged_string){ _temp1694, _temp1694, _temp1694 + 4u},(
struct _tagged_string){ _temp1697, _temp1697, _temp1697 + 4u},( struct
_tagged_string){ _temp1700, _temp1700, _temp1700 + 4u},( struct _tagged_string){
_temp1703, _temp1703, _temp1703 + 4u},( struct _tagged_string){ _temp1706,
_temp1706, _temp1706 + 4u},( struct _tagged_string){ _temp1709, _temp1709,
_temp1709 + 4u},( struct _tagged_string){ _temp1712, _temp1712, _temp1712 + 4u},(
struct _tagged_string){ _temp1715, _temp1715, _temp1715 + 4u},( struct
_tagged_string){ _temp1718, _temp1718, _temp1718 + 4u},( struct _tagged_string){
_temp1721, _temp1721, _temp1721 + 4u},( struct _tagged_string){ _temp1724,
_temp1724, _temp1724 + 4u},( struct _tagged_string){ _temp1727, _temp1727,
_temp1727 + 4u},( struct _tagged_string){ _temp1730, _temp1730, _temp1730 + 4u},(
struct _tagged_string){ _temp1733, _temp1733, _temp1733 + 4u},( struct
_tagged_string){ _temp1736, _temp1736, _temp1736 + 4u},( struct _tagged_string){
_temp1739, _temp1739, _temp1739 + 4u},( struct _tagged_string){ _temp1742,
_temp1742, _temp1742 + 4u},( struct _tagged_string){ _temp1745, _temp1745,
_temp1745 + 4u},( struct _tagged_string){ _temp1748, _temp1748, _temp1748 + 4u},(
struct _tagged_string){ _temp1751, _temp1751, _temp1751 + 5u},( struct
_tagged_string){ _temp1754, _temp1754, _temp1754 + 17u},( struct _tagged_string){
_temp1757, _temp1757, _temp1757 + 21u},( struct _tagged_string){ _temp1760,
_temp1760, _temp1760 + 21u},( struct _tagged_string){ _temp1763, _temp1763,
_temp1763 + 20u},( struct _tagged_string){ _temp1766, _temp1766, _temp1766 + 21u},(
struct _tagged_string){ _temp1769, _temp1769, _temp1769 + 13u},( struct
_tagged_string){ _temp1772, _temp1772, _temp1772 + 15u},( struct _tagged_string){
_temp1775, _temp1775, _temp1775 + 17u},( struct _tagged_string){ _temp1778,
_temp1778, _temp1778 + 19u},( struct _tagged_string){ _temp1781, _temp1781,
_temp1781 + 12u},( struct _tagged_string){ _temp1784, _temp1784, _temp1784 + 17u},(
struct _tagged_string){ _temp1787, _temp1787, _temp1787 + 23u},( struct
_tagged_string){ _temp1790, _temp1790, _temp1790 + 24u},( struct _tagged_string){
_temp1793, _temp1793, _temp1793 + 15u},( struct _tagged_string){ _temp1796,
_temp1796, _temp1796 + 11u},( struct _tagged_string){ _temp1799, _temp1799,
_temp1799 + 15u},( struct _tagged_string){ _temp1802, _temp1802, _temp1802 + 10u},(
struct _tagged_string){ _temp1805, _temp1805, _temp1805 + 15u},( struct
_tagged_string){ _temp1808, _temp1808, _temp1808 + 5u},( struct _tagged_string){
_temp1811, _temp1811, _temp1811 + 15u},( struct _tagged_string){ _temp1814,
_temp1814, _temp1814 + 15u},( struct _tagged_string){ _temp1817, _temp1817,
_temp1817 + 11u},( struct _tagged_string){ _temp1820, _temp1820, _temp1820 + 22u},(
struct _tagged_string){ _temp1823, _temp1823, _temp1823 + 26u},( struct
_tagged_string){ _temp1826, _temp1826, _temp1826 + 16u},( struct _tagged_string){
_temp1829, _temp1829, _temp1829 + 16u},( struct _tagged_string){ _temp1832,
_temp1832, _temp1832 + 24u},( struct _tagged_string){ _temp1835, _temp1835,
_temp1835 + 25u},( struct _tagged_string){ _temp1838, _temp1838, _temp1838 + 21u},(
struct _tagged_string){ _temp1841, _temp1841, _temp1841 + 22u},( struct
_tagged_string){ _temp1844, _temp1844, _temp1844 + 16u},( struct _tagged_string){
_temp1847, _temp1847, _temp1847 + 19u},( struct _tagged_string){ _temp1850,
_temp1850, _temp1850 + 25u},( struct _tagged_string){ _temp1853, _temp1853,
_temp1853 + 23u},( struct _tagged_string){ _temp1856, _temp1856, _temp1856 + 24u},(
struct _tagged_string){ _temp1859, _temp1859, _temp1859 + 18u},( struct
_tagged_string){ _temp1862, _temp1862, _temp1862 + 17u},( struct _tagged_string){
_temp1865, _temp1865, _temp1865 + 17u},( struct _tagged_string){ _temp1868,
_temp1868, _temp1868 + 12u},( struct _tagged_string){ _temp1871, _temp1871,
_temp1871 + 11u},( struct _tagged_string){ _temp1874, _temp1874, _temp1874 + 18u},(
struct _tagged_string){ _temp1877, _temp1877, _temp1877 + 8u},( struct
_tagged_string){ _temp1880, _temp1880, _temp1880 + 13u},( struct _tagged_string){
_temp1883, _temp1883, _temp1883 + 8u},( struct _tagged_string){ _temp1886,
_temp1886, _temp1886 + 4u},( struct _tagged_string){ _temp1889, _temp1889,
_temp1889 + 20u},( struct _tagged_string){ _temp1892, _temp1892, _temp1892 + 20u},(
struct _tagged_string){ _temp1895, _temp1895, _temp1895 + 11u},( struct
_tagged_string){ _temp1898, _temp1898, _temp1898 + 14u},( struct _tagged_string){
_temp1901, _temp1901, _temp1901 + 11u},( struct _tagged_string){ _temp1904,
_temp1904, _temp1904 + 15u},( struct _tagged_string){ _temp1907, _temp1907,
_temp1907 + 22u},( struct _tagged_string){ _temp1910, _temp1910, _temp1910 + 16u},(
struct _tagged_string){ _temp1913, _temp1913, _temp1913 + 17u},( struct
_tagged_string){ _temp1916, _temp1916, _temp1916 + 12u},( struct _tagged_string){
_temp1919, _temp1919, _temp1919 + 18u},( struct _tagged_string){ _temp1922,
_temp1922, _temp1922 + 17u},( struct _tagged_string){ _temp1925, _temp1925,
_temp1925 + 12u},( struct _tagged_string){ _temp1928, _temp1928, _temp1928 + 16u},(
struct _tagged_string){ _temp1931, _temp1931, _temp1931 + 11u},( struct
_tagged_string){ _temp1934, _temp1934, _temp1934 + 10u},( struct _tagged_string){
_temp1937, _temp1937, _temp1937 + 14u},( struct _tagged_string){ _temp1940,
_temp1940, _temp1940 + 15u},( struct _tagged_string){ _temp1943, _temp1943,
_temp1943 + 20u},( struct _tagged_string){ _temp1946, _temp1946, _temp1946 + 27u},(
struct _tagged_string){ _temp1949, _temp1949, _temp1949 + 10u},( struct
_tagged_string){ _temp1952, _temp1952, _temp1952 + 18u},( struct _tagged_string){
_temp1955, _temp1955, _temp1955 + 21u},( struct _tagged_string){ _temp1958,
_temp1958, _temp1958 + 19u},( struct _tagged_string){ _temp1961, _temp1961,
_temp1961 + 16u},( struct _tagged_string){ _temp1964, _temp1964, _temp1964 + 11u},(
struct _tagged_string){ _temp1967, _temp1967, _temp1967 + 20u},( struct
_tagged_string){ _temp1970, _temp1970, _temp1970 + 15u},( struct _tagged_string){
_temp1973, _temp1973, _temp1973 + 20u},( struct _tagged_string){ _temp1976,
_temp1976, _temp1976 + 15u},( struct _tagged_string){ _temp1979, _temp1979,
_temp1979 + 8u},( struct _tagged_string){ _temp1982, _temp1982, _temp1982 + 19u},(
struct _tagged_string){ _temp1985, _temp1985, _temp1985 + 20u},( struct
_tagged_string){ _temp1988, _temp1988, _temp1988 + 14u},( struct _tagged_string){
_temp1991, _temp1991, _temp1991 + 19u},( struct _tagged_string){ _temp1994,
_temp1994, _temp1994 + 20u},( struct _tagged_string){ _temp1997, _temp1997,
_temp1997 + 11u},( struct _tagged_string){ _temp2000, _temp2000, _temp2000 + 22u},(
struct _tagged_string){ _temp2003, _temp2003, _temp2003 + 20u},( struct
_tagged_string){ _temp2006, _temp2006, _temp2006 + 23u},( struct _tagged_string){
_temp2009, _temp2009, _temp2009 + 20u},( struct _tagged_string){ _temp2012,
_temp2012, _temp2012 + 22u},( struct _tagged_string){ _temp2015, _temp2015,
_temp2015 + 23u},( struct _tagged_string){ _temp2018, _temp2018, _temp2018 + 24u},(
struct _tagged_string){ _temp2021, _temp2021, _temp2021 + 24u},( struct
_tagged_string){ _temp2024, _temp2024, _temp2024 + 15u},( struct _tagged_string){
_temp2027, _temp2027, _temp2027 + 20u},( struct _tagged_string){ _temp2030,
_temp2030, _temp2030 + 22u},( struct _tagged_string){ _temp2033, _temp2033,
_temp2033 + 17u},( struct _tagged_string){ _temp2036, _temp2036, _temp2036 + 20u},(
struct _tagged_string){ _temp2039, _temp2039, _temp2039 + 26u},( struct
_tagged_string){ _temp2042, _temp2042, _temp2042 + 16u},( struct _tagged_string){
_temp2045, _temp2045, _temp2045 + 17u},( struct _tagged_string){ _temp2048,
_temp2048, _temp2048 + 14u},( struct _tagged_string){ _temp2051, _temp2051,
_temp2051 + 15u},( struct _tagged_string){ _temp2054, _temp2054, _temp2054 + 19u},(
struct _tagged_string){ _temp2057, _temp2057, _temp2057 + 19u},( struct
_tagged_string){ _temp2060, _temp2060, _temp2060 + 25u},( struct _tagged_string){
_temp2063, _temp2063, _temp2063 + 26u},( struct _tagged_string){ _temp2066,
_temp2066, _temp2066 + 9u},( struct _tagged_string){ _temp2069, _temp2069,
_temp2069 + 20u}}; static short Cyc_yyr1[ 389u]={ (short)0, (short)120, (short)121,
(short)121, (short)121, (short)121, (short)121, (short)121, (short)121, (short)122,
(short)122, (short)123, (short)123, (short)124, (short)124, (short)124, (short)124,
(short)125, (short)125, (short)126, (short)127, (short)128, (short)129, (short)130,
(short)130, (short)130, (short)131, (short)131, (short)132, (short)132, (short)132,
(short)132, (short)132, (short)132, (short)132, (short)132, (short)133, (short)133,
(short)133, (short)133, (short)133, (short)133, (short)133, (short)134, (short)134,
(short)135, (short)136, (short)136, (short)137, (short)137, (short)137, (short)137,
(short)138, (short)138, (short)138, (short)138, (short)138, (short)138, (short)138,
(short)138, (short)138, (short)138, (short)138, (short)138, (short)138, (short)138,
(short)138, (short)138, (short)138, (short)139, (short)139, (short)140, (short)140,
(short)140, (short)141, (short)141, (short)142, (short)142, (short)143, (short)143,
(short)144, (short)144, (short)144, (short)144, (short)144, (short)145, (short)145,
(short)145, (short)146, (short)146, (short)147, (short)148, (short)148, (short)149,
(short)150, (short)150, (short)151, (short)151, (short)152, (short)153, (short)153,
(short)153, (short)153, (short)154, (short)155, (short)155, (short)156, (short)156,
(short)156, (short)157, (short)157, (short)157, (short)157, (short)157, (short)157,
(short)157, (short)157, (short)158, (short)158, (short)158, (short)158, (short)159,
(short)159, (short)160, (short)160, (short)161, (short)161, (short)161, (short)161,
(short)161, (short)161, (short)161, (short)161, (short)161, (short)161, (short)161,
(short)162, (short)162, (short)162, (short)162, (short)163, (short)163, (short)163,
(short)163, (short)163, (short)164, (short)164, (short)165, (short)165, (short)165,
(short)166, (short)166, (short)167, (short)167, (short)167, (short)168, (short)168,
(short)169, (short)169, (short)169, (short)169, (short)170, (short)170, (short)170,
(short)170, (short)171, (short)171, (short)172, (short)172, (short)172, (short)173,
(short)174, (short)174, (short)175, (short)175, (short)176, (short)176, (short)176,
(short)176, (short)177, (short)177, (short)177, (short)177, (short)178, (short)179,
(short)179, (short)180, (short)180, (short)181, (short)181, (short)182, (short)182,
(short)182, (short)182, (short)183, (short)183, (short)184, (short)184, (short)184,
(short)185, (short)185, (short)185, (short)185, (short)185, (short)185, (short)185,
(short)185, (short)185, (short)185, (short)185, (short)185, (short)185, (short)185,
(short)186, (short)186, (short)186, (short)186, (short)186, (short)186, (short)186,
(short)186, (short)187, (short)188, (short)188, (short)189, (short)189, (short)190,
(short)190, (short)191, (short)191, (short)191, (short)192, (short)192, (short)192,
(short)192, (short)193, (short)193, (short)193, (short)193, (short)193, (short)193,
(short)194, (short)194, (short)194, (short)194, (short)194, (short)194, (short)194,
(short)194, (short)194, (short)194, (short)194, (short)194, (short)194, (short)194,
(short)195, (short)195, (short)195, (short)195, (short)195, (short)195, (short)195,
(short)195, (short)196, (short)196, (short)196, (short)196, (short)196, (short)196,
(short)196, (short)196, (short)196, (short)196, (short)196, (short)196, (short)196,
(short)196, (short)197, (short)197, (short)198, (short)198, (short)199, (short)199,
(short)200, (short)201, (short)201, (short)202, (short)202, (short)203, (short)203,
(short)204, (short)204, (short)204, (short)204, (short)204, (short)204, (short)204,
(short)204, (short)204, (short)204, (short)204, (short)205, (short)205, (short)205,
(short)205, (short)205, (short)206, (short)207, (short)207, (short)208, (short)208,
(short)209, (short)209, (short)210, (short)210, (short)211, (short)211, (short)212,
(short)212, (short)212, (short)213, (short)213, (short)213, (short)213, (short)213,
(short)214, (short)214, (short)214, (short)215, (short)215, (short)215, (short)216,
(short)216, (short)216, (short)216, (short)217, (short)217, (short)218, (short)218,
(short)218, (short)218, (short)218, (short)218, (short)218, (short)218, (short)218,
(short)218, (short)218, (short)219, (short)219, (short)219, (short)219, (short)219,
(short)219, (short)220, (short)220, (short)220, (short)221, (short)221, (short)221,
(short)221, (short)221, (short)221, (short)221, (short)221, (short)221, (short)221,
(short)221, (short)221, (short)221, (short)221, (short)222, (short)222, (short)222,
(short)222, (short)222, (short)222, (short)222, (short)222, (short)222, (short)223,
(short)224, (short)224, (short)225, (short)225, (short)225, (short)225, (short)226,
(short)226}; static short Cyc_yyr2[ 389u]={ (short)0, (short)1, (short)1,
(short)2, (short)3, (short)5, (short)3, (short)5, (short)6, (short)0, (short)1,
(short)1, (short)1, (short)2, (short)3, (short)3, (short)4, (short)3, (short)4,
(short)2, (short)1, (short)2, (short)1, (short)2, (short)3, (short)5, (short)1,
(short)2, (short)2, (short)3, (short)2, (short)3, (short)2, (short)3, (short)2,
(short)3, (short)1, (short)1, (short)1, (short)1, (short)2, (short)1, (short)1,
(short)0, (short)1, (short)6, (short)1, (short)3, (short)1, (short)1, (short)4,
(short)4, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)3,
(short)2, (short)4, (short)1, (short)1, (short)1, (short)1, (short)1, (short)5,
(short)2, (short)1, (short)3, (short)1, (short)3, (short)4, (short)6, (short)6,
(short)3, (short)3, (short)0, (short)3, (short)3, (short)1, (short)1, (short)1,
(short)1, (short)2, (short)1, (short)1, (short)3, (short)1, (short)3, (short)3,
(short)2, (short)3, (short)2, (short)3, (short)1, (short)1, (short)3, (short)1,
(short)2, (short)3, (short)6, (short)3, (short)4, (short)5, (short)5, (short)2,
(short)3, (short)4, (short)1, (short)2, (short)3, (short)3, (short)1, (short)5,
(short)1, (short)2, (short)1, (short)3, (short)3, (short)4, (short)4, (short)3,
(short)5, (short)4, (short)4, (short)4, (short)2, (short)2, (short)3, (short)3,
(short)4, (short)1, (short)1, (short)4, (short)4, (short)1, (short)0, (short)1,
(short)1, (short)3, (short)1, (short)1, (short)2, (short)1, (short)3, (short)3,
(short)1, (short)3, (short)2, (short)3, (short)1, (short)3, (short)1, (short)3,
(short)3, (short)5, (short)1, (short)3, (short)2, (short)1, (short)2, (short)1,
(short)1, (short)3, (short)1, (short)1, (short)2, (short)3, (short)4, (short)8,
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
(short)1, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)5, (short)2, (short)2, (short)2, (short)1,
(short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3,
(short)1, (short)3, (short)1, (short)3, (short)3, (short)1, (short)3, (short)3,
(short)3, (short)3, (short)1, (short)3, (short)3, (short)1, (short)3, (short)3,
(short)1, (short)3, (short)3, (short)3, (short)1, (short)4, (short)1, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)4, (short)2, (short)4,
(short)7, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)4, (short)3, (short)4, (short)3, (short)3,
(short)3, (short)3, (short)2, (short)2, (short)6, (short)7, (short)4, (short)4,
(short)1, (short)1, (short)1, (short)3, (short)2, (short)5, (short)4, (short)4,
(short)5, (short)1, (short)1, (short)3, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1}; static short Cyc_yydefact[ 728u]={ (short)0, (short)36,
(short)37, (short)38, (short)39, (short)41, (short)52, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, (short)60, (short)61, (short)71,
(short)72, (short)73, (short)88, (short)89, (short)43, (short)0, (short)0,
(short)42, (short)0, (short)0, (short)0, (short)0, (short)387, (short)65,
(short)388, (short)85, (short)0, (short)53, (short)0, (short)140, (short)141,
(short)144, (short)1, (short)2, (short)11, (short)0, (short)0, (short)12,
(short)0, (short)43, (short)43, (short)43, (short)62, (short)63, (short)0,
(short)64, (short)0, (short)123, (short)0, (short)145, (short)125, (short)40,
(short)0, (short)34, (short)44, (short)75, (short)269, (short)265, (short)268,
(short)267, (short)0, (short)263, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)270, (short)19, (short)21, (short)147, (short)149, (short)0,
(short)85, (short)0, (short)114, (short)0, (short)0, (short)67, (short)0,
(short)0, (short)0, (short)0, (short)3, (short)0, (short)0, (short)0, (short)0,
(short)23, (short)0, (short)93, (short)94, (short)96, (short)28, (short)30,
(short)32, (short)85, (short)0, (short)85, (short)39, (short)0, (short)26,
(short)0, (short)0, (short)13, (short)0, (short)0, (short)0, (short)135, (short)124,
(short)136, (short)146, (short)0, (short)0, (short)35, (short)0, (short)0,
(short)277, (short)276, (short)266, (short)275, (short)0, (short)0, (short)0,
(short)85, (short)0, (short)110, (short)115, (short)0, (short)0, (short)69,
(short)70, (short)66, (short)0, (short)43, (short)43, (short)188, (short)190,
(short)194, (short)0, (short)126, (short)168, (short)0, (short)165, (short)0,
(short)386, (short)0, (short)0, (short)0, (short)0, (short)348, (short)349,
(short)350, (short)351, (short)352, (short)353, (short)0, (short)0, (short)0,
(short)383, (short)373, (short)384, (short)385, (short)0, (short)0, (short)0,
(short)0, (short)356, (short)0, (short)354, (short)355, (short)0, (short)288,
(short)301, (short)307, (short)309, (short)311, (short)313, (short)315, (short)317,
(short)320, (short)325, (short)328, (short)331, (short)335, (short)0, (short)0,
(short)337, (short)357, (short)372, (short)371, (short)0, (short)4, (short)0,
(short)6, (short)0, (short)24, (short)0, (short)0, (short)0, (short)14, (short)29,
(short)31, (short)33, (short)84, (short)0, (short)90, (short)91, (short)0,
(short)83, (short)40, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)387,
(short)222, (short)224, (short)0, (short)230, (short)228, (short)0, (short)229,
(short)213, (short)214, (short)215, (short)0, (short)226, (short)216, (short)217,
(short)218, (short)0, (short)286, (short)27, (short)15, (short)96, (short)171,
(short)0, (short)130, (short)0, (short)152, (short)0, (short)170, (short)0,
(short)127, (short)0, (short)150, (short)138, (short)137, (short)0, (short)0,
(short)78, (short)0, (short)76, (short)264, (short)279, (short)0, (short)278,
(short)0, (short)0, (short)277, (short)148, (short)111, (short)85, (short)0,
(short)0, (short)117, (short)121, (short)116, (short)161, (short)191, (short)0,
(short)99, (short)101, (short)0, (short)0, (short)196, (short)189, (short)197,
(short)0, (short)87, (short)0, (short)86, (short)0, (short)167, (short)196,
(short)169, (short)68, (short)0, (short)0, (short)345, (short)303, (short)335,
(short)0, (short)304, (short)305, (short)0, (short)0, (short)0, (short)0,
(short)338, (short)339, (short)0, (short)0, (short)0, (short)0, (short)341,
(short)342, (short)340, (short)142, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)291,
(short)292, (short)293, (short)294, (short)295, (short)296, (short)297, (short)298,
(short)299, (short)300, (short)290, (short)0, (short)0, (short)343, (short)0,
(short)365, (short)366, (short)0, (short)0, (short)0, (short)375, (short)0,
(short)0, (short)143, (short)20, (short)9, (short)22, (short)9, (short)95,
(short)97, (short)174, (short)173, (short)16, (short)0, (short)80, (short)92,
(short)0, (short)0, (short)103, (short)104, (short)106, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)256, (short)257,
(short)258, (short)0, (short)0, (short)260, (short)0, (short)219, (short)220,
(short)0, (short)0, (short)96, (short)225, (short)227, (short)223, (short)0,
(short)159, (short)0, (short)0, (short)155, (short)129, (short)0, (short)0,
(short)132, (short)0, (short)134, (short)133, (short)128, (short)151, (short)139,
(short)9, (short)49, (short)48, (short)0, (short)46, (short)0, (short)74,
(short)0, (short)272, (short)0, (short)25, (short)273, (short)0, (short)0,
(short)0, (short)0, (short)184, (short)281, (short)284, (short)0, (short)283,
(short)0, (short)112, (short)0, (short)113, (short)118, (short)0, (short)0,
(short)0, (short)0, (short)192, (short)100, (short)102, (short)0, (short)204,
(short)0, (short)0, (short)200, (short)0, (short)198, (short)0, (short)0,
(short)0, (short)212, (short)193, (short)195, (short)166, (short)0, (short)0,
(short)175, (short)179, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)374, (short)381, (short)0,
(short)380, (short)308, (short)0, (short)310, (short)312, (short)314, (short)316,
(short)318, (short)319, (short)323, (short)324, (short)321, (short)322, (short)326,
(short)327, (short)329, (short)330, (short)332, (short)333, (short)334, (short)289,
(short)0, (short)363, (short)364, (short)359, (short)0, (short)361, (short)362,
(short)0, (short)0, (short)0, (short)10, (short)5, (short)7, (short)0, (short)306,
(short)107, (short)98, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)255, (short)259, (short)0,
(short)0, (short)0, (short)221, (short)0, (short)17, (short)287, (short)0,
(short)157, (short)0, (short)131, (short)0, (short)154, (short)153, (short)172,
(short)8, (short)0, (short)0, (short)0, (short)79, (short)77, (short)280,
(short)187, (short)0, (short)282, (short)183, (short)185, (short)274, (short)0,
(short)271, (short)109, (short)120, (short)119, (short)0, (short)163, (short)162,
(short)0, (short)206, (short)199, (short)202, (short)0, (short)207, (short)0,
(short)0, (short)201, (short)0, (short)344, (short)0, (short)176, (short)0,
(short)180, (short)369, (short)370, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)336, (short)377, (short)0, (short)0, (short)346, (short)360,
(short)358, (short)378, (short)0, (short)0, (short)82, (short)105, (short)108,
(short)81, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)235, (short)261, (short)0, (short)18, (short)160,
(short)158, (short)156, (short)0, (short)0, (short)45, (short)47, (short)186,
(short)285, (short)0, (short)0, (short)205, (short)0, (short)209, (short)211,
(short)210, (short)203, (short)0, (short)177, (short)181, (short)0, (short)0,
(short)379, (short)0, (short)382, (short)302, (short)376, (short)231, (short)235,
(short)241, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)262, (short)50, (short)51,
(short)122, (short)164, (short)208, (short)0, (short)182, (short)0, (short)367,
(short)0, (short)0, (short)0, (short)0, (short)243, (short)0, (short)0, (short)0,
(short)251, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)234, (short)0, (short)347, (short)368, (short)232, (short)233,
(short)242, (short)244, (short)245, (short)0, (short)253, (short)252, (short)0,
(short)247, (short)0, (short)0, (short)0, (short)0, (short)235, (short)236,
(short)0, (short)246, (short)254, (short)248, (short)249, (short)0, (short)0,
(short)235, (short)237, (short)178, (short)250, (short)235, (short)238, (short)235,
(short)239, (short)240, (short)0, (short)0, (short)0}; static short Cyc_yydefgoto[
107u]={ (short)725, (short)516, (short)517, (short)39, (short)40, (short)234,
(short)41, (short)369, (short)42, (short)371, (short)43, (short)205, (short)44,
(short)45, (short)59, (short)60, (short)425, (short)426, (short)46, (short)138,
(short)47, (short)48, (short)266, (short)267, (short)49, (short)84, (short)50,
(short)211, (short)212, (short)95, (short)96, (short)97, (short)213, (short)147,
(short)381, (short)382, (short)383, (short)51, (short)280, (short)281, (short)52,
(short)53, (short)54, (short)55, (short)116, (short)78, (short)263, (short)457,
(short)410, (short)411, (short)286, (short)255, (short)149, (short)256, (short)257,
(short)472, (short)374, (short)473, (short)474, (short)437, (short)438, (short)143,
(short)144, (short)145, (short)458, (short)293, (short)237, (short)238, (short)239,
(short)240, (short)241, (short)242, (short)243, (short)661, (short)244, (short)245,
(short)270, (short)271, (short)272, (short)440, (short)441, (short)442, (short)246,
(short)247, (short)355, (short)178, (short)521, (short)179, (short)180, (short)181,
(short)182, (short)183, (short)184, (short)185, (short)186, (short)187, (short)188,
(short)189, (short)190, (short)191, (short)192, (short)193, (short)194, (short)484,
(short)485, (short)195, (short)196}; static short Cyc_yypact[ 728u]={ (short)754,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)29, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)10,
(short)27, (short)2422, - (short)-32768, (short)27, (short)47, (short)247,
(short)247, - (short)-32768, (short)82, - (short)-32768, (short)97, (short)113,
- (short)-32768, (short)130, (short)135, (short)154, - (short)-32768, - (short)-32768,
(short)754, - (short)-32768, (short)188, (short)280, - (short)-32768, (short)251,
- (short)10, - (short)10, - (short)10, - (short)-32768, - (short)-32768, (short)168,
- (short)-32768, (short)1820, (short)198, (short)212, - (short)47, - (short)-32768,
(short)177, (short)233, (short)1869, - (short)-32768, (short)238, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)2422, - (short)-32768,
(short)260, (short)283, (short)296, (short)2422, (short)325, (short)98, -
(short)-32768, - (short)-32768, (short)324, - (short)-32768, (short)27, (short)333,
(short)27, - (short)28, (short)60, (short)1977, - (short)-32768, (short)327,
(short)2080, (short)3513, (short)3513, - (short)-32768, (short)754, (short)754,
(short)754, (short)754, - (short)-32768, (short)334, (short)318, - (short)-32768,
(short)1770, (short)1869, (short)1869, (short)1869, (short)97, (short)2080,
(short)97, (short)350, (short)1028, - (short)-32768, (short)1820, (short)251, -
(short)-32768, (short)799, (short)1977, (short)2441, - (short)-32768, (short)198,
(short)67, - (short)-32768, (short)754, (short)348, - (short)-32768, (short)27,
(short)358, (short)2422, - (short)-32768, - (short)-32768, - (short)-32768,
(short)3513, (short)366, (short)60, (short)97, (short)27, (short)353, - (short)-32768,
(short)27, (short)27, - (short)-32768, - (short)-32768, - (short)-32768, (short)119,
- (short)10, - (short)10, (short)269, - (short)-32768, (short)360, (short)69, -
(short)-32768, (short)279, (short)81, - (short)-32768, (short)3714, - (short)-32768,
(short)3513, (short)3580, (short)367, (short)372, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)378,
(short)3781, (short)3781, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)1374, (short)380, (short)3848, (short)3848, - (short)-32768,
(short)3848, - (short)-32768, - (short)-32768, (short)384, - (short)-32768, -
(short)32, (short)436, (short)382, (short)406, (short)368, (short)387, (short)153,
(short)398, (short)356, (short)26, - (short)-32768, (short)614, (short)437,
(short)3848, (short)77, - (short)-32768, - (short)-32768, (short)111, (short)440,
- (short)-32768, (short)442, - (short)-32768, (short)446, - (short)-32768,
(short)113, (short)2508, (short)1820, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)448, (short)449, (short)2080, - (short)-32768,
(short)349, (short)453, - (short)-32768, (short)452, (short)454, (short)455,
(short)1922, (short)457, (short)459, (short)462, (short)466, (short)2575,
(short)1922, (short)196, (short)1922, (short)1922, (short)456, - (short)-32768,
- (short)-32768, (short)464, - (short)-32768, - (short)-32768, (short)251, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)472,
(short)1145, - (short)-32768, - (short)-32768, - (short)-32768, (short)133, -
(short)-32768, - (short)-32768, - (short)-32768, (short)473, - (short)-32768,
(short)7, - (short)-32768, (short)470, (short)145, (short)475, (short)471,
(short)85, - (short)-32768, (short)467, - (short)-32768, - (short)-32768,
(short)67, (short)476, (short)22, (short)479, (short)480, (short)483, - (short)-32768,
- (short)-32768, (short)482, (short)484, (short)164, (short)561, (short)2422, -
(short)-32768, - (short)-32768, (short)97, (short)27, (short)486, (short)217,
(short)18, - (short)-32768, (short)14, - (short)-32768, (short)487, (short)2080,
(short)2080, (short)1662, (short)2642, (short)3, - (short)-32768, (short)298,
(short)7, - (short)-32768, (short)1977, - (short)-32768, (short)1648, - (short)-32768,
(short)144, - (short)-32768, - (short)-32768, (short)2080, (short)1374, -
(short)-32768, - (short)-32768, - (short)-32768, (short)2101, - (short)-32768,
(short)516, (short)3513, (short)1603, (short)554, (short)1374, - (short)-32768,
- (short)-32768, (short)1145, (short)492, (short)375, (short)3513, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)3848, (short)3513,
(short)3848, (short)3848, (short)3848, (short)3848, (short)3848, (short)3848,
(short)3848, (short)3848, (short)3848, (short)3848, (short)3848, (short)3848,
(short)3848, (short)3848, (short)3848, (short)3848, (short)3848, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)3513, (short)3513, - (short)-32768, (short)140, - (short)-32768, -
(short)-32768, (short)2709, (short)195, (short)3513, - (short)-32768, (short)2356,
(short)491, - (short)-32768, - (short)-32768, (short)754, - (short)-32768,
(short)754, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)2080, - (short)-32768, - (short)-32768, (short)3513,
(short)502, (short)497, - (short)-32768, (short)493, (short)2080, (short)3513,
(short)3513, (short)3513, (short)571, (short)1262, (short)507, - (short)-32768,
- (short)-32768, - (short)-32768, (short)218, (short)560, - (short)-32768,
(short)2776, - (short)-32768, - (short)-32768, (short)1922, (short)1486, (short)1770,
- (short)-32768, - (short)-32768, - (short)-32768, (short)3513, (short)519,
(short)239, (short)505, (short)498, - (short)-32768, (short)7, (short)2034, -
(short)-32768, (short)520, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)754, - (short)-32768, (short)514, (short)515,
(short)513, (short)27, - (short)-32768, (short)3513, - (short)-32768, (short)2422,
- (short)-32768, - (short)-32768, (short)530, (short)3513, (short)2422, -
(short)31, - (short)-32768, - (short)-32768, - (short)-32768, (short)521,
(short)525, (short)522, - (short)-32768, (short)523, - (short)-32768, (short)27,
(short)27, (short)529, (short)60, (short)538, - (short)-32768, - (short)-32768,
- (short)-32768, (short)7, - (short)-32768, (short)531, (short)532, - (short)-32768,
(short)512, (short)298, (short)1542, (short)1977, (short)2843, - (short)-32768,
- (short)-32768, (short)360, - (short)-32768, (short)533, (short)544, - (short)-32768,
- (short)-32768, (short)213, (short)2508, (short)391, (short)537, (short)113,
(short)540, (short)541, (short)543, (short)3647, - (short)-32768, - (short)-32768,
(short)545, (short)548, (short)436, (short)241, (short)382, (short)406, (short)368,
(short)387, (short)153, (short)153, (short)398, (short)398, (short)398, (short)398,
(short)356, (short)356, (short)26, (short)26, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)546, - (short)-32768, - (short)-32768,
- (short)-32768, (short)552, - (short)-32768, - (short)-32768, (short)62,
(short)220, (short)1977, - (short)-32768, - (short)-32768, - (short)-32768,
(short)547, - (short)-32768, - (short)-32768, - (short)-32768, (short)349,
(short)3513, (short)558, (short)394, (short)396, (short)400, (short)557, (short)2910,
(short)2977, (short)240, - (short)-32768, - (short)-32768, (short)563, (short)572,
(short)566, - (short)-32768, (short)1820, - (short)-32768, - (short)-32768,
(short)60, - (short)-32768, (short)575, - (short)-32768, (short)7, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)415, (short)573,
(short)22, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)568, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)3884, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)2080, (short)578, - (short)-32768, (short)580, - (short)-32768, - (short)-32768,
- (short)-32768, (short)7, - (short)-32768, (short)582, (short)86, - (short)-32768,
(short)574, (short)588, (short)581, - (short)-32768, (short)2186, - (short)-32768,
- (short)-32768, - (short)-32768, (short)1820, (short)2080, (short)588, (short)597,
(short)2356, - (short)-32768, - (short)-32768, (short)3513, (short)3513, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)2356,
(short)250, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)1922, (short)603, (short)1922, (short)3513, (short)3044, (short)253,
(short)3111, (short)257, (short)3178, (short)463, - (short)-32768, (short)606, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)602,
(short)607, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)404, (short)538, - (short)-32768, (short)610, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)3513, - (short)-32768, - (short)-32768,
(short)2508, (short)613, - (short)-32768, (short)302, - (short)-32768, - (short)-32768,
- (short)-32768, (short)675, (short)463, - (short)-32768, (short)409, (short)1922,
(short)412, (short)3245, (short)1922, (short)414, (short)3312, (short)3379,
(short)272, (short)2422, (short)608, (short)618, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)304,
- (short)-32768, (short)616, - (short)-32768, (short)2271, (short)1922, (short)620,
(short)623, - (short)-32768, (short)1922, (short)1922, (short)417, - (short)-32768,
(short)1922, (short)1922, (short)421, (short)1922, (short)427, (short)3446, -
(short)17, (short)1145, - (short)-32768, (short)3513, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)1922, - (short)-32768, - (short)-32768, (short)1922, - (short)-32768,
(short)1922, (short)1922, (short)429, (short)3513, (short)911, - (short)-32768,
(short)321, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)1922, (short)311, (short)463, - (short)-32768, - (short)-32768, - (short)-32768,
(short)911, - (short)-32768, (short)463, - (short)-32768, - (short)-32768,
(short)717, (short)718, - (short)-32768}; static short Cyc_yypgoto[ 107u]={ -
(short)-32768, (short)37, - (short)327, - (short)-32768, (short)407, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)48,
- (short)41, - (short)52, - (short)-32768, (short)23, - (short)27, (short)167, -
(short)-32768, (short)319, - (short)123, (short)95, - (short)-32768, - (short)-32768,
(short)295, - (short)-32768, (short)10, - (short)-32768, - (short)176, - (short)-32768,
- (short)-32768, - (short)-32768, (short)524, (short)518, - (short)71, - (short)-32768,
- (short)-32768, (short)202, - (short)-32768, - (short)261, - (short)-32768, -
(short)1, - (short)30, (short)5, - (short)-32768, - (short)-32768, (short)63, -
(short)-32768, - (short)98, - (short)384, (short)430, - (short)390, - (short)84,
- (short)269, - (short)-32768, - (short)-32768, - (short)190, (short)579, -
(short)345, - (short)258, - (short)-32768, (short)294, - (short)148, (short)438,
- (short)109, (short)271, - (short)184, (short)25, - (short)-32768, - (short)-32768,
- (short)35, - (short)237, - (short)-32768, - (short)-32768, - (short)549, -
(short)-32768, - (short)-32768, - (short)7, (short)460, - (short)-32768, (short)173,
- (short)-32768, - (short)-32768, - (short)65, - (short)78, - (short)-32768, -
(short)144, - (short)399, (short)584, (short)451, (short)450, (short)458,
(short)465, (short)461, (short)201, (short)107, (short)192, (short)199, -
(short)69, (short)413, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)202, - (short)-32768, - (short)-32768, (short)1}; static
short Cyc_yytable[ 3998u]={ (short)109, (short)56, (short)148, (short)258,
(short)107, (short)405, (short)276, (short)120, (short)306, (short)177, (short)197,
(short)108, (short)142, (short)254, (short)373, (short)72, (short)436, (short)110,
(short)445, (short)544, (short)514, (short)318, (short)61, (short)73, (short)115,
(short)74, (short)114, (short)79, (short)81, (short)547, (short)555, (short)85,
(short)214, (short)56, (short)468, (short)260, (short)558, (short)38, (short)325,
(short)423, (short)56, (short)142, (short)76, (short)98, (short)518, (short)56,
(short)109, (short)207, (short)208, (short)209, (short)107, (short)104, (short)706,
(short)77, (short)236, (short)56, (short)109, (short)57, (short)235, (short)122,
(short)248, (short)570, (short)273, (short)206, (short)126, (short)560, (short)134,
(short)73, (short)99, (short)100, (short)101, (short)571, (short)73, (short)249,
(short)434, (short)435, (short)89, (short)135, (short)326, (short)130, (short)480,
(short)133, (short)58, (short)128, (short)15, (short)16, (short)17, (short)707,
(short)114, (short)132, (short)80, (short)56, (short)56, (short)56, (short)56,
(short)550, (short)408, (short)450, (short)142, (short)674, (short)289, (short)409,
(short)321, (short)322, (short)319, (short)323, (short)424, (short)461, (short)250,
(short)290, (short)56, (short)28, (short)210, (short)451, (short)215, - (short)85,
(short)461, (short)30, (short)117, (short)56, (short)83, (short)262, (short)268,
(short)357, (short)73, (short)605, (short)375, (short)198, (short)199, (short)200,
(short)201, (short)75, (short)278, (short)295, (short)341, (short)282, (short)283,
(short)358, (short)359, (short)360, (short)277, (short)214, (short)342, (short)343,
(short)136, (short)468, (short)299, (short)291, (short)56, (short)417, (short)634,
(short)137, (short)300, (short)109, (short)506, (short)264, (short)469, (short)248,
(short)717, (short)510, (short)395, (short)407, (short)622, (short)287, (short)288,
(short)82, (short)479, (short)721, (short)296, (short)599, (short)376, (short)723,
(short)297, (short)724, (short)361, (short)35, (short)36, (short)37, (short)141,
(short)302, (short)303, (short)141, (short)362, (short)363, (short)296, (short)296,
(short)566, (short)567, (short)418, (short)635, (short)236, (short)632, -
(short)85, (short)364, (short)235, - (short)85, (short)537, (short)28, (short)141,
(short)83, (short)83, (short)519, (short)250, (short)30, (short)56, (short)365,
(short)141, (short)141, (short)284, (short)525, (short)32, (short)261, (short)460,
(short)384, (short)285, (short)56, (short)453, (short)454, (short)333, (short)334,
(short)366, (short)35, (short)36, (short)37, (short)507, (short)142, (short)406,
(short)86, (short)28, (short)87, (short)375, (short)508, (short)407, (short)142,
(short)30, (short)403, (short)520, (short)56, (short)413, (short)319, (short)666,
(short)298, (short)483, (short)142, (short)414, (short)389, (short)475, (short)643,
(short)88, (short)319, (short)290, (short)396, (short)28, (short)399, (short)400,
(short)335, (short)336, (short)432, (short)30, (short)102, (short)477, (short)487,
(short)103, (short)407, (short)141, (short)236, (short)465, (short)439, (short)421,
(short)235, (short)115, (short)118, (short)502, (short)503, (short)504, (short)73,
(short)73, (short)505, (short)483, (short)511, (short)282, (short)90, (short)91,
(short)483, (short)585, (short)520, (short)512, (short)375, (short)444, (short)397,
(short)58, (short)520, (short)449, (short)398, (short)291, (short)111, (short)28,
(short)85, (short)513, (short)56, (short)112, (short)56, (short)30, (short)300,
(short)113, (short)436, (short)214, (short)141, (short)583, (short)32, (short)447,
(short)534, (short)584, (short)56, (short)214, (short)600, (short)448, (short)407,
(short)148, (short)601, (short)483, (short)526, (short)527, (short)528, (short)483,
(short)532, (short)640, (short)569, (short)284, (short)541, (short)119, (short)28,
(short)121, (short)615, (short)543, (short)28, (short)76, (short)30, (short)109,
(short)407, (short)407, (short)30, (short)531, (short)640, (short)94, (short)596,
(short)653, (short)77, (short)32, (short)296, (short)656, (short)109, (short)407,
(short)646, (short)578, (short)107, (short)407, (short)123, (short)420, (short)35,
(short)36, (short)37, (short)539, (short)28, (short)577, (short)686, (short)289,
(short)124, (short)540, (short)30, (short)56, (short)407, (short)56, (short)92,
(short)93, (short)290, (short)298, (short)35, (short)36, (short)37, (short)520,
(short)125, (short)141, (short)141, (short)141, (short)290, (short)580, (short)35,
(short)36, (short)37, (short)58, (short)141, (short)142, (short)141, (short)639,
(short)462, (short)375, (short)671, (short)141, (short)141, (short)463, (short)672,
(short)140, (short)407, (short)464, (short)140, (short)602, (short)129, (short)690,
(short)141, (short)407, (short)639, (short)593, (short)292, (short)640, (short)720,
(short)718, (short)203, (short)301, (short)620, (short)407, (short)127, (short)140,
(short)56, (short)556, (short)146, (short)538, (short)202, (short)268, (short)559,
(short)140, (short)140, (short)73, (short)28, (short)465, (short)83, (short)216,
(short)73, (short)131, (short)30, (short)494, (short)495, (short)496, (short)497,
(short)142, (short)265, (short)32, (short)279, (short)282, (short)282, (short)669,
(short)708, (short)645, (short)380, (short)331, (short)332, (short)269, (short)35,
(short)36, (short)37, (short)274, (short)337, (short)338, (short)275, (short)311,
(short)612, (short)614, (short)339, (short)340, (short)312, (short)716, (short)294,
(short)141, (short)482, (short)407, (short)313, (short)588, (short)320, (short)56,
(short)324, (short)141, (short)330, (short)639, (short)722, (short)629, (short)659,
(short)660, (short)109, (short)140, (short)586, (short)407, (short)248, (short)607,
(short)407, (short)608, (short)407, (short)328, (short)141, (short)609, (short)407,
(short)623, (short)624, (short)665, (short)303, (short)619, (short)327, (short)375,
(short)675, (short)407, (short)141, (short)677, (short)407, (short)681, (short)407,
(short)375, (short)698, (short)407, (short)644, (short)641, (short)701, (short)407,
(short)329, (short)384, (short)375, (short)56, (short)703, (short)407, (short)714,
(short)407, (short)498, (short)499, (short)140, (short)492, (short)493, (short)356,
(short)367, (short)109, (short)368, (short)500, (short)501, (short)107, (short)370,
(short)377, (short)391, (short)378, (short)650, (short)652, (short)385, (short)655,
(short)386, (short)658, (short)387, (short)388, (short)206, (short)390, (short)392,
(short)439, (short)141, (short)141, (short)393, (short)401, (short)402, (short)375,
(short)305, (short)73, (short)307, (short)307, (short)404, (short)412, (short)204,
(short)416, (short)422, (short)668, (short)415, (short)419, (short)428, (short)315,
(short)316, (short)427, (short)429, (short)430, (short)446, (short)452, (short)431,
(short)307, (short)307, (short)325, (short)307, (short)679, (short)478, (short)481,
(short)683, (short)685, (short)515, (short)375, (short)522, (short)523, (short)524,
(short)62, (short)529, (short)533, (short)535, (short)542, (short)545, (short)549,
(short)307, (short)140, (short)140, (short)140, (short)546, (short)141, (short)551,
(short)553, (short)552, (short)557, (short)140, (short)562, (short)140, (short)565,
(short)574, (short)564, (short)705, (short)140, (short)140, (short)563, (short)709,
(short)568, (short)284, (short)582, (short)572, (short)573, (short)581, (short)647,
(short)140, (short)649, (short)587, (short)236, (short)589, (short)591, (short)590,
(short)235, (short)715, (short)603, (short)594, (short)597, (short)28, (short)63,
(short)595, (short)64, (short)65, (short)598, (short)30, (short)687, (short)606,
(short)610, (short)236, (short)433, (short)616, (short)66, (short)235, (short)73,
(short)67, (short)68, (short)141, (short)618, (short)617, (short)434, (short)435,
(short)236, (short)69, (short)621, (short)625, (short)235, (short)70, (short)71,
(short)627, (short)676, (short)630, (short)631, (short)680, (short)633, (short)636,
(short)592, (short)637, (short)141, (short)344, (short)345, (short)346, (short)347,
(short)348, (short)349, (short)350, (short)351, (short)352, (short)353, (short)642,
(short)140, (short)648, (short)693, (short)662, (short)663, (short)673, (short)696,
(short)697, (short)140, (short)664, (short)699, (short)700, (short)667, (short)702,
(short)354, (short)670, (short)688, (short)689, (short)691, (short)694, (short)695,
(short)726, (short)727, (short)476, (short)626, (short)140, (short)554, (short)710,
(short)466, (short)604, (short)711, (short)372, (short)712, (short)713, (short)379,
(short)561, (short)309, (short)140, (short)467, (short)443, (short)628, (short)310,
(short)307, (short)719, (short)307, (short)307, (short)307, (short)307, (short)307,
(short)307, (short)307, (short)307, (short)307, (short)307, (short)307, (short)307,
(short)307, (short)307, (short)307, (short)307, (short)307, (short)1, (short)2,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)486, (short)488, (short)20, (short)0, (short)0,
(short)140, (short)140, (short)0, (short)0, (short)0, (short)489, (short)0,
(short)0, (short)0, (short)21, (short)491, (short)22, (short)307, (short)490,
(short)0, (short)0, (short)23, (short)0, (short)24, (short)25, (short)26,
(short)27, (short)0, (short)0, (short)0, (short)0, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)140, (short)21, (short)0, (short)0, (short)28,
(short)0, (short)0, (short)0, (short)307, (short)29, (short)30, (short)31,
(short)26, (short)27, (short)307, (short)0, (short)0, (short)32, (short)0,
(short)0, (short)33, (short)34, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)35, (short)36, (short)37, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)251, (short)0,
(short)0, (short)0, (short)140, (short)29, (short)0, (short)31, (short)0,
(short)252, (short)0, (short)307, (short)0, (short)0, (short)253, (short)0,
(short)33, (short)34, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)140, (short)0, (short)0, (short)0, (short)0, (short)0, (short)1,
(short)2, (short)3, (short)105, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)659, (short)660, (short)20, (short)217,
(short)307, (short)218, (short)219, (short)220, (short)221, (short)222, (short)223,
(short)224, (short)225, (short)150, (short)21, (short)151, (short)22, (short)152,
(short)226, (short)0, (short)153, (short)23, (short)227, (short)0, (short)0,
(short)26, (short)27, (short)154, (short)155, (short)228, (short)229, (short)156,
(short)157, (short)158, (short)159, (short)160, (short)161, (short)162, (short)0,
(short)163, (short)164, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)230, (short)165, (short)166, (short)167, (short)168, (short)29, (short)30,
(short)31, (short)0, (short)231, (short)106, (short)0, (short)0, (short)169,
(short)307, (short)0, (short)33, (short)233, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)171, (short)0, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)0, (short)0, (short)175,
(short)176, (short)1, (short)2, (short)3, (short)105, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)217, (short)0, (short)218, (short)219, (short)220,
(short)221, (short)222, (short)223, (short)224, (short)225, (short)150, (short)21,
(short)151, (short)22, (short)152, (short)226, (short)0, (short)153, (short)23,
(short)227, (short)0, (short)0, (short)26, (short)27, (short)154, (short)155,
(short)228, (short)229, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)230, (short)165, (short)166, (short)167,
(short)168, (short)29, (short)30, (short)31, (short)0, (short)231, (short)106,
(short)232, (short)0, (short)169, (short)0, (short)0, (short)33, (short)233,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)171, (short)0,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)0,
(short)0, (short)175, (short)176, (short)1, (short)2, (short)3, (short)105,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)217, (short)0, (short)218, (short)219,
(short)220, (short)221, (short)222, (short)223, (short)224, (short)225, (short)150,
(short)21, (short)151, (short)22, (short)152, (short)226, (short)0, (short)153,
(short)23, (short)227, (short)0, (short)0, (short)26, (short)27, (short)154,
(short)155, (short)228, (short)229, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)163, (short)164, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)230, (short)165, (short)166,
(short)167, (short)168, (short)29, (short)30, (short)31, (short)0, (short)231,
(short)106, (short)0, (short)0, (short)169, (short)0, (short)0, (short)33,
(short)233, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)171,
(short)0, (short)0, (short)172, (short)173, (short)174, (short)0, (short)0,
(short)0, (short)0, (short)175, (short)176, (short)1, (short)2, (short)3,
(short)105, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)150, (short)21,
(short)151, (short)22, (short)152, (short)0, (short)0, (short)153, (short)23,
(short)0, (short)0, (short)0, (short)26, (short)27, (short)154, (short)155,
(short)0, (short)0, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)165, (short)166, (short)167,
(short)168, (short)29, (short)30, (short)31, (short)0, (short)530, (short)0,
(short)0, (short)0, (short)169, (short)0, (short)0, (short)33, (short)233,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)171, (short)0,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)0,
(short)0, (short)175, (short)176, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)150, (short)21, (short)151, (short)0, (short)152, (short)0, (short)0,
(short)153, (short)0, (short)0, (short)0, (short)0, (short)26, (short)27,
(short)154, (short)155, (short)0, (short)0, (short)156, (short)157, (short)158,
(short)159, (short)160, (short)161, (short)162, (short)0, (short)163, (short)164,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)29, (short)30, (short)31, (short)0, (short)0,
(short)317, (short)0, (short)0, (short)169, (short)0, (short)0, (short)33,
(short)233, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)171,
(short)0, (short)0, (short)172, (short)173, (short)174, (short)0, (short)0,
(short)0, (short)0, (short)175, (short)176, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)150, (short)21, (short)151, (short)0, (short)152,
(short)0, (short)0, (short)153, (short)0, (short)0, (short)0, (short)0, (short)26,
(short)27, (short)154, (short)155, (short)0, (short)0, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28,
(short)165, (short)166, (short)167, (short)168, (short)29, (short)30, (short)31,
(short)21, (short)0, (short)0, (short)0, (short)0, (short)169, (short)0, (short)0,
(short)33, (short)233, (short)0, (short)26, (short)27, (short)0, (short)0,
(short)0, (short)171, (short)0, (short)0, (short)172, (short)173, (short)174,
(short)0, (short)0, (short)0, (short)0, (short)175, (short)176, (short)1,
(short)2, (short)3, (short)105, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)20, (short)0,
(short)0, (short)0, (short)29, (short)0, (short)31, (short)0, (short)575,
(short)0, (short)0, (short)0, (short)21, (short)576, (short)0, (short)33,
(short)34, (short)0, (short)0, (short)23, (short)0, (short)0, (short)0, (short)26,
(short)27, (short)0, (short)0, (short)0, (short)0, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)21, (short)0, (short)0,
(short)28, (short)0, (short)0, (short)0, (short)0, (short)29, (short)30, (short)31,
(short)26, (short)27, (short)0, (short)21, (short)0, (short)32, (short)0,
(short)0, (short)33, (short)34, (short)0, (short)0, (short)0, (short)0, (short)26,
(short)27, (short)35, (short)36, (short)37, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)30, (short)31, (short)0,
(short)455, (short)0, (short)0, (short)0, (short)298, (short)456, (short)0,
(short)33, (short)34, (short)0, (short)29, (short)0, (short)31, (short)290,
(short)455, (short)35, (short)36, (short)37, (short)289, (short)456, (short)0,
(short)33, (short)34, (short)0, (short)0, (short)0, (short)0, (short)290,
(short)0, (short)35, (short)36, (short)37, (short)1, (short)2, (short)3, (short)105,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)21, (short)0,
(short)22, (short)0, (short)0, (short)0, (short)0, (short)23, (short)0, (short)0,
(short)0, (short)26, (short)27, (short)0, (short)0, (short)0, (short)0, (short)1,
(short)2, (short)3, (short)105, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)20, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)21, (short)0, (short)22, (short)29, (short)0, (short)31,
(short)0, (short)23, (short)106, (short)0, (short)204, (short)26, (short)27,
(short)0, (short)33, (short)34, (short)1, (short)2, (short)3, (short)105,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)21, (short)0,
(short)0, (short)0, (short)29, (short)0, (short)31, (short)23, (short)0, (short)106,
(short)0, (short)26, (short)27, (short)0, (short)0, (short)33, (short)34,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)217, (short)0, (short)218, (short)219, (short)220, (short)221,
(short)222, (short)223, (short)224, (short)225, (short)150, (short)29, (short)151,
(short)31, (short)152, (short)226, (short)0, (short)153, (short)0, (short)227,
(short)0, (short)0, (short)33, (short)34, (short)154, (short)155, (short)228,
(short)229, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)230, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)21, (short)0, (short)231,
(short)106, (short)0, (short)0, (short)169, (short)0, (short)0, (short)0,
(short)170, (short)26, (short)27, (short)0, (short)0, (short)0, (short)0,
(short)171, (short)0, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)0, (short)0, (short)0, (short)175, (short)176, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)0, (short)31, (short)0, (short)21, (short)139, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)33, (short)34, (short)0, (short)0,
(short)26, (short)27, (short)0, (short)0, (short)0, (short)0, (short)0, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)548, (short)21, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)0, (short)31, (short)0,
(short)26, (short)27, (short)0, (short)0, (short)470, (short)0, (short)0,
(short)33, (short)34, (short)150, (short)0, (short)151, (short)0, (short)152,
(short)0, (short)0, (short)153, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)154, (short)155, (short)0, (short)0, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)0,
(short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)33, (short)34, (short)0, (short)0, (short)0, (short)28, (short)165,
(short)166, (short)167, (short)168, (short)0, (short)30, (short)0, (short)0,
(short)0, (short)308, (short)471, (short)0, (short)169, (short)0, (short)0,
(short)0, (short)170, (short)0, (short)0, (short)0, (short)434, (short)435,
(short)0, (short)171, (short)0, (short)0, (short)172, (short)173, (short)174,
(short)0, (short)0, (short)0, (short)0, (short)175, (short)176, (short)150,
(short)0, (short)151, (short)0, (short)152, (short)0, (short)0, (short)153,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)154, (short)155,
(short)0, (short)0, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)165, (short)166, (short)167,
(short)168, (short)0, (short)30, (short)0, (short)0, (short)0, (short)308,
(short)638, (short)0, (short)169, (short)0, (short)0, (short)0, (short)170,
(short)0, (short)0, (short)0, (short)434, (short)435, (short)0, (short)171,
(short)0, (short)0, (short)172, (short)173, (short)174, (short)0, (short)0,
(short)0, (short)0, (short)175, (short)176, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)0, (short)153, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)154, (short)155, (short)0,
(short)0, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)165, (short)166, (short)167, (short)168,
(short)0, (short)30, (short)0, (short)0, (short)0, (short)308, (short)692,
(short)0, (short)169, (short)0, (short)0, (short)0, (short)170, (short)0,
(short)0, (short)0, (short)434, (short)435, (short)0, (short)171, (short)0,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)0,
(short)0, (short)175, (short)176, (short)150, (short)0, (short)151, (short)0,
(short)152, (short)0, (short)0, (short)153, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)154, (short)155, (short)0, (short)0,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)0, (short)163, (short)164, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)165, (short)166, (short)167, (short)168, (short)0,
(short)30, (short)0, (short)0, (short)0, (short)308, (short)0, (short)0, (short)169,
(short)0, (short)0, (short)0, (short)170, (short)0, (short)62, (short)0, (short)434,
(short)435, (short)0, (short)171, (short)0, (short)0, (short)172, (short)173,
(short)174, (short)0, (short)0, (short)0, (short)0, (short)175, (short)176,
(short)150, (short)0, (short)151, (short)0, (short)152, (short)0, (short)0,
(short)153, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)154,
(short)155, (short)0, (short)0, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)163, (short)164, (short)0,
(short)0, (short)28, (short)63, (short)0, (short)64, (short)65, (short)0,
(short)30, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)66,
(short)0, (short)0, (short)67, (short)68, (short)0, (short)28, (short)165,
(short)166, (short)167, (short)168, (short)69, (short)30, (short)0, (short)0,
(short)70, (short)71, (short)0, (short)0, (short)169, (short)0, (short)0,
(short)0, (short)170, (short)150, (short)0, (short)151, (short)0, (short)152,
(short)259, (short)171, (short)153, (short)0, (short)172, (short)173, (short)174,
(short)0, (short)0, (short)154, (short)155, (short)175, (short)176, (short)156,
(short)157, (short)158, (short)159, (short)160, (short)161, (short)162, (short)0,
(short)163, (short)164, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)165, (short)166, (short)167, (short)168, (short)0, (short)30,
(short)0, (short)0, (short)0, (short)308, (short)0, (short)0, (short)169,
(short)0, (short)0, (short)0, (short)170, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)171, (short)153, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)154, (short)155, (short)175,
(short)176, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)165, (short)166, (short)167, (short)168,
(short)0, (short)30, (short)0, (short)0, (short)394, (short)0, (short)0, (short)0,
(short)169, (short)0, (short)0, (short)0, (short)170, (short)150, (short)0,
(short)151, (short)0, (short)152, (short)0, (short)171, (short)153, (short)0,
(short)172, (short)173, (short)174, (short)0, (short)0, (short)154, (short)155,
(short)175, (short)176, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)165, (short)166, (short)167,
(short)168, (short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)169, (short)0, (short)0, (short)0, (short)170, (short)150,
(short)0, (short)151, (short)0, (short)152, (short)459, (short)171, (short)153,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)154,
(short)155, (short)175, (short)176, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)163, (short)164, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)169, (short)509, (short)0, (short)0,
(short)170, (short)150, (short)0, (short)151, (short)0, (short)152, (short)0,
(short)171, (short)153, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)0, (short)154, (short)155, (short)175, (short)176, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165,
(short)166, (short)167, (short)168, (short)0, (short)30, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)169, (short)536, (short)0,
(short)0, (short)170, (short)150, (short)0, (short)151, (short)0, (short)152,
(short)0, (short)171, (short)153, (short)0, (short)172, (short)173, (short)174,
(short)0, (short)0, (short)154, (short)155, (short)175, (short)176, (short)156,
(short)157, (short)158, (short)159, (short)160, (short)161, (short)162, (short)0,
(short)163, (short)164, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)165, (short)166, (short)167, (short)168, (short)0, (short)30,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)169, (short)0,
(short)0, (short)0, (short)170, (short)150, (short)0, (short)151, (short)0,
(short)152, (short)579, (short)171, (short)153, (short)0, (short)172, (short)173,
(short)174, (short)0, (short)0, (short)154, (short)155, (short)175, (short)176,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)0, (short)163, (short)164, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)165, (short)166, (short)167, (short)168, (short)0,
(short)30, (short)0, (short)0, (short)611, (short)0, (short)0, (short)0, (short)169,
(short)0, (short)0, (short)0, (short)170, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)171, (short)153, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)154, (short)155, (short)175,
(short)176, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)165, (short)166, (short)167, (short)168,
(short)0, (short)30, (short)0, (short)0, (short)613, (short)0, (short)0, (short)0,
(short)169, (short)0, (short)0, (short)0, (short)170, (short)150, (short)0,
(short)151, (short)0, (short)152, (short)0, (short)171, (short)153, (short)0,
(short)172, (short)173, (short)174, (short)0, (short)0, (short)154, (short)155,
(short)175, (short)176, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)165, (short)166, (short)167,
(short)168, (short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)169, (short)651, (short)0, (short)0, (short)170, (short)150,
(short)0, (short)151, (short)0, (short)152, (short)0, (short)171, (short)153,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)154,
(short)155, (short)175, (short)176, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)163, (short)164, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)169, (short)654, (short)0, (short)0,
(short)170, (short)150, (short)0, (short)151, (short)0, (short)152, (short)0,
(short)171, (short)153, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)0, (short)154, (short)155, (short)175, (short)176, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165,
(short)166, (short)167, (short)168, (short)0, (short)30, (short)0, (short)0,
(short)657, (short)0, (short)0, (short)0, (short)169, (short)0, (short)0,
(short)0, (short)170, (short)150, (short)0, (short)151, (short)0, (short)152,
(short)0, (short)171, (short)153, (short)0, (short)172, (short)173, (short)174,
(short)0, (short)0, (short)154, (short)155, (short)175, (short)176, (short)156,
(short)157, (short)158, (short)159, (short)160, (short)161, (short)162, (short)0,
(short)163, (short)164, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)165, (short)166, (short)167, (short)168, (short)0, (short)30,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)169, (short)678,
(short)0, (short)0, (short)170, (short)150, (short)0, (short)151, (short)0,
(short)152, (short)0, (short)171, (short)153, (short)0, (short)172, (short)173,
(short)174, (short)0, (short)0, (short)154, (short)155, (short)175, (short)176,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)0, (short)163, (short)164, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)165, (short)166, (short)167, (short)168, (short)0,
(short)30, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)169,
(short)682, (short)0, (short)0, (short)170, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)171, (short)153, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)154, (short)155, (short)175,
(short)176, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)165, (short)166, (short)167, (short)168,
(short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)169, (short)684, (short)0, (short)0, (short)170, (short)150, (short)0,
(short)151, (short)0, (short)152, (short)0, (short)171, (short)153, (short)0,
(short)172, (short)173, (short)174, (short)0, (short)0, (short)154, (short)155,
(short)175, (short)176, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)165, (short)166, (short)167,
(short)168, (short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)169, (short)704, (short)0, (short)0, (short)170, (short)150,
(short)0, (short)151, (short)0, (short)152, (short)0, (short)171, (short)153,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)154,
(short)155, (short)175, (short)176, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)163, (short)164, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)169, (short)0, (short)0, (short)0, (short)170,
(short)150, (short)0, (short)151, (short)0, (short)0, (short)0, (short)171,
(short)0, (short)0, (short)172, (short)173, (short)174, (short)0, (short)0,
(short)154, (short)155, (short)175, (short)176, (short)156, (short)157, (short)158,
(short)159, (short)160, (short)161, (short)162, (short)0, (short)163, (short)164,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)0,
(short)308, (short)0, (short)0, (short)169, (short)0, (short)0, (short)0,
(short)170, (short)150, (short)0, (short)151, (short)0, (short)0, (short)0,
(short)171, (short)0, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)0, (short)154, (short)155, (short)175, (short)176, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165,
(short)166, (short)167, (short)168, (short)0, (short)30, (short)0, (short)0,
(short)0, (short)592, (short)0, (short)0, (short)169, (short)0, (short)0,
(short)0, (short)170, (short)150, (short)0, (short)151, (short)0, (short)0,
(short)0, (short)171, (short)0, (short)0, (short)172, (short)173, (short)174,
(short)0, (short)0, (short)154, (short)155, (short)175, (short)176, (short)156,
(short)157, (short)158, (short)159, (short)160, (short)161, (short)162, (short)0,
(short)163, (short)164, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)165, (short)166, (short)167, (short)168, (short)0, (short)30,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)304, (short)0,
(short)0, (short)0, (short)170, (short)150, (short)0, (short)151, (short)0,
(short)0, (short)0, (short)171, (short)0, (short)0, (short)172, (short)173,
(short)174, (short)0, (short)0, (short)154, (short)155, (short)175, (short)176,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)0, (short)163, (short)164, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)165, (short)166, (short)167, (short)168, (short)0,
(short)30, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)314,
(short)0, (short)0, (short)0, (short)170, (short)150, (short)0, (short)151,
(short)0, (short)0, (short)0, (short)171, (short)0, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)154, (short)155, (short)175,
(short)176, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)62,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)165, (short)166, (short)167, (short)168,
(short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)169, (short)0, (short)0, (short)0, (short)170, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)171, (short)0, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)0, (short)0, (short)175,
(short)176, (short)28, (short)63, (short)0, (short)64, (short)65, (short)0,
(short)30, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)66,
(short)0, (short)0, (short)67, (short)68, (short)0, (short)0, (short)0, (short)434,
(short)435, (short)0, (short)69, (short)0, (short)0, (short)0, (short)70,
(short)71}; static short Cyc_yycheck[ 3998u]={ (short)52, (short)0, (short)86,
(short)112, (short)52, (short)242, (short)129, (short)59, (short)152, (short)87,
(short)88, (short)52, (short)83, (short)111, (short)204, (short)22, (short)274,
(short)52, (short)279, (short)409, (short)365, (short)169, (short)21, (short)22,
(short)54, (short)24, (short)53, (short)26, (short)27, (short)413, (short)429,
(short)32, (short)103, (short)32, (short)303, (short)113, (short)435, (short)0,
(short)70, (short)17, (short)39, (short)112, (short)89, (short)44, (short)371,
(short)44, (short)98, (short)99, (short)100, (short)101, (short)98, (short)50,
(short)69, (short)100, (short)106, (short)54, (short)108, (short)86, (short)106,
(short)66, (short)108, (short)451, (short)127, (short)98, (short)71, (short)96,
(short)94, (short)66, (short)45, (short)46, (short)47, (short)455, (short)71,
(short)108, (short)105, (short)106, (short)39, (short)105, (short)110, (short)78,
(short)317, (short)80, (short)92, (short)73, (short)17, (short)18, (short)19,
(short)104, (short)115, (short)79, (short)27, (short)90, (short)91, (short)92,
(short)93, (short)422, (short)89, (short)83, (short)169, (short)648, (short)97,
(short)94, (short)171, (short)172, (short)169, (short)174, (short)84, (short)291,
(short)109, (short)106, (short)109, (short)84, (short)102, (short)99, (short)104,
(short)97, (short)300, (short)90, (short)55, (short)118, (short)102, (short)116,
(short)121, (short)192, (short)123, (short)524, (short)204, (short)90, (short)91,
(short)92, (short)93, (short)84, (short)131, (short)64, (short)108, (short)134,
(short)135, (short)60, (short)61, (short)62, (short)130, (short)212, (short)116,
(short)117, (short)84, (short)414, (short)147, (short)142, (short)147, (short)64,
(short)64, (short)91, (short)147, (short)205, (short)356, (short)118, (short)304,
(short)205, (short)707, (short)361, (short)225, (short)99, (short)546, (short)140,
(short)141, (short)83, (short)314, (short)716, (short)99, (short)107, (short)205,
(short)720, (short)103, (short)722, (short)97, (short)108, (short)109, (short)110,
(short)83, (short)98, (short)99, (short)86, (short)105, (short)106, (short)99,
(short)99, (short)447, (short)448, (short)103, (short)103, (short)242, (short)575,
(short)94, (short)82, (short)242, (short)97, (short)398, (short)84, (short)103,
(short)102, (short)102, (short)377, (short)203, (short)90, (short)203, (short)94,
(short)111, (short)112, (short)89, (short)385, (short)97, (short)116, (short)290,
(short)214, (short)95, (short)214, (short)287, (short)288, (short)65, (short)66,
(short)109, (short)108, (short)109, (short)110, (short)84, (short)296, (short)93,
(short)97, (short)84, (short)94, (short)308, (short)91, (short)99, (short)304,
(short)90, (short)236, (short)380, (short)236, (short)93, (short)304, (short)630,
(short)97, (short)320, (short)314, (short)99, (short)220, (short)311, (short)592,
(short)94, (short)314, (short)106, (short)226, (short)84, (short)228, (short)229,
(short)102, (short)103, (short)93, (short)90, (short)91, (short)312, (short)326,
(short)94, (short)99, (short)169, (short)317, (short)293, (short)274, (short)263,
(short)317, (short)300, (short)94, (short)341, (short)342, (short)343, (short)274,
(short)275, (short)355, (short)356, (short)84, (short)279, (short)93, (short)94,
(short)361, (short)474, (short)429, (short)91, (short)365, (short)278, (short)93,
(short)92, (short)435, (short)282, (short)97, (short)289, (short)97, (short)84,
(short)298, (short)363, (short)298, (short)102, (short)300, (short)90, (short)298,
(short)106, (short)563, (short)377, (short)212, (short)95, (short)97, (short)93,
(short)93, (short)99, (short)312, (short)385, (short)95, (short)99, (short)99,
(short)402, (short)99, (short)398, (short)386, (short)387, (short)388, (short)402,
(short)390, (short)584, (short)450, (short)89, (short)407, (short)97, (short)84,
(short)94, (short)93, (short)95, (short)84, (short)89, (short)90, (short)390,
(short)99, (short)99, (short)90, (short)390, (short)601, (short)93, (short)104,
(short)93, (short)100, (short)97, (short)99, (short)93, (short)403, (short)99,
(short)103, (short)463, (short)403, (short)99, (short)97, (short)263, (short)108,
(short)109, (short)110, (short)403, (short)84, (short)462, (short)93, (short)97,
(short)84, (short)403, (short)90, (short)369, (short)99, (short)371, (short)93,
(short)94, (short)106, (short)97, (short)108, (short)109, (short)110, (short)524,
(short)85, (short)287, (short)288, (short)289, (short)106, (short)464, (short)108,
(short)109, (short)110, (short)92, (short)296, (short)463, (short)298, (short)584,
(short)97, (short)474, (short)95, (short)303, (short)304, (short)102, (short)99,
(short)83, (short)99, (short)106, (short)86, (short)515, (short)83, (short)104,
(short)314, (short)99, (short)601, (short)481, (short)142, (short)672, (short)104,
(short)95, (short)99, (short)147, (short)542, (short)99, (short)96, (short)103,
(short)422, (short)431, (short)98, (short)401, (short)93, (short)427, (short)436,
(short)111, (short)112, (short)431, (short)84, (short)461, (short)102, (short)86,
(short)436, (short)105, (short)90, (short)333, (short)334, (short)335, (short)336,
(short)515, (short)97, (short)97, (short)94, (short)447, (short)448, (short)640,
(short)688, (short)596, (short)104, (short)67, (short)68, (short)98, (short)108,
(short)109, (short)110, (short)94, (short)63, (short)64, (short)97, (short)97,
(short)530, (short)531, (short)111, (short)112, (short)97, (short)707, (short)111,
(short)377, (short)98, (short)99, (short)97, (short)477, (short)97, (short)477,
(short)95, (short)385, (short)113, (short)672, (short)720, (short)568, (short)22,
(short)23, (short)539, (short)169, (short)98, (short)99, (short)539, (short)98,
(short)99, (short)98, (short)99, (short)114, (short)402, (short)98, (short)99,
(short)85, (short)86, (short)98, (short)99, (short)539, (short)69, (short)584,
(short)98, (short)99, (short)414, (short)98, (short)99, (short)98, (short)99,
(short)592, (short)98, (short)99, (short)595, (short)589, (short)98, (short)99,
(short)115, (short)523, (short)601, (short)523, (short)98, (short)99, (short)98,
(short)99, (short)337, (short)338, (short)212, (short)331, (short)332, (short)97,
(short)95, (short)588, (short)95, (short)339, (short)340, (short)588, (short)95,
(short)94, (short)84, (short)95, (short)610, (short)611, (short)94, (short)613,
(short)97, (short)615, (short)97, (short)97, (short)588, (short)97, (short)93,
(short)563, (short)462, (short)463, (short)93, (short)104, (short)97, (short)640,
(short)150, (short)563, (short)152, (short)153, (short)95, (short)98, (short)96,
(short)99, (short)95, (short)637, (short)98, (short)107, (short)95, (short)163,
(short)164, (short)99, (short)96, (short)98, (short)95, (short)95, (short)99,
(short)171, (short)172, (short)70, (short)174, (short)653, (short)35, (short)98,
(short)656, (short)657, (short)102, (short)672, (short)93, (short)99, (short)104,
(short)37, (short)28, (short)93, (short)41, (short)83, (short)98, (short)84,
(short)192, (short)287, (short)288, (short)289, (short)111, (short)515, (short)97,
(short)99, (short)98, (short)84, (short)296, (short)95, (short)298, (short)95,
(short)107, (short)98, (short)686, (short)303, (short)304, (short)99, (short)690,
(short)97, (short)89, (short)84, (short)98, (short)98, (short)98, (short)607,
(short)314, (short)609, (short)98, (short)688, (short)97, (short)95, (short)98,
(short)688, (short)706, (short)95, (short)98, (short)98, (short)84, (short)85,
(short)99, (short)87, (short)88, (short)98, (short)90, (short)659, (short)95,
(short)97, (short)707, (short)95, (short)94, (short)97, (short)707, (short)659,
(short)100, (short)101, (short)568, (short)98, (short)93, (short)105, (short)106,
(short)720, (short)108, (short)95, (short)98, (short)720, (short)112, (short)113,
(short)107, (short)651, (short)99, (short)98, (short)654, (short)98, (short)107,
(short)94, (short)102, (short)589, (short)71, (short)72, (short)73, (short)74,
(short)75, (short)76, (short)77, (short)78, (short)79, (short)80, (short)98,
(short)377, (short)94, (short)673, (short)93, (short)98, (short)26, (short)677,
(short)678, (short)385, (short)98, (short)681, (short)682, (short)98, (short)684,
(short)96, (short)98, (short)104, (short)95, (short)98, (short)95, (short)93,
(short)0, (short)0, (short)312, (short)553, (short)402, (short)427, (short)698,
(short)294, (short)523, (short)701, (short)203, (short)703, (short)704, (short)212,
(short)437, (short)153, (short)414, (short)296, (short)275, (short)563, (short)153,
(short)325, (short)714, (short)327, (short)328, (short)329, (short)330, (short)331,
(short)332, (short)333, (short)334, (short)335, (short)336, (short)337, (short)338,
(short)339, (short)340, (short)341, (short)342, (short)343, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, (short)325, (short)327, (short)24, - (short)1, - (short)1,
(short)462, (short)463, - (short)1, - (short)1, - (short)1, (short)328, -
(short)1, - (short)1, - (short)1, (short)36, (short)330, (short)38, (short)380,
(short)329, - (short)1, - (short)1, (short)43, - (short)1, (short)45, (short)46,
(short)47, (short)48, - (short)1, - (short)1, - (short)1, - (short)1, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)515, (short)36,
- (short)1, - (short)1, (short)84, - (short)1, - (short)1, - (short)1, (short)429,
(short)89, (short)90, (short)91, (short)47, (short)48, (short)435, - (short)1, -
(short)1, (short)97, - (short)1, - (short)1, (short)100, (short)101, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)108, (short)109,
(short)110, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84,
- (short)1, - (short)1, - (short)1, (short)568, (short)89, - (short)1, (short)91,
- (short)1, (short)93, - (short)1, (short)481, - (short)1, - (short)1, (short)98,
- (short)1, (short)100, (short)101, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)589, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, (short)22, (short)23, (short)24, (short)25, (short)524, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)36, (short)37, (short)38, (short)39, (short)40, - (short)1,
(short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, (short)89,
(short)90, (short)91, - (short)1, (short)93, (short)94, - (short)1, - (short)1,
(short)97, (short)596, - (short)1, (short)100, (short)101, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)108, - (short)1, -
(short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, - (short)1,
- (short)1, (short)118, (short)119, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, (short)25, - (short)1, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)36, (short)37, (short)38, (short)39, (short)40, - (short)1,
(short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, (short)89,
(short)90, (short)91, - (short)1, (short)93, (short)94, (short)95, - (short)1,
(short)97, - (short)1, - (short)1, (short)100, (short)101, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)108, - (short)1, -
(short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, - (short)1,
- (short)1, (short)118, (short)119, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, (short)25, - (short)1, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)36, (short)37, (short)38, (short)39, (short)40, - (short)1,
(short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, (short)89,
(short)90, (short)91, - (short)1, (short)93, (short)94, - (short)1, - (short)1,
(short)97, - (short)1, - (short)1, (short)100, (short)101, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)108, - (short)1, -
(short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, - (short)1,
- (short)1, (short)118, (short)119, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)35, (short)36, (short)37, (short)38, (short)39, - (short)1, -
(short)1, (short)42, (short)43, - (short)1, - (short)1, - (short)1, (short)47,
(short)48, (short)49, (short)50, - (short)1, - (short)1, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, (short)61,
(short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87, (short)88,
(short)89, (short)90, (short)91, - (short)1, (short)93, - (short)1, - (short)1,
- (short)1, (short)97, - (short)1, - (short)1, (short)100, (short)101, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)108, -
(short)1, - (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1,
- (short)1, - (short)1, (short)118, (short)119, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)35, (short)36, (short)37, -
(short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1,
- (short)1, - (short)1, (short)47, (short)48, (short)49, (short)50, - (short)1,
- (short)1, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85,
(short)86, (short)87, (short)88, (short)89, (short)90, (short)91, - (short)1, -
(short)1, (short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
(short)100, (short)101, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112,
(short)113, - (short)1, - (short)1, - (short)1, - (short)1, (short)118, (short)119,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)35,
(short)36, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, (short)47, (short)48, (short)49,
(short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84,
(short)85, (short)86, (short)87, (short)88, (short)89, (short)90, (short)91,
(short)36, - (short)1, - (short)1, - (short)1, - (short)1, (short)97, - (short)1,
- (short)1, (short)100, (short)101, - (short)1, (short)47, (short)48, - (short)1,
- (short)1, - (short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112,
(short)113, - (short)1, - (short)1, - (short)1, - (short)1, (short)118, (short)119,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, (short)89, - (short)1, (short)91, - (short)1,
(short)93, - (short)1, - (short)1, - (short)1, (short)36, (short)98, - (short)1,
(short)100, (short)101, - (short)1, - (short)1, (short)43, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, - (short)1, - (short)1, - (short)1, - (short)1,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21,
(short)36, - (short)1, - (short)1, (short)84, - (short)1, - (short)1, - (short)1,
- (short)1, (short)89, (short)90, (short)91, (short)47, (short)48, - (short)1,
(short)36, - (short)1, (short)97, - (short)1, - (short)1, (short)100, (short)101,
- (short)1, - (short)1, - (short)1, - (short)1, (short)47, (short)48, (short)108,
(short)109, (short)110, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)84, - (short)1, - (short)1, - (short)1, - (short)1, (short)89,
(short)90, (short)91, - (short)1, (short)93, - (short)1, - (short)1, - (short)1,
(short)97, (short)98, - (short)1, (short)100, (short)101, - (short)1, (short)89,
- (short)1, (short)91, (short)106, (short)93, (short)108, (short)109, (short)110,
(short)97, (short)98, - (short)1, (short)100, (short)101, - (short)1, - (short)1,
- (short)1, - (short)1, (short)106, - (short)1, (short)108, (short)109, (short)110,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, (short)38,
- (short)1, - (short)1, - (short)1, - (short)1, (short)43, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, - (short)1, - (short)1, - (short)1, - (short)1,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, (short)38,
(short)89, - (short)1, (short)91, - (short)1, (short)43, (short)94, - (short)1,
(short)96, (short)47, (short)48, - (short)1, (short)100, (short)101, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, - (short)1,
- (short)1, (short)89, - (short)1, (short)91, (short)43, - (short)1, (short)94,
- (short)1, (short)47, (short)48, - (short)1, - (short)1, (short)100, (short)101,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)25,
- (short)1, (short)27, (short)28, (short)29, (short)30, (short)31, (short)32,
(short)33, (short)34, (short)35, (short)89, (short)37, (short)91, (short)39,
(short)40, - (short)1, (short)42, - (short)1, (short)44, - (short)1, - (short)1,
(short)100, (short)101, (short)49, (short)50, (short)51, (short)52, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1,
(short)61, (short)62, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, -
(short)1, (short)90, (short)36, - (short)1, (short)93, (short)94, - (short)1, -
(short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101, (short)47,
(short)48, - (short)1, - (short)1, - (short)1, - (short)1, (short)108, - (short)1,
- (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, -
(short)1, - (short)1, (short)118, (short)119, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)89, - (short)1, (short)91, - (short)1, (short)36, (short)94, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)100, (short)101,
- (short)1, - (short)1, (short)47, (short)48, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)81, (short)36, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)89, - (short)1, (short)91, - (short)1,
(short)47, (short)48, - (short)1, - (short)1, (short)30, - (short)1, - (short)1,
(short)100, (short)101, (short)35, - (short)1, (short)37, - (short)1, (short)39,
- (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)49, (short)50, - (short)1, - (short)1, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1,
(short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)89, - (short)1, (short)91, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)100, (short)101, -
(short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87,
(short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1, (short)94,
(short)95, - (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101,
- (short)1, - (short)1, - (short)1, (short)105, (short)106, - (short)1, (short)108,
- (short)1, - (short)1, (short)111, (short)112, (short)113, - (short)1, -
(short)1, - (short)1, - (short)1, (short)118, (short)119, (short)35, - (short)1,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49, (short)50,
- (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84,
(short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, -
(short)1, - (short)1, (short)94, (short)95, - (short)1, (short)97, - (short)1, -
(short)1, - (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105,
(short)106, - (short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112,
(short)113, - (short)1, - (short)1, - (short)1, - (short)1, (short)118, (short)119,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1,
(short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)49, (short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, - (short)1,
(short)90, - (short)1, - (short)1, - (short)1, (short)94, (short)95, - (short)1,
(short)97, - (short)1, - (short)1, - (short)1, (short)101, - (short)1, - (short)1,
- (short)1, (short)105, (short)106, - (short)1, (short)108, - (short)1, -
(short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, - (short)1,
- (short)1, (short)118, (short)119, (short)35, - (short)1, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)49, (short)50, - (short)1, - (short)1,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59, -
(short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86,
(short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1,
(short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1, - (short)1,
(short)101, - (short)1, (short)37, - (short)1, (short)105, (short)106, - (short)1,
(short)108, - (short)1, - (short)1, (short)111, (short)112, (short)113, -
(short)1, - (short)1, - (short)1, - (short)1, (short)118, (short)119, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49,
(short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, -
(short)1, (short)84, (short)85, - (short)1, (short)87, (short)88, - (short)1,
(short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)97, - (short)1, - (short)1, (short)100, (short)101, - (short)1, (short)84,
(short)85, (short)86, (short)87, (short)88, (short)108, (short)90, - (short)1, -
(short)1, (short)112, (short)113, - (short)1, - (short)1, (short)97, - (short)1,
- (short)1, - (short)1, (short)101, (short)35, - (short)1, (short)37, - (short)1,
(short)39, (short)107, (short)108, (short)42, - (short)1, (short)111, (short)112,
(short)113, - (short)1, - (short)1, (short)49, (short)50, (short)118, (short)119,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59, -
(short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86,
(short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1,
(short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1, - (short)1,
(short)101, (short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1,
(short)108, (short)42, - (short)1, (short)111, (short)112, (short)113, - (short)1,
- (short)1, (short)49, (short)50, (short)118, (short)119, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, (short)61,
(short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, -
(short)1, (short)90, - (short)1, - (short)1, (short)93, - (short)1, - (short)1,
- (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)108, (short)42,
- (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, (short)49,
(short)50, (short)118, (short)119, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)84, (short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)97,
- (short)1, - (short)1, - (short)1, (short)101, (short)35, - (short)1, (short)37,
- (short)1, (short)39, (short)107, (short)108, (short)42, - (short)1, (short)111,
(short)112, (short)113, - (short)1, - (short)1, (short)49, (short)50, (short)118,
(short)119, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85,
(short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)97, (short)98, - (short)1,
- (short)1, (short)101, (short)35, - (short)1, (short)37, - (short)1, (short)39,
- (short)1, (short)108, (short)42, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, (short)49, (short)50, (short)118, (short)119, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1,
(short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87,
(short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)97, (short)98, - (short)1, - (short)1, (short)101,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)108,
(short)42, - (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1,
(short)49, (short)50, (short)118, (short)119, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, - (short)1,
(short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)97, - (short)1, - (short)1, - (short)1, (short)101, (short)35, - (short)1,
(short)37, - (short)1, (short)39, (short)107, (short)108, (short)42, - (short)1,
(short)111, (short)112, (short)113, - (short)1, - (short)1, (short)49, (short)50,
(short)118, (short)119, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84,
(short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, -
(short)1, (short)93, - (short)1, - (short)1, - (short)1, (short)97, - (short)1,
- (short)1, - (short)1, (short)101, (short)35, - (short)1, (short)37, - (short)1,
(short)39, - (short)1, (short)108, (short)42, - (short)1, (short)111, (short)112,
(short)113, - (short)1, - (short)1, (short)49, (short)50, (short)118, (short)119,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59, -
(short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86,
(short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1, (short)93,
- (short)1, - (short)1, - (short)1, (short)97, - (short)1, - (short)1, - (short)1,
(short)101, (short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1,
(short)108, (short)42, - (short)1, (short)111, (short)112, (short)113, - (short)1,
- (short)1, (short)49, (short)50, (short)118, (short)119, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, (short)61,
(short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, -
(short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)97, (short)98, - (short)1, - (short)1, (short)101, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)108, (short)42,
- (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, (short)49,
(short)50, (short)118, (short)119, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)84, (short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)97,
(short)98, - (short)1, - (short)1, (short)101, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)108, (short)42, - (short)1, (short)111,
(short)112, (short)113, - (short)1, - (short)1, (short)49, (short)50, (short)118,
(short)119, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85,
(short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1,
(short)93, - (short)1, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, (short)35, - (short)1, (short)37, - (short)1, (short)39,
- (short)1, (short)108, (short)42, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, (short)49, (short)50, (short)118, (short)119, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1,
(short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87,
(short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)97, (short)98, - (short)1, - (short)1, (short)101,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)108,
(short)42, - (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1,
(short)49, (short)50, (short)118, (short)119, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, - (short)1,
(short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)97, (short)98, - (short)1, - (short)1, (short)101, (short)35, - (short)1,
(short)37, - (short)1, (short)39, - (short)1, (short)108, (short)42, - (short)1,
(short)111, (short)112, (short)113, - (short)1, - (short)1, (short)49, (short)50,
(short)118, (short)119, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84,
(short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)97, (short)98,
- (short)1, - (short)1, (short)101, (short)35, - (short)1, (short)37, - (short)1,
(short)39, - (short)1, (short)108, (short)42, - (short)1, (short)111, (short)112,
(short)113, - (short)1, - (short)1, (short)49, (short)50, (short)118, (short)119,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59, -
(short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86,
(short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)97, (short)98, - (short)1, - (short)1,
(short)101, (short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1,
(short)108, (short)42, - (short)1, (short)111, (short)112, (short)113, - (short)1,
- (short)1, (short)49, (short)50, (short)118, (short)119, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, (short)61,
(short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, -
(short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101, (short)35,
- (short)1, (short)37, - (short)1, - (short)1, - (short)1, (short)108, - (short)1,
- (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, (short)49,
(short)50, (short)118, (short)119, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)84, (short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, (short)94, - (short)1, - (short)1, (short)97,
- (short)1, - (short)1, - (short)1, (short)101, (short)35, - (short)1, (short)37,
- (short)1, - (short)1, - (short)1, (short)108, - (short)1, - (short)1, (short)111,
(short)112, (short)113, - (short)1, - (short)1, (short)49, (short)50, (short)118,
(short)119, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85,
(short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, - (short)1,
- (short)1, (short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, (short)35, - (short)1, (short)37, - (short)1, - (short)1,
- (short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, (short)49, (short)50, (short)118, (short)119, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1,
(short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87,
(short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101,
(short)35, - (short)1, (short)37, - (short)1, - (short)1, - (short)1, (short)108,
- (short)1, - (short)1, (short)111, (short)112, (short)113, - (short)1, -
(short)1, (short)49, (short)50, (short)118, (short)119, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, (short)61,
(short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, -
(short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101, (short)35,
- (short)1, (short)37, - (short)1, - (short)1, - (short)1, (short)108, - (short)1,
- (short)1, (short)111, (short)112, (short)113, - (short)1, - (short)1, (short)49,
(short)50, (short)118, (short)119, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)37, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84,
(short)85, (short)86, (short)87, (short)88, - (short)1, (short)90, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)97, - (short)1,
- (short)1, - (short)1, (short)101, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)108, - (short)1, - (short)1, (short)111,
(short)112, (short)113, - (short)1, - (short)1, - (short)1, - (short)1, (short)118,
(short)119, (short)84, (short)85, - (short)1, (short)87, (short)88, - (short)1,
(short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)97, - (short)1, - (short)1, (short)100, (short)101, - (short)1, - (short)1,
- (short)1, (short)105, (short)106, - (short)1, (short)108, - (short)1, -
(short)1, - (short)1, (short)112, (short)113}; char Cyc_Yyimpossible_tag[ 13u]="Yyimpossible";
struct Cyc_Yyimpossible_struct{ char* tag; } ; char Cyc_Yystack_overflow_tag[ 17u]="Yystack_overflow";
struct Cyc_Yystack_overflow_struct{ char* tag; } ; extern void Cyc_yyerror(
struct _tagged_string); extern int Cyc_yylex(); char Cyc_YYINITIALSVAL_tag[ 14u]="YYINITIALSVAL";
struct Cyc_YYINITIALSVAL_struct{ char* tag; } ; static int Cyc_yychar=( int)'\000';
struct Cyc_YYINITIALSVAL_struct Cyc_yyinitval={ Cyc_YYINITIALSVAL_tag}; struct
_xtunion_struct* Cyc_yylval=( struct _xtunion_struct*)& Cyc_yyinitval; static
int Cyc_yynerrs= 0; int Cyc_yyparse(){ int yystate; int yyn= 0; int yyerrstatus;
int yychar1= 0; int yyssp_offset; struct _tagged_ptr1 yyss=( struct _tagged_ptr1)({
short* _temp2072=( short*)({ unsigned int _temp2070= 200u; short* _temp2071=(
short*) GC_malloc_atomic( sizeof( short) * _temp2070); unsigned int i; for( i= 0;
i < _temp2070; i ++){ _temp2071[ i]= (short)0;} _temp2071;}); struct
_tagged_ptr1 _temp2073; _temp2073.curr= _temp2072; _temp2073.base= _temp2072;
_temp2073.last_plus_one= _temp2072 + 200; _temp2073;}); int yyvsp_offset; struct
_tagged_ptr2 yyvs=( struct _tagged_ptr2)({ struct _xtunion_struct** _temp2076=(
struct _xtunion_struct**)({ unsigned int _temp2074= 200u; struct _xtunion_struct**
_temp2075=( struct _xtunion_struct**) GC_malloc( sizeof( struct _xtunion_struct*)
* _temp2074); unsigned int i; for( i= 0; i < _temp2074; i ++){ _temp2075[ i]=
Cyc_yylval;} _temp2075;}); struct _tagged_ptr2 _temp2077; _temp2077.curr=
_temp2076; _temp2077.base= _temp2076; _temp2077.last_plus_one= _temp2076 + 200;
_temp2077;}); int yylsp_offset; struct _tagged_ptr3 yyls=( struct _tagged_ptr3)({
struct Cyc_Yyltype* _temp2080=( struct Cyc_Yyltype*)({ unsigned int _temp2078=
200u; struct Cyc_Yyltype* _temp2079=( struct Cyc_Yyltype*) GC_malloc( sizeof(
struct Cyc_Yyltype) * _temp2078); unsigned int i; for( i= 0; i < _temp2078; i ++){
_temp2079[ i]= Cyc_yynewloc();} _temp2079;}); struct _tagged_ptr3 _temp2081;
_temp2081.curr= _temp2080; _temp2081.base= _temp2080; _temp2081.last_plus_one=
_temp2080 + 200; _temp2081;}); int yystacksize= 200; struct _xtunion_struct*
yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar=
- 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:({ struct
_tagged_ptr1 _temp2082= yyss; short* _temp2084= _temp2082.curr +( ++
yyssp_offset); if( _temp2084 < _temp2082.base? 1: _temp2084 >= _temp2082.last_plus_one){
_throw( Null_Exception);}* _temp2084=( short) yystate;}); if( yyssp_offset >=
yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror(( struct _tagged_string)({
char* _temp2085=( char*)"parser stack overflow"; struct _tagged_string _temp2086;
_temp2086.curr= _temp2085; _temp2086.base= _temp2085; _temp2086.last_plus_one=
_temp2085 + 22; _temp2086;}));( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Yystack_overflow_struct* _temp2087=( struct Cyc_Yystack_overflow_struct*)
GC_malloc( sizeof( struct Cyc_Yystack_overflow_struct)); _temp2087[ 0]=({ struct
Cyc_Yystack_overflow_struct _temp2088; _temp2088.tag= Cyc_Yystack_overflow_tag;
_temp2088;}); _temp2087;}));} yystacksize *= 2; if( yystacksize > 10000){
yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp2089=(
unsigned int) yystacksize; short* _temp2090=( short*) GC_malloc_atomic( sizeof(
short) * _temp2089); unsigned int i; struct _tagged_ptr1 _temp2091={ _temp2090,
_temp2090, _temp2090 + _temp2089}; for( i= 0; i < _temp2089; i ++){ _temp2090[ i]=
(short)0;} _temp2091;}); struct _tagged_ptr2 yyvs1=({ unsigned int _temp2092=(
unsigned int) yystacksize; struct _xtunion_struct** _temp2093=( struct
_xtunion_struct**) GC_malloc( sizeof( struct _xtunion_struct*) * _temp2092);
unsigned int i; struct _tagged_ptr2 _temp2094={ _temp2093, _temp2093, _temp2093
+ _temp2092}; for( i= 0; i < _temp2092; i ++){ _temp2093[ i]= Cyc_yylval;}
_temp2094;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2095=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2096=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2095); unsigned int i; struct _tagged_ptr3
_temp2097={ _temp2096, _temp2096, _temp2096 + _temp2095}; for( i= 0; i <
_temp2095; i ++){ _temp2096[ i]= Cyc_yynewloc();} _temp2097;});{ int i= 0; for(
0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1 _temp2098= yyss1; short*
_temp2100= _temp2098.curr + i; if( _temp2100 < _temp2098.base? 1: _temp2100 >=
_temp2098.last_plus_one){ _throw( Null_Exception);}* _temp2100=({ struct
_tagged_ptr1 _temp2101= yyss; short* _temp2103= _temp2101.curr + i; if(
_temp2103 < _temp2101.base? 1: _temp2103 >= _temp2101.last_plus_one){ _throw(
Null_Exception);}* _temp2103;});});({ struct _tagged_ptr2 _temp2104= yyvs1;
struct _xtunion_struct** _temp2106= _temp2104.curr + i; if( _temp2106 <
_temp2104.base? 1: _temp2106 >= _temp2104.last_plus_one){ _throw( Null_Exception);}*
_temp2106=({ struct _tagged_ptr2 _temp2107= yyvs; struct _xtunion_struct**
_temp2109= _temp2107.curr + i; if( _temp2109 < _temp2107.base? 1: _temp2109 >=
_temp2107.last_plus_one){ _throw( Null_Exception);}* _temp2109;});});({ struct
_tagged_ptr3 _temp2110= yyls1; struct Cyc_Yyltype* _temp2112= _temp2110.curr + i;
if( _temp2112 < _temp2110.base? 1: _temp2112 >= _temp2110.last_plus_one){ _throw(
Null_Exception);}* _temp2112=({ struct _tagged_ptr3 _temp2113= yyls; struct Cyc_Yyltype*
_temp2115= _temp2113.curr + i; if( _temp2115 < _temp2113.base? 1: _temp2115 >=
_temp2113.last_plus_one){ _throw( Null_Exception);}* _temp2115;});});}} yyss=
yyss1; yyvs= yyvs1; yyls= yyls1;}} goto yybackup; yybackup: yyn=( int)({ short*
_temp2116=( short*) Cyc_yypact; unsigned int _temp2117= yystate; if( _temp2117
>= 728u){ _throw( Null_Exception);} _temp2116[ _temp2117];}); if( yyn == - 32768){
goto yydefault;} if( Cyc_yychar == - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar
<= 0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <=
347: 0)?( int)({ short* _temp2118=( short*) Cyc_yytranslate; unsigned int
_temp2119= Cyc_yychar; if( _temp2119 >= 348u){ _throw( Null_Exception);}
_temp2118[ _temp2119];}): 227;} yyn += yychar1; if(( yyn < 0? 1: yyn > 3997)? 1:(
int)({ short* _temp2120=( short*) Cyc_yycheck; unsigned int _temp2121= yyn; if(
_temp2121 >= 3998u){ _throw( Null_Exception);} _temp2120[ _temp2121];}) !=
yychar1){ goto yydefault;} yyn=( int)({ short* _temp2122=( short*) Cyc_yytable;
unsigned int _temp2123= yyn; if( _temp2123 >= 3998u){ _throw( Null_Exception);}
_temp2122[ _temp2123];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrlab;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrlab;}} if( yyn == 727){
return 0;} if( Cyc_yychar != 0){ Cyc_yychar= - 2;}({ struct _tagged_ptr2
_temp2124= yyvs; struct _xtunion_struct** _temp2126= _temp2124.curr +( ++
yyvsp_offset); if( _temp2126 < _temp2124.base? 1: _temp2126 >= _temp2124.last_plus_one){
_throw( Null_Exception);}* _temp2126= Cyc_yylval;});({ struct _tagged_ptr3
_temp2127= yyls; struct Cyc_Yyltype* _temp2129= _temp2127.curr +( ++
yylsp_offset); if( _temp2129 < _temp2127.base? 1: _temp2129 >= _temp2127.last_plus_one){
_throw( Null_Exception);}* _temp2129= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp2130=( short*) Cyc_yydefact; unsigned int _temp2131= yystate; if( _temp2131
>= 728u){ _throw( Null_Exception);} _temp2130[ _temp2131];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp2132=( short*) Cyc_yyr2;
unsigned int _temp2133= yyn; if( _temp2133 >= 389u){ _throw( Null_Exception);}
_temp2132[ _temp2133];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp2134=
yyvs; struct _xtunion_struct** _temp2136= _temp2134.curr +(( yyvsp_offset + 1) -
yylen); if( _temp2136 < _temp2134.base? 1: _temp2136 >= _temp2134.last_plus_one){
_throw( Null_Exception);}* _temp2136;});} switch( yyn){ case 1: _LL2137: yyval=({
struct _tagged_ptr2 _temp2139= yyvs; struct _xtunion_struct** _temp2141=
_temp2139.curr + yyvsp_offset; if( _temp2141 < _temp2139.base? 1: _temp2141 >=
_temp2139.last_plus_one){ _throw( Null_Exception);}* _temp2141;}); Cyc_Parse_parse_result=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2142= yyvs; struct
_xtunion_struct** _temp2144= _temp2142.curr + yyvsp_offset; if( _temp2144 <
_temp2142.base? 1: _temp2144 >= _temp2142.last_plus_one){ _throw( Null_Exception);}*
_temp2144;})); break; case 2: _LL2138: yyval=({ struct _tagged_ptr2 _temp2146=
yyvs; struct _xtunion_struct** _temp2148= _temp2146.curr + yyvsp_offset; if(
_temp2148 < _temp2146.base? 1: _temp2148 >= _temp2146.last_plus_one){ _throw(
Null_Exception);}* _temp2148;}); break; case 3: _LL2145: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2150=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2150[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2151; _temp2151.tag= Cyc_DeclList_tok_tag; _temp2151.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2152= yyvs; struct _xtunion_struct** _temp2154=
_temp2152.curr +( yyvsp_offset - 1); if( _temp2154 < _temp2152.base? 1:
_temp2154 >= _temp2152.last_plus_one){ _throw( Null_Exception);}* _temp2154;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2155= yyvs; struct
_xtunion_struct** _temp2157= _temp2155.curr + yyvsp_offset; if( _temp2157 <
_temp2155.base? 1: _temp2157 >= _temp2155.last_plus_one){ _throw( Null_Exception);}*
_temp2157;}))); _temp2151;}); _temp2150;}); break; case 4: _LL2149: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2159=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2159[ 0]=({ struct Cyc_DeclList_tok_struct _temp2160; _temp2160.tag= Cyc_DeclList_tok_tag;
_temp2160.f1=({ struct Cyc_List_List* _temp2161=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2161->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2162=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2162->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2169=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2169[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2170; _temp2170.tag= Cyc_Absyn_Using_d_tag;
_temp2170.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2171= yyvs;
struct _xtunion_struct** _temp2173= _temp2171.curr +( yyvsp_offset - 2); if(
_temp2173 < _temp2171.base? 1: _temp2173 >= _temp2171.last_plus_one){ _throw(
Null_Exception);}* _temp2173;})); _temp2170.f2= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2174= yyvs; struct _xtunion_struct** _temp2176= _temp2174.curr
+ yyvsp_offset; if( _temp2176 < _temp2174.base? 1: _temp2176 >= _temp2174.last_plus_one){
_throw( Null_Exception);}* _temp2176;})); _temp2170;}); _temp2169;})); _temp2162->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2163= yyls; struct Cyc_Yyltype*
_temp2165= _temp2163.curr +( yylsp_offset - 2); if( _temp2165 < _temp2163.base?
1: _temp2165 >= _temp2163.last_plus_one){ _throw( Null_Exception);}* _temp2165;}).first_line,({
struct _tagged_ptr3 _temp2166= yyls; struct Cyc_Yyltype* _temp2168= _temp2166.curr
+ yylsp_offset; if( _temp2168 < _temp2166.base? 1: _temp2168 >= _temp2166.last_plus_one){
_throw( Null_Exception);}* _temp2168;}).last_line); _temp2162;}); _temp2161->tl=
0; _temp2161;}); _temp2160;}); _temp2159;}); Cyc_Lex_leave_using(); break; case
5: _LL2158: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2178=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2178[ 0]=({ struct Cyc_DeclList_tok_struct _temp2179; _temp2179.tag= Cyc_DeclList_tok_tag;
_temp2179.f1=({ struct Cyc_List_List* _temp2180=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2180->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2184=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2184->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2191=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2191[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2192; _temp2192.tag= Cyc_Absyn_Using_d_tag;
_temp2192.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2193= yyvs;
struct _xtunion_struct** _temp2195= _temp2193.curr +( yyvsp_offset - 4); if(
_temp2195 < _temp2193.base? 1: _temp2195 >= _temp2193.last_plus_one){ _throw(
Null_Exception);}* _temp2195;})); _temp2192.f2= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2196= yyvs; struct _xtunion_struct** _temp2198= _temp2196.curr
+( yyvsp_offset - 2); if( _temp2198 < _temp2196.base? 1: _temp2198 >= _temp2196.last_plus_one){
_throw( Null_Exception);}* _temp2198;})); _temp2192;}); _temp2191;})); _temp2184->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2185= yyls; struct Cyc_Yyltype*
_temp2187= _temp2185.curr +( yylsp_offset - 4); if( _temp2187 < _temp2185.base?
1: _temp2187 >= _temp2185.last_plus_one){ _throw( Null_Exception);}* _temp2187;}).first_line,({
struct _tagged_ptr3 _temp2188= yyls; struct Cyc_Yyltype* _temp2190= _temp2188.curr
+( yylsp_offset - 1); if( _temp2190 < _temp2188.base? 1: _temp2190 >= _temp2188.last_plus_one){
_throw( Null_Exception);}* _temp2190;}).last_line); _temp2184;}); _temp2180->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2181= yyvs; struct
_xtunion_struct** _temp2183= _temp2181.curr + yyvsp_offset; if( _temp2183 <
_temp2181.base? 1: _temp2183 >= _temp2181.last_plus_one){ _throw( Null_Exception);}*
_temp2183;})); _temp2180;}); _temp2179;}); _temp2178;}); break; case 6: _LL2177:
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2200=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2200[ 0]=({ struct Cyc_DeclList_tok_struct _temp2201; _temp2201.tag= Cyc_DeclList_tok_tag;
_temp2201.f1=({ struct Cyc_List_List* _temp2202=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2202->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2203=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2203->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2210=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2210[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2211; _temp2211.tag=
Cyc_Absyn_Namespace_d_tag; _temp2211.f1=({ struct _tagged_string* _temp2212=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2212[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2213= yyvs;
struct _xtunion_struct** _temp2215= _temp2213.curr +( yyvsp_offset - 2); if(
_temp2215 < _temp2213.base? 1: _temp2215 >= _temp2213.last_plus_one){ _throw(
Null_Exception);}* _temp2215;})); _temp2212;}); _temp2211.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2216= yyvs; struct _xtunion_struct** _temp2218=
_temp2216.curr + yyvsp_offset; if( _temp2218 < _temp2216.base? 1: _temp2218 >=
_temp2216.last_plus_one){ _throw( Null_Exception);}* _temp2218;})); _temp2211;});
_temp2210;})); _temp2203->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2204= yyls; struct Cyc_Yyltype* _temp2206= _temp2204.curr +(
yylsp_offset - 2); if( _temp2206 < _temp2204.base? 1: _temp2206 >= _temp2204.last_plus_one){
_throw( Null_Exception);}* _temp2206;}).first_line,({ struct _tagged_ptr3
_temp2207= yyls; struct Cyc_Yyltype* _temp2209= _temp2207.curr + yylsp_offset;
if( _temp2209 < _temp2207.base? 1: _temp2209 >= _temp2207.last_plus_one){ _throw(
Null_Exception);}* _temp2209;}).last_line); _temp2203;}); _temp2202->tl= 0;
_temp2202;}); _temp2201;}); _temp2200;}); Cyc_Lex_leave_namespace(); break; case
7: _LL2199: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2220=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2220[ 0]=({ struct Cyc_DeclList_tok_struct _temp2221; _temp2221.tag= Cyc_DeclList_tok_tag;
_temp2221.f1=({ struct Cyc_List_List* _temp2222=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2222->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2226=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2226->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2233=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2233[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2234; _temp2234.tag=
Cyc_Absyn_Namespace_d_tag; _temp2234.f1=({ struct _tagged_string* _temp2235=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2235[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2236= yyvs;
struct _xtunion_struct** _temp2238= _temp2236.curr +( yyvsp_offset - 4); if(
_temp2238 < _temp2236.base? 1: _temp2238 >= _temp2236.last_plus_one){ _throw(
Null_Exception);}* _temp2238;})); _temp2235;}); _temp2234.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2239= yyvs; struct _xtunion_struct** _temp2241=
_temp2239.curr +( yyvsp_offset - 2); if( _temp2241 < _temp2239.base? 1:
_temp2241 >= _temp2239.last_plus_one){ _throw( Null_Exception);}* _temp2241;}));
_temp2234;}); _temp2233;})); _temp2226->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2227= yyls; struct Cyc_Yyltype* _temp2229= _temp2227.curr
+( yylsp_offset - 4); if( _temp2229 < _temp2227.base? 1: _temp2229 >= _temp2227.last_plus_one){
_throw( Null_Exception);}* _temp2229;}).first_line,({ struct _tagged_ptr3
_temp2230= yyls; struct Cyc_Yyltype* _temp2232= _temp2230.curr +( yylsp_offset -
1); if( _temp2232 < _temp2230.base? 1: _temp2232 >= _temp2230.last_plus_one){
_throw( Null_Exception);}* _temp2232;}).last_line); _temp2226;}); _temp2222->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2223= yyvs; struct
_xtunion_struct** _temp2225= _temp2223.curr + yyvsp_offset; if( _temp2225 <
_temp2223.base? 1: _temp2225 >= _temp2223.last_plus_one){ _throw( Null_Exception);}*
_temp2225;})); _temp2222;}); _temp2221;}); _temp2220;}); break; case 8: _LL2219:
if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2243=
yyvs; struct _xtunion_struct** _temp2245= _temp2243.curr +( yyvsp_offset - 4);
if( _temp2245 < _temp2243.base? 1: _temp2245 >= _temp2243.last_plus_one){ _throw(
Null_Exception);}* _temp2245;})),( struct _tagged_string)({ char* _temp2246=(
char*)"C"; struct _tagged_string _temp2247; _temp2247.curr= _temp2246; _temp2247.base=
_temp2246; _temp2247.last_plus_one= _temp2246 + 2; _temp2247;})) != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2248=( char*)"only extern \"C\" { ... } is allowed";
struct _tagged_string _temp2249; _temp2249.curr= _temp2248; _temp2249.base=
_temp2248; _temp2249.last_plus_one= _temp2248 + 35; _temp2249;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2250= yyls; struct Cyc_Yyltype* _temp2252= _temp2250.curr
+( yylsp_offset - 5); if( _temp2252 < _temp2250.base? 1: _temp2252 >= _temp2250.last_plus_one){
_throw( Null_Exception);}* _temp2252;}).first_line,({ struct _tagged_ptr3
_temp2253= yyls; struct Cyc_Yyltype* _temp2255= _temp2253.curr +( yylsp_offset -
4); if( _temp2255 < _temp2253.base? 1: _temp2255 >= _temp2253.last_plus_one){
_throw( Null_Exception);}* _temp2255;}).last_line));} yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2256=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2256[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2257; _temp2257.tag= Cyc_DeclList_tok_tag; _temp2257.f1=({ struct Cyc_List_List*
_temp2258=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2258->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2262=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2262->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp2269=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp2269[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp2270; _temp2270.tag= Cyc_Absyn_ExternC_d_tag;
_temp2270.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2271= yyvs;
struct _xtunion_struct** _temp2273= _temp2271.curr +( yyvsp_offset - 2); if(
_temp2273 < _temp2271.base? 1: _temp2273 >= _temp2271.last_plus_one){ _throw(
Null_Exception);}* _temp2273;})); _temp2270;}); _temp2269;})); _temp2262->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2263= yyls; struct Cyc_Yyltype*
_temp2265= _temp2263.curr +( yylsp_offset - 5); if( _temp2265 < _temp2263.base?
1: _temp2265 >= _temp2263.last_plus_one){ _throw( Null_Exception);}* _temp2265;}).first_line,({
struct _tagged_ptr3 _temp2266= yyls; struct Cyc_Yyltype* _temp2268= _temp2266.curr
+( yylsp_offset - 1); if( _temp2268 < _temp2266.base? 1: _temp2268 >= _temp2266.last_plus_one){
_throw( Null_Exception);}* _temp2268;}).last_line); _temp2262;}); _temp2258->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2259= yyvs; struct
_xtunion_struct** _temp2261= _temp2259.curr + yyvsp_offset; if( _temp2261 <
_temp2259.base? 1: _temp2261 >= _temp2259.last_plus_one){ _throw( Null_Exception);}*
_temp2261;})); _temp2258;}); _temp2257;}); _temp2256;}); break; case 9: _LL2242:
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2275=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2275[ 0]=({ struct Cyc_DeclList_tok_struct _temp2276; _temp2276.tag= Cyc_DeclList_tok_tag;
_temp2276.f1= 0; _temp2276;}); _temp2275;}); break; case 10: _LL2274: yyval=({
struct _tagged_ptr2 _temp2278= yyvs; struct _xtunion_struct** _temp2280=
_temp2278.curr + yyvsp_offset; if( _temp2280 < _temp2278.base? 1: _temp2280 >=
_temp2278.last_plus_one){ _throw( Null_Exception);}* _temp2280;}); break; case
11: _LL2277: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2282=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2282[ 0]=({ struct Cyc_DeclList_tok_struct _temp2283; _temp2283.tag= Cyc_DeclList_tok_tag;
_temp2283.f1=({ struct Cyc_List_List* _temp2284=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2284->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2285=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp2285[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp2286; _temp2286.tag= Cyc_Absyn_Fn_d_tag; _temp2286.f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp2287= yyvs; struct _xtunion_struct** _temp2289=
_temp2287.curr + yyvsp_offset; if( _temp2289 < _temp2287.base? 1: _temp2289 >=
_temp2287.last_plus_one){ _throw( Null_Exception);}* _temp2289;})); _temp2286;});
_temp2285;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2290= yyls;
struct Cyc_Yyltype* _temp2292= _temp2290.curr + yylsp_offset; if( _temp2292 <
_temp2290.base? 1: _temp2292 >= _temp2290.last_plus_one){ _throw( Null_Exception);}*
_temp2292;}).first_line,({ struct _tagged_ptr3 _temp2293= yyls; struct Cyc_Yyltype*
_temp2295= _temp2293.curr + yylsp_offset; if( _temp2295 < _temp2293.base? 1:
_temp2295 >= _temp2293.last_plus_one){ _throw( Null_Exception);}* _temp2295;}).last_line));
_temp2284->tl= 0; _temp2284;}); _temp2283;}); _temp2282;}); break; case 12:
_LL2281: yyval=({ struct _tagged_ptr2 _temp2297= yyvs; struct _xtunion_struct**
_temp2299= _temp2297.curr + yyvsp_offset; if( _temp2299 < _temp2297.base? 1:
_temp2299 >= _temp2297.last_plus_one){ _throw( Null_Exception);}* _temp2299;});
break; case 13: _LL2296: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2301=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2301[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2302; _temp2302.tag= Cyc_FnDecl_tok_tag;
_temp2302.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2303= yyvs; struct _xtunion_struct** _temp2305= _temp2303.curr
+( yyvsp_offset - 1); if( _temp2305 < _temp2303.base? 1: _temp2305 >= _temp2303.last_plus_one){
_throw( Null_Exception);}* _temp2305;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2306= yyvs; struct _xtunion_struct** _temp2308= _temp2306.curr
+ yyvsp_offset; if( _temp2308 < _temp2306.base? 1: _temp2308 >= _temp2306.last_plus_one){
_throw( Null_Exception);}* _temp2308;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2309= yyls; struct Cyc_Yyltype* _temp2311= _temp2309.curr +(
yylsp_offset - 1); if( _temp2311 < _temp2309.base? 1: _temp2311 >= _temp2309.last_plus_one){
_throw( Null_Exception);}* _temp2311;}).first_line,({ struct _tagged_ptr3
_temp2312= yyls; struct Cyc_Yyltype* _temp2314= _temp2312.curr + yylsp_offset;
if( _temp2314 < _temp2312.base? 1: _temp2314 >= _temp2312.last_plus_one){ _throw(
Null_Exception);}* _temp2314;}).last_line)); _temp2302;}); _temp2301;}); break;
case 14: _LL2300: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2316=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2316[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2317; _temp2317.tag= Cyc_FnDecl_tok_tag;
_temp2317.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2318=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2318->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2319= yyvs; struct
_xtunion_struct** _temp2321= _temp2319.curr +( yyvsp_offset - 2); if( _temp2321
< _temp2319.base? 1: _temp2321 >= _temp2319.last_plus_one){ _throw(
Null_Exception);}* _temp2321;})); _temp2318;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2322= yyvs; struct _xtunion_struct** _temp2324=
_temp2322.curr +( yyvsp_offset - 1); if( _temp2324 < _temp2322.base? 1:
_temp2324 >= _temp2322.last_plus_one){ _throw( Null_Exception);}* _temp2324;})),
0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2325= yyvs; struct
_xtunion_struct** _temp2327= _temp2325.curr + yyvsp_offset; if( _temp2327 <
_temp2325.base? 1: _temp2327 >= _temp2325.last_plus_one){ _throw( Null_Exception);}*
_temp2327;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2328=
yyls; struct Cyc_Yyltype* _temp2330= _temp2328.curr +( yylsp_offset - 2); if(
_temp2330 < _temp2328.base? 1: _temp2330 >= _temp2328.last_plus_one){ _throw(
Null_Exception);}* _temp2330;}).first_line,({ struct _tagged_ptr3 _temp2331=
yyls; struct Cyc_Yyltype* _temp2333= _temp2331.curr + yylsp_offset; if(
_temp2333 < _temp2331.base? 1: _temp2333 >= _temp2331.last_plus_one){ _throw(
Null_Exception);}* _temp2333;}).last_line)); _temp2317;}); _temp2316;}); break;
case 15: _LL2315: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2335=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2335[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2336; _temp2336.tag= Cyc_FnDecl_tok_tag;
_temp2336.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2337= yyvs; struct _xtunion_struct** _temp2339= _temp2337.curr
+( yyvsp_offset - 2); if( _temp2339 < _temp2337.base? 1: _temp2339 >= _temp2337.last_plus_one){
_throw( Null_Exception);}* _temp2339;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2340= yyvs; struct _xtunion_struct** _temp2342= _temp2340.curr
+( yyvsp_offset - 1); if( _temp2342 < _temp2340.base? 1: _temp2342 >= _temp2340.last_plus_one){
_throw( Null_Exception);}* _temp2342;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2343= yyvs; struct _xtunion_struct** _temp2345= _temp2343.curr
+ yyvsp_offset; if( _temp2345 < _temp2343.base? 1: _temp2345 >= _temp2343.last_plus_one){
_throw( Null_Exception);}* _temp2345;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2346= yyls; struct Cyc_Yyltype* _temp2348= _temp2346.curr +(
yylsp_offset - 2); if( _temp2348 < _temp2346.base? 1: _temp2348 >= _temp2346.last_plus_one){
_throw( Null_Exception);}* _temp2348;}).first_line,({ struct _tagged_ptr3
_temp2349= yyls; struct Cyc_Yyltype* _temp2351= _temp2349.curr + yylsp_offset;
if( _temp2351 < _temp2349.base? 1: _temp2351 >= _temp2349.last_plus_one){ _throw(
Null_Exception);}* _temp2351;}).last_line)); _temp2336;}); _temp2335;}); break;
case 16: _LL2334: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2353=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2353[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2354; _temp2354.tag= Cyc_FnDecl_tok_tag;
_temp2354.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2355=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2355->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2356= yyvs; struct
_xtunion_struct** _temp2358= _temp2356.curr +( yyvsp_offset - 3); if( _temp2358
< _temp2356.base? 1: _temp2358 >= _temp2356.last_plus_one){ _throw(
Null_Exception);}* _temp2358;})); _temp2355;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2359= yyvs; struct _xtunion_struct** _temp2361=
_temp2359.curr +( yyvsp_offset - 2); if( _temp2361 < _temp2359.base? 1:
_temp2361 >= _temp2359.last_plus_one){ _throw( Null_Exception);}* _temp2361;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2362= yyvs; struct
_xtunion_struct** _temp2364= _temp2362.curr +( yyvsp_offset - 1); if( _temp2364
< _temp2362.base? 1: _temp2364 >= _temp2362.last_plus_one){ _throw(
Null_Exception);}* _temp2364;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2365= yyvs; struct _xtunion_struct** _temp2367= _temp2365.curr +
yyvsp_offset; if( _temp2367 < _temp2365.base? 1: _temp2367 >= _temp2365.last_plus_one){
_throw( Null_Exception);}* _temp2367;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2368= yyls; struct Cyc_Yyltype* _temp2370= _temp2368.curr +(
yylsp_offset - 3); if( _temp2370 < _temp2368.base? 1: _temp2370 >= _temp2368.last_plus_one){
_throw( Null_Exception);}* _temp2370;}).first_line,({ struct _tagged_ptr3
_temp2371= yyls; struct Cyc_Yyltype* _temp2373= _temp2371.curr + yylsp_offset;
if( _temp2373 < _temp2371.base? 1: _temp2373 >= _temp2371.last_plus_one){ _throw(
Null_Exception);}* _temp2373;}).last_line)); _temp2354;}); _temp2353;}); break;
case 17: _LL2352: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2375=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2375[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2376; _temp2376.tag= Cyc_FnDecl_tok_tag;
_temp2376.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2377=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2377->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2378= yyvs; struct
_xtunion_struct** _temp2380= _temp2378.curr +( yyvsp_offset - 2); if( _temp2380
< _temp2378.base? 1: _temp2380 >= _temp2378.last_plus_one){ _throw(
Null_Exception);}* _temp2380;})); _temp2377;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2381= yyvs; struct _xtunion_struct** _temp2383=
_temp2381.curr +( yyvsp_offset - 1); if( _temp2383 < _temp2381.base? 1:
_temp2383 >= _temp2381.last_plus_one){ _throw( Null_Exception);}* _temp2383;})),
0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2384= yyvs; struct
_xtunion_struct** _temp2386= _temp2384.curr + yyvsp_offset; if( _temp2386 <
_temp2384.base? 1: _temp2386 >= _temp2384.last_plus_one){ _throw( Null_Exception);}*
_temp2386;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2387=
yyls; struct Cyc_Yyltype* _temp2389= _temp2387.curr +( yylsp_offset - 2); if(
_temp2389 < _temp2387.base? 1: _temp2389 >= _temp2387.last_plus_one){ _throw(
Null_Exception);}* _temp2389;}).first_line,({ struct _tagged_ptr3 _temp2390=
yyls; struct Cyc_Yyltype* _temp2392= _temp2390.curr + yylsp_offset; if(
_temp2392 < _temp2390.base? 1: _temp2392 >= _temp2390.last_plus_one){ _throw(
Null_Exception);}* _temp2392;}).last_line)); _temp2376;}); _temp2375;}); break;
case 18: _LL2374: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2394=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2394[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2395; _temp2395.tag= Cyc_FnDecl_tok_tag;
_temp2395.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2396=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2396->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2397= yyvs; struct
_xtunion_struct** _temp2399= _temp2397.curr +( yyvsp_offset - 3); if( _temp2399
< _temp2397.base? 1: _temp2399 >= _temp2397.last_plus_one){ _throw(
Null_Exception);}* _temp2399;})); _temp2396;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2400= yyvs; struct _xtunion_struct** _temp2402=
_temp2400.curr +( yyvsp_offset - 2); if( _temp2402 < _temp2400.base? 1:
_temp2402 >= _temp2400.last_plus_one){ _throw( Null_Exception);}* _temp2402;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2403= yyvs; struct
_xtunion_struct** _temp2405= _temp2403.curr +( yyvsp_offset - 1); if( _temp2405
< _temp2403.base? 1: _temp2405 >= _temp2403.last_plus_one){ _throw(
Null_Exception);}* _temp2405;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2406= yyvs; struct _xtunion_struct** _temp2408= _temp2406.curr +
yyvsp_offset; if( _temp2408 < _temp2406.base? 1: _temp2408 >= _temp2406.last_plus_one){
_throw( Null_Exception);}* _temp2408;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2409= yyls; struct Cyc_Yyltype* _temp2411= _temp2409.curr +(
yylsp_offset - 3); if( _temp2411 < _temp2409.base? 1: _temp2411 >= _temp2409.last_plus_one){
_throw( Null_Exception);}* _temp2411;}).first_line,({ struct _tagged_ptr3
_temp2412= yyls; struct Cyc_Yyltype* _temp2414= _temp2412.curr + yylsp_offset;
if( _temp2414 < _temp2412.base? 1: _temp2414 >= _temp2412.last_plus_one){ _throw(
Null_Exception);}* _temp2414;}).last_line)); _temp2395;}); _temp2394;}); break;
case 19: _LL2393: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2416= yyvs; struct _xtunion_struct** _temp2418= _temp2416.curr
+ yyvsp_offset; if( _temp2418 < _temp2416.base? 1: _temp2418 >= _temp2416.last_plus_one){
_throw( Null_Exception);}* _temp2418;}))); yyval=({ struct _tagged_ptr2
_temp2419= yyvs; struct _xtunion_struct** _temp2421= _temp2419.curr +
yyvsp_offset; if( _temp2421 < _temp2419.base? 1: _temp2421 >= _temp2419.last_plus_one){
_throw( Null_Exception);}* _temp2421;}); break; case 20: _LL2415: Cyc_Lex_leave_using();
break; case 21: _LL2422: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2424=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2424[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2425= yyvs;
struct _xtunion_struct** _temp2427= _temp2425.curr + yyvsp_offset; if( _temp2427
< _temp2425.base? 1: _temp2427 >= _temp2425.last_plus_one){ _throw(
Null_Exception);}* _temp2427;})); _temp2424;})); yyval=({ struct _tagged_ptr2
_temp2428= yyvs; struct _xtunion_struct** _temp2430= _temp2428.curr +
yyvsp_offset; if( _temp2430 < _temp2428.base? 1: _temp2430 >= _temp2428.last_plus_one){
_throw( Null_Exception);}* _temp2430;}); break; case 22: _LL2423: Cyc_Lex_leave_namespace();
break; case 23: _LL2431: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2433=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2433[ 0]=({ struct Cyc_DeclList_tok_struct _temp2434; _temp2434.tag= Cyc_DeclList_tok_tag;
_temp2434.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2435= yyvs; struct _xtunion_struct** _temp2437= _temp2435.curr
+( yyvsp_offset - 1); if( _temp2437 < _temp2435.base? 1: _temp2437 >= _temp2435.last_plus_one){
_throw( Null_Exception);}* _temp2437;})), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2438= yyls; struct Cyc_Yyltype* _temp2440= _temp2438.curr
+( yylsp_offset - 1); if( _temp2440 < _temp2438.base? 1: _temp2440 >= _temp2438.last_plus_one){
_throw( Null_Exception);}* _temp2440;}).first_line,({ struct _tagged_ptr3
_temp2441= yyls; struct Cyc_Yyltype* _temp2443= _temp2441.curr +( yylsp_offset -
1); if( _temp2443 < _temp2441.base? 1: _temp2443 >= _temp2441.last_plus_one){
_throw( Null_Exception);}* _temp2443;}).last_line)); _temp2434;}); _temp2433;});
break; case 24: _LL2432: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2445=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2445[ 0]=({ struct Cyc_DeclList_tok_struct _temp2446; _temp2446.tag= Cyc_DeclList_tok_tag;
_temp2446.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2447= yyvs; struct _xtunion_struct** _temp2449= _temp2447.curr
+( yyvsp_offset - 2); if( _temp2449 < _temp2447.base? 1: _temp2449 >= _temp2447.last_plus_one){
_throw( Null_Exception);}* _temp2449;})), Cyc_yyget_InitDeclList_tok(({ struct
_tagged_ptr2 _temp2450= yyvs; struct _xtunion_struct** _temp2452= _temp2450.curr
+( yyvsp_offset - 1); if( _temp2452 < _temp2450.base? 1: _temp2452 >= _temp2450.last_plus_one){
_throw( Null_Exception);}* _temp2452;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2453= yyls; struct Cyc_Yyltype* _temp2455= _temp2453.curr +(
yylsp_offset - 2); if( _temp2455 < _temp2453.base? 1: _temp2455 >= _temp2453.last_plus_one){
_throw( Null_Exception);}* _temp2455;}).first_line,({ struct _tagged_ptr3
_temp2456= yyls; struct Cyc_Yyltype* _temp2458= _temp2456.curr + yylsp_offset;
if( _temp2458 < _temp2456.base? 1: _temp2458 >= _temp2456.last_plus_one){ _throw(
Null_Exception);}* _temp2458;}).last_line)); _temp2446;}); _temp2445;}); break;
case 25: _LL2444: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2460=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2460[ 0]=({ struct Cyc_DeclList_tok_struct _temp2461; _temp2461.tag= Cyc_DeclList_tok_tag;
_temp2461.f1=({ struct Cyc_List_List* _temp2462=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2462->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp2463= yyvs; struct
_xtunion_struct** _temp2465= _temp2463.curr +( yyvsp_offset - 3); if( _temp2465
< _temp2463.base? 1: _temp2465 >= _temp2463.last_plus_one){ _throw(
Null_Exception);}* _temp2465;})), 0, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp2466= yyvs; struct _xtunion_struct** _temp2468= _temp2466.curr +(
yyvsp_offset - 1); if( _temp2468 < _temp2466.base? 1: _temp2468 >= _temp2466.last_plus_one){
_throw( Null_Exception);}* _temp2468;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2469= yyls; struct Cyc_Yyltype* _temp2471= _temp2469.curr +(
yylsp_offset - 4); if( _temp2471 < _temp2469.base? 1: _temp2471 >= _temp2469.last_plus_one){
_throw( Null_Exception);}* _temp2471;}).first_line,({ struct _tagged_ptr3
_temp2472= yyls; struct Cyc_Yyltype* _temp2474= _temp2472.curr + yylsp_offset;
if( _temp2474 < _temp2472.base? 1: _temp2474 >= _temp2472.last_plus_one){ _throw(
Null_Exception);}* _temp2474;}).last_line)); _temp2462->tl= 0; _temp2462;});
_temp2461;}); _temp2460;}); break; case 26: _LL2459: yyval=({ struct
_tagged_ptr2 _temp2476= yyvs; struct _xtunion_struct** _temp2478= _temp2476.curr
+ yyvsp_offset; if( _temp2478 < _temp2476.base? 1: _temp2478 >= _temp2476.last_plus_one){
_throw( Null_Exception);}* _temp2478;}); break; case 27: _LL2475: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2480=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2480[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2481; _temp2481.tag= Cyc_DeclList_tok_tag; _temp2481.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2482= yyvs; struct _xtunion_struct** _temp2484=
_temp2482.curr +( yyvsp_offset - 1); if( _temp2484 < _temp2482.base? 1:
_temp2484 >= _temp2482.last_plus_one){ _throw( Null_Exception);}* _temp2484;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2485= yyvs; struct
_xtunion_struct** _temp2487= _temp2485.curr + yyvsp_offset; if( _temp2487 <
_temp2485.base? 1: _temp2487 >= _temp2485.last_plus_one){ _throw( Null_Exception);}*
_temp2487;}))); _temp2481;}); _temp2480;}); break; case 28: _LL2479: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2489=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2489[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2490; _temp2490.tag= Cyc_DeclSpec_tok_tag;
_temp2490.f1=({ struct Cyc_Parse_Declaration_spec* _temp2491=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2491->sc=({ struct
Cyc_Core_Opt* _temp2495=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2495->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2496= yyvs; struct _xtunion_struct** _temp2498= _temp2496.curr +(
yyvsp_offset - 1); if( _temp2498 < _temp2496.base? 1: _temp2498 >= _temp2496.last_plus_one){
_throw( Null_Exception);}* _temp2498;})); _temp2495;}); _temp2491->tq= Cyc_Absyn_empty_tqual();
_temp2491->type_specs= 0; _temp2491->is_inline= 0; _temp2491->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2492= yyvs; struct _xtunion_struct** _temp2494=
_temp2492.curr + yyvsp_offset; if( _temp2494 < _temp2492.base? 1: _temp2494 >=
_temp2492.last_plus_one){ _throw( Null_Exception);}* _temp2494;})); _temp2491;});
_temp2490;}); _temp2489;}); break; case 29: _LL2488: if(( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2500= yyvs; struct _xtunion_struct** _temp2502=
_temp2500.curr + yyvsp_offset; if( _temp2502 < _temp2500.base? 1: _temp2502 >=
_temp2500.last_plus_one){ _throw( Null_Exception);}* _temp2502;})))->sc != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp2503=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2504; _temp2504.curr= _temp2503; _temp2504.base=
_temp2503; _temp2504.last_plus_one= _temp2503 + 51; _temp2504;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2505= yyls; struct Cyc_Yyltype* _temp2507= _temp2505.curr
+( yylsp_offset - 2); if( _temp2507 < _temp2505.base? 1: _temp2507 >= _temp2505.last_plus_one){
_throw( Null_Exception);}* _temp2507;}).first_line,({ struct _tagged_ptr3
_temp2508= yyls; struct Cyc_Yyltype* _temp2510= _temp2508.curr +( yylsp_offset -
1); if( _temp2510 < _temp2508.base? 1: _temp2510 >= _temp2508.last_plus_one){
_throw( Null_Exception);}* _temp2510;}).last_line));} yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2511=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2511[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2512; _temp2512.tag= Cyc_DeclSpec_tok_tag; _temp2512.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2513=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2513->sc=({ struct Cyc_Core_Opt* _temp2529=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2529->v=( void*) Cyc_yyget_StorageClass_tok(({
struct _tagged_ptr2 _temp2530= yyvs; struct _xtunion_struct** _temp2532=
_temp2530.curr +( yyvsp_offset - 2); if( _temp2532 < _temp2530.base? 1:
_temp2532 >= _temp2530.last_plus_one){ _throw( Null_Exception);}* _temp2532;}));
_temp2529;}); _temp2513->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2526= yyvs; struct _xtunion_struct** _temp2528= _temp2526.curr +
yyvsp_offset; if( _temp2528 < _temp2526.base? 1: _temp2528 >= _temp2526.last_plus_one){
_throw( Null_Exception);}* _temp2528;})))->tq; _temp2513->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2523= yyvs; struct _xtunion_struct** _temp2525=
_temp2523.curr + yyvsp_offset; if( _temp2525 < _temp2523.base? 1: _temp2525 >=
_temp2523.last_plus_one){ _throw( Null_Exception);}* _temp2525;})))->type_specs;
_temp2513->is_inline=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2520=
yyvs; struct _xtunion_struct** _temp2522= _temp2520.curr + yyvsp_offset; if(
_temp2522 < _temp2520.base? 1: _temp2522 >= _temp2520.last_plus_one){ _throw(
Null_Exception);}* _temp2522;})))->is_inline; _temp2513->attributes=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2514= yyvs; struct
_xtunion_struct** _temp2516= _temp2514.curr +( yyvsp_offset - 1); if( _temp2516
< _temp2514.base? 1: _temp2516 >= _temp2514.last_plus_one){ _throw(
Null_Exception);}* _temp2516;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2517= yyvs; struct _xtunion_struct** _temp2519= _temp2517.curr +
yyvsp_offset; if( _temp2519 < _temp2517.base? 1: _temp2519 >= _temp2517.last_plus_one){
_throw( Null_Exception);}* _temp2519;})))->attributes); _temp2513;}); _temp2512;});
_temp2511;}); break; case 30: _LL2499: yyval=( struct _xtunion_struct*)({ struct
Cyc_DeclSpec_tok_struct* _temp2534=( struct Cyc_DeclSpec_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2534[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2535; _temp2535.tag= Cyc_DeclSpec_tok_tag; _temp2535.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2536=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2536->sc= 0; _temp2536->tq= Cyc_Absyn_empty_tqual(); _temp2536->type_specs=({
struct Cyc_List_List* _temp2540=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2540->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp2541= yyvs; struct _xtunion_struct** _temp2543=
_temp2541.curr +( yyvsp_offset - 1); if( _temp2543 < _temp2541.base? 1:
_temp2543 >= _temp2541.last_plus_one){ _throw( Null_Exception);}* _temp2543;}));
_temp2540->tl= 0; _temp2540;}); _temp2536->is_inline= 0; _temp2536->attributes=
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2537= yyvs; struct
_xtunion_struct** _temp2539= _temp2537.curr + yyvsp_offset; if( _temp2539 <
_temp2537.base? 1: _temp2539 >= _temp2537.last_plus_one){ _throw( Null_Exception);}*
_temp2539;})); _temp2536;}); _temp2535;}); _temp2534;}); break; case 31: _LL2533:
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2545=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2545[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2546; _temp2546.tag= Cyc_DeclSpec_tok_tag;
_temp2546.f1=({ struct Cyc_Parse_Declaration_spec* _temp2547=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2547->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2567= yyvs; struct _xtunion_struct** _temp2569=
_temp2567.curr + yyvsp_offset; if( _temp2569 < _temp2567.base? 1: _temp2569 >=
_temp2567.last_plus_one){ _throw( Null_Exception);}* _temp2569;})))->sc;
_temp2547->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2564= yyvs;
struct _xtunion_struct** _temp2566= _temp2564.curr + yyvsp_offset; if( _temp2566
< _temp2564.base? 1: _temp2566 >= _temp2564.last_plus_one){ _throw(
Null_Exception);}* _temp2566;})))->tq; _temp2547->type_specs=({ struct Cyc_List_List*
_temp2557=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2557->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2561= yyvs; struct _xtunion_struct** _temp2563= _temp2561.curr +(
yyvsp_offset - 2); if( _temp2563 < _temp2561.base? 1: _temp2563 >= _temp2561.last_plus_one){
_throw( Null_Exception);}* _temp2563;})); _temp2557->tl=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2558= yyvs; struct _xtunion_struct** _temp2560=
_temp2558.curr + yyvsp_offset; if( _temp2560 < _temp2558.base? 1: _temp2560 >=
_temp2558.last_plus_one){ _throw( Null_Exception);}* _temp2560;})))->type_specs;
_temp2557;}); _temp2547->is_inline=( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2554= yyvs; struct _xtunion_struct** _temp2556= _temp2554.curr
+ yyvsp_offset; if( _temp2556 < _temp2554.base? 1: _temp2556 >= _temp2554.last_plus_one){
_throw( Null_Exception);}* _temp2556;})))->is_inline; _temp2547->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2548= yyvs; struct
_xtunion_struct** _temp2550= _temp2548.curr +( yyvsp_offset - 1); if( _temp2550
< _temp2548.base? 1: _temp2550 >= _temp2548.last_plus_one){ _throw(
Null_Exception);}* _temp2550;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2551= yyvs; struct _xtunion_struct** _temp2553= _temp2551.curr +
yyvsp_offset; if( _temp2553 < _temp2551.base? 1: _temp2553 >= _temp2551.last_plus_one){
_throw( Null_Exception);}* _temp2553;})))->attributes); _temp2547;}); _temp2546;});
_temp2545;}); break; case 32: _LL2544: yyval=( struct _xtunion_struct*)({ struct
Cyc_DeclSpec_tok_struct* _temp2571=( struct Cyc_DeclSpec_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2571[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2572; _temp2572.tag= Cyc_DeclSpec_tok_tag; _temp2572.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2573=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2573->sc= 0; _temp2573->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2
_temp2577= yyvs; struct _xtunion_struct** _temp2579= _temp2577.curr +(
yyvsp_offset - 1); if( _temp2579 < _temp2577.base? 1: _temp2579 >= _temp2577.last_plus_one){
_throw( Null_Exception);}* _temp2579;})); _temp2573->type_specs= 0; _temp2573->is_inline=
0; _temp2573->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2574= yyvs; struct _xtunion_struct** _temp2576= _temp2574.curr +
yyvsp_offset; if( _temp2576 < _temp2574.base? 1: _temp2576 >= _temp2574.last_plus_one){
_throw( Null_Exception);}* _temp2576;})); _temp2573;}); _temp2572;}); _temp2571;});
break; case 33: _LL2570: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct*
_temp2581=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2581[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2582; _temp2582.tag= Cyc_DeclSpec_tok_tag;
_temp2582.f1=({ struct Cyc_Parse_Declaration_spec* _temp2583=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2583->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2602= yyvs; struct _xtunion_struct** _temp2604=
_temp2602.curr + yyvsp_offset; if( _temp2604 < _temp2602.base? 1: _temp2604 >=
_temp2602.last_plus_one){ _throw( Null_Exception);}* _temp2604;})))->sc;
_temp2583->tq= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp2596= yyvs; struct _xtunion_struct** _temp2598= _temp2596.curr
+( yyvsp_offset - 2); if( _temp2598 < _temp2596.base? 1: _temp2598 >= _temp2596.last_plus_one){
_throw( Null_Exception);}* _temp2598;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2599= yyvs; struct _xtunion_struct** _temp2601= _temp2599.curr
+ yyvsp_offset; if( _temp2601 < _temp2599.base? 1: _temp2601 >= _temp2599.last_plus_one){
_throw( Null_Exception);}* _temp2601;})))->tq); _temp2583->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2593= yyvs; struct _xtunion_struct** _temp2595=
_temp2593.curr + yyvsp_offset; if( _temp2595 < _temp2593.base? 1: _temp2595 >=
_temp2593.last_plus_one){ _throw( Null_Exception);}* _temp2595;})))->type_specs;
_temp2583->is_inline=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2590=
yyvs; struct _xtunion_struct** _temp2592= _temp2590.curr + yyvsp_offset; if(
_temp2592 < _temp2590.base? 1: _temp2592 >= _temp2590.last_plus_one){ _throw(
Null_Exception);}* _temp2592;})))->is_inline; _temp2583->attributes=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2584= yyvs; struct
_xtunion_struct** _temp2586= _temp2584.curr +( yyvsp_offset - 1); if( _temp2586
< _temp2584.base? 1: _temp2586 >= _temp2584.last_plus_one){ _throw(
Null_Exception);}* _temp2586;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2587= yyvs; struct _xtunion_struct** _temp2589= _temp2587.curr +
yyvsp_offset; if( _temp2589 < _temp2587.base? 1: _temp2589 >= _temp2587.last_plus_one){
_throw( Null_Exception);}* _temp2589;})))->attributes); _temp2583;}); _temp2582;});
_temp2581;}); break; case 34: _LL2580: yyval=( struct _xtunion_struct*)({ struct
Cyc_DeclSpec_tok_struct* _temp2606=( struct Cyc_DeclSpec_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2606[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2607; _temp2607.tag= Cyc_DeclSpec_tok_tag; _temp2607.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2608=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2608->sc= 0; _temp2608->tq= Cyc_Absyn_empty_tqual(); _temp2608->type_specs=
0; _temp2608->is_inline= 1; _temp2608->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2609= yyvs; struct _xtunion_struct** _temp2611=
_temp2609.curr + yyvsp_offset; if( _temp2611 < _temp2609.base? 1: _temp2611 >=
_temp2609.last_plus_one){ _throw( Null_Exception);}* _temp2611;})); _temp2608;});
_temp2607;}); _temp2606;}); break; case 35: _LL2605: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2613=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2613[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2614; _temp2614.tag= Cyc_DeclSpec_tok_tag; _temp2614.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2615=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2615->sc=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2628= yyvs;
struct _xtunion_struct** _temp2630= _temp2628.curr + yyvsp_offset; if( _temp2630
< _temp2628.base? 1: _temp2630 >= _temp2628.last_plus_one){ _throw(
Null_Exception);}* _temp2630;})))->sc; _temp2615->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2625= yyvs; struct _xtunion_struct** _temp2627=
_temp2625.curr + yyvsp_offset; if( _temp2627 < _temp2625.base? 1: _temp2627 >=
_temp2625.last_plus_one){ _throw( Null_Exception);}* _temp2627;})))->tq;
_temp2615->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2622=
yyvs; struct _xtunion_struct** _temp2624= _temp2622.curr + yyvsp_offset; if(
_temp2624 < _temp2622.base? 1: _temp2624 >= _temp2622.last_plus_one){ _throw(
Null_Exception);}* _temp2624;})))->type_specs; _temp2615->is_inline= 1;
_temp2615->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2616= yyvs; struct _xtunion_struct** _temp2618=
_temp2616.curr +( yyvsp_offset - 1); if( _temp2618 < _temp2616.base? 1:
_temp2618 >= _temp2616.last_plus_one){ _throw( Null_Exception);}* _temp2618;})),(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2619= yyvs; struct
_xtunion_struct** _temp2621= _temp2619.curr + yyvsp_offset; if( _temp2621 <
_temp2619.base? 1: _temp2621 >= _temp2619.last_plus_one){ _throw( Null_Exception);}*
_temp2621;})))->attributes); _temp2615;}); _temp2614;}); _temp2613;}); break;
case 36: _LL2612: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2632=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2632[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2633; _temp2633.tag=
Cyc_StorageClass_tok_tag; _temp2633.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp2633;}); _temp2632;}); break; case 37: _LL2631: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2635=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2635[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2636; _temp2636.tag= Cyc_StorageClass_tok_tag;
_temp2636.f1=( void*)(( void*) Cyc_Parse_Register_sc); _temp2636;}); _temp2635;});
break; case 38: _LL2634: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2638=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2638[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2639; _temp2639.tag=
Cyc_StorageClass_tok_tag; _temp2639.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2639;}); _temp2638;}); break; case 39: _LL2637: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2641=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2641[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2642; _temp2642.tag= Cyc_StorageClass_tok_tag;
_temp2642.f1=( void*)(( void*) Cyc_Parse_Extern_sc); _temp2642;}); _temp2641;});
break; case 40: _LL2640: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2644= yyvs; struct _xtunion_struct** _temp2646= _temp2644.curr
+ yyvsp_offset; if( _temp2646 < _temp2644.base? 1: _temp2646 >= _temp2644.last_plus_one){
_throw( Null_Exception);}* _temp2646;})),( struct _tagged_string)({ char*
_temp2647=( char*)"C"; struct _tagged_string _temp2648; _temp2648.curr=
_temp2647; _temp2648.base= _temp2647; _temp2648.last_plus_one= _temp2647 + 2;
_temp2648;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2649=(
char*)"only extern or extern \"C\" is allowed"; struct _tagged_string _temp2650;
_temp2650.curr= _temp2649; _temp2650.base= _temp2649; _temp2650.last_plus_one=
_temp2649 + 37; _temp2650;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2651= yyls; struct Cyc_Yyltype* _temp2653= _temp2651.curr +( yylsp_offset -
1); if( _temp2653 < _temp2651.base? 1: _temp2653 >= _temp2651.last_plus_one){
_throw( Null_Exception);}* _temp2653;}).first_line,({ struct _tagged_ptr3
_temp2654= yyls; struct Cyc_Yyltype* _temp2656= _temp2654.curr + yylsp_offset;
if( _temp2656 < _temp2654.base? 1: _temp2656 >= _temp2654.last_plus_one){ _throw(
Null_Exception);}* _temp2656;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_StorageClass_tok_struct* _temp2657=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2657[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2658; _temp2658.tag= Cyc_StorageClass_tok_tag;
_temp2658.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp2658;}); _temp2657;});
break; case 41: _LL2643: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2660=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2660[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2661; _temp2661.tag=
Cyc_StorageClass_tok_tag; _temp2661.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp2661;}); _temp2660;}); break; case 42: _LL2659: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2663=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2663[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2664; _temp2664.tag= Cyc_StorageClass_tok_tag;
_temp2664.f1=( void*)(( void*) Cyc_Parse_Abstract_sc); _temp2664;}); _temp2663;});
break; case 43: _LL2662: yyval=( struct _xtunion_struct*)({ struct Cyc_AttributeList_tok_struct*
_temp2666=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2666[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2667; _temp2667.tag=
Cyc_AttributeList_tok_tag; _temp2667.f1= 0; _temp2667;}); _temp2666;}); break;
case 44: _LL2665: yyval=({ struct _tagged_ptr2 _temp2669= yyvs; struct
_xtunion_struct** _temp2671= _temp2669.curr + yyvsp_offset; if( _temp2671 <
_temp2669.base? 1: _temp2671 >= _temp2669.last_plus_one){ _throw( Null_Exception);}*
_temp2671;}); break; case 45: _LL2668: yyval=( struct _xtunion_struct*)({ struct
Cyc_AttributeList_tok_struct* _temp2673=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2673[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2674; _temp2674.tag= Cyc_AttributeList_tok_tag;
_temp2674.f1= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2675= yyvs;
struct _xtunion_struct** _temp2677= _temp2675.curr +( yyvsp_offset - 2); if(
_temp2677 < _temp2675.base? 1: _temp2677 >= _temp2675.last_plus_one){ _throw(
Null_Exception);}* _temp2677;})); _temp2674;}); _temp2673;}); break; case 46:
_LL2672: yyval=( struct _xtunion_struct*)({ struct Cyc_AttributeList_tok_struct*
_temp2679=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2679[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2680; _temp2680.tag=
Cyc_AttributeList_tok_tag; _temp2680.f1=({ struct Cyc_List_List* _temp2681=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2681->hd=(
void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2682= yyvs; struct
_xtunion_struct** _temp2684= _temp2682.curr + yyvsp_offset; if( _temp2684 <
_temp2682.base? 1: _temp2684 >= _temp2682.last_plus_one){ _throw( Null_Exception);}*
_temp2684;})); _temp2681->tl= 0; _temp2681;}); _temp2680;}); _temp2679;});
break; case 47: _LL2678: yyval=( struct _xtunion_struct*)({ struct Cyc_AttributeList_tok_struct*
_temp2686=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2686[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2687; _temp2687.tag=
Cyc_AttributeList_tok_tag; _temp2687.f1=({ struct Cyc_List_List* _temp2688=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2688->hd=(
void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2692= yyvs; struct
_xtunion_struct** _temp2694= _temp2692.curr +( yyvsp_offset - 2); if( _temp2694
< _temp2692.base? 1: _temp2694 >= _temp2692.last_plus_one){ _throw(
Null_Exception);}* _temp2694;})); _temp2688->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2689= yyvs; struct _xtunion_struct** _temp2691=
_temp2689.curr + yyvsp_offset; if( _temp2691 < _temp2689.base? 1: _temp2691 >=
_temp2689.last_plus_one){ _throw( Null_Exception);}* _temp2691;})); _temp2688;});
_temp2687;}); _temp2686;}); break; case 48: _LL2685: { struct _tagged_string s=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2696= yyvs; struct
_xtunion_struct** _temp2698= _temp2696.curr + yyvsp_offset; if( _temp2698 <
_temp2696.base? 1: _temp2698 >= _temp2696.last_plus_one){ _throw( Null_Exception);}*
_temp2698;})); void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp2699=( char*)"stdcall"; struct _tagged_string _temp2700; _temp2700.curr=
_temp2699; _temp2700.base= _temp2699; _temp2700.last_plus_one= _temp2699 + 8;
_temp2700;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2701=( char*)"__stdcall__"; struct _tagged_string _temp2702; _temp2702.curr=
_temp2701; _temp2702.base= _temp2701; _temp2702.last_plus_one= _temp2701 + 12;
_temp2702;})) == 0){ a=( void*) Cyc_Absyn_Stdcall_att;} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2703=( char*)"cdecl"; struct
_tagged_string _temp2704; _temp2704.curr= _temp2703; _temp2704.base= _temp2703;
_temp2704.last_plus_one= _temp2703 + 6; _temp2704;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2705=( char*)"__cdecl__"; struct
_tagged_string _temp2706; _temp2706.curr= _temp2705; _temp2706.base= _temp2705;
_temp2706.last_plus_one= _temp2705 + 10; _temp2706;})) == 0){ a=( void*) Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2707=(
char*)"noreturn"; struct _tagged_string _temp2708; _temp2708.curr= _temp2707;
_temp2708.base= _temp2707; _temp2708.last_plus_one= _temp2707 + 9; _temp2708;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2709=( char*)"__noreturn__";
struct _tagged_string _temp2710; _temp2710.curr= _temp2709; _temp2710.base=
_temp2709; _temp2710.last_plus_one= _temp2709 + 13; _temp2710;})) == 0){ a=(
void*) Cyc_Absyn_Noreturn_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2711=( char*)"noreturn"; struct _tagged_string
_temp2712; _temp2712.curr= _temp2711; _temp2712.base= _temp2711; _temp2712.last_plus_one=
_temp2711 + 9; _temp2712;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2713=( char*)"__noreturn__"; struct _tagged_string
_temp2714; _temp2714.curr= _temp2713; _temp2714.base= _temp2713; _temp2714.last_plus_one=
_temp2713 + 13; _temp2714;})) == 0){ a=( void*) Cyc_Absyn_Noreturn_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2715=( char*)"__const__";
struct _tagged_string _temp2716; _temp2716.curr= _temp2715; _temp2716.base=
_temp2715; _temp2716.last_plus_one= _temp2715 + 10; _temp2716;})) == 0){ a=(
void*) Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2717=( char*)"aligned"; struct _tagged_string
_temp2718; _temp2718.curr= _temp2717; _temp2718.base= _temp2717; _temp2718.last_plus_one=
_temp2717 + 8; _temp2718;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2719=( char*)"__aligned__"; struct _tagged_string
_temp2720; _temp2720.curr= _temp2719; _temp2720.base= _temp2719; _temp2720.last_plus_one=
_temp2719 + 12; _temp2720;})) == 0){ a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp2721=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct));
_temp2721[ 0]=({ struct Cyc_Absyn_Aligned_att_struct _temp2722; _temp2722.tag=
Cyc_Absyn_Aligned_att_tag; _temp2722.f1= - 1; _temp2722;}); _temp2721;});} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2723=( char*)"packed";
struct _tagged_string _temp2724; _temp2724.curr= _temp2723; _temp2724.base=
_temp2723; _temp2724.last_plus_one= _temp2723 + 7; _temp2724;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2725=( char*)"__packed__"; struct
_tagged_string _temp2726; _temp2726.curr= _temp2725; _temp2726.base= _temp2725;
_temp2726.last_plus_one= _temp2725 + 11; _temp2726;})) == 0){ a=( void*) Cyc_Absyn_Packed_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2727=(
char*)"shared"; struct _tagged_string _temp2728; _temp2728.curr= _temp2727;
_temp2728.base= _temp2727; _temp2728.last_plus_one= _temp2727 + 7; _temp2728;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2729=( char*)"__shared__";
struct _tagged_string _temp2730; _temp2730.curr= _temp2729; _temp2730.base=
_temp2729; _temp2730.last_plus_one= _temp2729 + 11; _temp2730;})) == 0){ a=(
void*) Cyc_Absyn_Shared_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2731=( char*)"unused"; struct _tagged_string
_temp2732; _temp2732.curr= _temp2731; _temp2732.base= _temp2731; _temp2732.last_plus_one=
_temp2731 + 7; _temp2732;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2733=( char*)"__unused__"; struct _tagged_string
_temp2734; _temp2734.curr= _temp2733; _temp2734.base= _temp2733; _temp2734.last_plus_one=
_temp2733 + 11; _temp2734;})) == 0){ a=( void*) Cyc_Absyn_Unused_att;} else{ if(
Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2735=( char*)"weak";
struct _tagged_string _temp2736; _temp2736.curr= _temp2735; _temp2736.base=
_temp2735; _temp2736.last_plus_one= _temp2735 + 5; _temp2736;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2737=( char*)"__weak__"; struct
_tagged_string _temp2738; _temp2738.curr= _temp2737; _temp2738.base= _temp2737;
_temp2738.last_plus_one= _temp2737 + 9; _temp2738;})) == 0){ a=( void*) Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2739=(
char*)"dllimport"; struct _tagged_string _temp2740; _temp2740.curr= _temp2739;
_temp2740.base= _temp2739; _temp2740.last_plus_one= _temp2739 + 10; _temp2740;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2741=( char*)"__dllimport__";
struct _tagged_string _temp2742; _temp2742.curr= _temp2741; _temp2742.base=
_temp2741; _temp2742.last_plus_one= _temp2741 + 14; _temp2742;})) == 0){ a=(
void*) Cyc_Absyn_Dllimport_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2743=( char*)"dllexport"; struct _tagged_string
_temp2744; _temp2744.curr= _temp2743; _temp2744.base= _temp2743; _temp2744.last_plus_one=
_temp2743 + 10; _temp2744;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2745=( char*)"__dllexport__"; struct _tagged_string
_temp2746; _temp2746.curr= _temp2745; _temp2746.base= _temp2745; _temp2746.last_plus_one=
_temp2745 + 14; _temp2746;})) == 0){ a=( void*) Cyc_Absyn_Dllexport_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2747=( char*)"no_instrument_function";
struct _tagged_string _temp2748; _temp2748.curr= _temp2747; _temp2748.base=
_temp2747; _temp2748.last_plus_one= _temp2747 + 23; _temp2748;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2749=( char*)"__no_instrument_function__";
struct _tagged_string _temp2750; _temp2750.curr= _temp2749; _temp2750.base=
_temp2749; _temp2750.last_plus_one= _temp2749 + 27; _temp2750;})) == 0){ a=(
void*) Cyc_Absyn_No_instrument_function_att;} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2751=( char*)"constructor"; struct
_tagged_string _temp2752; _temp2752.curr= _temp2751; _temp2752.base= _temp2751;
_temp2752.last_plus_one= _temp2751 + 12; _temp2752;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2753=( char*)"__constructor__"; struct
_tagged_string _temp2754; _temp2754.curr= _temp2753; _temp2754.base= _temp2753;
_temp2754.last_plus_one= _temp2753 + 16; _temp2754;})) == 0){ a=( void*) Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2755=(
char*)"destructor"; struct _tagged_string _temp2756; _temp2756.curr= _temp2755;
_temp2756.base= _temp2755; _temp2756.last_plus_one= _temp2755 + 11; _temp2756;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2757=( char*)"__destructor__";
struct _tagged_string _temp2758; _temp2758.curr= _temp2757; _temp2758.base=
_temp2757; _temp2758.last_plus_one= _temp2757 + 15; _temp2758;})) == 0){ a=(
void*) Cyc_Absyn_Destructor_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2759=( char*)"no_check_memory_usage"; struct
_tagged_string _temp2760; _temp2760.curr= _temp2759; _temp2760.base= _temp2759;
_temp2760.last_plus_one= _temp2759 + 22; _temp2760;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2761=( char*)"__no_check_memory_usage__";
struct _tagged_string _temp2762; _temp2762.curr= _temp2761; _temp2762.base=
_temp2761; _temp2762.last_plus_one= _temp2761 + 26; _temp2762;})) == 0){ a=(
void*) Cyc_Absyn_No_check_memory_usage_att;} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2763=( char*)"unrecognized attribute"; struct
_tagged_string _temp2764; _temp2764.curr= _temp2763; _temp2764.base= _temp2763;
_temp2764.last_plus_one= _temp2763 + 23; _temp2764;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2765= yyls; struct Cyc_Yyltype* _temp2767= _temp2765.curr
+ yylsp_offset; if( _temp2767 < _temp2765.base? 1: _temp2767 >= _temp2765.last_plus_one){
_throw( Null_Exception);}* _temp2767;}).first_line,({ struct _tagged_ptr3
_temp2768= yyls; struct Cyc_Yyltype* _temp2770= _temp2768.curr + yylsp_offset;
if( _temp2770 < _temp2768.base? 1: _temp2770 >= _temp2768.last_plus_one){ _throw(
Null_Exception);}* _temp2770;}).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}}
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2771=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2771[ 0]=({ struct Cyc_Attribute_tok_struct _temp2772; _temp2772.tag= Cyc_Attribute_tok_tag;
_temp2772.f1=( void*) a; _temp2772;}); _temp2771;}); break;} case 49: _LL2695:
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2774=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2774[ 0]=({ struct Cyc_Attribute_tok_struct _temp2775; _temp2775.tag= Cyc_Attribute_tok_tag;
_temp2775.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2775;}); _temp2774;});
break; case 50: _LL2773: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2777= yyvs; struct _xtunion_struct** _temp2779=
_temp2777.curr +( yyvsp_offset - 3); if( _temp2779 < _temp2777.base? 1:
_temp2779 >= _temp2777.last_plus_one){ _throw( Null_Exception);}* _temp2779;}));
struct _tuple12 _temp2785; int _temp2786; void* _temp2788; struct _tuple12*
_temp2783= Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp2780= yyvs; struct
_xtunion_struct** _temp2782= _temp2780.curr +( yyvsp_offset - 1); if( _temp2782
< _temp2780.base? 1: _temp2782 >= _temp2780.last_plus_one){ _throw(
Null_Exception);}* _temp2782;})); _temp2785=* _temp2783; _LL2789: _temp2788=
_temp2785.f1; goto _LL2787; _LL2787: _temp2786= _temp2785.f2; goto _LL2784;
_LL2784: { void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2790=( char*)"regparm"; struct _tagged_string _temp2791; _temp2791.curr=
_temp2790; _temp2791.base= _temp2790; _temp2791.last_plus_one= _temp2790 + 8;
_temp2791;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2792=( char*)"__regparm__"; struct _tagged_string _temp2793; _temp2793.curr=
_temp2792; _temp2793.base= _temp2792; _temp2793.last_plus_one= _temp2792 + 12;
_temp2793;})) == 0){ if( _temp2786 <= 0? 1: _temp2786 > 3){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2794=( char*)"regparm requires value between 1 and 3";
struct _tagged_string _temp2795; _temp2795.curr= _temp2794; _temp2795.base=
_temp2794; _temp2795.last_plus_one= _temp2794 + 39; _temp2795;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2796= yyls; struct Cyc_Yyltype* _temp2798= _temp2796.curr
+( yylsp_offset - 1); if( _temp2798 < _temp2796.base? 1: _temp2798 >= _temp2796.last_plus_one){
_throw( Null_Exception);}* _temp2798;}).first_line,({ struct _tagged_ptr3
_temp2799= yyls; struct Cyc_Yyltype* _temp2801= _temp2799.curr +( yylsp_offset -
1); if( _temp2801 < _temp2799.base? 1: _temp2801 >= _temp2799.last_plus_one){
_throw( Null_Exception);}* _temp2801;}).last_line));} a=( void*)({ struct Cyc_Absyn_Regparm_att_struct*
_temp2802=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct));
_temp2802[ 0]=({ struct Cyc_Absyn_Regparm_att_struct _temp2803; _temp2803.tag=
Cyc_Absyn_Regparm_att_tag; _temp2803.f1= _temp2786; _temp2803;}); _temp2802;});}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2804=(
char*)"aligned"; struct _tagged_string _temp2805; _temp2805.curr= _temp2804;
_temp2805.base= _temp2804; _temp2805.last_plus_one= _temp2804 + 8; _temp2805;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2806=( char*)"__aligned__";
struct _tagged_string _temp2807; _temp2807.curr= _temp2806; _temp2807.base=
_temp2806; _temp2807.last_plus_one= _temp2806 + 12; _temp2807;})) == 0){ if(
_temp2786 < 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2808=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2809; _temp2809.curr= _temp2808; _temp2809.base=
_temp2808; _temp2809.last_plus_one= _temp2808 + 39; _temp2809;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2810= yyls; struct Cyc_Yyltype* _temp2812= _temp2810.curr
+( yylsp_offset - 1); if( _temp2812 < _temp2810.base? 1: _temp2812 >= _temp2810.last_plus_one){
_throw( Null_Exception);}* _temp2812;}).first_line,({ struct _tagged_ptr3
_temp2813= yyls; struct Cyc_Yyltype* _temp2815= _temp2813.curr +( yylsp_offset -
1); if( _temp2815 < _temp2813.base? 1: _temp2815 >= _temp2813.last_plus_one){
_throw( Null_Exception);}* _temp2815;}).last_line));}{ unsigned int j=(
unsigned int) _temp2786; for( 0;( j &( unsigned int) 1) ==( unsigned int) 0; j=
j >>( unsigned int) 1){;} j= j >>( unsigned int) 1; if( j !=( unsigned int) 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2816=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2817; _temp2817.curr= _temp2816; _temp2817.base=
_temp2816; _temp2817.last_plus_one= _temp2816 + 39; _temp2817;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2818= yyls; struct Cyc_Yyltype* _temp2820= _temp2818.curr
+( yylsp_offset - 1); if( _temp2820 < _temp2818.base? 1: _temp2820 >= _temp2818.last_plus_one){
_throw( Null_Exception);}* _temp2820;}).first_line,({ struct _tagged_ptr3
_temp2821= yyls; struct Cyc_Yyltype* _temp2823= _temp2821.curr +( yylsp_offset -
1); if( _temp2823 < _temp2821.base? 1: _temp2823 >= _temp2821.last_plus_one){
_throw( Null_Exception);}* _temp2823;}).last_line));} a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp2824=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct));
_temp2824[ 0]=({ struct Cyc_Absyn_Aligned_att_struct _temp2825; _temp2825.tag=
Cyc_Absyn_Aligned_att_tag; _temp2825.f1= _temp2786; _temp2825;}); _temp2824;});}}
else{ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2826=( char*)"unrecognized attribute";
struct _tagged_string _temp2827; _temp2827.curr= _temp2826; _temp2827.base=
_temp2826; _temp2827.last_plus_one= _temp2826 + 23; _temp2827;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2828= yyls; struct Cyc_Yyltype* _temp2830= _temp2828.curr
+( yylsp_offset - 3); if( _temp2830 < _temp2828.base? 1: _temp2830 >= _temp2828.last_plus_one){
_throw( Null_Exception);}* _temp2830;}).first_line,({ struct _tagged_ptr3
_temp2831= yyls; struct Cyc_Yyltype* _temp2833= _temp2831.curr +( yylsp_offset -
3); if( _temp2833 < _temp2831.base? 1: _temp2833 >= _temp2831.last_plus_one){
_throw( Null_Exception);}* _temp2833;}).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;}}
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2834=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2834[ 0]=({ struct Cyc_Attribute_tok_struct _temp2835; _temp2835.tag= Cyc_Attribute_tok_tag;
_temp2835.f1=( void*) a; _temp2835;}); _temp2834;}); break;}} case 51: _LL2776: {
struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2837=
yyvs; struct _xtunion_struct** _temp2839= _temp2837.curr +( yyvsp_offset - 3);
if( _temp2839 < _temp2837.base? 1: _temp2839 >= _temp2837.last_plus_one){ _throw(
Null_Exception);}* _temp2839;})); struct _tagged_string str= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2840= yyvs; struct _xtunion_struct** _temp2842=
_temp2840.curr +( yyvsp_offset - 1); if( _temp2842 < _temp2840.base? 1:
_temp2842 >= _temp2840.last_plus_one){ _throw( Null_Exception);}* _temp2842;}));
void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2843=(
char*)"section"; struct _tagged_string _temp2844; _temp2844.curr= _temp2843;
_temp2844.base= _temp2843; _temp2844.last_plus_one= _temp2843 + 8; _temp2844;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2845=( char*)"__section__";
struct _tagged_string _temp2846; _temp2846.curr= _temp2845; _temp2846.base=
_temp2845; _temp2846.last_plus_one= _temp2845 + 12; _temp2846;}))){ a=( void*)({
struct Cyc_Absyn_Section_att_struct* _temp2847=( struct Cyc_Absyn_Section_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct)); _temp2847[ 0]=({
struct Cyc_Absyn_Section_att_struct _temp2848; _temp2848.tag= Cyc_Absyn_Section_att_tag;
_temp2848.f1= str; _temp2848;}); _temp2847;});} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2849=( char*)"unrecognized attribute"; struct
_tagged_string _temp2850; _temp2850.curr= _temp2849; _temp2850.base= _temp2849;
_temp2850.last_plus_one= _temp2849 + 23; _temp2850;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2851= yyls; struct Cyc_Yyltype* _temp2853= _temp2851.curr
+( yylsp_offset - 3); if( _temp2853 < _temp2851.base? 1: _temp2853 >= _temp2851.last_plus_one){
_throw( Null_Exception);}* _temp2853;}).first_line,({ struct _tagged_ptr3
_temp2854= yyls; struct Cyc_Yyltype* _temp2856= _temp2854.curr +( yylsp_offset -
3); if( _temp2856 < _temp2854.base? 1: _temp2856 >= _temp2854.last_plus_one){
_throw( Null_Exception);}* _temp2856;}).last_line)); a=( void*) Cyc_Absyn_Cdecl_att;}
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2857=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2857[ 0]=({ struct Cyc_Attribute_tok_struct _temp2858; _temp2858.tag= Cyc_Attribute_tok_tag;
_temp2858.f1=( void*) a; _temp2858;}); _temp2857;}); break;} case 52: _LL2836:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2860=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2860[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2861; _temp2861.tag=
Cyc_TypeSpecifier_tok_tag; _temp2861.f1=( void*) Cyc_Parse_type_spec(( void*)
Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2862=
yyls; struct Cyc_Yyltype* _temp2864= _temp2862.curr + yylsp_offset; if(
_temp2864 < _temp2862.base? 1: _temp2864 >= _temp2862.last_plus_one){ _throw(
Null_Exception);}* _temp2864;}).first_line,({ struct _tagged_ptr3 _temp2865=
yyls; struct Cyc_Yyltype* _temp2867= _temp2865.curr + yylsp_offset; if(
_temp2867 < _temp2865.base? 1: _temp2867 >= _temp2865.last_plus_one){ _throw(
Null_Exception);}* _temp2867;}).last_line)); _temp2861;}); _temp2860;}); break;
case 53: _LL2859: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2869=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2869[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2870; _temp2870.tag=
Cyc_TypeSpecifier_tok_tag; _temp2870.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2871= yyls; struct Cyc_Yyltype* _temp2873= _temp2871.curr + yylsp_offset;
if( _temp2873 < _temp2871.base? 1: _temp2873 >= _temp2871.last_plus_one){ _throw(
Null_Exception);}* _temp2873;}).first_line,({ struct _tagged_ptr3 _temp2874=
yyls; struct Cyc_Yyltype* _temp2876= _temp2874.curr + yylsp_offset; if(
_temp2876 < _temp2874.base? 1: _temp2876 >= _temp2874.last_plus_one){ _throw(
Null_Exception);}* _temp2876;}).last_line)); _temp2870;}); _temp2869;}); break;
case 54: _LL2868: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2878=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2878[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2879; _temp2879.tag=
Cyc_TypeSpecifier_tok_tag; _temp2879.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2880= yyls; struct Cyc_Yyltype*
_temp2882= _temp2880.curr + yylsp_offset; if( _temp2882 < _temp2880.base? 1:
_temp2882 >= _temp2880.last_plus_one){ _throw( Null_Exception);}* _temp2882;}).first_line,({
struct _tagged_ptr3 _temp2883= yyls; struct Cyc_Yyltype* _temp2885= _temp2883.curr
+ yylsp_offset; if( _temp2885 < _temp2883.base? 1: _temp2885 >= _temp2883.last_plus_one){
_throw( Null_Exception);}* _temp2885;}).last_line)); _temp2879;}); _temp2878;});
break; case 55: _LL2877: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2887=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2887[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2888; _temp2888.tag=
Cyc_TypeSpecifier_tok_tag; _temp2888.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp2889=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2889[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2890; _temp2890.tag=
Cyc_Parse_Short_spec_tag; _temp2890.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2891= yyls; struct Cyc_Yyltype* _temp2893= _temp2891.curr +
yylsp_offset; if( _temp2893 < _temp2891.base? 1: _temp2893 >= _temp2891.last_plus_one){
_throw( Null_Exception);}* _temp2893;}).first_line,({ struct _tagged_ptr3
_temp2894= yyls; struct Cyc_Yyltype* _temp2896= _temp2894.curr + yylsp_offset;
if( _temp2896 < _temp2894.base? 1: _temp2896 >= _temp2894.last_plus_one){ _throw(
Null_Exception);}* _temp2896;}).last_line); _temp2890;}); _temp2889;}));
_temp2888;}); _temp2887;}); break; case 56: _LL2886: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2898=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2898[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2899; _temp2899.tag= Cyc_TypeSpecifier_tok_tag;
_temp2899.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2900= yyls; struct Cyc_Yyltype* _temp2902= _temp2900.curr
+ yylsp_offset; if( _temp2902 < _temp2900.base? 1: _temp2902 >= _temp2900.last_plus_one){
_throw( Null_Exception);}* _temp2902;}).first_line,({ struct _tagged_ptr3
_temp2903= yyls; struct Cyc_Yyltype* _temp2905= _temp2903.curr + yylsp_offset;
if( _temp2905 < _temp2903.base? 1: _temp2905 >= _temp2903.last_plus_one){ _throw(
Null_Exception);}* _temp2905;}).last_line)); _temp2899;}); _temp2898;}); break;
case 57: _LL2897: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2907=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2907[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2908; _temp2908.tag=
Cyc_TypeSpecifier_tok_tag; _temp2908.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2909=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2909[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2910; _temp2910.tag= Cyc_Parse_Long_spec_tag;
_temp2910.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2911= yyls;
struct Cyc_Yyltype* _temp2913= _temp2911.curr + yylsp_offset; if( _temp2913 <
_temp2911.base? 1: _temp2913 >= _temp2911.last_plus_one){ _throw( Null_Exception);}*
_temp2913;}).first_line,({ struct _tagged_ptr3 _temp2914= yyls; struct Cyc_Yyltype*
_temp2916= _temp2914.curr + yylsp_offset; if( _temp2916 < _temp2914.base? 1:
_temp2916 >= _temp2914.last_plus_one){ _throw( Null_Exception);}* _temp2916;}).last_line);
_temp2910;}); _temp2909;})); _temp2908;}); _temp2907;}); break; case 58: _LL2906:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2918=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2918[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2919; _temp2919.tag=
Cyc_TypeSpecifier_tok_tag; _temp2919.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2920= yyls; struct Cyc_Yyltype*
_temp2922= _temp2920.curr + yylsp_offset; if( _temp2922 < _temp2920.base? 1:
_temp2922 >= _temp2920.last_plus_one){ _throw( Null_Exception);}* _temp2922;}).first_line,({
struct _tagged_ptr3 _temp2923= yyls; struct Cyc_Yyltype* _temp2925= _temp2923.curr
+ yylsp_offset; if( _temp2925 < _temp2923.base? 1: _temp2925 >= _temp2923.last_plus_one){
_throw( Null_Exception);}* _temp2925;}).last_line)); _temp2919;}); _temp2918;});
break; case 59: _LL2917: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2927=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2927[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2928; _temp2928.tag=
Cyc_TypeSpecifier_tok_tag; _temp2928.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2929= yyls; struct Cyc_Yyltype*
_temp2931= _temp2929.curr + yylsp_offset; if( _temp2931 < _temp2929.base? 1:
_temp2931 >= _temp2929.last_plus_one){ _throw( Null_Exception);}* _temp2931;}).first_line,({
struct _tagged_ptr3 _temp2932= yyls; struct Cyc_Yyltype* _temp2934= _temp2932.curr
+ yylsp_offset; if( _temp2934 < _temp2932.base? 1: _temp2934 >= _temp2932.last_plus_one){
_throw( Null_Exception);}* _temp2934;}).last_line)); _temp2928;}); _temp2927;});
break; case 60: _LL2926: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2936=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2936[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2937; _temp2937.tag=
Cyc_TypeSpecifier_tok_tag; _temp2937.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp2938=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2938[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp2939; _temp2939.tag=
Cyc_Parse_Signed_spec_tag; _temp2939.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2940= yyls; struct Cyc_Yyltype* _temp2942= _temp2940.curr +
yylsp_offset; if( _temp2942 < _temp2940.base? 1: _temp2942 >= _temp2940.last_plus_one){
_throw( Null_Exception);}* _temp2942;}).first_line,({ struct _tagged_ptr3
_temp2943= yyls; struct Cyc_Yyltype* _temp2945= _temp2943.curr + yylsp_offset;
if( _temp2945 < _temp2943.base? 1: _temp2945 >= _temp2943.last_plus_one){ _throw(
Null_Exception);}* _temp2945;}).last_line); _temp2939;}); _temp2938;}));
_temp2937;}); _temp2936;}); break; case 61: _LL2935: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2947=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2947[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2948; _temp2948.tag= Cyc_TypeSpecifier_tok_tag;
_temp2948.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp2949=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp2949[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp2950; _temp2950.tag= Cyc_Parse_Unsigned_spec_tag; _temp2950.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2951= yyls; struct Cyc_Yyltype* _temp2953= _temp2951.curr
+ yylsp_offset; if( _temp2953 < _temp2951.base? 1: _temp2953 >= _temp2951.last_plus_one){
_throw( Null_Exception);}* _temp2953;}).first_line,({ struct _tagged_ptr3
_temp2954= yyls; struct Cyc_Yyltype* _temp2956= _temp2954.curr + yylsp_offset;
if( _temp2956 < _temp2954.base? 1: _temp2956 >= _temp2954.last_plus_one){ _throw(
Null_Exception);}* _temp2956;}).last_line); _temp2950;}); _temp2949;}));
_temp2948;}); _temp2947;}); break; case 62: _LL2946: yyval=({ struct
_tagged_ptr2 _temp2958= yyvs; struct _xtunion_struct** _temp2960= _temp2958.curr
+ yyvsp_offset; if( _temp2960 < _temp2958.base? 1: _temp2960 >= _temp2958.last_plus_one){
_throw( Null_Exception);}* _temp2960;}); break; case 63: _LL2957: yyval=({
struct _tagged_ptr2 _temp2962= yyvs; struct _xtunion_struct** _temp2964=
_temp2962.curr + yyvsp_offset; if( _temp2964 < _temp2962.base? 1: _temp2964 >=
_temp2962.last_plus_one){ _throw( Null_Exception);}* _temp2964;}); break; case
64: _LL2961: yyval=({ struct _tagged_ptr2 _temp2966= yyvs; struct
_xtunion_struct** _temp2968= _temp2966.curr + yyvsp_offset; if( _temp2968 <
_temp2966.base? 1: _temp2968 >= _temp2966.last_plus_one){ _throw( Null_Exception);}*
_temp2968;}); break; case 65: _LL2965: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2970=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2970[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2971; _temp2971.tag= Cyc_TypeSpecifier_tok_tag;
_temp2971.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2972= yyvs; struct _xtunion_struct** _temp2974=
_temp2972.curr + yyvsp_offset; if( _temp2974 < _temp2972.base? 1: _temp2974 >=
_temp2972.last_plus_one){ _throw( Null_Exception);}* _temp2974;})),(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2975= yyls; struct Cyc_Yyltype* _temp2977= _temp2975.curr + yylsp_offset;
if( _temp2977 < _temp2975.base? 1: _temp2977 >= _temp2975.last_plus_one){ _throw(
Null_Exception);}* _temp2977;}).first_line,({ struct _tagged_ptr3 _temp2978=
yyls; struct Cyc_Yyltype* _temp2980= _temp2978.curr + yylsp_offset; if(
_temp2980 < _temp2978.base? 1: _temp2980 >= _temp2978.last_plus_one){ _throw(
Null_Exception);}* _temp2980;}).last_line)); _temp2971;}); _temp2970;}); break;
case 66: _LL2969: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2982=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2982[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2983; _temp2983.tag=
Cyc_TypeSpecifier_tok_tag; _temp2983.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2984= yyvs; struct
_xtunion_struct** _temp2986= _temp2984.curr +( yyvsp_offset - 2); if( _temp2986
< _temp2984.base? 1: _temp2986 >= _temp2984.last_plus_one){ _throw(
Null_Exception);}* _temp2986;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp2987= yyvs; struct
_xtunion_struct** _temp2989= _temp2987.curr + yyvsp_offset; if( _temp2989 <
_temp2987.base? 1: _temp2989 >= _temp2987.last_plus_one){ _throw( Null_Exception);}*
_temp2989;})))), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2990=
yyls; struct Cyc_Yyltype* _temp2992= _temp2990.curr +( yylsp_offset - 2); if(
_temp2992 < _temp2990.base? 1: _temp2992 >= _temp2990.last_plus_one){ _throw(
Null_Exception);}* _temp2992;}).first_line,({ struct _tagged_ptr3 _temp2993=
yyls; struct Cyc_Yyltype* _temp2995= _temp2993.curr + yylsp_offset; if(
_temp2995 < _temp2993.base? 1: _temp2995 >= _temp2993.last_plus_one){ _throw(
Null_Exception);}* _temp2995;}).last_line)); _temp2983;}); _temp2982;}); break;
case 67: _LL2981: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2997=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2997[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2998; _temp2998.tag=
Cyc_TypeSpecifier_tok_tag; _temp2998.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp2999=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp2999[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp3000; _temp3000.tag= Cyc_Absyn_TypedefType_tag;
_temp3000.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3001= yyvs;
struct _xtunion_struct** _temp3003= _temp3001.curr +( yyvsp_offset - 1); if(
_temp3003 < _temp3001.base? 1: _temp3003 >= _temp3001.last_plus_one){ _throw(
Null_Exception);}* _temp3003;})); _temp3000.f2= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3004= yyvs; struct _xtunion_struct** _temp3006= _temp3004.curr
+ yyvsp_offset; if( _temp3006 < _temp3004.base? 1: _temp3006 >= _temp3004.last_plus_one){
_throw( Null_Exception);}* _temp3006;})); _temp3000.f3= 0; _temp3000;});
_temp2999;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3007= yyls;
struct Cyc_Yyltype* _temp3009= _temp3007.curr +( yylsp_offset - 1); if(
_temp3009 < _temp3007.base? 1: _temp3009 >= _temp3007.last_plus_one){ _throw(
Null_Exception);}* _temp3009;}).first_line,({ struct _tagged_ptr3 _temp3010=
yyls; struct Cyc_Yyltype* _temp3012= _temp3010.curr + yylsp_offset; if(
_temp3012 < _temp3010.base? 1: _temp3012 >= _temp3010.last_plus_one){ _throw(
Null_Exception);}* _temp3012;}).last_line)); _temp2998;}); _temp2997;}); break;
case 68: _LL2996: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3014=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3014[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3015; _temp3015.tag=
Cyc_TypeSpecifier_tok_tag; _temp3015.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp3016=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp3016[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp3017; _temp3017.tag= Cyc_Absyn_TupleType_tag;
_temp3017.f1=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3018= yyls; struct Cyc_Yyltype* _temp3020= _temp3018.curr +(
yylsp_offset - 1); if( _temp3020 < _temp3018.base? 1: _temp3020 >= _temp3018.last_plus_one){
_throw( Null_Exception);}* _temp3020;}).first_line,({ struct _tagged_ptr3
_temp3021= yyls; struct Cyc_Yyltype* _temp3023= _temp3021.curr +( yylsp_offset -
1); if( _temp3023 < _temp3021.base? 1: _temp3023 >= _temp3021.last_plus_one){
_throw( Null_Exception);}* _temp3023;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3024= yyvs; struct _xtunion_struct** _temp3026=
_temp3024.curr +( yyvsp_offset - 1); if( _temp3026 < _temp3024.base? 1:
_temp3026 >= _temp3024.last_plus_one){ _throw( Null_Exception);}* _temp3026;}))));
_temp3017;}); _temp3016;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3027= yyls; struct Cyc_Yyltype* _temp3029= _temp3027.curr +( yylsp_offset -
3); if( _temp3029 < _temp3027.base? 1: _temp3029 >= _temp3027.last_plus_one){
_throw( Null_Exception);}* _temp3029;}).first_line,({ struct _tagged_ptr3
_temp3030= yyls; struct Cyc_Yyltype* _temp3032= _temp3030.curr + yylsp_offset;
if( _temp3032 < _temp3030.base? 1: _temp3032 >= _temp3030.last_plus_one){ _throw(
Null_Exception);}* _temp3032;}).last_line)); _temp3015;}); _temp3014;}); break;
case 69: _LL3013: yyval=( struct _xtunion_struct*)({ struct Cyc_Kind_tok_struct*
_temp3034=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp3034[ 0]=({ struct Cyc_Kind_tok_struct _temp3035; _temp3035.tag= Cyc_Kind_tok_tag;
_temp3035.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp3036= yyvs; struct _xtunion_struct** _temp3038= _temp3036.curr
+ yyvsp_offset; if( _temp3038 < _temp3036.base? 1: _temp3038 >= _temp3036.last_plus_one){
_throw( Null_Exception);}* _temp3038;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3039= yyls; struct Cyc_Yyltype* _temp3041= _temp3039.curr +
yylsp_offset; if( _temp3041 < _temp3039.base? 1: _temp3041 >= _temp3039.last_plus_one){
_throw( Null_Exception);}* _temp3041;}).first_line,({ struct _tagged_ptr3
_temp3042= yyls; struct Cyc_Yyltype* _temp3044= _temp3042.curr + yylsp_offset;
if( _temp3044 < _temp3042.base? 1: _temp3044 >= _temp3042.last_plus_one){ _throw(
Null_Exception);}* _temp3044;}).last_line)); _temp3035;}); _temp3034;}); break;
case 70: _LL3033: { struct _tagged_string* _temp3051; void* _temp3053; struct
_tuple1 _temp3049=* Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3046= yyvs;
struct _xtunion_struct** _temp3048= _temp3046.curr + yyvsp_offset; if( _temp3048
< _temp3046.base? 1: _temp3048 >= _temp3046.last_plus_one){ _throw(
Null_Exception);}* _temp3048;})); _LL3054: _temp3053= _temp3049.f1; goto _LL3052;
_LL3052: _temp3051= _temp3049.f2; goto _LL3050; _LL3050: if( _temp3053 !=( void*)
Cyc_Absyn_Loc_n){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp3055=(
char*)"bad kind in type specifier"; struct _tagged_string _temp3056; _temp3056.curr=
_temp3055; _temp3056.base= _temp3055; _temp3056.last_plus_one= _temp3055 + 27;
_temp3056;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3057= yyls;
struct Cyc_Yyltype* _temp3059= _temp3057.curr + yylsp_offset; if( _temp3059 <
_temp3057.base? 1: _temp3059 >= _temp3057.last_plus_one){ _throw( Null_Exception);}*
_temp3059;}).first_line,({ struct _tagged_ptr3 _temp3060= yyls; struct Cyc_Yyltype*
_temp3062= _temp3060.curr + yylsp_offset; if( _temp3062 < _temp3060.base? 1:
_temp3062 >= _temp3060.last_plus_one){ _throw( Null_Exception);}* _temp3062;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_Kind_tok_struct* _temp3063=(
struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp3063[ 0]=({ struct Cyc_Kind_tok_struct _temp3064; _temp3064.tag= Cyc_Kind_tok_tag;
_temp3064.f1=( void*) Cyc_Parse_id_to_kind(* _temp3051, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3065= yyls; struct Cyc_Yyltype* _temp3067= _temp3065.curr
+ yylsp_offset; if( _temp3067 < _temp3065.base? 1: _temp3067 >= _temp3065.last_plus_one){
_throw( Null_Exception);}* _temp3067;}).first_line,({ struct _tagged_ptr3
_temp3068= yyls; struct Cyc_Yyltype* _temp3070= _temp3068.curr + yylsp_offset;
if( _temp3070 < _temp3068.base? 1: _temp3070 >= _temp3068.last_plus_one){ _throw(
Null_Exception);}* _temp3070;}).last_line)); _temp3064;}); _temp3063;}); break;}
case 71: _LL3045: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeQual_tok_struct*
_temp3072=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp3072[ 0]=({ struct Cyc_TypeQual_tok_struct _temp3073; _temp3073.tag= Cyc_TypeQual_tok_tag;
_temp3073.f1=({ struct Cyc_Absyn_Tqual* _temp3074=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp3074->q_const= 1;
_temp3074->q_volatile= 0; _temp3074->q_restrict= 0; _temp3074;}); _temp3073;});
_temp3072;}); break; case 72: _LL3071: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeQual_tok_struct* _temp3076=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct)); _temp3076[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3077; _temp3077.tag= Cyc_TypeQual_tok_tag; _temp3077.f1=({ struct Cyc_Absyn_Tqual*
_temp3078=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp3078->q_const= 0; _temp3078->q_volatile= 1; _temp3078->q_restrict= 0;
_temp3078;}); _temp3077;}); _temp3076;}); break; case 73: _LL3075: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp3080=( struct
Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp3080[ 0]=({ struct Cyc_TypeQual_tok_struct _temp3081; _temp3081.tag= Cyc_TypeQual_tok_tag;
_temp3081.f1=({ struct Cyc_Absyn_Tqual* _temp3082=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp3082->q_const= 0;
_temp3082->q_volatile= 0; _temp3082->q_restrict= 1; _temp3082;}); _temp3081;});
_temp3080;}); break; case 74: _LL3079: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp3084=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3084[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3085; _temp3085.tag= Cyc_TypeSpecifier_tok_tag;
_temp3085.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3086=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3086[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3087; _temp3087.tag= Cyc_Parse_Decl_spec_tag;
_temp3087.f1=({ struct Cyc_Absyn_Decl* _temp3088=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp3088->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp3095=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp3095[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp3096; _temp3096.tag= Cyc_Absyn_Enum_d_tag; _temp3096.f1=({ struct Cyc_Absyn_Enumdecl*
_temp3097=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp3097->sc=( void*)(( void*) Cyc_Absyn_Public); _temp3097->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3101= yyvs; struct _xtunion_struct** _temp3103=
_temp3101.curr +( yyvsp_offset - 3); if( _temp3103 < _temp3101.base? 1:
_temp3103 >= _temp3101.last_plus_one){ _throw( Null_Exception);}* _temp3103;}));
_temp3097->fields= Cyc_yyget_EnumfieldList_tok(({ struct _tagged_ptr2 _temp3098=
yyvs; struct _xtunion_struct** _temp3100= _temp3098.curr +( yyvsp_offset - 1);
if( _temp3100 < _temp3098.base? 1: _temp3100 >= _temp3098.last_plus_one){ _throw(
Null_Exception);}* _temp3100;})); _temp3097;}); _temp3096;}); _temp3095;}));
_temp3088->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3089=
yyls; struct Cyc_Yyltype* _temp3091= _temp3089.curr +( yylsp_offset - 4); if(
_temp3091 < _temp3089.base? 1: _temp3091 >= _temp3089.last_plus_one){ _throw(
Null_Exception);}* _temp3091;}).first_line,({ struct _tagged_ptr3 _temp3092=
yyls; struct Cyc_Yyltype* _temp3094= _temp3092.curr + yylsp_offset; if(
_temp3094 < _temp3092.base? 1: _temp3094 >= _temp3092.last_plus_one){ _throw(
Null_Exception);}* _temp3094;}).last_line); _temp3088;}); _temp3087;});
_temp3086;})); _temp3085;}); _temp3084;}); break; case 75: _LL3083: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3105=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3105[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3106; _temp3106.tag=
Cyc_TypeSpecifier_tok_tag; _temp3106.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp3107=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3107[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp3108; _temp3108.tag= Cyc_Absyn_EnumType_tag;
_temp3108.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3109= yyvs;
struct _xtunion_struct** _temp3111= _temp3109.curr + yyvsp_offset; if( _temp3111
< _temp3109.base? 1: _temp3111 >= _temp3109.last_plus_one){ _throw(
Null_Exception);}* _temp3111;})); _temp3108.f2= 0; _temp3108;}); _temp3107;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3112= yyls; struct Cyc_Yyltype*
_temp3114= _temp3112.curr +( yylsp_offset - 1); if( _temp3114 < _temp3112.base?
1: _temp3114 >= _temp3112.last_plus_one){ _throw( Null_Exception);}* _temp3114;}).first_line,({
struct _tagged_ptr3 _temp3115= yyls; struct Cyc_Yyltype* _temp3117= _temp3115.curr
+ yylsp_offset; if( _temp3117 < _temp3115.base? 1: _temp3117 >= _temp3115.last_plus_one){
_throw( Null_Exception);}* _temp3117;}).last_line)); _temp3106;}); _temp3105;});
break; case 76: _LL3104: yyval=( struct _xtunion_struct*)({ struct Cyc_Enumfield_tok_struct*
_temp3119=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp3119[ 0]=({ struct Cyc_Enumfield_tok_struct _temp3120; _temp3120.tag= Cyc_Enumfield_tok_tag;
_temp3120.f1=({ struct Cyc_Absyn_Enumfield* _temp3121=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3121->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3128= yyvs; struct _xtunion_struct** _temp3130=
_temp3128.curr + yyvsp_offset; if( _temp3130 < _temp3128.base? 1: _temp3130 >=
_temp3128.last_plus_one){ _throw( Null_Exception);}* _temp3130;})); _temp3121->tag=
0; _temp3121->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3122=
yyls; struct Cyc_Yyltype* _temp3124= _temp3122.curr + yylsp_offset; if(
_temp3124 < _temp3122.base? 1: _temp3124 >= _temp3122.last_plus_one){ _throw(
Null_Exception);}* _temp3124;}).first_line,({ struct _tagged_ptr3 _temp3125=
yyls; struct Cyc_Yyltype* _temp3127= _temp3125.curr + yylsp_offset; if(
_temp3127 < _temp3125.base? 1: _temp3127 >= _temp3125.last_plus_one){ _throw(
Null_Exception);}* _temp3127;}).last_line); _temp3121;}); _temp3120;});
_temp3119;}); break; case 77: _LL3118: yyval=( struct _xtunion_struct*)({ struct
Cyc_Enumfield_tok_struct* _temp3132=( struct Cyc_Enumfield_tok_struct*)
GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp3132[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp3133; _temp3133.tag= Cyc_Enumfield_tok_tag;
_temp3133.f1=({ struct Cyc_Absyn_Enumfield* _temp3134=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3134->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3144= yyvs; struct _xtunion_struct** _temp3146=
_temp3144.curr +( yyvsp_offset - 2); if( _temp3146 < _temp3144.base? 1:
_temp3146 >= _temp3144.last_plus_one){ _throw( Null_Exception);}* _temp3146;}));
_temp3134->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp3141= yyvs; struct _xtunion_struct** _temp3143= _temp3141.curr +
yyvsp_offset; if( _temp3143 < _temp3141.base? 1: _temp3143 >= _temp3141.last_plus_one){
_throw( Null_Exception);}* _temp3143;})); _temp3134->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3135= yyls; struct Cyc_Yyltype* _temp3137= _temp3135.curr
+( yylsp_offset - 2); if( _temp3137 < _temp3135.base? 1: _temp3137 >= _temp3135.last_plus_one){
_throw( Null_Exception);}* _temp3137;}).first_line,({ struct _tagged_ptr3
_temp3138= yyls; struct Cyc_Yyltype* _temp3140= _temp3138.curr + yylsp_offset;
if( _temp3140 < _temp3138.base? 1: _temp3140 >= _temp3138.last_plus_one){ _throw(
Null_Exception);}* _temp3140;}).last_line); _temp3134;}); _temp3133;});
_temp3132;}); break; case 78: _LL3131: yyval=( struct _xtunion_struct*)({ struct
Cyc_EnumfieldList_tok_struct* _temp3148=( struct Cyc_EnumfieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct)); _temp3148[ 0]=({
struct Cyc_EnumfieldList_tok_struct _temp3149; _temp3149.tag= Cyc_EnumfieldList_tok_tag;
_temp3149.f1=({ struct Cyc_List_List* _temp3150=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3150->hd=( void*) Cyc_yyget_Enumfield_tok(({
struct _tagged_ptr2 _temp3151= yyvs; struct _xtunion_struct** _temp3153=
_temp3151.curr + yyvsp_offset; if( _temp3153 < _temp3151.base? 1: _temp3153 >=
_temp3151.last_plus_one){ _throw( Null_Exception);}* _temp3153;})); _temp3150->tl=
0; _temp3150;}); _temp3149;}); _temp3148;}); break; case 79: _LL3147: yyval=(
struct _xtunion_struct*)({ struct Cyc_EnumfieldList_tok_struct* _temp3155=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp3155[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp3156; _temp3156.tag=
Cyc_EnumfieldList_tok_tag; _temp3156.f1=({ struct Cyc_List_List* _temp3157=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3157->hd=(
void*) Cyc_yyget_Enumfield_tok(({ struct _tagged_ptr2 _temp3161= yyvs; struct
_xtunion_struct** _temp3163= _temp3161.curr +( yyvsp_offset - 2); if( _temp3163
< _temp3161.base? 1: _temp3163 >= _temp3161.last_plus_one){ _throw(
Null_Exception);}* _temp3163;})); _temp3157->tl= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3158= yyvs; struct _xtunion_struct** _temp3160=
_temp3158.curr + yyvsp_offset; if( _temp3160 < _temp3158.base? 1: _temp3160 >=
_temp3158.last_plus_one){ _throw( Null_Exception);}* _temp3160;})); _temp3157;});
_temp3156;}); _temp3155;}); break; case 80: _LL3154: { struct Cyc_Absyn_Decl* d;{
void* _temp3168= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3165=
yyvs; struct _xtunion_struct** _temp3167= _temp3165.curr +( yyvsp_offset - 3);
if( _temp3167 < _temp3165.base? 1: _temp3167 >= _temp3165.last_plus_one){ _throw(
Null_Exception);}* _temp3167;})); _LL3170: if(( int) _temp3168 == Cyc_Parse_Struct_su){
goto _LL3171;} else{ goto _LL3172;} _LL3172: if(( int) _temp3168 == Cyc_Parse_Union_su){
goto _LL3173;} else{ goto _LL3169;} _LL3171: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public, 0, 0,({ struct Cyc_Core_Opt* _temp3174=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3174->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3175= yyvs; struct _xtunion_struct** _temp3177=
_temp3175.curr +( yyvsp_offset - 1); if( _temp3177 < _temp3175.base? 1:
_temp3177 >= _temp3175.last_plus_one){ _throw( Null_Exception);}* _temp3177;}));
_temp3174;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3178=
yyls; struct Cyc_Yyltype* _temp3180= _temp3178.curr +( yylsp_offset - 3); if(
_temp3180 < _temp3178.base? 1: _temp3180 >= _temp3178.last_plus_one){ _throw(
Null_Exception);}* _temp3180;}).first_line,({ struct _tagged_ptr3 _temp3181=
yyls; struct Cyc_Yyltype* _temp3183= _temp3181.curr + yylsp_offset; if(
_temp3183 < _temp3181.base? 1: _temp3183 >= _temp3181.last_plus_one){ _throw(
Null_Exception);}* _temp3183;}).last_line)); goto _LL3169; _LL3173: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public, 0, 0,({ struct Cyc_Core_Opt* _temp3184=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3184->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3185= yyvs; struct _xtunion_struct** _temp3187=
_temp3185.curr +( yyvsp_offset - 1); if( _temp3187 < _temp3185.base? 1:
_temp3187 >= _temp3185.last_plus_one){ _throw( Null_Exception);}* _temp3187;}));
_temp3184;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3188=
yyls; struct Cyc_Yyltype* _temp3190= _temp3188.curr +( yylsp_offset - 3); if(
_temp3190 < _temp3188.base? 1: _temp3190 >= _temp3188.last_plus_one){ _throw(
Null_Exception);}* _temp3190;}).first_line,({ struct _tagged_ptr3 _temp3191=
yyls; struct Cyc_Yyltype* _temp3193= _temp3191.curr + yylsp_offset; if(
_temp3193 < _temp3191.base? 1: _temp3193 >= _temp3191.last_plus_one){ _throw(
Null_Exception);}* _temp3193;}).last_line)); goto _LL3169; _LL3169:;} yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3194=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3194[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3195; _temp3195.tag=
Cyc_TypeSpecifier_tok_tag; _temp3195.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3196=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3196[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3197; _temp3197.tag= Cyc_Parse_Decl_spec_tag;
_temp3197.f1= d; _temp3197;}); _temp3196;})); _temp3195;}); _temp3194;}); Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp3198=( char*)"anonymous structs/unions";
struct _tagged_string _temp3199; _temp3199.curr= _temp3198; _temp3199.base=
_temp3198; _temp3199.last_plus_one= _temp3198 + 25; _temp3199;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3200= yyls; struct Cyc_Yyltype* _temp3202= _temp3200.curr
+( yylsp_offset - 3); if( _temp3202 < _temp3200.base? 1: _temp3202 >= _temp3200.last_plus_one){
_throw( Null_Exception);}* _temp3202;}).first_line,({ struct _tagged_ptr3
_temp3203= yyls; struct Cyc_Yyltype* _temp3205= _temp3203.curr + yylsp_offset;
if( _temp3205 < _temp3203.base? 1: _temp3205 >= _temp3203.last_plus_one){ _throw(
Null_Exception);}* _temp3205;}).last_line)); break;} case 81: _LL3164: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3207= yyls; struct Cyc_Yyltype* _temp3209= _temp3207.curr
+( yylsp_offset - 3); if( _temp3209 < _temp3207.base? 1: _temp3209 >= _temp3207.last_plus_one){
_throw( Null_Exception);}* _temp3209;}).first_line,({ struct _tagged_ptr3
_temp3210= yyls; struct Cyc_Yyltype* _temp3212= _temp3210.curr +( yylsp_offset -
3); if( _temp3212 < _temp3210.base? 1: _temp3212 >= _temp3210.last_plus_one){
_throw( Null_Exception);}* _temp3212;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3213= yyvs; struct _xtunion_struct** _temp3215=
_temp3213.curr +( yyvsp_offset - 3); if( _temp3215 < _temp3213.base? 1:
_temp3215 >= _temp3213.last_plus_one){ _throw( Null_Exception);}* _temp3215;})));
struct Cyc_Absyn_Decl* d;{ void* _temp3219= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp3216= yyvs; struct _xtunion_struct** _temp3218=
_temp3216.curr +( yyvsp_offset - 5); if( _temp3218 < _temp3216.base? 1:
_temp3218 >= _temp3216.last_plus_one){ _throw( Null_Exception);}* _temp3218;}));
_LL3221: if(( int) _temp3219 == Cyc_Parse_Struct_su){ goto _LL3222;} else{ goto
_LL3223;} _LL3223: if(( int) _temp3219 == Cyc_Parse_Union_su){ goto _LL3224;}
else{ goto _LL3220;} _LL3222: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3225=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3225->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3226= yyvs; struct _xtunion_struct** _temp3228= _temp3226.curr
+( yyvsp_offset - 4); if( _temp3228 < _temp3226.base? 1: _temp3228 >= _temp3226.last_plus_one){
_throw( Null_Exception);}* _temp3228;})); _temp3225;}), ts,({ struct Cyc_Core_Opt*
_temp3229=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3229->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3230= yyvs; struct _xtunion_struct** _temp3232= _temp3230.curr +(
yyvsp_offset - 1); if( _temp3232 < _temp3230.base? 1: _temp3232 >= _temp3230.last_plus_one){
_throw( Null_Exception);}* _temp3232;})); _temp3229;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3233= yyls; struct Cyc_Yyltype* _temp3235= _temp3233.curr
+( yylsp_offset - 5); if( _temp3235 < _temp3233.base? 1: _temp3235 >= _temp3233.last_plus_one){
_throw( Null_Exception);}* _temp3235;}).first_line,({ struct _tagged_ptr3
_temp3236= yyls; struct Cyc_Yyltype* _temp3238= _temp3236.curr + yylsp_offset;
if( _temp3238 < _temp3236.base? 1: _temp3238 >= _temp3236.last_plus_one){ _throw(
Null_Exception);}* _temp3238;}).last_line)); goto _LL3220; _LL3224: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3239=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3239->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3240= yyvs; struct _xtunion_struct** _temp3242=
_temp3240.curr +( yyvsp_offset - 4); if( _temp3242 < _temp3240.base? 1:
_temp3242 >= _temp3240.last_plus_one){ _throw( Null_Exception);}* _temp3242;}));
_temp3239;}), ts,({ struct Cyc_Core_Opt* _temp3243=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3243->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3244= yyvs; struct _xtunion_struct** _temp3246=
_temp3244.curr +( yyvsp_offset - 1); if( _temp3246 < _temp3244.base? 1:
_temp3246 >= _temp3244.last_plus_one){ _throw( Null_Exception);}* _temp3246;}));
_temp3243;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3247=
yyls; struct Cyc_Yyltype* _temp3249= _temp3247.curr +( yylsp_offset - 5); if(
_temp3249 < _temp3247.base? 1: _temp3249 >= _temp3247.last_plus_one){ _throw(
Null_Exception);}* _temp3249;}).first_line,({ struct _tagged_ptr3 _temp3250=
yyls; struct Cyc_Yyltype* _temp3252= _temp3250.curr + yylsp_offset; if(
_temp3252 < _temp3250.base? 1: _temp3252 >= _temp3250.last_plus_one){ _throw(
Null_Exception);}* _temp3252;}).last_line)); goto _LL3220; _LL3220:;} yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3253=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3253[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3254; _temp3254.tag=
Cyc_TypeSpecifier_tok_tag; _temp3254.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3255=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3255[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3256; _temp3256.tag= Cyc_Parse_Decl_spec_tag;
_temp3256.f1= d; _temp3256;}); _temp3255;})); _temp3254;}); _temp3253;}); break;}
case 82: _LL3206: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3258= yyls; struct Cyc_Yyltype* _temp3260= _temp3258.curr
+( yylsp_offset - 3); if( _temp3260 < _temp3258.base? 1: _temp3260 >= _temp3258.last_plus_one){
_throw( Null_Exception);}* _temp3260;}).first_line,({ struct _tagged_ptr3
_temp3261= yyls; struct Cyc_Yyltype* _temp3263= _temp3261.curr +( yylsp_offset -
3); if( _temp3263 < _temp3261.base? 1: _temp3263 >= _temp3261.last_plus_one){
_throw( Null_Exception);}* _temp3263;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3264= yyvs; struct _xtunion_struct** _temp3266=
_temp3264.curr +( yyvsp_offset - 3); if( _temp3266 < _temp3264.base? 1:
_temp3266 >= _temp3264.last_plus_one){ _throw( Null_Exception);}* _temp3266;})));
struct Cyc_Absyn_Decl* d;{ void* _temp3270= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp3267= yyvs; struct _xtunion_struct** _temp3269=
_temp3267.curr +( yyvsp_offset - 5); if( _temp3269 < _temp3267.base? 1:
_temp3269 >= _temp3267.last_plus_one){ _throw( Null_Exception);}* _temp3269;}));
_LL3272: if(( int) _temp3270 == Cyc_Parse_Struct_su){ goto _LL3273;} else{ goto
_LL3274;} _LL3274: if(( int) _temp3270 == Cyc_Parse_Union_su){ goto _LL3275;}
else{ goto _LL3271;} _LL3273: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3276=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3276->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3277= yyvs; struct _xtunion_struct** _temp3279= _temp3277.curr
+( yyvsp_offset - 4); if( _temp3279 < _temp3277.base? 1: _temp3279 >= _temp3277.last_plus_one){
_throw( Null_Exception);}* _temp3279;})); _temp3276;}), ts,({ struct Cyc_Core_Opt*
_temp3280=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3280->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3281= yyvs; struct _xtunion_struct** _temp3283= _temp3281.curr +(
yyvsp_offset - 1); if( _temp3283 < _temp3281.base? 1: _temp3283 >= _temp3281.last_plus_one){
_throw( Null_Exception);}* _temp3283;})); _temp3280;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3284= yyls; struct Cyc_Yyltype* _temp3286= _temp3284.curr
+( yylsp_offset - 5); if( _temp3286 < _temp3284.base? 1: _temp3286 >= _temp3284.last_plus_one){
_throw( Null_Exception);}* _temp3286;}).first_line,({ struct _tagged_ptr3
_temp3287= yyls; struct Cyc_Yyltype* _temp3289= _temp3287.curr + yylsp_offset;
if( _temp3289 < _temp3287.base? 1: _temp3289 >= _temp3287.last_plus_one){ _throw(
Null_Exception);}* _temp3289;}).last_line)); goto _LL3271; _LL3275: d= Cyc_Absyn_union_decl((
void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3290=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3290->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3291= yyvs; struct _xtunion_struct** _temp3293=
_temp3291.curr +( yyvsp_offset - 4); if( _temp3293 < _temp3291.base? 1:
_temp3293 >= _temp3291.last_plus_one){ _throw( Null_Exception);}* _temp3293;}));
_temp3290;}), ts,({ struct Cyc_Core_Opt* _temp3294=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3294->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3295= yyvs; struct _xtunion_struct** _temp3297=
_temp3295.curr +( yyvsp_offset - 1); if( _temp3297 < _temp3295.base? 1:
_temp3297 >= _temp3295.last_plus_one){ _throw( Null_Exception);}* _temp3297;}));
_temp3294;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3298=
yyls; struct Cyc_Yyltype* _temp3300= _temp3298.curr +( yylsp_offset - 5); if(
_temp3300 < _temp3298.base? 1: _temp3300 >= _temp3298.last_plus_one){ _throw(
Null_Exception);}* _temp3300;}).first_line,({ struct _tagged_ptr3 _temp3301=
yyls; struct Cyc_Yyltype* _temp3303= _temp3301.curr + yylsp_offset; if(
_temp3303 < _temp3301.base? 1: _temp3303 >= _temp3301.last_plus_one){ _throw(
Null_Exception);}* _temp3303;}).last_line)); goto _LL3271; _LL3271:;} yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3304=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3304[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3305; _temp3305.tag=
Cyc_TypeSpecifier_tok_tag; _temp3305.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3306=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3306[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3307; _temp3307.tag= Cyc_Parse_Decl_spec_tag;
_temp3307.f1= d; _temp3307;}); _temp3306;})); _temp3305;}); _temp3304;}); break;}
case 83: _LL3257:{ void* _temp3312= Cyc_yyget_StructOrUnion_tok(({ struct
_tagged_ptr2 _temp3309= yyvs; struct _xtunion_struct** _temp3311= _temp3309.curr
+( yyvsp_offset - 2); if( _temp3311 < _temp3309.base? 1: _temp3311 >= _temp3309.last_plus_one){
_throw( Null_Exception);}* _temp3311;})); _LL3314: if(( int) _temp3312 == Cyc_Parse_Struct_su){
goto _LL3315;} else{ goto _LL3316;} _LL3316: if(( int) _temp3312 == Cyc_Parse_Union_su){
goto _LL3317;} else{ goto _LL3313;} _LL3315: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeSpecifier_tok_struct* _temp3318=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3318[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3319; _temp3319.tag= Cyc_TypeSpecifier_tok_tag;
_temp3319.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_StructType_struct*
_temp3320=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp3320[ 0]=({ struct Cyc_Absyn_StructType_struct _temp3321; _temp3321.tag=
Cyc_Absyn_StructType_tag; _temp3321.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3322= yyvs; struct _xtunion_struct** _temp3324=
_temp3322.curr +( yyvsp_offset - 1); if( _temp3324 < _temp3322.base? 1:
_temp3324 >= _temp3322.last_plus_one){ _throw( Null_Exception);}* _temp3324;}));
_temp3321.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3325= yyvs;
struct _xtunion_struct** _temp3327= _temp3325.curr + yyvsp_offset; if( _temp3327
< _temp3325.base? 1: _temp3327 >= _temp3325.last_plus_one){ _throw(
Null_Exception);}* _temp3327;})); _temp3321.f3= 0; _temp3321;}); _temp3320;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3328= yyls; struct Cyc_Yyltype*
_temp3330= _temp3328.curr +( yylsp_offset - 2); if( _temp3330 < _temp3328.base?
1: _temp3330 >= _temp3328.last_plus_one){ _throw( Null_Exception);}* _temp3330;}).first_line,({
struct _tagged_ptr3 _temp3331= yyls; struct Cyc_Yyltype* _temp3333= _temp3331.curr
+ yylsp_offset; if( _temp3333 < _temp3331.base? 1: _temp3333 >= _temp3331.last_plus_one){
_throw( Null_Exception);}* _temp3333;}).last_line)); _temp3319;}); _temp3318;});
goto _LL3313; _LL3317: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3334=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3334[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3335; _temp3335.tag=
Cyc_TypeSpecifier_tok_tag; _temp3335.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp3336=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3336[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp3337; _temp3337.tag= Cyc_Absyn_UnionType_tag;
_temp3337.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3338= yyvs; struct _xtunion_struct** _temp3340= _temp3338.curr +(
yyvsp_offset - 1); if( _temp3340 < _temp3338.base? 1: _temp3340 >= _temp3338.last_plus_one){
_throw( Null_Exception);}* _temp3340;})); _temp3337.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3341= yyvs; struct _xtunion_struct** _temp3343=
_temp3341.curr + yyvsp_offset; if( _temp3343 < _temp3341.base? 1: _temp3343 >=
_temp3341.last_plus_one){ _throw( Null_Exception);}* _temp3343;})); _temp3337.f3=
0; _temp3337;}); _temp3336;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3344= yyls; struct Cyc_Yyltype* _temp3346= _temp3344.curr +(
yylsp_offset - 2); if( _temp3346 < _temp3344.base? 1: _temp3346 >= _temp3344.last_plus_one){
_throw( Null_Exception);}* _temp3346;}).first_line,({ struct _tagged_ptr3
_temp3347= yyls; struct Cyc_Yyltype* _temp3349= _temp3347.curr + yylsp_offset;
if( _temp3349 < _temp3347.base? 1: _temp3349 >= _temp3347.last_plus_one){ _throw(
Null_Exception);}* _temp3349;}).last_line)); _temp3335;}); _temp3334;}); goto
_LL3313; _LL3313:;} break; case 84: _LL3308:{ void* _temp3354= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp3351= yyvs; struct _xtunion_struct** _temp3353=
_temp3351.curr +( yyvsp_offset - 2); if( _temp3353 < _temp3351.base? 1:
_temp3353 >= _temp3351.last_plus_one){ _throw( Null_Exception);}* _temp3353;}));
_LL3356: if(( int) _temp3354 == Cyc_Parse_Struct_su){ goto _LL3357;} else{ goto
_LL3358;} _LL3358: if(( int) _temp3354 == Cyc_Parse_Union_su){ goto _LL3359;}
else{ goto _LL3355;} _LL3357: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3360=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3360[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3361; _temp3361.tag=
Cyc_TypeSpecifier_tok_tag; _temp3361.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3362=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3362[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3363; _temp3363.tag= Cyc_Absyn_StructType_tag;
_temp3363.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3364= yyvs; struct _xtunion_struct** _temp3366= _temp3364.curr +(
yyvsp_offset - 1); if( _temp3366 < _temp3364.base? 1: _temp3366 >= _temp3364.last_plus_one){
_throw( Null_Exception);}* _temp3366;})); _temp3363.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3367= yyvs; struct _xtunion_struct** _temp3369=
_temp3367.curr + yyvsp_offset; if( _temp3369 < _temp3367.base? 1: _temp3369 >=
_temp3367.last_plus_one){ _throw( Null_Exception);}* _temp3369;})); _temp3363.f3=
0; _temp3363;}); _temp3362;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3370= yyls; struct Cyc_Yyltype* _temp3372= _temp3370.curr +(
yylsp_offset - 2); if( _temp3372 < _temp3370.base? 1: _temp3372 >= _temp3370.last_plus_one){
_throw( Null_Exception);}* _temp3372;}).first_line,({ struct _tagged_ptr3
_temp3373= yyls; struct Cyc_Yyltype* _temp3375= _temp3373.curr + yylsp_offset;
if( _temp3375 < _temp3373.base? 1: _temp3375 >= _temp3373.last_plus_one){ _throw(
Null_Exception);}* _temp3375;}).last_line)); _temp3361;}); _temp3360;}); goto
_LL3355; _LL3359: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3376=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3376[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3377; _temp3377.tag=
Cyc_TypeSpecifier_tok_tag; _temp3377.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp3378=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3378[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp3379; _temp3379.tag= Cyc_Absyn_UnionType_tag;
_temp3379.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3380= yyvs; struct _xtunion_struct** _temp3382= _temp3380.curr +(
yyvsp_offset - 1); if( _temp3382 < _temp3380.base? 1: _temp3382 >= _temp3380.last_plus_one){
_throw( Null_Exception);}* _temp3382;})); _temp3379.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3383= yyvs; struct _xtunion_struct** _temp3385=
_temp3383.curr + yyvsp_offset; if( _temp3385 < _temp3383.base? 1: _temp3385 >=
_temp3383.last_plus_one){ _throw( Null_Exception);}* _temp3385;})); _temp3379.f3=
0; _temp3379;}); _temp3378;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3386= yyls; struct Cyc_Yyltype* _temp3388= _temp3386.curr +(
yylsp_offset - 2); if( _temp3388 < _temp3386.base? 1: _temp3388 >= _temp3386.last_plus_one){
_throw( Null_Exception);}* _temp3388;}).first_line,({ struct _tagged_ptr3
_temp3389= yyls; struct Cyc_Yyltype* _temp3391= _temp3389.curr + yylsp_offset;
if( _temp3391 < _temp3389.base? 1: _temp3391 >= _temp3389.last_plus_one){ _throw(
Null_Exception);}* _temp3391;}).last_line)); _temp3377;}); _temp3376;}); goto
_LL3355; _LL3355:;} break; case 85: _LL3350: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp3393=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3393[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3394; _temp3394.tag= Cyc_TypeList_tok_tag; _temp3394.f1= 0; _temp3394;});
_temp3393;}); break; case 86: _LL3392: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeList_tok_struct* _temp3396=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3396[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3397; _temp3397.tag= Cyc_TypeList_tok_tag; _temp3397.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3398= yyvs; struct _xtunion_struct** _temp3400= _temp3398.curr
+( yyvsp_offset - 1); if( _temp3400 < _temp3398.base? 1: _temp3400 >= _temp3398.last_plus_one){
_throw( Null_Exception);}* _temp3400;}))); _temp3397;}); _temp3396;}); break;
case 87: _LL3395:(( struct Cyc_Lexing_lexbuf*) Cyc_Parse_lbuf->v)->lex_curr_pos
-= 1; yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp3402=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3402[ 0]=({ struct Cyc_TypeList_tok_struct _temp3403; _temp3403.tag= Cyc_TypeList_tok_tag;
_temp3403.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3404= yyvs; struct
_xtunion_struct** _temp3406= _temp3404.curr +( yyvsp_offset - 1); if( _temp3406
< _temp3404.base? 1: _temp3406 >= _temp3404.last_plus_one){ _throw(
Null_Exception);}* _temp3406;}))); _temp3403;}); _temp3402;}); break; case 88:
_LL3401: yyval=( struct _xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3408=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3408[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3409; _temp3409.tag=
Cyc_StructOrUnion_tok_tag; _temp3409.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp3409;}); _temp3408;}); break; case 89: _LL3407: yyval=( struct
_xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct* _temp3411=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct)); _temp3411[ 0]=({
struct Cyc_StructOrUnion_tok_struct _temp3412; _temp3412.tag= Cyc_StructOrUnion_tok_tag;
_temp3412.f1=( void*)(( void*) Cyc_Parse_Union_su); _temp3412;}); _temp3411;});
break; case 90: _LL3410: yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3414=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3414[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3415; _temp3415.tag= Cyc_StructFieldDeclList_tok_tag; _temp3415.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3416= yyvs; struct _xtunion_struct** _temp3418=
_temp3416.curr + yyvsp_offset; if( _temp3418 < _temp3416.base? 1: _temp3418 >=
_temp3416.last_plus_one){ _throw( Null_Exception);}* _temp3418;})))); _temp3415;});
_temp3414;}); break; case 91: _LL3413: yyval=( struct _xtunion_struct*)({ struct
Cyc_StructFieldDeclListList_tok_struct* _temp3420=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp3420[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp3421; _temp3421.tag= Cyc_StructFieldDeclListList_tok_tag;
_temp3421.f1=({ struct Cyc_List_List* _temp3422=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3422->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3423= yyvs; struct _xtunion_struct** _temp3425=
_temp3423.curr + yyvsp_offset; if( _temp3425 < _temp3423.base? 1: _temp3425 >=
_temp3423.last_plus_one){ _throw( Null_Exception);}* _temp3425;})); _temp3422->tl=
0; _temp3422;}); _temp3421;}); _temp3420;}); break; case 92: _LL3419: yyval=(
struct _xtunion_struct*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3427=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3427[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3428; _temp3428.tag= Cyc_StructFieldDeclListList_tok_tag; _temp3428.f1=({
struct Cyc_List_List* _temp3429=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3429->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3433= yyvs; struct _xtunion_struct** _temp3435=
_temp3433.curr + yyvsp_offset; if( _temp3435 < _temp3433.base? 1: _temp3435 >=
_temp3433.last_plus_one){ _throw( Null_Exception);}* _temp3435;})); _temp3429->tl=
Cyc_yyget_StructFieldDeclListList_tok(({ struct _tagged_ptr2 _temp3430= yyvs;
struct _xtunion_struct** _temp3432= _temp3430.curr +( yyvsp_offset - 1); if(
_temp3432 < _temp3430.base? 1: _temp3432 >= _temp3430.last_plus_one){ _throw(
Null_Exception);}* _temp3432;})); _temp3429;}); _temp3428;}); _temp3427;});
break; case 93: _LL3426: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct*
_temp3437=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3437[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3438; _temp3438.tag=
Cyc_InitDeclList_tok_tag; _temp3438.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2
_temp3439= yyvs; struct _xtunion_struct** _temp3441= _temp3439.curr +
yyvsp_offset; if( _temp3441 < _temp3439.base? 1: _temp3441 >= _temp3439.last_plus_one){
_throw( Null_Exception);}* _temp3441;}))); _temp3438;}); _temp3437;}); break;
case 94: _LL3436: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct*
_temp3443=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3443[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3444; _temp3444.tag=
Cyc_InitDeclList_tok_tag; _temp3444.f1=({ struct Cyc_List_List* _temp3445=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3445->hd=(
void*) Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3446= yyvs; struct
_xtunion_struct** _temp3448= _temp3446.curr + yyvsp_offset; if( _temp3448 <
_temp3446.base? 1: _temp3448 >= _temp3446.last_plus_one){ _throw( Null_Exception);}*
_temp3448;})); _temp3445->tl= 0; _temp3445;}); _temp3444;}); _temp3443;});
break; case 95: _LL3442: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct*
_temp3450=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3450[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3451; _temp3451.tag=
Cyc_InitDeclList_tok_tag; _temp3451.f1=({ struct Cyc_List_List* _temp3452=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3452->hd=(
void*) Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3456= yyvs; struct
_xtunion_struct** _temp3458= _temp3456.curr + yyvsp_offset; if( _temp3458 <
_temp3456.base? 1: _temp3458 >= _temp3456.last_plus_one){ _throw( Null_Exception);}*
_temp3458;})); _temp3452->tl= Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2
_temp3453= yyvs; struct _xtunion_struct** _temp3455= _temp3453.curr +(
yyvsp_offset - 2); if( _temp3455 < _temp3453.base? 1: _temp3455 >= _temp3453.last_plus_one){
_throw( Null_Exception);}* _temp3455;})); _temp3452;}); _temp3451;}); _temp3450;});
break; case 96: _LL3449: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDecl_tok_struct*
_temp3460=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3460[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3461; _temp3461.tag= Cyc_InitDecl_tok_tag;
_temp3461.f1=({ struct _tuple13* _temp3462=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3462->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3463= yyvs; struct _xtunion_struct** _temp3465= _temp3463.curr
+ yyvsp_offset; if( _temp3465 < _temp3463.base? 1: _temp3465 >= _temp3463.last_plus_one){
_throw( Null_Exception);}* _temp3465;})); _temp3462->f2= 0; _temp3462;});
_temp3461;}); _temp3460;}); break; case 97: _LL3459: yyval=( struct
_xtunion_struct*)({ struct Cyc_InitDecl_tok_struct* _temp3467=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp3467[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp3468; _temp3468.tag= Cyc_InitDecl_tok_tag; _temp3468.f1=({ struct _tuple13*
_temp3469=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp3469->f1=
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3473= yyvs; struct
_xtunion_struct** _temp3475= _temp3473.curr +( yyvsp_offset - 2); if( _temp3475
< _temp3473.base? 1: _temp3475 >= _temp3473.last_plus_one){ _throw(
Null_Exception);}* _temp3475;})); _temp3469->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3470= yyvs; struct _xtunion_struct** _temp3472=
_temp3470.curr + yyvsp_offset; if( _temp3472 < _temp3470.base? 1: _temp3472 >=
_temp3470.last_plus_one){ _throw( Null_Exception);}* _temp3472;})); _temp3469;});
_temp3468;}); _temp3467;}); break; case 98: _LL3466: { struct Cyc_Absyn_Tqual*
tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3477= yyvs; struct
_xtunion_struct** _temp3479= _temp3477.curr +( yyvsp_offset - 2); if( _temp3479
< _temp3477.base? 1: _temp3479 >= _temp3477.last_plus_one){ _throw(
Null_Exception);}* _temp3479;}))).f1; struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3480= yyvs; struct _xtunion_struct** _temp3482=
_temp3480.curr +( yyvsp_offset - 2); if( _temp3482 < _temp3480.base? 1:
_temp3482 >= _temp3480.last_plus_one){ _throw( Null_Exception);}* _temp3482;}))).f3;
void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3483= yyvs; struct _xtunion_struct** _temp3485= _temp3483.curr
+( yyvsp_offset - 2); if( _temp3485 < _temp3483.base? 1: _temp3485 >= _temp3483.last_plus_one){
_throw( Null_Exception);}* _temp3485;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3486= yyls; struct Cyc_Yyltype* _temp3488= _temp3486.curr
+( yylsp_offset - 2); if( _temp3488 < _temp3486.base? 1: _temp3488 >= _temp3486.last_plus_one){
_throw( Null_Exception);}* _temp3488;}).first_line,({ struct _tagged_ptr3
_temp3489= yyls; struct Cyc_Yyltype* _temp3491= _temp3489.curr +( yylsp_offset -
2); if( _temp3491 < _temp3489.base? 1: _temp3491 >= _temp3489.last_plus_one){
_throw( Null_Exception);}* _temp3491;}).last_line)); struct Cyc_List_List*
_temp3497; struct Cyc_List_List* _temp3499; struct _tuple0 _temp3495=(( struct
_tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3492= yyvs; struct _xtunion_struct** _temp3494=
_temp3492.curr +( yyvsp_offset - 1); if( _temp3494 < _temp3492.base? 1:
_temp3494 >= _temp3492.last_plus_one){ _throw( Null_Exception);}* _temp3494;})));
_LL3500: _temp3499= _temp3495.f1; goto _LL3498; _LL3498: _temp3497= _temp3495.f2;
goto _LL3496; _LL3496: { struct Cyc_List_List* info=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, t, _temp3499, atts), _temp3497); yyval=( struct _xtunion_struct*)({ struct
Cyc_StructFieldDeclList_tok_struct* _temp3501=( struct Cyc_StructFieldDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct)); _temp3501[ 0]=({
struct Cyc_StructFieldDeclList_tok_struct _temp3502; _temp3502.tag= Cyc_StructFieldDeclList_tok_tag;
_temp3502.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Position_Segment*, struct _tuple8*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3503= yyls; struct Cyc_Yyltype* _temp3505= _temp3503.curr
+( yylsp_offset - 2); if( _temp3505 < _temp3503.base? 1: _temp3505 >= _temp3503.last_plus_one){
_throw( Null_Exception);}* _temp3505;}).first_line,({ struct _tagged_ptr3
_temp3506= yyls; struct Cyc_Yyltype* _temp3508= _temp3506.curr +( yylsp_offset -
1); if( _temp3508 < _temp3506.base? 1: _temp3508 >= _temp3506.last_plus_one){
_throw( Null_Exception);}* _temp3508;}).last_line), info); _temp3502;});
_temp3501;}); break;}} case 99: _LL3476: yyval=( struct _xtunion_struct*)({
struct Cyc_QualSpecList_tok_struct* _temp3510=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3510[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3511; _temp3511.tag= Cyc_QualSpecList_tok_tag;
_temp3511.f1=({ struct _tuple14* _temp3512=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3512->f1= Cyc_Absyn_empty_tqual(); _temp3512->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp3516=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3516->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3517= yyvs; struct _xtunion_struct** _temp3519=
_temp3517.curr +( yyvsp_offset - 1); if( _temp3519 < _temp3517.base? 1:
_temp3519 >= _temp3517.last_plus_one){ _throw( Null_Exception);}* _temp3519;}));
_temp3516->tl= 0; _temp3516;}); _temp3512->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3513= yyvs; struct _xtunion_struct** _temp3515=
_temp3513.curr + yyvsp_offset; if( _temp3515 < _temp3513.base? 1: _temp3515 >=
_temp3513.last_plus_one){ _throw( Null_Exception);}* _temp3515;})); _temp3512;});
_temp3511;}); _temp3510;}); break; case 100: _LL3509: yyval=( struct
_xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct* _temp3521=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3521[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3522; _temp3522.tag= Cyc_QualSpecList_tok_tag;
_temp3522.f1=({ struct _tuple14* _temp3523=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3523->f1=(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3537= yyvs; struct _xtunion_struct** _temp3539= _temp3537.curr
+ yyvsp_offset; if( _temp3539 < _temp3537.base? 1: _temp3539 >= _temp3537.last_plus_one){
_throw( Null_Exception);}* _temp3539;}))).f1; _temp3523->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3530=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3530->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3534= yyvs; struct _xtunion_struct** _temp3536=
_temp3534.curr +( yyvsp_offset - 2); if( _temp3536 < _temp3534.base? 1:
_temp3536 >= _temp3534.last_plus_one){ _throw( Null_Exception);}* _temp3536;}));
_temp3530->tl=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3531=
yyvs; struct _xtunion_struct** _temp3533= _temp3531.curr + yyvsp_offset; if(
_temp3533 < _temp3531.base? 1: _temp3533 >= _temp3531.last_plus_one){ _throw(
Null_Exception);}* _temp3533;}))).f2; _temp3530;}); _temp3523->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3524= yyvs; struct _xtunion_struct** _temp3526=
_temp3524.curr +( yyvsp_offset - 1); if( _temp3526 < _temp3524.base? 1:
_temp3526 >= _temp3524.last_plus_one){ _throw( Null_Exception);}* _temp3526;})),(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3527= yyvs; struct
_xtunion_struct** _temp3529= _temp3527.curr + yyvsp_offset; if( _temp3529 <
_temp3527.base? 1: _temp3529 >= _temp3527.last_plus_one){ _throw( Null_Exception);}*
_temp3529;}))).f3); _temp3523;}); _temp3522;}); _temp3521;}); break; case 101:
_LL3520: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3541=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3541[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3542; _temp3542.tag=
Cyc_QualSpecList_tok_tag; _temp3542.f1=({ struct _tuple14* _temp3543=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3543->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3547= yyvs; struct _xtunion_struct** _temp3549=
_temp3547.curr +( yyvsp_offset - 1); if( _temp3549 < _temp3547.base? 1:
_temp3549 >= _temp3547.last_plus_one){ _throw( Null_Exception);}* _temp3549;}));
_temp3543->f2= 0; _temp3543->f3= Cyc_yyget_AttributeList_tok(({ struct
_tagged_ptr2 _temp3544= yyvs; struct _xtunion_struct** _temp3546= _temp3544.curr
+ yyvsp_offset; if( _temp3546 < _temp3544.base? 1: _temp3546 >= _temp3544.last_plus_one){
_throw( Null_Exception);}* _temp3546;})); _temp3543;}); _temp3542;}); _temp3541;});
break; case 102: _LL3540: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3551=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3551[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3552; _temp3552.tag=
Cyc_QualSpecList_tok_tag; _temp3552.f1=({ struct _tuple14* _temp3553=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3553->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3563= yyvs; struct
_xtunion_struct** _temp3565= _temp3563.curr +( yyvsp_offset - 2); if( _temp3565
< _temp3563.base? 1: _temp3565 >= _temp3563.last_plus_one){ _throw(
Null_Exception);}* _temp3565;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3566= yyvs; struct _xtunion_struct** _temp3568= _temp3566.curr
+ yyvsp_offset; if( _temp3568 < _temp3566.base? 1: _temp3568 >= _temp3566.last_plus_one){
_throw( Null_Exception);}* _temp3568;}))).f1); _temp3553->f2=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3560= yyvs; struct _xtunion_struct** _temp3562=
_temp3560.curr + yyvsp_offset; if( _temp3562 < _temp3560.base? 1: _temp3562 >=
_temp3560.last_plus_one){ _throw( Null_Exception);}* _temp3562;}))).f2;
_temp3553->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp3554= yyvs; struct _xtunion_struct** _temp3556= _temp3554.curr +(
yyvsp_offset - 1); if( _temp3556 < _temp3554.base? 1: _temp3556 >= _temp3554.last_plus_one){
_throw( Null_Exception);}* _temp3556;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3557= yyvs; struct _xtunion_struct** _temp3559= _temp3557.curr
+ yyvsp_offset; if( _temp3559 < _temp3557.base? 1: _temp3559 >= _temp3557.last_plus_one){
_throw( Null_Exception);}* _temp3559;}))).f3); _temp3553;}); _temp3552;});
_temp3551;}); break; case 103: _LL3550: yyval=( struct _xtunion_struct*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp3570=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3570[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp3571; _temp3571.tag= Cyc_DeclaratorExpoptList_tok_tag;
_temp3571.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(({ struct _tagged_ptr2 _temp3572= yyvs;
struct _xtunion_struct** _temp3574= _temp3572.curr + yyvsp_offset; if( _temp3574
< _temp3572.base? 1: _temp3574 >= _temp3572.last_plus_one){ _throw(
Null_Exception);}* _temp3574;}))); _temp3571;}); _temp3570;}); break; case 104:
_LL3569: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp3576=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3576[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp3577; _temp3577.tag= Cyc_DeclaratorExpoptList_tok_tag; _temp3577.f1=({
struct Cyc_List_List* _temp3578=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3578->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3579= yyvs; struct _xtunion_struct** _temp3581=
_temp3579.curr + yyvsp_offset; if( _temp3581 < _temp3579.base? 1: _temp3581 >=
_temp3579.last_plus_one){ _throw( Null_Exception);}* _temp3581;})); _temp3578->tl=
0; _temp3578;}); _temp3577;}); _temp3576;}); break; case 105: _LL3575: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp3583=(
struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp3583[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp3584; _temp3584.tag=
Cyc_DeclaratorExpoptList_tok_tag; _temp3584.f1=({ struct Cyc_List_List*
_temp3585=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3585->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({ struct _tagged_ptr2
_temp3589= yyvs; struct _xtunion_struct** _temp3591= _temp3589.curr +
yyvsp_offset; if( _temp3591 < _temp3589.base? 1: _temp3591 >= _temp3589.last_plus_one){
_throw( Null_Exception);}* _temp3591;})); _temp3585->tl= Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3586= yyvs; struct _xtunion_struct** _temp3588=
_temp3586.curr +( yyvsp_offset - 2); if( _temp3588 < _temp3586.base? 1:
_temp3588 >= _temp3586.last_plus_one){ _throw( Null_Exception);}* _temp3588;}));
_temp3585;}); _temp3584;}); _temp3583;}); break; case 106: _LL3582: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3593=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp3593[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp3594; _temp3594.tag=
Cyc_DeclaratorExpopt_tok_tag; _temp3594.f1=({ struct _tuple15* _temp3595=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp3595->f1= Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3596= yyvs; struct _xtunion_struct** _temp3598=
_temp3596.curr + yyvsp_offset; if( _temp3598 < _temp3596.base? 1: _temp3598 >=
_temp3596.last_plus_one){ _throw( Null_Exception);}* _temp3598;})); _temp3595->f2=
0; _temp3595;}); _temp3594;}); _temp3593;}); break; case 107: _LL3592: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3600=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp3600[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp3601; _temp3601.tag=
Cyc_DeclaratorExpopt_tok_tag; _temp3601.f1=({ struct _tuple15* _temp3602=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp3602->f1=({ struct
Cyc_Parse_Declarator* _temp3607=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3607->id=({ struct _tuple1*
_temp3608=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3608->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp3612=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp3612[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp3613; _temp3613.tag= Cyc_Absyn_Rel_n_tag; _temp3613.f1= 0; _temp3613;});
_temp3612;}); _temp3608->f2=({ struct _tagged_string* _temp3609=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3609[ 0]=(
struct _tagged_string)({ char* _temp3610=( char*)""; struct _tagged_string
_temp3611; _temp3611.curr= _temp3610; _temp3611.base= _temp3610; _temp3611.last_plus_one=
_temp3610 + 1; _temp3611;}); _temp3609;}); _temp3608;}); _temp3607->tms= 0;
_temp3607;}); _temp3602->f2=({ struct Cyc_Core_Opt* _temp3603=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3603->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3604= yyvs; struct _xtunion_struct** _temp3606=
_temp3604.curr + yyvsp_offset; if( _temp3606 < _temp3604.base? 1: _temp3606 >=
_temp3604.last_plus_one){ _throw( Null_Exception);}* _temp3606;})); _temp3603;});
_temp3602;}); _temp3601;}); _temp3600;}); break; case 108: _LL3599: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3615=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp3615[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp3616; _temp3616.tag=
Cyc_DeclaratorExpopt_tok_tag; _temp3616.f1=({ struct _tuple15* _temp3617=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp3617->f1= Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3622= yyvs; struct _xtunion_struct** _temp3624=
_temp3622.curr +( yyvsp_offset - 2); if( _temp3624 < _temp3622.base? 1:
_temp3624 >= _temp3622.last_plus_one){ _throw( Null_Exception);}* _temp3624;}));
_temp3617->f2=({ struct Cyc_Core_Opt* _temp3618=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3618->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3619= yyvs; struct _xtunion_struct** _temp3621=
_temp3619.curr + yyvsp_offset; if( _temp3621 < _temp3619.base? 1: _temp3621 >=
_temp3619.last_plus_one){ _throw( Null_Exception);}* _temp3621;})); _temp3618;});
_temp3617;}); _temp3616;}); _temp3615;}); break; case 109: _LL3614: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3626=
yyls; struct Cyc_Yyltype* _temp3628= _temp3626.curr +( yylsp_offset - 3); if(
_temp3628 < _temp3626.base? 1: _temp3628 >= _temp3626.last_plus_one){ _throw(
Null_Exception);}* _temp3628;}).first_line,({ struct _tagged_ptr3 _temp3629=
yyls; struct Cyc_Yyltype* _temp3631= _temp3629.curr +( yylsp_offset - 3); if(
_temp3631 < _temp3629.base? 1: _temp3631 >= _temp3629.last_plus_one){ _throw(
Null_Exception);}* _temp3631;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3632= yyvs; struct _xtunion_struct** _temp3634= _temp3632.curr
+( yyvsp_offset - 3); if( _temp3634 < _temp3632.base? 1: _temp3634 >= _temp3632.last_plus_one){
_throw( Null_Exception);}* _temp3634;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_TypeSpecifier_tok_struct* _temp3635=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3635[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3636; _temp3636.tag= Cyc_TypeSpecifier_tok_tag;
_temp3636.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3637=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3637[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3638; _temp3638.tag= Cyc_Parse_Decl_spec_tag;
_temp3638.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp3639=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3639->v=( void*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3640=
yyvs; struct _xtunion_struct** _temp3642= _temp3640.curr +( yyvsp_offset - 4);
if( _temp3642 < _temp3640.base? 1: _temp3642 >= _temp3640.last_plus_one){ _throw(
Null_Exception);}* _temp3642;})); _temp3639;}), ts,({ struct Cyc_Core_Opt*
_temp3643=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3643->v=( void*) Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2
_temp3644= yyvs; struct _xtunion_struct** _temp3646= _temp3644.curr +(
yyvsp_offset - 1); if( _temp3646 < _temp3644.base? 1: _temp3646 >= _temp3644.last_plus_one){
_throw( Null_Exception);}* _temp3646;})); _temp3643;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3647= yyls; struct Cyc_Yyltype* _temp3649= _temp3647.curr
+( yylsp_offset - 5); if( _temp3649 < _temp3647.base? 1: _temp3649 >= _temp3647.last_plus_one){
_throw( Null_Exception);}* _temp3649;}).first_line,({ struct _tagged_ptr3
_temp3650= yyls; struct Cyc_Yyltype* _temp3652= _temp3650.curr + yylsp_offset;
if( _temp3652 < _temp3650.base? 1: _temp3652 >= _temp3650.last_plus_one){ _throw(
Null_Exception);}* _temp3652;}).last_line)); _temp3638;}); _temp3637;}));
_temp3636;}); _temp3635;}); break;} case 110: _LL3625: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3654=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3654[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3655; _temp3655.tag= Cyc_TypeSpecifier_tok_tag;
_temp3655.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp3656=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp3656[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp3657; _temp3657.tag=
Cyc_Absyn_TunionType_tag; _temp3657.f1=({ struct Cyc_Absyn_TunionInfo _temp3658;
_temp3658.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3662= yyvs;
struct _xtunion_struct** _temp3664= _temp3662.curr +( yyvsp_offset - 1); if(
_temp3664 < _temp3662.base? 1: _temp3664 >= _temp3662.last_plus_one){ _throw(
Null_Exception);}* _temp3664;})); _temp3658.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3659= yyvs; struct _xtunion_struct** _temp3661=
_temp3659.curr + yyvsp_offset; if( _temp3661 < _temp3659.base? 1: _temp3661 >=
_temp3659.last_plus_one){ _throw( Null_Exception);}* _temp3661;})); _temp3658.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp3658.tud= 0; _temp3658;}); _temp3657;});
_temp3656;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3665= yyls;
struct Cyc_Yyltype* _temp3667= _temp3665.curr +( yylsp_offset - 2); if(
_temp3667 < _temp3665.base? 1: _temp3667 >= _temp3665.last_plus_one){ _throw(
Null_Exception);}* _temp3667;}).first_line,({ struct _tagged_ptr3 _temp3668=
yyls; struct Cyc_Yyltype* _temp3670= _temp3668.curr + yylsp_offset; if(
_temp3670 < _temp3668.base? 1: _temp3670 >= _temp3668.last_plus_one){ _throw(
Null_Exception);}* _temp3670;}).last_line)); _temp3655;}); _temp3654;}); break;
case 111: _LL3653: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3672=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3672[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3673; _temp3673.tag=
Cyc_TypeSpecifier_tok_tag; _temp3673.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp3674=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3674[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3675; _temp3675.tag= Cyc_Absyn_TunionType_tag;
_temp3675.f1=({ struct Cyc_Absyn_TunionInfo _temp3676; _temp3676.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3683= yyvs; struct _xtunion_struct** _temp3685=
_temp3683.curr +( yyvsp_offset - 1); if( _temp3685 < _temp3683.base? 1:
_temp3685 >= _temp3683.last_plus_one){ _throw( Null_Exception);}* _temp3685;}));
_temp3676.targs= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3680= yyvs;
struct _xtunion_struct** _temp3682= _temp3680.curr + yyvsp_offset; if( _temp3682
< _temp3680.base? 1: _temp3682 >= _temp3680.last_plus_one){ _throw(
Null_Exception);}* _temp3682;})); _temp3676.rgn=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3677= yyvs; struct _xtunion_struct** _temp3679=
_temp3677.curr +( yyvsp_offset - 2); if( _temp3679 < _temp3677.base? 1:
_temp3679 >= _temp3677.last_plus_one){ _throw( Null_Exception);}* _temp3679;}));
_temp3676.tud= 0; _temp3676;}); _temp3675;}); _temp3674;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3686= yyls; struct Cyc_Yyltype* _temp3688= _temp3686.curr
+( yylsp_offset - 3); if( _temp3688 < _temp3686.base? 1: _temp3688 >= _temp3686.last_plus_one){
_throw( Null_Exception);}* _temp3688;}).first_line,({ struct _tagged_ptr3
_temp3689= yyls; struct Cyc_Yyltype* _temp3691= _temp3689.curr + yylsp_offset;
if( _temp3691 < _temp3689.base? 1: _temp3691 >= _temp3689.last_plus_one){ _throw(
Null_Exception);}* _temp3691;}).last_line)); _temp3673;}); _temp3672;}); break;
case 112: _LL3671: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3693=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3693[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3694; _temp3694.tag=
Cyc_TypeSpecifier_tok_tag; _temp3694.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp3695=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp3695[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp3696; _temp3696.tag= Cyc_Absyn_TunionFieldType_tag;
_temp3696.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp3697; _temp3697.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3704= yyvs; struct _xtunion_struct** _temp3706=
_temp3704.curr +( yyvsp_offset - 3); if( _temp3706 < _temp3704.base? 1:
_temp3706 >= _temp3704.last_plus_one){ _throw( Null_Exception);}* _temp3706;}));
_temp3697.targs= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3701= yyvs;
struct _xtunion_struct** _temp3703= _temp3701.curr + yyvsp_offset; if( _temp3703
< _temp3701.base? 1: _temp3703 >= _temp3701.last_plus_one){ _throw(
Null_Exception);}* _temp3703;})); _temp3697.fname= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3698= yyvs; struct _xtunion_struct** _temp3700=
_temp3698.curr +( yyvsp_offset - 1); if( _temp3700 < _temp3698.base? 1:
_temp3700 >= _temp3698.last_plus_one){ _throw( Null_Exception);}* _temp3700;}));
_temp3697.tud= 0; _temp3697.tufd= 0; _temp3697;}); _temp3696;}); _temp3695;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3707= yyls; struct Cyc_Yyltype*
_temp3709= _temp3707.curr +( yylsp_offset - 4); if( _temp3709 < _temp3707.base?
1: _temp3709 >= _temp3707.last_plus_one){ _throw( Null_Exception);}* _temp3709;}).first_line,({
struct _tagged_ptr3 _temp3710= yyls; struct Cyc_Yyltype* _temp3712= _temp3710.curr
+ yylsp_offset; if( _temp3712 < _temp3710.base? 1: _temp3712 >= _temp3710.last_plus_one){
_throw( Null_Exception);}* _temp3712;}).last_line)); _temp3694;}); _temp3693;});
break; case 113: _LL3692: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3714=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3714[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3715; _temp3715.tag=
Cyc_TypeSpecifier_tok_tag; _temp3715.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3716=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3716[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3717; _temp3717.tag= Cyc_Parse_Decl_spec_tag;
_temp3717.f1= Cyc_Absyn_xtunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3718= yyvs; struct _xtunion_struct** _temp3720=
_temp3718.curr +( yyvsp_offset - 3); if( _temp3720 < _temp3718.base? 1:
_temp3720 >= _temp3718.last_plus_one){ _throw( Null_Exception);}* _temp3720;})),
Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2 _temp3721= yyvs; struct
_xtunion_struct** _temp3723= _temp3721.curr +( yyvsp_offset - 1); if( _temp3723
< _temp3721.base? 1: _temp3723 >= _temp3721.last_plus_one){ _throw(
Null_Exception);}* _temp3723;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3724= yyls; struct Cyc_Yyltype* _temp3726= _temp3724.curr +(
yylsp_offset - 4); if( _temp3726 < _temp3724.base? 1: _temp3726 >= _temp3724.last_plus_one){
_throw( Null_Exception);}* _temp3726;}).first_line,({ struct _tagged_ptr3
_temp3727= yyls; struct Cyc_Yyltype* _temp3729= _temp3727.curr + yylsp_offset;
if( _temp3729 < _temp3727.base? 1: _temp3729 >= _temp3727.last_plus_one){ _throw(
Null_Exception);}* _temp3729;}).last_line)); _temp3717;}); _temp3716;}));
_temp3715;}); _temp3714;}); break; case 114: _LL3713: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3731=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3731[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3732; _temp3732.tag= Cyc_TypeSpecifier_tok_tag;
_temp3732.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp3733=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp3733[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp3734; _temp3734.tag=
Cyc_Absyn_XTunionType_tag; _temp3734.f1=({ struct Cyc_Absyn_XTunionInfo
_temp3735; _temp3735.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3736=
yyvs; struct _xtunion_struct** _temp3738= _temp3736.curr + yyvsp_offset; if(
_temp3738 < _temp3736.base? 1: _temp3738 >= _temp3736.last_plus_one){ _throw(
Null_Exception);}* _temp3738;})); _temp3735.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp3735.xtud= 0; _temp3735;}); _temp3734;}); _temp3733;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3739= yyls; struct Cyc_Yyltype* _temp3741= _temp3739.curr
+( yylsp_offset - 1); if( _temp3741 < _temp3739.base? 1: _temp3741 >= _temp3739.last_plus_one){
_throw( Null_Exception);}* _temp3741;}).first_line,({ struct _tagged_ptr3
_temp3742= yyls; struct Cyc_Yyltype* _temp3744= _temp3742.curr + yylsp_offset;
if( _temp3744 < _temp3742.base? 1: _temp3744 >= _temp3742.last_plus_one){ _throw(
Null_Exception);}* _temp3744;}).last_line)); _temp3732;}); _temp3731;}); break;
case 115: _LL3730: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3746=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3746[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3747; _temp3747.tag=
Cyc_TypeSpecifier_tok_tag; _temp3747.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_XTunionType_struct* _temp3748=( struct Cyc_Absyn_XTunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct)); _temp3748[ 0]=({
struct Cyc_Absyn_XTunionType_struct _temp3749; _temp3749.tag= Cyc_Absyn_XTunionType_tag;
_temp3749.f1=({ struct Cyc_Absyn_XTunionInfo _temp3750; _temp3750.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3754= yyvs; struct _xtunion_struct** _temp3756=
_temp3754.curr + yyvsp_offset; if( _temp3756 < _temp3754.base? 1: _temp3756 >=
_temp3754.last_plus_one){ _throw( Null_Exception);}* _temp3756;})); _temp3750.rgn=(
void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp3751= yyvs; struct
_xtunion_struct** _temp3753= _temp3751.curr +( yyvsp_offset - 1); if( _temp3753
< _temp3751.base? 1: _temp3753 >= _temp3751.last_plus_one){ _throw(
Null_Exception);}* _temp3753;})); _temp3750.xtud= 0; _temp3750;}); _temp3749;});
_temp3748;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3757= yyls;
struct Cyc_Yyltype* _temp3759= _temp3757.curr +( yylsp_offset - 2); if(
_temp3759 < _temp3757.base? 1: _temp3759 >= _temp3757.last_plus_one){ _throw(
Null_Exception);}* _temp3759;}).first_line,({ struct _tagged_ptr3 _temp3760=
yyls; struct Cyc_Yyltype* _temp3762= _temp3760.curr + yylsp_offset; if(
_temp3762 < _temp3760.base? 1: _temp3762 >= _temp3760.last_plus_one){ _throw(
Null_Exception);}* _temp3762;}).last_line)); _temp3747;}); _temp3746;}); break;
case 116: _LL3745: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3764=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3764[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3765; _temp3765.tag=
Cyc_TypeSpecifier_tok_tag; _temp3765.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_XTunionFieldType_struct* _temp3766=( struct Cyc_Absyn_XTunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionFieldType_struct)); _temp3766[ 0]=({
struct Cyc_Absyn_XTunionFieldType_struct _temp3767; _temp3767.tag= Cyc_Absyn_XTunionFieldType_tag;
_temp3767.f1=({ struct Cyc_Absyn_XTunionFieldInfo _temp3768; _temp3768.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3772= yyvs; struct _xtunion_struct** _temp3774=
_temp3772.curr +( yyvsp_offset - 2); if( _temp3774 < _temp3772.base? 1:
_temp3774 >= _temp3772.last_plus_one){ _throw( Null_Exception);}* _temp3774;}));
_temp3768.fname= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3769= yyvs;
struct _xtunion_struct** _temp3771= _temp3769.curr + yyvsp_offset; if( _temp3771
< _temp3769.base? 1: _temp3771 >= _temp3769.last_plus_one){ _throw(
Null_Exception);}* _temp3771;})); _temp3768.xtud= 0; _temp3768.xtufd= 0;
_temp3768;}); _temp3767;}); _temp3766;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3775= yyls; struct Cyc_Yyltype* _temp3777= _temp3775.curr +(
yylsp_offset - 3); if( _temp3777 < _temp3775.base? 1: _temp3777 >= _temp3775.last_plus_one){
_throw( Null_Exception);}* _temp3777;}).first_line,({ struct _tagged_ptr3
_temp3778= yyls; struct Cyc_Yyltype* _temp3780= _temp3778.curr + yylsp_offset;
if( _temp3780 < _temp3778.base? 1: _temp3780 >= _temp3778.last_plus_one){ _throw(
Null_Exception);}* _temp3780;}).last_line)); _temp3765;}); _temp3764;}); break;
case 117: _LL3763: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3782=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3782[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3783; _temp3783.tag= Cyc_TunionFieldList_tok_tag; _temp3783.f1=({ struct
Cyc_List_List* _temp3784=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3784->hd=( void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2
_temp3785= yyvs; struct _xtunion_struct** _temp3787= _temp3785.curr +
yyvsp_offset; if( _temp3787 < _temp3785.base? 1: _temp3787 >= _temp3785.last_plus_one){
_throw( Null_Exception);}* _temp3787;})); _temp3784->tl= 0; _temp3784;});
_temp3783;}); _temp3782;}); break; case 118: _LL3781: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3789=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3789[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3790; _temp3790.tag=
Cyc_TunionFieldList_tok_tag; _temp3790.f1=({ struct Cyc_List_List* _temp3791=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3791->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3792= yyvs; struct
_xtunion_struct** _temp3794= _temp3792.curr +( yyvsp_offset - 1); if( _temp3794
< _temp3792.base? 1: _temp3794 >= _temp3792.last_plus_one){ _throw(
Null_Exception);}* _temp3794;})); _temp3791->tl= 0; _temp3791;}); _temp3790;});
_temp3789;}); break; case 119: _LL3788: yyval=( struct _xtunion_struct*)({
struct Cyc_TunionFieldList_tok_struct* _temp3796=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp3796[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp3797; _temp3797.tag= Cyc_TunionFieldList_tok_tag;
_temp3797.f1=({ struct Cyc_List_List* _temp3798=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3798->hd=( void*) Cyc_yyget_TunionField_tok(({
struct _tagged_ptr2 _temp3802= yyvs; struct _xtunion_struct** _temp3804=
_temp3802.curr +( yyvsp_offset - 2); if( _temp3804 < _temp3802.base? 1:
_temp3804 >= _temp3802.last_plus_one){ _throw( Null_Exception);}* _temp3804;}));
_temp3798->tl= Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2 _temp3799=
yyvs; struct _xtunion_struct** _temp3801= _temp3799.curr + yyvsp_offset; if(
_temp3801 < _temp3799.base? 1: _temp3801 >= _temp3799.last_plus_one){ _throw(
Null_Exception);}* _temp3801;})); _temp3798;}); _temp3797;}); _temp3796;});
break; case 120: _LL3795: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3806=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3806[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3807; _temp3807.tag= Cyc_TunionFieldList_tok_tag; _temp3807.f1=({ struct
Cyc_List_List* _temp3808=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3808->hd=( void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2
_temp3812= yyvs; struct _xtunion_struct** _temp3814= _temp3812.curr +(
yyvsp_offset - 2); if( _temp3814 < _temp3812.base? 1: _temp3814 >= _temp3812.last_plus_one){
_throw( Null_Exception);}* _temp3814;})); _temp3808->tl= Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3809= yyvs; struct _xtunion_struct** _temp3811=
_temp3809.curr + yyvsp_offset; if( _temp3811 < _temp3809.base? 1: _temp3811 >=
_temp3809.last_plus_one){ _throw( Null_Exception);}* _temp3811;})); _temp3808;});
_temp3807;}); _temp3806;}); break; case 121: _LL3805: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionField_tok_struct* _temp3816=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp3816[ 0]=({ struct
Cyc_TunionField_tok_struct _temp3817; _temp3817.tag= Cyc_TunionField_tok_tag;
_temp3817.f1=({ struct Cyc_Absyn_Tunionfield* _temp3818=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3818->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3825= yyvs; struct _xtunion_struct** _temp3827=
_temp3825.curr + yyvsp_offset; if( _temp3827 < _temp3825.base? 1: _temp3827 >=
_temp3825.last_plus_one){ _throw( Null_Exception);}* _temp3827;})); _temp3818->tvs=
0; _temp3818->typs= 0; _temp3818->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3819= yyls; struct Cyc_Yyltype* _temp3821= _temp3819.curr +
yylsp_offset; if( _temp3821 < _temp3819.base? 1: _temp3821 >= _temp3819.last_plus_one){
_throw( Null_Exception);}* _temp3821;}).first_line,({ struct _tagged_ptr3
_temp3822= yyls; struct Cyc_Yyltype* _temp3824= _temp3822.curr + yylsp_offset;
if( _temp3824 < _temp3822.base? 1: _temp3824 >= _temp3822.last_plus_one){ _throw(
Null_Exception);}* _temp3824;}).last_line); _temp3818;}); _temp3817;});
_temp3816;}); break; case 122: _LL3815: { struct Cyc_List_List* typs=(( struct
Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*, struct
_tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3829= yyls; struct Cyc_Yyltype* _temp3831= _temp3829.curr +( yylsp_offset -
1); if( _temp3831 < _temp3829.base? 1: _temp3831 >= _temp3829.last_plus_one){
_throw( Null_Exception);}* _temp3831;}).first_line,({ struct _tagged_ptr3
_temp3832= yyls; struct Cyc_Yyltype* _temp3834= _temp3832.curr +( yylsp_offset -
1); if( _temp3834 < _temp3832.base? 1: _temp3834 >= _temp3832.last_plus_one){
_throw( Null_Exception);}* _temp3834;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3835= yyvs; struct _xtunion_struct** _temp3837=
_temp3835.curr +( yyvsp_offset - 1); if( _temp3837 < _temp3835.base? 1:
_temp3837 >= _temp3835.last_plus_one){ _throw( Null_Exception);}* _temp3837;}))));
struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3838= yyls; struct Cyc_Yyltype* _temp3840= _temp3838.curr
+( yylsp_offset - 3); if( _temp3840 < _temp3838.base? 1: _temp3840 >= _temp3838.last_plus_one){
_throw( Null_Exception);}* _temp3840;}).first_line,({ struct _tagged_ptr3
_temp3841= yyls; struct Cyc_Yyltype* _temp3843= _temp3841.curr +( yylsp_offset -
3); if( _temp3843 < _temp3841.base? 1: _temp3843 >= _temp3841.last_plus_one){
_throw( Null_Exception);}* _temp3843;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3844= yyvs; struct _xtunion_struct** _temp3846=
_temp3844.curr +( yyvsp_offset - 3); if( _temp3846 < _temp3844.base? 1:
_temp3846 >= _temp3844.last_plus_one){ _throw( Null_Exception);}* _temp3846;})));
yyval=( struct _xtunion_struct*)({ struct Cyc_TunionField_tok_struct* _temp3847=(
struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3847[ 0]=({ struct Cyc_TunionField_tok_struct _temp3848; _temp3848.tag= Cyc_TunionField_tok_tag;
_temp3848.f1=({ struct Cyc_Absyn_Tunionfield* _temp3849=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3849->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3856= yyvs; struct _xtunion_struct** _temp3858=
_temp3856.curr +( yyvsp_offset - 4); if( _temp3858 < _temp3856.base? 1:
_temp3858 >= _temp3856.last_plus_one){ _throw( Null_Exception);}* _temp3858;}));
_temp3849->tvs= tvs; _temp3849->typs= typs; _temp3849->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3850= yyls; struct Cyc_Yyltype* _temp3852= _temp3850.curr
+( yylsp_offset - 4); if( _temp3852 < _temp3850.base? 1: _temp3852 >= _temp3850.last_plus_one){
_throw( Null_Exception);}* _temp3852;}).first_line,({ struct _tagged_ptr3
_temp3853= yyls; struct Cyc_Yyltype* _temp3855= _temp3853.curr + yylsp_offset;
if( _temp3855 < _temp3853.base? 1: _temp3855 >= _temp3853.last_plus_one){ _throw(
Null_Exception);}* _temp3855;}).last_line); _temp3849;}); _temp3848;});
_temp3847;}); break;} case 123: _LL3828: yyval=({ struct _tagged_ptr2 _temp3860=
yyvs; struct _xtunion_struct** _temp3862= _temp3860.curr + yyvsp_offset; if(
_temp3862 < _temp3860.base? 1: _temp3862 >= _temp3860.last_plus_one){ _throw(
Null_Exception);}* _temp3862;}); break; case 124: _LL3859: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3864=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3864[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3865; _temp3865.tag= Cyc_Declarator_tok_tag;
_temp3865.f1=({ struct Cyc_Parse_Declarator* _temp3866=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3866->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3873= yyvs; struct _xtunion_struct** _temp3875=
_temp3873.curr + yyvsp_offset; if( _temp3875 < _temp3873.base? 1: _temp3875 >=
_temp3873.last_plus_one){ _throw( Null_Exception);}* _temp3875;})))->id;
_temp3866->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp3867= yyvs; struct _xtunion_struct** _temp3869= _temp3867.curr +(
yyvsp_offset - 1); if( _temp3869 < _temp3867.base? 1: _temp3869 >= _temp3867.last_plus_one){
_throw( Null_Exception);}* _temp3869;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3870= yyvs; struct _xtunion_struct** _temp3872= _temp3870.curr
+ yyvsp_offset; if( _temp3872 < _temp3870.base? 1: _temp3872 >= _temp3870.last_plus_one){
_throw( Null_Exception);}* _temp3872;})))->tms); _temp3866;}); _temp3865;});
_temp3864;}); break; case 125: _LL3863: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp3877=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3877[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3878; _temp3878.tag= Cyc_Declarator_tok_tag;
_temp3878.f1=({ struct Cyc_Parse_Declarator* _temp3879=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3879->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3880= yyvs; struct _xtunion_struct** _temp3882=
_temp3880.curr + yyvsp_offset; if( _temp3882 < _temp3880.base? 1: _temp3882 >=
_temp3880.last_plus_one){ _throw( Null_Exception);}* _temp3882;})); _temp3879->tms=
0; _temp3879;}); _temp3878;}); _temp3877;}); break; case 126: _LL3876: yyval=({
struct _tagged_ptr2 _temp3884= yyvs; struct _xtunion_struct** _temp3886=
_temp3884.curr +( yyvsp_offset - 1); if( _temp3886 < _temp3884.base? 1:
_temp3886 >= _temp3884.last_plus_one){ _throw( Null_Exception);}* _temp3886;});
break; case 127: _LL3883: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp3888=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3888[ 0]=({ struct Cyc_Declarator_tok_struct _temp3889; _temp3889.tag= Cyc_Declarator_tok_tag;
_temp3889.f1=({ struct Cyc_Parse_Declarator* _temp3890=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3890->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3895= yyvs; struct _xtunion_struct** _temp3897=
_temp3895.curr +( yyvsp_offset - 2); if( _temp3897 < _temp3895.base? 1:
_temp3897 >= _temp3895.last_plus_one){ _throw( Null_Exception);}* _temp3897;})))->id;
_temp3890->tms=({ struct Cyc_List_List* _temp3891=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3891->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3891->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3892= yyvs;
struct _xtunion_struct** _temp3894= _temp3892.curr +( yyvsp_offset - 2); if(
_temp3894 < _temp3892.base? 1: _temp3894 >= _temp3892.last_plus_one){ _throw(
Null_Exception);}* _temp3894;})))->tms; _temp3891;}); _temp3890;}); _temp3889;});
_temp3888;}); break; case 128: _LL3887: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp3899=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3899[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3900; _temp3900.tag= Cyc_Declarator_tok_tag;
_temp3900.f1=({ struct Cyc_Parse_Declarator* _temp3901=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3901->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3911= yyvs; struct _xtunion_struct** _temp3913=
_temp3911.curr +( yyvsp_offset - 3); if( _temp3913 < _temp3911.base? 1:
_temp3913 >= _temp3911.last_plus_one){ _throw( Null_Exception);}* _temp3913;})))->id;
_temp3901->tms=({ struct Cyc_List_List* _temp3902=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3902->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3906=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3906[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3907; _temp3907.tag= Cyc_Absyn_ConstArray_mod_tag;
_temp3907.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3908= yyvs; struct
_xtunion_struct** _temp3910= _temp3908.curr +( yyvsp_offset - 1); if( _temp3910
< _temp3908.base? 1: _temp3910 >= _temp3908.last_plus_one){ _throw(
Null_Exception);}* _temp3910;})); _temp3907;}); _temp3906;})); _temp3902->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3903= yyvs; struct
_xtunion_struct** _temp3905= _temp3903.curr +( yyvsp_offset - 3); if( _temp3905
< _temp3903.base? 1: _temp3905 >= _temp3903.last_plus_one){ _throw(
Null_Exception);}* _temp3905;})))->tms; _temp3902;}); _temp3901;}); _temp3900;});
_temp3899;}); break; case 129: _LL3898: { struct _tuple16 _temp3920; struct Cyc_Core_Opt*
_temp3921; int _temp3923; struct Cyc_List_List* _temp3925; struct _tuple16*
_temp3918= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp3915=
yyvs; struct _xtunion_struct** _temp3917= _temp3915.curr +( yyvsp_offset - 1);
if( _temp3917 < _temp3915.base? 1: _temp3917 >= _temp3915.last_plus_one){ _throw(
Null_Exception);}* _temp3917;})); _temp3920=* _temp3918; _LL3926: _temp3925=
_temp3920.f1; goto _LL3924; _LL3924: _temp3923= _temp3920.f2; goto _LL3922;
_LL3922: _temp3921= _temp3920.f3; goto _LL3919; _LL3919: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3927=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3927[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3928; _temp3928.tag= Cyc_Declarator_tok_tag;
_temp3928.f1=({ struct Cyc_Parse_Declarator* _temp3929=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3929->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3938= yyvs; struct _xtunion_struct** _temp3940=
_temp3938.curr +( yyvsp_offset - 3); if( _temp3940 < _temp3938.base? 1:
_temp3940 >= _temp3938.last_plus_one){ _throw( Null_Exception);}* _temp3940;})))->id;
_temp3929->tms=({ struct Cyc_List_List* _temp3930=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3930->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3934=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3934[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3935; _temp3935.tag= Cyc_Absyn_Function_mod_tag;
_temp3935.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3936=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3936[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3937; _temp3937.tag= Cyc_Absyn_WithTypes_tag;
_temp3937.f1= _temp3925; _temp3937.f2= _temp3923; _temp3937.f3= _temp3921;
_temp3937;}); _temp3936;})); _temp3935;}); _temp3934;})); _temp3930->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3931= yyvs; struct _xtunion_struct** _temp3933=
_temp3931.curr +( yyvsp_offset - 3); if( _temp3933 < _temp3931.base? 1:
_temp3933 >= _temp3931.last_plus_one){ _throw( Null_Exception);}* _temp3933;})))->tms;
_temp3930;}); _temp3929;}); _temp3928;}); _temp3927;}); break;} case 130:
_LL3914: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp3942=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3942[ 0]=({ struct Cyc_Declarator_tok_struct _temp3943; _temp3943.tag= Cyc_Declarator_tok_tag;
_temp3943.f1=({ struct Cyc_Parse_Declarator* _temp3944=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3944->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3953= yyvs; struct _xtunion_struct** _temp3955=
_temp3953.curr +( yyvsp_offset - 2); if( _temp3955 < _temp3953.base? 1:
_temp3955 >= _temp3953.last_plus_one){ _throw( Null_Exception);}* _temp3955;})))->id;
_temp3944->tms=({ struct Cyc_List_List* _temp3945=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3945->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3949=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3949[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3950; _temp3950.tag= Cyc_Absyn_Function_mod_tag;
_temp3950.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3951=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3951[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3952; _temp3952.tag= Cyc_Absyn_WithTypes_tag;
_temp3952.f1= 0; _temp3952.f2= 0; _temp3952.f3= 0; _temp3952;}); _temp3951;}));
_temp3950;}); _temp3949;})); _temp3945->tl=( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3946= yyvs; struct _xtunion_struct** _temp3948= _temp3946.curr
+( yyvsp_offset - 2); if( _temp3948 < _temp3946.base? 1: _temp3948 >= _temp3946.last_plus_one){
_throw( Null_Exception);}* _temp3948;})))->tms; _temp3945;}); _temp3944;});
_temp3943;}); _temp3942;}); break; case 131: _LL3941: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3957=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3957[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3958; _temp3958.tag= Cyc_Declarator_tok_tag;
_temp3958.f1=({ struct Cyc_Parse_Declarator* _temp3959=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3959->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3974= yyvs; struct _xtunion_struct** _temp3976=
_temp3974.curr +( yyvsp_offset - 4); if( _temp3976 < _temp3974.base? 1:
_temp3976 >= _temp3974.last_plus_one){ _throw( Null_Exception);}* _temp3976;})))->id;
_temp3959->tms=({ struct Cyc_List_List* _temp3960=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3960->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3964=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3964[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3965; _temp3965.tag= Cyc_Absyn_Function_mod_tag;
_temp3965.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3966=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3966[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3967; _temp3967.tag= Cyc_Absyn_WithTypes_tag;
_temp3967.f1= 0; _temp3967.f2= 0; _temp3967.f3=({ struct Cyc_Core_Opt* _temp3968=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3968->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3969=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3969[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3970; _temp3970.tag= Cyc_Absyn_JoinEff_tag;
_temp3970.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3971= yyvs;
struct _xtunion_struct** _temp3973= _temp3971.curr +( yyvsp_offset - 1); if(
_temp3973 < _temp3971.base? 1: _temp3973 >= _temp3971.last_plus_one){ _throw(
Null_Exception);}* _temp3973;})); _temp3970;}); _temp3969;})); _temp3968;});
_temp3967;}); _temp3966;})); _temp3965;}); _temp3964;})); _temp3960->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3961= yyvs; struct _xtunion_struct** _temp3963=
_temp3961.curr +( yyvsp_offset - 4); if( _temp3963 < _temp3961.base? 1:
_temp3963 >= _temp3961.last_plus_one){ _throw( Null_Exception);}* _temp3963;})))->tms;
_temp3960;}); _temp3959;}); _temp3958;}); _temp3957;}); break; case 132: _LL3956:
yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3978=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3978[ 0]=({ struct Cyc_Declarator_tok_struct _temp3979; _temp3979.tag= Cyc_Declarator_tok_tag;
_temp3979.f1=({ struct Cyc_Parse_Declarator* _temp3980=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3980->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3998= yyvs; struct _xtunion_struct** _temp4000=
_temp3998.curr +( yyvsp_offset - 3); if( _temp4000 < _temp3998.base? 1:
_temp4000 >= _temp3998.last_plus_one){ _throw( Null_Exception);}* _temp4000;})))->id;
_temp3980->tms=({ struct Cyc_List_List* _temp3981=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3981->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3985=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3985[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3986; _temp3986.tag= Cyc_Absyn_Function_mod_tag;
_temp3986.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp3987=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp3987[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp3988; _temp3988.tag= Cyc_Absyn_NoTypes_tag;
_temp3988.f1= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp3989= yyvs;
struct _xtunion_struct** _temp3991= _temp3989.curr +( yyvsp_offset - 1); if(
_temp3991 < _temp3989.base? 1: _temp3991 >= _temp3989.last_plus_one){ _throw(
Null_Exception);}* _temp3991;})); _temp3988.f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3992= yyls; struct Cyc_Yyltype* _temp3994= _temp3992.curr
+( yylsp_offset - 3); if( _temp3994 < _temp3992.base? 1: _temp3994 >= _temp3992.last_plus_one){
_throw( Null_Exception);}* _temp3994;}).first_line,({ struct _tagged_ptr3
_temp3995= yyls; struct Cyc_Yyltype* _temp3997= _temp3995.curr + yylsp_offset;
if( _temp3997 < _temp3995.base? 1: _temp3997 >= _temp3995.last_plus_one){ _throw(
Null_Exception);}* _temp3997;}).last_line); _temp3988;}); _temp3987;}));
_temp3986;}); _temp3985;})); _temp3981->tl=( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3982= yyvs; struct _xtunion_struct** _temp3984= _temp3982.curr
+( yyvsp_offset - 3); if( _temp3984 < _temp3982.base? 1: _temp3984 >= _temp3982.last_plus_one){
_throw( Null_Exception);}* _temp3984;})))->tms; _temp3981;}); _temp3980;});
_temp3979;}); _temp3978;}); break; case 133: _LL3977: { struct Cyc_List_List* ts=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4002=
yyls; struct Cyc_Yyltype* _temp4004= _temp4002.curr +( yylsp_offset - 2); if(
_temp4004 < _temp4002.base? 1: _temp4004 >= _temp4002.last_plus_one){ _throw(
Null_Exception);}* _temp4004;}).first_line,({ struct _tagged_ptr3 _temp4005=
yyls; struct Cyc_Yyltype* _temp4007= _temp4005.curr + yylsp_offset; if(
_temp4007 < _temp4005.base? 1: _temp4007 >= _temp4005.last_plus_one){ _throw(
Null_Exception);}* _temp4007;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4008= yyvs; struct _xtunion_struct** _temp4010= _temp4008.curr
+( yyvsp_offset - 1); if( _temp4010 < _temp4008.base? 1: _temp4010 >= _temp4008.last_plus_one){
_throw( Null_Exception);}* _temp4010;})))); yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4011=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4011[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4012; _temp4012.tag= Cyc_Declarator_tok_tag;
_temp4012.f1=({ struct Cyc_Parse_Declarator* _temp4013=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4013->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4026= yyvs; struct _xtunion_struct** _temp4028=
_temp4026.curr +( yyvsp_offset - 3); if( _temp4028 < _temp4026.base? 1:
_temp4028 >= _temp4026.last_plus_one){ _throw( Null_Exception);}* _temp4028;})))->id;
_temp4013->tms=({ struct Cyc_List_List* _temp4014=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4014->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4018=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4018[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4019; _temp4019.tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4019.f1= ts; _temp4019.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4020= yyls; struct Cyc_Yyltype* _temp4022= _temp4020.curr +(
yylsp_offset - 3); if( _temp4022 < _temp4020.base? 1: _temp4022 >= _temp4020.last_plus_one){
_throw( Null_Exception);}* _temp4022;}).first_line,({ struct _tagged_ptr3
_temp4023= yyls; struct Cyc_Yyltype* _temp4025= _temp4023.curr + yylsp_offset;
if( _temp4025 < _temp4023.base? 1: _temp4025 >= _temp4023.last_plus_one){ _throw(
Null_Exception);}* _temp4025;}).last_line); _temp4019.f3= 0; _temp4019;});
_temp4018;})); _temp4014->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4015= yyvs; struct _xtunion_struct** _temp4017= _temp4015.curr +(
yyvsp_offset - 3); if( _temp4017 < _temp4015.base? 1: _temp4017 >= _temp4015.last_plus_one){
_throw( Null_Exception);}* _temp4017;})))->tms; _temp4014;}); _temp4013;});
_temp4012;}); _temp4011;}); break;} case 134: _LL4001:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4030= yyls; struct Cyc_Yyltype* _temp4032= _temp4030.curr
+( yylsp_offset - 2); if( _temp4032 < _temp4030.base? 1: _temp4032 >= _temp4030.last_plus_one){
_throw( Null_Exception);}* _temp4032;}).first_line,({ struct _tagged_ptr3
_temp4033= yyls; struct Cyc_Yyltype* _temp4035= _temp4033.curr + yylsp_offset;
if( _temp4035 < _temp4033.base? 1: _temp4035 >= _temp4033.last_plus_one){ _throw(
Null_Exception);}* _temp4035;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4036= yyvs; struct _xtunion_struct** _temp4038= _temp4036.curr
+( yyvsp_offset - 1); if( _temp4038 < _temp4036.base? 1: _temp4038 >= _temp4036.last_plus_one){
_throw( Null_Exception);}* _temp4038;})))); yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4039=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4039[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4040; _temp4040.tag= Cyc_Declarator_tok_tag;
_temp4040.f1=({ struct Cyc_Parse_Declarator* _temp4041=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4041->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4054= yyvs; struct _xtunion_struct** _temp4056=
_temp4054.curr +( yyvsp_offset - 3); if( _temp4056 < _temp4054.base? 1:
_temp4056 >= _temp4054.last_plus_one){ _throw( Null_Exception);}* _temp4056;})))->id;
_temp4041->tms=({ struct Cyc_List_List* _temp4042=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4042->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4046=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4046[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4047; _temp4047.tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4047.f1= ts; _temp4047.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4048= yyls; struct Cyc_Yyltype* _temp4050= _temp4048.curr +(
yylsp_offset - 3); if( _temp4050 < _temp4048.base? 1: _temp4050 >= _temp4048.last_plus_one){
_throw( Null_Exception);}* _temp4050;}).first_line,({ struct _tagged_ptr3
_temp4051= yyls; struct Cyc_Yyltype* _temp4053= _temp4051.curr + yylsp_offset;
if( _temp4053 < _temp4051.base? 1: _temp4053 >= _temp4051.last_plus_one){ _throw(
Null_Exception);}* _temp4053;}).last_line); _temp4047.f3= 0; _temp4047;});
_temp4046;})); _temp4042->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4043= yyvs; struct _xtunion_struct** _temp4045= _temp4043.curr +(
yyvsp_offset - 3); if( _temp4045 < _temp4043.base? 1: _temp4045 >= _temp4043.last_plus_one){
_throw( Null_Exception);}* _temp4045;})))->tms; _temp4042;}); _temp4041;});
_temp4040;}); _temp4039;}); break;} case 135: _LL4029: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp4058=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4058[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4059; _temp4059.tag= Cyc_Declarator_tok_tag;
_temp4059.f1=({ struct Cyc_Parse_Declarator* _temp4060=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4060->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4076= yyvs; struct _xtunion_struct** _temp4078=
_temp4076.curr +( yyvsp_offset - 1); if( _temp4078 < _temp4076.base? 1:
_temp4078 >= _temp4076.last_plus_one){ _throw( Null_Exception);}* _temp4078;})))->id;
_temp4060->tms=({ struct Cyc_List_List* _temp4061=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4061->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp4065=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp4065[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp4066; _temp4066.tag= Cyc_Absyn_Attributes_mod_tag;
_temp4066.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4067= yyls;
struct Cyc_Yyltype* _temp4069= _temp4067.curr + yylsp_offset; if( _temp4069 <
_temp4067.base? 1: _temp4069 >= _temp4067.last_plus_one){ _throw( Null_Exception);}*
_temp4069;}).first_line,({ struct _tagged_ptr3 _temp4070= yyls; struct Cyc_Yyltype*
_temp4072= _temp4070.curr + yylsp_offset; if( _temp4072 < _temp4070.base? 1:
_temp4072 >= _temp4070.last_plus_one){ _throw( Null_Exception);}* _temp4072;}).last_line);
_temp4066.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4073= yyvs;
struct _xtunion_struct** _temp4075= _temp4073.curr + yyvsp_offset; if( _temp4075
< _temp4073.base? 1: _temp4075 >= _temp4073.last_plus_one){ _throw(
Null_Exception);}* _temp4075;})); _temp4066;}); _temp4065;})); _temp4061->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4062= yyvs; struct
_xtunion_struct** _temp4064= _temp4062.curr +( yyvsp_offset - 1); if( _temp4064
< _temp4062.base? 1: _temp4064 >= _temp4062.last_plus_one){ _throw(
Null_Exception);}* _temp4064;})))->tms; _temp4061;}); _temp4060;}); _temp4059;});
_temp4058;}); break; case 136: _LL4057: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeModifierList_tok_struct* _temp4080=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4080[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4081; _temp4081.tag= Cyc_TypeModifierList_tok_tag;
_temp4081.f1=({ struct Cyc_List_List* _temp4082=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4082->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4083=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4083[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4084; _temp4084.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4084.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4085= yyvs; struct _xtunion_struct** _temp4087= _temp4085.curr +(
yyvsp_offset - 1); if( _temp4087 < _temp4085.base? 1: _temp4087 >= _temp4085.last_plus_one){
_throw( Null_Exception);}* _temp4087;})); _temp4084.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4088= yyvs; struct _xtunion_struct** _temp4090=
_temp4088.curr + yyvsp_offset; if( _temp4090 < _temp4088.base? 1: _temp4090 >=
_temp4088.last_plus_one){ _throw( Null_Exception);}* _temp4090;})); _temp4084.f3=
Cyc_Absyn_empty_tqual(); _temp4084;}); _temp4083;})); _temp4082->tl= 0;
_temp4082;}); _temp4081;}); _temp4080;}); break; case 137: _LL4079: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct* _temp4092=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp4092[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp4093; _temp4093.tag=
Cyc_TypeModifierList_tok_tag; _temp4093.f1=({ struct Cyc_List_List* _temp4094=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4094->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4095=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4095[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4096; _temp4096.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4096.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4097= yyvs; struct _xtunion_struct** _temp4099= _temp4097.curr +(
yyvsp_offset - 2); if( _temp4099 < _temp4097.base? 1: _temp4099 >= _temp4097.last_plus_one){
_throw( Null_Exception);}* _temp4099;})); _temp4096.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4100= yyvs; struct _xtunion_struct** _temp4102=
_temp4100.curr +( yyvsp_offset - 1); if( _temp4102 < _temp4100.base? 1:
_temp4102 >= _temp4100.last_plus_one){ _throw( Null_Exception);}* _temp4102;}));
_temp4096.f3= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp4103= yyvs;
struct _xtunion_struct** _temp4105= _temp4103.curr + yyvsp_offset; if( _temp4105
< _temp4103.base? 1: _temp4105 >= _temp4103.last_plus_one){ _throw(
Null_Exception);}* _temp4105;})); _temp4096;}); _temp4095;})); _temp4094->tl= 0;
_temp4094;}); _temp4093;}); _temp4092;}); break; case 138: _LL4091: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct* _temp4107=(
struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp4107[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp4108; _temp4108.tag=
Cyc_TypeModifierList_tok_tag; _temp4108.f1=({ struct Cyc_List_List* _temp4109=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4109->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4113=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4113[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4114; _temp4114.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4114.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4115= yyvs; struct _xtunion_struct** _temp4117= _temp4115.curr +(
yyvsp_offset - 2); if( _temp4117 < _temp4115.base? 1: _temp4117 >= _temp4115.last_plus_one){
_throw( Null_Exception);}* _temp4117;})); _temp4114.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4118= yyvs; struct _xtunion_struct** _temp4120=
_temp4118.curr +( yyvsp_offset - 1); if( _temp4120 < _temp4118.base? 1:
_temp4120 >= _temp4118.last_plus_one){ _throw( Null_Exception);}* _temp4120;}));
_temp4114.f3= Cyc_Absyn_empty_tqual(); _temp4114;}); _temp4113;})); _temp4109->tl=
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4110= yyvs; struct
_xtunion_struct** _temp4112= _temp4110.curr + yyvsp_offset; if( _temp4112 <
_temp4110.base? 1: _temp4112 >= _temp4110.last_plus_one){ _throw( Null_Exception);}*
_temp4112;})); _temp4109;}); _temp4108;}); _temp4107;}); break; case 139:
_LL4106: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4122=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4122[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4123; _temp4123.tag= Cyc_TypeModifierList_tok_tag; _temp4123.f1=({ struct
Cyc_List_List* _temp4124=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4124->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4128=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4128[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4129; _temp4129.tag=
Cyc_Absyn_Pointer_mod_tag; _temp4129.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4130= yyvs; struct _xtunion_struct** _temp4132=
_temp4130.curr +( yyvsp_offset - 3); if( _temp4132 < _temp4130.base? 1:
_temp4132 >= _temp4130.last_plus_one){ _throw( Null_Exception);}* _temp4132;}));
_temp4129.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4133= yyvs;
struct _xtunion_struct** _temp4135= _temp4133.curr +( yyvsp_offset - 2); if(
_temp4135 < _temp4133.base? 1: _temp4135 >= _temp4133.last_plus_one){ _throw(
Null_Exception);}* _temp4135;})); _temp4129.f3= Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4136= yyvs; struct _xtunion_struct** _temp4138= _temp4136.curr
+( yyvsp_offset - 1); if( _temp4138 < _temp4136.base? 1: _temp4138 >= _temp4136.last_plus_one){
_throw( Null_Exception);}* _temp4138;})); _temp4129;}); _temp4128;})); _temp4124->tl=
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4125= yyvs; struct
_xtunion_struct** _temp4127= _temp4125.curr + yyvsp_offset; if( _temp4127 <
_temp4125.base? 1: _temp4127 >= _temp4125.last_plus_one){ _throw( Null_Exception);}*
_temp4127;})); _temp4124;}); _temp4123;}); _temp4122;}); break; case 140:
_LL4121: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4140=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4140[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4141; _temp4141.tag=
Cyc_Pointer_Sort_tok_tag; _temp4141.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4142=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4142[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4143; _temp4143.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4143.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4144= yyls; struct Cyc_Yyltype* _temp4146= _temp4144.curr
+ yylsp_offset; if( _temp4146 < _temp4144.base? 1: _temp4146 >= _temp4144.last_plus_one){
_throw( Null_Exception);}* _temp4146;}).first_line,({ struct _tagged_ptr3
_temp4147= yyls; struct Cyc_Yyltype* _temp4149= _temp4147.curr + yylsp_offset;
if( _temp4149 < _temp4147.base? 1: _temp4149 >= _temp4147.last_plus_one){ _throw(
Null_Exception);}* _temp4149;}).last_line)); _temp4143;}); _temp4142;}));
_temp4141;}); _temp4140;}); break; case 141: _LL4139: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4151=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4151[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4152; _temp4152.tag= Cyc_Pointer_Sort_tok_tag;
_temp4152.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4153=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4153[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4154; _temp4154.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4154.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4155= yyls; struct
Cyc_Yyltype* _temp4157= _temp4155.curr + yylsp_offset; if( _temp4157 < _temp4155.base?
1: _temp4157 >= _temp4155.last_plus_one){ _throw( Null_Exception);}* _temp4157;}).first_line,({
struct _tagged_ptr3 _temp4158= yyls; struct Cyc_Yyltype* _temp4160= _temp4158.curr
+ yylsp_offset; if( _temp4160 < _temp4158.base? 1: _temp4160 >= _temp4158.last_plus_one){
_throw( Null_Exception);}* _temp4160;}).last_line)); _temp4154;}); _temp4153;}));
_temp4152;}); _temp4151;}); break; case 142: _LL4150: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4162=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4162[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4163; _temp4163.tag= Cyc_Pointer_Sort_tok_tag;
_temp4163.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp4164=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4164[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4165; _temp4165.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4165.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4166= yyvs; struct _xtunion_struct** _temp4168= _temp4166.curr
+( yyvsp_offset - 1); if( _temp4168 < _temp4166.base? 1: _temp4168 >= _temp4166.last_plus_one){
_throw( Null_Exception);}* _temp4168;})); _temp4165;}); _temp4164;})); _temp4163;});
_temp4162;}); break; case 143: _LL4161: yyval=( struct _xtunion_struct*)({
struct Cyc_Pointer_Sort_tok_struct* _temp4170=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4170[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4171; _temp4171.tag= Cyc_Pointer_Sort_tok_tag;
_temp4171.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4172=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4172[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4173; _temp4173.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4173.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4174= yyvs; struct _xtunion_struct** _temp4176=
_temp4174.curr +( yyvsp_offset - 1); if( _temp4176 < _temp4174.base? 1:
_temp4176 >= _temp4174.last_plus_one){ _throw( Null_Exception);}* _temp4176;}));
_temp4173;}); _temp4172;})); _temp4171;}); _temp4170;}); break; case 144:
_LL4169: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4178=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4178[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4179; _temp4179.tag=
Cyc_Pointer_Sort_tok_tag; _temp4179.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps);
_temp4179;}); _temp4178;}); break; case 145: _LL4177: yyval=( struct
_xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4181=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4181[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4182; _temp4182.tag= Cyc_Rgn_tok_tag; _temp4182.f1=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp4182;}); _temp4181;}); break; case 146: _LL4180: yyval=({ struct
_tagged_ptr2 _temp4184= yyvs; struct _xtunion_struct** _temp4186= _temp4184.curr
+ yyvsp_offset; if( _temp4186 < _temp4184.base? 1: _temp4186 >= _temp4184.last_plus_one){
_throw( Null_Exception);}* _temp4186;}); break; case 147: _LL4183: yyval=(
struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4188=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4188[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4189; _temp4189.tag= Cyc_Rgn_tok_tag; _temp4189.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4190= yyvs; struct
_xtunion_struct** _temp4192= _temp4190.curr + yyvsp_offset; if( _temp4192 <
_temp4190.base? 1: _temp4192 >= _temp4190.last_plus_one){ _throw( Null_Exception);}*
_temp4192;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp4189;}); _temp4188;}); break; case 148: _LL4187:
if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp4194= yyvs; struct
_xtunion_struct** _temp4196= _temp4194.curr + yyvsp_offset; if( _temp4196 <
_temp4194.base? 1: _temp4196 >= _temp4194.last_plus_one){ _throw( Null_Exception);}*
_temp4196;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp4197=( char*)"expecting region kind\n"; struct
_tagged_string _temp4198; _temp4198.curr= _temp4197; _temp4198.base= _temp4197;
_temp4198.last_plus_one= _temp4197 + 23; _temp4198;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4199= yyls; struct Cyc_Yyltype* _temp4201= _temp4199.curr
+ yylsp_offset; if( _temp4201 < _temp4199.base? 1: _temp4201 >= _temp4199.last_plus_one){
_throw( Null_Exception);}* _temp4201;}).first_line,({ struct _tagged_ptr3
_temp4202= yyls; struct Cyc_Yyltype* _temp4204= _temp4202.curr + yylsp_offset;
if( _temp4204 < _temp4202.base? 1: _temp4204 >= _temp4202.last_plus_one){ _throw(
Null_Exception);}* _temp4204;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_Rgn_tok_struct* _temp4205=( struct Cyc_Rgn_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgn_tok_struct)); _temp4205[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4206; _temp4206.tag= Cyc_Rgn_tok_tag; _temp4206.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4207= yyvs; struct
_xtunion_struct** _temp4209= _temp4207.curr +( yyvsp_offset - 2); if( _temp4209
< _temp4207.base? 1: _temp4209 >= _temp4207.last_plus_one){ _throw(
Null_Exception);}* _temp4209;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp4206;}); _temp4205;}); break; case 149: _LL4193:
yyval=( struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4211=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4211[
0]=({ struct Cyc_Rgn_tok_struct _temp4212; _temp4212.tag= Cyc_Rgn_tok_tag;
_temp4212.f1=( void*) Cyc_Absyn_new_evar(( void*) Cyc_Absyn_RgnKind); _temp4212;});
_temp4211;}); break; case 150: _LL4210: yyval=({ struct _tagged_ptr2 _temp4214=
yyvs; struct _xtunion_struct** _temp4216= _temp4214.curr + yyvsp_offset; if(
_temp4216 < _temp4214.base? 1: _temp4216 >= _temp4214.last_plus_one){ _throw(
Null_Exception);}* _temp4216;}); break; case 151: _LL4213: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp4218=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp4218[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp4219; _temp4219.tag= Cyc_TypeQual_tok_tag; _temp4219.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp4220= yyvs; struct
_xtunion_struct** _temp4222= _temp4220.curr +( yyvsp_offset - 1); if( _temp4222
< _temp4220.base? 1: _temp4222 >= _temp4220.last_plus_one){ _throw(
Null_Exception);}* _temp4222;})), Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2
_temp4223= yyvs; struct _xtunion_struct** _temp4225= _temp4223.curr +
yyvsp_offset; if( _temp4225 < _temp4223.base? 1: _temp4225 >= _temp4223.last_plus_one){
_throw( Null_Exception);}* _temp4225;}))); _temp4219;}); _temp4218;}); break;
case 152: _LL4217: yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct*
_temp4227=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp4227[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct
_temp4228; _temp4228.tag= Cyc_ParamDeclListBool_tok_tag; _temp4228.f1=({ struct
_tuple16* _temp4229=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp4229->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2 _temp4230= yyvs; struct
_xtunion_struct** _temp4232= _temp4230.curr + yyvsp_offset; if( _temp4232 <
_temp4230.base? 1: _temp4232 >= _temp4230.last_plus_one){ _throw( Null_Exception);}*
_temp4232;}))); _temp4229->f2= 0; _temp4229->f3= 0; _temp4229;}); _temp4228;});
_temp4227;}); break; case 153: _LL4226: yyval=( struct _xtunion_struct*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp4234=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp4234[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp4235; _temp4235.tag= Cyc_ParamDeclListBool_tok_tag;
_temp4235.f1=({ struct _tuple16* _temp4236=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp4236->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4237= yyvs; struct _xtunion_struct** _temp4239= _temp4237.curr +(
yyvsp_offset - 2); if( _temp4239 < _temp4237.base? 1: _temp4239 >= _temp4237.last_plus_one){
_throw( Null_Exception);}* _temp4239;}))); _temp4236->f2= 1; _temp4236->f3= 0;
_temp4236;}); _temp4235;}); _temp4234;}); break; case 154: _LL4233: yyval=(
struct _xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4241=(
struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4241[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4242; _temp4242.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4242.f1=({ struct _tuple16* _temp4243=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4243->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4250= yyvs; struct _xtunion_struct** _temp4252=
_temp4250.curr +( yyvsp_offset - 2); if( _temp4252 < _temp4250.base? 1:
_temp4252 >= _temp4250.last_plus_one){ _throw( Null_Exception);}* _temp4252;})));
_temp4243->f2= 0; _temp4243->f3=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp4244=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp4244->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4245=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4245[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4246; _temp4246.tag= Cyc_Absyn_JoinEff_tag;
_temp4246.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4247= yyvs;
struct _xtunion_struct** _temp4249= _temp4247.curr + yyvsp_offset; if( _temp4249
< _temp4247.base? 1: _temp4249 >= _temp4247.last_plus_one){ _throw(
Null_Exception);}* _temp4249;})); _temp4246;}); _temp4245;})); _temp4244;});
_temp4243;}); _temp4242;}); _temp4241;}); break; case 155: _LL4240: yyval=({
struct _tagged_ptr2 _temp4254= yyvs; struct _xtunion_struct** _temp4256=
_temp4254.curr + yyvsp_offset; if( _temp4256 < _temp4254.base? 1: _temp4256 >=
_temp4254.last_plus_one){ _throw( Null_Exception);}* _temp4256;}); break; case
156: _LL4253: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4258=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4258[ 0]=({ struct Cyc_TypeList_tok_struct _temp4259; _temp4259.tag= Cyc_TypeList_tok_tag;
_temp4259.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2
_temp4260= yyvs; struct _xtunion_struct** _temp4262= _temp4260.curr +(
yyvsp_offset - 2); if( _temp4262 < _temp4260.base? 1: _temp4262 >= _temp4260.last_plus_one){
_throw( Null_Exception);}* _temp4262;})), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4263= yyvs; struct _xtunion_struct** _temp4265= _temp4263.curr
+ yyvsp_offset; if( _temp4265 < _temp4263.base? 1: _temp4265 >= _temp4263.last_plus_one){
_throw( Null_Exception);}* _temp4265;}))); _temp4259;}); _temp4258;}); break;
case 157: _LL4257: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4267=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4267[ 0]=({ struct Cyc_TypeList_tok_struct _temp4268; _temp4268.tag= Cyc_TypeList_tok_tag;
_temp4268.f1= 0; _temp4268;}); _temp4267;}); break; case 158: _LL4266: yyval=({
struct _tagged_ptr2 _temp4270= yyvs; struct _xtunion_struct** _temp4272=
_temp4270.curr +( yyvsp_offset - 1); if( _temp4272 < _temp4270.base? 1:
_temp4272 >= _temp4270.last_plus_one){ _throw( Null_Exception);}* _temp4272;});
break; case 159: _LL4269: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4274=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4274[ 0]=({ struct Cyc_TypeList_tok_struct _temp4275; _temp4275.tag= Cyc_TypeList_tok_tag;
_temp4275.f1=({ struct Cyc_List_List* _temp4276=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4276->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4277= yyvs; struct
_xtunion_struct** _temp4279= _temp4277.curr + yyvsp_offset; if( _temp4279 <
_temp4277.base? 1: _temp4279 >= _temp4277.last_plus_one){ _throw( Null_Exception);}*
_temp4279;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_EffKind)); _temp4276->tl= 0; _temp4276;}); _temp4275;});
_temp4274;}); break; case 160: _LL4273: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4281= yyvs; struct _xtunion_struct** _temp4283= _temp4281.curr
+ yyvsp_offset; if( _temp4283 < _temp4281.base? 1: _temp4283 >= _temp4281.last_plus_one){
_throw( Null_Exception);}* _temp4283;})) !=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4284=( char*)"expecing effect kind (E)";
struct _tagged_string _temp4285; _temp4285.curr= _temp4284; _temp4285.base=
_temp4284; _temp4285.last_plus_one= _temp4284 + 25; _temp4285;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4286= yyls; struct Cyc_Yyltype* _temp4288= _temp4286.curr
+ yylsp_offset; if( _temp4288 < _temp4286.base? 1: _temp4288 >= _temp4286.last_plus_one){
_throw( Null_Exception);}* _temp4288;}).first_line,({ struct _tagged_ptr3
_temp4289= yyls; struct Cyc_Yyltype* _temp4291= _temp4289.curr + yylsp_offset;
if( _temp4291 < _temp4289.base? 1: _temp4291 >= _temp4289.last_plus_one){ _throw(
Null_Exception);}* _temp4291;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp4292=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4292[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4293; _temp4293.tag= Cyc_TypeList_tok_tag; _temp4293.f1=({ struct Cyc_List_List*
_temp4294=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4294->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4295= yyvs; struct _xtunion_struct** _temp4297= _temp4295.curr
+( yyvsp_offset - 2); if( _temp4297 < _temp4295.base? 1: _temp4297 >= _temp4295.last_plus_one){
_throw( Null_Exception);}* _temp4297;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp4294->tl= 0; _temp4294;});
_temp4293;}); _temp4292;}); break; case 161: _LL4280: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4299=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4299[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4300; _temp4300.tag= Cyc_TypeList_tok_tag; _temp4300.f1=({ struct Cyc_List_List*
_temp4301=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4301->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4302=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4302[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4303; _temp4303.tag= Cyc_Absyn_AccessEff_tag;
_temp4303.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4304= yyvs; struct _xtunion_struct** _temp4306= _temp4304.curr
+ yyvsp_offset; if( _temp4306 < _temp4304.base? 1: _temp4306 >= _temp4304.last_plus_one){
_throw( Null_Exception);}* _temp4306;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp4303;}); _temp4302;}));
_temp4301->tl= 0; _temp4301;}); _temp4300;}); _temp4299;}); break; case 162:
_LL4298: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4308=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4308[ 0]=({ struct Cyc_TypeList_tok_struct _temp4309; _temp4309.tag= Cyc_TypeList_tok_tag;
_temp4309.f1=({ struct Cyc_List_List* _temp4310=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4310->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4314=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4314[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4315; _temp4315.tag= Cyc_Absyn_AccessEff_tag;
_temp4315.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4316= yyvs; struct _xtunion_struct** _temp4318= _temp4316.curr
+( yyvsp_offset - 2); if( _temp4318 < _temp4316.base? 1: _temp4318 >= _temp4316.last_plus_one){
_throw( Null_Exception);}* _temp4318;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp4315;}); _temp4314;}));
_temp4310->tl= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4311= yyvs;
struct _xtunion_struct** _temp4313= _temp4311.curr + yyvsp_offset; if( _temp4313
< _temp4311.base? 1: _temp4313 >= _temp4311.last_plus_one){ _throw(
Null_Exception);}* _temp4313;})); _temp4310;}); _temp4309;}); _temp4308;});
break; case 163: _LL4307: if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2
_temp4320= yyvs; struct _xtunion_struct** _temp4322= _temp4320.curr +
yyvsp_offset; if( _temp4322 < _temp4320.base? 1: _temp4322 >= _temp4320.last_plus_one){
_throw( Null_Exception);}* _temp4322;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4323=( char*)"expecting region kind (R)";
struct _tagged_string _temp4324; _temp4324.curr= _temp4323; _temp4324.base=
_temp4323; _temp4324.last_plus_one= _temp4323 + 26; _temp4324;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4325= yyls; struct Cyc_Yyltype* _temp4327= _temp4325.curr
+ yylsp_offset; if( _temp4327 < _temp4325.base? 1: _temp4327 >= _temp4325.last_plus_one){
_throw( Null_Exception);}* _temp4327;}).first_line,({ struct _tagged_ptr3
_temp4328= yyls; struct Cyc_Yyltype* _temp4330= _temp4328.curr + yylsp_offset;
if( _temp4330 < _temp4328.base? 1: _temp4330 >= _temp4328.last_plus_one){ _throw(
Null_Exception);}* _temp4330;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp4331=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4331[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4332; _temp4332.tag= Cyc_TypeList_tok_tag; _temp4332.f1=({ struct Cyc_List_List*
_temp4333=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4333->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4334=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4334[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4335; _temp4335.tag= Cyc_Absyn_AccessEff_tag;
_temp4335.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4336= yyvs; struct _xtunion_struct** _temp4338= _temp4336.curr
+( yyvsp_offset - 2); if( _temp4338 < _temp4336.base? 1: _temp4338 >= _temp4336.last_plus_one){
_throw( Null_Exception);}* _temp4338;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp4335;}); _temp4334;}));
_temp4333->tl= 0; _temp4333;}); _temp4332;}); _temp4331;}); break; case 164:
_LL4319: if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp4340= yyvs; struct
_xtunion_struct** _temp4342= _temp4340.curr +( yyvsp_offset - 2); if( _temp4342
< _temp4340.base? 1: _temp4342 >= _temp4340.last_plus_one){ _throw(
Null_Exception);}* _temp4342;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4343=( char*)"expecting region kind (R)";
struct _tagged_string _temp4344; _temp4344.curr= _temp4343; _temp4344.base=
_temp4343; _temp4344.last_plus_one= _temp4343 + 26; _temp4344;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4345= yyls; struct Cyc_Yyltype* _temp4347= _temp4345.curr
+( yylsp_offset - 2); if( _temp4347 < _temp4345.base? 1: _temp4347 >= _temp4345.last_plus_one){
_throw( Null_Exception);}* _temp4347;}).first_line,({ struct _tagged_ptr3
_temp4348= yyls; struct Cyc_Yyltype* _temp4350= _temp4348.curr +( yylsp_offset -
2); if( _temp4350 < _temp4348.base? 1: _temp4350 >= _temp4348.last_plus_one){
_throw( Null_Exception);}* _temp4350;}).last_line));} yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4351=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4351[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4352; _temp4352.tag= Cyc_TypeList_tok_tag; _temp4352.f1=({ struct Cyc_List_List*
_temp4353=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4353->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4357=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4357[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4358; _temp4358.tag= Cyc_Absyn_AccessEff_tag;
_temp4358.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4359= yyvs; struct _xtunion_struct** _temp4361= _temp4359.curr
+( yyvsp_offset - 4); if( _temp4361 < _temp4359.base? 1: _temp4361 >= _temp4359.last_plus_one){
_throw( Null_Exception);}* _temp4361;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp4358;}); _temp4357;}));
_temp4353->tl= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4354= yyvs;
struct _xtunion_struct** _temp4356= _temp4354.curr + yyvsp_offset; if( _temp4356
< _temp4354.base? 1: _temp4356 >= _temp4354.last_plus_one){ _throw(
Null_Exception);}* _temp4356;})); _temp4353;}); _temp4352;}); _temp4351;});
break; case 165: _LL4339: yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct*
_temp4363=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4363[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4364; _temp4364.tag=
Cyc_ParamDeclList_tok_tag; _temp4364.f1=({ struct Cyc_List_List* _temp4365=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4365->hd=(
void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4366= yyvs; struct
_xtunion_struct** _temp4368= _temp4366.curr + yyvsp_offset; if( _temp4368 <
_temp4366.base? 1: _temp4368 >= _temp4366.last_plus_one){ _throw( Null_Exception);}*
_temp4368;})); _temp4365->tl= 0; _temp4365;}); _temp4364;}); _temp4363;});
break; case 166: _LL4362: yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct*
_temp4370=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4370[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4371; _temp4371.tag=
Cyc_ParamDeclList_tok_tag; _temp4371.f1=({ struct Cyc_List_List* _temp4372=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4372->hd=(
void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4376= yyvs; struct
_xtunion_struct** _temp4378= _temp4376.curr + yyvsp_offset; if( _temp4378 <
_temp4376.base? 1: _temp4378 >= _temp4376.last_plus_one){ _throw( Null_Exception);}*
_temp4378;})); _temp4372->tl= Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4373= yyvs; struct _xtunion_struct** _temp4375= _temp4373.curr +(
yyvsp_offset - 2); if( _temp4375 < _temp4373.base? 1: _temp4375 >= _temp4373.last_plus_one){
_throw( Null_Exception);}* _temp4375;})); _temp4372;}); _temp4371;}); _temp4370;});
break; case 167: _LL4369: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4380= yyvs; struct _xtunion_struct** _temp4382=
_temp4380.curr +( yyvsp_offset - 1); if( _temp4382 < _temp4380.base? 1:
_temp4382 >= _temp4380.last_plus_one){ _throw( Null_Exception);}* _temp4382;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4383= yyls; struct Cyc_Yyltype*
_temp4385= _temp4383.curr +( yylsp_offset - 1); if( _temp4385 < _temp4383.base?
1: _temp4385 >= _temp4383.last_plus_one){ _throw( Null_Exception);}* _temp4385;}).first_line,({
struct _tagged_ptr3 _temp4386= yyls; struct Cyc_Yyltype* _temp4388= _temp4386.curr
+( yylsp_offset - 1); if( _temp4388 < _temp4386.base? 1: _temp4388 >= _temp4386.last_plus_one){
_throw( Null_Exception);}* _temp4388;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4389= yyvs; struct
_xtunion_struct** _temp4391= _temp4389.curr +( yyvsp_offset - 1); if( _temp4391
< _temp4389.base? 1: _temp4391 >= _temp4389.last_plus_one){ _throw(
Null_Exception);}* _temp4391;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4392= yyvs; struct _xtunion_struct** _temp4394=
_temp4392.curr +( yyvsp_offset - 1); if( _temp4394 < _temp4392.base? 1:
_temp4394 >= _temp4392.last_plus_one){ _throw( Null_Exception);}* _temp4394;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4395= yyvs; struct _xtunion_struct** _temp4397= _temp4395.curr +
yyvsp_offset; if( _temp4397 < _temp4395.base? 1: _temp4397 >= _temp4395.last_plus_one){
_throw( Null_Exception);}* _temp4397;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp4398=( char*)"parameter with bad type params"; struct _tagged_string
_temp4399; _temp4399.curr= _temp4398; _temp4399.base= _temp4398; _temp4399.last_plus_one=
_temp4398 + 31; _temp4399;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4400= yyls; struct Cyc_Yyltype* _temp4402= _temp4400.curr + yylsp_offset;
if( _temp4402 < _temp4400.base? 1: _temp4402 >= _temp4400.last_plus_one){ _throw(
Null_Exception);}* _temp4402;}).first_line,({ struct _tagged_ptr3 _temp4403=
yyls; struct Cyc_Yyltype* _temp4405= _temp4403.curr + yylsp_offset; if(
_temp4405 < _temp4403.base? 1: _temp4405 >= _temp4403.last_plus_one){ _throw(
Null_Exception);}* _temp4405;}).last_line));}{ struct _tuple1* q=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4406= yyvs; struct _xtunion_struct** _temp4408=
_temp4406.curr + yyvsp_offset; if( _temp4408 < _temp4406.base? 1: _temp4408 >=
_temp4406.last_plus_one){ _throw( Null_Exception);}* _temp4408;})))->id;{ void*
_temp4409=(* q).f1; struct Cyc_List_List* _temp4419; struct Cyc_List_List*
_temp4421; _LL4411: if(( int) _temp4409 == Cyc_Absyn_Loc_n){ goto _LL4412;}
else{ goto _LL4413;} _LL4413: if(( unsigned int) _temp4409 > 1u?(( struct
_tunion_struct*) _temp4409)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL4420: _temp4419=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp4409)->f1; if(
_temp4419 == 0){ goto _LL4414;} else{ goto _LL4415;}} else{ goto _LL4415;}
_LL4415: if(( unsigned int) _temp4409 > 1u?(( struct _tunion_struct*) _temp4409)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL4422: _temp4421=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp4409)->f1; if( _temp4421 == 0){ goto _LL4416;}
else{ goto _LL4417;}} else{ goto _LL4417;} _LL4417: goto _LL4418; _LL4412: goto
_LL4410; _LL4414: goto _LL4410; _LL4416: goto _LL4410; _LL4418: Cyc_Parse_err((
struct _tagged_string)({ char* _temp4423=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp4424; _temp4424.curr= _temp4423; _temp4424.base=
_temp4423; _temp4424.last_plus_one= _temp4423 + 49; _temp4424;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4425= yyls; struct Cyc_Yyltype* _temp4427= _temp4425.curr
+( yylsp_offset - 1); if( _temp4427 < _temp4425.base? 1: _temp4427 >= _temp4425.last_plus_one){
_throw( Null_Exception);}* _temp4427;}).first_line,({ struct _tagged_ptr3
_temp4428= yyls; struct Cyc_Yyltype* _temp4430= _temp4428.curr +( yylsp_offset -
1); if( _temp4430 < _temp4428.base? 1: _temp4430 >= _temp4428.last_plus_one){
_throw( Null_Exception);}* _temp4430;}).last_line)); goto _LL4410; _LL4410:;}{
struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp4431=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp4431->v=( void*)(* q).f2; _temp4431;}); if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4432=( char*)"extra attributes on parameter, ignoring";
struct _tagged_string _temp4433; _temp4433.curr= _temp4432; _temp4433.base=
_temp4432; _temp4433.last_plus_one= _temp4432 + 40; _temp4433;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4434= yyls; struct Cyc_Yyltype* _temp4436= _temp4434.curr
+( yylsp_offset - 1); if( _temp4436 < _temp4434.base? 1: _temp4436 >= _temp4434.last_plus_one){
_throw( Null_Exception);}* _temp4436;}).first_line,({ struct _tagged_ptr3
_temp4437= yyls; struct Cyc_Yyltype* _temp4439= _temp4437.curr + yylsp_offset;
if( _temp4439 < _temp4437.base? 1: _temp4439 >= _temp4437.last_plus_one){ _throw(
Null_Exception);}* _temp4439;}).last_line));} if( t_info.f3 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4440=( char*)"extra type variables on parameter, ignoring";
struct _tagged_string _temp4441; _temp4441.curr= _temp4440; _temp4441.base=
_temp4440; _temp4441.last_plus_one= _temp4440 + 44; _temp4441;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4442= yyls; struct Cyc_Yyltype* _temp4444= _temp4442.curr
+( yylsp_offset - 1); if( _temp4444 < _temp4442.base? 1: _temp4444 >= _temp4442.last_plus_one){
_throw( Null_Exception);}* _temp4444;}).first_line,({ struct _tagged_ptr3
_temp4445= yyls; struct Cyc_Yyltype* _temp4447= _temp4445.curr + yylsp_offset;
if( _temp4447 < _temp4445.base? 1: _temp4447 >= _temp4445.last_plus_one){ _throw(
Null_Exception);}* _temp4447;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_ParamDecl_tok_struct* _temp4448=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4448[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4449; _temp4449.tag= Cyc_ParamDecl_tok_tag;
_temp4449.f1=({ struct _tuple2* _temp4450=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4450->f1= idopt; _temp4450->f2= t_info.f1; _temp4450->f3=
t_info.f2; _temp4450;}); _temp4449;}); _temp4448;}); break;}}} case 168: _LL4379: {
void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp4452= yyvs; struct _xtunion_struct** _temp4454= _temp4452.curr
+ yyvsp_offset; if( _temp4454 < _temp4452.base? 1: _temp4454 >= _temp4452.last_plus_one){
_throw( Null_Exception);}* _temp4454;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4455= yyls; struct Cyc_Yyltype* _temp4457= _temp4455.curr
+ yylsp_offset; if( _temp4457 < _temp4455.base? 1: _temp4457 >= _temp4455.last_plus_one){
_throw( Null_Exception);}* _temp4457;}).first_line,({ struct _tagged_ptr3
_temp4458= yyls; struct Cyc_Yyltype* _temp4460= _temp4458.curr + yylsp_offset;
if( _temp4460 < _temp4458.base? 1: _temp4460 >= _temp4458.last_plus_one){ _throw(
Null_Exception);}* _temp4460;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4461= yyvs; struct _xtunion_struct** _temp4463=
_temp4461.curr + yyvsp_offset; if( _temp4463 < _temp4461.base? 1: _temp4463 >=
_temp4461.last_plus_one){ _throw( Null_Exception);}* _temp4463;}))).f3; struct
Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4464= yyvs; struct _xtunion_struct** _temp4466= _temp4464.curr +
yyvsp_offset; if( _temp4466 < _temp4464.base? 1: _temp4466 >= _temp4464.last_plus_one){
_throw( Null_Exception);}* _temp4466;}))).f1; if( atts != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4467=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4468; _temp4468.curr= _temp4467; _temp4468.base=
_temp4467; _temp4468.last_plus_one= _temp4467 + 38; _temp4468;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4469= yyls; struct Cyc_Yyltype* _temp4471= _temp4469.curr
+ yylsp_offset; if( _temp4471 < _temp4469.base? 1: _temp4471 >= _temp4469.last_plus_one){
_throw( Null_Exception);}* _temp4471;}).first_line,({ struct _tagged_ptr3
_temp4472= yyls; struct Cyc_Yyltype* _temp4474= _temp4472.curr + yylsp_offset;
if( _temp4474 < _temp4472.base? 1: _temp4474 >= _temp4472.last_plus_one){ _throw(
Null_Exception);}* _temp4474;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_ParamDecl_tok_struct* _temp4475=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4475[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4476; _temp4476.tag= Cyc_ParamDecl_tok_tag;
_temp4476.f1=({ struct _tuple2* _temp4477=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4477->f1= 0; _temp4477->f2= tq; _temp4477->f3= t;
_temp4477;}); _temp4476;}); _temp4475;}); break;} case 169: _LL4451: { void* t=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4479= yyvs; struct _xtunion_struct** _temp4481= _temp4479.curr +(
yyvsp_offset - 1); if( _temp4481 < _temp4479.base? 1: _temp4481 >= _temp4479.last_plus_one){
_throw( Null_Exception);}* _temp4481;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4482= yyls; struct Cyc_Yyltype* _temp4484= _temp4482.curr
+( yylsp_offset - 1); if( _temp4484 < _temp4482.base? 1: _temp4484 >= _temp4482.last_plus_one){
_throw( Null_Exception);}* _temp4484;}).first_line,({ struct _tagged_ptr3
_temp4485= yyls; struct Cyc_Yyltype* _temp4487= _temp4485.curr +( yylsp_offset -
1); if( _temp4487 < _temp4485.base? 1: _temp4487 >= _temp4485.last_plus_one){
_throw( Null_Exception);}* _temp4487;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4488= yyvs; struct
_xtunion_struct** _temp4490= _temp4488.curr +( yyvsp_offset - 1); if( _temp4490
< _temp4488.base? 1: _temp4490 >= _temp4488.last_plus_one){ _throw(
Null_Exception);}* _temp4490;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4491= yyvs; struct _xtunion_struct** _temp4493=
_temp4491.curr +( yyvsp_offset - 1); if( _temp4493 < _temp4491.base? 1:
_temp4493 >= _temp4491.last_plus_one){ _throw( Null_Exception);}* _temp4493;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4494= yyvs; struct _xtunion_struct** _temp4496= _temp4494.curr
+ yyvsp_offset; if( _temp4496 < _temp4494.base? 1: _temp4496 >= _temp4494.last_plus_one){
_throw( Null_Exception);}* _temp4496;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4497=( char*)"bad type parameters on formal argument, ignoring";
struct _tagged_string _temp4498; _temp4498.curr= _temp4497; _temp4498.base=
_temp4497; _temp4498.last_plus_one= _temp4497 + 49; _temp4498;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4499= yyls; struct Cyc_Yyltype* _temp4501= _temp4499.curr
+( yylsp_offset - 1); if( _temp4501 < _temp4499.base? 1: _temp4501 >= _temp4499.last_plus_one){
_throw( Null_Exception);}* _temp4501;}).first_line,({ struct _tagged_ptr3
_temp4502= yyls; struct Cyc_Yyltype* _temp4504= _temp4502.curr + yylsp_offset;
if( _temp4504 < _temp4502.base? 1: _temp4504 >= _temp4502.last_plus_one){ _throw(
Null_Exception);}* _temp4504;}).last_line));} if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4505=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4506; _temp4506.curr= _temp4505; _temp4506.base=
_temp4505; _temp4506.last_plus_one= _temp4505 + 38; _temp4506;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4507= yyls; struct Cyc_Yyltype* _temp4509= _temp4507.curr
+( yylsp_offset - 1); if( _temp4509 < _temp4507.base? 1: _temp4509 >= _temp4507.last_plus_one){
_throw( Null_Exception);}* _temp4509;}).first_line,({ struct _tagged_ptr3
_temp4510= yyls; struct Cyc_Yyltype* _temp4512= _temp4510.curr + yylsp_offset;
if( _temp4512 < _temp4510.base? 1: _temp4512 >= _temp4510.last_plus_one){ _throw(
Null_Exception);}* _temp4512;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_ParamDecl_tok_struct* _temp4513=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4513[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4514; _temp4514.tag= Cyc_ParamDecl_tok_tag;
_temp4514.f1=({ struct _tuple2* _temp4515=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4515->f1= 0; _temp4515->f2= t_info.f1; _temp4515->f3=
t_info.f2; _temp4515;}); _temp4514;}); _temp4513;}); break;} case 170: _LL4478:
yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct* _temp4517=(
struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4517[ 0]=({ struct Cyc_IdList_tok_struct _temp4518; _temp4518.tag= Cyc_IdList_tok_tag;
_temp4518.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4519= yyvs; struct
_xtunion_struct** _temp4521= _temp4519.curr + yyvsp_offset; if( _temp4521 <
_temp4519.base? 1: _temp4521 >= _temp4519.last_plus_one){ _throw( Null_Exception);}*
_temp4521;}))); _temp4518;}); _temp4517;}); break; case 171: _LL4516: yyval=(
struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct* _temp4523=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp4523[ 0]=({ struct Cyc_IdList_tok_struct
_temp4524; _temp4524.tag= Cyc_IdList_tok_tag; _temp4524.f1=({ struct Cyc_List_List*
_temp4525=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4525->hd=( void*)({ struct _tagged_string* _temp4526=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4526[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4527= yyvs; struct
_xtunion_struct** _temp4529= _temp4527.curr + yyvsp_offset; if( _temp4529 <
_temp4527.base? 1: _temp4529 >= _temp4527.last_plus_one){ _throw( Null_Exception);}*
_temp4529;})); _temp4526;}); _temp4525->tl= 0; _temp4525;}); _temp4524;});
_temp4523;}); break; case 172: _LL4522: yyval=( struct _xtunion_struct*)({
struct Cyc_IdList_tok_struct* _temp4531=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp4531[ 0]=({ struct Cyc_IdList_tok_struct
_temp4532; _temp4532.tag= Cyc_IdList_tok_tag; _temp4532.f1=({ struct Cyc_List_List*
_temp4533=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4533->hd=( void*)({ struct _tagged_string* _temp4537=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4537[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4538= yyvs; struct
_xtunion_struct** _temp4540= _temp4538.curr + yyvsp_offset; if( _temp4540 <
_temp4538.base? 1: _temp4540 >= _temp4538.last_plus_one){ _throw( Null_Exception);}*
_temp4540;})); _temp4537;}); _temp4533->tl= Cyc_yyget_IdList_tok(({ struct
_tagged_ptr2 _temp4534= yyvs; struct _xtunion_struct** _temp4536= _temp4534.curr
+( yyvsp_offset - 2); if( _temp4536 < _temp4534.base? 1: _temp4536 >= _temp4534.last_plus_one){
_throw( Null_Exception);}* _temp4536;})); _temp4533;}); _temp4532;}); _temp4531;});
break; case 173: _LL4530: yyval=({ struct _tagged_ptr2 _temp4542= yyvs; struct
_xtunion_struct** _temp4544= _temp4542.curr + yyvsp_offset; if( _temp4544 <
_temp4542.base? 1: _temp4544 >= _temp4542.last_plus_one){ _throw( Null_Exception);}*
_temp4544;}); break; case 174: _LL4541: yyval=({ struct _tagged_ptr2 _temp4546=
yyvs; struct _xtunion_struct** _temp4548= _temp4546.curr + yyvsp_offset; if(
_temp4548 < _temp4546.base? 1: _temp4548 >= _temp4546.last_plus_one){ _throw(
Null_Exception);}* _temp4548;}); break; case 175: _LL4545: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4550=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4550[ 0]=({ struct Cyc_Exp_tok_struct
_temp4551; _temp4551.tag= Cyc_Exp_tok_tag; _temp4551.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4552=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4552[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4553; _temp4553.tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4553.f1= 0; _temp4553.f2= 0; _temp4553;}); _temp4552;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4554= yyls; struct Cyc_Yyltype* _temp4556= _temp4554.curr
+( yylsp_offset - 1); if( _temp4556 < _temp4554.base? 1: _temp4556 >= _temp4554.last_plus_one){
_throw( Null_Exception);}* _temp4556;}).first_line,({ struct _tagged_ptr3
_temp4557= yyls; struct Cyc_Yyltype* _temp4559= _temp4557.curr + yylsp_offset;
if( _temp4559 < _temp4557.base? 1: _temp4559 >= _temp4557.last_plus_one){ _throw(
Null_Exception);}* _temp4559;}).last_line)); _temp4551;}); _temp4550;}); break;
case 176: _LL4549: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp4561=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4561[ 0]=({ struct Cyc_Exp_tok_struct _temp4562; _temp4562.tag= Cyc_Exp_tok_tag;
_temp4562.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4563=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4563[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp4564; _temp4564.tag= Cyc_Absyn_UnresolvedMem_e_tag; _temp4564.f1= 0;
_temp4564.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp4565= yyvs; struct
_xtunion_struct** _temp4567= _temp4565.curr +( yyvsp_offset - 1); if( _temp4567
< _temp4565.base? 1: _temp4567 >= _temp4565.last_plus_one){ _throw(
Null_Exception);}* _temp4567;}))); _temp4564;}); _temp4563;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4568= yyls; struct Cyc_Yyltype* _temp4570= _temp4568.curr
+( yylsp_offset - 2); if( _temp4570 < _temp4568.base? 1: _temp4570 >= _temp4568.last_plus_one){
_throw( Null_Exception);}* _temp4570;}).first_line,({ struct _tagged_ptr3
_temp4571= yyls; struct Cyc_Yyltype* _temp4573= _temp4571.curr + yylsp_offset;
if( _temp4573 < _temp4571.base? 1: _temp4573 >= _temp4571.last_plus_one){ _throw(
Null_Exception);}* _temp4573;}).last_line)); _temp4562;}); _temp4561;}); break;
case 177: _LL4560: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp4575=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4575[ 0]=({ struct Cyc_Exp_tok_struct _temp4576; _temp4576.tag= Cyc_Exp_tok_tag;
_temp4576.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4577=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4577[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp4578; _temp4578.tag= Cyc_Absyn_UnresolvedMem_e_tag; _temp4578.f1= 0;
_temp4578.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp4579= yyvs; struct
_xtunion_struct** _temp4581= _temp4579.curr +( yyvsp_offset - 2); if( _temp4581
< _temp4579.base? 1: _temp4581 >= _temp4579.last_plus_one){ _throw(
Null_Exception);}* _temp4581;}))); _temp4578;}); _temp4577;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4582= yyls; struct Cyc_Yyltype* _temp4584= _temp4582.curr
+( yylsp_offset - 3); if( _temp4584 < _temp4582.base? 1: _temp4584 >= _temp4582.last_plus_one){
_throw( Null_Exception);}* _temp4584;}).first_line,({ struct _tagged_ptr3
_temp4585= yyls; struct Cyc_Yyltype* _temp4587= _temp4585.curr + yylsp_offset;
if( _temp4587 < _temp4585.base? 1: _temp4587 >= _temp4585.last_plus_one){ _throw(
Null_Exception);}* _temp4587;}).last_line)); _temp4576;}); _temp4575;}); break;
case 178: _LL4574: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp4589=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4589[ 0]=({ struct Cyc_Exp_tok_struct _temp4590; _temp4590.tag= Cyc_Exp_tok_tag;
_temp4590.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Comprehension_e_struct*
_temp4591=( struct Cyc_Absyn_Comprehension_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Comprehension_e_struct)); _temp4591[ 0]=({ struct Cyc_Absyn_Comprehension_e_struct
_temp4592; _temp4592.tag= Cyc_Absyn_Comprehension_e_tag; _temp4592.f1= Cyc_Absyn_new_vardecl(({
struct _tuple1* _temp4593=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp4593->f1=( void*) Cyc_Absyn_Loc_n; _temp4593->f2=({ struct _tagged_string*
_temp4594=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp4594[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4595= yyvs;
struct _xtunion_struct** _temp4597= _temp4595.curr +( yyvsp_offset - 5); if(
_temp4597 < _temp4595.base? 1: _temp4597 >= _temp4595.last_plus_one){ _throw(
Null_Exception);}* _temp4597;})); _temp4594;}); _temp4593;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4598= yyls; struct Cyc_Yyltype* _temp4600= _temp4598.curr
+( yylsp_offset - 5); if( _temp4600 < _temp4598.base? 1: _temp4600 >= _temp4598.last_plus_one){
_throw( Null_Exception);}* _temp4600;}).first_line,({ struct _tagged_ptr3
_temp4601= yyls; struct Cyc_Yyltype* _temp4603= _temp4601.curr +( yylsp_offset -
5); if( _temp4603 < _temp4601.base? 1: _temp4603 >= _temp4601.last_plus_one){
_throw( Null_Exception);}* _temp4603;}).last_line))); _temp4592.f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4604= yyvs; struct _xtunion_struct** _temp4606=
_temp4604.curr +( yyvsp_offset - 3); if( _temp4606 < _temp4604.base? 1:
_temp4606 >= _temp4604.last_plus_one){ _throw( Null_Exception);}* _temp4606;}));
_temp4592.f3= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4607= yyvs; struct
_xtunion_struct** _temp4609= _temp4607.curr +( yyvsp_offset - 1); if( _temp4609
< _temp4607.base? 1: _temp4609 >= _temp4607.last_plus_one){ _throw(
Null_Exception);}* _temp4609;})); _temp4592;}); _temp4591;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4610= yyls; struct Cyc_Yyltype* _temp4612= _temp4610.curr
+( yylsp_offset - 7); if( _temp4612 < _temp4610.base? 1: _temp4612 >= _temp4610.last_plus_one){
_throw( Null_Exception);}* _temp4612;}).first_line,({ struct _tagged_ptr3
_temp4613= yyls; struct Cyc_Yyltype* _temp4615= _temp4613.curr + yylsp_offset;
if( _temp4615 < _temp4613.base? 1: _temp4615 >= _temp4613.last_plus_one){ _throw(
Null_Exception);}* _temp4615;}).last_line)); _temp4590;}); _temp4589;}); break;
case 179: _LL4588: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4617=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4617[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4618; _temp4618.tag= Cyc_InitializerList_tok_tag; _temp4618.f1=({ struct
Cyc_List_List* _temp4619=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4619->hd=( void*)({ struct _tuple18* _temp4620=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4620->f1= 0; _temp4620->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4621= yyvs; struct _xtunion_struct** _temp4623=
_temp4621.curr + yyvsp_offset; if( _temp4623 < _temp4621.base? 1: _temp4623 >=
_temp4621.last_plus_one){ _throw( Null_Exception);}* _temp4623;})); _temp4620;});
_temp4619->tl= 0; _temp4619;}); _temp4618;}); _temp4617;}); break; case 180:
_LL4616: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4625=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4625[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4626; _temp4626.tag= Cyc_InitializerList_tok_tag; _temp4626.f1=({ struct
Cyc_List_List* _temp4627=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4627->hd=( void*)({ struct _tuple18* _temp4628=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4628->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4632= yyvs; struct _xtunion_struct** _temp4634=
_temp4632.curr +( yyvsp_offset - 1); if( _temp4634 < _temp4632.base? 1:
_temp4634 >= _temp4632.last_plus_one){ _throw( Null_Exception);}* _temp4634;}));
_temp4628->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4629= yyvs; struct
_xtunion_struct** _temp4631= _temp4629.curr + yyvsp_offset; if( _temp4631 <
_temp4629.base? 1: _temp4631 >= _temp4629.last_plus_one){ _throw( Null_Exception);}*
_temp4631;})); _temp4628;}); _temp4627->tl= 0; _temp4627;}); _temp4626;});
_temp4625;}); break; case 181: _LL4624: yyval=( struct _xtunion_struct*)({
struct Cyc_InitializerList_tok_struct* _temp4636=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4636[ 0]=({
struct Cyc_InitializerList_tok_struct _temp4637; _temp4637.tag= Cyc_InitializerList_tok_tag;
_temp4637.f1=({ struct Cyc_List_List* _temp4638=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4638->hd=( void*)({ struct
_tuple18* _temp4642=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4642->f1= 0; _temp4642->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4643= yyvs; struct _xtunion_struct** _temp4645= _temp4643.curr +
yyvsp_offset; if( _temp4645 < _temp4643.base? 1: _temp4645 >= _temp4643.last_plus_one){
_throw( Null_Exception);}* _temp4645;})); _temp4642;}); _temp4638->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4639= yyvs; struct _xtunion_struct** _temp4641=
_temp4639.curr +( yyvsp_offset - 2); if( _temp4641 < _temp4639.base? 1:
_temp4641 >= _temp4639.last_plus_one){ _throw( Null_Exception);}* _temp4641;}));
_temp4638;}); _temp4637;}); _temp4636;}); break; case 182: _LL4635: yyval=(
struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct* _temp4647=(
struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp4647[ 0]=({ struct Cyc_InitializerList_tok_struct _temp4648; _temp4648.tag=
Cyc_InitializerList_tok_tag; _temp4648.f1=({ struct Cyc_List_List* _temp4649=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4649->hd=(
void*)({ struct _tuple18* _temp4653=( struct _tuple18*) GC_malloc( sizeof(
struct _tuple18)); _temp4653->f1= Cyc_yyget_DesignatorList_tok(({ struct
_tagged_ptr2 _temp4657= yyvs; struct _xtunion_struct** _temp4659= _temp4657.curr
+( yyvsp_offset - 1); if( _temp4659 < _temp4657.base? 1: _temp4659 >= _temp4657.last_plus_one){
_throw( Null_Exception);}* _temp4659;})); _temp4653->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4654= yyvs; struct _xtunion_struct** _temp4656=
_temp4654.curr + yyvsp_offset; if( _temp4656 < _temp4654.base? 1: _temp4656 >=
_temp4654.last_plus_one){ _throw( Null_Exception);}* _temp4656;})); _temp4653;});
_temp4649->tl= Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp4650=
yyvs; struct _xtunion_struct** _temp4652= _temp4650.curr +( yyvsp_offset - 3);
if( _temp4652 < _temp4650.base? 1: _temp4652 >= _temp4650.last_plus_one){ _throw(
Null_Exception);}* _temp4652;})); _temp4649;}); _temp4648;}); _temp4647;});
break; case 183: _LL4646: yyval=( struct _xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct*
_temp4661=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4661[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4662; _temp4662.tag=
Cyc_DesignatorList_tok_tag; _temp4662.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2
_temp4663= yyvs; struct _xtunion_struct** _temp4665= _temp4663.curr +(
yyvsp_offset - 1); if( _temp4665 < _temp4663.base? 1: _temp4665 >= _temp4663.last_plus_one){
_throw( Null_Exception);}* _temp4665;}))); _temp4662;}); _temp4661;}); break;
case 184: _LL4660: yyval=( struct _xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct*
_temp4667=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4667[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4668; _temp4668.tag=
Cyc_DesignatorList_tok_tag; _temp4668.f1=({ struct Cyc_List_List* _temp4669=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4669->hd=(
void*) Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4670= yyvs; struct
_xtunion_struct** _temp4672= _temp4670.curr + yyvsp_offset; if( _temp4672 <
_temp4670.base? 1: _temp4672 >= _temp4670.last_plus_one){ _throw( Null_Exception);}*
_temp4672;})); _temp4669->tl= 0; _temp4669;}); _temp4668;}); _temp4667;});
break; case 185: _LL4666: yyval=( struct _xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct*
_temp4674=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4674[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4675; _temp4675.tag=
Cyc_DesignatorList_tok_tag; _temp4675.f1=({ struct Cyc_List_List* _temp4676=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4676->hd=(
void*) Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4680= yyvs; struct
_xtunion_struct** _temp4682= _temp4680.curr + yyvsp_offset; if( _temp4682 <
_temp4680.base? 1: _temp4682 >= _temp4680.last_plus_one){ _throw( Null_Exception);}*
_temp4682;})); _temp4676->tl= Cyc_yyget_DesignatorList_tok(({ struct
_tagged_ptr2 _temp4677= yyvs; struct _xtunion_struct** _temp4679= _temp4677.curr
+( yyvsp_offset - 1); if( _temp4679 < _temp4677.base? 1: _temp4679 >= _temp4677.last_plus_one){
_throw( Null_Exception);}* _temp4679;})); _temp4676;}); _temp4675;}); _temp4674;});
break; case 186: _LL4673: yyval=( struct _xtunion_struct*)({ struct Cyc_Designator_tok_struct*
_temp4684=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4684[ 0]=({ struct Cyc_Designator_tok_struct _temp4685; _temp4685.tag= Cyc_Designator_tok_tag;
_temp4685.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp4686=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4686[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp4687; _temp4687.tag=
Cyc_Absyn_ArrayElement_tag; _temp4687.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4688= yyvs; struct _xtunion_struct** _temp4690= _temp4688.curr
+( yyvsp_offset - 1); if( _temp4690 < _temp4688.base? 1: _temp4690 >= _temp4688.last_plus_one){
_throw( Null_Exception);}* _temp4690;})); _temp4687;}); _temp4686;})); _temp4685;});
_temp4684;}); break; case 187: _LL4683: yyval=( struct _xtunion_struct*)({
struct Cyc_Designator_tok_struct* _temp4692=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp4692[ 0]=({ struct
Cyc_Designator_tok_struct _temp4693; _temp4693.tag= Cyc_Designator_tok_tag;
_temp4693.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4694=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4694[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4695; _temp4695.tag= Cyc_Absyn_FieldName_tag;
_temp4695.f1=({ struct _tagged_string* _temp4696=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4696[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4697= yyvs; struct _xtunion_struct** _temp4699=
_temp4697.curr + yyvsp_offset; if( _temp4699 < _temp4697.base? 1: _temp4699 >=
_temp4697.last_plus_one){ _throw( Null_Exception);}* _temp4699;})); _temp4696;});
_temp4695;}); _temp4694;})); _temp4693;}); _temp4692;}); break; case 188:
_LL4691: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4701= yyvs; struct _xtunion_struct** _temp4703=
_temp4701.curr + yyvsp_offset; if( _temp4703 < _temp4701.base? 1: _temp4703 >=
_temp4701.last_plus_one){ _throw( Null_Exception);}* _temp4703;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4704= yyls; struct Cyc_Yyltype* _temp4706= _temp4704.curr
+ yylsp_offset; if( _temp4706 < _temp4704.base? 1: _temp4706 >= _temp4704.last_plus_one){
_throw( Null_Exception);}* _temp4706;}).first_line,({ struct _tagged_ptr3
_temp4707= yyls; struct Cyc_Yyltype* _temp4709= _temp4707.curr + yylsp_offset;
if( _temp4709 < _temp4707.base? 1: _temp4709 >= _temp4707.last_plus_one){ _throw(
Null_Exception);}* _temp4709;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4710= yyvs; struct _xtunion_struct** _temp4712=
_temp4710.curr + yyvsp_offset; if( _temp4712 < _temp4710.base? 1: _temp4712 >=
_temp4710.last_plus_one){ _throw( Null_Exception);}* _temp4712;}))).f3; if( atts
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4713=( char*)"ignoring attributes in type";
struct _tagged_string _temp4714; _temp4714.curr= _temp4713; _temp4714.base=
_temp4713; _temp4714.last_plus_one= _temp4713 + 28; _temp4714;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4715= yyls; struct Cyc_Yyltype* _temp4717= _temp4715.curr
+ yylsp_offset; if( _temp4717 < _temp4715.base? 1: _temp4717 >= _temp4715.last_plus_one){
_throw( Null_Exception);}* _temp4717;}).first_line,({ struct _tagged_ptr3
_temp4718= yyls; struct Cyc_Yyltype* _temp4720= _temp4718.curr + yylsp_offset;
if( _temp4720 < _temp4718.base? 1: _temp4720 >= _temp4718.last_plus_one){ _throw(
Null_Exception);}* _temp4720;}).last_line));}{ struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4721= yyvs; struct _xtunion_struct** _temp4723=
_temp4721.curr + yyvsp_offset; if( _temp4723 < _temp4721.base? 1: _temp4723 >=
_temp4721.last_plus_one){ _throw( Null_Exception);}* _temp4723;}))).f1; yyval=(
struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4724=( struct
Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4724[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4725; _temp4725.tag= Cyc_ParamDecl_tok_tag;
_temp4725.f1=({ struct _tuple2* _temp4726=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp4726[ 0]=({ struct _tuple2 _temp4727; _temp4727.f1= 0;
_temp4727.f2= tq; _temp4727.f3= t; _temp4727;}); _temp4726;}); _temp4725;});
_temp4724;}); break;}} case 189: _LL4700: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4729= yyvs; struct
_xtunion_struct** _temp4731= _temp4729.curr +( yyvsp_offset - 1); if( _temp4731
< _temp4729.base? 1: _temp4731 >= _temp4729.last_plus_one){ _throw(
Null_Exception);}* _temp4731;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4732= yyls; struct Cyc_Yyltype* _temp4734= _temp4732.curr +(
yylsp_offset - 1); if( _temp4734 < _temp4732.base? 1: _temp4734 >= _temp4732.last_plus_one){
_throw( Null_Exception);}* _temp4734;}).first_line,({ struct _tagged_ptr3
_temp4735= yyls; struct Cyc_Yyltype* _temp4737= _temp4735.curr +( yylsp_offset -
1); if( _temp4737 < _temp4735.base? 1: _temp4737 >= _temp4735.last_plus_one){
_throw( Null_Exception);}* _temp4737;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4738= yyvs; struct
_xtunion_struct** _temp4740= _temp4738.curr +( yyvsp_offset - 1); if( _temp4740
< _temp4738.base? 1: _temp4740 >= _temp4738.last_plus_one){ _throw(
Null_Exception);}* _temp4740;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4741= yyvs; struct _xtunion_struct** _temp4743=
_temp4741.curr +( yyvsp_offset - 1); if( _temp4743 < _temp4741.base? 1:
_temp4743 >= _temp4741.last_plus_one){ _throw( Null_Exception);}* _temp4743;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4744= yyvs; struct _xtunion_struct** _temp4746= _temp4744.curr
+ yyvsp_offset; if( _temp4746 < _temp4744.base? 1: _temp4746 >= _temp4744.last_plus_one){
_throw( Null_Exception);}* _temp4746;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4747=( char*)"bad type params, ignoring"; struct _tagged_string
_temp4748; _temp4748.curr= _temp4747; _temp4748.base= _temp4747; _temp4748.last_plus_one=
_temp4747 + 26; _temp4748;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4749= yyls; struct Cyc_Yyltype* _temp4751= _temp4749.curr + yylsp_offset;
if( _temp4751 < _temp4749.base? 1: _temp4751 >= _temp4749.last_plus_one){ _throw(
Null_Exception);}* _temp4751;}).first_line,({ struct _tagged_ptr3 _temp4752=
yyls; struct Cyc_Yyltype* _temp4754= _temp4752.curr + yylsp_offset; if(
_temp4754 < _temp4752.base? 1: _temp4754 >= _temp4752.last_plus_one){ _throw(
Null_Exception);}* _temp4754;}).last_line));} if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4755=( char*)"bad specifiers, ignoring";
struct _tagged_string _temp4756; _temp4756.curr= _temp4755; _temp4756.base=
_temp4755; _temp4756.last_plus_one= _temp4755 + 25; _temp4756;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4757= yyls; struct Cyc_Yyltype* _temp4759= _temp4757.curr
+ yylsp_offset; if( _temp4759 < _temp4757.base? 1: _temp4759 >= _temp4757.last_plus_one){
_throw( Null_Exception);}* _temp4759;}).first_line,({ struct _tagged_ptr3
_temp4760= yyls; struct Cyc_Yyltype* _temp4762= _temp4760.curr + yylsp_offset;
if( _temp4762 < _temp4760.base? 1: _temp4762 >= _temp4760.last_plus_one){ _throw(
Null_Exception);}* _temp4762;}).last_line));} yyval=( struct _xtunion_struct*)({
struct Cyc_ParamDecl_tok_struct* _temp4763=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4763[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4764; _temp4764.tag= Cyc_ParamDecl_tok_tag;
_temp4764.f1=({ struct _tuple2* _temp4765=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4765->f1= 0; _temp4765->f2= t_info.f1; _temp4765->f3=
t_info.f2; _temp4765;}); _temp4764;}); _temp4763;}); break;} case 190: _LL4728:
yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct* _temp4767=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4767[ 0]=({ struct Cyc_Type_tok_struct _temp4768; _temp4768.tag= Cyc_Type_tok_tag;
_temp4768.f1=( void*)(* Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4769=
yyvs; struct _xtunion_struct** _temp4771= _temp4769.curr + yyvsp_offset; if(
_temp4771 < _temp4769.base? 1: _temp4771 >= _temp4769.last_plus_one){ _throw(
Null_Exception);}* _temp4771;}))).f3; _temp4768;}); _temp4767;}); break; case
191: _LL4766: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4773=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4773[ 0]=({ struct Cyc_Type_tok_struct _temp4774; _temp4774.tag= Cyc_Type_tok_tag;
_temp4774.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4775=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4775[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4776; _temp4776.tag= Cyc_Absyn_JoinEff_tag;
_temp4776.f1= 0; _temp4776;}); _temp4775;})); _temp4774;}); _temp4773;}); break;
case 192: _LL4772: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4778=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4778[ 0]=({ struct Cyc_Type_tok_struct _temp4779; _temp4779.tag= Cyc_Type_tok_tag;
_temp4779.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4780=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4780[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4781; _temp4781.tag= Cyc_Absyn_JoinEff_tag;
_temp4781.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4782= yyvs;
struct _xtunion_struct** _temp4784= _temp4782.curr +( yyvsp_offset - 1); if(
_temp4784 < _temp4782.base? 1: _temp4784 >= _temp4782.last_plus_one){ _throw(
Null_Exception);}* _temp4784;})); _temp4781;}); _temp4780;})); _temp4779;});
_temp4778;}); break; case 193: _LL4777: yyval=( struct _xtunion_struct*)({
struct Cyc_Type_tok_struct* _temp4786=( struct Cyc_Type_tok_struct*) GC_malloc(
sizeof( struct Cyc_Type_tok_struct)); _temp4786[ 0]=({ struct Cyc_Type_tok_struct
_temp4787; _temp4787.tag= Cyc_Type_tok_tag; _temp4787.f1=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp4788=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4788[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4789; _temp4789.tag= Cyc_Absyn_JoinEff_tag;
_temp4789.f1=({ struct Cyc_List_List* _temp4790=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4790->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4794= yyvs; struct _xtunion_struct** _temp4796=
_temp4794.curr +( yyvsp_offset - 2); if( _temp4796 < _temp4794.base? 1:
_temp4796 >= _temp4794.last_plus_one){ _throw( Null_Exception);}* _temp4796;}));
_temp4790->tl= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4791= yyvs;
struct _xtunion_struct** _temp4793= _temp4791.curr + yyvsp_offset; if( _temp4793
< _temp4791.base? 1: _temp4793 >= _temp4791.last_plus_one){ _throw(
Null_Exception);}* _temp4793;})); _temp4790;}); _temp4789;}); _temp4788;}));
_temp4787;}); _temp4786;}); break; case 194: _LL4785: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4798=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4798[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4799; _temp4799.tag= Cyc_TypeList_tok_tag; _temp4799.f1=({ struct Cyc_List_List*
_temp4800=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4800->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4801= yyvs;
struct _xtunion_struct** _temp4803= _temp4801.curr + yyvsp_offset; if( _temp4803
< _temp4801.base? 1: _temp4803 >= _temp4801.last_plus_one){ _throw(
Null_Exception);}* _temp4803;})); _temp4800->tl= 0; _temp4800;}); _temp4799;});
_temp4798;}); break; case 195: _LL4797: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp4805=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4805[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4806; _temp4806.tag= Cyc_TypeList_tok_tag; _temp4806.f1=({ struct Cyc_List_List*
_temp4807=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4807->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4811= yyvs;
struct _xtunion_struct** _temp4813= _temp4811.curr + yyvsp_offset; if( _temp4813
< _temp4811.base? 1: _temp4813 >= _temp4811.last_plus_one){ _throw(
Null_Exception);}* _temp4813;})); _temp4807->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4808= yyvs; struct _xtunion_struct** _temp4810=
_temp4808.curr +( yyvsp_offset - 2); if( _temp4810 < _temp4808.base? 1:
_temp4810 >= _temp4808.last_plus_one){ _throw( Null_Exception);}* _temp4810;}));
_temp4807;}); _temp4806;}); _temp4805;}); break; case 196: _LL4804: yyval=(
struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4815=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4815[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4816; _temp4816.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4816.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4817=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4817->tms= Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4818=
yyvs; struct _xtunion_struct** _temp4820= _temp4818.curr + yyvsp_offset; if(
_temp4820 < _temp4818.base? 1: _temp4820 >= _temp4818.last_plus_one){ _throw(
Null_Exception);}* _temp4820;})); _temp4817;}); _temp4816;}); _temp4815;});
break; case 197: _LL4814: yyval=({ struct _tagged_ptr2 _temp4822= yyvs; struct
_xtunion_struct** _temp4824= _temp4822.curr + yyvsp_offset; if( _temp4824 <
_temp4822.base? 1: _temp4824 >= _temp4822.last_plus_one){ _throw( Null_Exception);}*
_temp4824;}); break; case 198: _LL4821: yyval=( struct _xtunion_struct*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp4826=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4826[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4827; _temp4827.tag= Cyc_AbstractDeclarator_tok_tag;
_temp4827.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4828=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4828->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4829= yyvs; struct
_xtunion_struct** _temp4831= _temp4829.curr +( yyvsp_offset - 1); if( _temp4831
< _temp4829.base? 1: _temp4831 >= _temp4829.last_plus_one){ _throw(
Null_Exception);}* _temp4831;})),( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4832= yyvs; struct _xtunion_struct** _temp4834= _temp4832.curr
+ yyvsp_offset; if( _temp4834 < _temp4832.base? 1: _temp4834 >= _temp4832.last_plus_one){
_throw( Null_Exception);}* _temp4834;})))->tms); _temp4828;}); _temp4827;});
_temp4826;}); break; case 199: _LL4825: yyval=({ struct _tagged_ptr2 _temp4836=
yyvs; struct _xtunion_struct** _temp4838= _temp4836.curr +( yyvsp_offset - 1);
if( _temp4838 < _temp4836.base? 1: _temp4838 >= _temp4836.last_plus_one){ _throw(
Null_Exception);}* _temp4838;}); break; case 200: _LL4835: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4840=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4840[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4841; _temp4841.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4841.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4842=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4842->tms=({ struct Cyc_List_List* _temp4843=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4843->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4843->tl= 0; _temp4843;}); _temp4842;}); _temp4841;}); _temp4840;}); break;
case 201: _LL4839: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4845=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4845[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4846; _temp4846.tag= Cyc_AbstractDeclarator_tok_tag; _temp4846.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4847=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4847->tms=({
struct Cyc_List_List* _temp4848=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4848->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4848->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4849= yyvs; struct _xtunion_struct** _temp4851= _temp4849.curr +(
yyvsp_offset - 2); if( _temp4851 < _temp4849.base? 1: _temp4851 >= _temp4849.last_plus_one){
_throw( Null_Exception);}* _temp4851;})))->tms; _temp4848;}); _temp4847;});
_temp4846;}); _temp4845;}); break; case 202: _LL4844: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4853=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4853[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4854; _temp4854.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4854.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4855=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4855->tms=({ struct Cyc_List_List* _temp4856=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4856->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp4857=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp4857[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp4858; _temp4858.tag= Cyc_Absyn_ConstArray_mod_tag;
_temp4858.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4859= yyvs; struct
_xtunion_struct** _temp4861= _temp4859.curr +( yyvsp_offset - 1); if( _temp4861
< _temp4859.base? 1: _temp4861 >= _temp4859.last_plus_one){ _throw(
Null_Exception);}* _temp4861;})); _temp4858;}); _temp4857;})); _temp4856->tl= 0;
_temp4856;}); _temp4855;}); _temp4854;}); _temp4853;}); break; case 203: _LL4852:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4863=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4863[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4864; _temp4864.tag= Cyc_AbstractDeclarator_tok_tag; _temp4864.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4865=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4865->tms=({
struct Cyc_List_List* _temp4866=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4866->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4870=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4870[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4871; _temp4871.tag= Cyc_Absyn_ConstArray_mod_tag; _temp4871.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4872= yyvs; struct _xtunion_struct** _temp4874=
_temp4872.curr +( yyvsp_offset - 1); if( _temp4874 < _temp4872.base? 1:
_temp4874 >= _temp4872.last_plus_one){ _throw( Null_Exception);}* _temp4874;}));
_temp4871;}); _temp4870;})); _temp4866->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4867= yyvs; struct _xtunion_struct** _temp4869=
_temp4867.curr +( yyvsp_offset - 3); if( _temp4869 < _temp4867.base? 1:
_temp4869 >= _temp4867.last_plus_one){ _throw( Null_Exception);}* _temp4869;})))->tms;
_temp4866;}); _temp4865;}); _temp4864;}); _temp4863;}); break; case 204: _LL4862:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4876=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4876[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4877; _temp4877.tag= Cyc_AbstractDeclarator_tok_tag; _temp4877.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4878=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4878->tms=({
struct Cyc_List_List* _temp4879=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4879->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4880=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4880[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4881; _temp4881.tag=
Cyc_Absyn_Function_mod_tag; _temp4881.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4882=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4882[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4883; _temp4883.tag= Cyc_Absyn_WithTypes_tag;
_temp4883.f1= 0; _temp4883.f2= 0; _temp4883.f3= 0; _temp4883;}); _temp4882;}));
_temp4881;}); _temp4880;})); _temp4879->tl= 0; _temp4879;}); _temp4878;});
_temp4877;}); _temp4876;}); break; case 205: _LL4875: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4885=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4885[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4886; _temp4886.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4886.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4887=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4887->tms=({ struct Cyc_List_List* _temp4888=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4888->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4889=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4889[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4890; _temp4890.tag= Cyc_Absyn_Function_mod_tag;
_temp4890.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4891=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4891[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4892; _temp4892.tag= Cyc_Absyn_WithTypes_tag;
_temp4892.f1= 0; _temp4892.f2= 0; _temp4892.f3=({ struct Cyc_Core_Opt* _temp4893=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4893->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4894=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4894[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4895; _temp4895.tag= Cyc_Absyn_JoinEff_tag;
_temp4895.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4896= yyvs;
struct _xtunion_struct** _temp4898= _temp4896.curr +( yyvsp_offset - 1); if(
_temp4898 < _temp4896.base? 1: _temp4898 >= _temp4896.last_plus_one){ _throw(
Null_Exception);}* _temp4898;})); _temp4895;}); _temp4894;})); _temp4893;});
_temp4892;}); _temp4891;})); _temp4890;}); _temp4889;})); _temp4888->tl= 0;
_temp4888;}); _temp4887;}); _temp4886;}); _temp4885;}); break; case 206: _LL4884: {
struct _tuple16 _temp4905; struct Cyc_Core_Opt* _temp4906; int _temp4908; struct
Cyc_List_List* _temp4910; struct _tuple16* _temp4903= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp4900= yyvs; struct _xtunion_struct** _temp4902=
_temp4900.curr +( yyvsp_offset - 1); if( _temp4902 < _temp4900.base? 1:
_temp4902 >= _temp4900.last_plus_one){ _throw( Null_Exception);}* _temp4902;}));
_temp4905=* _temp4903; _LL4911: _temp4910= _temp4905.f1; goto _LL4909; _LL4909:
_temp4908= _temp4905.f2; goto _LL4907; _LL4907: _temp4906= _temp4905.f3; goto
_LL4904; _LL4904: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4912=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4912[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4913; _temp4913.tag= Cyc_AbstractDeclarator_tok_tag; _temp4913.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4914=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4914->tms=({
struct Cyc_List_List* _temp4915=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4915->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4916=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4916[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4917; _temp4917.tag=
Cyc_Absyn_Function_mod_tag; _temp4917.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4918=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4918[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4919; _temp4919.tag= Cyc_Absyn_WithTypes_tag;
_temp4919.f1= _temp4910; _temp4919.f2= _temp4908; _temp4919.f3= _temp4906;
_temp4919;}); _temp4918;})); _temp4917;}); _temp4916;})); _temp4915->tl= 0;
_temp4915;}); _temp4914;}); _temp4913;}); _temp4912;}); break;} case 207:
_LL4899: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4921=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4921[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4922; _temp4922.tag= Cyc_AbstractDeclarator_tok_tag; _temp4922.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4923=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4923->tms=({
struct Cyc_List_List* _temp4924=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4924->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4928=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4928[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4929; _temp4929.tag=
Cyc_Absyn_Function_mod_tag; _temp4929.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4930=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4930[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4931; _temp4931.tag= Cyc_Absyn_WithTypes_tag;
_temp4931.f1= 0; _temp4931.f2= 0; _temp4931.f3= 0; _temp4931;}); _temp4930;}));
_temp4929;}); _temp4928;})); _temp4924->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4925= yyvs; struct _xtunion_struct** _temp4927=
_temp4925.curr +( yyvsp_offset - 2); if( _temp4927 < _temp4925.base? 1:
_temp4927 >= _temp4925.last_plus_one){ _throw( Null_Exception);}* _temp4927;})))->tms;
_temp4924;}); _temp4923;}); _temp4922;}); _temp4921;}); break; case 208: _LL4920: {
struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt* _temp4933=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4933->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp4934=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4934[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4935; _temp4935.tag= Cyc_Absyn_JoinEff_tag;
_temp4935.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4936= yyvs;
struct _xtunion_struct** _temp4938= _temp4936.curr +( yyvsp_offset - 1); if(
_temp4938 < _temp4936.base? 1: _temp4938 >= _temp4936.last_plus_one){ _throw(
Null_Exception);}* _temp4938;})); _temp4935;}); _temp4934;})); _temp4933;});
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4939=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4939[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4940; _temp4940.tag= Cyc_AbstractDeclarator_tok_tag; _temp4940.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4941=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4941->tms=({
struct Cyc_List_List* _temp4942=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4942->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4946=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4946[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4947; _temp4947.tag=
Cyc_Absyn_Function_mod_tag; _temp4947.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4948=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4948[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4949; _temp4949.tag= Cyc_Absyn_WithTypes_tag;
_temp4949.f1= 0; _temp4949.f2= 0; _temp4949.f3= eff; _temp4949;}); _temp4948;}));
_temp4947;}); _temp4946;})); _temp4942->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4943= yyvs; struct _xtunion_struct** _temp4945=
_temp4943.curr +( yyvsp_offset - 4); if( _temp4945 < _temp4943.base? 1:
_temp4945 >= _temp4943.last_plus_one){ _throw( Null_Exception);}* _temp4945;})))->tms;
_temp4942;}); _temp4941;}); _temp4940;}); _temp4939;}); break;} case 209:
_LL4932: { struct _tuple16 _temp4956; struct Cyc_Core_Opt* _temp4957; int
_temp4959; struct Cyc_List_List* _temp4961; struct _tuple16* _temp4954= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp4951= yyvs; struct _xtunion_struct** _temp4953=
_temp4951.curr +( yyvsp_offset - 1); if( _temp4953 < _temp4951.base? 1:
_temp4953 >= _temp4951.last_plus_one){ _throw( Null_Exception);}* _temp4953;}));
_temp4956=* _temp4954; _LL4962: _temp4961= _temp4956.f1; goto _LL4960; _LL4960:
_temp4959= _temp4956.f2; goto _LL4958; _LL4958: _temp4957= _temp4956.f3; goto
_LL4955; _LL4955: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4963=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4963[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4964; _temp4964.tag= Cyc_AbstractDeclarator_tok_tag; _temp4964.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4965=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4965->tms=({
struct Cyc_List_List* _temp4966=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4966->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4970=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4970[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4971; _temp4971.tag=
Cyc_Absyn_Function_mod_tag; _temp4971.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4972=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4972[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4973; _temp4973.tag= Cyc_Absyn_WithTypes_tag;
_temp4973.f1= _temp4961; _temp4973.f2= _temp4959; _temp4973.f3= _temp4957;
_temp4973;}); _temp4972;})); _temp4971;}); _temp4970;})); _temp4966->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4967= yyvs; struct _xtunion_struct** _temp4969=
_temp4967.curr +( yyvsp_offset - 3); if( _temp4969 < _temp4967.base? 1:
_temp4969 >= _temp4967.last_plus_one){ _throw( Null_Exception);}* _temp4969;})))->tms;
_temp4966;}); _temp4965;}); _temp4964;}); _temp4963;}); break;} case 210:
_LL4950: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4975= yyls; struct Cyc_Yyltype* _temp4977= _temp4975.curr
+( yylsp_offset - 2); if( _temp4977 < _temp4975.base? 1: _temp4977 >= _temp4975.last_plus_one){
_throw( Null_Exception);}* _temp4977;}).first_line,({ struct _tagged_ptr3
_temp4978= yyls; struct Cyc_Yyltype* _temp4980= _temp4978.curr + yylsp_offset;
if( _temp4980 < _temp4978.base? 1: _temp4980 >= _temp4978.last_plus_one){ _throw(
Null_Exception);}* _temp4980;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4981= yyvs; struct _xtunion_struct** _temp4983= _temp4981.curr
+( yyvsp_offset - 1); if( _temp4983 < _temp4981.base? 1: _temp4983 >= _temp4981.last_plus_one){
_throw( Null_Exception);}* _temp4983;})))); yyval=( struct _xtunion_struct*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp4984=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4984[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4985; _temp4985.tag= Cyc_AbstractDeclarator_tok_tag;
_temp4985.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4986=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4986->tms=({
struct Cyc_List_List* _temp4987=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4987->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4991=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4991[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp4992; _temp4992.tag= Cyc_Absyn_TypeParams_mod_tag; _temp4992.f1= ts;
_temp4992.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4993= yyls;
struct Cyc_Yyltype* _temp4995= _temp4993.curr +( yylsp_offset - 2); if(
_temp4995 < _temp4993.base? 1: _temp4995 >= _temp4993.last_plus_one){ _throw(
Null_Exception);}* _temp4995;}).first_line,({ struct _tagged_ptr3 _temp4996=
yyls; struct Cyc_Yyltype* _temp4998= _temp4996.curr + yylsp_offset; if(
_temp4998 < _temp4996.base? 1: _temp4998 >= _temp4996.last_plus_one){ _throw(
Null_Exception);}* _temp4998;}).last_line); _temp4992.f3= 0; _temp4992;});
_temp4991;})); _temp4987->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4988= yyvs; struct _xtunion_struct** _temp4990= _temp4988.curr
+( yyvsp_offset - 3); if( _temp4990 < _temp4988.base? 1: _temp4990 >= _temp4988.last_plus_one){
_throw( Null_Exception);}* _temp4990;})))->tms; _temp4987;}); _temp4986;});
_temp4985;}); _temp4984;}); break;} case 211: _LL4974:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5000= yyls; struct Cyc_Yyltype* _temp5002= _temp5000.curr
+( yylsp_offset - 2); if( _temp5002 < _temp5000.base? 1: _temp5002 >= _temp5000.last_plus_one){
_throw( Null_Exception);}* _temp5002;}).first_line,({ struct _tagged_ptr3
_temp5003= yyls; struct Cyc_Yyltype* _temp5005= _temp5003.curr + yylsp_offset;
if( _temp5005 < _temp5003.base? 1: _temp5005 >= _temp5003.last_plus_one){ _throw(
Null_Exception);}* _temp5005;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp5006= yyvs; struct _xtunion_struct** _temp5008= _temp5006.curr
+( yyvsp_offset - 1); if( _temp5008 < _temp5006.base? 1: _temp5008 >= _temp5006.last_plus_one){
_throw( Null_Exception);}* _temp5008;})))); yyval=( struct _xtunion_struct*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp5009=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5009[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5010; _temp5010.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5010.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5011=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5011->tms=({
struct Cyc_List_List* _temp5012=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5012->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5016=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5016[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5017; _temp5017.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5017.f1= ts;
_temp5017.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5018= yyls;
struct Cyc_Yyltype* _temp5020= _temp5018.curr +( yylsp_offset - 2); if(
_temp5020 < _temp5018.base? 1: _temp5020 >= _temp5018.last_plus_one){ _throw(
Null_Exception);}* _temp5020;}).first_line,({ struct _tagged_ptr3 _temp5021=
yyls; struct Cyc_Yyltype* _temp5023= _temp5021.curr + yylsp_offset; if(
_temp5023 < _temp5021.base? 1: _temp5023 >= _temp5021.last_plus_one){ _throw(
Null_Exception);}* _temp5023;}).last_line); _temp5017.f3= 0; _temp5017;});
_temp5016;})); _temp5012->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp5013= yyvs; struct _xtunion_struct** _temp5015= _temp5013.curr
+( yyvsp_offset - 3); if( _temp5015 < _temp5013.base? 1: _temp5015 >= _temp5013.last_plus_one){
_throw( Null_Exception);}* _temp5015;})))->tms; _temp5012;}); _temp5011;});
_temp5010;}); _temp5009;}); break;} case 212: _LL4999: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5025=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5025[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5026; _temp5026.tag=
Cyc_AbstractDeclarator_tok_tag; _temp5026.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5027=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5027->tms=({ struct Cyc_List_List* _temp5028=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5028->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp5032=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp5032[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp5033; _temp5033.tag= Cyc_Absyn_Attributes_mod_tag;
_temp5033.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5034= yyls;
struct Cyc_Yyltype* _temp5036= _temp5034.curr + yylsp_offset; if( _temp5036 <
_temp5034.base? 1: _temp5036 >= _temp5034.last_plus_one){ _throw( Null_Exception);}*
_temp5036;}).first_line,({ struct _tagged_ptr3 _temp5037= yyls; struct Cyc_Yyltype*
_temp5039= _temp5037.curr + yylsp_offset; if( _temp5039 < _temp5037.base? 1:
_temp5039 >= _temp5037.last_plus_one){ _throw( Null_Exception);}* _temp5039;}).last_line);
_temp5033.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp5040= yyvs;
struct _xtunion_struct** _temp5042= _temp5040.curr + yyvsp_offset; if( _temp5042
< _temp5040.base? 1: _temp5042 >= _temp5040.last_plus_one){ _throw(
Null_Exception);}* _temp5042;})); _temp5033;}); _temp5032;})); _temp5028->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp5029= yyvs; struct
_xtunion_struct** _temp5031= _temp5029.curr +( yyvsp_offset - 1); if( _temp5031
< _temp5029.base? 1: _temp5031 >= _temp5029.last_plus_one){ _throw(
Null_Exception);}* _temp5031;})))->tms; _temp5028;}); _temp5027;}); _temp5026;});
_temp5025;}); break; case 213: _LL5024: yyval=({ struct _tagged_ptr2 _temp5044=
yyvs; struct _xtunion_struct** _temp5046= _temp5044.curr + yyvsp_offset; if(
_temp5046 < _temp5044.base? 1: _temp5046 >= _temp5044.last_plus_one){ _throw(
Null_Exception);}* _temp5046;}); break; case 214: _LL5043: yyval=({ struct
_tagged_ptr2 _temp5048= yyvs; struct _xtunion_struct** _temp5050= _temp5048.curr
+ yyvsp_offset; if( _temp5050 < _temp5048.base? 1: _temp5050 >= _temp5048.last_plus_one){
_throw( Null_Exception);}* _temp5050;}); break; case 215: _LL5047: yyval=({
struct _tagged_ptr2 _temp5052= yyvs; struct _xtunion_struct** _temp5054=
_temp5052.curr + yyvsp_offset; if( _temp5054 < _temp5052.base? 1: _temp5054 >=
_temp5052.last_plus_one){ _throw( Null_Exception);}* _temp5054;}); break; case
216: _LL5051: yyval=({ struct _tagged_ptr2 _temp5056= yyvs; struct
_xtunion_struct** _temp5058= _temp5056.curr + yyvsp_offset; if( _temp5058 <
_temp5056.base? 1: _temp5058 >= _temp5056.last_plus_one){ _throw( Null_Exception);}*
_temp5058;}); break; case 217: _LL5055: yyval=({ struct _tagged_ptr2 _temp5060=
yyvs; struct _xtunion_struct** _temp5062= _temp5060.curr + yyvsp_offset; if(
_temp5062 < _temp5060.base? 1: _temp5062 >= _temp5060.last_plus_one){ _throw(
Null_Exception);}* _temp5062;}); break; case 218: _LL5059: yyval=({ struct
_tagged_ptr2 _temp5064= yyvs; struct _xtunion_struct** _temp5066= _temp5064.curr
+ yyvsp_offset; if( _temp5066 < _temp5064.base? 1: _temp5066 >= _temp5064.last_plus_one){
_throw( Null_Exception);}* _temp5066;}); break; case 219: _LL5063: yyval=(
struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5068=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5068[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5069; _temp5069.tag= Cyc_Stmt_tok_tag; _temp5069.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Cut_s_struct* _temp5070=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp5070[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp5071; _temp5071.tag= Cyc_Absyn_Cut_s_tag; _temp5071.f1= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5072= yyvs; struct _xtunion_struct** _temp5074=
_temp5072.curr + yyvsp_offset; if( _temp5074 < _temp5072.base? 1: _temp5074 >=
_temp5072.last_plus_one){ _throw( Null_Exception);}* _temp5074;})); _temp5071;});
_temp5070;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5075= yyls;
struct Cyc_Yyltype* _temp5077= _temp5075.curr +( yylsp_offset - 1); if(
_temp5077 < _temp5075.base? 1: _temp5077 >= _temp5075.last_plus_one){ _throw(
Null_Exception);}* _temp5077;}).first_line,({ struct _tagged_ptr3 _temp5078=
yyls; struct Cyc_Yyltype* _temp5080= _temp5078.curr + yylsp_offset; if(
_temp5080 < _temp5078.base? 1: _temp5080 >= _temp5078.last_plus_one){ _throw(
Null_Exception);}* _temp5080;}).last_line)); _temp5069;}); _temp5068;}); break;
case 220: _LL5067: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5082=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5082[ 0]=({ struct Cyc_Stmt_tok_struct _temp5083; _temp5083.tag= Cyc_Stmt_tok_tag;
_temp5083.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp5084=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp5084[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp5085; _temp5085.tag= Cyc_Absyn_Splice_s_tag;
_temp5085.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5086= yyvs; struct
_xtunion_struct** _temp5088= _temp5086.curr + yyvsp_offset; if( _temp5088 <
_temp5086.base? 1: _temp5088 >= _temp5086.last_plus_one){ _throw( Null_Exception);}*
_temp5088;})); _temp5085;}); _temp5084;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5089= yyls; struct Cyc_Yyltype* _temp5091= _temp5089.curr +(
yylsp_offset - 1); if( _temp5091 < _temp5089.base? 1: _temp5091 >= _temp5089.last_plus_one){
_throw( Null_Exception);}* _temp5091;}).first_line,({ struct _tagged_ptr3
_temp5092= yyls; struct Cyc_Yyltype* _temp5094= _temp5092.curr + yylsp_offset;
if( _temp5094 < _temp5092.base? 1: _temp5094 >= _temp5092.last_plus_one){ _throw(
Null_Exception);}* _temp5094;}).last_line)); _temp5083;}); _temp5082;}); break;
case 221: _LL5081: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5096=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5096[ 0]=({ struct Cyc_Stmt_tok_struct _temp5097; _temp5097.tag= Cyc_Stmt_tok_tag;
_temp5097.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp5098=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp5098[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp5099; _temp5099.tag= Cyc_Absyn_Label_s_tag;
_temp5099.f1=({ struct _tagged_string* _temp5100=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5100[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5101= yyvs; struct _xtunion_struct** _temp5103=
_temp5101.curr +( yyvsp_offset - 2); if( _temp5103 < _temp5101.base? 1:
_temp5103 >= _temp5101.last_plus_one){ _throw( Null_Exception);}* _temp5103;}));
_temp5100;}); _temp5099.f2= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5104=
yyvs; struct _xtunion_struct** _temp5106= _temp5104.curr + yyvsp_offset; if(
_temp5106 < _temp5104.base? 1: _temp5106 >= _temp5104.last_plus_one){ _throw(
Null_Exception);}* _temp5106;})); _temp5099;}); _temp5098;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5107= yyls; struct Cyc_Yyltype* _temp5109= _temp5107.curr
+( yylsp_offset - 2); if( _temp5109 < _temp5107.base? 1: _temp5109 >= _temp5107.last_plus_one){
_throw( Null_Exception);}* _temp5109;}).first_line,({ struct _tagged_ptr3
_temp5110= yyls; struct Cyc_Yyltype* _temp5112= _temp5110.curr + yylsp_offset;
if( _temp5112 < _temp5110.base? 1: _temp5112 >= _temp5110.last_plus_one){ _throw(
Null_Exception);}* _temp5112;}).last_line)); _temp5097;}); _temp5096;}); break;
case 222: _LL5095: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5114=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5114[ 0]=({ struct Cyc_Stmt_tok_struct _temp5115; _temp5115.tag= Cyc_Stmt_tok_tag;
_temp5115.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5116= yyls; struct Cyc_Yyltype* _temp5118= _temp5116.curr +
yylsp_offset; if( _temp5118 < _temp5116.base? 1: _temp5118 >= _temp5116.last_plus_one){
_throw( Null_Exception);}* _temp5118;}).first_line,({ struct _tagged_ptr3
_temp5119= yyls; struct Cyc_Yyltype* _temp5121= _temp5119.curr + yylsp_offset;
if( _temp5121 < _temp5119.base? 1: _temp5121 >= _temp5119.last_plus_one){ _throw(
Null_Exception);}* _temp5121;}).last_line)); _temp5115;}); _temp5114;}); break;
case 223: _LL5113: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5123=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5123[ 0]=({ struct Cyc_Stmt_tok_struct _temp5124; _temp5124.tag= Cyc_Stmt_tok_tag;
_temp5124.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5125= yyvs; struct _xtunion_struct** _temp5127= _temp5125.curr +(
yyvsp_offset - 1); if( _temp5127 < _temp5125.base? 1: _temp5127 >= _temp5125.last_plus_one){
_throw( Null_Exception);}* _temp5127;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5128= yyls; struct Cyc_Yyltype* _temp5130= _temp5128.curr +(
yylsp_offset - 1); if( _temp5130 < _temp5128.base? 1: _temp5130 >= _temp5128.last_plus_one){
_throw( Null_Exception);}* _temp5130;}).first_line,({ struct _tagged_ptr3
_temp5131= yyls; struct Cyc_Yyltype* _temp5133= _temp5131.curr + yylsp_offset;
if( _temp5133 < _temp5131.base? 1: _temp5133 >= _temp5131.last_plus_one){ _throw(
Null_Exception);}* _temp5133;}).last_line)); _temp5124;}); _temp5123;}); break;
case 224: _LL5122: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5135=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5135[ 0]=({ struct Cyc_Stmt_tok_struct _temp5136; _temp5136.tag= Cyc_Stmt_tok_tag;
_temp5136.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5137= yyls; struct Cyc_Yyltype* _temp5139= _temp5137.curr +(
yylsp_offset - 1); if( _temp5139 < _temp5137.base? 1: _temp5139 >= _temp5137.last_plus_one){
_throw( Null_Exception);}* _temp5139;}).first_line,({ struct _tagged_ptr3
_temp5140= yyls; struct Cyc_Yyltype* _temp5142= _temp5140.curr + yylsp_offset;
if( _temp5142 < _temp5140.base? 1: _temp5142 >= _temp5140.last_plus_one){ _throw(
Null_Exception);}* _temp5142;}).last_line)); _temp5136;}); _temp5135;}); break;
case 225: _LL5134: yyval=({ struct _tagged_ptr2 _temp5144= yyvs; struct
_xtunion_struct** _temp5146= _temp5144.curr +( yyvsp_offset - 1); if( _temp5146
< _temp5144.base? 1: _temp5146 >= _temp5144.last_plus_one){ _throw(
Null_Exception);}* _temp5146;}); break; case 226: _LL5143:{ void* _temp5151= Cyc_yyget_BlockItem_tok(({
struct _tagged_ptr2 _temp5148= yyvs; struct _xtunion_struct** _temp5150=
_temp5148.curr + yyvsp_offset; if( _temp5150 < _temp5148.base? 1: _temp5150 >=
_temp5148.last_plus_one){ _throw( Null_Exception);}* _temp5150;})); struct Cyc_List_List*
_temp5159; struct Cyc_Absyn_Fndecl* _temp5161; struct Cyc_Absyn_Stmt* _temp5163;
_LL5153: if((( struct _tunion_struct*) _temp5151)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5160: _temp5159=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5151)->f1; goto _LL5154;} else{ goto _LL5155;} _LL5155: if((( struct
_tunion_struct*) _temp5151)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5162: _temp5161=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5151)->f1;
goto _LL5156;} else{ goto _LL5157;} _LL5157: if((( struct _tunion_struct*)
_temp5151)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5164: _temp5163=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5151)->f1; goto _LL5158;} else{ goto
_LL5152;} _LL5154: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5165=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5165[ 0]=({ struct Cyc_Stmt_tok_struct _temp5166; _temp5166.tag= Cyc_Stmt_tok_tag;
_temp5166.f1= Cyc_Parse_flatten_declarations( _temp5159, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5167= yyls; struct Cyc_Yyltype*
_temp5169= _temp5167.curr + yylsp_offset; if( _temp5169 < _temp5167.base? 1:
_temp5169 >= _temp5167.last_plus_one){ _throw( Null_Exception);}* _temp5169;}).first_line,({
struct _tagged_ptr3 _temp5170= yyls; struct Cyc_Yyltype* _temp5172= _temp5170.curr
+ yylsp_offset; if( _temp5172 < _temp5170.base? 1: _temp5172 >= _temp5170.last_plus_one){
_throw( Null_Exception);}* _temp5172;}).last_line))); _temp5166;}); _temp5165;});
goto _LL5152; _LL5156: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5173=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5173[ 0]=({ struct Cyc_Stmt_tok_struct _temp5174; _temp5174.tag= Cyc_Stmt_tok_tag;
_temp5174.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5175=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5175[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5176; _temp5176.tag= Cyc_Absyn_Fn_d_tag;
_temp5176.f1= _temp5161; _temp5176;}); _temp5175;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5177= yyls; struct Cyc_Yyltype* _temp5179= _temp5177.curr
+ yylsp_offset; if( _temp5179 < _temp5177.base? 1: _temp5179 >= _temp5177.last_plus_one){
_throw( Null_Exception);}* _temp5179;}).first_line,({ struct _tagged_ptr3
_temp5180= yyls; struct Cyc_Yyltype* _temp5182= _temp5180.curr + yylsp_offset;
if( _temp5182 < _temp5180.base? 1: _temp5182 >= _temp5180.last_plus_one){ _throw(
Null_Exception);}* _temp5182;}).last_line)), Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5183= yyls; struct Cyc_Yyltype* _temp5185= _temp5183.curr
+ yylsp_offset; if( _temp5185 < _temp5183.base? 1: _temp5185 >= _temp5183.last_plus_one){
_throw( Null_Exception);}* _temp5185;}).first_line,({ struct _tagged_ptr3
_temp5186= yyls; struct Cyc_Yyltype* _temp5188= _temp5186.curr + yylsp_offset;
if( _temp5188 < _temp5186.base? 1: _temp5188 >= _temp5186.last_plus_one){ _throw(
Null_Exception);}* _temp5188;}).last_line))); _temp5174;}); _temp5173;}); goto
_LL5152; _LL5158: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5189=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5189[ 0]=({ struct Cyc_Stmt_tok_struct _temp5190; _temp5190.tag= Cyc_Stmt_tok_tag;
_temp5190.f1= _temp5163; _temp5190;}); _temp5189;}); goto _LL5152; _LL5152:;}
break; case 227: _LL5147:{ void* _temp5195= Cyc_yyget_BlockItem_tok(({ struct
_tagged_ptr2 _temp5192= yyvs; struct _xtunion_struct** _temp5194= _temp5192.curr
+( yyvsp_offset - 1); if( _temp5194 < _temp5192.base? 1: _temp5194 >= _temp5192.last_plus_one){
_throw( Null_Exception);}* _temp5194;})); struct Cyc_List_List* _temp5203;
struct Cyc_Absyn_Fndecl* _temp5205; struct Cyc_Absyn_Stmt* _temp5207; _LL5197:
if((( struct _tunion_struct*) _temp5195)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5204: _temp5203=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5195)->f1; goto _LL5198;} else{ goto _LL5199;} _LL5199: if((( struct
_tunion_struct*) _temp5195)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5206: _temp5205=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5195)->f1;
goto _LL5200;} else{ goto _LL5201;} _LL5201: if((( struct _tunion_struct*)
_temp5195)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5208: _temp5207=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5195)->f1; goto _LL5202;} else{ goto
_LL5196;} _LL5198: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5209=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5209[ 0]=({ struct Cyc_Stmt_tok_struct _temp5210; _temp5210.tag= Cyc_Stmt_tok_tag;
_temp5210.f1= Cyc_Parse_flatten_declarations( _temp5203, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5211= yyvs; struct _xtunion_struct** _temp5213=
_temp5211.curr + yyvsp_offset; if( _temp5213 < _temp5211.base? 1: _temp5213 >=
_temp5211.last_plus_one){ _throw( Null_Exception);}* _temp5213;}))); _temp5210;});
_temp5209;}); goto _LL5196; _LL5200: yyval=( struct _xtunion_struct*)({ struct
Cyc_Stmt_tok_struct* _temp5214=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5214[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5215; _temp5215.tag= Cyc_Stmt_tok_tag; _temp5215.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp5216=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5216[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5217; _temp5217.tag= Cyc_Absyn_Fn_d_tag;
_temp5217.f1= _temp5205; _temp5217;}); _temp5216;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5218= yyls; struct Cyc_Yyltype* _temp5220= _temp5218.curr
+( yylsp_offset - 1); if( _temp5220 < _temp5218.base? 1: _temp5220 >= _temp5218.last_plus_one){
_throw( Null_Exception);}* _temp5220;}).first_line,({ struct _tagged_ptr3
_temp5221= yyls; struct Cyc_Yyltype* _temp5223= _temp5221.curr +( yylsp_offset -
1); if( _temp5223 < _temp5221.base? 1: _temp5223 >= _temp5221.last_plus_one){
_throw( Null_Exception);}* _temp5223;}).last_line)), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5224= yyvs; struct _xtunion_struct** _temp5226=
_temp5224.curr + yyvsp_offset; if( _temp5226 < _temp5224.base? 1: _temp5226 >=
_temp5224.last_plus_one){ _throw( Null_Exception);}* _temp5226;}))); _temp5215;});
_temp5214;}); goto _LL5196; _LL5202: yyval=( struct _xtunion_struct*)({ struct
Cyc_Stmt_tok_struct* _temp5227=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5227[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5228; _temp5228.tag= Cyc_Stmt_tok_tag; _temp5228.f1= Cyc_Absyn_seq_stmt(
_temp5207, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5229= yyvs; struct
_xtunion_struct** _temp5231= _temp5229.curr + yyvsp_offset; if( _temp5231 <
_temp5229.base? 1: _temp5231 >= _temp5229.last_plus_one){ _throw( Null_Exception);}*
_temp5231;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5232=
yyls; struct Cyc_Yyltype* _temp5234= _temp5232.curr +( yylsp_offset - 1); if(
_temp5234 < _temp5232.base? 1: _temp5234 >= _temp5232.last_plus_one){ _throw(
Null_Exception);}* _temp5234;}).first_line,({ struct _tagged_ptr3 _temp5235=
yyls; struct Cyc_Yyltype* _temp5237= _temp5235.curr + yylsp_offset; if(
_temp5237 < _temp5235.base? 1: _temp5237 >= _temp5235.last_plus_one){ _throw(
Null_Exception);}* _temp5237;}).last_line)); _temp5228;}); _temp5227;}); goto
_LL5196; _LL5196:;} break; case 228: _LL5191: yyval=( struct _xtunion_struct*)({
struct Cyc_BlockItem_tok_struct* _temp5239=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5239[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5240; _temp5240.tag= Cyc_BlockItem_tok_tag;
_temp5240.f1=( void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp5241=(
struct Cyc_Parse_TopDecls_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct));
_temp5241[ 0]=({ struct Cyc_Parse_TopDecls_bl_struct _temp5242; _temp5242.tag=
Cyc_Parse_TopDecls_bl_tag; _temp5242.f1= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp5243= yyvs; struct _xtunion_struct** _temp5245= _temp5243.curr
+ yyvsp_offset; if( _temp5245 < _temp5243.base? 1: _temp5245 >= _temp5243.last_plus_one){
_throw( Null_Exception);}* _temp5245;})); _temp5242;}); _temp5241;})); _temp5240;});
_temp5239;}); break; case 229: _LL5238: yyval=( struct _xtunion_struct*)({
struct Cyc_BlockItem_tok_struct* _temp5247=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5247[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5248; _temp5248.tag= Cyc_BlockItem_tok_tag;
_temp5248.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp5249=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp5249[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp5250; _temp5250.tag= Cyc_Parse_Stmt_bl_tag;
_temp5250.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5251= yyvs; struct
_xtunion_struct** _temp5253= _temp5251.curr + yyvsp_offset; if( _temp5253 <
_temp5251.base? 1: _temp5253 >= _temp5251.last_plus_one){ _throw( Null_Exception);}*
_temp5253;})); _temp5250;}); _temp5249;})); _temp5248;}); _temp5247;}); break;
case 230: _LL5246: yyval=( struct _xtunion_struct*)({ struct Cyc_BlockItem_tok_struct*
_temp5255=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5255[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5256; _temp5256.tag= Cyc_BlockItem_tok_tag;
_temp5256.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp5257=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp5257[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp5258; _temp5258.tag= Cyc_Parse_FnDecl_bl_tag;
_temp5258.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp5259= yyvs;
struct _xtunion_struct** _temp5261= _temp5259.curr + yyvsp_offset; if( _temp5261
< _temp5259.base? 1: _temp5261 >= _temp5259.last_plus_one){ _throw(
Null_Exception);}* _temp5261;})); _temp5258;}); _temp5257;})); _temp5256;});
_temp5255;}); break; case 231: _LL5254: yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5263=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5263[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5264; _temp5264.tag= Cyc_Stmt_tok_tag; _temp5264.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5265= yyvs; struct _xtunion_struct**
_temp5267= _temp5265.curr +( yyvsp_offset - 2); if( _temp5267 < _temp5265.base?
1: _temp5267 >= _temp5265.last_plus_one){ _throw( Null_Exception);}* _temp5267;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5268= yyvs; struct
_xtunion_struct** _temp5270= _temp5268.curr + yyvsp_offset; if( _temp5270 <
_temp5268.base? 1: _temp5270 >= _temp5268.last_plus_one){ _throw( Null_Exception);}*
_temp5270;})), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5271= yyls; struct Cyc_Yyltype* _temp5273= _temp5271.curr +(
yylsp_offset - 4); if( _temp5273 < _temp5271.base? 1: _temp5273 >= _temp5271.last_plus_one){
_throw( Null_Exception);}* _temp5273;}).first_line,({ struct _tagged_ptr3
_temp5274= yyls; struct Cyc_Yyltype* _temp5276= _temp5274.curr + yylsp_offset;
if( _temp5276 < _temp5274.base? 1: _temp5276 >= _temp5274.last_plus_one){ _throw(
Null_Exception);}* _temp5276;}).last_line)); _temp5264;}); _temp5263;}); break;
case 232: _LL5262: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5278=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5278[ 0]=({ struct Cyc_Stmt_tok_struct _temp5279; _temp5279.tag= Cyc_Stmt_tok_tag;
_temp5279.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5280= yyvs; struct _xtunion_struct** _temp5282= _temp5280.curr
+( yyvsp_offset - 4); if( _temp5282 < _temp5280.base? 1: _temp5282 >= _temp5280.last_plus_one){
_throw( Null_Exception);}* _temp5282;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5283= yyvs; struct _xtunion_struct** _temp5285= _temp5283.curr
+( yyvsp_offset - 2); if( _temp5285 < _temp5283.base? 1: _temp5285 >= _temp5283.last_plus_one){
_throw( Null_Exception);}* _temp5285;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5286= yyvs; struct _xtunion_struct** _temp5288= _temp5286.curr
+ yyvsp_offset; if( _temp5288 < _temp5286.base? 1: _temp5288 >= _temp5286.last_plus_one){
_throw( Null_Exception);}* _temp5288;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5289= yyls; struct Cyc_Yyltype* _temp5291= _temp5289.curr +(
yylsp_offset - 6); if( _temp5291 < _temp5289.base? 1: _temp5291 >= _temp5289.last_plus_one){
_throw( Null_Exception);}* _temp5291;}).first_line,({ struct _tagged_ptr3
_temp5292= yyls; struct Cyc_Yyltype* _temp5294= _temp5292.curr + yylsp_offset;
if( _temp5294 < _temp5292.base? 1: _temp5294 >= _temp5292.last_plus_one){ _throw(
Null_Exception);}* _temp5294;}).last_line)); _temp5279;}); _temp5278;}); break;
case 233: _LL5277: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5296=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5296[ 0]=({ struct Cyc_Stmt_tok_struct _temp5297; _temp5297.tag= Cyc_Stmt_tok_tag;
_temp5297.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5298= yyvs; struct _xtunion_struct** _temp5300= _temp5298.curr +(
yyvsp_offset - 4); if( _temp5300 < _temp5298.base? 1: _temp5300 >= _temp5298.last_plus_one){
_throw( Null_Exception);}* _temp5300;})), Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp5301= yyvs; struct _xtunion_struct** _temp5303=
_temp5301.curr +( yyvsp_offset - 1); if( _temp5303 < _temp5301.base? 1:
_temp5303 >= _temp5301.last_plus_one){ _throw( Null_Exception);}* _temp5303;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5304= yyls; struct Cyc_Yyltype*
_temp5306= _temp5304.curr +( yylsp_offset - 6); if( _temp5306 < _temp5304.base?
1: _temp5306 >= _temp5304.last_plus_one){ _throw( Null_Exception);}* _temp5306;}).first_line,({
struct _tagged_ptr3 _temp5307= yyls; struct Cyc_Yyltype* _temp5309= _temp5307.curr
+ yylsp_offset; if( _temp5309 < _temp5307.base? 1: _temp5309 >= _temp5307.last_plus_one){
_throw( Null_Exception);}* _temp5309;}).last_line)); _temp5297;}); _temp5296;});
break; case 234: _LL5295: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5311=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5311[ 0]=({ struct Cyc_Stmt_tok_struct _temp5312; _temp5312.tag= Cyc_Stmt_tok_tag;
_temp5312.f1= Cyc_Absyn_trycatch_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5313= yyvs; struct _xtunion_struct** _temp5315= _temp5313.curr +(
yyvsp_offset - 4); if( _temp5315 < _temp5313.base? 1: _temp5315 >= _temp5313.last_plus_one){
_throw( Null_Exception);}* _temp5315;})), Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp5316= yyvs; struct _xtunion_struct** _temp5318=
_temp5316.curr +( yyvsp_offset - 1); if( _temp5318 < _temp5316.base? 1:
_temp5318 >= _temp5316.last_plus_one){ _throw( Null_Exception);}* _temp5318;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5319= yyls; struct Cyc_Yyltype*
_temp5321= _temp5319.curr +( yylsp_offset - 5); if( _temp5321 < _temp5319.base?
1: _temp5321 >= _temp5319.last_plus_one){ _throw( Null_Exception);}* _temp5321;}).first_line,({
struct _tagged_ptr3 _temp5322= yyls; struct Cyc_Yyltype* _temp5324= _temp5322.curr
+ yylsp_offset; if( _temp5324 < _temp5322.base? 1: _temp5324 >= _temp5322.last_plus_one){
_throw( Null_Exception);}* _temp5324;}).last_line)); _temp5312;}); _temp5311;});
break; case 235: _LL5310: yyval=( struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5326=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5326[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5327; _temp5327.tag= Cyc_SwitchClauseList_tok_tag; _temp5327.f1= 0;
_temp5327;}); _temp5326;}); break; case 236: _LL5325: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5329=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5329[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5330; _temp5330.tag=
Cyc_SwitchClauseList_tok_tag; _temp5330.f1=({ struct Cyc_List_List* _temp5331=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5331->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5332=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5332->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5342= yyls; struct Cyc_Yyltype* _temp5344= _temp5342.curr +( yylsp_offset -
2); if( _temp5344 < _temp5342.base? 1: _temp5344 >= _temp5342.last_plus_one){
_throw( Null_Exception);}* _temp5344;}).first_line,({ struct _tagged_ptr3
_temp5345= yyls; struct Cyc_Yyltype* _temp5347= _temp5345.curr +( yylsp_offset -
2); if( _temp5347 < _temp5345.base? 1: _temp5347 >= _temp5345.last_plus_one){
_throw( Null_Exception);}* _temp5347;}).last_line)); _temp5332->pat_vars= 0;
_temp5332->where_clause= 0; _temp5332->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5339= yyvs; struct _xtunion_struct** _temp5341= _temp5339.curr
+ yyvsp_offset; if( _temp5341 < _temp5339.base? 1: _temp5341 >= _temp5339.last_plus_one){
_throw( Null_Exception);}* _temp5341;})); _temp5332->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5333= yyls; struct Cyc_Yyltype* _temp5335= _temp5333.curr
+( yylsp_offset - 2); if( _temp5335 < _temp5333.base? 1: _temp5335 >= _temp5333.last_plus_one){
_throw( Null_Exception);}* _temp5335;}).first_line,({ struct _tagged_ptr3
_temp5336= yyls; struct Cyc_Yyltype* _temp5338= _temp5336.curr + yylsp_offset;
if( _temp5338 < _temp5336.base? 1: _temp5338 >= _temp5336.last_plus_one){ _throw(
Null_Exception);}* _temp5338;}).last_line); _temp5332;}); _temp5331->tl= 0;
_temp5331;}); _temp5330;}); _temp5329;}); break; case 237: _LL5328: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5349=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5349[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5350; _temp5350.tag=
Cyc_SwitchClauseList_tok_tag; _temp5350.f1=({ struct Cyc_List_List* _temp5351=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5351->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5355=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5355->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5368= yyvs; struct _xtunion_struct** _temp5370=
_temp5368.curr +( yyvsp_offset - 2); if( _temp5370 < _temp5368.base? 1:
_temp5370 >= _temp5368.last_plus_one){ _throw( Null_Exception);}* _temp5370;}));
_temp5355->pat_vars= 0; _temp5355->where_clause= 0; _temp5355->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5362= yyls; struct Cyc_Yyltype*
_temp5364= _temp5362.curr +( yylsp_offset - 1); if( _temp5364 < _temp5362.base?
1: _temp5364 >= _temp5362.last_plus_one){ _throw( Null_Exception);}* _temp5364;}).first_line,({
struct _tagged_ptr3 _temp5365= yyls; struct Cyc_Yyltype* _temp5367= _temp5365.curr
+( yylsp_offset - 1); if( _temp5367 < _temp5365.base? 1: _temp5367 >= _temp5365.last_plus_one){
_throw( Null_Exception);}* _temp5367;}).last_line)); _temp5355->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5356= yyls; struct Cyc_Yyltype* _temp5358= _temp5356.curr
+( yylsp_offset - 3); if( _temp5358 < _temp5356.base? 1: _temp5358 >= _temp5356.last_plus_one){
_throw( Null_Exception);}* _temp5358;}).first_line,({ struct _tagged_ptr3
_temp5359= yyls; struct Cyc_Yyltype* _temp5361= _temp5359.curr + yylsp_offset;
if( _temp5361 < _temp5359.base? 1: _temp5361 >= _temp5359.last_plus_one){ _throw(
Null_Exception);}* _temp5361;}).last_line); _temp5355;}); _temp5351->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp5352= yyvs; struct _xtunion_struct** _temp5354=
_temp5352.curr + yyvsp_offset; if( _temp5354 < _temp5352.base? 1: _temp5354 >=
_temp5352.last_plus_one){ _throw( Null_Exception);}* _temp5354;})); _temp5351;});
_temp5350;}); _temp5349;}); break; case 238: _LL5348: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5372=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5372[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5373; _temp5373.tag=
Cyc_SwitchClauseList_tok_tag; _temp5373.f1=({ struct Cyc_List_List* _temp5374=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5374->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5378=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5378->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5388= yyvs; struct _xtunion_struct** _temp5390=
_temp5388.curr +( yyvsp_offset - 3); if( _temp5390 < _temp5388.base? 1:
_temp5390 >= _temp5388.last_plus_one){ _throw( Null_Exception);}* _temp5390;}));
_temp5378->pat_vars= 0; _temp5378->where_clause= 0; _temp5378->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5385= yyvs; struct _xtunion_struct** _temp5387=
_temp5385.curr +( yyvsp_offset - 1); if( _temp5387 < _temp5385.base? 1:
_temp5387 >= _temp5385.last_plus_one){ _throw( Null_Exception);}* _temp5387;}));
_temp5378->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5379=
yyls; struct Cyc_Yyltype* _temp5381= _temp5379.curr +( yylsp_offset - 4); if(
_temp5381 < _temp5379.base? 1: _temp5381 >= _temp5379.last_plus_one){ _throw(
Null_Exception);}* _temp5381;}).first_line,({ struct _tagged_ptr3 _temp5382=
yyls; struct Cyc_Yyltype* _temp5384= _temp5382.curr +( yylsp_offset - 1); if(
_temp5384 < _temp5382.base? 1: _temp5384 >= _temp5382.last_plus_one){ _throw(
Null_Exception);}* _temp5384;}).last_line); _temp5378;}); _temp5374->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp5375= yyvs; struct _xtunion_struct** _temp5377=
_temp5375.curr + yyvsp_offset; if( _temp5377 < _temp5375.base? 1: _temp5377 >=
_temp5375.last_plus_one){ _throw( Null_Exception);}* _temp5377;})); _temp5374;});
_temp5373;}); _temp5372;}); break; case 239: _LL5371: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5392=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5392[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5393; _temp5393.tag=
Cyc_SwitchClauseList_tok_tag; _temp5393.f1=({ struct Cyc_List_List* _temp5394=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5394->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5398=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5398->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5414= yyvs; struct _xtunion_struct** _temp5416=
_temp5414.curr +( yyvsp_offset - 4); if( _temp5416 < _temp5414.base? 1:
_temp5416 >= _temp5414.last_plus_one){ _throw( Null_Exception);}* _temp5416;}));
_temp5398->pat_vars= 0; _temp5398->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5411= yyvs; struct _xtunion_struct** _temp5413=
_temp5411.curr +( yyvsp_offset - 2); if( _temp5413 < _temp5411.base? 1:
_temp5413 >= _temp5411.last_plus_one){ _throw( Null_Exception);}* _temp5413;}));
_temp5398->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5405= yyls; struct Cyc_Yyltype* _temp5407= _temp5405.curr +(
yylsp_offset - 1); if( _temp5407 < _temp5405.base? 1: _temp5407 >= _temp5405.last_plus_one){
_throw( Null_Exception);}* _temp5407;}).first_line,({ struct _tagged_ptr3
_temp5408= yyls; struct Cyc_Yyltype* _temp5410= _temp5408.curr +( yylsp_offset -
1); if( _temp5410 < _temp5408.base? 1: _temp5410 >= _temp5408.last_plus_one){
_throw( Null_Exception);}* _temp5410;}).last_line)); _temp5398->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5399= yyls; struct Cyc_Yyltype* _temp5401= _temp5399.curr
+( yylsp_offset - 5); if( _temp5401 < _temp5399.base? 1: _temp5401 >= _temp5399.last_plus_one){
_throw( Null_Exception);}* _temp5401;}).first_line,({ struct _tagged_ptr3
_temp5402= yyls; struct Cyc_Yyltype* _temp5404= _temp5402.curr + yylsp_offset;
if( _temp5404 < _temp5402.base? 1: _temp5404 >= _temp5402.last_plus_one){ _throw(
Null_Exception);}* _temp5404;}).last_line); _temp5398;}); _temp5394->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp5395= yyvs; struct _xtunion_struct** _temp5397=
_temp5395.curr + yyvsp_offset; if( _temp5397 < _temp5395.base? 1: _temp5397 >=
_temp5395.last_plus_one){ _throw( Null_Exception);}* _temp5397;})); _temp5394;});
_temp5393;}); _temp5392;}); break; case 240: _LL5391: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5418=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5418[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5419; _temp5419.tag=
Cyc_SwitchClauseList_tok_tag; _temp5419.f1=({ struct Cyc_List_List* _temp5420=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5420->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5424=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5424->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5437= yyvs; struct _xtunion_struct** _temp5439=
_temp5437.curr +( yyvsp_offset - 5); if( _temp5439 < _temp5437.base? 1:
_temp5439 >= _temp5437.last_plus_one){ _throw( Null_Exception);}* _temp5439;}));
_temp5424->pat_vars= 0; _temp5424->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5434= yyvs; struct _xtunion_struct** _temp5436=
_temp5434.curr +( yyvsp_offset - 3); if( _temp5436 < _temp5434.base? 1:
_temp5436 >= _temp5434.last_plus_one){ _throw( Null_Exception);}* _temp5436;}));
_temp5424->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5431= yyvs;
struct _xtunion_struct** _temp5433= _temp5431.curr +( yyvsp_offset - 1); if(
_temp5433 < _temp5431.base? 1: _temp5433 >= _temp5431.last_plus_one){ _throw(
Null_Exception);}* _temp5433;})); _temp5424->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5425= yyls; struct Cyc_Yyltype* _temp5427= _temp5425.curr
+( yylsp_offset - 6); if( _temp5427 < _temp5425.base? 1: _temp5427 >= _temp5425.last_plus_one){
_throw( Null_Exception);}* _temp5427;}).first_line,({ struct _tagged_ptr3
_temp5428= yyls; struct Cyc_Yyltype* _temp5430= _temp5428.curr + yylsp_offset;
if( _temp5430 < _temp5428.base? 1: _temp5430 >= _temp5428.last_plus_one){ _throw(
Null_Exception);}* _temp5430;}).last_line); _temp5424;}); _temp5420->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp5421= yyvs; struct _xtunion_struct** _temp5423=
_temp5421.curr + yyvsp_offset; if( _temp5423 < _temp5421.base? 1: _temp5423 >=
_temp5421.last_plus_one){ _throw( Null_Exception);}* _temp5423;})); _temp5420;});
_temp5419;}); _temp5418;}); break; case 241: _LL5417: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5441=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5441[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5442; _temp5442.tag= Cyc_Stmt_tok_tag; _temp5442.f1= Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5443= yyvs; struct _xtunion_struct**
_temp5445= _temp5443.curr +( yyvsp_offset - 2); if( _temp5445 < _temp5443.base?
1: _temp5445 >= _temp5443.last_plus_one){ _throw( Null_Exception);}* _temp5445;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5446= yyvs; struct
_xtunion_struct** _temp5448= _temp5446.curr + yyvsp_offset; if( _temp5448 <
_temp5446.base? 1: _temp5448 >= _temp5446.last_plus_one){ _throw( Null_Exception);}*
_temp5448;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5449=
yyls; struct Cyc_Yyltype* _temp5451= _temp5449.curr +( yylsp_offset - 4); if(
_temp5451 < _temp5449.base? 1: _temp5451 >= _temp5449.last_plus_one){ _throw(
Null_Exception);}* _temp5451;}).first_line,({ struct _tagged_ptr3 _temp5452=
yyls; struct Cyc_Yyltype* _temp5454= _temp5452.curr + yylsp_offset; if(
_temp5454 < _temp5452.base? 1: _temp5454 >= _temp5452.last_plus_one){ _throw(
Null_Exception);}* _temp5454;}).last_line)); _temp5442;}); _temp5441;}); break;
case 242: _LL5440: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5456=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5456[ 0]=({ struct Cyc_Stmt_tok_struct _temp5457; _temp5457.tag= Cyc_Stmt_tok_tag;
_temp5457.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5458= yyvs; struct _xtunion_struct** _temp5460= _temp5458.curr +(
yyvsp_offset - 5); if( _temp5460 < _temp5458.base? 1: _temp5460 >= _temp5458.last_plus_one){
_throw( Null_Exception);}* _temp5460;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5461= yyvs; struct _xtunion_struct** _temp5463= _temp5461.curr
+( yyvsp_offset - 2); if( _temp5463 < _temp5461.base? 1: _temp5463 >= _temp5461.last_plus_one){
_throw( Null_Exception);}* _temp5463;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5464= yyls; struct Cyc_Yyltype* _temp5466= _temp5464.curr +(
yylsp_offset - 6); if( _temp5466 < _temp5464.base? 1: _temp5466 >= _temp5464.last_plus_one){
_throw( Null_Exception);}* _temp5466;}).first_line,({ struct _tagged_ptr3
_temp5467= yyls; struct Cyc_Yyltype* _temp5469= _temp5467.curr + yylsp_offset;
if( _temp5469 < _temp5467.base? 1: _temp5469 >= _temp5467.last_plus_one){ _throw(
Null_Exception);}* _temp5469;}).last_line)); _temp5457;}); _temp5456;}); break;
case 243: _LL5455: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5471=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5471[ 0]=({ struct Cyc_Stmt_tok_struct _temp5472; _temp5472.tag= Cyc_Stmt_tok_tag;
_temp5472.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5473=
yyvs; struct _xtunion_struct** _temp5475= _temp5473.curr + yyvsp_offset; if(
_temp5475 < _temp5473.base? 1: _temp5475 >= _temp5473.last_plus_one){ _throw(
Null_Exception);}* _temp5475;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5476= yyls; struct Cyc_Yyltype* _temp5478= _temp5476.curr +(
yylsp_offset - 5); if( _temp5478 < _temp5476.base? 1: _temp5478 >= _temp5476.last_plus_one){
_throw( Null_Exception);}* _temp5478;}).first_line,({ struct _tagged_ptr3
_temp5479= yyls; struct Cyc_Yyltype* _temp5481= _temp5479.curr + yylsp_offset;
if( _temp5481 < _temp5479.base? 1: _temp5481 >= _temp5479.last_plus_one){ _throw(
Null_Exception);}* _temp5481;}).last_line)); _temp5472;}); _temp5471;}); break;
case 244: _LL5470: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5483=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5483[ 0]=({ struct Cyc_Stmt_tok_struct _temp5484; _temp5484.tag= Cyc_Stmt_tok_tag;
_temp5484.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5485= yyvs; struct _xtunion_struct**
_temp5487= _temp5485.curr +( yyvsp_offset - 2); if( _temp5487 < _temp5485.base?
1: _temp5487 >= _temp5485.last_plus_one){ _throw( Null_Exception);}* _temp5487;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5488= yyvs; struct
_xtunion_struct** _temp5490= _temp5488.curr + yyvsp_offset; if( _temp5490 <
_temp5488.base? 1: _temp5490 >= _temp5488.last_plus_one){ _throw( Null_Exception);}*
_temp5490;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5491=
yyls; struct Cyc_Yyltype* _temp5493= _temp5491.curr +( yylsp_offset - 6); if(
_temp5493 < _temp5491.base? 1: _temp5493 >= _temp5491.last_plus_one){ _throw(
Null_Exception);}* _temp5493;}).first_line,({ struct _tagged_ptr3 _temp5494=
yyls; struct Cyc_Yyltype* _temp5496= _temp5494.curr + yylsp_offset; if(
_temp5496 < _temp5494.base? 1: _temp5496 >= _temp5494.last_plus_one){ _throw(
Null_Exception);}* _temp5496;}).last_line)); _temp5484;}); _temp5483;}); break;
case 245: _LL5482: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5498=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5498[ 0]=({ struct Cyc_Stmt_tok_struct _temp5499; _temp5499.tag= Cyc_Stmt_tok_tag;
_temp5499.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5500= yyvs; struct _xtunion_struct** _temp5502=
_temp5500.curr +( yyvsp_offset - 3); if( _temp5502 < _temp5500.base? 1:
_temp5502 >= _temp5500.last_plus_one){ _throw( Null_Exception);}* _temp5502;})),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5503=
yyvs; struct _xtunion_struct** _temp5505= _temp5503.curr + yyvsp_offset; if(
_temp5505 < _temp5503.base? 1: _temp5505 >= _temp5503.last_plus_one){ _throw(
Null_Exception);}* _temp5505;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5506= yyls; struct Cyc_Yyltype* _temp5508= _temp5506.curr +(
yylsp_offset - 6); if( _temp5508 < _temp5506.base? 1: _temp5508 >= _temp5506.last_plus_one){
_throw( Null_Exception);}* _temp5508;}).first_line,({ struct _tagged_ptr3
_temp5509= yyls; struct Cyc_Yyltype* _temp5511= _temp5509.curr + yylsp_offset;
if( _temp5511 < _temp5509.base? 1: _temp5511 >= _temp5509.last_plus_one){ _throw(
Null_Exception);}* _temp5511;}).last_line)); _temp5499;}); _temp5498;}); break;
case 246: _LL5497: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5513=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5513[ 0]=({ struct Cyc_Stmt_tok_struct _temp5514; _temp5514.tag= Cyc_Stmt_tok_tag;
_temp5514.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5515= yyvs; struct _xtunion_struct** _temp5517=
_temp5515.curr +( yyvsp_offset - 4); if( _temp5517 < _temp5515.base? 1:
_temp5517 >= _temp5515.last_plus_one){ _throw( Null_Exception);}* _temp5517;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5518= yyvs; struct _xtunion_struct**
_temp5520= _temp5518.curr +( yyvsp_offset - 2); if( _temp5520 < _temp5518.base?
1: _temp5520 >= _temp5518.last_plus_one){ _throw( Null_Exception);}* _temp5520;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5521= yyvs; struct
_xtunion_struct** _temp5523= _temp5521.curr + yyvsp_offset; if( _temp5523 <
_temp5521.base? 1: _temp5523 >= _temp5521.last_plus_one){ _throw( Null_Exception);}*
_temp5523;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5524=
yyls; struct Cyc_Yyltype* _temp5526= _temp5524.curr +( yylsp_offset - 7); if(
_temp5526 < _temp5524.base? 1: _temp5526 >= _temp5524.last_plus_one){ _throw(
Null_Exception);}* _temp5526;}).first_line,({ struct _tagged_ptr3 _temp5527=
yyls; struct Cyc_Yyltype* _temp5529= _temp5527.curr +( yylsp_offset - 1); if(
_temp5529 < _temp5527.base? 1: _temp5529 >= _temp5527.last_plus_one){ _throw(
Null_Exception);}* _temp5529;}).last_line)); _temp5514;}); _temp5513;}); break;
case 247: _LL5512: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5531=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5531[ 0]=({ struct Cyc_Stmt_tok_struct _temp5532; _temp5532.tag= Cyc_Stmt_tok_tag;
_temp5532.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5533= yyvs; struct _xtunion_struct** _temp5535= _temp5533.curr +(
yyvsp_offset - 4); if( _temp5535 < _temp5533.base? 1: _temp5535 >= _temp5533.last_plus_one){
_throw( Null_Exception);}* _temp5535;})), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5536= yyvs; struct
_xtunion_struct** _temp5538= _temp5536.curr + yyvsp_offset; if( _temp5538 <
_temp5536.base? 1: _temp5538 >= _temp5536.last_plus_one){ _throw( Null_Exception);}*
_temp5538;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5539=
yyls; struct Cyc_Yyltype* _temp5541= _temp5539.curr +( yylsp_offset - 6); if(
_temp5541 < _temp5539.base? 1: _temp5541 >= _temp5539.last_plus_one){ _throw(
Null_Exception);}* _temp5541;}).first_line,({ struct _tagged_ptr3 _temp5542=
yyls; struct Cyc_Yyltype* _temp5544= _temp5542.curr + yylsp_offset; if(
_temp5544 < _temp5542.base? 1: _temp5544 >= _temp5542.last_plus_one){ _throw(
Null_Exception);}* _temp5544;}).last_line)); _temp5532;}); _temp5531;}); break;
case 248: _LL5530: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5546=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5546[ 0]=({ struct Cyc_Stmt_tok_struct _temp5547; _temp5547.tag= Cyc_Stmt_tok_tag;
_temp5547.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5548= yyvs; struct _xtunion_struct** _temp5550= _temp5548.curr +(
yyvsp_offset - 5); if( _temp5550 < _temp5548.base? 1: _temp5550 >= _temp5548.last_plus_one){
_throw( Null_Exception);}* _temp5550;})), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5551= yyvs; struct _xtunion_struct** _temp5553=
_temp5551.curr +( yyvsp_offset - 2); if( _temp5553 < _temp5551.base? 1:
_temp5553 >= _temp5551.last_plus_one){ _throw( Null_Exception);}* _temp5553;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5554= yyvs; struct
_xtunion_struct** _temp5556= _temp5554.curr + yyvsp_offset; if( _temp5556 <
_temp5554.base? 1: _temp5556 >= _temp5554.last_plus_one){ _throw( Null_Exception);}*
_temp5556;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5557=
yyls; struct Cyc_Yyltype* _temp5559= _temp5557.curr +( yylsp_offset - 7); if(
_temp5559 < _temp5557.base? 1: _temp5559 >= _temp5557.last_plus_one){ _throw(
Null_Exception);}* _temp5559;}).first_line,({ struct _tagged_ptr3 _temp5560=
yyls; struct Cyc_Yyltype* _temp5562= _temp5560.curr + yylsp_offset; if(
_temp5562 < _temp5560.base? 1: _temp5562 >= _temp5560.last_plus_one){ _throw(
Null_Exception);}* _temp5562;}).last_line)); _temp5547;}); _temp5546;}); break;
case 249: _LL5545: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5564=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5564[ 0]=({ struct Cyc_Stmt_tok_struct _temp5565; _temp5565.tag= Cyc_Stmt_tok_tag;
_temp5565.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5566= yyvs; struct _xtunion_struct** _temp5568= _temp5566.curr +(
yyvsp_offset - 5); if( _temp5568 < _temp5566.base? 1: _temp5568 >= _temp5566.last_plus_one){
_throw( Null_Exception);}* _temp5568;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5569= yyvs; struct _xtunion_struct** _temp5571= _temp5569.curr
+( yyvsp_offset - 3); if( _temp5571 < _temp5569.base? 1: _temp5571 >= _temp5569.last_plus_one){
_throw( Null_Exception);}* _temp5571;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5572= yyvs; struct _xtunion_struct** _temp5574=
_temp5572.curr + yyvsp_offset; if( _temp5574 < _temp5572.base? 1: _temp5574 >=
_temp5572.last_plus_one){ _throw( Null_Exception);}* _temp5574;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5575= yyls; struct Cyc_Yyltype* _temp5577= _temp5575.curr
+( yylsp_offset - 7); if( _temp5577 < _temp5575.base? 1: _temp5577 >= _temp5575.last_plus_one){
_throw( Null_Exception);}* _temp5577;}).first_line,({ struct _tagged_ptr3
_temp5578= yyls; struct Cyc_Yyltype* _temp5580= _temp5578.curr + yylsp_offset;
if( _temp5580 < _temp5578.base? 1: _temp5580 >= _temp5578.last_plus_one){ _throw(
Null_Exception);}* _temp5580;}).last_line)); _temp5565;}); _temp5564;}); break;
case 250: _LL5563: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5582=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5582[ 0]=({ struct Cyc_Stmt_tok_struct _temp5583; _temp5583.tag= Cyc_Stmt_tok_tag;
_temp5583.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5584= yyvs; struct _xtunion_struct** _temp5586= _temp5584.curr +(
yyvsp_offset - 6); if( _temp5586 < _temp5584.base? 1: _temp5586 >= _temp5584.last_plus_one){
_throw( Null_Exception);}* _temp5586;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5587= yyvs; struct _xtunion_struct** _temp5589= _temp5587.curr
+( yyvsp_offset - 4); if( _temp5589 < _temp5587.base? 1: _temp5589 >= _temp5587.last_plus_one){
_throw( Null_Exception);}* _temp5589;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5590= yyvs; struct _xtunion_struct** _temp5592= _temp5590.curr
+( yyvsp_offset - 2); if( _temp5592 < _temp5590.base? 1: _temp5592 >= _temp5590.last_plus_one){
_throw( Null_Exception);}* _temp5592;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5593= yyvs; struct _xtunion_struct** _temp5595= _temp5593.curr
+ yyvsp_offset; if( _temp5595 < _temp5593.base? 1: _temp5595 >= _temp5593.last_plus_one){
_throw( Null_Exception);}* _temp5595;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5596= yyls; struct Cyc_Yyltype* _temp5598= _temp5596.curr +(
yylsp_offset - 8); if( _temp5598 < _temp5596.base? 1: _temp5598 >= _temp5596.last_plus_one){
_throw( Null_Exception);}* _temp5598;}).first_line,({ struct _tagged_ptr3
_temp5599= yyls; struct Cyc_Yyltype* _temp5601= _temp5599.curr + yylsp_offset;
if( _temp5601 < _temp5599.base? 1: _temp5601 >= _temp5599.last_plus_one){ _throw(
Null_Exception);}* _temp5601;}).last_line)); _temp5583;}); _temp5582;}); break;
case 251: _LL5581: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5603= yyvs; struct _xtunion_struct** _temp5605=
_temp5603.curr +( yyvsp_offset - 3); if( _temp5605 < _temp5603.base? 1:
_temp5605 >= _temp5603.last_plus_one){ _throw( Null_Exception);}* _temp5605;}));
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5606=
yyvs; struct _xtunion_struct** _temp5608= _temp5606.curr + yyvsp_offset; if(
_temp5608 < _temp5606.base? 1: _temp5608 >= _temp5606.last_plus_one){ _throw(
Null_Exception);}* _temp5608;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5609= yyls; struct Cyc_Yyltype* _temp5611= _temp5609.curr +(
yylsp_offset - 5); if( _temp5611 < _temp5609.base? 1: _temp5611 >= _temp5609.last_plus_one){
_throw( Null_Exception);}* _temp5611;}).first_line,({ struct _tagged_ptr3
_temp5612= yyls; struct Cyc_Yyltype* _temp5614= _temp5612.curr + yylsp_offset;
if( _temp5614 < _temp5612.base? 1: _temp5614 >= _temp5612.last_plus_one){ _throw(
Null_Exception);}* _temp5614;}).last_line)); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5615=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5615[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5616; _temp5616.tag= Cyc_Stmt_tok_tag; _temp5616.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5616;}); _temp5615;}); break;} case 252: _LL5602: { struct Cyc_List_List*
decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5618= yyvs; struct
_xtunion_struct** _temp5620= _temp5618.curr +( yyvsp_offset - 4); if( _temp5620
< _temp5618.base? 1: _temp5620 >= _temp5618.last_plus_one){ _throw(
Null_Exception);}* _temp5620;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5621=
yyvs; struct _xtunion_struct** _temp5623= _temp5621.curr +( yyvsp_offset - 3);
if( _temp5623 < _temp5621.base? 1: _temp5623 >= _temp5621.last_plus_one){ _throw(
Null_Exception);}* _temp5623;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5624= yyvs; struct _xtunion_struct** _temp5626=
_temp5624.curr + yyvsp_offset; if( _temp5626 < _temp5624.base? 1: _temp5626 >=
_temp5624.last_plus_one){ _throw( Null_Exception);}* _temp5626;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5627= yyls; struct Cyc_Yyltype* _temp5629= _temp5627.curr
+( yylsp_offset - 6); if( _temp5629 < _temp5627.base? 1: _temp5629 >= _temp5627.last_plus_one){
_throw( Null_Exception);}* _temp5629;}).first_line,({ struct _tagged_ptr3
_temp5630= yyls; struct Cyc_Yyltype* _temp5632= _temp5630.curr + yylsp_offset;
if( _temp5632 < _temp5630.base? 1: _temp5632 >= _temp5630.last_plus_one){ _throw(
Null_Exception);}* _temp5632;}).last_line)); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5633=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5633[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5634; _temp5634.tag= Cyc_Stmt_tok_tag; _temp5634.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5634;}); _temp5633;}); break;} case 253: _LL5617: { struct Cyc_List_List*
decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5636= yyvs; struct
_xtunion_struct** _temp5638= _temp5636.curr +( yyvsp_offset - 4); if( _temp5638
< _temp5636.base? 1: _temp5638 >= _temp5636.last_plus_one){ _throw(
Null_Exception);}* _temp5638;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5639= yyvs; struct _xtunion_struct** _temp5641= _temp5639.curr
+( yyvsp_offset - 2); if( _temp5641 < _temp5639.base? 1: _temp5641 >= _temp5639.last_plus_one){
_throw( Null_Exception);}* _temp5641;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5642= yyvs; struct _xtunion_struct** _temp5644= _temp5642.curr
+ yyvsp_offset; if( _temp5644 < _temp5642.base? 1: _temp5644 >= _temp5642.last_plus_one){
_throw( Null_Exception);}* _temp5644;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5645= yyls; struct Cyc_Yyltype* _temp5647= _temp5645.curr +(
yylsp_offset - 6); if( _temp5647 < _temp5645.base? 1: _temp5647 >= _temp5645.last_plus_one){
_throw( Null_Exception);}* _temp5647;}).first_line,({ struct _tagged_ptr3
_temp5648= yyls; struct Cyc_Yyltype* _temp5650= _temp5648.curr + yylsp_offset;
if( _temp5650 < _temp5648.base? 1: _temp5650 >= _temp5648.last_plus_one){ _throw(
Null_Exception);}* _temp5650;}).last_line)); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5651=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5651[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5652; _temp5652.tag= Cyc_Stmt_tok_tag; _temp5652.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5652;}); _temp5651;}); break;} case 254: _LL5635: { struct Cyc_List_List*
decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5654= yyvs; struct
_xtunion_struct** _temp5656= _temp5654.curr +( yyvsp_offset - 5); if( _temp5656
< _temp5654.base? 1: _temp5656 >= _temp5654.last_plus_one){ _throw(
Null_Exception);}* _temp5656;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5657=
yyvs; struct _xtunion_struct** _temp5659= _temp5657.curr +( yyvsp_offset - 4);
if( _temp5659 < _temp5657.base? 1: _temp5659 >= _temp5657.last_plus_one){ _throw(
Null_Exception);}* _temp5659;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5660= yyvs; struct _xtunion_struct** _temp5662= _temp5660.curr +(
yyvsp_offset - 2); if( _temp5662 < _temp5660.base? 1: _temp5662 >= _temp5660.last_plus_one){
_throw( Null_Exception);}* _temp5662;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5663= yyvs; struct _xtunion_struct** _temp5665= _temp5663.curr
+ yyvsp_offset; if( _temp5665 < _temp5663.base? 1: _temp5665 >= _temp5663.last_plus_one){
_throw( Null_Exception);}* _temp5665;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5666= yyls; struct Cyc_Yyltype* _temp5668= _temp5666.curr +(
yylsp_offset - 7); if( _temp5668 < _temp5666.base? 1: _temp5668 >= _temp5666.last_plus_one){
_throw( Null_Exception);}* _temp5668;}).first_line,({ struct _tagged_ptr3
_temp5669= yyls; struct Cyc_Yyltype* _temp5671= _temp5669.curr + yylsp_offset;
if( _temp5671 < _temp5669.base? 1: _temp5671 >= _temp5669.last_plus_one){ _throw(
Null_Exception);}* _temp5671;}).last_line)); yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5672=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5672[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5673; _temp5673.tag= Cyc_Stmt_tok_tag; _temp5673.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5673;}); _temp5672;}); break;} case 255: _LL5653: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5675=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5675[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5676; _temp5676.tag= Cyc_Stmt_tok_tag; _temp5676.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_string* _temp5677=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5677[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5678= yyvs; struct _xtunion_struct** _temp5680= _temp5678.curr
+( yyvsp_offset - 1); if( _temp5680 < _temp5678.base? 1: _temp5680 >= _temp5678.last_plus_one){
_throw( Null_Exception);}* _temp5680;})); _temp5677;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5681= yyls; struct Cyc_Yyltype* _temp5683= _temp5681.curr
+( yylsp_offset - 2); if( _temp5683 < _temp5681.base? 1: _temp5683 >= _temp5681.last_plus_one){
_throw( Null_Exception);}* _temp5683;}).first_line,({ struct _tagged_ptr3
_temp5684= yyls; struct Cyc_Yyltype* _temp5686= _temp5684.curr +( yylsp_offset -
1); if( _temp5686 < _temp5684.base? 1: _temp5686 >= _temp5684.last_plus_one){
_throw( Null_Exception);}* _temp5686;}).last_line)); _temp5676;}); _temp5675;});
break; case 256: _LL5674: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5688=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5688[ 0]=({ struct Cyc_Stmt_tok_struct _temp5689; _temp5689.tag= Cyc_Stmt_tok_tag;
_temp5689.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5690= yyls; struct Cyc_Yyltype* _temp5692= _temp5690.curr +(
yylsp_offset - 1); if( _temp5692 < _temp5690.base? 1: _temp5692 >= _temp5690.last_plus_one){
_throw( Null_Exception);}* _temp5692;}).first_line,({ struct _tagged_ptr3
_temp5693= yyls; struct Cyc_Yyltype* _temp5695= _temp5693.curr +( yylsp_offset -
1); if( _temp5695 < _temp5693.base? 1: _temp5695 >= _temp5693.last_plus_one){
_throw( Null_Exception);}* _temp5695;}).last_line)); _temp5689;}); _temp5688;});
break; case 257: _LL5687: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5697=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5697[ 0]=({ struct Cyc_Stmt_tok_struct _temp5698; _temp5698.tag= Cyc_Stmt_tok_tag;
_temp5698.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5699= yyls; struct Cyc_Yyltype* _temp5701= _temp5699.curr +(
yylsp_offset - 1); if( _temp5701 < _temp5699.base? 1: _temp5701 >= _temp5699.last_plus_one){
_throw( Null_Exception);}* _temp5701;}).first_line,({ struct _tagged_ptr3
_temp5702= yyls; struct Cyc_Yyltype* _temp5704= _temp5702.curr +( yylsp_offset -
1); if( _temp5704 < _temp5702.base? 1: _temp5704 >= _temp5702.last_plus_one){
_throw( Null_Exception);}* _temp5704;}).last_line)); _temp5698;}); _temp5697;});
break; case 258: _LL5696: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5706=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5706[ 0]=({ struct Cyc_Stmt_tok_struct _temp5707; _temp5707.tag= Cyc_Stmt_tok_tag;
_temp5707.f1= Cyc_Absyn_return_stmt( 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5708= yyls; struct Cyc_Yyltype* _temp5710= _temp5708.curr +(
yylsp_offset - 1); if( _temp5710 < _temp5708.base? 1: _temp5710 >= _temp5708.last_plus_one){
_throw( Null_Exception);}* _temp5710;}).first_line,({ struct _tagged_ptr3
_temp5711= yyls; struct Cyc_Yyltype* _temp5713= _temp5711.curr +( yylsp_offset -
1); if( _temp5713 < _temp5711.base? 1: _temp5713 >= _temp5711.last_plus_one){
_throw( Null_Exception);}* _temp5713;}).last_line)); _temp5707;}); _temp5706;});
break; case 259: _LL5705: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5715=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5715[ 0]=({ struct Cyc_Stmt_tok_struct _temp5716; _temp5716.tag= Cyc_Stmt_tok_tag;
_temp5716.f1= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5717= yyvs; struct _xtunion_struct** _temp5719=
_temp5717.curr +( yyvsp_offset - 1); if( _temp5719 < _temp5717.base? 1:
_temp5719 >= _temp5717.last_plus_one){ _throw( Null_Exception);}* _temp5719;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5720= yyls; struct Cyc_Yyltype*
_temp5722= _temp5720.curr +( yylsp_offset - 2); if( _temp5722 < _temp5720.base?
1: _temp5722 >= _temp5720.last_plus_one){ _throw( Null_Exception);}* _temp5722;}).first_line,({
struct _tagged_ptr3 _temp5723= yyls; struct Cyc_Yyltype* _temp5725= _temp5723.curr
+( yylsp_offset - 1); if( _temp5725 < _temp5723.base? 1: _temp5725 >= _temp5723.last_plus_one){
_throw( Null_Exception);}* _temp5725;}).last_line)); _temp5716;}); _temp5715;});
break; case 260: _LL5714: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5727=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5727[ 0]=({ struct Cyc_Stmt_tok_struct _temp5728; _temp5728.tag= Cyc_Stmt_tok_tag;
_temp5728.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5729= yyls; struct Cyc_Yyltype* _temp5731= _temp5729.curr +(
yylsp_offset - 1); if( _temp5731 < _temp5729.base? 1: _temp5731 >= _temp5729.last_plus_one){
_throw( Null_Exception);}* _temp5731;}).first_line,({ struct _tagged_ptr3
_temp5732= yyls; struct Cyc_Yyltype* _temp5734= _temp5732.curr +( yylsp_offset -
1); if( _temp5734 < _temp5732.base? 1: _temp5734 >= _temp5732.last_plus_one){
_throw( Null_Exception);}* _temp5734;}).last_line)); _temp5728;}); _temp5727;});
break; case 261: _LL5726: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5736=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5736[ 0]=({ struct Cyc_Stmt_tok_struct _temp5737; _temp5737.tag= Cyc_Stmt_tok_tag;
_temp5737.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5738= yyls; struct Cyc_Yyltype* _temp5740= _temp5738.curr +(
yylsp_offset - 3); if( _temp5740 < _temp5738.base? 1: _temp5740 >= _temp5738.last_plus_one){
_throw( Null_Exception);}* _temp5740;}).first_line,({ struct _tagged_ptr3
_temp5741= yyls; struct Cyc_Yyltype* _temp5743= _temp5741.curr +( yylsp_offset -
3); if( _temp5743 < _temp5741.base? 1: _temp5743 >= _temp5741.last_plus_one){
_throw( Null_Exception);}* _temp5743;}).last_line)); _temp5737;}); _temp5736;});
break; case 262: _LL5735: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5745=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5745[ 0]=({ struct Cyc_Stmt_tok_struct _temp5746; _temp5746.tag= Cyc_Stmt_tok_tag;
_temp5746.f1= Cyc_Absyn_fallthru_stmt( Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp5747= yyvs; struct _xtunion_struct** _temp5749= _temp5747.curr
+( yyvsp_offset - 2); if( _temp5749 < _temp5747.base? 1: _temp5749 >= _temp5747.last_plus_one){
_throw( Null_Exception);}* _temp5749;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5750= yyls; struct Cyc_Yyltype* _temp5752= _temp5750.curr +(
yylsp_offset - 4); if( _temp5752 < _temp5750.base? 1: _temp5752 >= _temp5750.last_plus_one){
_throw( Null_Exception);}* _temp5752;}).first_line,({ struct _tagged_ptr3
_temp5753= yyls; struct Cyc_Yyltype* _temp5755= _temp5753.curr +( yylsp_offset -
4); if( _temp5755 < _temp5753.base? 1: _temp5755 >= _temp5753.last_plus_one){
_throw( Null_Exception);}* _temp5755;}).last_line)); _temp5746;}); _temp5745;});
break; case 263: _LL5744: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5757=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5757[ 0]=({ struct Cyc_Pattern_tok_struct _temp5758; _temp5758.tag= Cyc_Pattern_tok_tag;
_temp5758.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5759= yyls; struct Cyc_Yyltype* _temp5761= _temp5759.curr
+ yylsp_offset; if( _temp5761 < _temp5759.base? 1: _temp5761 >= _temp5759.last_plus_one){
_throw( Null_Exception);}* _temp5761;}).first_line,({ struct _tagged_ptr3
_temp5762= yyls; struct Cyc_Yyltype* _temp5764= _temp5762.curr + yylsp_offset;
if( _temp5764 < _temp5762.base? 1: _temp5764 >= _temp5762.last_plus_one){ _throw(
Null_Exception);}* _temp5764;}).last_line)); _temp5758;}); _temp5757;}); break;
case 264: _LL5756: yyval=({ struct _tagged_ptr2 _temp5766= yyvs; struct
_xtunion_struct** _temp5768= _temp5766.curr +( yyvsp_offset - 1); if( _temp5768
< _temp5766.base? 1: _temp5768 >= _temp5766.last_plus_one){ _throw(
Null_Exception);}* _temp5768;}); break; case 265: _LL5765: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5770=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5770[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5771; _temp5771.tag= Cyc_Pattern_tok_tag; _temp5771.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp5772=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5772[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp5773; _temp5773.tag= Cyc_Absyn_Int_p_tag; _temp5773.f1=( void*)(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5774= yyvs; struct _xtunion_struct** _temp5776=
_temp5774.curr + yyvsp_offset; if( _temp5776 < _temp5774.base? 1: _temp5776 >=
_temp5774.last_plus_one){ _throw( Null_Exception);}* _temp5776;}))).f1;
_temp5773.f2=(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5777= yyvs; struct
_xtunion_struct** _temp5779= _temp5777.curr + yyvsp_offset; if( _temp5779 <
_temp5777.base? 1: _temp5779 >= _temp5777.last_plus_one){ _throw( Null_Exception);}*
_temp5779;}))).f2; _temp5773;}); _temp5772;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5780= yyls; struct Cyc_Yyltype* _temp5782= _temp5780.curr
+ yylsp_offset; if( _temp5782 < _temp5780.base? 1: _temp5782 >= _temp5780.last_plus_one){
_throw( Null_Exception);}* _temp5782;}).first_line,({ struct _tagged_ptr3
_temp5783= yyls; struct Cyc_Yyltype* _temp5785= _temp5783.curr + yylsp_offset;
if( _temp5785 < _temp5783.base? 1: _temp5785 >= _temp5783.last_plus_one){ _throw(
Null_Exception);}* _temp5785;}).last_line)); _temp5771;}); _temp5770;}); break;
case 266: _LL5769: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5787=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5787[ 0]=({ struct Cyc_Pattern_tok_struct _temp5788; _temp5788.tag= Cyc_Pattern_tok_tag;
_temp5788.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp5789=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp5789[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp5790; _temp5790.tag= Cyc_Absyn_Int_p_tag;
_temp5790.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp5790.f2= -(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5791= yyvs; struct _xtunion_struct** _temp5793=
_temp5791.curr + yyvsp_offset; if( _temp5793 < _temp5791.base? 1: _temp5793 >=
_temp5791.last_plus_one){ _throw( Null_Exception);}* _temp5793;}))).f2;
_temp5790;}); _temp5789;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5794= yyls; struct Cyc_Yyltype* _temp5796= _temp5794.curr +( yylsp_offset -
1); if( _temp5796 < _temp5794.base? 1: _temp5796 >= _temp5794.last_plus_one){
_throw( Null_Exception);}* _temp5796;}).first_line,({ struct _tagged_ptr3
_temp5797= yyls; struct Cyc_Yyltype* _temp5799= _temp5797.curr + yylsp_offset;
if( _temp5799 < _temp5797.base? 1: _temp5799 >= _temp5797.last_plus_one){ _throw(
Null_Exception);}* _temp5799;}).last_line)); _temp5788;}); _temp5787;}); break;
case 267: _LL5786: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5801=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5801[ 0]=({ struct Cyc_Pattern_tok_struct _temp5802; _temp5802.tag= Cyc_Pattern_tok_tag;
_temp5802.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Float_p_struct*
_temp5803=( struct Cyc_Absyn_Float_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct));
_temp5803[ 0]=({ struct Cyc_Absyn_Float_p_struct _temp5804; _temp5804.tag= Cyc_Absyn_Float_p_tag;
_temp5804.f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5805= yyvs;
struct _xtunion_struct** _temp5807= _temp5805.curr + yyvsp_offset; if( _temp5807
< _temp5805.base? 1: _temp5807 >= _temp5805.last_plus_one){ _throw(
Null_Exception);}* _temp5807;})); _temp5804;}); _temp5803;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5808= yyls; struct Cyc_Yyltype* _temp5810= _temp5808.curr
+ yylsp_offset; if( _temp5810 < _temp5808.base? 1: _temp5810 >= _temp5808.last_plus_one){
_throw( Null_Exception);}* _temp5810;}).first_line,({ struct _tagged_ptr3
_temp5811= yyls; struct Cyc_Yyltype* _temp5813= _temp5811.curr + yylsp_offset;
if( _temp5813 < _temp5811.base? 1: _temp5813 >= _temp5811.last_plus_one){ _throw(
Null_Exception);}* _temp5813;}).last_line)); _temp5802;}); _temp5801;}); break;
case 268: _LL5800: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5815=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5815[ 0]=({ struct Cyc_Pattern_tok_struct _temp5816; _temp5816.tag= Cyc_Pattern_tok_tag;
_temp5816.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp5817=( struct Cyc_Absyn_Char_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct));
_temp5817[ 0]=({ struct Cyc_Absyn_Char_p_struct _temp5818; _temp5818.tag= Cyc_Absyn_Char_p_tag;
_temp5818.f1= Cyc_yyget_Char_tok(({ struct _tagged_ptr2 _temp5819= yyvs; struct
_xtunion_struct** _temp5821= _temp5819.curr + yyvsp_offset; if( _temp5821 <
_temp5819.base? 1: _temp5821 >= _temp5819.last_plus_one){ _throw( Null_Exception);}*
_temp5821;})); _temp5818;}); _temp5817;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5822= yyls; struct Cyc_Yyltype* _temp5824= _temp5822.curr +
yylsp_offset; if( _temp5824 < _temp5822.base? 1: _temp5824 >= _temp5822.last_plus_one){
_throw( Null_Exception);}* _temp5824;}).first_line,({ struct _tagged_ptr3
_temp5825= yyls; struct Cyc_Yyltype* _temp5827= _temp5825.curr + yylsp_offset;
if( _temp5827 < _temp5825.base? 1: _temp5827 >= _temp5825.last_plus_one){ _throw(
Null_Exception);}* _temp5827;}).last_line)); _temp5816;}); _temp5815;}); break;
case 269: _LL5814: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5829=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5829[ 0]=({ struct Cyc_Pattern_tok_struct _temp5830; _temp5830.tag= Cyc_Pattern_tok_tag;
_temp5830.f1= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5831= yyls; struct Cyc_Yyltype* _temp5833= _temp5831.curr
+ yylsp_offset; if( _temp5833 < _temp5831.base? 1: _temp5833 >= _temp5831.last_plus_one){
_throw( Null_Exception);}* _temp5833;}).first_line,({ struct _tagged_ptr3
_temp5834= yyls; struct Cyc_Yyltype* _temp5836= _temp5834.curr + yylsp_offset;
if( _temp5836 < _temp5834.base? 1: _temp5836 >= _temp5834.last_plus_one){ _throw(
Null_Exception);}* _temp5836;}).last_line)); _temp5830;}); _temp5829;}); break;
case 270: _LL5828: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5838=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5838[ 0]=({ struct Cyc_Pattern_tok_struct _temp5839; _temp5839.tag= Cyc_Pattern_tok_tag;
_temp5839.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp5840=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp5840[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp5841; _temp5841.tag=
Cyc_Absyn_UnknownId_p_tag; _temp5841.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5842= yyvs; struct _xtunion_struct** _temp5844= _temp5842.curr
+ yyvsp_offset; if( _temp5844 < _temp5842.base? 1: _temp5844 >= _temp5842.last_plus_one){
_throw( Null_Exception);}* _temp5844;})); _temp5841;}); _temp5840;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5845= yyls; struct Cyc_Yyltype* _temp5847= _temp5845.curr
+ yylsp_offset; if( _temp5847 < _temp5845.base? 1: _temp5847 >= _temp5845.last_plus_one){
_throw( Null_Exception);}* _temp5847;}).first_line,({ struct _tagged_ptr3
_temp5848= yyls; struct Cyc_Yyltype* _temp5850= _temp5848.curr + yylsp_offset;
if( _temp5850 < _temp5848.base? 1: _temp5850 >= _temp5848.last_plus_one){ _throw(
Null_Exception);}* _temp5850;}).last_line)); _temp5839;}); _temp5838;}); break;
case 271: _LL5837: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5852= yyls; struct Cyc_Yyltype* _temp5854= _temp5852.curr
+( yylsp_offset - 3); if( _temp5854 < _temp5852.base? 1: _temp5854 >= _temp5852.last_plus_one){
_throw( Null_Exception);}* _temp5854;}).first_line,({ struct _tagged_ptr3
_temp5855= yyls; struct Cyc_Yyltype* _temp5857= _temp5855.curr +( yylsp_offset -
3); if( _temp5857 < _temp5855.base? 1: _temp5857 >= _temp5855.last_plus_one){
_throw( Null_Exception);}* _temp5857;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5858= yyvs; struct _xtunion_struct** _temp5860=
_temp5858.curr +( yyvsp_offset - 3); if( _temp5860 < _temp5858.base? 1:
_temp5860 >= _temp5858.last_plus_one){ _throw( Null_Exception);}* _temp5860;})));
yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5861=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5861[ 0]=({ struct Cyc_Pattern_tok_struct _temp5862; _temp5862.tag= Cyc_Pattern_tok_tag;
_temp5862.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp5863=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp5863[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp5864; _temp5864.tag= Cyc_Absyn_UnknownCall_p_tag; _temp5864.f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp5865= yyvs; struct _xtunion_struct** _temp5867=
_temp5865.curr +( yyvsp_offset - 4); if( _temp5867 < _temp5865.base? 1:
_temp5867 >= _temp5865.last_plus_one){ _throw( Null_Exception);}* _temp5867;}));
_temp5864.f2= tvs; _temp5864.f3= Cyc_yyget_PatternList_tok(({ struct
_tagged_ptr2 _temp5868= yyvs; struct _xtunion_struct** _temp5870= _temp5868.curr
+( yyvsp_offset - 1); if( _temp5870 < _temp5868.base? 1: _temp5870 >= _temp5868.last_plus_one){
_throw( Null_Exception);}* _temp5870;})); _temp5864;}); _temp5863;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5871= yyls; struct Cyc_Yyltype* _temp5873= _temp5871.curr
+( yylsp_offset - 4); if( _temp5873 < _temp5871.base? 1: _temp5873 >= _temp5871.last_plus_one){
_throw( Null_Exception);}* _temp5873;}).first_line,({ struct _tagged_ptr3
_temp5874= yyls; struct Cyc_Yyltype* _temp5876= _temp5874.curr + yylsp_offset;
if( _temp5876 < _temp5874.base? 1: _temp5876 >= _temp5874.last_plus_one){ _throw(
Null_Exception);}* _temp5876;}).last_line)); _temp5862;}); _temp5861;}); break;}
case 272: _LL5851: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5878=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5878[ 0]=({ struct Cyc_Pattern_tok_struct _temp5879; _temp5879.tag= Cyc_Pattern_tok_tag;
_temp5879.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Tuple_p_struct*
_temp5880=( struct Cyc_Absyn_Tuple_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct));
_temp5880[ 0]=({ struct Cyc_Absyn_Tuple_p_struct _temp5881; _temp5881.tag= Cyc_Absyn_Tuple_p_tag;
_temp5881.f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5882= yyvs;
struct _xtunion_struct** _temp5884= _temp5882.curr +( yyvsp_offset - 1); if(
_temp5884 < _temp5882.base? 1: _temp5884 >= _temp5882.last_plus_one){ _throw(
Null_Exception);}* _temp5884;})); _temp5881;}); _temp5880;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5885= yyls; struct Cyc_Yyltype* _temp5887= _temp5885.curr
+( yylsp_offset - 3); if( _temp5887 < _temp5885.base? 1: _temp5887 >= _temp5885.last_plus_one){
_throw( Null_Exception);}* _temp5887;}).first_line,({ struct _tagged_ptr3
_temp5888= yyls; struct Cyc_Yyltype* _temp5890= _temp5888.curr + yylsp_offset;
if( _temp5890 < _temp5888.base? 1: _temp5890 >= _temp5888.last_plus_one){ _throw(
Null_Exception);}* _temp5890;}).last_line)); _temp5879;}); _temp5878;}); break;
case 273: _LL5877: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5892= yyls; struct Cyc_Yyltype* _temp5894= _temp5892.curr
+( yylsp_offset - 2); if( _temp5894 < _temp5892.base? 1: _temp5894 >= _temp5892.last_plus_one){
_throw( Null_Exception);}* _temp5894;}).first_line,({ struct _tagged_ptr3
_temp5895= yyls; struct Cyc_Yyltype* _temp5897= _temp5895.curr +( yylsp_offset -
2); if( _temp5897 < _temp5895.base? 1: _temp5897 >= _temp5895.last_plus_one){
_throw( Null_Exception);}* _temp5897;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5898= yyvs; struct _xtunion_struct** _temp5900=
_temp5898.curr +( yyvsp_offset - 2); if( _temp5900 < _temp5898.base? 1:
_temp5900 >= _temp5898.last_plus_one){ _throw( Null_Exception);}* _temp5900;})));
yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5901=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5901[ 0]=({ struct Cyc_Pattern_tok_struct _temp5902; _temp5902.tag= Cyc_Pattern_tok_tag;
_temp5902.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp5903=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp5903[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp5904; _temp5904.tag= Cyc_Absyn_UnknownFields_p_tag; _temp5904.f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp5905= yyvs; struct _xtunion_struct** _temp5907=
_temp5905.curr +( yyvsp_offset - 3); if( _temp5907 < _temp5905.base? 1:
_temp5907 >= _temp5905.last_plus_one){ _throw( Null_Exception);}* _temp5907;}));
_temp5904.f2= tvs; _temp5904.f3= 0; _temp5904;}); _temp5903;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5908= yyls; struct Cyc_Yyltype* _temp5910= _temp5908.curr
+( yylsp_offset - 3); if( _temp5910 < _temp5908.base? 1: _temp5910 >= _temp5908.last_plus_one){
_throw( Null_Exception);}* _temp5910;}).first_line,({ struct _tagged_ptr3
_temp5911= yyls; struct Cyc_Yyltype* _temp5913= _temp5911.curr + yylsp_offset;
if( _temp5913 < _temp5911.base? 1: _temp5913 >= _temp5911.last_plus_one){ _throw(
Null_Exception);}* _temp5913;}).last_line)); _temp5902;}); _temp5901;}); break;}
case 274: _LL5891: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5915= yyls; struct Cyc_Yyltype* _temp5917= _temp5915.curr
+( yylsp_offset - 3); if( _temp5917 < _temp5915.base? 1: _temp5917 >= _temp5915.last_plus_one){
_throw( Null_Exception);}* _temp5917;}).first_line,({ struct _tagged_ptr3
_temp5918= yyls; struct Cyc_Yyltype* _temp5920= _temp5918.curr +( yylsp_offset -
3); if( _temp5920 < _temp5918.base? 1: _temp5920 >= _temp5918.last_plus_one){
_throw( Null_Exception);}* _temp5920;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5921= yyvs; struct _xtunion_struct** _temp5923=
_temp5921.curr +( yyvsp_offset - 3); if( _temp5923 < _temp5921.base? 1:
_temp5923 >= _temp5921.last_plus_one){ _throw( Null_Exception);}* _temp5923;})));
yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5924=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5924[ 0]=({ struct Cyc_Pattern_tok_struct _temp5925; _temp5925.tag= Cyc_Pattern_tok_tag;
_temp5925.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp5926=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp5926[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp5927; _temp5927.tag= Cyc_Absyn_UnknownFields_p_tag; _temp5927.f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp5928= yyvs; struct _xtunion_struct** _temp5930=
_temp5928.curr +( yyvsp_offset - 4); if( _temp5930 < _temp5928.base? 1:
_temp5930 >= _temp5928.last_plus_one){ _throw( Null_Exception);}* _temp5930;}));
_temp5927.f2= tvs; _temp5927.f3= Cyc_yyget_FieldPatternList_tok(({ struct
_tagged_ptr2 _temp5931= yyvs; struct _xtunion_struct** _temp5933= _temp5931.curr
+( yyvsp_offset - 1); if( _temp5933 < _temp5931.base? 1: _temp5933 >= _temp5931.last_plus_one){
_throw( Null_Exception);}* _temp5933;})); _temp5927;}); _temp5926;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5934= yyls; struct Cyc_Yyltype* _temp5936= _temp5934.curr
+( yylsp_offset - 4); if( _temp5936 < _temp5934.base? 1: _temp5936 >= _temp5934.last_plus_one){
_throw( Null_Exception);}* _temp5936;}).first_line,({ struct _tagged_ptr3
_temp5937= yyls; struct Cyc_Yyltype* _temp5939= _temp5937.curr + yylsp_offset;
if( _temp5939 < _temp5937.base? 1: _temp5939 >= _temp5937.last_plus_one){ _throw(
Null_Exception);}* _temp5939;}).last_line)); _temp5925;}); _temp5924;}); break;}
case 275: _LL5914: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5941=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5941[ 0]=({ struct Cyc_Pattern_tok_struct _temp5942; _temp5942.tag= Cyc_Pattern_tok_tag;
_temp5942.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Pointer_p_struct*
_temp5943=( struct Cyc_Absyn_Pointer_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct));
_temp5943[ 0]=({ struct Cyc_Absyn_Pointer_p_struct _temp5944; _temp5944.tag= Cyc_Absyn_Pointer_p_tag;
_temp5944.f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5945= yyvs;
struct _xtunion_struct** _temp5947= _temp5945.curr + yyvsp_offset; if( _temp5947
< _temp5945.base? 1: _temp5947 >= _temp5945.last_plus_one){ _throw(
Null_Exception);}* _temp5947;})); _temp5944;}); _temp5943;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5948= yyls; struct Cyc_Yyltype* _temp5950= _temp5948.curr
+( yylsp_offset - 1); if( _temp5950 < _temp5948.base? 1: _temp5950 >= _temp5948.last_plus_one){
_throw( Null_Exception);}* _temp5950;}).first_line,({ struct _tagged_ptr3
_temp5951= yyls; struct Cyc_Yyltype* _temp5953= _temp5951.curr + yylsp_offset;
if( _temp5953 < _temp5951.base? 1: _temp5953 >= _temp5951.last_plus_one){ _throw(
Null_Exception);}* _temp5953;}).last_line)); _temp5942;}); _temp5941;}); break;
case 276: _LL5940: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5955=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5955[ 0]=({ struct Cyc_Pattern_tok_struct _temp5956; _temp5956.tag= Cyc_Pattern_tok_tag;
_temp5956.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Reference_p_struct*
_temp5957=( struct Cyc_Absyn_Reference_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct));
_temp5957[ 0]=({ struct Cyc_Absyn_Reference_p_struct _temp5958; _temp5958.tag=
Cyc_Absyn_Reference_p_tag; _temp5958.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5959=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5959->f1=(
void*) Cyc_Absyn_Loc_n; _temp5959->f2=({ struct _tagged_string* _temp5960=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp5960[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5961= yyvs;
struct _xtunion_struct** _temp5963= _temp5961.curr + yyvsp_offset; if( _temp5963
< _temp5961.base? 1: _temp5963 >= _temp5961.last_plus_one){ _throw(
Null_Exception);}* _temp5963;})); _temp5960;}); _temp5959;}),( void*) Cyc_Absyn_VoidType,
0); _temp5958;}); _temp5957;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5964= yyls; struct Cyc_Yyltype* _temp5966= _temp5964.curr +(
yylsp_offset - 1); if( _temp5966 < _temp5964.base? 1: _temp5966 >= _temp5964.last_plus_one){
_throw( Null_Exception);}* _temp5966;}).first_line,({ struct _tagged_ptr3
_temp5967= yyls; struct Cyc_Yyltype* _temp5969= _temp5967.curr + yylsp_offset;
if( _temp5969 < _temp5967.base? 1: _temp5969 >= _temp5967.last_plus_one){ _throw(
Null_Exception);}* _temp5969;}).last_line)); _temp5956;}); _temp5955;}); break;
case 277: _LL5954: yyval=( struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct*
_temp5971=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp5971[ 0]=({ struct Cyc_PatternList_tok_struct _temp5972; _temp5972.tag= Cyc_PatternList_tok_tag;
_temp5972.f1= 0; _temp5972;}); _temp5971;}); break; case 278: _LL5970: yyval=(
struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp5974=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp5974[ 0]=({ struct Cyc_PatternList_tok_struct _temp5975; _temp5975.tag= Cyc_PatternList_tok_tag;
_temp5975.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5976= yyvs; struct
_xtunion_struct** _temp5978= _temp5976.curr + yyvsp_offset; if( _temp5978 <
_temp5976.base? 1: _temp5978 >= _temp5976.last_plus_one){ _throw( Null_Exception);}*
_temp5978;}))); _temp5975;}); _temp5974;}); break; case 279: _LL5973: yyval=(
struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp5980=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp5980[ 0]=({ struct Cyc_PatternList_tok_struct _temp5981; _temp5981.tag= Cyc_PatternList_tok_tag;
_temp5981.f1=({ struct Cyc_List_List* _temp5982=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5982->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5983= yyvs; struct _xtunion_struct** _temp5985=
_temp5983.curr + yyvsp_offset; if( _temp5985 < _temp5983.base? 1: _temp5985 >=
_temp5983.last_plus_one){ _throw( Null_Exception);}* _temp5985;})); _temp5982->tl=
0; _temp5982;}); _temp5981;}); _temp5980;}); break; case 280: _LL5979: yyval=(
struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp5987=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp5987[ 0]=({ struct Cyc_PatternList_tok_struct _temp5988; _temp5988.tag= Cyc_PatternList_tok_tag;
_temp5988.f1=({ struct Cyc_List_List* _temp5989=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5989->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5993= yyvs; struct _xtunion_struct** _temp5995=
_temp5993.curr + yyvsp_offset; if( _temp5995 < _temp5993.base? 1: _temp5995 >=
_temp5993.last_plus_one){ _throw( Null_Exception);}* _temp5995;})); _temp5989->tl=
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5990= yyvs; struct
_xtunion_struct** _temp5992= _temp5990.curr +( yyvsp_offset - 2); if( _temp5992
< _temp5990.base? 1: _temp5992 >= _temp5990.last_plus_one){ _throw(
Null_Exception);}* _temp5992;})); _temp5989;}); _temp5988;}); _temp5987;});
break; case 281: _LL5986: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp5997=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp5997[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp5998; _temp5998.tag=
Cyc_FieldPattern_tok_tag; _temp5998.f1=({ struct _tuple17* _temp5999=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp5999->f1= 0; _temp5999->f2=
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6000= yyvs; struct
_xtunion_struct** _temp6002= _temp6000.curr + yyvsp_offset; if( _temp6002 <
_temp6000.base? 1: _temp6002 >= _temp6000.last_plus_one){ _throw( Null_Exception);}*
_temp6002;})); _temp5999;}); _temp5998;}); _temp5997;}); break; case 282:
_LL5996: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6004=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6004[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6005; _temp6005.tag=
Cyc_FieldPattern_tok_tag; _temp6005.f1=({ struct _tuple17* _temp6006=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6006->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp6010= yyvs; struct _xtunion_struct** _temp6012=
_temp6010.curr +( yyvsp_offset - 1); if( _temp6012 < _temp6010.base? 1:
_temp6012 >= _temp6010.last_plus_one){ _throw( Null_Exception);}* _temp6012;}));
_temp6006->f2= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6007= yyvs;
struct _xtunion_struct** _temp6009= _temp6007.curr + yyvsp_offset; if( _temp6009
< _temp6007.base? 1: _temp6009 >= _temp6007.last_plus_one){ _throw(
Null_Exception);}* _temp6009;})); _temp6006;}); _temp6005;}); _temp6004;});
break; case 283: _LL6003: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct*
_temp6014=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp6014[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp6015; _temp6015.tag= Cyc_FieldPatternList_tok_tag; _temp6015.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6016= yyvs; struct _xtunion_struct** _temp6018=
_temp6016.curr + yyvsp_offset; if( _temp6018 < _temp6016.base? 1: _temp6018 >=
_temp6016.last_plus_one){ _throw( Null_Exception);}* _temp6018;}))); _temp6015;});
_temp6014;}); break; case 284: _LL6013: yyval=( struct _xtunion_struct*)({
struct Cyc_FieldPatternList_tok_struct* _temp6020=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp6020[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp6021; _temp6021.tag= Cyc_FieldPatternList_tok_tag;
_temp6021.f1=({ struct Cyc_List_List* _temp6022=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6022->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp6023= yyvs; struct _xtunion_struct** _temp6025=
_temp6023.curr + yyvsp_offset; if( _temp6025 < _temp6023.base? 1: _temp6025 >=
_temp6023.last_plus_one){ _throw( Null_Exception);}* _temp6025;})); _temp6022->tl=
0; _temp6022;}); _temp6021;}); _temp6020;}); break; case 285: _LL6019: yyval=(
struct _xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6027=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6027[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6028; _temp6028.tag=
Cyc_FieldPatternList_tok_tag; _temp6028.f1=({ struct Cyc_List_List* _temp6029=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6029->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6033= yyvs; struct
_xtunion_struct** _temp6035= _temp6033.curr + yyvsp_offset; if( _temp6035 <
_temp6033.base? 1: _temp6035 >= _temp6033.last_plus_one){ _throw( Null_Exception);}*
_temp6035;})); _temp6029->tl= Cyc_yyget_FieldPatternList_tok(({ struct
_tagged_ptr2 _temp6030= yyvs; struct _xtunion_struct** _temp6032= _temp6030.curr
+( yyvsp_offset - 2); if( _temp6032 < _temp6030.base? 1: _temp6032 >= _temp6030.last_plus_one){
_throw( Null_Exception);}* _temp6032;})); _temp6029;}); _temp6028;}); _temp6027;});
break; case 286: _LL6026: yyval=({ struct _tagged_ptr2 _temp6037= yyvs; struct
_xtunion_struct** _temp6039= _temp6037.curr + yyvsp_offset; if( _temp6039 <
_temp6037.base? 1: _temp6039 >= _temp6037.last_plus_one){ _throw( Null_Exception);}*
_temp6039;}); break; case 287: _LL6036: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6041=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6041[ 0]=({ struct Cyc_Exp_tok_struct
_temp6042; _temp6042.tag= Cyc_Exp_tok_tag; _temp6042.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6043= yyvs; struct _xtunion_struct** _temp6045=
_temp6043.curr +( yyvsp_offset - 2); if( _temp6045 < _temp6043.base? 1:
_temp6045 >= _temp6043.last_plus_one){ _throw( Null_Exception);}* _temp6045;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6046= yyvs; struct _xtunion_struct**
_temp6048= _temp6046.curr + yyvsp_offset; if( _temp6048 < _temp6046.base? 1:
_temp6048 >= _temp6046.last_plus_one){ _throw( Null_Exception);}* _temp6048;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6049= yyls; struct Cyc_Yyltype*
_temp6051= _temp6049.curr +( yylsp_offset - 2); if( _temp6051 < _temp6049.base?
1: _temp6051 >= _temp6049.last_plus_one){ _throw( Null_Exception);}* _temp6051;}).first_line,({
struct _tagged_ptr3 _temp6052= yyls; struct Cyc_Yyltype* _temp6054= _temp6052.curr
+ yylsp_offset; if( _temp6054 < _temp6052.base? 1: _temp6054 >= _temp6052.last_plus_one){
_throw( Null_Exception);}* _temp6054;}).last_line)); _temp6042;}); _temp6041;});
break; case 288: _LL6040: yyval=({ struct _tagged_ptr2 _temp6056= yyvs; struct
_xtunion_struct** _temp6058= _temp6056.curr + yyvsp_offset; if( _temp6058 <
_temp6056.base? 1: _temp6058 >= _temp6056.last_plus_one){ _throw( Null_Exception);}*
_temp6058;}); break; case 289: _LL6055: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6060=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6060[ 0]=({ struct Cyc_Exp_tok_struct
_temp6061; _temp6061.tag= Cyc_Exp_tok_tag; _temp6061.f1= Cyc_Absyn_assignop_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6062= yyvs; struct _xtunion_struct**
_temp6064= _temp6062.curr +( yyvsp_offset - 2); if( _temp6064 < _temp6062.base?
1: _temp6064 >= _temp6062.last_plus_one){ _throw( Null_Exception);}* _temp6064;})),
Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2 _temp6065= yyvs; struct
_xtunion_struct** _temp6067= _temp6065.curr +( yyvsp_offset - 1); if( _temp6067
< _temp6065.base? 1: _temp6067 >= _temp6065.last_plus_one){ _throw(
Null_Exception);}* _temp6067;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6068= yyvs; struct _xtunion_struct** _temp6070= _temp6068.curr +
yyvsp_offset; if( _temp6070 < _temp6068.base? 1: _temp6070 >= _temp6068.last_plus_one){
_throw( Null_Exception);}* _temp6070;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6071= yyls; struct Cyc_Yyltype* _temp6073= _temp6071.curr +(
yylsp_offset - 2); if( _temp6073 < _temp6071.base? 1: _temp6073 >= _temp6071.last_plus_one){
_throw( Null_Exception);}* _temp6073;}).first_line,({ struct _tagged_ptr3
_temp6074= yyls; struct Cyc_Yyltype* _temp6076= _temp6074.curr + yylsp_offset;
if( _temp6076 < _temp6074.base? 1: _temp6076 >= _temp6074.last_plus_one){ _throw(
Null_Exception);}* _temp6076;}).last_line)); _temp6061;}); _temp6060;}); break;
case 290: _LL6059: yyval=( struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct*
_temp6078=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6078[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6079; _temp6079.tag= Cyc_Primopopt_tok_tag;
_temp6079.f1= 0; _temp6079;}); _temp6078;}); break; case 291: _LL6077: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6081=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6081[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6082; _temp6082.tag= Cyc_Primopopt_tok_tag;
_temp6082.f1=({ struct Cyc_Core_Opt* _temp6083=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6083->v=( void*)(( void*) Cyc_Absyn_Times);
_temp6083;}); _temp6082;}); _temp6081;}); break; case 292: _LL6080: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6085=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6085[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6086; _temp6086.tag= Cyc_Primopopt_tok_tag;
_temp6086.f1=({ struct Cyc_Core_Opt* _temp6087=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6087->v=( void*)(( void*) Cyc_Absyn_Div);
_temp6087;}); _temp6086;}); _temp6085;}); break; case 293: _LL6084: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6089=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6089[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6090; _temp6090.tag= Cyc_Primopopt_tok_tag;
_temp6090.f1=({ struct Cyc_Core_Opt* _temp6091=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6091->v=( void*)(( void*) Cyc_Absyn_Mod);
_temp6091;}); _temp6090;}); _temp6089;}); break; case 294: _LL6088: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6093=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6093[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6094; _temp6094.tag= Cyc_Primopopt_tok_tag;
_temp6094.f1=({ struct Cyc_Core_Opt* _temp6095=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6095->v=( void*)(( void*) Cyc_Absyn_Plus);
_temp6095;}); _temp6094;}); _temp6093;}); break; case 295: _LL6092: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6097=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6097[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6098; _temp6098.tag= Cyc_Primopopt_tok_tag;
_temp6098.f1=({ struct Cyc_Core_Opt* _temp6099=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6099->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp6099;}); _temp6098;}); _temp6097;}); break; case 296: _LL6096: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6101=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6101[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6102; _temp6102.tag= Cyc_Primopopt_tok_tag;
_temp6102.f1=({ struct Cyc_Core_Opt* _temp6103=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6103->v=( void*)(( void*) Cyc_Absyn_Bitlshift);
_temp6103;}); _temp6102;}); _temp6101;}); break; case 297: _LL6100: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6105=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6105[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6106; _temp6106.tag= Cyc_Primopopt_tok_tag;
_temp6106.f1=({ struct Cyc_Core_Opt* _temp6107=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6107->v=( void*)(( void*) Cyc_Absyn_Bitlrshift);
_temp6107;}); _temp6106;}); _temp6105;}); break; case 298: _LL6104: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6109=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6109[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6110; _temp6110.tag= Cyc_Primopopt_tok_tag;
_temp6110.f1=({ struct Cyc_Core_Opt* _temp6111=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6111->v=( void*)(( void*) Cyc_Absyn_Bitand);
_temp6111;}); _temp6110;}); _temp6109;}); break; case 299: _LL6108: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6113=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6113[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6114; _temp6114.tag= Cyc_Primopopt_tok_tag;
_temp6114.f1=({ struct Cyc_Core_Opt* _temp6115=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6115->v=( void*)(( void*) Cyc_Absyn_Bitxor);
_temp6115;}); _temp6114;}); _temp6113;}); break; case 300: _LL6112: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6117=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6117[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6118; _temp6118.tag= Cyc_Primopopt_tok_tag;
_temp6118.f1=({ struct Cyc_Core_Opt* _temp6119=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6119->v=( void*)(( void*) Cyc_Absyn_Bitor);
_temp6119;}); _temp6118;}); _temp6117;}); break; case 301: _LL6116: yyval=({
struct _tagged_ptr2 _temp6121= yyvs; struct _xtunion_struct** _temp6123=
_temp6121.curr + yyvsp_offset; if( _temp6123 < _temp6121.base? 1: _temp6123 >=
_temp6121.last_plus_one){ _throw( Null_Exception);}* _temp6123;}); break; case
302: _LL6120: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6125=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6125[ 0]=({ struct Cyc_Exp_tok_struct _temp6126; _temp6126.tag= Cyc_Exp_tok_tag;
_temp6126.f1= Cyc_Absyn_conditional_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6127= yyvs; struct _xtunion_struct** _temp6129= _temp6127.curr
+( yyvsp_offset - 4); if( _temp6129 < _temp6127.base? 1: _temp6129 >= _temp6127.last_plus_one){
_throw( Null_Exception);}* _temp6129;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6130= yyvs; struct _xtunion_struct** _temp6132= _temp6130.curr
+( yyvsp_offset - 2); if( _temp6132 < _temp6130.base? 1: _temp6132 >= _temp6130.last_plus_one){
_throw( Null_Exception);}* _temp6132;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6133= yyvs; struct _xtunion_struct** _temp6135= _temp6133.curr
+ yyvsp_offset; if( _temp6135 < _temp6133.base? 1: _temp6135 >= _temp6133.last_plus_one){
_throw( Null_Exception);}* _temp6135;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6136= yyls; struct Cyc_Yyltype* _temp6138= _temp6136.curr +(
yylsp_offset - 4); if( _temp6138 < _temp6136.base? 1: _temp6138 >= _temp6136.last_plus_one){
_throw( Null_Exception);}* _temp6138;}).first_line,({ struct _tagged_ptr3
_temp6139= yyls; struct Cyc_Yyltype* _temp6141= _temp6139.curr + yylsp_offset;
if( _temp6141 < _temp6139.base? 1: _temp6141 >= _temp6139.last_plus_one){ _throw(
Null_Exception);}* _temp6141;}).last_line)); _temp6126;}); _temp6125;}); break;
case 303: _LL6124: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6143=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6143[ 0]=({ struct Cyc_Exp_tok_struct _temp6144; _temp6144.tag= Cyc_Exp_tok_tag;
_temp6144.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6145= yyvs; struct _xtunion_struct** _temp6147= _temp6145.curr +
yyvsp_offset; if( _temp6147 < _temp6145.base? 1: _temp6147 >= _temp6145.last_plus_one){
_throw( Null_Exception);}* _temp6147;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6148= yyls; struct Cyc_Yyltype* _temp6150= _temp6148.curr +(
yylsp_offset - 1); if( _temp6150 < _temp6148.base? 1: _temp6150 >= _temp6148.last_plus_one){
_throw( Null_Exception);}* _temp6150;}).first_line,({ struct _tagged_ptr3
_temp6151= yyls; struct Cyc_Yyltype* _temp6153= _temp6151.curr + yylsp_offset;
if( _temp6153 < _temp6151.base? 1: _temp6153 >= _temp6151.last_plus_one){ _throw(
Null_Exception);}* _temp6153;}).last_line)); _temp6144;}); _temp6143;}); break;
case 304: _LL6142: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6155=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6155[ 0]=({ struct Cyc_Exp_tok_struct _temp6156; _temp6156.tag= Cyc_Exp_tok_tag;
_temp6156.f1= Cyc_Absyn_New_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6157= yyvs; struct _xtunion_struct** _temp6159= _temp6157.curr +
yyvsp_offset; if( _temp6159 < _temp6157.base? 1: _temp6159 >= _temp6157.last_plus_one){
_throw( Null_Exception);}* _temp6159;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6160= yyls; struct Cyc_Yyltype* _temp6162= _temp6160.curr +(
yylsp_offset - 1); if( _temp6162 < _temp6160.base? 1: _temp6162 >= _temp6160.last_plus_one){
_throw( Null_Exception);}* _temp6162;}).first_line,({ struct _tagged_ptr3
_temp6163= yyls; struct Cyc_Yyltype* _temp6165= _temp6163.curr +( yylsp_offset +
1); if( _temp6165 < _temp6163.base? 1: _temp6165 >= _temp6163.last_plus_one){
_throw( Null_Exception);}* _temp6165;}).last_line)); _temp6156;}); _temp6155;});
break; case 305: _LL6154: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6167=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6167[ 0]=({ struct Cyc_Exp_tok_struct _temp6168; _temp6168.tag= Cyc_Exp_tok_tag;
_temp6168.f1= Cyc_Absyn_New_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6169= yyvs; struct _xtunion_struct** _temp6171= _temp6169.curr +
yyvsp_offset; if( _temp6171 < _temp6169.base? 1: _temp6171 >= _temp6169.last_plus_one){
_throw( Null_Exception);}* _temp6171;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6172= yyls; struct Cyc_Yyltype* _temp6174= _temp6172.curr +(
yylsp_offset - 1); if( _temp6174 < _temp6172.base? 1: _temp6174 >= _temp6172.last_plus_one){
_throw( Null_Exception);}* _temp6174;}).first_line,({ struct _tagged_ptr3
_temp6175= yyls; struct Cyc_Yyltype* _temp6177= _temp6175.curr +( yylsp_offset +
1); if( _temp6177 < _temp6175.base? 1: _temp6177 >= _temp6175.last_plus_one){
_throw( Null_Exception);}* _temp6177;}).last_line)); _temp6168;}); _temp6167;});
break; case 306: _LL6166: yyval=({ struct _tagged_ptr2 _temp6179= yyvs; struct
_xtunion_struct** _temp6181= _temp6179.curr + yyvsp_offset; if( _temp6181 <
_temp6179.base? 1: _temp6181 >= _temp6179.last_plus_one){ _throw( Null_Exception);}*
_temp6181;}); break; case 307: _LL6178: yyval=({ struct _tagged_ptr2 _temp6183=
yyvs; struct _xtunion_struct** _temp6185= _temp6183.curr + yyvsp_offset; if(
_temp6185 < _temp6183.base? 1: _temp6185 >= _temp6183.last_plus_one){ _throw(
Null_Exception);}* _temp6185;}); break; case 308: _LL6182: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6187=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6187[ 0]=({ struct Cyc_Exp_tok_struct
_temp6188; _temp6188.tag= Cyc_Exp_tok_tag; _temp6188.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6189= yyvs; struct _xtunion_struct** _temp6191=
_temp6189.curr +( yyvsp_offset - 2); if( _temp6191 < _temp6189.base? 1:
_temp6191 >= _temp6189.last_plus_one){ _throw( Null_Exception);}* _temp6191;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6192= yyvs; struct _xtunion_struct**
_temp6194= _temp6192.curr + yyvsp_offset; if( _temp6194 < _temp6192.base? 1:
_temp6194 >= _temp6192.last_plus_one){ _throw( Null_Exception);}* _temp6194;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6195= yyls; struct Cyc_Yyltype*
_temp6197= _temp6195.curr +( yylsp_offset - 2); if( _temp6197 < _temp6195.base?
1: _temp6197 >= _temp6195.last_plus_one){ _throw( Null_Exception);}* _temp6197;}).first_line,({
struct _tagged_ptr3 _temp6198= yyls; struct Cyc_Yyltype* _temp6200= _temp6198.curr
+ yylsp_offset; if( _temp6200 < _temp6198.base? 1: _temp6200 >= _temp6198.last_plus_one){
_throw( Null_Exception);}* _temp6200;}).last_line)); _temp6188;}); _temp6187;});
break; case 309: _LL6186: yyval=({ struct _tagged_ptr2 _temp6202= yyvs; struct
_xtunion_struct** _temp6204= _temp6202.curr + yyvsp_offset; if( _temp6204 <
_temp6202.base? 1: _temp6204 >= _temp6202.last_plus_one){ _throw( Null_Exception);}*
_temp6204;}); break; case 310: _LL6201: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6206=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6206[ 0]=({ struct Cyc_Exp_tok_struct
_temp6207; _temp6207.tag= Cyc_Exp_tok_tag; _temp6207.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6208= yyvs; struct _xtunion_struct** _temp6210=
_temp6208.curr +( yyvsp_offset - 2); if( _temp6210 < _temp6208.base? 1:
_temp6210 >= _temp6208.last_plus_one){ _throw( Null_Exception);}* _temp6210;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6211= yyvs; struct _xtunion_struct**
_temp6213= _temp6211.curr + yyvsp_offset; if( _temp6213 < _temp6211.base? 1:
_temp6213 >= _temp6211.last_plus_one){ _throw( Null_Exception);}* _temp6213;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6214= yyls; struct Cyc_Yyltype*
_temp6216= _temp6214.curr +( yylsp_offset - 2); if( _temp6216 < _temp6214.base?
1: _temp6216 >= _temp6214.last_plus_one){ _throw( Null_Exception);}* _temp6216;}).first_line,({
struct _tagged_ptr3 _temp6217= yyls; struct Cyc_Yyltype* _temp6219= _temp6217.curr
+ yylsp_offset; if( _temp6219 < _temp6217.base? 1: _temp6219 >= _temp6217.last_plus_one){
_throw( Null_Exception);}* _temp6219;}).last_line)); _temp6207;}); _temp6206;});
break; case 311: _LL6205: yyval=({ struct _tagged_ptr2 _temp6221= yyvs; struct
_xtunion_struct** _temp6223= _temp6221.curr + yyvsp_offset; if( _temp6223 <
_temp6221.base? 1: _temp6223 >= _temp6221.last_plus_one){ _throw( Null_Exception);}*
_temp6223;}); break; case 312: _LL6220: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6225=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6225[ 0]=({ struct Cyc_Exp_tok_struct
_temp6226; _temp6226.tag= Cyc_Exp_tok_tag; _temp6226.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6227= yyvs;
struct _xtunion_struct** _temp6229= _temp6227.curr +( yyvsp_offset - 2); if(
_temp6229 < _temp6227.base? 1: _temp6229 >= _temp6227.last_plus_one){ _throw(
Null_Exception);}* _temp6229;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6230= yyvs; struct _xtunion_struct** _temp6232= _temp6230.curr +
yyvsp_offset; if( _temp6232 < _temp6230.base? 1: _temp6232 >= _temp6230.last_plus_one){
_throw( Null_Exception);}* _temp6232;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6233= yyls; struct Cyc_Yyltype* _temp6235= _temp6233.curr +(
yylsp_offset - 2); if( _temp6235 < _temp6233.base? 1: _temp6235 >= _temp6233.last_plus_one){
_throw( Null_Exception);}* _temp6235;}).first_line,({ struct _tagged_ptr3
_temp6236= yyls; struct Cyc_Yyltype* _temp6238= _temp6236.curr + yylsp_offset;
if( _temp6238 < _temp6236.base? 1: _temp6238 >= _temp6236.last_plus_one){ _throw(
Null_Exception);}* _temp6238;}).last_line)); _temp6226;}); _temp6225;}); break;
case 313: _LL6224: yyval=({ struct _tagged_ptr2 _temp6240= yyvs; struct
_xtunion_struct** _temp6242= _temp6240.curr + yyvsp_offset; if( _temp6242 <
_temp6240.base? 1: _temp6242 >= _temp6240.last_plus_one){ _throw( Null_Exception);}*
_temp6242;}); break; case 314: _LL6239: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6244=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6244[ 0]=({ struct Cyc_Exp_tok_struct
_temp6245; _temp6245.tag= Cyc_Exp_tok_tag; _temp6245.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6246=
yyvs; struct _xtunion_struct** _temp6248= _temp6246.curr +( yyvsp_offset - 2);
if( _temp6248 < _temp6246.base? 1: _temp6248 >= _temp6246.last_plus_one){ _throw(
Null_Exception);}* _temp6248;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6249= yyvs; struct _xtunion_struct** _temp6251= _temp6249.curr +
yyvsp_offset; if( _temp6251 < _temp6249.base? 1: _temp6251 >= _temp6249.last_plus_one){
_throw( Null_Exception);}* _temp6251;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6252= yyls; struct Cyc_Yyltype* _temp6254= _temp6252.curr +(
yylsp_offset - 2); if( _temp6254 < _temp6252.base? 1: _temp6254 >= _temp6252.last_plus_one){
_throw( Null_Exception);}* _temp6254;}).first_line,({ struct _tagged_ptr3
_temp6255= yyls; struct Cyc_Yyltype* _temp6257= _temp6255.curr + yylsp_offset;
if( _temp6257 < _temp6255.base? 1: _temp6257 >= _temp6255.last_plus_one){ _throw(
Null_Exception);}* _temp6257;}).last_line)); _temp6245;}); _temp6244;}); break;
case 315: _LL6243: yyval=({ struct _tagged_ptr2 _temp6259= yyvs; struct
_xtunion_struct** _temp6261= _temp6259.curr + yyvsp_offset; if( _temp6261 <
_temp6259.base? 1: _temp6261 >= _temp6259.last_plus_one){ _throw( Null_Exception);}*
_temp6261;}); break; case 316: _LL6258: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6263=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6263[ 0]=({ struct Cyc_Exp_tok_struct
_temp6264; _temp6264.tag= Cyc_Exp_tok_tag; _temp6264.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6265=
yyvs; struct _xtunion_struct** _temp6267= _temp6265.curr +( yyvsp_offset - 2);
if( _temp6267 < _temp6265.base? 1: _temp6267 >= _temp6265.last_plus_one){ _throw(
Null_Exception);}* _temp6267;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6268= yyvs; struct _xtunion_struct** _temp6270= _temp6268.curr +
yyvsp_offset; if( _temp6270 < _temp6268.base? 1: _temp6270 >= _temp6268.last_plus_one){
_throw( Null_Exception);}* _temp6270;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6271= yyls; struct Cyc_Yyltype* _temp6273= _temp6271.curr +(
yylsp_offset - 2); if( _temp6273 < _temp6271.base? 1: _temp6273 >= _temp6271.last_plus_one){
_throw( Null_Exception);}* _temp6273;}).first_line,({ struct _tagged_ptr3
_temp6274= yyls; struct Cyc_Yyltype* _temp6276= _temp6274.curr + yylsp_offset;
if( _temp6276 < _temp6274.base? 1: _temp6276 >= _temp6274.last_plus_one){ _throw(
Null_Exception);}* _temp6276;}).last_line)); _temp6264;}); _temp6263;}); break;
case 317: _LL6262: yyval=({ struct _tagged_ptr2 _temp6278= yyvs; struct
_xtunion_struct** _temp6280= _temp6278.curr + yyvsp_offset; if( _temp6280 <
_temp6278.base? 1: _temp6280 >= _temp6278.last_plus_one){ _throw( Null_Exception);}*
_temp6280;}); break; case 318: _LL6277: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6282=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6282[ 0]=({ struct Cyc_Exp_tok_struct
_temp6283; _temp6283.tag= Cyc_Exp_tok_tag; _temp6283.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6284= yyvs; struct _xtunion_struct** _temp6286=
_temp6284.curr +( yyvsp_offset - 2); if( _temp6286 < _temp6284.base? 1:
_temp6286 >= _temp6284.last_plus_one){ _throw( Null_Exception);}* _temp6286;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6287= yyvs; struct _xtunion_struct**
_temp6289= _temp6287.curr + yyvsp_offset; if( _temp6289 < _temp6287.base? 1:
_temp6289 >= _temp6287.last_plus_one){ _throw( Null_Exception);}* _temp6289;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6290= yyls; struct Cyc_Yyltype*
_temp6292= _temp6290.curr +( yylsp_offset - 2); if( _temp6292 < _temp6290.base?
1: _temp6292 >= _temp6290.last_plus_one){ _throw( Null_Exception);}* _temp6292;}).first_line,({
struct _tagged_ptr3 _temp6293= yyls; struct Cyc_Yyltype* _temp6295= _temp6293.curr
+ yylsp_offset; if( _temp6295 < _temp6293.base? 1: _temp6295 >= _temp6293.last_plus_one){
_throw( Null_Exception);}* _temp6295;}).last_line)); _temp6283;}); _temp6282;});
break; case 319: _LL6281: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6297=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6297[ 0]=({ struct Cyc_Exp_tok_struct _temp6298; _temp6298.tag= Cyc_Exp_tok_tag;
_temp6298.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6299= yyvs; struct _xtunion_struct** _temp6301= _temp6299.curr +(
yyvsp_offset - 2); if( _temp6301 < _temp6299.base? 1: _temp6301 >= _temp6299.last_plus_one){
_throw( Null_Exception);}* _temp6301;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6302= yyvs; struct _xtunion_struct** _temp6304= _temp6302.curr
+ yyvsp_offset; if( _temp6304 < _temp6302.base? 1: _temp6304 >= _temp6302.last_plus_one){
_throw( Null_Exception);}* _temp6304;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6305= yyls; struct Cyc_Yyltype* _temp6307= _temp6305.curr +(
yylsp_offset - 2); if( _temp6307 < _temp6305.base? 1: _temp6307 >= _temp6305.last_plus_one){
_throw( Null_Exception);}* _temp6307;}).first_line,({ struct _tagged_ptr3
_temp6308= yyls; struct Cyc_Yyltype* _temp6310= _temp6308.curr + yylsp_offset;
if( _temp6310 < _temp6308.base? 1: _temp6310 >= _temp6308.last_plus_one){ _throw(
Null_Exception);}* _temp6310;}).last_line)); _temp6298;}); _temp6297;}); break;
case 320: _LL6296: yyval=({ struct _tagged_ptr2 _temp6312= yyvs; struct
_xtunion_struct** _temp6314= _temp6312.curr + yyvsp_offset; if( _temp6314 <
_temp6312.base? 1: _temp6314 >= _temp6312.last_plus_one){ _throw( Null_Exception);}*
_temp6314;}); break; case 321: _LL6311: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6316=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6316[ 0]=({ struct Cyc_Exp_tok_struct
_temp6317; _temp6317.tag= Cyc_Exp_tok_tag; _temp6317.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6318= yyvs; struct _xtunion_struct** _temp6320=
_temp6318.curr +( yyvsp_offset - 2); if( _temp6320 < _temp6318.base? 1:
_temp6320 >= _temp6318.last_plus_one){ _throw( Null_Exception);}* _temp6320;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6321= yyvs; struct _xtunion_struct**
_temp6323= _temp6321.curr + yyvsp_offset; if( _temp6323 < _temp6321.base? 1:
_temp6323 >= _temp6321.last_plus_one){ _throw( Null_Exception);}* _temp6323;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6324= yyls; struct Cyc_Yyltype*
_temp6326= _temp6324.curr +( yylsp_offset - 2); if( _temp6326 < _temp6324.base?
1: _temp6326 >= _temp6324.last_plus_one){ _throw( Null_Exception);}* _temp6326;}).first_line,({
struct _tagged_ptr3 _temp6327= yyls; struct Cyc_Yyltype* _temp6329= _temp6327.curr
+ yylsp_offset; if( _temp6329 < _temp6327.base? 1: _temp6329 >= _temp6327.last_plus_one){
_throw( Null_Exception);}* _temp6329;}).last_line)); _temp6317;}); _temp6316;});
break; case 322: _LL6315: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6331=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6331[ 0]=({ struct Cyc_Exp_tok_struct _temp6332; _temp6332.tag= Cyc_Exp_tok_tag;
_temp6332.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6333= yyvs; struct _xtunion_struct** _temp6335= _temp6333.curr +(
yyvsp_offset - 2); if( _temp6335 < _temp6333.base? 1: _temp6335 >= _temp6333.last_plus_one){
_throw( Null_Exception);}* _temp6335;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6336= yyvs; struct _xtunion_struct** _temp6338= _temp6336.curr
+ yyvsp_offset; if( _temp6338 < _temp6336.base? 1: _temp6338 >= _temp6336.last_plus_one){
_throw( Null_Exception);}* _temp6338;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6339= yyls; struct Cyc_Yyltype* _temp6341= _temp6339.curr +(
yylsp_offset - 2); if( _temp6341 < _temp6339.base? 1: _temp6341 >= _temp6339.last_plus_one){
_throw( Null_Exception);}* _temp6341;}).first_line,({ struct _tagged_ptr3
_temp6342= yyls; struct Cyc_Yyltype* _temp6344= _temp6342.curr + yylsp_offset;
if( _temp6344 < _temp6342.base? 1: _temp6344 >= _temp6342.last_plus_one){ _throw(
Null_Exception);}* _temp6344;}).last_line)); _temp6332;}); _temp6331;}); break;
case 323: _LL6330: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6346=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6346[ 0]=({ struct Cyc_Exp_tok_struct _temp6347; _temp6347.tag= Cyc_Exp_tok_tag;
_temp6347.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6348= yyvs; struct _xtunion_struct** _temp6350= _temp6348.curr +(
yyvsp_offset - 2); if( _temp6350 < _temp6348.base? 1: _temp6350 >= _temp6348.last_plus_one){
_throw( Null_Exception);}* _temp6350;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6351= yyvs; struct _xtunion_struct** _temp6353= _temp6351.curr
+ yyvsp_offset; if( _temp6353 < _temp6351.base? 1: _temp6353 >= _temp6351.last_plus_one){
_throw( Null_Exception);}* _temp6353;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6354= yyls; struct Cyc_Yyltype* _temp6356= _temp6354.curr +(
yylsp_offset - 2); if( _temp6356 < _temp6354.base? 1: _temp6356 >= _temp6354.last_plus_one){
_throw( Null_Exception);}* _temp6356;}).first_line,({ struct _tagged_ptr3
_temp6357= yyls; struct Cyc_Yyltype* _temp6359= _temp6357.curr + yylsp_offset;
if( _temp6359 < _temp6357.base? 1: _temp6359 >= _temp6357.last_plus_one){ _throw(
Null_Exception);}* _temp6359;}).last_line)); _temp6347;}); _temp6346;}); break;
case 324: _LL6345: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6361=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6361[ 0]=({ struct Cyc_Exp_tok_struct _temp6362; _temp6362.tag= Cyc_Exp_tok_tag;
_temp6362.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6363= yyvs; struct _xtunion_struct** _temp6365= _temp6363.curr +(
yyvsp_offset - 2); if( _temp6365 < _temp6363.base? 1: _temp6365 >= _temp6363.last_plus_one){
_throw( Null_Exception);}* _temp6365;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6366= yyvs; struct _xtunion_struct** _temp6368= _temp6366.curr
+ yyvsp_offset; if( _temp6368 < _temp6366.base? 1: _temp6368 >= _temp6366.last_plus_one){
_throw( Null_Exception);}* _temp6368;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6369= yyls; struct Cyc_Yyltype* _temp6371= _temp6369.curr +(
yylsp_offset - 2); if( _temp6371 < _temp6369.base? 1: _temp6371 >= _temp6369.last_plus_one){
_throw( Null_Exception);}* _temp6371;}).first_line,({ struct _tagged_ptr3
_temp6372= yyls; struct Cyc_Yyltype* _temp6374= _temp6372.curr + yylsp_offset;
if( _temp6374 < _temp6372.base? 1: _temp6374 >= _temp6372.last_plus_one){ _throw(
Null_Exception);}* _temp6374;}).last_line)); _temp6362;}); _temp6361;}); break;
case 325: _LL6360: yyval=({ struct _tagged_ptr2 _temp6376= yyvs; struct
_xtunion_struct** _temp6378= _temp6376.curr + yyvsp_offset; if( _temp6378 <
_temp6376.base? 1: _temp6378 >= _temp6376.last_plus_one){ _throw( Null_Exception);}*
_temp6378;}); break; case 326: _LL6375: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6380=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6380[ 0]=({ struct Cyc_Exp_tok_struct
_temp6381; _temp6381.tag= Cyc_Exp_tok_tag; _temp6381.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6382=
yyvs; struct _xtunion_struct** _temp6384= _temp6382.curr +( yyvsp_offset - 2);
if( _temp6384 < _temp6382.base? 1: _temp6384 >= _temp6382.last_plus_one){ _throw(
Null_Exception);}* _temp6384;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6385= yyvs; struct _xtunion_struct** _temp6387= _temp6385.curr +
yyvsp_offset; if( _temp6387 < _temp6385.base? 1: _temp6387 >= _temp6385.last_plus_one){
_throw( Null_Exception);}* _temp6387;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6388= yyls; struct Cyc_Yyltype* _temp6390= _temp6388.curr +(
yylsp_offset - 2); if( _temp6390 < _temp6388.base? 1: _temp6390 >= _temp6388.last_plus_one){
_throw( Null_Exception);}* _temp6390;}).first_line,({ struct _tagged_ptr3
_temp6391= yyls; struct Cyc_Yyltype* _temp6393= _temp6391.curr + yylsp_offset;
if( _temp6393 < _temp6391.base? 1: _temp6393 >= _temp6391.last_plus_one){ _throw(
Null_Exception);}* _temp6393;}).last_line)); _temp6381;}); _temp6380;}); break;
case 327: _LL6379: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6395=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6395[ 0]=({ struct Cyc_Exp_tok_struct _temp6396; _temp6396.tag= Cyc_Exp_tok_tag;
_temp6396.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6397= yyvs; struct _xtunion_struct** _temp6399=
_temp6397.curr +( yyvsp_offset - 2); if( _temp6399 < _temp6397.base? 1:
_temp6399 >= _temp6397.last_plus_one){ _throw( Null_Exception);}* _temp6399;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6400= yyvs; struct _xtunion_struct**
_temp6402= _temp6400.curr + yyvsp_offset; if( _temp6402 < _temp6400.base? 1:
_temp6402 >= _temp6400.last_plus_one){ _throw( Null_Exception);}* _temp6402;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6403= yyls; struct Cyc_Yyltype*
_temp6405= _temp6403.curr +( yylsp_offset - 2); if( _temp6405 < _temp6403.base?
1: _temp6405 >= _temp6403.last_plus_one){ _throw( Null_Exception);}* _temp6405;}).first_line,({
struct _tagged_ptr3 _temp6406= yyls; struct Cyc_Yyltype* _temp6408= _temp6406.curr
+ yylsp_offset; if( _temp6408 < _temp6406.base? 1: _temp6408 >= _temp6406.last_plus_one){
_throw( Null_Exception);}* _temp6408;}).last_line)); _temp6396;}); _temp6395;});
break; case 328: _LL6394: yyval=({ struct _tagged_ptr2 _temp6410= yyvs; struct
_xtunion_struct** _temp6412= _temp6410.curr + yyvsp_offset; if( _temp6412 <
_temp6410.base? 1: _temp6412 >= _temp6410.last_plus_one){ _throw( Null_Exception);}*
_temp6412;}); break; case 329: _LL6409: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6414=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6414[ 0]=({ struct Cyc_Exp_tok_struct
_temp6415; _temp6415.tag= Cyc_Exp_tok_tag; _temp6415.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6416= yyvs;
struct _xtunion_struct** _temp6418= _temp6416.curr +( yyvsp_offset - 2); if(
_temp6418 < _temp6416.base? 1: _temp6418 >= _temp6416.last_plus_one){ _throw(
Null_Exception);}* _temp6418;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6419= yyvs; struct _xtunion_struct** _temp6421= _temp6419.curr +
yyvsp_offset; if( _temp6421 < _temp6419.base? 1: _temp6421 >= _temp6419.last_plus_one){
_throw( Null_Exception);}* _temp6421;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6422= yyls; struct Cyc_Yyltype* _temp6424= _temp6422.curr +(
yylsp_offset - 2); if( _temp6424 < _temp6422.base? 1: _temp6424 >= _temp6422.last_plus_one){
_throw( Null_Exception);}* _temp6424;}).first_line,({ struct _tagged_ptr3
_temp6425= yyls; struct Cyc_Yyltype* _temp6427= _temp6425.curr + yylsp_offset;
if( _temp6427 < _temp6425.base? 1: _temp6427 >= _temp6425.last_plus_one){ _throw(
Null_Exception);}* _temp6427;}).last_line)); _temp6415;}); _temp6414;}); break;
case 330: _LL6413: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6429=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6429[ 0]=({ struct Cyc_Exp_tok_struct _temp6430; _temp6430.tag= Cyc_Exp_tok_tag;
_temp6430.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6431= yyvs; struct _xtunion_struct** _temp6433=
_temp6431.curr +( yyvsp_offset - 2); if( _temp6433 < _temp6431.base? 1:
_temp6433 >= _temp6431.last_plus_one){ _throw( Null_Exception);}* _temp6433;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6434= yyvs; struct _xtunion_struct**
_temp6436= _temp6434.curr + yyvsp_offset; if( _temp6436 < _temp6434.base? 1:
_temp6436 >= _temp6434.last_plus_one){ _throw( Null_Exception);}* _temp6436;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6437= yyls; struct Cyc_Yyltype*
_temp6439= _temp6437.curr +( yylsp_offset - 2); if( _temp6439 < _temp6437.base?
1: _temp6439 >= _temp6437.last_plus_one){ _throw( Null_Exception);}* _temp6439;}).first_line,({
struct _tagged_ptr3 _temp6440= yyls; struct Cyc_Yyltype* _temp6442= _temp6440.curr
+ yylsp_offset; if( _temp6442 < _temp6440.base? 1: _temp6442 >= _temp6440.last_plus_one){
_throw( Null_Exception);}* _temp6442;}).last_line)); _temp6430;}); _temp6429;});
break; case 331: _LL6428: yyval=({ struct _tagged_ptr2 _temp6444= yyvs; struct
_xtunion_struct** _temp6446= _temp6444.curr + yyvsp_offset; if( _temp6446 <
_temp6444.base? 1: _temp6446 >= _temp6444.last_plus_one){ _throw( Null_Exception);}*
_temp6446;}); break; case 332: _LL6443: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6448=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6448[ 0]=({ struct Cyc_Exp_tok_struct
_temp6449; _temp6449.tag= Cyc_Exp_tok_tag; _temp6449.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6450= yyvs;
struct _xtunion_struct** _temp6452= _temp6450.curr +( yyvsp_offset - 2); if(
_temp6452 < _temp6450.base? 1: _temp6452 >= _temp6450.last_plus_one){ _throw(
Null_Exception);}* _temp6452;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6453= yyvs; struct _xtunion_struct** _temp6455= _temp6453.curr +
yyvsp_offset; if( _temp6455 < _temp6453.base? 1: _temp6455 >= _temp6453.last_plus_one){
_throw( Null_Exception);}* _temp6455;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6456= yyls; struct Cyc_Yyltype* _temp6458= _temp6456.curr +(
yylsp_offset - 2); if( _temp6458 < _temp6456.base? 1: _temp6458 >= _temp6456.last_plus_one){
_throw( Null_Exception);}* _temp6458;}).first_line,({ struct _tagged_ptr3
_temp6459= yyls; struct Cyc_Yyltype* _temp6461= _temp6459.curr + yylsp_offset;
if( _temp6461 < _temp6459.base? 1: _temp6461 >= _temp6459.last_plus_one){ _throw(
Null_Exception);}* _temp6461;}).last_line)); _temp6449;}); _temp6448;}); break;
case 333: _LL6447: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6463=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6463[ 0]=({ struct Cyc_Exp_tok_struct _temp6464; _temp6464.tag= Cyc_Exp_tok_tag;
_temp6464.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6465= yyvs; struct _xtunion_struct** _temp6467=
_temp6465.curr +( yyvsp_offset - 2); if( _temp6467 < _temp6465.base? 1:
_temp6467 >= _temp6465.last_plus_one){ _throw( Null_Exception);}* _temp6467;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6468= yyvs; struct _xtunion_struct**
_temp6470= _temp6468.curr + yyvsp_offset; if( _temp6470 < _temp6468.base? 1:
_temp6470 >= _temp6468.last_plus_one){ _throw( Null_Exception);}* _temp6470;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6471= yyls; struct Cyc_Yyltype*
_temp6473= _temp6471.curr +( yylsp_offset - 2); if( _temp6473 < _temp6471.base?
1: _temp6473 >= _temp6471.last_plus_one){ _throw( Null_Exception);}* _temp6473;}).first_line,({
struct _tagged_ptr3 _temp6474= yyls; struct Cyc_Yyltype* _temp6476= _temp6474.curr
+ yylsp_offset; if( _temp6476 < _temp6474.base? 1: _temp6476 >= _temp6474.last_plus_one){
_throw( Null_Exception);}* _temp6476;}).last_line)); _temp6464;}); _temp6463;});
break; case 334: _LL6462: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6478=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6478[ 0]=({ struct Cyc_Exp_tok_struct _temp6479; _temp6479.tag= Cyc_Exp_tok_tag;
_temp6479.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6480= yyvs; struct _xtunion_struct** _temp6482=
_temp6480.curr +( yyvsp_offset - 2); if( _temp6482 < _temp6480.base? 1:
_temp6482 >= _temp6480.last_plus_one){ _throw( Null_Exception);}* _temp6482;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6483= yyvs; struct _xtunion_struct**
_temp6485= _temp6483.curr + yyvsp_offset; if( _temp6485 < _temp6483.base? 1:
_temp6485 >= _temp6483.last_plus_one){ _throw( Null_Exception);}* _temp6485;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6486= yyls; struct Cyc_Yyltype*
_temp6488= _temp6486.curr +( yylsp_offset - 2); if( _temp6488 < _temp6486.base?
1: _temp6488 >= _temp6486.last_plus_one){ _throw( Null_Exception);}* _temp6488;}).first_line,({
struct _tagged_ptr3 _temp6489= yyls; struct Cyc_Yyltype* _temp6491= _temp6489.curr
+ yylsp_offset; if( _temp6491 < _temp6489.base? 1: _temp6491 >= _temp6489.last_plus_one){
_throw( Null_Exception);}* _temp6491;}).last_line)); _temp6479;}); _temp6478;});
break; case 335: _LL6477: yyval=({ struct _tagged_ptr2 _temp6493= yyvs; struct
_xtunion_struct** _temp6495= _temp6493.curr + yyvsp_offset; if( _temp6495 <
_temp6493.base? 1: _temp6495 >= _temp6493.last_plus_one){ _throw( Null_Exception);}*
_temp6495;}); break; case 336: _LL6492: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6497=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6497[ 0]=({ struct Cyc_Exp_tok_struct
_temp6498; _temp6498.tag= Cyc_Exp_tok_tag; _temp6498.f1= Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6499= yyvs; struct
_xtunion_struct** _temp6501= _temp6499.curr +( yyvsp_offset - 2); if( _temp6501
< _temp6499.base? 1: _temp6501 >= _temp6499.last_plus_one){ _throw(
Null_Exception);}* _temp6501;}))).f3, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6502= yyvs; struct _xtunion_struct** _temp6504= _temp6502.curr +
yyvsp_offset; if( _temp6504 < _temp6502.base? 1: _temp6504 >= _temp6502.last_plus_one){
_throw( Null_Exception);}* _temp6504;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6505= yyls; struct Cyc_Yyltype* _temp6507= _temp6505.curr +(
yylsp_offset - 3); if( _temp6507 < _temp6505.base? 1: _temp6507 >= _temp6505.last_plus_one){
_throw( Null_Exception);}* _temp6507;}).first_line,({ struct _tagged_ptr3
_temp6508= yyls; struct Cyc_Yyltype* _temp6510= _temp6508.curr + yylsp_offset;
if( _temp6510 < _temp6508.base? 1: _temp6510 >= _temp6508.last_plus_one){ _throw(
Null_Exception);}* _temp6510;}).last_line)); _temp6498;}); _temp6497;}); break;
case 337: _LL6496: yyval=({ struct _tagged_ptr2 _temp6512= yyvs; struct
_xtunion_struct** _temp6514= _temp6512.curr + yyvsp_offset; if( _temp6514 <
_temp6512.base? 1: _temp6514 >= _temp6512.last_plus_one){ _throw( Null_Exception);}*
_temp6514;}); break; case 338: _LL6511: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6516=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6516[ 0]=({ struct Cyc_Exp_tok_struct
_temp6517; _temp6517.tag= Cyc_Exp_tok_tag; _temp6517.f1= Cyc_Absyn_pre_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6518= yyvs; struct _xtunion_struct**
_temp6520= _temp6518.curr + yyvsp_offset; if( _temp6520 < _temp6518.base? 1:
_temp6520 >= _temp6518.last_plus_one){ _throw( Null_Exception);}* _temp6520;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6521= yyls; struct Cyc_Yyltype*
_temp6523= _temp6521.curr +( yylsp_offset - 1); if( _temp6523 < _temp6521.base?
1: _temp6523 >= _temp6521.last_plus_one){ _throw( Null_Exception);}* _temp6523;}).first_line,({
struct _tagged_ptr3 _temp6524= yyls; struct Cyc_Yyltype* _temp6526= _temp6524.curr
+ yylsp_offset; if( _temp6526 < _temp6524.base? 1: _temp6526 >= _temp6524.last_plus_one){
_throw( Null_Exception);}* _temp6526;}).last_line)); _temp6517;}); _temp6516;});
break; case 339: _LL6515: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6528=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6528[ 0]=({ struct Cyc_Exp_tok_struct _temp6529; _temp6529.tag= Cyc_Exp_tok_tag;
_temp6529.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6530= yyvs; struct _xtunion_struct** _temp6532= _temp6530.curr +
yyvsp_offset; if( _temp6532 < _temp6530.base? 1: _temp6532 >= _temp6530.last_plus_one){
_throw( Null_Exception);}* _temp6532;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6533= yyls; struct Cyc_Yyltype* _temp6535= _temp6533.curr +(
yylsp_offset - 1); if( _temp6535 < _temp6533.base? 1: _temp6535 >= _temp6533.last_plus_one){
_throw( Null_Exception);}* _temp6535;}).first_line,({ struct _tagged_ptr3
_temp6536= yyls; struct Cyc_Yyltype* _temp6538= _temp6536.curr + yylsp_offset;
if( _temp6538 < _temp6536.base? 1: _temp6538 >= _temp6536.last_plus_one){ _throw(
Null_Exception);}* _temp6538;}).last_line)); _temp6529;}); _temp6528;}); break;
case 340: _LL6527: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6540=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6540[ 0]=({ struct Cyc_Exp_tok_struct _temp6541; _temp6541.tag= Cyc_Exp_tok_tag;
_temp6541.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6542= yyvs; struct _xtunion_struct** _temp6544= _temp6542.curr +
yyvsp_offset; if( _temp6544 < _temp6542.base? 1: _temp6544 >= _temp6542.last_plus_one){
_throw( Null_Exception);}* _temp6544;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6545= yyls; struct Cyc_Yyltype* _temp6547= _temp6545.curr +(
yylsp_offset - 1); if( _temp6547 < _temp6545.base? 1: _temp6547 >= _temp6545.last_plus_one){
_throw( Null_Exception);}* _temp6547;}).first_line,({ struct _tagged_ptr3
_temp6548= yyls; struct Cyc_Yyltype* _temp6550= _temp6548.curr + yylsp_offset;
if( _temp6550 < _temp6548.base? 1: _temp6550 >= _temp6548.last_plus_one){ _throw(
Null_Exception);}* _temp6550;}).last_line)); _temp6541;}); _temp6540;}); break;
case 341: _LL6539: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6552=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6552[ 0]=({ struct Cyc_Exp_tok_struct _temp6553; _temp6553.tag= Cyc_Exp_tok_tag;
_temp6553.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6554= yyvs; struct _xtunion_struct** _temp6556= _temp6554.curr +
yyvsp_offset; if( _temp6556 < _temp6554.base? 1: _temp6556 >= _temp6554.last_plus_one){
_throw( Null_Exception);}* _temp6556;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6557= yyls; struct Cyc_Yyltype* _temp6559= _temp6557.curr +(
yylsp_offset - 1); if( _temp6559 < _temp6557.base? 1: _temp6559 >= _temp6557.last_plus_one){
_throw( Null_Exception);}* _temp6559;}).first_line,({ struct _tagged_ptr3
_temp6560= yyls; struct Cyc_Yyltype* _temp6562= _temp6560.curr + yylsp_offset;
if( _temp6562 < _temp6560.base? 1: _temp6562 >= _temp6560.last_plus_one){ _throw(
Null_Exception);}* _temp6562;}).last_line)); _temp6553;}); _temp6552;}); break;
case 342: _LL6551: yyval=({ struct _tagged_ptr2 _temp6564= yyvs; struct
_xtunion_struct** _temp6566= _temp6564.curr + yyvsp_offset; if( _temp6566 <
_temp6564.base? 1: _temp6566 >= _temp6564.last_plus_one){ _throw( Null_Exception);}*
_temp6566;}); break; case 343: _LL6563: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6568=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6568[ 0]=({ struct Cyc_Exp_tok_struct
_temp6569; _temp6569.tag= Cyc_Exp_tok_tag; _temp6569.f1= Cyc_Absyn_prim1_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp6570= yyvs; struct
_xtunion_struct** _temp6572= _temp6570.curr +( yyvsp_offset - 1); if( _temp6572
< _temp6570.base? 1: _temp6572 >= _temp6570.last_plus_one){ _throw(
Null_Exception);}* _temp6572;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6573= yyvs; struct _xtunion_struct** _temp6575= _temp6573.curr +
yyvsp_offset; if( _temp6575 < _temp6573.base? 1: _temp6575 >= _temp6573.last_plus_one){
_throw( Null_Exception);}* _temp6575;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6576= yyls; struct Cyc_Yyltype* _temp6578= _temp6576.curr +(
yylsp_offset - 1); if( _temp6578 < _temp6576.base? 1: _temp6578 >= _temp6576.last_plus_one){
_throw( Null_Exception);}* _temp6578;}).first_line,({ struct _tagged_ptr3
_temp6579= yyls; struct Cyc_Yyltype* _temp6581= _temp6579.curr + yylsp_offset;
if( _temp6581 < _temp6579.base? 1: _temp6581 >= _temp6579.last_plus_one){ _throw(
Null_Exception);}* _temp6581;}).last_line)); _temp6569;}); _temp6568;}); break;
case 344: _LL6567: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6583=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6583[ 0]=({ struct Cyc_Exp_tok_struct _temp6584; _temp6584.tag= Cyc_Exp_tok_tag;
_temp6584.f1= Cyc_Absyn_sizeoftyp_exp((* Cyc_yyget_ParamDecl_tok(({ struct
_tagged_ptr2 _temp6585= yyvs; struct _xtunion_struct** _temp6587= _temp6585.curr
+( yyvsp_offset - 1); if( _temp6587 < _temp6585.base? 1: _temp6587 >= _temp6585.last_plus_one){
_throw( Null_Exception);}* _temp6587;}))).f3, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6588= yyls; struct Cyc_Yyltype* _temp6590= _temp6588.curr
+( yylsp_offset - 3); if( _temp6590 < _temp6588.base? 1: _temp6590 >= _temp6588.last_plus_one){
_throw( Null_Exception);}* _temp6590;}).first_line,({ struct _tagged_ptr3
_temp6591= yyls; struct Cyc_Yyltype* _temp6593= _temp6591.curr + yylsp_offset;
if( _temp6593 < _temp6591.base? 1: _temp6593 >= _temp6591.last_plus_one){ _throw(
Null_Exception);}* _temp6593;}).last_line)); _temp6584;}); _temp6583;}); break;
case 345: _LL6582: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6595=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6595[ 0]=({ struct Cyc_Exp_tok_struct _temp6596; _temp6596.tag= Cyc_Exp_tok_tag;
_temp6596.f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6597= yyvs; struct _xtunion_struct** _temp6599= _temp6597.curr +
yyvsp_offset; if( _temp6599 < _temp6597.base? 1: _temp6599 >= _temp6597.last_plus_one){
_throw( Null_Exception);}* _temp6599;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6600= yyls; struct Cyc_Yyltype* _temp6602= _temp6600.curr +(
yylsp_offset - 1); if( _temp6602 < _temp6600.base? 1: _temp6602 >= _temp6600.last_plus_one){
_throw( Null_Exception);}* _temp6602;}).first_line,({ struct _tagged_ptr3
_temp6603= yyls; struct Cyc_Yyltype* _temp6605= _temp6603.curr + yylsp_offset;
if( _temp6605 < _temp6603.base? 1: _temp6605 >= _temp6603.last_plus_one){ _throw(
Null_Exception);}* _temp6605;}).last_line)); _temp6596;}); _temp6595;}); break;
case 346: _LL6594: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6607=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6607[ 0]=({ struct Cyc_Exp_tok_struct _temp6608; _temp6608.tag= Cyc_Exp_tok_tag;
_temp6608.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(({ struct _tagged_ptr2
_temp6609= yyvs; struct _xtunion_struct** _temp6611= _temp6609.curr +(
yyvsp_offset - 3); if( _temp6611 < _temp6609.base? 1: _temp6611 >= _temp6609.last_plus_one){
_throw( Null_Exception);}* _temp6611;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6612= yyvs; struct _xtunion_struct** _temp6614= _temp6612.curr
+( yyvsp_offset - 1); if( _temp6614 < _temp6612.base? 1: _temp6614 >= _temp6612.last_plus_one){
_throw( Null_Exception);}* _temp6614;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6615= yyls; struct Cyc_Yyltype* _temp6617= _temp6615.curr +(
yylsp_offset - 3); if( _temp6617 < _temp6615.base? 1: _temp6617 >= _temp6615.last_plus_one){
_throw( Null_Exception);}* _temp6617;}).first_line,({ struct _tagged_ptr3
_temp6618= yyls; struct Cyc_Yyltype* _temp6620= _temp6618.curr + yylsp_offset;
if( _temp6620 < _temp6618.base? 1: _temp6620 >= _temp6618.last_plus_one){ _throw(
Null_Exception);}* _temp6620;}).last_line)); _temp6608;}); _temp6607;}); break;
case 347: _LL6606: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6622=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6622[ 0]=({ struct Cyc_Exp_tok_struct _temp6623; _temp6623.tag= Cyc_Exp_tok_tag;
_temp6623.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6624=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6624[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6625; _temp6625.tag= Cyc_Absyn_Malloc_e_tag;
_temp6625.f1=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp6626= yyvs; struct _xtunion_struct** _temp6628=
_temp6626.curr +( yyvsp_offset - 2); if( _temp6628 < _temp6626.base? 1:
_temp6628 >= _temp6626.last_plus_one){ _throw( Null_Exception);}* _temp6628;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6629= yyls; struct Cyc_Yyltype*
_temp6631= _temp6629.curr +( yylsp_offset - 2); if( _temp6631 < _temp6629.base?
1: _temp6631 >= _temp6629.last_plus_one){ _throw( Null_Exception);}* _temp6631;}).first_line,({
struct _tagged_ptr3 _temp6632= yyls; struct Cyc_Yyltype* _temp6634= _temp6632.curr
+( yylsp_offset - 2); if( _temp6634 < _temp6632.base? 1: _temp6634 >= _temp6632.last_plus_one){
_throw( Null_Exception);}* _temp6634;}).last_line)); _temp6625;}); _temp6624;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6635= yyls; struct Cyc_Yyltype*
_temp6637= _temp6635.curr +( yylsp_offset - 6); if( _temp6637 < _temp6635.base?
1: _temp6637 >= _temp6635.last_plus_one){ _throw( Null_Exception);}* _temp6637;}).first_line,({
struct _tagged_ptr3 _temp6638= yyls; struct Cyc_Yyltype* _temp6640= _temp6638.curr
+ yylsp_offset; if( _temp6640 < _temp6638.base? 1: _temp6640 >= _temp6638.last_plus_one){
_throw( Null_Exception);}* _temp6640;}).last_line)); _temp6623;}); _temp6622;});
break; case 348: _LL6621: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6642=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6642[ 0]=({ struct Cyc_Primop_tok_struct _temp6643; _temp6643.tag= Cyc_Primop_tok_tag;
_temp6643.f1=( void*)(( void*) Cyc_Absyn_Printf); _temp6643;}); _temp6642;});
break; case 349: _LL6641: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6645=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6645[ 0]=({ struct Cyc_Primop_tok_struct _temp6646; _temp6646.tag= Cyc_Primop_tok_tag;
_temp6646.f1=( void*)(( void*) Cyc_Absyn_Fprintf); _temp6646;}); _temp6645;});
break; case 350: _LL6644: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6648=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6648[ 0]=({ struct Cyc_Primop_tok_struct _temp6649; _temp6649.tag= Cyc_Primop_tok_tag;
_temp6649.f1=( void*)(( void*) Cyc_Absyn_Xprintf); _temp6649;}); _temp6648;});
break; case 351: _LL6647: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6651=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6651[ 0]=({ struct Cyc_Primop_tok_struct _temp6652; _temp6652.tag= Cyc_Primop_tok_tag;
_temp6652.f1=( void*)(( void*) Cyc_Absyn_Scanf); _temp6652;}); _temp6651;});
break; case 352: _LL6650: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6654=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6654[ 0]=({ struct Cyc_Primop_tok_struct _temp6655; _temp6655.tag= Cyc_Primop_tok_tag;
_temp6655.f1=( void*)(( void*) Cyc_Absyn_Fscanf); _temp6655;}); _temp6654;});
break; case 353: _LL6653: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6657=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6657[ 0]=({ struct Cyc_Primop_tok_struct _temp6658; _temp6658.tag= Cyc_Primop_tok_tag;
_temp6658.f1=( void*)(( void*) Cyc_Absyn_Sscanf); _temp6658;}); _temp6657;});
break; case 354: _LL6656: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6660=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6660[ 0]=({ struct Cyc_Primop_tok_struct _temp6661; _temp6661.tag= Cyc_Primop_tok_tag;
_temp6661.f1=( void*)(( void*) Cyc_Absyn_Bitnot); _temp6661;}); _temp6660;});
break; case 355: _LL6659: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6663=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6663[ 0]=({ struct Cyc_Primop_tok_struct _temp6664; _temp6664.tag= Cyc_Primop_tok_tag;
_temp6664.f1=( void*)(( void*) Cyc_Absyn_Not); _temp6664;}); _temp6663;});
break; case 356: _LL6662: yyval=( struct _xtunion_struct*)({ struct Cyc_Primop_tok_struct*
_temp6666=( struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6666[ 0]=({ struct Cyc_Primop_tok_struct _temp6667; _temp6667.tag= Cyc_Primop_tok_tag;
_temp6667.f1=( void*)(( void*) Cyc_Absyn_Minus); _temp6667;}); _temp6666;});
break; case 357: _LL6665: yyval=({ struct _tagged_ptr2 _temp6669= yyvs; struct
_xtunion_struct** _temp6671= _temp6669.curr + yyvsp_offset; if( _temp6671 <
_temp6669.base? 1: _temp6671 >= _temp6669.last_plus_one){ _throw( Null_Exception);}*
_temp6671;}); break; case 358: _LL6668: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6673=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6673[ 0]=({ struct Cyc_Exp_tok_struct
_temp6674; _temp6674.tag= Cyc_Exp_tok_tag; _temp6674.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6675= yyvs; struct _xtunion_struct**
_temp6677= _temp6675.curr +( yyvsp_offset - 3); if( _temp6677 < _temp6675.base?
1: _temp6677 >= _temp6675.last_plus_one){ _throw( Null_Exception);}* _temp6677;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6678= yyvs; struct _xtunion_struct**
_temp6680= _temp6678.curr +( yyvsp_offset - 1); if( _temp6680 < _temp6678.base?
1: _temp6680 >= _temp6678.last_plus_one){ _throw( Null_Exception);}* _temp6680;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6681= yyls; struct Cyc_Yyltype*
_temp6683= _temp6681.curr +( yylsp_offset - 3); if( _temp6683 < _temp6681.base?
1: _temp6683 >= _temp6681.last_plus_one){ _throw( Null_Exception);}* _temp6683;}).first_line,({
struct _tagged_ptr3 _temp6684= yyls; struct Cyc_Yyltype* _temp6686= _temp6684.curr
+ yylsp_offset; if( _temp6686 < _temp6684.base? 1: _temp6686 >= _temp6684.last_plus_one){
_throw( Null_Exception);}* _temp6686;}).last_line)); _temp6674;}); _temp6673;});
break; case 359: _LL6672: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6688=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6688[ 0]=({ struct Cyc_Exp_tok_struct _temp6689; _temp6689.tag= Cyc_Exp_tok_tag;
_temp6689.f1= Cyc_Absyn_unknowncall_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6690= yyvs; struct _xtunion_struct** _temp6692= _temp6690.curr
+( yyvsp_offset - 2); if( _temp6692 < _temp6690.base? 1: _temp6692 >= _temp6690.last_plus_one){
_throw( Null_Exception);}* _temp6692;})), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6693= yyls; struct Cyc_Yyltype* _temp6695= _temp6693.curr
+( yylsp_offset - 2); if( _temp6695 < _temp6693.base? 1: _temp6695 >= _temp6693.last_plus_one){
_throw( Null_Exception);}* _temp6695;}).first_line,({ struct _tagged_ptr3
_temp6696= yyls; struct Cyc_Yyltype* _temp6698= _temp6696.curr + yylsp_offset;
if( _temp6698 < _temp6696.base? 1: _temp6698 >= _temp6696.last_plus_one){ _throw(
Null_Exception);}* _temp6698;}).last_line)); _temp6689;}); _temp6688;}); break;
case 360: _LL6687: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6700=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6700[ 0]=({ struct Cyc_Exp_tok_struct _temp6701; _temp6701.tag= Cyc_Exp_tok_tag;
_temp6701.f1= Cyc_Absyn_unknowncall_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6702= yyvs; struct _xtunion_struct** _temp6704= _temp6702.curr
+( yyvsp_offset - 3); if( _temp6704 < _temp6702.base? 1: _temp6704 >= _temp6702.last_plus_one){
_throw( Null_Exception);}* _temp6704;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6705= yyvs; struct _xtunion_struct** _temp6707= _temp6705.curr
+( yyvsp_offset - 1); if( _temp6707 < _temp6705.base? 1: _temp6707 >= _temp6705.last_plus_one){
_throw( Null_Exception);}* _temp6707;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6708= yyls; struct Cyc_Yyltype* _temp6710= _temp6708.curr +(
yylsp_offset - 3); if( _temp6710 < _temp6708.base? 1: _temp6710 >= _temp6708.last_plus_one){
_throw( Null_Exception);}* _temp6710;}).first_line,({ struct _tagged_ptr3
_temp6711= yyls; struct Cyc_Yyltype* _temp6713= _temp6711.curr + yylsp_offset;
if( _temp6713 < _temp6711.base? 1: _temp6713 >= _temp6711.last_plus_one){ _throw(
Null_Exception);}* _temp6713;}).last_line)); _temp6701;}); _temp6700;}); break;
case 361: _LL6699: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6715=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6715[ 0]=({ struct Cyc_Exp_tok_struct _temp6716; _temp6716.tag= Cyc_Exp_tok_tag;
_temp6716.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6717= yyvs; struct _xtunion_struct** _temp6719= _temp6717.curr
+( yyvsp_offset - 2); if( _temp6719 < _temp6717.base? 1: _temp6719 >= _temp6717.last_plus_one){
_throw( Null_Exception);}* _temp6719;})),({ struct _tagged_string* _temp6720=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp6720[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6721= yyvs;
struct _xtunion_struct** _temp6723= _temp6721.curr + yyvsp_offset; if( _temp6723
< _temp6721.base? 1: _temp6723 >= _temp6721.last_plus_one){ _throw(
Null_Exception);}* _temp6723;})); _temp6720;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6724= yyls; struct Cyc_Yyltype* _temp6726= _temp6724.curr
+( yylsp_offset - 2); if( _temp6726 < _temp6724.base? 1: _temp6726 >= _temp6724.last_plus_one){
_throw( Null_Exception);}* _temp6726;}).first_line,({ struct _tagged_ptr3
_temp6727= yyls; struct Cyc_Yyltype* _temp6729= _temp6727.curr + yylsp_offset;
if( _temp6729 < _temp6727.base? 1: _temp6729 >= _temp6727.last_plus_one){ _throw(
Null_Exception);}* _temp6729;}).last_line)); _temp6716;}); _temp6715;}); break;
case 362: _LL6714: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6731= yyvs; struct _xtunion_struct** _temp6733= _temp6731.curr
+ yyvsp_offset; if( _temp6733 < _temp6731.base? 1: _temp6733 >= _temp6731.last_plus_one){
_throw( Null_Exception);}* _temp6733;}));{ void* _temp6734=(* q).f1; struct Cyc_List_List*
_temp6744; struct Cyc_List_List* _temp6746; _LL6736: if(( int) _temp6734 == Cyc_Absyn_Loc_n){
goto _LL6737;} else{ goto _LL6738;} _LL6738: if(( unsigned int) _temp6734 > 1u?((
struct _tunion_struct*) _temp6734)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6745:
_temp6744=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6734)->f1;
if( _temp6744 == 0){ goto _LL6739;} else{ goto _LL6740;}} else{ goto _LL6740;}
_LL6740: if(( unsigned int) _temp6734 > 1u?(( struct _tunion_struct*) _temp6734)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6747: _temp6746=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6734)->f1; if( _temp6746 == 0){ goto _LL6741;}
else{ goto _LL6742;}} else{ goto _LL6742;} _LL6742: goto _LL6743; _LL6737: goto
_LL6735; _LL6739: goto _LL6735; _LL6741: goto _LL6735; _LL6743: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6748=( char*)"struct field name is qualified";
struct _tagged_string _temp6749; _temp6749.curr= _temp6748; _temp6749.base=
_temp6748; _temp6749.last_plus_one= _temp6748 + 31; _temp6749;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6750= yyls; struct Cyc_Yyltype* _temp6752= _temp6750.curr
+ yylsp_offset; if( _temp6752 < _temp6750.base? 1: _temp6752 >= _temp6750.last_plus_one){
_throw( Null_Exception);}* _temp6752;}).first_line,({ struct _tagged_ptr3
_temp6753= yyls; struct Cyc_Yyltype* _temp6755= _temp6753.curr + yylsp_offset;
if( _temp6755 < _temp6753.base? 1: _temp6755 >= _temp6753.last_plus_one){ _throw(
Null_Exception);}* _temp6755;}).last_line)); goto _LL6735; _LL6735:;} yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6756=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6756[ 0]=({ struct Cyc_Exp_tok_struct
_temp6757; _temp6757.tag= Cyc_Exp_tok_tag; _temp6757.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6758= yyvs; struct _xtunion_struct**
_temp6760= _temp6758.curr +( yyvsp_offset - 2); if( _temp6760 < _temp6758.base?
1: _temp6760 >= _temp6758.last_plus_one){ _throw( Null_Exception);}* _temp6760;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6761= yyls;
struct Cyc_Yyltype* _temp6763= _temp6761.curr +( yylsp_offset - 2); if(
_temp6763 < _temp6761.base? 1: _temp6763 >= _temp6761.last_plus_one){ _throw(
Null_Exception);}* _temp6763;}).first_line,({ struct _tagged_ptr3 _temp6764=
yyls; struct Cyc_Yyltype* _temp6766= _temp6764.curr + yylsp_offset; if(
_temp6766 < _temp6764.base? 1: _temp6766 >= _temp6764.last_plus_one){ _throw(
Null_Exception);}* _temp6766;}).last_line)); _temp6757;}); _temp6756;}); break;}
case 363: _LL6730: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6768=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6768[ 0]=({ struct Cyc_Exp_tok_struct _temp6769; _temp6769.tag= Cyc_Exp_tok_tag;
_temp6769.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6770= yyvs; struct _xtunion_struct** _temp6772= _temp6770.curr
+( yyvsp_offset - 2); if( _temp6772 < _temp6770.base? 1: _temp6772 >= _temp6770.last_plus_one){
_throw( Null_Exception);}* _temp6772;})),({ struct _tagged_string* _temp6773=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp6773[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6774= yyvs;
struct _xtunion_struct** _temp6776= _temp6774.curr + yyvsp_offset; if( _temp6776
< _temp6774.base? 1: _temp6776 >= _temp6774.last_plus_one){ _throw(
Null_Exception);}* _temp6776;})); _temp6773;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6777= yyls; struct Cyc_Yyltype* _temp6779= _temp6777.curr
+( yylsp_offset - 2); if( _temp6779 < _temp6777.base? 1: _temp6779 >= _temp6777.last_plus_one){
_throw( Null_Exception);}* _temp6779;}).first_line,({ struct _tagged_ptr3
_temp6780= yyls; struct Cyc_Yyltype* _temp6782= _temp6780.curr + yylsp_offset;
if( _temp6782 < _temp6780.base? 1: _temp6782 >= _temp6780.last_plus_one){ _throw(
Null_Exception);}* _temp6782;}).last_line)); _temp6769;}); _temp6768;}); break;
case 364: _LL6767: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6784= yyvs; struct _xtunion_struct** _temp6786= _temp6784.curr
+ yyvsp_offset; if( _temp6786 < _temp6784.base? 1: _temp6786 >= _temp6784.last_plus_one){
_throw( Null_Exception);}* _temp6786;}));{ void* _temp6787=(* q).f1; struct Cyc_List_List*
_temp6797; struct Cyc_List_List* _temp6799; _LL6789: if(( int) _temp6787 == Cyc_Absyn_Loc_n){
goto _LL6790;} else{ goto _LL6791;} _LL6791: if(( unsigned int) _temp6787 > 1u?((
struct _tunion_struct*) _temp6787)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6798:
_temp6797=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6787)->f1;
if( _temp6797 == 0){ goto _LL6792;} else{ goto _LL6793;}} else{ goto _LL6793;}
_LL6793: if(( unsigned int) _temp6787 > 1u?(( struct _tunion_struct*) _temp6787)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6800: _temp6799=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6787)->f1; if( _temp6799 == 0){ goto _LL6794;}
else{ goto _LL6795;}} else{ goto _LL6795;} _LL6795: goto _LL6796; _LL6790: goto
_LL6788; _LL6792: goto _LL6788; _LL6794: goto _LL6788; _LL6796: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6801=( char*)"struct field is qualified with module name";
struct _tagged_string _temp6802; _temp6802.curr= _temp6801; _temp6802.base=
_temp6801; _temp6802.last_plus_one= _temp6801 + 43; _temp6802;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6803= yyls; struct Cyc_Yyltype* _temp6805= _temp6803.curr
+ yylsp_offset; if( _temp6805 < _temp6803.base? 1: _temp6805 >= _temp6803.last_plus_one){
_throw( Null_Exception);}* _temp6805;}).first_line,({ struct _tagged_ptr3
_temp6806= yyls; struct Cyc_Yyltype* _temp6808= _temp6806.curr + yylsp_offset;
if( _temp6808 < _temp6806.base? 1: _temp6808 >= _temp6806.last_plus_one){ _throw(
Null_Exception);}* _temp6808;}).last_line)); goto _LL6788; _LL6788:;} yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6809=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6809[ 0]=({ struct Cyc_Exp_tok_struct
_temp6810; _temp6810.tag= Cyc_Exp_tok_tag; _temp6810.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6811= yyvs; struct _xtunion_struct**
_temp6813= _temp6811.curr +( yyvsp_offset - 2); if( _temp6813 < _temp6811.base?
1: _temp6813 >= _temp6811.last_plus_one){ _throw( Null_Exception);}* _temp6813;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6814= yyls;
struct Cyc_Yyltype* _temp6816= _temp6814.curr +( yylsp_offset - 2); if(
_temp6816 < _temp6814.base? 1: _temp6816 >= _temp6814.last_plus_one){ _throw(
Null_Exception);}* _temp6816;}).first_line,({ struct _tagged_ptr3 _temp6817=
yyls; struct Cyc_Yyltype* _temp6819= _temp6817.curr + yylsp_offset; if(
_temp6819 < _temp6817.base? 1: _temp6819 >= _temp6817.last_plus_one){ _throw(
Null_Exception);}* _temp6819;}).last_line)); _temp6810;}); _temp6809;}); break;}
case 365: _LL6783: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6821=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6821[ 0]=({ struct Cyc_Exp_tok_struct _temp6822; _temp6822.tag= Cyc_Exp_tok_tag;
_temp6822.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6823= yyvs; struct _xtunion_struct** _temp6825= _temp6823.curr +(
yyvsp_offset - 1); if( _temp6825 < _temp6823.base? 1: _temp6825 >= _temp6823.last_plus_one){
_throw( Null_Exception);}* _temp6825;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6826= yyls; struct Cyc_Yyltype* _temp6828= _temp6826.curr +(
yylsp_offset - 1); if( _temp6828 < _temp6826.base? 1: _temp6828 >= _temp6826.last_plus_one){
_throw( Null_Exception);}* _temp6828;}).first_line,({ struct _tagged_ptr3
_temp6829= yyls; struct Cyc_Yyltype* _temp6831= _temp6829.curr + yylsp_offset;
if( _temp6831 < _temp6829.base? 1: _temp6831 >= _temp6829.last_plus_one){ _throw(
Null_Exception);}* _temp6831;}).last_line)); _temp6822;}); _temp6821;}); break;
case 366: _LL6820: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6833=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6833[ 0]=({ struct Cyc_Exp_tok_struct _temp6834; _temp6834.tag= Cyc_Exp_tok_tag;
_temp6834.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6835= yyvs; struct _xtunion_struct** _temp6837= _temp6835.curr +(
yyvsp_offset - 1); if( _temp6837 < _temp6835.base? 1: _temp6837 >= _temp6835.last_plus_one){
_throw( Null_Exception);}* _temp6837;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6838= yyls; struct Cyc_Yyltype* _temp6840= _temp6838.curr +(
yylsp_offset - 1); if( _temp6840 < _temp6838.base? 1: _temp6840 >= _temp6838.last_plus_one){
_throw( Null_Exception);}* _temp6840;}).first_line,({ struct _tagged_ptr3
_temp6841= yyls; struct Cyc_Yyltype* _temp6843= _temp6841.curr + yylsp_offset;
if( _temp6843 < _temp6841.base? 1: _temp6843 >= _temp6841.last_plus_one){ _throw(
Null_Exception);}* _temp6843;}).last_line)); _temp6834;}); _temp6833;}); break;
case 367: _LL6832: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6845=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6845[ 0]=({ struct Cyc_Exp_tok_struct _temp6846; _temp6846.tag= Cyc_Exp_tok_tag;
_temp6846.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_CompoundLit_e_struct*
_temp6847=( struct Cyc_Absyn_CompoundLit_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_CompoundLit_e_struct)); _temp6847[ 0]=({ struct Cyc_Absyn_CompoundLit_e_struct
_temp6848; _temp6848.tag= Cyc_Absyn_CompoundLit_e_tag; _temp6848.f1= Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp6849= yyvs; struct _xtunion_struct** _temp6851=
_temp6849.curr +( yyvsp_offset - 4); if( _temp6851 < _temp6849.base? 1:
_temp6851 >= _temp6849.last_plus_one){ _throw( Null_Exception);}* _temp6851;}));
_temp6848.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp6852= yyvs; struct
_xtunion_struct** _temp6854= _temp6852.curr +( yyvsp_offset - 1); if( _temp6854
< _temp6852.base? 1: _temp6854 >= _temp6852.last_plus_one){ _throw(
Null_Exception);}* _temp6854;}))); _temp6848;}); _temp6847;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6855= yyls; struct Cyc_Yyltype* _temp6857= _temp6855.curr
+( yylsp_offset - 5); if( _temp6857 < _temp6855.base? 1: _temp6857 >= _temp6855.last_plus_one){
_throw( Null_Exception);}* _temp6857;}).first_line,({ struct _tagged_ptr3
_temp6858= yyls; struct Cyc_Yyltype* _temp6860= _temp6858.curr + yylsp_offset;
if( _temp6860 < _temp6858.base? 1: _temp6860 >= _temp6858.last_plus_one){ _throw(
Null_Exception);}* _temp6860;}).last_line)); _temp6846;}); _temp6845;}); break;
case 368: _LL6844: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6862=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6862[ 0]=({ struct Cyc_Exp_tok_struct _temp6863; _temp6863.tag= Cyc_Exp_tok_tag;
_temp6863.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_CompoundLit_e_struct*
_temp6864=( struct Cyc_Absyn_CompoundLit_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_CompoundLit_e_struct)); _temp6864[ 0]=({ struct Cyc_Absyn_CompoundLit_e_struct
_temp6865; _temp6865.tag= Cyc_Absyn_CompoundLit_e_tag; _temp6865.f1= Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp6866= yyvs; struct _xtunion_struct** _temp6868=
_temp6866.curr +( yyvsp_offset - 5); if( _temp6868 < _temp6866.base? 1:
_temp6868 >= _temp6866.last_plus_one){ _throw( Null_Exception);}* _temp6868;}));
_temp6865.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp6869= yyvs; struct
_xtunion_struct** _temp6871= _temp6869.curr +( yyvsp_offset - 2); if( _temp6871
< _temp6869.base? 1: _temp6871 >= _temp6869.last_plus_one){ _throw(
Null_Exception);}* _temp6871;}))); _temp6865;}); _temp6864;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6872= yyls; struct Cyc_Yyltype* _temp6874= _temp6872.curr
+( yylsp_offset - 6); if( _temp6874 < _temp6872.base? 1: _temp6874 >= _temp6872.last_plus_one){
_throw( Null_Exception);}* _temp6874;}).first_line,({ struct _tagged_ptr3
_temp6875= yyls; struct Cyc_Yyltype* _temp6877= _temp6875.curr + yylsp_offset;
if( _temp6877 < _temp6875.base? 1: _temp6877 >= _temp6875.last_plus_one){ _throw(
Null_Exception);}* _temp6877;}).last_line)); _temp6863;}); _temp6862;}); break;
case 369: _LL6861: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6879=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6879[ 0]=({ struct Cyc_Exp_tok_struct _temp6880; _temp6880.tag= Cyc_Exp_tok_tag;
_temp6880.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Fill_e_struct*
_temp6881=( struct Cyc_Absyn_Fill_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct));
_temp6881[ 0]=({ struct Cyc_Absyn_Fill_e_struct _temp6882; _temp6882.tag= Cyc_Absyn_Fill_e_tag;
_temp6882.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6883= yyvs; struct
_xtunion_struct** _temp6885= _temp6883.curr +( yyvsp_offset - 1); if( _temp6885
< _temp6883.base? 1: _temp6885 >= _temp6883.last_plus_one){ _throw(
Null_Exception);}* _temp6885;})); _temp6882;}); _temp6881;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6886= yyls; struct Cyc_Yyltype* _temp6888= _temp6886.curr
+( yylsp_offset - 3); if( _temp6888 < _temp6886.base? 1: _temp6888 >= _temp6886.last_plus_one){
_throw( Null_Exception);}* _temp6888;}).first_line,({ struct _tagged_ptr3
_temp6889= yyls; struct Cyc_Yyltype* _temp6891= _temp6889.curr + yylsp_offset;
if( _temp6891 < _temp6889.base? 1: _temp6891 >= _temp6889.last_plus_one){ _throw(
Null_Exception);}* _temp6891;}).last_line)); _temp6880;}); _temp6879;}); break;
case 370: _LL6878: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6893=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6893[ 0]=({ struct Cyc_Exp_tok_struct _temp6894; _temp6894.tag= Cyc_Exp_tok_tag;
_temp6894.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Codegen_e_struct*
_temp6895=( struct Cyc_Absyn_Codegen_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct));
_temp6895[ 0]=({ struct Cyc_Absyn_Codegen_e_struct _temp6896; _temp6896.tag= Cyc_Absyn_Codegen_e_tag;
_temp6896.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp6897= yyvs;
struct _xtunion_struct** _temp6899= _temp6897.curr +( yyvsp_offset - 1); if(
_temp6899 < _temp6897.base? 1: _temp6899 >= _temp6897.last_plus_one){ _throw(
Null_Exception);}* _temp6899;})); _temp6896;}); _temp6895;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6900= yyls; struct Cyc_Yyltype* _temp6902= _temp6900.curr
+( yylsp_offset - 3); if( _temp6902 < _temp6900.base? 1: _temp6902 >= _temp6900.last_plus_one){
_throw( Null_Exception);}* _temp6902;}).first_line,({ struct _tagged_ptr3
_temp6903= yyls; struct Cyc_Yyltype* _temp6905= _temp6903.curr + yylsp_offset;
if( _temp6905 < _temp6903.base? 1: _temp6905 >= _temp6903.last_plus_one){ _throw(
Null_Exception);}* _temp6905;}).last_line)); _temp6894;}); _temp6893;}); break;
case 371: _LL6892: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6907=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6907[ 0]=({ struct Cyc_Exp_tok_struct _temp6908; _temp6908.tag= Cyc_Exp_tok_tag;
_temp6908.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp6909=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp6909[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp6910; _temp6910.tag=
Cyc_Absyn_UnknownId_e_tag; _temp6910.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6911= yyvs; struct _xtunion_struct** _temp6913= _temp6911.curr
+ yyvsp_offset; if( _temp6913 < _temp6911.base? 1: _temp6913 >= _temp6911.last_plus_one){
_throw( Null_Exception);}* _temp6913;})); _temp6910;}); _temp6909;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6914= yyls; struct Cyc_Yyltype* _temp6916= _temp6914.curr
+ yylsp_offset; if( _temp6916 < _temp6914.base? 1: _temp6916 >= _temp6914.last_plus_one){
_throw( Null_Exception);}* _temp6916;}).first_line,({ struct _tagged_ptr3
_temp6917= yyls; struct Cyc_Yyltype* _temp6919= _temp6917.curr + yylsp_offset;
if( _temp6919 < _temp6917.base? 1: _temp6919 >= _temp6917.last_plus_one){ _throw(
Null_Exception);}* _temp6919;}).last_line)); _temp6908;}); _temp6907;}); break;
case 372: _LL6906: yyval=({ struct _tagged_ptr2 _temp6921= yyvs; struct
_xtunion_struct** _temp6923= _temp6921.curr + yyvsp_offset; if( _temp6923 <
_temp6921.base? 1: _temp6923 >= _temp6921.last_plus_one){ _throw( Null_Exception);}*
_temp6923;}); break; case 373: _LL6920: yyval=( struct _xtunion_struct*)({
struct Cyc_Exp_tok_struct* _temp6925=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6925[ 0]=({ struct Cyc_Exp_tok_struct
_temp6926; _temp6926.tag= Cyc_Exp_tok_tag; _temp6926.f1= Cyc_Absyn_string_exp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6927= yyvs; struct
_xtunion_struct** _temp6929= _temp6927.curr + yyvsp_offset; if( _temp6929 <
_temp6927.base? 1: _temp6929 >= _temp6927.last_plus_one){ _throw( Null_Exception);}*
_temp6929;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6930=
yyls; struct Cyc_Yyltype* _temp6932= _temp6930.curr + yylsp_offset; if(
_temp6932 < _temp6930.base? 1: _temp6932 >= _temp6930.last_plus_one){ _throw(
Null_Exception);}* _temp6932;}).first_line,({ struct _tagged_ptr3 _temp6933=
yyls; struct Cyc_Yyltype* _temp6935= _temp6933.curr + yylsp_offset; if(
_temp6935 < _temp6933.base? 1: _temp6935 >= _temp6933.last_plus_one){ _throw(
Null_Exception);}* _temp6935;}).last_line)); _temp6926;}); _temp6925;}); break;
case 374: _LL6924: yyval=({ struct _tagged_ptr2 _temp6937= yyvs; struct
_xtunion_struct** _temp6939= _temp6937.curr +( yyvsp_offset - 1); if( _temp6939
< _temp6937.base? 1: _temp6939 >= _temp6937.last_plus_one){ _throw(
Null_Exception);}* _temp6939;}); break; case 375: _LL6936: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6941=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6941[ 0]=({ struct Cyc_Exp_tok_struct
_temp6942; _temp6942.tag= Cyc_Exp_tok_tag; _temp6942.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp6943=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp6943[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp6944; _temp6944.tag=
Cyc_Absyn_UnknownId_e_tag; _temp6944.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6945= yyvs; struct _xtunion_struct** _temp6947= _temp6945.curr
+( yyvsp_offset - 1); if( _temp6947 < _temp6945.base? 1: _temp6947 >= _temp6945.last_plus_one){
_throw( Null_Exception);}* _temp6947;})); _temp6944;}); _temp6943;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6948= yyls; struct Cyc_Yyltype* _temp6950= _temp6948.curr
+( yylsp_offset - 1); if( _temp6950 < _temp6948.base? 1: _temp6950 >= _temp6948.last_plus_one){
_throw( Null_Exception);}* _temp6950;}).first_line,({ struct _tagged_ptr3
_temp6951= yyls; struct Cyc_Yyltype* _temp6953= _temp6951.curr +( yylsp_offset -
1); if( _temp6953 < _temp6951.base? 1: _temp6953 >= _temp6951.last_plus_one){
_throw( Null_Exception);}* _temp6953;}).last_line)), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6954= yyls; struct Cyc_Yyltype* _temp6956= _temp6954.curr
+( yylsp_offset - 1); if( _temp6956 < _temp6954.base? 1: _temp6956 >= _temp6954.last_plus_one){
_throw( Null_Exception);}* _temp6956;}).first_line,({ struct _tagged_ptr3
_temp6957= yyls; struct Cyc_Yyltype* _temp6959= _temp6957.curr + yylsp_offset;
if( _temp6959 < _temp6957.base? 1: _temp6959 >= _temp6957.last_plus_one){ _throw(
Null_Exception);}* _temp6959;}).last_line)); _temp6942;}); _temp6941;}); break;
case 376: _LL6940: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6961=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6961[ 0]=({ struct Cyc_Exp_tok_struct _temp6962; _temp6962.tag= Cyc_Exp_tok_tag;
_temp6962.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp6963=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp6963[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp6964; _temp6964.tag=
Cyc_Absyn_UnknownId_e_tag; _temp6964.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6965= yyvs; struct _xtunion_struct** _temp6967= _temp6965.curr
+( yyvsp_offset - 4); if( _temp6967 < _temp6965.base? 1: _temp6967 >= _temp6965.last_plus_one){
_throw( Null_Exception);}* _temp6967;})); _temp6964;}); _temp6963;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6968= yyls; struct Cyc_Yyltype* _temp6970= _temp6968.curr
+( yylsp_offset - 4); if( _temp6970 < _temp6968.base? 1: _temp6970 >= _temp6968.last_plus_one){
_throw( Null_Exception);}* _temp6970;}).first_line,({ struct _tagged_ptr3
_temp6971= yyls; struct Cyc_Yyltype* _temp6973= _temp6971.curr +( yylsp_offset -
4); if( _temp6973 < _temp6971.base? 1: _temp6973 >= _temp6971.last_plus_one){
_throw( Null_Exception);}* _temp6973;}).last_line)),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp6974= yyvs; struct _xtunion_struct** _temp6976= _temp6974.curr
+( yyvsp_offset - 1); if( _temp6976 < _temp6974.base? 1: _temp6976 >= _temp6974.last_plus_one){
_throw( Null_Exception);}* _temp6976;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6977= yyls; struct Cyc_Yyltype* _temp6979= _temp6977.curr +(
yylsp_offset - 4); if( _temp6979 < _temp6977.base? 1: _temp6979 >= _temp6977.last_plus_one){
_throw( Null_Exception);}* _temp6979;}).first_line,({ struct _tagged_ptr3
_temp6980= yyls; struct Cyc_Yyltype* _temp6982= _temp6980.curr + yylsp_offset;
if( _temp6982 < _temp6980.base? 1: _temp6982 >= _temp6980.last_plus_one){ _throw(
Null_Exception);}* _temp6982;}).last_line)); _temp6962;}); _temp6961;}); break;
case 377: _LL6960: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6984=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6984[ 0]=({ struct Cyc_Exp_tok_struct _temp6985; _temp6985.tag= Cyc_Exp_tok_tag;
_temp6985.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp6986= yyvs; struct _xtunion_struct** _temp6988= _temp6986.curr +(
yyvsp_offset - 1); if( _temp6988 < _temp6986.base? 1: _temp6988 >= _temp6986.last_plus_one){
_throw( Null_Exception);}* _temp6988;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6989= yyls; struct Cyc_Yyltype* _temp6991= _temp6989.curr +(
yylsp_offset - 3); if( _temp6991 < _temp6989.base? 1: _temp6991 >= _temp6989.last_plus_one){
_throw( Null_Exception);}* _temp6991;}).first_line,({ struct _tagged_ptr3
_temp6992= yyls; struct Cyc_Yyltype* _temp6994= _temp6992.curr + yylsp_offset;
if( _temp6994 < _temp6992.base? 1: _temp6994 >= _temp6992.last_plus_one){ _throw(
Null_Exception);}* _temp6994;}).last_line)); _temp6985;}); _temp6984;}); break;
case 378: _LL6983: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6996=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6996[ 0]=({ struct Cyc_Exp_tok_struct _temp6997; _temp6997.tag= Cyc_Exp_tok_tag;
_temp6997.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp6998=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp6998[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp6999; _temp6999.tag= Cyc_Absyn_Struct_e_tag;
_temp6999.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7000= yyvs;
struct _xtunion_struct** _temp7002= _temp7000.curr +( yyvsp_offset - 3); if(
_temp7002 < _temp7000.base? 1: _temp7002 >= _temp7000.last_plus_one){ _throw(
Null_Exception);}* _temp7002;})); _temp6999.f2= 0; _temp6999.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7003= yyvs; struct _xtunion_struct** _temp7005=
_temp7003.curr +( yyvsp_offset - 1); if( _temp7005 < _temp7003.base? 1:
_temp7005 >= _temp7003.last_plus_one){ _throw( Null_Exception);}* _temp7005;})));
_temp6999.f4= 0; _temp6999;}); _temp6998;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7006= yyls; struct Cyc_Yyltype* _temp7008= _temp7006.curr
+( yylsp_offset - 3); if( _temp7008 < _temp7006.base? 1: _temp7008 >= _temp7006.last_plus_one){
_throw( Null_Exception);}* _temp7008;}).first_line,({ struct _tagged_ptr3
_temp7009= yyls; struct Cyc_Yyltype* _temp7011= _temp7009.curr + yylsp_offset;
if( _temp7011 < _temp7009.base? 1: _temp7011 >= _temp7009.last_plus_one){ _throw(
Null_Exception);}* _temp7011;}).last_line)); _temp6997;}); _temp6996;}); break;
case 379: _LL6995: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7013=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7013[ 0]=({ struct Cyc_Exp_tok_struct _temp7014; _temp7014.tag= Cyc_Exp_tok_tag;
_temp7014.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp7015= yyvs; struct _xtunion_struct** _temp7017= _temp7015.curr +(
yyvsp_offset - 2); if( _temp7017 < _temp7015.base? 1: _temp7017 >= _temp7015.last_plus_one){
_throw( Null_Exception);}* _temp7017;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7018= yyls; struct Cyc_Yyltype* _temp7020= _temp7018.curr +(
yylsp_offset - 4); if( _temp7020 < _temp7018.base? 1: _temp7020 >= _temp7018.last_plus_one){
_throw( Null_Exception);}* _temp7020;}).first_line,({ struct _tagged_ptr3
_temp7021= yyls; struct Cyc_Yyltype* _temp7023= _temp7021.curr + yylsp_offset;
if( _temp7023 < _temp7021.base? 1: _temp7023 >= _temp7021.last_plus_one){ _throw(
Null_Exception);}* _temp7023;}).last_line)); _temp7014;}); _temp7013;}); break;
case 380: _LL7012: yyval=( struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct*
_temp7025=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7025[ 0]=({ struct Cyc_ExpList_tok_struct _temp7026; _temp7026.tag= Cyc_ExpList_tok_tag;
_temp7026.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7027= yyvs; struct
_xtunion_struct** _temp7029= _temp7027.curr + yyvsp_offset; if( _temp7029 <
_temp7027.base? 1: _temp7029 >= _temp7027.last_plus_one){ _throw( Null_Exception);}*
_temp7029;}))); _temp7026;}); _temp7025;}); break; case 381: _LL7024: yyval=(
struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct* _temp7031=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7031[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7032; _temp7032.tag= Cyc_ExpList_tok_tag; _temp7032.f1=({ struct Cyc_List_List*
_temp7033=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7033->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7034= yyvs;
struct _xtunion_struct** _temp7036= _temp7034.curr + yyvsp_offset; if( _temp7036
< _temp7034.base? 1: _temp7036 >= _temp7034.last_plus_one){ _throw(
Null_Exception);}* _temp7036;})); _temp7033->tl= 0; _temp7033;}); _temp7032;});
_temp7031;}); break; case 382: _LL7030: yyval=( struct _xtunion_struct*)({
struct Cyc_ExpList_tok_struct* _temp7038=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7038[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7039; _temp7039.tag= Cyc_ExpList_tok_tag; _temp7039.f1=({ struct Cyc_List_List*
_temp7040=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7040->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7044= yyvs;
struct _xtunion_struct** _temp7046= _temp7044.curr + yyvsp_offset; if( _temp7046
< _temp7044.base? 1: _temp7046 >= _temp7044.last_plus_one){ _throw(
Null_Exception);}* _temp7046;})); _temp7040->tl= Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp7041= yyvs; struct _xtunion_struct** _temp7043= _temp7041.curr
+( yyvsp_offset - 2); if( _temp7043 < _temp7041.base? 1: _temp7043 >= _temp7041.last_plus_one){
_throw( Null_Exception);}* _temp7043;})); _temp7040;}); _temp7039;}); _temp7038;});
break; case 383: _LL7037: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7048=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7048[ 0]=({ struct Cyc_Exp_tok_struct _temp7049; _temp7049.tag= Cyc_Exp_tok_tag;
_temp7049.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp7050= yyvs; struct _xtunion_struct** _temp7052= _temp7050.curr +
yyvsp_offset; if( _temp7052 < _temp7050.base? 1: _temp7052 >= _temp7050.last_plus_one){
_throw( Null_Exception);}* _temp7052;}))).f1,(* Cyc_yyget_Int_tok(({ struct
_tagged_ptr2 _temp7053= yyvs; struct _xtunion_struct** _temp7055= _temp7053.curr
+ yyvsp_offset; if( _temp7055 < _temp7053.base? 1: _temp7055 >= _temp7053.last_plus_one){
_throw( Null_Exception);}* _temp7055;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7056= yyls; struct Cyc_Yyltype* _temp7058= _temp7056.curr
+ yylsp_offset; if( _temp7058 < _temp7056.base? 1: _temp7058 >= _temp7056.last_plus_one){
_throw( Null_Exception);}* _temp7058;}).first_line,({ struct _tagged_ptr3
_temp7059= yyls; struct Cyc_Yyltype* _temp7061= _temp7059.curr + yylsp_offset;
if( _temp7061 < _temp7059.base? 1: _temp7061 >= _temp7059.last_plus_one){ _throw(
Null_Exception);}* _temp7061;}).last_line)); _temp7049;}); _temp7048;}); break;
case 384: _LL7047: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7063=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7063[ 0]=({ struct Cyc_Exp_tok_struct _temp7064; _temp7064.tag= Cyc_Exp_tok_tag;
_temp7064.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(({ struct _tagged_ptr2
_temp7065= yyvs; struct _xtunion_struct** _temp7067= _temp7065.curr +
yyvsp_offset; if( _temp7067 < _temp7065.base? 1: _temp7067 >= _temp7065.last_plus_one){
_throw( Null_Exception);}* _temp7067;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7068= yyls; struct Cyc_Yyltype* _temp7070= _temp7068.curr +
yylsp_offset; if( _temp7070 < _temp7068.base? 1: _temp7070 >= _temp7068.last_plus_one){
_throw( Null_Exception);}* _temp7070;}).first_line,({ struct _tagged_ptr3
_temp7071= yyls; struct Cyc_Yyltype* _temp7073= _temp7071.curr + yylsp_offset;
if( _temp7073 < _temp7071.base? 1: _temp7073 >= _temp7071.last_plus_one){ _throw(
Null_Exception);}* _temp7073;}).last_line)); _temp7064;}); _temp7063;}); break;
case 385: _LL7062: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7075=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7075[ 0]=({ struct Cyc_Exp_tok_struct _temp7076; _temp7076.tag= Cyc_Exp_tok_tag;
_temp7076.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7077= yyvs; struct _xtunion_struct** _temp7079= _temp7077.curr +
yyvsp_offset; if( _temp7079 < _temp7077.base? 1: _temp7079 >= _temp7077.last_plus_one){
_throw( Null_Exception);}* _temp7079;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7080= yyls; struct Cyc_Yyltype* _temp7082= _temp7080.curr +
yylsp_offset; if( _temp7082 < _temp7080.base? 1: _temp7082 >= _temp7080.last_plus_one){
_throw( Null_Exception);}* _temp7082;}).first_line,({ struct _tagged_ptr3
_temp7083= yyls; struct Cyc_Yyltype* _temp7085= _temp7083.curr + yylsp_offset;
if( _temp7085 < _temp7083.base? 1: _temp7085 >= _temp7083.last_plus_one){ _throw(
Null_Exception);}* _temp7085;}).last_line)); _temp7076;}); _temp7075;}); break;
case 386: _LL7074: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7087=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7087[ 0]=({ struct Cyc_Exp_tok_struct _temp7088; _temp7088.tag= Cyc_Exp_tok_tag;
_temp7088.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7089= yyls; struct Cyc_Yyltype* _temp7091= _temp7089.curr +
yylsp_offset; if( _temp7091 < _temp7089.base? 1: _temp7091 >= _temp7089.last_plus_one){
_throw( Null_Exception);}* _temp7091;}).first_line,({ struct _tagged_ptr3
_temp7092= yyls; struct Cyc_Yyltype* _temp7094= _temp7092.curr + yylsp_offset;
if( _temp7094 < _temp7092.base? 1: _temp7094 >= _temp7092.last_plus_one){ _throw(
Null_Exception);}* _temp7094;}).last_line)); _temp7088;}); _temp7087;}); break;
case 387: _LL7086: yyval=( struct _xtunion_struct*)({ struct Cyc_QualId_tok_struct*
_temp7096=( struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp7096[ 0]=({ struct Cyc_QualId_tok_struct _temp7097; _temp7097.tag= Cyc_QualId_tok_tag;
_temp7097.f1=({ struct _tuple1* _temp7098=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp7098->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp7103=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp7103[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp7104; _temp7104.tag= Cyc_Absyn_Rel_n_tag;
_temp7104.f1= 0; _temp7104;}); _temp7103;}); _temp7098->f2=({ struct
_tagged_string* _temp7099=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp7099[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7100= yyvs; struct _xtunion_struct** _temp7102= _temp7100.curr +
yyvsp_offset; if( _temp7102 < _temp7100.base? 1: _temp7102 >= _temp7100.last_plus_one){
_throw( Null_Exception);}* _temp7102;})); _temp7099;}); _temp7098;}); _temp7097;});
_temp7096;}); break; case 388: _LL7095: yyval=({ struct _tagged_ptr2 _temp7106=
yyvs; struct _xtunion_struct** _temp7108= _temp7106.curr + yyvsp_offset; if(
_temp7108 < _temp7106.base? 1: _temp7108 >= _temp7106.last_plus_one){ _throw(
Null_Exception);}* _temp7108;}); break; default: _LL7105:( void) _throw(( struct
_xtunion_struct*)({ struct Cyc_Yyimpossible_struct* _temp7110=( struct Cyc_Yyimpossible_struct*)
GC_malloc( sizeof( struct Cyc_Yyimpossible_struct)); _temp7110[ 0]=({ struct Cyc_Yyimpossible_struct
_temp7111; _temp7111.tag= Cyc_Yyimpossible_tag; _temp7111;}); _temp7110;}));}
yyvsp_offset -= yylen; yyssp_offset -= yylen; yylsp_offset -= yylen;({ struct
_tagged_ptr2 _temp7112= yyvs; struct _xtunion_struct** _temp7114= _temp7112.curr
+( ++ yyvsp_offset); if( _temp7114 < _temp7112.base? 1: _temp7114 >= _temp7112.last_plus_one){
_throw( Null_Exception);}* _temp7114= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp7115= yyls; struct Cyc_Yyltype* _temp7117= _temp7115.curr
+ yylsp_offset; if( _temp7117 < _temp7115.base? 1: _temp7117 >= _temp7115.last_plus_one){
_throw( Null_Exception);}(* _temp7117).first_line= Cyc_yylloc.first_line;});({
struct _tagged_ptr3 _temp7118= yyls; struct Cyc_Yyltype* _temp7120= _temp7118.curr
+ yylsp_offset; if( _temp7120 < _temp7118.base? 1: _temp7120 >= _temp7118.last_plus_one){
_throw( Null_Exception);}(* _temp7120).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp7121= yyls; struct Cyc_Yyltype* _temp7123= _temp7121.curr
+ yylsp_offset; if( _temp7123 < _temp7121.base? 1: _temp7123 >= _temp7121.last_plus_one){
_throw( Null_Exception);}(* _temp7123).last_line=({ struct _tagged_ptr3
_temp7124= yyls; struct Cyc_Yyltype* _temp7126= _temp7124.curr +( yylsp_offset -
1); if( _temp7126 < _temp7124.base? 1: _temp7126 >= _temp7124.last_plus_one){
_throw( Null_Exception);}* _temp7126;}).last_line;});({ struct _tagged_ptr3
_temp7127= yyls; struct Cyc_Yyltype* _temp7129= _temp7127.curr + yylsp_offset;
if( _temp7129 < _temp7127.base? 1: _temp7129 >= _temp7127.last_plus_one){ _throw(
Null_Exception);}(* _temp7129).last_column=({ struct _tagged_ptr3 _temp7130=
yyls; struct Cyc_Yyltype* _temp7132= _temp7130.curr +( yylsp_offset - 1); if(
_temp7132 < _temp7130.base? 1: _temp7132 >= _temp7130.last_plus_one){ _throw(
Null_Exception);}* _temp7132;}).last_column;});({ struct _tagged_ptr3 _temp7133=
yyls; struct Cyc_Yyltype* _temp7135= _temp7133.curr + yylsp_offset; if(
_temp7135 < _temp7133.base? 1: _temp7135 >= _temp7133.last_plus_one){ _throw(
Null_Exception);}(* _temp7135).text=( struct _tagged_string)({ char* _temp7136=(
char*)""; struct _tagged_string _temp7137; _temp7137.curr= _temp7136; _temp7137.base=
_temp7136; _temp7137.last_plus_one= _temp7136 + 1; _temp7137;});});} else{({
struct _tagged_ptr3 _temp7138= yyls; struct Cyc_Yyltype* _temp7140= _temp7138.curr
+ yylsp_offset; if( _temp7140 < _temp7138.base? 1: _temp7140 >= _temp7138.last_plus_one){
_throw( Null_Exception);}(* _temp7140).last_line=({ struct _tagged_ptr3
_temp7141= yyls; struct Cyc_Yyltype* _temp7143= _temp7141.curr +(( yylsp_offset
+ yylen) - 1); if( _temp7143 < _temp7141.base? 1: _temp7143 >= _temp7141.last_plus_one){
_throw( Null_Exception);}* _temp7143;}).last_line;});({ struct _tagged_ptr3
_temp7144= yyls; struct Cyc_Yyltype* _temp7146= _temp7144.curr + yylsp_offset;
if( _temp7146 < _temp7144.base? 1: _temp7146 >= _temp7144.last_plus_one){ _throw(
Null_Exception);}(* _temp7146).last_column=({ struct _tagged_ptr3 _temp7147=
yyls; struct Cyc_Yyltype* _temp7149= _temp7147.curr +(( yylsp_offset + yylen) -
1); if( _temp7149 < _temp7147.base? 1: _temp7149 >= _temp7147.last_plus_one){
_throw( Null_Exception);}* _temp7149;}).last_column;});} yyn=( int)({ short*
_temp7150=( short*) Cyc_yyr1; unsigned int _temp7151= yyn; if( _temp7151 >= 389u){
_throw( Null_Exception);} _temp7150[ _temp7151];}); yystate=( int)({ short*
_temp7152=( short*) Cyc_yypgoto; unsigned int _temp7153= yyn - 120; if(
_temp7153 >= 107u){ _throw( Null_Exception);} _temp7152[ _temp7153];}) +( int)({
struct _tagged_ptr1 _temp7154= yyss; short* _temp7156= _temp7154.curr +
yyssp_offset; if( _temp7156 < _temp7154.base? 1: _temp7156 >= _temp7154.last_plus_one){
_throw( Null_Exception);}* _temp7156;}); if(( yystate >= 0? yystate <= 3997: 0)?(
int)({ short* _temp7157=( short*) Cyc_yycheck; unsigned int _temp7158= yystate;
if( _temp7158 >= 3998u){ _throw( Null_Exception);} _temp7157[ _temp7158];}) ==(
int)({ struct _tagged_ptr1 _temp7159= yyss; short* _temp7161= _temp7159.curr +
yyssp_offset; if( _temp7161 < _temp7159.base? 1: _temp7161 >= _temp7159.last_plus_one){
_throw( Null_Exception);}* _temp7161;}): 0){ yystate=( int)({ short* _temp7162=(
short*) Cyc_yytable; unsigned int _temp7163= yystate; if( _temp7163 >= 3998u){
_throw( Null_Exception);} _temp7162[ _temp7163];});} else{ yystate=( int)({
short* _temp7164=( short*) Cyc_yydefgoto; unsigned int _temp7165= yyn - 120; if(
_temp7165 >= 107u){ _throw( Null_Exception);} _temp7164[ _temp7165];});} goto
yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short*
_temp7166=( short*) Cyc_yypact; unsigned int _temp7167= yystate; if( _temp7167
>= 728u){ _throw( Null_Exception);} _temp7166[ _temp7167];}); if( yyn > - 32768?
yyn < 3997: 0){ int sze= 0; struct _tagged_string msg; int x; int count; count=
0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 227u / sizeof( char*); x ++){
if(( int)({ short* _temp7168=( short*) Cyc_yycheck; unsigned int _temp7169= x +
yyn; if( _temp7169 >= 3998u){ _throw( Null_Exception);} _temp7168[ _temp7169];})
== x){( unsigned int)( sze += Cyc_String_strlen(({ struct _tagged_string*
_temp7170=( struct _tagged_string*) Cyc_yytname; unsigned int _temp7171= x; if(
_temp7171 >= 227u){ _throw( Null_Exception);} _temp7170[ _temp7171];})) +(
unsigned int) 15), count ++;}} msg=({ unsigned int _temp7172=( unsigned int)(
sze + 15); char* _temp7173=( char*) GC_malloc_atomic( sizeof( char) * _temp7172);
unsigned int i; struct _tagged_string _temp7174={ _temp7173, _temp7173,
_temp7173 + _temp7172}; for( i= 0; i < _temp7172; i ++){ _temp7173[ i]='\000';}
_temp7174;}); Cyc_String_strcpy( msg,( struct _tagged_string)({ char* _temp7175=(
char*)"parse error"; struct _tagged_string _temp7176; _temp7176.curr= _temp7175;
_temp7176.base= _temp7175; _temp7176.last_plus_one= _temp7175 + 12; _temp7176;}));
if( count < 5){ count= 0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 227u /
sizeof( char*); x ++){ if(( int)({ short* _temp7177=( short*) Cyc_yycheck;
unsigned int _temp7178= x + yyn; if( _temp7178 >= 3998u){ _throw( Null_Exception);}
_temp7177[ _temp7178];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp7179=( char*)", expecting `"; struct _tagged_string
_temp7180; _temp7180.curr= _temp7179; _temp7180.base= _temp7179; _temp7180.last_plus_one=
_temp7179 + 14; _temp7180;}):( struct _tagged_string)({ char* _temp7181=( char*)" or `";
struct _tagged_string _temp7182; _temp7182.curr= _temp7181; _temp7182.base=
_temp7181; _temp7182.last_plus_one= _temp7181 + 6; _temp7182;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp7183=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp7184= x; if( _temp7184 >= 227u){ _throw( Null_Exception);}
_temp7183[ _temp7184];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp7185=( char*)"'"; struct _tagged_string _temp7186; _temp7186.curr=
_temp7185; _temp7186.base= _temp7185; _temp7186.last_plus_one= _temp7185 + 2;
_temp7186;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp7187=( char*)"parse error"; struct _tagged_string
_temp7188; _temp7188.curr= _temp7187; _temp7188.base= _temp7187; _temp7188.last_plus_one=
_temp7187 + 12; _temp7188;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp7189= yyss; short*
_temp7191= _temp7189.curr +( -- yyssp_offset); if( _temp7191 < _temp7189.base? 1:
_temp7191 >= _temp7189.last_plus_one){ _throw( Null_Exception);}* _temp7191;});
yylsp_offset --; yyerrhandle: yyn=( int)({ short* _temp7192=( short*) Cyc_yypact;
unsigned int _temp7193= yystate; if( _temp7193 >= 728u){ _throw( Null_Exception);}
_temp7192[ _temp7193];}); if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if((
yyn < 0? 1: yyn > 3997)? 1:( int)({ short* _temp7194=( short*) Cyc_yycheck;
unsigned int _temp7195= yyn; if( _temp7195 >= 3998u){ _throw( Null_Exception);}
_temp7194[ _temp7195];}) != 1){ goto yyerrdefault;} yyn=( int)({ short*
_temp7196=( short*) Cyc_yytable; unsigned int _temp7197= yyn; if( _temp7197 >=
3998u){ _throw( Null_Exception);} _temp7196[ _temp7197];}); if( yyn < 0){ if(
yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn == 0){
goto yyerrpop;}} if( yyn == 727){ return 0;}({ struct _tagged_ptr2 _temp7198=
yyvs; struct _xtunion_struct** _temp7200= _temp7198.curr +( ++ yyvsp_offset);
if( _temp7200 < _temp7198.base? 1: _temp7200 >= _temp7198.last_plus_one){ _throw(
Null_Exception);}* _temp7200= Cyc_yylval;});({ struct _tagged_ptr3 _temp7201=
yyls; struct Cyc_Yyltype* _temp7203= _temp7201.curr +( ++ yylsp_offset); if(
_temp7203 < _temp7201.base? 1: _temp7203 >= _temp7201.last_plus_one){ _throw(
Null_Exception);}* _temp7203= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, struct _xtunion_struct* v){ struct _xtunion_struct* _temp7204= v; struct
_tuple12* _temp7224; struct _tuple12 _temp7226; int _temp7227; void* _temp7229;
char _temp7231; short _temp7233; struct _tagged_string _temp7235; struct Cyc_Core_Opt*
_temp7237; struct Cyc_Core_Opt* _temp7239; struct Cyc_Core_Opt _temp7241; struct
_tagged_string* _temp7242; struct _tuple1* _temp7244; struct _tuple1 _temp7246;
struct _tagged_string* _temp7247; void* _temp7249; _LL7206: if( _temp7204->tag
== Cyc_Okay_tok_tag){ goto _LL7207;} else{ goto _LL7208;} _LL7208: if((*((
struct _xtunion_struct*) _temp7204)).tag == Cyc_Int_tok_tag){ _LL7225: _temp7224=((
struct Cyc_Int_tok_struct*) _temp7204)->f1; _temp7226=* _temp7224; _LL7230:
_temp7229= _temp7226.f1; goto _LL7228; _LL7228: _temp7227= _temp7226.f2; goto
_LL7209;} else{ goto _LL7210;} _LL7210: if((*(( struct _xtunion_struct*)
_temp7204)).tag == Cyc_Char_tok_tag){ _LL7232: _temp7231=(( struct Cyc_Char_tok_struct*)
_temp7204)->f1; goto _LL7211;} else{ goto _LL7212;} _LL7212: if((*(( struct
_xtunion_struct*) _temp7204)).tag == Cyc_Short_tok_tag){ _LL7234: _temp7233=((
struct Cyc_Short_tok_struct*) _temp7204)->f1; goto _LL7213;} else{ goto _LL7214;}
_LL7214: if((*(( struct _xtunion_struct*) _temp7204)).tag == Cyc_String_tok_tag){
_LL7236: _temp7235=(( struct Cyc_String_tok_struct*) _temp7204)->f1; goto
_LL7215;} else{ goto _LL7216;} _LL7216: if((*(( struct _xtunion_struct*)
_temp7204)).tag == Cyc_Stringopt_tok_tag){ _LL7238: _temp7237=(( struct Cyc_Stringopt_tok_struct*)
_temp7204)->f1; if( _temp7237 == 0){ goto _LL7217;} else{ goto _LL7218;}} else{
goto _LL7218;} _LL7218: if((*(( struct _xtunion_struct*) _temp7204)).tag == Cyc_Stringopt_tok_tag){
_LL7240: _temp7239=(( struct Cyc_Stringopt_tok_struct*) _temp7204)->f1; if(
_temp7239 == 0){ goto _LL7220;} else{ _temp7241=* _temp7239; _LL7243: _temp7242=(
struct _tagged_string*) _temp7241.v; goto _LL7219;}} else{ goto _LL7220;}
_LL7220: if((*(( struct _xtunion_struct*) _temp7204)).tag == Cyc_QualId_tok_tag){
_LL7245: _temp7244=(( struct Cyc_QualId_tok_struct*) _temp7204)->f1; _temp7246=*
_temp7244; _LL7250: _temp7249= _temp7246.f1; goto _LL7248; _LL7248: _temp7247=
_temp7246.f2; goto _LL7221;} else{ goto _LL7222;} _LL7222: goto _LL7223; _LL7207:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL7205; _LL7209: fprintf( Cyc_Stdio_stderr,"%d",
_temp7227); goto _LL7205; _LL7211: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp7231); goto _LL7205; _LL7213: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp7233); goto _LL7205; _LL7215:({ struct _tagged_string _temp7251= _temp7235;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp7251.last_plus_one - _temp7251.curr,
_temp7251.curr);}); goto _LL7205; _LL7217: fprintf( Cyc_Stdio_stderr,"null");
goto _LL7205; _LL7219:({ struct _tagged_string _temp7252=* _temp7242; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp7252.last_plus_one - _temp7252.curr, _temp7252.curr);});
goto _LL7205; _LL7221: { struct Cyc_List_List* prefix= 0;{ void* _temp7253=
_temp7249; struct Cyc_List_List* _temp7261; struct Cyc_List_List* _temp7263;
_LL7255: if(( unsigned int) _temp7253 > 1u?(( struct _tunion_struct*) _temp7253)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL7262: _temp7261=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp7253)->f1; goto _LL7256;} else{ goto _LL7257;}
_LL7257: if(( unsigned int) _temp7253 > 1u?(( struct _tunion_struct*) _temp7253)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL7264: _temp7263=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp7253)->f1; goto _LL7258;} else{ goto _LL7259;}
_LL7259: if(( int) _temp7253 == Cyc_Absyn_Loc_n){ goto _LL7260;} else{ goto
_LL7254;} _LL7256: prefix= _temp7261; goto _LL7254; _LL7258: prefix= _temp7263;
goto _LL7254; _LL7260: goto _LL7254; _LL7254:;} for( 0; prefix != 0; prefix=
prefix->tl){({ struct _tagged_string _temp7265=*(( struct _tagged_string*)
prefix->hd); fprintf( Cyc_Stdio_stderr,"%.*s::", _temp7265.last_plus_one -
_temp7265.curr, _temp7265.curr);});}({ struct _tagged_string _temp7266=*
_temp7247; fprintf( Cyc_Stdio_stderr,"%.*s::", _temp7266.last_plus_one -
_temp7266.curr, _temp7266.curr);}); goto _LL7205;} _LL7223: fprintf( Cyc_Stdio_stderr,"?");
goto _LL7205; _LL7205:;} struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE*
f){ Cyc_Parse_parse_result= 0; Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp7267=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp7267->v=(
void*) Cyc_Lexing_from_file( f); _temp7267;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}