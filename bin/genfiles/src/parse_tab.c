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
struct Cyc_Absyn_Exp* f2; } ; typedef int Cyc_ptrdiff_t; typedef unsigned int
Cyc_size_t; typedef int Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef
char Cyc_u_char; typedef unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int;
typedef unsigned int Cyc_u_long; typedef unsigned short Cyc_ushort; typedef
unsigned int Cyc_uint; typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t;
struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t;
typedef char* Cyc_caddr_t; typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef unsigned int Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int
Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set
Cyc__types_fd_set; typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string;
typedef struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
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
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t;
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
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp43->v=( void*)(*(( struct _tuple1*)({
struct Cyc_Core_Opt* _temp44= _temp41; if( _temp44 == 0){ _throw( Null_Exception);}
_temp44->v;}))).f2; _temp43;});{ void* _temp46=(*(( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp45= _temp41; if( _temp45 == 0){ _throw( Null_Exception);}
_temp45->v;}))).f1; struct Cyc_List_List* _temp56; struct Cyc_List_List* _temp58;
_LL48: if(( unsigned int) _temp46 > 1u?(( struct _tunion_struct*) _temp46)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL57: _temp56=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp46)->f1; if( _temp56 == 0){ goto _LL49;} else{ goto _LL50;}} else{ goto
_LL50;} _LL50: if(( unsigned int) _temp46 > 1u?(( struct _tunion_struct*)
_temp46)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL59: _temp58=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp46)->f1; if( _temp58 == 0){ goto _LL51;}
else{ goto _LL52;}} else{ goto _LL52;} _LL52: if(( int) _temp46 == Cyc_Absyn_Loc_n){
goto _LL53;} else{ goto _LL54;} _LL54: goto _LL55; _LL49: goto _LL47; _LL51:
goto _LL47; _LL53: goto _LL47; _LL55: Cyc_Parse_err(( struct _tagged_string)({
char* _temp60=( char*)"parameter cannot be qualified with a module name"; struct
_tagged_string _temp61; _temp61.curr= _temp60; _temp61.base= _temp60; _temp61.last_plus_one=
_temp60 + 49; _temp61;}), loc); goto _LL47; _LL47:;}} if( _temp35 != 0){(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp62=( char*)"parameter should have no type parameters";
struct _tagged_string _temp63; _temp63.curr= _temp62; _temp63.base= _temp62;
_temp63.last_plus_one= _temp62 + 41; _temp63;}), loc);} return({ struct _tuple2*
_temp64=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp64->f1=
idopt; _temp64->f2= _temp39; _temp64->f3= _temp37; _temp64;});}} static void*
Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Parse_Type_spec_struct* _temp65=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp65[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp66; _temp66.tag= Cyc_Parse_Type_spec_tag;
_temp66.f1=( void*) t; _temp66.f2= loc; _temp66;}); _temp65;});} static void*
Cyc_Parse_array2ptr( void* t){ void* _temp67= t; struct Cyc_Absyn_Exp* _temp73;
struct Cyc_Absyn_Tqual* _temp75; void* _temp77; _LL69: if(( unsigned int)
_temp67 > 4u?(( struct _tunion_struct*) _temp67)->tag == Cyc_Absyn_ArrayType_tag:
0){ _LL78: _temp77=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp67)->f1;
goto _LL76; _LL76: _temp75=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp67)->f2; goto _LL74; _LL74: _temp73=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp67)->f3; goto _LL70;} else{ goto _LL71;} _LL71: goto _LL72; _LL70: return
Cyc_Absyn_starb_typ( _temp77,( void*) Cyc_Absyn_HeapRgn, _temp75, _temp73 == 0?(
void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp79=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp79[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp80; _temp80.tag= Cyc_Absyn_Upper_b_tag;
_temp80.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp81= _temp73;
if( _temp81 == 0){ _throw( Null_Exception);} _temp81;}); _temp80;}); _temp79;}));
_LL72: return t; _LL68:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} static struct _tuple10* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple10*
_temp82=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp82->f1=(*
t).f2; _temp82->f2=(* t).f3; _temp82;});} static void Cyc_Parse_only_vardecl(
struct Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_string
decl_kind;{ void* _temp83=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp109; int
_temp111; struct Cyc_Absyn_Exp* _temp113; struct Cyc_Core_Opt* _temp115; struct
Cyc_Core_Opt* _temp117; struct Cyc_Absyn_Pat* _temp119; struct Cyc_Absyn_Fndecl*
_temp121; struct Cyc_Absyn_Structdecl* _temp123; struct Cyc_Absyn_Uniondecl*
_temp125; struct Cyc_Absyn_Tuniondecl* _temp127; struct Cyc_Absyn_Typedefdecl*
_temp129; struct Cyc_Absyn_XTuniondecl* _temp131; struct Cyc_Absyn_Enumdecl*
_temp133; struct Cyc_List_List* _temp135; struct _tagged_string* _temp137;
struct Cyc_List_List* _temp139; struct _tuple1* _temp141; struct Cyc_List_List*
_temp143; _LL85: if((( struct _tunion_struct*) _temp83)->tag == Cyc_Absyn_Var_d_tag){
_LL110: _temp109=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp83)->f1; goto _LL86;} else{ goto _LL87;} _LL87: if((( struct _tunion_struct*)
_temp83)->tag == Cyc_Absyn_Let_d_tag){ _LL120: _temp119=( struct Cyc_Absyn_Pat*)((
struct Cyc_Absyn_Let_d_struct*) _temp83)->f1; goto _LL118; _LL118: _temp117=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp83)->f2; goto
_LL116; _LL116: _temp115=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp83)->f3; goto _LL114; _LL114: _temp113=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Let_d_struct*) _temp83)->f4; goto _LL112; _LL112: _temp111=( int)((
struct Cyc_Absyn_Let_d_struct*) _temp83)->f5; goto _LL88;} else{ goto _LL89;}
_LL89: if((( struct _tunion_struct*) _temp83)->tag == Cyc_Absyn_Fn_d_tag){
_LL122: _temp121=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp83)->f1; goto _LL90;} else{ goto _LL91;} _LL91: if((( struct _tunion_struct*)
_temp83)->tag == Cyc_Absyn_Struct_d_tag){ _LL124: _temp123=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp83)->f1; goto _LL92;} else{ goto _LL93;}
_LL93: if((( struct _tunion_struct*) _temp83)->tag == Cyc_Absyn_Union_d_tag){
_LL126: _temp125=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp83)->f1; goto _LL94;} else{ goto _LL95;} _LL95: if((( struct _tunion_struct*)
_temp83)->tag == Cyc_Absyn_Tunion_d_tag){ _LL128: _temp127=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp83)->f1; goto _LL96;} else{ goto _LL97;}
_LL97: if((( struct _tunion_struct*) _temp83)->tag == Cyc_Absyn_Typedef_d_tag){
_LL130: _temp129=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp83)->f1; goto _LL98;} else{ goto _LL99;} _LL99: if((( struct _tunion_struct*)
_temp83)->tag == Cyc_Absyn_XTunion_d_tag){ _LL132: _temp131=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp83)->f1; goto _LL100;} else{ goto
_LL101;} _LL101: if((( struct _tunion_struct*) _temp83)->tag == Cyc_Absyn_Enum_d_tag){
_LL134: _temp133=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp83)->f1; goto _LL102;} else{ goto _LL103;} _LL103: if((( struct
_tunion_struct*) _temp83)->tag == Cyc_Absyn_Namespace_d_tag){ _LL138: _temp137=(
struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp83)->f1;
goto _LL136; _LL136: _temp135=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp83)->f2; goto _LL104;} else{ goto _LL105;} _LL105: if((( struct
_tunion_struct*) _temp83)->tag == Cyc_Absyn_Using_d_tag){ _LL142: _temp141=(
struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp83)->f1; goto _LL140;
_LL140: _temp139=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp83)->f2; goto _LL106;} else{ goto _LL107;} _LL107: if((( struct
_tunion_struct*) _temp83)->tag == Cyc_Absyn_ExternC_d_tag){ _LL144: _temp143=(
struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*) _temp83)->f1; goto
_LL108;} else{ goto _LL84;} _LL86: if( _temp109->initializer != 0){(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp145=( char*)"initializers are not allowed in parameter declarations";
struct _tagged_string _temp146; _temp146.curr= _temp145; _temp146.base= _temp145;
_temp146.last_plus_one= _temp145 + 55; _temp146;}), x->loc);}{ void* _temp147=(*
_temp109->name).f1; struct Cyc_List_List* _temp157; struct Cyc_List_List*
_temp159; _LL149: if(( int) _temp147 == Cyc_Absyn_Loc_n){ goto _LL150;} else{
goto _LL151;} _LL151: if(( unsigned int) _temp147 > 1u?(( struct _tunion_struct*)
_temp147)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL158: _temp157=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp147)->f1; if( _temp157 == 0){ goto _LL152;}
else{ goto _LL153;}} else{ goto _LL153;} _LL153: if(( unsigned int) _temp147 > 1u?((
struct _tunion_struct*) _temp147)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL160:
_temp159=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp147)->f1;
if( _temp159 == 0){ goto _LL154;} else{ goto _LL155;}} else{ goto _LL155;}
_LL155: goto _LL156; _LL150: goto _LL148; _LL152: goto _LL148; _LL154: goto
_LL148; _LL156: Cyc_Parse_err(( struct _tagged_string)({ char* _temp161=( char*)"module names not allowed on parameter declarations";
struct _tagged_string _temp162; _temp162.curr= _temp161; _temp162.base= _temp161;
_temp162.last_plus_one= _temp161 + 51; _temp162;}), x->loc); goto _LL148; _LL148:;}{
int found= 0; for( 0; params != 0; params=({ struct Cyc_List_List* _temp163=
params; if( _temp163 == 0){ _throw( Null_Exception);} _temp163->tl;})){ if( Cyc_String_zstrptrcmp((*
_temp109->name).f2,( struct _tagged_string*)({ struct Cyc_List_List* _temp164=
params; if( _temp164 == 0){ _throw( Null_Exception);} _temp164->hd;})) == 0){
found= 1; break;}} if( ! found){(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp165=*(*
_temp109->name).f2; xprintf("%.*s is not listed as a parameter", _temp165.last_plus_one
- _temp165.curr, _temp165.curr);}), x->loc);} return;} _LL88: decl_kind=( struct
_tagged_string)({ char* _temp166=( char*)"let declaration"; struct
_tagged_string _temp167; _temp167.curr= _temp166; _temp167.base= _temp166;
_temp167.last_plus_one= _temp166 + 16; _temp167;}); goto _LL84; _LL90: decl_kind=(
struct _tagged_string)({ char* _temp168=( char*)"function declaration"; struct
_tagged_string _temp169; _temp169.curr= _temp168; _temp169.base= _temp168;
_temp169.last_plus_one= _temp168 + 21; _temp169;}); goto _LL84; _LL92: decl_kind=(
struct _tagged_string)({ char* _temp170=( char*)"struct declaration"; struct
_tagged_string _temp171; _temp171.curr= _temp170; _temp171.base= _temp170;
_temp171.last_plus_one= _temp170 + 19; _temp171;}); goto _LL84; _LL94: decl_kind=(
struct _tagged_string)({ char* _temp172=( char*)"union declaration"; struct
_tagged_string _temp173; _temp173.curr= _temp172; _temp173.base= _temp172;
_temp173.last_plus_one= _temp172 + 18; _temp173;}); goto _LL84; _LL96: decl_kind=(
struct _tagged_string)({ char* _temp174=( char*)"tunion declaration"; struct
_tagged_string _temp175; _temp175.curr= _temp174; _temp175.base= _temp174;
_temp175.last_plus_one= _temp174 + 19; _temp175;}); goto _LL84; _LL98: decl_kind=(
struct _tagged_string)({ char* _temp176=( char*)"typedef"; struct _tagged_string
_temp177; _temp177.curr= _temp176; _temp177.base= _temp176; _temp177.last_plus_one=
_temp176 + 8; _temp177;}); goto _LL84; _LL100: decl_kind=( struct _tagged_string)({
char* _temp178=( char*)"xtunion declaration"; struct _tagged_string _temp179;
_temp179.curr= _temp178; _temp179.base= _temp178; _temp179.last_plus_one=
_temp178 + 20; _temp179;}); goto _LL84; _LL102: decl_kind=( struct
_tagged_string)({ char* _temp180=( char*)"enum declaration"; struct
_tagged_string _temp181; _temp181.curr= _temp180; _temp181.base= _temp180;
_temp181.last_plus_one= _temp180 + 17; _temp181;}); goto _LL84; _LL104:
decl_kind=( struct _tagged_string)({ char* _temp182=( char*)"namespace declaration";
struct _tagged_string _temp183; _temp183.curr= _temp182; _temp183.base= _temp182;
_temp183.last_plus_one= _temp182 + 22; _temp183;}); goto _LL84; _LL106:
decl_kind=( struct _tagged_string)({ char* _temp184=( char*)"using declaration";
struct _tagged_string _temp185; _temp185.curr= _temp184; _temp185.base= _temp184;
_temp185.last_plus_one= _temp184 + 18; _temp185;}); goto _LL84; _LL108:
decl_kind=( struct _tagged_string)({ char* _temp186=( char*)"extern C declaration";
struct _tagged_string _temp187; _temp187.curr= _temp186; _temp187.base= _temp186;
_temp187.last_plus_one= _temp186 + 21; _temp187;}); goto _LL84; _LL84:;}(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp188= decl_kind; xprintf("%.*s appears in parameter type",
_temp188.last_plus_one - _temp188.curr, _temp188.curr);}), x->loc); return;}
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple11* env, struct
_tagged_string* x){ struct _tuple11 _temp191; struct Cyc_Position_Segment*
_temp192; struct Cyc_List_List* _temp194; struct _tuple11* _temp189= env;
_temp191=* _temp189; _LL195: _temp194= _temp191.f1; goto _LL193; _LL193:
_temp192= _temp191.f2; goto _LL190; _LL190: if( _temp194 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp196=* x; xprintf("missing type for parameter %.*s",
_temp196.last_plus_one - _temp196.curr, _temp196.curr);}), _temp192);}{ void*
_temp198=( void*)(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp197=
_temp194; if( _temp197 == 0){ _throw( Null_Exception);} _temp197->hd;}))->r;
struct Cyc_Absyn_Vardecl* _temp204; _LL200: if((( struct _tunion_struct*)
_temp198)->tag == Cyc_Absyn_Var_d_tag){ _LL205: _temp204=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp198)->f1; goto _LL201;} else{ goto _LL202;}
_LL202: goto _LL203; _LL201:{ void* _temp206=(* _temp204->name).f1; struct Cyc_List_List*
_temp216; struct Cyc_List_List* _temp218; _LL208: if(( int) _temp206 == Cyc_Absyn_Loc_n){
goto _LL209;} else{ goto _LL210;} _LL210: if(( unsigned int) _temp206 > 1u?((
struct _tunion_struct*) _temp206)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL217:
_temp216=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp206)->f1;
if( _temp216 == 0){ goto _LL211;} else{ goto _LL212;}} else{ goto _LL212;}
_LL212: if(( unsigned int) _temp206 > 1u?(( struct _tunion_struct*) _temp206)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL219: _temp218=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp206)->f1; if( _temp218 == 0){ goto _LL213;} else{
goto _LL214;}} else{ goto _LL214;} _LL214: goto _LL215; _LL209: goto _LL207;
_LL211: goto _LL207; _LL213: goto _LL207; _LL215: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp220=( char*)"module name not allowed on parameter";
struct _tagged_string _temp221; _temp221.curr= _temp220; _temp221.base= _temp220;
_temp221.last_plus_one= _temp220 + 37; _temp221;}), _temp192); goto _LL207;
_LL207:;} if( Cyc_String_zstrptrcmp((* _temp204->name).f2, x) == 0){ return({
struct _tuple2* _temp222=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp222[ 0]=({ struct _tuple2 _temp223; _temp223.f1=({ struct Cyc_Core_Opt*
_temp224=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp224[ 0]=({ struct Cyc_Core_Opt _temp225; _temp225.v=( void*)(* _temp204->name).f2;
_temp225;}); _temp224;}); _temp223.f2= _temp204->tq; _temp223.f3=( void*)
_temp204->type; _temp223;}); _temp222;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple11* _temp226=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp226->f1=({ struct Cyc_List_List* _temp227= _temp194; if( _temp227 == 0){
_throw( Null_Exception);} _temp227->tl;}); _temp226->f2= _temp192; _temp226;}),
x);} _LL203: return(( struct _tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp228=( char*)"non-variable declaration";
struct _tagged_string _temp229; _temp229.curr= _temp228; _temp229.base= _temp228;
_temp229.last_plus_one= _temp228 + 25; _temp229;}),(( struct Cyc_Absyn_Decl*)({
struct Cyc_List_List* _temp230= _temp194; if( _temp230 == 0){ _throw(
Null_Exception);} _temp230->hd;}))->loc); _LL199:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp231= tm; int _temp237; struct Cyc_Position_Segment*
_temp239; struct Cyc_List_List* _temp241; _LL233: if(( unsigned int) _temp231 >
1u?(( struct _tunion_struct*) _temp231)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL242: _temp241=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp231)->f1; goto _LL240; _LL240: _temp239=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp231)->f2; goto _LL238; _LL238:
_temp237=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp231)->f3; goto
_LL234;} else{ goto _LL235;} _LL235: goto _LL236; _LL234: return 1; _LL236:
return 0; _LL232:;} static void* Cyc_Parse_id2type( struct _tagged_string s,
struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp243=( char*)"`H"; struct _tagged_string _temp244; _temp244.curr=
_temp243; _temp244.base= _temp243; _temp244.last_plus_one= _temp243 + 3;
_temp244;})) == 0){ return( void*) Cyc_Absyn_HeapRgn;} else{ return( void*)({
struct Cyc_Absyn_VarType_struct* _temp245=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp245[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp246; _temp246.tag= Cyc_Absyn_VarType_tag; _temp246.f1=({ struct Cyc_Absyn_Tvar*
_temp247=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp247->name=({ struct _tagged_string* _temp248=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp248[ 0]= s; _temp248;});
_temp247->kind= k; _temp247;}); _temp246;}); _temp245;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp249=
t; struct Cyc_Absyn_Tvar* _temp255; _LL251: if(( unsigned int) _temp249 > 4u?((
struct _tunion_struct*) _temp249)->tag == Cyc_Absyn_VarType_tag: 0){ _LL256:
_temp255=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp249)->f1;
goto _LL252;} else{ goto _LL253;} _LL253: goto _LL254; _LL252: return _temp255;
_LL254: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char*
_temp257=( char*)"expecting a list of type variables, not types"; struct
_tagged_string _temp258; _temp258.curr= _temp257; _temp258.base= _temp257;
_temp258.last_plus_one= _temp257 + 46; _temp258;}), loc); _LL250:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp259=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp259[ 0]=({ struct Cyc_Absyn_VarType_struct _temp260; _temp260.tag= Cyc_Absyn_VarType_tag;
_temp260.f1= pr; _temp260;}); _temp259;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp262=(
void*)({ struct Cyc_List_List* _temp261= tms; if( _temp261 == 0){ _throw(
Null_Exception);} _temp261->hd;}); void* _temp268; _LL264: if(( unsigned int)
_temp262 > 1u?(( struct _tunion_struct*) _temp262)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL269: _temp268=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp262)->f1;
goto _LL265;} else{ goto _LL266;} _LL266: goto _LL267; _LL265: if(({ struct Cyc_List_List*
_temp270= tms; if( _temp270 == 0){ _throw( Null_Exception);} _temp270->tl;}) ==
0? 1:( Cyc_Parse_is_typeparam(( void*)({ struct Cyc_List_List* _temp272=({
struct Cyc_List_List* _temp271= tms; if( _temp271 == 0){ _throw( Null_Exception);}
_temp271->tl;}); if( _temp272 == 0){ _throw( Null_Exception);} _temp272->hd;}))?({
struct Cyc_List_List* _temp274=({ struct Cyc_List_List* _temp273= tms; if(
_temp273 == 0){ _throw( Null_Exception);} _temp273->tl;}); if( _temp274 == 0){
_throw( Null_Exception);} _temp274->tl;}) == 0: 0)){ void* _temp275= _temp268;
struct Cyc_Core_Opt* _temp281; int _temp283; struct Cyc_List_List* _temp285;
struct Cyc_Position_Segment* _temp287; struct Cyc_List_List* _temp289; _LL277:
if((( struct _tunion_struct*) _temp275)->tag == Cyc_Absyn_WithTypes_tag){ _LL286:
_temp285=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*) _temp275)->f1;
goto _LL284; _LL284: _temp283=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp275)->f2; goto _LL282; _LL282: _temp281=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp275)->f3; goto _LL278;} else{ goto _LL279;}
_LL279: if((( struct _tunion_struct*) _temp275)->tag == Cyc_Absyn_NoTypes_tag){
_LL290: _temp289=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp275)->f1; goto _LL288; _LL288: _temp287=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp275)->f2; goto _LL280;} else{ goto _LL276;}
_LL278: Cyc_Parse_warn(( struct _tagged_string)({ char* _temp291=( char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp292; _temp292.curr= _temp291; _temp292.base= _temp291;
_temp292.last_plus_one= _temp291 + 92; _temp292;}), loc); return tms; _LL280:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp289, tds);{ struct _tuple11* env=({ struct _tuple11* _temp293=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp293->f1= tds; _temp293->f2=
loc; _temp293;}); return({ struct Cyc_List_List* _temp294=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp294->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp295=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp295[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp296; _temp296.tag= Cyc_Absyn_Function_mod_tag;
_temp296.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp297=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp297[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp298; _temp298.tag= Cyc_Absyn_WithTypes_tag;
_temp298.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple11*,
struct _tagged_string*), struct _tuple11* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, env, _temp289); _temp298.f2= 0; _temp298.f3= 0;
_temp298;}); _temp297;})); _temp296;}); _temp295;})); _temp294->tl= 0; _temp294;});}
_LL276:;} else{ return({ struct Cyc_List_List* _temp299=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp299->hd=( void*)(( void*)({
struct Cyc_List_List* _temp301= tms; if( _temp301 == 0){ _throw( Null_Exception);}
_temp301->hd;})); _temp299->tl= Cyc_Parse_oldstyle2newstyle(({ struct Cyc_List_List*
_temp300= tms; if( _temp300 == 0){ _throw( Null_Exception);} _temp300->tl;}),
tds, loc); _temp299;});} _LL267: return({ struct Cyc_List_List* _temp302=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp302->hd=(
void*)(( void*)({ struct Cyc_List_List* _temp304= tms; if( _temp304 == 0){
_throw( Null_Exception);} _temp304->hd;})); _temp302->tl= Cyc_Parse_oldstyle2newstyle(({
struct Cyc_List_List* _temp303= tms; if( _temp303 == 0){ _throw( Null_Exception);}
_temp303->tl;}), tds, loc); _temp302;}); _LL263:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp305=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp305->id= d->id; _temp305->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp305;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)({ struct Cyc_Core_Opt* _temp306= dso; if( _temp306
== 0){ _throw( Null_Exception);} _temp306->v;}))->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*)({
struct Cyc_Core_Opt* _temp307= dso; if( _temp307 == 0){ _throw( Null_Exception);}
_temp307->v;}))->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)({ struct
Cyc_Core_Opt* _temp308= dso; if( _temp308 == 0){ _throw( Null_Exception);}
_temp308->v;}))->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)({ struct
Cyc_Core_Opt* _temp309= dso; if( _temp309 == 0){ _throw( Null_Exception);}
_temp309->v;}))->attributes; if((( struct Cyc_Parse_Declaration_spec*)({ struct
Cyc_Core_Opt* _temp310= dso; if( _temp310 == 0){ _throw( Null_Exception);}
_temp310->v;}))->sc != 0){ void* _temp313=( void*)({ struct Cyc_Core_Opt*
_temp312=(( struct Cyc_Parse_Declaration_spec*)({ struct Cyc_Core_Opt* _temp311=
dso; if( _temp311 == 0){ _throw( Null_Exception);} _temp311->v;}))->sc; if(
_temp312 == 0){ _throw( Null_Exception);} _temp312->v;}); _LL315: if(( int)
_temp313 == Cyc_Parse_Extern_sc){ goto _LL316;} else{ goto _LL317;} _LL317: if((
int) _temp313 == Cyc_Parse_ExternC_sc){ goto _LL318;} else{ goto _LL319;} _LL319:
if(( int) _temp313 == Cyc_Parse_Static_sc){ goto _LL320;} else{ goto _LL321;}
_LL321: goto _LL322; _LL316: sc=( void*) Cyc_Absyn_Extern; goto _LL314; _LL318:
sc=( void*) Cyc_Absyn_ExternC; goto _LL314; _LL320: sc=( void*) Cyc_Absyn_Static;
goto _LL314; _LL322: Cyc_Parse_err(( struct _tagged_string)({ char* _temp323=(
char*)"bad storage class on function"; struct _tagged_string _temp324; _temp324.curr=
_temp323; _temp324.base= _temp323; _temp324.last_plus_one= _temp323 + 30;
_temp324;}), loc); goto _LL314; _LL314:;}}{ struct Cyc_Core_Opt* _temp327; void*
_temp329; struct _tuple5 _temp325= Cyc_Parse_collapse_type_specifiers( tss, loc);
_LL330: _temp329= _temp325.f1; goto _LL328; _LL328: _temp327= _temp325.f2; goto
_LL326; _LL326: { struct Cyc_List_List* _temp333; struct Cyc_List_List* _temp335;
void* _temp337; struct Cyc_Absyn_Tqual* _temp339; struct _tuple6 _temp331= Cyc_Parse_apply_tms(
tq, _temp329, atts, d->tms); _LL340: _temp339= _temp331.f1; goto _LL338; _LL338:
_temp337= _temp331.f2; goto _LL336; _LL336: _temp335= _temp331.f3; goto _LL334;
_LL334: _temp333= _temp331.f4; goto _LL332; _LL332: if( _temp327 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp341=( char*)"nested type declaration within function prototype";
struct _tagged_string _temp342; _temp342.curr= _temp341; _temp342.base= _temp341;
_temp342.last_plus_one= _temp341 + 50; _temp342;}), loc);} if( _temp335 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp343=( char*)"bad type params, ignoring";
struct _tagged_string _temp344; _temp344.curr= _temp343; _temp344.base= _temp343;
_temp344.last_plus_one= _temp343 + 26; _temp344;}), loc);}{ void* _temp345=
_temp337; struct Cyc_Absyn_FnInfo _temp351; struct Cyc_List_List* _temp353; int
_temp355; struct Cyc_List_List* _temp357; void* _temp359; struct Cyc_Core_Opt*
_temp361; struct Cyc_List_List* _temp363; _LL347: if(( unsigned int) _temp345 >
4u?(( struct _tunion_struct*) _temp345)->tag == Cyc_Absyn_FnType_tag: 0){ _LL352:
_temp351=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp345)->f1;
_LL364: _temp363=( struct Cyc_List_List*) _temp351.tvars; goto _LL362; _LL362:
_temp361=( struct Cyc_Core_Opt*) _temp351.effect; goto _LL360; _LL360: _temp359=(
void*) _temp351.ret_typ; goto _LL358; _LL358: _temp357=( struct Cyc_List_List*)
_temp351.args; goto _LL356; _LL356: _temp355=( int) _temp351.varargs; goto
_LL354; _LL354: _temp353=( struct Cyc_List_List*) _temp351.attributes; goto
_LL348;} else{ goto _LL349;} _LL349: goto _LL350; _LL348: { struct Cyc_List_List*
args2=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp357); return({ struct Cyc_Absyn_Fndecl*
_temp365=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp365->sc=( void*) sc; _temp365->name= d->id; _temp365->tvs= _temp363;
_temp365->is_inline= is_inline; _temp365->effect= _temp361; _temp365->ret_type=(
void*) _temp359; _temp365->args= args2; _temp365->varargs= _temp355; _temp365->body=
body; _temp365->cached_typ= 0; _temp365->param_vardecls= 0; _temp365->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp353, _temp333); _temp365;});} _LL350: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp366=( char*)"declarator is not a function prototype";
struct _tagged_string _temp367; _temp367.curr= _temp366; _temp367.base= _temp366;
_temp367.last_plus_one= _temp366 + 39; _temp367;}), loc); _LL346:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp368=( char*)"missing argument variable in function prototype"; struct
_tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 48; _temp369;}), loc); return({ struct
_tuple4* _temp370=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp370->f1=({ struct _tagged_string* _temp371=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp371[ 0]=( struct
_tagged_string)({ char* _temp372=( char*)"?"; struct _tagged_string _temp373;
_temp373.curr= _temp372; _temp373.base= _temp372; _temp373.last_plus_one=
_temp372 + 2; _temp373;}); _temp371;}); _temp370->f2=(* t).f2; _temp370->f3=(* t).f3;
_temp370;});} else{ return({ struct _tuple4* _temp374=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp374->f1=( struct _tagged_string*)({
struct Cyc_Core_Opt* _temp375=(* t).f1; if( _temp375 == 0){ _throw(
Null_Exception);} _temp375->v;}); _temp374->f2=(* t).f2; _temp374->f3=(* t).f3;
_temp374;});}} static char _temp378[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_string Cyc_Parse_msg1=( struct _tagged_string){ _temp378,
_temp378, _temp378 + 52u}; static char _temp381[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2=( struct _tagged_string){ _temp381,
_temp381, _temp381 + 63u}; static char _temp384[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3=( struct _tagged_string){ _temp384,
_temp384, _temp384 + 50u}; static char _temp387[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4=( struct _tagged_string){ _temp387,
_temp387, _temp387 + 60u}; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
declopt= 0; int seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*)
Cyc_Absyn_VoidType; void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed;
struct Cyc_Position_Segment* last_loc= loc; for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp388= ts; if( _temp388 == 0){ _throw( Null_Exception);} _temp388->tl;})){
void* _temp390=( void*)({ struct Cyc_List_List* _temp389= ts; if( _temp389 == 0){
_throw( Null_Exception);} _temp389->hd;}); struct Cyc_Position_Segment* _temp404;
void* _temp406; struct Cyc_Position_Segment* _temp408; struct Cyc_Position_Segment*
_temp410; struct Cyc_Position_Segment* _temp412; struct Cyc_Position_Segment*
_temp414; struct Cyc_Absyn_Decl* _temp416; _LL392: if((( struct _tunion_struct*)
_temp390)->tag == Cyc_Parse_Type_spec_tag){ _LL407: _temp406=( void*)(( struct
Cyc_Parse_Type_spec_struct*) _temp390)->f1; goto _LL405; _LL405: _temp404=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Type_spec_struct*) _temp390)->f2;
goto _LL393;} else{ goto _LL394;} _LL394: if((( struct _tunion_struct*) _temp390)->tag
== Cyc_Parse_Signed_spec_tag){ _LL409: _temp408=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Signed_spec_struct*) _temp390)->f1; goto _LL395;} else{ goto
_LL396;} _LL396: if((( struct _tunion_struct*) _temp390)->tag == Cyc_Parse_Unsigned_spec_tag){
_LL411: _temp410=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Unsigned_spec_struct*)
_temp390)->f1; goto _LL397;} else{ goto _LL398;} _LL398: if((( struct
_tunion_struct*) _temp390)->tag == Cyc_Parse_Short_spec_tag){ _LL413: _temp412=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Short_spec_struct*) _temp390)->f1;
goto _LL399;} else{ goto _LL400;} _LL400: if((( struct _tunion_struct*) _temp390)->tag
== Cyc_Parse_Long_spec_tag){ _LL415: _temp414=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Long_spec_struct*) _temp390)->f1; goto _LL401;} else{ goto
_LL402;} _LL402: if((( struct _tunion_struct*) _temp390)->tag == Cyc_Parse_Decl_spec_tag){
_LL417: _temp416=( struct Cyc_Absyn_Decl*)(( struct Cyc_Parse_Decl_spec_struct*)
_temp390)->f1; goto _LL403;} else{ goto _LL391;} _LL393: if( seen_type){ Cyc_Parse_err(
Cyc_Parse_msg1, _temp404);} last_loc= _temp404; seen_type= 1; t= _temp406; goto
_LL391; _LL395: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp408);} if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp418=( char*)"signed qualifier must come before type";
struct _tagged_string _temp419; _temp419.curr= _temp418; _temp419.base= _temp418;
_temp419.last_plus_one= _temp418 + 39; _temp419;}), _temp408);} last_loc=
_temp408; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed; goto _LL391; _LL397: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp410);} if( seen_type){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp420=( char*)"signed qualifier must come before type";
struct _tagged_string _temp421; _temp421.curr= _temp420; _temp421.base= _temp420;
_temp421.last_plus_one= _temp420 + 39; _temp421;}), _temp410);} last_loc=
_temp410; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL391; _LL399:
if( seen_size){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp422=( char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp423; _temp423.curr= _temp422; _temp423.base= _temp422;
_temp423.last_plus_one= _temp422 + 59; _temp423;}), _temp412);} if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp424=( char*)"short modifier must come before base type";
struct _tagged_string _temp425; _temp425.curr= _temp424; _temp425.base= _temp424;
_temp425.last_plus_one= _temp424 + 42; _temp425;}), _temp412);} last_loc=
_temp412; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL391; _LL401: if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp426=( char*)"long modifier must come before base type";
struct _tagged_string _temp427; _temp427.curr= _temp426; _temp427.base= _temp426;
_temp427.last_plus_one= _temp426 + 41; _temp427;}), _temp414);} if( seen_size){
void* _temp428= sz; _LL430: if(( int) _temp428 == Cyc_Absyn_B4){ goto _LL431;}
else{ goto _LL432;} _LL432: goto _LL433; _LL431: sz=( void*) Cyc_Absyn_B8; goto
_LL429; _LL433: Cyc_Parse_err(( struct _tagged_string)({ char* _temp434=( char*)"extra long in type specifier";
struct _tagged_string _temp435; _temp435.curr= _temp434; _temp435.base= _temp434;
_temp435.last_plus_one= _temp434 + 29; _temp435;}), _temp414); goto _LL429;
_LL429:;} last_loc= _temp414; seen_size= 1; goto _LL391; _LL403: last_loc=
_temp416->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp436=(
void*) _temp416->r; struct Cyc_Absyn_Structdecl* _temp450; struct Cyc_Absyn_Tuniondecl*
_temp452; struct Cyc_Absyn_XTuniondecl* _temp454; struct Cyc_Absyn_Uniondecl*
_temp456; struct Cyc_Absyn_Enumdecl* _temp458; _LL438: if((( struct
_tunion_struct*) _temp436)->tag == Cyc_Absyn_Struct_d_tag){ _LL451: _temp450=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp436)->f1;
goto _LL439;} else{ goto _LL440;} _LL440: if((( struct _tunion_struct*) _temp436)->tag
== Cyc_Absyn_Tunion_d_tag){ _LL453: _temp452=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp436)->f1; goto _LL441;} else{ goto
_LL442;} _LL442: if((( struct _tunion_struct*) _temp436)->tag == Cyc_Absyn_XTunion_d_tag){
_LL455: _temp454=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp436)->f1; goto _LL443;} else{ goto _LL444;} _LL444: if((( struct
_tunion_struct*) _temp436)->tag == Cyc_Absyn_Union_d_tag){ _LL457: _temp456=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp436)->f1;
goto _LL445;} else{ goto _LL446;} _LL446: if((( struct _tunion_struct*) _temp436)->tag
== Cyc_Absyn_Enum_d_tag){ _LL459: _temp458=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp436)->f1; goto _LL447;} else{ goto _LL448;}
_LL448: goto _LL449; _LL439: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp450->tvs); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp460=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp460[ 0]=({ struct Cyc_Absyn_StructType_struct _temp461; _temp461.tag= Cyc_Absyn_StructType_tag;
_temp461.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp462= _temp450->name; if( _temp462 == 0){ _throw( Null_Exception);} _temp462->v;}));
_temp461.f2= args; _temp461.f3= 0; _temp461;}); _temp460;}); if( _temp450->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp463=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp463->v=( void*) _temp416;
_temp463;});} goto _LL437;} _LL441: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp452->tvs); t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp464=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp464[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp465; _temp465.tag= Cyc_Absyn_TunionType_tag;
_temp465.f1=({ struct Cyc_Absyn_TunionInfo _temp466; _temp466.name=( struct
_tuple1*)({ struct Cyc_Core_Opt* _temp467= _temp452->name; if( _temp467 == 0){
_throw( Null_Exception);} _temp467->v;}); _temp466.targs= args; _temp466.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp466.tud= 0; _temp466;}); _temp465;});
_temp464;}); if( _temp452->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp468=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp468->v=( void*) _temp416; _temp468;});} goto _LL437;} _LL443: t=( void*)({
struct Cyc_Absyn_XTunionType_struct* _temp469=( struct Cyc_Absyn_XTunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct)); _temp469[ 0]=({ struct
Cyc_Absyn_XTunionType_struct _temp470; _temp470.tag= Cyc_Absyn_XTunionType_tag;
_temp470.f1=({ struct Cyc_Absyn_XTunionInfo _temp471; _temp471.name= _temp454->name;
_temp471.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp471.xtud= 0; _temp471;});
_temp470;}); _temp469;}); if( _temp454->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp472=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp472->v=( void*) _temp416; _temp472;});} goto _LL437; _LL445: { struct Cyc_List_List*
args=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp456->tvs); t=( void*)({
struct Cyc_Absyn_UnionType_struct* _temp473=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp473[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp474; _temp474.tag= Cyc_Absyn_UnionType_tag;
_temp474.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp475= _temp456->name; if( _temp475 == 0){ _throw( Null_Exception);} _temp475->v;}));
_temp474.f2= args; _temp474.f3= 0; _temp474;}); _temp473;}); if( _temp456->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp476=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp476->v=( void*) _temp416;
_temp476;});} goto _LL437;} _LL447: t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp477=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp477[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp478; _temp478.tag= Cyc_Absyn_EnumType_tag;
_temp478.f1= _temp458->name; _temp478.f2= 0; _temp478;}); _temp477;}); declopt=({
struct Cyc_Core_Opt* _temp479=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp479->v=( void*) _temp416; _temp479;}); goto _LL437; _LL449:((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp480=( char*)"bad declaration within type specifier";
struct _tagged_string _temp481; _temp481.curr= _temp480; _temp481.base= _temp480;
_temp481.last_plus_one= _temp480 + 38; _temp481;}), _temp416->loc); goto _LL437;
_LL437:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp416->loc);} goto _LL391;
_LL391:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp482=( char*)"missing type withing specifier";
struct _tagged_string _temp483; _temp483.curr= _temp482; _temp483.base= _temp482;
_temp483.last_plus_one= _temp482 + 31; _temp483;}), last_loc);} t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp484=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp484[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp485; _temp485.tag= Cyc_Absyn_IntType_tag; _temp485.f1=( void*) sgn;
_temp485.f2=( void*) sz; _temp485;}); _temp484;});} else{ if( seen_sign){ void*
_temp486= t; void* _temp492; void* _temp494; _LL488: if(( unsigned int) _temp486
> 4u?(( struct _tunion_struct*) _temp486)->tag == Cyc_Absyn_IntType_tag: 0){
_LL495: _temp494=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp486)->f1;
goto _LL493; _LL493: _temp492=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp486)->f2; goto _LL489;} else{ goto _LL490;} _LL490: goto _LL491; _LL489: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp496=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp496[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp497; _temp497.tag= Cyc_Absyn_IntType_tag; _temp497.f1=( void*) sgn;
_temp497.f2=( void*) _temp492; _temp497;}); _temp496;}); goto _LL487; _LL491:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp498=( char*)"sign specification on non-integral type";
struct _tagged_string _temp499; _temp499.curr= _temp498; _temp499.base= _temp498;
_temp499.last_plus_one= _temp498 + 40; _temp499;}), last_loc); goto _LL487;
_LL487:;} if( seen_size){ void* _temp500= t; void* _temp506; void* _temp508;
_LL502: if(( unsigned int) _temp500 > 4u?(( struct _tunion_struct*) _temp500)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL509: _temp508=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp500)->f1; goto _LL507; _LL507: _temp506=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp500)->f2; goto _LL503;} else{ goto _LL504;} _LL504: goto _LL505; _LL503: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp510=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp510[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp511; _temp511.tag= Cyc_Absyn_IntType_tag; _temp511.f1=( void*) _temp508;
_temp511.f2=( void*) sz; _temp511;}); _temp510;}); goto _LL501; _LL505: Cyc_Parse_err((
struct _tagged_string)({ char* _temp512=( char*)"size qualifier on non-integral type";
struct _tagged_string _temp513; _temp513.curr= _temp512; _temp513.base= _temp512;
_temp513.last_plus_one= _temp512 + 36; _temp513;}), last_loc); goto _LL501;
_LL501:;}} return({ struct _tuple5 _temp514; _temp514.f1= t; _temp514.f2=
declopt; _temp514;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* d=( struct
Cyc_Parse_Declarator*)({ struct Cyc_List_List* _temp515= ds; if( _temp515 == 0){
_throw( Null_Exception);} _temp515->hd;}); struct _tuple1* q= d->id; struct Cyc_List_List*
_temp518; struct Cyc_List_List* _temp520; void* _temp522; struct Cyc_Absyn_Tqual*
_temp524; struct _tuple6 _temp516= Cyc_Parse_apply_tms( tq, t, shared_atts, d->tms);
_LL525: _temp524= _temp516.f1; goto _LL523; _LL523: _temp522= _temp516.f2; goto
_LL521; _LL521: _temp520= _temp516.f3; goto _LL519; _LL519: _temp518= _temp516.f4;
goto _LL517; _LL517: return({ struct Cyc_List_List* _temp526=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp526->hd=( void*)({ struct
_tuple7* _temp528=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp528->f1= q; _temp528->f2= _temp524; _temp528->f3= _temp522; _temp528->f4=
_temp520; _temp528->f5= _temp518; _temp528;}); _temp526->tl= Cyc_Parse_apply_tmss(
_temp524, t,({ struct Cyc_List_List* _temp527= ds; if( _temp527 == 0){ _throw(
Null_Exception);} _temp527->tl;}), shared_atts); _temp526;});}} static int Cyc_Parse_fn_type_att(
void* a){ void* _temp529= a; int _temp543; _LL531: if(( unsigned int) _temp529 >
15u?(( struct _tunion_struct*) _temp529)->tag == Cyc_Absyn_Regparm_att_tag: 0){
_LL544: _temp543=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp529)->f1;
goto _LL532;} else{ goto _LL533;} _LL533: if(( int) _temp529 == Cyc_Absyn_Stdcall_att){
goto _LL534;} else{ goto _LL535;} _LL535: if(( int) _temp529 == Cyc_Absyn_Cdecl_att){
goto _LL536;} else{ goto _LL537;} _LL537: if(( int) _temp529 == Cyc_Absyn_Noreturn_att){
goto _LL538;} else{ goto _LL539;} _LL539: if(( int) _temp529 == Cyc_Absyn_Const_att){
goto _LL540;} else{ goto _LL541;} _LL541: goto _LL542; _LL532: goto _LL534;
_LL534: goto _LL536; _LL536: goto _LL538; _LL538: goto _LL540; _LL540: return 1;
_LL542: return 0; _LL530:;} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp545; _temp545.f1= tq; _temp545.f2=
t; _temp545.f3= 0; _temp545.f4= atts; _temp545;});}{ void* _temp547=( void*)({
struct Cyc_List_List* _temp546= tms; if( _temp546 == 0){ _throw( Null_Exception);}
_temp546->hd;}); struct Cyc_Absyn_Exp* _temp561; void* _temp563; int _temp565;
struct Cyc_Position_Segment* _temp567; struct Cyc_List_List* _temp569; struct
Cyc_Absyn_Tqual* _temp571; void* _temp573; void* _temp575; struct Cyc_List_List*
_temp577; struct Cyc_Position_Segment* _temp579; _LL549: if(( int) _temp547 ==
Cyc_Absyn_Carray_mod){ goto _LL550;} else{ goto _LL551;} _LL551: if((
unsigned int) _temp547 > 1u?(( struct _tunion_struct*) _temp547)->tag == Cyc_Absyn_ConstArray_mod_tag:
0){ _LL562: _temp561=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp547)->f1; goto _LL552;} else{ goto _LL553;} _LL553: if(( unsigned int)
_temp547 > 1u?(( struct _tunion_struct*) _temp547)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL564: _temp563=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp547)->f1;
goto _LL554;} else{ goto _LL555;} _LL555: if(( unsigned int) _temp547 > 1u?((
struct _tunion_struct*) _temp547)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL570: _temp569=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp547)->f1; goto _LL568; _LL568: _temp567=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp547)->f2; goto _LL566; _LL566:
_temp565=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp547)->f3; goto
_LL556;} else{ goto _LL557;} _LL557: if(( unsigned int) _temp547 > 1u?(( struct
_tunion_struct*) _temp547)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL576:
_temp575=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp547)->f1; goto
_LL574; _LL574: _temp573=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp547)->f2; goto _LL572; _LL572: _temp571=( struct Cyc_Absyn_Tqual*)(( struct
Cyc_Absyn_Pointer_mod_struct*) _temp547)->f3; goto _LL558;} else{ goto _LL559;}
_LL559: if(( unsigned int) _temp547 > 1u?(( struct _tunion_struct*) _temp547)->tag
== Cyc_Absyn_Attributes_mod_tag: 0){ _LL580: _temp579=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_Attributes_mod_struct*) _temp547)->f1; goto _LL578; _LL578:
_temp577=( struct Cyc_List_List*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp547)->f2; goto _LL560;} else{ goto _LL548;} _LL550: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp581=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp581[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp582; _temp582.tag= Cyc_Absyn_ArrayType_tag;
_temp582.f1=( void*) t; _temp582.f2= tq; _temp582.f3= 0; _temp582;}); _temp581;}),
atts,({ struct Cyc_List_List* _temp583= tms; if( _temp583 == 0){ _throw(
Null_Exception);} _temp583->tl;})); _LL552: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp584=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp584[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp585; _temp585.tag= Cyc_Absyn_ArrayType_tag;
_temp585.f1=( void*) t; _temp585.f2= tq; _temp585.f3=( struct Cyc_Absyn_Exp*)
_temp561; _temp585;}); _temp584;}), atts,({ struct Cyc_List_List* _temp586= tms;
if( _temp586 == 0){ _throw( Null_Exception);} _temp586->tl;})); _LL554: { void*
_temp587= _temp563; struct Cyc_Core_Opt* _temp593; int _temp595; struct Cyc_List_List*
_temp597; struct Cyc_Position_Segment* _temp599; struct Cyc_List_List* _temp601;
_LL589: if((( struct _tunion_struct*) _temp587)->tag == Cyc_Absyn_WithTypes_tag){
_LL598: _temp597=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp587)->f1; goto _LL596; _LL596: _temp595=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp587)->f2; goto _LL594; _LL594: _temp593=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp587)->f3; goto _LL590;} else{ goto _LL591;}
_LL591: if((( struct _tunion_struct*) _temp587)->tag == Cyc_Absyn_NoTypes_tag){
_LL602: _temp601=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp587)->f1; goto _LL600; _LL600: _temp599=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp587)->f2; goto _LL592;} else{ goto _LL588;}
_LL590: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as=({ struct Cyc_List_List* _temp603= as; if( _temp603 == 0){ _throw(
Null_Exception);} _temp603->tl;})){ if( Cyc_Parse_fn_type_att(( void*)({ struct
Cyc_List_List* _temp604= as; if( _temp604 == 0){ _throw( Null_Exception);}
_temp604->hd;}))){ fn_atts=({ struct Cyc_List_List* _temp605=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp605->hd=( void*)(( void*)({
struct Cyc_List_List* _temp606= as; if( _temp606 == 0){ _throw( Null_Exception);}
_temp606->hd;})); _temp605->tl= fn_atts; _temp605;});} else{ new_atts=({ struct
Cyc_List_List* _temp607=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp607->hd=( void*)(( void*)({ struct Cyc_List_List* _temp608= as; if(
_temp608 == 0){ _throw( Null_Exception);} _temp608->hd;})); _temp607->tl=
new_atts; _temp607;});}}} if(({ struct Cyc_List_List* _temp609= tms; if(
_temp609 == 0){ _throw( Null_Exception);} _temp609->tl;}) != 0){ void* _temp612=(
void*)({ struct Cyc_List_List* _temp611=({ struct Cyc_List_List* _temp610= tms;
if( _temp610 == 0){ _throw( Null_Exception);} _temp610->tl;}); if( _temp611 == 0){
_throw( Null_Exception);} _temp611->hd;}); int _temp618; struct Cyc_Position_Segment*
_temp620; struct Cyc_List_List* _temp622; _LL614: if(( unsigned int) _temp612 >
1u?(( struct _tunion_struct*) _temp612)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL623: _temp622=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp612)->f1; goto _LL621; _LL621: _temp620=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp612)->f2; goto _LL619; _LL619:
_temp618=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp612)->f3; goto
_LL615;} else{ goto _LL616;} _LL616: goto _LL617; _LL615: typvars= _temp622; tms=({
struct Cyc_List_List* _temp624= tms; if( _temp624 == 0){ _throw( Null_Exception);}
_temp624->tl;}); goto _LL613; _LL617: goto _LL613; _LL613:;} if(((( ! _temp595?
_temp597 != 0: 0)?({ struct Cyc_List_List* _temp625= _temp597; if( _temp625 == 0){
_throw( Null_Exception);} _temp625->tl;}) == 0: 0)?(*(( struct _tuple2*)({
struct Cyc_List_List* _temp626= _temp597; if( _temp626 == 0){ _throw(
Null_Exception);} _temp626->hd;}))).f1 == 0: 0)?(*(( struct _tuple2*)({ struct
Cyc_List_List* _temp627= _temp597; if( _temp627 == 0){ _throw( Null_Exception);}
_temp627->hd;}))).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp597= 0; _temp595= 0;}
t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*), struct Cyc_List_List*
x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp597); return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ( typvars, _temp593, t, _temp597,
_temp595, fn_atts), new_atts,({ struct Cyc_List_List* _temp628= tms; if(
_temp628 == 0){ _throw( Null_Exception);} _temp628->tl;}));} _LL592:( void)
_throw((( struct _xtunion_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp629=( char*)"function declaration without parameter types";
struct _tagged_string _temp630; _temp630.curr= _temp629; _temp630.base= _temp629;
_temp630.last_plus_one= _temp629 + 45; _temp630;}), _temp599)); _LL588:;} _LL556:
if(({ struct Cyc_List_List* _temp631= tms; if( _temp631 == 0){ _throw(
Null_Exception);} _temp631->tl;}) == 0){ return({ struct _tuple6 _temp632;
_temp632.f1= tq; _temp632.f2= t; _temp632.f3= _temp569; _temp632.f4= atts;
_temp632;});}( void) _throw((( struct _xtunion_struct*(*)( struct _tagged_string
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({
char* _temp633=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp634; _temp634.curr= _temp633; _temp634.base= _temp633;
_temp634.last_plus_one= _temp633 + 68; _temp634;}), _temp567)); _LL558: { void*
_temp635= _temp575; struct Cyc_Absyn_Exp* _temp643; struct Cyc_Absyn_Exp*
_temp645; _LL637: if(( unsigned int) _temp635 > 1u?(( struct _tunion_struct*)
_temp635)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL644: _temp643=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp635)->f1; goto
_LL638;} else{ goto _LL639;} _LL639: if(( unsigned int) _temp635 > 1u?(( struct
_tunion_struct*) _temp635)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL646:
_temp645=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*)
_temp635)->f1; goto _LL640;} else{ goto _LL641;} _LL641: if(( int) _temp635 ==
Cyc_Absyn_TaggedArray_ps){ goto _LL642;} else{ goto _LL636;} _LL638: return Cyc_Parse_apply_tms(
_temp571, Cyc_Absyn_atb_typ( t, _temp573, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp647=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp647[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp648; _temp648.tag= Cyc_Absyn_Upper_b_tag;
_temp648.f1= _temp643; _temp648;}); _temp647;})), atts,({ struct Cyc_List_List*
_temp649= tms; if( _temp649 == 0){ _throw( Null_Exception);} _temp649->tl;}));
_LL640: return Cyc_Parse_apply_tms( _temp571, Cyc_Absyn_starb_typ( t, _temp573,
tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp650=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp650[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp651; _temp651.tag= Cyc_Absyn_Upper_b_tag; _temp651.f1= _temp645; _temp651;});
_temp650;})), atts,({ struct Cyc_List_List* _temp652= tms; if( _temp652 == 0){
_throw( Null_Exception);} _temp652->tl;})); _LL642: return Cyc_Parse_apply_tms(
_temp571, Cyc_Absyn_tagged_typ( t, _temp573, tq), atts,({ struct Cyc_List_List*
_temp653= tms; if( _temp653 == 0){ _throw( Null_Exception);} _temp653->tl;}));
_LL636:;} _LL560: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts,
_temp577),({ struct Cyc_List_List* _temp654= tms; if( _temp654 == 0){ _throw(
Null_Exception);} _temp654->tl;})); _LL548:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp657; void* _temp659; struct _tuple5 _temp655= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL660: _temp659= _temp655.f1; goto _LL658; _LL658: _temp657=
_temp655.f2; goto _LL656; _LL656: if( _temp657 != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp661=( char*)"ignoring nested type declaration(s) in specifier list";
struct _tagged_string _temp662; _temp662.curr= _temp661; _temp662.base= _temp661;
_temp662.last_plus_one= _temp661 + 54; _temp662;}), loc);} return _temp659;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp663=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp663[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp664; _temp664.tag= Cyc_Absyn_Decl_s_tag;
_temp664.f1= d; _temp664.f2= s; _temp664;}); _temp663;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual* tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp665=( char*)"inline is only allowed on function definitions"; struct
_tagged_string _temp666; _temp666.curr= _temp665; _temp666.base= _temp665;
_temp666.last_plus_one= _temp665 + 47; _temp666;}), loc);} if( tss == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp667=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp668; _temp668.curr= _temp667; _temp668.base= _temp667;
_temp668.last_plus_one= _temp667 + 39; _temp668;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp670=( void*)({ struct Cyc_Core_Opt* _temp669= ds->sc; if(
_temp669 == 0){ _throw( Null_Exception);} _temp669->v;}); _LL672: if(( int)
_temp670 == Cyc_Parse_Typedef_sc){ goto _LL673;} else{ goto _LL674;} _LL674: if((
int) _temp670 == Cyc_Parse_Extern_sc){ goto _LL675;} else{ goto _LL676;} _LL676:
if(( int) _temp670 == Cyc_Parse_ExternC_sc){ goto _LL677;} else{ goto _LL678;}
_LL678: if(( int) _temp670 == Cyc_Parse_Static_sc){ goto _LL679;} else{ goto
_LL680;} _LL680: if(( int) _temp670 == Cyc_Parse_Auto_sc){ goto _LL681;} else{
goto _LL682;} _LL682: if(( int) _temp670 == Cyc_Parse_Register_sc){ goto _LL683;}
else{ goto _LL684;} _LL684: if(( int) _temp670 == Cyc_Parse_Abstract_sc){ goto
_LL685;} else{ goto _LL671;} _LL673: istypedef= 1; goto _LL671; _LL675: s=( void*)
Cyc_Absyn_Extern; goto _LL671; _LL677: s=( void*) Cyc_Absyn_ExternC; goto _LL671;
_LL679: s=( void*) Cyc_Absyn_Static; goto _LL671; _LL681: s=( void*) Cyc_Absyn_Public;
goto _LL671; _LL683: s=( void*) Cyc_Absyn_Public; goto _LL671; _LL685: s=( void*)
Cyc_Absyn_Abstract; goto _LL671; _LL671:;}{ struct Cyc_List_List* _temp688;
struct Cyc_List_List* _temp690; struct _tuple0 _temp686=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL691: _temp690= _temp686.f1;
goto _LL689; _LL689: _temp688= _temp686.f2; goto _LL687; _LL687: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp688; for( 0; es != 0; es=({
struct Cyc_List_List* _temp692= es; if( _temp692 == 0){ _throw( Null_Exception);}
_temp692->tl;})){ if(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp693=
es; if( _temp693 == 0){ _throw( Null_Exception);} _temp693->hd;}) != 0){
exps_empty= 0; break;}}}{ struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, loc); if( _temp690 == 0){ struct Cyc_Core_Opt* _temp696; void* _temp698;
struct _tuple5 _temp694= ts_info; _LL699: _temp698= _temp694.f1; goto _LL697;
_LL697: _temp696= _temp694.f2; goto _LL695; _LL695: if( _temp696 != 0){ struct
Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_Core_Opt* _temp700=
_temp696; if( _temp700 == 0){ _throw( Null_Exception);} _temp700->v;});{ void*
_temp701=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp715; struct Cyc_Absyn_Structdecl*
_temp717; struct Cyc_Absyn_Uniondecl* _temp719; struct Cyc_Absyn_Tuniondecl*
_temp721; struct Cyc_Absyn_XTuniondecl* _temp723; _LL703: if((( struct
_tunion_struct*) _temp701)->tag == Cyc_Absyn_Enum_d_tag){ _LL716: _temp715=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp701)->f1;
goto _LL704;} else{ goto _LL705;} _LL705: if((( struct _tunion_struct*) _temp701)->tag
== Cyc_Absyn_Struct_d_tag){ _LL718: _temp717=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp701)->f1; goto _LL706;} else{ goto
_LL707;} _LL707: if((( struct _tunion_struct*) _temp701)->tag == Cyc_Absyn_Union_d_tag){
_LL720: _temp719=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp701)->f1; goto _LL708;} else{ goto _LL709;} _LL709: if((( struct
_tunion_struct*) _temp701)->tag == Cyc_Absyn_Tunion_d_tag){ _LL722: _temp721=(
struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp701)->f1;
goto _LL710;} else{ goto _LL711;} _LL711: if((( struct _tunion_struct*) _temp701)->tag
== Cyc_Absyn_XTunion_d_tag){ _LL724: _temp723=( struct Cyc_Absyn_XTuniondecl*)((
struct Cyc_Absyn_XTunion_d_struct*) _temp701)->f1; goto _LL712;} else{ goto
_LL713;} _LL713: goto _LL714; _LL704:( void*)( _temp715->sc=( void*) s); if(
atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp725=( char*)"bad attributes on enum";
struct _tagged_string _temp726; _temp726.curr= _temp725; _temp726.base= _temp725;
_temp726.last_plus_one= _temp725 + 23; _temp726;}), loc);} goto _LL702; _LL706:(
void*)( _temp717->sc=( void*) s); _temp717->attributes= atts; goto _LL702;
_LL708:( void*)( _temp719->sc=( void*) s); _temp719->attributes= atts; goto
_LL702; _LL710:( void*)( _temp721->sc=( void*) s); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp727=( char*)"bad attributes on tunion";
struct _tagged_string _temp728; _temp728.curr= _temp727; _temp728.base= _temp727;
_temp728.last_plus_one= _temp727 + 25; _temp728;}), loc);} goto _LL702; _LL712:(
void*)( _temp723->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp729=( char*)"bad attributes on xtunion"; struct
_tagged_string _temp730; _temp730.curr= _temp729; _temp730.base= _temp729;
_temp730.last_plus_one= _temp729 + 26; _temp730;}), loc);} goto _LL702; _LL714:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp731=( char*)"bad declaration";
struct _tagged_string _temp732; _temp732.curr= _temp731; _temp732.base= _temp731;
_temp732.last_plus_one= _temp731 + 16; _temp732;}), loc); return 0; _LL702:;}
return({ struct Cyc_List_List* _temp733=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp733->hd=( void*) d; _temp733->tl= 0;
_temp733;});} else{ void* _temp734= _temp698; struct Cyc_Absyn_Structdecl**
_temp748; struct Cyc_List_List* _temp750; struct _tuple1* _temp752; struct Cyc_Absyn_TunionInfo
_temp754; struct Cyc_Absyn_Tuniondecl* _temp756; void* _temp758; struct Cyc_List_List*
_temp760; struct _tuple1* _temp762; struct Cyc_Absyn_XTunionInfo _temp764;
struct Cyc_Absyn_XTuniondecl* _temp766; void* _temp768; struct _tuple1* _temp770;
struct Cyc_Absyn_Uniondecl** _temp772; struct Cyc_List_List* _temp774; struct
_tuple1* _temp776; struct Cyc_Absyn_Enumdecl* _temp778; struct _tuple1* _temp780;
_LL736: if(( unsigned int) _temp734 > 4u?(( struct _tunion_struct*) _temp734)->tag
== Cyc_Absyn_StructType_tag: 0){ _LL753: _temp752=( struct _tuple1*)(( struct
Cyc_Absyn_StructType_struct*) _temp734)->f1; goto _LL751; _LL751: _temp750=(
struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*) _temp734)->f2;
goto _LL749; _LL749: _temp748=( struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*)
_temp734)->f3; goto _LL737;} else{ goto _LL738;} _LL738: if(( unsigned int)
_temp734 > 4u?(( struct _tunion_struct*) _temp734)->tag == Cyc_Absyn_TunionType_tag:
0){ _LL755: _temp754=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp734)->f1; _LL763: _temp762=( struct _tuple1*) _temp754.name; goto _LL761;
_LL761: _temp760=( struct Cyc_List_List*) _temp754.targs; goto _LL759; _LL759:
_temp758=( void*) _temp754.rgn; goto _LL757; _LL757: _temp756=( struct Cyc_Absyn_Tuniondecl*)
_temp754.tud; goto _LL739;} else{ goto _LL740;} _LL740: if(( unsigned int)
_temp734 > 4u?(( struct _tunion_struct*) _temp734)->tag == Cyc_Absyn_XTunionType_tag:
0){ _LL765: _temp764=( struct Cyc_Absyn_XTunionInfo)(( struct Cyc_Absyn_XTunionType_struct*)
_temp734)->f1; _LL771: _temp770=( struct _tuple1*) _temp764.name; goto _LL769;
_LL769: _temp768=( void*) _temp764.rgn; goto _LL767; _LL767: _temp766=( struct
Cyc_Absyn_XTuniondecl*) _temp764.xtud; goto _LL741;} else{ goto _LL742;} _LL742:
if(( unsigned int) _temp734 > 4u?(( struct _tunion_struct*) _temp734)->tag ==
Cyc_Absyn_UnionType_tag: 0){ _LL777: _temp776=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp734)->f1; goto _LL775; _LL775: _temp774=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp734)->f2; goto _LL773; _LL773: _temp772=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp734)->f3;
goto _LL743;} else{ goto _LL744;} _LL744: if(( unsigned int) _temp734 > 4u?((
struct _tunion_struct*) _temp734)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL781:
_temp780=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp734)->f1;
goto _LL779; _LL779: _temp778=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*)
_temp734)->f2; goto _LL745;} else{ goto _LL746;} _LL746: goto _LL747; _LL737: {
struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp750);
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp782=(
struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp782->sc=( void*) s; _temp782->name=({ struct Cyc_Core_Opt* _temp783=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp783->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp784= _temp752; if( _temp784 ==
0){ _throw( Null_Exception);} _temp784;})); _temp783;}); _temp782->tvs= ts2;
_temp782->fields= 0; _temp782->attributes= 0; _temp782;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp785=( char*)"bad attributes on struct";
struct _tagged_string _temp786; _temp786.curr= _temp785; _temp786.base= _temp785;
_temp786.last_plus_one= _temp785 + 25; _temp786;}), loc);} return({ struct Cyc_List_List*
_temp787=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp787->hd=( void*)({ struct Cyc_Absyn_Decl* _temp788=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp788->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_d_struct* _temp789=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp789[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp790; _temp790.tag= Cyc_Absyn_Struct_d_tag;
_temp790.f1= sd; _temp790;}); _temp789;})); _temp788->loc= loc; _temp788;});
_temp787->tl= 0; _temp787;});} _LL739: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp760); struct Cyc_Absyn_Decl* tud= Cyc_Absyn_tunion_decl( s,({ struct Cyc_Core_Opt*
_temp791=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp791->v=( void*)(( struct _tuple1*) _temp762); _temp791;}), ts2, 0, loc);
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp792=( char*)"bad attributes on tunion";
struct _tagged_string _temp793; _temp793.curr= _temp792; _temp793.base= _temp792;
_temp793.last_plus_one= _temp792 + 25; _temp793;}), loc);} return({ struct Cyc_List_List*
_temp794=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp794->hd=( void*) tud; _temp794->tl= 0; _temp794;});} _LL741: { struct Cyc_Absyn_XTuniondecl*
xtud=({ struct Cyc_Absyn_XTuniondecl* _temp795=( struct Cyc_Absyn_XTuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl)); _temp795->sc=( void*) s;
_temp795->name= _temp770; _temp795->fields= 0; _temp795;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp796=( char*)"bad attributes on xtunion";
struct _tagged_string _temp797; _temp797.curr= _temp796; _temp797.base= _temp796;
_temp797.last_plus_one= _temp796 + 26; _temp797;}), loc);} return({ struct Cyc_List_List*
_temp798=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp798->hd=( void*)({ struct Cyc_Absyn_Decl* _temp799=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp799->r=( void*)(( void*)({
struct Cyc_Absyn_XTunion_d_struct* _temp800=( struct Cyc_Absyn_XTunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XTunion_d_struct)); _temp800[ 0]=({ struct
Cyc_Absyn_XTunion_d_struct _temp801; _temp801.tag= Cyc_Absyn_XTunion_d_tag;
_temp801.f1= xtud; _temp801;}); _temp800;})); _temp799->loc= loc; _temp799;});
_temp798->tl= 0; _temp798;});} _LL743: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp774); struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl*
_temp802=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp802->sc=( void*) s; _temp802->name=({ struct Cyc_Core_Opt* _temp803=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp803->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp804= _temp776; if( _temp804 ==
0){ _throw( Null_Exception);} _temp804;})); _temp803;}); _temp802->tvs= ts2;
_temp802->fields= 0; _temp802->attributes= 0; _temp802;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp805=( char*)"bad attributes on union";
struct _tagged_string _temp806; _temp806.curr= _temp805; _temp806.base= _temp805;
_temp806.last_plus_one= _temp805 + 24; _temp806;}), loc);} return({ struct Cyc_List_List*
_temp807=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp807->hd=( void*)({ struct Cyc_Absyn_Decl* _temp808=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp808->r=( void*)(( void*)({
struct Cyc_Absyn_Union_d_struct* _temp809=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp809[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp810; _temp810.tag= Cyc_Absyn_Union_d_tag; _temp810.f1= ud; _temp810;});
_temp809;})); _temp808->loc= loc; _temp808;}); _temp807->tl= 0; _temp807;});}
_LL745: { struct Cyc_Absyn_Enumdecl* ed=({ struct Cyc_Absyn_Enumdecl* _temp811=(
struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp811->sc=( void*) s; _temp811->name= _temp780; _temp811->fields= 0; _temp811;});
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp812=( char*)"bad attributes on enum";
struct _tagged_string _temp813; _temp813.curr= _temp812; _temp813.base= _temp812;
_temp813.last_plus_one= _temp812 + 23; _temp813;}), loc);} return({ struct Cyc_List_List*
_temp814=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp814->hd=( void*)({ struct Cyc_Absyn_Decl* _temp815=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp815->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp816=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp816[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp817; _temp817.tag= Cyc_Absyn_Enum_d_tag; _temp817.f1= ed; _temp817;});
_temp816;})); _temp815->loc= loc; _temp815;}); _temp814->tl= 0; _temp814;});}
_LL747: Cyc_Parse_err(( struct _tagged_string)({ char* _temp818=( char*)"missing declarator";
struct _tagged_string _temp819; _temp819.curr= _temp818; _temp819.base= _temp818;
_temp819.last_plus_one= _temp818 + 19; _temp819;}), loc); return 0; _LL735:;}}
else{ void* t= ts_info.f1; struct Cyc_List_List* fields= Cyc_Parse_apply_tmss(
tq, t, _temp690, atts); if( istypedef){ if( ! exps_empty){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp820=( char*)"initializer in typedef declaration";
struct _tagged_string _temp821; _temp821.curr= _temp820; _temp821.base= _temp820;
_temp821.last_plus_one= _temp820 + 35; _temp821;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, fields); if( ts_info.f2 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_Core_Opt*
_temp822= ts_info.f2; if( _temp822 == 0){ _throw( Null_Exception);} _temp822->v;});{
void* _temp823=( void*) d->r; struct Cyc_Absyn_Structdecl* _temp837; struct Cyc_Absyn_Tuniondecl*
_temp839; struct Cyc_Absyn_XTuniondecl* _temp841; struct Cyc_Absyn_Uniondecl*
_temp843; struct Cyc_Absyn_Enumdecl* _temp845; _LL825: if((( struct
_tunion_struct*) _temp823)->tag == Cyc_Absyn_Struct_d_tag){ _LL838: _temp837=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp823)->f1;
goto _LL826;} else{ goto _LL827;} _LL827: if((( struct _tunion_struct*) _temp823)->tag
== Cyc_Absyn_Tunion_d_tag){ _LL840: _temp839=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp823)->f1; goto _LL828;} else{ goto
_LL829;} _LL829: if((( struct _tunion_struct*) _temp823)->tag == Cyc_Absyn_XTunion_d_tag){
_LL842: _temp841=( struct Cyc_Absyn_XTuniondecl*)(( struct Cyc_Absyn_XTunion_d_struct*)
_temp823)->f1; goto _LL830;} else{ goto _LL831;} _LL831: if((( struct
_tunion_struct*) _temp823)->tag == Cyc_Absyn_Union_d_tag){ _LL844: _temp843=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp823)->f1;
goto _LL832;} else{ goto _LL833;} _LL833: if((( struct _tunion_struct*) _temp823)->tag
== Cyc_Absyn_Enum_d_tag){ _LL846: _temp845=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp823)->f1; goto _LL834;} else{ goto _LL835;}
_LL835: goto _LL836; _LL826:( void*)( _temp837->sc=( void*) s); _temp837->attributes=
atts; atts= 0; goto _LL824; _LL828:( void*)( _temp839->sc=( void*) s); goto
_LL824; _LL830:( void*)( _temp841->sc=( void*) s); goto _LL824; _LL832:( void*)(
_temp843->sc=( void*) s); goto _LL824; _LL834:( void*)( _temp845->sc=( void*) s);
goto _LL824; _LL836: Cyc_Parse_err(( struct _tagged_string)({ char* _temp847=(
char*)"declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp848; _temp848.curr= _temp847; _temp848.base= _temp847;
_temp848.last_plus_one= _temp847 + 70; _temp848;}), loc); goto _LL824; _LL824:;}
decls=({ struct Cyc_List_List* _temp849=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp849->hd=( void*) d; _temp849->tl= decls;
_temp849;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp851=
Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp850= atts; if(
_temp850 == 0){ _throw( Null_Exception);} _temp850->hd;})); xprintf("bad attribute %.*s in typedef",
_temp851.last_plus_one - _temp851.curr, _temp851.curr);}), loc);} return decls;}}
else{ if( ts_info.f2 != 0){ Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp852=( char*)"nested type declaration within declarator"; struct
_tagged_string _temp853; _temp853.curr= _temp852; _temp853.base= _temp852;
_temp853.last_plus_one= _temp852 + 42; _temp853;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* ds= fields; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp854= ds; if( _temp854 == 0){ _throw( Null_Exception);} _temp854->tl;}),
_temp688=({ struct Cyc_List_List* _temp855= _temp688; if( _temp855 == 0){ _throw(
Null_Exception);} _temp855->tl;})){ struct _tuple7 _temp859; struct Cyc_List_List*
_temp860; struct Cyc_List_List* _temp862; void* _temp864; struct Cyc_Absyn_Tqual*
_temp866; struct _tuple1* _temp868; struct _tuple7* _temp857=( struct _tuple7*)({
struct Cyc_List_List* _temp856= ds; if( _temp856 == 0){ _throw( Null_Exception);}
_temp856->hd;}); _temp859=* _temp857; _LL869: _temp868= _temp859.f1; goto _LL867;
_LL867: _temp866= _temp859.f2; goto _LL865; _LL865: _temp864= _temp859.f3; goto
_LL863; _LL863: _temp862= _temp859.f4; goto _LL861; _LL861: _temp860= _temp859.f5;
goto _LL858; _LL858: if( _temp862 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp870=( char*)"bad type params, ignoring"; struct _tagged_string
_temp871; _temp871.curr= _temp870; _temp871.base= _temp870; _temp871.last_plus_one=
_temp870 + 26; _temp871;}), loc);} if( _temp688 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp872=( char*)"unexpected null in parse!"; struct
_tagged_string _temp873; _temp873.curr= _temp872; _temp873.base= _temp872;
_temp873.last_plus_one= _temp872 + 26; _temp873;}), loc);}{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp874= _temp688; if(
_temp874 == 0){ _throw( Null_Exception);} _temp874->hd;}); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_new_vardecl( _temp868, _temp864, eopt); vd->tq= _temp866;( void*)(
vd->sc=( void*) s); vd->attributes= _temp860;{ struct Cyc_Absyn_Decl* d=({
struct Cyc_Absyn_Decl* _temp875=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp875->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp876=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp876[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp877; _temp877.tag= Cyc_Absyn_Var_d_tag;
_temp877.f1= vd; _temp877;}); _temp876;})); _temp875->loc= loc; _temp875;});
decls=({ struct Cyc_List_List* _temp878=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp878->hd=( void*) d; _temp878->tl= decls;
_temp878;});}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind( struct
_tagged_string s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen( s)
!= 1){ Cyc_Parse_err(({ struct _tagged_string _temp879= s; xprintf("bad kind: %.*s",
_temp879.last_plus_one - _temp879.curr, _temp879.curr);}), loc); return( void*)
Cyc_Absyn_BoxKind;} else{ switch(({ struct _tagged_string _temp880= s; char*
_temp882= _temp880.curr + 0; if( _temp880.base == 0? 1:( _temp882 < _temp880.base?
1: _temp882 >= _temp880.last_plus_one)){ _throw( Null_Exception);}* _temp882;})){
case 'A': _LL883: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL884: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL885: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL886: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL887: return(
void*) Cyc_Absyn_EffKind; default: _LL888: Cyc_Parse_err(({ struct
_tagged_string _temp890= s; xprintf("bad kind: %.*s", _temp890.last_plus_one -
_temp890.curr, _temp890.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp891=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp891->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp892=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp892[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp893; _temp893.tag= Cyc_Absyn_Attributes_mod_tag;
_temp893.f1= loc; _temp893.f2= atts; _temp893;}); _temp892;})); _temp891->tl=
tms; _temp891;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(({ struct
_tagged_string _temp895= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp894=(* t).f5; if( _temp894 == 0){ _throw( Null_Exception);} _temp894->hd;}));
xprintf("bad attribute %.*s within typedef", _temp895.last_plus_one - _temp895.curr,
_temp895.curr);}), loc);} return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp896=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp896[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp897; _temp897.tag= Cyc_Absyn_Typedef_d_tag;
_temp897.f1=({ struct Cyc_Absyn_Typedefdecl* _temp898=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp898->name= x; _temp898->tvs=(*
t).f4; _temp898->defn=( void*)(* t).f3; _temp898;}); _temp897;}); _temp896;}),
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
char* tag; struct Cyc_List_List* f1; } ; static char _temp901[ 8u]="Int_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp901, _temp901, _temp901 + 8u}}; struct _tuple12*
Cyc_yyget_Int_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp902= yy1; struct _tuple12* _temp908; _LL904: if((*(( struct _xtunion_struct*)
_temp902)).tag == Cyc_Int_tok_tag){ _LL909: _temp908=(( struct Cyc_Int_tok_struct*)
_temp902)->f1; goto _LL905;} else{ goto _LL906;} _LL906: goto _LL907; _LL905:
return _temp908; _LL907:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Int_tok);
_LL903:;} static char _temp912[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp912,
_temp912, _temp912 + 11u}}; struct _tagged_string Cyc_yyget_String_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp913= yy1; struct
_tagged_string _temp919; _LL915: if((*(( struct _xtunion_struct*) _temp913)).tag
== Cyc_String_tok_tag){ _LL920: _temp919=(( struct Cyc_String_tok_struct*)
_temp913)->f1; goto _LL916;} else{ goto _LL917;} _LL917: goto _LL918; _LL916:
return _temp919; _LL918:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_String_tok);
_LL914:;} static char _temp923[ 9u]="Char_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp923,
_temp923, _temp923 + 9u}}; char Cyc_yyget_Char_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp924= yy1; char _temp930; _LL926: if((*(( struct
_xtunion_struct*) _temp924)).tag == Cyc_Char_tok_tag){ _LL931: _temp930=((
struct Cyc_Char_tok_struct*) _temp924)->f1; goto _LL927;} else{ goto _LL928;}
_LL928: goto _LL929; _LL927: return _temp930; _LL929:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Char_tok); _LL925:;} static char _temp934[ 17u]="Pointer_Sort_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp934, _temp934, _temp934 + 17u}}; void* Cyc_yyget_Pointer_Sort_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp935= yy1; void*
_temp941; _LL937: if((*(( struct _xtunion_struct*) _temp935)).tag == Cyc_Pointer_Sort_tok_tag){
_LL942: _temp941=(( struct Cyc_Pointer_Sort_tok_struct*) _temp935)->f1; goto
_LL938;} else{ goto _LL939;} _LL939: goto _LL940; _LL938: return _temp941;
_LL940:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Pointer_Sort_tok);
_LL936:;} static char _temp945[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp945,
_temp945, _temp945 + 8u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp946= yy1; struct Cyc_Absyn_Exp*
_temp952; _LL948: if((*(( struct _xtunion_struct*) _temp946)).tag == Cyc_Exp_tok_tag){
_LL953: _temp952=(( struct Cyc_Exp_tok_struct*) _temp946)->f1; goto _LL949;}
else{ goto _LL950;} _LL950: goto _LL951; _LL949: return _temp952; _LL951:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_Exp_tok); _LL947:;} static char
_temp956[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ExpList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp956, _temp956, _temp956 + 12u}};
struct Cyc_List_List* Cyc_yyget_ExpList_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp957= yy1; struct Cyc_List_List* _temp963; _LL959:
if((*(( struct _xtunion_struct*) _temp957)).tag == Cyc_ExpList_tok_tag){ _LL964:
_temp963=(( struct Cyc_ExpList_tok_struct*) _temp957)->f1; goto _LL960;} else{
goto _LL961;} _LL961: goto _LL962; _LL960: return _temp963; _LL962:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_ExpList_tok); _LL958:;} static
char _temp967[ 20u]="InitializerList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp967, _temp967, _temp967 + 20u}}; struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp968= yy1; struct Cyc_List_List*
_temp974; _LL970: if((*(( struct _xtunion_struct*) _temp968)).tag == Cyc_InitializerList_tok_tag){
_LL975: _temp974=(( struct Cyc_InitializerList_tok_struct*) _temp968)->f1; goto
_LL971;} else{ goto _LL972;} _LL972: goto _LL973; _LL971: return _temp974;
_LL973:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_InitializerList_tok);
_LL969:;} static char _temp978[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp978,
_temp978, _temp978 + 11u}}; void* Cyc_yyget_Primop_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp979= yy1; void* _temp985; _LL981: if((*((
struct _xtunion_struct*) _temp979)).tag == Cyc_Primop_tok_tag){ _LL986: _temp985=((
struct Cyc_Primop_tok_struct*) _temp979)->f1; goto _LL982;} else{ goto _LL983;}
_LL983: goto _LL984; _LL982: return _temp985; _LL984:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Primop_tok); _LL980:;} static char _temp989[ 14u]="Primopopt_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp989, _temp989, _temp989 + 14u}}; struct Cyc_Core_Opt*
Cyc_yyget_Primopopt_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp990= yy1; struct Cyc_Core_Opt* _temp996; _LL992: if((*(( struct
_xtunion_struct*) _temp990)).tag == Cyc_Primopopt_tok_tag){ _LL997: _temp996=((
struct Cyc_Primopopt_tok_struct*) _temp990)->f1; goto _LL993;} else{ goto _LL994;}
_LL994: goto _LL995; _LL993: return _temp996; _LL995:( void) _throw(( struct
_xtunion_struct*)& Cyc_yyfail_Primopopt_tok); _LL991:;} static char _temp1000[
11u]="QualId_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1000, _temp1000, _temp1000 +
11u}}; struct _tuple1* Cyc_yyget_QualId_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp1001= yy1; struct _tuple1* _temp1007; _LL1003: if((*((
struct _xtunion_struct*) _temp1001)).tag == Cyc_QualId_tok_tag){ _LL1008:
_temp1007=(( struct Cyc_QualId_tok_struct*) _temp1001)->f1; goto _LL1004;} else{
goto _LL1005;} _LL1005: goto _LL1006; _LL1004: return _temp1007; _LL1006:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_QualId_tok); _LL1002:;} static
char _temp1011[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1011, _temp1011, _temp1011 +
9u}}; struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp1012= yy1; struct Cyc_Absyn_Stmt* _temp1018;
_LL1014: if((*(( struct _xtunion_struct*) _temp1012)).tag == Cyc_Stmt_tok_tag){
_LL1019: _temp1018=(( struct Cyc_Stmt_tok_struct*) _temp1012)->f1; goto _LL1015;}
else{ goto _LL1016;} _LL1016: goto _LL1017; _LL1015: return _temp1018; _LL1017:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Stmt_tok); _LL1013:;} static
char _temp1022[ 14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_BlockItem_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1022, _temp1022, _temp1022 +
14u}}; void* Cyc_yyget_BlockItem_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1023= yy1; void* _temp1029; _LL1025: if((*(( struct
_xtunion_struct*) _temp1023)).tag == Cyc_BlockItem_tok_tag){ _LL1030: _temp1029=((
struct Cyc_BlockItem_tok_struct*) _temp1023)->f1; goto _LL1026;} else{ goto
_LL1027;} _LL1027: goto _LL1028; _LL1026: return _temp1029; _LL1028:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_BlockItem_tok); _LL1024:;} static
char _temp1033[ 21u]="SwitchClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1033, _temp1033, _temp1033 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1034= yy1; struct Cyc_List_List*
_temp1040; _LL1036: if((*(( struct _xtunion_struct*) _temp1034)).tag == Cyc_SwitchClauseList_tok_tag){
_LL1041: _temp1040=(( struct Cyc_SwitchClauseList_tok_struct*) _temp1034)->f1;
goto _LL1037;} else{ goto _LL1038;} _LL1038: goto _LL1039; _LL1037: return
_temp1040; _LL1039:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_SwitchClauseList_tok);
_LL1035:;} static char _temp1044[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pattern_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1044, _temp1044, _temp1044 + 12u}}; struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1045= yy1; struct Cyc_Absyn_Pat*
_temp1051; _LL1047: if((*(( struct _xtunion_struct*) _temp1045)).tag == Cyc_Pattern_tok_tag){
_LL1052: _temp1051=(( struct Cyc_Pattern_tok_struct*) _temp1045)->f1; goto
_LL1048;} else{ goto _LL1049;} _LL1049: goto _LL1050; _LL1048: return _temp1051;
_LL1050:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Pattern_tok);
_LL1046:;} static char _temp1055[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1055, _temp1055, _temp1055 + 16u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1056= yy1; struct Cyc_List_List*
_temp1062; _LL1058: if((*(( struct _xtunion_struct*) _temp1056)).tag == Cyc_PatternList_tok_tag){
_LL1063: _temp1062=(( struct Cyc_PatternList_tok_struct*) _temp1056)->f1; goto
_LL1059;} else{ goto _LL1060;} _LL1060: goto _LL1061; _LL1059: return _temp1062;
_LL1061:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_PatternList_tok);
_LL1057:;} static char _temp1066[ 17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FieldPattern_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1066, _temp1066, _temp1066 + 17u}}; struct _tuple17* Cyc_yyget_FieldPattern_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1067= yy1; struct
_tuple17* _temp1073; _LL1069: if((*(( struct _xtunion_struct*) _temp1067)).tag
== Cyc_FieldPattern_tok_tag){ _LL1074: _temp1073=(( struct Cyc_FieldPattern_tok_struct*)
_temp1067)->f1; goto _LL1070;} else{ goto _LL1071;} _LL1071: goto _LL1072;
_LL1070: return _temp1073; _LL1072:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_FieldPattern_tok); _LL1068:;} static char _temp1077[ 21u]="FieldPatternList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1077, _temp1077, _temp1077 + 21u}}; struct Cyc_List_List*
Cyc_yyget_FieldPatternList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1078= yy1; struct Cyc_List_List* _temp1084; _LL1080: if((*((
struct _xtunion_struct*) _temp1078)).tag == Cyc_FieldPatternList_tok_tag){
_LL1085: _temp1084=(( struct Cyc_FieldPatternList_tok_struct*) _temp1078)->f1;
goto _LL1081;} else{ goto _LL1082;} _LL1082: goto _LL1083; _LL1081: return
_temp1084; _LL1083:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_FieldPatternList_tok);
_LL1079:;} static char _temp1088[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FnDecl_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1088,
_temp1088, _temp1088 + 11u}}; struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1089= yy1; struct Cyc_Absyn_Fndecl*
_temp1095; _LL1091: if((*(( struct _xtunion_struct*) _temp1089)).tag == Cyc_FnDecl_tok_tag){
_LL1096: _temp1095=(( struct Cyc_FnDecl_tok_struct*) _temp1089)->f1; goto
_LL1092;} else{ goto _LL1093;} _LL1093: goto _LL1094; _LL1092: return _temp1095;
_LL1094:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_FnDecl_tok);
_LL1090:;} static char _temp1099[ 13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1099, _temp1099, _temp1099 + 13u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1100= yy1; struct Cyc_List_List*
_temp1106; _LL1102: if((*(( struct _xtunion_struct*) _temp1100)).tag == Cyc_DeclList_tok_tag){
_LL1107: _temp1106=(( struct Cyc_DeclList_tok_struct*) _temp1100)->f1; goto
_LL1103;} else{ goto _LL1104;} _LL1104: goto _LL1105; _LL1103: return _temp1106;
_LL1105:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DeclList_tok);
_LL1101:;} static char _temp1110[ 13u]="DeclSpec_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1110, _temp1110, _temp1110 + 13u}}; struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1111= yy1; struct Cyc_Parse_Declaration_spec*
_temp1117; _LL1113: if((*(( struct _xtunion_struct*) _temp1111)).tag == Cyc_DeclSpec_tok_tag){
_LL1118: _temp1117=(( struct Cyc_DeclSpec_tok_struct*) _temp1111)->f1; goto
_LL1114;} else{ goto _LL1115;} _LL1115: goto _LL1116; _LL1114: return _temp1117;
_LL1116:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DeclSpec_tok);
_LL1112:;} static char _temp1121[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1121, _temp1121, _temp1121 + 13u}}; struct _tuple13* Cyc_yyget_InitDecl_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1122= yy1; struct
_tuple13* _temp1128; _LL1124: if((*(( struct _xtunion_struct*) _temp1122)).tag
== Cyc_InitDecl_tok_tag){ _LL1129: _temp1128=(( struct Cyc_InitDecl_tok_struct*)
_temp1122)->f1; goto _LL1125;} else{ goto _LL1126;} _LL1126: goto _LL1127;
_LL1125: return _temp1128; _LL1127:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_InitDecl_tok); _LL1123:;} static char _temp1132[ 17u]="InitDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1132, _temp1132, _temp1132 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp1133= yy1; struct Cyc_List_List* _temp1139; _LL1135: if((*(( struct
_xtunion_struct*) _temp1133)).tag == Cyc_InitDeclList_tok_tag){ _LL1140:
_temp1139=(( struct Cyc_InitDeclList_tok_struct*) _temp1133)->f1; goto _LL1136;}
else{ goto _LL1137;} _LL1137: goto _LL1138; _LL1136: return _temp1139; _LL1138:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_InitDeclList_tok); _LL1134:;}
static char _temp1143[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1143, _temp1143, _temp1143 + 17u}}; void* Cyc_yyget_StorageClass_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1144= yy1; void*
_temp1150; _LL1146: if((*(( struct _xtunion_struct*) _temp1144)).tag == Cyc_StorageClass_tok_tag){
_LL1151: _temp1150=(( struct Cyc_StorageClass_tok_struct*) _temp1144)->f1; goto
_LL1147;} else{ goto _LL1148;} _LL1148: goto _LL1149; _LL1147: return _temp1150;
_LL1149:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StorageClass_tok);
_LL1145:;} static char _temp1154[ 18u]="TypeSpecifier_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1154, _temp1154, _temp1154 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1155= yy1; void*
_temp1161; _LL1157: if((*(( struct _xtunion_struct*) _temp1155)).tag == Cyc_TypeSpecifier_tok_tag){
_LL1162: _temp1161=(( struct Cyc_TypeSpecifier_tok_struct*) _temp1155)->f1; goto
_LL1158;} else{ goto _LL1159;} _LL1159: goto _LL1160; _LL1158: return _temp1161;
_LL1160:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1156:;} static char _temp1165[ 18u]="StructOrUnion_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1165, _temp1165, _temp1165 + 18u}}; void* Cyc_yyget_StructOrUnion_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1166= yy1; void*
_temp1172; _LL1168: if((*(( struct _xtunion_struct*) _temp1166)).tag == Cyc_StructOrUnion_tok_tag){
_LL1173: _temp1172=(( struct Cyc_StructOrUnion_tok_struct*) _temp1166)->f1; goto
_LL1169;} else{ goto _LL1170;} _LL1170: goto _LL1171; _LL1169: return _temp1172;
_LL1171:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StructOrUnion_tok);
_LL1167:;} static char _temp1176[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1176, _temp1176, _temp1176 + 13u}}; struct Cyc_Absyn_Tqual* Cyc_yyget_TypeQual_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1177= yy1; struct Cyc_Absyn_Tqual*
_temp1183; _LL1179: if((*(( struct _xtunion_struct*) _temp1177)).tag == Cyc_TypeQual_tok_tag){
_LL1184: _temp1183=(( struct Cyc_TypeQual_tok_struct*) _temp1177)->f1; goto
_LL1180;} else{ goto _LL1181;} _LL1181: goto _LL1182; _LL1180: return _temp1183;
_LL1182:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeQual_tok);
_LL1178:;} static char _temp1187[ 24u]="StructFieldDeclList_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1187, _temp1187, _temp1187 + 24u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1188= yy1; struct Cyc_List_List* _temp1194; _LL1190: if((*((
struct _xtunion_struct*) _temp1188)).tag == Cyc_StructFieldDeclList_tok_tag){
_LL1195: _temp1194=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp1188)->f1;
goto _LL1191;} else{ goto _LL1192;} _LL1192: goto _LL1193; _LL1191: return
_temp1194; _LL1193:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL1189:;} static char _temp1198[ 28u]="StructFieldDeclListList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1198, _temp1198, _temp1198 + 28u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1199= yy1; struct Cyc_List_List* _temp1205; _LL1201: if((*((
struct _xtunion_struct*) _temp1199)).tag == Cyc_StructFieldDeclListList_tok_tag){
_LL1206: _temp1205=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp1199)->f1;
goto _LL1202;} else{ goto _LL1203;} _LL1203: goto _LL1204; _LL1202: return
_temp1205; _LL1204:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL1200:;} static char _temp1209[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1209, _temp1209, _temp1209 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1210= yy1; struct Cyc_List_List*
_temp1216; _LL1212: if((*(( struct _xtunion_struct*) _temp1210)).tag == Cyc_TypeModifierList_tok_tag){
_LL1217: _temp1216=(( struct Cyc_TypeModifierList_tok_struct*) _temp1210)->f1;
goto _LL1213;} else{ goto _LL1214;} _LL1214: goto _LL1215; _LL1213: return
_temp1216; _LL1215:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeModifierList_tok);
_LL1211:;} static char _temp1220[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Rgn_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1220,
_temp1220, _temp1220 + 8u}}; void* Cyc_yyget_Rgn_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1221= yy1; void* _temp1227; _LL1223: if((*((
struct _xtunion_struct*) _temp1221)).tag == Cyc_Rgn_tok_tag){ _LL1228: _temp1227=((
struct Cyc_Rgn_tok_struct*) _temp1221)->f1; goto _LL1224;} else{ goto _LL1225;}
_LL1225: goto _LL1226; _LL1224: return _temp1227; _LL1226:( void) _throw((
struct _xtunion_struct*)& Cyc_yyfail_Rgn_tok); _LL1222:;} static char _temp1231[
15u]="Declarator_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1231, _temp1231, _temp1231 +
15u}}; struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp1232= yy1; struct Cyc_Parse_Declarator*
_temp1238; _LL1234: if((*(( struct _xtunion_struct*) _temp1232)).tag == Cyc_Declarator_tok_tag){
_LL1239: _temp1238=(( struct Cyc_Declarator_tok_struct*) _temp1232)->f1; goto
_LL1235;} else{ goto _LL1236;} _LL1236: goto _LL1237; _LL1235: return _temp1238;
_LL1237:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Declarator_tok);
_LL1233:;} static char _temp1242[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1242, _temp1242, _temp1242 + 21u}}; struct _tuple15* Cyc_yyget_DeclaratorExpopt_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1243= yy1; struct
_tuple15* _temp1249; _LL1245: if((*(( struct _xtunion_struct*) _temp1243)).tag
== Cyc_DeclaratorExpopt_tok_tag){ _LL1250: _temp1249=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1243)->f1; goto _LL1246;} else{ goto _LL1247;} _LL1247: goto _LL1248;
_LL1246: return _temp1249; _LL1248:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_DeclaratorExpopt_tok); _LL1244:;} static char _temp1253[ 25u]="DeclaratorExpoptList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1253, _temp1253, _temp1253 + 25u}}; struct Cyc_List_List*
Cyc_yyget_DeclaratorExpoptList_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1254= yy1; struct Cyc_List_List* _temp1260; _LL1256: if((*((
struct _xtunion_struct*) _temp1254)).tag == Cyc_DeclaratorExpoptList_tok_tag){
_LL1261: _temp1260=(( struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1254)->f1;
goto _LL1257;} else{ goto _LL1258;} _LL1258: goto _LL1259; _LL1257: return
_temp1260; _LL1259:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DeclaratorExpoptList_tok);
_LL1255:;} static char _temp1264[ 23u]="AbstractDeclarator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1264, _temp1264, _temp1264 + 23u}}; struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_AbstractDeclarator_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1265= yy1; struct Cyc_Parse_Abstractdeclarator* _temp1271;
_LL1267: if((*(( struct _xtunion_struct*) _temp1265)).tag == Cyc_AbstractDeclarator_tok_tag){
_LL1272: _temp1271=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1265)->f1;
goto _LL1268;} else{ goto _LL1269;} _LL1269: goto _LL1270; _LL1268: return
_temp1271; _LL1270:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL1266:;} static char _temp1275[ 16u]="TunionField_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1275, _temp1275, _temp1275 + 16u}}; struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1276= yy1; struct Cyc_Absyn_Tunionfield*
_temp1282; _LL1278: if((*(( struct _xtunion_struct*) _temp1276)).tag == Cyc_TunionField_tok_tag){
_LL1283: _temp1282=(( struct Cyc_TunionField_tok_struct*) _temp1276)->f1; goto
_LL1279;} else{ goto _LL1280;} _LL1280: goto _LL1281; _LL1279: return _temp1282;
_LL1281:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TunionField_tok);
_LL1277:;} static char _temp1286[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1286, _temp1286, _temp1286 + 20u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1287= yy1; struct Cyc_List_List*
_temp1293; _LL1289: if((*(( struct _xtunion_struct*) _temp1287)).tag == Cyc_TunionFieldList_tok_tag){
_LL1294: _temp1293=(( struct Cyc_TunionFieldList_tok_struct*) _temp1287)->f1;
goto _LL1290;} else{ goto _LL1291;} _LL1291: goto _LL1292; _LL1290: return
_temp1293; _LL1292:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TunionFieldList_tok);
_LL1288:;} static char _temp1297[ 17u]="QualSpecList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1297, _temp1297, _temp1297 + 17u}}; struct _tuple14* Cyc_yyget_QualSpecList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1298= yy1; struct
_tuple14* _temp1304; _LL1300: if((*(( struct _xtunion_struct*) _temp1298)).tag
== Cyc_QualSpecList_tok_tag){ _LL1305: _temp1304=(( struct Cyc_QualSpecList_tok_struct*)
_temp1298)->f1; goto _LL1301;} else{ goto _LL1302;} _LL1302: goto _LL1303;
_LL1301: return _temp1304; _LL1303:( void) _throw(( struct _xtunion_struct*)&
Cyc_yyfail_QualSpecList_tok); _LL1299:;} static char _temp1308[ 11u]="IdList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1308, _temp1308, _temp1308 + 11u}}; struct Cyc_List_List*
Cyc_yyget_IdList_tok( struct _xtunion_struct* yy1){ struct _xtunion_struct*
_temp1309= yy1; struct Cyc_List_List* _temp1315; _LL1311: if((*(( struct
_xtunion_struct*) _temp1309)).tag == Cyc_IdList_tok_tag){ _LL1316: _temp1315=((
struct Cyc_IdList_tok_struct*) _temp1309)->f1; goto _LL1312;} else{ goto _LL1313;}
_LL1313: goto _LL1314; _LL1312: return _temp1315; _LL1314:( void) _throw((
struct _xtunion_struct*)& Cyc_yyfail_IdList_tok); _LL1310:;} static char
_temp1319[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1319, _temp1319, _temp1319 +
14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( struct _xtunion_struct* yy1){
struct _xtunion_struct* _temp1320= yy1; struct _tuple2* _temp1326; _LL1322: if((*((
struct _xtunion_struct*) _temp1320)).tag == Cyc_ParamDecl_tok_tag){ _LL1327:
_temp1326=(( struct Cyc_ParamDecl_tok_struct*) _temp1320)->f1; goto _LL1323;}
else{ goto _LL1324;} _LL1324: goto _LL1325; _LL1323: return _temp1326; _LL1325:(
void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_ParamDecl_tok); _LL1321:;}
static char _temp1330[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1330, _temp1330, _temp1330 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1331= yy1; struct Cyc_List_List*
_temp1337; _LL1333: if((*(( struct _xtunion_struct*) _temp1331)).tag == Cyc_ParamDeclList_tok_tag){
_LL1338: _temp1337=(( struct Cyc_ParamDeclList_tok_struct*) _temp1331)->f1; goto
_LL1334;} else{ goto _LL1335;} _LL1335: goto _LL1336; _LL1334: return _temp1337;
_LL1336:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_ParamDeclList_tok);
_LL1332:;} static char _temp1341[ 22u]="ParamDeclListBool_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure_tag,(
struct _tagged_string){ _temp1341, _temp1341, _temp1341 + 22u}}; struct _tuple16*
Cyc_yyget_ParamDeclListBool_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1342= yy1; struct _tuple16* _temp1348; _LL1344: if((*((
struct _xtunion_struct*) _temp1342)).tag == Cyc_ParamDeclListBool_tok_tag){
_LL1349: _temp1348=(( struct Cyc_ParamDeclListBool_tok_struct*) _temp1342)->f1;
goto _LL1345;} else{ goto _LL1346;} _LL1346: goto _LL1347; _LL1345: return
_temp1348; _LL1347:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL1343:;} static char _temp1352[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1352, _temp1352, _temp1352 + 13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1353= yy1; struct Cyc_List_List*
_temp1359; _LL1355: if((*(( struct _xtunion_struct*) _temp1353)).tag == Cyc_TypeList_tok_tag){
_LL1360: _temp1359=(( struct Cyc_TypeList_tok_struct*) _temp1353)->f1; goto
_LL1356;} else{ goto _LL1357;} _LL1357: goto _LL1358; _LL1356: return _temp1359;
_LL1358:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_TypeList_tok);
_LL1354:;} static char _temp1363[ 19u]="DesignatorList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1363, _temp1363, _temp1363 + 19u}}; struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1364= yy1; struct Cyc_List_List*
_temp1370; _LL1366: if((*(( struct _xtunion_struct*) _temp1364)).tag == Cyc_DesignatorList_tok_tag){
_LL1371: _temp1370=(( struct Cyc_DesignatorList_tok_struct*) _temp1364)->f1;
goto _LL1367;} else{ goto _LL1368;} _LL1368: goto _LL1369; _LL1367: return
_temp1370; _LL1369:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_DesignatorList_tok);
_LL1365:;} static char _temp1374[ 15u]="Designator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1374, _temp1374, _temp1374 + 15u}}; void* Cyc_yyget_Designator_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp1375= yy1; void* _temp1381;
_LL1377: if((*(( struct _xtunion_struct*) _temp1375)).tag == Cyc_Designator_tok_tag){
_LL1382: _temp1381=(( struct Cyc_Designator_tok_struct*) _temp1375)->f1; goto
_LL1378;} else{ goto _LL1379;} _LL1379: goto _LL1380; _LL1378: return _temp1381;
_LL1380:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Designator_tok);
_LL1376:;} static char _temp1385[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Kind_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1385,
_temp1385, _temp1385 + 9u}}; void* Cyc_yyget_Kind_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1386= yy1; void* _temp1392; _LL1388: if((*((
struct _xtunion_struct*) _temp1386)).tag == Cyc_Kind_tok_tag){ _LL1393:
_temp1392=(( struct Cyc_Kind_tok_struct*) _temp1386)->f1; goto _LL1389;} else{
goto _LL1390;} _LL1390: goto _LL1391; _LL1389: return _temp1392; _LL1391:( void)
_throw(( struct _xtunion_struct*)& Cyc_yyfail_Kind_tok); _LL1387:;} static char
_temp1396[ 9u]="Type_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1396, _temp1396, _temp1396 +
9u}}; void* Cyc_yyget_Type_tok( struct _xtunion_struct* yy1){ struct
_xtunion_struct* _temp1397= yy1; void* _temp1403; _LL1399: if((*(( struct
_xtunion_struct*) _temp1397)).tag == Cyc_Type_tok_tag){ _LL1404: _temp1403=((
struct Cyc_Type_tok_struct*) _temp1397)->f1; goto _LL1400;} else{ goto _LL1401;}
_LL1401: goto _LL1402; _LL1400: return _temp1403; _LL1402:( void) _throw((
struct _xtunion_struct*)& Cyc_yyfail_Type_tok); _LL1398:;} static char _temp1407[
18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_AttributeList_tok={
Cyc_Core_Failure_tag,( struct _tagged_string){ _temp1407, _temp1407, _temp1407 +
18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok( struct _xtunion_struct*
yy1){ struct _xtunion_struct* _temp1408= yy1; struct Cyc_List_List* _temp1414;
_LL1410: if((*(( struct _xtunion_struct*) _temp1408)).tag == Cyc_AttributeList_tok_tag){
_LL1415: _temp1414=(( struct Cyc_AttributeList_tok_struct*) _temp1408)->f1; goto
_LL1411;} else{ goto _LL1412;} _LL1412: goto _LL1413; _LL1411: return _temp1414;
_LL1413:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_AttributeList_tok);
_LL1409:;} static char _temp1418[ 14u]="Attribute_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Attribute_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1418, _temp1418, _temp1418 + 14u}}; void* Cyc_yyget_Attribute_tok( struct
_xtunion_struct* yy1){ struct _xtunion_struct* _temp1419= yy1; void* _temp1425;
_LL1421: if((*(( struct _xtunion_struct*) _temp1419)).tag == Cyc_Attribute_tok_tag){
_LL1426: _temp1425=(( struct Cyc_Attribute_tok_struct*) _temp1419)->f1; goto
_LL1422;} else{ goto _LL1423;} _LL1423: goto _LL1424; _LL1422: return _temp1425;
_LL1424:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Attribute_tok);
_LL1420:;} static char _temp1429[ 14u]="Enumfield_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1429, _temp1429, _temp1429 + 14u}}; struct Cyc_Absyn_Enumfield* Cyc_yyget_Enumfield_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1430= yy1; struct Cyc_Absyn_Enumfield*
_temp1436; _LL1432: if((*(( struct _xtunion_struct*) _temp1430)).tag == Cyc_Enumfield_tok_tag){
_LL1437: _temp1436=(( struct Cyc_Enumfield_tok_struct*) _temp1430)->f1; goto
_LL1433;} else{ goto _LL1434;} _LL1434: goto _LL1435; _LL1433: return _temp1436;
_LL1435:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_Enumfield_tok);
_LL1431:;} static char _temp1440[ 18u]="EnumfieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure_tag,( struct _tagged_string){
_temp1440, _temp1440, _temp1440 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
struct _xtunion_struct* yy1){ struct _xtunion_struct* _temp1441= yy1; struct Cyc_List_List*
_temp1447; _LL1443: if((*(( struct _xtunion_struct*) _temp1441)).tag == Cyc_EnumfieldList_tok_tag){
_LL1448: _temp1447=(( struct Cyc_EnumfieldList_tok_struct*) _temp1441)->f1; goto
_LL1444;} else{ goto _LL1445;} _LL1445: goto _LL1446; _LL1444: return _temp1447;
_LL1446:( void) _throw(( struct _xtunion_struct*)& Cyc_yyfail_EnumfieldList_tok);
_LL1442:;} struct Cyc_Yyltype{ int timestamp; int first_line; int first_column;
int last_line; int last_column; struct _tagged_string text; } ; typedef struct
Cyc_Yyltype Cyc_yyltype; struct Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype
_temp1449; _temp1449.timestamp= 0; _temp1449.first_line= 0; _temp1449.first_column=
0; _temp1449.last_line= 0; _temp1449.last_column= 0; _temp1449.text=( struct
_tagged_string)({ char* _temp1450=( char*)""; struct _tagged_string _temp1451;
_temp1451.curr= _temp1450; _temp1451.base= _temp1450; _temp1451.last_plus_one=
_temp1450 + 1; _temp1451;}); _temp1449;});} static char _temp1454[ 1u]="";
struct Cyc_Yyltype Cyc_yylloc=( struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column=
0,.last_line= 0,.last_column= 0,.text=( struct _tagged_string){ _temp1454,
_temp1454, _temp1454 + 1u}}; static short Cyc_yytranslate[ 348u]={ (short)0,
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
(short)89, (short)90, (short)91, (short)92}; static char _temp1457[ 2u]="$";
static char _temp1460[ 6u]="error"; static char _temp1463[ 12u]="$undefined.";
static char _temp1466[ 5u]="AUTO"; static char _temp1469[ 9u]="REGISTER"; static
char _temp1472[ 7u]="STATIC"; static char _temp1475[ 7u]="EXTERN"; static char
_temp1478[ 8u]="TYPEDEF"; static char _temp1481[ 5u]="VOID"; static char
_temp1484[ 5u]="CHAR"; static char _temp1487[ 6u]="SHORT"; static char _temp1490[
4u]="INT"; static char _temp1493[ 5u]="LONG"; static char _temp1496[ 6u]="FLOAT";
static char _temp1499[ 7u]="DOUBLE"; static char _temp1502[ 7u]="SIGNED"; static
char _temp1505[ 9u]="UNSIGNED"; static char _temp1508[ 6u]="CONST"; static char
_temp1511[ 9u]="VOLATILE"; static char _temp1514[ 9u]="RESTRICT"; static char
_temp1517[ 7u]="STRUCT"; static char _temp1520[ 6u]="UNION"; static char
_temp1523[ 5u]="CASE"; static char _temp1526[ 8u]="DEFAULT"; static char
_temp1529[ 7u]="INLINE"; static char _temp1532[ 3u]="IF"; static char _temp1535[
5u]="ELSE"; static char _temp1538[ 7u]="SWITCH"; static char _temp1541[ 6u]="WHILE";
static char _temp1544[ 3u]="DO"; static char _temp1547[ 4u]="FOR"; static char
_temp1550[ 5u]="GOTO"; static char _temp1553[ 9u]="CONTINUE"; static char
_temp1556[ 6u]="BREAK"; static char _temp1559[ 7u]="RETURN"; static char
_temp1562[ 7u]="SIZEOF"; static char _temp1565[ 5u]="ENUM"; static char
_temp1568[ 8u]="NULL_kw"; static char _temp1571[ 4u]="LET"; static char
_temp1574[ 6u]="THROW"; static char _temp1577[ 4u]="TRY"; static char _temp1580[
6u]="CATCH"; static char _temp1583[ 4u]="NEW"; static char _temp1586[ 9u]="ABSTRACT";
static char _temp1589[ 9u]="FALLTHRU"; static char _temp1592[ 6u]="USING";
static char _temp1595[ 10u]="NAMESPACE"; static char _temp1598[ 7u]="TUNION";
static char _temp1601[ 8u]="XTUNION"; static char _temp1604[ 5u]="FILL"; static
char _temp1607[ 8u]="CODEGEN"; static char _temp1610[ 4u]="CUT"; static char
_temp1613[ 7u]="SPLICE"; static char _temp1616[ 7u]="PRINTF"; static char
_temp1619[ 8u]="FPRINTF"; static char _temp1622[ 8u]="XPRINTF"; static char
_temp1625[ 6u]="SCANF"; static char _temp1628[ 7u]="FSCANF"; static char
_temp1631[ 7u]="SSCANF"; static char _temp1634[ 7u]="MALLOC"; static char
_temp1637[ 7u]="PTR_OP"; static char _temp1640[ 7u]="INC_OP"; static char
_temp1643[ 7u]="DEC_OP"; static char _temp1646[ 8u]="LEFT_OP"; static char
_temp1649[ 9u]="RIGHT_OP"; static char _temp1652[ 6u]="LE_OP"; static char
_temp1655[ 6u]="GE_OP"; static char _temp1658[ 6u]="EQ_OP"; static char
_temp1661[ 6u]="NE_OP"; static char _temp1664[ 7u]="AND_OP"; static char
_temp1667[ 6u]="OR_OP"; static char _temp1670[ 11u]="MUL_ASSIGN"; static char
_temp1673[ 11u]="DIV_ASSIGN"; static char _temp1676[ 11u]="MOD_ASSIGN"; static
char _temp1679[ 11u]="ADD_ASSIGN"; static char _temp1682[ 11u]="SUB_ASSIGN";
static char _temp1685[ 12u]="LEFT_ASSIGN"; static char _temp1688[ 13u]="RIGHT_ASSIGN";
static char _temp1691[ 11u]="AND_ASSIGN"; static char _temp1694[ 11u]="XOR_ASSIGN";
static char _temp1697[ 10u]="OR_ASSIGN"; static char _temp1700[ 9u]="ELLIPSIS";
static char _temp1703[ 11u]="LEFT_RIGHT"; static char _temp1706[ 12u]="COLON_COLON";
static char _temp1709[ 11u]="IDENTIFIER"; static char _temp1712[ 17u]="INTEGER_CONSTANT";
static char _temp1715[ 7u]="STRING"; static char _temp1718[ 19u]="CHARACTER_CONSTANT";
static char _temp1721[ 18u]="FLOATING_CONSTANT"; static char _temp1724[ 9u]="TYPE_VAR";
static char _temp1727[ 16u]="QUAL_IDENTIFIER"; static char _temp1730[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1733[ 10u]="ATTRIBUTE"; static char _temp1736[ 4u]="';'";
static char _temp1739[ 4u]="'{'"; static char _temp1742[ 4u]="'}'"; static char
_temp1745[ 4u]="'='"; static char _temp1748[ 4u]="'('"; static char _temp1751[ 4u]="')'";
static char _temp1754[ 4u]="','"; static char _temp1757[ 4u]="'_'"; static char
_temp1760[ 4u]="'$'"; static char _temp1763[ 4u]="'<'"; static char _temp1766[ 4u]="'>'";
static char _temp1769[ 4u]="':'"; static char _temp1772[ 4u]="'.'"; static char
_temp1775[ 4u]="'['"; static char _temp1778[ 4u]="']'"; static char _temp1781[ 4u]="'*'";
static char _temp1784[ 4u]="'@'"; static char _temp1787[ 4u]="'?'"; static char
_temp1790[ 4u]="'+'"; static char _temp1793[ 4u]="'-'"; static char _temp1796[ 4u]="'&'";
static char _temp1799[ 4u]="'|'"; static char _temp1802[ 4u]="'^'"; static char
_temp1805[ 4u]="'/'"; static char _temp1808[ 4u]="'%'"; static char _temp1811[ 4u]="'~'";
static char _temp1814[ 4u]="'!'"; static char _temp1817[ 5u]="prog"; static char
_temp1820[ 17u]="translation_unit"; static char _temp1823[ 21u]="external_declaration";
static char _temp1826[ 20u]="function_definition"; static char _temp1829[ 21u]="function_definition2";
static char _temp1832[ 13u]="using_action"; static char _temp1835[ 15u]="unusing_action";
static char _temp1838[ 17u]="namespace_action"; static char _temp1841[ 19u]="unnamespace_action";
static char _temp1844[ 12u]="declaration"; static char _temp1847[ 17u]="declaration_list";
static char _temp1850[ 23u]="declaration_specifiers"; static char _temp1853[ 24u]="storage_class_specifier";
static char _temp1856[ 15u]="attributes_opt"; static char _temp1859[ 11u]="attributes";
static char _temp1862[ 15u]="attribute_list"; static char _temp1865[ 10u]="attribute";
static char _temp1868[ 15u]="type_specifier"; static char _temp1871[ 5u]="kind";
static char _temp1874[ 15u]="type_qualifier"; static char _temp1877[ 15u]="enum_specifier";
static char _temp1880[ 11u]="enum_field"; static char _temp1883[ 22u]="enum_declaration_list";
static char _temp1886[ 26u]="struct_or_union_specifier"; static char _temp1889[
16u]="type_params_opt"; static char _temp1892[ 16u]="struct_or_union"; static
char _temp1895[ 24u]="struct_declaration_list"; static char _temp1898[ 25u]="struct_declaration_list0";
static char _temp1901[ 21u]="init_declarator_list"; static char _temp1904[ 22u]="init_declarator_list0";
static char _temp1907[ 16u]="init_declarator"; static char _temp1910[ 19u]="struct_declaration";
static char _temp1913[ 25u]="specifier_qualifier_list"; static char _temp1916[
23u]="struct_declarator_list"; static char _temp1919[ 24u]="struct_declarator_list0";
static char _temp1922[ 18u]="struct_declarator"; static char _temp1925[ 17u]="tunion_specifier";
static char _temp1928[ 17u]="tunionfield_list"; static char _temp1931[ 12u]="tunionfield";
static char _temp1934[ 11u]="declarator"; static char _temp1937[ 18u]="direct_declarator";
static char _temp1940[ 8u]="pointer"; static char _temp1943[ 13u]="pointer_char";
static char _temp1946[ 8u]="rgn_opt"; static char _temp1949[ 4u]="rgn"; static
char _temp1952[ 20u]="type_qualifier_list"; static char _temp1955[ 20u]="parameter_type_list";
static char _temp1958[ 11u]="effect_set"; static char _temp1961[ 14u]="atomic_effect";
static char _temp1964[ 11u]="region_set"; static char _temp1967[ 15u]="parameter_list";
static char _temp1970[ 22u]="parameter_declaration"; static char _temp1973[ 16u]="identifier_list";
static char _temp1976[ 17u]="identifier_list0"; static char _temp1979[ 12u]="initializer";
static char _temp1982[ 18u]="array_initializer"; static char _temp1985[ 17u]="initializer_list";
static char _temp1988[ 12u]="designation"; static char _temp1991[ 16u]="designator_list";
static char _temp1994[ 11u]="designator"; static char _temp1997[ 10u]="type_name";
static char _temp2000[ 14u]="any_type_name"; static char _temp2003[ 15u]="type_name_list";
static char _temp2006[ 20u]="abstract_declarator"; static char _temp2009[ 27u]="direct_abstract_declarator";
static char _temp2012[ 10u]="statement"; static char _temp2015[ 18u]="labeled_statement";
static char _temp2018[ 21u]="expression_statement"; static char _temp2021[ 19u]="compound_statement";
static char _temp2024[ 16u]="block_item_list"; static char _temp2027[ 11u]="block_item";
static char _temp2030[ 20u]="selection_statement"; static char _temp2033[ 15u]="switch_clauses";
static char _temp2036[ 20u]="iteration_statement"; static char _temp2039[ 15u]="jump_statement";
static char _temp2042[ 8u]="pattern"; static char _temp2045[ 19u]="tuple_pattern_list";
static char _temp2048[ 20u]="tuple_pattern_list0"; static char _temp2051[ 14u]="field_pattern";
static char _temp2054[ 19u]="field_pattern_list"; static char _temp2057[ 20u]="field_pattern_list0";
static char _temp2060[ 11u]="expression"; static char _temp2063[ 22u]="assignment_expression";
static char _temp2066[ 20u]="assignment_operator"; static char _temp2069[ 23u]="conditional_expression";
static char _temp2072[ 20u]="constant_expression"; static char _temp2075[ 22u]="logical_or_expression";
static char _temp2078[ 23u]="logical_and_expression"; static char _temp2081[ 24u]="inclusive_or_expression";
static char _temp2084[ 24u]="exclusive_or_expression"; static char _temp2087[ 15u]="and_expression";
static char _temp2090[ 20u]="equality_expression"; static char _temp2093[ 22u]="relational_expression";
static char _temp2096[ 17u]="shift_expression"; static char _temp2099[ 20u]="additive_expression";
static char _temp2102[ 26u]="multiplicative_expression"; static char _temp2105[
16u]="cast_expression"; static char _temp2108[ 17u]="unary_expression"; static
char _temp2111[ 14u]="format_primop"; static char _temp2114[ 15u]="unary_operator";
static char _temp2117[ 19u]="postfix_expression"; static char _temp2120[ 19u]="primary_expression";
static char _temp2123[ 25u]="argument_expression_list"; static char _temp2126[
26u]="argument_expression_list0"; static char _temp2129[ 9u]="constant"; static
char _temp2132[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
226u]={( struct _tagged_string){ _temp1457, _temp1457, _temp1457 + 2u},( struct
_tagged_string){ _temp1460, _temp1460, _temp1460 + 6u},( struct _tagged_string){
_temp1463, _temp1463, _temp1463 + 12u},( struct _tagged_string){ _temp1466,
_temp1466, _temp1466 + 5u},( struct _tagged_string){ _temp1469, _temp1469,
_temp1469 + 9u},( struct _tagged_string){ _temp1472, _temp1472, _temp1472 + 7u},(
struct _tagged_string){ _temp1475, _temp1475, _temp1475 + 7u},( struct
_tagged_string){ _temp1478, _temp1478, _temp1478 + 8u},( struct _tagged_string){
_temp1481, _temp1481, _temp1481 + 5u},( struct _tagged_string){ _temp1484,
_temp1484, _temp1484 + 5u},( struct _tagged_string){ _temp1487, _temp1487,
_temp1487 + 6u},( struct _tagged_string){ _temp1490, _temp1490, _temp1490 + 4u},(
struct _tagged_string){ _temp1493, _temp1493, _temp1493 + 5u},( struct
_tagged_string){ _temp1496, _temp1496, _temp1496 + 6u},( struct _tagged_string){
_temp1499, _temp1499, _temp1499 + 7u},( struct _tagged_string){ _temp1502,
_temp1502, _temp1502 + 7u},( struct _tagged_string){ _temp1505, _temp1505,
_temp1505 + 9u},( struct _tagged_string){ _temp1508, _temp1508, _temp1508 + 6u},(
struct _tagged_string){ _temp1511, _temp1511, _temp1511 + 9u},( struct
_tagged_string){ _temp1514, _temp1514, _temp1514 + 9u},( struct _tagged_string){
_temp1517, _temp1517, _temp1517 + 7u},( struct _tagged_string){ _temp1520,
_temp1520, _temp1520 + 6u},( struct _tagged_string){ _temp1523, _temp1523,
_temp1523 + 5u},( struct _tagged_string){ _temp1526, _temp1526, _temp1526 + 8u},(
struct _tagged_string){ _temp1529, _temp1529, _temp1529 + 7u},( struct
_tagged_string){ _temp1532, _temp1532, _temp1532 + 3u},( struct _tagged_string){
_temp1535, _temp1535, _temp1535 + 5u},( struct _tagged_string){ _temp1538,
_temp1538, _temp1538 + 7u},( struct _tagged_string){ _temp1541, _temp1541,
_temp1541 + 6u},( struct _tagged_string){ _temp1544, _temp1544, _temp1544 + 3u},(
struct _tagged_string){ _temp1547, _temp1547, _temp1547 + 4u},( struct
_tagged_string){ _temp1550, _temp1550, _temp1550 + 5u},( struct _tagged_string){
_temp1553, _temp1553, _temp1553 + 9u},( struct _tagged_string){ _temp1556,
_temp1556, _temp1556 + 6u},( struct _tagged_string){ _temp1559, _temp1559,
_temp1559 + 7u},( struct _tagged_string){ _temp1562, _temp1562, _temp1562 + 7u},(
struct _tagged_string){ _temp1565, _temp1565, _temp1565 + 5u},( struct
_tagged_string){ _temp1568, _temp1568, _temp1568 + 8u},( struct _tagged_string){
_temp1571, _temp1571, _temp1571 + 4u},( struct _tagged_string){ _temp1574,
_temp1574, _temp1574 + 6u},( struct _tagged_string){ _temp1577, _temp1577,
_temp1577 + 4u},( struct _tagged_string){ _temp1580, _temp1580, _temp1580 + 6u},(
struct _tagged_string){ _temp1583, _temp1583, _temp1583 + 4u},( struct
_tagged_string){ _temp1586, _temp1586, _temp1586 + 9u},( struct _tagged_string){
_temp1589, _temp1589, _temp1589 + 9u},( struct _tagged_string){ _temp1592,
_temp1592, _temp1592 + 6u},( struct _tagged_string){ _temp1595, _temp1595,
_temp1595 + 10u},( struct _tagged_string){ _temp1598, _temp1598, _temp1598 + 7u},(
struct _tagged_string){ _temp1601, _temp1601, _temp1601 + 8u},( struct
_tagged_string){ _temp1604, _temp1604, _temp1604 + 5u},( struct _tagged_string){
_temp1607, _temp1607, _temp1607 + 8u},( struct _tagged_string){ _temp1610,
_temp1610, _temp1610 + 4u},( struct _tagged_string){ _temp1613, _temp1613,
_temp1613 + 7u},( struct _tagged_string){ _temp1616, _temp1616, _temp1616 + 7u},(
struct _tagged_string){ _temp1619, _temp1619, _temp1619 + 8u},( struct
_tagged_string){ _temp1622, _temp1622, _temp1622 + 8u},( struct _tagged_string){
_temp1625, _temp1625, _temp1625 + 6u},( struct _tagged_string){ _temp1628,
_temp1628, _temp1628 + 7u},( struct _tagged_string){ _temp1631, _temp1631,
_temp1631 + 7u},( struct _tagged_string){ _temp1634, _temp1634, _temp1634 + 7u},(
struct _tagged_string){ _temp1637, _temp1637, _temp1637 + 7u},( struct
_tagged_string){ _temp1640, _temp1640, _temp1640 + 7u},( struct _tagged_string){
_temp1643, _temp1643, _temp1643 + 7u},( struct _tagged_string){ _temp1646,
_temp1646, _temp1646 + 8u},( struct _tagged_string){ _temp1649, _temp1649,
_temp1649 + 9u},( struct _tagged_string){ _temp1652, _temp1652, _temp1652 + 6u},(
struct _tagged_string){ _temp1655, _temp1655, _temp1655 + 6u},( struct
_tagged_string){ _temp1658, _temp1658, _temp1658 + 6u},( struct _tagged_string){
_temp1661, _temp1661, _temp1661 + 6u},( struct _tagged_string){ _temp1664,
_temp1664, _temp1664 + 7u},( struct _tagged_string){ _temp1667, _temp1667,
_temp1667 + 6u},( struct _tagged_string){ _temp1670, _temp1670, _temp1670 + 11u},(
struct _tagged_string){ _temp1673, _temp1673, _temp1673 + 11u},( struct
_tagged_string){ _temp1676, _temp1676, _temp1676 + 11u},( struct _tagged_string){
_temp1679, _temp1679, _temp1679 + 11u},( struct _tagged_string){ _temp1682,
_temp1682, _temp1682 + 11u},( struct _tagged_string){ _temp1685, _temp1685,
_temp1685 + 12u},( struct _tagged_string){ _temp1688, _temp1688, _temp1688 + 13u},(
struct _tagged_string){ _temp1691, _temp1691, _temp1691 + 11u},( struct
_tagged_string){ _temp1694, _temp1694, _temp1694 + 11u},( struct _tagged_string){
_temp1697, _temp1697, _temp1697 + 10u},( struct _tagged_string){ _temp1700,
_temp1700, _temp1700 + 9u},( struct _tagged_string){ _temp1703, _temp1703,
_temp1703 + 11u},( struct _tagged_string){ _temp1706, _temp1706, _temp1706 + 12u},(
struct _tagged_string){ _temp1709, _temp1709, _temp1709 + 11u},( struct
_tagged_string){ _temp1712, _temp1712, _temp1712 + 17u},( struct _tagged_string){
_temp1715, _temp1715, _temp1715 + 7u},( struct _tagged_string){ _temp1718,
_temp1718, _temp1718 + 19u},( struct _tagged_string){ _temp1721, _temp1721,
_temp1721 + 18u},( struct _tagged_string){ _temp1724, _temp1724, _temp1724 + 9u},(
struct _tagged_string){ _temp1727, _temp1727, _temp1727 + 16u},( struct
_tagged_string){ _temp1730, _temp1730, _temp1730 + 18u},( struct _tagged_string){
_temp1733, _temp1733, _temp1733 + 10u},( struct _tagged_string){ _temp1736,
_temp1736, _temp1736 + 4u},( struct _tagged_string){ _temp1739, _temp1739,
_temp1739 + 4u},( struct _tagged_string){ _temp1742, _temp1742, _temp1742 + 4u},(
struct _tagged_string){ _temp1745, _temp1745, _temp1745 + 4u},( struct
_tagged_string){ _temp1748, _temp1748, _temp1748 + 4u},( struct _tagged_string){
_temp1751, _temp1751, _temp1751 + 4u},( struct _tagged_string){ _temp1754,
_temp1754, _temp1754 + 4u},( struct _tagged_string){ _temp1757, _temp1757,
_temp1757 + 4u},( struct _tagged_string){ _temp1760, _temp1760, _temp1760 + 4u},(
struct _tagged_string){ _temp1763, _temp1763, _temp1763 + 4u},( struct
_tagged_string){ _temp1766, _temp1766, _temp1766 + 4u},( struct _tagged_string){
_temp1769, _temp1769, _temp1769 + 4u},( struct _tagged_string){ _temp1772,
_temp1772, _temp1772 + 4u},( struct _tagged_string){ _temp1775, _temp1775,
_temp1775 + 4u},( struct _tagged_string){ _temp1778, _temp1778, _temp1778 + 4u},(
struct _tagged_string){ _temp1781, _temp1781, _temp1781 + 4u},( struct
_tagged_string){ _temp1784, _temp1784, _temp1784 + 4u},( struct _tagged_string){
_temp1787, _temp1787, _temp1787 + 4u},( struct _tagged_string){ _temp1790,
_temp1790, _temp1790 + 4u},( struct _tagged_string){ _temp1793, _temp1793,
_temp1793 + 4u},( struct _tagged_string){ _temp1796, _temp1796, _temp1796 + 4u},(
struct _tagged_string){ _temp1799, _temp1799, _temp1799 + 4u},( struct
_tagged_string){ _temp1802, _temp1802, _temp1802 + 4u},( struct _tagged_string){
_temp1805, _temp1805, _temp1805 + 4u},( struct _tagged_string){ _temp1808,
_temp1808, _temp1808 + 4u},( struct _tagged_string){ _temp1811, _temp1811,
_temp1811 + 4u},( struct _tagged_string){ _temp1814, _temp1814, _temp1814 + 4u},(
struct _tagged_string){ _temp1817, _temp1817, _temp1817 + 5u},( struct
_tagged_string){ _temp1820, _temp1820, _temp1820 + 17u},( struct _tagged_string){
_temp1823, _temp1823, _temp1823 + 21u},( struct _tagged_string){ _temp1826,
_temp1826, _temp1826 + 20u},( struct _tagged_string){ _temp1829, _temp1829,
_temp1829 + 21u},( struct _tagged_string){ _temp1832, _temp1832, _temp1832 + 13u},(
struct _tagged_string){ _temp1835, _temp1835, _temp1835 + 15u},( struct
_tagged_string){ _temp1838, _temp1838, _temp1838 + 17u},( struct _tagged_string){
_temp1841, _temp1841, _temp1841 + 19u},( struct _tagged_string){ _temp1844,
_temp1844, _temp1844 + 12u},( struct _tagged_string){ _temp1847, _temp1847,
_temp1847 + 17u},( struct _tagged_string){ _temp1850, _temp1850, _temp1850 + 23u},(
struct _tagged_string){ _temp1853, _temp1853, _temp1853 + 24u},( struct
_tagged_string){ _temp1856, _temp1856, _temp1856 + 15u},( struct _tagged_string){
_temp1859, _temp1859, _temp1859 + 11u},( struct _tagged_string){ _temp1862,
_temp1862, _temp1862 + 15u},( struct _tagged_string){ _temp1865, _temp1865,
_temp1865 + 10u},( struct _tagged_string){ _temp1868, _temp1868, _temp1868 + 15u},(
struct _tagged_string){ _temp1871, _temp1871, _temp1871 + 5u},( struct
_tagged_string){ _temp1874, _temp1874, _temp1874 + 15u},( struct _tagged_string){
_temp1877, _temp1877, _temp1877 + 15u},( struct _tagged_string){ _temp1880,
_temp1880, _temp1880 + 11u},( struct _tagged_string){ _temp1883, _temp1883,
_temp1883 + 22u},( struct _tagged_string){ _temp1886, _temp1886, _temp1886 + 26u},(
struct _tagged_string){ _temp1889, _temp1889, _temp1889 + 16u},( struct
_tagged_string){ _temp1892, _temp1892, _temp1892 + 16u},( struct _tagged_string){
_temp1895, _temp1895, _temp1895 + 24u},( struct _tagged_string){ _temp1898,
_temp1898, _temp1898 + 25u},( struct _tagged_string){ _temp1901, _temp1901,
_temp1901 + 21u},( struct _tagged_string){ _temp1904, _temp1904, _temp1904 + 22u},(
struct _tagged_string){ _temp1907, _temp1907, _temp1907 + 16u},( struct
_tagged_string){ _temp1910, _temp1910, _temp1910 + 19u},( struct _tagged_string){
_temp1913, _temp1913, _temp1913 + 25u},( struct _tagged_string){ _temp1916,
_temp1916, _temp1916 + 23u},( struct _tagged_string){ _temp1919, _temp1919,
_temp1919 + 24u},( struct _tagged_string){ _temp1922, _temp1922, _temp1922 + 18u},(
struct _tagged_string){ _temp1925, _temp1925, _temp1925 + 17u},( struct
_tagged_string){ _temp1928, _temp1928, _temp1928 + 17u},( struct _tagged_string){
_temp1931, _temp1931, _temp1931 + 12u},( struct _tagged_string){ _temp1934,
_temp1934, _temp1934 + 11u},( struct _tagged_string){ _temp1937, _temp1937,
_temp1937 + 18u},( struct _tagged_string){ _temp1940, _temp1940, _temp1940 + 8u},(
struct _tagged_string){ _temp1943, _temp1943, _temp1943 + 13u},( struct
_tagged_string){ _temp1946, _temp1946, _temp1946 + 8u},( struct _tagged_string){
_temp1949, _temp1949, _temp1949 + 4u},( struct _tagged_string){ _temp1952,
_temp1952, _temp1952 + 20u},( struct _tagged_string){ _temp1955, _temp1955,
_temp1955 + 20u},( struct _tagged_string){ _temp1958, _temp1958, _temp1958 + 11u},(
struct _tagged_string){ _temp1961, _temp1961, _temp1961 + 14u},( struct
_tagged_string){ _temp1964, _temp1964, _temp1964 + 11u},( struct _tagged_string){
_temp1967, _temp1967, _temp1967 + 15u},( struct _tagged_string){ _temp1970,
_temp1970, _temp1970 + 22u},( struct _tagged_string){ _temp1973, _temp1973,
_temp1973 + 16u},( struct _tagged_string){ _temp1976, _temp1976, _temp1976 + 17u},(
struct _tagged_string){ _temp1979, _temp1979, _temp1979 + 12u},( struct
_tagged_string){ _temp1982, _temp1982, _temp1982 + 18u},( struct _tagged_string){
_temp1985, _temp1985, _temp1985 + 17u},( struct _tagged_string){ _temp1988,
_temp1988, _temp1988 + 12u},( struct _tagged_string){ _temp1991, _temp1991,
_temp1991 + 16u},( struct _tagged_string){ _temp1994, _temp1994, _temp1994 + 11u},(
struct _tagged_string){ _temp1997, _temp1997, _temp1997 + 10u},( struct
_tagged_string){ _temp2000, _temp2000, _temp2000 + 14u},( struct _tagged_string){
_temp2003, _temp2003, _temp2003 + 15u},( struct _tagged_string){ _temp2006,
_temp2006, _temp2006 + 20u},( struct _tagged_string){ _temp2009, _temp2009,
_temp2009 + 27u},( struct _tagged_string){ _temp2012, _temp2012, _temp2012 + 10u},(
struct _tagged_string){ _temp2015, _temp2015, _temp2015 + 18u},( struct
_tagged_string){ _temp2018, _temp2018, _temp2018 + 21u},( struct _tagged_string){
_temp2021, _temp2021, _temp2021 + 19u},( struct _tagged_string){ _temp2024,
_temp2024, _temp2024 + 16u},( struct _tagged_string){ _temp2027, _temp2027,
_temp2027 + 11u},( struct _tagged_string){ _temp2030, _temp2030, _temp2030 + 20u},(
struct _tagged_string){ _temp2033, _temp2033, _temp2033 + 15u},( struct
_tagged_string){ _temp2036, _temp2036, _temp2036 + 20u},( struct _tagged_string){
_temp2039, _temp2039, _temp2039 + 15u},( struct _tagged_string){ _temp2042,
_temp2042, _temp2042 + 8u},( struct _tagged_string){ _temp2045, _temp2045,
_temp2045 + 19u},( struct _tagged_string){ _temp2048, _temp2048, _temp2048 + 20u},(
struct _tagged_string){ _temp2051, _temp2051, _temp2051 + 14u},( struct
_tagged_string){ _temp2054, _temp2054, _temp2054 + 19u},( struct _tagged_string){
_temp2057, _temp2057, _temp2057 + 20u},( struct _tagged_string){ _temp2060,
_temp2060, _temp2060 + 11u},( struct _tagged_string){ _temp2063, _temp2063,
_temp2063 + 22u},( struct _tagged_string){ _temp2066, _temp2066, _temp2066 + 20u},(
struct _tagged_string){ _temp2069, _temp2069, _temp2069 + 23u},( struct
_tagged_string){ _temp2072, _temp2072, _temp2072 + 20u},( struct _tagged_string){
_temp2075, _temp2075, _temp2075 + 22u},( struct _tagged_string){ _temp2078,
_temp2078, _temp2078 + 23u},( struct _tagged_string){ _temp2081, _temp2081,
_temp2081 + 24u},( struct _tagged_string){ _temp2084, _temp2084, _temp2084 + 24u},(
struct _tagged_string){ _temp2087, _temp2087, _temp2087 + 15u},( struct
_tagged_string){ _temp2090, _temp2090, _temp2090 + 20u},( struct _tagged_string){
_temp2093, _temp2093, _temp2093 + 22u},( struct _tagged_string){ _temp2096,
_temp2096, _temp2096 + 17u},( struct _tagged_string){ _temp2099, _temp2099,
_temp2099 + 20u},( struct _tagged_string){ _temp2102, _temp2102, _temp2102 + 26u},(
struct _tagged_string){ _temp2105, _temp2105, _temp2105 + 16u},( struct
_tagged_string){ _temp2108, _temp2108, _temp2108 + 17u},( struct _tagged_string){
_temp2111, _temp2111, _temp2111 + 14u},( struct _tagged_string){ _temp2114,
_temp2114, _temp2114 + 15u},( struct _tagged_string){ _temp2117, _temp2117,
_temp2117 + 19u},( struct _tagged_string){ _temp2120, _temp2120, _temp2120 + 19u},(
struct _tagged_string){ _temp2123, _temp2123, _temp2123 + 25u},( struct
_tagged_string){ _temp2126, _temp2126, _temp2126 + 26u},( struct _tagged_string){
_temp2129, _temp2129, _temp2129 + 9u},( struct _tagged_string){ _temp2132,
_temp2132, _temp2132 + 20u}}; static short Cyc_yyr1[ 387u]={ (short)0, (short)120,
(short)121, (short)121, (short)121, (short)121, (short)121, (short)121, (short)121,
(short)122, (short)122, (short)123, (short)123, (short)123, (short)123, (short)124,
(short)124, (short)125, (short)126, (short)127, (short)128, (short)129, (short)129,
(short)129, (short)130, (short)130, (short)131, (short)131, (short)131, (short)131,
(short)131, (short)131, (short)131, (short)131, (short)132, (short)132, (short)132,
(short)132, (short)132, (short)132, (short)132, (short)133, (short)133, (short)134,
(short)135, (short)135, (short)136, (short)136, (short)136, (short)136, (short)137,
(short)137, (short)137, (short)137, (short)137, (short)137, (short)137, (short)137,
(short)137, (short)137, (short)137, (short)137, (short)137, (short)137, (short)137,
(short)137, (short)137, (short)138, (short)138, (short)139, (short)139, (short)139,
(short)140, (short)140, (short)141, (short)141, (short)142, (short)142, (short)143,
(short)143, (short)143, (short)143, (short)143, (short)144, (short)144, (short)144,
(short)145, (short)145, (short)146, (short)147, (short)147, (short)148, (short)149,
(short)149, (short)150, (short)150, (short)151, (short)152, (short)152, (short)152,
(short)152, (short)153, (short)154, (short)154, (short)155, (short)155, (short)155,
(short)156, (short)156, (short)156, (short)156, (short)156, (short)156, (short)156,
(short)156, (short)157, (short)157, (short)157, (short)157, (short)158, (short)158,
(short)159, (short)159, (short)160, (short)160, (short)160, (short)160, (short)160,
(short)160, (short)160, (short)160, (short)160, (short)160, (short)160, (short)161,
(short)161, (short)161, (short)161, (short)162, (short)162, (short)162, (short)162,
(short)162, (short)163, (short)163, (short)164, (short)164, (short)164, (short)165,
(short)165, (short)166, (short)166, (short)166, (short)167, (short)167, (short)168,
(short)168, (short)168, (short)168, (short)169, (short)169, (short)169, (short)169,
(short)170, (short)170, (short)171, (short)171, (short)171, (short)172, (short)173,
(short)173, (short)174, (short)174, (short)175, (short)175, (short)175, (short)175,
(short)176, (short)176, (short)176, (short)176, (short)177, (short)178, (short)178,
(short)179, (short)179, (short)180, (short)180, (short)181, (short)181, (short)181,
(short)181, (short)182, (short)182, (short)183, (short)183, (short)183, (short)184,
(short)184, (short)184, (short)184, (short)184, (short)184, (short)184, (short)184,
(short)184, (short)184, (short)184, (short)184, (short)184, (short)184, (short)185,
(short)185, (short)185, (short)185, (short)185, (short)185, (short)185, (short)185,
(short)186, (short)187, (short)187, (short)188, (short)188, (short)189, (short)189,
(short)190, (short)190, (short)190, (short)191, (short)191, (short)191, (short)191,
(short)192, (short)192, (short)192, (short)192, (short)192, (short)192, (short)193,
(short)193, (short)193, (short)193, (short)193, (short)193, (short)193, (short)193,
(short)193, (short)193, (short)193, (short)193, (short)193, (short)193, (short)194,
(short)194, (short)194, (short)194, (short)194, (short)194, (short)194, (short)194,
(short)195, (short)195, (short)195, (short)195, (short)195, (short)195, (short)195,
(short)195, (short)195, (short)195, (short)195, (short)195, (short)195, (short)195,
(short)196, (short)196, (short)197, (short)197, (short)198, (short)198, (short)199,
(short)200, (short)200, (short)201, (short)201, (short)202, (short)202, (short)203,
(short)203, (short)203, (short)203, (short)203, (short)203, (short)203, (short)203,
(short)203, (short)203, (short)203, (short)204, (short)204, (short)204, (short)204,
(short)204, (short)205, (short)206, (short)206, (short)207, (short)207, (short)208,
(short)208, (short)209, (short)209, (short)210, (short)210, (short)211, (short)211,
(short)211, (short)212, (short)212, (short)212, (short)212, (short)212, (short)213,
(short)213, (short)213, (short)214, (short)214, (short)214, (short)215, (short)215,
(short)215, (short)215, (short)216, (short)216, (short)217, (short)217, (short)217,
(short)217, (short)217, (short)217, (short)217, (short)217, (short)217, (short)217,
(short)217, (short)218, (short)218, (short)218, (short)218, (short)218, (short)218,
(short)219, (short)219, (short)219, (short)220, (short)220, (short)220, (short)220,
(short)220, (short)220, (short)220, (short)220, (short)220, (short)220, (short)220,
(short)220, (short)220, (short)220, (short)221, (short)221, (short)221, (short)221,
(short)221, (short)221, (short)221, (short)221, (short)221, (short)222, (short)223,
(short)223, (short)224, (short)224, (short)224, (short)224, (short)225, (short)225};
static short Cyc_yyr2[ 387u]={ (short)0, (short)1, (short)2, (short)3, (short)5,
(short)3, (short)5, (short)6, (short)0, (short)1, (short)1, (short)2, (short)3,
(short)3, (short)4, (short)3, (short)4, (short)2, (short)1, (short)2, (short)1,
(short)2, (short)3, (short)5, (short)1, (short)2, (short)2, (short)3, (short)2,
(short)3, (short)2, (short)3, (short)2, (short)3, (short)1, (short)1, (short)1,
(short)1, (short)2, (short)1, (short)1, (short)0, (short)1, (short)6, (short)1,
(short)3, (short)1, (short)1, (short)4, (short)4, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)3, (short)2, (short)4, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)5, (short)2, (short)1, (short)3, (short)1,
(short)3, (short)4, (short)6, (short)6, (short)3, (short)3, (short)0, (short)3,
(short)3, (short)1, (short)1, (short)1, (short)1, (short)2, (short)1, (short)1,
(short)3, (short)1, (short)3, (short)3, (short)2, (short)3, (short)2, (short)3,
(short)1, (short)1, (short)3, (short)1, (short)2, (short)3, (short)6, (short)3,
(short)4, (short)5, (short)5, (short)2, (short)3, (short)4, (short)1, (short)2,
(short)3, (short)3, (short)1, (short)5, (short)1, (short)2, (short)1, (short)3,
(short)3, (short)4, (short)4, (short)3, (short)5, (short)4, (short)4, (short)4,
(short)2, (short)3, (short)4, (short)4, (short)5, (short)1, (short)1, (short)4,
(short)4, (short)1, (short)0, (short)1, (short)1, (short)3, (short)1, (short)1,
(short)2, (short)1, (short)3, (short)3, (short)1, (short)3, (short)2, (short)3,
(short)1, (short)3, (short)1, (short)3, (short)3, (short)5, (short)1, (short)3,
(short)2, (short)1, (short)2, (short)1, (short)1, (short)3, (short)1, (short)1,
(short)2, (short)3, (short)4, (short)8, (short)1, (short)2, (short)3, (short)4,
(short)2, (short)1, (short)2, (short)3, (short)2, (short)1, (short)2, (short)1,
(short)2, (short)3, (short)3, (short)1, (short)3, (short)1, (short)1, (short)2,
(short)3, (short)2, (short)3, (short)3, (short)4, (short)2, (short)4, (short)3,
(short)3, (short)5, (short)4, (short)4, (short)4, (short)2, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)2, (short)2, (short)3, (short)1,
(short)2, (short)2, (short)3, (short)1, (short)2, (short)1, (short)1, (short)1,
(short)5, (short)7, (short)7, (short)6, (short)0, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)5, (short)7, (short)6, (short)7, (short)7, (short)8,
(short)7, (short)8, (short)8, (short)9, (short)6, (short)7, (short)7, (short)8,
(short)3, (short)2, (short)2, (short)2, (short)3, (short)2, (short)4, (short)5,
(short)1, (short)3, (short)1, (short)2, (short)1, (short)1, (short)1, (short)1,
(short)5, (short)4, (short)4, (short)5, (short)2, (short)2, (short)0, (short)1,
(short)1, (short)3, (short)1, (short)2, (short)1, (short)1, (short)3, (short)1,
(short)3, (short)1, (short)3, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)5,
(short)2, (short)2, (short)2, (short)1, (short)1, (short)3, (short)1, (short)3,
(short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3,
(short)3, (short)1, (short)3, (short)3, (short)3, (short)3, (short)1, (short)3,
(short)3, (short)1, (short)3, (short)3, (short)1, (short)3, (short)3, (short)3,
(short)1, (short)4, (short)1, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)4, (short)2, (short)4, (short)7, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)4,
(short)3, (short)4, (short)3, (short)3, (short)3, (short)3, (short)2, (short)2,
(short)6, (short)7, (short)4, (short)4, (short)1, (short)1, (short)1, (short)3,
(short)2, (short)5, (short)4, (short)4, (short)5, (short)1, (short)1, (short)3,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1}; static short Cyc_yydefact[
728u]={ (short)8, (short)34, (short)35, (short)36, (short)37, (short)39, (short)50,
(short)52, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, (short)69, (short)70, (short)71, (short)86, (short)87, (short)41,
(short)0, (short)0, (short)40, (short)0, (short)0, (short)0, (short)0, (short)385,
(short)63, (short)386, (short)83, (short)0, (short)51, (short)0, (short)138,
(short)139, (short)142, (short)1, (short)8, (short)9, (short)0, (short)0,
(short)10, (short)0, (short)41, (short)41, (short)41, (short)60, (short)61,
(short)0, (short)62, (short)0, (short)121, (short)0, (short)143, (short)123,
(short)38, (short)0, (short)32, (short)42, (short)73, (short)267, (short)263,
(short)266, (short)265, (short)0, (short)261, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)268, (short)17, (short)19, (short)145, (short)147,
(short)0, (short)83, (short)0, (short)112, (short)0, (short)0, (short)65,
(short)0, (short)0, (short)0, (short)0, (short)2, (short)8, (short)8, (short)8,
(short)8, (short)21, (short)0, (short)91, (short)92, (short)94, (short)26,
(short)28, (short)30, (short)83, (short)0, (short)83, (short)37, (short)0,
(short)24, (short)0, (short)0, (short)11, (short)0, (short)0, (short)0, (short)133,
(short)122, (short)41, (short)144, (short)8, (short)0, (short)33, (short)0,
(short)0, (short)275, (short)274, (short)264, (short)273, (short)0, (short)0,
(short)0, (short)83, (short)0, (short)108, (short)113, (short)0, (short)0,
(short)67, (short)68, (short)64, (short)0, (short)41, (short)41, (short)186,
(short)188, (short)192, (short)0, (short)124, (short)166, (short)0, (short)163,
(short)0, (short)384, (short)0, (short)0, (short)0, (short)0, (short)346,
(short)347, (short)348, (short)349, (short)350, (short)351, (short)0, (short)0,
(short)0, (short)381, (short)371, (short)382, (short)383, (short)0, (short)0,
(short)0, (short)0, (short)354, (short)0, (short)352, (short)353, (short)0,
(short)286, (short)299, (short)305, (short)307, (short)309, (short)311, (short)313,
(short)315, (short)318, (short)323, (short)326, (short)329, (short)333, (short)0,
(short)0, (short)335, (short)355, (short)370, (short)369, (short)0, (short)3,
(short)0, (short)5, (short)0, (short)22, (short)0, (short)0, (short)0, (short)12,
(short)27, (short)29, (short)31, (short)82, (short)0, (short)88, (short)89,
(short)0, (short)81, (short)38, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)385, (short)220, (short)222, (short)0, (short)228, (short)226, (short)0,
(short)227, (short)211, (short)212, (short)213, (short)0, (short)224, (short)214,
(short)215, (short)216, (short)0, (short)284, (short)25, (short)13, (short)94,
(short)169, (short)0, (short)128, (short)0, (short)150, (short)0, (short)168,
(short)0, (short)125, (short)0, (short)134, (short)0, (short)0, (short)76,
(short)0, (short)74, (short)262, (short)277, (short)0, (short)276, (short)0,
(short)0, (short)275, (short)146, (short)109, (short)83, (short)0, (short)0,
(short)115, (short)119, (short)114, (short)159, (short)189, (short)0, (short)97,
(short)99, (short)0, (short)0, (short)194, (short)187, (short)195, (short)0,
(short)85, (short)0, (short)84, (short)0, (short)165, (short)194, (short)167,
(short)66, (short)0, (short)0, (short)343, (short)301, (short)333, (short)0,
(short)302, (short)303, (short)0, (short)0, (short)0, (short)0, (short)336,
(short)337, (short)0, (short)0, (short)0, (short)0, (short)339, (short)340,
(short)338, (short)140, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)289, (short)290,
(short)291, (short)292, (short)293, (short)294, (short)295, (short)296, (short)297,
(short)298, (short)288, (short)0, (short)0, (short)341, (short)0, (short)363,
(short)364, (short)0, (short)0, (short)0, (short)373, (short)0, (short)0,
(short)141, (short)18, (short)8, (short)20, (short)8, (short)93, (short)95,
(short)172, (short)171, (short)14, (short)0, (short)78, (short)90, (short)0,
(short)0, (short)101, (short)102, (short)104, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)254, (short)255, (short)256,
(short)0, (short)0, (short)258, (short)0, (short)217, (short)218, (short)0,
(short)0, (short)94, (short)223, (short)225, (short)221, (short)0, (short)157,
(short)0, (short)0, (short)153, (short)127, (short)0, (short)0, (short)130,
(short)0, (short)132, (short)131, (short)126, (short)148, (short)136, (short)135,
(short)8, (short)47, (short)46, (short)0, (short)44, (short)0, (short)72,
(short)0, (short)270, (short)0, (short)23, (short)271, (short)0, (short)0,
(short)0, (short)0, (short)182, (short)279, (short)282, (short)0, (short)281,
(short)0, (short)110, (short)0, (short)111, (short)116, (short)0, (short)0,
(short)0, (short)0, (short)190, (short)98, (short)100, (short)0, (short)202,
(short)0, (short)0, (short)198, (short)0, (short)196, (short)0, (short)0,
(short)0, (short)210, (short)191, (short)193, (short)164, (short)0, (short)0,
(short)173, (short)177, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)372, (short)379, (short)0,
(short)378, (short)306, (short)0, (short)308, (short)310, (short)312, (short)314,
(short)316, (short)317, (short)321, (short)322, (short)319, (short)320, (short)324,
(short)325, (short)327, (short)328, (short)330, (short)331, (short)332, (short)287,
(short)0, (short)361, (short)362, (short)357, (short)0, (short)359, (short)360,
(short)0, (short)0, (short)0, (short)4, (short)6, (short)0, (short)304, (short)105,
(short)96, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)253, (short)257, (short)0, (short)0,
(short)0, (short)219, (short)0, (short)15, (short)285, (short)0, (short)155,
(short)0, (short)129, (short)0, (short)152, (short)151, (short)170, (short)149,
(short)137, (short)7, (short)0, (short)0, (short)0, (short)77, (short)75,
(short)278, (short)185, (short)0, (short)280, (short)181, (short)183, (short)272,
(short)0, (short)269, (short)107, (short)118, (short)117, (short)0, (short)161,
(short)160, (short)0, (short)204, (short)197, (short)200, (short)0, (short)205,
(short)0, (short)0, (short)199, (short)0, (short)342, (short)0, (short)174,
(short)0, (short)178, (short)367, (short)368, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)334, (short)375, (short)0, (short)0, (short)344,
(short)358, (short)356, (short)376, (short)0, (short)0, (short)80, (short)103,
(short)106, (short)79, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)233, (short)259, (short)0, (short)16,
(short)158, (short)156, (short)154, (short)0, (short)0, (short)43, (short)45,
(short)184, (short)283, (short)0, (short)0, (short)203, (short)0, (short)207,
(short)209, (short)208, (short)201, (short)0, (short)175, (short)179, (short)0,
(short)0, (short)377, (short)0, (short)380, (short)300, (short)374, (short)229,
(short)233, (short)239, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)260,
(short)48, (short)49, (short)120, (short)162, (short)206, (short)0, (short)180,
(short)0, (short)365, (short)0, (short)0, (short)0, (short)0, (short)241,
(short)0, (short)0, (short)0, (short)249, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)232, (short)0, (short)345,
(short)366, (short)230, (short)231, (short)240, (short)242, (short)243, (short)0,
(short)251, (short)250, (short)0, (short)245, (short)0, (short)0, (short)0,
(short)0, (short)233, (short)234, (short)0, (short)244, (short)252, (short)246,
(short)247, (short)0, (short)0, (short)233, (short)235, (short)176, (short)248,
(short)233, (short)236, (short)233, (short)237, (short)238, (short)0, (short)0,
(short)0}; static short Cyc_yydefgoto[ 106u]={ (short)725, (short)38, (short)39,
(short)40, (short)234, (short)41, (short)367, (short)42, (short)369, (short)43,
(short)205, (short)44, (short)45, (short)59, (short)60, (short)424, (short)425,
(short)46, (short)138, (short)47, (short)48, (short)264, (short)265, (short)49,
(short)84, (short)50, (short)211, (short)212, (short)95, (short)96, (short)97,
(short)213, (short)147, (short)379, (short)380, (short)381, (short)51, (short)278,
(short)279, (short)52, (short)53, (short)54, (short)55, (short)116, (short)78,
(short)420, (short)456, (short)408, (short)409, (short)284, (short)255, (short)149,
(short)256, (short)257, (short)471, (short)372, (short)472, (short)473, (short)436,
(short)437, (short)143, (short)144, (short)145, (short)457, (short)291, (short)237,
(short)238, (short)239, (short)240, (short)241, (short)242, (short)243, (short)661,
(short)244, (short)245, (short)268, (short)269, (short)270, (short)439, (short)440,
(short)441, (short)246, (short)247, (short)353, (short)178, (short)519, (short)179,
(short)180, (short)181, (short)182, (short)183, (short)184, (short)185, (short)186,
(short)187, (short)188, (short)189, (short)190, (short)191, (short)192, (short)193,
(short)194, (short)483, (short)484, (short)195, (short)196}; static short Cyc_yypact[
728u]={ (short)1411, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)45, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)8, (short)17, (short)2371, - (short)-32768, (short)17,
(short)15, (short)202, (short)202, - (short)-32768, (short)40, - (short)-32768,
(short)35, (short)482, - (short)-32768, (short)52, (short)85, (short)101, -
(short)-32768, - (short)-32768, (short)1411, - (short)-32768, (short)177,
(short)241, - (short)-32768, (short)385, - (short)8, - (short)8, - (short)8, -
(short)-32768, - (short)-32768, (short)151, - (short)-32768, (short)1723,
(short)59, (short)5, - (short)2, - (short)-32768, (short)120, (short)108,
(short)1772, - (short)-32768, (short)142, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)2371, - (short)-32768, (short)141,
(short)196, (short)176, (short)2371, (short)198, (short)210, - (short)-32768, -
(short)-32768, (short)225, - (short)-32768, (short)17, (short)145, (short)17,
(short)83, (short)140, (short)1938, - (short)-32768, (short)212, (short)1997,
(short)3462, (short)3462, - (short)-32768, (short)1411, (short)1411, (short)1411,
(short)1411, - (short)-32768, (short)231, (short)227, - (short)-32768, (short)1673,
(short)1772, (short)1772, (short)1772, (short)35, (short)1997, (short)35,
(short)268, (short)836, - (short)-32768, (short)1723, (short)385, - (short)-32768,
(short)1350, (short)1938, (short)2390, - (short)-32768, (short)59, - (short)8, -
(short)-32768, (short)1411, (short)255, - (short)-32768, (short)17, (short)281,
(short)2371, - (short)-32768, - (short)-32768, - (short)-32768, (short)3462,
(short)284, (short)140, (short)35, (short)17, (short)270, - (short)-32768,
(short)17, (short)17, - (short)-32768, - (short)-32768, - (short)-32768, (short)94,
- (short)8, - (short)8, (short)301, - (short)-32768, (short)283, (short)6, -
(short)-32768, (short)356, (short)326, - (short)-32768, (short)3663, - (short)-32768,
(short)3462, (short)3529, (short)299, (short)304, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)315,
(short)3730, (short)3730, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)1182, (short)346, (short)3797, (short)3797, - (short)-32768,
(short)3797, - (short)-32768, - (short)-32768, (short)309, - (short)-32768, -
(short)49, (short)368, (short)342, (short)300, (short)348, (short)374, (short)79,
(short)388, (short)343, (short)56, - (short)-32768, (short)832, (short)370,
(short)3797, (short)106, - (short)-32768, - (short)-32768, (short)33, (short)379,
- (short)-32768, (short)395, - (short)-32768, (short)397, - (short)-32768,
(short)482, (short)2457, (short)1723, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)389, (short)403, (short)1997, - (short)-32768,
(short)461, (short)419, - (short)-32768, (short)424, (short)431, (short)447,
(short)1825, (short)449, (short)464, (short)423, (short)468, (short)2524,
(short)1825, (short)306, (short)1825, (short)1825, (short)459, - (short)-32768,
- (short)-32768, (short)483, - (short)-32768, - (short)-32768, (short)385, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)489,
(short)953, - (short)-32768, - (short)-32768, - (short)-32768, (short)189, -
(short)-32768, - (short)-32768, - (short)-32768, (short)490, - (short)-32768,
(short)58, - (short)-32768, (short)495, (short)220, (short)500, (short)488,
(short)13, - (short)-32768, (short)492, (short)111, (short)508, (short)27,
(short)505, (short)510, (short)512, - (short)-32768, - (short)-32768, (short)511,
(short)507, (short)222, (short)3833, (short)2371, - (short)-32768, - (short)-32768,
(short)35, (short)17, (short)515, (short)224, - (short)17, - (short)-32768, -
(short)9, - (short)-32768, (short)518, (short)1997, (short)1997, (short)1565,
(short)2591, (short)96, - (short)-32768, (short)78, (short)58, - (short)-32768,
(short)1938, - (short)-32768, (short)1518, - (short)-32768, (short)49, - (short)-32768,
- (short)-32768, (short)1997, (short)1182, - (short)-32768, - (short)-32768, -
(short)-32768, (short)2050, - (short)-32768, (short)544, (short)3462, (short)1462,
(short)582, (short)1182, - (short)-32768, - (short)-32768, (short)953, (short)521,
(short)361, (short)3462, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)3797, (short)3462, (short)3797, (short)3797, (short)3797,
(short)3797, (short)3797, (short)3797, (short)3797, (short)3797, (short)3797,
(short)3797, (short)3797, (short)3797, (short)3797, (short)3797, (short)3797,
(short)3797, (short)3797, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)3462, (short)3462, -
(short)-32768, (short)175, - (short)-32768, - (short)-32768, (short)2658,
(short)178, (short)3462, - (short)-32768, (short)2305, (short)520, - (short)-32768,
- (short)-32768, (short)1411, - (short)-32768, (short)1411, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)1997, -
(short)-32768, - (short)-32768, (short)3462, (short)527, (short)525, - (short)-32768,
(short)522, (short)1997, (short)3462, (short)3462, (short)3462, (short)597,
(short)1070, (short)534, - (short)-32768, - (short)-32768, - (short)-32768,
(short)249, (short)587, - (short)-32768, (short)2725, - (short)-32768, - (short)-32768,
(short)1825, (short)1294, (short)1673, - (short)-32768, - (short)-32768, -
(short)-32768, (short)3462, (short)546, (short)261, (short)533, (short)528, -
(short)-32768, (short)58, (short)1983, - (short)-32768, (short)549, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)111,
(short)1411, - (short)-32768, (short)545, (short)543, (short)547, (short)17, -
(short)-32768, (short)3462, - (short)-32768, (short)2371, - (short)-32768, -
(short)-32768, (short)559, (short)3462, (short)2371, (short)117, - (short)-32768,
- (short)-32768, - (short)-32768, (short)550, (short)548, (short)551, - (short)-32768,
(short)556, - (short)-32768, (short)17, (short)17, (short)557, (short)140,
(short)555, - (short)-32768, - (short)-32768, - (short)-32768, (short)58, -
(short)-32768, (short)560, (short)563, - (short)-32768, (short)558, (short)78,
(short)1880, (short)1938, (short)2792, - (short)-32768, - (short)-32768, (short)283,
- (short)-32768, (short)564, (short)569, - (short)-32768, - (short)-32768,
(short)307, (short)2457, (short)381, (short)565, (short)482, (short)570, (short)568,
(short)562, (short)3596, - (short)-32768, - (short)-32768, (short)571, (short)572,
(short)368, (short)180, (short)342, (short)300, (short)348, (short)374, (short)79,
(short)79, (short)388, (short)388, (short)388, (short)388, (short)343, (short)343,
(short)56, (short)56, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)575, - (short)-32768, - (short)-32768, - (short)-32768,
(short)576, - (short)-32768, - (short)-32768, (short)102, (short)318, (short)1938,
- (short)-32768, - (short)-32768, (short)581, - (short)-32768, - (short)-32768,
- (short)-32768, (short)461, (short)3462, (short)584, (short)398, (short)411,
(short)413, (short)573, (short)2859, (short)2926, (short)264, - (short)-32768, -
(short)-32768, (short)603, (short)605, (short)601, - (short)-32768, (short)1723,
- (short)-32768, - (short)-32768, (short)140, - (short)-32768, (short)606, -
(short)-32768, (short)58, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)401, (short)602, (short)27,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)595,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)3863,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)1997,
(short)604, - (short)-32768, (short)607, - (short)-32768, - (short)-32768, -
(short)-32768, (short)58, - (short)-32768, (short)608, (short)18, - (short)-32768,
(short)600, (short)610, (short)646, - (short)-32768, (short)2135, - (short)-32768,
- (short)-32768, - (short)-32768, (short)1723, (short)1997, (short)610, (short)611,
(short)2305, - (short)-32768, - (short)-32768, (short)3462, (short)3462, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)2305,
(short)327, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)1825, (short)614, (short)1825, (short)3462, (short)2993, (short)266,
(short)3060, (short)287, (short)3127, (short)502, - (short)-32768, (short)659, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)655,
(short)669, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)432, (short)555, - (short)-32768, (short)672, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)3462, - (short)-32768, - (short)-32768,
(short)2457, (short)673, - (short)-32768, (short)339, - (short)-32768, - (short)-32768,
- (short)-32768, (short)746, (short)502, - (short)-32768, (short)434, (short)1825,
(short)436, (short)3194, (short)1825, (short)440, (short)3261, (short)3328,
(short)298, (short)2371, (short)670, (short)678, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)197,
- (short)-32768, (short)677, - (short)-32768, (short)2220, (short)1825, (short)681,
(short)684, - (short)-32768, (short)1825, (short)1825, (short)454, - (short)-32768,
(short)1825, (short)1825, (short)469, (short)1825, (short)475, (short)3395,
(short)0, (short)953, - (short)-32768, (short)3462, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)1825, - (short)-32768, - (short)-32768, (short)1825, - (short)-32768,
(short)1825, (short)1825, (short)477, (short)3462, (short)707, - (short)-32768,
(short)340, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)1825, (short)199, (short)502, - (short)-32768, - (short)-32768, - (short)-32768,
(short)707, - (short)-32768, (short)502, - (short)-32768, - (short)-32768,
(short)778, (short)779, - (short)-32768}; static short Cyc_yypgoto[ 106u]={ -
(short)-32768, (short)254, - (short)-32768, (short)470, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)48, -
(short)34, - (short)52, - (short)-32768, (short)292, - (short)42, (short)228, -
(short)-32768, (short)88, - (short)124, - (short)69, - (short)-32768, - (short)-32768,
(short)358, - (short)-32768, - (short)16, - (short)-32768, - (short)149, -
(short)-32768, - (short)-32768, - (short)-32768, (short)579, (short)574, (short)75,
- (short)-32768, - (short)-32768, (short)267, - (short)-32768, - (short)240, -
(short)-32768, - (short)6, - (short)15, - (short)111, - (short)-32768, - (short)-32768,
(short)41, - (short)-32768, - (short)95, - (short)346, (short)493, - (short)375,
- (short)84, - (short)281, - (short)-32768, - (short)-32768, - (short)196,
(short)634, - (short)344, - (short)243, - (short)-32768, (short)354, - (short)116,
(short)509, - (short)100, (short)229, - (short)192, - (short)66, - (short)-32768,
- (short)-32768, - (short)46, - (short)239, - (short)-32768, - (short)-32768, -
(short)582, - (short)-32768, - (short)-32768, - (short)7, (short)526, - (short)-32768,
(short)239, - (short)-32768, - (short)-32768, - (short)56, - (short)78, -
(short)-32768, - (short)139, - (short)404, (short)652, (short)486, (short)481,
(short)484, (short)485, (short)494, (short)248, (short)170, (short)247, (short)258,
(short)80, (short)355, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)276, - (short)-32768, - (short)-32768, (short)1}; static
short Cyc_yytable[ 3977u]={ (short)109, (short)56, (short)148, (short)403,
(short)107, (short)274, (short)110, (short)120, (short)371, (short)177, (short)197,
(short)114, (short)258, (short)304, (short)141, (short)72, (short)254, (short)141,
(short)108, (short)513, (short)467, (short)323, (short)61, (short)73, (short)555,
(short)74, (short)85, (short)79, (short)81, (short)435, (short)558, (short)289,
(short)542, (short)56, (short)141, (short)260, (short)298, (short)444, (short)98,
(short)115, (short)56, (short)57, (short)141, (short)141, (short)422, (short)56,
(short)109, (short)207, (short)208, (short)209, (short)107, (short)104, (short)206,
(short)316, (short)236, (short)56, (short)109, (short)128, (short)235, (short)122,
(short)248, (short)324, (short)249, (short)132, (short)126, (short)545, (short)674,
(short)73, (short)80, (short)706, (short)293, (short)271, (short)73, (short)114,
(short)449, (short)570, (short)479, (short)415, (short)505, (short)130, -
(short)83, (short)133, (short)634, (short)509, (short)58, (short)83, (short)210,
(short)76, (short)215, (short)28, (short)450, (short)56, (short)56, (short)56,
(short)56, (short)30, (short)117, (short)460, (short)77, (short)75, (short)141,
(short)28, (short)32, (short)250, (short)707, (short)294, (short)460, (short)30,
(short)571, (short)295, (short)56, (short)423, (short)294, (short)317, (short)275,
(short)362, (short)416, (short)294, (short)605, (short)56, (short)535, (short)635,
(short)266, (short)82, (short)73, (short)717, (short)373, (short)363, (short)15,
(short)16, (short)17, (short)467, (short)276, (short)28, (short)721, (short)280,
(short)281, (short)83, (short)723, (short)30, (short)724, (short)297, (short)364,
(short)141, (short)331, (short)332, (short)296, (short)406, (short)56, (short)86,
(short)419, (short)58, (short)407, (short)109, (short)387, (short)288, (short)111,
(short)248, (short)142, (short)374, (short)394, (short)112, (short)397, (short)398,
(short)339, (short)113, (short)356, (short)357, (short)358, (short)393, (short)58,
(short)140, (short)340, (short)341, (short)140, (short)461, (short)289, (short)134,
(short)214, (short)87, (short)462, (short)333, (short)334, (short)282, (short)463,
(short)298, (short)468, (short)142, (short)135, (short)283, (short)236, (short)140,
(short)418, (short)287, (short)235, (short)88, (short)478, (short)250, (short)622,
(short)140, (short)140, (short)405, (short)288, (short)359, (short)56, (short)119,
(short)566, (short)567, (short)382, (short)599, (short)459, (short)360, (short)361,
(short)560, (short)118, (short)56, (short)141, (short)141, (short)141, (short)35,
(short)36, (short)37, (short)433, (short)434, (short)136, (short)141, (short)517,
(short)141, (short)373, (short)632, (short)401, (short)137, (short)141, (short)141,
(short)523, (short)28, (short)121, (short)56, (short)123, (short)518, (short)482,
(short)30, (short)102, (short)141, (short)142, (short)103, (short)317, (short)83,
(short)643, (short)464, (short)131, (short)319, (short)320, (short)474, (short)321,
(short)666, (short)317, (short)140, (short)476, (short)506, (short)443, (short)125,
(short)510, (short)236, (short)448, (short)438, (short)507, (short)235, (short)486,
(short)511, (short)90, (short)91, (short)355, (short)73, (short)73, (short)504,
(short)482, (short)585, (short)280, (short)405, (short)124, (short)482, (short)404,
(short)115, (short)596, (short)373, (short)28, (short)214, (short)405, (short)518,
(short)85, (short)76, (short)30, (short)89, (short)127, (short)518, (short)405,
(short)56, (short)405, (short)56, (short)140, (short)690, (short)77, (short)720,
- (short)83, (short)512, (short)141, - (short)83, (short)129, (short)549,
(short)146, (short)56, (short)83, (short)411, (short)141, (short)431, (short)148,
(short)446, (short)482, (short)412, (short)435, (short)405, (short)482, (short)447,
(short)202, (short)569, (short)203, (short)539, (short)524, (short)525, (short)526,
(short)141, (short)530, (short)536, (short)92, (short)93, (short)109, (short)99,
(short)100, (short)101, (short)529, (short)640, (short)532, (short)141, (short)198,
(short)199, (short)200, (short)201, (short)405, (short)109, (short)282, (short)548,
(short)263, (short)107, (short)216, (short)538, (short)541, (short)615, (short)640,
(short)653, (short)452, (short)453, (short)578, (short)405, (short)277, (short)405,
(short)577, (short)537, (short)56, (short)142, (short)56, (short)290, (short)262,
(short)140, (short)140, (short)140, (short)299, (short)142, (short)272, (short)267,
(short)656, (short)273, (short)140, (short)518, (short)140, (short)580, (short)405,
(short)142, (short)639, (short)140, (short)140, (short)686, (short)141, (short)141,
(short)292, (short)373, (short)309, (short)405, (short)287, (short)395, (short)140,
(short)310, (short)583, (short)396, (short)322, (short)639, (short)584, (short)288,
(short)261, (short)35, (short)36, (short)37, (short)311, (short)600, (short)602,
(short)327, (short)620, (short)601, (short)464, (short)501, (short)502, (short)503,
(short)56, (short)556, (short)300, (short)301, (short)294, (short)266, (short)559,
(short)640, (short)646, (short)73, (short)285, (short)286, (short)671, (short)718,
(short)73, (short)325, (short)672, (short)405, (short)28, (short)329, (short)330,
(short)318, (short)669, (short)141, (short)30, (short)280, (short)280, (short)708,
(short)214, (short)335, (short)336, (short)296, (short)337, (short)338, (short)326,
(short)645, (short)214, (short)481, (short)405, (short)328, (short)288, (short)140,
(short)35, (short)36, (short)37, (short)354, (short)716, (short)28, (short)588,
(short)140, (short)612, (short)614, (short)365, (short)30, (short)639, (short)56,
(short)94, (short)586, (short)405, (short)722, (short)32, (short)375, (short)629,
(short)109, (short)623, (short)624, (short)140, (short)248, (short)366, (short)619,
(short)368, (short)35, (short)36, (short)37, (short)607, (short)405, (short)376,
(short)141, (short)140, (short)493, (short)494, (short)495, (short)496, (short)303,
(short)373, (short)305, (short)305, (short)608, (short)405, (short)609, (short)405,
(short)383, (short)373, (short)382, (short)390, (short)644, (short)313, (short)314,
(short)141, (short)384, (short)56, (short)373, (short)659, (short)660, (short)305,
(short)305, (short)385, (short)305, (short)665, (short)301, (short)675, (short)405,
(short)677, (short)405, (short)109, (short)142, (short)681, (short)405, (short)107,
(short)647, (short)206, (short)649, (short)386, (short)28, (short)388, (short)305,
(short)389, (short)140, (short)140, (short)30, (short)698, (short)405, (short)650,
(short)652, (short)438, (short)655, (short)32, (short)658, (short)593, (short)391,
(short)373, (short)399, (short)73, (short)378, (short)28, (short)701, (short)405,
(short)35, (short)36, (short)37, (short)30, (short)703, (short)405, (short)714,
(short)405, (short)491, (short)492, (short)32, (short)400, (short)668, (short)497,
(short)498, (short)402, (short)676, (short)204, (short)414, (short)680, (short)142,
(short)35, (short)36, (short)37, (short)410, (short)373, (short)499, (short)500,
(short)679, (short)413, (short)417, (short)683, (short)685, (short)140, (short)421,
(short)426, (short)427, (short)430, (short)693, (short)428, (short)429, (short)445,
(short)696, (short)697, (short)451, (short)323, (short)699, (short)700, (short)477,
(short)702, (short)480, (short)520, (short)515, (short)514, (short)516, (short)521,
(short)527, (short)522, (short)531, (short)533, (short)540, (short)705, (short)543,
(short)710, (short)547, (short)709, (short)711, (short)236, (short)712, (short)713,
(short)544, (short)235, (short)552, (short)551, (short)557, (short)282, (short)562,
(short)553, (short)563, (short)719, (short)564, (short)715, (short)565, (short)687,
(short)582, (short)568, (short)236, (short)140, (short)591, (short)572, (short)235,
(short)73, (short)573, (short)581, (short)587, (short)641, (short)574, (short)590,
(short)589, (short)236, (short)594, (short)610, (short)595, (short)235, (short)597,
(short)598, (short)550, (short)603, (short)140, (short)305, (short)606, (short)305,
(short)305, (short)305, (short)305, (short)305, (short)305, (short)305, (short)305,
(short)305, (short)305, (short)305, (short)305, (short)305, (short)305, (short)305,
(short)305, (short)305, (short)616, (short)617, (short)618, (short)625, (short)621,
(short)627, (short)630, (short)592, (short)631, (short)633, (short)636, (short)648,
(short)642, (short)1, (short)2, (short)3, (short)105, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)659,
(short)660, (short)20, (short)217, (short)305, (short)218, (short)219, (short)220,
(short)221, (short)222, (short)223, (short)224, (short)225, (short)150, (short)21,
(short)151, (short)22, (short)152, (short)226, (short)637, (short)153, (short)23,
(short)227, (short)662, (short)663, (short)26, (short)27, (short)154, (short)155,
(short)228, (short)229, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)664, (short)163, (short)164, (short)667, (short)670,
(short)673, (short)689, (short)688, (short)691, (short)694, (short)695, (short)726,
(short)727, (short)475, (short)626, (short)370, (short)305, (short)554, (short)465,
(short)377, (short)307, (short)604, (short)305, (short)561, (short)230, (short)165,
(short)166, (short)167, (short)168, (short)29, (short)30, (short)31, (short)442,
(short)231, (short)106, (short)628, (short)466, (short)169, (short)308, (short)487,
(short)33, (short)233, (short)485, (short)488, (short)0, (short)489, (short)0,
(short)0, (short)171, (short)0, (short)0, (short)172, (short)173, (short)174,
(short)0, (short)490, (short)0, (short)0, (short)175, (short)176, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)305,
(short)0, (short)0, (short)0, (short)1, (short)2, (short)3, (short)105, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)217, (short)0, (short)218, (short)219, (short)220,
(short)221, (short)222, (short)223, (short)224, (short)225, (short)150, (short)21,
(short)151, (short)22, (short)152, (short)226, (short)305, (short)153, (short)23,
(short)227, (short)0, (short)0, (short)26, (short)27, (short)154, (short)155,
(short)228, (short)229, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)342, (short)343, (short)344, (short)345, (short)346,
(short)347, (short)348, (short)349, (short)350, (short)351, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)230, (short)165,
(short)166, (short)167, (short)168, (short)29, (short)30, (short)31, (short)352,
(short)231, (short)106, (short)232, (short)0, (short)169, (short)0, (short)0,
(short)33, (short)233, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)171, (short)0, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)305, (short)0, (short)0, (short)175, (short)176, (short)1, (short)2,
(short)3, (short)105, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)217, (short)0,
(short)218, (short)219, (short)220, (short)221, (short)222, (short)223, (short)224,
(short)225, (short)150, (short)21, (short)151, (short)22, (short)152, (short)226,
(short)0, (short)153, (short)23, (short)227, (short)0, (short)0, (short)26,
(short)27, (short)154, (short)155, (short)228, (short)229, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)230, (short)165,
(short)166, (short)167, (short)168, (short)29, (short)30, (short)31, (short)0,
(short)231, (short)106, (short)0, (short)0, (short)169, (short)0, (short)0,
(short)33, (short)233, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)171, (short)0, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)0, (short)0, (short)0, (short)175, (short)176, (short)1, (short)2,
(short)3, (short)105, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)150,
(short)21, (short)151, (short)22, (short)152, (short)0, (short)0, (short)153,
(short)23, (short)0, (short)0, (short)0, (short)26, (short)27, (short)154,
(short)155, (short)0, (short)0, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)163, (short)164, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)29, (short)30, (short)31, (short)0, (short)528,
(short)0, (short)0, (short)0, (short)169, (short)0, (short)0, (short)33, (short)233,
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
(short)315, (short)0, (short)0, (short)169, (short)0, (short)0, (short)33,
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
(short)2, (short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)251, (short)20, (short)0,
(short)0, (short)0, (short)29, (short)0, (short)31, (short)0, (short)252,
(short)0, (short)0, (short)0, (short)21, (short)253, (short)22, (short)33,
(short)34, (short)0, (short)0, (short)23, (short)0, (short)24, (short)25,
(short)26, (short)27, (short)0, (short)0, (short)0, (short)0, (short)0, (short)1,
(short)2, (short)3, (short)105, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)20, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28,
(short)0, (short)0, (short)21, (short)0, (short)29, (short)30, (short)31,
(short)0, (short)0, (short)23, (short)0, (short)0, (short)32, (short)26, (short)27,
(short)33, (short)34, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)35, (short)36, (short)37, (short)0, (short)0, (short)0, (short)0, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)30, (short)31, (short)21, (short)0,
(short)0, (short)0, (short)0, (short)32, (short)0, (short)0, (short)33, (short)34,
(short)0, (short)26, (short)27, (short)0, (short)0, (short)0, (short)35, (short)36,
(short)37, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)21, (short)28,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)30, (short)31, (short)0,
(short)454, (short)26, (short)27, (short)0, (short)296, (short)455, (short)0,
(short)33, (short)34, (short)0, (short)0, (short)0, (short)0, (short)288,
(short)0, (short)35, (short)36, (short)37, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)0, (short)31,
(short)0, (short)454, (short)0, (short)0, (short)0, (short)287, (short)455,
(short)0, (short)33, (short)34, (short)0, (short)0, (short)0, (short)0, (short)288,
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
(short)0, (short)0, (short)0, (short)175, (short)176, (short)0, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)0, (short)31, (short)0, (short)575, (short)21, (short)0,
(short)0, (short)0, (short)576, (short)0, (short)33, (short)34, (short)0,
(short)0, (short)0, (short)26, (short)27, (short)0, (short)0, (short)0, (short)0,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)21,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29,
(short)0, (short)31, (short)26, (short)27, (short)139, (short)21, (short)0,
(short)0, (short)0, (short)0, (short)33, (short)34, (short)0, (short)0, (short)0,
(short)0, (short)26, (short)27, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)546, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)469, (short)0, (short)0,
(short)33, (short)34, (short)150, (short)29, (short)151, (short)31, (short)152,
(short)0, (short)0, (short)153, (short)0, (short)0, (short)0, (short)0, (short)33,
(short)34, (short)154, (short)155, (short)0, (short)0, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165,
(short)166, (short)167, (short)168, (short)0, (short)30, (short)0, (short)0,
(short)0, (short)306, (short)470, (short)0, (short)169, (short)0, (short)0,
(short)0, (short)170, (short)0, (short)0, (short)0, (short)433, (short)434,
(short)0, (short)171, (short)0, (short)0, (short)172, (short)173, (short)174,
(short)0, (short)0, (short)0, (short)0, (short)175, (short)176, (short)150,
(short)0, (short)151, (short)0, (short)152, (short)0, (short)0, (short)153,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)154, (short)155,
(short)0, (short)0, (short)156, (short)157, (short)158, (short)159, (short)160,
(short)161, (short)162, (short)0, (short)163, (short)164, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)165, (short)166, (short)167,
(short)168, (short)0, (short)30, (short)0, (short)0, (short)0, (short)306,
(short)638, (short)0, (short)169, (short)0, (short)0, (short)0, (short)170,
(short)0, (short)0, (short)0, (short)433, (short)434, (short)0, (short)171,
(short)0, (short)0, (short)172, (short)173, (short)174, (short)0, (short)0,
(short)0, (short)0, (short)175, (short)176, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)0, (short)153, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)154, (short)155, (short)0,
(short)0, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)165, (short)166, (short)167, (short)168,
(short)0, (short)30, (short)0, (short)0, (short)0, (short)306, (short)692,
(short)0, (short)169, (short)0, (short)0, (short)0, (short)170, (short)0,
(short)0, (short)0, (short)433, (short)434, (short)0, (short)171, (short)0,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)0,
(short)0, (short)175, (short)176, (short)150, (short)0, (short)151, (short)0,
(short)152, (short)0, (short)0, (short)153, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)154, (short)155, (short)0, (short)0,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)0, (short)163, (short)164, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)165, (short)166, (short)167, (short)168, (short)0,
(short)30, (short)0, (short)0, (short)0, (short)306, (short)0, (short)0, (short)169,
(short)0, (short)0, (short)0, (short)170, (short)0, (short)62, (short)0, (short)433,
(short)434, (short)0, (short)171, (short)0, (short)0, (short)172, (short)173,
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
(short)0, (short)0, (short)0, (short)306, (short)0, (short)0, (short)169,
(short)0, (short)0, (short)0, (short)170, (short)150, (short)0, (short)151,
(short)0, (short)152, (short)0, (short)171, (short)153, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)154, (short)155, (short)175,
(short)176, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)165, (short)166, (short)167, (short)168,
(short)0, (short)30, (short)0, (short)0, (short)392, (short)0, (short)0, (short)0,
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
(short)0, (short)151, (short)0, (short)152, (short)458, (short)171, (short)153,
(short)0, (short)172, (short)173, (short)174, (short)0, (short)0, (short)154,
(short)155, (short)175, (short)176, (short)156, (short)157, (short)158, (short)159,
(short)160, (short)161, (short)162, (short)0, (short)163, (short)164, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)165, (short)166,
(short)167, (short)168, (short)0, (short)30, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)169, (short)508, (short)0, (short)0,
(short)170, (short)150, (short)0, (short)151, (short)0, (short)152, (short)0,
(short)171, (short)153, (short)0, (short)172, (short)173, (short)174, (short)0,
(short)0, (short)154, (short)155, (short)175, (short)176, (short)156, (short)157,
(short)158, (short)159, (short)160, (short)161, (short)162, (short)0, (short)163,
(short)164, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)165,
(short)166, (short)167, (short)168, (short)0, (short)30, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)169, (short)534, (short)0,
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
(short)306, (short)0, (short)0, (short)169, (short)0, (short)0, (short)0,
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
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)302, (short)0,
(short)0, (short)0, (short)170, (short)150, (short)0, (short)151, (short)0,
(short)0, (short)0, (short)171, (short)0, (short)0, (short)172, (short)173,
(short)174, (short)0, (short)0, (short)154, (short)155, (short)175, (short)176,
(short)156, (short)157, (short)158, (short)159, (short)160, (short)161, (short)162,
(short)0, (short)163, (short)164, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)165, (short)166, (short)167, (short)168, (short)0,
(short)30, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)312,
(short)0, (short)0, (short)0, (short)170, (short)150, (short)0, (short)151,
(short)0, (short)0, (short)0, (short)171, (short)0, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)154, (short)155, (short)175,
(short)176, (short)156, (short)157, (short)158, (short)159, (short)160, (short)161,
(short)162, (short)0, (short)163, (short)164, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)62,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)165, (short)166, (short)167, (short)168,
(short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)169, (short)0, (short)0, (short)0, (short)170, (short)0, (short)62,
(short)0, (short)0, (short)0, (short)0, (short)171, (short)0, (short)0, (short)172,
(short)173, (short)174, (short)0, (short)0, (short)0, (short)0, (short)175,
(short)176, (short)28, (short)63, (short)0, (short)64, (short)65, (short)0,
(short)30, (short)0, (short)0, (short)0, (short)0, (short)432, (short)0, (short)66,
(short)0, (short)0, (short)67, (short)68, (short)0, (short)0, (short)0, (short)433,
(short)434, (short)0, (short)69, (short)0, (short)0, (short)0, (short)70,
(short)71, (short)28, (short)63, (short)0, (short)64, (short)65, (short)0,
(short)30, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)66,
(short)0, (short)0, (short)67, (short)68, (short)0, (short)0, (short)0, (short)433,
(short)434, (short)0, (short)69, (short)0, (short)0, (short)0, (short)70,
(short)71}; static short Cyc_yycheck[ 3977u]={ (short)52, (short)0, (short)86,
(short)242, (short)52, (short)129, (short)52, (short)59, (short)204, (short)87,
(short)88, (short)53, (short)112, (short)152, (short)83, (short)22, (short)111,
(short)86, (short)52, (short)363, (short)301, (short)70, (short)21, (short)22,
(short)428, (short)24, (short)32, (short)26, (short)27, (short)272, (short)434,
(short)142, (short)407, (short)32, (short)103, (short)113, (short)147, (short)277,
(short)44, (short)54, (short)39, (short)86, (short)111, (short)112, (short)17,
(short)44, (short)98, (short)99, (short)100, (short)101, (short)98, (short)50,
(short)98, (short)169, (short)106, (short)54, (short)108, (short)73, (short)106,
(short)66, (short)108, (short)110, (short)108, (short)79, (short)71, (short)411,
(short)648, (short)66, (short)27, (short)69, (short)64, (short)127, (short)71,
(short)115, (short)83, (short)450, (short)315, (short)64, (short)354, (short)78,
(short)97, (short)80, (short)64, (short)359, (short)92, (short)102, (short)102,
(short)89, (short)104, (short)84, (short)99, (short)90, (short)91, (short)92,
(short)93, (short)90, (short)55, (short)289, (short)100, (short)84, (short)169,
(short)84, (short)97, (short)109, (short)104, (short)99, (short)298, (short)90,
(short)454, (short)103, (short)109, (short)84, (short)99, (short)169, (short)130,
(short)82, (short)103, (short)99, (short)522, (short)118, (short)396, (short)103,
(short)121, (short)83, (short)123, (short)707, (short)204, (short)94, (short)17,
(short)18, (short)19, (short)412, (short)131, (short)84, (short)716, (short)134,
(short)135, (short)102, (short)720, (short)90, (short)722, (short)147, (short)109,
(short)212, (short)65, (short)66, (short)97, (short)89, (short)147, (short)97,
(short)261, (short)92, (short)94, (short)205, (short)220, (short)106, (short)97,
(short)205, (short)83, (short)205, (short)226, (short)102, (short)228, (short)229,
(short)108, (short)106, (short)60, (short)61, (short)62, (short)225, (short)92,
(short)83, (short)116, (short)117, (short)86, (short)97, (short)287, (short)94,
(short)103, (short)94, (short)102, (short)102, (short)103, (short)89, (short)106,
(short)296, (short)302, (short)112, (short)105, (short)95, (short)242, (short)103,
(short)261, (short)97, (short)242, (short)94, (short)312, (short)203, (short)544,
(short)111, (short)112, (short)99, (short)106, (short)97, (short)203, (short)97,
(short)446, (short)447, (short)214, (short)107, (short)288, (short)105, (short)106,
(short)96, (short)94, (short)214, (short)285, (short)286, (short)287, (short)108,
(short)109, (short)110, (short)105, (short)106, (short)84, (short)294, (short)375,
(short)296, (short)306, (short)575, (short)236, (short)91, (short)301, (short)302,
(short)383, (short)84, (short)94, (short)236, (short)97, (short)378, (short)318,
(short)90, (short)91, (short)312, (short)169, (short)94, (short)302, (short)102,
(short)592, (short)291, (short)105, (short)171, (short)172, (short)309, (short)174,
(short)630, (short)312, (short)169, (short)310, (short)84, (short)276, (short)85,
(short)84, (short)315, (short)280, (short)272, (short)91, (short)315, (short)324,
(short)91, (short)93, (short)94, (short)192, (short)272, (short)273, (short)353,
(short)354, (short)473, (short)277, (short)99, (short)84, (short)359, (short)93,
(short)298, (short)104, (short)363, (short)84, (short)212, (short)99, (short)428,
(short)296, (short)89, (short)90, (short)39, (short)96, (short)434, (short)99,
(short)296, (short)99, (short)298, (short)212, (short)104, (short)100, (short)104,
(short)94, (short)361, (short)375, (short)97, (short)83, (short)420, (short)98,
(short)310, (short)102, (short)93, (short)383, (short)93, (short)400, (short)93,
(short)396, (short)99, (short)563, (short)99, (short)400, (short)99, (short)93,
(short)449, (short)99, (short)405, (short)384, (short)385, (short)386, (short)400,
(short)388, (short)399, (short)93, (short)94, (short)388, (short)45, (short)46,
(short)47, (short)388, (short)584, (short)93, (short)412, (short)90, (short)91,
(short)92, (short)93, (short)99, (short)401, (short)89, (short)420, (short)97,
(short)401, (short)86, (short)401, (short)95, (short)93, (short)601, (short)93,
(short)285, (short)286, (short)462, (short)99, (short)94, (short)99, (short)461,
(short)401, (short)367, (short)294, (short)369, (short)142, (short)118, (short)285,
(short)286, (short)287, (short)147, (short)302, (short)94, (short)98, (short)93,
(short)97, (short)294, (short)522, (short)296, (short)463, (short)99, (short)312,
(short)584, (short)301, (short)302, (short)93, (short)461, (short)462, (short)111,
(short)473, (short)97, (short)99, (short)97, (short)93, (short)312, (short)97,
(short)95, (short)97, (short)95, (short)601, (short)99, (short)106, (short)116,
(short)108, (short)109, (short)110, (short)97, (short)95, (short)514, (short)115,
(short)540, (short)99, (short)460, (short)339, (short)340, (short)341, (short)421,
(short)430, (short)98, (short)99, (short)99, (short)426, (short)435, (short)672,
(short)103, (short)430, (short)140, (short)141, (short)95, (short)95, (short)435,
(short)69, (short)99, (short)99, (short)84, (short)67, (short)68, (short)97,
(short)640, (short)514, (short)90, (short)446, (short)447, (short)688, (short)375,
(short)63, (short)64, (short)97, (short)111, (short)112, (short)114, (short)596,
(short)383, (short)98, (short)99, (short)113, (short)106, (short)375, (short)108,
(short)109, (short)110, (short)97, (short)707, (short)84, (short)476, (short)383,
(short)528, (short)529, (short)95, (short)90, (short)672, (short)476, (short)93,
(short)98, (short)99, (short)720, (short)97, (short)94, (short)568, (short)537,
(short)85, (short)86, (short)400, (short)537, (short)95, (short)537, (short)95,
(short)108, (short)109, (short)110, (short)98, (short)99, (short)95, (short)568,
(short)412, (short)331, (short)332, (short)333, (short)334, (short)150, (short)584,
(short)152, (short)153, (short)98, (short)99, (short)98, (short)99, (short)94,
(short)592, (short)521, (short)93, (short)595, (short)163, (short)164, (short)589,
(short)97, (short)521, (short)601, (short)22, (short)23, (short)171, (short)172,
(short)97, (short)174, (short)98, (short)99, (short)98, (short)99, (short)98,
(short)99, (short)588, (short)462, (short)98, (short)99, (short)588, (short)607,
(short)588, (short)609, (short)97, (short)84, (short)97, (short)192, (short)84,
(short)461, (short)462, (short)90, (short)98, (short)99, (short)610, (short)611,
(short)563, (short)613, (short)97, (short)615, (short)480, (short)93, (short)640,
(short)104, (short)563, (short)104, (short)84, (short)98, (short)99, (short)108,
(short)109, (short)110, (short)90, (short)98, (short)99, (short)98, (short)99,
(short)329, (short)330, (short)97, (short)97, (short)637, (short)335, (short)336,
(short)95, (short)651, (short)96, (short)99, (short)654, (short)514, (short)108,
(short)109, (short)110, (short)98, (short)672, (short)337, (short)338, (short)653,
(short)98, (short)107, (short)656, (short)657, (short)514, (short)95, (short)99,
(short)95, (short)99, (short)673, (short)96, (short)98, (short)95, (short)677,
(short)678, (short)95, (short)70, (short)681, (short)682, (short)35, (short)684,
(short)98, (short)93, (short)367, (short)102, (short)369, (short)99, (short)28,
(short)104, (short)93, (short)41, (short)83, (short)686, (short)98, (short)698,
(short)84, (short)690, (short)701, (short)688, (short)703, (short)704, (short)111,
(short)688, (short)98, (short)97, (short)84, (short)89, (short)95, (short)99,
(short)99, (short)714, (short)98, (short)706, (short)95, (short)659, (short)84,
(short)97, (short)707, (short)568, (short)95, (short)98, (short)707, (short)659,
(short)98, (short)98, (short)98, (short)589, (short)107, (short)98, (short)97,
(short)720, (short)98, (short)97, (short)99, (short)720, (short)98, (short)98,
(short)421, (short)95, (short)589, (short)323, (short)95, (short)325, (short)326,
(short)327, (short)328, (short)329, (short)330, (short)331, (short)332, (short)333,
(short)334, (short)335, (short)336, (short)337, (short)338, (short)339, (short)340,
(short)341, (short)94, (short)93, (short)98, (short)98, (short)95, (short)107,
(short)99, (short)94, (short)98, (short)98, (short)107, (short)94, (short)98,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, (short)22, (short)23, (short)24,
(short)25, (short)378, (short)27, (short)28, (short)29, (short)30, (short)31,
(short)32, (short)33, (short)34, (short)35, (short)36, (short)37, (short)38,
(short)39, (short)40, (short)102, (short)42, (short)43, (short)44, (short)93,
(short)98, (short)47, (short)48, (short)49, (short)50, (short)51, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59,
(short)98, (short)61, (short)62, (short)98, (short)98, (short)26, (short)95,
(short)104, (short)98, (short)95, (short)93, (short)0, (short)0, (short)310,
(short)553, (short)203, (short)428, (short)426, (short)292, (short)212, (short)153,
(short)521, (short)434, (short)436, (short)84, (short)85, (short)86, (short)87,
(short)88, (short)89, (short)90, (short)91, (short)273, (short)93, (short)94,
(short)563, (short)294, (short)97, (short)153, (short)325, (short)100, (short)101,
(short)323, (short)326, - (short)1, (short)327, - (short)1, - (short)1, (short)108,
- (short)1, - (short)1, (short)111, (short)112, (short)113, - (short)1, (short)328,
- (short)1, - (short)1, (short)118, (short)119, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)480,
- (short)1, - (short)1, - (short)1, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, (short)25, - (short)1, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)36, (short)37, (short)38, (short)39, (short)40, (short)522,
(short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, (short)61, (short)62, -
(short)1, - (short)1, - (short)1, - (short)1, (short)71, (short)72, (short)73,
(short)74, (short)75, (short)76, (short)77, (short)78, (short)79, (short)80, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)84, (short)85, (short)86, (short)87, (short)88, (short)89, (short)90,
(short)91, (short)96, (short)93, (short)94, (short)95, - (short)1, (short)97, -
(short)1, - (short)1, (short)100, (short)101, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)108, - (short)1, - (short)1, (short)111,
(short)112, (short)113, - (short)1, (short)596, - (short)1, - (short)1, (short)118,
(short)119, (short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1,
(short)24, (short)25, - (short)1, (short)27, (short)28, (short)29, (short)30,
(short)31, (short)32, (short)33, (short)34, (short)35, (short)36, (short)37,
(short)38, (short)39, (short)40, - (short)1, (short)42, (short)43, (short)44, -
(short)1, - (short)1, (short)47, (short)48, (short)49, (short)50, (short)51,
(short)52, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85,
(short)86, (short)87, (short)88, (short)89, (short)90, (short)91, - (short)1,
(short)93, (short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
(short)100, (short)101, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112,
(short)113, - (short)1, - (short)1, - (short)1, - (short)1, (short)118, (short)119,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)35, (short)36, (short)37, (short)38,
(short)39, - (short)1, - (short)1, (short)42, (short)43, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, (short)49, (short)50, - (short)1, - (short)1,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59, -
(short)1, (short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86,
(short)87, (short)88, (short)89, (short)90, (short)91, - (short)1, (short)93, -
(short)1, - (short)1, - (short)1, (short)97, - (short)1, - (short)1, (short)100,
(short)101, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112, (short)113,
- (short)1, - (short)1, - (short)1, - (short)1, (short)118, (short)119, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)35, (short)36,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1,
- (short)1, - (short)1, - (short)1, (short)47, (short)48, (short)49, (short)50,
- (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, (short)59, - (short)1, (short)61, (short)62, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84,
(short)85, (short)86, (short)87, (short)88, (short)89, (short)90, (short)91, -
(short)1, - (short)1, (short)94, - (short)1, - (short)1, (short)97, - (short)1,
- (short)1, (short)100, (short)101, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)108, - (short)1, - (short)1, (short)111,
(short)112, (short)113, - (short)1, - (short)1, - (short)1, - (short)1, (short)118,
(short)119, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)35, (short)36, (short)37, - (short)1, (short)39, - (short)1, -
(short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1, (short)47,
(short)48, (short)49, (short)50, - (short)1, - (short)1, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, (short)61,
(short)62, - (short)1, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)84, (short)85, (short)86, (short)87, (short)88, (short)89,
(short)90, (short)91, (short)36, - (short)1, - (short)1, - (short)1, - (short)1,
(short)97, - (short)1, - (short)1, (short)100, (short)101, - (short)1, (short)47,
(short)48, - (short)1, - (short)1, - (short)1, (short)108, - (short)1, - (short)1,
(short)111, (short)112, (short)113, - (short)1, - (short)1, - (short)1, -
(short)1, (short)118, (short)119, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, (short)84, (short)24, - (short)1, - (short)1, - (short)1,
(short)89, - (short)1, (short)91, - (short)1, (short)93, - (short)1, - (short)1,
- (short)1, (short)36, (short)98, (short)38, (short)100, (short)101, - (short)1,
- (short)1, (short)43, - (short)1, (short)45, (short)46, (short)47, (short)48, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)84,
- (short)1, - (short)1, (short)36, - (short)1, (short)89, (short)90, (short)91,
- (short)1, - (short)1, (short)43, - (short)1, - (short)1, (short)97, (short)47,
(short)48, (short)100, (short)101, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)108, (short)109, (short)110, - (short)1, -
(short)1, - (short)1, - (short)1, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)84, - (short)1, - (short)1, - (short)1, - (short)1,
(short)89, (short)90, (short)91, (short)36, - (short)1, - (short)1, - (short)1,
- (short)1, (short)97, - (short)1, - (short)1, (short)100, (short)101, - (short)1,
(short)47, (short)48, - (short)1, - (short)1, - (short)1, (short)108, (short)109,
(short)110, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)36, (short)84, - (short)1, - (short)1, - (short)1,
- (short)1, (short)89, (short)90, (short)91, - (short)1, (short)93, (short)47,
(short)48, - (short)1, (short)97, (short)98, - (short)1, (short)100, (short)101,
- (short)1, - (short)1, - (short)1, - (short)1, (short)106, - (short)1, (short)108,
(short)109, (short)110, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)89, - (short)1, (short)91, - (short)1, (short)93, - (short)1,
- (short)1, - (short)1, (short)97, (short)98, - (short)1, (short)100, (short)101,
- (short)1, - (short)1, - (short)1, - (short)1, (short)106, - (short)1, (short)108,
(short)109, (short)110, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)36, - (short)1, (short)38, - (short)1, - (short)1, - (short)1,
- (short)1, (short)43, - (short)1, - (short)1, - (short)1, (short)47, (short)48,
- (short)1, - (short)1, - (short)1, - (short)1, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)36, - (short)1, (short)38, (short)89, - (short)1,
(short)91, - (short)1, (short)43, (short)94, - (short)1, (short)96, (short)47,
(short)48, - (short)1, (short)100, (short)101, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)36, - (short)1, - (short)1, - (short)1, (short)89,
- (short)1, (short)91, (short)43, - (short)1, (short)94, - (short)1, (short)47,
(short)48, - (short)1, - (short)1, (short)100, (short)101, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)25, - (short)1, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)89, (short)37, (short)91, (short)39, (short)40, - (short)1,
(short)42, - (short)1, (short)44, - (short)1, - (short)1, (short)100, (short)101,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, (short)61, (short)62,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)84, (short)85, (short)86, (short)87, (short)88, - (short)1, (short)90,
(short)36, - (short)1, (short)93, (short)94, - (short)1, - (short)1, (short)97,
- (short)1, - (short)1, - (short)1, (short)101, (short)47, (short)48, - (short)1,
- (short)1, - (short)1, - (short)1, (short)108, - (short)1, - (short)1, (short)111,
(short)112, (short)113, - (short)1, - (short)1, - (short)1, - (short)1, (short)118,
(short)119, - (short)1, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)89, - (short)1, (short)91,
- (short)1, (short)93, (short)36, - (short)1, - (short)1, - (short)1, (short)98,
- (short)1, (short)100, (short)101, - (short)1, - (short)1, - (short)1, (short)47,
(short)48, - (short)1, - (short)1, - (short)1, - (short)1, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, (short)36, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)89,
- (short)1, (short)91, (short)47, (short)48, (short)94, (short)36, - (short)1, -
(short)1, - (short)1, - (short)1, (short)100, (short)101, - (short)1, - (short)1,
- (short)1, - (short)1, (short)47, (short)48, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)81, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)89, - (short)1, (short)91, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)30, - (short)1, - (short)1,
(short)100, (short)101, (short)35, (short)89, (short)37, (short)91, (short)39, -
(short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1,
(short)100, (short)101, (short)49, (short)50, - (short)1, - (short)1, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1,
(short)61, (short)62, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)84, (short)85, (short)86, (short)87,
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
- (short)1, - (short)1, (short)101, - (short)1, (short)37, - (short)1, - (short)1,
- (short)1, - (short)1, (short)108, - (short)1, - (short)1, (short)111, (short)112,
(short)113, - (short)1, - (short)1, - (short)1, - (short)1, (short)118, (short)119,
(short)84, (short)85, - (short)1, (short)87, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, - (short)1, (short)95, - (short)1, (short)97,
- (short)1, - (short)1, (short)100, (short)101, - (short)1, - (short)1, -
(short)1, (short)105, (short)106, - (short)1, (short)108, - (short)1, - (short)1,
- (short)1, (short)112, (short)113, (short)84, (short)85, - (short)1, (short)87,
(short)88, - (short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)97, - (short)1, - (short)1, (short)100, (short)101,
- (short)1, - (short)1, - (short)1, (short)105, (short)106, - (short)1, (short)108,
- (short)1, - (short)1, - (short)1, (short)112, (short)113}; char Cyc_Yyimpossible_tag[
13u]="Yyimpossible"; struct Cyc_Yyimpossible_struct{ char* tag; } ; char Cyc_Yystack_overflow_tag[
17u]="Yystack_overflow"; struct Cyc_Yystack_overflow_struct{ char* tag; } ;
extern void Cyc_yyerror( struct _tagged_string); extern int Cyc_yylex(); char
Cyc_YYINITIALSVAL_tag[ 14u]="YYINITIALSVAL"; struct Cyc_YYINITIALSVAL_struct{
char* tag; } ; static int Cyc_yychar=( int)'\000'; struct Cyc_YYINITIALSVAL_struct
Cyc_yyinitval={ Cyc_YYINITIALSVAL_tag}; struct _xtunion_struct* Cyc_yylval=(
struct _xtunion_struct*)& Cyc_yyinitval; static int Cyc_yynerrs= 0; int Cyc_yyparse(){
int yystate; int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset;
struct _tagged_ptr1 yyss=( struct _tagged_ptr1)({ short* _temp2135=( short*)({
unsigned int _temp2133= 200u; short* _temp2134=( short*) GC_malloc_atomic(
sizeof( short) * _temp2133); unsigned int i; for( i= 0; i < _temp2133; i ++){
_temp2134[ i]= (short)0;} _temp2134;}); struct _tagged_ptr1 _temp2136; _temp2136.curr=
_temp2135; _temp2136.base= _temp2135; _temp2136.last_plus_one= _temp2135 + 200;
_temp2136;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=( struct _tagged_ptr2)({
struct _xtunion_struct** _temp2139=( struct _xtunion_struct**)({ unsigned int
_temp2137= 200u; struct _xtunion_struct** _temp2138=( struct _xtunion_struct**)
GC_malloc( sizeof( struct _xtunion_struct*) * _temp2137); unsigned int i; for( i=
0; i < _temp2137; i ++){ _temp2138[ i]= Cyc_yylval;} _temp2138;}); struct
_tagged_ptr2 _temp2140; _temp2140.curr= _temp2139; _temp2140.base= _temp2139;
_temp2140.last_plus_one= _temp2139 + 200; _temp2140;}); int yylsp_offset; struct
_tagged_ptr3 yyls=( struct _tagged_ptr3)({ struct Cyc_Yyltype* _temp2143=(
struct Cyc_Yyltype*)({ unsigned int _temp2141= 200u; struct Cyc_Yyltype*
_temp2142=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp2141); unsigned int i; for( i= 0; i < _temp2141; i ++){ _temp2142[ i]= Cyc_yynewloc();}
_temp2142;}); struct _tagged_ptr3 _temp2144; _temp2144.curr= _temp2143;
_temp2144.base= _temp2143; _temp2144.last_plus_one= _temp2143 + 200; _temp2144;});
int yystacksize= 200; struct _xtunion_struct* yyval= Cyc_yylval; int yylen;
yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1;
yyvsp_offset= 0; yylsp_offset= 0; yynewstate:({ struct _tagged_ptr1 _temp2145=
yyss; short* _temp2147= _temp2145.curr +( ++ yyssp_offset); if( _temp2145.base
== 0? 1:( _temp2147 < _temp2145.base? 1: _temp2147 >= _temp2145.last_plus_one)){
_throw( Null_Exception);}* _temp2147=( short) yystate;}); if( yyssp_offset >=
yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror(( struct _tagged_string)({
char* _temp2148=( char*)"parser stack overflow"; struct _tagged_string _temp2149;
_temp2149.curr= _temp2148; _temp2149.base= _temp2148; _temp2149.last_plus_one=
_temp2148 + 22; _temp2149;}));( void) _throw(( struct _xtunion_struct*)({ struct
Cyc_Yystack_overflow_struct* _temp2150=( struct Cyc_Yystack_overflow_struct*)
GC_malloc( sizeof( struct Cyc_Yystack_overflow_struct)); _temp2150[ 0]=({ struct
Cyc_Yystack_overflow_struct _temp2151; _temp2151.tag= Cyc_Yystack_overflow_tag;
_temp2151;}); _temp2150;}));} yystacksize *= 2; if( yystacksize > 10000){
yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp2152=(
unsigned int) yystacksize; short* _temp2153=( short*) GC_malloc_atomic( sizeof(
short) * _temp2152); unsigned int i; struct _tagged_ptr1 _temp2154={ _temp2153,
_temp2153, _temp2153 + _temp2152}; for( i= 0; i < _temp2152; i ++){ _temp2153[ i]=
(short)0;} _temp2154;}); struct _tagged_ptr2 yyvs1=({ unsigned int _temp2155=(
unsigned int) yystacksize; struct _xtunion_struct** _temp2156=( struct
_xtunion_struct**) GC_malloc( sizeof( struct _xtunion_struct*) * _temp2155);
unsigned int i; struct _tagged_ptr2 _temp2157={ _temp2156, _temp2156, _temp2156
+ _temp2155}; for( i= 0; i < _temp2155; i ++){ _temp2156[ i]= Cyc_yylval;}
_temp2157;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2158=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2159=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2158); unsigned int i; struct _tagged_ptr3
_temp2160={ _temp2159, _temp2159, _temp2159 + _temp2158}; for( i= 0; i <
_temp2158; i ++){ _temp2159[ i]= Cyc_yynewloc();} _temp2160;});{ int i= 0; for(
0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1 _temp2161= yyss1; short*
_temp2163= _temp2161.curr + i; if( _temp2161.base == 0? 1:( _temp2163 <
_temp2161.base? 1: _temp2163 >= _temp2161.last_plus_one)){ _throw(
Null_Exception);}* _temp2163=({ struct _tagged_ptr1 _temp2164= yyss; short*
_temp2166= _temp2164.curr + i; if( _temp2164.base == 0? 1:( _temp2166 <
_temp2164.base? 1: _temp2166 >= _temp2164.last_plus_one)){ _throw(
Null_Exception);}* _temp2166;});});({ struct _tagged_ptr2 _temp2167= yyvs1;
struct _xtunion_struct** _temp2169= _temp2167.curr + i; if( _temp2167.base == 0?
1:( _temp2169 < _temp2167.base? 1: _temp2169 >= _temp2167.last_plus_one)){
_throw( Null_Exception);}* _temp2169=({ struct _tagged_ptr2 _temp2170= yyvs;
struct _xtunion_struct** _temp2172= _temp2170.curr + i; if( _temp2170.base == 0?
1:( _temp2172 < _temp2170.base? 1: _temp2172 >= _temp2170.last_plus_one)){
_throw( Null_Exception);}* _temp2172;});});({ struct _tagged_ptr3 _temp2173=
yyls1; struct Cyc_Yyltype* _temp2175= _temp2173.curr + i; if( _temp2173.base ==
0? 1:( _temp2175 < _temp2173.base? 1: _temp2175 >= _temp2173.last_plus_one)){
_throw( Null_Exception);}* _temp2175=({ struct _tagged_ptr3 _temp2176= yyls;
struct Cyc_Yyltype* _temp2178= _temp2176.curr + i; if( _temp2176.base == 0? 1:(
_temp2178 < _temp2176.base? 1: _temp2178 >= _temp2176.last_plus_one)){ _throw(
Null_Exception);}* _temp2178;});});}} yyss= yyss1; yyvs= yyvs1; yyls= yyls1;}}
goto yybackup; yybackup: yyn=( int)({ short* _temp2179=( short*) Cyc_yypact;
unsigned int _temp2180= yystate; if( _temp2180 >= 728u){ _throw( Null_Exception);}
_temp2179[ _temp2180];}); if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar
== - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar=
0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 347: 0)?( int)({ short*
_temp2181=( short*) Cyc_yytranslate; unsigned int _temp2182= Cyc_yychar; if(
_temp2182 >= 348u){ _throw( Null_Exception);} _temp2181[ _temp2182];}): 226;}
yyn += yychar1; if(( yyn < 0? 1: yyn > 3976)? 1:({ short* _temp2183=( short*)
Cyc_yycheck; unsigned int _temp2184= yyn; if( _temp2184 >= 3977u){ _throw(
Null_Exception);} _temp2183[ _temp2184];}) != yychar1){ goto yydefault;} yyn=(
int)({ short* _temp2185=( short*) Cyc_yytable; unsigned int _temp2186= yyn; if(
_temp2186 >= 3977u){ _throw( Null_Exception);} _temp2185[ _temp2186];}); if( yyn
< 0){ if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if(
yyn == 0){ goto yyerrlab;}} if( yyn == 727){ return 0;} if( Cyc_yychar != 0){
Cyc_yychar= - 2;}({ struct _tagged_ptr2 _temp2187= yyvs; struct _xtunion_struct**
_temp2189= _temp2187.curr +( ++ yyvsp_offset); if( _temp2187.base == 0? 1:(
_temp2189 < _temp2187.base? 1: _temp2189 >= _temp2187.last_plus_one)){ _throw(
Null_Exception);}* _temp2189= Cyc_yylval;});({ struct _tagged_ptr3 _temp2190=
yyls; struct Cyc_Yyltype* _temp2192= _temp2190.curr +( ++ yylsp_offset); if(
_temp2190.base == 0? 1:( _temp2192 < _temp2190.base? 1: _temp2192 >= _temp2190.last_plus_one)){
_throw( Null_Exception);}* _temp2192= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp2193=( short*) Cyc_yydefact; unsigned int _temp2194= yystate; if( _temp2194
>= 728u){ _throw( Null_Exception);} _temp2193[ _temp2194];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp2195=( short*) Cyc_yyr2;
unsigned int _temp2196= yyn; if( _temp2196 >= 387u){ _throw( Null_Exception);}
_temp2195[ _temp2196];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp2197=
yyvs; struct _xtunion_struct** _temp2199= _temp2197.curr +(( yyvsp_offset + 1) -
yylen); if( _temp2197.base == 0? 1:( _temp2199 < _temp2197.base? 1: _temp2199 >=
_temp2197.last_plus_one)){ _throw( Null_Exception);}* _temp2199;});} switch( yyn){
case 1: _LL2200: yyval=({ struct _tagged_ptr2 _temp2202= yyvs; struct
_xtunion_struct** _temp2204= _temp2202.curr + yyvsp_offset; if( _temp2202.base
== 0? 1:( _temp2204 < _temp2202.base? 1: _temp2204 >= _temp2202.last_plus_one)){
_throw( Null_Exception);}* _temp2204;}); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2205= yyvs; struct _xtunion_struct** _temp2207=
_temp2205.curr + yyvsp_offset; if( _temp2205.base == 0? 1:( _temp2207 <
_temp2205.base? 1: _temp2207 >= _temp2205.last_plus_one)){ _throw(
Null_Exception);}* _temp2207;})); break; case 2: _LL2201: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2209=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2209[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2210; _temp2210.tag= Cyc_DeclList_tok_tag; _temp2210.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2211= yyvs; struct _xtunion_struct** _temp2213=
_temp2211.curr +( yyvsp_offset - 1); if( _temp2211.base == 0? 1:( _temp2213 <
_temp2211.base? 1: _temp2213 >= _temp2211.last_plus_one)){ _throw(
Null_Exception);}* _temp2213;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2214= yyvs; struct _xtunion_struct** _temp2216= _temp2214.curr +
yyvsp_offset; if( _temp2214.base == 0? 1:( _temp2216 < _temp2214.base? 1:
_temp2216 >= _temp2214.last_plus_one)){ _throw( Null_Exception);}* _temp2216;})));
_temp2210;}); _temp2209;}); break; case 3: _LL2208: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2218=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2218[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2219; _temp2219.tag= Cyc_DeclList_tok_tag; _temp2219.f1=({ struct Cyc_List_List*
_temp2220=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2220->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2221=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2221->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp2228=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2228[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp2229; _temp2229.tag= Cyc_Absyn_Using_d_tag;
_temp2229.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2230= yyvs;
struct _xtunion_struct** _temp2232= _temp2230.curr +( yyvsp_offset - 2); if(
_temp2230.base == 0? 1:( _temp2232 < _temp2230.base? 1: _temp2232 >= _temp2230.last_plus_one)){
_throw( Null_Exception);}* _temp2232;})); _temp2229.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2233= yyvs; struct _xtunion_struct** _temp2235=
_temp2233.curr + yyvsp_offset; if( _temp2233.base == 0? 1:( _temp2235 <
_temp2233.base? 1: _temp2235 >= _temp2233.last_plus_one)){ _throw(
Null_Exception);}* _temp2235;})); _temp2229;}); _temp2228;})); _temp2221->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2222= yyls; struct Cyc_Yyltype*
_temp2224= _temp2222.curr +( yylsp_offset - 2); if( _temp2222.base == 0? 1:(
_temp2224 < _temp2222.base? 1: _temp2224 >= _temp2222.last_plus_one)){ _throw(
Null_Exception);}* _temp2224;}).first_line,({ struct _tagged_ptr3 _temp2225=
yyls; struct Cyc_Yyltype* _temp2227= _temp2225.curr + yylsp_offset; if(
_temp2225.base == 0? 1:( _temp2227 < _temp2225.base? 1: _temp2227 >= _temp2225.last_plus_one)){
_throw( Null_Exception);}* _temp2227;}).last_line); _temp2221;}); _temp2220->tl=
0; _temp2220;}); _temp2219;}); _temp2218;}); Cyc_Lex_leave_using(); break; case
4: _LL2217: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2237=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2237[ 0]=({ struct Cyc_DeclList_tok_struct _temp2238; _temp2238.tag= Cyc_DeclList_tok_tag;
_temp2238.f1=({ struct Cyc_List_List* _temp2239=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2239->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2243=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2243->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2250=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2250[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2251; _temp2251.tag= Cyc_Absyn_Using_d_tag;
_temp2251.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2252= yyvs;
struct _xtunion_struct** _temp2254= _temp2252.curr +( yyvsp_offset - 4); if(
_temp2252.base == 0? 1:( _temp2254 < _temp2252.base? 1: _temp2254 >= _temp2252.last_plus_one)){
_throw( Null_Exception);}* _temp2254;})); _temp2251.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2255= yyvs; struct _xtunion_struct** _temp2257=
_temp2255.curr +( yyvsp_offset - 2); if( _temp2255.base == 0? 1:( _temp2257 <
_temp2255.base? 1: _temp2257 >= _temp2255.last_plus_one)){ _throw(
Null_Exception);}* _temp2257;})); _temp2251;}); _temp2250;})); _temp2243->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2244= yyls; struct Cyc_Yyltype*
_temp2246= _temp2244.curr +( yylsp_offset - 4); if( _temp2244.base == 0? 1:(
_temp2246 < _temp2244.base? 1: _temp2246 >= _temp2244.last_plus_one)){ _throw(
Null_Exception);}* _temp2246;}).first_line,({ struct _tagged_ptr3 _temp2247=
yyls; struct Cyc_Yyltype* _temp2249= _temp2247.curr +( yylsp_offset - 1); if(
_temp2247.base == 0? 1:( _temp2249 < _temp2247.base? 1: _temp2249 >= _temp2247.last_plus_one)){
_throw( Null_Exception);}* _temp2249;}).last_line); _temp2243;}); _temp2239->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2240= yyvs; struct
_xtunion_struct** _temp2242= _temp2240.curr + yyvsp_offset; if( _temp2240.base
== 0? 1:( _temp2242 < _temp2240.base? 1: _temp2242 >= _temp2240.last_plus_one)){
_throw( Null_Exception);}* _temp2242;})); _temp2239;}); _temp2238;}); _temp2237;});
break; case 5: _LL2236: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2259=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2259[ 0]=({ struct Cyc_DeclList_tok_struct _temp2260; _temp2260.tag= Cyc_DeclList_tok_tag;
_temp2260.f1=({ struct Cyc_List_List* _temp2261=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2261->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2262=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2262->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2269=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2269[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2270; _temp2270.tag=
Cyc_Absyn_Namespace_d_tag; _temp2270.f1=({ struct _tagged_string* _temp2271=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2271[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2272= yyvs;
struct _xtunion_struct** _temp2274= _temp2272.curr +( yyvsp_offset - 2); if(
_temp2272.base == 0? 1:( _temp2274 < _temp2272.base? 1: _temp2274 >= _temp2272.last_plus_one)){
_throw( Null_Exception);}* _temp2274;})); _temp2271;}); _temp2270.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2275= yyvs; struct _xtunion_struct** _temp2277=
_temp2275.curr + yyvsp_offset; if( _temp2275.base == 0? 1:( _temp2277 <
_temp2275.base? 1: _temp2277 >= _temp2275.last_plus_one)){ _throw(
Null_Exception);}* _temp2277;})); _temp2270;}); _temp2269;})); _temp2262->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2263= yyls; struct Cyc_Yyltype*
_temp2265= _temp2263.curr +( yylsp_offset - 2); if( _temp2263.base == 0? 1:(
_temp2265 < _temp2263.base? 1: _temp2265 >= _temp2263.last_plus_one)){ _throw(
Null_Exception);}* _temp2265;}).first_line,({ struct _tagged_ptr3 _temp2266=
yyls; struct Cyc_Yyltype* _temp2268= _temp2266.curr + yylsp_offset; if(
_temp2266.base == 0? 1:( _temp2268 < _temp2266.base? 1: _temp2268 >= _temp2266.last_plus_one)){
_throw( Null_Exception);}* _temp2268;}).last_line); _temp2262;}); _temp2261->tl=
0; _temp2261;}); _temp2260;}); _temp2259;}); Cyc_Lex_leave_namespace(); break;
case 6: _LL2258: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2279=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2279[ 0]=({ struct Cyc_DeclList_tok_struct _temp2280; _temp2280.tag= Cyc_DeclList_tok_tag;
_temp2280.f1=({ struct Cyc_List_List* _temp2281=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2281->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2285=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2285->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2292=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2292[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2293; _temp2293.tag=
Cyc_Absyn_Namespace_d_tag; _temp2293.f1=({ struct _tagged_string* _temp2294=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp2294[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2295= yyvs;
struct _xtunion_struct** _temp2297= _temp2295.curr +( yyvsp_offset - 4); if(
_temp2295.base == 0? 1:( _temp2297 < _temp2295.base? 1: _temp2297 >= _temp2295.last_plus_one)){
_throw( Null_Exception);}* _temp2297;})); _temp2294;}); _temp2293.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2298= yyvs; struct _xtunion_struct** _temp2300=
_temp2298.curr +( yyvsp_offset - 2); if( _temp2298.base == 0? 1:( _temp2300 <
_temp2298.base? 1: _temp2300 >= _temp2298.last_plus_one)){ _throw(
Null_Exception);}* _temp2300;})); _temp2293;}); _temp2292;})); _temp2285->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2286= yyls; struct Cyc_Yyltype*
_temp2288= _temp2286.curr +( yylsp_offset - 4); if( _temp2286.base == 0? 1:(
_temp2288 < _temp2286.base? 1: _temp2288 >= _temp2286.last_plus_one)){ _throw(
Null_Exception);}* _temp2288;}).first_line,({ struct _tagged_ptr3 _temp2289=
yyls; struct Cyc_Yyltype* _temp2291= _temp2289.curr +( yylsp_offset - 1); if(
_temp2289.base == 0? 1:( _temp2291 < _temp2289.base? 1: _temp2291 >= _temp2289.last_plus_one)){
_throw( Null_Exception);}* _temp2291;}).last_line); _temp2285;}); _temp2281->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2282= yyvs; struct
_xtunion_struct** _temp2284= _temp2282.curr + yyvsp_offset; if( _temp2282.base
== 0? 1:( _temp2284 < _temp2282.base? 1: _temp2284 >= _temp2282.last_plus_one)){
_throw( Null_Exception);}* _temp2284;})); _temp2281;}); _temp2280;}); _temp2279;});
break; case 7: _LL2278: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2302= yyvs; struct _xtunion_struct** _temp2304= _temp2302.curr
+( yyvsp_offset - 4); if( _temp2302.base == 0? 1:( _temp2304 < _temp2302.base? 1:
_temp2304 >= _temp2302.last_plus_one)){ _throw( Null_Exception);}* _temp2304;})),(
struct _tagged_string)({ char* _temp2305=( char*)"C"; struct _tagged_string
_temp2306; _temp2306.curr= _temp2305; _temp2306.base= _temp2305; _temp2306.last_plus_one=
_temp2305 + 2; _temp2306;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2307=( char*)"only extern \"C\" { ... } is allowed"; struct
_tagged_string _temp2308; _temp2308.curr= _temp2307; _temp2308.base= _temp2307;
_temp2308.last_plus_one= _temp2307 + 35; _temp2308;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2309= yyls; struct Cyc_Yyltype* _temp2311= _temp2309.curr
+( yylsp_offset - 5); if( _temp2309.base == 0? 1:( _temp2311 < _temp2309.base? 1:
_temp2311 >= _temp2309.last_plus_one)){ _throw( Null_Exception);}* _temp2311;}).first_line,({
struct _tagged_ptr3 _temp2312= yyls; struct Cyc_Yyltype* _temp2314= _temp2312.curr
+( yylsp_offset - 4); if( _temp2312.base == 0? 1:( _temp2314 < _temp2312.base? 1:
_temp2314 >= _temp2312.last_plus_one)){ _throw( Null_Exception);}* _temp2314;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2315=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2315[ 0]=({ struct Cyc_DeclList_tok_struct _temp2316; _temp2316.tag= Cyc_DeclList_tok_tag;
_temp2316.f1=({ struct Cyc_List_List* _temp2317=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2317->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2321=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2321->r=( void*)(( void*)({ struct Cyc_Absyn_ExternC_d_struct* _temp2328=(
struct Cyc_Absyn_ExternC_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp2328[ 0]=({ struct Cyc_Absyn_ExternC_d_struct _temp2329; _temp2329.tag= Cyc_Absyn_ExternC_d_tag;
_temp2329.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2330= yyvs;
struct _xtunion_struct** _temp2332= _temp2330.curr +( yyvsp_offset - 2); if(
_temp2330.base == 0? 1:( _temp2332 < _temp2330.base? 1: _temp2332 >= _temp2330.last_plus_one)){
_throw( Null_Exception);}* _temp2332;})); _temp2329;}); _temp2328;})); _temp2321->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2322= yyls; struct Cyc_Yyltype*
_temp2324= _temp2322.curr +( yylsp_offset - 5); if( _temp2322.base == 0? 1:(
_temp2324 < _temp2322.base? 1: _temp2324 >= _temp2322.last_plus_one)){ _throw(
Null_Exception);}* _temp2324;}).first_line,({ struct _tagged_ptr3 _temp2325=
yyls; struct Cyc_Yyltype* _temp2327= _temp2325.curr +( yylsp_offset - 1); if(
_temp2325.base == 0? 1:( _temp2327 < _temp2325.base? 1: _temp2327 >= _temp2325.last_plus_one)){
_throw( Null_Exception);}* _temp2327;}).last_line); _temp2321;}); _temp2317->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2318= yyvs; struct
_xtunion_struct** _temp2320= _temp2318.curr + yyvsp_offset; if( _temp2318.base
== 0? 1:( _temp2320 < _temp2318.base? 1: _temp2320 >= _temp2318.last_plus_one)){
_throw( Null_Exception);}* _temp2320;})); _temp2317;}); _temp2316;}); _temp2315;});
break; case 8: _LL2301: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2334=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2334[ 0]=({ struct Cyc_DeclList_tok_struct _temp2335; _temp2335.tag= Cyc_DeclList_tok_tag;
_temp2335.f1= 0; _temp2335;}); _temp2334;}); break; case 9: _LL2333: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2337=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2337[ 0]=({ struct Cyc_DeclList_tok_struct _temp2338; _temp2338.tag= Cyc_DeclList_tok_tag;
_temp2338.f1=({ struct Cyc_List_List* _temp2339=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2339->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Fn_d_struct* _temp2340=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp2340[ 0]=({ struct Cyc_Absyn_Fn_d_struct
_temp2341; _temp2341.tag= Cyc_Absyn_Fn_d_tag; _temp2341.f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp2342= yyvs; struct _xtunion_struct** _temp2344=
_temp2342.curr + yyvsp_offset; if( _temp2342.base == 0? 1:( _temp2344 <
_temp2342.base? 1: _temp2344 >= _temp2342.last_plus_one)){ _throw(
Null_Exception);}* _temp2344;})); _temp2341;}); _temp2340;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2345= yyls; struct Cyc_Yyltype* _temp2347= _temp2345.curr
+ yylsp_offset; if( _temp2345.base == 0? 1:( _temp2347 < _temp2345.base? 1:
_temp2347 >= _temp2345.last_plus_one)){ _throw( Null_Exception);}* _temp2347;}).first_line,({
struct _tagged_ptr3 _temp2348= yyls; struct Cyc_Yyltype* _temp2350= _temp2348.curr
+ yylsp_offset; if( _temp2348.base == 0? 1:( _temp2350 < _temp2348.base? 1:
_temp2350 >= _temp2348.last_plus_one)){ _throw( Null_Exception);}* _temp2350;}).last_line));
_temp2339->tl= 0; _temp2339;}); _temp2338;}); _temp2337;}); break; case 10:
_LL2336: yyval=({ struct _tagged_ptr2 _temp2352= yyvs; struct _xtunion_struct**
_temp2354= _temp2352.curr + yyvsp_offset; if( _temp2352.base == 0? 1:( _temp2354
< _temp2352.base? 1: _temp2354 >= _temp2352.last_plus_one)){ _throw(
Null_Exception);}* _temp2354;}); break; case 11: _LL2351: yyval=( struct
_xtunion_struct*)({ struct Cyc_FnDecl_tok_struct* _temp2356=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2356[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2357; _temp2357.tag= Cyc_FnDecl_tok_tag; _temp2357.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2358= yyvs; struct
_xtunion_struct** _temp2360= _temp2358.curr +( yyvsp_offset - 1); if( _temp2358.base
== 0? 1:( _temp2360 < _temp2358.base? 1: _temp2360 >= _temp2358.last_plus_one)){
_throw( Null_Exception);}* _temp2360;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2361= yyvs; struct _xtunion_struct** _temp2363= _temp2361.curr
+ yyvsp_offset; if( _temp2361.base == 0? 1:( _temp2363 < _temp2361.base? 1:
_temp2363 >= _temp2361.last_plus_one)){ _throw( Null_Exception);}* _temp2363;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2364= yyls; struct Cyc_Yyltype*
_temp2366= _temp2364.curr +( yylsp_offset - 1); if( _temp2364.base == 0? 1:(
_temp2366 < _temp2364.base? 1: _temp2366 >= _temp2364.last_plus_one)){ _throw(
Null_Exception);}* _temp2366;}).first_line,({ struct _tagged_ptr3 _temp2367=
yyls; struct Cyc_Yyltype* _temp2369= _temp2367.curr + yylsp_offset; if(
_temp2367.base == 0? 1:( _temp2369 < _temp2367.base? 1: _temp2369 >= _temp2367.last_plus_one)){
_throw( Null_Exception);}* _temp2369;}).last_line)); _temp2357;}); _temp2356;});
break; case 12: _LL2355: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2371=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2371[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2372; _temp2372.tag= Cyc_FnDecl_tok_tag;
_temp2372.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2373=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2373->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2374= yyvs; struct
_xtunion_struct** _temp2376= _temp2374.curr +( yyvsp_offset - 2); if( _temp2374.base
== 0? 1:( _temp2376 < _temp2374.base? 1: _temp2376 >= _temp2374.last_plus_one)){
_throw( Null_Exception);}* _temp2376;})); _temp2373;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2377= yyvs; struct _xtunion_struct** _temp2379=
_temp2377.curr +( yyvsp_offset - 1); if( _temp2377.base == 0? 1:( _temp2379 <
_temp2377.base? 1: _temp2379 >= _temp2377.last_plus_one)){ _throw(
Null_Exception);}* _temp2379;})), 0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2380= yyvs; struct _xtunion_struct** _temp2382= _temp2380.curr +
yyvsp_offset; if( _temp2380.base == 0? 1:( _temp2382 < _temp2380.base? 1:
_temp2382 >= _temp2380.last_plus_one)){ _throw( Null_Exception);}* _temp2382;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2383= yyls; struct Cyc_Yyltype*
_temp2385= _temp2383.curr +( yylsp_offset - 2); if( _temp2383.base == 0? 1:(
_temp2385 < _temp2383.base? 1: _temp2385 >= _temp2383.last_plus_one)){ _throw(
Null_Exception);}* _temp2385;}).first_line,({ struct _tagged_ptr3 _temp2386=
yyls; struct Cyc_Yyltype* _temp2388= _temp2386.curr + yylsp_offset; if(
_temp2386.base == 0? 1:( _temp2388 < _temp2386.base? 1: _temp2388 >= _temp2386.last_plus_one)){
_throw( Null_Exception);}* _temp2388;}).last_line)); _temp2372;}); _temp2371;});
break; case 13: _LL2370: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2390=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2390[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2391; _temp2391.tag= Cyc_FnDecl_tok_tag;
_temp2391.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2392= yyvs; struct _xtunion_struct** _temp2394= _temp2392.curr
+( yyvsp_offset - 2); if( _temp2392.base == 0? 1:( _temp2394 < _temp2392.base? 1:
_temp2394 >= _temp2392.last_plus_one)){ _throw( Null_Exception);}* _temp2394;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2395= yyvs; struct
_xtunion_struct** _temp2397= _temp2395.curr +( yyvsp_offset - 1); if( _temp2395.base
== 0? 1:( _temp2397 < _temp2395.base? 1: _temp2397 >= _temp2395.last_plus_one)){
_throw( Null_Exception);}* _temp2397;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2398= yyvs; struct _xtunion_struct** _temp2400= _temp2398.curr
+ yyvsp_offset; if( _temp2398.base == 0? 1:( _temp2400 < _temp2398.base? 1:
_temp2400 >= _temp2398.last_plus_one)){ _throw( Null_Exception);}* _temp2400;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2401= yyls; struct Cyc_Yyltype*
_temp2403= _temp2401.curr +( yylsp_offset - 2); if( _temp2401.base == 0? 1:(
_temp2403 < _temp2401.base? 1: _temp2403 >= _temp2401.last_plus_one)){ _throw(
Null_Exception);}* _temp2403;}).first_line,({ struct _tagged_ptr3 _temp2404=
yyls; struct Cyc_Yyltype* _temp2406= _temp2404.curr + yylsp_offset; if(
_temp2404.base == 0? 1:( _temp2406 < _temp2404.base? 1: _temp2406 >= _temp2404.last_plus_one)){
_throw( Null_Exception);}* _temp2406;}).last_line)); _temp2391;}); _temp2390;});
break; case 14: _LL2389: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2408=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2408[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2409; _temp2409.tag= Cyc_FnDecl_tok_tag;
_temp2409.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2410=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2410->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2411= yyvs; struct
_xtunion_struct** _temp2413= _temp2411.curr +( yyvsp_offset - 3); if( _temp2411.base
== 0? 1:( _temp2413 < _temp2411.base? 1: _temp2413 >= _temp2411.last_plus_one)){
_throw( Null_Exception);}* _temp2413;})); _temp2410;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2414= yyvs; struct _xtunion_struct** _temp2416=
_temp2414.curr +( yyvsp_offset - 2); if( _temp2414.base == 0? 1:( _temp2416 <
_temp2414.base? 1: _temp2416 >= _temp2414.last_plus_one)){ _throw(
Null_Exception);}* _temp2416;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2417= yyvs; struct _xtunion_struct** _temp2419= _temp2417.curr +(
yyvsp_offset - 1); if( _temp2417.base == 0? 1:( _temp2419 < _temp2417.base? 1:
_temp2419 >= _temp2417.last_plus_one)){ _throw( Null_Exception);}* _temp2419;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2420= yyvs; struct
_xtunion_struct** _temp2422= _temp2420.curr + yyvsp_offset; if( _temp2420.base
== 0? 1:( _temp2422 < _temp2420.base? 1: _temp2422 >= _temp2420.last_plus_one)){
_throw( Null_Exception);}* _temp2422;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2423= yyls; struct Cyc_Yyltype* _temp2425= _temp2423.curr +(
yylsp_offset - 3); if( _temp2423.base == 0? 1:( _temp2425 < _temp2423.base? 1:
_temp2425 >= _temp2423.last_plus_one)){ _throw( Null_Exception);}* _temp2425;}).first_line,({
struct _tagged_ptr3 _temp2426= yyls; struct Cyc_Yyltype* _temp2428= _temp2426.curr
+ yylsp_offset; if( _temp2426.base == 0? 1:( _temp2428 < _temp2426.base? 1:
_temp2428 >= _temp2426.last_plus_one)){ _throw( Null_Exception);}* _temp2428;}).last_line));
_temp2409;}); _temp2408;}); break; case 15: _LL2407: yyval=( struct
_xtunion_struct*)({ struct Cyc_FnDecl_tok_struct* _temp2430=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2430[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2431; _temp2431.tag= Cyc_FnDecl_tok_tag; _temp2431.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2432=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2432->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2433= yyvs; struct _xtunion_struct** _temp2435= _temp2433.curr
+( yyvsp_offset - 2); if( _temp2433.base == 0? 1:( _temp2435 < _temp2433.base? 1:
_temp2435 >= _temp2433.last_plus_one)){ _throw( Null_Exception);}* _temp2435;}));
_temp2432;}), Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2436= yyvs;
struct _xtunion_struct** _temp2438= _temp2436.curr +( yyvsp_offset - 1); if(
_temp2436.base == 0? 1:( _temp2438 < _temp2436.base? 1: _temp2438 >= _temp2436.last_plus_one)){
_throw( Null_Exception);}* _temp2438;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2439= yyvs; struct _xtunion_struct** _temp2441= _temp2439.curr
+ yyvsp_offset; if( _temp2439.base == 0? 1:( _temp2441 < _temp2439.base? 1:
_temp2441 >= _temp2439.last_plus_one)){ _throw( Null_Exception);}* _temp2441;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2442= yyls; struct Cyc_Yyltype*
_temp2444= _temp2442.curr +( yylsp_offset - 2); if( _temp2442.base == 0? 1:(
_temp2444 < _temp2442.base? 1: _temp2444 >= _temp2442.last_plus_one)){ _throw(
Null_Exception);}* _temp2444;}).first_line,({ struct _tagged_ptr3 _temp2445=
yyls; struct Cyc_Yyltype* _temp2447= _temp2445.curr + yylsp_offset; if(
_temp2445.base == 0? 1:( _temp2447 < _temp2445.base? 1: _temp2447 >= _temp2445.last_plus_one)){
_throw( Null_Exception);}* _temp2447;}).last_line)); _temp2431;}); _temp2430;});
break; case 16: _LL2429: yyval=( struct _xtunion_struct*)({ struct Cyc_FnDecl_tok_struct*
_temp2449=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2449[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2450; _temp2450.tag= Cyc_FnDecl_tok_tag;
_temp2450.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2451=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2451->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2452= yyvs; struct
_xtunion_struct** _temp2454= _temp2452.curr +( yyvsp_offset - 3); if( _temp2452.base
== 0? 1:( _temp2454 < _temp2452.base? 1: _temp2454 >= _temp2452.last_plus_one)){
_throw( Null_Exception);}* _temp2454;})); _temp2451;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2455= yyvs; struct _xtunion_struct** _temp2457=
_temp2455.curr +( yyvsp_offset - 2); if( _temp2455.base == 0? 1:( _temp2457 <
_temp2455.base? 1: _temp2457 >= _temp2455.last_plus_one)){ _throw(
Null_Exception);}* _temp2457;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2458= yyvs; struct _xtunion_struct** _temp2460= _temp2458.curr +(
yyvsp_offset - 1); if( _temp2458.base == 0? 1:( _temp2460 < _temp2458.base? 1:
_temp2460 >= _temp2458.last_plus_one)){ _throw( Null_Exception);}* _temp2460;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2461= yyvs; struct
_xtunion_struct** _temp2463= _temp2461.curr + yyvsp_offset; if( _temp2461.base
== 0? 1:( _temp2463 < _temp2461.base? 1: _temp2463 >= _temp2461.last_plus_one)){
_throw( Null_Exception);}* _temp2463;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2464= yyls; struct Cyc_Yyltype* _temp2466= _temp2464.curr +(
yylsp_offset - 3); if( _temp2464.base == 0? 1:( _temp2466 < _temp2464.base? 1:
_temp2466 >= _temp2464.last_plus_one)){ _throw( Null_Exception);}* _temp2466;}).first_line,({
struct _tagged_ptr3 _temp2467= yyls; struct Cyc_Yyltype* _temp2469= _temp2467.curr
+ yylsp_offset; if( _temp2467.base == 0? 1:( _temp2469 < _temp2467.base? 1:
_temp2469 >= _temp2467.last_plus_one)){ _throw( Null_Exception);}* _temp2469;}).last_line));
_temp2450;}); _temp2449;}); break; case 17: _LL2448: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2471= yyvs; struct _xtunion_struct** _temp2473=
_temp2471.curr + yyvsp_offset; if( _temp2471.base == 0? 1:( _temp2473 <
_temp2471.base? 1: _temp2473 >= _temp2471.last_plus_one)){ _throw(
Null_Exception);}* _temp2473;}))); yyval=({ struct _tagged_ptr2 _temp2474= yyvs;
struct _xtunion_struct** _temp2476= _temp2474.curr + yyvsp_offset; if( _temp2474.base
== 0? 1:( _temp2476 < _temp2474.base? 1: _temp2476 >= _temp2474.last_plus_one)){
_throw( Null_Exception);}* _temp2476;}); break; case 18: _LL2470: Cyc_Lex_leave_using();
break; case 19: _LL2477: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2479=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2479[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2480= yyvs;
struct _xtunion_struct** _temp2482= _temp2480.curr + yyvsp_offset; if( _temp2480.base
== 0? 1:( _temp2482 < _temp2480.base? 1: _temp2482 >= _temp2480.last_plus_one)){
_throw( Null_Exception);}* _temp2482;})); _temp2479;})); yyval=({ struct
_tagged_ptr2 _temp2483= yyvs; struct _xtunion_struct** _temp2485= _temp2483.curr
+ yyvsp_offset; if( _temp2483.base == 0? 1:( _temp2485 < _temp2483.base? 1:
_temp2485 >= _temp2483.last_plus_one)){ _throw( Null_Exception);}* _temp2485;});
break; case 20: _LL2478: Cyc_Lex_leave_namespace(); break; case 21: _LL2486:
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2488=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2488[ 0]=({ struct Cyc_DeclList_tok_struct _temp2489; _temp2489.tag= Cyc_DeclList_tok_tag;
_temp2489.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2490= yyvs; struct _xtunion_struct** _temp2492= _temp2490.curr
+( yyvsp_offset - 1); if( _temp2490.base == 0? 1:( _temp2492 < _temp2490.base? 1:
_temp2492 >= _temp2490.last_plus_one)){ _throw( Null_Exception);}* _temp2492;})),
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2493= yyls; struct
Cyc_Yyltype* _temp2495= _temp2493.curr +( yylsp_offset - 1); if( _temp2493.base
== 0? 1:( _temp2495 < _temp2493.base? 1: _temp2495 >= _temp2493.last_plus_one)){
_throw( Null_Exception);}* _temp2495;}).first_line,({ struct _tagged_ptr3
_temp2496= yyls; struct Cyc_Yyltype* _temp2498= _temp2496.curr +( yylsp_offset -
1); if( _temp2496.base == 0? 1:( _temp2498 < _temp2496.base? 1: _temp2498 >=
_temp2496.last_plus_one)){ _throw( Null_Exception);}* _temp2498;}).last_line));
_temp2489;}); _temp2488;}); break; case 22: _LL2487: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclList_tok_struct* _temp2500=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2500[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2501; _temp2501.tag= Cyc_DeclList_tok_tag; _temp2501.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2502= yyvs; struct
_xtunion_struct** _temp2504= _temp2502.curr +( yyvsp_offset - 2); if( _temp2502.base
== 0? 1:( _temp2504 < _temp2502.base? 1: _temp2504 >= _temp2502.last_plus_one)){
_throw( Null_Exception);}* _temp2504;})), Cyc_yyget_InitDeclList_tok(({ struct
_tagged_ptr2 _temp2505= yyvs; struct _xtunion_struct** _temp2507= _temp2505.curr
+( yyvsp_offset - 1); if( _temp2505.base == 0? 1:( _temp2507 < _temp2505.base? 1:
_temp2507 >= _temp2505.last_plus_one)){ _throw( Null_Exception);}* _temp2507;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2508= yyls; struct Cyc_Yyltype*
_temp2510= _temp2508.curr +( yylsp_offset - 2); if( _temp2508.base == 0? 1:(
_temp2510 < _temp2508.base? 1: _temp2510 >= _temp2508.last_plus_one)){ _throw(
Null_Exception);}* _temp2510;}).first_line,({ struct _tagged_ptr3 _temp2511=
yyls; struct Cyc_Yyltype* _temp2513= _temp2511.curr + yylsp_offset; if(
_temp2511.base == 0? 1:( _temp2513 < _temp2511.base? 1: _temp2513 >= _temp2511.last_plus_one)){
_throw( Null_Exception);}* _temp2513;}).last_line)); _temp2501;}); _temp2500;});
break; case 23: _LL2499: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2515=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2515[ 0]=({ struct Cyc_DeclList_tok_struct _temp2516; _temp2516.tag= Cyc_DeclList_tok_tag;
_temp2516.f1=({ struct Cyc_List_List* _temp2517=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2517->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp2518= yyvs; struct
_xtunion_struct** _temp2520= _temp2518.curr +( yyvsp_offset - 3); if( _temp2518.base
== 0? 1:( _temp2520 < _temp2518.base? 1: _temp2520 >= _temp2518.last_plus_one)){
_throw( Null_Exception);}* _temp2520;})), 0, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp2521= yyvs; struct _xtunion_struct** _temp2523= _temp2521.curr
+( yyvsp_offset - 1); if( _temp2521.base == 0? 1:( _temp2523 < _temp2521.base? 1:
_temp2523 >= _temp2521.last_plus_one)){ _throw( Null_Exception);}* _temp2523;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2524= yyls; struct Cyc_Yyltype*
_temp2526= _temp2524.curr +( yylsp_offset - 4); if( _temp2524.base == 0? 1:(
_temp2526 < _temp2524.base? 1: _temp2526 >= _temp2524.last_plus_one)){ _throw(
Null_Exception);}* _temp2526;}).first_line,({ struct _tagged_ptr3 _temp2527=
yyls; struct Cyc_Yyltype* _temp2529= _temp2527.curr + yylsp_offset; if(
_temp2527.base == 0? 1:( _temp2529 < _temp2527.base? 1: _temp2529 >= _temp2527.last_plus_one)){
_throw( Null_Exception);}* _temp2529;}).last_line)); _temp2517->tl= 0; _temp2517;});
_temp2516;}); _temp2515;}); break; case 24: _LL2514: yyval=({ struct
_tagged_ptr2 _temp2531= yyvs; struct _xtunion_struct** _temp2533= _temp2531.curr
+ yyvsp_offset; if( _temp2531.base == 0? 1:( _temp2533 < _temp2531.base? 1:
_temp2533 >= _temp2531.last_plus_one)){ _throw( Null_Exception);}* _temp2533;});
break; case 25: _LL2530: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclList_tok_struct*
_temp2535=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2535[ 0]=({ struct Cyc_DeclList_tok_struct _temp2536; _temp2536.tag= Cyc_DeclList_tok_tag;
_temp2536.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2537= yyvs; struct _xtunion_struct** _temp2539= _temp2537.curr +(
yyvsp_offset - 1); if( _temp2537.base == 0? 1:( _temp2539 < _temp2537.base? 1:
_temp2539 >= _temp2537.last_plus_one)){ _throw( Null_Exception);}* _temp2539;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2540= yyvs; struct
_xtunion_struct** _temp2542= _temp2540.curr + yyvsp_offset; if( _temp2540.base
== 0? 1:( _temp2542 < _temp2540.base? 1: _temp2542 >= _temp2540.last_plus_one)){
_throw( Null_Exception);}* _temp2542;}))); _temp2536;}); _temp2535;}); break;
case 26: _LL2534: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct*
_temp2544=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2544[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2545; _temp2545.tag= Cyc_DeclSpec_tok_tag;
_temp2545.f1=({ struct Cyc_Parse_Declaration_spec* _temp2546=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2546->sc=({ struct
Cyc_Core_Opt* _temp2550=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2550->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2551= yyvs; struct _xtunion_struct** _temp2553= _temp2551.curr +(
yyvsp_offset - 1); if( _temp2551.base == 0? 1:( _temp2553 < _temp2551.base? 1:
_temp2553 >= _temp2551.last_plus_one)){ _throw( Null_Exception);}* _temp2553;}));
_temp2550;}); _temp2546->tq= Cyc_Absyn_empty_tqual(); _temp2546->type_specs= 0;
_temp2546->is_inline= 0; _temp2546->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2547= yyvs; struct _xtunion_struct** _temp2549=
_temp2547.curr + yyvsp_offset; if( _temp2547.base == 0? 1:( _temp2549 <
_temp2547.base? 1: _temp2549 >= _temp2547.last_plus_one)){ _throw(
Null_Exception);}* _temp2549;})); _temp2546;}); _temp2545;}); _temp2544;});
break; case 27: _LL2543: if(( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2555= yyvs; struct _xtunion_struct** _temp2557= _temp2555.curr +
yyvsp_offset; if( _temp2555.base == 0? 1:( _temp2557 < _temp2555.base? 1:
_temp2557 >= _temp2555.last_plus_one)){ _throw( Null_Exception);}* _temp2557;})))->sc
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp2558=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2559; _temp2559.curr= _temp2558; _temp2559.base=
_temp2558; _temp2559.last_plus_one= _temp2558 + 51; _temp2559;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2560= yyls; struct Cyc_Yyltype* _temp2562= _temp2560.curr
+( yylsp_offset - 2); if( _temp2560.base == 0? 1:( _temp2562 < _temp2560.base? 1:
_temp2562 >= _temp2560.last_plus_one)){ _throw( Null_Exception);}* _temp2562;}).first_line,({
struct _tagged_ptr3 _temp2563= yyls; struct Cyc_Yyltype* _temp2565= _temp2563.curr
+( yylsp_offset - 1); if( _temp2563.base == 0? 1:( _temp2565 < _temp2563.base? 1:
_temp2565 >= _temp2563.last_plus_one)){ _throw( Null_Exception);}* _temp2565;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2566=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2566[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2567; _temp2567.tag= Cyc_DeclSpec_tok_tag;
_temp2567.f1=({ struct Cyc_Parse_Declaration_spec* _temp2568=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2568->sc=({ struct
Cyc_Core_Opt* _temp2584=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2584->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2585= yyvs; struct _xtunion_struct** _temp2587= _temp2585.curr +(
yyvsp_offset - 2); if( _temp2585.base == 0? 1:( _temp2587 < _temp2585.base? 1:
_temp2587 >= _temp2585.last_plus_one)){ _throw( Null_Exception);}* _temp2587;}));
_temp2584;}); _temp2568->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2581= yyvs; struct _xtunion_struct** _temp2583= _temp2581.curr +
yyvsp_offset; if( _temp2581.base == 0? 1:( _temp2583 < _temp2581.base? 1:
_temp2583 >= _temp2581.last_plus_one)){ _throw( Null_Exception);}* _temp2583;})))->tq;
_temp2568->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2578=
yyvs; struct _xtunion_struct** _temp2580= _temp2578.curr + yyvsp_offset; if(
_temp2578.base == 0? 1:( _temp2580 < _temp2578.base? 1: _temp2580 >= _temp2578.last_plus_one)){
_throw( Null_Exception);}* _temp2580;})))->type_specs; _temp2568->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2575= yyvs; struct
_xtunion_struct** _temp2577= _temp2575.curr + yyvsp_offset; if( _temp2575.base
== 0? 1:( _temp2577 < _temp2575.base? 1: _temp2577 >= _temp2575.last_plus_one)){
_throw( Null_Exception);}* _temp2577;})))->is_inline; _temp2568->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2569= yyvs; struct
_xtunion_struct** _temp2571= _temp2569.curr +( yyvsp_offset - 1); if( _temp2569.base
== 0? 1:( _temp2571 < _temp2569.base? 1: _temp2571 >= _temp2569.last_plus_one)){
_throw( Null_Exception);}* _temp2571;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2572= yyvs; struct _xtunion_struct** _temp2574= _temp2572.curr
+ yyvsp_offset; if( _temp2572.base == 0? 1:( _temp2574 < _temp2572.base? 1:
_temp2574 >= _temp2572.last_plus_one)){ _throw( Null_Exception);}* _temp2574;})))->attributes);
_temp2568;}); _temp2567;}); _temp2566;}); break; case 28: _LL2554: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2589=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2589[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2590; _temp2590.tag= Cyc_DeclSpec_tok_tag;
_temp2590.f1=({ struct Cyc_Parse_Declaration_spec* _temp2591=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2591->sc= 0;
_temp2591->tq= Cyc_Absyn_empty_tqual(); _temp2591->type_specs=({ struct Cyc_List_List*
_temp2595=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2595->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2596= yyvs; struct _xtunion_struct** _temp2598= _temp2596.curr +(
yyvsp_offset - 1); if( _temp2596.base == 0? 1:( _temp2598 < _temp2596.base? 1:
_temp2598 >= _temp2596.last_plus_one)){ _throw( Null_Exception);}* _temp2598;}));
_temp2595->tl= 0; _temp2595;}); _temp2591->is_inline= 0; _temp2591->attributes=
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2592= yyvs; struct
_xtunion_struct** _temp2594= _temp2592.curr + yyvsp_offset; if( _temp2592.base
== 0? 1:( _temp2594 < _temp2592.base? 1: _temp2594 >= _temp2592.last_plus_one)){
_throw( Null_Exception);}* _temp2594;})); _temp2591;}); _temp2590;}); _temp2589;});
break; case 29: _LL2588: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct*
_temp2600=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2600[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2601; _temp2601.tag= Cyc_DeclSpec_tok_tag;
_temp2601.f1=({ struct Cyc_Parse_Declaration_spec* _temp2602=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2602->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2622= yyvs; struct _xtunion_struct** _temp2624=
_temp2622.curr + yyvsp_offset; if( _temp2622.base == 0? 1:( _temp2624 <
_temp2622.base? 1: _temp2624 >= _temp2622.last_plus_one)){ _throw(
Null_Exception);}* _temp2624;})))->sc; _temp2602->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2619= yyvs; struct _xtunion_struct** _temp2621=
_temp2619.curr + yyvsp_offset; if( _temp2619.base == 0? 1:( _temp2621 <
_temp2619.base? 1: _temp2621 >= _temp2619.last_plus_one)){ _throw(
Null_Exception);}* _temp2621;})))->tq; _temp2602->type_specs=({ struct Cyc_List_List*
_temp2612=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2612->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2616= yyvs; struct _xtunion_struct** _temp2618= _temp2616.curr +(
yyvsp_offset - 2); if( _temp2616.base == 0? 1:( _temp2618 < _temp2616.base? 1:
_temp2618 >= _temp2616.last_plus_one)){ _throw( Null_Exception);}* _temp2618;}));
_temp2612->tl=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2613= yyvs;
struct _xtunion_struct** _temp2615= _temp2613.curr + yyvsp_offset; if( _temp2613.base
== 0? 1:( _temp2615 < _temp2613.base? 1: _temp2615 >= _temp2613.last_plus_one)){
_throw( Null_Exception);}* _temp2615;})))->type_specs; _temp2612;}); _temp2602->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2609= yyvs; struct
_xtunion_struct** _temp2611= _temp2609.curr + yyvsp_offset; if( _temp2609.base
== 0? 1:( _temp2611 < _temp2609.base? 1: _temp2611 >= _temp2609.last_plus_one)){
_throw( Null_Exception);}* _temp2611;})))->is_inline; _temp2602->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2603= yyvs; struct
_xtunion_struct** _temp2605= _temp2603.curr +( yyvsp_offset - 1); if( _temp2603.base
== 0? 1:( _temp2605 < _temp2603.base? 1: _temp2605 >= _temp2603.last_plus_one)){
_throw( Null_Exception);}* _temp2605;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2606= yyvs; struct _xtunion_struct** _temp2608= _temp2606.curr
+ yyvsp_offset; if( _temp2606.base == 0? 1:( _temp2608 < _temp2606.base? 1:
_temp2608 >= _temp2606.last_plus_one)){ _throw( Null_Exception);}* _temp2608;})))->attributes);
_temp2602;}); _temp2601;}); _temp2600;}); break; case 30: _LL2599: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2626=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2626[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2627; _temp2627.tag= Cyc_DeclSpec_tok_tag;
_temp2627.f1=({ struct Cyc_Parse_Declaration_spec* _temp2628=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2628->sc= 0;
_temp2628->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2632= yyvs;
struct _xtunion_struct** _temp2634= _temp2632.curr +( yyvsp_offset - 1); if(
_temp2632.base == 0? 1:( _temp2634 < _temp2632.base? 1: _temp2634 >= _temp2632.last_plus_one)){
_throw( Null_Exception);}* _temp2634;})); _temp2628->type_specs= 0; _temp2628->is_inline=
0; _temp2628->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2629= yyvs; struct _xtunion_struct** _temp2631= _temp2629.curr +
yyvsp_offset; if( _temp2629.base == 0? 1:( _temp2631 < _temp2629.base? 1:
_temp2631 >= _temp2629.last_plus_one)){ _throw( Null_Exception);}* _temp2631;}));
_temp2628;}); _temp2627;}); _temp2626;}); break; case 31: _LL2625: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2636=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2636[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2637; _temp2637.tag= Cyc_DeclSpec_tok_tag;
_temp2637.f1=({ struct Cyc_Parse_Declaration_spec* _temp2638=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2638->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2657= yyvs; struct _xtunion_struct** _temp2659=
_temp2657.curr + yyvsp_offset; if( _temp2657.base == 0? 1:( _temp2659 <
_temp2657.base? 1: _temp2659 >= _temp2657.last_plus_one)){ _throw(
Null_Exception);}* _temp2659;})))->sc; _temp2638->tq= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2651= yyvs; struct
_xtunion_struct** _temp2653= _temp2651.curr +( yyvsp_offset - 2); if( _temp2651.base
== 0? 1:( _temp2653 < _temp2651.base? 1: _temp2653 >= _temp2651.last_plus_one)){
_throw( Null_Exception);}* _temp2653;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2654= yyvs; struct _xtunion_struct** _temp2656= _temp2654.curr
+ yyvsp_offset; if( _temp2654.base == 0? 1:( _temp2656 < _temp2654.base? 1:
_temp2656 >= _temp2654.last_plus_one)){ _throw( Null_Exception);}* _temp2656;})))->tq);
_temp2638->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2648=
yyvs; struct _xtunion_struct** _temp2650= _temp2648.curr + yyvsp_offset; if(
_temp2648.base == 0? 1:( _temp2650 < _temp2648.base? 1: _temp2650 >= _temp2648.last_plus_one)){
_throw( Null_Exception);}* _temp2650;})))->type_specs; _temp2638->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2645= yyvs; struct
_xtunion_struct** _temp2647= _temp2645.curr + yyvsp_offset; if( _temp2645.base
== 0? 1:( _temp2647 < _temp2645.base? 1: _temp2647 >= _temp2645.last_plus_one)){
_throw( Null_Exception);}* _temp2647;})))->is_inline; _temp2638->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2639= yyvs; struct
_xtunion_struct** _temp2641= _temp2639.curr +( yyvsp_offset - 1); if( _temp2639.base
== 0? 1:( _temp2641 < _temp2639.base? 1: _temp2641 >= _temp2639.last_plus_one)){
_throw( Null_Exception);}* _temp2641;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2642= yyvs; struct _xtunion_struct** _temp2644= _temp2642.curr
+ yyvsp_offset; if( _temp2642.base == 0? 1:( _temp2644 < _temp2642.base? 1:
_temp2644 >= _temp2642.last_plus_one)){ _throw( Null_Exception);}* _temp2644;})))->attributes);
_temp2638;}); _temp2637;}); _temp2636;}); break; case 32: _LL2635: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2661=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2661[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2662; _temp2662.tag= Cyc_DeclSpec_tok_tag;
_temp2662.f1=({ struct Cyc_Parse_Declaration_spec* _temp2663=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2663->sc= 0;
_temp2663->tq= Cyc_Absyn_empty_tqual(); _temp2663->type_specs= 0; _temp2663->is_inline=
1; _temp2663->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2664= yyvs; struct _xtunion_struct** _temp2666= _temp2664.curr +
yyvsp_offset; if( _temp2664.base == 0? 1:( _temp2666 < _temp2664.base? 1:
_temp2666 >= _temp2664.last_plus_one)){ _throw( Null_Exception);}* _temp2666;}));
_temp2663;}); _temp2662;}); _temp2661;}); break; case 33: _LL2660: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclSpec_tok_struct* _temp2668=( struct
Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2668[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2669; _temp2669.tag= Cyc_DeclSpec_tok_tag;
_temp2669.f1=({ struct Cyc_Parse_Declaration_spec* _temp2670=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2670->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2683= yyvs; struct _xtunion_struct** _temp2685=
_temp2683.curr + yyvsp_offset; if( _temp2683.base == 0? 1:( _temp2685 <
_temp2683.base? 1: _temp2685 >= _temp2683.last_plus_one)){ _throw(
Null_Exception);}* _temp2685;})))->sc; _temp2670->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2680= yyvs; struct _xtunion_struct** _temp2682=
_temp2680.curr + yyvsp_offset; if( _temp2680.base == 0? 1:( _temp2682 <
_temp2680.base? 1: _temp2682 >= _temp2680.last_plus_one)){ _throw(
Null_Exception);}* _temp2682;})))->tq; _temp2670->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2677= yyvs; struct _xtunion_struct** _temp2679=
_temp2677.curr + yyvsp_offset; if( _temp2677.base == 0? 1:( _temp2679 <
_temp2677.base? 1: _temp2679 >= _temp2677.last_plus_one)){ _throw(
Null_Exception);}* _temp2679;})))->type_specs; _temp2670->is_inline= 1;
_temp2670->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2671= yyvs; struct _xtunion_struct** _temp2673=
_temp2671.curr +( yyvsp_offset - 1); if( _temp2671.base == 0? 1:( _temp2673 <
_temp2671.base? 1: _temp2673 >= _temp2671.last_plus_one)){ _throw(
Null_Exception);}* _temp2673;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2674= yyvs; struct _xtunion_struct** _temp2676= _temp2674.curr +
yyvsp_offset; if( _temp2674.base == 0? 1:( _temp2676 < _temp2674.base? 1:
_temp2676 >= _temp2674.last_plus_one)){ _throw( Null_Exception);}* _temp2676;})))->attributes);
_temp2670;}); _temp2669;}); _temp2668;}); break; case 34: _LL2667: yyval=(
struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2687=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2687[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2688; _temp2688.tag=
Cyc_StorageClass_tok_tag; _temp2688.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp2688;}); _temp2687;}); break; case 35: _LL2686: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2690=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2690[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2691; _temp2691.tag= Cyc_StorageClass_tok_tag;
_temp2691.f1=( void*)(( void*) Cyc_Parse_Register_sc); _temp2691;}); _temp2690;});
break; case 36: _LL2689: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2693=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2693[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2694; _temp2694.tag=
Cyc_StorageClass_tok_tag; _temp2694.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2694;}); _temp2693;}); break; case 37: _LL2692: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2696=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2696[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2697; _temp2697.tag= Cyc_StorageClass_tok_tag;
_temp2697.f1=( void*)(( void*) Cyc_Parse_Extern_sc); _temp2697;}); _temp2696;});
break; case 38: _LL2695: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2699= yyvs; struct _xtunion_struct** _temp2701= _temp2699.curr
+ yyvsp_offset; if( _temp2699.base == 0? 1:( _temp2701 < _temp2699.base? 1:
_temp2701 >= _temp2699.last_plus_one)){ _throw( Null_Exception);}* _temp2701;})),(
struct _tagged_string)({ char* _temp2702=( char*)"C"; struct _tagged_string
_temp2703; _temp2703.curr= _temp2702; _temp2703.base= _temp2702; _temp2703.last_plus_one=
_temp2702 + 2; _temp2703;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2704=( char*)"only extern or extern \"C\" is allowed"; struct
_tagged_string _temp2705; _temp2705.curr= _temp2704; _temp2705.base= _temp2704;
_temp2705.last_plus_one= _temp2704 + 37; _temp2705;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2706= yyls; struct Cyc_Yyltype* _temp2708= _temp2706.curr
+( yylsp_offset - 1); if( _temp2706.base == 0? 1:( _temp2708 < _temp2706.base? 1:
_temp2708 >= _temp2706.last_plus_one)){ _throw( Null_Exception);}* _temp2708;}).first_line,({
struct _tagged_ptr3 _temp2709= yyls; struct Cyc_Yyltype* _temp2711= _temp2709.curr
+ yylsp_offset; if( _temp2709.base == 0? 1:( _temp2711 < _temp2709.base? 1:
_temp2711 >= _temp2709.last_plus_one)){ _throw( Null_Exception);}* _temp2711;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2712=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2712[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2713; _temp2713.tag=
Cyc_StorageClass_tok_tag; _temp2713.f1=( void*)(( void*) Cyc_Parse_ExternC_sc);
_temp2713;}); _temp2712;}); break; case 39: _LL2698: yyval=( struct
_xtunion_struct*)({ struct Cyc_StorageClass_tok_struct* _temp2715=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2715[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2716; _temp2716.tag= Cyc_StorageClass_tok_tag;
_temp2716.f1=( void*)(( void*) Cyc_Parse_Typedef_sc); _temp2716;}); _temp2715;});
break; case 40: _LL2714: yyval=( struct _xtunion_struct*)({ struct Cyc_StorageClass_tok_struct*
_temp2718=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2718[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2719; _temp2719.tag=
Cyc_StorageClass_tok_tag; _temp2719.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2719;}); _temp2718;}); break; case 41: _LL2717: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2721=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2721[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2722; _temp2722.tag= Cyc_AttributeList_tok_tag;
_temp2722.f1= 0; _temp2722;}); _temp2721;}); break; case 42: _LL2720: yyval=({
struct _tagged_ptr2 _temp2724= yyvs; struct _xtunion_struct** _temp2726=
_temp2724.curr + yyvsp_offset; if( _temp2724.base == 0? 1:( _temp2726 <
_temp2724.base? 1: _temp2726 >= _temp2724.last_plus_one)){ _throw(
Null_Exception);}* _temp2726;}); break; case 43: _LL2723: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2728=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2728[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2729; _temp2729.tag= Cyc_AttributeList_tok_tag;
_temp2729.f1= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2730= yyvs;
struct _xtunion_struct** _temp2732= _temp2730.curr +( yyvsp_offset - 2); if(
_temp2730.base == 0? 1:( _temp2732 < _temp2730.base? 1: _temp2732 >= _temp2730.last_plus_one)){
_throw( Null_Exception);}* _temp2732;})); _temp2729;}); _temp2728;}); break;
case 44: _LL2727: yyval=( struct _xtunion_struct*)({ struct Cyc_AttributeList_tok_struct*
_temp2734=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2734[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2735; _temp2735.tag=
Cyc_AttributeList_tok_tag; _temp2735.f1=({ struct Cyc_List_List* _temp2736=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2736->hd=(
void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2737= yyvs; struct
_xtunion_struct** _temp2739= _temp2737.curr + yyvsp_offset; if( _temp2737.base
== 0? 1:( _temp2739 < _temp2737.base? 1: _temp2739 >= _temp2737.last_plus_one)){
_throw( Null_Exception);}* _temp2739;})); _temp2736->tl= 0; _temp2736;});
_temp2735;}); _temp2734;}); break; case 45: _LL2733: yyval=( struct
_xtunion_struct*)({ struct Cyc_AttributeList_tok_struct* _temp2741=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2741[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2742; _temp2742.tag= Cyc_AttributeList_tok_tag;
_temp2742.f1=({ struct Cyc_List_List* _temp2743=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2743->hd=( void*) Cyc_yyget_Attribute_tok(({
struct _tagged_ptr2 _temp2747= yyvs; struct _xtunion_struct** _temp2749=
_temp2747.curr +( yyvsp_offset - 2); if( _temp2747.base == 0? 1:( _temp2749 <
_temp2747.base? 1: _temp2749 >= _temp2747.last_plus_one)){ _throw(
Null_Exception);}* _temp2749;})); _temp2743->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2744= yyvs; struct _xtunion_struct** _temp2746=
_temp2744.curr + yyvsp_offset; if( _temp2744.base == 0? 1:( _temp2746 <
_temp2744.base? 1: _temp2746 >= _temp2744.last_plus_one)){ _throw(
Null_Exception);}* _temp2746;})); _temp2743;}); _temp2742;}); _temp2741;});
break; case 46: _LL2740: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2751= yyvs; struct _xtunion_struct** _temp2753=
_temp2751.curr + yyvsp_offset; if( _temp2751.base == 0? 1:( _temp2753 <
_temp2751.base? 1: _temp2753 >= _temp2751.last_plus_one)){ _throw(
Null_Exception);}* _temp2753;})); void* a; if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2754=( char*)"stdcall"; struct _tagged_string
_temp2755; _temp2755.curr= _temp2754; _temp2755.base= _temp2754; _temp2755.last_plus_one=
_temp2754 + 8; _temp2755;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2756=( char*)"__stdcall__"; struct _tagged_string
_temp2757; _temp2757.curr= _temp2756; _temp2757.base= _temp2756; _temp2757.last_plus_one=
_temp2756 + 12; _temp2757;})) == 0){ a=( void*) Cyc_Absyn_Stdcall_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2758=( char*)"cdecl";
struct _tagged_string _temp2759; _temp2759.curr= _temp2758; _temp2759.base=
_temp2758; _temp2759.last_plus_one= _temp2758 + 6; _temp2759;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2760=( char*)"__cdecl__"; struct
_tagged_string _temp2761; _temp2761.curr= _temp2760; _temp2761.base= _temp2760;
_temp2761.last_plus_one= _temp2760 + 10; _temp2761;})) == 0){ a=( void*) Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2762=(
char*)"noreturn"; struct _tagged_string _temp2763; _temp2763.curr= _temp2762;
_temp2763.base= _temp2762; _temp2763.last_plus_one= _temp2762 + 9; _temp2763;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2764=( char*)"__noreturn__";
struct _tagged_string _temp2765; _temp2765.curr= _temp2764; _temp2765.base=
_temp2764; _temp2765.last_plus_one= _temp2764 + 13; _temp2765;})) == 0){ a=(
void*) Cyc_Absyn_Noreturn_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2766=( char*)"noreturn"; struct _tagged_string
_temp2767; _temp2767.curr= _temp2766; _temp2767.base= _temp2766; _temp2767.last_plus_one=
_temp2766 + 9; _temp2767;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2768=( char*)"__noreturn__"; struct _tagged_string
_temp2769; _temp2769.curr= _temp2768; _temp2769.base= _temp2768; _temp2769.last_plus_one=
_temp2768 + 13; _temp2769;})) == 0){ a=( void*) Cyc_Absyn_Noreturn_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2770=( char*)"__const__";
struct _tagged_string _temp2771; _temp2771.curr= _temp2770; _temp2771.base=
_temp2770; _temp2771.last_plus_one= _temp2770 + 10; _temp2771;})) == 0){ a=(
void*) Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2772=( char*)"aligned"; struct _tagged_string
_temp2773; _temp2773.curr= _temp2772; _temp2773.base= _temp2772; _temp2773.last_plus_one=
_temp2772 + 8; _temp2773;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2774=( char*)"__aligned__"; struct _tagged_string
_temp2775; _temp2775.curr= _temp2774; _temp2775.base= _temp2774; _temp2775.last_plus_one=
_temp2774 + 12; _temp2775;})) == 0){ a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp2776=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct));
_temp2776[ 0]=({ struct Cyc_Absyn_Aligned_att_struct _temp2777; _temp2777.tag=
Cyc_Absyn_Aligned_att_tag; _temp2777.f1= - 1; _temp2777;}); _temp2776;});} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2778=( char*)"packed";
struct _tagged_string _temp2779; _temp2779.curr= _temp2778; _temp2779.base=
_temp2778; _temp2779.last_plus_one= _temp2778 + 7; _temp2779;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2780=( char*)"__packed__"; struct
_tagged_string _temp2781; _temp2781.curr= _temp2780; _temp2781.base= _temp2780;
_temp2781.last_plus_one= _temp2780 + 11; _temp2781;})) == 0){ a=( void*) Cyc_Absyn_Packed_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2782=(
char*)"shared"; struct _tagged_string _temp2783; _temp2783.curr= _temp2782;
_temp2783.base= _temp2782; _temp2783.last_plus_one= _temp2782 + 7; _temp2783;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2784=( char*)"__shared__";
struct _tagged_string _temp2785; _temp2785.curr= _temp2784; _temp2785.base=
_temp2784; _temp2785.last_plus_one= _temp2784 + 11; _temp2785;})) == 0){ a=(
void*) Cyc_Absyn_Shared_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2786=( char*)"unused"; struct _tagged_string
_temp2787; _temp2787.curr= _temp2786; _temp2787.base= _temp2786; _temp2787.last_plus_one=
_temp2786 + 7; _temp2787;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2788=( char*)"__unused__"; struct _tagged_string
_temp2789; _temp2789.curr= _temp2788; _temp2789.base= _temp2788; _temp2789.last_plus_one=
_temp2788 + 11; _temp2789;})) == 0){ a=( void*) Cyc_Absyn_Unused_att;} else{ if(
Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2790=( char*)"weak";
struct _tagged_string _temp2791; _temp2791.curr= _temp2790; _temp2791.base=
_temp2790; _temp2791.last_plus_one= _temp2790 + 5; _temp2791;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2792=( char*)"__weak__"; struct
_tagged_string _temp2793; _temp2793.curr= _temp2792; _temp2793.base= _temp2792;
_temp2793.last_plus_one= _temp2792 + 9; _temp2793;})) == 0){ a=( void*) Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2794=(
char*)"dllimport"; struct _tagged_string _temp2795; _temp2795.curr= _temp2794;
_temp2795.base= _temp2794; _temp2795.last_plus_one= _temp2794 + 10; _temp2795;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2796=( char*)"__dllimport__";
struct _tagged_string _temp2797; _temp2797.curr= _temp2796; _temp2797.base=
_temp2796; _temp2797.last_plus_one= _temp2796 + 14; _temp2797;})) == 0){ a=(
void*) Cyc_Absyn_Dllimport_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2798=( char*)"dllexport"; struct _tagged_string
_temp2799; _temp2799.curr= _temp2798; _temp2799.base= _temp2798; _temp2799.last_plus_one=
_temp2798 + 10; _temp2799;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2800=( char*)"__dllexport__"; struct _tagged_string
_temp2801; _temp2801.curr= _temp2800; _temp2801.base= _temp2800; _temp2801.last_plus_one=
_temp2800 + 14; _temp2801;})) == 0){ a=( void*) Cyc_Absyn_Dllexport_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2802=( char*)"no_instrument_function";
struct _tagged_string _temp2803; _temp2803.curr= _temp2802; _temp2803.base=
_temp2802; _temp2803.last_plus_one= _temp2802 + 23; _temp2803;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2804=( char*)"__no_instrument_function__";
struct _tagged_string _temp2805; _temp2805.curr= _temp2804; _temp2805.base=
_temp2804; _temp2805.last_plus_one= _temp2804 + 27; _temp2805;})) == 0){ a=(
void*) Cyc_Absyn_No_instrument_function_att;} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2806=( char*)"constructor"; struct
_tagged_string _temp2807; _temp2807.curr= _temp2806; _temp2807.base= _temp2806;
_temp2807.last_plus_one= _temp2806 + 12; _temp2807;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2808=( char*)"__constructor__"; struct
_tagged_string _temp2809; _temp2809.curr= _temp2808; _temp2809.base= _temp2808;
_temp2809.last_plus_one= _temp2808 + 16; _temp2809;})) == 0){ a=( void*) Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2810=(
char*)"destructor"; struct _tagged_string _temp2811; _temp2811.curr= _temp2810;
_temp2811.base= _temp2810; _temp2811.last_plus_one= _temp2810 + 11; _temp2811;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2812=( char*)"__destructor__";
struct _tagged_string _temp2813; _temp2813.curr= _temp2812; _temp2813.base=
_temp2812; _temp2813.last_plus_one= _temp2812 + 15; _temp2813;})) == 0){ a=(
void*) Cyc_Absyn_Destructor_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2814=( char*)"no_check_memory_usage"; struct
_tagged_string _temp2815; _temp2815.curr= _temp2814; _temp2815.base= _temp2814;
_temp2815.last_plus_one= _temp2814 + 22; _temp2815;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2816=( char*)"__no_check_memory_usage__";
struct _tagged_string _temp2817; _temp2817.curr= _temp2816; _temp2817.base=
_temp2816; _temp2817.last_plus_one= _temp2816 + 26; _temp2817;})) == 0){ a=(
void*) Cyc_Absyn_No_check_memory_usage_att;} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2818=( char*)"unrecognized attribute"; struct
_tagged_string _temp2819; _temp2819.curr= _temp2818; _temp2819.base= _temp2818;
_temp2819.last_plus_one= _temp2818 + 23; _temp2819;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2820= yyls; struct Cyc_Yyltype* _temp2822= _temp2820.curr
+ yylsp_offset; if( _temp2820.base == 0? 1:( _temp2822 < _temp2820.base? 1:
_temp2822 >= _temp2820.last_plus_one)){ _throw( Null_Exception);}* _temp2822;}).first_line,({
struct _tagged_ptr3 _temp2823= yyls; struct Cyc_Yyltype* _temp2825= _temp2823.curr
+ yylsp_offset; if( _temp2823.base == 0? 1:( _temp2825 < _temp2823.base? 1:
_temp2825 >= _temp2823.last_plus_one)){ _throw( Null_Exception);}* _temp2825;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}} yyval=( struct _xtunion_struct*)({
struct Cyc_Attribute_tok_struct* _temp2826=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2826[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2827; _temp2827.tag= Cyc_Attribute_tok_tag;
_temp2827.f1=( void*) a; _temp2827;}); _temp2826;}); break;} case 47: _LL2750:
yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct* _temp2829=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2829[ 0]=({ struct Cyc_Attribute_tok_struct _temp2830; _temp2830.tag= Cyc_Attribute_tok_tag;
_temp2830.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2830;}); _temp2829;});
break; case 48: _LL2828: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2832= yyvs; struct _xtunion_struct** _temp2834=
_temp2832.curr +( yyvsp_offset - 3); if( _temp2832.base == 0? 1:( _temp2834 <
_temp2832.base? 1: _temp2834 >= _temp2832.last_plus_one)){ _throw(
Null_Exception);}* _temp2834;})); struct _tuple12 _temp2840; int _temp2841; void*
_temp2843; struct _tuple12* _temp2838= Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp2835= yyvs; struct _xtunion_struct** _temp2837= _temp2835.curr +(
yyvsp_offset - 1); if( _temp2835.base == 0? 1:( _temp2837 < _temp2835.base? 1:
_temp2837 >= _temp2835.last_plus_one)){ _throw( Null_Exception);}* _temp2837;}));
_temp2840=* _temp2838; _LL2844: _temp2843= _temp2840.f1; goto _LL2842; _LL2842:
_temp2841= _temp2840.f2; goto _LL2839; _LL2839: { void* a; if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2845=( char*)"regparm"; struct
_tagged_string _temp2846; _temp2846.curr= _temp2845; _temp2846.base= _temp2845;
_temp2846.last_plus_one= _temp2845 + 8; _temp2846;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2847=( char*)"__regparm__"; struct
_tagged_string _temp2848; _temp2848.curr= _temp2847; _temp2848.base= _temp2847;
_temp2848.last_plus_one= _temp2847 + 12; _temp2848;})) == 0){ if( _temp2841 <= 0?
1: _temp2841 > 3){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2849=(
char*)"regparm requires value between 1 and 3"; struct _tagged_string _temp2850;
_temp2850.curr= _temp2849; _temp2850.base= _temp2849; _temp2850.last_plus_one=
_temp2849 + 39; _temp2850;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2851= yyls; struct Cyc_Yyltype* _temp2853= _temp2851.curr +( yylsp_offset -
1); if( _temp2851.base == 0? 1:( _temp2853 < _temp2851.base? 1: _temp2853 >=
_temp2851.last_plus_one)){ _throw( Null_Exception);}* _temp2853;}).first_line,({
struct _tagged_ptr3 _temp2854= yyls; struct Cyc_Yyltype* _temp2856= _temp2854.curr
+( yylsp_offset - 1); if( _temp2854.base == 0? 1:( _temp2856 < _temp2854.base? 1:
_temp2856 >= _temp2854.last_plus_one)){ _throw( Null_Exception);}* _temp2856;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2857=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2857[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2858; _temp2858.tag= Cyc_Absyn_Regparm_att_tag;
_temp2858.f1= _temp2841; _temp2858;}); _temp2857;});} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2859=( char*)"aligned"; struct
_tagged_string _temp2860; _temp2860.curr= _temp2859; _temp2860.base= _temp2859;
_temp2860.last_plus_one= _temp2859 + 8; _temp2860;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2861=( char*)"__aligned__"; struct
_tagged_string _temp2862; _temp2862.curr= _temp2861; _temp2862.base= _temp2861;
_temp2862.last_plus_one= _temp2861 + 12; _temp2862;})) == 0){ if( _temp2841 < 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2863=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2864; _temp2864.curr= _temp2863; _temp2864.base=
_temp2863; _temp2864.last_plus_one= _temp2863 + 39; _temp2864;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2865= yyls; struct Cyc_Yyltype* _temp2867= _temp2865.curr
+( yylsp_offset - 1); if( _temp2865.base == 0? 1:( _temp2867 < _temp2865.base? 1:
_temp2867 >= _temp2865.last_plus_one)){ _throw( Null_Exception);}* _temp2867;}).first_line,({
struct _tagged_ptr3 _temp2868= yyls; struct Cyc_Yyltype* _temp2870= _temp2868.curr
+( yylsp_offset - 1); if( _temp2868.base == 0? 1:( _temp2870 < _temp2868.base? 1:
_temp2870 >= _temp2868.last_plus_one)){ _throw( Null_Exception);}* _temp2870;}).last_line));}{
unsigned int j=( unsigned int) _temp2841; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2871=(
char*)"aligned requires positive power of two"; struct _tagged_string _temp2872;
_temp2872.curr= _temp2871; _temp2872.base= _temp2871; _temp2872.last_plus_one=
_temp2871 + 39; _temp2872;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2873= yyls; struct Cyc_Yyltype* _temp2875= _temp2873.curr +( yylsp_offset -
1); if( _temp2873.base == 0? 1:( _temp2875 < _temp2873.base? 1: _temp2875 >=
_temp2873.last_plus_one)){ _throw( Null_Exception);}* _temp2875;}).first_line,({
struct _tagged_ptr3 _temp2876= yyls; struct Cyc_Yyltype* _temp2878= _temp2876.curr
+( yylsp_offset - 1); if( _temp2876.base == 0? 1:( _temp2878 < _temp2876.base? 1:
_temp2878 >= _temp2876.last_plus_one)){ _throw( Null_Exception);}* _temp2878;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2879=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2879[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2880; _temp2880.tag= Cyc_Absyn_Aligned_att_tag;
_temp2880.f1= _temp2841; _temp2880;}); _temp2879;});}} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2881=( char*)"unrecognized attribute";
struct _tagged_string _temp2882; _temp2882.curr= _temp2881; _temp2882.base=
_temp2881; _temp2882.last_plus_one= _temp2881 + 23; _temp2882;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2883= yyls; struct Cyc_Yyltype* _temp2885= _temp2883.curr
+( yylsp_offset - 3); if( _temp2883.base == 0? 1:( _temp2885 < _temp2883.base? 1:
_temp2885 >= _temp2883.last_plus_one)){ _throw( Null_Exception);}* _temp2885;}).first_line,({
struct _tagged_ptr3 _temp2886= yyls; struct Cyc_Yyltype* _temp2888= _temp2886.curr
+( yylsp_offset - 3); if( _temp2886.base == 0? 1:( _temp2888 < _temp2886.base? 1:
_temp2888 >= _temp2886.last_plus_one)){ _throw( Null_Exception);}* _temp2888;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct*
_temp2889=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2889[ 0]=({ struct Cyc_Attribute_tok_struct _temp2890; _temp2890.tag= Cyc_Attribute_tok_tag;
_temp2890.f1=( void*) a; _temp2890;}); _temp2889;}); break;}} case 49: _LL2831: {
struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2892=
yyvs; struct _xtunion_struct** _temp2894= _temp2892.curr +( yyvsp_offset - 3);
if( _temp2892.base == 0? 1:( _temp2894 < _temp2892.base? 1: _temp2894 >=
_temp2892.last_plus_one)){ _throw( Null_Exception);}* _temp2894;})); struct
_tagged_string str= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2895= yyvs;
struct _xtunion_struct** _temp2897= _temp2895.curr +( yyvsp_offset - 1); if(
_temp2895.base == 0? 1:( _temp2897 < _temp2895.base? 1: _temp2897 >= _temp2895.last_plus_one)){
_throw( Null_Exception);}* _temp2897;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2898=( char*)"section"; struct
_tagged_string _temp2899; _temp2899.curr= _temp2898; _temp2899.base= _temp2898;
_temp2899.last_plus_one= _temp2898 + 8; _temp2899;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2900=( char*)"__section__"; struct
_tagged_string _temp2901; _temp2901.curr= _temp2900; _temp2901.base= _temp2900;
_temp2901.last_plus_one= _temp2900 + 12; _temp2901;}))){ a=( void*)({ struct Cyc_Absyn_Section_att_struct*
_temp2902=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2902[ 0]=({ struct Cyc_Absyn_Section_att_struct _temp2903; _temp2903.tag=
Cyc_Absyn_Section_att_tag; _temp2903.f1= str; _temp2903;}); _temp2902;});} else{
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2904=( char*)"unrecognized attribute";
struct _tagged_string _temp2905; _temp2905.curr= _temp2904; _temp2905.base=
_temp2904; _temp2905.last_plus_one= _temp2904 + 23; _temp2905;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2906= yyls; struct Cyc_Yyltype* _temp2908= _temp2906.curr
+( yylsp_offset - 3); if( _temp2906.base == 0? 1:( _temp2908 < _temp2906.base? 1:
_temp2908 >= _temp2906.last_plus_one)){ _throw( Null_Exception);}* _temp2908;}).first_line,({
struct _tagged_ptr3 _temp2909= yyls; struct Cyc_Yyltype* _temp2911= _temp2909.curr
+( yylsp_offset - 3); if( _temp2909.base == 0? 1:( _temp2911 < _temp2909.base? 1:
_temp2911 >= _temp2909.last_plus_one)){ _throw( Null_Exception);}* _temp2911;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( struct _xtunion_struct*)({ struct Cyc_Attribute_tok_struct*
_temp2912=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2912[ 0]=({ struct Cyc_Attribute_tok_struct _temp2913; _temp2913.tag= Cyc_Attribute_tok_tag;
_temp2913.f1=( void*) a; _temp2913;}); _temp2912;}); break;} case 50: _LL2891:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2915=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2915[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2916; _temp2916.tag=
Cyc_TypeSpecifier_tok_tag; _temp2916.f1=( void*) Cyc_Parse_type_spec(( void*)
Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2917=
yyls; struct Cyc_Yyltype* _temp2919= _temp2917.curr + yylsp_offset; if(
_temp2917.base == 0? 1:( _temp2919 < _temp2917.base? 1: _temp2919 >= _temp2917.last_plus_one)){
_throw( Null_Exception);}* _temp2919;}).first_line,({ struct _tagged_ptr3
_temp2920= yyls; struct Cyc_Yyltype* _temp2922= _temp2920.curr + yylsp_offset;
if( _temp2920.base == 0? 1:( _temp2922 < _temp2920.base? 1: _temp2922 >=
_temp2920.last_plus_one)){ _throw( Null_Exception);}* _temp2922;}).last_line));
_temp2916;}); _temp2915;}); break; case 51: _LL2914: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2924=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2924[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2925; _temp2925.tag= Cyc_TypeSpecifier_tok_tag;
_temp2925.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2926= yyls; struct Cyc_Yyltype*
_temp2928= _temp2926.curr + yylsp_offset; if( _temp2926.base == 0? 1:( _temp2928
< _temp2926.base? 1: _temp2928 >= _temp2926.last_plus_one)){ _throw(
Null_Exception);}* _temp2928;}).first_line,({ struct _tagged_ptr3 _temp2929=
yyls; struct Cyc_Yyltype* _temp2931= _temp2929.curr + yylsp_offset; if(
_temp2929.base == 0? 1:( _temp2931 < _temp2929.base? 1: _temp2931 >= _temp2929.last_plus_one)){
_throw( Null_Exception);}* _temp2931;}).last_line)); _temp2925;}); _temp2924;});
break; case 52: _LL2923: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2933=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2933[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2934; _temp2934.tag=
Cyc_TypeSpecifier_tok_tag; _temp2934.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2935= yyls; struct Cyc_Yyltype*
_temp2937= _temp2935.curr + yylsp_offset; if( _temp2935.base == 0? 1:( _temp2937
< _temp2935.base? 1: _temp2937 >= _temp2935.last_plus_one)){ _throw(
Null_Exception);}* _temp2937;}).first_line,({ struct _tagged_ptr3 _temp2938=
yyls; struct Cyc_Yyltype* _temp2940= _temp2938.curr + yylsp_offset; if(
_temp2938.base == 0? 1:( _temp2940 < _temp2938.base? 1: _temp2940 >= _temp2938.last_plus_one)){
_throw( Null_Exception);}* _temp2940;}).last_line)); _temp2934;}); _temp2933;});
break; case 53: _LL2932: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2942=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2942[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2943; _temp2943.tag=
Cyc_TypeSpecifier_tok_tag; _temp2943.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp2944=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2944[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2945; _temp2945.tag=
Cyc_Parse_Short_spec_tag; _temp2945.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2946= yyls; struct Cyc_Yyltype* _temp2948= _temp2946.curr +
yylsp_offset; if( _temp2946.base == 0? 1:( _temp2948 < _temp2946.base? 1:
_temp2948 >= _temp2946.last_plus_one)){ _throw( Null_Exception);}* _temp2948;}).first_line,({
struct _tagged_ptr3 _temp2949= yyls; struct Cyc_Yyltype* _temp2951= _temp2949.curr
+ yylsp_offset; if( _temp2949.base == 0? 1:( _temp2951 < _temp2949.base? 1:
_temp2951 >= _temp2949.last_plus_one)){ _throw( Null_Exception);}* _temp2951;}).last_line);
_temp2945;}); _temp2944;})); _temp2943;}); _temp2942;}); break; case 54: _LL2941:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2953=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2953[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2954; _temp2954.tag=
Cyc_TypeSpecifier_tok_tag; _temp2954.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2955= yyls; struct Cyc_Yyltype*
_temp2957= _temp2955.curr + yylsp_offset; if( _temp2955.base == 0? 1:( _temp2957
< _temp2955.base? 1: _temp2957 >= _temp2955.last_plus_one)){ _throw(
Null_Exception);}* _temp2957;}).first_line,({ struct _tagged_ptr3 _temp2958=
yyls; struct Cyc_Yyltype* _temp2960= _temp2958.curr + yylsp_offset; if(
_temp2958.base == 0? 1:( _temp2960 < _temp2958.base? 1: _temp2960 >= _temp2958.last_plus_one)){
_throw( Null_Exception);}* _temp2960;}).last_line)); _temp2954;}); _temp2953;});
break; case 55: _LL2952: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2962=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2962[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2963; _temp2963.tag=
Cyc_TypeSpecifier_tok_tag; _temp2963.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2964=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2964[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2965; _temp2965.tag= Cyc_Parse_Long_spec_tag;
_temp2965.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2966= yyls;
struct Cyc_Yyltype* _temp2968= _temp2966.curr + yylsp_offset; if( _temp2966.base
== 0? 1:( _temp2968 < _temp2966.base? 1: _temp2968 >= _temp2966.last_plus_one)){
_throw( Null_Exception);}* _temp2968;}).first_line,({ struct _tagged_ptr3
_temp2969= yyls; struct Cyc_Yyltype* _temp2971= _temp2969.curr + yylsp_offset;
if( _temp2969.base == 0? 1:( _temp2971 < _temp2969.base? 1: _temp2971 >=
_temp2969.last_plus_one)){ _throw( Null_Exception);}* _temp2971;}).last_line);
_temp2965;}); _temp2964;})); _temp2963;}); _temp2962;}); break; case 56: _LL2961:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2973=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2973[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2974; _temp2974.tag=
Cyc_TypeSpecifier_tok_tag; _temp2974.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2975= yyls; struct Cyc_Yyltype*
_temp2977= _temp2975.curr + yylsp_offset; if( _temp2975.base == 0? 1:( _temp2977
< _temp2975.base? 1: _temp2977 >= _temp2975.last_plus_one)){ _throw(
Null_Exception);}* _temp2977;}).first_line,({ struct _tagged_ptr3 _temp2978=
yyls; struct Cyc_Yyltype* _temp2980= _temp2978.curr + yylsp_offset; if(
_temp2978.base == 0? 1:( _temp2980 < _temp2978.base? 1: _temp2980 >= _temp2978.last_plus_one)){
_throw( Null_Exception);}* _temp2980;}).last_line)); _temp2974;}); _temp2973;});
break; case 57: _LL2972: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2982=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2982[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2983; _temp2983.tag=
Cyc_TypeSpecifier_tok_tag; _temp2983.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2984= yyls; struct Cyc_Yyltype*
_temp2986= _temp2984.curr + yylsp_offset; if( _temp2984.base == 0? 1:( _temp2986
< _temp2984.base? 1: _temp2986 >= _temp2984.last_plus_one)){ _throw(
Null_Exception);}* _temp2986;}).first_line,({ struct _tagged_ptr3 _temp2987=
yyls; struct Cyc_Yyltype* _temp2989= _temp2987.curr + yylsp_offset; if(
_temp2987.base == 0? 1:( _temp2989 < _temp2987.base? 1: _temp2989 >= _temp2987.last_plus_one)){
_throw( Null_Exception);}* _temp2989;}).last_line)); _temp2983;}); _temp2982;});
break; case 58: _LL2981: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2991=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2991[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2992; _temp2992.tag=
Cyc_TypeSpecifier_tok_tag; _temp2992.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp2993=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2993[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp2994; _temp2994.tag=
Cyc_Parse_Signed_spec_tag; _temp2994.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2995= yyls; struct Cyc_Yyltype* _temp2997= _temp2995.curr +
yylsp_offset; if( _temp2995.base == 0? 1:( _temp2997 < _temp2995.base? 1:
_temp2997 >= _temp2995.last_plus_one)){ _throw( Null_Exception);}* _temp2997;}).first_line,({
struct _tagged_ptr3 _temp2998= yyls; struct Cyc_Yyltype* _temp3000= _temp2998.curr
+ yylsp_offset; if( _temp2998.base == 0? 1:( _temp3000 < _temp2998.base? 1:
_temp3000 >= _temp2998.last_plus_one)){ _throw( Null_Exception);}* _temp3000;}).last_line);
_temp2994;}); _temp2993;})); _temp2992;}); _temp2991;}); break; case 59: _LL2990:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3002=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3002[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3003; _temp3003.tag=
Cyc_TypeSpecifier_tok_tag; _temp3003.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp3004=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp3004[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp3005; _temp3005.tag= Cyc_Parse_Unsigned_spec_tag; _temp3005.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3006= yyls; struct Cyc_Yyltype* _temp3008= _temp3006.curr
+ yylsp_offset; if( _temp3006.base == 0? 1:( _temp3008 < _temp3006.base? 1:
_temp3008 >= _temp3006.last_plus_one)){ _throw( Null_Exception);}* _temp3008;}).first_line,({
struct _tagged_ptr3 _temp3009= yyls; struct Cyc_Yyltype* _temp3011= _temp3009.curr
+ yylsp_offset; if( _temp3009.base == 0? 1:( _temp3011 < _temp3009.base? 1:
_temp3011 >= _temp3009.last_plus_one)){ _throw( Null_Exception);}* _temp3011;}).last_line);
_temp3005;}); _temp3004;})); _temp3003;}); _temp3002;}); break; case 60: _LL3001:
yyval=({ struct _tagged_ptr2 _temp3013= yyvs; struct _xtunion_struct** _temp3015=
_temp3013.curr + yyvsp_offset; if( _temp3013.base == 0? 1:( _temp3015 <
_temp3013.base? 1: _temp3015 >= _temp3013.last_plus_one)){ _throw(
Null_Exception);}* _temp3015;}); break; case 61: _LL3012: yyval=({ struct
_tagged_ptr2 _temp3017= yyvs; struct _xtunion_struct** _temp3019= _temp3017.curr
+ yyvsp_offset; if( _temp3017.base == 0? 1:( _temp3019 < _temp3017.base? 1:
_temp3019 >= _temp3017.last_plus_one)){ _throw( Null_Exception);}* _temp3019;});
break; case 62: _LL3016: yyval=({ struct _tagged_ptr2 _temp3021= yyvs; struct
_xtunion_struct** _temp3023= _temp3021.curr + yyvsp_offset; if( _temp3021.base
== 0? 1:( _temp3023 < _temp3021.base? 1: _temp3023 >= _temp3021.last_plus_one)){
_throw( Null_Exception);}* _temp3023;}); break; case 63: _LL3020: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3025=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3025[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3026; _temp3026.tag= Cyc_TypeSpecifier_tok_tag;
_temp3026.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3027= yyvs; struct _xtunion_struct** _temp3029=
_temp3027.curr + yyvsp_offset; if( _temp3027.base == 0? 1:( _temp3029 <
_temp3027.base? 1: _temp3029 >= _temp3027.last_plus_one)){ _throw(
Null_Exception);}* _temp3029;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3030= yyls; struct Cyc_Yyltype*
_temp3032= _temp3030.curr + yylsp_offset; if( _temp3030.base == 0? 1:( _temp3032
< _temp3030.base? 1: _temp3032 >= _temp3030.last_plus_one)){ _throw(
Null_Exception);}* _temp3032;}).first_line,({ struct _tagged_ptr3 _temp3033=
yyls; struct Cyc_Yyltype* _temp3035= _temp3033.curr + yylsp_offset; if(
_temp3033.base == 0? 1:( _temp3035 < _temp3033.base? 1: _temp3035 >= _temp3033.last_plus_one)){
_throw( Null_Exception);}* _temp3035;}).last_line)); _temp3026;}); _temp3025;});
break; case 64: _LL3024: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3037=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3037[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3038; _temp3038.tag=
Cyc_TypeSpecifier_tok_tag; _temp3038.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3039= yyvs; struct
_xtunion_struct** _temp3041= _temp3039.curr +( yyvsp_offset - 2); if( _temp3039.base
== 0? 1:( _temp3041 < _temp3039.base? 1: _temp3041 >= _temp3039.last_plus_one)){
_throw( Null_Exception);}* _temp3041;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp3042= yyvs;
struct _xtunion_struct** _temp3044= _temp3042.curr + yyvsp_offset; if( _temp3042.base
== 0? 1:( _temp3044 < _temp3042.base? 1: _temp3044 >= _temp3042.last_plus_one)){
_throw( Null_Exception);}* _temp3044;})))), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3045= yyls; struct Cyc_Yyltype* _temp3047= _temp3045.curr
+( yylsp_offset - 2); if( _temp3045.base == 0? 1:( _temp3047 < _temp3045.base? 1:
_temp3047 >= _temp3045.last_plus_one)){ _throw( Null_Exception);}* _temp3047;}).first_line,({
struct _tagged_ptr3 _temp3048= yyls; struct Cyc_Yyltype* _temp3050= _temp3048.curr
+ yylsp_offset; if( _temp3048.base == 0? 1:( _temp3050 < _temp3048.base? 1:
_temp3050 >= _temp3048.last_plus_one)){ _throw( Null_Exception);}* _temp3050;}).last_line));
_temp3038;}); _temp3037;}); break; case 65: _LL3036: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3052=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3052[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3053; _temp3053.tag= Cyc_TypeSpecifier_tok_tag;
_temp3053.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TypedefType_struct*
_temp3054=( struct Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp3054[ 0]=({ struct Cyc_Absyn_TypedefType_struct _temp3055; _temp3055.tag=
Cyc_Absyn_TypedefType_tag; _temp3055.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3056= yyvs; struct _xtunion_struct** _temp3058= _temp3056.curr
+( yyvsp_offset - 1); if( _temp3056.base == 0? 1:( _temp3058 < _temp3056.base? 1:
_temp3058 >= _temp3056.last_plus_one)){ _throw( Null_Exception);}* _temp3058;}));
_temp3055.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3059= yyvs;
struct _xtunion_struct** _temp3061= _temp3059.curr + yyvsp_offset; if( _temp3059.base
== 0? 1:( _temp3061 < _temp3059.base? 1: _temp3061 >= _temp3059.last_plus_one)){
_throw( Null_Exception);}* _temp3061;})); _temp3055.f3= 0; _temp3055;});
_temp3054;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3062= yyls;
struct Cyc_Yyltype* _temp3064= _temp3062.curr +( yylsp_offset - 1); if(
_temp3062.base == 0? 1:( _temp3064 < _temp3062.base? 1: _temp3064 >= _temp3062.last_plus_one)){
_throw( Null_Exception);}* _temp3064;}).first_line,({ struct _tagged_ptr3
_temp3065= yyls; struct Cyc_Yyltype* _temp3067= _temp3065.curr + yylsp_offset;
if( _temp3065.base == 0? 1:( _temp3067 < _temp3065.base? 1: _temp3067 >=
_temp3065.last_plus_one)){ _throw( Null_Exception);}* _temp3067;}).last_line));
_temp3053;}); _temp3052;}); break; case 66: _LL3051: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3069=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3069[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3070; _temp3070.tag= Cyc_TypeSpecifier_tok_tag;
_temp3070.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp3071=( struct Cyc_Absyn_TupleType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp3071[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp3072; _temp3072.tag= Cyc_Absyn_TupleType_tag;
_temp3072.f1=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3073= yyls; struct Cyc_Yyltype* _temp3075= _temp3073.curr +(
yylsp_offset - 1); if( _temp3073.base == 0? 1:( _temp3075 < _temp3073.base? 1:
_temp3075 >= _temp3073.last_plus_one)){ _throw( Null_Exception);}* _temp3075;}).first_line,({
struct _tagged_ptr3 _temp3076= yyls; struct Cyc_Yyltype* _temp3078= _temp3076.curr
+( yylsp_offset - 1); if( _temp3076.base == 0? 1:( _temp3078 < _temp3076.base? 1:
_temp3078 >= _temp3076.last_plus_one)){ _throw( Null_Exception);}* _temp3078;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3079= yyvs; struct _xtunion_struct** _temp3081=
_temp3079.curr +( yyvsp_offset - 1); if( _temp3079.base == 0? 1:( _temp3081 <
_temp3079.base? 1: _temp3081 >= _temp3079.last_plus_one)){ _throw(
Null_Exception);}* _temp3081;})))); _temp3072;}); _temp3071;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3082= yyls; struct Cyc_Yyltype* _temp3084= _temp3082.curr
+( yylsp_offset - 3); if( _temp3082.base == 0? 1:( _temp3084 < _temp3082.base? 1:
_temp3084 >= _temp3082.last_plus_one)){ _throw( Null_Exception);}* _temp3084;}).first_line,({
struct _tagged_ptr3 _temp3085= yyls; struct Cyc_Yyltype* _temp3087= _temp3085.curr
+ yylsp_offset; if( _temp3085.base == 0? 1:( _temp3087 < _temp3085.base? 1:
_temp3087 >= _temp3085.last_plus_one)){ _throw( Null_Exception);}* _temp3087;}).last_line));
_temp3070;}); _temp3069;}); break; case 67: _LL3068: yyval=( struct
_xtunion_struct*)({ struct Cyc_Kind_tok_struct* _temp3089=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp3089[ 0]=({ struct Cyc_Kind_tok_struct
_temp3090; _temp3090.tag= Cyc_Kind_tok_tag; _temp3090.f1=( void*) Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3091= yyvs; struct
_xtunion_struct** _temp3093= _temp3091.curr + yyvsp_offset; if( _temp3091.base
== 0? 1:( _temp3093 < _temp3091.base? 1: _temp3093 >= _temp3091.last_plus_one)){
_throw( Null_Exception);}* _temp3093;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3094= yyls; struct Cyc_Yyltype* _temp3096= _temp3094.curr +
yylsp_offset; if( _temp3094.base == 0? 1:( _temp3096 < _temp3094.base? 1:
_temp3096 >= _temp3094.last_plus_one)){ _throw( Null_Exception);}* _temp3096;}).first_line,({
struct _tagged_ptr3 _temp3097= yyls; struct Cyc_Yyltype* _temp3099= _temp3097.curr
+ yylsp_offset; if( _temp3097.base == 0? 1:( _temp3099 < _temp3097.base? 1:
_temp3099 >= _temp3097.last_plus_one)){ _throw( Null_Exception);}* _temp3099;}).last_line));
_temp3090;}); _temp3089;}); break; case 68: _LL3088: { struct _tagged_string*
_temp3106; void* _temp3108; struct _tuple1 _temp3104=* Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3101= yyvs; struct _xtunion_struct** _temp3103=
_temp3101.curr + yyvsp_offset; if( _temp3101.base == 0? 1:( _temp3103 <
_temp3101.base? 1: _temp3103 >= _temp3101.last_plus_one)){ _throw(
Null_Exception);}* _temp3103;})); _LL3109: _temp3108= _temp3104.f1; goto _LL3107;
_LL3107: _temp3106= _temp3104.f2; goto _LL3105; _LL3105: if( _temp3108 !=( void*)
Cyc_Absyn_Loc_n){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp3110=(
char*)"bad kind in type specifier"; struct _tagged_string _temp3111; _temp3111.curr=
_temp3110; _temp3111.base= _temp3110; _temp3111.last_plus_one= _temp3110 + 27;
_temp3111;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3112= yyls;
struct Cyc_Yyltype* _temp3114= _temp3112.curr + yylsp_offset; if( _temp3112.base
== 0? 1:( _temp3114 < _temp3112.base? 1: _temp3114 >= _temp3112.last_plus_one)){
_throw( Null_Exception);}* _temp3114;}).first_line,({ struct _tagged_ptr3
_temp3115= yyls; struct Cyc_Yyltype* _temp3117= _temp3115.curr + yylsp_offset;
if( _temp3115.base == 0? 1:( _temp3117 < _temp3115.base? 1: _temp3117 >=
_temp3115.last_plus_one)){ _throw( Null_Exception);}* _temp3117;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_Kind_tok_struct* _temp3118=(
struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp3118[ 0]=({ struct Cyc_Kind_tok_struct _temp3119; _temp3119.tag= Cyc_Kind_tok_tag;
_temp3119.f1=( void*) Cyc_Parse_id_to_kind(* _temp3106, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3120= yyls; struct Cyc_Yyltype* _temp3122= _temp3120.curr
+ yylsp_offset; if( _temp3120.base == 0? 1:( _temp3122 < _temp3120.base? 1:
_temp3122 >= _temp3120.last_plus_one)){ _throw( Null_Exception);}* _temp3122;}).first_line,({
struct _tagged_ptr3 _temp3123= yyls; struct Cyc_Yyltype* _temp3125= _temp3123.curr
+ yylsp_offset; if( _temp3123.base == 0? 1:( _temp3125 < _temp3123.base? 1:
_temp3125 >= _temp3123.last_plus_one)){ _throw( Null_Exception);}* _temp3125;}).last_line));
_temp3119;}); _temp3118;}); break;} case 69: _LL3100: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp3127=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3127[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3128; _temp3128.tag= Cyc_TypeQual_tok_tag; _temp3128.f1=({ struct Cyc_Absyn_Tqual*
_temp3129=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp3129->q_const= 1; _temp3129->q_volatile= 0; _temp3129->q_restrict= 0;
_temp3129;}); _temp3128;}); _temp3127;}); break; case 70: _LL3126: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp3131=( struct
Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp3131[ 0]=({ struct Cyc_TypeQual_tok_struct _temp3132; _temp3132.tag= Cyc_TypeQual_tok_tag;
_temp3132.f1=({ struct Cyc_Absyn_Tqual* _temp3133=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp3133->q_const= 0;
_temp3133->q_volatile= 1; _temp3133->q_restrict= 0; _temp3133;}); _temp3132;});
_temp3131;}); break; case 71: _LL3130: yyval=( struct _xtunion_struct*)({ struct
Cyc_TypeQual_tok_struct* _temp3135=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct)); _temp3135[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3136; _temp3136.tag= Cyc_TypeQual_tok_tag; _temp3136.f1=({ struct Cyc_Absyn_Tqual*
_temp3137=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp3137->q_const= 0; _temp3137->q_volatile= 0; _temp3137->q_restrict= 1;
_temp3137;}); _temp3136;}); _temp3135;}); break; case 72: _LL3134: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3139=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3139[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3140; _temp3140.tag=
Cyc_TypeSpecifier_tok_tag; _temp3140.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3141=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3141[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3142; _temp3142.tag= Cyc_Parse_Decl_spec_tag;
_temp3142.f1=({ struct Cyc_Absyn_Decl* _temp3143=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp3143->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp3150=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp3150[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp3151; _temp3151.tag= Cyc_Absyn_Enum_d_tag; _temp3151.f1=({ struct Cyc_Absyn_Enumdecl*
_temp3152=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp3152->sc=( void*)(( void*) Cyc_Absyn_Public); _temp3152->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3156= yyvs; struct _xtunion_struct** _temp3158=
_temp3156.curr +( yyvsp_offset - 3); if( _temp3156.base == 0? 1:( _temp3158 <
_temp3156.base? 1: _temp3158 >= _temp3156.last_plus_one)){ _throw(
Null_Exception);}* _temp3158;})); _temp3152->fields= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3153= yyvs; struct _xtunion_struct** _temp3155=
_temp3153.curr +( yyvsp_offset - 1); if( _temp3153.base == 0? 1:( _temp3155 <
_temp3153.base? 1: _temp3155 >= _temp3153.last_plus_one)){ _throw(
Null_Exception);}* _temp3155;})); _temp3152;}); _temp3151;}); _temp3150;}));
_temp3143->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3144=
yyls; struct Cyc_Yyltype* _temp3146= _temp3144.curr +( yylsp_offset - 4); if(
_temp3144.base == 0? 1:( _temp3146 < _temp3144.base? 1: _temp3146 >= _temp3144.last_plus_one)){
_throw( Null_Exception);}* _temp3146;}).first_line,({ struct _tagged_ptr3
_temp3147= yyls; struct Cyc_Yyltype* _temp3149= _temp3147.curr + yylsp_offset;
if( _temp3147.base == 0? 1:( _temp3149 < _temp3147.base? 1: _temp3149 >=
_temp3147.last_plus_one)){ _throw( Null_Exception);}* _temp3149;}).last_line);
_temp3143;}); _temp3142;}); _temp3141;})); _temp3140;}); _temp3139;}); break;
case 73: _LL3138: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3160=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3160[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3161; _temp3161.tag=
Cyc_TypeSpecifier_tok_tag; _temp3161.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp3162=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3162[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp3163; _temp3163.tag= Cyc_Absyn_EnumType_tag;
_temp3163.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3164= yyvs;
struct _xtunion_struct** _temp3166= _temp3164.curr + yyvsp_offset; if( _temp3164.base
== 0? 1:( _temp3166 < _temp3164.base? 1: _temp3166 >= _temp3164.last_plus_one)){
_throw( Null_Exception);}* _temp3166;})); _temp3163.f2= 0; _temp3163;});
_temp3162;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3167= yyls;
struct Cyc_Yyltype* _temp3169= _temp3167.curr +( yylsp_offset - 1); if(
_temp3167.base == 0? 1:( _temp3169 < _temp3167.base? 1: _temp3169 >= _temp3167.last_plus_one)){
_throw( Null_Exception);}* _temp3169;}).first_line,({ struct _tagged_ptr3
_temp3170= yyls; struct Cyc_Yyltype* _temp3172= _temp3170.curr + yylsp_offset;
if( _temp3170.base == 0? 1:( _temp3172 < _temp3170.base? 1: _temp3172 >=
_temp3170.last_plus_one)){ _throw( Null_Exception);}* _temp3172;}).last_line));
_temp3161;}); _temp3160;}); break; case 74: _LL3159: yyval=( struct
_xtunion_struct*)({ struct Cyc_Enumfield_tok_struct* _temp3174=( struct Cyc_Enumfield_tok_struct*)
GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp3174[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp3175; _temp3175.tag= Cyc_Enumfield_tok_tag;
_temp3175.f1=({ struct Cyc_Absyn_Enumfield* _temp3176=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3176->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3183= yyvs; struct _xtunion_struct** _temp3185=
_temp3183.curr + yyvsp_offset; if( _temp3183.base == 0? 1:( _temp3185 <
_temp3183.base? 1: _temp3185 >= _temp3183.last_plus_one)){ _throw(
Null_Exception);}* _temp3185;})); _temp3176->tag= 0; _temp3176->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3177= yyls; struct Cyc_Yyltype* _temp3179= _temp3177.curr
+ yylsp_offset; if( _temp3177.base == 0? 1:( _temp3179 < _temp3177.base? 1:
_temp3179 >= _temp3177.last_plus_one)){ _throw( Null_Exception);}* _temp3179;}).first_line,({
struct _tagged_ptr3 _temp3180= yyls; struct Cyc_Yyltype* _temp3182= _temp3180.curr
+ yylsp_offset; if( _temp3180.base == 0? 1:( _temp3182 < _temp3180.base? 1:
_temp3182 >= _temp3180.last_plus_one)){ _throw( Null_Exception);}* _temp3182;}).last_line);
_temp3176;}); _temp3175;}); _temp3174;}); break; case 75: _LL3173: yyval=(
struct _xtunion_struct*)({ struct Cyc_Enumfield_tok_struct* _temp3187=( struct
Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp3187[ 0]=({ struct Cyc_Enumfield_tok_struct _temp3188; _temp3188.tag= Cyc_Enumfield_tok_tag;
_temp3188.f1=({ struct Cyc_Absyn_Enumfield* _temp3189=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3189->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3199= yyvs; struct _xtunion_struct** _temp3201=
_temp3199.curr +( yyvsp_offset - 2); if( _temp3199.base == 0? 1:( _temp3201 <
_temp3199.base? 1: _temp3201 >= _temp3199.last_plus_one)){ _throw(
Null_Exception);}* _temp3201;})); _temp3189->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3196= yyvs; struct _xtunion_struct** _temp3198=
_temp3196.curr + yyvsp_offset; if( _temp3196.base == 0? 1:( _temp3198 <
_temp3196.base? 1: _temp3198 >= _temp3196.last_plus_one)){ _throw(
Null_Exception);}* _temp3198;})); _temp3189->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3190= yyls; struct Cyc_Yyltype* _temp3192= _temp3190.curr
+( yylsp_offset - 2); if( _temp3190.base == 0? 1:( _temp3192 < _temp3190.base? 1:
_temp3192 >= _temp3190.last_plus_one)){ _throw( Null_Exception);}* _temp3192;}).first_line,({
struct _tagged_ptr3 _temp3193= yyls; struct Cyc_Yyltype* _temp3195= _temp3193.curr
+ yylsp_offset; if( _temp3193.base == 0? 1:( _temp3195 < _temp3193.base? 1:
_temp3195 >= _temp3193.last_plus_one)){ _throw( Null_Exception);}* _temp3195;}).last_line);
_temp3189;}); _temp3188;}); _temp3187;}); break; case 76: _LL3186: yyval=(
struct _xtunion_struct*)({ struct Cyc_EnumfieldList_tok_struct* _temp3203=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp3203[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp3204; _temp3204.tag=
Cyc_EnumfieldList_tok_tag; _temp3204.f1=({ struct Cyc_List_List* _temp3205=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3205->hd=(
void*) Cyc_yyget_Enumfield_tok(({ struct _tagged_ptr2 _temp3206= yyvs; struct
_xtunion_struct** _temp3208= _temp3206.curr + yyvsp_offset; if( _temp3206.base
== 0? 1:( _temp3208 < _temp3206.base? 1: _temp3208 >= _temp3206.last_plus_one)){
_throw( Null_Exception);}* _temp3208;})); _temp3205->tl= 0; _temp3205;});
_temp3204;}); _temp3203;}); break; case 77: _LL3202: yyval=( struct
_xtunion_struct*)({ struct Cyc_EnumfieldList_tok_struct* _temp3210=( struct Cyc_EnumfieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct)); _temp3210[ 0]=({
struct Cyc_EnumfieldList_tok_struct _temp3211; _temp3211.tag= Cyc_EnumfieldList_tok_tag;
_temp3211.f1=({ struct Cyc_List_List* _temp3212=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3212->hd=( void*) Cyc_yyget_Enumfield_tok(({
struct _tagged_ptr2 _temp3216= yyvs; struct _xtunion_struct** _temp3218=
_temp3216.curr +( yyvsp_offset - 2); if( _temp3216.base == 0? 1:( _temp3218 <
_temp3216.base? 1: _temp3218 >= _temp3216.last_plus_one)){ _throw(
Null_Exception);}* _temp3218;})); _temp3212->tl= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3213= yyvs; struct _xtunion_struct** _temp3215=
_temp3213.curr + yyvsp_offset; if( _temp3213.base == 0? 1:( _temp3215 <
_temp3213.base? 1: _temp3215 >= _temp3213.last_plus_one)){ _throw(
Null_Exception);}* _temp3215;})); _temp3212;}); _temp3211;}); _temp3210;});
break; case 78: _LL3209: { struct Cyc_Absyn_Decl* d;{ void* _temp3223= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp3220= yyvs; struct _xtunion_struct** _temp3222=
_temp3220.curr +( yyvsp_offset - 3); if( _temp3220.base == 0? 1:( _temp3222 <
_temp3220.base? 1: _temp3222 >= _temp3220.last_plus_one)){ _throw(
Null_Exception);}* _temp3222;})); _LL3225: if(( int) _temp3223 == Cyc_Parse_Struct_su){
goto _LL3226;} else{ goto _LL3227;} _LL3227: if(( int) _temp3223 == Cyc_Parse_Union_su){
goto _LL3228;} else{ goto _LL3224;} _LL3226: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public, 0, 0,({ struct Cyc_Core_Opt* _temp3229=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3229->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3230= yyvs; struct _xtunion_struct** _temp3232=
_temp3230.curr +( yyvsp_offset - 1); if( _temp3230.base == 0? 1:( _temp3232 <
_temp3230.base? 1: _temp3232 >= _temp3230.last_plus_one)){ _throw(
Null_Exception);}* _temp3232;})); _temp3229;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3233= yyls; struct Cyc_Yyltype* _temp3235= _temp3233.curr
+( yylsp_offset - 3); if( _temp3233.base == 0? 1:( _temp3235 < _temp3233.base? 1:
_temp3235 >= _temp3233.last_plus_one)){ _throw( Null_Exception);}* _temp3235;}).first_line,({
struct _tagged_ptr3 _temp3236= yyls; struct Cyc_Yyltype* _temp3238= _temp3236.curr
+ yylsp_offset; if( _temp3236.base == 0? 1:( _temp3238 < _temp3236.base? 1:
_temp3238 >= _temp3236.last_plus_one)){ _throw( Null_Exception);}* _temp3238;}).last_line));
goto _LL3224; _LL3228: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public, 0, 0,({
struct Cyc_Core_Opt* _temp3239=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3239->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3240= yyvs; struct _xtunion_struct** _temp3242=
_temp3240.curr +( yyvsp_offset - 1); if( _temp3240.base == 0? 1:( _temp3242 <
_temp3240.base? 1: _temp3242 >= _temp3240.last_plus_one)){ _throw(
Null_Exception);}* _temp3242;})); _temp3239;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3243= yyls; struct Cyc_Yyltype* _temp3245= _temp3243.curr
+( yylsp_offset - 3); if( _temp3243.base == 0? 1:( _temp3245 < _temp3243.base? 1:
_temp3245 >= _temp3243.last_plus_one)){ _throw( Null_Exception);}* _temp3245;}).first_line,({
struct _tagged_ptr3 _temp3246= yyls; struct Cyc_Yyltype* _temp3248= _temp3246.curr
+ yylsp_offset; if( _temp3246.base == 0? 1:( _temp3248 < _temp3246.base? 1:
_temp3248 >= _temp3246.last_plus_one)){ _throw( Null_Exception);}* _temp3248;}).last_line));
goto _LL3224; _LL3224:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3249=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3249[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3250; _temp3250.tag=
Cyc_TypeSpecifier_tok_tag; _temp3250.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3251=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3251[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3252; _temp3252.tag= Cyc_Parse_Decl_spec_tag;
_temp3252.f1= d; _temp3252;}); _temp3251;})); _temp3250;}); _temp3249;}); Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp3253=( char*)"anonymous structs/unions";
struct _tagged_string _temp3254; _temp3254.curr= _temp3253; _temp3254.base=
_temp3253; _temp3254.last_plus_one= _temp3253 + 25; _temp3254;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3255= yyls; struct Cyc_Yyltype* _temp3257= _temp3255.curr
+( yylsp_offset - 3); if( _temp3255.base == 0? 1:( _temp3257 < _temp3255.base? 1:
_temp3257 >= _temp3255.last_plus_one)){ _throw( Null_Exception);}* _temp3257;}).first_line,({
struct _tagged_ptr3 _temp3258= yyls; struct Cyc_Yyltype* _temp3260= _temp3258.curr
+ yylsp_offset; if( _temp3258.base == 0? 1:( _temp3260 < _temp3258.base? 1:
_temp3260 >= _temp3258.last_plus_one)){ _throw( Null_Exception);}* _temp3260;}).last_line));
break;} case 79: _LL3219: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3262= yyls; struct Cyc_Yyltype* _temp3264= _temp3262.curr
+( yylsp_offset - 3); if( _temp3262.base == 0? 1:( _temp3264 < _temp3262.base? 1:
_temp3264 >= _temp3262.last_plus_one)){ _throw( Null_Exception);}* _temp3264;}).first_line,({
struct _tagged_ptr3 _temp3265= yyls; struct Cyc_Yyltype* _temp3267= _temp3265.curr
+( yylsp_offset - 3); if( _temp3265.base == 0? 1:( _temp3267 < _temp3265.base? 1:
_temp3267 >= _temp3265.last_plus_one)){ _throw( Null_Exception);}* _temp3267;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3268= yyvs; struct
_xtunion_struct** _temp3270= _temp3268.curr +( yyvsp_offset - 3); if( _temp3268.base
== 0? 1:( _temp3270 < _temp3268.base? 1: _temp3270 >= _temp3268.last_plus_one)){
_throw( Null_Exception);}* _temp3270;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp3274= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3271= yyvs;
struct _xtunion_struct** _temp3273= _temp3271.curr +( yyvsp_offset - 5); if(
_temp3271.base == 0? 1:( _temp3273 < _temp3271.base? 1: _temp3273 >= _temp3271.last_plus_one)){
_throw( Null_Exception);}* _temp3273;})); _LL3276: if(( int) _temp3274 == Cyc_Parse_Struct_su){
goto _LL3277;} else{ goto _LL3278;} _LL3278: if(( int) _temp3274 == Cyc_Parse_Union_su){
goto _LL3279;} else{ goto _LL3275;} _LL3277: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3280=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3280->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3281= yyvs; struct _xtunion_struct** _temp3283=
_temp3281.curr +( yyvsp_offset - 4); if( _temp3281.base == 0? 1:( _temp3283 <
_temp3281.base? 1: _temp3283 >= _temp3281.last_plus_one)){ _throw(
Null_Exception);}* _temp3283;})); _temp3280;}), ts,({ struct Cyc_Core_Opt*
_temp3284=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3284->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3285= yyvs; struct _xtunion_struct** _temp3287= _temp3285.curr +(
yyvsp_offset - 1); if( _temp3285.base == 0? 1:( _temp3287 < _temp3285.base? 1:
_temp3287 >= _temp3285.last_plus_one)){ _throw( Null_Exception);}* _temp3287;}));
_temp3284;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3288=
yyls; struct Cyc_Yyltype* _temp3290= _temp3288.curr +( yylsp_offset - 5); if(
_temp3288.base == 0? 1:( _temp3290 < _temp3288.base? 1: _temp3290 >= _temp3288.last_plus_one)){
_throw( Null_Exception);}* _temp3290;}).first_line,({ struct _tagged_ptr3
_temp3291= yyls; struct Cyc_Yyltype* _temp3293= _temp3291.curr + yylsp_offset;
if( _temp3291.base == 0? 1:( _temp3293 < _temp3291.base? 1: _temp3293 >=
_temp3291.last_plus_one)){ _throw( Null_Exception);}* _temp3293;}).last_line));
goto _LL3275; _LL3279: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3294=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3294->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3295= yyvs; struct _xtunion_struct** _temp3297= _temp3295.curr
+( yyvsp_offset - 4); if( _temp3295.base == 0? 1:( _temp3297 < _temp3295.base? 1:
_temp3297 >= _temp3295.last_plus_one)){ _throw( Null_Exception);}* _temp3297;}));
_temp3294;}), ts,({ struct Cyc_Core_Opt* _temp3298=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3298->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3299= yyvs; struct _xtunion_struct** _temp3301=
_temp3299.curr +( yyvsp_offset - 1); if( _temp3299.base == 0? 1:( _temp3301 <
_temp3299.base? 1: _temp3301 >= _temp3299.last_plus_one)){ _throw(
Null_Exception);}* _temp3301;})); _temp3298;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3302= yyls; struct Cyc_Yyltype* _temp3304= _temp3302.curr
+( yylsp_offset - 5); if( _temp3302.base == 0? 1:( _temp3304 < _temp3302.base? 1:
_temp3304 >= _temp3302.last_plus_one)){ _throw( Null_Exception);}* _temp3304;}).first_line,({
struct _tagged_ptr3 _temp3305= yyls; struct Cyc_Yyltype* _temp3307= _temp3305.curr
+ yylsp_offset; if( _temp3305.base == 0? 1:( _temp3307 < _temp3305.base? 1:
_temp3307 >= _temp3305.last_plus_one)){ _throw( Null_Exception);}* _temp3307;}).last_line));
goto _LL3275; _LL3275:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3308=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3308[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3309; _temp3309.tag=
Cyc_TypeSpecifier_tok_tag; _temp3309.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3310=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3310[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3311; _temp3311.tag= Cyc_Parse_Decl_spec_tag;
_temp3311.f1= d; _temp3311;}); _temp3310;})); _temp3309;}); _temp3308;}); break;}
case 80: _LL3261: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3313= yyls; struct Cyc_Yyltype* _temp3315= _temp3313.curr
+( yylsp_offset - 3); if( _temp3313.base == 0? 1:( _temp3315 < _temp3313.base? 1:
_temp3315 >= _temp3313.last_plus_one)){ _throw( Null_Exception);}* _temp3315;}).first_line,({
struct _tagged_ptr3 _temp3316= yyls; struct Cyc_Yyltype* _temp3318= _temp3316.curr
+( yylsp_offset - 3); if( _temp3316.base == 0? 1:( _temp3318 < _temp3316.base? 1:
_temp3318 >= _temp3316.last_plus_one)){ _throw( Null_Exception);}* _temp3318;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3319= yyvs; struct
_xtunion_struct** _temp3321= _temp3319.curr +( yyvsp_offset - 3); if( _temp3319.base
== 0? 1:( _temp3321 < _temp3319.base? 1: _temp3321 >= _temp3319.last_plus_one)){
_throw( Null_Exception);}* _temp3321;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp3325= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3322= yyvs;
struct _xtunion_struct** _temp3324= _temp3322.curr +( yyvsp_offset - 5); if(
_temp3322.base == 0? 1:( _temp3324 < _temp3322.base? 1: _temp3324 >= _temp3322.last_plus_one)){
_throw( Null_Exception);}* _temp3324;})); _LL3327: if(( int) _temp3325 == Cyc_Parse_Struct_su){
goto _LL3328;} else{ goto _LL3329;} _LL3329: if(( int) _temp3325 == Cyc_Parse_Union_su){
goto _LL3330;} else{ goto _LL3326;} _LL3328: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3331=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3331->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3332= yyvs; struct _xtunion_struct** _temp3334=
_temp3332.curr +( yyvsp_offset - 4); if( _temp3332.base == 0? 1:( _temp3334 <
_temp3332.base? 1: _temp3334 >= _temp3332.last_plus_one)){ _throw(
Null_Exception);}* _temp3334;})); _temp3331;}), ts,({ struct Cyc_Core_Opt*
_temp3335=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3335->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3336= yyvs; struct _xtunion_struct** _temp3338= _temp3336.curr +(
yyvsp_offset - 1); if( _temp3336.base == 0? 1:( _temp3338 < _temp3336.base? 1:
_temp3338 >= _temp3336.last_plus_one)){ _throw( Null_Exception);}* _temp3338;}));
_temp3335;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3339=
yyls; struct Cyc_Yyltype* _temp3341= _temp3339.curr +( yylsp_offset - 5); if(
_temp3339.base == 0? 1:( _temp3341 < _temp3339.base? 1: _temp3341 >= _temp3339.last_plus_one)){
_throw( Null_Exception);}* _temp3341;}).first_line,({ struct _tagged_ptr3
_temp3342= yyls; struct Cyc_Yyltype* _temp3344= _temp3342.curr + yylsp_offset;
if( _temp3342.base == 0? 1:( _temp3344 < _temp3342.base? 1: _temp3344 >=
_temp3342.last_plus_one)){ _throw( Null_Exception);}* _temp3344;}).last_line));
goto _LL3326; _LL3330: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3345=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3345->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3346= yyvs; struct _xtunion_struct** _temp3348= _temp3346.curr
+( yyvsp_offset - 4); if( _temp3346.base == 0? 1:( _temp3348 < _temp3346.base? 1:
_temp3348 >= _temp3346.last_plus_one)){ _throw( Null_Exception);}* _temp3348;}));
_temp3345;}), ts,({ struct Cyc_Core_Opt* _temp3349=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3349->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3350= yyvs; struct _xtunion_struct** _temp3352=
_temp3350.curr +( yyvsp_offset - 1); if( _temp3350.base == 0? 1:( _temp3352 <
_temp3350.base? 1: _temp3352 >= _temp3350.last_plus_one)){ _throw(
Null_Exception);}* _temp3352;})); _temp3349;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3353= yyls; struct Cyc_Yyltype* _temp3355= _temp3353.curr
+( yylsp_offset - 5); if( _temp3353.base == 0? 1:( _temp3355 < _temp3353.base? 1:
_temp3355 >= _temp3353.last_plus_one)){ _throw( Null_Exception);}* _temp3355;}).first_line,({
struct _tagged_ptr3 _temp3356= yyls; struct Cyc_Yyltype* _temp3358= _temp3356.curr
+ yylsp_offset; if( _temp3356.base == 0? 1:( _temp3358 < _temp3356.base? 1:
_temp3358 >= _temp3356.last_plus_one)){ _throw( Null_Exception);}* _temp3358;}).last_line));
goto _LL3326; _LL3326:;} yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3359=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3359[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3360; _temp3360.tag=
Cyc_TypeSpecifier_tok_tag; _temp3360.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3361=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3361[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3362; _temp3362.tag= Cyc_Parse_Decl_spec_tag;
_temp3362.f1= d; _temp3362;}); _temp3361;})); _temp3360;}); _temp3359;}); break;}
case 81: _LL3312:{ void* _temp3367= Cyc_yyget_StructOrUnion_tok(({ struct
_tagged_ptr2 _temp3364= yyvs; struct _xtunion_struct** _temp3366= _temp3364.curr
+( yyvsp_offset - 2); if( _temp3364.base == 0? 1:( _temp3366 < _temp3364.base? 1:
_temp3366 >= _temp3364.last_plus_one)){ _throw( Null_Exception);}* _temp3366;}));
_LL3369: if(( int) _temp3367 == Cyc_Parse_Struct_su){ goto _LL3370;} else{ goto
_LL3371;} _LL3371: if(( int) _temp3367 == Cyc_Parse_Union_su){ goto _LL3372;}
else{ goto _LL3368;} _LL3370: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3373=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3373[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3374; _temp3374.tag=
Cyc_TypeSpecifier_tok_tag; _temp3374.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3375=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3375[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3376; _temp3376.tag= Cyc_Absyn_StructType_tag;
_temp3376.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3377= yyvs; struct _xtunion_struct** _temp3379= _temp3377.curr +(
yyvsp_offset - 1); if( _temp3377.base == 0? 1:( _temp3379 < _temp3377.base? 1:
_temp3379 >= _temp3377.last_plus_one)){ _throw( Null_Exception);}* _temp3379;}));
_temp3376.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3380= yyvs;
struct _xtunion_struct** _temp3382= _temp3380.curr + yyvsp_offset; if( _temp3380.base
== 0? 1:( _temp3382 < _temp3380.base? 1: _temp3382 >= _temp3380.last_plus_one)){
_throw( Null_Exception);}* _temp3382;})); _temp3376.f3= 0; _temp3376;});
_temp3375;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3383= yyls;
struct Cyc_Yyltype* _temp3385= _temp3383.curr +( yylsp_offset - 2); if(
_temp3383.base == 0? 1:( _temp3385 < _temp3383.base? 1: _temp3385 >= _temp3383.last_plus_one)){
_throw( Null_Exception);}* _temp3385;}).first_line,({ struct _tagged_ptr3
_temp3386= yyls; struct Cyc_Yyltype* _temp3388= _temp3386.curr + yylsp_offset;
if( _temp3386.base == 0? 1:( _temp3388 < _temp3386.base? 1: _temp3388 >=
_temp3386.last_plus_one)){ _throw( Null_Exception);}* _temp3388;}).last_line));
_temp3374;}); _temp3373;}); goto _LL3368; _LL3372: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3389=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3389[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3390; _temp3390.tag= Cyc_TypeSpecifier_tok_tag;
_temp3390.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp3391=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3391[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3392; _temp3392.tag= Cyc_Absyn_UnionType_tag;
_temp3392.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3393= yyvs; struct _xtunion_struct** _temp3395= _temp3393.curr +(
yyvsp_offset - 1); if( _temp3393.base == 0? 1:( _temp3395 < _temp3393.base? 1:
_temp3395 >= _temp3393.last_plus_one)){ _throw( Null_Exception);}* _temp3395;}));
_temp3392.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3396= yyvs;
struct _xtunion_struct** _temp3398= _temp3396.curr + yyvsp_offset; if( _temp3396.base
== 0? 1:( _temp3398 < _temp3396.base? 1: _temp3398 >= _temp3396.last_plus_one)){
_throw( Null_Exception);}* _temp3398;})); _temp3392.f3= 0; _temp3392;});
_temp3391;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3399= yyls;
struct Cyc_Yyltype* _temp3401= _temp3399.curr +( yylsp_offset - 2); if(
_temp3399.base == 0? 1:( _temp3401 < _temp3399.base? 1: _temp3401 >= _temp3399.last_plus_one)){
_throw( Null_Exception);}* _temp3401;}).first_line,({ struct _tagged_ptr3
_temp3402= yyls; struct Cyc_Yyltype* _temp3404= _temp3402.curr + yylsp_offset;
if( _temp3402.base == 0? 1:( _temp3404 < _temp3402.base? 1: _temp3404 >=
_temp3402.last_plus_one)){ _throw( Null_Exception);}* _temp3404;}).last_line));
_temp3390;}); _temp3389;}); goto _LL3368; _LL3368:;} break; case 82: _LL3363:{
void* _temp3409= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3406=
yyvs; struct _xtunion_struct** _temp3408= _temp3406.curr +( yyvsp_offset - 2);
if( _temp3406.base == 0? 1:( _temp3408 < _temp3406.base? 1: _temp3408 >=
_temp3406.last_plus_one)){ _throw( Null_Exception);}* _temp3408;})); _LL3411:
if(( int) _temp3409 == Cyc_Parse_Struct_su){ goto _LL3412;} else{ goto _LL3413;}
_LL3413: if(( int) _temp3409 == Cyc_Parse_Union_su){ goto _LL3414;} else{ goto
_LL3410;} _LL3412: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3415=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3415[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3416; _temp3416.tag=
Cyc_TypeSpecifier_tok_tag; _temp3416.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3417=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3417[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3418; _temp3418.tag= Cyc_Absyn_StructType_tag;
_temp3418.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3419= yyvs; struct _xtunion_struct** _temp3421= _temp3419.curr +(
yyvsp_offset - 1); if( _temp3419.base == 0? 1:( _temp3421 < _temp3419.base? 1:
_temp3421 >= _temp3419.last_plus_one)){ _throw( Null_Exception);}* _temp3421;}));
_temp3418.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3422= yyvs;
struct _xtunion_struct** _temp3424= _temp3422.curr + yyvsp_offset; if( _temp3422.base
== 0? 1:( _temp3424 < _temp3422.base? 1: _temp3424 >= _temp3422.last_plus_one)){
_throw( Null_Exception);}* _temp3424;})); _temp3418.f3= 0; _temp3418;});
_temp3417;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3425= yyls;
struct Cyc_Yyltype* _temp3427= _temp3425.curr +( yylsp_offset - 2); if(
_temp3425.base == 0? 1:( _temp3427 < _temp3425.base? 1: _temp3427 >= _temp3425.last_plus_one)){
_throw( Null_Exception);}* _temp3427;}).first_line,({ struct _tagged_ptr3
_temp3428= yyls; struct Cyc_Yyltype* _temp3430= _temp3428.curr + yylsp_offset;
if( _temp3428.base == 0? 1:( _temp3430 < _temp3428.base? 1: _temp3430 >=
_temp3428.last_plus_one)){ _throw( Null_Exception);}* _temp3430;}).last_line));
_temp3416;}); _temp3415;}); goto _LL3410; _LL3414: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3431=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3431[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3432; _temp3432.tag= Cyc_TypeSpecifier_tok_tag;
_temp3432.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp3433=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp3433[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp3434; _temp3434.tag= Cyc_Absyn_UnionType_tag;
_temp3434.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3435= yyvs; struct _xtunion_struct** _temp3437= _temp3435.curr +(
yyvsp_offset - 1); if( _temp3435.base == 0? 1:( _temp3437 < _temp3435.base? 1:
_temp3437 >= _temp3435.last_plus_one)){ _throw( Null_Exception);}* _temp3437;}));
_temp3434.f2= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3438= yyvs;
struct _xtunion_struct** _temp3440= _temp3438.curr + yyvsp_offset; if( _temp3438.base
== 0? 1:( _temp3440 < _temp3438.base? 1: _temp3440 >= _temp3438.last_plus_one)){
_throw( Null_Exception);}* _temp3440;})); _temp3434.f3= 0; _temp3434;});
_temp3433;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3441= yyls;
struct Cyc_Yyltype* _temp3443= _temp3441.curr +( yylsp_offset - 2); if(
_temp3441.base == 0? 1:( _temp3443 < _temp3441.base? 1: _temp3443 >= _temp3441.last_plus_one)){
_throw( Null_Exception);}* _temp3443;}).first_line,({ struct _tagged_ptr3
_temp3444= yyls; struct Cyc_Yyltype* _temp3446= _temp3444.curr + yylsp_offset;
if( _temp3444.base == 0? 1:( _temp3446 < _temp3444.base? 1: _temp3446 >=
_temp3444.last_plus_one)){ _throw( Null_Exception);}* _temp3446;}).last_line));
_temp3432;}); _temp3431;}); goto _LL3410; _LL3410:;} break; case 83: _LL3405:
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp3448=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3448[ 0]=({ struct Cyc_TypeList_tok_struct _temp3449; _temp3449.tag= Cyc_TypeList_tok_tag;
_temp3449.f1= 0; _temp3449;}); _temp3448;}); break; case 84: _LL3447: yyval=(
struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp3451=( struct
Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3451[ 0]=({ struct Cyc_TypeList_tok_struct _temp3452; _temp3452.tag= Cyc_TypeList_tok_tag;
_temp3452.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3453= yyvs; struct
_xtunion_struct** _temp3455= _temp3453.curr +( yyvsp_offset - 1); if( _temp3453.base
== 0? 1:( _temp3455 < _temp3453.base? 1: _temp3455 >= _temp3453.last_plus_one)){
_throw( Null_Exception);}* _temp3455;}))); _temp3452;}); _temp3451;}); break;
case 85: _LL3450:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp3457=
Cyc_Parse_lbuf; if( _temp3457 == 0){ _throw( Null_Exception);} _temp3457->v;}))->lex_curr_pos
-= 1; yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp3458=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3458[ 0]=({ struct Cyc_TypeList_tok_struct _temp3459; _temp3459.tag= Cyc_TypeList_tok_tag;
_temp3459.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3460= yyvs; struct
_xtunion_struct** _temp3462= _temp3460.curr +( yyvsp_offset - 1); if( _temp3460.base
== 0? 1:( _temp3462 < _temp3460.base? 1: _temp3462 >= _temp3460.last_plus_one)){
_throw( Null_Exception);}* _temp3462;}))); _temp3459;}); _temp3458;}); break;
case 86: _LL3456: yyval=( struct _xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3464=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3464[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3465; _temp3465.tag=
Cyc_StructOrUnion_tok_tag; _temp3465.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp3465;}); _temp3464;}); break; case 87: _LL3463: yyval=( struct
_xtunion_struct*)({ struct Cyc_StructOrUnion_tok_struct* _temp3467=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct)); _temp3467[ 0]=({
struct Cyc_StructOrUnion_tok_struct _temp3468; _temp3468.tag= Cyc_StructOrUnion_tok_tag;
_temp3468.f1=( void*)(( void*) Cyc_Parse_Union_su); _temp3468;}); _temp3467;});
break; case 88: _LL3466: yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3470=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3470[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3471; _temp3471.tag= Cyc_StructFieldDeclList_tok_tag; _temp3471.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3472= yyvs; struct _xtunion_struct** _temp3474=
_temp3472.curr + yyvsp_offset; if( _temp3472.base == 0? 1:( _temp3474 <
_temp3472.base? 1: _temp3474 >= _temp3472.last_plus_one)){ _throw(
Null_Exception);}* _temp3474;})))); _temp3471;}); _temp3470;}); break; case 89:
_LL3469: yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3476=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3476[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3477; _temp3477.tag= Cyc_StructFieldDeclListList_tok_tag; _temp3477.f1=({
struct Cyc_List_List* _temp3478=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3478->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3479= yyvs; struct _xtunion_struct** _temp3481=
_temp3479.curr + yyvsp_offset; if( _temp3479.base == 0? 1:( _temp3481 <
_temp3479.base? 1: _temp3481 >= _temp3479.last_plus_one)){ _throw(
Null_Exception);}* _temp3481;})); _temp3478->tl= 0; _temp3478;}); _temp3477;});
_temp3476;}); break; case 90: _LL3475: yyval=( struct _xtunion_struct*)({ struct
Cyc_StructFieldDeclListList_tok_struct* _temp3483=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp3483[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp3484; _temp3484.tag= Cyc_StructFieldDeclListList_tok_tag;
_temp3484.f1=({ struct Cyc_List_List* _temp3485=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3485->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3489= yyvs; struct _xtunion_struct** _temp3491=
_temp3489.curr + yyvsp_offset; if( _temp3489.base == 0? 1:( _temp3491 <
_temp3489.base? 1: _temp3491 >= _temp3489.last_plus_one)){ _throw(
Null_Exception);}* _temp3491;})); _temp3485->tl= Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3486= yyvs; struct _xtunion_struct** _temp3488=
_temp3486.curr +( yyvsp_offset - 1); if( _temp3486.base == 0? 1:( _temp3488 <
_temp3486.base? 1: _temp3488 >= _temp3486.last_plus_one)){ _throw(
Null_Exception);}* _temp3488;})); _temp3485;}); _temp3484;}); _temp3483;});
break; case 91: _LL3482: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct*
_temp3493=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3493[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3494; _temp3494.tag=
Cyc_InitDeclList_tok_tag; _temp3494.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2
_temp3495= yyvs; struct _xtunion_struct** _temp3497= _temp3495.curr +
yyvsp_offset; if( _temp3495.base == 0? 1:( _temp3497 < _temp3495.base? 1:
_temp3497 >= _temp3495.last_plus_one)){ _throw( Null_Exception);}* _temp3497;})));
_temp3494;}); _temp3493;}); break; case 92: _LL3492: yyval=( struct
_xtunion_struct*)({ struct Cyc_InitDeclList_tok_struct* _temp3499=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp3499[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp3500; _temp3500.tag= Cyc_InitDeclList_tok_tag;
_temp3500.f1=({ struct Cyc_List_List* _temp3501=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3501->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3502= yyvs; struct _xtunion_struct** _temp3504=
_temp3502.curr + yyvsp_offset; if( _temp3502.base == 0? 1:( _temp3504 <
_temp3502.base? 1: _temp3504 >= _temp3502.last_plus_one)){ _throw(
Null_Exception);}* _temp3504;})); _temp3501->tl= 0; _temp3501;}); _temp3500;});
_temp3499;}); break; case 93: _LL3498: yyval=( struct _xtunion_struct*)({ struct
Cyc_InitDeclList_tok_struct* _temp3506=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct)); _temp3506[ 0]=({ struct
Cyc_InitDeclList_tok_struct _temp3507; _temp3507.tag= Cyc_InitDeclList_tok_tag;
_temp3507.f1=({ struct Cyc_List_List* _temp3508=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3508->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3512= yyvs; struct _xtunion_struct** _temp3514=
_temp3512.curr + yyvsp_offset; if( _temp3512.base == 0? 1:( _temp3514 <
_temp3512.base? 1: _temp3514 >= _temp3512.last_plus_one)){ _throw(
Null_Exception);}* _temp3514;})); _temp3508->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3509= yyvs; struct _xtunion_struct** _temp3511=
_temp3509.curr +( yyvsp_offset - 2); if( _temp3509.base == 0? 1:( _temp3511 <
_temp3509.base? 1: _temp3511 >= _temp3509.last_plus_one)){ _throw(
Null_Exception);}* _temp3511;})); _temp3508;}); _temp3507;}); _temp3506;});
break; case 94: _LL3505: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDecl_tok_struct*
_temp3516=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3516[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3517; _temp3517.tag= Cyc_InitDecl_tok_tag;
_temp3517.f1=({ struct _tuple13* _temp3518=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3518->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3519= yyvs; struct _xtunion_struct** _temp3521= _temp3519.curr
+ yyvsp_offset; if( _temp3519.base == 0? 1:( _temp3521 < _temp3519.base? 1:
_temp3521 >= _temp3519.last_plus_one)){ _throw( Null_Exception);}* _temp3521;}));
_temp3518->f2= 0; _temp3518;}); _temp3517;}); _temp3516;}); break; case 95:
_LL3515: yyval=( struct _xtunion_struct*)({ struct Cyc_InitDecl_tok_struct*
_temp3523=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3523[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3524; _temp3524.tag= Cyc_InitDecl_tok_tag;
_temp3524.f1=({ struct _tuple13* _temp3525=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3525->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3529= yyvs; struct _xtunion_struct** _temp3531= _temp3529.curr
+( yyvsp_offset - 2); if( _temp3529.base == 0? 1:( _temp3531 < _temp3529.base? 1:
_temp3531 >= _temp3529.last_plus_one)){ _throw( Null_Exception);}* _temp3531;}));
_temp3525->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp3526= yyvs; struct _xtunion_struct** _temp3528= _temp3526.curr +
yyvsp_offset; if( _temp3526.base == 0? 1:( _temp3528 < _temp3526.base? 1:
_temp3528 >= _temp3526.last_plus_one)){ _throw( Null_Exception);}* _temp3528;}));
_temp3525;}); _temp3524;}); _temp3523;}); break; case 96: _LL3522: { struct Cyc_Absyn_Tqual*
tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3533= yyvs; struct
_xtunion_struct** _temp3535= _temp3533.curr +( yyvsp_offset - 2); if( _temp3533.base
== 0? 1:( _temp3535 < _temp3533.base? 1: _temp3535 >= _temp3533.last_plus_one)){
_throw( Null_Exception);}* _temp3535;}))).f1; struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3536= yyvs; struct _xtunion_struct** _temp3538=
_temp3536.curr +( yyvsp_offset - 2); if( _temp3536.base == 0? 1:( _temp3538 <
_temp3536.base? 1: _temp3538 >= _temp3536.last_plus_one)){ _throw(
Null_Exception);}* _temp3538;}))).f3; void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3539= yyvs; struct _xtunion_struct** _temp3541=
_temp3539.curr +( yyvsp_offset - 2); if( _temp3539.base == 0? 1:( _temp3541 <
_temp3539.base? 1: _temp3541 >= _temp3539.last_plus_one)){ _throw(
Null_Exception);}* _temp3541;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3542= yyls; struct Cyc_Yyltype* _temp3544= _temp3542.curr +(
yylsp_offset - 2); if( _temp3542.base == 0? 1:( _temp3544 < _temp3542.base? 1:
_temp3544 >= _temp3542.last_plus_one)){ _throw( Null_Exception);}* _temp3544;}).first_line,({
struct _tagged_ptr3 _temp3545= yyls; struct Cyc_Yyltype* _temp3547= _temp3545.curr
+( yylsp_offset - 2); if( _temp3545.base == 0? 1:( _temp3547 < _temp3545.base? 1:
_temp3547 >= _temp3545.last_plus_one)){ _throw( Null_Exception);}* _temp3547;}).last_line));
struct Cyc_List_List* _temp3553; struct Cyc_List_List* _temp3555; struct _tuple0
_temp3551=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3548= yyvs; struct _xtunion_struct** _temp3550=
_temp3548.curr +( yyvsp_offset - 1); if( _temp3548.base == 0? 1:( _temp3550 <
_temp3548.base? 1: _temp3550 >= _temp3548.last_plus_one)){ _throw(
Null_Exception);}* _temp3550;}))); _LL3556: _temp3555= _temp3551.f1; goto
_LL3554; _LL3554: _temp3553= _temp3551.f2; goto _LL3552; _LL3552: { struct Cyc_List_List*
info=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, t, _temp3555, atts), _temp3553);
yyval=( struct _xtunion_struct*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3557=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3557[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3558; _temp3558.tag= Cyc_StructFieldDeclList_tok_tag; _temp3558.f1=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple8*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3559= yyls; struct Cyc_Yyltype* _temp3561= _temp3559.curr
+( yylsp_offset - 2); if( _temp3559.base == 0? 1:( _temp3561 < _temp3559.base? 1:
_temp3561 >= _temp3559.last_plus_one)){ _throw( Null_Exception);}* _temp3561;}).first_line,({
struct _tagged_ptr3 _temp3562= yyls; struct Cyc_Yyltype* _temp3564= _temp3562.curr
+( yylsp_offset - 1); if( _temp3562.base == 0? 1:( _temp3564 < _temp3562.base? 1:
_temp3564 >= _temp3562.last_plus_one)){ _throw( Null_Exception);}* _temp3564;}).last_line),
info); _temp3558;}); _temp3557;}); break;}} case 97: _LL3532: yyval=( struct
_xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct* _temp3566=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3566[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3567; _temp3567.tag= Cyc_QualSpecList_tok_tag;
_temp3567.f1=({ struct _tuple14* _temp3568=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3568->f1= Cyc_Absyn_empty_tqual(); _temp3568->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp3572=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3572->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3573= yyvs; struct _xtunion_struct** _temp3575=
_temp3573.curr +( yyvsp_offset - 1); if( _temp3573.base == 0? 1:( _temp3575 <
_temp3573.base? 1: _temp3575 >= _temp3573.last_plus_one)){ _throw(
Null_Exception);}* _temp3575;})); _temp3572->tl= 0; _temp3572;}); _temp3568->f3=
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp3569= yyvs; struct
_xtunion_struct** _temp3571= _temp3569.curr + yyvsp_offset; if( _temp3569.base
== 0? 1:( _temp3571 < _temp3569.base? 1: _temp3571 >= _temp3569.last_plus_one)){
_throw( Null_Exception);}* _temp3571;})); _temp3568;}); _temp3567;}); _temp3566;});
break; case 98: _LL3565: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3577=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3577[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3578; _temp3578.tag=
Cyc_QualSpecList_tok_tag; _temp3578.f1=({ struct _tuple14* _temp3579=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3579->f1=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3593= yyvs; struct _xtunion_struct** _temp3595=
_temp3593.curr + yyvsp_offset; if( _temp3593.base == 0? 1:( _temp3595 <
_temp3593.base? 1: _temp3595 >= _temp3593.last_plus_one)){ _throw(
Null_Exception);}* _temp3595;}))).f1; _temp3579->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3586=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3586->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3590= yyvs; struct _xtunion_struct** _temp3592=
_temp3590.curr +( yyvsp_offset - 2); if( _temp3590.base == 0? 1:( _temp3592 <
_temp3590.base? 1: _temp3592 >= _temp3590.last_plus_one)){ _throw(
Null_Exception);}* _temp3592;})); _temp3586->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3587= yyvs; struct _xtunion_struct** _temp3589=
_temp3587.curr + yyvsp_offset; if( _temp3587.base == 0? 1:( _temp3589 <
_temp3587.base? 1: _temp3589 >= _temp3587.last_plus_one)){ _throw(
Null_Exception);}* _temp3589;}))).f2; _temp3586;}); _temp3579->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3580= yyvs; struct _xtunion_struct** _temp3582=
_temp3580.curr +( yyvsp_offset - 1); if( _temp3580.base == 0? 1:( _temp3582 <
_temp3580.base? 1: _temp3582 >= _temp3580.last_plus_one)){ _throw(
Null_Exception);}* _temp3582;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3583= yyvs; struct _xtunion_struct** _temp3585= _temp3583.curr
+ yyvsp_offset; if( _temp3583.base == 0? 1:( _temp3585 < _temp3583.base? 1:
_temp3585 >= _temp3583.last_plus_one)){ _throw( Null_Exception);}* _temp3585;}))).f3);
_temp3579;}); _temp3578;}); _temp3577;}); break; case 99: _LL3576: yyval=(
struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct* _temp3597=(
struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3597[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3598; _temp3598.tag=
Cyc_QualSpecList_tok_tag; _temp3598.f1=({ struct _tuple14* _temp3599=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3599->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3603= yyvs; struct _xtunion_struct** _temp3605=
_temp3603.curr +( yyvsp_offset - 1); if( _temp3603.base == 0? 1:( _temp3605 <
_temp3603.base? 1: _temp3605 >= _temp3603.last_plus_one)){ _throw(
Null_Exception);}* _temp3605;})); _temp3599->f2= 0; _temp3599->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3600= yyvs; struct _xtunion_struct** _temp3602=
_temp3600.curr + yyvsp_offset; if( _temp3600.base == 0? 1:( _temp3602 <
_temp3600.base? 1: _temp3602 >= _temp3600.last_plus_one)){ _throw(
Null_Exception);}* _temp3602;})); _temp3599;}); _temp3598;}); _temp3597;});
break; case 100: _LL3596: yyval=( struct _xtunion_struct*)({ struct Cyc_QualSpecList_tok_struct*
_temp3607=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3607[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3608; _temp3608.tag=
Cyc_QualSpecList_tok_tag; _temp3608.f1=({ struct _tuple14* _temp3609=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3609->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3619= yyvs; struct
_xtunion_struct** _temp3621= _temp3619.curr +( yyvsp_offset - 2); if( _temp3619.base
== 0? 1:( _temp3621 < _temp3619.base? 1: _temp3621 >= _temp3619.last_plus_one)){
_throw( Null_Exception);}* _temp3621;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3622= yyvs; struct _xtunion_struct** _temp3624= _temp3622.curr
+ yyvsp_offset; if( _temp3622.base == 0? 1:( _temp3624 < _temp3622.base? 1:
_temp3624 >= _temp3622.last_plus_one)){ _throw( Null_Exception);}* _temp3624;}))).f1);
_temp3609->f2=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3616=
yyvs; struct _xtunion_struct** _temp3618= _temp3616.curr + yyvsp_offset; if(
_temp3616.base == 0? 1:( _temp3618 < _temp3616.base? 1: _temp3618 >= _temp3616.last_plus_one)){
_throw( Null_Exception);}* _temp3618;}))).f2; _temp3609->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3610= yyvs; struct _xtunion_struct** _temp3612=
_temp3610.curr +( yyvsp_offset - 1); if( _temp3610.base == 0? 1:( _temp3612 <
_temp3610.base? 1: _temp3612 >= _temp3610.last_plus_one)){ _throw(
Null_Exception);}* _temp3612;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3613= yyvs; struct _xtunion_struct** _temp3615= _temp3613.curr
+ yyvsp_offset; if( _temp3613.base == 0? 1:( _temp3615 < _temp3613.base? 1:
_temp3615 >= _temp3613.last_plus_one)){ _throw( Null_Exception);}* _temp3615;}))).f3);
_temp3609;}); _temp3608;}); _temp3607;}); break; case 101: _LL3606: yyval=(
struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp3626=(
struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp3626[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp3627; _temp3627.tag=
Cyc_DeclaratorExpoptList_tok_tag; _temp3627.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3628= yyvs; struct _xtunion_struct** _temp3630=
_temp3628.curr + yyvsp_offset; if( _temp3628.base == 0? 1:( _temp3630 <
_temp3628.base? 1: _temp3630 >= _temp3628.last_plus_one)){ _throw(
Null_Exception);}* _temp3630;}))); _temp3627;}); _temp3626;}); break; case 102:
_LL3625: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp3632=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3632[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp3633; _temp3633.tag= Cyc_DeclaratorExpoptList_tok_tag; _temp3633.f1=({
struct Cyc_List_List* _temp3634=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3634->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3635= yyvs; struct _xtunion_struct** _temp3637=
_temp3635.curr + yyvsp_offset; if( _temp3635.base == 0? 1:( _temp3637 <
_temp3635.base? 1: _temp3637 >= _temp3635.last_plus_one)){ _throw(
Null_Exception);}* _temp3637;})); _temp3634->tl= 0; _temp3634;}); _temp3633;});
_temp3632;}); break; case 103: _LL3631: yyval=( struct _xtunion_struct*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp3639=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3639[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp3640; _temp3640.tag= Cyc_DeclaratorExpoptList_tok_tag;
_temp3640.f1=({ struct Cyc_List_List* _temp3641=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3641->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3645= yyvs; struct _xtunion_struct** _temp3647=
_temp3645.curr + yyvsp_offset; if( _temp3645.base == 0? 1:( _temp3647 <
_temp3645.base? 1: _temp3647 >= _temp3645.last_plus_one)){ _throw(
Null_Exception);}* _temp3647;})); _temp3641->tl= Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3642= yyvs; struct _xtunion_struct** _temp3644=
_temp3642.curr +( yyvsp_offset - 2); if( _temp3642.base == 0? 1:( _temp3644 <
_temp3642.base? 1: _temp3644 >= _temp3642.last_plus_one)){ _throw(
Null_Exception);}* _temp3644;})); _temp3641;}); _temp3640;}); _temp3639;});
break; case 104: _LL3638: yyval=( struct _xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp3649=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp3649[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp3650; _temp3650.tag= Cyc_DeclaratorExpopt_tok_tag; _temp3650.f1=({ struct
_tuple15* _temp3651=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15));
_temp3651->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3652= yyvs;
struct _xtunion_struct** _temp3654= _temp3652.curr + yyvsp_offset; if( _temp3652.base
== 0? 1:( _temp3654 < _temp3652.base? 1: _temp3654 >= _temp3652.last_plus_one)){
_throw( Null_Exception);}* _temp3654;})); _temp3651->f2= 0; _temp3651;});
_temp3650;}); _temp3649;}); break; case 105: _LL3648: yyval=( struct
_xtunion_struct*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3656=( struct
Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp3656[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp3657; _temp3657.tag=
Cyc_DeclaratorExpopt_tok_tag; _temp3657.f1=({ struct _tuple15* _temp3658=(
struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp3658->f1=({ struct
Cyc_Parse_Declarator* _temp3663=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3663->id=({ struct _tuple1*
_temp3664=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3664->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp3668=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp3668[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp3669; _temp3669.tag= Cyc_Absyn_Rel_n_tag; _temp3669.f1= 0; _temp3669;});
_temp3668;}); _temp3664->f2=({ struct _tagged_string* _temp3665=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3665[ 0]=(
struct _tagged_string)({ char* _temp3666=( char*)""; struct _tagged_string
_temp3667; _temp3667.curr= _temp3666; _temp3667.base= _temp3666; _temp3667.last_plus_one=
_temp3666 + 1; _temp3667;}); _temp3665;}); _temp3664;}); _temp3663->tms= 0;
_temp3663;}); _temp3658->f2=({ struct Cyc_Core_Opt* _temp3659=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3659->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3660= yyvs; struct _xtunion_struct** _temp3662=
_temp3660.curr + yyvsp_offset; if( _temp3660.base == 0? 1:( _temp3662 <
_temp3660.base? 1: _temp3662 >= _temp3660.last_plus_one)){ _throw(
Null_Exception);}* _temp3662;})); _temp3659;}); _temp3658;}); _temp3657;});
_temp3656;}); break; case 106: _LL3655: yyval=( struct _xtunion_struct*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp3671=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp3671[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp3672; _temp3672.tag= Cyc_DeclaratorExpopt_tok_tag;
_temp3672.f1=({ struct _tuple15* _temp3673=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp3673->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3678= yyvs; struct _xtunion_struct** _temp3680= _temp3678.curr
+( yyvsp_offset - 2); if( _temp3678.base == 0? 1:( _temp3680 < _temp3678.base? 1:
_temp3680 >= _temp3678.last_plus_one)){ _throw( Null_Exception);}* _temp3680;}));
_temp3673->f2=({ struct Cyc_Core_Opt* _temp3674=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3674->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3675= yyvs; struct _xtunion_struct** _temp3677=
_temp3675.curr + yyvsp_offset; if( _temp3675.base == 0? 1:( _temp3677 <
_temp3675.base? 1: _temp3677 >= _temp3675.last_plus_one)){ _throw(
Null_Exception);}* _temp3677;})); _temp3674;}); _temp3673;}); _temp3672;});
_temp3671;}); break; case 107: _LL3670: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3682= yyls; struct Cyc_Yyltype* _temp3684= _temp3682.curr
+( yylsp_offset - 3); if( _temp3682.base == 0? 1:( _temp3684 < _temp3682.base? 1:
_temp3684 >= _temp3682.last_plus_one)){ _throw( Null_Exception);}* _temp3684;}).first_line,({
struct _tagged_ptr3 _temp3685= yyls; struct Cyc_Yyltype* _temp3687= _temp3685.curr
+( yylsp_offset - 3); if( _temp3685.base == 0? 1:( _temp3687 < _temp3685.base? 1:
_temp3687 >= _temp3685.last_plus_one)){ _throw( Null_Exception);}* _temp3687;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3688= yyvs; struct
_xtunion_struct** _temp3690= _temp3688.curr +( yyvsp_offset - 3); if( _temp3688.base
== 0? 1:( _temp3690 < _temp3688.base? 1: _temp3690 >= _temp3688.last_plus_one)){
_throw( Null_Exception);}* _temp3690;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_TypeSpecifier_tok_struct* _temp3691=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3691[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3692; _temp3692.tag= Cyc_TypeSpecifier_tok_tag;
_temp3692.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3693=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3693[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3694; _temp3694.tag= Cyc_Parse_Decl_spec_tag;
_temp3694.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp3695=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3695->v=( void*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3696=
yyvs; struct _xtunion_struct** _temp3698= _temp3696.curr +( yyvsp_offset - 4);
if( _temp3696.base == 0? 1:( _temp3698 < _temp3696.base? 1: _temp3698 >=
_temp3696.last_plus_one)){ _throw( Null_Exception);}* _temp3698;})); _temp3695;}),
ts,({ struct Cyc_Core_Opt* _temp3699=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp3699->v=( void*) Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3700= yyvs; struct _xtunion_struct** _temp3702=
_temp3700.curr +( yyvsp_offset - 1); if( _temp3700.base == 0? 1:( _temp3702 <
_temp3700.base? 1: _temp3702 >= _temp3700.last_plus_one)){ _throw(
Null_Exception);}* _temp3702;})); _temp3699;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3703= yyls; struct Cyc_Yyltype* _temp3705= _temp3703.curr
+( yylsp_offset - 5); if( _temp3703.base == 0? 1:( _temp3705 < _temp3703.base? 1:
_temp3705 >= _temp3703.last_plus_one)){ _throw( Null_Exception);}* _temp3705;}).first_line,({
struct _tagged_ptr3 _temp3706= yyls; struct Cyc_Yyltype* _temp3708= _temp3706.curr
+ yylsp_offset; if( _temp3706.base == 0? 1:( _temp3708 < _temp3706.base? 1:
_temp3708 >= _temp3706.last_plus_one)){ _throw( Null_Exception);}* _temp3708;}).last_line));
_temp3694;}); _temp3693;})); _temp3692;}); _temp3691;}); break;} case 108:
_LL3681: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3710=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3710[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3711; _temp3711.tag=
Cyc_TypeSpecifier_tok_tag; _temp3711.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp3712=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3712[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3713; _temp3713.tag= Cyc_Absyn_TunionType_tag;
_temp3713.f1=({ struct Cyc_Absyn_TunionInfo _temp3714; _temp3714.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3718= yyvs; struct _xtunion_struct** _temp3720=
_temp3718.curr +( yyvsp_offset - 1); if( _temp3718.base == 0? 1:( _temp3720 <
_temp3718.base? 1: _temp3720 >= _temp3718.last_plus_one)){ _throw(
Null_Exception);}* _temp3720;})); _temp3714.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3715= yyvs; struct _xtunion_struct** _temp3717=
_temp3715.curr + yyvsp_offset; if( _temp3715.base == 0? 1:( _temp3717 <
_temp3715.base? 1: _temp3717 >= _temp3715.last_plus_one)){ _throw(
Null_Exception);}* _temp3717;})); _temp3714.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp3714.tud= 0; _temp3714;}); _temp3713;}); _temp3712;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3721= yyls; struct Cyc_Yyltype* _temp3723= _temp3721.curr
+( yylsp_offset - 2); if( _temp3721.base == 0? 1:( _temp3723 < _temp3721.base? 1:
_temp3723 >= _temp3721.last_plus_one)){ _throw( Null_Exception);}* _temp3723;}).first_line,({
struct _tagged_ptr3 _temp3724= yyls; struct Cyc_Yyltype* _temp3726= _temp3724.curr
+ yylsp_offset; if( _temp3724.base == 0? 1:( _temp3726 < _temp3724.base? 1:
_temp3726 >= _temp3724.last_plus_one)){ _throw( Null_Exception);}* _temp3726;}).last_line));
_temp3711;}); _temp3710;}); break; case 109: _LL3709: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3728=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3728[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3729; _temp3729.tag= Cyc_TypeSpecifier_tok_tag;
_temp3729.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp3730=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp3730[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp3731; _temp3731.tag=
Cyc_Absyn_TunionType_tag; _temp3731.f1=({ struct Cyc_Absyn_TunionInfo _temp3732;
_temp3732.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3739= yyvs;
struct _xtunion_struct** _temp3741= _temp3739.curr +( yyvsp_offset - 1); if(
_temp3739.base == 0? 1:( _temp3741 < _temp3739.base? 1: _temp3741 >= _temp3739.last_plus_one)){
_throw( Null_Exception);}* _temp3741;})); _temp3732.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3736= yyvs; struct _xtunion_struct** _temp3738=
_temp3736.curr + yyvsp_offset; if( _temp3736.base == 0? 1:( _temp3738 <
_temp3736.base? 1: _temp3738 >= _temp3736.last_plus_one)){ _throw(
Null_Exception);}* _temp3738;})); _temp3732.rgn=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3733= yyvs; struct _xtunion_struct** _temp3735=
_temp3733.curr +( yyvsp_offset - 2); if( _temp3733.base == 0? 1:( _temp3735 <
_temp3733.base? 1: _temp3735 >= _temp3733.last_plus_one)){ _throw(
Null_Exception);}* _temp3735;})); _temp3732.tud= 0; _temp3732;}); _temp3731;});
_temp3730;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3742= yyls;
struct Cyc_Yyltype* _temp3744= _temp3742.curr +( yylsp_offset - 3); if(
_temp3742.base == 0? 1:( _temp3744 < _temp3742.base? 1: _temp3744 >= _temp3742.last_plus_one)){
_throw( Null_Exception);}* _temp3744;}).first_line,({ struct _tagged_ptr3
_temp3745= yyls; struct Cyc_Yyltype* _temp3747= _temp3745.curr + yylsp_offset;
if( _temp3745.base == 0? 1:( _temp3747 < _temp3745.base? 1: _temp3747 >=
_temp3745.last_plus_one)){ _throw( Null_Exception);}* _temp3747;}).last_line));
_temp3729;}); _temp3728;}); break; case 110: _LL3727: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3749=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3749[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3750; _temp3750.tag= Cyc_TypeSpecifier_tok_tag;
_temp3750.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp3751=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp3751[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp3752; _temp3752.tag= Cyc_Absyn_TunionFieldType_tag; _temp3752.f1=({ struct
Cyc_Absyn_TunionFieldInfo _temp3753; _temp3753.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3760= yyvs; struct _xtunion_struct** _temp3762=
_temp3760.curr +( yyvsp_offset - 3); if( _temp3760.base == 0? 1:( _temp3762 <
_temp3760.base? 1: _temp3762 >= _temp3760.last_plus_one)){ _throw(
Null_Exception);}* _temp3762;})); _temp3753.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3757= yyvs; struct _xtunion_struct** _temp3759=
_temp3757.curr + yyvsp_offset; if( _temp3757.base == 0? 1:( _temp3759 <
_temp3757.base? 1: _temp3759 >= _temp3757.last_plus_one)){ _throw(
Null_Exception);}* _temp3759;})); _temp3753.fname= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3754= yyvs; struct _xtunion_struct** _temp3756=
_temp3754.curr +( yyvsp_offset - 1); if( _temp3754.base == 0? 1:( _temp3756 <
_temp3754.base? 1: _temp3756 >= _temp3754.last_plus_one)){ _throw(
Null_Exception);}* _temp3756;})); _temp3753.tud= 0; _temp3753.tufd= 0; _temp3753;});
_temp3752;}); _temp3751;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3763= yyls; struct Cyc_Yyltype* _temp3765= _temp3763.curr +( yylsp_offset -
4); if( _temp3763.base == 0? 1:( _temp3765 < _temp3763.base? 1: _temp3765 >=
_temp3763.last_plus_one)){ _throw( Null_Exception);}* _temp3765;}).first_line,({
struct _tagged_ptr3 _temp3766= yyls; struct Cyc_Yyltype* _temp3768= _temp3766.curr
+ yylsp_offset; if( _temp3766.base == 0? 1:( _temp3768 < _temp3766.base? 1:
_temp3768 >= _temp3766.last_plus_one)){ _throw( Null_Exception);}* _temp3768;}).last_line));
_temp3750;}); _temp3749;}); break; case 111: _LL3748: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3770=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3770[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3771; _temp3771.tag= Cyc_TypeSpecifier_tok_tag;
_temp3771.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3772=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3772[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3773; _temp3773.tag= Cyc_Parse_Decl_spec_tag;
_temp3773.f1= Cyc_Absyn_xtunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3774= yyvs; struct _xtunion_struct** _temp3776=
_temp3774.curr +( yyvsp_offset - 3); if( _temp3774.base == 0? 1:( _temp3776 <
_temp3774.base? 1: _temp3776 >= _temp3774.last_plus_one)){ _throw(
Null_Exception);}* _temp3776;})), Cyc_yyget_TunionFieldList_tok(({ struct
_tagged_ptr2 _temp3777= yyvs; struct _xtunion_struct** _temp3779= _temp3777.curr
+( yyvsp_offset - 1); if( _temp3777.base == 0? 1:( _temp3779 < _temp3777.base? 1:
_temp3779 >= _temp3777.last_plus_one)){ _throw( Null_Exception);}* _temp3779;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3780= yyls; struct Cyc_Yyltype*
_temp3782= _temp3780.curr +( yylsp_offset - 4); if( _temp3780.base == 0? 1:(
_temp3782 < _temp3780.base? 1: _temp3782 >= _temp3780.last_plus_one)){ _throw(
Null_Exception);}* _temp3782;}).first_line,({ struct _tagged_ptr3 _temp3783=
yyls; struct Cyc_Yyltype* _temp3785= _temp3783.curr + yylsp_offset; if(
_temp3783.base == 0? 1:( _temp3785 < _temp3783.base? 1: _temp3785 >= _temp3783.last_plus_one)){
_throw( Null_Exception);}* _temp3785;}).last_line)); _temp3773;}); _temp3772;}));
_temp3771;}); _temp3770;}); break; case 112: _LL3769: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3787=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3787[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3788; _temp3788.tag= Cyc_TypeSpecifier_tok_tag;
_temp3788.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp3789=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp3789[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp3790; _temp3790.tag=
Cyc_Absyn_XTunionType_tag; _temp3790.f1=({ struct Cyc_Absyn_XTunionInfo
_temp3791; _temp3791.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3792=
yyvs; struct _xtunion_struct** _temp3794= _temp3792.curr + yyvsp_offset; if(
_temp3792.base == 0? 1:( _temp3794 < _temp3792.base? 1: _temp3794 >= _temp3792.last_plus_one)){
_throw( Null_Exception);}* _temp3794;})); _temp3791.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp3791.xtud= 0; _temp3791;}); _temp3790;}); _temp3789;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3795= yyls; struct Cyc_Yyltype* _temp3797= _temp3795.curr
+( yylsp_offset - 1); if( _temp3795.base == 0? 1:( _temp3797 < _temp3795.base? 1:
_temp3797 >= _temp3795.last_plus_one)){ _throw( Null_Exception);}* _temp3797;}).first_line,({
struct _tagged_ptr3 _temp3798= yyls; struct Cyc_Yyltype* _temp3800= _temp3798.curr
+ yylsp_offset; if( _temp3798.base == 0? 1:( _temp3800 < _temp3798.base? 1:
_temp3800 >= _temp3798.last_plus_one)){ _throw( Null_Exception);}* _temp3800;}).last_line));
_temp3788;}); _temp3787;}); break; case 113: _LL3786: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3802=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3802[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3803; _temp3803.tag= Cyc_TypeSpecifier_tok_tag;
_temp3803.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionType_struct*
_temp3804=( struct Cyc_Absyn_XTunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunionType_struct));
_temp3804[ 0]=({ struct Cyc_Absyn_XTunionType_struct _temp3805; _temp3805.tag=
Cyc_Absyn_XTunionType_tag; _temp3805.f1=({ struct Cyc_Absyn_XTunionInfo
_temp3806; _temp3806.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3810=
yyvs; struct _xtunion_struct** _temp3812= _temp3810.curr + yyvsp_offset; if(
_temp3810.base == 0? 1:( _temp3812 < _temp3810.base? 1: _temp3812 >= _temp3810.last_plus_one)){
_throw( Null_Exception);}* _temp3812;})); _temp3806.rgn=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3807= yyvs; struct _xtunion_struct** _temp3809=
_temp3807.curr +( yyvsp_offset - 1); if( _temp3807.base == 0? 1:( _temp3809 <
_temp3807.base? 1: _temp3809 >= _temp3807.last_plus_one)){ _throw(
Null_Exception);}* _temp3809;})); _temp3806.xtud= 0; _temp3806;}); _temp3805;});
_temp3804;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3813= yyls;
struct Cyc_Yyltype* _temp3815= _temp3813.curr +( yylsp_offset - 2); if(
_temp3813.base == 0? 1:( _temp3815 < _temp3813.base? 1: _temp3815 >= _temp3813.last_plus_one)){
_throw( Null_Exception);}* _temp3815;}).first_line,({ struct _tagged_ptr3
_temp3816= yyls; struct Cyc_Yyltype* _temp3818= _temp3816.curr + yylsp_offset;
if( _temp3816.base == 0? 1:( _temp3818 < _temp3816.base? 1: _temp3818 >=
_temp3816.last_plus_one)){ _throw( Null_Exception);}* _temp3818;}).last_line));
_temp3803;}); _temp3802;}); break; case 114: _LL3801: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3820=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3820[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3821; _temp3821.tag= Cyc_TypeSpecifier_tok_tag;
_temp3821.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_XTunionFieldType_struct*
_temp3822=( struct Cyc_Absyn_XTunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_XTunionFieldType_struct)); _temp3822[ 0]=({ struct Cyc_Absyn_XTunionFieldType_struct
_temp3823; _temp3823.tag= Cyc_Absyn_XTunionFieldType_tag; _temp3823.f1=({ struct
Cyc_Absyn_XTunionFieldInfo _temp3824; _temp3824.name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3828= yyvs; struct _xtunion_struct** _temp3830=
_temp3828.curr +( yyvsp_offset - 2); if( _temp3828.base == 0? 1:( _temp3830 <
_temp3828.base? 1: _temp3830 >= _temp3828.last_plus_one)){ _throw(
Null_Exception);}* _temp3830;})); _temp3824.fname= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3825= yyvs; struct _xtunion_struct** _temp3827=
_temp3825.curr + yyvsp_offset; if( _temp3825.base == 0? 1:( _temp3827 <
_temp3825.base? 1: _temp3827 >= _temp3825.last_plus_one)){ _throw(
Null_Exception);}* _temp3827;})); _temp3824.xtud= 0; _temp3824.xtufd= 0;
_temp3824;}); _temp3823;}); _temp3822;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3831= yyls; struct Cyc_Yyltype* _temp3833= _temp3831.curr +(
yylsp_offset - 3); if( _temp3831.base == 0? 1:( _temp3833 < _temp3831.base? 1:
_temp3833 >= _temp3831.last_plus_one)){ _throw( Null_Exception);}* _temp3833;}).first_line,({
struct _tagged_ptr3 _temp3834= yyls; struct Cyc_Yyltype* _temp3836= _temp3834.curr
+ yylsp_offset; if( _temp3834.base == 0? 1:( _temp3836 < _temp3834.base? 1:
_temp3836 >= _temp3834.last_plus_one)){ _throw( Null_Exception);}* _temp3836;}).last_line));
_temp3821;}); _temp3820;}); break; case 115: _LL3819: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3838=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3838[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3839; _temp3839.tag=
Cyc_TunionFieldList_tok_tag; _temp3839.f1=({ struct Cyc_List_List* _temp3840=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3840->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3841= yyvs; struct
_xtunion_struct** _temp3843= _temp3841.curr + yyvsp_offset; if( _temp3841.base
== 0? 1:( _temp3843 < _temp3841.base? 1: _temp3843 >= _temp3841.last_plus_one)){
_throw( Null_Exception);}* _temp3843;})); _temp3840->tl= 0; _temp3840;});
_temp3839;}); _temp3838;}); break; case 116: _LL3837: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3845=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3845[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3846; _temp3846.tag=
Cyc_TunionFieldList_tok_tag; _temp3846.f1=({ struct Cyc_List_List* _temp3847=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3847->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3848= yyvs; struct
_xtunion_struct** _temp3850= _temp3848.curr +( yyvsp_offset - 1); if( _temp3848.base
== 0? 1:( _temp3850 < _temp3848.base? 1: _temp3850 >= _temp3848.last_plus_one)){
_throw( Null_Exception);}* _temp3850;})); _temp3847->tl= 0; _temp3847;});
_temp3846;}); _temp3845;}); break; case 117: _LL3844: yyval=( struct
_xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct* _temp3852=( struct
Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3852[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3853; _temp3853.tag=
Cyc_TunionFieldList_tok_tag; _temp3853.f1=({ struct Cyc_List_List* _temp3854=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3854->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3858= yyvs; struct
_xtunion_struct** _temp3860= _temp3858.curr +( yyvsp_offset - 2); if( _temp3858.base
== 0? 1:( _temp3860 < _temp3858.base? 1: _temp3860 >= _temp3858.last_plus_one)){
_throw( Null_Exception);}* _temp3860;})); _temp3854->tl= Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3855= yyvs; struct _xtunion_struct** _temp3857=
_temp3855.curr + yyvsp_offset; if( _temp3855.base == 0? 1:( _temp3857 <
_temp3855.base? 1: _temp3857 >= _temp3855.last_plus_one)){ _throw(
Null_Exception);}* _temp3857;})); _temp3854;}); _temp3853;}); _temp3852;});
break; case 118: _LL3851: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3862=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3862[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3863; _temp3863.tag= Cyc_TunionFieldList_tok_tag; _temp3863.f1=({ struct
Cyc_List_List* _temp3864=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3864->hd=( void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2
_temp3868= yyvs; struct _xtunion_struct** _temp3870= _temp3868.curr +(
yyvsp_offset - 2); if( _temp3868.base == 0? 1:( _temp3870 < _temp3868.base? 1:
_temp3870 >= _temp3868.last_plus_one)){ _throw( Null_Exception);}* _temp3870;}));
_temp3864->tl= Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2 _temp3865=
yyvs; struct _xtunion_struct** _temp3867= _temp3865.curr + yyvsp_offset; if(
_temp3865.base == 0? 1:( _temp3867 < _temp3865.base? 1: _temp3867 >= _temp3865.last_plus_one)){
_throw( Null_Exception);}* _temp3867;})); _temp3864;}); _temp3863;}); _temp3862;});
break; case 119: _LL3861: yyval=( struct _xtunion_struct*)({ struct Cyc_TunionField_tok_struct*
_temp3872=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3872[ 0]=({ struct Cyc_TunionField_tok_struct _temp3873; _temp3873.tag= Cyc_TunionField_tok_tag;
_temp3873.f1=({ struct Cyc_Absyn_Tunionfield* _temp3874=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3874->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3881= yyvs; struct _xtunion_struct** _temp3883=
_temp3881.curr + yyvsp_offset; if( _temp3881.base == 0? 1:( _temp3883 <
_temp3881.base? 1: _temp3883 >= _temp3881.last_plus_one)){ _throw(
Null_Exception);}* _temp3883;})); _temp3874->tvs= 0; _temp3874->typs= 0;
_temp3874->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3875=
yyls; struct Cyc_Yyltype* _temp3877= _temp3875.curr + yylsp_offset; if(
_temp3875.base == 0? 1:( _temp3877 < _temp3875.base? 1: _temp3877 >= _temp3875.last_plus_one)){
_throw( Null_Exception);}* _temp3877;}).first_line,({ struct _tagged_ptr3
_temp3878= yyls; struct Cyc_Yyltype* _temp3880= _temp3878.curr + yylsp_offset;
if( _temp3878.base == 0? 1:( _temp3880 < _temp3878.base? 1: _temp3880 >=
_temp3878.last_plus_one)){ _throw( Null_Exception);}* _temp3880;}).last_line);
_temp3874;}); _temp3873;}); _temp3872;}); break; case 120: _LL3871: { struct Cyc_List_List*
typs=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3885= yyls; struct Cyc_Yyltype* _temp3887= _temp3885.curr +(
yylsp_offset - 1); if( _temp3885.base == 0? 1:( _temp3887 < _temp3885.base? 1:
_temp3887 >= _temp3885.last_plus_one)){ _throw( Null_Exception);}* _temp3887;}).first_line,({
struct _tagged_ptr3 _temp3888= yyls; struct Cyc_Yyltype* _temp3890= _temp3888.curr
+( yylsp_offset - 1); if( _temp3888.base == 0? 1:( _temp3890 < _temp3888.base? 1:
_temp3890 >= _temp3888.last_plus_one)){ _throw( Null_Exception);}* _temp3890;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3891= yyvs; struct _xtunion_struct** _temp3893=
_temp3891.curr +( yyvsp_offset - 1); if( _temp3891.base == 0? 1:( _temp3893 <
_temp3891.base? 1: _temp3893 >= _temp3891.last_plus_one)){ _throw(
Null_Exception);}* _temp3893;})))); struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3894= yyls; struct Cyc_Yyltype* _temp3896= _temp3894.curr
+( yylsp_offset - 3); if( _temp3894.base == 0? 1:( _temp3896 < _temp3894.base? 1:
_temp3896 >= _temp3894.last_plus_one)){ _throw( Null_Exception);}* _temp3896;}).first_line,({
struct _tagged_ptr3 _temp3897= yyls; struct Cyc_Yyltype* _temp3899= _temp3897.curr
+( yylsp_offset - 3); if( _temp3897.base == 0? 1:( _temp3899 < _temp3897.base? 1:
_temp3899 >= _temp3897.last_plus_one)){ _throw( Null_Exception);}* _temp3899;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3900= yyvs; struct
_xtunion_struct** _temp3902= _temp3900.curr +( yyvsp_offset - 3); if( _temp3900.base
== 0? 1:( _temp3902 < _temp3900.base? 1: _temp3902 >= _temp3900.last_plus_one)){
_throw( Null_Exception);}* _temp3902;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_TunionField_tok_struct* _temp3903=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp3903[ 0]=({ struct
Cyc_TunionField_tok_struct _temp3904; _temp3904.tag= Cyc_TunionField_tok_tag;
_temp3904.f1=({ struct Cyc_Absyn_Tunionfield* _temp3905=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3905->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3912= yyvs; struct _xtunion_struct** _temp3914=
_temp3912.curr +( yyvsp_offset - 4); if( _temp3912.base == 0? 1:( _temp3914 <
_temp3912.base? 1: _temp3914 >= _temp3912.last_plus_one)){ _throw(
Null_Exception);}* _temp3914;})); _temp3905->tvs= tvs; _temp3905->typs= typs;
_temp3905->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3906=
yyls; struct Cyc_Yyltype* _temp3908= _temp3906.curr +( yylsp_offset - 4); if(
_temp3906.base == 0? 1:( _temp3908 < _temp3906.base? 1: _temp3908 >= _temp3906.last_plus_one)){
_throw( Null_Exception);}* _temp3908;}).first_line,({ struct _tagged_ptr3
_temp3909= yyls; struct Cyc_Yyltype* _temp3911= _temp3909.curr + yylsp_offset;
if( _temp3909.base == 0? 1:( _temp3911 < _temp3909.base? 1: _temp3911 >=
_temp3909.last_plus_one)){ _throw( Null_Exception);}* _temp3911;}).last_line);
_temp3905;}); _temp3904;}); _temp3903;}); break;} case 121: _LL3884: yyval=({
struct _tagged_ptr2 _temp3916= yyvs; struct _xtunion_struct** _temp3918=
_temp3916.curr + yyvsp_offset; if( _temp3916.base == 0? 1:( _temp3918 <
_temp3916.base? 1: _temp3918 >= _temp3916.last_plus_one)){ _throw(
Null_Exception);}* _temp3918;}); break; case 122: _LL3915: yyval=( struct
_xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3920=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3920[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3921; _temp3921.tag= Cyc_Declarator_tok_tag;
_temp3921.f1=({ struct Cyc_Parse_Declarator* _temp3922=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3922->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3929= yyvs; struct _xtunion_struct** _temp3931=
_temp3929.curr + yyvsp_offset; if( _temp3929.base == 0? 1:( _temp3931 <
_temp3929.base? 1: _temp3931 >= _temp3929.last_plus_one)){ _throw(
Null_Exception);}* _temp3931;})))->id; _temp3922->tms=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp3923= yyvs; struct _xtunion_struct** _temp3925=
_temp3923.curr +( yyvsp_offset - 1); if( _temp3923.base == 0? 1:( _temp3925 <
_temp3923.base? 1: _temp3925 >= _temp3923.last_plus_one)){ _throw(
Null_Exception);}* _temp3925;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3926= yyvs; struct _xtunion_struct** _temp3928= _temp3926.curr
+ yyvsp_offset; if( _temp3926.base == 0? 1:( _temp3928 < _temp3926.base? 1:
_temp3928 >= _temp3926.last_plus_one)){ _throw( Null_Exception);}* _temp3928;})))->tms);
_temp3922;}); _temp3921;}); _temp3920;}); break; case 123: _LL3919: yyval=(
struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp3933=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3933[ 0]=({ struct Cyc_Declarator_tok_struct _temp3934; _temp3934.tag= Cyc_Declarator_tok_tag;
_temp3934.f1=({ struct Cyc_Parse_Declarator* _temp3935=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3935->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3936= yyvs; struct _xtunion_struct** _temp3938=
_temp3936.curr + yyvsp_offset; if( _temp3936.base == 0? 1:( _temp3938 <
_temp3936.base? 1: _temp3938 >= _temp3936.last_plus_one)){ _throw(
Null_Exception);}* _temp3938;})); _temp3935->tms= 0; _temp3935;}); _temp3934;});
_temp3933;}); break; case 124: _LL3932: yyval=({ struct _tagged_ptr2 _temp3940=
yyvs; struct _xtunion_struct** _temp3942= _temp3940.curr +( yyvsp_offset - 1);
if( _temp3940.base == 0? 1:( _temp3942 < _temp3940.base? 1: _temp3942 >=
_temp3940.last_plus_one)){ _throw( Null_Exception);}* _temp3942;}); break; case
125: _LL3939: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp3944=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3944[ 0]=({ struct Cyc_Declarator_tok_struct _temp3945; _temp3945.tag= Cyc_Declarator_tok_tag;
_temp3945.f1=({ struct Cyc_Parse_Declarator* _temp3946=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3946->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3951= yyvs; struct _xtunion_struct** _temp3953=
_temp3951.curr +( yyvsp_offset - 2); if( _temp3951.base == 0? 1:( _temp3953 <
_temp3951.base? 1: _temp3953 >= _temp3951.last_plus_one)){ _throw(
Null_Exception);}* _temp3953;})))->id; _temp3946->tms=({ struct Cyc_List_List*
_temp3947=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3947->hd=( void*)(( void*) Cyc_Absyn_Carray_mod); _temp3947->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3948= yyvs; struct _xtunion_struct** _temp3950=
_temp3948.curr +( yyvsp_offset - 2); if( _temp3948.base == 0? 1:( _temp3950 <
_temp3948.base? 1: _temp3950 >= _temp3948.last_plus_one)){ _throw(
Null_Exception);}* _temp3950;})))->tms; _temp3947;}); _temp3946;}); _temp3945;});
_temp3944;}); break; case 126: _LL3943: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp3955=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3955[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3956; _temp3956.tag= Cyc_Declarator_tok_tag;
_temp3956.f1=({ struct Cyc_Parse_Declarator* _temp3957=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3957->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3967= yyvs; struct _xtunion_struct** _temp3969=
_temp3967.curr +( yyvsp_offset - 3); if( _temp3967.base == 0? 1:( _temp3969 <
_temp3967.base? 1: _temp3969 >= _temp3967.last_plus_one)){ _throw(
Null_Exception);}* _temp3969;})))->id; _temp3957->tms=({ struct Cyc_List_List*
_temp3958=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3958->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3962=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3962[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp3963; _temp3963.tag= Cyc_Absyn_ConstArray_mod_tag; _temp3963.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3964= yyvs; struct _xtunion_struct** _temp3966=
_temp3964.curr +( yyvsp_offset - 1); if( _temp3964.base == 0? 1:( _temp3966 <
_temp3964.base? 1: _temp3966 >= _temp3964.last_plus_one)){ _throw(
Null_Exception);}* _temp3966;})); _temp3963;}); _temp3962;})); _temp3958->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3959= yyvs; struct
_xtunion_struct** _temp3961= _temp3959.curr +( yyvsp_offset - 3); if( _temp3959.base
== 0? 1:( _temp3961 < _temp3959.base? 1: _temp3961 >= _temp3959.last_plus_one)){
_throw( Null_Exception);}* _temp3961;})))->tms; _temp3958;}); _temp3957;});
_temp3956;}); _temp3955;}); break; case 127: _LL3954: { struct _tuple16
_temp3976; struct Cyc_Core_Opt* _temp3977; int _temp3979; struct Cyc_List_List*
_temp3981; struct _tuple16* _temp3974= Cyc_yyget_ParamDeclListBool_tok(({ struct
_tagged_ptr2 _temp3971= yyvs; struct _xtunion_struct** _temp3973= _temp3971.curr
+( yyvsp_offset - 1); if( _temp3971.base == 0? 1:( _temp3973 < _temp3971.base? 1:
_temp3973 >= _temp3971.last_plus_one)){ _throw( Null_Exception);}* _temp3973;}));
_temp3976=* _temp3974; _LL3982: _temp3981= _temp3976.f1; goto _LL3980; _LL3980:
_temp3979= _temp3976.f2; goto _LL3978; _LL3978: _temp3977= _temp3976.f3; goto
_LL3975; _LL3975: yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct*
_temp3983=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3983[ 0]=({ struct Cyc_Declarator_tok_struct _temp3984; _temp3984.tag= Cyc_Declarator_tok_tag;
_temp3984.f1=({ struct Cyc_Parse_Declarator* _temp3985=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3985->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3994= yyvs; struct _xtunion_struct** _temp3996=
_temp3994.curr +( yyvsp_offset - 3); if( _temp3994.base == 0? 1:( _temp3996 <
_temp3994.base? 1: _temp3996 >= _temp3994.last_plus_one)){ _throw(
Null_Exception);}* _temp3996;})))->id; _temp3985->tms=({ struct Cyc_List_List*
_temp3986=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3986->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3990=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3990[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3991; _temp3991.tag=
Cyc_Absyn_Function_mod_tag; _temp3991.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3992=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3992[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3993; _temp3993.tag= Cyc_Absyn_WithTypes_tag;
_temp3993.f1= _temp3981; _temp3993.f2= _temp3979; _temp3993.f3= _temp3977;
_temp3993;}); _temp3992;})); _temp3991;}); _temp3990;})); _temp3986->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3987= yyvs; struct _xtunion_struct** _temp3989=
_temp3987.curr +( yyvsp_offset - 3); if( _temp3987.base == 0? 1:( _temp3989 <
_temp3987.base? 1: _temp3989 >= _temp3987.last_plus_one)){ _throw(
Null_Exception);}* _temp3989;})))->tms; _temp3986;}); _temp3985;}); _temp3984;});
_temp3983;}); break;} case 128: _LL3970: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp3998=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3998[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3999; _temp3999.tag= Cyc_Declarator_tok_tag;
_temp3999.f1=({ struct Cyc_Parse_Declarator* _temp4000=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4000->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4009= yyvs; struct _xtunion_struct** _temp4011=
_temp4009.curr +( yyvsp_offset - 2); if( _temp4009.base == 0? 1:( _temp4011 <
_temp4009.base? 1: _temp4011 >= _temp4009.last_plus_one)){ _throw(
Null_Exception);}* _temp4011;})))->id; _temp4000->tms=({ struct Cyc_List_List*
_temp4001=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4001->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4005=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4005[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4006; _temp4006.tag=
Cyc_Absyn_Function_mod_tag; _temp4006.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4007=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4007[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4008; _temp4008.tag= Cyc_Absyn_WithTypes_tag;
_temp4008.f1= 0; _temp4008.f2= 0; _temp4008.f3= 0; _temp4008;}); _temp4007;}));
_temp4006;}); _temp4005;})); _temp4001->tl=( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp4002= yyvs; struct _xtunion_struct** _temp4004= _temp4002.curr
+( yyvsp_offset - 2); if( _temp4002.base == 0? 1:( _temp4004 < _temp4002.base? 1:
_temp4004 >= _temp4002.last_plus_one)){ _throw( Null_Exception);}* _temp4004;})))->tms;
_temp4001;}); _temp4000;}); _temp3999;}); _temp3998;}); break; case 129: _LL3997:
yyval=( struct _xtunion_struct*)({ struct Cyc_Declarator_tok_struct* _temp4013=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4013[ 0]=({ struct Cyc_Declarator_tok_struct _temp4014; _temp4014.tag= Cyc_Declarator_tok_tag;
_temp4014.f1=({ struct Cyc_Parse_Declarator* _temp4015=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4015->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4030= yyvs; struct _xtunion_struct** _temp4032=
_temp4030.curr +( yyvsp_offset - 4); if( _temp4030.base == 0? 1:( _temp4032 <
_temp4030.base? 1: _temp4032 >= _temp4030.last_plus_one)){ _throw(
Null_Exception);}* _temp4032;})))->id; _temp4015->tms=({ struct Cyc_List_List*
_temp4016=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4016->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4020=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4020[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4021; _temp4021.tag=
Cyc_Absyn_Function_mod_tag; _temp4021.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4022=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4022[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4023; _temp4023.tag= Cyc_Absyn_WithTypes_tag;
_temp4023.f1= 0; _temp4023.f2= 0; _temp4023.f3=({ struct Cyc_Core_Opt* _temp4024=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4024->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4025=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4025[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4026; _temp4026.tag= Cyc_Absyn_JoinEff_tag;
_temp4026.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4027= yyvs;
struct _xtunion_struct** _temp4029= _temp4027.curr +( yyvsp_offset - 1); if(
_temp4027.base == 0? 1:( _temp4029 < _temp4027.base? 1: _temp4029 >= _temp4027.last_plus_one)){
_throw( Null_Exception);}* _temp4029;})); _temp4026;}); _temp4025;})); _temp4024;});
_temp4023;}); _temp4022;})); _temp4021;}); _temp4020;})); _temp4016->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4017= yyvs; struct _xtunion_struct** _temp4019=
_temp4017.curr +( yyvsp_offset - 4); if( _temp4017.base == 0? 1:( _temp4019 <
_temp4017.base? 1: _temp4019 >= _temp4017.last_plus_one)){ _throw(
Null_Exception);}* _temp4019;})))->tms; _temp4016;}); _temp4015;}); _temp4014;});
_temp4013;}); break; case 130: _LL4012: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4034=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4034[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4035; _temp4035.tag= Cyc_Declarator_tok_tag;
_temp4035.f1=({ struct Cyc_Parse_Declarator* _temp4036=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4036->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4054= yyvs; struct _xtunion_struct** _temp4056=
_temp4054.curr +( yyvsp_offset - 3); if( _temp4054.base == 0? 1:( _temp4056 <
_temp4054.base? 1: _temp4056 >= _temp4054.last_plus_one)){ _throw(
Null_Exception);}* _temp4056;})))->id; _temp4036->tms=({ struct Cyc_List_List*
_temp4037=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4037->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4041=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4041[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp4042; _temp4042.tag=
Cyc_Absyn_Function_mod_tag; _temp4042.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct*
_temp4043=( struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp4043[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp4044; _temp4044.tag= Cyc_Absyn_NoTypes_tag;
_temp4044.f1= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4045= yyvs;
struct _xtunion_struct** _temp4047= _temp4045.curr +( yyvsp_offset - 1); if(
_temp4045.base == 0? 1:( _temp4047 < _temp4045.base? 1: _temp4047 >= _temp4045.last_plus_one)){
_throw( Null_Exception);}* _temp4047;})); _temp4044.f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4048= yyls; struct Cyc_Yyltype* _temp4050= _temp4048.curr
+( yylsp_offset - 3); if( _temp4048.base == 0? 1:( _temp4050 < _temp4048.base? 1:
_temp4050 >= _temp4048.last_plus_one)){ _throw( Null_Exception);}* _temp4050;}).first_line,({
struct _tagged_ptr3 _temp4051= yyls; struct Cyc_Yyltype* _temp4053= _temp4051.curr
+ yylsp_offset; if( _temp4051.base == 0? 1:( _temp4053 < _temp4051.base? 1:
_temp4053 >= _temp4051.last_plus_one)){ _throw( Null_Exception);}* _temp4053;}).last_line);
_temp4044;}); _temp4043;})); _temp4042;}); _temp4041;})); _temp4037->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4038= yyvs; struct _xtunion_struct** _temp4040=
_temp4038.curr +( yyvsp_offset - 3); if( _temp4038.base == 0? 1:( _temp4040 <
_temp4038.base? 1: _temp4040 >= _temp4038.last_plus_one)){ _throw(
Null_Exception);}* _temp4040;})))->tms; _temp4037;}); _temp4036;}); _temp4035;});
_temp4034;}); break; case 131: _LL4033: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4058= yyls; struct Cyc_Yyltype* _temp4060= _temp4058.curr
+( yylsp_offset - 2); if( _temp4058.base == 0? 1:( _temp4060 < _temp4058.base? 1:
_temp4060 >= _temp4058.last_plus_one)){ _throw( Null_Exception);}* _temp4060;}).first_line,({
struct _tagged_ptr3 _temp4061= yyls; struct Cyc_Yyltype* _temp4063= _temp4061.curr
+ yylsp_offset; if( _temp4061.base == 0? 1:( _temp4063 < _temp4061.base? 1:
_temp4063 >= _temp4061.last_plus_one)){ _throw( Null_Exception);}* _temp4063;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4064= yyvs; struct _xtunion_struct** _temp4066=
_temp4064.curr +( yyvsp_offset - 1); if( _temp4064.base == 0? 1:( _temp4066 <
_temp4064.base? 1: _temp4066 >= _temp4064.last_plus_one)){ _throw(
Null_Exception);}* _temp4066;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_Declarator_tok_struct* _temp4067=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4067[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4068; _temp4068.tag= Cyc_Declarator_tok_tag;
_temp4068.f1=({ struct Cyc_Parse_Declarator* _temp4069=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4069->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4082= yyvs; struct _xtunion_struct** _temp4084=
_temp4082.curr +( yyvsp_offset - 3); if( _temp4082.base == 0? 1:( _temp4084 <
_temp4082.base? 1: _temp4084 >= _temp4082.last_plus_one)){ _throw(
Null_Exception);}* _temp4084;})))->id; _temp4069->tms=({ struct Cyc_List_List*
_temp4070=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4070->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4074=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4074[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp4075; _temp4075.tag= Cyc_Absyn_TypeParams_mod_tag; _temp4075.f1= ts;
_temp4075.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4076= yyls;
struct Cyc_Yyltype* _temp4078= _temp4076.curr +( yylsp_offset - 3); if(
_temp4076.base == 0? 1:( _temp4078 < _temp4076.base? 1: _temp4078 >= _temp4076.last_plus_one)){
_throw( Null_Exception);}* _temp4078;}).first_line,({ struct _tagged_ptr3
_temp4079= yyls; struct Cyc_Yyltype* _temp4081= _temp4079.curr + yylsp_offset;
if( _temp4079.base == 0? 1:( _temp4081 < _temp4079.base? 1: _temp4081 >=
_temp4079.last_plus_one)){ _throw( Null_Exception);}* _temp4081;}).last_line);
_temp4075.f3= 0; _temp4075;}); _temp4074;})); _temp4070->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4071= yyvs; struct _xtunion_struct** _temp4073=
_temp4071.curr +( yyvsp_offset - 3); if( _temp4071.base == 0? 1:( _temp4073 <
_temp4071.base? 1: _temp4073 >= _temp4071.last_plus_one)){ _throw(
Null_Exception);}* _temp4073;})))->tms; _temp4070;}); _temp4069;}); _temp4068;});
_temp4067;}); break;} case 132: _LL4057:(( struct Cyc_Lexing_lexbuf*)({ struct
Cyc_Core_Opt* _temp4086= Cyc_Parse_lbuf; if( _temp4086 == 0){ _throw(
Null_Exception);} _temp4086->v;}))->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4087=
yyls; struct Cyc_Yyltype* _temp4089= _temp4087.curr +( yylsp_offset - 2); if(
_temp4087.base == 0? 1:( _temp4089 < _temp4087.base? 1: _temp4089 >= _temp4087.last_plus_one)){
_throw( Null_Exception);}* _temp4089;}).first_line,({ struct _tagged_ptr3
_temp4090= yyls; struct Cyc_Yyltype* _temp4092= _temp4090.curr + yylsp_offset;
if( _temp4090.base == 0? 1:( _temp4092 < _temp4090.base? 1: _temp4092 >=
_temp4090.last_plus_one)){ _throw( Null_Exception);}* _temp4092;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4093= yyvs; struct _xtunion_struct** _temp4095=
_temp4093.curr +( yyvsp_offset - 1); if( _temp4093.base == 0? 1:( _temp4095 <
_temp4093.base? 1: _temp4095 >= _temp4093.last_plus_one)){ _throw(
Null_Exception);}* _temp4095;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_Declarator_tok_struct* _temp4096=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4096[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4097; _temp4097.tag= Cyc_Declarator_tok_tag;
_temp4097.f1=({ struct Cyc_Parse_Declarator* _temp4098=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4098->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4111= yyvs; struct _xtunion_struct** _temp4113=
_temp4111.curr +( yyvsp_offset - 3); if( _temp4111.base == 0? 1:( _temp4113 <
_temp4111.base? 1: _temp4113 >= _temp4111.last_plus_one)){ _throw(
Null_Exception);}* _temp4113;})))->id; _temp4098->tms=({ struct Cyc_List_List*
_temp4099=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4099->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4103=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4103[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp4104; _temp4104.tag= Cyc_Absyn_TypeParams_mod_tag; _temp4104.f1= ts;
_temp4104.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4105= yyls;
struct Cyc_Yyltype* _temp4107= _temp4105.curr +( yylsp_offset - 3); if(
_temp4105.base == 0? 1:( _temp4107 < _temp4105.base? 1: _temp4107 >= _temp4105.last_plus_one)){
_throw( Null_Exception);}* _temp4107;}).first_line,({ struct _tagged_ptr3
_temp4108= yyls; struct Cyc_Yyltype* _temp4110= _temp4108.curr + yylsp_offset;
if( _temp4108.base == 0? 1:( _temp4110 < _temp4108.base? 1: _temp4110 >=
_temp4108.last_plus_one)){ _throw( Null_Exception);}* _temp4110;}).last_line);
_temp4104.f3= 0; _temp4104;}); _temp4103;})); _temp4099->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4100= yyvs; struct _xtunion_struct** _temp4102=
_temp4100.curr +( yyvsp_offset - 3); if( _temp4100.base == 0? 1:( _temp4102 <
_temp4100.base? 1: _temp4102 >= _temp4100.last_plus_one)){ _throw(
Null_Exception);}* _temp4102;})))->tms; _temp4099;}); _temp4098;}); _temp4097;});
_temp4096;}); break;} case 133: _LL4085: yyval=( struct _xtunion_struct*)({
struct Cyc_Declarator_tok_struct* _temp4115=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4115[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4116; _temp4116.tag= Cyc_Declarator_tok_tag;
_temp4116.f1=({ struct Cyc_Parse_Declarator* _temp4117=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4117->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4133= yyvs; struct _xtunion_struct** _temp4135=
_temp4133.curr +( yyvsp_offset - 1); if( _temp4133.base == 0? 1:( _temp4135 <
_temp4133.base? 1: _temp4135 >= _temp4133.last_plus_one)){ _throw(
Null_Exception);}* _temp4135;})))->id; _temp4117->tms=({ struct Cyc_List_List*
_temp4118=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4118->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp4122=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp4122[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp4123; _temp4123.tag= Cyc_Absyn_Attributes_mod_tag; _temp4123.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4124= yyls; struct Cyc_Yyltype* _temp4126= _temp4124.curr
+ yylsp_offset; if( _temp4124.base == 0? 1:( _temp4126 < _temp4124.base? 1:
_temp4126 >= _temp4124.last_plus_one)){ _throw( Null_Exception);}* _temp4126;}).first_line,({
struct _tagged_ptr3 _temp4127= yyls; struct Cyc_Yyltype* _temp4129= _temp4127.curr
+ yylsp_offset; if( _temp4127.base == 0? 1:( _temp4129 < _temp4127.base? 1:
_temp4129 >= _temp4127.last_plus_one)){ _throw( Null_Exception);}* _temp4129;}).last_line);
_temp4123.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4130= yyvs;
struct _xtunion_struct** _temp4132= _temp4130.curr + yyvsp_offset; if( _temp4130.base
== 0? 1:( _temp4132 < _temp4130.base? 1: _temp4132 >= _temp4130.last_plus_one)){
_throw( Null_Exception);}* _temp4132;})); _temp4123;}); _temp4122;})); _temp4118->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4119= yyvs; struct
_xtunion_struct** _temp4121= _temp4119.curr +( yyvsp_offset - 1); if( _temp4119.base
== 0? 1:( _temp4121 < _temp4119.base? 1: _temp4121 >= _temp4119.last_plus_one)){
_throw( Null_Exception);}* _temp4121;})))->tms; _temp4118;}); _temp4117;});
_temp4116;}); _temp4115;}); break; case 134: _LL4114: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct* _temp4137=( struct
Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));
_temp4137[ 0]=({ struct Cyc_TypeModifierList_tok_struct _temp4138; _temp4138.tag=
Cyc_TypeModifierList_tok_tag; _temp4138.f1=({ struct Cyc_List_List* _temp4139=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4139->hd=(
void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4149=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4149[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4150; _temp4150.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4150.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4151= yyvs; struct _xtunion_struct** _temp4153= _temp4151.curr +(
yyvsp_offset - 2); if( _temp4151.base == 0? 1:( _temp4153 < _temp4151.base? 1:
_temp4153 >= _temp4151.last_plus_one)){ _throw( Null_Exception);}* _temp4153;}));
_temp4150.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4154= yyvs;
struct _xtunion_struct** _temp4156= _temp4154.curr +( yyvsp_offset - 1); if(
_temp4154.base == 0? 1:( _temp4156 < _temp4154.base? 1: _temp4156 >= _temp4154.last_plus_one)){
_throw( Null_Exception);}* _temp4156;})); _temp4150.f3= Cyc_Absyn_empty_tqual();
_temp4150;}); _temp4149;})); _temp4139->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4140= yyls; struct Cyc_Yyltype* _temp4142= _temp4140.curr
+ yylsp_offset; if( _temp4140.base == 0? 1:( _temp4142 < _temp4140.base? 1:
_temp4142 >= _temp4140.last_plus_one)){ _throw( Null_Exception);}* _temp4142;}).first_line,({
struct _tagged_ptr3 _temp4143= yyls; struct Cyc_Yyltype* _temp4145= _temp4143.curr
+ yylsp_offset; if( _temp4143.base == 0? 1:( _temp4145 < _temp4143.base? 1:
_temp4145 >= _temp4143.last_plus_one)){ _throw( Null_Exception);}* _temp4145;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4146= yyvs; struct
_xtunion_struct** _temp4148= _temp4146.curr + yyvsp_offset; if( _temp4146.base
== 0? 1:( _temp4148 < _temp4146.base? 1: _temp4148 >= _temp4146.last_plus_one)){
_throw( Null_Exception);}* _temp4148;})), 0); _temp4139;}); _temp4138;});
_temp4137;}); break; case 135: _LL4136: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeModifierList_tok_struct* _temp4158=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4158[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4159; _temp4159.tag= Cyc_TypeModifierList_tok_tag;
_temp4159.f1=({ struct Cyc_List_List* _temp4160=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4160->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4170=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4170[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4171; _temp4171.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4171.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4172= yyvs; struct _xtunion_struct** _temp4174= _temp4172.curr +(
yyvsp_offset - 3); if( _temp4172.base == 0? 1:( _temp4174 < _temp4172.base? 1:
_temp4174 >= _temp4172.last_plus_one)){ _throw( Null_Exception);}* _temp4174;}));
_temp4171.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4175= yyvs;
struct _xtunion_struct** _temp4177= _temp4175.curr +( yyvsp_offset - 2); if(
_temp4175.base == 0? 1:( _temp4177 < _temp4175.base? 1: _temp4177 >= _temp4175.last_plus_one)){
_throw( Null_Exception);}* _temp4177;})); _temp4171.f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp4178= yyvs; struct _xtunion_struct** _temp4180=
_temp4178.curr + yyvsp_offset; if( _temp4178.base == 0? 1:( _temp4180 <
_temp4178.base? 1: _temp4180 >= _temp4178.last_plus_one)){ _throw(
Null_Exception);}* _temp4180;})); _temp4171;}); _temp4170;})); _temp4160->tl=
Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4161= yyls; struct Cyc_Yyltype* _temp4163= _temp4161.curr +( yylsp_offset -
1); if( _temp4161.base == 0? 1:( _temp4163 < _temp4161.base? 1: _temp4163 >=
_temp4161.last_plus_one)){ _throw( Null_Exception);}* _temp4163;}).first_line,({
struct _tagged_ptr3 _temp4164= yyls; struct Cyc_Yyltype* _temp4166= _temp4164.curr
+( yylsp_offset - 1); if( _temp4164.base == 0? 1:( _temp4166 < _temp4164.base? 1:
_temp4166 >= _temp4164.last_plus_one)){ _throw( Null_Exception);}* _temp4166;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4167= yyvs; struct
_xtunion_struct** _temp4169= _temp4167.curr +( yyvsp_offset - 1); if( _temp4167.base
== 0? 1:( _temp4169 < _temp4167.base? 1: _temp4169 >= _temp4167.last_plus_one)){
_throw( Null_Exception);}* _temp4169;})), 0); _temp4160;}); _temp4159;});
_temp4158;}); break; case 136: _LL4157: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeModifierList_tok_struct* _temp4182=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4182[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4183; _temp4183.tag= Cyc_TypeModifierList_tok_tag;
_temp4183.f1=({ struct Cyc_List_List* _temp4184=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4184->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4197=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4197[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4198; _temp4198.tag= Cyc_Absyn_Pointer_mod_tag;
_temp4198.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4199= yyvs; struct _xtunion_struct** _temp4201= _temp4199.curr +(
yyvsp_offset - 3); if( _temp4199.base == 0? 1:( _temp4201 < _temp4199.base? 1:
_temp4201 >= _temp4199.last_plus_one)){ _throw( Null_Exception);}* _temp4201;}));
_temp4198.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4202= yyvs;
struct _xtunion_struct** _temp4204= _temp4202.curr +( yyvsp_offset - 2); if(
_temp4202.base == 0? 1:( _temp4204 < _temp4202.base? 1: _temp4204 >= _temp4202.last_plus_one)){
_throw( Null_Exception);}* _temp4204;})); _temp4198.f3= Cyc_Absyn_empty_tqual();
_temp4198;}); _temp4197;})); _temp4184->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4185= yyls; struct Cyc_Yyltype* _temp4187= _temp4185.curr
+( yylsp_offset - 1); if( _temp4185.base == 0? 1:( _temp4187 < _temp4185.base? 1:
_temp4187 >= _temp4185.last_plus_one)){ _throw( Null_Exception);}* _temp4187;}).first_line,({
struct _tagged_ptr3 _temp4188= yyls; struct Cyc_Yyltype* _temp4190= _temp4188.curr
+( yylsp_offset - 1); if( _temp4188.base == 0? 1:( _temp4190 < _temp4188.base? 1:
_temp4190 >= _temp4188.last_plus_one)){ _throw( Null_Exception);}* _temp4190;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4191= yyvs; struct
_xtunion_struct** _temp4193= _temp4191.curr +( yyvsp_offset - 1); if( _temp4191.base
== 0? 1:( _temp4193 < _temp4191.base? 1: _temp4193 >= _temp4191.last_plus_one)){
_throw( Null_Exception);}* _temp4193;})), Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4194= yyvs; struct _xtunion_struct** _temp4196=
_temp4194.curr + yyvsp_offset; if( _temp4194.base == 0? 1:( _temp4196 <
_temp4194.base? 1: _temp4196 >= _temp4194.last_plus_one)){ _throw(
Null_Exception);}* _temp4196;}))); _temp4184;}); _temp4183;}); _temp4182;});
break; case 137: _LL4181: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4206=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4206[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4207; _temp4207.tag= Cyc_TypeModifierList_tok_tag; _temp4207.f1=({ struct
Cyc_List_List* _temp4208=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4208->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4221=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4221[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4222; _temp4222.tag=
Cyc_Absyn_Pointer_mod_tag; _temp4222.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4223= yyvs; struct _xtunion_struct** _temp4225=
_temp4223.curr +( yyvsp_offset - 4); if( _temp4223.base == 0? 1:( _temp4225 <
_temp4223.base? 1: _temp4225 >= _temp4223.last_plus_one)){ _throw(
Null_Exception);}* _temp4225;})); _temp4222.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4226= yyvs; struct _xtunion_struct** _temp4228=
_temp4226.curr +( yyvsp_offset - 3); if( _temp4226.base == 0? 1:( _temp4228 <
_temp4226.base? 1: _temp4228 >= _temp4226.last_plus_one)){ _throw(
Null_Exception);}* _temp4228;})); _temp4222.f3= Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4229= yyvs; struct _xtunion_struct** _temp4231= _temp4229.curr
+( yyvsp_offset - 1); if( _temp4229.base == 0? 1:( _temp4231 < _temp4229.base? 1:
_temp4231 >= _temp4229.last_plus_one)){ _throw( Null_Exception);}* _temp4231;}));
_temp4222;}); _temp4221;})); _temp4208->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4209= yyls; struct Cyc_Yyltype* _temp4211= _temp4209.curr
+( yylsp_offset - 2); if( _temp4209.base == 0? 1:( _temp4211 < _temp4209.base? 1:
_temp4211 >= _temp4209.last_plus_one)){ _throw( Null_Exception);}* _temp4211;}).first_line,({
struct _tagged_ptr3 _temp4212= yyls; struct Cyc_Yyltype* _temp4214= _temp4212.curr
+( yylsp_offset - 2); if( _temp4212.base == 0? 1:( _temp4214 < _temp4212.base? 1:
_temp4214 >= _temp4212.last_plus_one)){ _throw( Null_Exception);}* _temp4214;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4215= yyvs; struct
_xtunion_struct** _temp4217= _temp4215.curr +( yyvsp_offset - 2); if( _temp4215.base
== 0? 1:( _temp4217 < _temp4215.base? 1: _temp4217 >= _temp4215.last_plus_one)){
_throw( Null_Exception);}* _temp4217;})), Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4218= yyvs; struct _xtunion_struct** _temp4220=
_temp4218.curr + yyvsp_offset; if( _temp4218.base == 0? 1:( _temp4220 <
_temp4218.base? 1: _temp4220 >= _temp4218.last_plus_one)){ _throw(
Null_Exception);}* _temp4220;}))); _temp4208;}); _temp4207;}); _temp4206;});
break; case 138: _LL4205: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4233=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4233[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4234; _temp4234.tag=
Cyc_Pointer_Sort_tok_tag; _temp4234.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4235=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4235[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4236; _temp4236.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4236.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4237= yyls; struct Cyc_Yyltype* _temp4239= _temp4237.curr
+ yylsp_offset; if( _temp4237.base == 0? 1:( _temp4239 < _temp4237.base? 1:
_temp4239 >= _temp4237.last_plus_one)){ _throw( Null_Exception);}* _temp4239;}).first_line,({
struct _tagged_ptr3 _temp4240= yyls; struct Cyc_Yyltype* _temp4242= _temp4240.curr
+ yylsp_offset; if( _temp4240.base == 0? 1:( _temp4242 < _temp4240.base? 1:
_temp4242 >= _temp4240.last_plus_one)){ _throw( Null_Exception);}* _temp4242;}).last_line));
_temp4236;}); _temp4235;})); _temp4234;}); _temp4233;}); break; case 139:
_LL4232: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4244=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4244[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4245; _temp4245.tag=
Cyc_Pointer_Sort_tok_tag; _temp4245.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4246=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4246[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4247; _temp4247.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4247.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4248= yyls; struct
Cyc_Yyltype* _temp4250= _temp4248.curr + yylsp_offset; if( _temp4248.base == 0?
1:( _temp4250 < _temp4248.base? 1: _temp4250 >= _temp4248.last_plus_one)){
_throw( Null_Exception);}* _temp4250;}).first_line,({ struct _tagged_ptr3
_temp4251= yyls; struct Cyc_Yyltype* _temp4253= _temp4251.curr + yylsp_offset;
if( _temp4251.base == 0? 1:( _temp4253 < _temp4251.base? 1: _temp4253 >=
_temp4251.last_plus_one)){ _throw( Null_Exception);}* _temp4253;}).last_line));
_temp4247;}); _temp4246;})); _temp4245;}); _temp4244;}); break; case 140:
_LL4243: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4255=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4255[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4256; _temp4256.tag=
Cyc_Pointer_Sort_tok_tag; _temp4256.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4257=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4257[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4258; _temp4258.tag=
Cyc_Absyn_Nullable_ps_tag; _temp4258.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4259= yyvs; struct _xtunion_struct** _temp4261= _temp4259.curr
+( yyvsp_offset - 1); if( _temp4259.base == 0? 1:( _temp4261 < _temp4259.base? 1:
_temp4261 >= _temp4259.last_plus_one)){ _throw( Null_Exception);}* _temp4261;}));
_temp4258;}); _temp4257;})); _temp4256;}); _temp4255;}); break; case 141:
_LL4254: yyval=( struct _xtunion_struct*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4263=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4263[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4264; _temp4264.tag=
Cyc_Pointer_Sort_tok_tag; _temp4264.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4265=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4265[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4266; _temp4266.tag= Cyc_Absyn_NonNullable_ps_tag; _temp4266.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4267= yyvs; struct _xtunion_struct** _temp4269=
_temp4267.curr +( yyvsp_offset - 1); if( _temp4267.base == 0? 1:( _temp4269 <
_temp4267.base? 1: _temp4269 >= _temp4267.last_plus_one)){ _throw(
Null_Exception);}* _temp4269;})); _temp4266;}); _temp4265;})); _temp4264;});
_temp4263;}); break; case 142: _LL4262: yyval=( struct _xtunion_struct*)({
struct Cyc_Pointer_Sort_tok_struct* _temp4271=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4271[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4272; _temp4272.tag= Cyc_Pointer_Sort_tok_tag;
_temp4272.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp4272;});
_temp4271;}); break; case 143: _LL4270: yyval=( struct _xtunion_struct*)({
struct Cyc_Rgn_tok_struct* _temp4274=( struct Cyc_Rgn_tok_struct*) GC_malloc(
sizeof( struct Cyc_Rgn_tok_struct)); _temp4274[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4275; _temp4275.tag= Cyc_Rgn_tok_tag; _temp4275.f1=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp4275;}); _temp4274;}); break; case 144: _LL4273: yyval=({ struct
_tagged_ptr2 _temp4277= yyvs; struct _xtunion_struct** _temp4279= _temp4277.curr
+ yyvsp_offset; if( _temp4277.base == 0? 1:( _temp4279 < _temp4277.base? 1:
_temp4279 >= _temp4277.last_plus_one)){ _throw( Null_Exception);}* _temp4279;});
break; case 145: _LL4276: yyval=( struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct*
_temp4281=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp4281[ 0]=({ struct Cyc_Rgn_tok_struct _temp4282; _temp4282.tag= Cyc_Rgn_tok_tag;
_temp4282.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4283= yyvs; struct _xtunion_struct** _temp4285= _temp4283.curr
+ yyvsp_offset; if( _temp4283.base == 0? 1:( _temp4285 < _temp4283.base? 1:
_temp4285 >= _temp4283.last_plus_one)){ _throw( Null_Exception);}* _temp4285;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4282;}); _temp4281;}); break; case 146: _LL4280: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4287= yyvs; struct _xtunion_struct** _temp4289=
_temp4287.curr + yyvsp_offset; if( _temp4287.base == 0? 1:( _temp4289 <
_temp4287.base? 1: _temp4289 >= _temp4287.last_plus_one)){ _throw(
Null_Exception);}* _temp4289;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4290=( char*)"expecting region kind\n";
struct _tagged_string _temp4291; _temp4291.curr= _temp4290; _temp4291.base=
_temp4290; _temp4291.last_plus_one= _temp4290 + 23; _temp4291;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4292= yyls; struct Cyc_Yyltype* _temp4294= _temp4292.curr
+ yylsp_offset; if( _temp4292.base == 0? 1:( _temp4294 < _temp4292.base? 1:
_temp4294 >= _temp4292.last_plus_one)){ _throw( Null_Exception);}* _temp4294;}).first_line,({
struct _tagged_ptr3 _temp4295= yyls; struct Cyc_Yyltype* _temp4297= _temp4295.curr
+ yylsp_offset; if( _temp4295.base == 0? 1:( _temp4297 < _temp4295.base? 1:
_temp4297 >= _temp4295.last_plus_one)){ _throw( Null_Exception);}* _temp4297;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4298=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4298[
0]=({ struct Cyc_Rgn_tok_struct _temp4299; _temp4299.tag= Cyc_Rgn_tok_tag;
_temp4299.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4300= yyvs; struct _xtunion_struct** _temp4302= _temp4300.curr
+( yyvsp_offset - 2); if( _temp4300.base == 0? 1:( _temp4302 < _temp4300.base? 1:
_temp4302 >= _temp4300.last_plus_one)){ _throw( Null_Exception);}* _temp4302;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4299;}); _temp4298;}); break; case 147: _LL4286: yyval=( struct
_xtunion_struct*)({ struct Cyc_Rgn_tok_struct* _temp4304=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4304[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4305; _temp4305.tag= Cyc_Rgn_tok_tag; _temp4305.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp4305;}); _temp4304;}); break; case 148: _LL4303:
yyval=({ struct _tagged_ptr2 _temp4307= yyvs; struct _xtunion_struct** _temp4309=
_temp4307.curr + yyvsp_offset; if( _temp4307.base == 0? 1:( _temp4309 <
_temp4307.base? 1: _temp4309 >= _temp4307.last_plus_one)){ _throw(
Null_Exception);}* _temp4309;}); break; case 149: _LL4306: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeQual_tok_struct* _temp4311=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp4311[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp4312; _temp4312.tag= Cyc_TypeQual_tok_tag; _temp4312.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp4313= yyvs; struct
_xtunion_struct** _temp4315= _temp4313.curr +( yyvsp_offset - 1); if( _temp4313.base
== 0? 1:( _temp4315 < _temp4313.base? 1: _temp4315 >= _temp4313.last_plus_one)){
_throw( Null_Exception);}* _temp4315;})), Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4316= yyvs; struct _xtunion_struct** _temp4318= _temp4316.curr
+ yyvsp_offset; if( _temp4316.base == 0? 1:( _temp4318 < _temp4316.base? 1:
_temp4318 >= _temp4316.last_plus_one)){ _throw( Null_Exception);}* _temp4318;})));
_temp4312;}); _temp4311;}); break; case 150: _LL4310: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4320=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4320[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4321; _temp4321.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4321.f1=({ struct _tuple16* _temp4322=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4322->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4323= yyvs; struct _xtunion_struct** _temp4325=
_temp4323.curr + yyvsp_offset; if( _temp4323.base == 0? 1:( _temp4325 <
_temp4323.base? 1: _temp4325 >= _temp4323.last_plus_one)){ _throw(
Null_Exception);}* _temp4325;}))); _temp4322->f2= 0; _temp4322->f3= 0; _temp4322;});
_temp4321;}); _temp4320;}); break; case 151: _LL4319: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4327=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4327[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4328; _temp4328.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4328.f1=({ struct _tuple16* _temp4329=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4329->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4330= yyvs; struct _xtunion_struct** _temp4332=
_temp4330.curr +( yyvsp_offset - 2); if( _temp4330.base == 0? 1:( _temp4332 <
_temp4330.base? 1: _temp4332 >= _temp4330.last_plus_one)){ _throw(
Null_Exception);}* _temp4332;}))); _temp4329->f2= 1; _temp4329->f3= 0; _temp4329;});
_temp4328;}); _temp4327;}); break; case 152: _LL4326: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclListBool_tok_struct* _temp4334=( struct
Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));
_temp4334[ 0]=({ struct Cyc_ParamDeclListBool_tok_struct _temp4335; _temp4335.tag=
Cyc_ParamDeclListBool_tok_tag; _temp4335.f1=({ struct _tuple16* _temp4336=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp4336->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4343= yyvs; struct _xtunion_struct** _temp4345=
_temp4343.curr +( yyvsp_offset - 2); if( _temp4343.base == 0? 1:( _temp4345 <
_temp4343.base? 1: _temp4345 >= _temp4343.last_plus_one)){ _throw(
Null_Exception);}* _temp4345;}))); _temp4336->f2= 0; _temp4336->f3=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4337=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4337->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4338=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4338[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4339; _temp4339.tag= Cyc_Absyn_JoinEff_tag;
_temp4339.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4340= yyvs;
struct _xtunion_struct** _temp4342= _temp4340.curr + yyvsp_offset; if( _temp4340.base
== 0? 1:( _temp4342 < _temp4340.base? 1: _temp4342 >= _temp4340.last_plus_one)){
_throw( Null_Exception);}* _temp4342;})); _temp4339;}); _temp4338;})); _temp4337;});
_temp4336;}); _temp4335;}); _temp4334;}); break; case 153: _LL4333: yyval=({
struct _tagged_ptr2 _temp4347= yyvs; struct _xtunion_struct** _temp4349=
_temp4347.curr + yyvsp_offset; if( _temp4347.base == 0? 1:( _temp4349 <
_temp4347.base? 1: _temp4349 >= _temp4347.last_plus_one)){ _throw(
Null_Exception);}* _temp4349;}); break; case 154: _LL4346: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4351=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4351[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4352; _temp4352.tag= Cyc_TypeList_tok_tag; _temp4352.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4353= yyvs; struct _xtunion_struct** _temp4355=
_temp4353.curr +( yyvsp_offset - 2); if( _temp4353.base == 0? 1:( _temp4355 <
_temp4353.base? 1: _temp4355 >= _temp4353.last_plus_one)){ _throw(
Null_Exception);}* _temp4355;})), Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2
_temp4356= yyvs; struct _xtunion_struct** _temp4358= _temp4356.curr +
yyvsp_offset; if( _temp4356.base == 0? 1:( _temp4358 < _temp4356.base? 1:
_temp4358 >= _temp4356.last_plus_one)){ _throw( Null_Exception);}* _temp4358;})));
_temp4352;}); _temp4351;}); break; case 155: _LL4350: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4360=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4360[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4361; _temp4361.tag= Cyc_TypeList_tok_tag; _temp4361.f1= 0; _temp4361;});
_temp4360;}); break; case 156: _LL4359: yyval=({ struct _tagged_ptr2 _temp4363=
yyvs; struct _xtunion_struct** _temp4365= _temp4363.curr +( yyvsp_offset - 1);
if( _temp4363.base == 0? 1:( _temp4365 < _temp4363.base? 1: _temp4365 >=
_temp4363.last_plus_one)){ _throw( Null_Exception);}* _temp4365;}); break; case
157: _LL4362: yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct*
_temp4367=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4367[ 0]=({ struct Cyc_TypeList_tok_struct _temp4368; _temp4368.tag= Cyc_TypeList_tok_tag;
_temp4368.f1=({ struct Cyc_List_List* _temp4369=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4369->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4370= yyvs; struct
_xtunion_struct** _temp4372= _temp4370.curr + yyvsp_offset; if( _temp4370.base
== 0? 1:( _temp4372 < _temp4370.base? 1: _temp4372 >= _temp4370.last_plus_one)){
_throw( Null_Exception);}* _temp4372;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp4369->tl= 0; _temp4369;});
_temp4368;}); _temp4367;}); break; case 158: _LL4366: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4374= yyvs; struct _xtunion_struct** _temp4376=
_temp4374.curr + yyvsp_offset; if( _temp4374.base == 0? 1:( _temp4376 <
_temp4374.base? 1: _temp4376 >= _temp4374.last_plus_one)){ _throw(
Null_Exception);}* _temp4376;})) !=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4377=( char*)"expecing effect kind (E)";
struct _tagged_string _temp4378; _temp4378.curr= _temp4377; _temp4378.base=
_temp4377; _temp4378.last_plus_one= _temp4377 + 25; _temp4378;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4379= yyls; struct Cyc_Yyltype* _temp4381= _temp4379.curr
+ yylsp_offset; if( _temp4379.base == 0? 1:( _temp4381 < _temp4379.base? 1:
_temp4381 >= _temp4379.last_plus_one)){ _throw( Null_Exception);}* _temp4381;}).first_line,({
struct _tagged_ptr3 _temp4382= yyls; struct Cyc_Yyltype* _temp4384= _temp4382.curr
+ yylsp_offset; if( _temp4382.base == 0? 1:( _temp4384 < _temp4382.base? 1:
_temp4384 >= _temp4382.last_plus_one)){ _throw( Null_Exception);}* _temp4384;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4385=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4385[ 0]=({ struct Cyc_TypeList_tok_struct _temp4386; _temp4386.tag= Cyc_TypeList_tok_tag;
_temp4386.f1=({ struct Cyc_List_List* _temp4387=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4387->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4388= yyvs; struct
_xtunion_struct** _temp4390= _temp4388.curr +( yyvsp_offset - 2); if( _temp4388.base
== 0? 1:( _temp4390 < _temp4388.base? 1: _temp4390 >= _temp4388.last_plus_one)){
_throw( Null_Exception);}* _temp4390;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp4387->tl= 0; _temp4387;});
_temp4386;}); _temp4385;}); break; case 159: _LL4373: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4392=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4392[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4393; _temp4393.tag= Cyc_TypeList_tok_tag; _temp4393.f1=({ struct Cyc_List_List*
_temp4394=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4394->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4395=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4395[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4396; _temp4396.tag= Cyc_Absyn_AccessEff_tag;
_temp4396.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4397= yyvs; struct _xtunion_struct** _temp4399= _temp4397.curr
+ yyvsp_offset; if( _temp4397.base == 0? 1:( _temp4399 < _temp4397.base? 1:
_temp4399 >= _temp4397.last_plus_one)){ _throw( Null_Exception);}* _temp4399;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4396;}); _temp4395;})); _temp4394->tl= 0; _temp4394;}); _temp4393;});
_temp4392;}); break; case 160: _LL4391: yyval=( struct _xtunion_struct*)({
struct Cyc_TypeList_tok_struct* _temp4401=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4401[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4402; _temp4402.tag= Cyc_TypeList_tok_tag; _temp4402.f1=({ struct Cyc_List_List*
_temp4403=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4403->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4407=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4407[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4408; _temp4408.tag= Cyc_Absyn_AccessEff_tag;
_temp4408.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4409= yyvs; struct _xtunion_struct** _temp4411= _temp4409.curr
+( yyvsp_offset - 2); if( _temp4409.base == 0? 1:( _temp4411 < _temp4409.base? 1:
_temp4411 >= _temp4409.last_plus_one)){ _throw( Null_Exception);}* _temp4411;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4408;}); _temp4407;})); _temp4403->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4404= yyvs; struct _xtunion_struct** _temp4406= _temp4404.curr
+ yyvsp_offset; if( _temp4404.base == 0? 1:( _temp4406 < _temp4404.base? 1:
_temp4406 >= _temp4404.last_plus_one)){ _throw( Null_Exception);}* _temp4406;}));
_temp4403;}); _temp4402;}); _temp4401;}); break; case 161: _LL4400: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4413= yyvs; struct _xtunion_struct** _temp4415=
_temp4413.curr + yyvsp_offset; if( _temp4413.base == 0? 1:( _temp4415 <
_temp4413.base? 1: _temp4415 >= _temp4413.last_plus_one)){ _throw(
Null_Exception);}* _temp4415;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4416=( char*)"expecting region kind (R)";
struct _tagged_string _temp4417; _temp4417.curr= _temp4416; _temp4417.base=
_temp4416; _temp4417.last_plus_one= _temp4416 + 26; _temp4417;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4418= yyls; struct Cyc_Yyltype* _temp4420= _temp4418.curr
+ yylsp_offset; if( _temp4418.base == 0? 1:( _temp4420 < _temp4418.base? 1:
_temp4420 >= _temp4418.last_plus_one)){ _throw( Null_Exception);}* _temp4420;}).first_line,({
struct _tagged_ptr3 _temp4421= yyls; struct Cyc_Yyltype* _temp4423= _temp4421.curr
+ yylsp_offset; if( _temp4421.base == 0? 1:( _temp4423 < _temp4421.base? 1:
_temp4423 >= _temp4421.last_plus_one)){ _throw( Null_Exception);}* _temp4423;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4424=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4424[ 0]=({ struct Cyc_TypeList_tok_struct _temp4425; _temp4425.tag= Cyc_TypeList_tok_tag;
_temp4425.f1=({ struct Cyc_List_List* _temp4426=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4426->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4427=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4427[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4428; _temp4428.tag= Cyc_Absyn_AccessEff_tag;
_temp4428.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4429= yyvs; struct _xtunion_struct** _temp4431= _temp4429.curr
+( yyvsp_offset - 2); if( _temp4429.base == 0? 1:( _temp4431 < _temp4429.base? 1:
_temp4431 >= _temp4429.last_plus_one)){ _throw( Null_Exception);}* _temp4431;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4428;}); _temp4427;})); _temp4426->tl= 0; _temp4426;}); _temp4425;});
_temp4424;}); break; case 162: _LL4412: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4433= yyvs; struct _xtunion_struct** _temp4435= _temp4433.curr
+( yyvsp_offset - 2); if( _temp4433.base == 0? 1:( _temp4435 < _temp4433.base? 1:
_temp4435 >= _temp4433.last_plus_one)){ _throw( Null_Exception);}* _temp4435;}))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp4436=( char*)"expecting region kind (R)"; struct _tagged_string _temp4437;
_temp4437.curr= _temp4436; _temp4437.base= _temp4436; _temp4437.last_plus_one=
_temp4436 + 26; _temp4437;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4438= yyls; struct Cyc_Yyltype* _temp4440= _temp4438.curr +( yylsp_offset -
2); if( _temp4438.base == 0? 1:( _temp4440 < _temp4438.base? 1: _temp4440 >=
_temp4438.last_plus_one)){ _throw( Null_Exception);}* _temp4440;}).first_line,({
struct _tagged_ptr3 _temp4441= yyls; struct Cyc_Yyltype* _temp4443= _temp4441.curr
+( yylsp_offset - 2); if( _temp4441.base == 0? 1:( _temp4443 < _temp4441.base? 1:
_temp4443 >= _temp4441.last_plus_one)){ _throw( Null_Exception);}* _temp4443;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4444=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4444[ 0]=({ struct Cyc_TypeList_tok_struct _temp4445; _temp4445.tag= Cyc_TypeList_tok_tag;
_temp4445.f1=({ struct Cyc_List_List* _temp4446=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4446->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4450=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4450[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4451; _temp4451.tag= Cyc_Absyn_AccessEff_tag;
_temp4451.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4452= yyvs; struct _xtunion_struct** _temp4454= _temp4452.curr
+( yyvsp_offset - 4); if( _temp4452.base == 0? 1:( _temp4454 < _temp4452.base? 1:
_temp4454 >= _temp4452.last_plus_one)){ _throw( Null_Exception);}* _temp4454;})),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4451;}); _temp4450;})); _temp4446->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4447= yyvs; struct _xtunion_struct** _temp4449= _temp4447.curr
+ yyvsp_offset; if( _temp4447.base == 0? 1:( _temp4449 < _temp4447.base? 1:
_temp4449 >= _temp4447.last_plus_one)){ _throw( Null_Exception);}* _temp4449;}));
_temp4446;}); _temp4445;}); _temp4444;}); break; case 163: _LL4432: yyval=(
struct _xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct* _temp4456=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4456[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4457; _temp4457.tag=
Cyc_ParamDeclList_tok_tag; _temp4457.f1=({ struct Cyc_List_List* _temp4458=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4458->hd=(
void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4459= yyvs; struct
_xtunion_struct** _temp4461= _temp4459.curr + yyvsp_offset; if( _temp4459.base
== 0? 1:( _temp4461 < _temp4459.base? 1: _temp4461 >= _temp4459.last_plus_one)){
_throw( Null_Exception);}* _temp4461;})); _temp4458->tl= 0; _temp4458;});
_temp4457;}); _temp4456;}); break; case 164: _LL4455: yyval=( struct
_xtunion_struct*)({ struct Cyc_ParamDeclList_tok_struct* _temp4463=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp4463[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp4464; _temp4464.tag= Cyc_ParamDeclList_tok_tag;
_temp4464.f1=({ struct Cyc_List_List* _temp4465=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4465->hd=( void*) Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp4469= yyvs; struct _xtunion_struct** _temp4471=
_temp4469.curr + yyvsp_offset; if( _temp4469.base == 0? 1:( _temp4471 <
_temp4469.base? 1: _temp4471 >= _temp4469.last_plus_one)){ _throw(
Null_Exception);}* _temp4471;})); _temp4465->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4466= yyvs; struct _xtunion_struct** _temp4468=
_temp4466.curr +( yyvsp_offset - 2); if( _temp4466.base == 0? 1:( _temp4468 <
_temp4466.base? 1: _temp4468 >= _temp4466.last_plus_one)){ _throw(
Null_Exception);}* _temp4468;})); _temp4465;}); _temp4464;}); _temp4463;});
break; case 165: _LL4462: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4473= yyvs; struct _xtunion_struct** _temp4475=
_temp4473.curr +( yyvsp_offset - 1); if( _temp4473.base == 0? 1:( _temp4475 <
_temp4473.base? 1: _temp4475 >= _temp4473.last_plus_one)){ _throw(
Null_Exception);}* _temp4475;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4476= yyls; struct Cyc_Yyltype* _temp4478= _temp4476.curr +(
yylsp_offset - 1); if( _temp4476.base == 0? 1:( _temp4478 < _temp4476.base? 1:
_temp4478 >= _temp4476.last_plus_one)){ _throw( Null_Exception);}* _temp4478;}).first_line,({
struct _tagged_ptr3 _temp4479= yyls; struct Cyc_Yyltype* _temp4481= _temp4479.curr
+( yylsp_offset - 1); if( _temp4479.base == 0? 1:( _temp4481 < _temp4479.base? 1:
_temp4481 >= _temp4479.last_plus_one)){ _throw( Null_Exception);}* _temp4481;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4482= yyvs; struct _xtunion_struct** _temp4484= _temp4482.curr +(
yyvsp_offset - 1); if( _temp4482.base == 0? 1:( _temp4484 < _temp4482.base? 1:
_temp4484 >= _temp4482.last_plus_one)){ _throw( Null_Exception);}* _temp4484;}))).f3;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4485= yyvs; struct _xtunion_struct** _temp4487= _temp4485.curr +(
yyvsp_offset - 1); if( _temp4485.base == 0? 1:( _temp4487 < _temp4485.base? 1:
_temp4487 >= _temp4485.last_plus_one)){ _throw( Null_Exception);}* _temp4487;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4488= yyvs; struct _xtunion_struct** _temp4490= _temp4488.curr +
yyvsp_offset; if( _temp4488.base == 0? 1:( _temp4490 < _temp4488.base? 1:
_temp4490 >= _temp4488.last_plus_one)){ _throw( Null_Exception);}* _temp4490;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp4491=( char*)"parameter with bad type params";
struct _tagged_string _temp4492; _temp4492.curr= _temp4491; _temp4492.base=
_temp4491; _temp4492.last_plus_one= _temp4491 + 31; _temp4492;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4493= yyls; struct Cyc_Yyltype* _temp4495= _temp4493.curr
+ yylsp_offset; if( _temp4493.base == 0? 1:( _temp4495 < _temp4493.base? 1:
_temp4495 >= _temp4493.last_plus_one)){ _throw( Null_Exception);}* _temp4495;}).first_line,({
struct _tagged_ptr3 _temp4496= yyls; struct Cyc_Yyltype* _temp4498= _temp4496.curr
+ yylsp_offset; if( _temp4496.base == 0? 1:( _temp4498 < _temp4496.base? 1:
_temp4498 >= _temp4496.last_plus_one)){ _throw( Null_Exception);}* _temp4498;}).last_line));}{
struct _tuple1* q=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4499=
yyvs; struct _xtunion_struct** _temp4501= _temp4499.curr + yyvsp_offset; if(
_temp4499.base == 0? 1:( _temp4501 < _temp4499.base? 1: _temp4501 >= _temp4499.last_plus_one)){
_throw( Null_Exception);}* _temp4501;})))->id;{ void* _temp4502=(* q).f1; struct
Cyc_List_List* _temp4512; struct Cyc_List_List* _temp4514; _LL4504: if(( int)
_temp4502 == Cyc_Absyn_Loc_n){ goto _LL4505;} else{ goto _LL4506;} _LL4506: if((
unsigned int) _temp4502 > 1u?(( struct _tunion_struct*) _temp4502)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL4513: _temp4512=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp4502)->f1; if( _temp4512 == 0){ goto _LL4507;} else{ goto _LL4508;}} else{
goto _LL4508;} _LL4508: if(( unsigned int) _temp4502 > 1u?(( struct
_tunion_struct*) _temp4502)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL4515: _temp4514=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp4502)->f1; if(
_temp4514 == 0){ goto _LL4509;} else{ goto _LL4510;}} else{ goto _LL4510;}
_LL4510: goto _LL4511; _LL4505: goto _LL4503; _LL4507: goto _LL4503; _LL4509:
goto _LL4503; _LL4511: Cyc_Parse_err(( struct _tagged_string)({ char* _temp4516=(
char*)"parameter cannot be qualified with a module name"; struct _tagged_string
_temp4517; _temp4517.curr= _temp4516; _temp4517.base= _temp4516; _temp4517.last_plus_one=
_temp4516 + 49; _temp4517;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4518= yyls; struct Cyc_Yyltype* _temp4520= _temp4518.curr +( yylsp_offset -
1); if( _temp4518.base == 0? 1:( _temp4520 < _temp4518.base? 1: _temp4520 >=
_temp4518.last_plus_one)){ _throw( Null_Exception);}* _temp4520;}).first_line,({
struct _tagged_ptr3 _temp4521= yyls; struct Cyc_Yyltype* _temp4523= _temp4521.curr
+( yylsp_offset - 1); if( _temp4521.base == 0? 1:( _temp4523 < _temp4521.base? 1:
_temp4523 >= _temp4521.last_plus_one)){ _throw( Null_Exception);}* _temp4523;}).last_line));
goto _LL4503; _LL4503:;}{ struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4524=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4524->v=( void*)(* q).f2; _temp4524;}); if( t_info.f4 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4525=( char*)"extra attributes on parameter, ignoring";
struct _tagged_string _temp4526; _temp4526.curr= _temp4525; _temp4526.base=
_temp4525; _temp4526.last_plus_one= _temp4525 + 40; _temp4526;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4527= yyls; struct Cyc_Yyltype* _temp4529= _temp4527.curr
+( yylsp_offset - 1); if( _temp4527.base == 0? 1:( _temp4529 < _temp4527.base? 1:
_temp4529 >= _temp4527.last_plus_one)){ _throw( Null_Exception);}* _temp4529;}).first_line,({
struct _tagged_ptr3 _temp4530= yyls; struct Cyc_Yyltype* _temp4532= _temp4530.curr
+ yylsp_offset; if( _temp4530.base == 0? 1:( _temp4532 < _temp4530.base? 1:
_temp4532 >= _temp4530.last_plus_one)){ _throw( Null_Exception);}* _temp4532;}).last_line));}
if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4533=(
char*)"extra type variables on parameter, ignoring"; struct _tagged_string
_temp4534; _temp4534.curr= _temp4533; _temp4534.base= _temp4533; _temp4534.last_plus_one=
_temp4533 + 44; _temp4534;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4535= yyls; struct Cyc_Yyltype* _temp4537= _temp4535.curr +( yylsp_offset -
1); if( _temp4535.base == 0? 1:( _temp4537 < _temp4535.base? 1: _temp4537 >=
_temp4535.last_plus_one)){ _throw( Null_Exception);}* _temp4537;}).first_line,({
struct _tagged_ptr3 _temp4538= yyls; struct Cyc_Yyltype* _temp4540= _temp4538.curr
+ yylsp_offset; if( _temp4538.base == 0? 1:( _temp4540 < _temp4538.base? 1:
_temp4540 >= _temp4538.last_plus_one)){ _throw( Null_Exception);}* _temp4540;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4541=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4541[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4542; _temp4542.tag= Cyc_ParamDecl_tok_tag;
_temp4542.f1=({ struct _tuple2* _temp4543=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4543->f1= idopt; _temp4543->f2= t_info.f1; _temp4543->f3=
t_info.f2; _temp4543;}); _temp4542;}); _temp4541;}); break;}}} case 166: _LL4472: {
void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp4545= yyvs; struct _xtunion_struct** _temp4547= _temp4545.curr
+ yyvsp_offset; if( _temp4545.base == 0? 1:( _temp4547 < _temp4545.base? 1:
_temp4547 >= _temp4545.last_plus_one)){ _throw( Null_Exception);}* _temp4547;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4548= yyls; struct Cyc_Yyltype*
_temp4550= _temp4548.curr + yylsp_offset; if( _temp4548.base == 0? 1:( _temp4550
< _temp4548.base? 1: _temp4550 >= _temp4548.last_plus_one)){ _throw(
Null_Exception);}* _temp4550;}).first_line,({ struct _tagged_ptr3 _temp4551=
yyls; struct Cyc_Yyltype* _temp4553= _temp4551.curr + yylsp_offset; if(
_temp4551.base == 0? 1:( _temp4553 < _temp4551.base? 1: _temp4553 >= _temp4551.last_plus_one)){
_throw( Null_Exception);}* _temp4553;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4554= yyvs; struct
_xtunion_struct** _temp4556= _temp4554.curr + yyvsp_offset; if( _temp4554.base
== 0? 1:( _temp4556 < _temp4554.base? 1: _temp4556 >= _temp4554.last_plus_one)){
_throw( Null_Exception);}* _temp4556;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4557= yyvs; struct _xtunion_struct** _temp4559=
_temp4557.curr + yyvsp_offset; if( _temp4557.base == 0? 1:( _temp4559 <
_temp4557.base? 1: _temp4559 >= _temp4557.last_plus_one)){ _throw(
Null_Exception);}* _temp4559;}))).f1; if( atts != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp4560=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4561; _temp4561.curr= _temp4560; _temp4561.base=
_temp4560; _temp4561.last_plus_one= _temp4560 + 38; _temp4561;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4562= yyls; struct Cyc_Yyltype* _temp4564= _temp4562.curr
+ yylsp_offset; if( _temp4562.base == 0? 1:( _temp4564 < _temp4562.base? 1:
_temp4564 >= _temp4562.last_plus_one)){ _throw( Null_Exception);}* _temp4564;}).first_line,({
struct _tagged_ptr3 _temp4565= yyls; struct Cyc_Yyltype* _temp4567= _temp4565.curr
+ yylsp_offset; if( _temp4565.base == 0? 1:( _temp4567 < _temp4565.base? 1:
_temp4567 >= _temp4565.last_plus_one)){ _throw( Null_Exception);}* _temp4567;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4568=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4568[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4569; _temp4569.tag= Cyc_ParamDecl_tok_tag;
_temp4569.f1=({ struct _tuple2* _temp4570=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4570->f1= 0; _temp4570->f2= tq; _temp4570->f3= t;
_temp4570;}); _temp4569;}); _temp4568;}); break;} case 167: _LL4544: { void* t=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4572= yyvs; struct _xtunion_struct** _temp4574= _temp4572.curr +(
yyvsp_offset - 1); if( _temp4572.base == 0? 1:( _temp4574 < _temp4572.base? 1:
_temp4574 >= _temp4572.last_plus_one)){ _throw( Null_Exception);}* _temp4574;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4575= yyls; struct Cyc_Yyltype*
_temp4577= _temp4575.curr +( yylsp_offset - 1); if( _temp4575.base == 0? 1:(
_temp4577 < _temp4575.base? 1: _temp4577 >= _temp4575.last_plus_one)){ _throw(
Null_Exception);}* _temp4577;}).first_line,({ struct _tagged_ptr3 _temp4578=
yyls; struct Cyc_Yyltype* _temp4580= _temp4578.curr +( yylsp_offset - 1); if(
_temp4578.base == 0? 1:( _temp4580 < _temp4578.base? 1: _temp4580 >= _temp4578.last_plus_one)){
_throw( Null_Exception);}* _temp4580;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4581= yyvs; struct
_xtunion_struct** _temp4583= _temp4581.curr +( yyvsp_offset - 1); if( _temp4581.base
== 0? 1:( _temp4583 < _temp4581.base? 1: _temp4583 >= _temp4581.last_plus_one)){
_throw( Null_Exception);}* _temp4583;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4584= yyvs; struct _xtunion_struct** _temp4586=
_temp4584.curr +( yyvsp_offset - 1); if( _temp4584.base == 0? 1:( _temp4586 <
_temp4584.base? 1: _temp4586 >= _temp4584.last_plus_one)){ _throw(
Null_Exception);}* _temp4586;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4587= yyvs; struct _xtunion_struct** _temp4589=
_temp4587.curr + yyvsp_offset; if( _temp4587.base == 0? 1:( _temp4589 <
_temp4587.base? 1: _temp4589 >= _temp4587.last_plus_one)){ _throw(
Null_Exception);}* _temp4589;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4590=( char*)"bad type parameters on formal argument, ignoring";
struct _tagged_string _temp4591; _temp4591.curr= _temp4590; _temp4591.base=
_temp4590; _temp4591.last_plus_one= _temp4590 + 49; _temp4591;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4592= yyls; struct Cyc_Yyltype* _temp4594= _temp4592.curr
+( yylsp_offset - 1); if( _temp4592.base == 0? 1:( _temp4594 < _temp4592.base? 1:
_temp4594 >= _temp4592.last_plus_one)){ _throw( Null_Exception);}* _temp4594;}).first_line,({
struct _tagged_ptr3 _temp4595= yyls; struct Cyc_Yyltype* _temp4597= _temp4595.curr
+ yylsp_offset; if( _temp4595.base == 0? 1:( _temp4597 < _temp4595.base? 1:
_temp4597 >= _temp4595.last_plus_one)){ _throw( Null_Exception);}* _temp4597;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4598=(
char*)"bad attributes on parameter, ignoring"; struct _tagged_string _temp4599;
_temp4599.curr= _temp4598; _temp4599.base= _temp4598; _temp4599.last_plus_one=
_temp4598 + 38; _temp4599;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4600= yyls; struct Cyc_Yyltype* _temp4602= _temp4600.curr +( yylsp_offset -
1); if( _temp4600.base == 0? 1:( _temp4602 < _temp4600.base? 1: _temp4602 >=
_temp4600.last_plus_one)){ _throw( Null_Exception);}* _temp4602;}).first_line,({
struct _tagged_ptr3 _temp4603= yyls; struct Cyc_Yyltype* _temp4605= _temp4603.curr
+ yylsp_offset; if( _temp4603.base == 0? 1:( _temp4605 < _temp4603.base? 1:
_temp4605 >= _temp4603.last_plus_one)){ _throw( Null_Exception);}* _temp4605;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4606=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4606[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4607; _temp4607.tag= Cyc_ParamDecl_tok_tag;
_temp4607.f1=({ struct _tuple2* _temp4608=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4608->f1= 0; _temp4608->f2= t_info.f1; _temp4608->f3=
t_info.f2; _temp4608;}); _temp4607;}); _temp4606;}); break;} case 168: _LL4571:
yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct* _temp4610=(
struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4610[ 0]=({ struct Cyc_IdList_tok_struct _temp4611; _temp4611.tag= Cyc_IdList_tok_tag;
_temp4611.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4612= yyvs; struct
_xtunion_struct** _temp4614= _temp4612.curr + yyvsp_offset; if( _temp4612.base
== 0? 1:( _temp4614 < _temp4612.base? 1: _temp4614 >= _temp4612.last_plus_one)){
_throw( Null_Exception);}* _temp4614;}))); _temp4611;}); _temp4610;}); break;
case 169: _LL4609: yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct*
_temp4616=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4616[ 0]=({ struct Cyc_IdList_tok_struct _temp4617; _temp4617.tag= Cyc_IdList_tok_tag;
_temp4617.f1=({ struct Cyc_List_List* _temp4618=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4618->hd=( void*)({ struct
_tagged_string* _temp4619=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4619[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4620= yyvs; struct _xtunion_struct** _temp4622= _temp4620.curr +
yyvsp_offset; if( _temp4620.base == 0? 1:( _temp4622 < _temp4620.base? 1:
_temp4622 >= _temp4620.last_plus_one)){ _throw( Null_Exception);}* _temp4622;}));
_temp4619;}); _temp4618->tl= 0; _temp4618;}); _temp4617;}); _temp4616;}); break;
case 170: _LL4615: yyval=( struct _xtunion_struct*)({ struct Cyc_IdList_tok_struct*
_temp4624=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4624[ 0]=({ struct Cyc_IdList_tok_struct _temp4625; _temp4625.tag= Cyc_IdList_tok_tag;
_temp4625.f1=({ struct Cyc_List_List* _temp4626=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4626->hd=( void*)({ struct
_tagged_string* _temp4630=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4630[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4631= yyvs; struct _xtunion_struct** _temp4633= _temp4631.curr +
yyvsp_offset; if( _temp4631.base == 0? 1:( _temp4633 < _temp4631.base? 1:
_temp4633 >= _temp4631.last_plus_one)){ _throw( Null_Exception);}* _temp4633;}));
_temp4630;}); _temp4626->tl= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2
_temp4627= yyvs; struct _xtunion_struct** _temp4629= _temp4627.curr +(
yyvsp_offset - 2); if( _temp4627.base == 0? 1:( _temp4629 < _temp4627.base? 1:
_temp4629 >= _temp4627.last_plus_one)){ _throw( Null_Exception);}* _temp4629;}));
_temp4626;}); _temp4625;}); _temp4624;}); break; case 171: _LL4623: yyval=({
struct _tagged_ptr2 _temp4635= yyvs; struct _xtunion_struct** _temp4637=
_temp4635.curr + yyvsp_offset; if( _temp4635.base == 0? 1:( _temp4637 <
_temp4635.base? 1: _temp4637 >= _temp4635.last_plus_one)){ _throw(
Null_Exception);}* _temp4637;}); break; case 172: _LL4634: yyval=({ struct
_tagged_ptr2 _temp4639= yyvs; struct _xtunion_struct** _temp4641= _temp4639.curr
+ yyvsp_offset; if( _temp4639.base == 0? 1:( _temp4641 < _temp4639.base? 1:
_temp4641 >= _temp4639.last_plus_one)){ _throw( Null_Exception);}* _temp4641;});
break; case 173: _LL4638: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp4643=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4643[ 0]=({ struct Cyc_Exp_tok_struct _temp4644; _temp4644.tag= Cyc_Exp_tok_tag;
_temp4644.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4645=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4645[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp4646; _temp4646.tag= Cyc_Absyn_UnresolvedMem_e_tag; _temp4646.f1= 0;
_temp4646.f2= 0; _temp4646;}); _temp4645;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4647= yyls; struct Cyc_Yyltype* _temp4649= _temp4647.curr
+( yylsp_offset - 1); if( _temp4647.base == 0? 1:( _temp4649 < _temp4647.base? 1:
_temp4649 >= _temp4647.last_plus_one)){ _throw( Null_Exception);}* _temp4649;}).first_line,({
struct _tagged_ptr3 _temp4650= yyls; struct Cyc_Yyltype* _temp4652= _temp4650.curr
+ yylsp_offset; if( _temp4650.base == 0? 1:( _temp4652 < _temp4650.base? 1:
_temp4652 >= _temp4650.last_plus_one)){ _throw( Null_Exception);}* _temp4652;}).last_line));
_temp4644;}); _temp4643;}); break; case 174: _LL4642: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4654=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4654[ 0]=({ struct Cyc_Exp_tok_struct
_temp4655; _temp4655.tag= Cyc_Exp_tok_tag; _temp4655.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4656=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4656[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4657; _temp4657.tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4657.f1= 0; _temp4657.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4658= yyvs; struct _xtunion_struct** _temp4660= _temp4658.curr +(
yyvsp_offset - 1); if( _temp4658.base == 0? 1:( _temp4660 < _temp4658.base? 1:
_temp4660 >= _temp4658.last_plus_one)){ _throw( Null_Exception);}* _temp4660;})));
_temp4657;}); _temp4656;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4661= yyls; struct Cyc_Yyltype* _temp4663= _temp4661.curr +( yylsp_offset -
2); if( _temp4661.base == 0? 1:( _temp4663 < _temp4661.base? 1: _temp4663 >=
_temp4661.last_plus_one)){ _throw( Null_Exception);}* _temp4663;}).first_line,({
struct _tagged_ptr3 _temp4664= yyls; struct Cyc_Yyltype* _temp4666= _temp4664.curr
+ yylsp_offset; if( _temp4664.base == 0? 1:( _temp4666 < _temp4664.base? 1:
_temp4666 >= _temp4664.last_plus_one)){ _throw( Null_Exception);}* _temp4666;}).last_line));
_temp4655;}); _temp4654;}); break; case 175: _LL4653: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4668=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4668[ 0]=({ struct Cyc_Exp_tok_struct
_temp4669; _temp4669.tag= Cyc_Exp_tok_tag; _temp4669.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4670=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4670[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4671; _temp4671.tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4671.f1= 0; _temp4671.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4672= yyvs; struct _xtunion_struct** _temp4674= _temp4672.curr +(
yyvsp_offset - 2); if( _temp4672.base == 0? 1:( _temp4674 < _temp4672.base? 1:
_temp4674 >= _temp4672.last_plus_one)){ _throw( Null_Exception);}* _temp4674;})));
_temp4671;}); _temp4670;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4675= yyls; struct Cyc_Yyltype* _temp4677= _temp4675.curr +( yylsp_offset -
3); if( _temp4675.base == 0? 1:( _temp4677 < _temp4675.base? 1: _temp4677 >=
_temp4675.last_plus_one)){ _throw( Null_Exception);}* _temp4677;}).first_line,({
struct _tagged_ptr3 _temp4678= yyls; struct Cyc_Yyltype* _temp4680= _temp4678.curr
+ yylsp_offset; if( _temp4678.base == 0? 1:( _temp4680 < _temp4678.base? 1:
_temp4680 >= _temp4678.last_plus_one)){ _throw( Null_Exception);}* _temp4680;}).last_line));
_temp4669;}); _temp4668;}); break; case 176: _LL4667: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp4682=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4682[ 0]=({ struct Cyc_Exp_tok_struct
_temp4683; _temp4683.tag= Cyc_Exp_tok_tag; _temp4683.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Comprehension_e_struct* _temp4684=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp4684[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp4685; _temp4685.tag= Cyc_Absyn_Comprehension_e_tag;
_temp4685.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp4686=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4686->f1=( void*) Cyc_Absyn_Loc_n;
_temp4686->f2=({ struct _tagged_string* _temp4687=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4687[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4688= yyvs; struct _xtunion_struct** _temp4690=
_temp4688.curr +( yyvsp_offset - 5); if( _temp4688.base == 0? 1:( _temp4690 <
_temp4688.base? 1: _temp4690 >= _temp4688.last_plus_one)){ _throw(
Null_Exception);}* _temp4690;})); _temp4687;}); _temp4686;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4691= yyls; struct Cyc_Yyltype* _temp4693= _temp4691.curr
+( yylsp_offset - 5); if( _temp4691.base == 0? 1:( _temp4693 < _temp4691.base? 1:
_temp4693 >= _temp4691.last_plus_one)){ _throw( Null_Exception);}* _temp4693;}).first_line,({
struct _tagged_ptr3 _temp4694= yyls; struct Cyc_Yyltype* _temp4696= _temp4694.curr
+( yylsp_offset - 5); if( _temp4694.base == 0? 1:( _temp4696 < _temp4694.base? 1:
_temp4696 >= _temp4694.last_plus_one)){ _throw( Null_Exception);}* _temp4696;}).last_line)));
_temp4685.f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4697= yyvs; struct
_xtunion_struct** _temp4699= _temp4697.curr +( yyvsp_offset - 3); if( _temp4697.base
== 0? 1:( _temp4699 < _temp4697.base? 1: _temp4699 >= _temp4697.last_plus_one)){
_throw( Null_Exception);}* _temp4699;})); _temp4685.f3= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4700= yyvs; struct _xtunion_struct** _temp4702=
_temp4700.curr +( yyvsp_offset - 1); if( _temp4700.base == 0? 1:( _temp4702 <
_temp4700.base? 1: _temp4702 >= _temp4700.last_plus_one)){ _throw(
Null_Exception);}* _temp4702;})); _temp4685;}); _temp4684;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4703= yyls; struct Cyc_Yyltype* _temp4705= _temp4703.curr
+( yylsp_offset - 7); if( _temp4703.base == 0? 1:( _temp4705 < _temp4703.base? 1:
_temp4705 >= _temp4703.last_plus_one)){ _throw( Null_Exception);}* _temp4705;}).first_line,({
struct _tagged_ptr3 _temp4706= yyls; struct Cyc_Yyltype* _temp4708= _temp4706.curr
+ yylsp_offset; if( _temp4706.base == 0? 1:( _temp4708 < _temp4706.base? 1:
_temp4708 >= _temp4706.last_plus_one)){ _throw( Null_Exception);}* _temp4708;}).last_line));
_temp4683;}); _temp4682;}); break; case 177: _LL4681: yyval=( struct
_xtunion_struct*)({ struct Cyc_InitializerList_tok_struct* _temp4710=( struct
Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));
_temp4710[ 0]=({ struct Cyc_InitializerList_tok_struct _temp4711; _temp4711.tag=
Cyc_InitializerList_tok_tag; _temp4711.f1=({ struct Cyc_List_List* _temp4712=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4712->hd=(
void*)({ struct _tuple18* _temp4713=( struct _tuple18*) GC_malloc( sizeof(
struct _tuple18)); _temp4713->f1= 0; _temp4713->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4714= yyvs; struct _xtunion_struct** _temp4716= _temp4714.curr
+ yyvsp_offset; if( _temp4714.base == 0? 1:( _temp4716 < _temp4714.base? 1:
_temp4716 >= _temp4714.last_plus_one)){ _throw( Null_Exception);}* _temp4716;}));
_temp4713;}); _temp4712->tl= 0; _temp4712;}); _temp4711;}); _temp4710;}); break;
case 178: _LL4709: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4718=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4718[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4719; _temp4719.tag= Cyc_InitializerList_tok_tag; _temp4719.f1=({ struct
Cyc_List_List* _temp4720=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4720->hd=( void*)({ struct _tuple18* _temp4721=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4721->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4725= yyvs; struct _xtunion_struct** _temp4727=
_temp4725.curr +( yyvsp_offset - 1); if( _temp4725.base == 0? 1:( _temp4727 <
_temp4725.base? 1: _temp4727 >= _temp4725.last_plus_one)){ _throw(
Null_Exception);}* _temp4727;})); _temp4721->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4722= yyvs; struct _xtunion_struct** _temp4724= _temp4722.curr
+ yyvsp_offset; if( _temp4722.base == 0? 1:( _temp4724 < _temp4722.base? 1:
_temp4724 >= _temp4722.last_plus_one)){ _throw( Null_Exception);}* _temp4724;}));
_temp4721;}); _temp4720->tl= 0; _temp4720;}); _temp4719;}); _temp4718;}); break;
case 179: _LL4717: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4729=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4729[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4730; _temp4730.tag= Cyc_InitializerList_tok_tag; _temp4730.f1=({ struct
Cyc_List_List* _temp4731=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4731->hd=( void*)({ struct _tuple18* _temp4735=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4735->f1= 0; _temp4735->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4736= yyvs; struct _xtunion_struct** _temp4738=
_temp4736.curr + yyvsp_offset; if( _temp4736.base == 0? 1:( _temp4738 <
_temp4736.base? 1: _temp4738 >= _temp4736.last_plus_one)){ _throw(
Null_Exception);}* _temp4738;})); _temp4735;}); _temp4731->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4732= yyvs; struct _xtunion_struct** _temp4734=
_temp4732.curr +( yyvsp_offset - 2); if( _temp4732.base == 0? 1:( _temp4734 <
_temp4732.base? 1: _temp4734 >= _temp4732.last_plus_one)){ _throw(
Null_Exception);}* _temp4734;})); _temp4731;}); _temp4730;}); _temp4729;});
break; case 180: _LL4728: yyval=( struct _xtunion_struct*)({ struct Cyc_InitializerList_tok_struct*
_temp4740=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4740[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4741; _temp4741.tag= Cyc_InitializerList_tok_tag; _temp4741.f1=({ struct
Cyc_List_List* _temp4742=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4742->hd=( void*)({ struct _tuple18* _temp4746=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4746->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4750= yyvs; struct _xtunion_struct** _temp4752=
_temp4750.curr +( yyvsp_offset - 1); if( _temp4750.base == 0? 1:( _temp4752 <
_temp4750.base? 1: _temp4752 >= _temp4750.last_plus_one)){ _throw(
Null_Exception);}* _temp4752;})); _temp4746->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4747= yyvs; struct _xtunion_struct** _temp4749= _temp4747.curr
+ yyvsp_offset; if( _temp4747.base == 0? 1:( _temp4749 < _temp4747.base? 1:
_temp4749 >= _temp4747.last_plus_one)){ _throw( Null_Exception);}* _temp4749;}));
_temp4746;}); _temp4742->tl= Cyc_yyget_InitializerList_tok(({ struct
_tagged_ptr2 _temp4743= yyvs; struct _xtunion_struct** _temp4745= _temp4743.curr
+( yyvsp_offset - 3); if( _temp4743.base == 0? 1:( _temp4745 < _temp4743.base? 1:
_temp4745 >= _temp4743.last_plus_one)){ _throw( Null_Exception);}* _temp4745;}));
_temp4742;}); _temp4741;}); _temp4740;}); break; case 181: _LL4739: yyval=(
struct _xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct* _temp4754=(
struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4754[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4755; _temp4755.tag=
Cyc_DesignatorList_tok_tag; _temp4755.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2
_temp4756= yyvs; struct _xtunion_struct** _temp4758= _temp4756.curr +(
yyvsp_offset - 1); if( _temp4756.base == 0? 1:( _temp4758 < _temp4756.base? 1:
_temp4758 >= _temp4756.last_plus_one)){ _throw( Null_Exception);}* _temp4758;})));
_temp4755;}); _temp4754;}); break; case 182: _LL4753: yyval=( struct
_xtunion_struct*)({ struct Cyc_DesignatorList_tok_struct* _temp4760=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4760[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4761; _temp4761.tag= Cyc_DesignatorList_tok_tag;
_temp4761.f1=({ struct Cyc_List_List* _temp4762=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4762->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4763= yyvs; struct _xtunion_struct** _temp4765=
_temp4763.curr + yyvsp_offset; if( _temp4763.base == 0? 1:( _temp4765 <
_temp4763.base? 1: _temp4765 >= _temp4763.last_plus_one)){ _throw(
Null_Exception);}* _temp4765;})); _temp4762->tl= 0; _temp4762;}); _temp4761;});
_temp4760;}); break; case 183: _LL4759: yyval=( struct _xtunion_struct*)({
struct Cyc_DesignatorList_tok_struct* _temp4767=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4767[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4768; _temp4768.tag= Cyc_DesignatorList_tok_tag;
_temp4768.f1=({ struct Cyc_List_List* _temp4769=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4769->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4773= yyvs; struct _xtunion_struct** _temp4775=
_temp4773.curr + yyvsp_offset; if( _temp4773.base == 0? 1:( _temp4775 <
_temp4773.base? 1: _temp4775 >= _temp4773.last_plus_one)){ _throw(
Null_Exception);}* _temp4775;})); _temp4769->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4770= yyvs; struct _xtunion_struct** _temp4772=
_temp4770.curr +( yyvsp_offset - 1); if( _temp4770.base == 0? 1:( _temp4772 <
_temp4770.base? 1: _temp4772 >= _temp4770.last_plus_one)){ _throw(
Null_Exception);}* _temp4772;})); _temp4769;}); _temp4768;}); _temp4767;});
break; case 184: _LL4766: yyval=( struct _xtunion_struct*)({ struct Cyc_Designator_tok_struct*
_temp4777=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4777[ 0]=({ struct Cyc_Designator_tok_struct _temp4778; _temp4778.tag= Cyc_Designator_tok_tag;
_temp4778.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp4779=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4779[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp4780; _temp4780.tag=
Cyc_Absyn_ArrayElement_tag; _temp4780.f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4781= yyvs; struct _xtunion_struct** _temp4783= _temp4781.curr
+( yyvsp_offset - 1); if( _temp4781.base == 0? 1:( _temp4783 < _temp4781.base? 1:
_temp4783 >= _temp4781.last_plus_one)){ _throw( Null_Exception);}* _temp4783;}));
_temp4780;}); _temp4779;})); _temp4778;}); _temp4777;}); break; case 185:
_LL4776: yyval=( struct _xtunion_struct*)({ struct Cyc_Designator_tok_struct*
_temp4785=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4785[ 0]=({ struct Cyc_Designator_tok_struct _temp4786; _temp4786.tag= Cyc_Designator_tok_tag;
_temp4786.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4787=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4787[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4788; _temp4788.tag= Cyc_Absyn_FieldName_tag;
_temp4788.f1=({ struct _tagged_string* _temp4789=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4789[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4790= yyvs; struct _xtunion_struct** _temp4792=
_temp4790.curr + yyvsp_offset; if( _temp4790.base == 0? 1:( _temp4792 <
_temp4790.base? 1: _temp4792 >= _temp4790.last_plus_one)){ _throw(
Null_Exception);}* _temp4792;})); _temp4789;}); _temp4788;}); _temp4787;}));
_temp4786;}); _temp4785;}); break; case 186: _LL4784: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4794= yyvs; struct
_xtunion_struct** _temp4796= _temp4794.curr + yyvsp_offset; if( _temp4794.base
== 0? 1:( _temp4796 < _temp4794.base? 1: _temp4796 >= _temp4794.last_plus_one)){
_throw( Null_Exception);}* _temp4796;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4797= yyls; struct Cyc_Yyltype* _temp4799= _temp4797.curr
+ yylsp_offset; if( _temp4797.base == 0? 1:( _temp4799 < _temp4797.base? 1:
_temp4799 >= _temp4797.last_plus_one)){ _throw( Null_Exception);}* _temp4799;}).first_line,({
struct _tagged_ptr3 _temp4800= yyls; struct Cyc_Yyltype* _temp4802= _temp4800.curr
+ yylsp_offset; if( _temp4800.base == 0? 1:( _temp4802 < _temp4800.base? 1:
_temp4802 >= _temp4800.last_plus_one)){ _throw( Null_Exception);}* _temp4802;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4803= yyvs; struct _xtunion_struct** _temp4805= _temp4803.curr +
yyvsp_offset; if( _temp4803.base == 0? 1:( _temp4805 < _temp4803.base? 1:
_temp4805 >= _temp4803.last_plus_one)){ _throw( Null_Exception);}* _temp4805;}))).f3;
if( atts != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4806=( char*)"ignoring attributes in type";
struct _tagged_string _temp4807; _temp4807.curr= _temp4806; _temp4807.base=
_temp4806; _temp4807.last_plus_one= _temp4806 + 28; _temp4807;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4808= yyls; struct Cyc_Yyltype* _temp4810= _temp4808.curr
+ yylsp_offset; if( _temp4808.base == 0? 1:( _temp4810 < _temp4808.base? 1:
_temp4810 >= _temp4808.last_plus_one)){ _throw( Null_Exception);}* _temp4810;}).first_line,({
struct _tagged_ptr3 _temp4811= yyls; struct Cyc_Yyltype* _temp4813= _temp4811.curr
+ yylsp_offset; if( _temp4811.base == 0? 1:( _temp4813 < _temp4811.base? 1:
_temp4813 >= _temp4811.last_plus_one)){ _throw( Null_Exception);}* _temp4813;}).last_line));}{
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4814= yyvs; struct _xtunion_struct** _temp4816= _temp4814.curr +
yyvsp_offset; if( _temp4814.base == 0? 1:( _temp4816 < _temp4814.base? 1:
_temp4816 >= _temp4814.last_plus_one)){ _throw( Null_Exception);}* _temp4816;}))).f1;
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4817=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4817[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4818; _temp4818.tag= Cyc_ParamDecl_tok_tag;
_temp4818.f1=({ struct _tuple2* _temp4819=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp4819[ 0]=({ struct _tuple2 _temp4820; _temp4820.f1= 0;
_temp4820.f2= tq; _temp4820.f3= t; _temp4820;}); _temp4819;}); _temp4818;});
_temp4817;}); break;}} case 187: _LL4793: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4822= yyvs; struct
_xtunion_struct** _temp4824= _temp4822.curr +( yyvsp_offset - 1); if( _temp4822.base
== 0? 1:( _temp4824 < _temp4822.base? 1: _temp4824 >= _temp4822.last_plus_one)){
_throw( Null_Exception);}* _temp4824;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4825= yyls; struct Cyc_Yyltype* _temp4827= _temp4825.curr
+( yylsp_offset - 1); if( _temp4825.base == 0? 1:( _temp4827 < _temp4825.base? 1:
_temp4827 >= _temp4825.last_plus_one)){ _throw( Null_Exception);}* _temp4827;}).first_line,({
struct _tagged_ptr3 _temp4828= yyls; struct Cyc_Yyltype* _temp4830= _temp4828.curr
+( yylsp_offset - 1); if( _temp4828.base == 0? 1:( _temp4830 < _temp4828.base? 1:
_temp4830 >= _temp4828.last_plus_one)){ _throw( Null_Exception);}* _temp4830;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4831= yyvs; struct _xtunion_struct** _temp4833= _temp4831.curr +(
yyvsp_offset - 1); if( _temp4831.base == 0? 1:( _temp4833 < _temp4831.base? 1:
_temp4833 >= _temp4831.last_plus_one)){ _throw( Null_Exception);}* _temp4833;}))).f3;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4834= yyvs; struct _xtunion_struct** _temp4836= _temp4834.curr +(
yyvsp_offset - 1); if( _temp4834.base == 0? 1:( _temp4836 < _temp4834.base? 1:
_temp4836 >= _temp4834.last_plus_one)){ _throw( Null_Exception);}* _temp4836;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4837= yyvs; struct _xtunion_struct** _temp4839= _temp4837.curr
+ yyvsp_offset; if( _temp4837.base == 0? 1:( _temp4839 < _temp4837.base? 1:
_temp4839 >= _temp4837.last_plus_one)){ _throw( Null_Exception);}* _temp4839;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4840=( char*)"bad type params, ignoring";
struct _tagged_string _temp4841; _temp4841.curr= _temp4840; _temp4841.base=
_temp4840; _temp4841.last_plus_one= _temp4840 + 26; _temp4841;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4842= yyls; struct Cyc_Yyltype* _temp4844= _temp4842.curr
+ yylsp_offset; if( _temp4842.base == 0? 1:( _temp4844 < _temp4842.base? 1:
_temp4844 >= _temp4842.last_plus_one)){ _throw( Null_Exception);}* _temp4844;}).first_line,({
struct _tagged_ptr3 _temp4845= yyls; struct Cyc_Yyltype* _temp4847= _temp4845.curr
+ yylsp_offset; if( _temp4845.base == 0? 1:( _temp4847 < _temp4845.base? 1:
_temp4847 >= _temp4845.last_plus_one)){ _throw( Null_Exception);}* _temp4847;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4848=(
char*)"bad specifiers, ignoring"; struct _tagged_string _temp4849; _temp4849.curr=
_temp4848; _temp4849.base= _temp4848; _temp4849.last_plus_one= _temp4848 + 25;
_temp4849;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4850= yyls;
struct Cyc_Yyltype* _temp4852= _temp4850.curr + yylsp_offset; if( _temp4850.base
== 0? 1:( _temp4852 < _temp4850.base? 1: _temp4852 >= _temp4850.last_plus_one)){
_throw( Null_Exception);}* _temp4852;}).first_line,({ struct _tagged_ptr3
_temp4853= yyls; struct Cyc_Yyltype* _temp4855= _temp4853.curr + yylsp_offset;
if( _temp4853.base == 0? 1:( _temp4855 < _temp4853.base? 1: _temp4855 >=
_temp4853.last_plus_one)){ _throw( Null_Exception);}* _temp4855;}).last_line));}
yyval=( struct _xtunion_struct*)({ struct Cyc_ParamDecl_tok_struct* _temp4856=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4856[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4857; _temp4857.tag= Cyc_ParamDecl_tok_tag;
_temp4857.f1=({ struct _tuple2* _temp4858=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2)); _temp4858->f1= 0; _temp4858->f2= t_info.f1; _temp4858->f3=
t_info.f2; _temp4858;}); _temp4857;}); _temp4856;}); break;} case 188: _LL4821:
yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct* _temp4860=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4860[ 0]=({ struct Cyc_Type_tok_struct _temp4861; _temp4861.tag= Cyc_Type_tok_tag;
_temp4861.f1=( void*)(* Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4862=
yyvs; struct _xtunion_struct** _temp4864= _temp4862.curr + yyvsp_offset; if(
_temp4862.base == 0? 1:( _temp4864 < _temp4862.base? 1: _temp4864 >= _temp4862.last_plus_one)){
_throw( Null_Exception);}* _temp4864;}))).f3; _temp4861;}); _temp4860;}); break;
case 189: _LL4859: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4866=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4866[ 0]=({ struct Cyc_Type_tok_struct _temp4867; _temp4867.tag= Cyc_Type_tok_tag;
_temp4867.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4868=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4868[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4869; _temp4869.tag= Cyc_Absyn_JoinEff_tag;
_temp4869.f1= 0; _temp4869;}); _temp4868;})); _temp4867;}); _temp4866;}); break;
case 190: _LL4865: yyval=( struct _xtunion_struct*)({ struct Cyc_Type_tok_struct*
_temp4871=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4871[ 0]=({ struct Cyc_Type_tok_struct _temp4872; _temp4872.tag= Cyc_Type_tok_tag;
_temp4872.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4873=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4873[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4874; _temp4874.tag= Cyc_Absyn_JoinEff_tag;
_temp4874.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4875= yyvs;
struct _xtunion_struct** _temp4877= _temp4875.curr +( yyvsp_offset - 1); if(
_temp4875.base == 0? 1:( _temp4877 < _temp4875.base? 1: _temp4877 >= _temp4875.last_plus_one)){
_throw( Null_Exception);}* _temp4877;})); _temp4874;}); _temp4873;})); _temp4872;});
_temp4871;}); break; case 191: _LL4870: yyval=( struct _xtunion_struct*)({
struct Cyc_Type_tok_struct* _temp4879=( struct Cyc_Type_tok_struct*) GC_malloc(
sizeof( struct Cyc_Type_tok_struct)); _temp4879[ 0]=({ struct Cyc_Type_tok_struct
_temp4880; _temp4880.tag= Cyc_Type_tok_tag; _temp4880.f1=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp4881=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4881[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4882; _temp4882.tag= Cyc_Absyn_JoinEff_tag;
_temp4882.f1=({ struct Cyc_List_List* _temp4883=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4883->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4887= yyvs; struct _xtunion_struct** _temp4889=
_temp4887.curr +( yyvsp_offset - 2); if( _temp4887.base == 0? 1:( _temp4889 <
_temp4887.base? 1: _temp4889 >= _temp4887.last_plus_one)){ _throw(
Null_Exception);}* _temp4889;})); _temp4883->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4884= yyvs; struct _xtunion_struct** _temp4886=
_temp4884.curr + yyvsp_offset; if( _temp4884.base == 0? 1:( _temp4886 <
_temp4884.base? 1: _temp4886 >= _temp4884.last_plus_one)){ _throw(
Null_Exception);}* _temp4886;})); _temp4883;}); _temp4882;}); _temp4881;}));
_temp4880;}); _temp4879;}); break; case 192: _LL4878: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4891=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4891[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4892; _temp4892.tag= Cyc_TypeList_tok_tag; _temp4892.f1=({ struct Cyc_List_List*
_temp4893=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4893->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4894= yyvs;
struct _xtunion_struct** _temp4896= _temp4894.curr + yyvsp_offset; if( _temp4894.base
== 0? 1:( _temp4896 < _temp4894.base? 1: _temp4896 >= _temp4894.last_plus_one)){
_throw( Null_Exception);}* _temp4896;})); _temp4893->tl= 0; _temp4893;});
_temp4892;}); _temp4891;}); break; case 193: _LL4890: yyval=( struct
_xtunion_struct*)({ struct Cyc_TypeList_tok_struct* _temp4898=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4898[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4899; _temp4899.tag= Cyc_TypeList_tok_tag; _temp4899.f1=({ struct Cyc_List_List*
_temp4900=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4900->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4904= yyvs;
struct _xtunion_struct** _temp4906= _temp4904.curr + yyvsp_offset; if( _temp4904.base
== 0? 1:( _temp4906 < _temp4904.base? 1: _temp4906 >= _temp4904.last_plus_one)){
_throw( Null_Exception);}* _temp4906;})); _temp4900->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4901= yyvs; struct _xtunion_struct** _temp4903=
_temp4901.curr +( yyvsp_offset - 2); if( _temp4901.base == 0? 1:( _temp4903 <
_temp4901.base? 1: _temp4903 >= _temp4901.last_plus_one)){ _throw(
Null_Exception);}* _temp4903;})); _temp4900;}); _temp4899;}); _temp4898;});
break; case 194: _LL4897: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4908=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4908[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4909; _temp4909.tag= Cyc_AbstractDeclarator_tok_tag; _temp4909.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4910=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4910->tms= Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4911= yyvs; struct _xtunion_struct** _temp4913=
_temp4911.curr + yyvsp_offset; if( _temp4911.base == 0? 1:( _temp4913 <
_temp4911.base? 1: _temp4913 >= _temp4911.last_plus_one)){ _throw(
Null_Exception);}* _temp4913;})); _temp4910;}); _temp4909;}); _temp4908;});
break; case 195: _LL4907: yyval=({ struct _tagged_ptr2 _temp4915= yyvs; struct
_xtunion_struct** _temp4917= _temp4915.curr + yyvsp_offset; if( _temp4915.base
== 0? 1:( _temp4917 < _temp4915.base? 1: _temp4917 >= _temp4915.last_plus_one)){
_throw( Null_Exception);}* _temp4917;}); break; case 196: _LL4914: yyval=(
struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4919=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4919[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4920; _temp4920.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4920.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4921=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4921->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp4922= yyvs; struct _xtunion_struct** _temp4924= _temp4922.curr +(
yyvsp_offset - 1); if( _temp4922.base == 0? 1:( _temp4924 < _temp4922.base? 1:
_temp4924 >= _temp4922.last_plus_one)){ _throw( Null_Exception);}* _temp4924;})),(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4925= yyvs; struct
_xtunion_struct** _temp4927= _temp4925.curr + yyvsp_offset; if( _temp4925.base
== 0? 1:( _temp4927 < _temp4925.base? 1: _temp4927 >= _temp4925.last_plus_one)){
_throw( Null_Exception);}* _temp4927;})))->tms); _temp4921;}); _temp4920;});
_temp4919;}); break; case 197: _LL4918: yyval=({ struct _tagged_ptr2 _temp4929=
yyvs; struct _xtunion_struct** _temp4931= _temp4929.curr +( yyvsp_offset - 1);
if( _temp4929.base == 0? 1:( _temp4931 < _temp4929.base? 1: _temp4931 >=
_temp4929.last_plus_one)){ _throw( Null_Exception);}* _temp4931;}); break; case
198: _LL4928: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4933=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4933[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4934; _temp4934.tag= Cyc_AbstractDeclarator_tok_tag; _temp4934.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4935=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4935->tms=({
struct Cyc_List_List* _temp4936=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4936->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4936->tl= 0; _temp4936;}); _temp4935;}); _temp4934;}); _temp4933;}); break;
case 199: _LL4932: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4938=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4938[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4939; _temp4939.tag= Cyc_AbstractDeclarator_tok_tag; _temp4939.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4940=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4940->tms=({
struct Cyc_List_List* _temp4941=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4941->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4941->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4942= yyvs; struct _xtunion_struct** _temp4944= _temp4942.curr +(
yyvsp_offset - 2); if( _temp4942.base == 0? 1:( _temp4944 < _temp4942.base? 1:
_temp4944 >= _temp4942.last_plus_one)){ _throw( Null_Exception);}* _temp4944;})))->tms;
_temp4941;}); _temp4940;}); _temp4939;}); _temp4938;}); break; case 200: _LL4937:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4946=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4946[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4947; _temp4947.tag= Cyc_AbstractDeclarator_tok_tag; _temp4947.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4948=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4948->tms=({
struct Cyc_List_List* _temp4949=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4949->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4950=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4950[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4951; _temp4951.tag= Cyc_Absyn_ConstArray_mod_tag; _temp4951.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4952= yyvs; struct _xtunion_struct** _temp4954=
_temp4952.curr +( yyvsp_offset - 1); if( _temp4952.base == 0? 1:( _temp4954 <
_temp4952.base? 1: _temp4954 >= _temp4952.last_plus_one)){ _throw(
Null_Exception);}* _temp4954;})); _temp4951;}); _temp4950;})); _temp4949->tl= 0;
_temp4949;}); _temp4948;}); _temp4947;}); _temp4946;}); break; case 201: _LL4945:
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4956=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4956[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4957; _temp4957.tag= Cyc_AbstractDeclarator_tok_tag; _temp4957.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp4958=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4958->tms=({
struct Cyc_List_List* _temp4959=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4959->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4963=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4963[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4964; _temp4964.tag= Cyc_Absyn_ConstArray_mod_tag; _temp4964.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4965= yyvs; struct _xtunion_struct** _temp4967=
_temp4965.curr +( yyvsp_offset - 1); if( _temp4965.base == 0? 1:( _temp4967 <
_temp4965.base? 1: _temp4967 >= _temp4965.last_plus_one)){ _throw(
Null_Exception);}* _temp4967;})); _temp4964;}); _temp4963;})); _temp4959->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4960= yyvs; struct
_xtunion_struct** _temp4962= _temp4960.curr +( yyvsp_offset - 3); if( _temp4960.base
== 0? 1:( _temp4962 < _temp4960.base? 1: _temp4962 >= _temp4960.last_plus_one)){
_throw( Null_Exception);}* _temp4962;})))->tms; _temp4959;}); _temp4958;});
_temp4957;}); _temp4956;}); break; case 202: _LL4955: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4969=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4969[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4970; _temp4970.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4970.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4971=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4971->tms=({ struct Cyc_List_List* _temp4972=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4972->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4973=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4973[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4974; _temp4974.tag= Cyc_Absyn_Function_mod_tag;
_temp4974.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4975=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4975[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4976; _temp4976.tag= Cyc_Absyn_WithTypes_tag;
_temp4976.f1= 0; _temp4976.f2= 0; _temp4976.f3= 0; _temp4976;}); _temp4975;}));
_temp4974;}); _temp4973;})); _temp4972->tl= 0; _temp4972;}); _temp4971;});
_temp4970;}); _temp4969;}); break; case 203: _LL4968: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4978=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4978[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4979; _temp4979.tag=
Cyc_AbstractDeclarator_tok_tag; _temp4979.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4980=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4980->tms=({ struct Cyc_List_List* _temp4981=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4981->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4982=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4982[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4983; _temp4983.tag= Cyc_Absyn_Function_mod_tag;
_temp4983.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4984=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4984[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4985; _temp4985.tag= Cyc_Absyn_WithTypes_tag;
_temp4985.f1= 0; _temp4985.f2= 0; _temp4985.f3=({ struct Cyc_Core_Opt* _temp4986=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4986->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4987=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4987[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4988; _temp4988.tag= Cyc_Absyn_JoinEff_tag;
_temp4988.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4989= yyvs;
struct _xtunion_struct** _temp4991= _temp4989.curr +( yyvsp_offset - 1); if(
_temp4989.base == 0? 1:( _temp4991 < _temp4989.base? 1: _temp4991 >= _temp4989.last_plus_one)){
_throw( Null_Exception);}* _temp4991;})); _temp4988;}); _temp4987;})); _temp4986;});
_temp4985;}); _temp4984;})); _temp4983;}); _temp4982;})); _temp4981->tl= 0;
_temp4981;}); _temp4980;}); _temp4979;}); _temp4978;}); break; case 204: _LL4977: {
struct _tuple16 _temp4998; struct Cyc_Core_Opt* _temp4999; int _temp5001; struct
Cyc_List_List* _temp5003; struct _tuple16* _temp4996= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp4993= yyvs; struct _xtunion_struct** _temp4995=
_temp4993.curr +( yyvsp_offset - 1); if( _temp4993.base == 0? 1:( _temp4995 <
_temp4993.base? 1: _temp4995 >= _temp4993.last_plus_one)){ _throw(
Null_Exception);}* _temp4995;})); _temp4998=* _temp4996; _LL5004: _temp5003=
_temp4998.f1; goto _LL5002; _LL5002: _temp5001= _temp4998.f2; goto _LL5000;
_LL5000: _temp4999= _temp4998.f3; goto _LL4997; _LL4997: yyval=( struct
_xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5005=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5005[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5006; _temp5006.tag=
Cyc_AbstractDeclarator_tok_tag; _temp5006.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5007=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5007->tms=({ struct Cyc_List_List* _temp5008=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5008->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5009=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5009[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5010; _temp5010.tag= Cyc_Absyn_Function_mod_tag;
_temp5010.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5011=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5011[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5012; _temp5012.tag= Cyc_Absyn_WithTypes_tag;
_temp5012.f1= _temp5003; _temp5012.f2= _temp5001; _temp5012.f3= _temp4999;
_temp5012;}); _temp5011;})); _temp5010;}); _temp5009;})); _temp5008->tl= 0;
_temp5008;}); _temp5007;}); _temp5006;}); _temp5005;}); break;} case 205:
_LL4992: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5014=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5014[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5015; _temp5015.tag= Cyc_AbstractDeclarator_tok_tag; _temp5015.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5016=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5016->tms=({
struct Cyc_List_List* _temp5017=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5017->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5021=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5021[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5022; _temp5022.tag=
Cyc_Absyn_Function_mod_tag; _temp5022.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5023=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5023[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5024; _temp5024.tag= Cyc_Absyn_WithTypes_tag;
_temp5024.f1= 0; _temp5024.f2= 0; _temp5024.f3= 0; _temp5024;}); _temp5023;}));
_temp5022;}); _temp5021;})); _temp5017->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5018= yyvs; struct _xtunion_struct** _temp5020=
_temp5018.curr +( yyvsp_offset - 2); if( _temp5018.base == 0? 1:( _temp5020 <
_temp5018.base? 1: _temp5020 >= _temp5018.last_plus_one)){ _throw(
Null_Exception);}* _temp5020;})))->tms; _temp5017;}); _temp5016;}); _temp5015;});
_temp5014;}); break; case 206: _LL5013: { struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt*
_temp5026=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5026->v=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp5027=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp5027[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp5028; _temp5028.tag= Cyc_Absyn_JoinEff_tag;
_temp5028.f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5029= yyvs;
struct _xtunion_struct** _temp5031= _temp5029.curr +( yyvsp_offset - 1); if(
_temp5029.base == 0? 1:( _temp5031 < _temp5029.base? 1: _temp5031 >= _temp5029.last_plus_one)){
_throw( Null_Exception);}* _temp5031;})); _temp5028;}); _temp5027;})); _temp5026;});
yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5032=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5032[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5033; _temp5033.tag= Cyc_AbstractDeclarator_tok_tag; _temp5033.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5034=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5034->tms=({
struct Cyc_List_List* _temp5035=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5035->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5039=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5039[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5040; _temp5040.tag=
Cyc_Absyn_Function_mod_tag; _temp5040.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5041=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5041[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5042; _temp5042.tag= Cyc_Absyn_WithTypes_tag;
_temp5042.f1= 0; _temp5042.f2= 0; _temp5042.f3= eff; _temp5042;}); _temp5041;}));
_temp5040;}); _temp5039;})); _temp5035->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5036= yyvs; struct _xtunion_struct** _temp5038=
_temp5036.curr +( yyvsp_offset - 4); if( _temp5036.base == 0? 1:( _temp5038 <
_temp5036.base? 1: _temp5038 >= _temp5036.last_plus_one)){ _throw(
Null_Exception);}* _temp5038;})))->tms; _temp5035;}); _temp5034;}); _temp5033;});
_temp5032;}); break;} case 207: _LL5025: { struct _tuple16 _temp5049; struct Cyc_Core_Opt*
_temp5050; int _temp5052; struct Cyc_List_List* _temp5054; struct _tuple16*
_temp5047= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp5044=
yyvs; struct _xtunion_struct** _temp5046= _temp5044.curr +( yyvsp_offset - 1);
if( _temp5044.base == 0? 1:( _temp5046 < _temp5044.base? 1: _temp5046 >=
_temp5044.last_plus_one)){ _throw( Null_Exception);}* _temp5046;})); _temp5049=*
_temp5047; _LL5055: _temp5054= _temp5049.f1; goto _LL5053; _LL5053: _temp5052=
_temp5049.f2; goto _LL5051; _LL5051: _temp5050= _temp5049.f3; goto _LL5048;
_LL5048: yyval=( struct _xtunion_struct*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5056=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5056[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5057; _temp5057.tag= Cyc_AbstractDeclarator_tok_tag; _temp5057.f1=({ struct
Cyc_Parse_Abstractdeclarator* _temp5058=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5058->tms=({
struct Cyc_List_List* _temp5059=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5059->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5063=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5063[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5064; _temp5064.tag=
Cyc_Absyn_Function_mod_tag; _temp5064.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5065=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5065[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5066; _temp5066.tag= Cyc_Absyn_WithTypes_tag;
_temp5066.f1= _temp5054; _temp5066.f2= _temp5052; _temp5066.f3= _temp5050;
_temp5066;}); _temp5065;})); _temp5064;}); _temp5063;})); _temp5059->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5060= yyvs; struct _xtunion_struct** _temp5062=
_temp5060.curr +( yyvsp_offset - 3); if( _temp5060.base == 0? 1:( _temp5062 <
_temp5060.base? 1: _temp5062 >= _temp5060.last_plus_one)){ _throw(
Null_Exception);}* _temp5062;})))->tms; _temp5059;}); _temp5058;}); _temp5057;});
_temp5056;}); break;} case 208: _LL5043: { struct Cyc_List_List* ts=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5068=
yyls; struct Cyc_Yyltype* _temp5070= _temp5068.curr +( yylsp_offset - 2); if(
_temp5068.base == 0? 1:( _temp5070 < _temp5068.base? 1: _temp5070 >= _temp5068.last_plus_one)){
_throw( Null_Exception);}* _temp5070;}).first_line,({ struct _tagged_ptr3
_temp5071= yyls; struct Cyc_Yyltype* _temp5073= _temp5071.curr + yylsp_offset;
if( _temp5071.base == 0? 1:( _temp5073 < _temp5071.base? 1: _temp5073 >=
_temp5071.last_plus_one)){ _throw( Null_Exception);}* _temp5073;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5074= yyvs; struct _xtunion_struct** _temp5076=
_temp5074.curr +( yyvsp_offset - 1); if( _temp5074.base == 0? 1:( _temp5076 <
_temp5074.base? 1: _temp5076 >= _temp5074.last_plus_one)){ _throw(
Null_Exception);}* _temp5076;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5077=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5077[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5078; _temp5078.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5078.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5079=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5079->tms=({
struct Cyc_List_List* _temp5080=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5080->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5084=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5084[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5085; _temp5085.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5085.f1= ts;
_temp5085.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5086= yyls;
struct Cyc_Yyltype* _temp5088= _temp5086.curr +( yylsp_offset - 2); if(
_temp5086.base == 0? 1:( _temp5088 < _temp5086.base? 1: _temp5088 >= _temp5086.last_plus_one)){
_throw( Null_Exception);}* _temp5088;}).first_line,({ struct _tagged_ptr3
_temp5089= yyls; struct Cyc_Yyltype* _temp5091= _temp5089.curr + yylsp_offset;
if( _temp5089.base == 0? 1:( _temp5091 < _temp5089.base? 1: _temp5091 >=
_temp5089.last_plus_one)){ _throw( Null_Exception);}* _temp5091;}).last_line);
_temp5085.f3= 0; _temp5085;}); _temp5084;})); _temp5080->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5081= yyvs; struct _xtunion_struct** _temp5083=
_temp5081.curr +( yyvsp_offset - 3); if( _temp5081.base == 0? 1:( _temp5083 <
_temp5081.base? 1: _temp5083 >= _temp5081.last_plus_one)){ _throw(
Null_Exception);}* _temp5083;})))->tms; _temp5080;}); _temp5079;}); _temp5078;});
_temp5077;}); break;} case 209: _LL5067:(( struct Cyc_Lexing_lexbuf*)({ struct
Cyc_Core_Opt* _temp5093= Cyc_Parse_lbuf; if( _temp5093 == 0){ _throw(
Null_Exception);} _temp5093->v;}))->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5094=
yyls; struct Cyc_Yyltype* _temp5096= _temp5094.curr +( yylsp_offset - 2); if(
_temp5094.base == 0? 1:( _temp5096 < _temp5094.base? 1: _temp5096 >= _temp5094.last_plus_one)){
_throw( Null_Exception);}* _temp5096;}).first_line,({ struct _tagged_ptr3
_temp5097= yyls; struct Cyc_Yyltype* _temp5099= _temp5097.curr + yylsp_offset;
if( _temp5097.base == 0? 1:( _temp5099 < _temp5097.base? 1: _temp5099 >=
_temp5097.last_plus_one)){ _throw( Null_Exception);}* _temp5099;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5100= yyvs; struct _xtunion_struct** _temp5102=
_temp5100.curr +( yyvsp_offset - 1); if( _temp5100.base == 0? 1:( _temp5102 <
_temp5100.base? 1: _temp5102 >= _temp5100.last_plus_one)){ _throw(
Null_Exception);}* _temp5102;})))); yyval=( struct _xtunion_struct*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5103=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5103[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5104; _temp5104.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5104.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5105=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5105->tms=({
struct Cyc_List_List* _temp5106=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5106->hd=( void*)(( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp5110=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp5110[ 0]=({ struct Cyc_Absyn_TypeParams_mod_struct
_temp5111; _temp5111.tag= Cyc_Absyn_TypeParams_mod_tag; _temp5111.f1= ts;
_temp5111.f2= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5112= yyls;
struct Cyc_Yyltype* _temp5114= _temp5112.curr +( yylsp_offset - 2); if(
_temp5112.base == 0? 1:( _temp5114 < _temp5112.base? 1: _temp5114 >= _temp5112.last_plus_one)){
_throw( Null_Exception);}* _temp5114;}).first_line,({ struct _tagged_ptr3
_temp5115= yyls; struct Cyc_Yyltype* _temp5117= _temp5115.curr + yylsp_offset;
if( _temp5115.base == 0? 1:( _temp5117 < _temp5115.base? 1: _temp5117 >=
_temp5115.last_plus_one)){ _throw( Null_Exception);}* _temp5117;}).last_line);
_temp5111.f3= 0; _temp5111;}); _temp5110;})); _temp5106->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5107= yyvs; struct _xtunion_struct** _temp5109=
_temp5107.curr +( yyvsp_offset - 3); if( _temp5107.base == 0? 1:( _temp5109 <
_temp5107.base? 1: _temp5109 >= _temp5107.last_plus_one)){ _throw(
Null_Exception);}* _temp5109;})))->tms; _temp5106;}); _temp5105;}); _temp5104;});
_temp5103;}); break;} case 210: _LL5092: yyval=( struct _xtunion_struct*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp5119=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5119[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5120; _temp5120.tag= Cyc_AbstractDeclarator_tok_tag;
_temp5120.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5121=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5121->tms=({
struct Cyc_List_List* _temp5122=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5122->hd=( void*)(( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp5126=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp5126[ 0]=({ struct Cyc_Absyn_Attributes_mod_struct
_temp5127; _temp5127.tag= Cyc_Absyn_Attributes_mod_tag; _temp5127.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5128= yyls; struct Cyc_Yyltype* _temp5130= _temp5128.curr
+ yylsp_offset; if( _temp5128.base == 0? 1:( _temp5130 < _temp5128.base? 1:
_temp5130 >= _temp5128.last_plus_one)){ _throw( Null_Exception);}* _temp5130;}).first_line,({
struct _tagged_ptr3 _temp5131= yyls; struct Cyc_Yyltype* _temp5133= _temp5131.curr
+ yylsp_offset; if( _temp5131.base == 0? 1:( _temp5133 < _temp5131.base? 1:
_temp5133 >= _temp5131.last_plus_one)){ _throw( Null_Exception);}* _temp5133;}).last_line);
_temp5127.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp5134= yyvs;
struct _xtunion_struct** _temp5136= _temp5134.curr + yyvsp_offset; if( _temp5134.base
== 0? 1:( _temp5136 < _temp5134.base? 1: _temp5136 >= _temp5134.last_plus_one)){
_throw( Null_Exception);}* _temp5136;})); _temp5127;}); _temp5126;})); _temp5122->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp5123= yyvs; struct
_xtunion_struct** _temp5125= _temp5123.curr +( yyvsp_offset - 1); if( _temp5123.base
== 0? 1:( _temp5125 < _temp5123.base? 1: _temp5125 >= _temp5123.last_plus_one)){
_throw( Null_Exception);}* _temp5125;})))->tms; _temp5122;}); _temp5121;});
_temp5120;}); _temp5119;}); break; case 211: _LL5118: yyval=({ struct
_tagged_ptr2 _temp5138= yyvs; struct _xtunion_struct** _temp5140= _temp5138.curr
+ yyvsp_offset; if( _temp5138.base == 0? 1:( _temp5140 < _temp5138.base? 1:
_temp5140 >= _temp5138.last_plus_one)){ _throw( Null_Exception);}* _temp5140;});
break; case 212: _LL5137: yyval=({ struct _tagged_ptr2 _temp5142= yyvs; struct
_xtunion_struct** _temp5144= _temp5142.curr + yyvsp_offset; if( _temp5142.base
== 0? 1:( _temp5144 < _temp5142.base? 1: _temp5144 >= _temp5142.last_plus_one)){
_throw( Null_Exception);}* _temp5144;}); break; case 213: _LL5141: yyval=({
struct _tagged_ptr2 _temp5146= yyvs; struct _xtunion_struct** _temp5148=
_temp5146.curr + yyvsp_offset; if( _temp5146.base == 0? 1:( _temp5148 <
_temp5146.base? 1: _temp5148 >= _temp5146.last_plus_one)){ _throw(
Null_Exception);}* _temp5148;}); break; case 214: _LL5145: yyval=({ struct
_tagged_ptr2 _temp5150= yyvs; struct _xtunion_struct** _temp5152= _temp5150.curr
+ yyvsp_offset; if( _temp5150.base == 0? 1:( _temp5152 < _temp5150.base? 1:
_temp5152 >= _temp5150.last_plus_one)){ _throw( Null_Exception);}* _temp5152;});
break; case 215: _LL5149: yyval=({ struct _tagged_ptr2 _temp5154= yyvs; struct
_xtunion_struct** _temp5156= _temp5154.curr + yyvsp_offset; if( _temp5154.base
== 0? 1:( _temp5156 < _temp5154.base? 1: _temp5156 >= _temp5154.last_plus_one)){
_throw( Null_Exception);}* _temp5156;}); break; case 216: _LL5153: yyval=({
struct _tagged_ptr2 _temp5158= yyvs; struct _xtunion_struct** _temp5160=
_temp5158.curr + yyvsp_offset; if( _temp5158.base == 0? 1:( _temp5160 <
_temp5158.base? 1: _temp5160 >= _temp5158.last_plus_one)){ _throw(
Null_Exception);}* _temp5160;}); break; case 217: _LL5157: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5162=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5162[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5163; _temp5163.tag= Cyc_Stmt_tok_tag; _temp5163.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Cut_s_struct* _temp5164=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp5164[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp5165; _temp5165.tag= Cyc_Absyn_Cut_s_tag; _temp5165.f1= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5166= yyvs; struct _xtunion_struct** _temp5168=
_temp5166.curr + yyvsp_offset; if( _temp5166.base == 0? 1:( _temp5168 <
_temp5166.base? 1: _temp5168 >= _temp5166.last_plus_one)){ _throw(
Null_Exception);}* _temp5168;})); _temp5165;}); _temp5164;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5169= yyls; struct Cyc_Yyltype* _temp5171= _temp5169.curr
+( yylsp_offset - 1); if( _temp5169.base == 0? 1:( _temp5171 < _temp5169.base? 1:
_temp5171 >= _temp5169.last_plus_one)){ _throw( Null_Exception);}* _temp5171;}).first_line,({
struct _tagged_ptr3 _temp5172= yyls; struct Cyc_Yyltype* _temp5174= _temp5172.curr
+ yylsp_offset; if( _temp5172.base == 0? 1:( _temp5174 < _temp5172.base? 1:
_temp5174 >= _temp5172.last_plus_one)){ _throw( Null_Exception);}* _temp5174;}).last_line));
_temp5163;}); _temp5162;}); break; case 218: _LL5161: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5176=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5176[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5177; _temp5177.tag= Cyc_Stmt_tok_tag; _temp5177.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Splice_s_struct* _temp5178=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp5178[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp5179; _temp5179.tag= Cyc_Absyn_Splice_s_tag;
_temp5179.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5180= yyvs; struct
_xtunion_struct** _temp5182= _temp5180.curr + yyvsp_offset; if( _temp5180.base
== 0? 1:( _temp5182 < _temp5180.base? 1: _temp5182 >= _temp5180.last_plus_one)){
_throw( Null_Exception);}* _temp5182;})); _temp5179;}); _temp5178;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5183= yyls; struct Cyc_Yyltype* _temp5185= _temp5183.curr
+( yylsp_offset - 1); if( _temp5183.base == 0? 1:( _temp5185 < _temp5183.base? 1:
_temp5185 >= _temp5183.last_plus_one)){ _throw( Null_Exception);}* _temp5185;}).first_line,({
struct _tagged_ptr3 _temp5186= yyls; struct Cyc_Yyltype* _temp5188= _temp5186.curr
+ yylsp_offset; if( _temp5186.base == 0? 1:( _temp5188 < _temp5186.base? 1:
_temp5188 >= _temp5186.last_plus_one)){ _throw( Null_Exception);}* _temp5188;}).last_line));
_temp5177;}); _temp5176;}); break; case 219: _LL5175: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5190=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5190[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5191; _temp5191.tag= Cyc_Stmt_tok_tag; _temp5191.f1= Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Label_s_struct* _temp5192=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp5192[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp5193; _temp5193.tag= Cyc_Absyn_Label_s_tag;
_temp5193.f1=({ struct _tagged_string* _temp5194=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5194[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5195= yyvs; struct _xtunion_struct** _temp5197=
_temp5195.curr +( yyvsp_offset - 2); if( _temp5195.base == 0? 1:( _temp5197 <
_temp5195.base? 1: _temp5197 >= _temp5195.last_plus_one)){ _throw(
Null_Exception);}* _temp5197;})); _temp5194;}); _temp5193.f2= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5198= yyvs; struct _xtunion_struct** _temp5200=
_temp5198.curr + yyvsp_offset; if( _temp5198.base == 0? 1:( _temp5200 <
_temp5198.base? 1: _temp5200 >= _temp5198.last_plus_one)){ _throw(
Null_Exception);}* _temp5200;})); _temp5193;}); _temp5192;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5201= yyls; struct Cyc_Yyltype* _temp5203= _temp5201.curr
+( yylsp_offset - 2); if( _temp5201.base == 0? 1:( _temp5203 < _temp5201.base? 1:
_temp5203 >= _temp5201.last_plus_one)){ _throw( Null_Exception);}* _temp5203;}).first_line,({
struct _tagged_ptr3 _temp5204= yyls; struct Cyc_Yyltype* _temp5206= _temp5204.curr
+ yylsp_offset; if( _temp5204.base == 0? 1:( _temp5206 < _temp5204.base? 1:
_temp5206 >= _temp5204.last_plus_one)){ _throw( Null_Exception);}* _temp5206;}).last_line));
_temp5191;}); _temp5190;}); break; case 220: _LL5189: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5208=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5208[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5209; _temp5209.tag= Cyc_Stmt_tok_tag; _temp5209.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5210= yyls; struct Cyc_Yyltype*
_temp5212= _temp5210.curr + yylsp_offset; if( _temp5210.base == 0? 1:( _temp5212
< _temp5210.base? 1: _temp5212 >= _temp5210.last_plus_one)){ _throw(
Null_Exception);}* _temp5212;}).first_line,({ struct _tagged_ptr3 _temp5213=
yyls; struct Cyc_Yyltype* _temp5215= _temp5213.curr + yylsp_offset; if(
_temp5213.base == 0? 1:( _temp5215 < _temp5213.base? 1: _temp5215 >= _temp5213.last_plus_one)){
_throw( Null_Exception);}* _temp5215;}).last_line)); _temp5209;}); _temp5208;});
break; case 221: _LL5207: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5217=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5217[ 0]=({ struct Cyc_Stmt_tok_struct _temp5218; _temp5218.tag= Cyc_Stmt_tok_tag;
_temp5218.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5219= yyvs; struct _xtunion_struct** _temp5221= _temp5219.curr +(
yyvsp_offset - 1); if( _temp5219.base == 0? 1:( _temp5221 < _temp5219.base? 1:
_temp5221 >= _temp5219.last_plus_one)){ _throw( Null_Exception);}* _temp5221;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5222= yyls; struct Cyc_Yyltype*
_temp5224= _temp5222.curr +( yylsp_offset - 1); if( _temp5222.base == 0? 1:(
_temp5224 < _temp5222.base? 1: _temp5224 >= _temp5222.last_plus_one)){ _throw(
Null_Exception);}* _temp5224;}).first_line,({ struct _tagged_ptr3 _temp5225=
yyls; struct Cyc_Yyltype* _temp5227= _temp5225.curr + yylsp_offset; if(
_temp5225.base == 0? 1:( _temp5227 < _temp5225.base? 1: _temp5227 >= _temp5225.last_plus_one)){
_throw( Null_Exception);}* _temp5227;}).last_line)); _temp5218;}); _temp5217;});
break; case 222: _LL5216: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5229=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5229[ 0]=({ struct Cyc_Stmt_tok_struct _temp5230; _temp5230.tag= Cyc_Stmt_tok_tag;
_temp5230.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5231= yyls; struct Cyc_Yyltype* _temp5233= _temp5231.curr +(
yylsp_offset - 1); if( _temp5231.base == 0? 1:( _temp5233 < _temp5231.base? 1:
_temp5233 >= _temp5231.last_plus_one)){ _throw( Null_Exception);}* _temp5233;}).first_line,({
struct _tagged_ptr3 _temp5234= yyls; struct Cyc_Yyltype* _temp5236= _temp5234.curr
+ yylsp_offset; if( _temp5234.base == 0? 1:( _temp5236 < _temp5234.base? 1:
_temp5236 >= _temp5234.last_plus_one)){ _throw( Null_Exception);}* _temp5236;}).last_line));
_temp5230;}); _temp5229;}); break; case 223: _LL5228: yyval=({ struct
_tagged_ptr2 _temp5238= yyvs; struct _xtunion_struct** _temp5240= _temp5238.curr
+( yyvsp_offset - 1); if( _temp5238.base == 0? 1:( _temp5240 < _temp5238.base? 1:
_temp5240 >= _temp5238.last_plus_one)){ _throw( Null_Exception);}* _temp5240;});
break; case 224: _LL5237:{ void* _temp5245= Cyc_yyget_BlockItem_tok(({ struct
_tagged_ptr2 _temp5242= yyvs; struct _xtunion_struct** _temp5244= _temp5242.curr
+ yyvsp_offset; if( _temp5242.base == 0? 1:( _temp5244 < _temp5242.base? 1:
_temp5244 >= _temp5242.last_plus_one)){ _throw( Null_Exception);}* _temp5244;}));
struct Cyc_List_List* _temp5253; struct Cyc_Absyn_Fndecl* _temp5255; struct Cyc_Absyn_Stmt*
_temp5257; _LL5247: if((( struct _tunion_struct*) _temp5245)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5254: _temp5253=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5245)->f1; goto _LL5248;} else{ goto _LL5249;} _LL5249: if((( struct
_tunion_struct*) _temp5245)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5256: _temp5255=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5245)->f1;
goto _LL5250;} else{ goto _LL5251;} _LL5251: if((( struct _tunion_struct*)
_temp5245)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5258: _temp5257=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5245)->f1; goto _LL5252;} else{ goto
_LL5246;} _LL5248: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5259=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5259[ 0]=({ struct Cyc_Stmt_tok_struct _temp5260; _temp5260.tag= Cyc_Stmt_tok_tag;
_temp5260.f1= Cyc_Parse_flatten_declarations( _temp5253, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5261= yyls; struct Cyc_Yyltype*
_temp5263= _temp5261.curr + yylsp_offset; if( _temp5261.base == 0? 1:( _temp5263
< _temp5261.base? 1: _temp5263 >= _temp5261.last_plus_one)){ _throw(
Null_Exception);}* _temp5263;}).first_line,({ struct _tagged_ptr3 _temp5264=
yyls; struct Cyc_Yyltype* _temp5266= _temp5264.curr + yylsp_offset; if(
_temp5264.base == 0? 1:( _temp5266 < _temp5264.base? 1: _temp5266 >= _temp5264.last_plus_one)){
_throw( Null_Exception);}* _temp5266;}).last_line))); _temp5260;}); _temp5259;});
goto _LL5246; _LL5250: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5267=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5267[ 0]=({ struct Cyc_Stmt_tok_struct _temp5268; _temp5268.tag= Cyc_Stmt_tok_tag;
_temp5268.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5269=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5269[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5270; _temp5270.tag= Cyc_Absyn_Fn_d_tag;
_temp5270.f1= _temp5255; _temp5270;}); _temp5269;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5271= yyls; struct Cyc_Yyltype* _temp5273= _temp5271.curr
+ yylsp_offset; if( _temp5271.base == 0? 1:( _temp5273 < _temp5271.base? 1:
_temp5273 >= _temp5271.last_plus_one)){ _throw( Null_Exception);}* _temp5273;}).first_line,({
struct _tagged_ptr3 _temp5274= yyls; struct Cyc_Yyltype* _temp5276= _temp5274.curr
+ yylsp_offset; if( _temp5274.base == 0? 1:( _temp5276 < _temp5274.base? 1:
_temp5276 >= _temp5274.last_plus_one)){ _throw( Null_Exception);}* _temp5276;}).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5277= yyls; struct Cyc_Yyltype* _temp5279= _temp5277.curr + yylsp_offset;
if( _temp5277.base == 0? 1:( _temp5279 < _temp5277.base? 1: _temp5279 >=
_temp5277.last_plus_one)){ _throw( Null_Exception);}* _temp5279;}).first_line,({
struct _tagged_ptr3 _temp5280= yyls; struct Cyc_Yyltype* _temp5282= _temp5280.curr
+ yylsp_offset; if( _temp5280.base == 0? 1:( _temp5282 < _temp5280.base? 1:
_temp5282 >= _temp5280.last_plus_one)){ _throw( Null_Exception);}* _temp5282;}).last_line)));
_temp5268;}); _temp5267;}); goto _LL5246; _LL5252: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5283=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5283[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5284; _temp5284.tag= Cyc_Stmt_tok_tag; _temp5284.f1= _temp5257; _temp5284;});
_temp5283;}); goto _LL5246; _LL5246:;} break; case 225: _LL5241:{ void*
_temp5289= Cyc_yyget_BlockItem_tok(({ struct _tagged_ptr2 _temp5286= yyvs;
struct _xtunion_struct** _temp5288= _temp5286.curr +( yyvsp_offset - 1); if(
_temp5286.base == 0? 1:( _temp5288 < _temp5286.base? 1: _temp5288 >= _temp5286.last_plus_one)){
_throw( Null_Exception);}* _temp5288;})); struct Cyc_List_List* _temp5297;
struct Cyc_Absyn_Fndecl* _temp5299; struct Cyc_Absyn_Stmt* _temp5301; _LL5291:
if((( struct _tunion_struct*) _temp5289)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL5298: _temp5297=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp5289)->f1; goto _LL5292;} else{ goto _LL5293;} _LL5293: if((( struct
_tunion_struct*) _temp5289)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL5300: _temp5299=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp5289)->f1;
goto _LL5294;} else{ goto _LL5295;} _LL5295: if((( struct _tunion_struct*)
_temp5289)->tag == Cyc_Parse_Stmt_bl_tag){ _LL5302: _temp5301=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5289)->f1; goto _LL5296;} else{ goto
_LL5290;} _LL5292: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5303=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5303[ 0]=({ struct Cyc_Stmt_tok_struct _temp5304; _temp5304.tag= Cyc_Stmt_tok_tag;
_temp5304.f1= Cyc_Parse_flatten_declarations( _temp5297, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5305= yyvs; struct _xtunion_struct** _temp5307=
_temp5305.curr + yyvsp_offset; if( _temp5305.base == 0? 1:( _temp5307 <
_temp5305.base? 1: _temp5307 >= _temp5305.last_plus_one)){ _throw(
Null_Exception);}* _temp5307;}))); _temp5304;}); _temp5303;}); goto _LL5290;
_LL5294: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5308=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5308[ 0]=({ struct Cyc_Stmt_tok_struct _temp5309; _temp5309.tag= Cyc_Stmt_tok_tag;
_temp5309.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5310=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5310[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5311; _temp5311.tag= Cyc_Absyn_Fn_d_tag;
_temp5311.f1= _temp5299; _temp5311;}); _temp5310;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5312= yyls; struct Cyc_Yyltype* _temp5314= _temp5312.curr
+( yylsp_offset - 1); if( _temp5312.base == 0? 1:( _temp5314 < _temp5312.base? 1:
_temp5314 >= _temp5312.last_plus_one)){ _throw( Null_Exception);}* _temp5314;}).first_line,({
struct _tagged_ptr3 _temp5315= yyls; struct Cyc_Yyltype* _temp5317= _temp5315.curr
+( yylsp_offset - 1); if( _temp5315.base == 0? 1:( _temp5317 < _temp5315.base? 1:
_temp5317 >= _temp5315.last_plus_one)){ _throw( Null_Exception);}* _temp5317;}).last_line)),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5318= yyvs; struct
_xtunion_struct** _temp5320= _temp5318.curr + yyvsp_offset; if( _temp5318.base
== 0? 1:( _temp5320 < _temp5318.base? 1: _temp5320 >= _temp5318.last_plus_one)){
_throw( Null_Exception);}* _temp5320;}))); _temp5309;}); _temp5308;}); goto
_LL5290; _LL5296: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5321=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5321[ 0]=({ struct Cyc_Stmt_tok_struct _temp5322; _temp5322.tag= Cyc_Stmt_tok_tag;
_temp5322.f1= Cyc_Absyn_seq_stmt( _temp5301, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5323= yyvs; struct _xtunion_struct** _temp5325= _temp5323.curr
+ yyvsp_offset; if( _temp5323.base == 0? 1:( _temp5325 < _temp5323.base? 1:
_temp5325 >= _temp5323.last_plus_one)){ _throw( Null_Exception);}* _temp5325;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5326= yyls; struct Cyc_Yyltype*
_temp5328= _temp5326.curr +( yylsp_offset - 1); if( _temp5326.base == 0? 1:(
_temp5328 < _temp5326.base? 1: _temp5328 >= _temp5326.last_plus_one)){ _throw(
Null_Exception);}* _temp5328;}).first_line,({ struct _tagged_ptr3 _temp5329=
yyls; struct Cyc_Yyltype* _temp5331= _temp5329.curr + yylsp_offset; if(
_temp5329.base == 0? 1:( _temp5331 < _temp5329.base? 1: _temp5331 >= _temp5329.last_plus_one)){
_throw( Null_Exception);}* _temp5331;}).last_line)); _temp5322;}); _temp5321;});
goto _LL5290; _LL5290:;} break; case 226: _LL5285: yyval=( struct
_xtunion_struct*)({ struct Cyc_BlockItem_tok_struct* _temp5333=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5333[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5334; _temp5334.tag= Cyc_BlockItem_tok_tag;
_temp5334.f1=( void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp5335=(
struct Cyc_Parse_TopDecls_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct));
_temp5335[ 0]=({ struct Cyc_Parse_TopDecls_bl_struct _temp5336; _temp5336.tag=
Cyc_Parse_TopDecls_bl_tag; _temp5336.f1= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp5337= yyvs; struct _xtunion_struct** _temp5339= _temp5337.curr
+ yyvsp_offset; if( _temp5337.base == 0? 1:( _temp5339 < _temp5337.base? 1:
_temp5339 >= _temp5337.last_plus_one)){ _throw( Null_Exception);}* _temp5339;}));
_temp5336;}); _temp5335;})); _temp5334;}); _temp5333;}); break; case 227:
_LL5332: yyval=( struct _xtunion_struct*)({ struct Cyc_BlockItem_tok_struct*
_temp5341=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5341[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5342; _temp5342.tag= Cyc_BlockItem_tok_tag;
_temp5342.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp5343=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp5343[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp5344; _temp5344.tag= Cyc_Parse_Stmt_bl_tag;
_temp5344.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5345= yyvs; struct
_xtunion_struct** _temp5347= _temp5345.curr + yyvsp_offset; if( _temp5345.base
== 0? 1:( _temp5347 < _temp5345.base? 1: _temp5347 >= _temp5345.last_plus_one)){
_throw( Null_Exception);}* _temp5347;})); _temp5344;}); _temp5343;})); _temp5342;});
_temp5341;}); break; case 228: _LL5340: yyval=( struct _xtunion_struct*)({
struct Cyc_BlockItem_tok_struct* _temp5349=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5349[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5350; _temp5350.tag= Cyc_BlockItem_tok_tag;
_temp5350.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp5351=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp5351[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp5352; _temp5352.tag= Cyc_Parse_FnDecl_bl_tag;
_temp5352.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp5353= yyvs;
struct _xtunion_struct** _temp5355= _temp5353.curr + yyvsp_offset; if( _temp5353.base
== 0? 1:( _temp5355 < _temp5353.base? 1: _temp5355 >= _temp5353.last_plus_one)){
_throw( Null_Exception);}* _temp5355;})); _temp5352;}); _temp5351;})); _temp5350;});
_temp5349;}); break; case 229: _LL5348: yyval=( struct _xtunion_struct*)({
struct Cyc_Stmt_tok_struct* _temp5357=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5357[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5358; _temp5358.tag= Cyc_Stmt_tok_tag; _temp5358.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5359= yyvs; struct _xtunion_struct**
_temp5361= _temp5359.curr +( yyvsp_offset - 2); if( _temp5359.base == 0? 1:(
_temp5361 < _temp5359.base? 1: _temp5361 >= _temp5359.last_plus_one)){ _throw(
Null_Exception);}* _temp5361;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5362= yyvs; struct _xtunion_struct** _temp5364= _temp5362.curr +
yyvsp_offset; if( _temp5362.base == 0? 1:( _temp5364 < _temp5362.base? 1:
_temp5364 >= _temp5362.last_plus_one)){ _throw( Null_Exception);}* _temp5364;})),
Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5365= yyls; struct Cyc_Yyltype* _temp5367= _temp5365.curr +( yylsp_offset -
4); if( _temp5365.base == 0? 1:( _temp5367 < _temp5365.base? 1: _temp5367 >=
_temp5365.last_plus_one)){ _throw( Null_Exception);}* _temp5367;}).first_line,({
struct _tagged_ptr3 _temp5368= yyls; struct Cyc_Yyltype* _temp5370= _temp5368.curr
+ yylsp_offset; if( _temp5368.base == 0? 1:( _temp5370 < _temp5368.base? 1:
_temp5370 >= _temp5368.last_plus_one)){ _throw( Null_Exception);}* _temp5370;}).last_line));
_temp5358;}); _temp5357;}); break; case 230: _LL5356: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5372=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5372[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5373; _temp5373.tag= Cyc_Stmt_tok_tag; _temp5373.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5374= yyvs; struct _xtunion_struct**
_temp5376= _temp5374.curr +( yyvsp_offset - 4); if( _temp5374.base == 0? 1:(
_temp5376 < _temp5374.base? 1: _temp5376 >= _temp5374.last_plus_one)){ _throw(
Null_Exception);}* _temp5376;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5377= yyvs; struct _xtunion_struct** _temp5379= _temp5377.curr +(
yyvsp_offset - 2); if( _temp5377.base == 0? 1:( _temp5379 < _temp5377.base? 1:
_temp5379 >= _temp5377.last_plus_one)){ _throw( Null_Exception);}* _temp5379;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5380= yyvs; struct
_xtunion_struct** _temp5382= _temp5380.curr + yyvsp_offset; if( _temp5380.base
== 0? 1:( _temp5382 < _temp5380.base? 1: _temp5382 >= _temp5380.last_plus_one)){
_throw( Null_Exception);}* _temp5382;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5383= yyls; struct Cyc_Yyltype* _temp5385= _temp5383.curr +(
yylsp_offset - 6); if( _temp5383.base == 0? 1:( _temp5385 < _temp5383.base? 1:
_temp5385 >= _temp5383.last_plus_one)){ _throw( Null_Exception);}* _temp5385;}).first_line,({
struct _tagged_ptr3 _temp5386= yyls; struct Cyc_Yyltype* _temp5388= _temp5386.curr
+ yylsp_offset; if( _temp5386.base == 0? 1:( _temp5388 < _temp5386.base? 1:
_temp5388 >= _temp5386.last_plus_one)){ _throw( Null_Exception);}* _temp5388;}).last_line));
_temp5373;}); _temp5372;}); break; case 231: _LL5371: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5390=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5390[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5391; _temp5391.tag= Cyc_Stmt_tok_tag; _temp5391.f1= Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5392= yyvs; struct _xtunion_struct**
_temp5394= _temp5392.curr +( yyvsp_offset - 4); if( _temp5392.base == 0? 1:(
_temp5394 < _temp5392.base? 1: _temp5394 >= _temp5392.last_plus_one)){ _throw(
Null_Exception);}* _temp5394;})), Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5395= yyvs; struct _xtunion_struct** _temp5397= _temp5395.curr
+( yyvsp_offset - 1); if( _temp5395.base == 0? 1:( _temp5397 < _temp5395.base? 1:
_temp5397 >= _temp5395.last_plus_one)){ _throw( Null_Exception);}* _temp5397;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5398= yyls; struct Cyc_Yyltype*
_temp5400= _temp5398.curr +( yylsp_offset - 6); if( _temp5398.base == 0? 1:(
_temp5400 < _temp5398.base? 1: _temp5400 >= _temp5398.last_plus_one)){ _throw(
Null_Exception);}* _temp5400;}).first_line,({ struct _tagged_ptr3 _temp5401=
yyls; struct Cyc_Yyltype* _temp5403= _temp5401.curr + yylsp_offset; if(
_temp5401.base == 0? 1:( _temp5403 < _temp5401.base? 1: _temp5403 >= _temp5401.last_plus_one)){
_throw( Null_Exception);}* _temp5403;}).last_line)); _temp5391;}); _temp5390;});
break; case 232: _LL5389: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5405=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5405[ 0]=({ struct Cyc_Stmt_tok_struct _temp5406; _temp5406.tag= Cyc_Stmt_tok_tag;
_temp5406.f1= Cyc_Absyn_trycatch_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5407= yyvs; struct _xtunion_struct** _temp5409= _temp5407.curr +(
yyvsp_offset - 4); if( _temp5407.base == 0? 1:( _temp5409 < _temp5407.base? 1:
_temp5409 >= _temp5407.last_plus_one)){ _throw( Null_Exception);}* _temp5409;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp5410= yyvs; struct
_xtunion_struct** _temp5412= _temp5410.curr +( yyvsp_offset - 1); if( _temp5410.base
== 0? 1:( _temp5412 < _temp5410.base? 1: _temp5412 >= _temp5410.last_plus_one)){
_throw( Null_Exception);}* _temp5412;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5413= yyls; struct Cyc_Yyltype* _temp5415= _temp5413.curr +(
yylsp_offset - 5); if( _temp5413.base == 0? 1:( _temp5415 < _temp5413.base? 1:
_temp5415 >= _temp5413.last_plus_one)){ _throw( Null_Exception);}* _temp5415;}).first_line,({
struct _tagged_ptr3 _temp5416= yyls; struct Cyc_Yyltype* _temp5418= _temp5416.curr
+ yylsp_offset; if( _temp5416.base == 0? 1:( _temp5418 < _temp5416.base? 1:
_temp5418 >= _temp5416.last_plus_one)){ _throw( Null_Exception);}* _temp5418;}).last_line));
_temp5406;}); _temp5405;}); break; case 233: _LL5404: yyval=( struct
_xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5420=( struct
Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5420[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5421; _temp5421.tag=
Cyc_SwitchClauseList_tok_tag; _temp5421.f1= 0; _temp5421;}); _temp5420;});
break; case 234: _LL5419: yyval=( struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5423=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5423[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5424; _temp5424.tag= Cyc_SwitchClauseList_tok_tag; _temp5424.f1=({ struct
Cyc_List_List* _temp5425=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5425->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5426=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5426->pattern= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5436= yyls; struct Cyc_Yyltype* _temp5438= _temp5436.curr +( yylsp_offset -
2); if( _temp5436.base == 0? 1:( _temp5438 < _temp5436.base? 1: _temp5438 >=
_temp5436.last_plus_one)){ _throw( Null_Exception);}* _temp5438;}).first_line,({
struct _tagged_ptr3 _temp5439= yyls; struct Cyc_Yyltype* _temp5441= _temp5439.curr
+( yylsp_offset - 2); if( _temp5439.base == 0? 1:( _temp5441 < _temp5439.base? 1:
_temp5441 >= _temp5439.last_plus_one)){ _throw( Null_Exception);}* _temp5441;}).last_line));
_temp5426->pat_vars= 0; _temp5426->where_clause= 0; _temp5426->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5433= yyvs; struct _xtunion_struct** _temp5435=
_temp5433.curr + yyvsp_offset; if( _temp5433.base == 0? 1:( _temp5435 <
_temp5433.base? 1: _temp5435 >= _temp5433.last_plus_one)){ _throw(
Null_Exception);}* _temp5435;})); _temp5426->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5427= yyls; struct Cyc_Yyltype* _temp5429= _temp5427.curr
+( yylsp_offset - 2); if( _temp5427.base == 0? 1:( _temp5429 < _temp5427.base? 1:
_temp5429 >= _temp5427.last_plus_one)){ _throw( Null_Exception);}* _temp5429;}).first_line,({
struct _tagged_ptr3 _temp5430= yyls; struct Cyc_Yyltype* _temp5432= _temp5430.curr
+ yylsp_offset; if( _temp5430.base == 0? 1:( _temp5432 < _temp5430.base? 1:
_temp5432 >= _temp5430.last_plus_one)){ _throw( Null_Exception);}* _temp5432;}).last_line);
_temp5426;}); _temp5425->tl= 0; _temp5425;}); _temp5424;}); _temp5423;}); break;
case 235: _LL5422: yyval=( struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5443=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5443[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5444; _temp5444.tag= Cyc_SwitchClauseList_tok_tag; _temp5444.f1=({ struct
Cyc_List_List* _temp5445=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5445->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5449=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5449->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5462= yyvs; struct _xtunion_struct** _temp5464=
_temp5462.curr +( yyvsp_offset - 2); if( _temp5462.base == 0? 1:( _temp5464 <
_temp5462.base? 1: _temp5464 >= _temp5462.last_plus_one)){ _throw(
Null_Exception);}* _temp5464;})); _temp5449->pat_vars= 0; _temp5449->where_clause=
0; _temp5449->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5456= yyls; struct Cyc_Yyltype* _temp5458= _temp5456.curr +(
yylsp_offset - 1); if( _temp5456.base == 0? 1:( _temp5458 < _temp5456.base? 1:
_temp5458 >= _temp5456.last_plus_one)){ _throw( Null_Exception);}* _temp5458;}).first_line,({
struct _tagged_ptr3 _temp5459= yyls; struct Cyc_Yyltype* _temp5461= _temp5459.curr
+( yylsp_offset - 1); if( _temp5459.base == 0? 1:( _temp5461 < _temp5459.base? 1:
_temp5461 >= _temp5459.last_plus_one)){ _throw( Null_Exception);}* _temp5461;}).last_line));
_temp5449->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5450=
yyls; struct Cyc_Yyltype* _temp5452= _temp5450.curr +( yylsp_offset - 3); if(
_temp5450.base == 0? 1:( _temp5452 < _temp5450.base? 1: _temp5452 >= _temp5450.last_plus_one)){
_throw( Null_Exception);}* _temp5452;}).first_line,({ struct _tagged_ptr3
_temp5453= yyls; struct Cyc_Yyltype* _temp5455= _temp5453.curr + yylsp_offset;
if( _temp5453.base == 0? 1:( _temp5455 < _temp5453.base? 1: _temp5455 >=
_temp5453.last_plus_one)){ _throw( Null_Exception);}* _temp5455;}).last_line);
_temp5449;}); _temp5445->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5446= yyvs; struct _xtunion_struct** _temp5448= _temp5446.curr
+ yyvsp_offset; if( _temp5446.base == 0? 1:( _temp5448 < _temp5446.base? 1:
_temp5448 >= _temp5446.last_plus_one)){ _throw( Null_Exception);}* _temp5448;}));
_temp5445;}); _temp5444;}); _temp5443;}); break; case 236: _LL5442: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5466=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5466[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5467; _temp5467.tag=
Cyc_SwitchClauseList_tok_tag; _temp5467.f1=({ struct Cyc_List_List* _temp5468=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5468->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5472=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5472->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5482= yyvs; struct _xtunion_struct** _temp5484=
_temp5482.curr +( yyvsp_offset - 3); if( _temp5482.base == 0? 1:( _temp5484 <
_temp5482.base? 1: _temp5484 >= _temp5482.last_plus_one)){ _throw(
Null_Exception);}* _temp5484;})); _temp5472->pat_vars= 0; _temp5472->where_clause=
0; _temp5472->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5479= yyvs;
struct _xtunion_struct** _temp5481= _temp5479.curr +( yyvsp_offset - 1); if(
_temp5479.base == 0? 1:( _temp5481 < _temp5479.base? 1: _temp5481 >= _temp5479.last_plus_one)){
_throw( Null_Exception);}* _temp5481;})); _temp5472->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5473= yyls; struct Cyc_Yyltype* _temp5475= _temp5473.curr
+( yylsp_offset - 4); if( _temp5473.base == 0? 1:( _temp5475 < _temp5473.base? 1:
_temp5475 >= _temp5473.last_plus_one)){ _throw( Null_Exception);}* _temp5475;}).first_line,({
struct _tagged_ptr3 _temp5476= yyls; struct Cyc_Yyltype* _temp5478= _temp5476.curr
+( yylsp_offset - 1); if( _temp5476.base == 0? 1:( _temp5478 < _temp5476.base? 1:
_temp5478 >= _temp5476.last_plus_one)){ _throw( Null_Exception);}* _temp5478;}).last_line);
_temp5472;}); _temp5468->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5469= yyvs; struct _xtunion_struct** _temp5471= _temp5469.curr
+ yyvsp_offset; if( _temp5469.base == 0? 1:( _temp5471 < _temp5469.base? 1:
_temp5471 >= _temp5469.last_plus_one)){ _throw( Null_Exception);}* _temp5471;}));
_temp5468;}); _temp5467;}); _temp5466;}); break; case 237: _LL5465: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5486=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5486[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5487; _temp5487.tag=
Cyc_SwitchClauseList_tok_tag; _temp5487.f1=({ struct Cyc_List_List* _temp5488=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5488->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5492=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5492->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5508= yyvs; struct _xtunion_struct** _temp5510=
_temp5508.curr +( yyvsp_offset - 4); if( _temp5508.base == 0? 1:( _temp5510 <
_temp5508.base? 1: _temp5510 >= _temp5508.last_plus_one)){ _throw(
Null_Exception);}* _temp5510;})); _temp5492->pat_vars= 0; _temp5492->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5505= yyvs;
struct _xtunion_struct** _temp5507= _temp5505.curr +( yyvsp_offset - 2); if(
_temp5505.base == 0? 1:( _temp5507 < _temp5505.base? 1: _temp5507 >= _temp5505.last_plus_one)){
_throw( Null_Exception);}* _temp5507;})); _temp5492->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5499= yyls; struct Cyc_Yyltype*
_temp5501= _temp5499.curr +( yylsp_offset - 1); if( _temp5499.base == 0? 1:(
_temp5501 < _temp5499.base? 1: _temp5501 >= _temp5499.last_plus_one)){ _throw(
Null_Exception);}* _temp5501;}).first_line,({ struct _tagged_ptr3 _temp5502=
yyls; struct Cyc_Yyltype* _temp5504= _temp5502.curr +( yylsp_offset - 1); if(
_temp5502.base == 0? 1:( _temp5504 < _temp5502.base? 1: _temp5504 >= _temp5502.last_plus_one)){
_throw( Null_Exception);}* _temp5504;}).last_line)); _temp5492->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5493= yyls; struct Cyc_Yyltype* _temp5495= _temp5493.curr
+( yylsp_offset - 5); if( _temp5493.base == 0? 1:( _temp5495 < _temp5493.base? 1:
_temp5495 >= _temp5493.last_plus_one)){ _throw( Null_Exception);}* _temp5495;}).first_line,({
struct _tagged_ptr3 _temp5496= yyls; struct Cyc_Yyltype* _temp5498= _temp5496.curr
+ yylsp_offset; if( _temp5496.base == 0? 1:( _temp5498 < _temp5496.base? 1:
_temp5498 >= _temp5496.last_plus_one)){ _throw( Null_Exception);}* _temp5498;}).last_line);
_temp5492;}); _temp5488->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5489= yyvs; struct _xtunion_struct** _temp5491= _temp5489.curr
+ yyvsp_offset; if( _temp5489.base == 0? 1:( _temp5491 < _temp5489.base? 1:
_temp5491 >= _temp5489.last_plus_one)){ _throw( Null_Exception);}* _temp5491;}));
_temp5488;}); _temp5487;}); _temp5486;}); break; case 238: _LL5485: yyval=(
struct _xtunion_struct*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5512=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp5512[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp5513; _temp5513.tag=
Cyc_SwitchClauseList_tok_tag; _temp5513.f1=({ struct Cyc_List_List* _temp5514=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp5514->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp5518=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5518->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5531= yyvs; struct _xtunion_struct** _temp5533=
_temp5531.curr +( yyvsp_offset - 5); if( _temp5531.base == 0? 1:( _temp5533 <
_temp5531.base? 1: _temp5533 >= _temp5531.last_plus_one)){ _throw(
Null_Exception);}* _temp5533;})); _temp5518->pat_vars= 0; _temp5518->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5528= yyvs;
struct _xtunion_struct** _temp5530= _temp5528.curr +( yyvsp_offset - 3); if(
_temp5528.base == 0? 1:( _temp5530 < _temp5528.base? 1: _temp5530 >= _temp5528.last_plus_one)){
_throw( Null_Exception);}* _temp5530;})); _temp5518->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5525= yyvs; struct _xtunion_struct** _temp5527=
_temp5525.curr +( yyvsp_offset - 1); if( _temp5525.base == 0? 1:( _temp5527 <
_temp5525.base? 1: _temp5527 >= _temp5525.last_plus_one)){ _throw(
Null_Exception);}* _temp5527;})); _temp5518->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5519= yyls; struct Cyc_Yyltype* _temp5521= _temp5519.curr
+( yylsp_offset - 6); if( _temp5519.base == 0? 1:( _temp5521 < _temp5519.base? 1:
_temp5521 >= _temp5519.last_plus_one)){ _throw( Null_Exception);}* _temp5521;}).first_line,({
struct _tagged_ptr3 _temp5522= yyls; struct Cyc_Yyltype* _temp5524= _temp5522.curr
+ yylsp_offset; if( _temp5522.base == 0? 1:( _temp5524 < _temp5522.base? 1:
_temp5524 >= _temp5522.last_plus_one)){ _throw( Null_Exception);}* _temp5524;}).last_line);
_temp5518;}); _temp5514->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5515= yyvs; struct _xtunion_struct** _temp5517= _temp5515.curr
+ yyvsp_offset; if( _temp5515.base == 0? 1:( _temp5517 < _temp5515.base? 1:
_temp5517 >= _temp5515.last_plus_one)){ _throw( Null_Exception);}* _temp5517;}));
_temp5514;}); _temp5513;}); _temp5512;}); break; case 239: _LL5511: yyval=(
struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5535=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5535[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5536; _temp5536.tag= Cyc_Stmt_tok_tag; _temp5536.f1= Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5537= yyvs; struct _xtunion_struct**
_temp5539= _temp5537.curr +( yyvsp_offset - 2); if( _temp5537.base == 0? 1:(
_temp5539 < _temp5537.base? 1: _temp5539 >= _temp5537.last_plus_one)){ _throw(
Null_Exception);}* _temp5539;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5540= yyvs; struct _xtunion_struct** _temp5542= _temp5540.curr +
yyvsp_offset; if( _temp5540.base == 0? 1:( _temp5542 < _temp5540.base? 1:
_temp5542 >= _temp5540.last_plus_one)){ _throw( Null_Exception);}* _temp5542;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5543= yyls; struct Cyc_Yyltype*
_temp5545= _temp5543.curr +( yylsp_offset - 4); if( _temp5543.base == 0? 1:(
_temp5545 < _temp5543.base? 1: _temp5545 >= _temp5543.last_plus_one)){ _throw(
Null_Exception);}* _temp5545;}).first_line,({ struct _tagged_ptr3 _temp5546=
yyls; struct Cyc_Yyltype* _temp5548= _temp5546.curr + yylsp_offset; if(
_temp5546.base == 0? 1:( _temp5548 < _temp5546.base? 1: _temp5548 >= _temp5546.last_plus_one)){
_throw( Null_Exception);}* _temp5548;}).last_line)); _temp5536;}); _temp5535;});
break; case 240: _LL5534: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5550=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5550[ 0]=({ struct Cyc_Stmt_tok_struct _temp5551; _temp5551.tag= Cyc_Stmt_tok_tag;
_temp5551.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5552= yyvs; struct _xtunion_struct** _temp5554= _temp5552.curr +(
yyvsp_offset - 5); if( _temp5552.base == 0? 1:( _temp5554 < _temp5552.base? 1:
_temp5554 >= _temp5552.last_plus_one)){ _throw( Null_Exception);}* _temp5554;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5555= yyvs; struct _xtunion_struct**
_temp5557= _temp5555.curr +( yyvsp_offset - 2); if( _temp5555.base == 0? 1:(
_temp5557 < _temp5555.base? 1: _temp5557 >= _temp5555.last_plus_one)){ _throw(
Null_Exception);}* _temp5557;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5558= yyls; struct Cyc_Yyltype* _temp5560= _temp5558.curr +(
yylsp_offset - 6); if( _temp5558.base == 0? 1:( _temp5560 < _temp5558.base? 1:
_temp5560 >= _temp5558.last_plus_one)){ _throw( Null_Exception);}* _temp5560;}).first_line,({
struct _tagged_ptr3 _temp5561= yyls; struct Cyc_Yyltype* _temp5563= _temp5561.curr
+ yylsp_offset; if( _temp5561.base == 0? 1:( _temp5563 < _temp5561.base? 1:
_temp5563 >= _temp5561.last_plus_one)){ _throw( Null_Exception);}* _temp5563;}).last_line));
_temp5551;}); _temp5550;}); break; case 241: _LL5549: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5565=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5565[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5566; _temp5566.tag= Cyc_Stmt_tok_tag; _temp5566.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5567= yyvs; struct _xtunion_struct** _temp5569=
_temp5567.curr + yyvsp_offset; if( _temp5567.base == 0? 1:( _temp5569 <
_temp5567.base? 1: _temp5569 >= _temp5567.last_plus_one)){ _throw(
Null_Exception);}* _temp5569;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5570= yyls; struct Cyc_Yyltype* _temp5572= _temp5570.curr +(
yylsp_offset - 5); if( _temp5570.base == 0? 1:( _temp5572 < _temp5570.base? 1:
_temp5572 >= _temp5570.last_plus_one)){ _throw( Null_Exception);}* _temp5572;}).first_line,({
struct _tagged_ptr3 _temp5573= yyls; struct Cyc_Yyltype* _temp5575= _temp5573.curr
+ yylsp_offset; if( _temp5573.base == 0? 1:( _temp5575 < _temp5573.base? 1:
_temp5575 >= _temp5573.last_plus_one)){ _throw( Null_Exception);}* _temp5575;}).last_line));
_temp5566;}); _temp5565;}); break; case 242: _LL5564: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5577=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5577[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5578; _temp5578.tag= Cyc_Stmt_tok_tag; _temp5578.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5579= yyvs; struct _xtunion_struct** _temp5581= _temp5579.curr
+( yyvsp_offset - 2); if( _temp5579.base == 0? 1:( _temp5581 < _temp5579.base? 1:
_temp5581 >= _temp5579.last_plus_one)){ _throw( Null_Exception);}* _temp5581;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5582= yyvs; struct
_xtunion_struct** _temp5584= _temp5582.curr + yyvsp_offset; if( _temp5582.base
== 0? 1:( _temp5584 < _temp5582.base? 1: _temp5584 >= _temp5582.last_plus_one)){
_throw( Null_Exception);}* _temp5584;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5585= yyls; struct Cyc_Yyltype* _temp5587= _temp5585.curr +(
yylsp_offset - 6); if( _temp5585.base == 0? 1:( _temp5587 < _temp5585.base? 1:
_temp5587 >= _temp5585.last_plus_one)){ _throw( Null_Exception);}* _temp5587;}).first_line,({
struct _tagged_ptr3 _temp5588= yyls; struct Cyc_Yyltype* _temp5590= _temp5588.curr
+ yylsp_offset; if( _temp5588.base == 0? 1:( _temp5590 < _temp5588.base? 1:
_temp5590 >= _temp5588.last_plus_one)){ _throw( Null_Exception);}* _temp5590;}).last_line));
_temp5578;}); _temp5577;}); break; case 243: _LL5576: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5592=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5592[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5593; _temp5593.tag= Cyc_Stmt_tok_tag; _temp5593.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5594=
yyvs; struct _xtunion_struct** _temp5596= _temp5594.curr +( yyvsp_offset - 3);
if( _temp5594.base == 0? 1:( _temp5596 < _temp5594.base? 1: _temp5596 >=
_temp5594.last_plus_one)){ _throw( Null_Exception);}* _temp5596;})), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5597= yyvs; struct
_xtunion_struct** _temp5599= _temp5597.curr + yyvsp_offset; if( _temp5597.base
== 0? 1:( _temp5599 < _temp5597.base? 1: _temp5599 >= _temp5597.last_plus_one)){
_throw( Null_Exception);}* _temp5599;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5600= yyls; struct Cyc_Yyltype* _temp5602= _temp5600.curr +(
yylsp_offset - 6); if( _temp5600.base == 0? 1:( _temp5602 < _temp5600.base? 1:
_temp5602 >= _temp5600.last_plus_one)){ _throw( Null_Exception);}* _temp5602;}).first_line,({
struct _tagged_ptr3 _temp5603= yyls; struct Cyc_Yyltype* _temp5605= _temp5603.curr
+ yylsp_offset; if( _temp5603.base == 0? 1:( _temp5605 < _temp5603.base? 1:
_temp5605 >= _temp5603.last_plus_one)){ _throw( Null_Exception);}* _temp5605;}).last_line));
_temp5593;}); _temp5592;}); break; case 244: _LL5591: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5607=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5607[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5608; _temp5608.tag= Cyc_Stmt_tok_tag; _temp5608.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5609=
yyvs; struct _xtunion_struct** _temp5611= _temp5609.curr +( yyvsp_offset - 4);
if( _temp5609.base == 0? 1:( _temp5611 < _temp5609.base? 1: _temp5611 >=
_temp5609.last_plus_one)){ _throw( Null_Exception);}* _temp5611;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5612= yyvs; struct _xtunion_struct** _temp5614=
_temp5612.curr +( yyvsp_offset - 2); if( _temp5612.base == 0? 1:( _temp5614 <
_temp5612.base? 1: _temp5614 >= _temp5612.last_plus_one)){ _throw(
Null_Exception);}* _temp5614;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5615= yyvs; struct _xtunion_struct** _temp5617= _temp5615.curr +
yyvsp_offset; if( _temp5615.base == 0? 1:( _temp5617 < _temp5615.base? 1:
_temp5617 >= _temp5615.last_plus_one)){ _throw( Null_Exception);}* _temp5617;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5618= yyls; struct Cyc_Yyltype*
_temp5620= _temp5618.curr +( yylsp_offset - 7); if( _temp5618.base == 0? 1:(
_temp5620 < _temp5618.base? 1: _temp5620 >= _temp5618.last_plus_one)){ _throw(
Null_Exception);}* _temp5620;}).first_line,({ struct _tagged_ptr3 _temp5621=
yyls; struct Cyc_Yyltype* _temp5623= _temp5621.curr +( yylsp_offset - 1); if(
_temp5621.base == 0? 1:( _temp5623 < _temp5621.base? 1: _temp5623 >= _temp5621.last_plus_one)){
_throw( Null_Exception);}* _temp5623;}).last_line)); _temp5608;}); _temp5607;});
break; case 245: _LL5606: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5625=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5625[ 0]=({ struct Cyc_Stmt_tok_struct _temp5626; _temp5626.tag= Cyc_Stmt_tok_tag;
_temp5626.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5627= yyvs; struct _xtunion_struct** _temp5629= _temp5627.curr +(
yyvsp_offset - 4); if( _temp5627.base == 0? 1:( _temp5629 < _temp5627.base? 1:
_temp5629 >= _temp5627.last_plus_one)){ _throw( Null_Exception);}* _temp5629;})),
Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5630= yyvs; struct _xtunion_struct** _temp5632= _temp5630.curr
+ yyvsp_offset; if( _temp5630.base == 0? 1:( _temp5632 < _temp5630.base? 1:
_temp5632 >= _temp5630.last_plus_one)){ _throw( Null_Exception);}* _temp5632;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5633= yyls; struct Cyc_Yyltype*
_temp5635= _temp5633.curr +( yylsp_offset - 6); if( _temp5633.base == 0? 1:(
_temp5635 < _temp5633.base? 1: _temp5635 >= _temp5633.last_plus_one)){ _throw(
Null_Exception);}* _temp5635;}).first_line,({ struct _tagged_ptr3 _temp5636=
yyls; struct Cyc_Yyltype* _temp5638= _temp5636.curr + yylsp_offset; if(
_temp5636.base == 0? 1:( _temp5638 < _temp5636.base? 1: _temp5638 >= _temp5636.last_plus_one)){
_throw( Null_Exception);}* _temp5638;}).last_line)); _temp5626;}); _temp5625;});
break; case 246: _LL5624: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5640=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5640[ 0]=({ struct Cyc_Stmt_tok_struct _temp5641; _temp5641.tag= Cyc_Stmt_tok_tag;
_temp5641.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5642= yyvs; struct _xtunion_struct** _temp5644= _temp5642.curr +(
yyvsp_offset - 5); if( _temp5642.base == 0? 1:( _temp5644 < _temp5642.base? 1:
_temp5644 >= _temp5642.last_plus_one)){ _throw( Null_Exception);}* _temp5644;})),
Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5645= yyvs;
struct _xtunion_struct** _temp5647= _temp5645.curr +( yyvsp_offset - 2); if(
_temp5645.base == 0? 1:( _temp5647 < _temp5645.base? 1: _temp5647 >= _temp5645.last_plus_one)){
_throw( Null_Exception);}* _temp5647;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5648= yyvs; struct _xtunion_struct** _temp5650= _temp5648.curr
+ yyvsp_offset; if( _temp5648.base == 0? 1:( _temp5650 < _temp5648.base? 1:
_temp5650 >= _temp5648.last_plus_one)){ _throw( Null_Exception);}* _temp5650;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5651= yyls; struct Cyc_Yyltype*
_temp5653= _temp5651.curr +( yylsp_offset - 7); if( _temp5651.base == 0? 1:(
_temp5653 < _temp5651.base? 1: _temp5653 >= _temp5651.last_plus_one)){ _throw(
Null_Exception);}* _temp5653;}).first_line,({ struct _tagged_ptr3 _temp5654=
yyls; struct Cyc_Yyltype* _temp5656= _temp5654.curr + yylsp_offset; if(
_temp5654.base == 0? 1:( _temp5656 < _temp5654.base? 1: _temp5656 >= _temp5654.last_plus_one)){
_throw( Null_Exception);}* _temp5656;}).last_line)); _temp5641;}); _temp5640;});
break; case 247: _LL5639: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5658=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5658[ 0]=({ struct Cyc_Stmt_tok_struct _temp5659; _temp5659.tag= Cyc_Stmt_tok_tag;
_temp5659.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5660= yyvs; struct _xtunion_struct** _temp5662= _temp5660.curr +(
yyvsp_offset - 5); if( _temp5660.base == 0? 1:( _temp5662 < _temp5660.base? 1:
_temp5662 >= _temp5660.last_plus_one)){ _throw( Null_Exception);}* _temp5662;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5663= yyvs; struct _xtunion_struct**
_temp5665= _temp5663.curr +( yyvsp_offset - 3); if( _temp5663.base == 0? 1:(
_temp5665 < _temp5663.base? 1: _temp5665 >= _temp5663.last_plus_one)){ _throw(
Null_Exception);}* _temp5665;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5666= yyvs; struct _xtunion_struct** _temp5668=
_temp5666.curr + yyvsp_offset; if( _temp5666.base == 0? 1:( _temp5668 <
_temp5666.base? 1: _temp5668 >= _temp5666.last_plus_one)){ _throw(
Null_Exception);}* _temp5668;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5669= yyls; struct Cyc_Yyltype* _temp5671= _temp5669.curr +(
yylsp_offset - 7); if( _temp5669.base == 0? 1:( _temp5671 < _temp5669.base? 1:
_temp5671 >= _temp5669.last_plus_one)){ _throw( Null_Exception);}* _temp5671;}).first_line,({
struct _tagged_ptr3 _temp5672= yyls; struct Cyc_Yyltype* _temp5674= _temp5672.curr
+ yylsp_offset; if( _temp5672.base == 0? 1:( _temp5674 < _temp5672.base? 1:
_temp5674 >= _temp5672.last_plus_one)){ _throw( Null_Exception);}* _temp5674;}).last_line));
_temp5659;}); _temp5658;}); break; case 248: _LL5657: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5676=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5676[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5677; _temp5677.tag= Cyc_Stmt_tok_tag; _temp5677.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5678= yyvs; struct _xtunion_struct**
_temp5680= _temp5678.curr +( yyvsp_offset - 6); if( _temp5678.base == 0? 1:(
_temp5680 < _temp5678.base? 1: _temp5680 >= _temp5678.last_plus_one)){ _throw(
Null_Exception);}* _temp5680;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5681= yyvs; struct _xtunion_struct** _temp5683= _temp5681.curr +(
yyvsp_offset - 4); if( _temp5681.base == 0? 1:( _temp5683 < _temp5681.base? 1:
_temp5683 >= _temp5681.last_plus_one)){ _throw( Null_Exception);}* _temp5683;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5684= yyvs; struct _xtunion_struct**
_temp5686= _temp5684.curr +( yyvsp_offset - 2); if( _temp5684.base == 0? 1:(
_temp5686 < _temp5684.base? 1: _temp5686 >= _temp5684.last_plus_one)){ _throw(
Null_Exception);}* _temp5686;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5687= yyvs; struct _xtunion_struct** _temp5689= _temp5687.curr +
yyvsp_offset; if( _temp5687.base == 0? 1:( _temp5689 < _temp5687.base? 1:
_temp5689 >= _temp5687.last_plus_one)){ _throw( Null_Exception);}* _temp5689;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5690= yyls; struct Cyc_Yyltype*
_temp5692= _temp5690.curr +( yylsp_offset - 8); if( _temp5690.base == 0? 1:(
_temp5692 < _temp5690.base? 1: _temp5692 >= _temp5690.last_plus_one)){ _throw(
Null_Exception);}* _temp5692;}).first_line,({ struct _tagged_ptr3 _temp5693=
yyls; struct Cyc_Yyltype* _temp5695= _temp5693.curr + yylsp_offset; if(
_temp5693.base == 0? 1:( _temp5695 < _temp5693.base? 1: _temp5695 >= _temp5693.last_plus_one)){
_throw( Null_Exception);}* _temp5695;}).last_line)); _temp5677;}); _temp5676;});
break; case 249: _LL5675: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5697= yyvs; struct _xtunion_struct** _temp5699=
_temp5697.curr +( yyvsp_offset - 3); if( _temp5697.base == 0? 1:( _temp5699 <
_temp5697.base? 1: _temp5699 >= _temp5697.last_plus_one)){ _throw(
Null_Exception);}* _temp5699;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5700= yyvs; struct _xtunion_struct** _temp5702=
_temp5700.curr + yyvsp_offset; if( _temp5700.base == 0? 1:( _temp5702 <
_temp5700.base? 1: _temp5702 >= _temp5700.last_plus_one)){ _throw(
Null_Exception);}* _temp5702;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5703= yyls; struct Cyc_Yyltype* _temp5705= _temp5703.curr +(
yylsp_offset - 5); if( _temp5703.base == 0? 1:( _temp5705 < _temp5703.base? 1:
_temp5705 >= _temp5703.last_plus_one)){ _throw( Null_Exception);}* _temp5705;}).first_line,({
struct _tagged_ptr3 _temp5706= yyls; struct Cyc_Yyltype* _temp5708= _temp5706.curr
+ yylsp_offset; if( _temp5706.base == 0? 1:( _temp5708 < _temp5706.base? 1:
_temp5708 >= _temp5706.last_plus_one)){ _throw( Null_Exception);}* _temp5708;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5709=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5709[ 0]=({ struct Cyc_Stmt_tok_struct _temp5710; _temp5710.tag= Cyc_Stmt_tok_tag;
_temp5710.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5710;}); _temp5709;});
break;} case 250: _LL5696: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5712= yyvs; struct _xtunion_struct** _temp5714=
_temp5712.curr +( yyvsp_offset - 4); if( _temp5712.base == 0? 1:( _temp5714 <
_temp5712.base? 1: _temp5714 >= _temp5712.last_plus_one)){ _throw(
Null_Exception);}* _temp5714;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5715=
yyvs; struct _xtunion_struct** _temp5717= _temp5715.curr +( yyvsp_offset - 3);
if( _temp5715.base == 0? 1:( _temp5717 < _temp5715.base? 1: _temp5717 >=
_temp5715.last_plus_one)){ _throw( Null_Exception);}* _temp5717;})), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5718= yyvs; struct
_xtunion_struct** _temp5720= _temp5718.curr + yyvsp_offset; if( _temp5718.base
== 0? 1:( _temp5720 < _temp5718.base? 1: _temp5720 >= _temp5718.last_plus_one)){
_throw( Null_Exception);}* _temp5720;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5721= yyls; struct Cyc_Yyltype* _temp5723= _temp5721.curr +(
yylsp_offset - 6); if( _temp5721.base == 0? 1:( _temp5723 < _temp5721.base? 1:
_temp5723 >= _temp5721.last_plus_one)){ _throw( Null_Exception);}* _temp5723;}).first_line,({
struct _tagged_ptr3 _temp5724= yyls; struct Cyc_Yyltype* _temp5726= _temp5724.curr
+ yylsp_offset; if( _temp5724.base == 0? 1:( _temp5726 < _temp5724.base? 1:
_temp5726 >= _temp5724.last_plus_one)){ _throw( Null_Exception);}* _temp5726;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5727=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5727[ 0]=({ struct Cyc_Stmt_tok_struct _temp5728; _temp5728.tag= Cyc_Stmt_tok_tag;
_temp5728.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5728;}); _temp5727;});
break;} case 251: _LL5711: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5730= yyvs; struct _xtunion_struct** _temp5732=
_temp5730.curr +( yyvsp_offset - 4); if( _temp5730.base == 0? 1:( _temp5732 <
_temp5730.base? 1: _temp5732 >= _temp5730.last_plus_one)){ _throw(
Null_Exception);}* _temp5732;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5733= yyvs; struct _xtunion_struct** _temp5735= _temp5733.curr
+( yyvsp_offset - 2); if( _temp5733.base == 0? 1:( _temp5735 < _temp5733.base? 1:
_temp5735 >= _temp5733.last_plus_one)){ _throw( Null_Exception);}* _temp5735;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5736= yyvs; struct
_xtunion_struct** _temp5738= _temp5736.curr + yyvsp_offset; if( _temp5736.base
== 0? 1:( _temp5738 < _temp5736.base? 1: _temp5738 >= _temp5736.last_plus_one)){
_throw( Null_Exception);}* _temp5738;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5739= yyls; struct Cyc_Yyltype* _temp5741= _temp5739.curr +(
yylsp_offset - 6); if( _temp5739.base == 0? 1:( _temp5741 < _temp5739.base? 1:
_temp5741 >= _temp5739.last_plus_one)){ _throw( Null_Exception);}* _temp5741;}).first_line,({
struct _tagged_ptr3 _temp5742= yyls; struct Cyc_Yyltype* _temp5744= _temp5742.curr
+ yylsp_offset; if( _temp5742.base == 0? 1:( _temp5744 < _temp5742.base? 1:
_temp5744 >= _temp5742.last_plus_one)){ _throw( Null_Exception);}* _temp5744;}).last_line));
yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5745=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5745[ 0]=({ struct Cyc_Stmt_tok_struct _temp5746; _temp5746.tag= Cyc_Stmt_tok_tag;
_temp5746.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5746;}); _temp5745;});
break;} case 252: _LL5729: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5748= yyvs; struct _xtunion_struct** _temp5750=
_temp5748.curr +( yyvsp_offset - 5); if( _temp5748.base == 0? 1:( _temp5750 <
_temp5748.base? 1: _temp5750 >= _temp5748.last_plus_one)){ _throw(
Null_Exception);}* _temp5750;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5751=
yyvs; struct _xtunion_struct** _temp5753= _temp5751.curr +( yyvsp_offset - 4);
if( _temp5751.base == 0? 1:( _temp5753 < _temp5751.base? 1: _temp5753 >=
_temp5751.last_plus_one)){ _throw( Null_Exception);}* _temp5753;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5754= yyvs; struct _xtunion_struct** _temp5756=
_temp5754.curr +( yyvsp_offset - 2); if( _temp5754.base == 0? 1:( _temp5756 <
_temp5754.base? 1: _temp5756 >= _temp5754.last_plus_one)){ _throw(
Null_Exception);}* _temp5756;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5757= yyvs; struct _xtunion_struct** _temp5759= _temp5757.curr +
yyvsp_offset; if( _temp5757.base == 0? 1:( _temp5759 < _temp5757.base? 1:
_temp5759 >= _temp5757.last_plus_one)){ _throw( Null_Exception);}* _temp5759;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5760= yyls; struct Cyc_Yyltype*
_temp5762= _temp5760.curr +( yylsp_offset - 7); if( _temp5760.base == 0? 1:(
_temp5762 < _temp5760.base? 1: _temp5762 >= _temp5760.last_plus_one)){ _throw(
Null_Exception);}* _temp5762;}).first_line,({ struct _tagged_ptr3 _temp5763=
yyls; struct Cyc_Yyltype* _temp5765= _temp5763.curr + yylsp_offset; if(
_temp5763.base == 0? 1:( _temp5765 < _temp5763.base? 1: _temp5765 >= _temp5763.last_plus_one)){
_throw( Null_Exception);}* _temp5765;}).last_line)); yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5766=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5766[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5767; _temp5767.tag= Cyc_Stmt_tok_tag; _temp5767.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5767;}); _temp5766;}); break;} case 253: _LL5747: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5769=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5769[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5770; _temp5770.tag= Cyc_Stmt_tok_tag; _temp5770.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_string* _temp5771=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5771[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5772= yyvs; struct _xtunion_struct** _temp5774= _temp5772.curr
+( yyvsp_offset - 1); if( _temp5772.base == 0? 1:( _temp5774 < _temp5772.base? 1:
_temp5774 >= _temp5772.last_plus_one)){ _throw( Null_Exception);}* _temp5774;}));
_temp5771;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5775= yyls;
struct Cyc_Yyltype* _temp5777= _temp5775.curr +( yylsp_offset - 2); if(
_temp5775.base == 0? 1:( _temp5777 < _temp5775.base? 1: _temp5777 >= _temp5775.last_plus_one)){
_throw( Null_Exception);}* _temp5777;}).first_line,({ struct _tagged_ptr3
_temp5778= yyls; struct Cyc_Yyltype* _temp5780= _temp5778.curr +( yylsp_offset -
1); if( _temp5778.base == 0? 1:( _temp5780 < _temp5778.base? 1: _temp5780 >=
_temp5778.last_plus_one)){ _throw( Null_Exception);}* _temp5780;}).last_line));
_temp5770;}); _temp5769;}); break; case 254: _LL5768: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5782=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5782[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5783; _temp5783.tag= Cyc_Stmt_tok_tag; _temp5783.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5784= yyls; struct Cyc_Yyltype*
_temp5786= _temp5784.curr +( yylsp_offset - 1); if( _temp5784.base == 0? 1:(
_temp5786 < _temp5784.base? 1: _temp5786 >= _temp5784.last_plus_one)){ _throw(
Null_Exception);}* _temp5786;}).first_line,({ struct _tagged_ptr3 _temp5787=
yyls; struct Cyc_Yyltype* _temp5789= _temp5787.curr +( yylsp_offset - 1); if(
_temp5787.base == 0? 1:( _temp5789 < _temp5787.base? 1: _temp5789 >= _temp5787.last_plus_one)){
_throw( Null_Exception);}* _temp5789;}).last_line)); _temp5783;}); _temp5782;});
break; case 255: _LL5781: yyval=( struct _xtunion_struct*)({ struct Cyc_Stmt_tok_struct*
_temp5791=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5791[ 0]=({ struct Cyc_Stmt_tok_struct _temp5792; _temp5792.tag= Cyc_Stmt_tok_tag;
_temp5792.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5793= yyls; struct Cyc_Yyltype* _temp5795= _temp5793.curr +(
yylsp_offset - 1); if( _temp5793.base == 0? 1:( _temp5795 < _temp5793.base? 1:
_temp5795 >= _temp5793.last_plus_one)){ _throw( Null_Exception);}* _temp5795;}).first_line,({
struct _tagged_ptr3 _temp5796= yyls; struct Cyc_Yyltype* _temp5798= _temp5796.curr
+( yylsp_offset - 1); if( _temp5796.base == 0? 1:( _temp5798 < _temp5796.base? 1:
_temp5798 >= _temp5796.last_plus_one)){ _throw( Null_Exception);}* _temp5798;}).last_line));
_temp5792;}); _temp5791;}); break; case 256: _LL5790: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5800=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5800[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5801; _temp5801.tag= Cyc_Stmt_tok_tag; _temp5801.f1= Cyc_Absyn_return_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5802= yyls; struct
Cyc_Yyltype* _temp5804= _temp5802.curr +( yylsp_offset - 1); if( _temp5802.base
== 0? 1:( _temp5804 < _temp5802.base? 1: _temp5804 >= _temp5802.last_plus_one)){
_throw( Null_Exception);}* _temp5804;}).first_line,({ struct _tagged_ptr3
_temp5805= yyls; struct Cyc_Yyltype* _temp5807= _temp5805.curr +( yylsp_offset -
1); if( _temp5805.base == 0? 1:( _temp5807 < _temp5805.base? 1: _temp5807 >=
_temp5805.last_plus_one)){ _throw( Null_Exception);}* _temp5807;}).last_line));
_temp5801;}); _temp5800;}); break; case 257: _LL5799: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5809=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5809[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5810; _temp5810.tag= Cyc_Stmt_tok_tag; _temp5810.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5811= yyvs;
struct _xtunion_struct** _temp5813= _temp5811.curr +( yyvsp_offset - 1); if(
_temp5811.base == 0? 1:( _temp5813 < _temp5811.base? 1: _temp5813 >= _temp5811.last_plus_one)){
_throw( Null_Exception);}* _temp5813;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5814= yyls; struct Cyc_Yyltype* _temp5816= _temp5814.curr +(
yylsp_offset - 2); if( _temp5814.base == 0? 1:( _temp5816 < _temp5814.base? 1:
_temp5816 >= _temp5814.last_plus_one)){ _throw( Null_Exception);}* _temp5816;}).first_line,({
struct _tagged_ptr3 _temp5817= yyls; struct Cyc_Yyltype* _temp5819= _temp5817.curr
+( yylsp_offset - 1); if( _temp5817.base == 0? 1:( _temp5819 < _temp5817.base? 1:
_temp5819 >= _temp5817.last_plus_one)){ _throw( Null_Exception);}* _temp5819;}).last_line));
_temp5810;}); _temp5809;}); break; case 258: _LL5808: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5821=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5821[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5822; _temp5822.tag= Cyc_Stmt_tok_tag; _temp5822.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5823= yyls; struct
Cyc_Yyltype* _temp5825= _temp5823.curr +( yylsp_offset - 1); if( _temp5823.base
== 0? 1:( _temp5825 < _temp5823.base? 1: _temp5825 >= _temp5823.last_plus_one)){
_throw( Null_Exception);}* _temp5825;}).first_line,({ struct _tagged_ptr3
_temp5826= yyls; struct Cyc_Yyltype* _temp5828= _temp5826.curr +( yylsp_offset -
1); if( _temp5826.base == 0? 1:( _temp5828 < _temp5826.base? 1: _temp5828 >=
_temp5826.last_plus_one)){ _throw( Null_Exception);}* _temp5828;}).last_line));
_temp5822;}); _temp5821;}); break; case 259: _LL5820: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5830=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5830[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5831; _temp5831.tag= Cyc_Stmt_tok_tag; _temp5831.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5832= yyls; struct
Cyc_Yyltype* _temp5834= _temp5832.curr +( yylsp_offset - 3); if( _temp5832.base
== 0? 1:( _temp5834 < _temp5832.base? 1: _temp5834 >= _temp5832.last_plus_one)){
_throw( Null_Exception);}* _temp5834;}).first_line,({ struct _tagged_ptr3
_temp5835= yyls; struct Cyc_Yyltype* _temp5837= _temp5835.curr +( yylsp_offset -
3); if( _temp5835.base == 0? 1:( _temp5837 < _temp5835.base? 1: _temp5837 >=
_temp5835.last_plus_one)){ _throw( Null_Exception);}* _temp5837;}).last_line));
_temp5831;}); _temp5830;}); break; case 260: _LL5829: yyval=( struct
_xtunion_struct*)({ struct Cyc_Stmt_tok_struct* _temp5839=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5839[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5840; _temp5840.tag= Cyc_Stmt_tok_tag; _temp5840.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp5841= yyvs; struct
_xtunion_struct** _temp5843= _temp5841.curr +( yyvsp_offset - 2); if( _temp5841.base
== 0? 1:( _temp5843 < _temp5841.base? 1: _temp5843 >= _temp5841.last_plus_one)){
_throw( Null_Exception);}* _temp5843;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5844= yyls; struct Cyc_Yyltype* _temp5846= _temp5844.curr +(
yylsp_offset - 4); if( _temp5844.base == 0? 1:( _temp5846 < _temp5844.base? 1:
_temp5846 >= _temp5844.last_plus_one)){ _throw( Null_Exception);}* _temp5846;}).first_line,({
struct _tagged_ptr3 _temp5847= yyls; struct Cyc_Yyltype* _temp5849= _temp5847.curr
+( yylsp_offset - 4); if( _temp5847.base == 0? 1:( _temp5849 < _temp5847.base? 1:
_temp5849 >= _temp5847.last_plus_one)){ _throw( Null_Exception);}* _temp5849;}).last_line));
_temp5840;}); _temp5839;}); break; case 261: _LL5838: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5851=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5851[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5852; _temp5852.tag= Cyc_Pattern_tok_tag; _temp5852.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5853= yyls; struct Cyc_Yyltype* _temp5855= _temp5853.curr + yylsp_offset;
if( _temp5853.base == 0? 1:( _temp5855 < _temp5853.base? 1: _temp5855 >=
_temp5853.last_plus_one)){ _throw( Null_Exception);}* _temp5855;}).first_line,({
struct _tagged_ptr3 _temp5856= yyls; struct Cyc_Yyltype* _temp5858= _temp5856.curr
+ yylsp_offset; if( _temp5856.base == 0? 1:( _temp5858 < _temp5856.base? 1:
_temp5858 >= _temp5856.last_plus_one)){ _throw( Null_Exception);}* _temp5858;}).last_line));
_temp5852;}); _temp5851;}); break; case 262: _LL5850: yyval=({ struct
_tagged_ptr2 _temp5860= yyvs; struct _xtunion_struct** _temp5862= _temp5860.curr
+( yyvsp_offset - 1); if( _temp5860.base == 0? 1:( _temp5862 < _temp5860.base? 1:
_temp5862 >= _temp5860.last_plus_one)){ _throw( Null_Exception);}* _temp5862;});
break; case 263: _LL5859: yyval=( struct _xtunion_struct*)({ struct Cyc_Pattern_tok_struct*
_temp5864=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5864[ 0]=({ struct Cyc_Pattern_tok_struct _temp5865; _temp5865.tag= Cyc_Pattern_tok_tag;
_temp5865.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp5866=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp5866[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp5867; _temp5867.tag= Cyc_Absyn_Int_p_tag;
_temp5867.f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5868= yyvs;
struct _xtunion_struct** _temp5870= _temp5868.curr + yyvsp_offset; if( _temp5868.base
== 0? 1:( _temp5870 < _temp5868.base? 1: _temp5870 >= _temp5868.last_plus_one)){
_throw( Null_Exception);}* _temp5870;}))).f1; _temp5867.f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5871= yyvs; struct _xtunion_struct** _temp5873=
_temp5871.curr + yyvsp_offset; if( _temp5871.base == 0? 1:( _temp5873 <
_temp5871.base? 1: _temp5873 >= _temp5871.last_plus_one)){ _throw(
Null_Exception);}* _temp5873;}))).f2; _temp5867;}); _temp5866;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5874= yyls; struct Cyc_Yyltype* _temp5876= _temp5874.curr
+ yylsp_offset; if( _temp5874.base == 0? 1:( _temp5876 < _temp5874.base? 1:
_temp5876 >= _temp5874.last_plus_one)){ _throw( Null_Exception);}* _temp5876;}).first_line,({
struct _tagged_ptr3 _temp5877= yyls; struct Cyc_Yyltype* _temp5879= _temp5877.curr
+ yylsp_offset; if( _temp5877.base == 0? 1:( _temp5879 < _temp5877.base? 1:
_temp5879 >= _temp5877.last_plus_one)){ _throw( Null_Exception);}* _temp5879;}).last_line));
_temp5865;}); _temp5864;}); break; case 264: _LL5863: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5881=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5881[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5882; _temp5882.tag= Cyc_Pattern_tok_tag; _temp5882.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp5883=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5883[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp5884; _temp5884.tag= Cyc_Absyn_Int_p_tag; _temp5884.f1=( void*)(( void*)
Cyc_Absyn_Signed); _temp5884.f2= -(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp5885= yyvs; struct _xtunion_struct** _temp5887= _temp5885.curr +
yyvsp_offset; if( _temp5885.base == 0? 1:( _temp5887 < _temp5885.base? 1:
_temp5887 >= _temp5885.last_plus_one)){ _throw( Null_Exception);}* _temp5887;}))).f2;
_temp5884;}); _temp5883;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5888= yyls; struct Cyc_Yyltype* _temp5890= _temp5888.curr +( yylsp_offset -
1); if( _temp5888.base == 0? 1:( _temp5890 < _temp5888.base? 1: _temp5890 >=
_temp5888.last_plus_one)){ _throw( Null_Exception);}* _temp5890;}).first_line,({
struct _tagged_ptr3 _temp5891= yyls; struct Cyc_Yyltype* _temp5893= _temp5891.curr
+ yylsp_offset; if( _temp5891.base == 0? 1:( _temp5893 < _temp5891.base? 1:
_temp5893 >= _temp5891.last_plus_one)){ _throw( Null_Exception);}* _temp5893;}).last_line));
_temp5882;}); _temp5881;}); break; case 265: _LL5880: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5895=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5895[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5896; _temp5896.tag= Cyc_Pattern_tok_tag; _temp5896.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp5897=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp5897[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp5898; _temp5898.tag= Cyc_Absyn_Float_p_tag;
_temp5898.f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5899= yyvs;
struct _xtunion_struct** _temp5901= _temp5899.curr + yyvsp_offset; if( _temp5899.base
== 0? 1:( _temp5901 < _temp5899.base? 1: _temp5901 >= _temp5899.last_plus_one)){
_throw( Null_Exception);}* _temp5901;})); _temp5898;}); _temp5897;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5902= yyls; struct Cyc_Yyltype* _temp5904= _temp5902.curr
+ yylsp_offset; if( _temp5902.base == 0? 1:( _temp5904 < _temp5902.base? 1:
_temp5904 >= _temp5902.last_plus_one)){ _throw( Null_Exception);}* _temp5904;}).first_line,({
struct _tagged_ptr3 _temp5905= yyls; struct Cyc_Yyltype* _temp5907= _temp5905.curr
+ yylsp_offset; if( _temp5905.base == 0? 1:( _temp5907 < _temp5905.base? 1:
_temp5907 >= _temp5905.last_plus_one)){ _throw( Null_Exception);}* _temp5907;}).last_line));
_temp5896;}); _temp5895;}); break; case 266: _LL5894: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5909=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5909[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5910; _temp5910.tag= Cyc_Pattern_tok_tag; _temp5910.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Char_p_struct* _temp5911=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp5911[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp5912; _temp5912.tag= Cyc_Absyn_Char_p_tag; _temp5912.f1= Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp5913= yyvs; struct _xtunion_struct** _temp5915=
_temp5913.curr + yyvsp_offset; if( _temp5913.base == 0? 1:( _temp5915 <
_temp5913.base? 1: _temp5915 >= _temp5913.last_plus_one)){ _throw(
Null_Exception);}* _temp5915;})); _temp5912;}); _temp5911;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5916= yyls; struct Cyc_Yyltype* _temp5918= _temp5916.curr
+ yylsp_offset; if( _temp5916.base == 0? 1:( _temp5918 < _temp5916.base? 1:
_temp5918 >= _temp5916.last_plus_one)){ _throw( Null_Exception);}* _temp5918;}).first_line,({
struct _tagged_ptr3 _temp5919= yyls; struct Cyc_Yyltype* _temp5921= _temp5919.curr
+ yylsp_offset; if( _temp5919.base == 0? 1:( _temp5921 < _temp5919.base? 1:
_temp5921 >= _temp5919.last_plus_one)){ _throw( Null_Exception);}* _temp5921;}).last_line));
_temp5910;}); _temp5909;}); break; case 267: _LL5908: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5923=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5923[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5924; _temp5924.tag= Cyc_Pattern_tok_tag; _temp5924.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5925= yyls; struct Cyc_Yyltype* _temp5927= _temp5925.curr + yylsp_offset;
if( _temp5925.base == 0? 1:( _temp5927 < _temp5925.base? 1: _temp5927 >=
_temp5925.last_plus_one)){ _throw( Null_Exception);}* _temp5927;}).first_line,({
struct _tagged_ptr3 _temp5928= yyls; struct Cyc_Yyltype* _temp5930= _temp5928.curr
+ yylsp_offset; if( _temp5928.base == 0? 1:( _temp5930 < _temp5928.base? 1:
_temp5930 >= _temp5928.last_plus_one)){ _throw( Null_Exception);}* _temp5930;}).last_line));
_temp5924;}); _temp5923;}); break; case 268: _LL5922: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5932=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5932[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5933; _temp5933.tag= Cyc_Pattern_tok_tag; _temp5933.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownId_p_struct* _temp5934=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp5934[ 0]=({
struct Cyc_Absyn_UnknownId_p_struct _temp5935; _temp5935.tag= Cyc_Absyn_UnknownId_p_tag;
_temp5935.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5936= yyvs;
struct _xtunion_struct** _temp5938= _temp5936.curr + yyvsp_offset; if( _temp5936.base
== 0? 1:( _temp5938 < _temp5936.base? 1: _temp5938 >= _temp5936.last_plus_one)){
_throw( Null_Exception);}* _temp5938;})); _temp5935;}); _temp5934;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5939= yyls; struct Cyc_Yyltype* _temp5941= _temp5939.curr
+ yylsp_offset; if( _temp5939.base == 0? 1:( _temp5941 < _temp5939.base? 1:
_temp5941 >= _temp5939.last_plus_one)){ _throw( Null_Exception);}* _temp5941;}).first_line,({
struct _tagged_ptr3 _temp5942= yyls; struct Cyc_Yyltype* _temp5944= _temp5942.curr
+ yylsp_offset; if( _temp5942.base == 0? 1:( _temp5944 < _temp5942.base? 1:
_temp5944 >= _temp5942.last_plus_one)){ _throw( Null_Exception);}* _temp5944;}).last_line));
_temp5933;}); _temp5932;}); break; case 269: _LL5931: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5946=
yyls; struct Cyc_Yyltype* _temp5948= _temp5946.curr +( yylsp_offset - 3); if(
_temp5946.base == 0? 1:( _temp5948 < _temp5946.base? 1: _temp5948 >= _temp5946.last_plus_one)){
_throw( Null_Exception);}* _temp5948;}).first_line,({ struct _tagged_ptr3
_temp5949= yyls; struct Cyc_Yyltype* _temp5951= _temp5949.curr +( yylsp_offset -
3); if( _temp5949.base == 0? 1:( _temp5951 < _temp5949.base? 1: _temp5951 >=
_temp5949.last_plus_one)){ _throw( Null_Exception);}* _temp5951;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5952= yyvs; struct
_xtunion_struct** _temp5954= _temp5952.curr +( yyvsp_offset - 3); if( _temp5952.base
== 0? 1:( _temp5954 < _temp5952.base? 1: _temp5954 >= _temp5952.last_plus_one)){
_throw( Null_Exception);}* _temp5954;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp5955=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5955[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5956; _temp5956.tag= Cyc_Pattern_tok_tag; _temp5956.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownCall_p_struct* _temp5957=( struct Cyc_Absyn_UnknownCall_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_p_struct)); _temp5957[ 0]=({
struct Cyc_Absyn_UnknownCall_p_struct _temp5958; _temp5958.tag= Cyc_Absyn_UnknownCall_p_tag;
_temp5958.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5959= yyvs;
struct _xtunion_struct** _temp5961= _temp5959.curr +( yyvsp_offset - 4); if(
_temp5959.base == 0? 1:( _temp5961 < _temp5959.base? 1: _temp5961 >= _temp5959.last_plus_one)){
_throw( Null_Exception);}* _temp5961;})); _temp5958.f2= tvs; _temp5958.f3= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp5962= yyvs; struct _xtunion_struct** _temp5964=
_temp5962.curr +( yyvsp_offset - 1); if( _temp5962.base == 0? 1:( _temp5964 <
_temp5962.base? 1: _temp5964 >= _temp5962.last_plus_one)){ _throw(
Null_Exception);}* _temp5964;})); _temp5958;}); _temp5957;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5965= yyls; struct Cyc_Yyltype* _temp5967= _temp5965.curr
+( yylsp_offset - 4); if( _temp5965.base == 0? 1:( _temp5967 < _temp5965.base? 1:
_temp5967 >= _temp5965.last_plus_one)){ _throw( Null_Exception);}* _temp5967;}).first_line,({
struct _tagged_ptr3 _temp5968= yyls; struct Cyc_Yyltype* _temp5970= _temp5968.curr
+ yylsp_offset; if( _temp5968.base == 0? 1:( _temp5970 < _temp5968.base? 1:
_temp5970 >= _temp5968.last_plus_one)){ _throw( Null_Exception);}* _temp5970;}).last_line));
_temp5956;}); _temp5955;}); break;} case 270: _LL5945: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp5972=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5972[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5973; _temp5973.tag= Cyc_Pattern_tok_tag; _temp5973.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp5974=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp5974[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp5975; _temp5975.tag= Cyc_Absyn_Tuple_p_tag;
_temp5975.f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5976= yyvs;
struct _xtunion_struct** _temp5978= _temp5976.curr +( yyvsp_offset - 1); if(
_temp5976.base == 0? 1:( _temp5978 < _temp5976.base? 1: _temp5978 >= _temp5976.last_plus_one)){
_throw( Null_Exception);}* _temp5978;})); _temp5975;}); _temp5974;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5979= yyls; struct Cyc_Yyltype* _temp5981= _temp5979.curr
+( yylsp_offset - 3); if( _temp5979.base == 0? 1:( _temp5981 < _temp5979.base? 1:
_temp5981 >= _temp5979.last_plus_one)){ _throw( Null_Exception);}* _temp5981;}).first_line,({
struct _tagged_ptr3 _temp5982= yyls; struct Cyc_Yyltype* _temp5984= _temp5982.curr
+ yylsp_offset; if( _temp5982.base == 0? 1:( _temp5984 < _temp5982.base? 1:
_temp5984 >= _temp5982.last_plus_one)){ _throw( Null_Exception);}* _temp5984;}).last_line));
_temp5973;}); _temp5972;}); break; case 271: _LL5971: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5986=
yyls; struct Cyc_Yyltype* _temp5988= _temp5986.curr +( yylsp_offset - 2); if(
_temp5986.base == 0? 1:( _temp5988 < _temp5986.base? 1: _temp5988 >= _temp5986.last_plus_one)){
_throw( Null_Exception);}* _temp5988;}).first_line,({ struct _tagged_ptr3
_temp5989= yyls; struct Cyc_Yyltype* _temp5991= _temp5989.curr +( yylsp_offset -
2); if( _temp5989.base == 0? 1:( _temp5991 < _temp5989.base? 1: _temp5991 >=
_temp5989.last_plus_one)){ _throw( Null_Exception);}* _temp5991;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5992= yyvs; struct
_xtunion_struct** _temp5994= _temp5992.curr +( yyvsp_offset - 2); if( _temp5992.base
== 0? 1:( _temp5994 < _temp5992.base? 1: _temp5994 >= _temp5992.last_plus_one)){
_throw( Null_Exception);}* _temp5994;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp5995=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp5995[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5996; _temp5996.tag= Cyc_Pattern_tok_tag; _temp5996.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp5997=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp5997[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp5998; _temp5998.tag= Cyc_Absyn_UnknownFields_p_tag;
_temp5998.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5999= yyvs;
struct _xtunion_struct** _temp6001= _temp5999.curr +( yyvsp_offset - 3); if(
_temp5999.base == 0? 1:( _temp6001 < _temp5999.base? 1: _temp6001 >= _temp5999.last_plus_one)){
_throw( Null_Exception);}* _temp6001;})); _temp5998.f2= tvs; _temp5998.f3= 0;
_temp5998;}); _temp5997;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6002= yyls; struct Cyc_Yyltype* _temp6004= _temp6002.curr +( yylsp_offset -
3); if( _temp6002.base == 0? 1:( _temp6004 < _temp6002.base? 1: _temp6004 >=
_temp6002.last_plus_one)){ _throw( Null_Exception);}* _temp6004;}).first_line,({
struct _tagged_ptr3 _temp6005= yyls; struct Cyc_Yyltype* _temp6007= _temp6005.curr
+ yylsp_offset; if( _temp6005.base == 0? 1:( _temp6007 < _temp6005.base? 1:
_temp6007 >= _temp6005.last_plus_one)){ _throw( Null_Exception);}* _temp6007;}).last_line));
_temp5996;}); _temp5995;}); break;} case 272: _LL5985: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6009=
yyls; struct Cyc_Yyltype* _temp6011= _temp6009.curr +( yylsp_offset - 3); if(
_temp6009.base == 0? 1:( _temp6011 < _temp6009.base? 1: _temp6011 >= _temp6009.last_plus_one)){
_throw( Null_Exception);}* _temp6011;}).first_line,({ struct _tagged_ptr3
_temp6012= yyls; struct Cyc_Yyltype* _temp6014= _temp6012.curr +( yylsp_offset -
3); if( _temp6012.base == 0? 1:( _temp6014 < _temp6012.base? 1: _temp6014 >=
_temp6012.last_plus_one)){ _throw( Null_Exception);}* _temp6014;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6015= yyvs; struct
_xtunion_struct** _temp6017= _temp6015.curr +( yyvsp_offset - 3); if( _temp6015.base
== 0? 1:( _temp6017 < _temp6015.base? 1: _temp6017 >= _temp6015.last_plus_one)){
_throw( Null_Exception);}* _temp6017;}))); yyval=( struct _xtunion_struct*)({
struct Cyc_Pattern_tok_struct* _temp6018=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6018[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6019; _temp6019.tag= Cyc_Pattern_tok_tag; _temp6019.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp6020=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp6020[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp6021; _temp6021.tag= Cyc_Absyn_UnknownFields_p_tag;
_temp6021.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6022= yyvs;
struct _xtunion_struct** _temp6024= _temp6022.curr +( yyvsp_offset - 4); if(
_temp6022.base == 0? 1:( _temp6024 < _temp6022.base? 1: _temp6024 >= _temp6022.last_plus_one)){
_throw( Null_Exception);}* _temp6024;})); _temp6021.f2= tvs; _temp6021.f3= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6025= yyvs; struct _xtunion_struct** _temp6027=
_temp6025.curr +( yyvsp_offset - 1); if( _temp6025.base == 0? 1:( _temp6027 <
_temp6025.base? 1: _temp6027 >= _temp6025.last_plus_one)){ _throw(
Null_Exception);}* _temp6027;})); _temp6021;}); _temp6020;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6028= yyls; struct Cyc_Yyltype* _temp6030= _temp6028.curr
+( yylsp_offset - 4); if( _temp6028.base == 0? 1:( _temp6030 < _temp6028.base? 1:
_temp6030 >= _temp6028.last_plus_one)){ _throw( Null_Exception);}* _temp6030;}).first_line,({
struct _tagged_ptr3 _temp6031= yyls; struct Cyc_Yyltype* _temp6033= _temp6031.curr
+ yylsp_offset; if( _temp6031.base == 0? 1:( _temp6033 < _temp6031.base? 1:
_temp6033 >= _temp6031.last_plus_one)){ _throw( Null_Exception);}* _temp6033;}).last_line));
_temp6019;}); _temp6018;}); break;} case 273: _LL6008: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6035=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6035[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6036; _temp6036.tag= Cyc_Pattern_tok_tag; _temp6036.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp6037=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp6037[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp6038; _temp6038.tag= Cyc_Absyn_Pointer_p_tag;
_temp6038.f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6039= yyvs;
struct _xtunion_struct** _temp6041= _temp6039.curr + yyvsp_offset; if( _temp6039.base
== 0? 1:( _temp6041 < _temp6039.base? 1: _temp6041 >= _temp6039.last_plus_one)){
_throw( Null_Exception);}* _temp6041;})); _temp6038;}); _temp6037;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6042= yyls; struct Cyc_Yyltype* _temp6044= _temp6042.curr
+( yylsp_offset - 1); if( _temp6042.base == 0? 1:( _temp6044 < _temp6042.base? 1:
_temp6044 >= _temp6042.last_plus_one)){ _throw( Null_Exception);}* _temp6044;}).first_line,({
struct _tagged_ptr3 _temp6045= yyls; struct Cyc_Yyltype* _temp6047= _temp6045.curr
+ yylsp_offset; if( _temp6045.base == 0? 1:( _temp6047 < _temp6045.base? 1:
_temp6047 >= _temp6045.last_plus_one)){ _throw( Null_Exception);}* _temp6047;}).last_line));
_temp6036;}); _temp6035;}); break; case 274: _LL6034: yyval=( struct
_xtunion_struct*)({ struct Cyc_Pattern_tok_struct* _temp6049=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6049[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6050; _temp6050.tag= Cyc_Pattern_tok_tag; _temp6050.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp6051=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp6051[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp6052; _temp6052.tag= Cyc_Absyn_Reference_p_tag;
_temp6052.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp6053=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6053->f1=( void*) Cyc_Absyn_Loc_n;
_temp6053->f2=({ struct _tagged_string* _temp6054=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp6054[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6055= yyvs; struct _xtunion_struct** _temp6057=
_temp6055.curr + yyvsp_offset; if( _temp6055.base == 0? 1:( _temp6057 <
_temp6055.base? 1: _temp6057 >= _temp6055.last_plus_one)){ _throw(
Null_Exception);}* _temp6057;})); _temp6054;}); _temp6053;}),( void*) Cyc_Absyn_VoidType,
0); _temp6052;}); _temp6051;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6058= yyls; struct Cyc_Yyltype* _temp6060= _temp6058.curr +(
yylsp_offset - 1); if( _temp6058.base == 0? 1:( _temp6060 < _temp6058.base? 1:
_temp6060 >= _temp6058.last_plus_one)){ _throw( Null_Exception);}* _temp6060;}).first_line,({
struct _tagged_ptr3 _temp6061= yyls; struct Cyc_Yyltype* _temp6063= _temp6061.curr
+ yylsp_offset; if( _temp6061.base == 0? 1:( _temp6063 < _temp6061.base? 1:
_temp6063 >= _temp6061.last_plus_one)){ _throw( Null_Exception);}* _temp6063;}).last_line));
_temp6050;}); _temp6049;}); break; case 275: _LL6048: yyval=( struct
_xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6065=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6065[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6066; _temp6066.tag= Cyc_PatternList_tok_tag;
_temp6066.f1= 0; _temp6066;}); _temp6065;}); break; case 276: _LL6064: yyval=(
struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct* _temp6068=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6068[ 0]=({ struct Cyc_PatternList_tok_struct _temp6069; _temp6069.tag= Cyc_PatternList_tok_tag;
_temp6069.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6070= yyvs; struct
_xtunion_struct** _temp6072= _temp6070.curr + yyvsp_offset; if( _temp6070.base
== 0? 1:( _temp6072 < _temp6070.base? 1: _temp6072 >= _temp6070.last_plus_one)){
_throw( Null_Exception);}* _temp6072;}))); _temp6069;}); _temp6068;}); break;
case 277: _LL6067: yyval=( struct _xtunion_struct*)({ struct Cyc_PatternList_tok_struct*
_temp6074=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6074[ 0]=({ struct Cyc_PatternList_tok_struct _temp6075; _temp6075.tag= Cyc_PatternList_tok_tag;
_temp6075.f1=({ struct Cyc_List_List* _temp6076=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6076->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6077= yyvs; struct _xtunion_struct** _temp6079=
_temp6077.curr + yyvsp_offset; if( _temp6077.base == 0? 1:( _temp6079 <
_temp6077.base? 1: _temp6079 >= _temp6077.last_plus_one)){ _throw(
Null_Exception);}* _temp6079;})); _temp6076->tl= 0; _temp6076;}); _temp6075;});
_temp6074;}); break; case 278: _LL6073: yyval=( struct _xtunion_struct*)({
struct Cyc_PatternList_tok_struct* _temp6081=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6081[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6082; _temp6082.tag= Cyc_PatternList_tok_tag;
_temp6082.f1=({ struct Cyc_List_List* _temp6083=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6083->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6087= yyvs; struct _xtunion_struct** _temp6089=
_temp6087.curr + yyvsp_offset; if( _temp6087.base == 0? 1:( _temp6089 <
_temp6087.base? 1: _temp6089 >= _temp6087.last_plus_one)){ _throw(
Null_Exception);}* _temp6089;})); _temp6083->tl= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp6084= yyvs; struct _xtunion_struct** _temp6086=
_temp6084.curr +( yyvsp_offset - 2); if( _temp6084.base == 0? 1:( _temp6086 <
_temp6084.base? 1: _temp6086 >= _temp6084.last_plus_one)){ _throw(
Null_Exception);}* _temp6086;})); _temp6083;}); _temp6082;}); _temp6081;});
break; case 279: _LL6080: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6091=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6091[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6092; _temp6092.tag=
Cyc_FieldPattern_tok_tag; _temp6092.f1=({ struct _tuple17* _temp6093=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6093->f1= 0; _temp6093->f2=
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6094= yyvs; struct
_xtunion_struct** _temp6096= _temp6094.curr + yyvsp_offset; if( _temp6094.base
== 0? 1:( _temp6096 < _temp6094.base? 1: _temp6096 >= _temp6094.last_plus_one)){
_throw( Null_Exception);}* _temp6096;})); _temp6093;}); _temp6092;}); _temp6091;});
break; case 280: _LL6090: yyval=( struct _xtunion_struct*)({ struct Cyc_FieldPattern_tok_struct*
_temp6098=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6098[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6099; _temp6099.tag=
Cyc_FieldPattern_tok_tag; _temp6099.f1=({ struct _tuple17* _temp6100=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6100->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp6104= yyvs; struct _xtunion_struct** _temp6106=
_temp6104.curr +( yyvsp_offset - 1); if( _temp6104.base == 0? 1:( _temp6106 <
_temp6104.base? 1: _temp6106 >= _temp6104.last_plus_one)){ _throw(
Null_Exception);}* _temp6106;})); _temp6100->f2= Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp6101= yyvs; struct _xtunion_struct** _temp6103= _temp6101.curr
+ yyvsp_offset; if( _temp6101.base == 0? 1:( _temp6103 < _temp6101.base? 1:
_temp6103 >= _temp6101.last_plus_one)){ _throw( Null_Exception);}* _temp6103;}));
_temp6100;}); _temp6099;}); _temp6098;}); break; case 281: _LL6097: yyval=(
struct _xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6108=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6108[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6109; _temp6109.tag=
Cyc_FieldPatternList_tok_tag; _temp6109.f1=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({ struct
_tagged_ptr2 _temp6110= yyvs; struct _xtunion_struct** _temp6112= _temp6110.curr
+ yyvsp_offset; if( _temp6110.base == 0? 1:( _temp6112 < _temp6110.base? 1:
_temp6112 >= _temp6110.last_plus_one)){ _throw( Null_Exception);}* _temp6112;})));
_temp6109;}); _temp6108;}); break; case 282: _LL6107: yyval=( struct
_xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6114=( struct
Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6114[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6115; _temp6115.tag=
Cyc_FieldPatternList_tok_tag; _temp6115.f1=({ struct Cyc_List_List* _temp6116=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6116->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6117= yyvs; struct
_xtunion_struct** _temp6119= _temp6117.curr + yyvsp_offset; if( _temp6117.base
== 0? 1:( _temp6119 < _temp6117.base? 1: _temp6119 >= _temp6117.last_plus_one)){
_throw( Null_Exception);}* _temp6119;})); _temp6116->tl= 0; _temp6116;});
_temp6115;}); _temp6114;}); break; case 283: _LL6113: yyval=( struct
_xtunion_struct*)({ struct Cyc_FieldPatternList_tok_struct* _temp6121=( struct
Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6121[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6122; _temp6122.tag=
Cyc_FieldPatternList_tok_tag; _temp6122.f1=({ struct Cyc_List_List* _temp6123=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6123->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6127= yyvs; struct
_xtunion_struct** _temp6129= _temp6127.curr + yyvsp_offset; if( _temp6127.base
== 0? 1:( _temp6129 < _temp6127.base? 1: _temp6129 >= _temp6127.last_plus_one)){
_throw( Null_Exception);}* _temp6129;})); _temp6123->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6124= yyvs; struct _xtunion_struct** _temp6126=
_temp6124.curr +( yyvsp_offset - 2); if( _temp6124.base == 0? 1:( _temp6126 <
_temp6124.base? 1: _temp6126 >= _temp6124.last_plus_one)){ _throw(
Null_Exception);}* _temp6126;})); _temp6123;}); _temp6122;}); _temp6121;});
break; case 284: _LL6120: yyval=({ struct _tagged_ptr2 _temp6131= yyvs; struct
_xtunion_struct** _temp6133= _temp6131.curr + yyvsp_offset; if( _temp6131.base
== 0? 1:( _temp6133 < _temp6131.base? 1: _temp6133 >= _temp6131.last_plus_one)){
_throw( Null_Exception);}* _temp6133;}); break; case 285: _LL6130: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6135=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6135[ 0]=({ struct Cyc_Exp_tok_struct
_temp6136; _temp6136.tag= Cyc_Exp_tok_tag; _temp6136.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6137= yyvs; struct _xtunion_struct** _temp6139=
_temp6137.curr +( yyvsp_offset - 2); if( _temp6137.base == 0? 1:( _temp6139 <
_temp6137.base? 1: _temp6139 >= _temp6137.last_plus_one)){ _throw(
Null_Exception);}* _temp6139;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6140= yyvs; struct _xtunion_struct** _temp6142= _temp6140.curr +
yyvsp_offset; if( _temp6140.base == 0? 1:( _temp6142 < _temp6140.base? 1:
_temp6142 >= _temp6140.last_plus_one)){ _throw( Null_Exception);}* _temp6142;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6143= yyls; struct Cyc_Yyltype*
_temp6145= _temp6143.curr +( yylsp_offset - 2); if( _temp6143.base == 0? 1:(
_temp6145 < _temp6143.base? 1: _temp6145 >= _temp6143.last_plus_one)){ _throw(
Null_Exception);}* _temp6145;}).first_line,({ struct _tagged_ptr3 _temp6146=
yyls; struct Cyc_Yyltype* _temp6148= _temp6146.curr + yylsp_offset; if(
_temp6146.base == 0? 1:( _temp6148 < _temp6146.base? 1: _temp6148 >= _temp6146.last_plus_one)){
_throw( Null_Exception);}* _temp6148;}).last_line)); _temp6136;}); _temp6135;});
break; case 286: _LL6134: yyval=({ struct _tagged_ptr2 _temp6150= yyvs; struct
_xtunion_struct** _temp6152= _temp6150.curr + yyvsp_offset; if( _temp6150.base
== 0? 1:( _temp6152 < _temp6150.base? 1: _temp6152 >= _temp6150.last_plus_one)){
_throw( Null_Exception);}* _temp6152;}); break; case 287: _LL6149: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6154=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6154[ 0]=({ struct Cyc_Exp_tok_struct
_temp6155; _temp6155.tag= Cyc_Exp_tok_tag; _temp6155.f1= Cyc_Absyn_assignop_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6156= yyvs; struct _xtunion_struct**
_temp6158= _temp6156.curr +( yyvsp_offset - 2); if( _temp6156.base == 0? 1:(
_temp6158 < _temp6156.base? 1: _temp6158 >= _temp6156.last_plus_one)){ _throw(
Null_Exception);}* _temp6158;})), Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2
_temp6159= yyvs; struct _xtunion_struct** _temp6161= _temp6159.curr +(
yyvsp_offset - 1); if( _temp6159.base == 0? 1:( _temp6161 < _temp6159.base? 1:
_temp6161 >= _temp6159.last_plus_one)){ _throw( Null_Exception);}* _temp6161;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6162= yyvs; struct _xtunion_struct**
_temp6164= _temp6162.curr + yyvsp_offset; if( _temp6162.base == 0? 1:( _temp6164
< _temp6162.base? 1: _temp6164 >= _temp6162.last_plus_one)){ _throw(
Null_Exception);}* _temp6164;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6165= yyls; struct Cyc_Yyltype* _temp6167= _temp6165.curr +(
yylsp_offset - 2); if( _temp6165.base == 0? 1:( _temp6167 < _temp6165.base? 1:
_temp6167 >= _temp6165.last_plus_one)){ _throw( Null_Exception);}* _temp6167;}).first_line,({
struct _tagged_ptr3 _temp6168= yyls; struct Cyc_Yyltype* _temp6170= _temp6168.curr
+ yylsp_offset; if( _temp6168.base == 0? 1:( _temp6170 < _temp6168.base? 1:
_temp6170 >= _temp6168.last_plus_one)){ _throw( Null_Exception);}* _temp6170;}).last_line));
_temp6155;}); _temp6154;}); break; case 288: _LL6153: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6172=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6172[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6173; _temp6173.tag= Cyc_Primopopt_tok_tag;
_temp6173.f1= 0; _temp6173;}); _temp6172;}); break; case 289: _LL6171: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6175=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6175[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6176; _temp6176.tag= Cyc_Primopopt_tok_tag;
_temp6176.f1=({ struct Cyc_Core_Opt* _temp6177=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6177->v=( void*)(( void*) Cyc_Absyn_Times);
_temp6177;}); _temp6176;}); _temp6175;}); break; case 290: _LL6174: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6179=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6179[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6180; _temp6180.tag= Cyc_Primopopt_tok_tag;
_temp6180.f1=({ struct Cyc_Core_Opt* _temp6181=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6181->v=( void*)(( void*) Cyc_Absyn_Div);
_temp6181;}); _temp6180;}); _temp6179;}); break; case 291: _LL6178: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6183=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6183[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6184; _temp6184.tag= Cyc_Primopopt_tok_tag;
_temp6184.f1=({ struct Cyc_Core_Opt* _temp6185=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6185->v=( void*)(( void*) Cyc_Absyn_Mod);
_temp6185;}); _temp6184;}); _temp6183;}); break; case 292: _LL6182: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6187=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6187[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6188; _temp6188.tag= Cyc_Primopopt_tok_tag;
_temp6188.f1=({ struct Cyc_Core_Opt* _temp6189=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6189->v=( void*)(( void*) Cyc_Absyn_Plus);
_temp6189;}); _temp6188;}); _temp6187;}); break; case 293: _LL6186: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6191=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6191[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6192; _temp6192.tag= Cyc_Primopopt_tok_tag;
_temp6192.f1=({ struct Cyc_Core_Opt* _temp6193=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6193->v=( void*)(( void*) Cyc_Absyn_Minus);
_temp6193;}); _temp6192;}); _temp6191;}); break; case 294: _LL6190: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6195=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6195[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6196; _temp6196.tag= Cyc_Primopopt_tok_tag;
_temp6196.f1=({ struct Cyc_Core_Opt* _temp6197=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6197->v=( void*)(( void*) Cyc_Absyn_Bitlshift);
_temp6197;}); _temp6196;}); _temp6195;}); break; case 295: _LL6194: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6199=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6199[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6200; _temp6200.tag= Cyc_Primopopt_tok_tag;
_temp6200.f1=({ struct Cyc_Core_Opt* _temp6201=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6201->v=( void*)(( void*) Cyc_Absyn_Bitlrshift);
_temp6201;}); _temp6200;}); _temp6199;}); break; case 296: _LL6198: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6203=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6203[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6204; _temp6204.tag= Cyc_Primopopt_tok_tag;
_temp6204.f1=({ struct Cyc_Core_Opt* _temp6205=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6205->v=( void*)(( void*) Cyc_Absyn_Bitand);
_temp6205;}); _temp6204;}); _temp6203;}); break; case 297: _LL6202: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6207=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6207[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6208; _temp6208.tag= Cyc_Primopopt_tok_tag;
_temp6208.f1=({ struct Cyc_Core_Opt* _temp6209=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6209->v=( void*)(( void*) Cyc_Absyn_Bitxor);
_temp6209;}); _temp6208;}); _temp6207;}); break; case 298: _LL6206: yyval=(
struct _xtunion_struct*)({ struct Cyc_Primopopt_tok_struct* _temp6211=( struct
Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp6211[ 0]=({ struct Cyc_Primopopt_tok_struct _temp6212; _temp6212.tag= Cyc_Primopopt_tok_tag;
_temp6212.f1=({ struct Cyc_Core_Opt* _temp6213=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp6213->v=( void*)(( void*) Cyc_Absyn_Bitor);
_temp6213;}); _temp6212;}); _temp6211;}); break; case 299: _LL6210: yyval=({
struct _tagged_ptr2 _temp6215= yyvs; struct _xtunion_struct** _temp6217=
_temp6215.curr + yyvsp_offset; if( _temp6215.base == 0? 1:( _temp6217 <
_temp6215.base? 1: _temp6217 >= _temp6215.last_plus_one)){ _throw(
Null_Exception);}* _temp6217;}); break; case 300: _LL6214: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6219=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6219[ 0]=({ struct Cyc_Exp_tok_struct
_temp6220; _temp6220.tag= Cyc_Exp_tok_tag; _temp6220.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6221= yyvs; struct _xtunion_struct**
_temp6223= _temp6221.curr +( yyvsp_offset - 4); if( _temp6221.base == 0? 1:(
_temp6223 < _temp6221.base? 1: _temp6223 >= _temp6221.last_plus_one)){ _throw(
Null_Exception);}* _temp6223;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6224= yyvs; struct _xtunion_struct** _temp6226= _temp6224.curr +(
yyvsp_offset - 2); if( _temp6224.base == 0? 1:( _temp6226 < _temp6224.base? 1:
_temp6226 >= _temp6224.last_plus_one)){ _throw( Null_Exception);}* _temp6226;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6227= yyvs; struct _xtunion_struct**
_temp6229= _temp6227.curr + yyvsp_offset; if( _temp6227.base == 0? 1:( _temp6229
< _temp6227.base? 1: _temp6229 >= _temp6227.last_plus_one)){ _throw(
Null_Exception);}* _temp6229;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6230= yyls; struct Cyc_Yyltype* _temp6232= _temp6230.curr +(
yylsp_offset - 4); if( _temp6230.base == 0? 1:( _temp6232 < _temp6230.base? 1:
_temp6232 >= _temp6230.last_plus_one)){ _throw( Null_Exception);}* _temp6232;}).first_line,({
struct _tagged_ptr3 _temp6233= yyls; struct Cyc_Yyltype* _temp6235= _temp6233.curr
+ yylsp_offset; if( _temp6233.base == 0? 1:( _temp6235 < _temp6233.base? 1:
_temp6235 >= _temp6233.last_plus_one)){ _throw( Null_Exception);}* _temp6235;}).last_line));
_temp6220;}); _temp6219;}); break; case 301: _LL6218: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6237=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6237[ 0]=({ struct Cyc_Exp_tok_struct
_temp6238; _temp6238.tag= Cyc_Exp_tok_tag; _temp6238.f1= Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6239= yyvs; struct _xtunion_struct**
_temp6241= _temp6239.curr + yyvsp_offset; if( _temp6239.base == 0? 1:( _temp6241
< _temp6239.base? 1: _temp6241 >= _temp6239.last_plus_one)){ _throw(
Null_Exception);}* _temp6241;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6242= yyls; struct Cyc_Yyltype* _temp6244= _temp6242.curr +(
yylsp_offset - 1); if( _temp6242.base == 0? 1:( _temp6244 < _temp6242.base? 1:
_temp6244 >= _temp6242.last_plus_one)){ _throw( Null_Exception);}* _temp6244;}).first_line,({
struct _tagged_ptr3 _temp6245= yyls; struct Cyc_Yyltype* _temp6247= _temp6245.curr
+ yylsp_offset; if( _temp6245.base == 0? 1:( _temp6247 < _temp6245.base? 1:
_temp6247 >= _temp6245.last_plus_one)){ _throw( Null_Exception);}* _temp6247;}).last_line));
_temp6238;}); _temp6237;}); break; case 302: _LL6236: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6249=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6249[ 0]=({ struct Cyc_Exp_tok_struct
_temp6250; _temp6250.tag= Cyc_Exp_tok_tag; _temp6250.f1= Cyc_Absyn_New_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6251= yyvs; struct _xtunion_struct** _temp6253=
_temp6251.curr + yyvsp_offset; if( _temp6251.base == 0? 1:( _temp6253 <
_temp6251.base? 1: _temp6253 >= _temp6251.last_plus_one)){ _throw(
Null_Exception);}* _temp6253;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6254= yyls; struct Cyc_Yyltype* _temp6256= _temp6254.curr +(
yylsp_offset - 1); if( _temp6254.base == 0? 1:( _temp6256 < _temp6254.base? 1:
_temp6256 >= _temp6254.last_plus_one)){ _throw( Null_Exception);}* _temp6256;}).first_line,({
struct _tagged_ptr3 _temp6257= yyls; struct Cyc_Yyltype* _temp6259= _temp6257.curr
+( yylsp_offset + 1); if( _temp6257.base == 0? 1:( _temp6259 < _temp6257.base? 1:
_temp6259 >= _temp6257.last_plus_one)){ _throw( Null_Exception);}* _temp6259;}).last_line));
_temp6250;}); _temp6249;}); break; case 303: _LL6248: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6261=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6261[ 0]=({ struct Cyc_Exp_tok_struct
_temp6262; _temp6262.tag= Cyc_Exp_tok_tag; _temp6262.f1= Cyc_Absyn_New_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6263= yyvs; struct _xtunion_struct** _temp6265=
_temp6263.curr + yyvsp_offset; if( _temp6263.base == 0? 1:( _temp6265 <
_temp6263.base? 1: _temp6265 >= _temp6263.last_plus_one)){ _throw(
Null_Exception);}* _temp6265;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6266= yyls; struct Cyc_Yyltype* _temp6268= _temp6266.curr +(
yylsp_offset - 1); if( _temp6266.base == 0? 1:( _temp6268 < _temp6266.base? 1:
_temp6268 >= _temp6266.last_plus_one)){ _throw( Null_Exception);}* _temp6268;}).first_line,({
struct _tagged_ptr3 _temp6269= yyls; struct Cyc_Yyltype* _temp6271= _temp6269.curr
+( yylsp_offset + 1); if( _temp6269.base == 0? 1:( _temp6271 < _temp6269.base? 1:
_temp6271 >= _temp6269.last_plus_one)){ _throw( Null_Exception);}* _temp6271;}).last_line));
_temp6262;}); _temp6261;}); break; case 304: _LL6260: yyval=({ struct
_tagged_ptr2 _temp6273= yyvs; struct _xtunion_struct** _temp6275= _temp6273.curr
+ yyvsp_offset; if( _temp6273.base == 0? 1:( _temp6275 < _temp6273.base? 1:
_temp6275 >= _temp6273.last_plus_one)){ _throw( Null_Exception);}* _temp6275;});
break; case 305: _LL6272: yyval=({ struct _tagged_ptr2 _temp6277= yyvs; struct
_xtunion_struct** _temp6279= _temp6277.curr + yyvsp_offset; if( _temp6277.base
== 0? 1:( _temp6279 < _temp6277.base? 1: _temp6279 >= _temp6277.last_plus_one)){
_throw( Null_Exception);}* _temp6279;}); break; case 306: _LL6276: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6281=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6281[ 0]=({ struct Cyc_Exp_tok_struct
_temp6282; _temp6282.tag= Cyc_Exp_tok_tag; _temp6282.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6283= yyvs; struct _xtunion_struct** _temp6285=
_temp6283.curr +( yyvsp_offset - 2); if( _temp6283.base == 0? 1:( _temp6285 <
_temp6283.base? 1: _temp6285 >= _temp6283.last_plus_one)){ _throw(
Null_Exception);}* _temp6285;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6286= yyvs; struct _xtunion_struct** _temp6288= _temp6286.curr +
yyvsp_offset; if( _temp6286.base == 0? 1:( _temp6288 < _temp6286.base? 1:
_temp6288 >= _temp6286.last_plus_one)){ _throw( Null_Exception);}* _temp6288;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6289= yyls; struct Cyc_Yyltype*
_temp6291= _temp6289.curr +( yylsp_offset - 2); if( _temp6289.base == 0? 1:(
_temp6291 < _temp6289.base? 1: _temp6291 >= _temp6289.last_plus_one)){ _throw(
Null_Exception);}* _temp6291;}).first_line,({ struct _tagged_ptr3 _temp6292=
yyls; struct Cyc_Yyltype* _temp6294= _temp6292.curr + yylsp_offset; if(
_temp6292.base == 0? 1:( _temp6294 < _temp6292.base? 1: _temp6294 >= _temp6292.last_plus_one)){
_throw( Null_Exception);}* _temp6294;}).last_line)); _temp6282;}); _temp6281;});
break; case 307: _LL6280: yyval=({ struct _tagged_ptr2 _temp6296= yyvs; struct
_xtunion_struct** _temp6298= _temp6296.curr + yyvsp_offset; if( _temp6296.base
== 0? 1:( _temp6298 < _temp6296.base? 1: _temp6298 >= _temp6296.last_plus_one)){
_throw( Null_Exception);}* _temp6298;}); break; case 308: _LL6295: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6300=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6300[ 0]=({ struct Cyc_Exp_tok_struct
_temp6301; _temp6301.tag= Cyc_Exp_tok_tag; _temp6301.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6302= yyvs; struct _xtunion_struct** _temp6304=
_temp6302.curr +( yyvsp_offset - 2); if( _temp6302.base == 0? 1:( _temp6304 <
_temp6302.base? 1: _temp6304 >= _temp6302.last_plus_one)){ _throw(
Null_Exception);}* _temp6304;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6305= yyvs; struct _xtunion_struct** _temp6307= _temp6305.curr +
yyvsp_offset; if( _temp6305.base == 0? 1:( _temp6307 < _temp6305.base? 1:
_temp6307 >= _temp6305.last_plus_one)){ _throw( Null_Exception);}* _temp6307;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6308= yyls; struct Cyc_Yyltype*
_temp6310= _temp6308.curr +( yylsp_offset - 2); if( _temp6308.base == 0? 1:(
_temp6310 < _temp6308.base? 1: _temp6310 >= _temp6308.last_plus_one)){ _throw(
Null_Exception);}* _temp6310;}).first_line,({ struct _tagged_ptr3 _temp6311=
yyls; struct Cyc_Yyltype* _temp6313= _temp6311.curr + yylsp_offset; if(
_temp6311.base == 0? 1:( _temp6313 < _temp6311.base? 1: _temp6313 >= _temp6311.last_plus_one)){
_throw( Null_Exception);}* _temp6313;}).last_line)); _temp6301;}); _temp6300;});
break; case 309: _LL6299: yyval=({ struct _tagged_ptr2 _temp6315= yyvs; struct
_xtunion_struct** _temp6317= _temp6315.curr + yyvsp_offset; if( _temp6315.base
== 0? 1:( _temp6317 < _temp6315.base? 1: _temp6317 >= _temp6315.last_plus_one)){
_throw( Null_Exception);}* _temp6317;}); break; case 310: _LL6314: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6319=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6319[ 0]=({ struct Cyc_Exp_tok_struct
_temp6320; _temp6320.tag= Cyc_Exp_tok_tag; _temp6320.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6321= yyvs;
struct _xtunion_struct** _temp6323= _temp6321.curr +( yyvsp_offset - 2); if(
_temp6321.base == 0? 1:( _temp6323 < _temp6321.base? 1: _temp6323 >= _temp6321.last_plus_one)){
_throw( Null_Exception);}* _temp6323;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6324= yyvs; struct _xtunion_struct** _temp6326= _temp6324.curr
+ yyvsp_offset; if( _temp6324.base == 0? 1:( _temp6326 < _temp6324.base? 1:
_temp6326 >= _temp6324.last_plus_one)){ _throw( Null_Exception);}* _temp6326;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6327= yyls; struct Cyc_Yyltype*
_temp6329= _temp6327.curr +( yylsp_offset - 2); if( _temp6327.base == 0? 1:(
_temp6329 < _temp6327.base? 1: _temp6329 >= _temp6327.last_plus_one)){ _throw(
Null_Exception);}* _temp6329;}).first_line,({ struct _tagged_ptr3 _temp6330=
yyls; struct Cyc_Yyltype* _temp6332= _temp6330.curr + yylsp_offset; if(
_temp6330.base == 0? 1:( _temp6332 < _temp6330.base? 1: _temp6332 >= _temp6330.last_plus_one)){
_throw( Null_Exception);}* _temp6332;}).last_line)); _temp6320;}); _temp6319;});
break; case 311: _LL6318: yyval=({ struct _tagged_ptr2 _temp6334= yyvs; struct
_xtunion_struct** _temp6336= _temp6334.curr + yyvsp_offset; if( _temp6334.base
== 0? 1:( _temp6336 < _temp6334.base? 1: _temp6336 >= _temp6334.last_plus_one)){
_throw( Null_Exception);}* _temp6336;}); break; case 312: _LL6333: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6338=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6338[ 0]=({ struct Cyc_Exp_tok_struct
_temp6339; _temp6339.tag= Cyc_Exp_tok_tag; _temp6339.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6340=
yyvs; struct _xtunion_struct** _temp6342= _temp6340.curr +( yyvsp_offset - 2);
if( _temp6340.base == 0? 1:( _temp6342 < _temp6340.base? 1: _temp6342 >=
_temp6340.last_plus_one)){ _throw( Null_Exception);}* _temp6342;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6343= yyvs; struct _xtunion_struct** _temp6345=
_temp6343.curr + yyvsp_offset; if( _temp6343.base == 0? 1:( _temp6345 <
_temp6343.base? 1: _temp6345 >= _temp6343.last_plus_one)){ _throw(
Null_Exception);}* _temp6345;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6346= yyls; struct Cyc_Yyltype* _temp6348= _temp6346.curr +(
yylsp_offset - 2); if( _temp6346.base == 0? 1:( _temp6348 < _temp6346.base? 1:
_temp6348 >= _temp6346.last_plus_one)){ _throw( Null_Exception);}* _temp6348;}).first_line,({
struct _tagged_ptr3 _temp6349= yyls; struct Cyc_Yyltype* _temp6351= _temp6349.curr
+ yylsp_offset; if( _temp6349.base == 0? 1:( _temp6351 < _temp6349.base? 1:
_temp6351 >= _temp6349.last_plus_one)){ _throw( Null_Exception);}* _temp6351;}).last_line));
_temp6339;}); _temp6338;}); break; case 313: _LL6337: yyval=({ struct
_tagged_ptr2 _temp6353= yyvs; struct _xtunion_struct** _temp6355= _temp6353.curr
+ yyvsp_offset; if( _temp6353.base == 0? 1:( _temp6355 < _temp6353.base? 1:
_temp6355 >= _temp6353.last_plus_one)){ _throw( Null_Exception);}* _temp6355;});
break; case 314: _LL6352: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6357=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6357[ 0]=({ struct Cyc_Exp_tok_struct _temp6358; _temp6358.tag= Cyc_Exp_tok_tag;
_temp6358.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6359= yyvs; struct _xtunion_struct** _temp6361=
_temp6359.curr +( yyvsp_offset - 2); if( _temp6359.base == 0? 1:( _temp6361 <
_temp6359.base? 1: _temp6361 >= _temp6359.last_plus_one)){ _throw(
Null_Exception);}* _temp6361;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6362= yyvs; struct _xtunion_struct** _temp6364= _temp6362.curr +
yyvsp_offset; if( _temp6362.base == 0? 1:( _temp6364 < _temp6362.base? 1:
_temp6364 >= _temp6362.last_plus_one)){ _throw( Null_Exception);}* _temp6364;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6365= yyls; struct Cyc_Yyltype*
_temp6367= _temp6365.curr +( yylsp_offset - 2); if( _temp6365.base == 0? 1:(
_temp6367 < _temp6365.base? 1: _temp6367 >= _temp6365.last_plus_one)){ _throw(
Null_Exception);}* _temp6367;}).first_line,({ struct _tagged_ptr3 _temp6368=
yyls; struct Cyc_Yyltype* _temp6370= _temp6368.curr + yylsp_offset; if(
_temp6368.base == 0? 1:( _temp6370 < _temp6368.base? 1: _temp6370 >= _temp6368.last_plus_one)){
_throw( Null_Exception);}* _temp6370;}).last_line)); _temp6358;}); _temp6357;});
break; case 315: _LL6356: yyval=({ struct _tagged_ptr2 _temp6372= yyvs; struct
_xtunion_struct** _temp6374= _temp6372.curr + yyvsp_offset; if( _temp6372.base
== 0? 1:( _temp6374 < _temp6372.base? 1: _temp6374 >= _temp6372.last_plus_one)){
_throw( Null_Exception);}* _temp6374;}); break; case 316: _LL6371: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6376=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6376[ 0]=({ struct Cyc_Exp_tok_struct
_temp6377; _temp6377.tag= Cyc_Exp_tok_tag; _temp6377.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6378= yyvs; struct _xtunion_struct** _temp6380=
_temp6378.curr +( yyvsp_offset - 2); if( _temp6378.base == 0? 1:( _temp6380 <
_temp6378.base? 1: _temp6380 >= _temp6378.last_plus_one)){ _throw(
Null_Exception);}* _temp6380;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6381= yyvs; struct _xtunion_struct** _temp6383= _temp6381.curr +
yyvsp_offset; if( _temp6381.base == 0? 1:( _temp6383 < _temp6381.base? 1:
_temp6383 >= _temp6381.last_plus_one)){ _throw( Null_Exception);}* _temp6383;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6384= yyls; struct Cyc_Yyltype*
_temp6386= _temp6384.curr +( yylsp_offset - 2); if( _temp6384.base == 0? 1:(
_temp6386 < _temp6384.base? 1: _temp6386 >= _temp6384.last_plus_one)){ _throw(
Null_Exception);}* _temp6386;}).first_line,({ struct _tagged_ptr3 _temp6387=
yyls; struct Cyc_Yyltype* _temp6389= _temp6387.curr + yylsp_offset; if(
_temp6387.base == 0? 1:( _temp6389 < _temp6387.base? 1: _temp6389 >= _temp6387.last_plus_one)){
_throw( Null_Exception);}* _temp6389;}).last_line)); _temp6377;}); _temp6376;});
break; case 317: _LL6375: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6391=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6391[ 0]=({ struct Cyc_Exp_tok_struct _temp6392; _temp6392.tag= Cyc_Exp_tok_tag;
_temp6392.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6393= yyvs; struct _xtunion_struct** _temp6395= _temp6393.curr +(
yyvsp_offset - 2); if( _temp6393.base == 0? 1:( _temp6395 < _temp6393.base? 1:
_temp6395 >= _temp6393.last_plus_one)){ _throw( Null_Exception);}* _temp6395;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6396= yyvs; struct _xtunion_struct**
_temp6398= _temp6396.curr + yyvsp_offset; if( _temp6396.base == 0? 1:( _temp6398
< _temp6396.base? 1: _temp6398 >= _temp6396.last_plus_one)){ _throw(
Null_Exception);}* _temp6398;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6399= yyls; struct Cyc_Yyltype* _temp6401= _temp6399.curr +(
yylsp_offset - 2); if( _temp6399.base == 0? 1:( _temp6401 < _temp6399.base? 1:
_temp6401 >= _temp6399.last_plus_one)){ _throw( Null_Exception);}* _temp6401;}).first_line,({
struct _tagged_ptr3 _temp6402= yyls; struct Cyc_Yyltype* _temp6404= _temp6402.curr
+ yylsp_offset; if( _temp6402.base == 0? 1:( _temp6404 < _temp6402.base? 1:
_temp6404 >= _temp6402.last_plus_one)){ _throw( Null_Exception);}* _temp6404;}).last_line));
_temp6392;}); _temp6391;}); break; case 318: _LL6390: yyval=({ struct
_tagged_ptr2 _temp6406= yyvs; struct _xtunion_struct** _temp6408= _temp6406.curr
+ yyvsp_offset; if( _temp6406.base == 0? 1:( _temp6408 < _temp6406.base? 1:
_temp6408 >= _temp6406.last_plus_one)){ _throw( Null_Exception);}* _temp6408;});
break; case 319: _LL6405: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6410=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6410[ 0]=({ struct Cyc_Exp_tok_struct _temp6411; _temp6411.tag= Cyc_Exp_tok_tag;
_temp6411.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6412= yyvs; struct _xtunion_struct** _temp6414= _temp6412.curr +(
yyvsp_offset - 2); if( _temp6412.base == 0? 1:( _temp6414 < _temp6412.base? 1:
_temp6414 >= _temp6412.last_plus_one)){ _throw( Null_Exception);}* _temp6414;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6415= yyvs; struct _xtunion_struct**
_temp6417= _temp6415.curr + yyvsp_offset; if( _temp6415.base == 0? 1:( _temp6417
< _temp6415.base? 1: _temp6417 >= _temp6415.last_plus_one)){ _throw(
Null_Exception);}* _temp6417;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6418= yyls; struct Cyc_Yyltype* _temp6420= _temp6418.curr +(
yylsp_offset - 2); if( _temp6418.base == 0? 1:( _temp6420 < _temp6418.base? 1:
_temp6420 >= _temp6418.last_plus_one)){ _throw( Null_Exception);}* _temp6420;}).first_line,({
struct _tagged_ptr3 _temp6421= yyls; struct Cyc_Yyltype* _temp6423= _temp6421.curr
+ yylsp_offset; if( _temp6421.base == 0? 1:( _temp6423 < _temp6421.base? 1:
_temp6423 >= _temp6421.last_plus_one)){ _throw( Null_Exception);}* _temp6423;}).last_line));
_temp6411;}); _temp6410;}); break; case 320: _LL6409: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6425=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6425[ 0]=({ struct Cyc_Exp_tok_struct
_temp6426; _temp6426.tag= Cyc_Exp_tok_tag; _temp6426.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6427= yyvs; struct _xtunion_struct** _temp6429=
_temp6427.curr +( yyvsp_offset - 2); if( _temp6427.base == 0? 1:( _temp6429 <
_temp6427.base? 1: _temp6429 >= _temp6427.last_plus_one)){ _throw(
Null_Exception);}* _temp6429;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6430= yyvs; struct _xtunion_struct** _temp6432= _temp6430.curr +
yyvsp_offset; if( _temp6430.base == 0? 1:( _temp6432 < _temp6430.base? 1:
_temp6432 >= _temp6430.last_plus_one)){ _throw( Null_Exception);}* _temp6432;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6433= yyls; struct Cyc_Yyltype*
_temp6435= _temp6433.curr +( yylsp_offset - 2); if( _temp6433.base == 0? 1:(
_temp6435 < _temp6433.base? 1: _temp6435 >= _temp6433.last_plus_one)){ _throw(
Null_Exception);}* _temp6435;}).first_line,({ struct _tagged_ptr3 _temp6436=
yyls; struct Cyc_Yyltype* _temp6438= _temp6436.curr + yylsp_offset; if(
_temp6436.base == 0? 1:( _temp6438 < _temp6436.base? 1: _temp6438 >= _temp6436.last_plus_one)){
_throw( Null_Exception);}* _temp6438;}).last_line)); _temp6426;}); _temp6425;});
break; case 321: _LL6424: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6440=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6440[ 0]=({ struct Cyc_Exp_tok_struct _temp6441; _temp6441.tag= Cyc_Exp_tok_tag;
_temp6441.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6442= yyvs; struct _xtunion_struct** _temp6444= _temp6442.curr +(
yyvsp_offset - 2); if( _temp6442.base == 0? 1:( _temp6444 < _temp6442.base? 1:
_temp6444 >= _temp6442.last_plus_one)){ _throw( Null_Exception);}* _temp6444;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6445= yyvs; struct _xtunion_struct**
_temp6447= _temp6445.curr + yyvsp_offset; if( _temp6445.base == 0? 1:( _temp6447
< _temp6445.base? 1: _temp6447 >= _temp6445.last_plus_one)){ _throw(
Null_Exception);}* _temp6447;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6448= yyls; struct Cyc_Yyltype* _temp6450= _temp6448.curr +(
yylsp_offset - 2); if( _temp6448.base == 0? 1:( _temp6450 < _temp6448.base? 1:
_temp6450 >= _temp6448.last_plus_one)){ _throw( Null_Exception);}* _temp6450;}).first_line,({
struct _tagged_ptr3 _temp6451= yyls; struct Cyc_Yyltype* _temp6453= _temp6451.curr
+ yylsp_offset; if( _temp6451.base == 0? 1:( _temp6453 < _temp6451.base? 1:
_temp6453 >= _temp6451.last_plus_one)){ _throw( Null_Exception);}* _temp6453;}).last_line));
_temp6441;}); _temp6440;}); break; case 322: _LL6439: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6455=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6455[ 0]=({ struct Cyc_Exp_tok_struct
_temp6456; _temp6456.tag= Cyc_Exp_tok_tag; _temp6456.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6457= yyvs; struct _xtunion_struct** _temp6459=
_temp6457.curr +( yyvsp_offset - 2); if( _temp6457.base == 0? 1:( _temp6459 <
_temp6457.base? 1: _temp6459 >= _temp6457.last_plus_one)){ _throw(
Null_Exception);}* _temp6459;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6460= yyvs; struct _xtunion_struct** _temp6462= _temp6460.curr +
yyvsp_offset; if( _temp6460.base == 0? 1:( _temp6462 < _temp6460.base? 1:
_temp6462 >= _temp6460.last_plus_one)){ _throw( Null_Exception);}* _temp6462;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6463= yyls; struct Cyc_Yyltype*
_temp6465= _temp6463.curr +( yylsp_offset - 2); if( _temp6463.base == 0? 1:(
_temp6465 < _temp6463.base? 1: _temp6465 >= _temp6463.last_plus_one)){ _throw(
Null_Exception);}* _temp6465;}).first_line,({ struct _tagged_ptr3 _temp6466=
yyls; struct Cyc_Yyltype* _temp6468= _temp6466.curr + yylsp_offset; if(
_temp6466.base == 0? 1:( _temp6468 < _temp6466.base? 1: _temp6468 >= _temp6466.last_plus_one)){
_throw( Null_Exception);}* _temp6468;}).last_line)); _temp6456;}); _temp6455;});
break; case 323: _LL6454: yyval=({ struct _tagged_ptr2 _temp6470= yyvs; struct
_xtunion_struct** _temp6472= _temp6470.curr + yyvsp_offset; if( _temp6470.base
== 0? 1:( _temp6472 < _temp6470.base? 1: _temp6472 >= _temp6470.last_plus_one)){
_throw( Null_Exception);}* _temp6472;}); break; case 324: _LL6469: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6474=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6474[ 0]=({ struct Cyc_Exp_tok_struct
_temp6475; _temp6475.tag= Cyc_Exp_tok_tag; _temp6475.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6476=
yyvs; struct _xtunion_struct** _temp6478= _temp6476.curr +( yyvsp_offset - 2);
if( _temp6476.base == 0? 1:( _temp6478 < _temp6476.base? 1: _temp6478 >=
_temp6476.last_plus_one)){ _throw( Null_Exception);}* _temp6478;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6479= yyvs; struct _xtunion_struct** _temp6481=
_temp6479.curr + yyvsp_offset; if( _temp6479.base == 0? 1:( _temp6481 <
_temp6479.base? 1: _temp6481 >= _temp6479.last_plus_one)){ _throw(
Null_Exception);}* _temp6481;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6482= yyls; struct Cyc_Yyltype* _temp6484= _temp6482.curr +(
yylsp_offset - 2); if( _temp6482.base == 0? 1:( _temp6484 < _temp6482.base? 1:
_temp6484 >= _temp6482.last_plus_one)){ _throw( Null_Exception);}* _temp6484;}).first_line,({
struct _tagged_ptr3 _temp6485= yyls; struct Cyc_Yyltype* _temp6487= _temp6485.curr
+ yylsp_offset; if( _temp6485.base == 0? 1:( _temp6487 < _temp6485.base? 1:
_temp6487 >= _temp6485.last_plus_one)){ _throw( Null_Exception);}* _temp6487;}).last_line));
_temp6475;}); _temp6474;}); break; case 325: _LL6473: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6489=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6489[ 0]=({ struct Cyc_Exp_tok_struct
_temp6490; _temp6490.tag= Cyc_Exp_tok_tag; _temp6490.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6491=
yyvs; struct _xtunion_struct** _temp6493= _temp6491.curr +( yyvsp_offset - 2);
if( _temp6491.base == 0? 1:( _temp6493 < _temp6491.base? 1: _temp6493 >=
_temp6491.last_plus_one)){ _throw( Null_Exception);}* _temp6493;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6494= yyvs; struct _xtunion_struct** _temp6496=
_temp6494.curr + yyvsp_offset; if( _temp6494.base == 0? 1:( _temp6496 <
_temp6494.base? 1: _temp6496 >= _temp6494.last_plus_one)){ _throw(
Null_Exception);}* _temp6496;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6497= yyls; struct Cyc_Yyltype* _temp6499= _temp6497.curr +(
yylsp_offset - 2); if( _temp6497.base == 0? 1:( _temp6499 < _temp6497.base? 1:
_temp6499 >= _temp6497.last_plus_one)){ _throw( Null_Exception);}* _temp6499;}).first_line,({
struct _tagged_ptr3 _temp6500= yyls; struct Cyc_Yyltype* _temp6502= _temp6500.curr
+ yylsp_offset; if( _temp6500.base == 0? 1:( _temp6502 < _temp6500.base? 1:
_temp6502 >= _temp6500.last_plus_one)){ _throw( Null_Exception);}* _temp6502;}).last_line));
_temp6490;}); _temp6489;}); break; case 326: _LL6488: yyval=({ struct
_tagged_ptr2 _temp6504= yyvs; struct _xtunion_struct** _temp6506= _temp6504.curr
+ yyvsp_offset; if( _temp6504.base == 0? 1:( _temp6506 < _temp6504.base? 1:
_temp6506 >= _temp6504.last_plus_one)){ _throw( Null_Exception);}* _temp6506;});
break; case 327: _LL6503: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6508=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6508[ 0]=({ struct Cyc_Exp_tok_struct _temp6509; _temp6509.tag= Cyc_Exp_tok_tag;
_temp6509.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6510= yyvs; struct _xtunion_struct** _temp6512=
_temp6510.curr +( yyvsp_offset - 2); if( _temp6510.base == 0? 1:( _temp6512 <
_temp6510.base? 1: _temp6512 >= _temp6510.last_plus_one)){ _throw(
Null_Exception);}* _temp6512;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6513= yyvs; struct _xtunion_struct** _temp6515= _temp6513.curr +
yyvsp_offset; if( _temp6513.base == 0? 1:( _temp6515 < _temp6513.base? 1:
_temp6515 >= _temp6513.last_plus_one)){ _throw( Null_Exception);}* _temp6515;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6516= yyls; struct Cyc_Yyltype*
_temp6518= _temp6516.curr +( yylsp_offset - 2); if( _temp6516.base == 0? 1:(
_temp6518 < _temp6516.base? 1: _temp6518 >= _temp6516.last_plus_one)){ _throw(
Null_Exception);}* _temp6518;}).first_line,({ struct _tagged_ptr3 _temp6519=
yyls; struct Cyc_Yyltype* _temp6521= _temp6519.curr + yylsp_offset; if(
_temp6519.base == 0? 1:( _temp6521 < _temp6519.base? 1: _temp6521 >= _temp6519.last_plus_one)){
_throw( Null_Exception);}* _temp6521;}).last_line)); _temp6509;}); _temp6508;});
break; case 328: _LL6507: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6523=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6523[ 0]=({ struct Cyc_Exp_tok_struct _temp6524; _temp6524.tag= Cyc_Exp_tok_tag;
_temp6524.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6525= yyvs; struct _xtunion_struct** _temp6527=
_temp6525.curr +( yyvsp_offset - 2); if( _temp6525.base == 0? 1:( _temp6527 <
_temp6525.base? 1: _temp6527 >= _temp6525.last_plus_one)){ _throw(
Null_Exception);}* _temp6527;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6528= yyvs; struct _xtunion_struct** _temp6530= _temp6528.curr +
yyvsp_offset; if( _temp6528.base == 0? 1:( _temp6530 < _temp6528.base? 1:
_temp6530 >= _temp6528.last_plus_one)){ _throw( Null_Exception);}* _temp6530;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6531= yyls; struct Cyc_Yyltype*
_temp6533= _temp6531.curr +( yylsp_offset - 2); if( _temp6531.base == 0? 1:(
_temp6533 < _temp6531.base? 1: _temp6533 >= _temp6531.last_plus_one)){ _throw(
Null_Exception);}* _temp6533;}).first_line,({ struct _tagged_ptr3 _temp6534=
yyls; struct Cyc_Yyltype* _temp6536= _temp6534.curr + yylsp_offset; if(
_temp6534.base == 0? 1:( _temp6536 < _temp6534.base? 1: _temp6536 >= _temp6534.last_plus_one)){
_throw( Null_Exception);}* _temp6536;}).last_line)); _temp6524;}); _temp6523;});
break; case 329: _LL6522: yyval=({ struct _tagged_ptr2 _temp6538= yyvs; struct
_xtunion_struct** _temp6540= _temp6538.curr + yyvsp_offset; if( _temp6538.base
== 0? 1:( _temp6540 < _temp6538.base? 1: _temp6540 >= _temp6538.last_plus_one)){
_throw( Null_Exception);}* _temp6540;}); break; case 330: _LL6537: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6542=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6542[ 0]=({ struct Cyc_Exp_tok_struct
_temp6543; _temp6543.tag= Cyc_Exp_tok_tag; _temp6543.f1= Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6544= yyvs;
struct _xtunion_struct** _temp6546= _temp6544.curr +( yyvsp_offset - 2); if(
_temp6544.base == 0? 1:( _temp6546 < _temp6544.base? 1: _temp6546 >= _temp6544.last_plus_one)){
_throw( Null_Exception);}* _temp6546;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6547= yyvs; struct _xtunion_struct** _temp6549= _temp6547.curr
+ yyvsp_offset; if( _temp6547.base == 0? 1:( _temp6549 < _temp6547.base? 1:
_temp6549 >= _temp6547.last_plus_one)){ _throw( Null_Exception);}* _temp6549;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6550= yyls; struct Cyc_Yyltype*
_temp6552= _temp6550.curr +( yylsp_offset - 2); if( _temp6550.base == 0? 1:(
_temp6552 < _temp6550.base? 1: _temp6552 >= _temp6550.last_plus_one)){ _throw(
Null_Exception);}* _temp6552;}).first_line,({ struct _tagged_ptr3 _temp6553=
yyls; struct Cyc_Yyltype* _temp6555= _temp6553.curr + yylsp_offset; if(
_temp6553.base == 0? 1:( _temp6555 < _temp6553.base? 1: _temp6555 >= _temp6553.last_plus_one)){
_throw( Null_Exception);}* _temp6555;}).last_line)); _temp6543;}); _temp6542;});
break; case 331: _LL6541: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6557=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6557[ 0]=({ struct Cyc_Exp_tok_struct _temp6558; _temp6558.tag= Cyc_Exp_tok_tag;
_temp6558.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6559= yyvs; struct _xtunion_struct** _temp6561=
_temp6559.curr +( yyvsp_offset - 2); if( _temp6559.base == 0? 1:( _temp6561 <
_temp6559.base? 1: _temp6561 >= _temp6559.last_plus_one)){ _throw(
Null_Exception);}* _temp6561;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6562= yyvs; struct _xtunion_struct** _temp6564= _temp6562.curr +
yyvsp_offset; if( _temp6562.base == 0? 1:( _temp6564 < _temp6562.base? 1:
_temp6564 >= _temp6562.last_plus_one)){ _throw( Null_Exception);}* _temp6564;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6565= yyls; struct Cyc_Yyltype*
_temp6567= _temp6565.curr +( yylsp_offset - 2); if( _temp6565.base == 0? 1:(
_temp6567 < _temp6565.base? 1: _temp6567 >= _temp6565.last_plus_one)){ _throw(
Null_Exception);}* _temp6567;}).first_line,({ struct _tagged_ptr3 _temp6568=
yyls; struct Cyc_Yyltype* _temp6570= _temp6568.curr + yylsp_offset; if(
_temp6568.base == 0? 1:( _temp6570 < _temp6568.base? 1: _temp6570 >= _temp6568.last_plus_one)){
_throw( Null_Exception);}* _temp6570;}).last_line)); _temp6558;}); _temp6557;});
break; case 332: _LL6556: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6572=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6572[ 0]=({ struct Cyc_Exp_tok_struct _temp6573; _temp6573.tag= Cyc_Exp_tok_tag;
_temp6573.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6574= yyvs; struct _xtunion_struct** _temp6576=
_temp6574.curr +( yyvsp_offset - 2); if( _temp6574.base == 0? 1:( _temp6576 <
_temp6574.base? 1: _temp6576 >= _temp6574.last_plus_one)){ _throw(
Null_Exception);}* _temp6576;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6577= yyvs; struct _xtunion_struct** _temp6579= _temp6577.curr +
yyvsp_offset; if( _temp6577.base == 0? 1:( _temp6579 < _temp6577.base? 1:
_temp6579 >= _temp6577.last_plus_one)){ _throw( Null_Exception);}* _temp6579;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6580= yyls; struct Cyc_Yyltype*
_temp6582= _temp6580.curr +( yylsp_offset - 2); if( _temp6580.base == 0? 1:(
_temp6582 < _temp6580.base? 1: _temp6582 >= _temp6580.last_plus_one)){ _throw(
Null_Exception);}* _temp6582;}).first_line,({ struct _tagged_ptr3 _temp6583=
yyls; struct Cyc_Yyltype* _temp6585= _temp6583.curr + yylsp_offset; if(
_temp6583.base == 0? 1:( _temp6585 < _temp6583.base? 1: _temp6585 >= _temp6583.last_plus_one)){
_throw( Null_Exception);}* _temp6585;}).last_line)); _temp6573;}); _temp6572;});
break; case 333: _LL6571: yyval=({ struct _tagged_ptr2 _temp6587= yyvs; struct
_xtunion_struct** _temp6589= _temp6587.curr + yyvsp_offset; if( _temp6587.base
== 0? 1:( _temp6589 < _temp6587.base? 1: _temp6589 >= _temp6587.last_plus_one)){
_throw( Null_Exception);}* _temp6589;}); break; case 334: _LL6586: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6591=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6591[ 0]=({ struct Cyc_Exp_tok_struct
_temp6592; _temp6592.tag= Cyc_Exp_tok_tag; _temp6592.f1= Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6593= yyvs; struct
_xtunion_struct** _temp6595= _temp6593.curr +( yyvsp_offset - 2); if( _temp6593.base
== 0? 1:( _temp6595 < _temp6593.base? 1: _temp6595 >= _temp6593.last_plus_one)){
_throw( Null_Exception);}* _temp6595;}))).f3, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6596= yyvs; struct _xtunion_struct** _temp6598= _temp6596.curr
+ yyvsp_offset; if( _temp6596.base == 0? 1:( _temp6598 < _temp6596.base? 1:
_temp6598 >= _temp6596.last_plus_one)){ _throw( Null_Exception);}* _temp6598;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6599= yyls; struct Cyc_Yyltype*
_temp6601= _temp6599.curr +( yylsp_offset - 3); if( _temp6599.base == 0? 1:(
_temp6601 < _temp6599.base? 1: _temp6601 >= _temp6599.last_plus_one)){ _throw(
Null_Exception);}* _temp6601;}).first_line,({ struct _tagged_ptr3 _temp6602=
yyls; struct Cyc_Yyltype* _temp6604= _temp6602.curr + yylsp_offset; if(
_temp6602.base == 0? 1:( _temp6604 < _temp6602.base? 1: _temp6604 >= _temp6602.last_plus_one)){
_throw( Null_Exception);}* _temp6604;}).last_line)); _temp6592;}); _temp6591;});
break; case 335: _LL6590: yyval=({ struct _tagged_ptr2 _temp6606= yyvs; struct
_xtunion_struct** _temp6608= _temp6606.curr + yyvsp_offset; if( _temp6606.base
== 0? 1:( _temp6608 < _temp6606.base? 1: _temp6608 >= _temp6606.last_plus_one)){
_throw( Null_Exception);}* _temp6608;}); break; case 336: _LL6605: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6610=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6610[ 0]=({ struct Cyc_Exp_tok_struct
_temp6611; _temp6611.tag= Cyc_Exp_tok_tag; _temp6611.f1= Cyc_Absyn_pre_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6612= yyvs; struct _xtunion_struct**
_temp6614= _temp6612.curr + yyvsp_offset; if( _temp6612.base == 0? 1:( _temp6614
< _temp6612.base? 1: _temp6614 >= _temp6612.last_plus_one)){ _throw(
Null_Exception);}* _temp6614;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6615= yyls; struct Cyc_Yyltype* _temp6617= _temp6615.curr +(
yylsp_offset - 1); if( _temp6615.base == 0? 1:( _temp6617 < _temp6615.base? 1:
_temp6617 >= _temp6615.last_plus_one)){ _throw( Null_Exception);}* _temp6617;}).first_line,({
struct _tagged_ptr3 _temp6618= yyls; struct Cyc_Yyltype* _temp6620= _temp6618.curr
+ yylsp_offset; if( _temp6618.base == 0? 1:( _temp6620 < _temp6618.base? 1:
_temp6620 >= _temp6618.last_plus_one)){ _throw( Null_Exception);}* _temp6620;}).last_line));
_temp6611;}); _temp6610;}); break; case 337: _LL6609: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6622=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6622[ 0]=({ struct Cyc_Exp_tok_struct
_temp6623; _temp6623.tag= Cyc_Exp_tok_tag; _temp6623.f1= Cyc_Absyn_pre_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6624= yyvs; struct _xtunion_struct**
_temp6626= _temp6624.curr + yyvsp_offset; if( _temp6624.base == 0? 1:( _temp6626
< _temp6624.base? 1: _temp6626 >= _temp6624.last_plus_one)){ _throw(
Null_Exception);}* _temp6626;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6627= yyls; struct Cyc_Yyltype* _temp6629= _temp6627.curr +(
yylsp_offset - 1); if( _temp6627.base == 0? 1:( _temp6629 < _temp6627.base? 1:
_temp6629 >= _temp6627.last_plus_one)){ _throw( Null_Exception);}* _temp6629;}).first_line,({
struct _tagged_ptr3 _temp6630= yyls; struct Cyc_Yyltype* _temp6632= _temp6630.curr
+ yylsp_offset; if( _temp6630.base == 0? 1:( _temp6632 < _temp6630.base? 1:
_temp6632 >= _temp6630.last_plus_one)){ _throw( Null_Exception);}* _temp6632;}).last_line));
_temp6623;}); _temp6622;}); break; case 338: _LL6621: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6634=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6634[ 0]=({ struct Cyc_Exp_tok_struct
_temp6635; _temp6635.tag= Cyc_Exp_tok_tag; _temp6635.f1= Cyc_Absyn_address_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6636= yyvs; struct _xtunion_struct**
_temp6638= _temp6636.curr + yyvsp_offset; if( _temp6636.base == 0? 1:( _temp6638
< _temp6636.base? 1: _temp6638 >= _temp6636.last_plus_one)){ _throw(
Null_Exception);}* _temp6638;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6639= yyls; struct Cyc_Yyltype* _temp6641= _temp6639.curr +(
yylsp_offset - 1); if( _temp6639.base == 0? 1:( _temp6641 < _temp6639.base? 1:
_temp6641 >= _temp6639.last_plus_one)){ _throw( Null_Exception);}* _temp6641;}).first_line,({
struct _tagged_ptr3 _temp6642= yyls; struct Cyc_Yyltype* _temp6644= _temp6642.curr
+ yylsp_offset; if( _temp6642.base == 0? 1:( _temp6644 < _temp6642.base? 1:
_temp6644 >= _temp6642.last_plus_one)){ _throw( Null_Exception);}* _temp6644;}).last_line));
_temp6635;}); _temp6634;}); break; case 339: _LL6633: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6646=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6646[ 0]=({ struct Cyc_Exp_tok_struct
_temp6647; _temp6647.tag= Cyc_Exp_tok_tag; _temp6647.f1= Cyc_Absyn_deref_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6648= yyvs; struct _xtunion_struct**
_temp6650= _temp6648.curr + yyvsp_offset; if( _temp6648.base == 0? 1:( _temp6650
< _temp6648.base? 1: _temp6650 >= _temp6648.last_plus_one)){ _throw(
Null_Exception);}* _temp6650;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6651= yyls; struct Cyc_Yyltype* _temp6653= _temp6651.curr +(
yylsp_offset - 1); if( _temp6651.base == 0? 1:( _temp6653 < _temp6651.base? 1:
_temp6653 >= _temp6651.last_plus_one)){ _throw( Null_Exception);}* _temp6653;}).first_line,({
struct _tagged_ptr3 _temp6654= yyls; struct Cyc_Yyltype* _temp6656= _temp6654.curr
+ yylsp_offset; if( _temp6654.base == 0? 1:( _temp6656 < _temp6654.base? 1:
_temp6656 >= _temp6654.last_plus_one)){ _throw( Null_Exception);}* _temp6656;}).last_line));
_temp6647;}); _temp6646;}); break; case 340: _LL6645: yyval=({ struct
_tagged_ptr2 _temp6658= yyvs; struct _xtunion_struct** _temp6660= _temp6658.curr
+ yyvsp_offset; if( _temp6658.base == 0? 1:( _temp6660 < _temp6658.base? 1:
_temp6660 >= _temp6658.last_plus_one)){ _throw( Null_Exception);}* _temp6660;});
break; case 341: _LL6657: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6662=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6662[ 0]=({ struct Cyc_Exp_tok_struct _temp6663; _temp6663.tag= Cyc_Exp_tok_tag;
_temp6663.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(({ struct _tagged_ptr2
_temp6664= yyvs; struct _xtunion_struct** _temp6666= _temp6664.curr +(
yyvsp_offset - 1); if( _temp6664.base == 0? 1:( _temp6666 < _temp6664.base? 1:
_temp6666 >= _temp6664.last_plus_one)){ _throw( Null_Exception);}* _temp6666;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6667= yyvs; struct _xtunion_struct**
_temp6669= _temp6667.curr + yyvsp_offset; if( _temp6667.base == 0? 1:( _temp6669
< _temp6667.base? 1: _temp6669 >= _temp6667.last_plus_one)){ _throw(
Null_Exception);}* _temp6669;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6670= yyls; struct Cyc_Yyltype* _temp6672= _temp6670.curr +(
yylsp_offset - 1); if( _temp6670.base == 0? 1:( _temp6672 < _temp6670.base? 1:
_temp6672 >= _temp6670.last_plus_one)){ _throw( Null_Exception);}* _temp6672;}).first_line,({
struct _tagged_ptr3 _temp6673= yyls; struct Cyc_Yyltype* _temp6675= _temp6673.curr
+ yylsp_offset; if( _temp6673.base == 0? 1:( _temp6675 < _temp6673.base? 1:
_temp6675 >= _temp6673.last_plus_one)){ _throw( Null_Exception);}* _temp6675;}).last_line));
_temp6663;}); _temp6662;}); break; case 342: _LL6661: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6677=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6677[ 0]=({ struct Cyc_Exp_tok_struct
_temp6678; _temp6678.tag= Cyc_Exp_tok_tag; _temp6678.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6679= yyvs; struct
_xtunion_struct** _temp6681= _temp6679.curr +( yyvsp_offset - 1); if( _temp6679.base
== 0? 1:( _temp6681 < _temp6679.base? 1: _temp6681 >= _temp6679.last_plus_one)){
_throw( Null_Exception);}* _temp6681;}))).f3, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6682= yyls; struct Cyc_Yyltype* _temp6684= _temp6682.curr
+( yylsp_offset - 3); if( _temp6682.base == 0? 1:( _temp6684 < _temp6682.base? 1:
_temp6684 >= _temp6682.last_plus_one)){ _throw( Null_Exception);}* _temp6684;}).first_line,({
struct _tagged_ptr3 _temp6685= yyls; struct Cyc_Yyltype* _temp6687= _temp6685.curr
+ yylsp_offset; if( _temp6685.base == 0? 1:( _temp6687 < _temp6685.base? 1:
_temp6687 >= _temp6685.last_plus_one)){ _throw( Null_Exception);}* _temp6687;}).last_line));
_temp6678;}); _temp6677;}); break; case 343: _LL6676: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6689=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6689[ 0]=({ struct Cyc_Exp_tok_struct
_temp6690; _temp6690.tag= Cyc_Exp_tok_tag; _temp6690.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6691= yyvs; struct _xtunion_struct**
_temp6693= _temp6691.curr + yyvsp_offset; if( _temp6691.base == 0? 1:( _temp6693
< _temp6691.base? 1: _temp6693 >= _temp6691.last_plus_one)){ _throw(
Null_Exception);}* _temp6693;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6694= yyls; struct Cyc_Yyltype* _temp6696= _temp6694.curr +(
yylsp_offset - 1); if( _temp6694.base == 0? 1:( _temp6696 < _temp6694.base? 1:
_temp6696 >= _temp6694.last_plus_one)){ _throw( Null_Exception);}* _temp6696;}).first_line,({
struct _tagged_ptr3 _temp6697= yyls; struct Cyc_Yyltype* _temp6699= _temp6697.curr
+ yylsp_offset; if( _temp6697.base == 0? 1:( _temp6699 < _temp6697.base? 1:
_temp6699 >= _temp6697.last_plus_one)){ _throw( Null_Exception);}* _temp6699;}).last_line));
_temp6690;}); _temp6689;}); break; case 344: _LL6688: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6701=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6701[ 0]=({ struct Cyc_Exp_tok_struct
_temp6702; _temp6702.tag= Cyc_Exp_tok_tag; _temp6702.f1= Cyc_Absyn_primop_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp6703= yyvs; struct
_xtunion_struct** _temp6705= _temp6703.curr +( yyvsp_offset - 3); if( _temp6703.base
== 0? 1:( _temp6705 < _temp6703.base? 1: _temp6705 >= _temp6703.last_plus_one)){
_throw( Null_Exception);}* _temp6705;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6706= yyvs; struct _xtunion_struct** _temp6708= _temp6706.curr
+( yyvsp_offset - 1); if( _temp6706.base == 0? 1:( _temp6708 < _temp6706.base? 1:
_temp6708 >= _temp6706.last_plus_one)){ _throw( Null_Exception);}* _temp6708;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6709= yyls; struct Cyc_Yyltype*
_temp6711= _temp6709.curr +( yylsp_offset - 3); if( _temp6709.base == 0? 1:(
_temp6711 < _temp6709.base? 1: _temp6711 >= _temp6709.last_plus_one)){ _throw(
Null_Exception);}* _temp6711;}).first_line,({ struct _tagged_ptr3 _temp6712=
yyls; struct Cyc_Yyltype* _temp6714= _temp6712.curr + yylsp_offset; if(
_temp6712.base == 0? 1:( _temp6714 < _temp6712.base? 1: _temp6714 >= _temp6712.last_plus_one)){
_throw( Null_Exception);}* _temp6714;}).last_line)); _temp6702;}); _temp6701;});
break; case 345: _LL6700: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6716=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6716[ 0]=({ struct Cyc_Exp_tok_struct _temp6717; _temp6717.tag= Cyc_Exp_tok_tag;
_temp6717.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6718=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6718[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6719; _temp6719.tag= Cyc_Absyn_Malloc_e_tag;
_temp6719.f1=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp6720= yyvs; struct _xtunion_struct** _temp6722=
_temp6720.curr +( yyvsp_offset - 2); if( _temp6720.base == 0? 1:( _temp6722 <
_temp6720.base? 1: _temp6722 >= _temp6720.last_plus_one)){ _throw(
Null_Exception);}* _temp6722;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6723= yyls; struct Cyc_Yyltype* _temp6725= _temp6723.curr +(
yylsp_offset - 2); if( _temp6723.base == 0? 1:( _temp6725 < _temp6723.base? 1:
_temp6725 >= _temp6723.last_plus_one)){ _throw( Null_Exception);}* _temp6725;}).first_line,({
struct _tagged_ptr3 _temp6726= yyls; struct Cyc_Yyltype* _temp6728= _temp6726.curr
+( yylsp_offset - 2); if( _temp6726.base == 0? 1:( _temp6728 < _temp6726.base? 1:
_temp6728 >= _temp6726.last_plus_one)){ _throw( Null_Exception);}* _temp6728;}).last_line));
_temp6719;}); _temp6718;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6729= yyls; struct Cyc_Yyltype* _temp6731= _temp6729.curr +( yylsp_offset -
6); if( _temp6729.base == 0? 1:( _temp6731 < _temp6729.base? 1: _temp6731 >=
_temp6729.last_plus_one)){ _throw( Null_Exception);}* _temp6731;}).first_line,({
struct _tagged_ptr3 _temp6732= yyls; struct Cyc_Yyltype* _temp6734= _temp6732.curr
+ yylsp_offset; if( _temp6732.base == 0? 1:( _temp6734 < _temp6732.base? 1:
_temp6734 >= _temp6732.last_plus_one)){ _throw( Null_Exception);}* _temp6734;}).last_line));
_temp6717;}); _temp6716;}); break; case 346: _LL6715: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6736=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6736[ 0]=({ struct Cyc_Primop_tok_struct
_temp6737; _temp6737.tag= Cyc_Primop_tok_tag; _temp6737.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp6737;}); _temp6736;}); break; case 347: _LL6735: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6739=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6739[ 0]=({ struct Cyc_Primop_tok_struct
_temp6740; _temp6740.tag= Cyc_Primop_tok_tag; _temp6740.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp6740;}); _temp6739;}); break; case 348: _LL6738: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6742=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6742[ 0]=({ struct Cyc_Primop_tok_struct
_temp6743; _temp6743.tag= Cyc_Primop_tok_tag; _temp6743.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp6743;}); _temp6742;}); break; case 349: _LL6741: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6745=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6745[ 0]=({ struct Cyc_Primop_tok_struct
_temp6746; _temp6746.tag= Cyc_Primop_tok_tag; _temp6746.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp6746;}); _temp6745;}); break; case 350: _LL6744: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6748=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6748[ 0]=({ struct Cyc_Primop_tok_struct
_temp6749; _temp6749.tag= Cyc_Primop_tok_tag; _temp6749.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp6749;}); _temp6748;}); break; case 351: _LL6747: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6751=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6751[ 0]=({ struct Cyc_Primop_tok_struct
_temp6752; _temp6752.tag= Cyc_Primop_tok_tag; _temp6752.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp6752;}); _temp6751;}); break; case 352: _LL6750: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6754=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6754[ 0]=({ struct Cyc_Primop_tok_struct
_temp6755; _temp6755.tag= Cyc_Primop_tok_tag; _temp6755.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp6755;}); _temp6754;}); break; case 353: _LL6753: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6757=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6757[ 0]=({ struct Cyc_Primop_tok_struct
_temp6758; _temp6758.tag= Cyc_Primop_tok_tag; _temp6758.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp6758;}); _temp6757;}); break; case 354: _LL6756: yyval=( struct
_xtunion_struct*)({ struct Cyc_Primop_tok_struct* _temp6760=( struct Cyc_Primop_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primop_tok_struct)); _temp6760[ 0]=({ struct Cyc_Primop_tok_struct
_temp6761; _temp6761.tag= Cyc_Primop_tok_tag; _temp6761.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp6761;}); _temp6760;}); break; case 355: _LL6759: yyval=({ struct
_tagged_ptr2 _temp6763= yyvs; struct _xtunion_struct** _temp6765= _temp6763.curr
+ yyvsp_offset; if( _temp6763.base == 0? 1:( _temp6765 < _temp6763.base? 1:
_temp6765 >= _temp6763.last_plus_one)){ _throw( Null_Exception);}* _temp6765;});
break; case 356: _LL6762: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6767=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6767[ 0]=({ struct Cyc_Exp_tok_struct _temp6768; _temp6768.tag= Cyc_Exp_tok_tag;
_temp6768.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6769= yyvs; struct _xtunion_struct** _temp6771= _temp6769.curr +(
yyvsp_offset - 3); if( _temp6769.base == 0? 1:( _temp6771 < _temp6769.base? 1:
_temp6771 >= _temp6769.last_plus_one)){ _throw( Null_Exception);}* _temp6771;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6772= yyvs; struct _xtunion_struct**
_temp6774= _temp6772.curr +( yyvsp_offset - 1); if( _temp6772.base == 0? 1:(
_temp6774 < _temp6772.base? 1: _temp6774 >= _temp6772.last_plus_one)){ _throw(
Null_Exception);}* _temp6774;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6775= yyls; struct Cyc_Yyltype* _temp6777= _temp6775.curr +(
yylsp_offset - 3); if( _temp6775.base == 0? 1:( _temp6777 < _temp6775.base? 1:
_temp6777 >= _temp6775.last_plus_one)){ _throw( Null_Exception);}* _temp6777;}).first_line,({
struct _tagged_ptr3 _temp6778= yyls; struct Cyc_Yyltype* _temp6780= _temp6778.curr
+ yylsp_offset; if( _temp6778.base == 0? 1:( _temp6780 < _temp6778.base? 1:
_temp6780 >= _temp6778.last_plus_one)){ _throw( Null_Exception);}* _temp6780;}).last_line));
_temp6768;}); _temp6767;}); break; case 357: _LL6766: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6782=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6782[ 0]=({ struct Cyc_Exp_tok_struct
_temp6783; _temp6783.tag= Cyc_Exp_tok_tag; _temp6783.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6784= yyvs; struct _xtunion_struct**
_temp6786= _temp6784.curr +( yyvsp_offset - 2); if( _temp6784.base == 0? 1:(
_temp6786 < _temp6784.base? 1: _temp6786 >= _temp6784.last_plus_one)){ _throw(
Null_Exception);}* _temp6786;})), 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6787= yyls; struct Cyc_Yyltype* _temp6789= _temp6787.curr +(
yylsp_offset - 2); if( _temp6787.base == 0? 1:( _temp6789 < _temp6787.base? 1:
_temp6789 >= _temp6787.last_plus_one)){ _throw( Null_Exception);}* _temp6789;}).first_line,({
struct _tagged_ptr3 _temp6790= yyls; struct Cyc_Yyltype* _temp6792= _temp6790.curr
+ yylsp_offset; if( _temp6790.base == 0? 1:( _temp6792 < _temp6790.base? 1:
_temp6792 >= _temp6790.last_plus_one)){ _throw( Null_Exception);}* _temp6792;}).last_line));
_temp6783;}); _temp6782;}); break; case 358: _LL6781: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6794=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6794[ 0]=({ struct Cyc_Exp_tok_struct
_temp6795; _temp6795.tag= Cyc_Exp_tok_tag; _temp6795.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6796= yyvs; struct _xtunion_struct**
_temp6798= _temp6796.curr +( yyvsp_offset - 3); if( _temp6796.base == 0? 1:(
_temp6798 < _temp6796.base? 1: _temp6798 >= _temp6796.last_plus_one)){ _throw(
Null_Exception);}* _temp6798;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp6799= yyvs; struct _xtunion_struct** _temp6801= _temp6799.curr +(
yyvsp_offset - 1); if( _temp6799.base == 0? 1:( _temp6801 < _temp6799.base? 1:
_temp6801 >= _temp6799.last_plus_one)){ _throw( Null_Exception);}* _temp6801;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6802= yyls; struct Cyc_Yyltype*
_temp6804= _temp6802.curr +( yylsp_offset - 3); if( _temp6802.base == 0? 1:(
_temp6804 < _temp6802.base? 1: _temp6804 >= _temp6802.last_plus_one)){ _throw(
Null_Exception);}* _temp6804;}).first_line,({ struct _tagged_ptr3 _temp6805=
yyls; struct Cyc_Yyltype* _temp6807= _temp6805.curr + yylsp_offset; if(
_temp6805.base == 0? 1:( _temp6807 < _temp6805.base? 1: _temp6807 >= _temp6805.last_plus_one)){
_throw( Null_Exception);}* _temp6807;}).last_line)); _temp6795;}); _temp6794;});
break; case 359: _LL6793: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6809=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6809[ 0]=({ struct Cyc_Exp_tok_struct _temp6810; _temp6810.tag= Cyc_Exp_tok_tag;
_temp6810.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6811= yyvs; struct _xtunion_struct** _temp6813= _temp6811.curr
+( yyvsp_offset - 2); if( _temp6811.base == 0? 1:( _temp6813 < _temp6811.base? 1:
_temp6813 >= _temp6811.last_plus_one)){ _throw( Null_Exception);}* _temp6813;})),({
struct _tagged_string* _temp6814=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp6814[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp6815= yyvs; struct _xtunion_struct** _temp6817= _temp6815.curr
+ yyvsp_offset; if( _temp6815.base == 0? 1:( _temp6817 < _temp6815.base? 1:
_temp6817 >= _temp6815.last_plus_one)){ _throw( Null_Exception);}* _temp6817;}));
_temp6814;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6818= yyls;
struct Cyc_Yyltype* _temp6820= _temp6818.curr +( yylsp_offset - 2); if(
_temp6818.base == 0? 1:( _temp6820 < _temp6818.base? 1: _temp6820 >= _temp6818.last_plus_one)){
_throw( Null_Exception);}* _temp6820;}).first_line,({ struct _tagged_ptr3
_temp6821= yyls; struct Cyc_Yyltype* _temp6823= _temp6821.curr + yylsp_offset;
if( _temp6821.base == 0? 1:( _temp6823 < _temp6821.base? 1: _temp6823 >=
_temp6821.last_plus_one)){ _throw( Null_Exception);}* _temp6823;}).last_line));
_temp6810;}); _temp6809;}); break; case 360: _LL6808: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6825= yyvs; struct _xtunion_struct** _temp6827=
_temp6825.curr + yyvsp_offset; if( _temp6825.base == 0? 1:( _temp6827 <
_temp6825.base? 1: _temp6827 >= _temp6825.last_plus_one)){ _throw(
Null_Exception);}* _temp6827;}));{ void* _temp6828=(* q).f1; struct Cyc_List_List*
_temp6838; struct Cyc_List_List* _temp6840; _LL6830: if(( int) _temp6828 == Cyc_Absyn_Loc_n){
goto _LL6831;} else{ goto _LL6832;} _LL6832: if(( unsigned int) _temp6828 > 1u?((
struct _tunion_struct*) _temp6828)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6839:
_temp6838=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6828)->f1;
if( _temp6838 == 0){ goto _LL6833;} else{ goto _LL6834;}} else{ goto _LL6834;}
_LL6834: if(( unsigned int) _temp6828 > 1u?(( struct _tunion_struct*) _temp6828)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6841: _temp6840=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6828)->f1; if( _temp6840 == 0){ goto _LL6835;}
else{ goto _LL6836;}} else{ goto _LL6836;} _LL6836: goto _LL6837; _LL6831: goto
_LL6829; _LL6833: goto _LL6829; _LL6835: goto _LL6829; _LL6837: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6842=( char*)"struct field name is qualified";
struct _tagged_string _temp6843; _temp6843.curr= _temp6842; _temp6843.base=
_temp6842; _temp6843.last_plus_one= _temp6842 + 31; _temp6843;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6844= yyls; struct Cyc_Yyltype* _temp6846= _temp6844.curr
+ yylsp_offset; if( _temp6844.base == 0? 1:( _temp6846 < _temp6844.base? 1:
_temp6846 >= _temp6844.last_plus_one)){ _throw( Null_Exception);}* _temp6846;}).first_line,({
struct _tagged_ptr3 _temp6847= yyls; struct Cyc_Yyltype* _temp6849= _temp6847.curr
+ yylsp_offset; if( _temp6847.base == 0? 1:( _temp6849 < _temp6847.base? 1:
_temp6849 >= _temp6847.last_plus_one)){ _throw( Null_Exception);}* _temp6849;}).last_line));
goto _LL6829; _LL6829:;} yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6850=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6850[ 0]=({ struct Cyc_Exp_tok_struct _temp6851; _temp6851.tag= Cyc_Exp_tok_tag;
_temp6851.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6852= yyvs; struct _xtunion_struct** _temp6854= _temp6852.curr
+( yyvsp_offset - 2); if( _temp6852.base == 0? 1:( _temp6854 < _temp6852.base? 1:
_temp6854 >= _temp6852.last_plus_one)){ _throw( Null_Exception);}* _temp6854;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6855= yyls;
struct Cyc_Yyltype* _temp6857= _temp6855.curr +( yylsp_offset - 2); if(
_temp6855.base == 0? 1:( _temp6857 < _temp6855.base? 1: _temp6857 >= _temp6855.last_plus_one)){
_throw( Null_Exception);}* _temp6857;}).first_line,({ struct _tagged_ptr3
_temp6858= yyls; struct Cyc_Yyltype* _temp6860= _temp6858.curr + yylsp_offset;
if( _temp6858.base == 0? 1:( _temp6860 < _temp6858.base? 1: _temp6860 >=
_temp6858.last_plus_one)){ _throw( Null_Exception);}* _temp6860;}).last_line));
_temp6851;}); _temp6850;}); break;} case 361: _LL6824: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6862=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6862[ 0]=({ struct Cyc_Exp_tok_struct
_temp6863; _temp6863.tag= Cyc_Exp_tok_tag; _temp6863.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6864= yyvs; struct _xtunion_struct**
_temp6866= _temp6864.curr +( yyvsp_offset - 2); if( _temp6864.base == 0? 1:(
_temp6866 < _temp6864.base? 1: _temp6866 >= _temp6864.last_plus_one)){ _throw(
Null_Exception);}* _temp6866;})),({ struct _tagged_string* _temp6867=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp6867[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6868= yyvs; struct
_xtunion_struct** _temp6870= _temp6868.curr + yyvsp_offset; if( _temp6868.base
== 0? 1:( _temp6870 < _temp6868.base? 1: _temp6870 >= _temp6868.last_plus_one)){
_throw( Null_Exception);}* _temp6870;})); _temp6867;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6871= yyls; struct Cyc_Yyltype* _temp6873= _temp6871.curr
+( yylsp_offset - 2); if( _temp6871.base == 0? 1:( _temp6873 < _temp6871.base? 1:
_temp6873 >= _temp6871.last_plus_one)){ _throw( Null_Exception);}* _temp6873;}).first_line,({
struct _tagged_ptr3 _temp6874= yyls; struct Cyc_Yyltype* _temp6876= _temp6874.curr
+ yylsp_offset; if( _temp6874.base == 0? 1:( _temp6876 < _temp6874.base? 1:
_temp6876 >= _temp6874.last_plus_one)){ _throw( Null_Exception);}* _temp6876;}).last_line));
_temp6863;}); _temp6862;}); break; case 362: _LL6861: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6878= yyvs; struct _xtunion_struct** _temp6880=
_temp6878.curr + yyvsp_offset; if( _temp6878.base == 0? 1:( _temp6880 <
_temp6878.base? 1: _temp6880 >= _temp6878.last_plus_one)){ _throw(
Null_Exception);}* _temp6880;}));{ void* _temp6881=(* q).f1; struct Cyc_List_List*
_temp6891; struct Cyc_List_List* _temp6893; _LL6883: if(( int) _temp6881 == Cyc_Absyn_Loc_n){
goto _LL6884;} else{ goto _LL6885;} _LL6885: if(( unsigned int) _temp6881 > 1u?((
struct _tunion_struct*) _temp6881)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6892:
_temp6891=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6881)->f1;
if( _temp6891 == 0){ goto _LL6886;} else{ goto _LL6887;}} else{ goto _LL6887;}
_LL6887: if(( unsigned int) _temp6881 > 1u?(( struct _tunion_struct*) _temp6881)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6894: _temp6893=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6881)->f1; if( _temp6893 == 0){ goto _LL6888;}
else{ goto _LL6889;}} else{ goto _LL6889;} _LL6889: goto _LL6890; _LL6884: goto
_LL6882; _LL6886: goto _LL6882; _LL6888: goto _LL6882; _LL6890: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6895=( char*)"struct field is qualified with module name";
struct _tagged_string _temp6896; _temp6896.curr= _temp6895; _temp6896.base=
_temp6895; _temp6896.last_plus_one= _temp6895 + 43; _temp6896;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6897= yyls; struct Cyc_Yyltype* _temp6899= _temp6897.curr
+ yylsp_offset; if( _temp6897.base == 0? 1:( _temp6899 < _temp6897.base? 1:
_temp6899 >= _temp6897.last_plus_one)){ _throw( Null_Exception);}* _temp6899;}).first_line,({
struct _tagged_ptr3 _temp6900= yyls; struct Cyc_Yyltype* _temp6902= _temp6900.curr
+ yylsp_offset; if( _temp6900.base == 0? 1:( _temp6902 < _temp6900.base? 1:
_temp6902 >= _temp6900.last_plus_one)){ _throw( Null_Exception);}* _temp6902;}).last_line));
goto _LL6882; _LL6882:;} yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp6903=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6903[ 0]=({ struct Cyc_Exp_tok_struct _temp6904; _temp6904.tag= Cyc_Exp_tok_tag;
_temp6904.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6905= yyvs; struct _xtunion_struct** _temp6907= _temp6905.curr
+( yyvsp_offset - 2); if( _temp6905.base == 0? 1:( _temp6907 < _temp6905.base? 1:
_temp6907 >= _temp6905.last_plus_one)){ _throw( Null_Exception);}* _temp6907;})),(*
q).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6908= yyls;
struct Cyc_Yyltype* _temp6910= _temp6908.curr +( yylsp_offset - 2); if(
_temp6908.base == 0? 1:( _temp6910 < _temp6908.base? 1: _temp6910 >= _temp6908.last_plus_one)){
_throw( Null_Exception);}* _temp6910;}).first_line,({ struct _tagged_ptr3
_temp6911= yyls; struct Cyc_Yyltype* _temp6913= _temp6911.curr + yylsp_offset;
if( _temp6911.base == 0? 1:( _temp6913 < _temp6911.base? 1: _temp6913 >=
_temp6911.last_plus_one)){ _throw( Null_Exception);}* _temp6913;}).last_line));
_temp6904;}); _temp6903;}); break;} case 363: _LL6877: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6915=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6915[ 0]=({ struct Cyc_Exp_tok_struct
_temp6916; _temp6916.tag= Cyc_Exp_tok_tag; _temp6916.f1= Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6917= yyvs; struct _xtunion_struct**
_temp6919= _temp6917.curr +( yyvsp_offset - 1); if( _temp6917.base == 0? 1:(
_temp6919 < _temp6917.base? 1: _temp6919 >= _temp6917.last_plus_one)){ _throw(
Null_Exception);}* _temp6919;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6920= yyls; struct Cyc_Yyltype* _temp6922= _temp6920.curr +(
yylsp_offset - 1); if( _temp6920.base == 0? 1:( _temp6922 < _temp6920.base? 1:
_temp6922 >= _temp6920.last_plus_one)){ _throw( Null_Exception);}* _temp6922;}).first_line,({
struct _tagged_ptr3 _temp6923= yyls; struct Cyc_Yyltype* _temp6925= _temp6923.curr
+ yylsp_offset; if( _temp6923.base == 0? 1:( _temp6925 < _temp6923.base? 1:
_temp6925 >= _temp6923.last_plus_one)){ _throw( Null_Exception);}* _temp6925;}).last_line));
_temp6916;}); _temp6915;}); break; case 364: _LL6914: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6927=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6927[ 0]=({ struct Cyc_Exp_tok_struct
_temp6928; _temp6928.tag= Cyc_Exp_tok_tag; _temp6928.f1= Cyc_Absyn_post_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6929= yyvs; struct _xtunion_struct**
_temp6931= _temp6929.curr +( yyvsp_offset - 1); if( _temp6929.base == 0? 1:(
_temp6931 < _temp6929.base? 1: _temp6931 >= _temp6929.last_plus_one)){ _throw(
Null_Exception);}* _temp6931;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6932= yyls; struct Cyc_Yyltype* _temp6934= _temp6932.curr +(
yylsp_offset - 1); if( _temp6932.base == 0? 1:( _temp6934 < _temp6932.base? 1:
_temp6934 >= _temp6932.last_plus_one)){ _throw( Null_Exception);}* _temp6934;}).first_line,({
struct _tagged_ptr3 _temp6935= yyls; struct Cyc_Yyltype* _temp6937= _temp6935.curr
+ yylsp_offset; if( _temp6935.base == 0? 1:( _temp6937 < _temp6935.base? 1:
_temp6937 >= _temp6935.last_plus_one)){ _throw( Null_Exception);}* _temp6937;}).last_line));
_temp6928;}); _temp6927;}); break; case 365: _LL6926: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6939=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6939[ 0]=({ struct Cyc_Exp_tok_struct
_temp6940; _temp6940.tag= Cyc_Exp_tok_tag; _temp6940.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_CompoundLit_e_struct* _temp6941=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp6941[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp6942; _temp6942.tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6942.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6943= yyvs;
struct _xtunion_struct** _temp6945= _temp6943.curr +( yyvsp_offset - 4); if(
_temp6943.base == 0? 1:( _temp6945 < _temp6943.base? 1: _temp6945 >= _temp6943.last_plus_one)){
_throw( Null_Exception);}* _temp6945;})); _temp6942.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6946= yyvs; struct _xtunion_struct** _temp6948=
_temp6946.curr +( yyvsp_offset - 1); if( _temp6946.base == 0? 1:( _temp6948 <
_temp6946.base? 1: _temp6948 >= _temp6946.last_plus_one)){ _throw(
Null_Exception);}* _temp6948;}))); _temp6942;}); _temp6941;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6949= yyls; struct Cyc_Yyltype* _temp6951= _temp6949.curr
+( yylsp_offset - 5); if( _temp6949.base == 0? 1:( _temp6951 < _temp6949.base? 1:
_temp6951 >= _temp6949.last_plus_one)){ _throw( Null_Exception);}* _temp6951;}).first_line,({
struct _tagged_ptr3 _temp6952= yyls; struct Cyc_Yyltype* _temp6954= _temp6952.curr
+ yylsp_offset; if( _temp6952.base == 0? 1:( _temp6954 < _temp6952.base? 1:
_temp6954 >= _temp6952.last_plus_one)){ _throw( Null_Exception);}* _temp6954;}).last_line));
_temp6940;}); _temp6939;}); break; case 366: _LL6938: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6956=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6956[ 0]=({ struct Cyc_Exp_tok_struct
_temp6957; _temp6957.tag= Cyc_Exp_tok_tag; _temp6957.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_CompoundLit_e_struct* _temp6958=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp6958[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp6959; _temp6959.tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6959.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6960= yyvs;
struct _xtunion_struct** _temp6962= _temp6960.curr +( yyvsp_offset - 5); if(
_temp6960.base == 0? 1:( _temp6962 < _temp6960.base? 1: _temp6962 >= _temp6960.last_plus_one)){
_throw( Null_Exception);}* _temp6962;})); _temp6959.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6963= yyvs; struct _xtunion_struct** _temp6965=
_temp6963.curr +( yyvsp_offset - 2); if( _temp6963.base == 0? 1:( _temp6965 <
_temp6963.base? 1: _temp6965 >= _temp6963.last_plus_one)){ _throw(
Null_Exception);}* _temp6965;}))); _temp6959;}); _temp6958;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6966= yyls; struct Cyc_Yyltype* _temp6968= _temp6966.curr
+( yylsp_offset - 6); if( _temp6966.base == 0? 1:( _temp6968 < _temp6966.base? 1:
_temp6968 >= _temp6966.last_plus_one)){ _throw( Null_Exception);}* _temp6968;}).first_line,({
struct _tagged_ptr3 _temp6969= yyls; struct Cyc_Yyltype* _temp6971= _temp6969.curr
+ yylsp_offset; if( _temp6969.base == 0? 1:( _temp6971 < _temp6969.base? 1:
_temp6971 >= _temp6969.last_plus_one)){ _throw( Null_Exception);}* _temp6971;}).last_line));
_temp6957;}); _temp6956;}); break; case 367: _LL6955: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6973=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6973[ 0]=({ struct Cyc_Exp_tok_struct
_temp6974; _temp6974.tag= Cyc_Exp_tok_tag; _temp6974.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Fill_e_struct* _temp6975=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp6975[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp6976; _temp6976.tag= Cyc_Absyn_Fill_e_tag; _temp6976.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6977= yyvs; struct _xtunion_struct** _temp6979=
_temp6977.curr +( yyvsp_offset - 1); if( _temp6977.base == 0? 1:( _temp6979 <
_temp6977.base? 1: _temp6979 >= _temp6977.last_plus_one)){ _throw(
Null_Exception);}* _temp6979;})); _temp6976;}); _temp6975;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6980= yyls; struct Cyc_Yyltype* _temp6982= _temp6980.curr
+( yylsp_offset - 3); if( _temp6980.base == 0? 1:( _temp6982 < _temp6980.base? 1:
_temp6982 >= _temp6980.last_plus_one)){ _throw( Null_Exception);}* _temp6982;}).first_line,({
struct _tagged_ptr3 _temp6983= yyls; struct Cyc_Yyltype* _temp6985= _temp6983.curr
+ yylsp_offset; if( _temp6983.base == 0? 1:( _temp6985 < _temp6983.base? 1:
_temp6985 >= _temp6983.last_plus_one)){ _throw( Null_Exception);}* _temp6985;}).last_line));
_temp6974;}); _temp6973;}); break; case 368: _LL6972: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp6987=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6987[ 0]=({ struct Cyc_Exp_tok_struct
_temp6988; _temp6988.tag= Cyc_Exp_tok_tag; _temp6988.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Codegen_e_struct* _temp6989=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp6989[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp6990; _temp6990.tag= Cyc_Absyn_Codegen_e_tag;
_temp6990.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp6991= yyvs;
struct _xtunion_struct** _temp6993= _temp6991.curr +( yyvsp_offset - 1); if(
_temp6991.base == 0? 1:( _temp6993 < _temp6991.base? 1: _temp6993 >= _temp6991.last_plus_one)){
_throw( Null_Exception);}* _temp6993;})); _temp6990;}); _temp6989;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6994= yyls; struct Cyc_Yyltype* _temp6996= _temp6994.curr
+( yylsp_offset - 3); if( _temp6994.base == 0? 1:( _temp6996 < _temp6994.base? 1:
_temp6996 >= _temp6994.last_plus_one)){ _throw( Null_Exception);}* _temp6996;}).first_line,({
struct _tagged_ptr3 _temp6997= yyls; struct Cyc_Yyltype* _temp6999= _temp6997.curr
+ yylsp_offset; if( _temp6997.base == 0? 1:( _temp6999 < _temp6997.base? 1:
_temp6999 >= _temp6997.last_plus_one)){ _throw( Null_Exception);}* _temp6999;}).last_line));
_temp6988;}); _temp6987;}); break; case 369: _LL6986: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7001=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7001[ 0]=({ struct Cyc_Exp_tok_struct
_temp7002; _temp7002.tag= Cyc_Exp_tok_tag; _temp7002.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7003=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp7003[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp7004; _temp7004.tag= Cyc_Absyn_UnknownId_e_tag;
_temp7004.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7005= yyvs;
struct _xtunion_struct** _temp7007= _temp7005.curr + yyvsp_offset; if( _temp7005.base
== 0? 1:( _temp7007 < _temp7005.base? 1: _temp7007 >= _temp7005.last_plus_one)){
_throw( Null_Exception);}* _temp7007;})); _temp7004;}); _temp7003;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7008= yyls; struct Cyc_Yyltype* _temp7010= _temp7008.curr
+ yylsp_offset; if( _temp7008.base == 0? 1:( _temp7010 < _temp7008.base? 1:
_temp7010 >= _temp7008.last_plus_one)){ _throw( Null_Exception);}* _temp7010;}).first_line,({
struct _tagged_ptr3 _temp7011= yyls; struct Cyc_Yyltype* _temp7013= _temp7011.curr
+ yylsp_offset; if( _temp7011.base == 0? 1:( _temp7013 < _temp7011.base? 1:
_temp7013 >= _temp7011.last_plus_one)){ _throw( Null_Exception);}* _temp7013;}).last_line));
_temp7002;}); _temp7001;}); break; case 370: _LL7000: yyval=({ struct
_tagged_ptr2 _temp7015= yyvs; struct _xtunion_struct** _temp7017= _temp7015.curr
+ yyvsp_offset; if( _temp7015.base == 0? 1:( _temp7017 < _temp7015.base? 1:
_temp7017 >= _temp7015.last_plus_one)){ _throw( Null_Exception);}* _temp7017;});
break; case 371: _LL7014: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7019=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7019[ 0]=({ struct Cyc_Exp_tok_struct _temp7020; _temp7020.tag= Cyc_Exp_tok_tag;
_temp7020.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7021= yyvs; struct _xtunion_struct** _temp7023= _temp7021.curr +
yyvsp_offset; if( _temp7021.base == 0? 1:( _temp7023 < _temp7021.base? 1:
_temp7023 >= _temp7021.last_plus_one)){ _throw( Null_Exception);}* _temp7023;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7024= yyls; struct Cyc_Yyltype*
_temp7026= _temp7024.curr + yylsp_offset; if( _temp7024.base == 0? 1:( _temp7026
< _temp7024.base? 1: _temp7026 >= _temp7024.last_plus_one)){ _throw(
Null_Exception);}* _temp7026;}).first_line,({ struct _tagged_ptr3 _temp7027=
yyls; struct Cyc_Yyltype* _temp7029= _temp7027.curr + yylsp_offset; if(
_temp7027.base == 0? 1:( _temp7029 < _temp7027.base? 1: _temp7029 >= _temp7027.last_plus_one)){
_throw( Null_Exception);}* _temp7029;}).last_line)); _temp7020;}); _temp7019;});
break; case 372: _LL7018: yyval=({ struct _tagged_ptr2 _temp7031= yyvs; struct
_xtunion_struct** _temp7033= _temp7031.curr +( yyvsp_offset - 1); if( _temp7031.base
== 0? 1:( _temp7033 < _temp7031.base? 1: _temp7033 >= _temp7031.last_plus_one)){
_throw( Null_Exception);}* _temp7033;}); break; case 373: _LL7030: yyval=(
struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7035=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7035[ 0]=({ struct Cyc_Exp_tok_struct
_temp7036; _temp7036.tag= Cyc_Exp_tok_tag; _temp7036.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7037=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7037[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7038; _temp7038.tag=
Cyc_Absyn_UnknownId_e_tag; _temp7038.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp7039= yyvs; struct _xtunion_struct** _temp7041= _temp7039.curr
+( yyvsp_offset - 1); if( _temp7039.base == 0? 1:( _temp7041 < _temp7039.base? 1:
_temp7041 >= _temp7039.last_plus_one)){ _throw( Null_Exception);}* _temp7041;}));
_temp7038;}); _temp7037;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7042= yyls; struct Cyc_Yyltype* _temp7044= _temp7042.curr +( yylsp_offset -
1); if( _temp7042.base == 0? 1:( _temp7044 < _temp7042.base? 1: _temp7044 >=
_temp7042.last_plus_one)){ _throw( Null_Exception);}* _temp7044;}).first_line,({
struct _tagged_ptr3 _temp7045= yyls; struct Cyc_Yyltype* _temp7047= _temp7045.curr
+( yylsp_offset - 1); if( _temp7045.base == 0? 1:( _temp7047 < _temp7045.base? 1:
_temp7047 >= _temp7045.last_plus_one)){ _throw( Null_Exception);}* _temp7047;}).last_line)),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7048= yyls; struct Cyc_Yyltype*
_temp7050= _temp7048.curr +( yylsp_offset - 1); if( _temp7048.base == 0? 1:(
_temp7050 < _temp7048.base? 1: _temp7050 >= _temp7048.last_plus_one)){ _throw(
Null_Exception);}* _temp7050;}).first_line,({ struct _tagged_ptr3 _temp7051=
yyls; struct Cyc_Yyltype* _temp7053= _temp7051.curr + yylsp_offset; if(
_temp7051.base == 0? 1:( _temp7053 < _temp7051.base? 1: _temp7053 >= _temp7051.last_plus_one)){
_throw( Null_Exception);}* _temp7053;}).last_line)); _temp7036;}); _temp7035;});
break; case 374: _LL7034: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7055=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7055[ 0]=({ struct Cyc_Exp_tok_struct _temp7056; _temp7056.tag= Cyc_Exp_tok_tag;
_temp7056.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp7057=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7057[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7058; _temp7058.tag=
Cyc_Absyn_UnknownId_e_tag; _temp7058.f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp7059= yyvs; struct _xtunion_struct** _temp7061= _temp7059.curr
+( yyvsp_offset - 4); if( _temp7059.base == 0? 1:( _temp7061 < _temp7059.base? 1:
_temp7061 >= _temp7059.last_plus_one)){ _throw( Null_Exception);}* _temp7061;}));
_temp7058;}); _temp7057;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7062= yyls; struct Cyc_Yyltype* _temp7064= _temp7062.curr +( yylsp_offset -
4); if( _temp7062.base == 0? 1:( _temp7064 < _temp7062.base? 1: _temp7064 >=
_temp7062.last_plus_one)){ _throw( Null_Exception);}* _temp7064;}).first_line,({
struct _tagged_ptr3 _temp7065= yyls; struct Cyc_Yyltype* _temp7067= _temp7065.curr
+( yylsp_offset - 4); if( _temp7065.base == 0? 1:( _temp7067 < _temp7065.base? 1:
_temp7067 >= _temp7065.last_plus_one)){ _throw( Null_Exception);}* _temp7067;}).last_line)),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp7068= yyvs; struct _xtunion_struct** _temp7070=
_temp7068.curr +( yyvsp_offset - 1); if( _temp7068.base == 0? 1:( _temp7070 <
_temp7068.base? 1: _temp7070 >= _temp7068.last_plus_one)){ _throw(
Null_Exception);}* _temp7070;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7071= yyls; struct Cyc_Yyltype* _temp7073= _temp7071.curr +(
yylsp_offset - 4); if( _temp7071.base == 0? 1:( _temp7073 < _temp7071.base? 1:
_temp7073 >= _temp7071.last_plus_one)){ _throw( Null_Exception);}* _temp7073;}).first_line,({
struct _tagged_ptr3 _temp7074= yyls; struct Cyc_Yyltype* _temp7076= _temp7074.curr
+ yylsp_offset; if( _temp7074.base == 0? 1:( _temp7076 < _temp7074.base? 1:
_temp7076 >= _temp7074.last_plus_one)){ _throw( Null_Exception);}* _temp7076;}).last_line));
_temp7056;}); _temp7055;}); break; case 375: _LL7054: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7078=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7078[ 0]=({ struct Cyc_Exp_tok_struct
_temp7079; _temp7079.tag= Cyc_Exp_tok_tag; _temp7079.f1= Cyc_Absyn_tuple_exp(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7080= yyvs; struct
_xtunion_struct** _temp7082= _temp7080.curr +( yyvsp_offset - 1); if( _temp7080.base
== 0? 1:( _temp7082 < _temp7080.base? 1: _temp7082 >= _temp7080.last_plus_one)){
_throw( Null_Exception);}* _temp7082;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7083= yyls; struct Cyc_Yyltype* _temp7085= _temp7083.curr +(
yylsp_offset - 3); if( _temp7083.base == 0? 1:( _temp7085 < _temp7083.base? 1:
_temp7085 >= _temp7083.last_plus_one)){ _throw( Null_Exception);}* _temp7085;}).first_line,({
struct _tagged_ptr3 _temp7086= yyls; struct Cyc_Yyltype* _temp7088= _temp7086.curr
+ yylsp_offset; if( _temp7086.base == 0? 1:( _temp7088 < _temp7086.base? 1:
_temp7088 >= _temp7086.last_plus_one)){ _throw( Null_Exception);}* _temp7088;}).last_line));
_temp7079;}); _temp7078;}); break; case 376: _LL7077: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7090=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7090[ 0]=({ struct Cyc_Exp_tok_struct
_temp7091; _temp7091.tag= Cyc_Exp_tok_tag; _temp7091.f1= Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Struct_e_struct* _temp7092=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp7092[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp7093; _temp7093.tag= Cyc_Absyn_Struct_e_tag;
_temp7093.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7094= yyvs;
struct _xtunion_struct** _temp7096= _temp7094.curr +( yyvsp_offset - 3); if(
_temp7094.base == 0? 1:( _temp7096 < _temp7094.base? 1: _temp7096 >= _temp7094.last_plus_one)){
_throw( Null_Exception);}* _temp7096;})); _temp7093.f2= 0; _temp7093.f3=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7097= yyvs; struct _xtunion_struct** _temp7099=
_temp7097.curr +( yyvsp_offset - 1); if( _temp7097.base == 0? 1:( _temp7099 <
_temp7097.base? 1: _temp7099 >= _temp7097.last_plus_one)){ _throw(
Null_Exception);}* _temp7099;}))); _temp7093.f4= 0; _temp7093;}); _temp7092;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7100= yyls; struct Cyc_Yyltype*
_temp7102= _temp7100.curr +( yylsp_offset - 3); if( _temp7100.base == 0? 1:(
_temp7102 < _temp7100.base? 1: _temp7102 >= _temp7100.last_plus_one)){ _throw(
Null_Exception);}* _temp7102;}).first_line,({ struct _tagged_ptr3 _temp7103=
yyls; struct Cyc_Yyltype* _temp7105= _temp7103.curr + yylsp_offset; if(
_temp7103.base == 0? 1:( _temp7105 < _temp7103.base? 1: _temp7105 >= _temp7103.last_plus_one)){
_throw( Null_Exception);}* _temp7105;}).last_line)); _temp7091;}); _temp7090;});
break; case 377: _LL7089: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7107=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7107[ 0]=({ struct Cyc_Exp_tok_struct _temp7108; _temp7108.tag= Cyc_Exp_tok_tag;
_temp7108.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp7109= yyvs; struct _xtunion_struct** _temp7111= _temp7109.curr +(
yyvsp_offset - 2); if( _temp7109.base == 0? 1:( _temp7111 < _temp7109.base? 1:
_temp7111 >= _temp7109.last_plus_one)){ _throw( Null_Exception);}* _temp7111;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7112= yyls; struct Cyc_Yyltype*
_temp7114= _temp7112.curr +( yylsp_offset - 4); if( _temp7112.base == 0? 1:(
_temp7114 < _temp7112.base? 1: _temp7114 >= _temp7112.last_plus_one)){ _throw(
Null_Exception);}* _temp7114;}).first_line,({ struct _tagged_ptr3 _temp7115=
yyls; struct Cyc_Yyltype* _temp7117= _temp7115.curr + yylsp_offset; if(
_temp7115.base == 0? 1:( _temp7117 < _temp7115.base? 1: _temp7117 >= _temp7115.last_plus_one)){
_throw( Null_Exception);}* _temp7117;}).last_line)); _temp7108;}); _temp7107;});
break; case 378: _LL7106: yyval=( struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct*
_temp7119=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7119[ 0]=({ struct Cyc_ExpList_tok_struct _temp7120; _temp7120.tag= Cyc_ExpList_tok_tag;
_temp7120.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7121= yyvs; struct
_xtunion_struct** _temp7123= _temp7121.curr + yyvsp_offset; if( _temp7121.base
== 0? 1:( _temp7123 < _temp7121.base? 1: _temp7123 >= _temp7121.last_plus_one)){
_throw( Null_Exception);}* _temp7123;}))); _temp7120;}); _temp7119;}); break;
case 379: _LL7118: yyval=( struct _xtunion_struct*)({ struct Cyc_ExpList_tok_struct*
_temp7125=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7125[ 0]=({ struct Cyc_ExpList_tok_struct _temp7126; _temp7126.tag= Cyc_ExpList_tok_tag;
_temp7126.f1=({ struct Cyc_List_List* _temp7127=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp7127->hd=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7128= yyvs; struct _xtunion_struct** _temp7130=
_temp7128.curr + yyvsp_offset; if( _temp7128.base == 0? 1:( _temp7130 <
_temp7128.base? 1: _temp7130 >= _temp7128.last_plus_one)){ _throw(
Null_Exception);}* _temp7130;})); _temp7127->tl= 0; _temp7127;}); _temp7126;});
_temp7125;}); break; case 380: _LL7124: yyval=( struct _xtunion_struct*)({
struct Cyc_ExpList_tok_struct* _temp7132=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7132[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7133; _temp7133.tag= Cyc_ExpList_tok_tag; _temp7133.f1=({ struct Cyc_List_List*
_temp7134=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7134->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7138= yyvs;
struct _xtunion_struct** _temp7140= _temp7138.curr + yyvsp_offset; if( _temp7138.base
== 0? 1:( _temp7140 < _temp7138.base? 1: _temp7140 >= _temp7138.last_plus_one)){
_throw( Null_Exception);}* _temp7140;})); _temp7134->tl= Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp7135= yyvs; struct _xtunion_struct** _temp7137=
_temp7135.curr +( yyvsp_offset - 2); if( _temp7135.base == 0? 1:( _temp7137 <
_temp7135.base? 1: _temp7137 >= _temp7135.last_plus_one)){ _throw(
Null_Exception);}* _temp7137;})); _temp7134;}); _temp7133;}); _temp7132;});
break; case 381: _LL7131: yyval=( struct _xtunion_struct*)({ struct Cyc_Exp_tok_struct*
_temp7142=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7142[ 0]=({ struct Cyc_Exp_tok_struct _temp7143; _temp7143.tag= Cyc_Exp_tok_tag;
_temp7143.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp7144= yyvs; struct _xtunion_struct** _temp7146= _temp7144.curr +
yyvsp_offset; if( _temp7144.base == 0? 1:( _temp7146 < _temp7144.base? 1:
_temp7146 >= _temp7144.last_plus_one)){ _throw( Null_Exception);}* _temp7146;}))).f1,(*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp7147= yyvs; struct _xtunion_struct**
_temp7149= _temp7147.curr + yyvsp_offset; if( _temp7147.base == 0? 1:( _temp7149
< _temp7147.base? 1: _temp7149 >= _temp7147.last_plus_one)){ _throw(
Null_Exception);}* _temp7149;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7150= yyls; struct Cyc_Yyltype* _temp7152= _temp7150.curr +
yylsp_offset; if( _temp7150.base == 0? 1:( _temp7152 < _temp7150.base? 1:
_temp7152 >= _temp7150.last_plus_one)){ _throw( Null_Exception);}* _temp7152;}).first_line,({
struct _tagged_ptr3 _temp7153= yyls; struct Cyc_Yyltype* _temp7155= _temp7153.curr
+ yylsp_offset; if( _temp7153.base == 0? 1:( _temp7155 < _temp7153.base? 1:
_temp7155 >= _temp7153.last_plus_one)){ _throw( Null_Exception);}* _temp7155;}).last_line));
_temp7143;}); _temp7142;}); break; case 382: _LL7141: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7157=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7157[ 0]=({ struct Cyc_Exp_tok_struct
_temp7158; _temp7158.tag= Cyc_Exp_tok_tag; _temp7158.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp7159= yyvs; struct _xtunion_struct** _temp7161=
_temp7159.curr + yyvsp_offset; if( _temp7159.base == 0? 1:( _temp7161 <
_temp7159.base? 1: _temp7161 >= _temp7159.last_plus_one)){ _throw(
Null_Exception);}* _temp7161;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7162= yyls; struct Cyc_Yyltype* _temp7164= _temp7162.curr +
yylsp_offset; if( _temp7162.base == 0? 1:( _temp7164 < _temp7162.base? 1:
_temp7164 >= _temp7162.last_plus_one)){ _throw( Null_Exception);}* _temp7164;}).first_line,({
struct _tagged_ptr3 _temp7165= yyls; struct Cyc_Yyltype* _temp7167= _temp7165.curr
+ yylsp_offset; if( _temp7165.base == 0? 1:( _temp7167 < _temp7165.base? 1:
_temp7167 >= _temp7165.last_plus_one)){ _throw( Null_Exception);}* _temp7167;}).last_line));
_temp7158;}); _temp7157;}); break; case 383: _LL7156: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7169=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7169[ 0]=({ struct Cyc_Exp_tok_struct
_temp7170; _temp7170.tag= Cyc_Exp_tok_tag; _temp7170.f1= Cyc_Absyn_float_exp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7171= yyvs; struct
_xtunion_struct** _temp7173= _temp7171.curr + yyvsp_offset; if( _temp7171.base
== 0? 1:( _temp7173 < _temp7171.base? 1: _temp7173 >= _temp7171.last_plus_one)){
_throw( Null_Exception);}* _temp7173;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7174= yyls; struct Cyc_Yyltype* _temp7176= _temp7174.curr +
yylsp_offset; if( _temp7174.base == 0? 1:( _temp7176 < _temp7174.base? 1:
_temp7176 >= _temp7174.last_plus_one)){ _throw( Null_Exception);}* _temp7176;}).first_line,({
struct _tagged_ptr3 _temp7177= yyls; struct Cyc_Yyltype* _temp7179= _temp7177.curr
+ yylsp_offset; if( _temp7177.base == 0? 1:( _temp7179 < _temp7177.base? 1:
_temp7179 >= _temp7177.last_plus_one)){ _throw( Null_Exception);}* _temp7179;}).last_line));
_temp7170;}); _temp7169;}); break; case 384: _LL7168: yyval=( struct
_xtunion_struct*)({ struct Cyc_Exp_tok_struct* _temp7181=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7181[ 0]=({ struct Cyc_Exp_tok_struct
_temp7182; _temp7182.tag= Cyc_Exp_tok_tag; _temp7182.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7183= yyls; struct Cyc_Yyltype* _temp7185= _temp7183.curr
+ yylsp_offset; if( _temp7183.base == 0? 1:( _temp7185 < _temp7183.base? 1:
_temp7185 >= _temp7183.last_plus_one)){ _throw( Null_Exception);}* _temp7185;}).first_line,({
struct _tagged_ptr3 _temp7186= yyls; struct Cyc_Yyltype* _temp7188= _temp7186.curr
+ yylsp_offset; if( _temp7186.base == 0? 1:( _temp7188 < _temp7186.base? 1:
_temp7188 >= _temp7186.last_plus_one)){ _throw( Null_Exception);}* _temp7188;}).last_line));
_temp7182;}); _temp7181;}); break; case 385: _LL7180: yyval=( struct
_xtunion_struct*)({ struct Cyc_QualId_tok_struct* _temp7190=( struct Cyc_QualId_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualId_tok_struct)); _temp7190[ 0]=({ struct Cyc_QualId_tok_struct
_temp7191; _temp7191.tag= Cyc_QualId_tok_tag; _temp7191.f1=({ struct _tuple1*
_temp7192=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp7192->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp7197=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp7197[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp7198; _temp7198.tag= Cyc_Absyn_Rel_n_tag; _temp7198.f1= 0; _temp7198;});
_temp7197;}); _temp7192->f2=({ struct _tagged_string* _temp7193=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7193[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7194= yyvs; struct
_xtunion_struct** _temp7196= _temp7194.curr + yyvsp_offset; if( _temp7194.base
== 0? 1:( _temp7196 < _temp7194.base? 1: _temp7196 >= _temp7194.last_plus_one)){
_throw( Null_Exception);}* _temp7196;})); _temp7193;}); _temp7192;}); _temp7191;});
_temp7190;}); break; case 386: _LL7189: yyval=({ struct _tagged_ptr2 _temp7200=
yyvs; struct _xtunion_struct** _temp7202= _temp7200.curr + yyvsp_offset; if(
_temp7200.base == 0? 1:( _temp7202 < _temp7200.base? 1: _temp7202 >= _temp7200.last_plus_one)){
_throw( Null_Exception);}* _temp7202;}); break; default: _LL7199:( void) _throw((
struct _xtunion_struct*)({ struct Cyc_Yyimpossible_struct* _temp7204=( struct
Cyc_Yyimpossible_struct*) GC_malloc( sizeof( struct Cyc_Yyimpossible_struct));
_temp7204[ 0]=({ struct Cyc_Yyimpossible_struct _temp7205; _temp7205.tag= Cyc_Yyimpossible_tag;
_temp7205;}); _temp7204;}));} yyvsp_offset -= yylen; yyssp_offset -= yylen;
yylsp_offset -= yylen;({ struct _tagged_ptr2 _temp7206= yyvs; struct
_xtunion_struct** _temp7208= _temp7206.curr +( ++ yyvsp_offset); if( _temp7206.base
== 0? 1:( _temp7208 < _temp7206.base? 1: _temp7208 >= _temp7206.last_plus_one)){
_throw( Null_Exception);}* _temp7208= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp7209= yyls; struct Cyc_Yyltype* _temp7211= _temp7209.curr
+ yylsp_offset; if( _temp7209.base == 0? 1:( _temp7211 < _temp7209.base? 1:
_temp7211 >= _temp7209.last_plus_one)){ _throw( Null_Exception);}(* _temp7211).first_line=
Cyc_yylloc.first_line;});({ struct _tagged_ptr3 _temp7212= yyls; struct Cyc_Yyltype*
_temp7214= _temp7212.curr + yylsp_offset; if( _temp7212.base == 0? 1:( _temp7214
< _temp7212.base? 1: _temp7214 >= _temp7212.last_plus_one)){ _throw(
Null_Exception);}(* _temp7214).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp7215= yyls; struct Cyc_Yyltype* _temp7217= _temp7215.curr
+ yylsp_offset; if( _temp7215.base == 0? 1:( _temp7217 < _temp7215.base? 1:
_temp7217 >= _temp7215.last_plus_one)){ _throw( Null_Exception);}(* _temp7217).last_line=({
struct _tagged_ptr3 _temp7218= yyls; struct Cyc_Yyltype* _temp7220= _temp7218.curr
+( yylsp_offset - 1); if( _temp7218.base == 0? 1:( _temp7220 < _temp7218.base? 1:
_temp7220 >= _temp7218.last_plus_one)){ _throw( Null_Exception);}* _temp7220;}).last_line;});({
struct _tagged_ptr3 _temp7221= yyls; struct Cyc_Yyltype* _temp7223= _temp7221.curr
+ yylsp_offset; if( _temp7221.base == 0? 1:( _temp7223 < _temp7221.base? 1:
_temp7223 >= _temp7221.last_plus_one)){ _throw( Null_Exception);}(* _temp7223).last_column=({
struct _tagged_ptr3 _temp7224= yyls; struct Cyc_Yyltype* _temp7226= _temp7224.curr
+( yylsp_offset - 1); if( _temp7224.base == 0? 1:( _temp7226 < _temp7224.base? 1:
_temp7226 >= _temp7224.last_plus_one)){ _throw( Null_Exception);}* _temp7226;}).last_column;});({
struct _tagged_ptr3 _temp7227= yyls; struct Cyc_Yyltype* _temp7229= _temp7227.curr
+ yylsp_offset; if( _temp7227.base == 0? 1:( _temp7229 < _temp7227.base? 1:
_temp7229 >= _temp7227.last_plus_one)){ _throw( Null_Exception);}(* _temp7229).text=(
struct _tagged_string)({ char* _temp7230=( char*)""; struct _tagged_string
_temp7231; _temp7231.curr= _temp7230; _temp7231.base= _temp7230; _temp7231.last_plus_one=
_temp7230 + 1; _temp7231;});});} else{({ struct _tagged_ptr3 _temp7232= yyls;
struct Cyc_Yyltype* _temp7234= _temp7232.curr + yylsp_offset; if( _temp7232.base
== 0? 1:( _temp7234 < _temp7232.base? 1: _temp7234 >= _temp7232.last_plus_one)){
_throw( Null_Exception);}(* _temp7234).last_line=({ struct _tagged_ptr3
_temp7235= yyls; struct Cyc_Yyltype* _temp7237= _temp7235.curr +(( yylsp_offset
+ yylen) - 1); if( _temp7235.base == 0? 1:( _temp7237 < _temp7235.base? 1:
_temp7237 >= _temp7235.last_plus_one)){ _throw( Null_Exception);}* _temp7237;}).last_line;});({
struct _tagged_ptr3 _temp7238= yyls; struct Cyc_Yyltype* _temp7240= _temp7238.curr
+ yylsp_offset; if( _temp7238.base == 0? 1:( _temp7240 < _temp7238.base? 1:
_temp7240 >= _temp7238.last_plus_one)){ _throw( Null_Exception);}(* _temp7240).last_column=({
struct _tagged_ptr3 _temp7241= yyls; struct Cyc_Yyltype* _temp7243= _temp7241.curr
+(( yylsp_offset + yylen) - 1); if( _temp7241.base == 0? 1:( _temp7243 <
_temp7241.base? 1: _temp7243 >= _temp7241.last_plus_one)){ _throw(
Null_Exception);}* _temp7243;}).last_column;});} yyn=( int)({ short* _temp7244=(
short*) Cyc_yyr1; unsigned int _temp7245= yyn; if( _temp7245 >= 387u){ _throw(
Null_Exception);} _temp7244[ _temp7245];}); yystate=({ short* _temp7246=( short*)
Cyc_yypgoto; unsigned int _temp7247= yyn - 120; if( _temp7247 >= 106u){ _throw(
Null_Exception);} _temp7246[ _temp7247];}) +({ struct _tagged_ptr1 _temp7248=
yyss; short* _temp7250= _temp7248.curr + yyssp_offset; if( _temp7248.base == 0?
1:( _temp7250 < _temp7248.base? 1: _temp7250 >= _temp7248.last_plus_one)){
_throw( Null_Exception);}* _temp7250;}); if(( yystate >= 0? yystate <= 3976: 0)?({
short* _temp7251=( short*) Cyc_yycheck; unsigned int _temp7252= yystate; if(
_temp7252 >= 3977u){ _throw( Null_Exception);} _temp7251[ _temp7252];}) ==({
struct _tagged_ptr1 _temp7253= yyss; short* _temp7255= _temp7253.curr +
yyssp_offset; if( _temp7253.base == 0? 1:( _temp7255 < _temp7253.base? 1:
_temp7255 >= _temp7253.last_plus_one)){ _throw( Null_Exception);}* _temp7255;}):
0){ yystate=( int)({ short* _temp7256=( short*) Cyc_yytable; unsigned int
_temp7257= yystate; if( _temp7257 >= 3977u){ _throw( Null_Exception);} _temp7256[
_temp7257];});} else{ yystate=( int)({ short* _temp7258=( short*) Cyc_yydefgoto;
unsigned int _temp7259= yyn - 120; if( _temp7259 >= 106u){ _throw(
Null_Exception);} _temp7258[ _temp7259];});} goto yynewstate; yyerrlab: if(
yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short* _temp7260=( short*) Cyc_yypact;
unsigned int _temp7261= yystate; if( _temp7261 >= 728u){ _throw( Null_Exception);}
_temp7260[ _temp7261];}); if( yyn > - 32768? yyn < 3976: 0){ int sze= 0; struct
_tagged_string msg; int x; int count; count= 0; for( x= yyn < 0? - yyn: 0; x <
226u / sizeof( char*); x ++){ if(({ short* _temp7262=( short*) Cyc_yycheck;
unsigned int _temp7263= x + yyn; if( _temp7263 >= 3977u){ _throw( Null_Exception);}
_temp7262[ _temp7263];}) == x){ sze += Cyc_String_strlen(({ struct
_tagged_string* _temp7264=( struct _tagged_string*) Cyc_yytname; unsigned int
_temp7265= x; if( _temp7265 >= 226u){ _throw( Null_Exception);} _temp7264[
_temp7265];})) + 15, count ++;}} msg=({ unsigned int _temp7266=( unsigned int)(
sze + 15); char* _temp7267=( char*) GC_malloc_atomic( sizeof( char) * _temp7266);
unsigned int i; struct _tagged_string _temp7268={ _temp7267, _temp7267,
_temp7267 + _temp7266}; for( i= 0; i < _temp7266; i ++){ _temp7267[ i]='\000';}
_temp7268;}); Cyc_String_strcpy( msg,( struct _tagged_string)({ char* _temp7269=(
char*)"parse error"; struct _tagged_string _temp7270; _temp7270.curr= _temp7269;
_temp7270.base= _temp7269; _temp7270.last_plus_one= _temp7269 + 12; _temp7270;}));
if( count < 5){ count= 0; for( x= yyn < 0? - yyn: 0; x < 226u / sizeof( char*);
x ++){ if(({ short* _temp7271=( short*) Cyc_yycheck; unsigned int _temp7272= x +
yyn; if( _temp7272 >= 3977u){ _throw( Null_Exception);} _temp7271[ _temp7272];})
== x){ Cyc_String_strcat( msg, count == 0?( struct _tagged_string)({ char*
_temp7273=( char*)", expecting `"; struct _tagged_string _temp7274; _temp7274.curr=
_temp7273; _temp7274.base= _temp7273; _temp7274.last_plus_one= _temp7273 + 14;
_temp7274;}):( struct _tagged_string)({ char* _temp7275=( char*)" or `"; struct
_tagged_string _temp7276; _temp7276.curr= _temp7275; _temp7276.base= _temp7275;
_temp7276.last_plus_one= _temp7275 + 6; _temp7276;})); Cyc_String_strcat( msg,({
struct _tagged_string* _temp7277=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp7278= x; if( _temp7278 >= 226u){ _throw( Null_Exception);}
_temp7277[ _temp7278];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp7279=( char*)"'"; struct _tagged_string _temp7280; _temp7280.curr=
_temp7279; _temp7280.base= _temp7279; _temp7280.last_plus_one= _temp7279 + 2;
_temp7280;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp7281=( char*)"parse error"; struct _tagged_string
_temp7282; _temp7282.curr= _temp7281; _temp7282.base= _temp7281; _temp7282.last_plus_one=
_temp7281 + 12; _temp7282;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp7283= yyss; short*
_temp7285= _temp7283.curr +( -- yyssp_offset); if( _temp7283.base == 0? 1:(
_temp7285 < _temp7283.base? 1: _temp7285 >= _temp7283.last_plus_one)){ _throw(
Null_Exception);}* _temp7285;}); yylsp_offset --; yyerrhandle: yyn=( int)({
short* _temp7286=( short*) Cyc_yypact; unsigned int _temp7287= yystate; if(
_temp7287 >= 728u){ _throw( Null_Exception);} _temp7286[ _temp7287];}); if( yyn
== - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 3976)? 1:({
short* _temp7288=( short*) Cyc_yycheck; unsigned int _temp7289= yyn; if(
_temp7289 >= 3977u){ _throw( Null_Exception);} _temp7288[ _temp7289];}) != 1){
goto yyerrdefault;} yyn=( int)({ short* _temp7290=( short*) Cyc_yytable;
unsigned int _temp7291= yyn; if( _temp7291 >= 3977u){ _throw( Null_Exception);}
_temp7290[ _temp7291];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 727){
return 0;}({ struct _tagged_ptr2 _temp7292= yyvs; struct _xtunion_struct**
_temp7294= _temp7292.curr +( ++ yyvsp_offset); if( _temp7292.base == 0? 1:(
_temp7294 < _temp7292.base? 1: _temp7294 >= _temp7292.last_plus_one)){ _throw(
Null_Exception);}* _temp7294= Cyc_yylval;});({ struct _tagged_ptr3 _temp7295=
yyls; struct Cyc_Yyltype* _temp7297= _temp7295.curr +( ++ yylsp_offset); if(
_temp7295.base == 0? 1:( _temp7297 < _temp7295.base? 1: _temp7297 >= _temp7295.last_plus_one)){
_throw( Null_Exception);}* _temp7297= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, struct _xtunion_struct* v){ struct _xtunion_struct* _temp7298= v; struct
_tuple12* _temp7318; struct _tuple12 _temp7320; int _temp7321; void* _temp7323;
char _temp7325; short _temp7327; struct _tagged_string _temp7329; struct Cyc_Core_Opt*
_temp7331; struct Cyc_Core_Opt* _temp7333; struct Cyc_Core_Opt _temp7335; struct
_tagged_string* _temp7336; struct _tuple1* _temp7338; struct _tuple1 _temp7340;
struct _tagged_string* _temp7341; void* _temp7343; _LL7300: if( _temp7298->tag
== Cyc_Okay_tok_tag){ goto _LL7301;} else{ goto _LL7302;} _LL7302: if((*((
struct _xtunion_struct*) _temp7298)).tag == Cyc_Int_tok_tag){ _LL7319: _temp7318=((
struct Cyc_Int_tok_struct*) _temp7298)->f1; _temp7320=* _temp7318; _LL7324:
_temp7323= _temp7320.f1; goto _LL7322; _LL7322: _temp7321= _temp7320.f2; goto
_LL7303;} else{ goto _LL7304;} _LL7304: if((*(( struct _xtunion_struct*)
_temp7298)).tag == Cyc_Char_tok_tag){ _LL7326: _temp7325=(( struct Cyc_Char_tok_struct*)
_temp7298)->f1; goto _LL7305;} else{ goto _LL7306;} _LL7306: if((*(( struct
_xtunion_struct*) _temp7298)).tag == Cyc_Short_tok_tag){ _LL7328: _temp7327=((
struct Cyc_Short_tok_struct*) _temp7298)->f1; goto _LL7307;} else{ goto _LL7308;}
_LL7308: if((*(( struct _xtunion_struct*) _temp7298)).tag == Cyc_String_tok_tag){
_LL7330: _temp7329=(( struct Cyc_String_tok_struct*) _temp7298)->f1; goto
_LL7309;} else{ goto _LL7310;} _LL7310: if((*(( struct _xtunion_struct*)
_temp7298)).tag == Cyc_Stringopt_tok_tag){ _LL7332: _temp7331=(( struct Cyc_Stringopt_tok_struct*)
_temp7298)->f1; if( _temp7331 == 0){ goto _LL7311;} else{ goto _LL7312;}} else{
goto _LL7312;} _LL7312: if((*(( struct _xtunion_struct*) _temp7298)).tag == Cyc_Stringopt_tok_tag){
_LL7334: _temp7333=(( struct Cyc_Stringopt_tok_struct*) _temp7298)->f1; if(
_temp7333 == 0){ goto _LL7314;} else{ _temp7335=* _temp7333; _LL7337: _temp7336=(
struct _tagged_string*) _temp7335.v; goto _LL7313;}} else{ goto _LL7314;}
_LL7314: if((*(( struct _xtunion_struct*) _temp7298)).tag == Cyc_QualId_tok_tag){
_LL7339: _temp7338=(( struct Cyc_QualId_tok_struct*) _temp7298)->f1; _temp7340=*
_temp7338; _LL7344: _temp7343= _temp7340.f1; goto _LL7342; _LL7342: _temp7341=
_temp7340.f2; goto _LL7315;} else{ goto _LL7316;} _LL7316: goto _LL7317; _LL7301:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL7299; _LL7303: fprintf( Cyc_Stdio_stderr,"%d",
_temp7321); goto _LL7299; _LL7305: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp7325); goto _LL7299; _LL7307: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp7327); goto _LL7299; _LL7309:({ struct _tagged_string _temp7345= _temp7329;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp7345.last_plus_one - _temp7345.curr,
_temp7345.curr);}); goto _LL7299; _LL7311: fprintf( Cyc_Stdio_stderr,"null");
goto _LL7299; _LL7313:({ struct _tagged_string _temp7346=* _temp7336; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp7346.last_plus_one - _temp7346.curr, _temp7346.curr);});
goto _LL7299; _LL7315: { struct Cyc_List_List* prefix= 0;{ void* _temp7347=
_temp7343; struct Cyc_List_List* _temp7355; struct Cyc_List_List* _temp7357;
_LL7349: if(( unsigned int) _temp7347 > 1u?(( struct _tunion_struct*) _temp7347)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL7356: _temp7355=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp7347)->f1; goto _LL7350;} else{ goto _LL7351;}
_LL7351: if(( unsigned int) _temp7347 > 1u?(( struct _tunion_struct*) _temp7347)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL7358: _temp7357=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp7347)->f1; goto _LL7352;} else{ goto _LL7353;}
_LL7353: if(( int) _temp7347 == Cyc_Absyn_Loc_n){ goto _LL7354;} else{ goto
_LL7348;} _LL7350: prefix= _temp7355; goto _LL7348; _LL7352: prefix= _temp7357;
goto _LL7348; _LL7354: goto _LL7348; _LL7348:;} for( 0; prefix != 0; prefix=({
struct Cyc_List_List* _temp7359= prefix; if( _temp7359 == 0){ _throw(
Null_Exception);} _temp7359->tl;})){({ struct _tagged_string _temp7361=*((
struct _tagged_string*)({ struct Cyc_List_List* _temp7360= prefix; if( _temp7360
== 0){ _throw( Null_Exception);} _temp7360->hd;})); fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7361.last_plus_one - _temp7361.curr, _temp7361.curr);});}({ struct
_tagged_string _temp7362=* _temp7341; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7362.last_plus_one - _temp7362.curr, _temp7362.curr);}); goto _LL7299;}
_LL7317: fprintf( Cyc_Stdio_stderr,"?"); goto _LL7299; _LL7299:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp7363=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp7363->v=( void*) Cyc_Lexing_from_file(
f); _temp7363;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}