#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef int Cyc_wchar_t;
typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char; typedef unsigned short
Cyc_u_short; typedef unsigned int Cyc_u_int; typedef unsigned int Cyc_u_long;
typedef unsigned short Cyc_ushort; typedef unsigned int Cyc_uint; typedef
unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{ int
tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec it_interval;
struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef char* Cyc_caddr_t;
typedef unsigned short Cyc_ino_t; typedef short Cyc_dev_t; typedef int Cyc_off_t;
typedef unsigned short Cyc_uid_t; typedef unsigned short Cyc_gid_t; typedef int
Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t; typedef unsigned int
Cyc_mode_t; typedef unsigned short Cyc_nlink_t; typedef int Cyc_fd_mask; struct
Cyc__types_fd_set{ int fds_bits[ 8u]; } ; typedef struct Cyc__types_fd_set Cyc__types_fd_set;
typedef char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef
struct _tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError[ 18u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError[
19u]; extern char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{ char*
tag; struct _tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(* refill_buff)(
struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_string lex_buffer;
int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int lex_curr_pos; int
lex_last_pos; int lex_last_action; int lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf*
Cyc_Lexing_Lexbuf; struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)(
struct _tagged_string, int, void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct _tagged_ptr0{ int* curr; int* base; int*
last_plus_one; } ; struct Cyc_Lexing_lex_tables{ struct _tagged_ptr0 lex_base;
struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0 lex_default; struct
_tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ; typedef struct Cyc_Lexing_lex_tables*
Cyc_Lexing_LexTables; extern struct Cyc_Lexing_lexbuf* Cyc_Lexing_from_file(
struct Cyc_Stdio___sFILE*); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t; typedef struct Cyc_List_List*
Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty[
15u]; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void*
env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch[ 18u]; extern
void Cyc_List_iter( void(* f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter_c(
void(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern void* Cyc_List_fold_right(
void*(* f)( void*, void*), struct Cyc_List_List* x, void* accum); extern struct
Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x);
extern char Cyc_List_Nth[ 8u]; extern struct Cyc_List_List* Cyc_List_zip( struct
Cyc_List_List* x, struct Cyc_List_List* y); struct _tuple0{ struct Cyc_List_List*
f1; struct Cyc_List_List* f2; } ; extern struct _tuple0 Cyc_List_split( struct
Cyc_List_List* x); extern unsigned int Cyc_String_strlen( struct _tagged_string
s); extern int Cyc_String_strcmp( struct _tagged_string s1, struct
_tagged_string s2); extern int Cyc_String_zstrcmp( struct _tagged_string, struct
_tagged_string); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); extern struct _tagged_string Cyc_String_strcat( struct
_tagged_string dest, struct _tagged_string src); extern struct _tagged_string
Cyc_String_strcpy( struct _tagged_string dest, struct _tagged_string src);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
char Cyc_Set_Absent[ 11u]; struct Cyc_Lineno_Pos{ struct _tagged_string
logical_file; struct _tagged_string line; int line_no; int col; } ; typedef
struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int);
extern struct Cyc_Position_Segment* Cyc_Position_segment_join( struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*); extern struct _tagged_string Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*); static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; typedef
void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse( struct Cyc_Position_Segment*,
struct _tagged_string); extern char Cyc_Position_Nocontext[ 14u]; extern void
Cyc_Position_post_error( struct Cyc_Position_Error*); typedef struct
_tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string* Cyc_Absyn_var_t;
typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t;
struct _tuple1{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple1*
Cyc_Absyn_qvar_t; typedef struct _tuple1* Cyc_Absyn_qvar_opt_t; typedef struct
_tuple1* Cyc_Absyn_typedef_name_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct
Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct
Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_Fndecl; struct
Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl;
struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl;
struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl; struct Cyc_Absyn_Decl;
struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t; typedef struct
Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t; typedef
void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t; typedef
void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t;
typedef struct Cyc_Absyn_TunionFieldInfo Cyc_Absyn_tunion_field_info_t; typedef
void* Cyc_Absyn_type_t; typedef void* Cyc_Absyn_rgntype_t; typedef void* Cyc_Absyn_funcparams_t;
typedef void* Cyc_Absyn_type_modifier_t; typedef void* Cyc_Absyn_cnst_t; typedef
void* Cyc_Absyn_primop_t; typedef void* Cyc_Absyn_incrementor_t; typedef void*
Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t; typedef
struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_t; typedef struct Cyc_Absyn_Stmt*
Cyc_Absyn_stmt_opt_t; typedef void* Cyc_Absyn_raw_pat_t; typedef struct Cyc_Absyn_Pat*
Cyc_Absyn_pat_t; typedef void* Cyc_Absyn_binding_t; typedef struct Cyc_Absyn_Switch_clause*
Cyc_Absyn_switch_clause_t; typedef struct Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl_t; typedef struct Cyc_Absyn_Uniondecl*
Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Tunionfield* Cyc_Absyn_tunionfield_t;
typedef struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_tuniondecl_t; typedef struct Cyc_Absyn_Typedefdecl*
Cyc_Absyn_typedefdecl_t; typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl* Cyc_Absyn_enumdecl_t; typedef struct Cyc_Absyn_Vardecl*
Cyc_Absyn_vardecl_t; typedef void* Cyc_Absyn_raw_decl_t; typedef struct Cyc_Absyn_Decl*
Cyc_Absyn_decl_t; typedef void* Cyc_Absyn_designator_t; typedef void* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; static const int
Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract= 1; static
const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3; static
const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1; int
q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0; static
const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const int
Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
1; static const int Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3;
static const int Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0;
static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{ void* v; } ;
static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{ int
tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple1* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple1*
tunion_name; struct _tuple1* field_name; int is_xtunion; } ; static const int
Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{ int
tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
1; struct Cyc_Absyn_KnownTunionfield_struct{ int tag; struct Cyc_Absyn_Tuniondecl*
f1; struct Cyc_Absyn_Tunionfield* f2; } ; struct Cyc_Absyn_TunionFieldInfo{ void*
field_info; struct Cyc_List_List* targs; } ; static const int Cyc_Absyn_VoidType=
0; static const int Cyc_Absyn_Evar= 0; struct Cyc_Absyn_Evar_struct{ int tag;
void* f1; struct Cyc_Core_Opt* f2; int f3; } ; static const int Cyc_Absyn_VarType=
1; struct Cyc_Absyn_VarType_struct{ int tag; struct Cyc_Absyn_Tvar* f1; } ;
static const int Cyc_Absyn_TunionType= 2; struct Cyc_Absyn_TunionType_struct{
int tag; struct Cyc_Absyn_TunionInfo f1; } ; static const int Cyc_Absyn_TunionFieldType=
3; struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; static const int Cyc_Absyn_PointerType= 4; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; static const int Cyc_Absyn_IntType= 5;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Absyn_FloatType= 1; static const int Cyc_Absyn_DoubleType= 2; static
const int Cyc_Absyn_ArrayType= 6; struct Cyc_Absyn_ArrayType_struct{ int tag;
void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ; static const
int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo
f1; } ; static const int Cyc_Absyn_TupleType= 8; struct Cyc_Absyn_TupleType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_StructType= 9;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_AnonStructType=
11; struct Cyc_Absyn_AnonStructType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonUnionType= 12; struct Cyc_Absyn_AnonUnionType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_EnumType= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
16; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ; static const int
Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1; struct
Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att= 0;
struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int Cyc_Absyn_Stdcall_att=
0; static const int Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Noreturn_att=
2; static const int Cyc_Absyn_Const_att= 3; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 4; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
5; static const int Cyc_Absyn_Shared_att= 6; static const int Cyc_Absyn_Unused_att=
7; static const int Cyc_Absyn_Weak_att= 8; static const int Cyc_Absyn_Dllimport_att=
9; static const int Cyc_Absyn_Dllexport_att= 10; static const int Cyc_Absyn_No_instrument_function_att=
11; static const int Cyc_Absyn_Constructor_att= 12; static const int Cyc_Absyn_Destructor_att=
13; static const int Cyc_Absyn_No_check_memory_usage_att= 14; static const int
Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod= 0; struct
Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{ int tag;
void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
2; struct Cyc_Absyn_Function_mod_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_TypeParams_mod= 3; struct Cyc_Absyn_TypeParams_mod_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ; static
const int Cyc_Absyn_Attributes_mod= 4; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; static
const int Cyc_Absyn_Char_c= 0; struct Cyc_Absyn_Char_c_struct{ int tag; void* f1;
char f2; } ; static const int Cyc_Absyn_Short_c= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; static const int Cyc_Absyn_Int_c= 2; struct Cyc_Absyn_Int_c_struct{
int tag; void* f1; int f2; } ; static const int Cyc_Absyn_LongLong_c= 3; struct
Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ; static const
int Cyc_Absyn_Float_c= 4; struct Cyc_Absyn_Float_c_struct{ int tag; struct
_tagged_string f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Null_c= 0;
static const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static
const int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const
int Cyc_Absyn_Mod= 4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq=
6; static const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static
const int Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int
Cyc_Absyn_Not= 11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand=
13; static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
static const int Cyc_Absyn_Bitlshift= 16; static const int Cyc_Absyn_Bitlrshift=
17; static const int Cyc_Absyn_Bitarshift= 18; static const int Cyc_Absyn_Size=
19; static const int Cyc_Absyn_Printf= 20; static const int Cyc_Absyn_Fprintf=
21; static const int Cyc_Absyn_Xprintf= 22; static const int Cyc_Absyn_Scanf= 23;
static const int Cyc_Absyn_Fscanf= 24; static const int Cyc_Absyn_Sscanf= 25;
static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
static const int Cyc_Absyn_PreDec= 2; static const int Cyc_Absyn_PostDec= 3;
static const int Cyc_Absyn_Const_e= 0; struct Cyc_Absyn_Const_e_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_Var_e= 1; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple1* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_Primop_e= 3; struct Cyc_Absyn_Primop_e_struct{ int tag; void*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_AssignOp_e= 4;
struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Increment_e=
5; struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2; } ; static const int Cyc_Absyn_Conditional_e= 6; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_SeqExp_e= 7; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_FnCall_e=
9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e=
11; struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
} ; static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Cast_e= 13; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Address_e= 14; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static const
int Cyc_Absyn_New_e= 15; struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Sizeoftyp_e= 16;
struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e=
17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple2{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 27;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 28; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 30; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
31; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 32; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 33; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 34; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Skip_s=
0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt*
f4; } ; static const int Cyc_Absyn_Switch_s= 9; struct Cyc_Absyn_Switch_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Fallthru_s= 10; struct Cyc_Absyn_Fallthru_s_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2; } ; static const
int Cyc_Absyn_Decl_s= 11; struct Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl*
f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Cut_s= 12; struct
Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int
Cyc_Absyn_Splice_s= 13; struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; } ; static const int Cyc_Absyn_Label_s= 14; struct Cyc_Absyn_Label_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Do_s= 15; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; struct _tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s= 16; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Region_s= 17; struct Cyc_Absyn_Region_s_struct{ int tag; struct
Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Stmt* f3; } ;
struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List*
non_local_preds; int try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p=
0; static const int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Null_p= 1; static
const int Cyc_Absyn_Int_p= 1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1;
int f2; } ; static const int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Pointer_p= 5; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; static const int Cyc_Absyn_Reference_p= 6; struct
Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Struct_p= 7; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Tunion_p= 8; struct
Cyc_Absyn_Tunion_p_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Enum_p=
9; struct Cyc_Absyn_Enum_p_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1;
struct Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Absyn_UnknownId_p= 10;
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple1* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Switch_clause{ struct Cyc_Absyn_Pat* pattern; struct
Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp* where_clause; struct Cyc_Absyn_Stmt*
body; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Unresolved_b=
0; static const int Cyc_Absyn_Global_b= 0; struct Cyc_Absyn_Global_b_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Funname_b= 1;
struct Cyc_Absyn_Funname_b_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ;
static const int Cyc_Absyn_Param_b= 2; struct Cyc_Absyn_Param_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Local_b= 3; struct
Cyc_Absyn_Local_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Pat_b= 4; struct Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt*
width; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structdecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Uniondecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Tunionfield{
struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_List_List* typs;
struct Cyc_Position_Segment* loc; void* sc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple1* name; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d= 1; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d= 2;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Struct_d= 3; struct Cyc_Absyn_Struct_d_struct{ int tag; struct Cyc_Absyn_Structdecl*
f1; } ; static const int Cyc_Absyn_Union_d= 4; struct Cyc_Absyn_Union_d_struct{
int tag; struct Cyc_Absyn_Uniondecl* f1; } ; static const int Cyc_Absyn_Tunion_d=
5; struct Cyc_Absyn_Tunion_d_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; }
; static const int Cyc_Absyn_Enum_d= 6; struct Cyc_Absyn_Enum_d_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; } ; static const int Cyc_Absyn_Typedef_d= 7;
struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1; }
; static const int Cyc_Absyn_Namespace_d= 8; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d= 9; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
10; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y); extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern void*
Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t;
extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t;
extern void* Cyc_Absyn_starb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual tq,
void* b); extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual
tq, void* b); extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual
tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp( struct Cyc_Absyn_Exp* rgn_handle,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp( struct _tagged_string s,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp(
void*, struct Cyc_List_List* es, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
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
void* s, struct _tuple1* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs,
int is_xtunion, struct Cyc_Position_Segment* loc); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int varargs, struct Cyc_List_List*); extern struct
_tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern void Cyc_Lex_register_typedef( struct _tuple1* s); extern
void Cyc_Lex_enter_namespace( struct _tagged_string*); extern void Cyc_Lex_leave_namespace();
extern void Cyc_Lex_enter_using( struct _tuple1*); extern void Cyc_Lex_leave_using();
static const int Cyc_Parse_Struct_su= 0; static const int Cyc_Parse_Union_su= 1;
typedef void* Cyc_Parse_struct_or_union_t; static const int Cyc_Parse_TopDecls_bl=
0; struct Cyc_Parse_TopDecls_bl_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Parse_Stmt_bl= 1; struct Cyc_Parse_Stmt_bl_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Parse_FnDecl_bl= 2; struct
Cyc_Parse_FnDecl_bl_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ; typedef
void* Cyc_Parse_blockitem_t; static const int Cyc_Parse_Signed_spec= 0; struct
Cyc_Parse_Signed_spec_struct{ int tag; struct Cyc_Position_Segment* f1; } ;
static const int Cyc_Parse_Unsigned_spec= 1; struct Cyc_Parse_Unsigned_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Short_spec=
2; struct Cyc_Parse_Short_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; static const int Cyc_Parse_Long_spec= 3; struct Cyc_Parse_Long_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; static const int Cyc_Parse_Type_spec=
4; struct Cyc_Parse_Type_spec_struct{ int tag; void* f1; struct Cyc_Position_Segment*
f2; } ; static const int Cyc_Parse_Decl_spec= 5; struct Cyc_Parse_Decl_spec_struct{
int tag; struct Cyc_Absyn_Decl* f1; } ; typedef void* Cyc_Parse_type_specifier_t;
static const int Cyc_Parse_Typedef_sc= 0; static const int Cyc_Parse_Extern_sc=
1; static const int Cyc_Parse_ExternC_sc= 2; static const int Cyc_Parse_Static_sc=
3; static const int Cyc_Parse_Auto_sc= 4; static const int Cyc_Parse_Register_sc=
5; static const int Cyc_Parse_Abstract_sc= 6; typedef void* Cyc_Parse_storage_class_t;
struct Cyc_Parse_Declaration_spec{ struct Cyc_Core_Opt* sc; struct Cyc_Absyn_Tqual
tq; struct Cyc_List_List* type_specs; int is_inline; struct Cyc_List_List*
attributes; } ; typedef struct Cyc_Parse_Declaration_spec* Cyc_Parse_decl_spec_t;
struct Cyc_Parse_Declarator{ struct _tuple1* id; struct Cyc_List_List* tms; } ;
typedef struct Cyc_Parse_Declarator* Cyc_Parse_declarator_t; struct Cyc_Parse_Abstractdeclarator{
struct Cyc_List_List* tms; } ; typedef struct Cyc_Parse_Abstractdeclarator* Cyc_Parse_abstractdeclarator_t;
struct _tuple4{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment*
loc, struct _tuple2* t); struct _tuple5{ void* f1; struct Cyc_Core_Opt* f2; } ;
static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct Cyc_List_List*
ts, struct Cyc_Position_Segment* loc); struct _tuple6{ struct Cyc_Absyn_Tqual f1;
void* f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; } ; static struct
_tuple6 Cyc_Parse_apply_tms( struct Cyc_Absyn_Tqual, void*, struct Cyc_List_List*,
struct Cyc_List_List*); struct _tuple7{ struct _tuple1* f1; struct Cyc_Absyn_Tqual
f2; void* f3; struct Cyc_List_List* f4; struct Cyc_List_List* f5; } ; static
struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment*
loc, struct _tuple7* t); struct Cyc_Core_Opt* Cyc_Parse_lbuf= 0; static struct
Cyc_List_List* Cyc_Parse_parse_result= 0; static void Cyc_Parse_err( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){ Cyc_Position_post_error(
Cyc_Position_mk_err_parse( sg, msg));} static void* Cyc_Parse_abort( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){ Cyc_Parse_err( msg, sg);(
void) _throw(( void*) Cyc_Position_Exit);} static void Cyc_Parse_warn( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){({ struct _tagged_string
_temp0= Cyc_Position_string_of_segment( sg); struct _tagged_string _temp1= msg;
fprintf( Cyc_Stdio_stderr,"%.*s: Warning: %.*s\n", _temp0.last_plus_one - _temp0.curr,
_temp0.curr, _temp1.last_plus_one - _temp1.curr, _temp1.curr);}); return;}
static void* Cyc_Parse_unimp( struct _tagged_string msg, struct Cyc_Position_Segment*
sg){ return(( void*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp2= msg; xprintf("%.*s unimplemented",
_temp2.last_plus_one - _temp2.curr, _temp2.curr);}), sg);} static void Cyc_Parse_unimp2(
struct _tagged_string msg, struct Cyc_Position_Segment* sg){({ struct
_tagged_string _temp3= Cyc_Position_string_of_segment( sg); struct
_tagged_string _temp4= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: Cyclone does not yet support %.*s\n",
_temp3.last_plus_one - _temp3.curr, _temp3.curr, _temp4.last_plus_one - _temp4.curr,
_temp4.curr);}); return;} struct _tuple8{ struct _tuple7* f1; struct Cyc_Core_Opt*
f2; } ; static struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field( struct
Cyc_Position_Segment* loc, struct _tuple8* field_info){ struct Cyc_Core_Opt*
_temp7; struct _tuple7* _temp9; struct _tuple8 _temp5=* field_info; _LL10:
_temp9= _temp5.f1; goto _LL8; _LL8: _temp7= _temp5.f2; goto _LL6; _LL6: if((*
_temp9).f4 != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp11=( char*)"bad type params in struct field";
struct _tagged_string _temp12; _temp12.curr= _temp11; _temp12.base= _temp11;
_temp12.last_plus_one= _temp11 + 32; _temp12;}), loc);}{ struct _tuple1* qid=(*
_temp9).f1;{ void* _temp13=(* qid).f1; struct Cyc_List_List* _temp23; struct Cyc_List_List*
_temp25; _LL15: if(( unsigned int) _temp13 > 1u?*(( int*) _temp13) == Cyc_Absyn_Rel_n:
0){ _LL24: _temp23=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp13)->f1; if( _temp23 == 0){ goto _LL16;} else{ goto _LL17;}} else{ goto
_LL17;} _LL17: if(( unsigned int) _temp13 > 1u?*(( int*) _temp13) == Cyc_Absyn_Abs_n:
0){ _LL26: _temp25=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp13)->f1; if( _temp25 == 0){ goto _LL18;} else{ goto _LL19;}} else{ goto
_LL19;} _LL19: if( _temp13 ==( void*) Cyc_Absyn_Loc_n){ goto _LL20;} else{ goto
_LL21;} _LL21: goto _LL22; _LL16: goto _LL14; _LL18: goto _LL14; _LL20: goto
_LL14; _LL22: Cyc_Parse_err(( struct _tagged_string)({ char* _temp27=( char*)"struct field cannot be qualified with a module name";
struct _tagged_string _temp28; _temp28.curr= _temp27; _temp28.base= _temp27;
_temp28.last_plus_one= _temp27 + 52; _temp28;}), loc); goto _LL14; _LL14:;}{
struct Cyc_List_List* atts=(* _temp9).f5; return({ struct Cyc_Absyn_Structfield*
_temp29=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp29->name=(* qid).f2; _temp29->tq=(* _temp9).f2; _temp29->type=( void*)(*
_temp9).f3; _temp29->width= _temp7; _temp29->attributes= atts; _temp29;});}}}
struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple9* field){ struct _tuple9 _temp32;
struct Cyc_List_List* _temp33; void* _temp35; struct Cyc_Absyn_Tqual _temp37;
struct Cyc_Core_Opt* _temp39; struct _tuple9* _temp30= field; _temp32=* _temp30;
_LL40: _temp39= _temp32.f1; goto _LL38; _LL38: _temp37= _temp32.f2; goto _LL36;
_LL36: _temp35= _temp32.f3; goto _LL34; _LL34: _temp33= _temp32.f4; goto _LL31;
_LL31: { struct Cyc_Core_Opt* idopt= 0; if( _temp39 != 0){ idopt=({ struct Cyc_Core_Opt*
_temp41=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp41->v=( void*)(*(( struct _tuple1*)({ struct Cyc_Core_Opt* _temp42= _temp39;
if( _temp42 == 0){ _throw( Null_Exception);} _temp42->v;}))).f2; _temp41;});{
void* _temp44=(*(( struct _tuple1*)({ struct Cyc_Core_Opt* _temp43= _temp39; if(
_temp43 == 0){ _throw( Null_Exception);} _temp43->v;}))).f1; struct Cyc_List_List*
_temp54; struct Cyc_List_List* _temp56; _LL46: if(( unsigned int) _temp44 > 1u?*((
int*) _temp44) == Cyc_Absyn_Rel_n: 0){ _LL55: _temp54=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp44)->f1; if( _temp54 == 0){ goto _LL47;}
else{ goto _LL48;}} else{ goto _LL48;} _LL48: if(( unsigned int) _temp44 > 1u?*((
int*) _temp44) == Cyc_Absyn_Abs_n: 0){ _LL57: _temp56=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp44)->f1; if( _temp56 == 0){ goto _LL49;}
else{ goto _LL50;}} else{ goto _LL50;} _LL50: if( _temp44 ==( void*) Cyc_Absyn_Loc_n){
goto _LL51;} else{ goto _LL52;} _LL52: goto _LL53; _LL47: goto _LL45; _LL49:
goto _LL45; _LL51: goto _LL45; _LL53: Cyc_Parse_err(( struct _tagged_string)({
char* _temp58=( char*)"parameter cannot be qualified with a module name"; struct
_tagged_string _temp59; _temp59.curr= _temp58; _temp59.base= _temp58; _temp59.last_plus_one=
_temp58 + 49; _temp59;}), loc); goto _LL45; _LL45:;}} if( _temp33 != 0){(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp60=( char*)"parameter should have no type parameters";
struct _tagged_string _temp61; _temp61.curr= _temp60; _temp61.base= _temp60;
_temp61.last_plus_one= _temp60 + 41; _temp61;}), loc);} return({ struct _tuple2*
_temp62=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp62->f1=
idopt; _temp62->f2= _temp37; _temp62->f3= _temp35; _temp62;});}} static void*
Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Parse_Type_spec_struct* _temp63=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp63[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp64; _temp64.tag= Cyc_Parse_Type_spec; _temp64.f1=(
void*) t; _temp64.f2= loc; _temp64;}); _temp63;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp65= t; struct Cyc_Absyn_Exp* _temp71; struct Cyc_Absyn_Tqual
_temp73; void* _temp75; _LL67: if(( unsigned int) _temp65 > 4u?*(( int*) _temp65)
== Cyc_Absyn_ArrayType: 0){ _LL76: _temp75=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp65)->f1; goto _LL74; _LL74: _temp73=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp65)->f2; goto _LL72; _LL72: _temp71=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp65)->f3; goto _LL68;} else{ goto _LL69;} _LL69: goto _LL70; _LL68: return
Cyc_Absyn_starb_typ( _temp75,( void*) Cyc_Absyn_HeapRgn, _temp73, _temp71 == 0?(
void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp77=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp77[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp78; _temp78.tag= Cyc_Absyn_Upper_b;
_temp78.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp79= _temp71;
if( _temp79 == 0){ _throw( Null_Exception);} _temp79;}); _temp78;}); _temp77;}));
_LL70: return t; _LL66:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple10{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct _tuple10* Cyc_Parse_get_tqual_typ( struct Cyc_Position_Segment*
loc, struct _tuple2* t){ return({ struct _tuple10* _temp80=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp80->f1=(* t).f2; _temp80->f2=(* t).f3;
_temp80;});} static void Cyc_Parse_only_vardecl( struct Cyc_List_List* params,
struct Cyc_Absyn_Decl* x){ struct _tagged_string decl_kind;{ void* _temp81=(
void*) x->r; struct Cyc_Absyn_Vardecl* _temp105; int _temp107; struct Cyc_Absyn_Exp*
_temp109; struct Cyc_Core_Opt* _temp111; struct Cyc_Core_Opt* _temp113; struct
Cyc_Absyn_Pat* _temp115; struct Cyc_Absyn_Fndecl* _temp117; struct Cyc_Absyn_Structdecl*
_temp119; struct Cyc_Absyn_Uniondecl* _temp121; struct Cyc_Absyn_Tuniondecl*
_temp123; struct Cyc_Absyn_Typedefdecl* _temp125; struct Cyc_Absyn_Enumdecl*
_temp127; struct Cyc_List_List* _temp129; struct _tagged_string* _temp131;
struct Cyc_List_List* _temp133; struct _tuple1* _temp135; struct Cyc_List_List*
_temp137; _LL83: if(*(( int*) _temp81) == Cyc_Absyn_Var_d){ _LL106: _temp105=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp81)->f1; goto
_LL84;} else{ goto _LL85;} _LL85: if(*(( int*) _temp81) == Cyc_Absyn_Let_d){
_LL116: _temp115=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp81)->f1; goto _LL114; _LL114: _temp113=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp81)->f2; goto _LL112; _LL112: _temp111=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp81)->f3; goto _LL110; _LL110: _temp109=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Let_d_struct*) _temp81)->f4; goto _LL108; _LL108: _temp107=( int)((
struct Cyc_Absyn_Let_d_struct*) _temp81)->f5; goto _LL86;} else{ goto _LL87;}
_LL87: if(*(( int*) _temp81) == Cyc_Absyn_Fn_d){ _LL118: _temp117=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp81)->f1; goto _LL88;} else{ goto _LL89;}
_LL89: if(*(( int*) _temp81) == Cyc_Absyn_Struct_d){ _LL120: _temp119=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp81)->f1; goto
_LL90;} else{ goto _LL91;} _LL91: if(*(( int*) _temp81) == Cyc_Absyn_Union_d){
_LL122: _temp121=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp81)->f1; goto _LL92;} else{ goto _LL93;} _LL93: if(*(( int*) _temp81) ==
Cyc_Absyn_Tunion_d){ _LL124: _temp123=( struct Cyc_Absyn_Tuniondecl*)(( struct
Cyc_Absyn_Tunion_d_struct*) _temp81)->f1; goto _LL94;} else{ goto _LL95;} _LL95:
if(*(( int*) _temp81) == Cyc_Absyn_Typedef_d){ _LL126: _temp125=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp81)->f1; goto _LL96;} else{ goto _LL97;}
_LL97: if(*(( int*) _temp81) == Cyc_Absyn_Enum_d){ _LL128: _temp127=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp81)->f1; goto _LL98;} else{ goto _LL99;}
_LL99: if(*(( int*) _temp81) == Cyc_Absyn_Namespace_d){ _LL132: _temp131=(
struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp81)->f1;
goto _LL130; _LL130: _temp129=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp81)->f2; goto _LL100;} else{ goto _LL101;} _LL101: if(*(( int*) _temp81) ==
Cyc_Absyn_Using_d){ _LL136: _temp135=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*)
_temp81)->f1; goto _LL134; _LL134: _temp133=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Using_d_struct*) _temp81)->f2; goto _LL102;} else{ goto _LL103;}
_LL103: if(*(( int*) _temp81) == Cyc_Absyn_ExternC_d){ _LL138: _temp137=( struct
Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*) _temp81)->f1; goto _LL104;}
else{ goto _LL82;} _LL84: if( _temp105->initializer != 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp139=( char*)"initializers are not allowed in parameter declarations";
struct _tagged_string _temp140; _temp140.curr= _temp139; _temp140.base= _temp139;
_temp140.last_plus_one= _temp139 + 55; _temp140;}), x->loc);}{ void* _temp141=(*
_temp105->name).f1; struct Cyc_List_List* _temp151; struct Cyc_List_List*
_temp153; _LL143: if( _temp141 ==( void*) Cyc_Absyn_Loc_n){ goto _LL144;} else{
goto _LL145;} _LL145: if(( unsigned int) _temp141 > 1u?*(( int*) _temp141) ==
Cyc_Absyn_Rel_n: 0){ _LL152: _temp151=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp141)->f1; if( _temp151 == 0){ goto _LL146;} else{ goto _LL147;}} else{ goto
_LL147;} _LL147: if(( unsigned int) _temp141 > 1u?*(( int*) _temp141) == Cyc_Absyn_Abs_n:
0){ _LL154: _temp153=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp141)->f1; if( _temp153 == 0){ goto _LL148;} else{ goto _LL149;}} else{ goto
_LL149;} _LL149: goto _LL150; _LL144: goto _LL142; _LL146: goto _LL142; _LL148:
goto _LL142; _LL150: Cyc_Parse_err(( struct _tagged_string)({ char* _temp155=(
char*)"module names not allowed on parameter declarations"; struct
_tagged_string _temp156; _temp156.curr= _temp155; _temp156.base= _temp155;
_temp156.last_plus_one= _temp155 + 51; _temp156;}), x->loc); goto _LL142; _LL142:;}{
int found= 0; for( 0; params != 0; params=({ struct Cyc_List_List* _temp157=
params; if( _temp157 == 0){ _throw( Null_Exception);} _temp157->tl;})){ if( Cyc_String_zstrptrcmp((*
_temp105->name).f2,( struct _tagged_string*)({ struct Cyc_List_List* _temp158=
params; if( _temp158 == 0){ _throw( Null_Exception);} _temp158->hd;})) == 0){
found= 1; break;}} if( ! found){(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp159=*(*
_temp105->name).f2; xprintf("%.*s is not listed as a parameter", _temp159.last_plus_one
- _temp159.curr, _temp159.curr);}), x->loc);} return;} _LL86: decl_kind=( struct
_tagged_string)({ char* _temp160=( char*)"let declaration"; struct
_tagged_string _temp161; _temp161.curr= _temp160; _temp161.base= _temp160;
_temp161.last_plus_one= _temp160 + 16; _temp161;}); goto _LL82; _LL88: decl_kind=(
struct _tagged_string)({ char* _temp162=( char*)"function declaration"; struct
_tagged_string _temp163; _temp163.curr= _temp162; _temp163.base= _temp162;
_temp163.last_plus_one= _temp162 + 21; _temp163;}); goto _LL82; _LL90: decl_kind=(
struct _tagged_string)({ char* _temp164=( char*)"struct declaration"; struct
_tagged_string _temp165; _temp165.curr= _temp164; _temp165.base= _temp164;
_temp165.last_plus_one= _temp164 + 19; _temp165;}); goto _LL82; _LL92: decl_kind=(
struct _tagged_string)({ char* _temp166=( char*)"union declaration"; struct
_tagged_string _temp167; _temp167.curr= _temp166; _temp167.base= _temp166;
_temp167.last_plus_one= _temp166 + 18; _temp167;}); goto _LL82; _LL94: decl_kind=(
struct _tagged_string)({ char* _temp168=( char*)"tunion declaration"; struct
_tagged_string _temp169; _temp169.curr= _temp168; _temp169.base= _temp168;
_temp169.last_plus_one= _temp168 + 19; _temp169;}); goto _LL82; _LL96: decl_kind=(
struct _tagged_string)({ char* _temp170=( char*)"typedef"; struct _tagged_string
_temp171; _temp171.curr= _temp170; _temp171.base= _temp170; _temp171.last_plus_one=
_temp170 + 8; _temp171;}); goto _LL82; _LL98: decl_kind=( struct _tagged_string)({
char* _temp172=( char*)"enum declaration"; struct _tagged_string _temp173;
_temp173.curr= _temp172; _temp173.base= _temp172; _temp173.last_plus_one=
_temp172 + 17; _temp173;}); goto _LL82; _LL100: decl_kind=( struct
_tagged_string)({ char* _temp174=( char*)"namespace declaration"; struct
_tagged_string _temp175; _temp175.curr= _temp174; _temp175.base= _temp174;
_temp175.last_plus_one= _temp174 + 22; _temp175;}); goto _LL82; _LL102:
decl_kind=( struct _tagged_string)({ char* _temp176=( char*)"using declaration";
struct _tagged_string _temp177; _temp177.curr= _temp176; _temp177.base= _temp176;
_temp177.last_plus_one= _temp176 + 18; _temp177;}); goto _LL82; _LL104:
decl_kind=( struct _tagged_string)({ char* _temp178=( char*)"extern C declaration";
struct _tagged_string _temp179; _temp179.curr= _temp178; _temp179.base= _temp178;
_temp179.last_plus_one= _temp178 + 21; _temp179;}); goto _LL82; _LL82:;}(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp180= decl_kind; xprintf("%.*s appears in parameter type",
_temp180.last_plus_one - _temp180.curr, _temp180.curr);}), x->loc); return;}
struct _tuple11{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ;
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple11* env, struct
_tagged_string* x){ struct _tuple11 _temp183; struct Cyc_Position_Segment*
_temp184; struct Cyc_List_List* _temp186; struct _tuple11* _temp181= env;
_temp183=* _temp181; _LL187: _temp186= _temp183.f1; goto _LL185; _LL185:
_temp184= _temp183.f2; goto _LL182; _LL182: if( _temp186 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp188=* x; xprintf("missing type for parameter %.*s",
_temp188.last_plus_one - _temp188.curr, _temp188.curr);}), _temp184);}{ void*
_temp190=( void*)(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp189=
_temp186; if( _temp189 == 0){ _throw( Null_Exception);} _temp189->hd;}))->r;
struct Cyc_Absyn_Vardecl* _temp196; _LL192: if(*(( int*) _temp190) == Cyc_Absyn_Var_d){
_LL197: _temp196=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp190)->f1; goto _LL193;} else{ goto _LL194;} _LL194: goto _LL195; _LL193:{
void* _temp198=(* _temp196->name).f1; struct Cyc_List_List* _temp208; struct Cyc_List_List*
_temp210; _LL200: if( _temp198 ==( void*) Cyc_Absyn_Loc_n){ goto _LL201;} else{
goto _LL202;} _LL202: if(( unsigned int) _temp198 > 1u?*(( int*) _temp198) ==
Cyc_Absyn_Rel_n: 0){ _LL209: _temp208=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp198)->f1; if( _temp208 == 0){ goto _LL203;} else{ goto _LL204;}} else{ goto
_LL204;} _LL204: if(( unsigned int) _temp198 > 1u?*(( int*) _temp198) == Cyc_Absyn_Abs_n:
0){ _LL211: _temp210=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp198)->f1; if( _temp210 == 0){ goto _LL205;} else{ goto _LL206;}} else{ goto
_LL206;} _LL206: goto _LL207; _LL201: goto _LL199; _LL203: goto _LL199; _LL205:
goto _LL199; _LL207: Cyc_Parse_err(( struct _tagged_string)({ char* _temp212=(
char*)"module name not allowed on parameter"; struct _tagged_string _temp213;
_temp213.curr= _temp212; _temp213.base= _temp212; _temp213.last_plus_one=
_temp212 + 37; _temp213;}), _temp184); goto _LL199; _LL199:;} if( Cyc_String_zstrptrcmp((*
_temp196->name).f2, x) == 0){ return({ struct _tuple2* _temp214=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2) * 1); _temp214[ 0]=({ struct _tuple2 _temp215;
_temp215.f1=({ struct Cyc_Core_Opt* _temp216=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt) * 1); _temp216[ 0]=({ struct Cyc_Core_Opt _temp217;
_temp217.v=( void*)(* _temp196->name).f2; _temp217;}); _temp216;}); _temp215.f2=
_temp196->tq; _temp215.f3=( void*) _temp196->type; _temp215;}); _temp214;});}
else{ return Cyc_Parse_get_param_type(({ struct _tuple11* _temp218=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp218->f1=({ struct Cyc_List_List*
_temp219= _temp186; if( _temp219 == 0){ _throw( Null_Exception);} _temp219->tl;});
_temp218->f2= _temp184; _temp218;}), x);} _LL195: return(( struct _tuple2*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp220=( char*)"non-variable declaration";
struct _tagged_string _temp221; _temp221.curr= _temp220; _temp221.base= _temp220;
_temp221.last_plus_one= _temp220 + 25; _temp221;}),(( struct Cyc_Absyn_Decl*)({
struct Cyc_List_List* _temp222= _temp186; if( _temp222 == 0){ _throw(
Null_Exception);} _temp222->hd;}))->loc); _LL191:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp223= tm; int _temp229; struct Cyc_Position_Segment*
_temp231; struct Cyc_List_List* _temp233; _LL225: if(( unsigned int) _temp223 >
1u?*(( int*) _temp223) == Cyc_Absyn_TypeParams_mod: 0){ _LL234: _temp233=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp223)->f1;
goto _LL232; _LL232: _temp231=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp223)->f2; goto _LL230; _LL230: _temp229=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp223)->f3; goto _LL226;} else{ goto _LL227;} _LL227: goto _LL228; _LL226:
return 1; _LL228: return 0; _LL224:;} static void* Cyc_Parse_id2type( struct
_tagged_string s, struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp235=( char*)"`H"; struct _tagged_string
_temp236; _temp236.curr= _temp235; _temp236.base= _temp235; _temp236.last_plus_one=
_temp235 + 3; _temp236;})) == 0){ return( void*) Cyc_Absyn_HeapRgn;} else{
return( void*)({ struct Cyc_Absyn_VarType_struct* _temp237=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp237[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp238; _temp238.tag= Cyc_Absyn_VarType; _temp238.f1=({ struct Cyc_Absyn_Tvar*
_temp239=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp239->name=({ struct _tagged_string* _temp240=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp240[ 0]= s; _temp240;});
_temp239->kind= k; _temp239;}); _temp238;}); _temp237;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp241=
t; struct Cyc_Absyn_Tvar* _temp247; _LL243: if(( unsigned int) _temp241 > 4u?*((
int*) _temp241) == Cyc_Absyn_VarType: 0){ _LL248: _temp247=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp241)->f1; goto _LL244;} else{ goto _LL245;}
_LL245: goto _LL246; _LL244: return _temp247; _LL246: return(( struct Cyc_Absyn_Tvar*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp249=( char*)"expecting a list of type variables, not types";
struct _tagged_string _temp250; _temp250.curr= _temp249; _temp250.base= _temp249;
_temp250.last_plus_one= _temp249 + 46; _temp250;}), loc); _LL242:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp251=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp251[ 0]=({ struct Cyc_Absyn_VarType_struct _temp252; _temp252.tag= Cyc_Absyn_VarType;
_temp252.f1= pr; _temp252;}); _temp251;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp254=(
void*)({ struct Cyc_List_List* _temp253= tms; if( _temp253 == 0){ _throw(
Null_Exception);} _temp253->hd;}); void* _temp260; _LL256: if(( unsigned int)
_temp254 > 1u?*(( int*) _temp254) == Cyc_Absyn_Function_mod: 0){ _LL261:
_temp260=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp254)->f1; goto
_LL257;} else{ goto _LL258;} _LL258: goto _LL259; _LL257: if(({ struct Cyc_List_List*
_temp262= tms; if( _temp262 == 0){ _throw( Null_Exception);} _temp262->tl;}) ==
0? 1:( Cyc_Parse_is_typeparam(( void*)({ struct Cyc_List_List* _temp264=({
struct Cyc_List_List* _temp263= tms; if( _temp263 == 0){ _throw( Null_Exception);}
_temp263->tl;}); if( _temp264 == 0){ _throw( Null_Exception);} _temp264->hd;}))?({
struct Cyc_List_List* _temp266=({ struct Cyc_List_List* _temp265= tms; if(
_temp265 == 0){ _throw( Null_Exception);} _temp265->tl;}); if( _temp266 == 0){
_throw( Null_Exception);} _temp266->tl;}) == 0: 0)){ void* _temp267= _temp260;
struct Cyc_Core_Opt* _temp273; int _temp275; struct Cyc_List_List* _temp277;
struct Cyc_Position_Segment* _temp279; struct Cyc_List_List* _temp281; _LL269:
if(*(( int*) _temp267) == Cyc_Absyn_WithTypes){ _LL278: _temp277=( struct Cyc_List_List*)((
struct Cyc_Absyn_WithTypes_struct*) _temp267)->f1; goto _LL276; _LL276: _temp275=(
int)(( struct Cyc_Absyn_WithTypes_struct*) _temp267)->f2; goto _LL274; _LL274:
_temp273=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp267)->f3;
goto _LL270;} else{ goto _LL271;} _LL271: if(*(( int*) _temp267) == Cyc_Absyn_NoTypes){
_LL282: _temp281=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp267)->f1; goto _LL280; _LL280: _temp279=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp267)->f2; goto _LL272;} else{ goto _LL268;}
_LL270: Cyc_Parse_warn(( struct _tagged_string)({ char* _temp283=( char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp284; _temp284.curr= _temp283; _temp284.base= _temp283;
_temp284.last_plus_one= _temp283 + 92; _temp284;}), loc); return tms; _LL272:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp281, tds);{ struct _tuple11* env=({ struct _tuple11* _temp290=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp290->f1= tds; _temp290->f2=
loc; _temp290;}); return({ struct Cyc_List_List* _temp285=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp285->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp286=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp286[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp287; _temp287.tag= Cyc_Absyn_Function_mod;
_temp287.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp288=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp288[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp289; _temp289.tag= Cyc_Absyn_WithTypes;
_temp289.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple11*,
struct _tagged_string*), struct _tuple11* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, env, _temp281); _temp289.f2= 0; _temp289.f3= 0;
_temp289;}); _temp288;})); _temp287;}); _temp286;})); _temp285->tl= 0; _temp285;});}
_LL268:;} else{ return({ struct Cyc_List_List* _temp291=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp291->hd=( void*)(( void*)({
struct Cyc_List_List* _temp293= tms; if( _temp293 == 0){ _throw( Null_Exception);}
_temp293->hd;})); _temp291->tl= Cyc_Parse_oldstyle2newstyle(({ struct Cyc_List_List*
_temp292= tms; if( _temp292 == 0){ _throw( Null_Exception);} _temp292->tl;}),
tds, loc); _temp291;});} _LL259: return({ struct Cyc_List_List* _temp294=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp294->hd=(
void*)(( void*)({ struct Cyc_List_List* _temp296= tms; if( _temp296 == 0){
_throw( Null_Exception);} _temp296->hd;})); _temp294->tl= Cyc_Parse_oldstyle2newstyle(({
struct Cyc_List_List* _temp295= tms; if( _temp295 == 0){ _throw( Null_Exception);}
_temp295->tl;}), tds, loc); _temp294;}); _LL255:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp297=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp297->id= d->id; _temp297->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp297;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)({ struct Cyc_Core_Opt* _temp298= dso; if( _temp298
== 0){ _throw( Null_Exception);} _temp298->v;}))->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*)({
struct Cyc_Core_Opt* _temp299= dso; if( _temp299 == 0){ _throw( Null_Exception);}
_temp299->v;}))->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)({ struct
Cyc_Core_Opt* _temp300= dso; if( _temp300 == 0){ _throw( Null_Exception);}
_temp300->v;}))->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)({ struct
Cyc_Core_Opt* _temp301= dso; if( _temp301 == 0){ _throw( Null_Exception);}
_temp301->v;}))->attributes; if((( struct Cyc_Parse_Declaration_spec*)({ struct
Cyc_Core_Opt* _temp302= dso; if( _temp302 == 0){ _throw( Null_Exception);}
_temp302->v;}))->sc != 0){ void* _temp305=( void*)({ struct Cyc_Core_Opt*
_temp304=(( struct Cyc_Parse_Declaration_spec*)({ struct Cyc_Core_Opt* _temp303=
dso; if( _temp303 == 0){ _throw( Null_Exception);} _temp303->v;}))->sc; if(
_temp304 == 0){ _throw( Null_Exception);} _temp304->v;}); _LL307: if( _temp305
==( void*) Cyc_Parse_Extern_sc){ goto _LL308;} else{ goto _LL309;} _LL309: if(
_temp305 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL310;} else{ goto _LL311;}
_LL311: if( _temp305 ==( void*) Cyc_Parse_Static_sc){ goto _LL312;} else{ goto
_LL313;} _LL313: goto _LL314; _LL308: sc=( void*) Cyc_Absyn_Extern; goto _LL306;
_LL310: sc=( void*) Cyc_Absyn_ExternC; goto _LL306; _LL312: sc=( void*) Cyc_Absyn_Static;
goto _LL306; _LL314: Cyc_Parse_err(( struct _tagged_string)({ char* _temp315=(
char*)"bad storage class on function"; struct _tagged_string _temp316; _temp316.curr=
_temp315; _temp316.base= _temp315; _temp316.last_plus_one= _temp315 + 30;
_temp316;}), loc); goto _LL306; _LL306:;}}{ struct Cyc_Core_Opt* _temp319; void*
_temp321; struct _tuple5 _temp317= Cyc_Parse_collapse_type_specifiers( tss, loc);
_LL322: _temp321= _temp317.f1; goto _LL320; _LL320: _temp319= _temp317.f2; goto
_LL318; _LL318: { struct Cyc_List_List* _temp325; struct Cyc_List_List* _temp327;
void* _temp329; struct Cyc_Absyn_Tqual _temp331; struct _tuple6 _temp323= Cyc_Parse_apply_tms(
tq, _temp321, atts, d->tms); _LL332: _temp331= _temp323.f1; goto _LL330; _LL330:
_temp329= _temp323.f2; goto _LL328; _LL328: _temp327= _temp323.f3; goto _LL326;
_LL326: _temp325= _temp323.f4; goto _LL324; _LL324: if( _temp319 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp333=( char*)"nested type declaration within function prototype";
struct _tagged_string _temp334; _temp334.curr= _temp333; _temp334.base= _temp333;
_temp334.last_plus_one= _temp333 + 50; _temp334;}), loc);} if( _temp327 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp335=( char*)"bad type params, ignoring";
struct _tagged_string _temp336; _temp336.curr= _temp335; _temp336.base= _temp335;
_temp336.last_plus_one= _temp335 + 26; _temp336;}), loc);}{ void* _temp337=
_temp329; struct Cyc_Absyn_FnInfo _temp343; struct Cyc_List_List* _temp345; int
_temp347; struct Cyc_List_List* _temp349; void* _temp351; struct Cyc_Core_Opt*
_temp353; struct Cyc_List_List* _temp355; _LL339: if(( unsigned int) _temp337 >
4u?*(( int*) _temp337) == Cyc_Absyn_FnType: 0){ _LL344: _temp343=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp337)->f1; _LL356: _temp355=( struct Cyc_List_List*)
_temp343.tvars; goto _LL354; _LL354: _temp353=( struct Cyc_Core_Opt*) _temp343.effect;
goto _LL352; _LL352: _temp351=( void*) _temp343.ret_typ; goto _LL350; _LL350:
_temp349=( struct Cyc_List_List*) _temp343.args; goto _LL348; _LL348: _temp347=(
int) _temp343.varargs; goto _LL346; _LL346: _temp345=( struct Cyc_List_List*)
_temp343.attributes; goto _LL340;} else{ goto _LL341;} _LL341: goto _LL342;
_LL340: { struct Cyc_List_List* args2=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc,
_temp349); return({ struct Cyc_Absyn_Fndecl* _temp357=( struct Cyc_Absyn_Fndecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Fndecl)); _temp357->sc=( void*) sc; _temp357->name=
d->id; _temp357->tvs= _temp355; _temp357->is_inline= is_inline; _temp357->effect=
_temp353; _temp357->ret_type=( void*) _temp351; _temp357->args= args2; _temp357->varargs=
_temp347; _temp357->body= body; _temp357->cached_typ= 0; _temp357->param_vardecls=
0; _temp357->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp345, _temp325); _temp357;});}
_LL342: return(( struct Cyc_Absyn_Fndecl*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char*
_temp358=( char*)"declarator is not a function prototype"; struct _tagged_string
_temp359; _temp359.curr= _temp358; _temp359.base= _temp358; _temp359.last_plus_one=
_temp358 + 39; _temp359;}), loc); _LL338:;}}}}} static struct _tuple4* Cyc_Parse_fnargspec_to_arg(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp360=( char*)"missing argument variable in function prototype";
struct _tagged_string _temp361; _temp361.curr= _temp360; _temp361.base= _temp360;
_temp361.last_plus_one= _temp360 + 48; _temp361;}), loc); return({ struct
_tuple4* _temp362=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp362->f1=({ struct _tagged_string* _temp363=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp363[ 0]=( struct
_tagged_string)({ char* _temp364=( char*)"?"; struct _tagged_string _temp365;
_temp365.curr= _temp364; _temp365.base= _temp364; _temp365.last_plus_one=
_temp364 + 2; _temp365;}); _temp363;}); _temp362->f2=(* t).f2; _temp362->f3=(* t).f3;
_temp362;});} else{ return({ struct _tuple4* _temp366=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp366->f1=( struct _tagged_string*)({
struct Cyc_Core_Opt* _temp367=(* t).f1; if( _temp367 == 0){ _throw(
Null_Exception);} _temp367->v;}); _temp366->f2=(* t).f2; _temp366->f3=(* t).f3;
_temp366;});}} static char _temp370[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_string Cyc_Parse_msg1=( struct _tagged_string){ _temp370,
_temp370, _temp370 + 52u}; static char _temp373[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2=( struct _tagged_string){ _temp373,
_temp373, _temp373 + 63u}; static char _temp376[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3=( struct _tagged_string){ _temp376,
_temp376, _temp376 + 50u}; static char _temp379[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4=( struct _tagged_string){ _temp379,
_temp379, _temp379 + 60u}; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
declopt= 0; int seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*)
Cyc_Absyn_VoidType; void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed;
struct Cyc_Position_Segment* last_loc= loc; for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp380= ts; if( _temp380 == 0){ _throw( Null_Exception);} _temp380->tl;})){
void* _temp382=( void*)({ struct Cyc_List_List* _temp381= ts; if( _temp381 == 0){
_throw( Null_Exception);} _temp381->hd;}); struct Cyc_Position_Segment* _temp396;
void* _temp398; struct Cyc_Position_Segment* _temp400; struct Cyc_Position_Segment*
_temp402; struct Cyc_Position_Segment* _temp404; struct Cyc_Position_Segment*
_temp406; struct Cyc_Absyn_Decl* _temp408; _LL384: if(*(( int*) _temp382) == Cyc_Parse_Type_spec){
_LL399: _temp398=( void*)(( struct Cyc_Parse_Type_spec_struct*) _temp382)->f1;
goto _LL397; _LL397: _temp396=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Type_spec_struct*)
_temp382)->f2; goto _LL385;} else{ goto _LL386;} _LL386: if(*(( int*) _temp382)
== Cyc_Parse_Signed_spec){ _LL401: _temp400=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Signed_spec_struct*) _temp382)->f1; goto _LL387;} else{ goto
_LL388;} _LL388: if(*(( int*) _temp382) == Cyc_Parse_Unsigned_spec){ _LL403:
_temp402=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Unsigned_spec_struct*)
_temp382)->f1; goto _LL389;} else{ goto _LL390;} _LL390: if(*(( int*) _temp382)
== Cyc_Parse_Short_spec){ _LL405: _temp404=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Short_spec_struct*) _temp382)->f1; goto _LL391;} else{ goto
_LL392;} _LL392: if(*(( int*) _temp382) == Cyc_Parse_Long_spec){ _LL407:
_temp406=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Long_spec_struct*)
_temp382)->f1; goto _LL393;} else{ goto _LL394;} _LL394: if(*(( int*) _temp382)
== Cyc_Parse_Decl_spec){ _LL409: _temp408=( struct Cyc_Absyn_Decl*)(( struct Cyc_Parse_Decl_spec_struct*)
_temp382)->f1; goto _LL395;} else{ goto _LL383;} _LL385: if( seen_type){ Cyc_Parse_err(
Cyc_Parse_msg1, _temp396);} last_loc= _temp396; seen_type= 1; t= _temp398; goto
_LL383; _LL387: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp400);} if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp410=( char*)"signed qualifier must come before type";
struct _tagged_string _temp411; _temp411.curr= _temp410; _temp411.base= _temp410;
_temp411.last_plus_one= _temp410 + 39; _temp411;}), _temp400);} last_loc=
_temp400; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed; goto _LL383; _LL389: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp402);} if( seen_type){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp412=( char*)"signed qualifier must come before type";
struct _tagged_string _temp413; _temp413.curr= _temp412; _temp413.base= _temp412;
_temp413.last_plus_one= _temp412 + 39; _temp413;}), _temp402);} last_loc=
_temp402; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL383; _LL391:
if( seen_size){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp414=( char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp415; _temp415.curr= _temp414; _temp415.base= _temp414;
_temp415.last_plus_one= _temp414 + 59; _temp415;}), _temp404);} if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp416=( char*)"short modifier must come before base type";
struct _tagged_string _temp417; _temp417.curr= _temp416; _temp417.base= _temp416;
_temp417.last_plus_one= _temp416 + 42; _temp417;}), _temp404);} last_loc=
_temp404; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL383; _LL393: if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp418=( char*)"long modifier must come before base type";
struct _tagged_string _temp419; _temp419.curr= _temp418; _temp419.base= _temp418;
_temp419.last_plus_one= _temp418 + 41; _temp419;}), _temp406);} if( seen_size){
void* _temp420= sz; _LL422: if( _temp420 ==( void*) Cyc_Absyn_B4){ goto _LL423;}
else{ goto _LL424;} _LL424: goto _LL425; _LL423: sz=( void*) Cyc_Absyn_B8; goto
_LL421; _LL425: Cyc_Parse_err(( struct _tagged_string)({ char* _temp426=( char*)"extra long in type specifier";
struct _tagged_string _temp427; _temp427.curr= _temp426; _temp427.base= _temp426;
_temp427.last_plus_one= _temp426 + 29; _temp427;}), _temp406); goto _LL421;
_LL421:;} last_loc= _temp406; seen_size= 1; goto _LL383; _LL395: last_loc=
_temp408->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp428=(
void*) _temp408->r; struct Cyc_Absyn_Structdecl* _temp440; struct Cyc_Absyn_Tuniondecl*
_temp442; struct Cyc_Absyn_Uniondecl* _temp444; struct Cyc_Absyn_Enumdecl*
_temp446; _LL430: if(*(( int*) _temp428) == Cyc_Absyn_Struct_d){ _LL441:
_temp440=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp428)->f1; goto _LL431;} else{ goto _LL432;} _LL432: if(*(( int*) _temp428)
== Cyc_Absyn_Tunion_d){ _LL443: _temp442=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp428)->f1; goto _LL433;} else{ goto
_LL434;} _LL434: if(*(( int*) _temp428) == Cyc_Absyn_Union_d){ _LL445: _temp444=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp428)->f1;
goto _LL435;} else{ goto _LL436;} _LL436: if(*(( int*) _temp428) == Cyc_Absyn_Enum_d){
_LL447: _temp446=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp428)->f1; goto _LL437;} else{ goto _LL438;} _LL438: goto _LL439; _LL431: {
struct Cyc_List_List* args=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp440->tvs); t=(
void*)({ struct Cyc_Absyn_StructType_struct* _temp448=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp448[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp449; _temp449.tag= Cyc_Absyn_StructType;
_temp449.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp450= _temp440->name; if( _temp450 == 0){ _throw( Null_Exception);} _temp450->v;}));
_temp449.f2= args; _temp449.f3= 0; _temp449;}); _temp448;}); if( _temp440->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp451=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp451->v=( void*) _temp408;
_temp451;});} goto _LL429;} _LL433: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp442->tvs); t=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp452=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp452[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp453; _temp453.tag= Cyc_Absyn_TunionType;
_temp453.f1=({ struct Cyc_Absyn_TunionInfo _temp454; _temp454.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp455=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp455[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp456; _temp456.tag= Cyc_Absyn_KnownTunion;
_temp456.f1= _temp442; _temp456;}); _temp455;})); _temp454.targs= args; _temp454.rgn=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp454;}); _temp453;}); _temp452;}); if(
_temp442->fields != 0){ declopt=({ struct Cyc_Core_Opt* _temp457=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp457->v=( void*) _temp408;
_temp457;});} goto _LL429;} _LL435: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp444->tvs); t=( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp458=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp458[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp459; _temp459.tag= Cyc_Absyn_UnionType;
_temp459.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp460= _temp444->name; if( _temp460 == 0){ _throw( Null_Exception);} _temp460->v;}));
_temp459.f2= args; _temp459.f3= 0; _temp459;}); _temp458;}); if( _temp444->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp461=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp461->v=( void*) _temp408;
_temp461;});} goto _LL429;} _LL437: t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp462=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp462[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp463; _temp463.tag= Cyc_Absyn_EnumType;
_temp463.f1= _temp446->name; _temp463.f2= 0; _temp463;}); _temp462;}); declopt=({
struct Cyc_Core_Opt* _temp464=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp464->v=( void*) _temp408; _temp464;}); goto _LL429; _LL439:((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp465=( char*)"bad declaration within type specifier";
struct _tagged_string _temp466; _temp466.curr= _temp465; _temp466.base= _temp465;
_temp466.last_plus_one= _temp465 + 38; _temp466;}), _temp408->loc); goto _LL429;
_LL429:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp408->loc);} goto _LL383;
_LL383:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp467=( char*)"missing type withing specifier";
struct _tagged_string _temp468; _temp468.curr= _temp467; _temp468.base= _temp467;
_temp468.last_plus_one= _temp467 + 31; _temp468;}), last_loc);} t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp469=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp469[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp470; _temp470.tag= Cyc_Absyn_IntType; _temp470.f1=( void*) sgn; _temp470.f2=(
void*) sz; _temp470;}); _temp469;});} else{ if( seen_sign){ void* _temp471= t;
void* _temp477; void* _temp479; _LL473: if(( unsigned int) _temp471 > 4u?*(( int*)
_temp471) == Cyc_Absyn_IntType: 0){ _LL480: _temp479=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp471)->f1; goto _LL478; _LL478: _temp477=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp471)->f2; goto _LL474;} else{ goto _LL475;} _LL475: goto _LL476; _LL474: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp481=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp481[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp482; _temp482.tag= Cyc_Absyn_IntType; _temp482.f1=( void*) sgn; _temp482.f2=(
void*) _temp477; _temp482;}); _temp481;}); goto _LL472; _LL476: Cyc_Parse_err((
struct _tagged_string)({ char* _temp483=( char*)"sign specification on non-integral type";
struct _tagged_string _temp484; _temp484.curr= _temp483; _temp484.base= _temp483;
_temp484.last_plus_one= _temp483 + 40; _temp484;}), last_loc); goto _LL472;
_LL472:;} if( seen_size){ void* _temp485= t; void* _temp491; void* _temp493;
_LL487: if(( unsigned int) _temp485 > 4u?*(( int*) _temp485) == Cyc_Absyn_IntType:
0){ _LL494: _temp493=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp485)->f1;
goto _LL492; _LL492: _temp491=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp485)->f2; goto _LL488;} else{ goto _LL489;} _LL489: goto _LL490; _LL488: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp495=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp495[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp496; _temp496.tag= Cyc_Absyn_IntType; _temp496.f1=( void*) _temp493;
_temp496.f2=( void*) sz; _temp496;}); _temp495;}); goto _LL486; _LL490: Cyc_Parse_err((
struct _tagged_string)({ char* _temp497=( char*)"size qualifier on non-integral type";
struct _tagged_string _temp498; _temp498.curr= _temp497; _temp498.base= _temp497;
_temp498.last_plus_one= _temp497 + 36; _temp498;}), last_loc); goto _LL486;
_LL486:;}} return({ struct _tuple5 _temp499; _temp499.f1= t; _temp499.f2=
declopt; _temp499;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* d=( struct
Cyc_Parse_Declarator*)({ struct Cyc_List_List* _temp513= ds; if( _temp513 == 0){
_throw( Null_Exception);} _temp513->hd;}); struct _tuple1* q= d->id; struct Cyc_List_List*
_temp502; struct Cyc_List_List* _temp504; void* _temp506; struct Cyc_Absyn_Tqual
_temp508; struct _tuple6 _temp500= Cyc_Parse_apply_tms( tq, t, shared_atts, d->tms);
_LL509: _temp508= _temp500.f1; goto _LL507; _LL507: _temp506= _temp500.f2; goto
_LL505; _LL505: _temp504= _temp500.f3; goto _LL503; _LL503: _temp502= _temp500.f4;
goto _LL501; _LL501: return({ struct Cyc_List_List* _temp510=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp510->hd=( void*)({ struct
_tuple7* _temp512=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp512->f1= q; _temp512->f2= _temp508; _temp512->f3= _temp506; _temp512->f4=
_temp504; _temp512->f5= _temp502; _temp512;}); _temp510->tl= Cyc_Parse_apply_tmss(
_temp508, t,({ struct Cyc_List_List* _temp511= ds; if( _temp511 == 0){ _throw(
Null_Exception);} _temp511->tl;}), shared_atts); _temp510;});}} static int Cyc_Parse_fn_type_att(
void* a){ void* _temp514= a; int _temp528; _LL516: if(( unsigned int) _temp514 >
15u?*(( int*) _temp514) == Cyc_Absyn_Regparm_att: 0){ _LL529: _temp528=( int)((
struct Cyc_Absyn_Regparm_att_struct*) _temp514)->f1; goto _LL517;} else{ goto
_LL518;} _LL518: if( _temp514 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL519;}
else{ goto _LL520;} _LL520: if( _temp514 ==( void*) Cyc_Absyn_Cdecl_att){ goto
_LL521;} else{ goto _LL522;} _LL522: if( _temp514 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL523;} else{ goto _LL524;} _LL524: if( _temp514 ==( void*) Cyc_Absyn_Const_att){
goto _LL525;} else{ goto _LL526;} _LL526: goto _LL527; _LL517: goto _LL519;
_LL519: goto _LL521; _LL521: goto _LL523; _LL523: goto _LL525; _LL525: return 1;
_LL527: return 0; _LL515:;} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp530; _temp530.f1= tq; _temp530.f2=
t; _temp530.f3= 0; _temp530.f4= atts; _temp530;});}{ void* _temp532=( void*)({
struct Cyc_List_List* _temp531= tms; if( _temp531 == 0){ _throw( Null_Exception);}
_temp531->hd;}); struct Cyc_Absyn_Exp* _temp546; void* _temp548; int _temp550;
struct Cyc_Position_Segment* _temp552; struct Cyc_List_List* _temp554; struct
Cyc_Absyn_Tqual _temp556; void* _temp558; void* _temp560; struct Cyc_List_List*
_temp562; struct Cyc_Position_Segment* _temp564; _LL534: if( _temp532 ==( void*)
Cyc_Absyn_Carray_mod){ goto _LL535;} else{ goto _LL536;} _LL536: if((
unsigned int) _temp532 > 1u?*(( int*) _temp532) == Cyc_Absyn_ConstArray_mod: 0){
_LL547: _temp546=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp532)->f1; goto _LL537;} else{ goto _LL538;} _LL538: if(( unsigned int)
_temp532 > 1u?*(( int*) _temp532) == Cyc_Absyn_Function_mod: 0){ _LL549:
_temp548=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp532)->f1; goto
_LL539;} else{ goto _LL540;} _LL540: if(( unsigned int) _temp532 > 1u?*(( int*)
_temp532) == Cyc_Absyn_TypeParams_mod: 0){ _LL555: _temp554=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp532)->f1; goto _LL553; _LL553:
_temp552=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp532)->f2; goto _LL551; _LL551: _temp550=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp532)->f3; goto _LL541;} else{ goto _LL542;} _LL542: if(( unsigned int)
_temp532 > 1u?*(( int*) _temp532) == Cyc_Absyn_Pointer_mod: 0){ _LL561: _temp560=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp532)->f1; goto _LL559;
_LL559: _temp558=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp532)->f2;
goto _LL557; _LL557: _temp556=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp532)->f3; goto _LL543;} else{ goto _LL544;} _LL544: if(( unsigned int)
_temp532 > 1u?*(( int*) _temp532) == Cyc_Absyn_Attributes_mod: 0){ _LL565:
_temp564=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp532)->f1; goto _LL563; _LL563: _temp562=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp532)->f2; goto _LL545;} else{ goto _LL533;}
_LL535: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp566=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp566[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp567; _temp567.tag= Cyc_Absyn_ArrayType; _temp567.f1=(
void*) t; _temp567.f2= tq; _temp567.f3= 0; _temp567;}); _temp566;}), atts,({
struct Cyc_List_List* _temp568= tms; if( _temp568 == 0){ _throw( Null_Exception);}
_temp568->tl;})); _LL537: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp569=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp569[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp570; _temp570.tag= Cyc_Absyn_ArrayType; _temp570.f1=(
void*) t; _temp570.f2= tq; _temp570.f3=( struct Cyc_Absyn_Exp*) _temp546;
_temp570;}); _temp569;}), atts,({ struct Cyc_List_List* _temp571= tms; if(
_temp571 == 0){ _throw( Null_Exception);} _temp571->tl;})); _LL539: { void*
_temp572= _temp548; struct Cyc_Core_Opt* _temp578; int _temp580; struct Cyc_List_List*
_temp582; struct Cyc_Position_Segment* _temp584; struct Cyc_List_List* _temp586;
_LL574: if(*(( int*) _temp572) == Cyc_Absyn_WithTypes){ _LL583: _temp582=(
struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*) _temp572)->f1; goto
_LL581; _LL581: _temp580=( int)(( struct Cyc_Absyn_WithTypes_struct*) _temp572)->f2;
goto _LL579; _LL579: _temp578=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp572)->f3; goto _LL575;} else{ goto _LL576;} _LL576: if(*(( int*) _temp572)
== Cyc_Absyn_NoTypes){ _LL587: _temp586=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp572)->f1; goto _LL585; _LL585: _temp584=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp572)->f2; goto _LL577;} else{ goto _LL573;}
_LL575: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as=({ struct Cyc_List_List* _temp588= as; if( _temp588 == 0){ _throw(
Null_Exception);} _temp588->tl;})){ if( Cyc_Parse_fn_type_att(( void*)({ struct
Cyc_List_List* _temp589= as; if( _temp589 == 0){ _throw( Null_Exception);}
_temp589->hd;}))){ fn_atts=({ struct Cyc_List_List* _temp590=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp590->hd=( void*)(( void*)({
struct Cyc_List_List* _temp591= as; if( _temp591 == 0){ _throw( Null_Exception);}
_temp591->hd;})); _temp590->tl= fn_atts; _temp590;});} else{ new_atts=({ struct
Cyc_List_List* _temp592=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp592->hd=( void*)(( void*)({ struct Cyc_List_List* _temp593= as; if(
_temp593 == 0){ _throw( Null_Exception);} _temp593->hd;})); _temp592->tl=
new_atts; _temp592;});}}} if(({ struct Cyc_List_List* _temp594= tms; if(
_temp594 == 0){ _throw( Null_Exception);} _temp594->tl;}) != 0){ void* _temp597=(
void*)({ struct Cyc_List_List* _temp596=({ struct Cyc_List_List* _temp595= tms;
if( _temp595 == 0){ _throw( Null_Exception);} _temp595->tl;}); if( _temp596 == 0){
_throw( Null_Exception);} _temp596->hd;}); int _temp603; struct Cyc_Position_Segment*
_temp605; struct Cyc_List_List* _temp607; _LL599: if(( unsigned int) _temp597 >
1u?*(( int*) _temp597) == Cyc_Absyn_TypeParams_mod: 0){ _LL608: _temp607=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp597)->f1;
goto _LL606; _LL606: _temp605=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp597)->f2; goto _LL604; _LL604: _temp603=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp597)->f3; goto _LL600;} else{ goto _LL601;} _LL601: goto _LL602; _LL600:
typvars= _temp607; tms=({ struct Cyc_List_List* _temp609= tms; if( _temp609 == 0){
_throw( Null_Exception);} _temp609->tl;}); goto _LL598; _LL602: goto _LL598;
_LL598:;} if(((( ! _temp580? _temp582 != 0: 0)?({ struct Cyc_List_List* _temp610=
_temp582; if( _temp610 == 0){ _throw( Null_Exception);} _temp610->tl;}) == 0: 0)?(*((
struct _tuple2*)({ struct Cyc_List_List* _temp611= _temp582; if( _temp611 == 0){
_throw( Null_Exception);} _temp611->hd;}))).f1 == 0: 0)?(*(( struct _tuple2*)({
struct Cyc_List_List* _temp612= _temp582; if( _temp612 == 0){ _throw(
Null_Exception);} _temp612->hd;}))).f3 ==( void*) Cyc_Absyn_VoidType: 0){
_temp582= 0; _temp580= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp582); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp578, t, _temp582, _temp580, fn_atts), new_atts,({ struct Cyc_List_List*
_temp613= tms; if( _temp613 == 0){ _throw( Null_Exception);} _temp613->tl;}));}
_LL577:( void) _throw((( void*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp614=( char*)"function declaration without parameter types";
struct _tagged_string _temp615; _temp615.curr= _temp614; _temp615.base= _temp614;
_temp615.last_plus_one= _temp614 + 45; _temp615;}), _temp584)); _LL573:;} _LL541:
if(({ struct Cyc_List_List* _temp616= tms; if( _temp616 == 0){ _throw(
Null_Exception);} _temp616->tl;}) == 0){ return({ struct _tuple6 _temp617;
_temp617.f1= tq; _temp617.f2= t; _temp617.f3= _temp554; _temp617.f4= atts;
_temp617;});}( void) _throw((( void*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp618=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp619; _temp619.curr= _temp618; _temp619.base= _temp618;
_temp619.last_plus_one= _temp618 + 68; _temp619;}), _temp552)); _LL543: { void*
_temp620= _temp560; struct Cyc_Absyn_Exp* _temp628; struct Cyc_Absyn_Exp*
_temp630; _LL622: if(( unsigned int) _temp620 > 1u?*(( int*) _temp620) == Cyc_Absyn_NonNullable_ps:
0){ _LL629: _temp628=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp620)->f1; goto _LL623;} else{ goto _LL624;} _LL624: if(( unsigned int)
_temp620 > 1u?*(( int*) _temp620) == Cyc_Absyn_Nullable_ps: 0){ _LL631: _temp630=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp620)->f1;
goto _LL625;} else{ goto _LL626;} _LL626: if( _temp620 ==( void*) Cyc_Absyn_TaggedArray_ps){
goto _LL627;} else{ goto _LL621;} _LL623: return Cyc_Parse_apply_tms( _temp556,
Cyc_Absyn_atb_typ( t, _temp558, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp632=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp632[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp633; _temp633.tag= Cyc_Absyn_Upper_b;
_temp633.f1= _temp628; _temp633;}); _temp632;})), atts,({ struct Cyc_List_List*
_temp634= tms; if( _temp634 == 0){ _throw( Null_Exception);} _temp634->tl;}));
_LL625: return Cyc_Parse_apply_tms( _temp556, Cyc_Absyn_starb_typ( t, _temp558,
tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp635=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp635[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp636; _temp636.tag= Cyc_Absyn_Upper_b; _temp636.f1= _temp630; _temp636;});
_temp635;})), atts,({ struct Cyc_List_List* _temp637= tms; if( _temp637 == 0){
_throw( Null_Exception);} _temp637->tl;})); _LL627: return Cyc_Parse_apply_tms(
_temp556, Cyc_Absyn_tagged_typ( t, _temp558, tq), atts,({ struct Cyc_List_List*
_temp638= tms; if( _temp638 == 0){ _throw( Null_Exception);} _temp638->tl;}));
_LL621:;} _LL545: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts,
_temp562),({ struct Cyc_List_List* _temp639= tms; if( _temp639 == 0){ _throw(
Null_Exception);} _temp639->tl;})); _LL533:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp642; void* _temp644; struct _tuple5 _temp640= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL645: _temp644= _temp640.f1; goto _LL643; _LL643: _temp642=
_temp640.f2; goto _LL641; _LL641: if( _temp642 != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp646=( char*)"ignoring nested type declaration(s) in specifier list";
struct _tagged_string _temp647; _temp647.curr= _temp646; _temp647.base= _temp646;
_temp647.last_plus_one= _temp646 + 54; _temp647;}), loc);} return _temp644;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp648=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp648[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp649; _temp649.tag= Cyc_Absyn_Decl_s;
_temp649.f1= d; _temp649.f2= s; _temp649;}); _temp648;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp650=( char*)"inline is only allowed on function definitions"; struct
_tagged_string _temp651; _temp651.curr= _temp650; _temp651.base= _temp650;
_temp651.last_plus_one= _temp650 + 47; _temp651;}), loc);} if( tss == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp652=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp653; _temp653.curr= _temp652; _temp653.base= _temp652;
_temp653.last_plus_one= _temp652 + 39; _temp653;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp655=( void*)({ struct Cyc_Core_Opt* _temp654= ds->sc; if(
_temp654 == 0){ _throw( Null_Exception);} _temp654->v;}); _LL657: if( _temp655
==( void*) Cyc_Parse_Typedef_sc){ goto _LL658;} else{ goto _LL659;} _LL659: if(
_temp655 ==( void*) Cyc_Parse_Extern_sc){ goto _LL660;} else{ goto _LL661;}
_LL661: if( _temp655 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL662;} else{ goto
_LL663;} _LL663: if( _temp655 ==( void*) Cyc_Parse_Static_sc){ goto _LL664;}
else{ goto _LL665;} _LL665: if( _temp655 ==( void*) Cyc_Parse_Auto_sc){ goto
_LL666;} else{ goto _LL667;} _LL667: if( _temp655 ==( void*) Cyc_Parse_Register_sc){
goto _LL668;} else{ goto _LL669;} _LL669: if( _temp655 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL670;} else{ goto _LL656;} _LL658: istypedef= 1; goto _LL656; _LL660: s=(
void*) Cyc_Absyn_Extern; goto _LL656; _LL662: s=( void*) Cyc_Absyn_ExternC; goto
_LL656; _LL664: s=( void*) Cyc_Absyn_Static; goto _LL656; _LL666: s=( void*) Cyc_Absyn_Public;
goto _LL656; _LL668: s=( void*) Cyc_Absyn_Public; goto _LL656; _LL670: s=( void*)
Cyc_Absyn_Abstract; goto _LL656; _LL656:;}{ struct Cyc_List_List* _temp673;
struct Cyc_List_List* _temp675; struct _tuple0 _temp671=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL676: _temp675= _temp671.f1;
goto _LL674; _LL674: _temp673= _temp671.f2; goto _LL672; _LL672: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp673; for( 0; es != 0; es=({
struct Cyc_List_List* _temp677= es; if( _temp677 == 0){ _throw( Null_Exception);}
_temp677->tl;})){ if(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp678=
es; if( _temp678 == 0){ _throw( Null_Exception);} _temp678->hd;}) != 0){
exps_empty= 0; break;}}}{ struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, loc); if( _temp675 == 0){ struct Cyc_Core_Opt* _temp681; void* _temp683;
struct _tuple5 _temp679= ts_info; _LL684: _temp683= _temp679.f1; goto _LL682;
_LL682: _temp681= _temp679.f2; goto _LL680; _LL680: if( _temp681 != 0){ struct
Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_Core_Opt* _temp712=
_temp681; if( _temp712 == 0){ _throw( Null_Exception);} _temp712->v;});{ void*
_temp685=( void*) d->r; struct Cyc_Absyn_Enumdecl* _temp697; struct Cyc_Absyn_Structdecl*
_temp699; struct Cyc_Absyn_Uniondecl* _temp701; struct Cyc_Absyn_Tuniondecl*
_temp703; _LL687: if(*(( int*) _temp685) == Cyc_Absyn_Enum_d){ _LL698: _temp697=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp685)->f1;
goto _LL688;} else{ goto _LL689;} _LL689: if(*(( int*) _temp685) == Cyc_Absyn_Struct_d){
_LL700: _temp699=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp685)->f1; goto _LL690;} else{ goto _LL691;} _LL691: if(*(( int*) _temp685)
== Cyc_Absyn_Union_d){ _LL702: _temp701=( struct Cyc_Absyn_Uniondecl*)(( struct
Cyc_Absyn_Union_d_struct*) _temp685)->f1; goto _LL692;} else{ goto _LL693;}
_LL693: if(*(( int*) _temp685) == Cyc_Absyn_Tunion_d){ _LL704: _temp703=( struct
Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp685)->f1; goto
_LL694;} else{ goto _LL695;} _LL695: goto _LL696; _LL688:( void*)( _temp697->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp705=( char*)"bad attributes on enum"; struct _tagged_string _temp706;
_temp706.curr= _temp705; _temp706.base= _temp705; _temp706.last_plus_one=
_temp705 + 23; _temp706;}), loc);} goto _LL686; _LL690:( void*)( _temp699->sc=(
void*) s); _temp699->attributes= atts; goto _LL686; _LL692:( void*)( _temp701->sc=(
void*) s); _temp701->attributes= atts; goto _LL686; _LL694:( void*)( _temp703->sc=(
void*) s); if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp707=( char*)"bad attributes on tunion"; struct _tagged_string _temp708;
_temp708.curr= _temp707; _temp708.base= _temp707; _temp708.last_plus_one=
_temp707 + 25; _temp708;}), loc);} goto _LL686; _LL696: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp709=( char*)"bad declaration"; struct
_tagged_string _temp710; _temp710.curr= _temp709; _temp710.base= _temp709;
_temp710.last_plus_one= _temp709 + 16; _temp710;}), loc); return 0; _LL686:;}
return({ struct Cyc_List_List* _temp711=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp711->hd=( void*) d; _temp711->tl= 0;
_temp711;});} else{ void* _temp713= _temp683; struct Cyc_Absyn_Structdecl**
_temp727; struct Cyc_List_List* _temp729; struct _tuple1* _temp731; struct Cyc_Absyn_TunionInfo
_temp733; void* _temp735; struct Cyc_List_List* _temp737; void* _temp739; struct
Cyc_Absyn_Tuniondecl* _temp741; struct Cyc_Absyn_TunionInfo _temp743; void*
_temp745; struct Cyc_List_List* _temp747; void* _temp749; struct Cyc_Absyn_UnknownTunionInfo
_temp751; int _temp753; struct _tuple1* _temp755; struct Cyc_Absyn_Uniondecl**
_temp757; struct Cyc_List_List* _temp759; struct _tuple1* _temp761; struct Cyc_Absyn_Enumdecl*
_temp763; struct _tuple1* _temp765; _LL715: if(( unsigned int) _temp713 > 4u?*((
int*) _temp713) == Cyc_Absyn_StructType: 0){ _LL732: _temp731=( struct _tuple1*)((
struct Cyc_Absyn_StructType_struct*) _temp713)->f1; goto _LL730; _LL730:
_temp729=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp713)->f2; goto _LL728; _LL728: _temp727=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp713)->f3; goto _LL716;} else{ goto
_LL717;} _LL717: if(( unsigned int) _temp713 > 4u?*(( int*) _temp713) == Cyc_Absyn_TunionType:
0){ _LL734: _temp733=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp713)->f1; _LL740: _temp739=( void*) _temp733.tunion_info; if(*(( int*)
_temp739) == Cyc_Absyn_KnownTunion){ _LL742: _temp741=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp739)->f1; goto _LL738;} else{ goto
_LL719;} _LL738: _temp737=( struct Cyc_List_List*) _temp733.targs; goto _LL736;
_LL736: _temp735=( void*) _temp733.rgn; goto _LL718;} else{ goto _LL719;} _LL719:
if(( unsigned int) _temp713 > 4u?*(( int*) _temp713) == Cyc_Absyn_TunionType: 0){
_LL744: _temp743=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp713)->f1; _LL750: _temp749=( void*) _temp743.tunion_info; if(*(( int*)
_temp749) == Cyc_Absyn_UnknownTunion){ _LL752: _temp751=( struct Cyc_Absyn_UnknownTunionInfo)((
struct Cyc_Absyn_UnknownTunion_struct*) _temp749)->f1; _LL756: _temp755=( struct
_tuple1*) _temp751.name; goto _LL754; _LL754: _temp753=( int) _temp751.is_xtunion;
goto _LL748;} else{ goto _LL721;} _LL748: _temp747=( struct Cyc_List_List*)
_temp743.targs; goto _LL746; _LL746: _temp745=( void*) _temp743.rgn; goto _LL720;}
else{ goto _LL721;} _LL721: if(( unsigned int) _temp713 > 4u?*(( int*) _temp713)
== Cyc_Absyn_UnionType: 0){ _LL762: _temp761=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp713)->f1; goto _LL760; _LL760: _temp759=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp713)->f2; goto _LL758; _LL758: _temp757=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp713)->f3;
goto _LL722;} else{ goto _LL723;} _LL723: if(( unsigned int) _temp713 > 4u?*((
int*) _temp713) == Cyc_Absyn_EnumType: 0){ _LL766: _temp765=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp713)->f1; goto _LL764; _LL764: _temp763=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp713)->f2;
goto _LL724;} else{ goto _LL725;} _LL725: goto _LL726; _LL716: { struct Cyc_List_List*
ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp729); struct Cyc_Absyn_Structdecl* sd=({ struct
Cyc_Absyn_Structdecl* _temp772=( struct Cyc_Absyn_Structdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp772->sc=( void*) s; _temp772->name=({
struct Cyc_Core_Opt* _temp773=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp773->v=( void*)(( struct _tuple1*)({ struct _tuple1*
_temp774= _temp731; if( _temp774 == 0){ _throw( Null_Exception);} _temp774;}));
_temp773;}); _temp772->tvs= ts2; _temp772->fields= 0; _temp772->attributes= 0;
_temp772;}); if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp767=( char*)"bad attributes on struct"; struct _tagged_string _temp768;
_temp768.curr= _temp767; _temp768.base= _temp767; _temp768.last_plus_one=
_temp767 + 25; _temp768;}), loc);} return({ struct Cyc_List_List* _temp769=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp769->hd=(
void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct* _temp770=(
struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp770[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp771; _temp771.tag= Cyc_Absyn_Struct_d;
_temp771.f1= sd; _temp771;}); _temp770;}), loc); _temp769->tl= 0; _temp769;});}
_LL718: if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp775=(
char*)"bad attributes on tunion"; struct _tagged_string _temp776; _temp776.curr=
_temp775; _temp776.base= _temp775; _temp776.last_plus_one= _temp775 + 25;
_temp776;}), loc);} return({ struct Cyc_List_List* _temp777=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp777->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp778=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp778[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp779; _temp779.tag= Cyc_Absyn_Tunion_d; _temp779.f1=
_temp741; _temp779;}); _temp778;}), loc); _temp777->tl= 0; _temp777;}); _LL720: {
struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp747);
struct Cyc_Absyn_Decl* tud= Cyc_Absyn_tunion_decl( s, _temp755, ts2, 0, _temp753,
loc); if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp780=(
char*)"bad attributes on tunion"; struct _tagged_string _temp781; _temp781.curr=
_temp780; _temp781.base= _temp780; _temp781.last_plus_one= _temp780 + 25;
_temp781;}), loc);} return({ struct Cyc_List_List* _temp782=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp782->hd=( void*) tud; _temp782->tl=
0; _temp782;});} _LL722: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp759); struct Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl*
_temp789=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp789->sc=( void*) s; _temp789->name=({ struct Cyc_Core_Opt* _temp790=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp790->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp791= _temp761; if( _temp791 ==
0){ _throw( Null_Exception);} _temp791;})); _temp790;}); _temp789->tvs= ts2;
_temp789->fields= 0; _temp789->attributes= 0; _temp789;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp783=( char*)"bad attributes on union";
struct _tagged_string _temp784; _temp784.curr= _temp783; _temp784.base= _temp783;
_temp784.last_plus_one= _temp783 + 24; _temp784;}), loc);} return({ struct Cyc_List_List*
_temp785=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp785->hd=( void*)({ struct Cyc_Absyn_Decl* _temp786=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp786->r=( void*)(( void*)({
struct Cyc_Absyn_Union_d_struct* _temp787=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp787[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp788; _temp788.tag= Cyc_Absyn_Union_d; _temp788.f1= ud; _temp788;});
_temp787;})); _temp786->loc= loc; _temp786;}); _temp785->tl= 0; _temp785;});}
_LL724: { struct Cyc_Absyn_Enumdecl* ed=({ struct Cyc_Absyn_Enumdecl* _temp798=(
struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp798->sc=( void*) s; _temp798->name= _temp765; _temp798->fields= 0; _temp798;});
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp792=( char*)"bad attributes on enum";
struct _tagged_string _temp793; _temp793.curr= _temp792; _temp793.base= _temp792;
_temp793.last_plus_one= _temp792 + 23; _temp793;}), loc);} return({ struct Cyc_List_List*
_temp794=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp794->hd=( void*)({ struct Cyc_Absyn_Decl* _temp795=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp795->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp796=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp796[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp797; _temp797.tag= Cyc_Absyn_Enum_d; _temp797.f1= ed; _temp797;}); _temp796;}));
_temp795->loc= loc; _temp795;}); _temp794->tl= 0; _temp794;});} _LL726: Cyc_Parse_err((
struct _tagged_string)({ char* _temp799=( char*)"missing declarator"; struct
_tagged_string _temp800; _temp800.curr= _temp799; _temp800.base= _temp799;
_temp800.last_plus_one= _temp799 + 19; _temp800;}), loc); return 0; _LL714:;}}
else{ void* t= ts_info.f1; struct Cyc_List_List* fields= Cyc_Parse_apply_tmss(
tq, t, _temp675, atts); if( istypedef){ if( ! exps_empty){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp801=( char*)"initializer in typedef declaration";
struct _tagged_string _temp802; _temp802.curr= _temp801; _temp802.base= _temp801;
_temp802.last_plus_one= _temp801 + 35; _temp802;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, fields); if( ts_info.f2 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_Core_Opt*
_temp826= ts_info.f2; if( _temp826 == 0){ _throw( Null_Exception);} _temp826->v;});{
void* _temp803=( void*) d->r; struct Cyc_Absyn_Structdecl* _temp815; struct Cyc_Absyn_Tuniondecl*
_temp817; struct Cyc_Absyn_Uniondecl* _temp819; struct Cyc_Absyn_Enumdecl*
_temp821; _LL805: if(*(( int*) _temp803) == Cyc_Absyn_Struct_d){ _LL816:
_temp815=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp803)->f1; goto _LL806;} else{ goto _LL807;} _LL807: if(*(( int*) _temp803)
== Cyc_Absyn_Tunion_d){ _LL818: _temp817=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp803)->f1; goto _LL808;} else{ goto
_LL809;} _LL809: if(*(( int*) _temp803) == Cyc_Absyn_Union_d){ _LL820: _temp819=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp803)->f1;
goto _LL810;} else{ goto _LL811;} _LL811: if(*(( int*) _temp803) == Cyc_Absyn_Enum_d){
_LL822: _temp821=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp803)->f1; goto _LL812;} else{ goto _LL813;} _LL813: goto _LL814; _LL806:(
void*)( _temp815->sc=( void*) s); _temp815->attributes= atts; atts= 0; goto
_LL804; _LL808:( void*)( _temp817->sc=( void*) s); goto _LL804; _LL810:( void*)(
_temp819->sc=( void*) s); goto _LL804; _LL812:( void*)( _temp821->sc=( void*) s);
goto _LL804; _LL814: Cyc_Parse_err(( struct _tagged_string)({ char* _temp823=(
char*)"declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp824; _temp824.curr= _temp823; _temp824.base= _temp823;
_temp824.last_plus_one= _temp823 + 70; _temp824;}), loc); goto _LL804; _LL804:;}
decls=({ struct Cyc_List_List* _temp825=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp825->hd=( void*) d; _temp825->tl= decls;
_temp825;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp828=
Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp827= atts; if(
_temp827 == 0){ _throw( Null_Exception);} _temp827->hd;})); xprintf("bad attribute %.*s in typedef",
_temp828.last_plus_one - _temp828.curr, _temp828.curr);}), loc);} return decls;}}
else{ if( ts_info.f2 != 0){ Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp829=( char*)"nested type declaration within declarator"; struct
_tagged_string _temp830; _temp830.curr= _temp829; _temp830.base= _temp829;
_temp830.last_plus_one= _temp829 + 42; _temp830;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* ds= fields; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp831= ds; if( _temp831 == 0){ _throw( Null_Exception);} _temp831->tl;}),
_temp673=({ struct Cyc_List_List* _temp832= _temp673; if( _temp832 == 0){ _throw(
Null_Exception);} _temp832->tl;})){ struct _tuple7 _temp836; struct Cyc_List_List*
_temp837; struct Cyc_List_List* _temp839; void* _temp841; struct Cyc_Absyn_Tqual
_temp843; struct _tuple1* _temp845; struct _tuple7* _temp834=( struct _tuple7*)({
struct Cyc_List_List* _temp833= ds; if( _temp833 == 0){ _throw( Null_Exception);}
_temp833->hd;}); _temp836=* _temp834; _LL846: _temp845= _temp836.f1; goto _LL844;
_LL844: _temp843= _temp836.f2; goto _LL842; _LL842: _temp841= _temp836.f3; goto
_LL840; _LL840: _temp839= _temp836.f4; goto _LL838; _LL838: _temp837= _temp836.f5;
goto _LL835; _LL835: if( _temp839 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp847=( char*)"bad type params, ignoring"; struct _tagged_string
_temp848; _temp848.curr= _temp847; _temp848.base= _temp847; _temp848.last_plus_one=
_temp847 + 26; _temp848;}), loc);} if( _temp673 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp849=( char*)"unexpected null in parse!"; struct
_tagged_string _temp850; _temp850.curr= _temp849; _temp850.base= _temp849;
_temp850.last_plus_one= _temp849 + 26; _temp850;}), loc);}{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp855= _temp673; if(
_temp855 == 0){ _throw( Null_Exception);} _temp855->hd;}); struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_new_vardecl( _temp845, _temp841, eopt); vd->tq= _temp843;( void*)(
vd->sc=( void*) s); vd->attributes= _temp837;{ struct Cyc_Absyn_Decl* d=({
struct Cyc_Absyn_Decl* _temp852=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof(
struct Cyc_Absyn_Decl)); _temp852->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp853=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp853[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp854; _temp854.tag= Cyc_Absyn_Var_d;
_temp854.f1= vd; _temp854;}); _temp853;})); _temp852->loc= loc; _temp852;});
decls=({ struct Cyc_List_List* _temp851=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp851->hd=( void*) d; _temp851->tl= decls;
_temp851;});}}}} return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind( struct
_tagged_string s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen( s)
!= 1){ Cyc_Parse_err(({ struct _tagged_string _temp856= s; xprintf("bad kind: %.*s",
_temp856.last_plus_one - _temp856.curr, _temp856.curr);}), loc); return( void*)
Cyc_Absyn_BoxKind;} else{ switch(({ struct _tagged_string _temp857= s; char*
_temp859= _temp857.curr + 0; if( _temp857.base == 0? 1:( _temp859 < _temp857.base?
1: _temp859 >= _temp857.last_plus_one)){ _throw( Null_Exception);}* _temp859;})){
case 'A': _LL860: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL861: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL862: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL863: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL864: return(
void*) Cyc_Absyn_EffKind; default: _LL865: Cyc_Parse_err(({ struct
_tagged_string _temp867= s; xprintf("bad kind: %.*s", _temp867.last_plus_one -
_temp867.curr, _temp867.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp868=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp868->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp869=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp869[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp870; _temp870.tag= Cyc_Absyn_Attributes_mod;
_temp870.f1= loc; _temp870.f2= atts; _temp870;}); _temp869;})); _temp868->tl=
tms; _temp868;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(({ struct
_tagged_string _temp872= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp871=(* t).f5; if( _temp871 == 0){ _throw( Null_Exception);} _temp871->hd;}));
xprintf("bad attribute %.*s within typedef", _temp872.last_plus_one - _temp872.curr,
_temp872.curr);}), loc);} return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp873=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp873[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp874; _temp874.tag= Cyc_Absyn_Typedef_d;
_temp874.f1=({ struct Cyc_Absyn_Typedefdecl* _temp875=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp875->name= x; _temp875->tvs=(*
t).f4; _temp875->defn=( void*)(* t).f3; _temp875;}); _temp874;}); _temp873;}),
loc);} char Cyc_Okay_tok[ 13u]="\000\000\000\000Okay_tok"; char Cyc_Bool_tok[ 13u]="\000\000\000\000Bool_tok";
struct Cyc_Bool_tok_struct{ char* tag; int f1; } ; char Cyc_Int_tok[ 12u]="\000\000\000\000Int_tok";
struct _tuple12{ void* f1; int f2; } ; struct Cyc_Int_tok_struct{ char* tag;
struct _tuple12* f1; } ; char Cyc_Char_tok[ 13u]="\000\000\000\000Char_tok";
struct Cyc_Char_tok_struct{ char* tag; char f1; } ; char Cyc_Pointer_Sort_tok[
21u]="\000\000\000\000Pointer_Sort_tok"; struct Cyc_Pointer_Sort_tok_struct{
char* tag; void* f1; } ; char Cyc_Short_tok[ 14u]="\000\000\000\000Short_tok";
struct Cyc_Short_tok_struct{ char* tag; short f1; } ; char Cyc_String_tok[ 15u]="\000\000\000\000String_tok";
struct Cyc_String_tok_struct{ char* tag; struct _tagged_string f1; } ; char Cyc_Stringopt_tok[
18u]="\000\000\000\000Stringopt_tok"; struct Cyc_Stringopt_tok_struct{ char* tag;
struct Cyc_Core_Opt* f1; } ; char Cyc_Type_tok[ 13u]="\000\000\000\000Type_tok";
struct Cyc_Type_tok_struct{ char* tag; void* f1; } ; char Cyc_TypeList_tok[ 17u]="\000\000\000\000TypeList_tok";
struct Cyc_TypeList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char
Cyc_Exp_tok[ 12u]="\000\000\000\000Exp_tok"; struct Cyc_Exp_tok_struct{ char*
tag; struct Cyc_Absyn_Exp* f1; } ; char Cyc_ExpList_tok[ 16u]="\000\000\000\000ExpList_tok";
struct Cyc_ExpList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char Cyc_Primop_tok[
15u]="\000\000\000\000Primop_tok"; struct Cyc_Primop_tok_struct{ char* tag; void*
f1; } ; char Cyc_Primopopt_tok[ 18u]="\000\000\000\000Primopopt_tok"; struct Cyc_Primopopt_tok_struct{
char* tag; struct Cyc_Core_Opt* f1; } ; char Cyc_QualId_tok[ 15u]="\000\000\000\000QualId_tok";
struct Cyc_QualId_tok_struct{ char* tag; struct _tuple1* f1; } ; char Cyc_Stmt_tok[
13u]="\000\000\000\000Stmt_tok"; struct Cyc_Stmt_tok_struct{ char* tag; struct
Cyc_Absyn_Stmt* f1; } ; char Cyc_SwitchClauseList_tok[ 25u]="\000\000\000\000SwitchClauseList_tok";
struct Cyc_SwitchClauseList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_Pattern_tok[ 16u]="\000\000\000\000Pattern_tok"; struct Cyc_Pattern_tok_struct{
char* tag; struct Cyc_Absyn_Pat* f1; } ; char Cyc_PatternList_tok[ 20u]="\000\000\000\000PatternList_tok";
struct Cyc_PatternList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char
Cyc_FnDecl_tok[ 15u]="\000\000\000\000FnDecl_tok"; struct Cyc_FnDecl_tok_struct{
char* tag; struct Cyc_Absyn_Fndecl* f1; } ; char Cyc_DeclList_tok[ 17u]="\000\000\000\000DeclList_tok";
struct Cyc_DeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char
Cyc_DeclSpec_tok[ 17u]="\000\000\000\000DeclSpec_tok"; struct Cyc_DeclSpec_tok_struct{
char* tag; struct Cyc_Parse_Declaration_spec* f1; } ; char Cyc_InitDecl_tok[ 17u]="\000\000\000\000InitDecl_tok";
struct _tuple13{ struct Cyc_Parse_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_InitDecl_tok_struct{ char* tag; struct _tuple13* f1; } ; char Cyc_InitDeclList_tok[
21u]="\000\000\000\000InitDeclList_tok"; struct Cyc_InitDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_StorageClass_tok[ 21u]="\000\000\000\000StorageClass_tok";
struct Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; char Cyc_TypeSpecifier_tok[
22u]="\000\000\000\000TypeSpecifier_tok"; struct Cyc_TypeSpecifier_tok_struct{
char* tag; void* f1; } ; char Cyc_QualSpecList_tok[ 21u]="\000\000\000\000QualSpecList_tok";
struct _tuple14{ struct Cyc_Absyn_Tqual f1; struct Cyc_List_List* f2; struct Cyc_List_List*
f3; } ; struct Cyc_QualSpecList_tok_struct{ char* tag; struct _tuple14* f1; } ;
char Cyc_TypeQual_tok[ 17u]="\000\000\000\000TypeQual_tok"; struct Cyc_TypeQual_tok_struct{
char* tag; struct Cyc_Absyn_Tqual f1; } ; char Cyc_StructFieldDeclList_tok[ 28u]="\000\000\000\000StructFieldDeclList_tok";
struct Cyc_StructFieldDeclList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; char Cyc_StructFieldDeclListList_tok[ 32u]="\000\000\000\000StructFieldDeclListList_tok";
struct Cyc_StructFieldDeclListList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; char Cyc_Declarator_tok[ 19u]="\000\000\000\000Declarator_tok"; struct
Cyc_Declarator_tok_struct{ char* tag; struct Cyc_Parse_Declarator* f1; } ; char
Cyc_DeclaratorExpopt_tok[ 25u]="\000\000\000\000DeclaratorExpopt_tok"; struct
_tuple15{ struct Cyc_Parse_Declarator* f1; struct Cyc_Core_Opt* f2; } ; struct
Cyc_DeclaratorExpopt_tok_struct{ char* tag; struct _tuple15* f1; } ; char Cyc_DeclaratorExpoptList_tok[
29u]="\000\000\000\000DeclaratorExpoptList_tok"; struct Cyc_DeclaratorExpoptList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
struct Cyc_AbstractDeclarator_tok_struct{ char* tag; struct Cyc_Parse_Abstractdeclarator*
f1; } ; char Cyc_TunionField_tok[ 20u]="\000\000\000\000TunionField_tok"; struct
Cyc_TunionField_tok_struct{ char* tag; struct Cyc_Absyn_Tunionfield* f1; } ;
char Cyc_TunionFieldList_tok[ 24u]="\000\000\000\000TunionFieldList_tok"; struct
Cyc_TunionFieldList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char
Cyc_ParamDecl_tok[ 18u]="\000\000\000\000ParamDecl_tok"; struct Cyc_ParamDecl_tok_struct{
char* tag; struct _tuple2* f1; } ; char Cyc_ParamDeclList_tok[ 22u]="\000\000\000\000ParamDeclList_tok";
struct Cyc_ParamDeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_ParamDeclListBool_tok[ 26u]="\000\000\000\000ParamDeclListBool_tok";
struct _tuple16{ struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ;
struct Cyc_ParamDeclListBool_tok_struct{ char* tag; struct _tuple16* f1; } ;
char Cyc_StructOrUnion_tok[ 22u]="\000\000\000\000StructOrUnion_tok"; struct Cyc_StructOrUnion_tok_struct{
char* tag; void* f1; } ; char Cyc_IdList_tok[ 15u]="\000\000\000\000IdList_tok";
struct Cyc_IdList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char Cyc_Designator_tok[
19u]="\000\000\000\000Designator_tok"; struct Cyc_Designator_tok_struct{ char*
tag; void* f1; } ; char Cyc_DesignatorList_tok[ 23u]="\000\000\000\000DesignatorList_tok";
struct Cyc_DesignatorList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_TypeModifierList_tok[ 25u]="\000\000\000\000TypeModifierList_tok";
struct Cyc_TypeModifierList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_Rgn_tok[ 12u]="\000\000\000\000Rgn_tok"; struct Cyc_Rgn_tok_struct{
char* tag; void* f1; } ; char Cyc_InitializerList_tok[ 24u]="\000\000\000\000InitializerList_tok";
struct Cyc_InitializerList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_FieldPattern_tok[ 21u]="\000\000\000\000FieldPattern_tok"; struct
_tuple17{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
char* tag; struct _tuple17* f1; } ; char Cyc_FieldPatternList_tok[ 25u]="\000\000\000\000FieldPatternList_tok";
struct Cyc_FieldPatternList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_BlockItem_tok[ 18u]="\000\000\000\000BlockItem_tok"; struct Cyc_BlockItem_tok_struct{
char* tag; void* f1; } ; char Cyc_Kind_tok[ 13u]="\000\000\000\000Kind_tok";
struct Cyc_Kind_tok_struct{ char* tag; void* f1; } ; char Cyc_Attribute_tok[ 18u]="\000\000\000\000Attribute_tok";
struct Cyc_Attribute_tok_struct{ char* tag; void* f1; } ; char Cyc_AttributeList_tok[
22u]="\000\000\000\000AttributeList_tok"; struct Cyc_AttributeList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_Enumfield_tok[ 18u]="\000\000\000\000Enumfield_tok";
struct Cyc_Enumfield_tok_struct{ char* tag; struct Cyc_Absyn_Enumfield* f1; } ;
char Cyc_EnumfieldList_tok[ 22u]="\000\000\000\000EnumfieldList_tok"; struct Cyc_EnumfieldList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_Scope_tok[ 14u]="\000\000\000\000Scope_tok";
struct Cyc_Scope_tok_struct{ char* tag; void* f1; } ; static char _temp879[ 8u]="Int_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp879, _temp879, _temp879 + 8u}}; struct _tuple12*
Cyc_yyget_Int_tok( void* yy1){ void* _temp880= yy1; struct _tuple12* _temp886;
_LL882: if(*(( void**) _temp880) == Cyc_Int_tok){ _LL887: _temp886=( struct
_tuple12*)(( struct Cyc_Int_tok_struct*) _temp880)->f1; goto _LL883;} else{ goto
_LL884;} _LL884: goto _LL885; _LL883: return _temp886; _LL885:( void) _throw((
void*)& Cyc_yyfail_Int_tok); _LL881:;} static char _temp891[ 11u]="String_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp891, _temp891, _temp891 + 11u}}; struct
_tagged_string Cyc_yyget_String_tok( void* yy1){ void* _temp892= yy1; struct
_tagged_string _temp898; _LL894: if(*(( void**) _temp892) == Cyc_String_tok){
_LL899: _temp898=( struct _tagged_string)(( struct Cyc_String_tok_struct*)
_temp892)->f1; goto _LL895;} else{ goto _LL896;} _LL896: goto _LL897; _LL895:
return _temp898; _LL897:( void) _throw(( void*)& Cyc_yyfail_String_tok); _LL893:;}
static char _temp903[ 9u]="Char_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp903, _temp903, _temp903 + 9u}};
char Cyc_yyget_Char_tok( void* yy1){ void* _temp904= yy1; char _temp910; _LL906:
if(*(( void**) _temp904) == Cyc_Char_tok){ _LL911: _temp910=( char)(( struct Cyc_Char_tok_struct*)
_temp904)->f1; goto _LL907;} else{ goto _LL908;} _LL908: goto _LL909; _LL907:
return _temp910; _LL909:( void) _throw(( void*)& Cyc_yyfail_Char_tok); _LL905:;}
static char _temp915[ 17u]="Pointer_Sort_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp915, _temp915, _temp915 + 17u}}; void* Cyc_yyget_Pointer_Sort_tok( void*
yy1){ void* _temp916= yy1; void* _temp922; _LL918: if(*(( void**) _temp916) ==
Cyc_Pointer_Sort_tok){ _LL923: _temp922=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp916)->f1; goto _LL919;} else{ goto _LL920;} _LL920: goto _LL921; _LL919:
return _temp922; _LL921:( void) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL917:;} static char _temp927[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp927,
_temp927, _temp927 + 8u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
void* _temp928= yy1; struct Cyc_Absyn_Exp* _temp934; _LL930: if(*(( void**)
_temp928) == Cyc_Exp_tok){ _LL935: _temp934=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Exp_tok_struct*) _temp928)->f1; goto _LL931;} else{ goto _LL932;} _LL932:
goto _LL933; _LL931: return _temp934; _LL933:( void) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL929:;} static char _temp939[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp939,
_temp939, _temp939 + 12u}}; struct Cyc_List_List* Cyc_yyget_ExpList_tok( void*
yy1){ void* _temp940= yy1; struct Cyc_List_List* _temp946; _LL942: if(*(( void**)
_temp940) == Cyc_ExpList_tok){ _LL947: _temp946=( struct Cyc_List_List*)((
struct Cyc_ExpList_tok_struct*) _temp940)->f1; goto _LL943;} else{ goto _LL944;}
_LL944: goto _LL945; _LL943: return _temp946; _LL945:( void) _throw(( void*)&
Cyc_yyfail_ExpList_tok); _LL941:;} static char _temp951[ 20u]="InitializerList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp951, _temp951, _temp951 + 20u}}; struct Cyc_List_List*
Cyc_yyget_InitializerList_tok( void* yy1){ void* _temp952= yy1; struct Cyc_List_List*
_temp958; _LL954: if(*(( void**) _temp952) == Cyc_InitializerList_tok){ _LL959:
_temp958=( struct Cyc_List_List*)(( struct Cyc_InitializerList_tok_struct*)
_temp952)->f1; goto _LL955;} else{ goto _LL956;} _LL956: goto _LL957; _LL955:
return _temp958; _LL957:( void) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL953:;} static char _temp963[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp963,
_temp963, _temp963 + 11u}}; void* Cyc_yyget_Primop_tok( void* yy1){ void*
_temp964= yy1; void* _temp970; _LL966: if(*(( void**) _temp964) == Cyc_Primop_tok){
_LL971: _temp970=( void*)(( struct Cyc_Primop_tok_struct*) _temp964)->f1; goto
_LL967;} else{ goto _LL968;} _LL968: goto _LL969; _LL967: return _temp970;
_LL969:( void) _throw(( void*)& Cyc_yyfail_Primop_tok); _LL965:;} static char
_temp975[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp975, _temp975, _temp975 + 14u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ void* _temp976= yy1;
struct Cyc_Core_Opt* _temp982; _LL978: if(*(( void**) _temp976) == Cyc_Primopopt_tok){
_LL983: _temp982=( struct Cyc_Core_Opt*)(( struct Cyc_Primopopt_tok_struct*)
_temp976)->f1; goto _LL979;} else{ goto _LL980;} _LL980: goto _LL981; _LL979:
return _temp982; _LL981:( void) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL977:;} static char _temp987[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp987,
_temp987, _temp987 + 11u}}; struct _tuple1* Cyc_yyget_QualId_tok( void* yy1){
void* _temp988= yy1; struct _tuple1* _temp994; _LL990: if(*(( void**) _temp988)
== Cyc_QualId_tok){ _LL995: _temp994=( struct _tuple1*)(( struct Cyc_QualId_tok_struct*)
_temp988)->f1; goto _LL991;} else{ goto _LL992;} _LL992: goto _LL993; _LL991:
return _temp994; _LL993:( void) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL989:;}
static char _temp999[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp999, _temp999, _temp999 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp1000= yy1;
struct Cyc_Absyn_Stmt* _temp1006; _LL1002: if(*(( void**) _temp1000) == Cyc_Stmt_tok){
_LL1007: _temp1006=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Stmt_tok_struct*)
_temp1000)->f1; goto _LL1003;} else{ goto _LL1004;} _LL1004: goto _LL1005;
_LL1003: return _temp1006; _LL1005:( void) _throw(( void*)& Cyc_yyfail_Stmt_tok);
_LL1001:;} static char _temp1011[ 14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_BlockItem_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1011,
_temp1011, _temp1011 + 14u}}; void* Cyc_yyget_BlockItem_tok( void* yy1){ void*
_temp1012= yy1; void* _temp1018; _LL1014: if(*(( void**) _temp1012) == Cyc_BlockItem_tok){
_LL1019: _temp1018=( void*)(( struct Cyc_BlockItem_tok_struct*) _temp1012)->f1;
goto _LL1015;} else{ goto _LL1016;} _LL1016: goto _LL1017; _LL1015: return
_temp1018; _LL1017:( void) _throw(( void*)& Cyc_yyfail_BlockItem_tok); _LL1013:;}
static char _temp1023[ 21u]="SwitchClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1023, _temp1023, _temp1023 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
void* yy1){ void* _temp1024= yy1; struct Cyc_List_List* _temp1030; _LL1026: if(*((
void**) _temp1024) == Cyc_SwitchClauseList_tok){ _LL1031: _temp1030=( struct Cyc_List_List*)((
struct Cyc_SwitchClauseList_tok_struct*) _temp1024)->f1; goto _LL1027;} else{
goto _LL1028;} _LL1028: goto _LL1029; _LL1027: return _temp1030; _LL1029:( void)
_throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL1025:;} static char
_temp1035[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1035, _temp1035, _temp1035 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp1036= yy1;
struct Cyc_Absyn_Pat* _temp1042; _LL1038: if(*(( void**) _temp1036) == Cyc_Pattern_tok){
_LL1043: _temp1042=( struct Cyc_Absyn_Pat*)(( struct Cyc_Pattern_tok_struct*)
_temp1036)->f1; goto _LL1039;} else{ goto _LL1040;} _LL1040: goto _LL1041;
_LL1039: return _temp1042; _LL1041:( void) _throw(( void*)& Cyc_yyfail_Pattern_tok);
_LL1037:;} static char _temp1047[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1047, _temp1047, _temp1047 + 16u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok(
void* yy1){ void* _temp1048= yy1; struct Cyc_List_List* _temp1054; _LL1050: if(*((
void**) _temp1048) == Cyc_PatternList_tok){ _LL1055: _temp1054=( struct Cyc_List_List*)((
struct Cyc_PatternList_tok_struct*) _temp1048)->f1; goto _LL1051;} else{ goto
_LL1052;} _LL1052: goto _LL1053; _LL1051: return _temp1054; _LL1053:( void)
_throw(( void*)& Cyc_yyfail_PatternList_tok); _LL1049:;} static char _temp1059[
17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1059, _temp1059, _temp1059 + 17u}};
struct _tuple17* Cyc_yyget_FieldPattern_tok( void* yy1){ void* _temp1060= yy1;
struct _tuple17* _temp1066; _LL1062: if(*(( void**) _temp1060) == Cyc_FieldPattern_tok){
_LL1067: _temp1066=( struct _tuple17*)(( struct Cyc_FieldPattern_tok_struct*)
_temp1060)->f1; goto _LL1063;} else{ goto _LL1064;} _LL1064: goto _LL1065;
_LL1063: return _temp1066; _LL1065:( void) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL1061:;} static char _temp1071[ 21u]="FieldPatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1071, _temp1071, _temp1071 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp1072= yy1; struct Cyc_List_List* _temp1078; _LL1074: if(*((
void**) _temp1072) == Cyc_FieldPatternList_tok){ _LL1079: _temp1078=( struct Cyc_List_List*)((
struct Cyc_FieldPatternList_tok_struct*) _temp1072)->f1; goto _LL1075;} else{
goto _LL1076;} _LL1076: goto _LL1077; _LL1075: return _temp1078; _LL1077:( void)
_throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL1073:;} static char
_temp1083[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FnDecl_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1083, _temp1083, _temp1083 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp1084= yy1;
struct Cyc_Absyn_Fndecl* _temp1090; _LL1086: if(*(( void**) _temp1084) == Cyc_FnDecl_tok){
_LL1091: _temp1090=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_FnDecl_tok_struct*)
_temp1084)->f1; goto _LL1087;} else{ goto _LL1088;} _LL1088: goto _LL1089;
_LL1087: return _temp1090; _LL1089:( void) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL1085:;} static char _temp1095[ 13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1095,
_temp1095, _temp1095 + 13u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok( void*
yy1){ void* _temp1096= yy1; struct Cyc_List_List* _temp1102; _LL1098: if(*((
void**) _temp1096) == Cyc_DeclList_tok){ _LL1103: _temp1102=( struct Cyc_List_List*)((
struct Cyc_DeclList_tok_struct*) _temp1096)->f1; goto _LL1099;} else{ goto
_LL1100;} _LL1100: goto _LL1101; _LL1099: return _temp1102; _LL1101:( void)
_throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1097:;} static char _temp1107[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1107, _temp1107, _temp1107 + 13u}}; struct Cyc_Parse_Declaration_spec*
Cyc_yyget_DeclSpec_tok( void* yy1){ void* _temp1108= yy1; struct Cyc_Parse_Declaration_spec*
_temp1114; _LL1110: if(*(( void**) _temp1108) == Cyc_DeclSpec_tok){ _LL1115:
_temp1114=( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_DeclSpec_tok_struct*)
_temp1108)->f1; goto _LL1111;} else{ goto _LL1112;} _LL1112: goto _LL1113;
_LL1111: return _temp1114; _LL1113:( void) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1109:;} static char _temp1119[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1119,
_temp1119, _temp1119 + 13u}}; struct _tuple13* Cyc_yyget_InitDecl_tok( void* yy1){
void* _temp1120= yy1; struct _tuple13* _temp1126; _LL1122: if(*(( void**)
_temp1120) == Cyc_InitDecl_tok){ _LL1127: _temp1126=( struct _tuple13*)(( struct
Cyc_InitDecl_tok_struct*) _temp1120)->f1; goto _LL1123;} else{ goto _LL1124;}
_LL1124: goto _LL1125; _LL1123: return _temp1126; _LL1125:( void) _throw(( void*)&
Cyc_yyfail_InitDecl_tok); _LL1121:;} static char _temp1131[ 17u]="InitDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1131, _temp1131, _temp1131 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( void* yy1){ void* _temp1132= yy1; struct Cyc_List_List*
_temp1138; _LL1134: if(*(( void**) _temp1132) == Cyc_InitDeclList_tok){ _LL1139:
_temp1138=( struct Cyc_List_List*)(( struct Cyc_InitDeclList_tok_struct*)
_temp1132)->f1; goto _LL1135;} else{ goto _LL1136;} _LL1136: goto _LL1137;
_LL1135: return _temp1138; _LL1137:( void) _throw(( void*)& Cyc_yyfail_InitDeclList_tok);
_LL1133:;} static char _temp1143[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1143, _temp1143, _temp1143 + 17u}}; void* Cyc_yyget_StorageClass_tok( void*
yy1){ void* _temp1144= yy1; void* _temp1150; _LL1146: if(*(( void**) _temp1144)
== Cyc_StorageClass_tok){ _LL1151: _temp1150=( void*)(( struct Cyc_StorageClass_tok_struct*)
_temp1144)->f1; goto _LL1147;} else{ goto _LL1148;} _LL1148: goto _LL1149;
_LL1147: return _temp1150; _LL1149:( void) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL1145:;} static char _temp1155[ 18u]="TypeSpecifier_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1155, _temp1155, _temp1155 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* _temp1156= yy1; void* _temp1162; _LL1158: if(*(( void**) _temp1156)
== Cyc_TypeSpecifier_tok){ _LL1163: _temp1162=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp1156)->f1; goto _LL1159;} else{ goto _LL1160;} _LL1160: goto _LL1161;
_LL1159: return _temp1162; _LL1161:( void) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1157:;} static char _temp1167[ 18u]="StructOrUnion_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1167, _temp1167, _temp1167 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void*
yy1){ void* _temp1168= yy1; void* _temp1174; _LL1170: if(*(( void**) _temp1168)
== Cyc_StructOrUnion_tok){ _LL1175: _temp1174=( void*)(( struct Cyc_StructOrUnion_tok_struct*)
_temp1168)->f1; goto _LL1171;} else{ goto _LL1172;} _LL1172: goto _LL1173;
_LL1171: return _temp1174; _LL1173:( void) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL1169:;} static char _temp1179[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1179,
_temp1179, _temp1179 + 13u}}; struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(
void* yy1){ void* _temp1180= yy1; struct Cyc_Absyn_Tqual _temp1186; _LL1182: if(*((
void**) _temp1180) == Cyc_TypeQual_tok){ _LL1187: _temp1186=( struct Cyc_Absyn_Tqual)((
struct Cyc_TypeQual_tok_struct*) _temp1180)->f1; goto _LL1183;} else{ goto
_LL1184;} _LL1184: goto _LL1185; _LL1183: return _temp1186; _LL1185:( void)
_throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL1181:;} static char _temp1191[ 24u]="StructFieldDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1191, _temp1191, _temp1191 + 24u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclList_tok( void* yy1){ void* _temp1192= yy1; struct Cyc_List_List*
_temp1198; _LL1194: if(*(( void**) _temp1192) == Cyc_StructFieldDeclList_tok){
_LL1199: _temp1198=( struct Cyc_List_List*)(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp1192)->f1; goto _LL1195;} else{ goto _LL1196;} _LL1196: goto _LL1197;
_LL1195: return _temp1198; _LL1197:( void) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL1193:;} static char _temp1203[ 28u]="StructFieldDeclListList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1203, _temp1203, _temp1203 + 28u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ void* _temp1204= yy1; struct
Cyc_List_List* _temp1210; _LL1206: if(*(( void**) _temp1204) == Cyc_StructFieldDeclListList_tok){
_LL1211: _temp1210=( struct Cyc_List_List*)(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp1204)->f1; goto _LL1207;} else{ goto _LL1208;} _LL1208: goto _LL1209;
_LL1207: return _temp1210; _LL1209:( void) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL1205:;} static char _temp1215[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1215, _temp1215, _temp1215 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ void* _temp1216= yy1; struct Cyc_List_List* _temp1222; _LL1218: if(*((
void**) _temp1216) == Cyc_TypeModifierList_tok){ _LL1223: _temp1222=( struct Cyc_List_List*)((
struct Cyc_TypeModifierList_tok_struct*) _temp1216)->f1; goto _LL1219;} else{
goto _LL1220;} _LL1220: goto _LL1221; _LL1219: return _temp1222; _LL1221:( void)
_throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL1217:;} static char
_temp1227[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgn_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1227, _temp1227, _temp1227 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp1228= yy1; void* _temp1234;
_LL1230: if(*(( void**) _temp1228) == Cyc_Rgn_tok){ _LL1235: _temp1234=( void*)((
struct Cyc_Rgn_tok_struct*) _temp1228)->f1; goto _LL1231;} else{ goto _LL1232;}
_LL1232: goto _LL1233; _LL1231: return _temp1234; _LL1233:( void) _throw(( void*)&
Cyc_yyfail_Rgn_tok); _LL1229:;} static char _temp1239[ 15u]="Declarator_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1239, _temp1239, _temp1239 + 15u}}; struct Cyc_Parse_Declarator*
Cyc_yyget_Declarator_tok( void* yy1){ void* _temp1240= yy1; struct Cyc_Parse_Declarator*
_temp1246; _LL1242: if(*(( void**) _temp1240) == Cyc_Declarator_tok){ _LL1247:
_temp1246=( struct Cyc_Parse_Declarator*)(( struct Cyc_Declarator_tok_struct*)
_temp1240)->f1; goto _LL1243;} else{ goto _LL1244;} _LL1244: goto _LL1245;
_LL1243: return _temp1246; _LL1245:( void) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL1241:;} static char _temp1251[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1251, _temp1251, _temp1251 + 21u}}; struct _tuple15* Cyc_yyget_DeclaratorExpopt_tok(
void* yy1){ void* _temp1252= yy1; struct _tuple15* _temp1258; _LL1254: if(*((
void**) _temp1252) == Cyc_DeclaratorExpopt_tok){ _LL1259: _temp1258=( struct
_tuple15*)(( struct Cyc_DeclaratorExpopt_tok_struct*) _temp1252)->f1; goto
_LL1255;} else{ goto _LL1256;} _LL1256: goto _LL1257; _LL1255: return _temp1258;
_LL1257:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok); _LL1253:;}
static char _temp1263[ 25u]="DeclaratorExpoptList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1263, _temp1263, _temp1263 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1264= yy1; struct Cyc_List_List* _temp1270; _LL1266: if(*((
void**) _temp1264) == Cyc_DeclaratorExpoptList_tok){ _LL1271: _temp1270=( struct
Cyc_List_List*)(( struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1264)->f1;
goto _LL1267;} else{ goto _LL1268;} _LL1268: goto _LL1269; _LL1267: return
_temp1270; _LL1269:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok);
_LL1265:;} static char _temp1275[ 23u]="AbstractDeclarator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1275, _temp1275, _temp1275 + 23u}}; struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_AbstractDeclarator_tok( void* yy1){ void* _temp1276= yy1; struct Cyc_Parse_Abstractdeclarator*
_temp1282; _LL1278: if(*(( void**) _temp1276) == Cyc_AbstractDeclarator_tok){
_LL1283: _temp1282=( struct Cyc_Parse_Abstractdeclarator*)(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp1276)->f1; goto _LL1279;} else{ goto _LL1280;} _LL1280: goto _LL1281;
_LL1279: return _temp1282; _LL1281:( void) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL1277:;} static char _temp1287[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1287,
_temp1287, _temp1287 + 9u}}; int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1288=
yy1; int _temp1294; _LL1290: if(*(( void**) _temp1288) == Cyc_Bool_tok){ _LL1295:
_temp1294=( int)(( struct Cyc_Bool_tok_struct*) _temp1288)->f1; goto _LL1291;}
else{ goto _LL1292;} _LL1292: goto _LL1293; _LL1291: return _temp1294; _LL1293:(
void) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL1289:;} static char _temp1299[
10u]="Scope_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1299, _temp1299, _temp1299 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1300= yy1; void* _temp1306;
_LL1302: if(*(( void**) _temp1300) == Cyc_Scope_tok){ _LL1307: _temp1306=( void*)((
struct Cyc_Scope_tok_struct*) _temp1300)->f1; goto _LL1303;} else{ goto _LL1304;}
_LL1304: goto _LL1305; _LL1303: return _temp1306; _LL1305:( void) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1301:;} static char _temp1311[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1311, _temp1311, _temp1311 + 16u}}; struct Cyc_Absyn_Tunionfield*
Cyc_yyget_TunionField_tok( void* yy1){ void* _temp1312= yy1; struct Cyc_Absyn_Tunionfield*
_temp1318; _LL1314: if(*(( void**) _temp1312) == Cyc_TunionField_tok){ _LL1319:
_temp1318=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_TunionField_tok_struct*)
_temp1312)->f1; goto _LL1315;} else{ goto _LL1316;} _LL1316: goto _LL1317;
_LL1315: return _temp1318; _LL1317:( void) _throw(( void*)& Cyc_yyfail_TunionField_tok);
_LL1313:;} static char _temp1323[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1323, _temp1323, _temp1323 + 20u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ void* _temp1324= yy1; struct Cyc_List_List* _temp1330; _LL1326: if(*((
void**) _temp1324) == Cyc_TunionFieldList_tok){ _LL1331: _temp1330=( struct Cyc_List_List*)((
struct Cyc_TunionFieldList_tok_struct*) _temp1324)->f1; goto _LL1327;} else{
goto _LL1328;} _LL1328: goto _LL1329; _LL1327: return _temp1330; _LL1329:( void)
_throw(( void*)& Cyc_yyfail_TunionFieldList_tok); _LL1325:;} static char
_temp1335[ 17u]="QualSpecList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1335, _temp1335, _temp1335 + 17u}};
struct _tuple14* Cyc_yyget_QualSpecList_tok( void* yy1){ void* _temp1336= yy1;
struct _tuple14* _temp1342; _LL1338: if(*(( void**) _temp1336) == Cyc_QualSpecList_tok){
_LL1343: _temp1342=( struct _tuple14*)(( struct Cyc_QualSpecList_tok_struct*)
_temp1336)->f1; goto _LL1339;} else{ goto _LL1340;} _LL1340: goto _LL1341;
_LL1339: return _temp1342; _LL1341:( void) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1337:;} static char _temp1347[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1347,
_temp1347, _temp1347 + 11u}}; struct Cyc_List_List* Cyc_yyget_IdList_tok( void*
yy1){ void* _temp1348= yy1; struct Cyc_List_List* _temp1354; _LL1350: if(*((
void**) _temp1348) == Cyc_IdList_tok){ _LL1355: _temp1354=( struct Cyc_List_List*)((
struct Cyc_IdList_tok_struct*) _temp1348)->f1; goto _LL1351;} else{ goto _LL1352;}
_LL1352: goto _LL1353; _LL1351: return _temp1354; _LL1353:( void) _throw(( void*)&
Cyc_yyfail_IdList_tok); _LL1349:;} static char _temp1359[ 14u]="ParamDecl_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1359, _temp1359, _temp1359 + 14u}}; struct _tuple2*
Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1360= yy1; struct _tuple2*
_temp1366; _LL1362: if(*(( void**) _temp1360) == Cyc_ParamDecl_tok){ _LL1367:
_temp1366=( struct _tuple2*)(( struct Cyc_ParamDecl_tok_struct*) _temp1360)->f1;
goto _LL1363;} else{ goto _LL1364;} _LL1364: goto _LL1365; _LL1363: return
_temp1366; _LL1365:( void) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1361:;}
static char _temp1371[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1371, _temp1371, _temp1371 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ void* _temp1372= yy1; struct Cyc_List_List* _temp1378; _LL1374: if(*((
void**) _temp1372) == Cyc_ParamDeclList_tok){ _LL1379: _temp1378=( struct Cyc_List_List*)((
struct Cyc_ParamDeclList_tok_struct*) _temp1372)->f1; goto _LL1375;} else{ goto
_LL1376;} _LL1376: goto _LL1377; _LL1375: return _temp1378; _LL1377:( void)
_throw(( void*)& Cyc_yyfail_ParamDeclList_tok); _LL1373:;} static char _temp1383[
22u]="ParamDeclListBool_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1383, _temp1383, _temp1383 + 22u}};
struct _tuple16* Cyc_yyget_ParamDeclListBool_tok( void* yy1){ void* _temp1384=
yy1; struct _tuple16* _temp1390; _LL1386: if(*(( void**) _temp1384) == Cyc_ParamDeclListBool_tok){
_LL1391: _temp1390=( struct _tuple16*)(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp1384)->f1; goto _LL1387;} else{ goto _LL1388;} _LL1388: goto _LL1389;
_LL1387: return _temp1390; _LL1389:( void) _throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL1385:;} static char _temp1395[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1395,
_temp1395, _temp1395 + 13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void*
yy1){ void* _temp1396= yy1; struct Cyc_List_List* _temp1402; _LL1398: if(*((
void**) _temp1396) == Cyc_TypeList_tok){ _LL1403: _temp1402=( struct Cyc_List_List*)((
struct Cyc_TypeList_tok_struct*) _temp1396)->f1; goto _LL1399;} else{ goto
_LL1400;} _LL1400: goto _LL1401; _LL1399: return _temp1402; _LL1401:( void)
_throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1397:;} static char _temp1407[ 19u]="DesignatorList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1407, _temp1407, _temp1407 + 19u}}; struct Cyc_List_List*
Cyc_yyget_DesignatorList_tok( void* yy1){ void* _temp1408= yy1; struct Cyc_List_List*
_temp1414; _LL1410: if(*(( void**) _temp1408) == Cyc_DesignatorList_tok){
_LL1415: _temp1414=( struct Cyc_List_List*)(( struct Cyc_DesignatorList_tok_struct*)
_temp1408)->f1; goto _LL1411;} else{ goto _LL1412;} _LL1412: goto _LL1413;
_LL1411: return _temp1414; _LL1413:( void) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1409:;} static char _temp1419[ 15u]="Designator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1419,
_temp1419, _temp1419 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1420= yy1; void* _temp1426; _LL1422: if(*(( void**) _temp1420) == Cyc_Designator_tok){
_LL1427: _temp1426=( void*)(( struct Cyc_Designator_tok_struct*) _temp1420)->f1;
goto _LL1423;} else{ goto _LL1424;} _LL1424: goto _LL1425; _LL1423: return
_temp1426; _LL1425:( void) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1421:;}
static char _temp1431[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1431, _temp1431, _temp1431 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1432= yy1; void* _temp1438;
_LL1434: if(*(( void**) _temp1432) == Cyc_Kind_tok){ _LL1439: _temp1438=( void*)((
struct Cyc_Kind_tok_struct*) _temp1432)->f1; goto _LL1435;} else{ goto _LL1436;}
_LL1436: goto _LL1437; _LL1435: return _temp1438; _LL1437:( void) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1433:;} static char _temp1443[ 9u]="Type_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,( struct
_tagged_string){ _temp1443, _temp1443, _temp1443 + 9u}}; void* Cyc_yyget_Type_tok(
void* yy1){ void* _temp1444= yy1; void* _temp1450; _LL1446: if(*(( void**)
_temp1444) == Cyc_Type_tok){ _LL1451: _temp1450=( void*)(( struct Cyc_Type_tok_struct*)
_temp1444)->f1; goto _LL1447;} else{ goto _LL1448;} _LL1448: goto _LL1449;
_LL1447: return _temp1450; _LL1449:( void) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1445:;} static char _temp1455[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1455, _temp1455, _temp1455 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ void* _temp1456= yy1; struct Cyc_List_List* _temp1462; _LL1458: if(*((
void**) _temp1456) == Cyc_AttributeList_tok){ _LL1463: _temp1462=( struct Cyc_List_List*)((
struct Cyc_AttributeList_tok_struct*) _temp1456)->f1; goto _LL1459;} else{ goto
_LL1460;} _LL1460: goto _LL1461; _LL1459: return _temp1462; _LL1461:( void)
_throw(( void*)& Cyc_yyfail_AttributeList_tok); _LL1457:;} static char _temp1467[
14u]="Attribute_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1467, _temp1467, _temp1467 + 14u}};
void* Cyc_yyget_Attribute_tok( void* yy1){ void* _temp1468= yy1; void* _temp1474;
_LL1470: if(*(( void**) _temp1468) == Cyc_Attribute_tok){ _LL1475: _temp1474=(
void*)(( struct Cyc_Attribute_tok_struct*) _temp1468)->f1; goto _LL1471;} else{
goto _LL1472;} _LL1472: goto _LL1473; _LL1471: return _temp1474; _LL1473:( void)
_throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1469:;} static char _temp1479[ 14u]="Enumfield_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1479, _temp1479, _temp1479 + 14u}}; struct Cyc_Absyn_Enumfield*
Cyc_yyget_Enumfield_tok( void* yy1){ void* _temp1480= yy1; struct Cyc_Absyn_Enumfield*
_temp1486; _LL1482: if(*(( void**) _temp1480) == Cyc_Enumfield_tok){ _LL1487:
_temp1486=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Enumfield_tok_struct*)
_temp1480)->f1; goto _LL1483;} else{ goto _LL1484;} _LL1484: goto _LL1485;
_LL1483: return _temp1486; _LL1485:( void) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1481:;} static char _temp1491[ 18u]="EnumfieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1491, _temp1491, _temp1491 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1492= yy1; struct Cyc_List_List* _temp1498; _LL1494: if(*((
void**) _temp1492) == Cyc_EnumfieldList_tok){ _LL1499: _temp1498=( struct Cyc_List_List*)((
struct Cyc_EnumfieldList_tok_struct*) _temp1492)->f1; goto _LL1495;} else{ goto
_LL1496;} _LL1496: goto _LL1497; _LL1495: return _temp1498; _LL1497:( void)
_throw(( void*)& Cyc_yyfail_EnumfieldList_tok); _LL1493:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct
Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1500; _temp1500.timestamp=
0; _temp1500.first_line= 0; _temp1500.first_column= 0; _temp1500.last_line= 0;
_temp1500.last_column= 0; _temp1500.text=( struct _tagged_string)({ char*
_temp1501=( char*)""; struct _tagged_string _temp1502; _temp1502.curr= _temp1501;
_temp1502.base= _temp1501; _temp1502.last_plus_one= _temp1501 + 1; _temp1502;});
_temp1500;});} static char _temp1505[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc=(
struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column= 0,.last_line= 0,.last_column=
0,.text=( struct _tagged_string){ _temp1505, _temp1505, _temp1505 + 1u}}; static
short Cyc_yytranslate[ 352u]={ (short)0, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)123, (short)2, (short)2, (short)105,
(short)121, (short)117, (short)2, (short)101, (short)102, (short)112, (short)115,
(short)103, (short)116, (short)109, (short)120, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)108,
(short)97, (short)106, (short)100, (short)107, (short)114, (short)113, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)110, (short)2, (short)111, (short)119, (short)104, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)98, (short)118, (short)99, (short)122, (short)2,
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
(short)86, (short)87, (short)88, (short)89, (short)90, (short)91, (short)92,
(short)93, (short)94, (short)95, (short)96}; static char _temp1508[ 2u]="$";
static char _temp1511[ 6u]="error"; static char _temp1514[ 12u]="$undefined.";
static char _temp1517[ 5u]="AUTO"; static char _temp1520[ 9u]="REGISTER"; static
char _temp1523[ 7u]="STATIC"; static char _temp1526[ 7u]="EXTERN"; static char
_temp1529[ 8u]="TYPEDEF"; static char _temp1532[ 5u]="VOID"; static char
_temp1535[ 5u]="CHAR"; static char _temp1538[ 6u]="SHORT"; static char _temp1541[
4u]="INT"; static char _temp1544[ 5u]="LONG"; static char _temp1547[ 6u]="FLOAT";
static char _temp1550[ 7u]="DOUBLE"; static char _temp1553[ 7u]="SIGNED"; static
char _temp1556[ 9u]="UNSIGNED"; static char _temp1559[ 6u]="CONST"; static char
_temp1562[ 9u]="VOLATILE"; static char _temp1565[ 9u]="RESTRICT"; static char
_temp1568[ 7u]="STRUCT"; static char _temp1571[ 6u]="UNION"; static char
_temp1574[ 5u]="CASE"; static char _temp1577[ 8u]="DEFAULT"; static char
_temp1580[ 7u]="INLINE"; static char _temp1583[ 3u]="IF"; static char _temp1586[
5u]="ELSE"; static char _temp1589[ 7u]="SWITCH"; static char _temp1592[ 6u]="WHILE";
static char _temp1595[ 3u]="DO"; static char _temp1598[ 4u]="FOR"; static char
_temp1601[ 5u]="GOTO"; static char _temp1604[ 9u]="CONTINUE"; static char
_temp1607[ 6u]="BREAK"; static char _temp1610[ 7u]="RETURN"; static char
_temp1613[ 7u]="SIZEOF"; static char _temp1616[ 5u]="ENUM"; static char
_temp1619[ 8u]="NULL_kw"; static char _temp1622[ 4u]="LET"; static char
_temp1625[ 6u]="THROW"; static char _temp1628[ 4u]="TRY"; static char _temp1631[
6u]="CATCH"; static char _temp1634[ 4u]="NEW"; static char _temp1637[ 9u]="ABSTRACT";
static char _temp1640[ 9u]="FALLTHRU"; static char _temp1643[ 6u]="USING";
static char _temp1646[ 10u]="NAMESPACE"; static char _temp1649[ 7u]="TUNION";
static char _temp1652[ 8u]="XTUNION"; static char _temp1655[ 5u]="FILL"; static
char _temp1658[ 8u]="CODEGEN"; static char _temp1661[ 4u]="CUT"; static char
_temp1664[ 7u]="SPLICE"; static char _temp1667[ 7u]="PRINTF"; static char
_temp1670[ 8u]="FPRINTF"; static char _temp1673[ 8u]="XPRINTF"; static char
_temp1676[ 6u]="SCANF"; static char _temp1679[ 7u]="FSCANF"; static char
_temp1682[ 7u]="SSCANF"; static char _temp1685[ 7u]="MALLOC"; static char
_temp1688[ 9u]="REGION_T"; static char _temp1691[ 7u]="REGION"; static char
_temp1694[ 5u]="RNEW"; static char _temp1697[ 8u]="RMALLOC"; static char
_temp1700[ 7u]="PTR_OP"; static char _temp1703[ 7u]="INC_OP"; static char
_temp1706[ 7u]="DEC_OP"; static char _temp1709[ 8u]="LEFT_OP"; static char
_temp1712[ 9u]="RIGHT_OP"; static char _temp1715[ 6u]="LE_OP"; static char
_temp1718[ 6u]="GE_OP"; static char _temp1721[ 6u]="EQ_OP"; static char
_temp1724[ 6u]="NE_OP"; static char _temp1727[ 7u]="AND_OP"; static char
_temp1730[ 6u]="OR_OP"; static char _temp1733[ 11u]="MUL_ASSIGN"; static char
_temp1736[ 11u]="DIV_ASSIGN"; static char _temp1739[ 11u]="MOD_ASSIGN"; static
char _temp1742[ 11u]="ADD_ASSIGN"; static char _temp1745[ 11u]="SUB_ASSIGN";
static char _temp1748[ 12u]="LEFT_ASSIGN"; static char _temp1751[ 13u]="RIGHT_ASSIGN";
static char _temp1754[ 11u]="AND_ASSIGN"; static char _temp1757[ 11u]="XOR_ASSIGN";
static char _temp1760[ 10u]="OR_ASSIGN"; static char _temp1763[ 9u]="ELLIPSIS";
static char _temp1766[ 11u]="LEFT_RIGHT"; static char _temp1769[ 12u]="COLON_COLON";
static char _temp1772[ 11u]="IDENTIFIER"; static char _temp1775[ 17u]="INTEGER_CONSTANT";
static char _temp1778[ 7u]="STRING"; static char _temp1781[ 19u]="CHARACTER_CONSTANT";
static char _temp1784[ 18u]="FLOATING_CONSTANT"; static char _temp1787[ 9u]="TYPE_VAR";
static char _temp1790[ 16u]="QUAL_IDENTIFIER"; static char _temp1793[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1796[ 10u]="ATTRIBUTE"; static char _temp1799[ 4u]="';'";
static char _temp1802[ 4u]="'{'"; static char _temp1805[ 4u]="'}'"; static char
_temp1808[ 4u]="'='"; static char _temp1811[ 4u]="'('"; static char _temp1814[ 4u]="')'";
static char _temp1817[ 4u]="','"; static char _temp1820[ 4u]="'_'"; static char
_temp1823[ 4u]="'$'"; static char _temp1826[ 4u]="'<'"; static char _temp1829[ 4u]="'>'";
static char _temp1832[ 4u]="':'"; static char _temp1835[ 4u]="'.'"; static char
_temp1838[ 4u]="'['"; static char _temp1841[ 4u]="']'"; static char _temp1844[ 4u]="'*'";
static char _temp1847[ 4u]="'@'"; static char _temp1850[ 4u]="'?'"; static char
_temp1853[ 4u]="'+'"; static char _temp1856[ 4u]="'-'"; static char _temp1859[ 4u]="'&'";
static char _temp1862[ 4u]="'|'"; static char _temp1865[ 4u]="'^'"; static char
_temp1868[ 4u]="'/'"; static char _temp1871[ 4u]="'%'"; static char _temp1874[ 4u]="'~'";
static char _temp1877[ 4u]="'!'"; static char _temp1880[ 5u]="prog"; static char
_temp1883[ 17u]="translation_unit"; static char _temp1886[ 21u]="external_declaration";
static char _temp1889[ 20u]="function_definition"; static char _temp1892[ 21u]="function_definition2";
static char _temp1895[ 13u]="using_action"; static char _temp1898[ 15u]="unusing_action";
static char _temp1901[ 17u]="namespace_action"; static char _temp1904[ 19u]="unnamespace_action";
static char _temp1907[ 12u]="declaration"; static char _temp1910[ 17u]="declaration_list";
static char _temp1913[ 23u]="declaration_specifiers"; static char _temp1916[ 24u]="storage_class_specifier";
static char _temp1919[ 15u]="attributes_opt"; static char _temp1922[ 11u]="attributes";
static char _temp1925[ 15u]="attribute_list"; static char _temp1928[ 10u]="attribute";
static char _temp1931[ 15u]="type_specifier"; static char _temp1934[ 5u]="kind";
static char _temp1937[ 15u]="type_qualifier"; static char _temp1940[ 15u]="enum_specifier";
static char _temp1943[ 11u]="enum_field"; static char _temp1946[ 22u]="enum_declaration_list";
static char _temp1949[ 26u]="struct_or_union_specifier"; static char _temp1952[
16u]="type_params_opt"; static char _temp1955[ 16u]="struct_or_union"; static
char _temp1958[ 24u]="struct_declaration_list"; static char _temp1961[ 25u]="struct_declaration_list0";
static char _temp1964[ 21u]="init_declarator_list"; static char _temp1967[ 22u]="init_declarator_list0";
static char _temp1970[ 16u]="init_declarator"; static char _temp1973[ 19u]="struct_declaration";
static char _temp1976[ 25u]="specifier_qualifier_list"; static char _temp1979[
23u]="struct_declarator_list"; static char _temp1982[ 24u]="struct_declarator_list0";
static char _temp1985[ 18u]="struct_declarator"; static char _temp1988[ 17u]="tunion_specifier";
static char _temp1991[ 18u]="tunion_or_xtunion"; static char _temp1994[ 17u]="tunionfield_list";
static char _temp1997[ 18u]="tunionfield_scope"; static char _temp2000[ 12u]="tunionfield";
static char _temp2003[ 11u]="declarator"; static char _temp2006[ 18u]="direct_declarator";
static char _temp2009[ 8u]="pointer"; static char _temp2012[ 13u]="pointer_char";
static char _temp2015[ 8u]="rgn_opt"; static char _temp2018[ 4u]="rgn"; static
char _temp2021[ 20u]="type_qualifier_list"; static char _temp2024[ 20u]="parameter_type_list";
static char _temp2027[ 11u]="effect_set"; static char _temp2030[ 14u]="atomic_effect";
static char _temp2033[ 11u]="region_set"; static char _temp2036[ 15u]="parameter_list";
static char _temp2039[ 22u]="parameter_declaration"; static char _temp2042[ 16u]="identifier_list";
static char _temp2045[ 17u]="identifier_list0"; static char _temp2048[ 12u]="initializer";
static char _temp2051[ 18u]="array_initializer"; static char _temp2054[ 17u]="initializer_list";
static char _temp2057[ 12u]="designation"; static char _temp2060[ 16u]="designator_list";
static char _temp2063[ 11u]="designator"; static char _temp2066[ 10u]="type_name";
static char _temp2069[ 14u]="any_type_name"; static char _temp2072[ 15u]="type_name_list";
static char _temp2075[ 20u]="abstract_declarator"; static char _temp2078[ 27u]="direct_abstract_declarator";
static char _temp2081[ 10u]="statement"; static char _temp2084[ 18u]="labeled_statement";
static char _temp2087[ 21u]="expression_statement"; static char _temp2090[ 19u]="compound_statement";
static char _temp2093[ 16u]="block_item_list"; static char _temp2096[ 11u]="block_item";
static char _temp2099[ 20u]="selection_statement"; static char _temp2102[ 15u]="switch_clauses";
static char _temp2105[ 20u]="iteration_statement"; static char _temp2108[ 15u]="jump_statement";
static char _temp2111[ 8u]="pattern"; static char _temp2114[ 19u]="tuple_pattern_list";
static char _temp2117[ 20u]="tuple_pattern_list0"; static char _temp2120[ 14u]="field_pattern";
static char _temp2123[ 19u]="field_pattern_list"; static char _temp2126[ 20u]="field_pattern_list0";
static char _temp2129[ 11u]="expression"; static char _temp2132[ 22u]="assignment_expression";
static char _temp2135[ 20u]="assignment_operator"; static char _temp2138[ 23u]="conditional_expression";
static char _temp2141[ 20u]="constant_expression"; static char _temp2144[ 22u]="logical_or_expression";
static char _temp2147[ 23u]="logical_and_expression"; static char _temp2150[ 24u]="inclusive_or_expression";
static char _temp2153[ 24u]="exclusive_or_expression"; static char _temp2156[ 15u]="and_expression";
static char _temp2159[ 20u]="equality_expression"; static char _temp2162[ 22u]="relational_expression";
static char _temp2165[ 17u]="shift_expression"; static char _temp2168[ 20u]="additive_expression";
static char _temp2171[ 26u]="multiplicative_expression"; static char _temp2174[
16u]="cast_expression"; static char _temp2177[ 17u]="unary_expression"; static
char _temp2180[ 14u]="format_primop"; static char _temp2183[ 15u]="unary_operator";
static char _temp2186[ 19u]="postfix_expression"; static char _temp2189[ 19u]="primary_expression";
static char _temp2192[ 25u]="argument_expression_list"; static char _temp2195[
26u]="argument_expression_list0"; static char _temp2198[ 9u]="constant"; static
char _temp2201[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
232u]={( struct _tagged_string){ _temp1508, _temp1508, _temp1508 + 2u},( struct
_tagged_string){ _temp1511, _temp1511, _temp1511 + 6u},( struct _tagged_string){
_temp1514, _temp1514, _temp1514 + 12u},( struct _tagged_string){ _temp1517,
_temp1517, _temp1517 + 5u},( struct _tagged_string){ _temp1520, _temp1520,
_temp1520 + 9u},( struct _tagged_string){ _temp1523, _temp1523, _temp1523 + 7u},(
struct _tagged_string){ _temp1526, _temp1526, _temp1526 + 7u},( struct
_tagged_string){ _temp1529, _temp1529, _temp1529 + 8u},( struct _tagged_string){
_temp1532, _temp1532, _temp1532 + 5u},( struct _tagged_string){ _temp1535,
_temp1535, _temp1535 + 5u},( struct _tagged_string){ _temp1538, _temp1538,
_temp1538 + 6u},( struct _tagged_string){ _temp1541, _temp1541, _temp1541 + 4u},(
struct _tagged_string){ _temp1544, _temp1544, _temp1544 + 5u},( struct
_tagged_string){ _temp1547, _temp1547, _temp1547 + 6u},( struct _tagged_string){
_temp1550, _temp1550, _temp1550 + 7u},( struct _tagged_string){ _temp1553,
_temp1553, _temp1553 + 7u},( struct _tagged_string){ _temp1556, _temp1556,
_temp1556 + 9u},( struct _tagged_string){ _temp1559, _temp1559, _temp1559 + 6u},(
struct _tagged_string){ _temp1562, _temp1562, _temp1562 + 9u},( struct
_tagged_string){ _temp1565, _temp1565, _temp1565 + 9u},( struct _tagged_string){
_temp1568, _temp1568, _temp1568 + 7u},( struct _tagged_string){ _temp1571,
_temp1571, _temp1571 + 6u},( struct _tagged_string){ _temp1574, _temp1574,
_temp1574 + 5u},( struct _tagged_string){ _temp1577, _temp1577, _temp1577 + 8u},(
struct _tagged_string){ _temp1580, _temp1580, _temp1580 + 7u},( struct
_tagged_string){ _temp1583, _temp1583, _temp1583 + 3u},( struct _tagged_string){
_temp1586, _temp1586, _temp1586 + 5u},( struct _tagged_string){ _temp1589,
_temp1589, _temp1589 + 7u},( struct _tagged_string){ _temp1592, _temp1592,
_temp1592 + 6u},( struct _tagged_string){ _temp1595, _temp1595, _temp1595 + 3u},(
struct _tagged_string){ _temp1598, _temp1598, _temp1598 + 4u},( struct
_tagged_string){ _temp1601, _temp1601, _temp1601 + 5u},( struct _tagged_string){
_temp1604, _temp1604, _temp1604 + 9u},( struct _tagged_string){ _temp1607,
_temp1607, _temp1607 + 6u},( struct _tagged_string){ _temp1610, _temp1610,
_temp1610 + 7u},( struct _tagged_string){ _temp1613, _temp1613, _temp1613 + 7u},(
struct _tagged_string){ _temp1616, _temp1616, _temp1616 + 5u},( struct
_tagged_string){ _temp1619, _temp1619, _temp1619 + 8u},( struct _tagged_string){
_temp1622, _temp1622, _temp1622 + 4u},( struct _tagged_string){ _temp1625,
_temp1625, _temp1625 + 6u},( struct _tagged_string){ _temp1628, _temp1628,
_temp1628 + 4u},( struct _tagged_string){ _temp1631, _temp1631, _temp1631 + 6u},(
struct _tagged_string){ _temp1634, _temp1634, _temp1634 + 4u},( struct
_tagged_string){ _temp1637, _temp1637, _temp1637 + 9u},( struct _tagged_string){
_temp1640, _temp1640, _temp1640 + 9u},( struct _tagged_string){ _temp1643,
_temp1643, _temp1643 + 6u},( struct _tagged_string){ _temp1646, _temp1646,
_temp1646 + 10u},( struct _tagged_string){ _temp1649, _temp1649, _temp1649 + 7u},(
struct _tagged_string){ _temp1652, _temp1652, _temp1652 + 8u},( struct
_tagged_string){ _temp1655, _temp1655, _temp1655 + 5u},( struct _tagged_string){
_temp1658, _temp1658, _temp1658 + 8u},( struct _tagged_string){ _temp1661,
_temp1661, _temp1661 + 4u},( struct _tagged_string){ _temp1664, _temp1664,
_temp1664 + 7u},( struct _tagged_string){ _temp1667, _temp1667, _temp1667 + 7u},(
struct _tagged_string){ _temp1670, _temp1670, _temp1670 + 8u},( struct
_tagged_string){ _temp1673, _temp1673, _temp1673 + 8u},( struct _tagged_string){
_temp1676, _temp1676, _temp1676 + 6u},( struct _tagged_string){ _temp1679,
_temp1679, _temp1679 + 7u},( struct _tagged_string){ _temp1682, _temp1682,
_temp1682 + 7u},( struct _tagged_string){ _temp1685, _temp1685, _temp1685 + 7u},(
struct _tagged_string){ _temp1688, _temp1688, _temp1688 + 9u},( struct
_tagged_string){ _temp1691, _temp1691, _temp1691 + 7u},( struct _tagged_string){
_temp1694, _temp1694, _temp1694 + 5u},( struct _tagged_string){ _temp1697,
_temp1697, _temp1697 + 8u},( struct _tagged_string){ _temp1700, _temp1700,
_temp1700 + 7u},( struct _tagged_string){ _temp1703, _temp1703, _temp1703 + 7u},(
struct _tagged_string){ _temp1706, _temp1706, _temp1706 + 7u},( struct
_tagged_string){ _temp1709, _temp1709, _temp1709 + 8u},( struct _tagged_string){
_temp1712, _temp1712, _temp1712 + 9u},( struct _tagged_string){ _temp1715,
_temp1715, _temp1715 + 6u},( struct _tagged_string){ _temp1718, _temp1718,
_temp1718 + 6u},( struct _tagged_string){ _temp1721, _temp1721, _temp1721 + 6u},(
struct _tagged_string){ _temp1724, _temp1724, _temp1724 + 6u},( struct
_tagged_string){ _temp1727, _temp1727, _temp1727 + 7u},( struct _tagged_string){
_temp1730, _temp1730, _temp1730 + 6u},( struct _tagged_string){ _temp1733,
_temp1733, _temp1733 + 11u},( struct _tagged_string){ _temp1736, _temp1736,
_temp1736 + 11u},( struct _tagged_string){ _temp1739, _temp1739, _temp1739 + 11u},(
struct _tagged_string){ _temp1742, _temp1742, _temp1742 + 11u},( struct
_tagged_string){ _temp1745, _temp1745, _temp1745 + 11u},( struct _tagged_string){
_temp1748, _temp1748, _temp1748 + 12u},( struct _tagged_string){ _temp1751,
_temp1751, _temp1751 + 13u},( struct _tagged_string){ _temp1754, _temp1754,
_temp1754 + 11u},( struct _tagged_string){ _temp1757, _temp1757, _temp1757 + 11u},(
struct _tagged_string){ _temp1760, _temp1760, _temp1760 + 10u},( struct
_tagged_string){ _temp1763, _temp1763, _temp1763 + 9u},( struct _tagged_string){
_temp1766, _temp1766, _temp1766 + 11u},( struct _tagged_string){ _temp1769,
_temp1769, _temp1769 + 12u},( struct _tagged_string){ _temp1772, _temp1772,
_temp1772 + 11u},( struct _tagged_string){ _temp1775, _temp1775, _temp1775 + 17u},(
struct _tagged_string){ _temp1778, _temp1778, _temp1778 + 7u},( struct
_tagged_string){ _temp1781, _temp1781, _temp1781 + 19u},( struct _tagged_string){
_temp1784, _temp1784, _temp1784 + 18u},( struct _tagged_string){ _temp1787,
_temp1787, _temp1787 + 9u},( struct _tagged_string){ _temp1790, _temp1790,
_temp1790 + 16u},( struct _tagged_string){ _temp1793, _temp1793, _temp1793 + 18u},(
struct _tagged_string){ _temp1796, _temp1796, _temp1796 + 10u},( struct
_tagged_string){ _temp1799, _temp1799, _temp1799 + 4u},( struct _tagged_string){
_temp1802, _temp1802, _temp1802 + 4u},( struct _tagged_string){ _temp1805,
_temp1805, _temp1805 + 4u},( struct _tagged_string){ _temp1808, _temp1808,
_temp1808 + 4u},( struct _tagged_string){ _temp1811, _temp1811, _temp1811 + 4u},(
struct _tagged_string){ _temp1814, _temp1814, _temp1814 + 4u},( struct
_tagged_string){ _temp1817, _temp1817, _temp1817 + 4u},( struct _tagged_string){
_temp1820, _temp1820, _temp1820 + 4u},( struct _tagged_string){ _temp1823,
_temp1823, _temp1823 + 4u},( struct _tagged_string){ _temp1826, _temp1826,
_temp1826 + 4u},( struct _tagged_string){ _temp1829, _temp1829, _temp1829 + 4u},(
struct _tagged_string){ _temp1832, _temp1832, _temp1832 + 4u},( struct
_tagged_string){ _temp1835, _temp1835, _temp1835 + 4u},( struct _tagged_string){
_temp1838, _temp1838, _temp1838 + 4u},( struct _tagged_string){ _temp1841,
_temp1841, _temp1841 + 4u},( struct _tagged_string){ _temp1844, _temp1844,
_temp1844 + 4u},( struct _tagged_string){ _temp1847, _temp1847, _temp1847 + 4u},(
struct _tagged_string){ _temp1850, _temp1850, _temp1850 + 4u},( struct
_tagged_string){ _temp1853, _temp1853, _temp1853 + 4u},( struct _tagged_string){
_temp1856, _temp1856, _temp1856 + 4u},( struct _tagged_string){ _temp1859,
_temp1859, _temp1859 + 4u},( struct _tagged_string){ _temp1862, _temp1862,
_temp1862 + 4u},( struct _tagged_string){ _temp1865, _temp1865, _temp1865 + 4u},(
struct _tagged_string){ _temp1868, _temp1868, _temp1868 + 4u},( struct
_tagged_string){ _temp1871, _temp1871, _temp1871 + 4u},( struct _tagged_string){
_temp1874, _temp1874, _temp1874 + 4u},( struct _tagged_string){ _temp1877,
_temp1877, _temp1877 + 4u},( struct _tagged_string){ _temp1880, _temp1880,
_temp1880 + 5u},( struct _tagged_string){ _temp1883, _temp1883, _temp1883 + 17u},(
struct _tagged_string){ _temp1886, _temp1886, _temp1886 + 21u},( struct
_tagged_string){ _temp1889, _temp1889, _temp1889 + 20u},( struct _tagged_string){
_temp1892, _temp1892, _temp1892 + 21u},( struct _tagged_string){ _temp1895,
_temp1895, _temp1895 + 13u},( struct _tagged_string){ _temp1898, _temp1898,
_temp1898 + 15u},( struct _tagged_string){ _temp1901, _temp1901, _temp1901 + 17u},(
struct _tagged_string){ _temp1904, _temp1904, _temp1904 + 19u},( struct
_tagged_string){ _temp1907, _temp1907, _temp1907 + 12u},( struct _tagged_string){
_temp1910, _temp1910, _temp1910 + 17u},( struct _tagged_string){ _temp1913,
_temp1913, _temp1913 + 23u},( struct _tagged_string){ _temp1916, _temp1916,
_temp1916 + 24u},( struct _tagged_string){ _temp1919, _temp1919, _temp1919 + 15u},(
struct _tagged_string){ _temp1922, _temp1922, _temp1922 + 11u},( struct
_tagged_string){ _temp1925, _temp1925, _temp1925 + 15u},( struct _tagged_string){
_temp1928, _temp1928, _temp1928 + 10u},( struct _tagged_string){ _temp1931,
_temp1931, _temp1931 + 15u},( struct _tagged_string){ _temp1934, _temp1934,
_temp1934 + 5u},( struct _tagged_string){ _temp1937, _temp1937, _temp1937 + 15u},(
struct _tagged_string){ _temp1940, _temp1940, _temp1940 + 15u},( struct
_tagged_string){ _temp1943, _temp1943, _temp1943 + 11u},( struct _tagged_string){
_temp1946, _temp1946, _temp1946 + 22u},( struct _tagged_string){ _temp1949,
_temp1949, _temp1949 + 26u},( struct _tagged_string){ _temp1952, _temp1952,
_temp1952 + 16u},( struct _tagged_string){ _temp1955, _temp1955, _temp1955 + 16u},(
struct _tagged_string){ _temp1958, _temp1958, _temp1958 + 24u},( struct
_tagged_string){ _temp1961, _temp1961, _temp1961 + 25u},( struct _tagged_string){
_temp1964, _temp1964, _temp1964 + 21u},( struct _tagged_string){ _temp1967,
_temp1967, _temp1967 + 22u},( struct _tagged_string){ _temp1970, _temp1970,
_temp1970 + 16u},( struct _tagged_string){ _temp1973, _temp1973, _temp1973 + 19u},(
struct _tagged_string){ _temp1976, _temp1976, _temp1976 + 25u},( struct
_tagged_string){ _temp1979, _temp1979, _temp1979 + 23u},( struct _tagged_string){
_temp1982, _temp1982, _temp1982 + 24u},( struct _tagged_string){ _temp1985,
_temp1985, _temp1985 + 18u},( struct _tagged_string){ _temp1988, _temp1988,
_temp1988 + 17u},( struct _tagged_string){ _temp1991, _temp1991, _temp1991 + 18u},(
struct _tagged_string){ _temp1994, _temp1994, _temp1994 + 17u},( struct
_tagged_string){ _temp1997, _temp1997, _temp1997 + 18u},( struct _tagged_string){
_temp2000, _temp2000, _temp2000 + 12u},( struct _tagged_string){ _temp2003,
_temp2003, _temp2003 + 11u},( struct _tagged_string){ _temp2006, _temp2006,
_temp2006 + 18u},( struct _tagged_string){ _temp2009, _temp2009, _temp2009 + 8u},(
struct _tagged_string){ _temp2012, _temp2012, _temp2012 + 13u},( struct
_tagged_string){ _temp2015, _temp2015, _temp2015 + 8u},( struct _tagged_string){
_temp2018, _temp2018, _temp2018 + 4u},( struct _tagged_string){ _temp2021,
_temp2021, _temp2021 + 20u},( struct _tagged_string){ _temp2024, _temp2024,
_temp2024 + 20u},( struct _tagged_string){ _temp2027, _temp2027, _temp2027 + 11u},(
struct _tagged_string){ _temp2030, _temp2030, _temp2030 + 14u},( struct
_tagged_string){ _temp2033, _temp2033, _temp2033 + 11u},( struct _tagged_string){
_temp2036, _temp2036, _temp2036 + 15u},( struct _tagged_string){ _temp2039,
_temp2039, _temp2039 + 22u},( struct _tagged_string){ _temp2042, _temp2042,
_temp2042 + 16u},( struct _tagged_string){ _temp2045, _temp2045, _temp2045 + 17u},(
struct _tagged_string){ _temp2048, _temp2048, _temp2048 + 12u},( struct
_tagged_string){ _temp2051, _temp2051, _temp2051 + 18u},( struct _tagged_string){
_temp2054, _temp2054, _temp2054 + 17u},( struct _tagged_string){ _temp2057,
_temp2057, _temp2057 + 12u},( struct _tagged_string){ _temp2060, _temp2060,
_temp2060 + 16u},( struct _tagged_string){ _temp2063, _temp2063, _temp2063 + 11u},(
struct _tagged_string){ _temp2066, _temp2066, _temp2066 + 10u},( struct
_tagged_string){ _temp2069, _temp2069, _temp2069 + 14u},( struct _tagged_string){
_temp2072, _temp2072, _temp2072 + 15u},( struct _tagged_string){ _temp2075,
_temp2075, _temp2075 + 20u},( struct _tagged_string){ _temp2078, _temp2078,
_temp2078 + 27u},( struct _tagged_string){ _temp2081, _temp2081, _temp2081 + 10u},(
struct _tagged_string){ _temp2084, _temp2084, _temp2084 + 18u},( struct
_tagged_string){ _temp2087, _temp2087, _temp2087 + 21u},( struct _tagged_string){
_temp2090, _temp2090, _temp2090 + 19u},( struct _tagged_string){ _temp2093,
_temp2093, _temp2093 + 16u},( struct _tagged_string){ _temp2096, _temp2096,
_temp2096 + 11u},( struct _tagged_string){ _temp2099, _temp2099, _temp2099 + 20u},(
struct _tagged_string){ _temp2102, _temp2102, _temp2102 + 15u},( struct
_tagged_string){ _temp2105, _temp2105, _temp2105 + 20u},( struct _tagged_string){
_temp2108, _temp2108, _temp2108 + 15u},( struct _tagged_string){ _temp2111,
_temp2111, _temp2111 + 8u},( struct _tagged_string){ _temp2114, _temp2114,
_temp2114 + 19u},( struct _tagged_string){ _temp2117, _temp2117, _temp2117 + 20u},(
struct _tagged_string){ _temp2120, _temp2120, _temp2120 + 14u},( struct
_tagged_string){ _temp2123, _temp2123, _temp2123 + 19u},( struct _tagged_string){
_temp2126, _temp2126, _temp2126 + 20u},( struct _tagged_string){ _temp2129,
_temp2129, _temp2129 + 11u},( struct _tagged_string){ _temp2132, _temp2132,
_temp2132 + 22u},( struct _tagged_string){ _temp2135, _temp2135, _temp2135 + 20u},(
struct _tagged_string){ _temp2138, _temp2138, _temp2138 + 23u},( struct
_tagged_string){ _temp2141, _temp2141, _temp2141 + 20u},( struct _tagged_string){
_temp2144, _temp2144, _temp2144 + 22u},( struct _tagged_string){ _temp2147,
_temp2147, _temp2147 + 23u},( struct _tagged_string){ _temp2150, _temp2150,
_temp2150 + 24u},( struct _tagged_string){ _temp2153, _temp2153, _temp2153 + 24u},(
struct _tagged_string){ _temp2156, _temp2156, _temp2156 + 15u},( struct
_tagged_string){ _temp2159, _temp2159, _temp2159 + 20u},( struct _tagged_string){
_temp2162, _temp2162, _temp2162 + 22u},( struct _tagged_string){ _temp2165,
_temp2165, _temp2165 + 17u},( struct _tagged_string){ _temp2168, _temp2168,
_temp2168 + 20u},( struct _tagged_string){ _temp2171, _temp2171, _temp2171 + 26u},(
struct _tagged_string){ _temp2174, _temp2174, _temp2174 + 16u},( struct
_tagged_string){ _temp2177, _temp2177, _temp2177 + 17u},( struct _tagged_string){
_temp2180, _temp2180, _temp2180 + 14u},( struct _tagged_string){ _temp2183,
_temp2183, _temp2183 + 15u},( struct _tagged_string){ _temp2186, _temp2186,
_temp2186 + 19u},( struct _tagged_string){ _temp2189, _temp2189, _temp2189 + 19u},(
struct _tagged_string){ _temp2192, _temp2192, _temp2192 + 25u},( struct
_tagged_string){ _temp2195, _temp2195, _temp2195 + 26u},( struct _tagged_string){
_temp2198, _temp2198, _temp2198 + 9u},( struct _tagged_string){ _temp2201,
_temp2201, _temp2201 + 20u}}; static short Cyc_yyr1[ 394u]={ (short)0, (short)124,
(short)125, (short)125, (short)125, (short)125, (short)125, (short)125, (short)125,
(short)126, (short)126, (short)127, (short)127, (short)127, (short)127, (short)128,
(short)128, (short)129, (short)130, (short)131, (short)132, (short)133, (short)133,
(short)133, (short)134, (short)134, (short)135, (short)135, (short)135, (short)135,
(short)135, (short)135, (short)135, (short)135, (short)136, (short)136, (short)136,
(short)136, (short)136, (short)136, (short)136, (short)137, (short)137, (short)138,
(short)139, (short)139, (short)140, (short)140, (short)140, (short)140, (short)141,
(short)141, (short)141, (short)141, (short)141, (short)141, (short)141, (short)141,
(short)141, (short)141, (short)141, (short)141, (short)141, (short)141, (short)141,
(short)141, (short)141, (short)141, (short)142, (short)142, (short)143, (short)143,
(short)143, (short)144, (short)144, (short)145, (short)145, (short)146, (short)146,
(short)147, (short)147, (short)147, (short)147, (short)147, (short)148, (short)148,
(short)148, (short)149, (short)149, (short)150, (short)151, (short)151, (short)152,
(short)153, (short)153, (short)154, (short)154, (short)155, (short)156, (short)156,
(short)156, (short)156, (short)157, (short)158, (short)158, (short)159, (short)159,
(short)159, (short)160, (short)160, (short)160, (short)160, (short)161, (short)161,
(short)162, (short)162, (short)162, (short)162, (short)163, (short)163, (short)163,
(short)164, (short)164, (short)165, (short)165, (short)166, (short)166, (short)166,
(short)166, (short)166, (short)166, (short)166, (short)166, (short)166, (short)166,
(short)166, (short)167, (short)167, (short)167, (short)167, (short)168, (short)168,
(short)168, (short)168, (short)168, (short)169, (short)169, (short)170, (short)170,
(short)170, (short)171, (short)171, (short)172, (short)172, (short)172, (short)173,
(short)173, (short)174, (short)174, (short)174, (short)174, (short)175, (short)175,
(short)175, (short)175, (short)176, (short)176, (short)177, (short)177, (short)177,
(short)178, (short)179, (short)179, (short)180, (short)180, (short)181, (short)181,
(short)181, (short)181, (short)182, (short)182, (short)182, (short)182, (short)183,
(short)184, (short)184, (short)185, (short)185, (short)186, (short)186, (short)187,
(short)187, (short)187, (short)187, (short)188, (short)188, (short)189, (short)189,
(short)189, (short)190, (short)190, (short)190, (short)190, (short)190, (short)190,
(short)190, (short)190, (short)190, (short)190, (short)190, (short)190, (short)190,
(short)190, (short)191, (short)191, (short)191, (short)191, (short)191, (short)191,
(short)191, (short)191, (short)191, (short)191, (short)192, (short)193, (short)193,
(short)194, (short)194, (short)195, (short)195, (short)196, (short)196, (short)196,
(short)197, (short)197, (short)197, (short)197, (short)198, (short)198, (short)198,
(short)198, (short)198, (short)198, (short)199, (short)199, (short)199, (short)199,
(short)199, (short)199, (short)199, (short)199, (short)199, (short)199, (short)199,
(short)199, (short)199, (short)199, (short)200, (short)200, (short)200, (short)200,
(short)200, (short)200, (short)200, (short)200, (short)201, (short)201, (short)201,
(short)201, (short)201, (short)201, (short)201, (short)201, (short)201, (short)201,
(short)201, (short)201, (short)201, (short)201, (short)202, (short)202, (short)203,
(short)203, (short)204, (short)204, (short)205, (short)206, (short)206, (short)207,
(short)207, (short)208, (short)208, (short)209, (short)209, (short)209, (short)209,
(short)209, (short)209, (short)209, (short)209, (short)209, (short)209, (short)209,
(short)210, (short)210, (short)210, (short)210, (short)210, (short)210, (short)210,
(short)211, (short)212, (short)212, (short)213, (short)213, (short)214, (short)214,
(short)215, (short)215, (short)216, (short)216, (short)217, (short)217, (short)217,
(short)218, (short)218, (short)218, (short)218, (short)218, (short)219, (short)219,
(short)219, (short)220, (short)220, (short)220, (short)221, (short)221, (short)221,
(short)221, (short)222, (short)222, (short)223, (short)223, (short)223, (short)223,
(short)223, (short)223, (short)223, (short)223, (short)223, (short)223, (short)223,
(short)223, (short)224, (short)224, (short)224, (short)224, (short)224, (short)224,
(short)225, (short)225, (short)225, (short)226, (short)226, (short)226, (short)226,
(short)226, (short)226, (short)226, (short)226, (short)226, (short)226, (short)226,
(short)226, (short)226, (short)226, (short)227, (short)227, (short)227, (short)227,
(short)227, (short)227, (short)227, (short)227, (short)227, (short)228, (short)229,
(short)229, (short)230, (short)230, (short)230, (short)230, (short)231, (short)231};
static short Cyc_yyr2[ 394u]={ (short)0, (short)1, (short)2, (short)3, (short)5,
(short)3, (short)5, (short)6, (short)0, (short)1, (short)1, (short)2, (short)3,
(short)3, (short)4, (short)3, (short)4, (short)2, (short)1, (short)2, (short)1,
(short)2, (short)3, (short)5, (short)1, (short)2, (short)2, (short)3, (short)2,
(short)3, (short)2, (short)3, (short)2, (short)3, (short)1, (short)1, (short)1,
(short)1, (short)2, (short)1, (short)1, (short)0, (short)1, (short)6, (short)1,
(short)3, (short)1, (short)1, (short)4, (short)4, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)3, (short)2, (short)4, (short)4, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)5, (short)2, (short)1, (short)3,
(short)1, (short)3, (short)4, (short)6, (short)6, (short)3, (short)3, (short)0,
(short)3, (short)3, (short)1, (short)1, (short)1, (short)1, (short)2, (short)1,
(short)1, (short)3, (short)1, (short)3, (short)3, (short)2, (short)3, (short)2,
(short)3, (short)1, (short)1, (short)3, (short)1, (short)2, (short)3, (short)6,
(short)3, (short)4, (short)5, (short)1, (short)1, (short)1, (short)2, (short)3,
(short)3, (short)0, (short)1, (short)1, (short)2, (short)6, (short)1, (short)2,
(short)1, (short)3, (short)3, (short)4, (short)4, (short)3, (short)5, (short)4,
(short)4, (short)4, (short)2, (short)3, (short)4, (short)4, (short)5, (short)1,
(short)1, (short)4, (short)4, (short)1, (short)0, (short)1, (short)1, (short)3,
(short)1, (short)1, (short)2, (short)1, (short)3, (short)3, (short)1, (short)3,
(short)2, (short)3, (short)1, (short)3, (short)1, (short)3, (short)3, (short)5,
(short)1, (short)3, (short)2, (short)1, (short)2, (short)1, (short)1, (short)3,
(short)1, (short)1, (short)2, (short)3, (short)4, (short)8, (short)1, (short)2,
(short)3, (short)4, (short)2, (short)1, (short)2, (short)3, (short)2, (short)1,
(short)2, (short)1, (short)2, (short)3, (short)3, (short)1, (short)3, (short)1,
(short)1, (short)2, (short)3, (short)2, (short)3, (short)3, (short)4, (short)2,
(short)4, (short)3, (short)3, (short)5, (short)4, (short)4, (short)4, (short)2,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)6, (short)3,
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
(short)1, (short)1, (short)1, (short)5, (short)2, (short)2, (short)2, (short)5,
(short)5, (short)1, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3,
(short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)3, (short)1,
(short)3, (short)3, (short)3, (short)3, (short)1, (short)3, (short)3, (short)1,
(short)3, (short)3, (short)1, (short)3, (short)3, (short)3, (short)1, (short)4,
(short)1, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)4,
(short)2, (short)4, (short)7, (short)9, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)4, (short)3,
(short)4, (short)3, (short)3, (short)3, (short)3, (short)2, (short)2, (short)6,
(short)7, (short)4, (short)4, (short)1, (short)1, (short)1, (short)3, (short)2,
(short)5, (short)4, (short)4, (short)5, (short)1, (short)1, (short)3, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1}; static short Cyc_yydefact[
751u]={ (short)8, (short)34, (short)35, (short)36, (short)37, (short)39, (short)50,
(short)52, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, (short)70, (short)71, (short)72, (short)87, (short)88, (short)41,
(short)0, (short)0, (short)40, (short)0, (short)0, (short)112, (short)113,
(short)0, (short)392, (short)63, (short)393, (short)84, (short)0, (short)51,
(short)0, (short)140, (short)141, (short)144, (short)1, (short)8, (short)9,
(short)0, (short)0, (short)10, (short)0, (short)41, (short)41, (short)41,
(short)60, (short)61, (short)0, (short)62, (short)0, (short)0, (short)123,
(short)0, (short)145, (short)125, (short)38, (short)0, (short)32, (short)42,
(short)74, (short)271, (short)267, (short)270, (short)269, (short)0, (short)265,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)272, (short)17, (short)19,
(short)0, (short)0, (short)0, (short)65, (short)0, (short)0, (short)0, (short)0,
(short)2, (short)8, (short)8, (short)8, (short)8, (short)21, (short)0, (short)92,
(short)93, (short)95, (short)26, (short)28, (short)30, (short)84, (short)0,
(short)84, (short)147, (short)149, (short)0, (short)84, (short)37, (short)0,
(short)24, (short)0, (short)0, (short)11, (short)0, (short)0, (short)0, (short)135,
(short)124, (short)41, (short)146, (short)8, (short)0, (short)33, (short)0,
(short)0, (short)279, (short)278, (short)268, (short)277, (short)0, (short)0,
(short)0, (short)41, (short)41, (short)188, (short)190, (short)0, (short)68,
(short)69, (short)64, (short)194, (short)0, (short)126, (short)168, (short)0,
(short)165, (short)0, (short)391, (short)0, (short)0, (short)0, (short)0,
(short)353, (short)354, (short)355, (short)356, (short)357, (short)358, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)388, (short)378, (short)389,
(short)390, (short)0, (short)0, (short)0, (short)0, (short)361, (short)0,
(short)359, (short)360, (short)0, (short)290, (short)303, (short)311, (short)313,
(short)315, (short)317, (short)319, (short)321, (short)324, (short)329, (short)332,
(short)335, (short)339, (short)0, (short)0, (short)341, (short)362, (short)377,
(short)376, (short)0, (short)3, (short)0, (short)5, (short)0, (short)22, (short)0,
(short)0, (short)0, (short)12, (short)27, (short)29, (short)31, (short)83,
(short)0, (short)89, (short)90, (short)0, (short)82, (short)0, (short)84,
(short)0, (short)109, (short)38, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)392, (short)224, (short)226, (short)0, (short)232, (short)230,
(short)0, (short)231, (short)213, (short)214, (short)215, (short)0, (short)228,
(short)216, (short)217, (short)218, (short)0, (short)288, (short)25, (short)13,
(short)95, (short)171, (short)0, (short)130, (short)0, (short)152, (short)0,
(short)170, (short)0, (short)127, (short)0, (short)136, (short)0, (short)0,
(short)77, (short)0, (short)75, (short)266, (short)281, (short)0, (short)280,
(short)0, (short)0, (short)279, (short)161, (short)191, (short)0, (short)98,
(short)100, (short)0, (short)0, (short)196, (short)189, (short)197, (short)67,
(short)0, (short)86, (short)0, (short)85, (short)0, (short)167, (short)196,
(short)169, (short)66, (short)0, (short)0, (short)349, (short)305, (short)339,
(short)0, (short)306, (short)307, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)342, (short)343, (short)0, (short)0, (short)0,
(short)0, (short)345, (short)346, (short)344, (short)142, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)293, (short)294, (short)295, (short)296, (short)297, (short)298,
(short)299, (short)300, (short)301, (short)302, (short)292, (short)0, (short)0,
(short)347, (short)0, (short)370, (short)371, (short)0, (short)0, (short)0,
(short)380, (short)0, (short)0, (short)143, (short)18, (short)8, (short)20,
(short)8, (short)94, (short)96, (short)174, (short)173, (short)14, (short)0,
(short)79, (short)91, (short)0, (short)0, (short)102, (short)103, (short)105,
(short)0, (short)148, (short)110, (short)84, (short)118, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)258, (short)259, (short)260,
(short)0, (short)0, (short)262, (short)0, (short)221, (short)222, (short)0,
(short)0, (short)0, (short)0, (short)95, (short)227, (short)229, (short)225,
(short)0, (short)159, (short)0, (short)0, (short)155, (short)129, (short)0,
(short)0, (short)132, (short)0, (short)134, (short)133, (short)128, (short)150,
(short)138, (short)137, (short)8, (short)47, (short)46, (short)0, (short)44,
(short)0, (short)73, (short)0, (short)274, (short)0, (short)23, (short)275,
(short)0, (short)0, (short)0, (short)0, (short)184, (short)283, (short)286,
(short)0, (short)285, (short)0, (short)0, (short)0, (short)192, (short)99,
(short)101, (short)0, (short)204, (short)0, (short)0, (short)200, (short)0,
(short)198, (short)0, (short)0, (short)0, (short)212, (short)193, (short)195,
(short)166, (short)0, (short)0, (short)175, (short)179, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)379, (short)386, (short)0, (short)385, (short)312, (short)0,
(short)314, (short)316, (short)318, (short)320, (short)322, (short)323, (short)327,
(short)328, (short)325, (short)326, (short)330, (short)331, (short)333, (short)334,
(short)336, (short)337, (short)338, (short)291, (short)0, (short)368, (short)369,
(short)364, (short)0, (short)366, (short)367, (short)0, (short)0, (short)0,
(short)4, (short)6, (short)0, (short)310, (short)106, (short)97, (short)0,
(short)0, (short)0, (short)111, (short)120, (short)119, (short)0, (short)0,
(short)114, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)257, (short)261, (short)0, (short)0, (short)0, (short)220, (short)0,
(short)223, (short)0, (short)15, (short)289, (short)0, (short)157, (short)0,
(short)131, (short)0, (short)154, (short)153, (short)172, (short)151, (short)139,
(short)7, (short)0, (short)0, (short)0, (short)78, (short)76, (short)282,
(short)187, (short)0, (short)284, (short)183, (short)185, (short)276, (short)0,
(short)273, (short)163, (short)162, (short)0, (short)206, (short)199, (short)202,
(short)0, (short)207, (short)0, (short)0, (short)201, (short)0, (short)348,
(short)0, (short)176, (short)0, (short)180, (short)374, (short)375, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)340, (short)382,
(short)0, (short)0, (short)350, (short)365, (short)363, (short)383, (short)0,
(short)0, (short)81, (short)104, (short)107, (short)80, (short)108, (short)121,
(short)118, (short)118, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)237, (short)263, (short)0,
(short)0, (short)16, (short)160, (short)158, (short)156, (short)0, (short)0,
(short)43, (short)45, (short)186, (short)287, (short)0, (short)205, (short)0,
(short)209, (short)211, (short)210, (short)203, (short)0, (short)177, (short)181,
(short)0, (short)0, (short)308, (short)309, (short)0, (short)384, (short)0,
(short)387, (short)304, (short)381, (short)0, (short)117, (short)116, (short)233,
(short)237, (short)243, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)264,
(short)0, (short)48, (short)49, (short)164, (short)208, (short)0, (short)182,
(short)0, (short)0, (short)372, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)245, (short)0, (short)0, (short)0, (short)253, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)236, (short)219,
(short)0, (short)351, (short)0, (short)373, (short)0, (short)234, (short)235,
(short)244, (short)246, (short)247, (short)0, (short)255, (short)254, (short)0,
(short)249, (short)0, (short)0, (short)0, (short)0, (short)237, (short)238,
(short)0, (short)0, (short)122, (short)248, (short)256, (short)250, (short)251,
(short)0, (short)0, (short)237, (short)239, (short)178, (short)352, (short)252,
(short)237, (short)240, (short)237, (short)241, (short)242, (short)0, (short)0,
(short)0}; static short Cyc_yydefgoto[ 108u]={ (short)748, (short)39, (short)40,
(short)41, (short)236, (short)42, (short)364, (short)43, (short)366, (short)44,
(short)202, (short)45, (short)46, (short)61, (short)62, (short)427, (short)428,
(short)47, (short)138, (short)48, (short)49, (short)266, (short)267, (short)50,
(short)81, (short)51, (short)208, (short)209, (short)92, (short)93, (short)94,
(short)210, (short)133, (short)376, (short)377, (short)378, (short)52, (short)53,
(short)526, (short)527, (short)528, (short)54, (short)55, (short)56, (short)57,
(short)117, (short)104, (short)423, (short)453, (short)411, (short)412, (short)278,
(short)257, (short)144, (short)258, (short)259, (short)468, (short)369, (short)469,
(short)470, (short)439, (short)440, (short)134, (short)139, (short)140, (short)454,
(short)285, (short)239, (short)240, (short)241, (short)242, (short)243, (short)244,
(short)245, (short)676, (short)246, (short)247, (short)270, (short)271, (short)272,
(short)442, (short)443, (short)444, (short)248, (short)249, (short)350, (short)175,
(short)518, (short)176, (short)177, (short)178, (short)179, (short)180, (short)181,
(short)182, (short)183, (short)184, (short)185, (short)186, (short)187, (short)188,
(short)189, (short)190, (short)191, (short)482, (short)483, (short)192, (short)193};
static short Cyc_yypact[ 751u]={ (short)1642, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)58, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)45, - (short)68, (short)2091, -
(short)-32768, - (short)68, - (short)23, - (short)-32768, - (short)-32768, -
(short)28, - (short)-32768, (short)9, - (short)-32768, (short)3, (short)43, -
(short)-32768, (short)38, (short)79, (short)194, - (short)-32768, - (short)-32768,
(short)1642, - (short)-32768, (short)95, (short)175, - (short)-32768, (short)411,
- (short)45, - (short)45, - (short)45, - (short)-32768, - (short)-32768, (short)205,
- (short)-32768, (short)190, (short)1940, (short)140, (short)151, - (short)14, -
(short)-32768, (short)206, (short)74, (short)2000, - (short)-32768, (short)236,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)2091,
- (short)-32768, (short)240, (short)259, (short)287, (short)2091, (short)299,
(short)181, - (short)-32768, - (short)-32768, (short)813, (short)66, (short)813,
- (short)-32768, (short)272, (short)2266, (short)3815, (short)3815, - (short)-32768,
(short)1642, (short)1642, (short)1642, (short)1642, - (short)-32768, (short)309,
(short)282, - (short)-32768, (short)1877, (short)2000, (short)2000, (short)2000,
(short)3, (short)2266, (short)3, (short)321, - (short)-32768, - (short)68,
(short)292, (short)324, (short)1047, - (short)-32768, (short)1940, (short)411, -
(short)-32768, (short)2140, (short)813, (short)2679, - (short)-32768, (short)140,
- (short)45, - (short)-32768, (short)1642, (short)310, - (short)-32768, -
(short)68, (short)319, (short)2091, - (short)-32768, - (short)-32768, - (short)-32768,
(short)3815, (short)306, (short)81, - (short)45, - (short)45, (short)99, -
(short)-32768, (short)21, - (short)-32768, - (short)-32768, - (short)-32768,
(short)308, (short)59, - (short)-32768, (short)489, (short)291, - (short)-32768,
(short)3957, - (short)-32768, (short)3815, (short)569, (short)336, (short)351, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)362, (short)376, (short)379, (short)4028, (short)4028, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)1405,
(short)391, (short)4099, (short)4099, - (short)-32768, (short)4099, - (short)-32768,
- (short)-32768, (short)356, - (short)-32768, - (short)39, (short)434, (short)380,
(short)390, (short)396, (short)354, (short)18, (short)363, (short)326, (short)98,
- (short)-32768, (short)562, (short)415, (short)4099, (short)77, - (short)-32768,
- (short)-32768, (short)84, (short)430, - (short)-32768, (short)432, - (short)-32768,
(short)443, - (short)-32768, (short)43, (short)2750, (short)1940, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)435, (short)445,
(short)2266, - (short)-32768, (short)257, (short)437, (short)66, (short)3, -
(short)68, (short)442, - (short)-32768, (short)452, (short)455, (short)457,
(short)2024, (short)465, (short)481, (short)473, (short)474, (short)2821,
(short)2024, (short)219, (short)2024, (short)2024, - (short)67, (short)471, -
(short)-32768, - (short)-32768, (short)479, - (short)-32768, - (short)-32768,
(short)411, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)482, (short)1168, - (short)-32768, - (short)-32768, - (short)-32768,
(short)141, - (short)-32768, - (short)-32768, - (short)-32768, (short)485, -
(short)-32768, (short)188, - (short)-32768, (short)480, (short)167, (short)486,
(short)484, (short)64, - (short)-32768, (short)483, (short)93, (short)493,
(short)14, (short)490, (short)496, (short)497, - (short)-32768, - (short)-32768,
(short)498, (short)495, (short)177, (short)667, (short)2091, - (short)41, -
(short)-32768, (short)506, (short)2266, (short)2266, (short)1817, (short)2892, -
(short)16, - (short)-32768, (short)223, - (short)-32768, (short)188, - (short)-32768,
(short)813, - (short)-32768, (short)1757, - (short)-32768, (short)120, - (short)-32768,
- (short)-32768, (short)2266, (short)1405, - (short)-32768, - (short)-32768, -
(short)-32768, (short)2323, - (short)-32768, (short)533, (short)3815, (short)1702,
(short)575, (short)3815, (short)3815, (short)1405, - (short)-32768, - (short)-32768,
(short)1168, (short)509, (short)341, (short)3815, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)4099, (short)3815, (short)4099, (short)4099,
(short)4099, (short)4099, (short)4099, (short)4099, (short)4099, (short)4099,
(short)4099, (short)4099, (short)4099, (short)4099, (short)4099, (short)4099,
(short)4099, (short)4099, (short)4099, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)3815,
(short)3815, - (short)-32768, (short)107, - (short)-32768, - (short)-32768,
(short)2963, (short)145, (short)3815, - (short)-32768, (short)2590, (short)507,
- (short)-32768, - (short)-32768, (short)1642, - (short)-32768, (short)1642, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)2266, - (short)-32768, - (short)-32768, (short)3815, (short)515, (short)511,
- (short)-32768, (short)508, (short)2266, - (short)-32768, - (short)-32768,
(short)3, (short)441, (short)3815, (short)3815, (short)3815, (short)587, (short)1289,
(short)520, - (short)-32768, - (short)-32768, - (short)-32768, (short)198,
(short)579, - (short)-32768, (short)3034, - (short)-32768, - (short)-32768,
(short)2024, (short)528, (short)2024, (short)1521, (short)1877, - (short)-32768,
- (short)-32768, - (short)-32768, (short)3815, (short)542, (short)124, (short)529,
(short)518, - (short)-32768, (short)188, (short)2252, - (short)-32768, (short)548,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)93, (short)1642, - (short)-32768, (short)546, (short)547, (short)527, -
(short)68, - (short)-32768, (short)3815, - (short)-32768, (short)2091, - (short)-32768,
- (short)-32768, (short)560, (short)3815, (short)2091, (short)115, - (short)-32768,
- (short)-32768, - (short)-32768, (short)551, (short)549, (short)552, (short)66,
(short)563, - (short)-32768, - (short)-32768, - (short)-32768, (short)188, -
(short)-32768, (short)570, (short)573, - (short)-32768, (short)553, (short)223,
(short)2154, (short)813, (short)3105, - (short)-32768, - (short)-32768, (short)308,
- (short)-32768, (short)574, (short)577, - (short)-32768, - (short)-32768,
(short)263, (short)2750, (short)355, (short)576, (short)43, (short)578, (short)357,
(short)568, (short)580, (short)581, (short)3886, - (short)-32768, - (short)-32768,
(short)585, (short)591, (short)434, (short)234, (short)380, (short)390, (short)396,
(short)354, (short)18, (short)18, (short)363, (short)363, (short)363, (short)363,
(short)326, (short)326, (short)98, (short)98, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)586, - (short)-32768, - (short)-32768,
- (short)-32768, (short)593, - (short)-32768, - (short)-32768, (short)70,
(short)276, (short)813, - (short)-32768, - (short)-32768, (short)584, - (short)-32768,
- (short)-32768, - (short)-32768, (short)257, (short)3815, (short)597, - (short)-32768,
- (short)-32768, - (short)-32768, (short)598, - (short)68, (short)208, (short)359,
(short)366, (short)370, (short)599, (short)3176, (short)3247, (short)220, -
(short)-32768, - (short)-32768, (short)600, (short)602, (short)603, - (short)-32768,
(short)595, - (short)-32768, (short)1940, - (short)-32768, - (short)-32768,
(short)66, - (short)-32768, (short)610, - (short)-32768, (short)188, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)395, (short)608, (short)14, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)601, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)2215, - (short)-32768, (short)612, - (short)-32768,
(short)611, - (short)-32768, - (short)-32768, - (short)-32768, (short)188, -
(short)-32768, (short)614, (short)65, - (short)-32768, (short)606, (short)620,
(short)613, - (short)-32768, (short)2412, - (short)-32768, - (short)-32768, -
(short)-32768, (short)1940, (short)2266, (short)569, (short)686, (short)620,
(short)621, (short)2590, - (short)-32768, - (short)-32768, (short)3815, (short)3815,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)2590,
(short)274, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)247, (short)24, (short)441, (short)2024, (short)624,
(short)2024, (short)3815, (short)3318, (short)228, (short)3389, (short)233,
(short)3460, (short)464, - (short)-32768, (short)627, (short)637, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)626, (short)644, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)563, -
(short)-32768, (short)645, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)3815, - (short)-32768, - (short)-32768, (short)2750,
(short)646, - (short)-32768, (short)533, (short)625, - (short)-32768, (short)285,
- (short)-32768, - (short)-32768, - (short)-32768, (short)648, - (short)-32768,
- (short)-32768, (short)724, (short)464, - (short)-32768, (short)394, (short)2024,
(short)398, (short)3531, (short)2024, (short)400, (short)3602, (short)3673,
(short)241, (short)2091, (short)643, (short)653, - (short)-32768, (short)2024, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)260, -
(short)-32768, (short)651, (short)2266, - (short)-32768, (short)2501, (short)2266,
(short)2024, (short)655, (short)660, - (short)-32768, (short)2024, (short)2024,
(short)412, - (short)-32768, (short)2024, (short)2024, (short)416, (short)2024,
(short)418, (short)3744, - (short)40, (short)1168, - (short)-32768, - (short)-32768,
(short)3815, - (short)-32768, (short)658, - (short)-32768, (short)425, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)2024,
- (short)-32768, - (short)-32768, (short)2024, - (short)-32768, (short)2024,
(short)2024, (short)436, (short)3815, (short)926, - (short)-32768, (short)288,
(short)662, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)2024, (short)270, (short)464, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)926, - (short)-32768, (short)464, -
(short)-32768, - (short)-32768, (short)767, (short)769, - (short)-32768}; static
short Cyc_yypgoto[ 108u]={ - (short)-32768, (short)26, - (short)-32768, (short)468,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)50, - (short)43, - (short)54, - (short)-32768, (short)103, - (short)30,
(short)210, - (short)-32768, (short)17, - (short)204, - (short)20, - (short)-32768,
- (short)-32768, (short)345, - (short)-32768, - (short)51, - (short)-32768, -
(short)115, - (short)-32768, - (short)-32768, - (short)-32768, (short)588,
(short)566, (short)76, - (short)-32768, - (short)-32768, (short)258, - (short)-32768,
- (short)-32768, - (short)69, - (short)-32768, - (short)-32768, (short)37, -
(short)46, - (short)59, - (short)-32768, - (short)-32768, (short)723, - (short)-32768,
- (short)109, - (short)387, (short)494, - (short)376, - (short)81, - (short)281,
- (short)-32768, - (short)-32768, - (short)182, - (short)142, - (short)348, -
(short)260, - (short)-32768, (short)346, - (short)144, - (short)60, - (short)100,
(short)27, - (short)175, (short)189, - (short)-32768, - (short)-32768, - (short)18,
- (short)239, - (short)-32768, - (short)-32768, - (short)542, - (short)-32768, -
(short)-32768, - (short)21, (short)512, - (short)-32768, (short)216, - (short)-32768,
- (short)-32768, (short)174, (short)114, - (short)-32768, - (short)119, -
(short)414, - (short)140, (short)469, (short)470, (short)467, (short)475,
(short)466, (short)221, (short)105, (short)218, (short)230, - (short)70, (short)407,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)275,
- (short)-32768, - (short)-32768, (short)16}; static short Cyc_yytable[ 4223u]={
(short)110, (short)74, (short)143, (short)256, (short)108, (short)406, (short)302,
(short)121, (short)303, (short)381, (short)116, (short)109, (short)512, (short)260,
(short)438, (short)464, (short)58, (short)562, (short)135, (short)368, (short)29,
(short)400, (short)313, (short)565, (short)129, (short)115, (short)31, (short)552,
(short)299, (short)524, (short)525, (short)425, (short)59, (short)726, (short)549,
(short)320, (short)111, (short)63, (short)75, (short)401, (short)76, (short)110,
(short)204, (short)205, (short)206, (short)108, (short)446, (short)123, (short)207,
(short)58, (short)212, (short)60, (short)127, (short)238, (short)216, (short)110,
(short)58, (short)237, (short)132, (short)250, (short)132, (short)58, (short)447,
(short)132, (short)574, (short)77, (short)86, (short)101, (short)727, (short)105,
(short)82, (short)573, (short)58, (short)478, (short)283, (short)321, (short)504,
(short)203, (short)78, (short)102, (short)132, (short)508, (short)95, (short)293,
(short)75, (short)281, (short)115, (short)328, (short)329, (short)75, (short)103,
(short)251, (short)132, (short)132, (short)282, (short)131, (short)79, (short)131,
(short)316, (short)317, (short)131, (short)318, (short)426, (short)58, (short)58,
(short)58, (short)58, (short)610, (short)457, (short)80, (short)15, (short)16,
(short)17, (short)195, (short)196, (short)197, (short)198, (short)131, (short)457,
(short)352, (short)214, (short)691, (short)540, - (short)115, (short)330,
(short)331, (short)58, (short)288, (short)286, (short)131, (short)131, (short)29,
(short)418, (short)643, (short)464, (short)58, (short)287, (short)31, (short)268,
(short)83, (short)75, (short)353, (short)354, (short)355, (short)33, (short)264,
(short)132, (short)252, (short)110, (short)96, (short)97, (short)98, (short)250,
(short)465, (short)136, (short)36, (short)37, (short)38, (short)58, (short)142,
(short)284, (short)137, (short)289, (short)382, (short)632, (short)477, (short)290,
(short)289, (short)289, (short)294, (short)359, (short)419, (short)644, (short)408,
(short)276, (short)120, (short)211, (short)84, (short)356, (short)292, (short)277,
(short)604, (short)360, (short)131, (short)371, (short)739, (short)357, (short)358,
(short)142, (short)132, (short)238, (short)641, (short)87, (short)88, (short)237,
(short)505, (short)744, (short)361, (short)174, (short)194, (short)281, (short)746,
(short)506, (short)747, (short)422, (short)36, (short)37, (short)38, (short)29,
(short)282, (short)336, (short)36, (short)37, (short)38, (short)31, (short)567,
(short)58, (short)276, (short)337, (short)338, (short)263, (short)291, (short)283,
(short)548, (short)436, (short)437, (short)131, (short)58, (short)262, (short)463,
(short)282, (short)383, (short)293, (short)509, (short)279, (short)280, (short)60,
(short)252, (short)407, (short)29, (short)510, (short)112, (short)572, (short)421,
(short)408, (short)31, (short)113, (short)116, (short)379, (short)655, (short)114,
(short)473, (short)33, (short)441, (short)58, (short)461, (short)517, (short)516,
(short)238, (short)132, (short)132, (short)132, (short)237, (short)681, (short)414,
(short)522, (short)500, (short)501, (short)502, (short)132, (short)415, (short)132,
(short)89, (short)90, (short)434, (short)404, (short)132, (short)132, (short)29,
- (short)84, (short)408, (short)409, - (short)84, (short)102, (short)31, (short)211,
(short)410, (short)80, (short)588, (short)132, (short)75, (short)75, (short)85,
(short)29, (short)103, (short)537, (short)131, (short)131, (short)131, (short)31,
(short)99, (short)408, (short)273, (short)100, (short)119, (short)614, (short)131,
(short)58, (short)131, (short)58, (short)438, (short)615, (short)517, (short)131,
(short)131, (short)370, (short)396, (short)624, (short)517, (short)60, (short)397,
(short)58, (short)143, (short)408, (short)458, (short)668, (short)131, (short)649,
(short)82, (short)459, (short)671, (short)408, (short)523, (short)460, (short)122,
(short)110, (short)408, (short)408, (short)703, (short)534, (short)314, (short)124,
(short)601, (short)630, (short)408, (short)29, (short)649, (short)125, - (short)84,
(short)580, (short)110, (short)31, (short)132, (short)80, (short)108, (short)449,
(short)450, (short)142, (short)33, (short)581, (short)132, (short)544, (short)586,
(short)408, (short)556, (short)375, (short)587, (short)142, (short)708, (short)36,
(short)37, (short)38, (short)142, (short)408, (short)141, (short)605, (short)126,
(short)289, (short)743, (short)606, (short)58, (short)658, (short)58, (short)132,
(short)687, (short)200, (short)545, (short)740, (short)688, (short)131, (short)514,
(short)408, (short)515, (short)295, (short)296, (short)132, (short)456, (short)131,
(short)80, (short)128, (short)394, (short)215, (short)517, (short)555, (short)274,
(short)648, (short)199, (short)275, (short)213, (short)598, (short)388, (short)265,
(short)563, (short)607, (short)217, (short)370, (short)395, (short)566, (short)398,
(short)399, (short)131, (short)269, (short)476, (short)287, (short)648, (short)326,
(short)327, (short)461, (short)649, (short)481, (short)332, (short)333, (short)131,
(short)492, (short)493, (short)494, (short)495, (short)304, (short)132, (short)132,
(short)58, (short)334, (short)335, (short)480, (short)408, (short)268, (short)524,
(short)525, (short)211, (short)75, (short)557, (short)651, (short)305, (short)652,
(short)75, (short)319, (short)211, (short)589, (short)408, (short)593, (short)408,
(short)616, (short)408, (short)306, (short)503, (short)481, (short)728, (short)684,
(short)617, (short)408, (short)481, (short)314, (short)618, (short)408, (short)370,
(short)131, (short)131, (short)307, (short)471, (short)142, (short)308, (short)475,
(short)657, (short)314, (short)633, (short)634, (short)674, (short)675, (short)738,
(short)58, (short)110, (short)142, (short)315, (short)132, (short)250, (short)485,
(short)692, (short)408, (short)323, (short)29, (short)694, (short)408, (short)698,
(short)408, (short)745, (short)31, (short)648, (short)322, (short)91, (short)324,
(short)591, (short)481, (short)33, (short)325, (short)718, (short)408, (short)351,
(short)481, (short)721, (short)408, (short)723, (short)408, (short)546, (short)36,
(short)37, (short)38, (short)629, (short)731, (short)296, (short)362, (short)131,
(short)363, (short)511, (short)372, (short)142, (short)380, (short)58, (short)110,
(short)736, (short)408, (short)384, (short)108, (short)365, (short)613, (short)373,
(short)660, (short)661, (short)490, (short)491, (short)441, (short)496, (short)497,
(short)298, (short)385, (short)300, (short)300, (short)386, (short)379, (short)387,
(short)529, (short)530, (short)531, (short)659, (short)535, (short)498, (short)499,
(short)389, (short)310, (short)311, (short)390, (short)391, (short)392, (short)132,
(short)203, (short)583, (short)300, (short)300, (short)29, (short)300, (short)402,
(short)403, (short)405, (short)413, (short)31, (short)370, (short)201, (short)75,
(short)417, (short)416, (short)541, (short)291, (short)543, (short)424, (short)429,
(short)420, (short)430, (short)300, (short)431, (short)433, (short)282, (short)432,
(short)36, (short)37, (short)38, (short)145, (short)448, (short)146, (short)320,
(short)712, (short)131, (short)474, (short)479, (short)519, (short)513, (short)520,
(short)532, (short)521, (short)536, (short)149, (short)150, (short)538, (short)542,
(short)151, (short)152, (short)153, (short)154, (short)155, (short)156, (short)157,
(short)547, (short)560, (short)550, (short)159, (short)551, (short)160, (short)161,
(short)554, (short)339, (short)340, (short)341, (short)342, (short)343, (short)344,
(short)345, (short)346, (short)347, (short)348, (short)558, (short)564, (short)559,
(short)569, (short)238, (short)570, (short)704, (short)571, (short)237, (short)276,
(short)29, (short)162, (short)163, (short)164, (short)165, (short)349, (short)31,
(short)577, (short)585, (short)132, (short)301, (short)650, (short)132, (short)166,
(short)594, (short)575, (short)238, (short)167, (short)576, (short)584, (short)237,
(short)590, (short)592, (short)596, (short)168, (short)595, (short)608, (short)169,
(short)170, (short)171, (short)599, (short)602, (short)238, (short)75, (short)172,
(short)173, (short)237, (short)600, (short)603, (short)611, (short)612, (short)625,
(short)626, (short)619, (short)370, (short)628, (short)131, (short)64, (short)627,
(short)131, (short)621, (short)623, (short)631, (short)635, (short)370, (short)637,
(short)640, (short)656, (short)639, (short)642, (short)645, (short)597, (short)646,
(short)370, (short)653, (short)663, (short)654, (short)677, (short)678, (short)686,
(short)300, (short)679, (short)300, (short)300, (short)300, (short)300, (short)300,
(short)300, (short)300, (short)300, (short)300, (short)300, (short)300, (short)300,
(short)300, (short)300, (short)300, (short)300, (short)300, (short)680, (short)682,
(short)685, (short)689, (short)690, (short)705, (short)706, (short)709, (short)714,
(short)29, (short)65, (short)715, (short)66, (short)67, (short)730, (short)31,
(short)710, (short)370, (short)741, (short)142, (short)435, (short)749, (short)68,
(short)750, (short)636, (short)69, (short)70, (short)472, (short)561, (short)374,
(short)436, (short)437, (short)609, (short)71, (short)118, (short)462, (short)300,
(short)72, (short)73, (short)568, (short)638, (short)445, (short)367, (short)484,
(short)487, (short)489, (short)486, (short)665, (short)667, (short)0, (short)670,
(short)0, (short)673, (short)488, (short)0, (short)0, (short)370, (short)0,
(short)0, (short)662, (short)0, (short)664, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)683, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)0, (short)300, (short)0, (short)0, (short)0,
(short)696, (short)0, (short)300, (short)700, (short)702, (short)0, (short)0,
(short)21, (short)0, (short)0, (short)0, (short)0, (short)0, (short)693, (short)0,
(short)0, (short)697, (short)0, (short)26, (short)27, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)707, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)0, (short)0, (short)0, (short)725, (short)0, (short)713,
(short)0, (short)0, (short)729, (short)716, (short)717, (short)0, (short)300,
(short)719, (short)720, (short)0, (short)722, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)737, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)30, (short)732, (short)32,
(short)0, (short)733, (short)130, (short)734, (short)735, (short)0, (short)0,
(short)0, (short)34, (short)35, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)742, (short)0, (short)0, (short)300, (short)1, (short)2,
(short)3, (short)106, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)674, (short)675, (short)20, (short)218, (short)0,
(short)219, (short)220, (short)221, (short)222, (short)223, (short)224, (short)225,
(short)226, (short)145, (short)21, (short)146, (short)22, (short)147, (short)227,
(short)0, (short)148, (short)23, (short)228, (short)0, (short)0, (short)26,
(short)27, (short)149, (short)150, (short)229, (short)230, (short)151, (short)152,
(short)153, (short)154, (short)155, (short)156, (short)157, (short)28, (short)231,
(short)158, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)300, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)300, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)232, (short)162, (short)163, (short)164,
(short)165, (short)30, (short)31, (short)32, (short)0, (short)233, (short)107,
(short)0, (short)0, (short)166, (short)0, (short)0, (short)34, (short)235,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)168, (short)0,
(short)0, (short)169, (short)170, (short)171, (short)0, (short)0, (short)0,
(short)0, (short)172, (short)173, (short)1, (short)2, (short)3, (short)106,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)218, (short)0, (short)219, (short)220,
(short)221, (short)222, (short)223, (short)224, (short)225, (short)226, (short)145,
(short)21, (short)146, (short)22, (short)147, (short)227, (short)0, (short)148,
(short)23, (short)228, (short)0, (short)0, (short)26, (short)27, (short)149,
(short)150, (short)229, (short)230, (short)151, (short)152, (short)153, (short)154,
(short)155, (short)156, (short)157, (short)28, (short)231, (short)158, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)232, (short)162, (short)163, (short)164, (short)165, (short)30,
(short)31, (short)32, (short)0, (short)233, (short)107, (short)234, (short)0,
(short)166, (short)0, (short)0, (short)34, (short)235, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)168, (short)0, (short)0, (short)169,
(short)170, (short)171, (short)0, (short)0, (short)0, (short)0, (short)172,
(short)173, (short)1, (short)2, (short)3, (short)106, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)218, (short)0, (short)219, (short)220, (short)221,
(short)222, (short)223, (short)224, (short)225, (short)226, (short)145, (short)21,
(short)146, (short)22, (short)147, (short)227, (short)0, (short)148, (short)23,
(short)228, (short)0, (short)0, (short)26, (short)27, (short)149, (short)150,
(short)229, (short)230, (short)151, (short)152, (short)153, (short)154, (short)155,
(short)156, (short)157, (short)28, (short)231, (short)158, (short)159, (short)0,
(short)160, (short)161, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)232, (short)162, (short)163, (short)164, (short)165, (short)30, (short)31,
(short)32, (short)0, (short)233, (short)107, (short)0, (short)0, (short)166,
(short)0, (short)0, (short)34, (short)235, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)168, (short)0, (short)0, (short)169, (short)170,
(short)171, (short)0, (short)0, (short)0, (short)0, (short)172, (short)173,
(short)1, (short)2, (short)3, (short)106, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)20,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)145, (short)21, (short)146, (short)22, (short)147,
(short)0, (short)0, (short)148, (short)23, (short)0, (short)0, (short)0, (short)26,
(short)27, (short)149, (short)150, (short)0, (short)0, (short)151, (short)152,
(short)153, (short)154, (short)155, (short)156, (short)157, (short)28, (short)0,
(short)158, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)162, (short)163, (short)164,
(short)165, (short)30, (short)31, (short)32, (short)0, (short)533, (short)0,
(short)0, (short)0, (short)166, (short)0, (short)0, (short)34, (short)235,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)168, (short)0,
(short)0, (short)169, (short)170, (short)171, (short)0, (short)0, (short)0,
(short)0, (short)172, (short)173, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)145, (short)21, (short)146, (short)0, (short)147, (short)0, (short)0,
(short)148, (short)0, (short)0, (short)0, (short)0, (short)26, (short)27,
(short)149, (short)150, (short)0, (short)0, (short)151, (short)152, (short)153,
(short)154, (short)155, (short)156, (short)157, (short)28, (short)0, (short)158,
(short)159, (short)0, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)162, (short)163, (short)164, (short)165,
(short)30, (short)31, (short)32, (short)0, (short)0, (short)312, (short)0,
(short)0, (short)166, (short)0, (short)0, (short)34, (short)235, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)168, (short)0, (short)0,
(short)169, (short)170, (short)171, (short)0, (short)0, (short)0, (short)0,
(short)172, (short)173, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)145,
(short)21, (short)146, (short)0, (short)147, (short)0, (short)0, (short)148,
(short)0, (short)0, (short)0, (short)0, (short)26, (short)27, (short)149,
(short)150, (short)0, (short)0, (short)151, (short)152, (short)153, (short)154,
(short)155, (short)156, (short)157, (short)28, (short)0, (short)158, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)162, (short)163, (short)164, (short)165, (short)30,
(short)31, (short)32, (short)0, (short)0, (short)0, (short)0, (short)0, (short)166,
(short)0, (short)0, (short)34, (short)235, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)168, (short)0, (short)0, (short)169, (short)170,
(short)171, (short)0, (short)0, (short)0, (short)0, (short)172, (short)173,
(short)1, (short)2, (short)3, (short)4, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)20,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)21, (short)0, (short)22, (short)0, (short)0,
(short)0, (short)0, (short)23, (short)0, (short)24, (short)25, (short)26,
(short)27, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)0, (short)0, (short)1,
(short)2, (short)3, (short)106, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)20, (short)0,
(short)0, (short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)30,
(short)31, (short)32, (short)21, (short)0, (short)0, (short)0, (short)0, (short)33,
(short)0, (short)23, (short)34, (short)35, (short)0, (short)26, (short)27,
(short)0, (short)0, (short)0, (short)36, (short)37, (short)38, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)0, (short)0, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)0, (short)0, (short)21, (short)0, (short)30, (short)31,
(short)32, (short)0, (short)0, (short)0, (short)0, (short)0, (short)33, (short)26,
(short)27, (short)34, (short)35, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)36, (short)37, (short)38, (short)28, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)0, (short)0, (short)0, (short)0,
(short)30, (short)31, (short)32, (short)21, (short)451, (short)0, (short)0,
(short)0, (short)291, (short)452, (short)0, (short)34, (short)35, (short)0,
(short)26, (short)27, (short)0, (short)282, (short)0, (short)36, (short)37,
(short)38, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)0,
(short)0, (short)1, (short)2, (short)3, (short)106, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)20, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)30, (short)0, (short)32, (short)21, (short)451, (short)22,
(short)0, (short)0, (short)281, (short)452, (short)23, (short)34, (short)35,
(short)0, (short)26, (short)27, (short)0, (short)282, (short)0, (short)36,
(short)37, (short)38, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)1, (short)2, (short)3,
(short)106, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)30, (short)0, (short)32, (short)0, (short)0, (short)107,
(short)21, (short)201, (short)22, (short)0, (short)0, (short)34, (short)35,
(short)23, (short)0, (short)0, (short)0, (short)26, (short)27, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)0, (short)0, (short)1, (short)2, (short)3, (short)106,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)30, (short)0, (short)32, (short)21, (short)0,
(short)107, (short)0, (short)0, (short)0, (short)0, (short)23, (short)34,
(short)35, (short)0, (short)26, (short)27, (short)218, (short)0, (short)219,
(short)220, (short)221, (short)222, (short)223, (short)224, (short)225, (short)226,
(short)145, (short)28, (short)146, (short)0, (short)147, (short)227, (short)0,
(short)148, (short)0, (short)228, (short)0, (short)0, (short)0, (short)0,
(short)149, (short)150, (short)229, (short)230, (short)151, (short)152, (short)153,
(short)154, (short)155, (short)156, (short)157, (short)0, (short)231, (short)158,
(short)159, (short)0, (short)160, (short)161, (short)0, (short)0, (short)30,
(short)0, (short)32, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)34, (short)35, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)232, (short)162, (short)163, (short)164, (short)165,
(short)0, (short)31, (short)0, (short)0, (short)233, (short)107, (short)0,
(short)0, (short)166, (short)0, (short)0, (short)64, (short)167, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)168, (short)0, (short)0,
(short)169, (short)170, (short)171, (short)0, (short)0, (short)0, (short)0,
(short)172, (short)173, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)21, (short)0, (short)0, (short)29, (short)65, (short)0,
(short)66, (short)67, (short)0, (short)31, (short)0, (short)26, (short)27,
(short)0, (short)21, (short)0, (short)68, (short)0, (short)0, (short)69, (short)70,
(short)0, (short)0, (short)0, (short)28, (short)26, (short)27, (short)71,
(short)0, (short)0, (short)0, (short)72, (short)73, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)28, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)253, (short)0, (short)0, (short)0, (short)0, (short)30, (short)0, (short)32,
(short)0, (short)254, (short)0, (short)0, (short)0, (short)0, (short)255,
(short)0, (short)34, (short)35, (short)0, (short)30, (short)0, (short)32,
(short)0, (short)578, (short)64, (short)0, (short)0, (short)0, (short)579,
(short)0, (short)34, (short)35, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)21, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)26, (short)27, (short)0, (short)21,
(short)29, (short)65, (short)0, (short)66, (short)67, (short)0, (short)31,
(short)0, (short)0, (short)28, (short)26, (short)27, (short)0, (short)68,
(short)0, (short)0, (short)69, (short)70, (short)0, (short)0, (short)0, (short)436,
(short)437, (short)28, (short)71, (short)0, (short)0, (short)0, (short)72,
(short)73, (short)0, (short)0, (short)0, (short)0, (short)553, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)30, (short)0, (short)32,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)466, (short)0, (short)0,
(short)34, (short)35, (short)145, (short)30, (short)146, (short)32, (short)147,
(short)0, (short)0, (short)148, (short)0, (short)0, (short)0, (short)0, (short)34,
(short)35, (short)149, (short)150, (short)0, (short)0, (short)151, (short)152,
(short)153, (short)154, (short)155, (short)156, (short)157, (short)0, (short)0,
(short)158, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)162, (short)163, (short)164,
(short)165, (short)0, (short)31, (short)0, (short)0, (short)0, (short)301,
(short)467, (short)0, (short)166, (short)0, (short)0, (short)0, (short)167,
(short)0, (short)0, (short)0, (short)436, (short)437, (short)0, (short)168,
(short)0, (short)0, (short)169, (short)170, (short)171, (short)0, (short)0,
(short)0, (short)0, (short)172, (short)173, (short)145, (short)0, (short)146,
(short)0, (short)147, (short)0, (short)0, (short)148, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)149, (short)150, (short)0,
(short)0, (short)151, (short)152, (short)153, (short)154, (short)155, (short)156,
(short)157, (short)0, (short)0, (short)158, (short)159, (short)0, (short)160,
(short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162,
(short)163, (short)164, (short)165, (short)0, (short)31, (short)0, (short)0,
(short)0, (short)301, (short)647, (short)0, (short)166, (short)0, (short)0,
(short)0, (short)167, (short)0, (short)0, (short)0, (short)436, (short)437,
(short)0, (short)168, (short)0, (short)0, (short)169, (short)170, (short)171,
(short)0, (short)0, (short)0, (short)0, (short)172, (short)173, (short)145,
(short)0, (short)146, (short)0, (short)147, (short)0, (short)0, (short)148,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)149, (short)150,
(short)0, (short)0, (short)151, (short)152, (short)153, (short)154, (short)155,
(short)156, (short)157, (short)0, (short)0, (short)158, (short)159, (short)0,
(short)160, (short)161, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)162, (short)163, (short)164, (short)165, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)301, (short)711, (short)0, (short)166,
(short)0, (short)0, (short)0, (short)167, (short)0, (short)0, (short)0, (short)436,
(short)437, (short)0, (short)168, (short)0, (short)0, (short)169, (short)170,
(short)171, (short)0, (short)0, (short)0, (short)0, (short)172, (short)173,
(short)145, (short)0, (short)146, (short)0, (short)147, (short)0, (short)0,
(short)148, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)149,
(short)150, (short)0, (short)0, (short)151, (short)152, (short)153, (short)154,
(short)155, (short)156, (short)157, (short)0, (short)0, (short)158, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)162, (short)163, (short)164, (short)165, (short)0,
(short)31, (short)0, (short)0, (short)0, (short)301, (short)0, (short)0, (short)166,
(short)0, (short)0, (short)0, (short)167, (short)0, (short)0, (short)0, (short)436,
(short)437, (short)0, (short)168, (short)0, (short)0, (short)169, (short)170,
(short)171, (short)0, (short)0, (short)0, (short)0, (short)172, (short)173,
(short)145, (short)0, (short)146, (short)0, (short)147, (short)0, (short)0,
(short)148, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)149,
(short)150, (short)0, (short)0, (short)151, (short)152, (short)153, (short)154,
(short)155, (short)156, (short)157, (short)0, (short)0, (short)158, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)162, (short)163, (short)164, (short)165, (short)0,
(short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)166,
(short)0, (short)0, (short)0, (short)167, (short)145, (short)0, (short)146,
(short)0, (short)147, (short)261, (short)168, (short)148, (short)0, (short)169,
(short)170, (short)171, (short)0, (short)0, (short)149, (short)150, (short)172,
(short)173, (short)151, (short)152, (short)153, (short)154, (short)155, (short)156,
(short)157, (short)0, (short)0, (short)158, (short)159, (short)0, (short)160,
(short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162,
(short)163, (short)164, (short)165, (short)0, (short)31, (short)0, (short)0,
(short)0, (short)301, (short)0, (short)0, (short)166, (short)0, (short)0,
(short)0, (short)167, (short)145, (short)0, (short)146, (short)0, (short)147,
(short)0, (short)168, (short)148, (short)0, (short)169, (short)170, (short)171,
(short)0, (short)0, (short)149, (short)150, (short)172, (short)173, (short)151,
(short)152, (short)153, (short)154, (short)155, (short)156, (short)157, (short)0,
(short)0, (short)158, (short)159, (short)0, (short)160, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)162, (short)163,
(short)164, (short)165, (short)0, (short)31, (short)0, (short)0, (short)393,
(short)0, (short)0, (short)0, (short)166, (short)0, (short)0, (short)0, (short)167,
(short)145, (short)0, (short)146, (short)0, (short)147, (short)0, (short)168,
(short)148, (short)0, (short)169, (short)170, (short)171, (short)0, (short)0,
(short)149, (short)150, (short)172, (short)173, (short)151, (short)152, (short)153,
(short)154, (short)155, (short)156, (short)157, (short)0, (short)0, (short)158,
(short)159, (short)0, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)162, (short)163, (short)164, (short)165,
(short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)166, (short)0, (short)0, (short)0, (short)167, (short)145, (short)0,
(short)146, (short)0, (short)147, (short)455, (short)168, (short)148, (short)0,
(short)169, (short)170, (short)171, (short)0, (short)0, (short)149, (short)150,
(short)172, (short)173, (short)151, (short)152, (short)153, (short)154, (short)155,
(short)156, (short)157, (short)0, (short)0, (short)158, (short)159, (short)0,
(short)160, (short)161, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)162, (short)163, (short)164, (short)165, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)166, (short)507,
(short)0, (short)0, (short)167, (short)145, (short)0, (short)146, (short)0,
(short)147, (short)0, (short)168, (short)148, (short)0, (short)169, (short)170,
(short)171, (short)0, (short)0, (short)149, (short)150, (short)172, (short)173,
(short)151, (short)152, (short)153, (short)154, (short)155, (short)156, (short)157,
(short)0, (short)0, (short)158, (short)159, (short)0, (short)160, (short)161,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162, (short)163,
(short)164, (short)165, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)166, (short)539, (short)0, (short)0,
(short)167, (short)145, (short)0, (short)146, (short)0, (short)147, (short)0,
(short)168, (short)148, (short)0, (short)169, (short)170, (short)171, (short)0,
(short)0, (short)149, (short)150, (short)172, (short)173, (short)151, (short)152,
(short)153, (short)154, (short)155, (short)156, (short)157, (short)0, (short)0,
(short)158, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)162, (short)163, (short)164,
(short)165, (short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)0, (short)0, (short)0, (short)167, (short)145,
(short)0, (short)146, (short)0, (short)147, (short)582, (short)168, (short)148,
(short)0, (short)169, (short)170, (short)171, (short)0, (short)0, (short)149,
(short)150, (short)172, (short)173, (short)151, (short)152, (short)153, (short)154,
(short)155, (short)156, (short)157, (short)0, (short)0, (short)158, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)162, (short)163, (short)164, (short)165, (short)0,
(short)31, (short)0, (short)0, (short)620, (short)0, (short)0, (short)0, (short)166,
(short)0, (short)0, (short)0, (short)167, (short)145, (short)0, (short)146,
(short)0, (short)147, (short)0, (short)168, (short)148, (short)0, (short)169,
(short)170, (short)171, (short)0, (short)0, (short)149, (short)150, (short)172,
(short)173, (short)151, (short)152, (short)153, (short)154, (short)155, (short)156,
(short)157, (short)0, (short)0, (short)158, (short)159, (short)0, (short)160,
(short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162,
(short)163, (short)164, (short)165, (short)0, (short)31, (short)0, (short)0,
(short)622, (short)0, (short)0, (short)0, (short)166, (short)0, (short)0,
(short)0, (short)167, (short)145, (short)0, (short)146, (short)0, (short)147,
(short)0, (short)168, (short)148, (short)0, (short)169, (short)170, (short)171,
(short)0, (short)0, (short)149, (short)150, (short)172, (short)173, (short)151,
(short)152, (short)153, (short)154, (short)155, (short)156, (short)157, (short)0,
(short)0, (short)158, (short)159, (short)0, (short)160, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)162, (short)163,
(short)164, (short)165, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)166, (short)666, (short)0, (short)0,
(short)167, (short)145, (short)0, (short)146, (short)0, (short)147, (short)0,
(short)168, (short)148, (short)0, (short)169, (short)170, (short)171, (short)0,
(short)0, (short)149, (short)150, (short)172, (short)173, (short)151, (short)152,
(short)153, (short)154, (short)155, (short)156, (short)157, (short)0, (short)0,
(short)158, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)162, (short)163, (short)164,
(short)165, (short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)669, (short)0, (short)0, (short)167, (short)145,
(short)0, (short)146, (short)0, (short)147, (short)0, (short)168, (short)148,
(short)0, (short)169, (short)170, (short)171, (short)0, (short)0, (short)149,
(short)150, (short)172, (short)173, (short)151, (short)152, (short)153, (short)154,
(short)155, (short)156, (short)157, (short)0, (short)0, (short)158, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)162, (short)163, (short)164, (short)165, (short)0,
(short)31, (short)0, (short)0, (short)672, (short)0, (short)0, (short)0, (short)166,
(short)0, (short)0, (short)0, (short)167, (short)145, (short)0, (short)146,
(short)0, (short)147, (short)0, (short)168, (short)148, (short)0, (short)169,
(short)170, (short)171, (short)0, (short)0, (short)149, (short)150, (short)172,
(short)173, (short)151, (short)152, (short)153, (short)154, (short)155, (short)156,
(short)157, (short)0, (short)0, (short)158, (short)159, (short)0, (short)160,
(short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162,
(short)163, (short)164, (short)165, (short)0, (short)31, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)166, (short)695, (short)0,
(short)0, (short)167, (short)145, (short)0, (short)146, (short)0, (short)147,
(short)0, (short)168, (short)148, (short)0, (short)169, (short)170, (short)171,
(short)0, (short)0, (short)149, (short)150, (short)172, (short)173, (short)151,
(short)152, (short)153, (short)154, (short)155, (short)156, (short)157, (short)0,
(short)0, (short)158, (short)159, (short)0, (short)160, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)162, (short)163,
(short)164, (short)165, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)166, (short)699, (short)0, (short)0,
(short)167, (short)145, (short)0, (short)146, (short)0, (short)147, (short)0,
(short)168, (short)148, (short)0, (short)169, (short)170, (short)171, (short)0,
(short)0, (short)149, (short)150, (short)172, (short)173, (short)151, (short)152,
(short)153, (short)154, (short)155, (short)156, (short)157, (short)0, (short)0,
(short)158, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)162, (short)163, (short)164,
(short)165, (short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)701, (short)0, (short)0, (short)167, (short)145,
(short)0, (short)146, (short)0, (short)147, (short)0, (short)168, (short)148,
(short)0, (short)169, (short)170, (short)171, (short)0, (short)0, (short)149,
(short)150, (short)172, (short)173, (short)151, (short)152, (short)153, (short)154,
(short)155, (short)156, (short)157, (short)0, (short)0, (short)158, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)162, (short)163, (short)164, (short)165, (short)0,
(short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)166,
(short)724, (short)0, (short)0, (short)167, (short)145, (short)0, (short)146,
(short)0, (short)147, (short)0, (short)168, (short)148, (short)0, (short)169,
(short)170, (short)171, (short)0, (short)0, (short)149, (short)150, (short)172,
(short)173, (short)151, (short)152, (short)153, (short)154, (short)155, (short)156,
(short)157, (short)0, (short)0, (short)158, (short)159, (short)0, (short)160,
(short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162,
(short)163, (short)164, (short)165, (short)0, (short)31, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)166, (short)0, (short)0, (short)0,
(short)167, (short)145, (short)0, (short)146, (short)0, (short)0, (short)0,
(short)168, (short)0, (short)0, (short)169, (short)170, (short)171, (short)0,
(short)0, (short)149, (short)150, (short)172, (short)173, (short)151, (short)152,
(short)153, (short)154, (short)155, (short)156, (short)157, (short)0, (short)0,
(short)0, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)162, (short)163, (short)164,
(short)165, (short)0, (short)31, (short)0, (short)0, (short)0, (short)597,
(short)0, (short)0, (short)166, (short)0, (short)0, (short)0, (short)167,
(short)145, (short)0, (short)146, (short)0, (short)0, (short)0, (short)168,
(short)0, (short)0, (short)169, (short)170, (short)171, (short)0, (short)0,
(short)149, (short)150, (short)172, (short)173, (short)151, (short)152, (short)153,
(short)154, (short)155, (short)156, (short)157, (short)0, (short)0, (short)0,
(short)159, (short)0, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)162, (short)163, (short)164, (short)165,
(short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)297, (short)0, (short)0, (short)0, (short)167, (short)145, (short)0,
(short)146, (short)0, (short)0, (short)0, (short)168, (short)0, (short)0,
(short)169, (short)170, (short)171, (short)0, (short)0, (short)149, (short)150,
(short)172, (short)173, (short)151, (short)152, (short)153, (short)154, (short)155,
(short)156, (short)157, (short)0, (short)0, (short)0, (short)159, (short)0,
(short)160, (short)161, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)162, (short)163, (short)164, (short)165, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)309, (short)0,
(short)0, (short)0, (short)167, (short)145, (short)0, (short)146, (short)0,
(short)0, (short)0, (short)168, (short)0, (short)0, (short)169, (short)170,
(short)171, (short)0, (short)0, (short)149, (short)150, (short)172, (short)173,
(short)151, (short)152, (short)153, (short)154, (short)155, (short)156, (short)157,
(short)0, (short)0, (short)0, (short)159, (short)0, (short)160, (short)161,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162, (short)163,
(short)164, (short)165, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)166, (short)0, (short)0, (short)0, (short)167,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)168, (short)0,
(short)0, (short)169, (short)170, (short)171, (short)0, (short)0, (short)0,
(short)0, (short)172, (short)173}; static short Cyc_yycheck[ 4223u]={ (short)54,
(short)22, (short)83, (short)112, (short)54, (short)244, (short)148, (short)61,
(short)148, (short)213, (short)56, (short)54, (short)360, (short)113, (short)274,
(short)296, (short)0, (short)431, (short)78, (short)201, (short)88, (short)88,
(short)166, (short)437, (short)75, (short)55, (short)94, (short)414, (short)147,
(short)5, (short)6, (short)17, (short)90, (short)73, (short)410, (short)74,
(short)54, (short)21, (short)22, (short)106, (short)24, (short)95, (short)96,
(short)97, (short)98, (short)95, (short)87, (short)68, (short)99, (short)33,
(short)101, (short)96, (short)73, (short)107, (short)105, (short)109, (short)40,
(short)107, (short)78, (short)109, (short)80, (short)45, (short)103, (short)83,
(short)451, (short)88, (short)40, (short)51, (short)108, (short)53, (short)33,
(short)447, (short)56, (short)312, (short)133, (short)114, (short)351, (short)95,
(short)106, (short)93, (short)100, (short)356, (short)45, (short)142, (short)68,
(short)101, (short)116, (short)69, (short)70, (short)73, (short)104, (short)109,
(short)112, (short)113, (short)110, (short)78, (short)87, (short)80, (short)168,
(short)169, (short)83, (short)171, (short)88, (short)87, (short)88, (short)89,
(short)90, (short)521, (short)283, (short)106, (short)17, (short)18, (short)19,
(short)87, (short)88, (short)89, (short)90, (short)100, (short)293, (short)189,
(short)104, (short)663, (short)397, (short)99, (short)106, (short)107, (short)110,
(short)68, (short)107, (short)112, (short)113, (short)88, (short)68, (short)68,
(short)415, (short)119, (short)115, (short)94, (short)122, (short)101, (short)124,
(short)64, (short)65, (short)66, (short)101, (short)119, (short)166, (short)110,
(short)202, (short)46, (short)47, (short)48, (short)202, (short)297, (short)88,
(short)112, (short)113, (short)114, (short)142, (short)83, (short)133, (short)95,
(short)103, (short)214, (short)551, (short)309, (short)107, (short)103, (short)103,
(short)142, (short)86, (short)107, (short)107, (short)103, (short)93, (short)101,
(short)100, (short)98, (short)101, (short)142, (short)99, (short)111, (short)98,
(short)166, (short)202, (short)727, (short)109, (short)110, (short)112, (short)209,
(short)244, (short)578, (short)97, (short)98, (short)244, (short)88, (short)738,
(short)113, (short)84, (short)85, (short)101, (short)743, (short)95, (short)745,
(short)263, (short)112, (short)113, (short)114, (short)88, (short)110, (short)112,
(short)112, (short)113, (short)114, (short)94, (short)100, (short)200, (short)93,
(short)120, (short)121, (short)117, (short)101, (short)281, (short)99, (short)109,
(short)110, (short)209, (short)211, (short)114, (short)289, (short)110, (short)215,
(short)291, (short)88, (short)131, (short)132, (short)96, (short)200, (short)97,
(short)88, (short)95, (short)101, (short)446, (short)263, (short)103, (short)94,
(short)106, (short)293, (short)211, (short)597, (short)110, (short)305, (short)101,
(short)274, (short)238, (short)285, (short)375, (short)372, (short)312, (short)279,
(short)280, (short)281, (short)312, (short)639, (short)97, (short)380, (short)336,
(short)337, (short)338, (short)289, (short)103, (short)291, (short)97, (short)98,
(short)97, (short)238, (short)296, (short)297, (short)88, (short)98, (short)103,
(short)93, (short)101, (short)93, (short)94, (short)209, (short)98, (short)106,
(short)470, (short)309, (short)274, (short)275, (short)98, (short)88, (short)104,
(short)97, (short)279, (short)280, (short)281, (short)94, (short)95, (short)103,
(short)128, (short)98, (short)98, (short)97, (short)289, (short)291, (short)291,
(short)293, (short)570, (short)103, (short)431, (short)296, (short)297, (short)201,
(short)97, (short)97, (short)437, (short)96, (short)101, (short)305, (short)403,
(short)103, (short)101, (short)97, (short)309, (short)587, (short)291, (short)106,
(short)97, (short)103, (short)383, (short)110, (short)98, (short)389, (short)103,
(short)103, (short)97, (short)389, (short)166, (short)101, (short)108, (short)547,
(short)103, (short)88, (short)606, (short)88, (short)101, (short)458, (short)404,
(short)94, (short)372, (short)106, (short)404, (short)279, (short)280, (short)281,
(short)101, (short)459, (short)380, (short)404, (short)99, (short)103, (short)423,
(short)108, (short)103, (short)291, (short)108, (short)112, (short)113, (short)114,
(short)296, (short)103, (short)102, (short)99, (short)89, (short)103, (short)108,
(short)103, (short)364, (short)107, (short)366, (short)403, (short)99, (short)103,
(short)404, (short)99, (short)103, (short)372, (short)364, (short)103, (short)366,
(short)102, (short)103, (short)415, (short)282, (short)380, (short)106, (short)100,
(short)226, (short)109, (short)521, (short)423, (short)98, (short)587, (short)97,
(short)101, (short)87, (short)479, (short)221, (short)101, (short)433, (short)513,
(short)90, (short)301, (short)227, (short)438, (short)229, (short)230, (short)403,
(short)102, (short)308, (short)115, (short)606, (short)71, (short)72, (short)457,
(short)688, (short)315, (short)67, (short)68, (short)415, (short)328, (short)329,
(short)330, (short)331, (short)101, (short)458, (short)459, (short)424, (short)115,
(short)116, (short)102, (short)103, (short)429, (short)5, (short)6, (short)372,
(short)433, (short)424, (short)593, (short)101, (short)593, (short)438, (short)99,
(short)380, (short)102, (short)103, (short)102, (short)103, (short)102, (short)103,
(short)101, (short)350, (short)351, (short)705, (short)649, (short)102, (short)103,
(short)356, (short)297, (short)102, (short)103, (short)360, (short)458, (short)459,
(short)101, (short)304, (short)403, (short)101, (short)307, (short)601, (short)309,
(short)89, (short)90, (short)22, (short)23, (short)727, (short)473, (short)544,
(short)415, (short)101, (short)513, (short)544, (short)321, (short)102, (short)103,
(short)118, (short)88, (short)102, (short)103, (short)102, (short)103, (short)743,
(short)94, (short)688, (short)73, (short)97, (short)119, (short)473, (short)397,
(short)101, (short)117, (short)102, (short)103, (short)101, (short)403, (short)102,
(short)103, (short)102, (short)103, (short)408, (short)112, (short)113, (short)114,
(short)544, (short)102, (short)103, (short)99, (short)513, (short)99, (short)358,
(short)98, (short)458, (short)98, (short)520, (short)591, (short)102, (short)103,
(short)98, (short)591, (short)99, (short)527, (short)99, (short)614, (short)615,
(short)326, (short)327, (short)570, (short)332, (short)333, (short)145, (short)101,
(short)147, (short)148, (short)101, (short)520, (short)101, (short)385, (short)386,
(short)387, (short)613, (short)389, (short)334, (short)335, (short)101, (short)160,
(short)161, (short)88, (short)97, (short)97, (short)592, (short)591, (short)460,
(short)168, (short)169, (short)88, (short)171, (short)108, (short)101, (short)99,
(short)102, (short)94, (short)470, (short)100, (short)570, (short)103, (short)102,
(short)400, (short)101, (short)402, (short)99, (short)103, (short)111, (short)99,
(short)189, (short)100, (short)103, (short)110, (short)102, (short)112, (short)113,
(short)114, (short)35, (short)99, (short)37, (short)74, (short)689, (short)592,
(short)35, (short)102, (short)97, (short)106, (short)103, (short)28, (short)108,
(short)97, (short)49, (short)50, (short)41, (short)93, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, (short)87, (short)103,
(short)102, (short)63, (short)115, (short)65, (short)66, (short)88, (short)75,
(short)76, (short)77, (short)78, (short)79, (short)80, (short)81, (short)82,
(short)83, (short)84, (short)101, (short)88, (short)102, (short)99, (short)705,
(short)103, (short)674, (short)102, (short)705, (short)93, (short)88, (short)89,
(short)90, (short)91, (short)92, (short)100, (short)94, (short)111, (short)88,
(short)686, (short)98, (short)592, (short)689, (short)101, (short)103, (short)102,
(short)727, (short)105, (short)102, (short)102, (short)727, (short)102, (short)101,
(short)99, (short)112, (short)102, (short)99, (short)115, (short)116, (short)117,
(short)102, (short)102, (short)743, (short)674, (short)122, (short)123, (short)743,
(short)103, (short)102, (short)99, (short)99, (short)98, (short)97, (short)101,
(short)587, (short)107, (short)686, (short)37, (short)102, (short)689, (short)533,
(short)534, (short)99, (short)102, (short)597, (short)111, (short)102, (short)600,
(short)103, (short)102, (short)111, (short)98, (short)106, (short)606, (short)35,
(short)98, (short)102, (short)97, (short)88, (short)101, (short)320, (short)102,
(short)322, (short)323, (short)324, (short)325, (short)326, (short)327, (short)328,
(short)329, (short)330, (short)331, (short)332, (short)333, (short)334, (short)335,
(short)336, (short)337, (short)338, (short)102, (short)102, (short)102, (short)101,
(short)26, (short)108, (short)99, (short)102, (short)99, (short)88, (short)89,
(short)97, (short)91, (short)92, (short)102, (short)94, (short)686, (short)649,
(short)102, (short)689, (short)99, (short)0, (short)101, (short)0, (short)560,
(short)104, (short)105, (short)305, (short)429, (short)209, (short)109, (short)110,
(short)520, (short)112, (short)57, (short)287, (short)375, (short)116, (short)117,
(short)439, (short)570, (short)275, (short)200, (short)320, (short)323, (short)325,
(short)322, (short)619, (short)620, - (short)1, (short)622, - (short)1, (short)624,
(short)324, - (short)1, - (short)1, (short)688, - (short)1, - (short)1, (short)616,
- (short)1, (short)618, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)646, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, (short)431, - (short)1,
- (short)1, - (short)1, (short)668, - (short)1, (short)437, (short)671, (short)672,
- (short)1, - (short)1, (short)36, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)666, - (short)1, - (short)1, (short)669, - (short)1, (short)47,
(short)48, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)678,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)60, - (short)1,
- (short)1, - (short)1, (short)703, - (short)1, (short)690, - (short)1, -
(short)1, (short)708, (short)694, (short)695, - (short)1, (short)479, (short)698,
(short)699, - (short)1, (short)701, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)726,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)93, (short)718,
(short)95, - (short)1, (short)721, (short)98, (short)723, (short)724, - (short)1,
- (short)1, - (short)1, (short)104, (short)105, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)736, - (short)1, - (short)1,
(short)521, (short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, (short)22, (short)23,
(short)24, (short)25, - (short)1, (short)27, (short)28, (short)29, (short)30,
(short)31, (short)32, (short)33, (short)34, (short)35, (short)36, (short)37,
(short)38, (short)39, (short)40, - (short)1, (short)42, (short)43, (short)44, -
(short)1, - (short)1, (short)47, (short)48, (short)49, (short)50, (short)51,
(short)52, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, (short)60, (short)61, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)593, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)601, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92,
(short)93, (short)94, (short)95, - (short)1, (short)97, (short)98, - (short)1, -
(short)1, (short)101, - (short)1, - (short)1, (short)104, (short)105, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)112, -
(short)1, - (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1,
- (short)1, - (short)1, (short)122, (short)123, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, (short)25, - (short)1, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)36, (short)37, (short)38, (short)39, (short)40, - (short)1,
(short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, (short)60, (short)61, (short)62,
(short)63, - (short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89,
(short)90, (short)91, (short)92, (short)93, (short)94, (short)95, - (short)1,
(short)97, (short)98, (short)99, - (short)1, (short)101, - (short)1, - (short)1,
(short)104, (short)105, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116,
(short)117, - (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24,
(short)25, - (short)1, (short)27, (short)28, (short)29, (short)30, (short)31,
(short)32, (short)33, (short)34, (short)35, (short)36, (short)37, (short)38,
(short)39, (short)40, - (short)1, (short)42, (short)43, (short)44, - (short)1, -
(short)1, (short)47, (short)48, (short)49, (short)50, (short)51, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59,
(short)60, (short)61, (short)62, (short)63, - (short)1, (short)65, (short)66, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, (short)93,
(short)94, (short)95, - (short)1, (short)97, (short)98, - (short)1, - (short)1,
(short)101, - (short)1, - (short)1, (short)104, (short)105, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)112, - (short)1,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, -
(short)1, - (short)1, (short)122, (short)123, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)35, (short)36, (short)37, (short)38, (short)39, - (short)1, -
(short)1, (short)42, (short)43, - (short)1, - (short)1, - (short)1, (short)47,
(short)48, (short)49, (short)50, - (short)1, - (short)1, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, (short)60, - (short)1,
(short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88,
(short)89, (short)90, (short)91, (short)92, (short)93, (short)94, (short)95, -
(short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101, - (short)1,
- (short)1, (short)104, (short)105, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, - (short)1, - (short)1, (short)122,
(short)123, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)35, (short)36, (short)37, - (short)1, (short)39, - (short)1, -
(short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1, (short)47,
(short)48, (short)49, (short)50, - (short)1, - (short)1, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, (short)60, - (short)1,
(short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88,
(short)89, (short)90, (short)91, (short)92, (short)93, (short)94, (short)95, -
(short)1, - (short)1, (short)98, - (short)1, - (short)1, (short)101, - (short)1,
- (short)1, (short)104, (short)105, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, - (short)1, - (short)1, (short)122,
(short)123, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)35, (short)36, (short)37, - (short)1, (short)39, - (short)1, -
(short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1, (short)47,
(short)48, (short)49, (short)50, - (short)1, - (short)1, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, (short)59, (short)60, - (short)1,
(short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88,
(short)89, (short)90, (short)91, (short)92, (short)93, (short)94, (short)95, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)101, - (short)1,
- (short)1, (short)104, (short)105, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, - (short)1, - (short)1, (short)122,
(short)123, (short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1,
(short)24, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1,
(short)38, - (short)1, - (short)1, - (short)1, - (short)1, (short)43, - (short)1,
(short)45, (short)46, (short)47, (short)48, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)60, - (short)1, - (short)1, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1,
- (short)1, (short)88, - (short)1, - (short)1, - (short)1, - (short)1, (short)93,
(short)94, (short)95, (short)36, - (short)1, - (short)1, - (short)1, - (short)1,
(short)101, - (short)1, (short)43, (short)104, (short)105, - (short)1, (short)47,
(short)48, - (short)1, - (short)1, - (short)1, (short)112, (short)113, (short)114,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)60, - (short)1,
- (short)1, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, - (short)1,
- (short)1, (short)36, - (short)1, (short)93, (short)94, (short)95, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)101, (short)47, (short)48,
(short)104, (short)105, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)112, (short)113, (short)114, (short)60, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, - (short)1,
- (short)1, - (short)1, - (short)1, (short)93, (short)94, (short)95, (short)36,
(short)97, - (short)1, - (short)1, - (short)1, (short)101, (short)102, - (short)1,
(short)104, (short)105, - (short)1, (short)47, (short)48, - (short)1, (short)110,
- (short)1, (short)112, (short)113, (short)114, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)60, - (short)1, - (short)1, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)93, - (short)1, (short)95, (short)36, (short)97, (short)38, -
(short)1, - (short)1, (short)101, (short)102, (short)43, (short)104, (short)105,
- (short)1, (short)47, (short)48, - (short)1, (short)110, - (short)1, (short)112,
(short)113, (short)114, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)60, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)93, - (short)1,
(short)95, - (short)1, - (short)1, (short)98, (short)36, (short)100, (short)38,
- (short)1, - (short)1, (short)104, (short)105, (short)43, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)60, - (short)1, - (short)1, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)93, - (short)1,
(short)95, (short)36, - (short)1, (short)98, - (short)1, - (short)1, - (short)1,
- (short)1, (short)43, (short)104, (short)105, - (short)1, (short)47, (short)48,
(short)25, - (short)1, (short)27, (short)28, (short)29, (short)30, (short)31,
(short)32, (short)33, (short)34, (short)35, (short)60, (short)37, - (short)1,
(short)39, (short)40, - (short)1, (short)42, - (short)1, (short)44, - (short)1,
- (short)1, - (short)1, - (short)1, (short)49, (short)50, (short)51, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59, -
(short)1, (short)61, (short)62, (short)63, - (short)1, (short)65, (short)66, -
(short)1, - (short)1, (short)93, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
(short)105, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)88, (short)89, (short)90, (short)91, (short)92, - (short)1,
(short)94, - (short)1, - (short)1, (short)97, (short)98, - (short)1, - (short)1,
(short)101, - (short)1, - (short)1, (short)37, (short)105, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)112, - (short)1, -
(short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, - (short)1,
- (short)1, (short)122, (short)123, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, (short)36, - (short)1, - (short)1, (short)88,
(short)89, - (short)1, (short)91, (short)92, - (short)1, (short)94, - (short)1,
(short)47, (short)48, - (short)1, (short)36, - (short)1, (short)101, - (short)1,
- (short)1, (short)104, (short)105, - (short)1, - (short)1, - (short)1, (short)60,
(short)47, (short)48, (short)112, - (short)1, - (short)1, - (short)1, (short)116,
(short)117, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)60,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, - (short)1, - (short)1, - (short)1, - (short)1, (short)93, - (short)1,
(short)95, - (short)1, (short)97, - (short)1, - (short)1, - (short)1, - (short)1,
(short)102, - (short)1, (short)104, (short)105, - (short)1, (short)93, - (short)1,
(short)95, - (short)1, (short)97, (short)37, - (short)1, - (short)1, - (short)1,
(short)102, - (short)1, (short)104, (short)105, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, (short)36, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, - (short)1, (short)36, (short)88, (short)89, -
(short)1, (short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1,
(short)60, (short)47, (short)48, - (short)1, (short)101, - (short)1, - (short)1,
(short)104, (short)105, - (short)1, - (short)1, - (short)1, (short)109, (short)110,
(short)60, (short)112, - (short)1, - (short)1, - (short)1, (short)116, (short)117,
- (short)1, - (short)1, - (short)1, - (short)1, (short)85, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)93, - (short)1,
(short)95, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)30,
- (short)1, - (short)1, (short)104, (short)105, (short)35, (short)93, (short)37,
(short)95, (short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1,
- (short)1, - (short)1, (short)104, (short)105, (short)49, (short)50, - (short)1,
- (short)1, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, (short)98, (short)99, -
(short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, - (short)1,
- (short)1, - (short)1, (short)109, (short)110, - (short)1, (short)112, -
(short)1, - (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1,
- (short)1, - (short)1, (short)122, (short)123, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)49, (short)50, - (short)1,
- (short)1, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, (short)98, (short)99, -
(short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, - (short)1,
- (short)1, - (short)1, (short)109, (short)110, - (short)1, (short)112, -
(short)1, - (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1,
- (short)1, - (short)1, (short)122, (short)123, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)49, (short)50, - (short)1,
- (short)1, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, (short)98, (short)99, -
(short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, - (short)1,
- (short)1, - (short)1, (short)109, (short)110, - (short)1, (short)112, -
(short)1, - (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1,
- (short)1, - (short)1, (short)122, (short)123, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)49, (short)50, - (short)1,
- (short)1, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, (short)98, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, -
(short)1, - (short)1, - (short)1, (short)109, (short)110, - (short)1, (short)112,
- (short)1, - (short)1, (short)115, (short)116, (short)117, - (short)1, -
(short)1, - (short)1, - (short)1, (short)122, (short)123, (short)35, - (short)1,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49, (short)50,
- (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1,
(short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91,
(short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105,
(short)35, - (short)1, (short)37, - (short)1, (short)39, (short)111, (short)112,
(short)42, - (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1,
(short)49, (short)50, (short)122, (short)123, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62,
(short)63, - (short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89,
(short)90, (short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1,
- (short)1, (short)98, - (short)1, - (short)1, (short)101, - (short)1, - (short)1,
- (short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39,
- (short)1, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101,
- (short)1, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)112, (short)42, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, (short)39, (short)111, (short)112, (short)42,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, (short)102, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39, -
(short)1, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)101,
(short)102, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)112, (short)42, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, (short)39, (short)111, (short)112, (short)42,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, (short)97,
- (short)1, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39, -
(short)1, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101,
- (short)1, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)112, (short)42, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)101, (short)102, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)112, (short)42,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, (short)102, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39, -
(short)1, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, (short)97, - (short)1, - (short)1, - (short)1, (short)101,
- (short)1, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)112, (short)42, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)101, (short)102, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)112, (short)42,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, (short)102, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39, -
(short)1, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)101,
(short)102, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)112, (short)42, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)101, (short)102, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, (short)112, (short)42,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, - (short)1,
- (short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, - (short)1, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, - (short)1, (short)98, - (short)1, - (short)1, (short)101,
- (short)1, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, - (short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, - (short)1, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)35,
- (short)1, (short)37, - (short)1, - (short)1, - (short)1, (short)112, - (short)1,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, (short)49,
(short)50, (short)122, (short)123, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, - (short)1, (short)63,
- (short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, - (short)1,
- (short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, - (short)1, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)101,
- (short)1, - (short)1, - (short)1, (short)105, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)112, - (short)1, - (short)1,
(short)115, (short)116, (short)117, - (short)1, - (short)1, - (short)1, -
(short)1, (short)122, (short)123}; char Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible";
char Cyc_Yystack_overflow[ 21u]="\000\000\000\000Yystack_overflow"; extern void
Cyc_yyerror( struct _tagged_string); extern int Cyc_yylex(); char Cyc_YYINITIALSVAL[
18u]="\000\000\000\000YYINITIALSVAL"; static int Cyc_yychar=( int)'\000'; void*
Cyc_yylval=( void*) Cyc_YYINITIALSVAL; static int Cyc_yynerrs= 0; struct
_tagged_ptr1{ short* curr; short* base; short* last_plus_one; } ; struct
_tagged_ptr2{ void** curr; void** base; void** last_plus_one; } ; struct
_tagged_ptr3{ struct Cyc_Yyltype* curr; struct Cyc_Yyltype* base; struct Cyc_Yyltype*
last_plus_one; } ; struct _tuple18{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; int Cyc_yyparse(){ int yystate; int yyn= 0; int yyerrstatus; int yychar1=
0; int yyssp_offset; struct _tagged_ptr1 yyss=( struct _tagged_ptr1)({ short*
_temp7488=( short*)({ unsigned int _temp7484= 200u; short* _temp7485=( short*)
GC_malloc_atomic( sizeof( short) * _temp7484);{ unsigned int _temp7486=
_temp7484; unsigned int i; for( i= 0; i < _temp7486; i ++){ _temp7485[ i]=
(short)0;}}; _temp7485;}); struct _tagged_ptr1 _temp7489; _temp7489.curr=
_temp7488; _temp7489.base= _temp7488; _temp7489.last_plus_one= _temp7488 + 200;
_temp7489;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=( struct _tagged_ptr2)({
void** _temp7482=( void**)({ unsigned int _temp7478= 200u; void** _temp7479=(
void**) GC_malloc( sizeof( void*) * _temp7478);{ unsigned int _temp7480=
_temp7478; unsigned int i; for( i= 0; i < _temp7480; i ++){ _temp7479[ i]= Cyc_yylval;}};
_temp7479;}); struct _tagged_ptr2 _temp7483; _temp7483.curr= _temp7482;
_temp7483.base= _temp7482; _temp7483.last_plus_one= _temp7482 + 200; _temp7483;});
int yylsp_offset; struct _tagged_ptr3 yyls=( struct _tagged_ptr3)({ struct Cyc_Yyltype*
_temp7476=( struct Cyc_Yyltype*)({ unsigned int _temp7472= 200u; struct Cyc_Yyltype*
_temp7473=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp7472);{ unsigned int _temp7474= _temp7472; unsigned int i; for( i= 0; i <
_temp7474; i ++){ _temp7473[ i]= Cyc_yynewloc();}}; _temp7473;}); struct
_tagged_ptr3 _temp7477; _temp7477.curr= _temp7476; _temp7477.base= _temp7476;
_temp7477.last_plus_one= _temp7476 + 200; _temp7477;}); int yystacksize= 200;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:({
struct _tagged_ptr1 _temp2202= yyss; short* _temp2204= _temp2202.curr +( ++
yyssp_offset); if( _temp2202.base == 0? 1:( _temp2204 < _temp2202.base? 1:
_temp2204 >= _temp2202.last_plus_one)){ _throw( Null_Exception);}* _temp2204=(
short) yystate;}); if( yyssp_offset >= yystacksize - 1){ if( yystacksize >=
10000){ Cyc_yyerror(( struct _tagged_string)({ char* _temp2205=( char*)"parser stack overflow";
struct _tagged_string _temp2206; _temp2206.curr= _temp2205; _temp2206.base=
_temp2205; _temp2206.last_plus_one= _temp2205 + 22; _temp2206;}));( void) _throw((
void*) Cyc_Yystack_overflow);} yystacksize *= 2; if( yystacksize > 10000){
yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp2235=(
unsigned int) yystacksize; short* _temp2236=( short*) GC_malloc_atomic( sizeof(
short) * _temp2235); struct _tagged_ptr1 _temp2239={ _temp2236, _temp2236,
_temp2236 + _temp2235};{ unsigned int _temp2237= _temp2235; unsigned int i; for(
i= 0; i < _temp2237; i ++){ _temp2236[ i]= (short)0;}}; _temp2239;}); struct
_tagged_ptr2 yyvs1=({ unsigned int _temp2230=( unsigned int) yystacksize; void**
_temp2231=( void**) GC_malloc( sizeof( void*) * _temp2230); struct _tagged_ptr2
_temp2234={ _temp2231, _temp2231, _temp2231 + _temp2230};{ unsigned int
_temp2232= _temp2230; unsigned int i; for( i= 0; i < _temp2232; i ++){ _temp2231[
i]= Cyc_yylval;}}; _temp2234;}); struct _tagged_ptr3 yyls1=({ unsigned int
_temp2225=( unsigned int) yystacksize; struct Cyc_Yyltype* _temp2226=( struct
Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) * _temp2225); struct
_tagged_ptr3 _temp2229={ _temp2226, _temp2226, _temp2226 + _temp2225};{
unsigned int _temp2227= _temp2225; unsigned int i; for( i= 0; i < _temp2227; i
++){ _temp2226[ i]= Cyc_yynewloc();}}; _temp2229;});{ int i= 0; for( 0; i <=
yyssp_offset; i ++){({ struct _tagged_ptr1 _temp2207= yyss1; short* _temp2209=
_temp2207.curr + i; if( _temp2207.base == 0? 1:( _temp2209 < _temp2207.base? 1:
_temp2209 >= _temp2207.last_plus_one)){ _throw( Null_Exception);}* _temp2209=({
struct _tagged_ptr1 _temp2210= yyss; short* _temp2212= _temp2210.curr + i; if(
_temp2210.base == 0? 1:( _temp2212 < _temp2210.base? 1: _temp2212 >= _temp2210.last_plus_one)){
_throw( Null_Exception);}* _temp2212;});});({ struct _tagged_ptr2 _temp2213=
yyvs1; void** _temp2215= _temp2213.curr + i; if( _temp2213.base == 0? 1:(
_temp2215 < _temp2213.base? 1: _temp2215 >= _temp2213.last_plus_one)){ _throw(
Null_Exception);}* _temp2215=({ struct _tagged_ptr2 _temp2216= yyvs; void**
_temp2218= _temp2216.curr + i; if( _temp2216.base == 0? 1:( _temp2218 <
_temp2216.base? 1: _temp2218 >= _temp2216.last_plus_one)){ _throw(
Null_Exception);}* _temp2218;});});({ struct _tagged_ptr3 _temp2219= yyls1;
struct Cyc_Yyltype* _temp2221= _temp2219.curr + i; if( _temp2219.base == 0? 1:(
_temp2221 < _temp2219.base? 1: _temp2221 >= _temp2219.last_plus_one)){ _throw(
Null_Exception);}* _temp2221=({ struct _tagged_ptr3 _temp2222= yyls; struct Cyc_Yyltype*
_temp2224= _temp2222.curr + i; if( _temp2222.base == 0? 1:( _temp2224 <
_temp2222.base? 1: _temp2224 >= _temp2222.last_plus_one)){ _throw(
Null_Exception);}* _temp2224;});});}} yyss= yyss1; yyvs= yyvs1; yyls= yyls1;}}
goto yybackup; yybackup: yyn=( int)({ short* _temp2240=( short*) Cyc_yypact;
unsigned int _temp2241= yystate; if( _temp2241 >= 751u){ _throw( Null_Exception);}
_temp2240[ _temp2241];}); if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar
== - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar=
0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 351: 0)?( int)({ short*
_temp2242=( short*) Cyc_yytranslate; unsigned int _temp2243= Cyc_yychar; if(
_temp2243 >= 352u){ _throw( Null_Exception);} _temp2242[ _temp2243];}): 232;}
yyn += yychar1; if(( yyn < 0? 1: yyn > 4222)? 1:({ short* _temp2244=( short*)
Cyc_yycheck; unsigned int _temp2245= yyn; if( _temp2245 >= 4223u){ _throw(
Null_Exception);} _temp2244[ _temp2245];}) != yychar1){ goto yydefault;} yyn=(
int)({ short* _temp2246=( short*) Cyc_yytable; unsigned int _temp2247= yyn; if(
_temp2247 >= 4223u){ _throw( Null_Exception);} _temp2246[ _temp2247];}); if( yyn
< 0){ if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if(
yyn == 0){ goto yyerrlab;}} if( yyn == 750){ return 0;} if( Cyc_yychar != 0){
Cyc_yychar= - 2;}({ struct _tagged_ptr2 _temp2248= yyvs; void** _temp2250=
_temp2248.curr +( ++ yyvsp_offset); if( _temp2248.base == 0? 1:( _temp2250 <
_temp2248.base? 1: _temp2250 >= _temp2248.last_plus_one)){ _throw(
Null_Exception);}* _temp2250= Cyc_yylval;});({ struct _tagged_ptr3 _temp2251=
yyls; struct Cyc_Yyltype* _temp2253= _temp2251.curr +( ++ yylsp_offset); if(
_temp2251.base == 0? 1:( _temp2253 < _temp2251.base? 1: _temp2253 >= _temp2251.last_plus_one)){
_throw( Null_Exception);}* _temp2253= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp2254=( short*) Cyc_yydefact; unsigned int _temp2255= yystate; if( _temp2255
>= 751u){ _throw( Null_Exception);} _temp2254[ _temp2255];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp2256=( short*) Cyc_yyr2;
unsigned int _temp2257= yyn; if( _temp2257 >= 394u){ _throw( Null_Exception);}
_temp2256[ _temp2257];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp2258=
yyvs; void** _temp2260= _temp2258.curr +(( yyvsp_offset + 1) - yylen); if(
_temp2258.base == 0? 1:( _temp2260 < _temp2258.base? 1: _temp2260 >= _temp2258.last_plus_one)){
_throw( Null_Exception);}* _temp2260;});} switch( yyn){ case 1: _LL2261: yyval=({
struct _tagged_ptr2 _temp2263= yyvs; void** _temp2265= _temp2263.curr +
yyvsp_offset; if( _temp2263.base == 0? 1:( _temp2265 < _temp2263.base? 1:
_temp2265 >= _temp2263.last_plus_one)){ _throw( Null_Exception);}* _temp2265;});
Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2266=
yyvs; void** _temp2268= _temp2266.curr + yyvsp_offset; if( _temp2266.base == 0?
1:( _temp2268 < _temp2266.base? 1: _temp2268 >= _temp2266.last_plus_one)){
_throw( Null_Exception);}* _temp2268;})); break; case 2: _LL2262: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp2270=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2270[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2271; _temp2271.tag= Cyc_DeclList_tok; _temp2271.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2272= yyvs; void** _temp2274= _temp2272.curr +(
yyvsp_offset - 1); if( _temp2272.base == 0? 1:( _temp2274 < _temp2272.base? 1:
_temp2274 >= _temp2272.last_plus_one)){ _throw( Null_Exception);}* _temp2274;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2275= yyvs; void** _temp2277=
_temp2275.curr + yyvsp_offset; if( _temp2275.base == 0? 1:( _temp2277 <
_temp2275.base? 1: _temp2277 >= _temp2275.last_plus_one)){ _throw(
Null_Exception);}* _temp2277;}))); _temp2271;}); _temp2270;}); break; case 3:
_LL2269: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2279=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2279[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2280; _temp2280.tag= Cyc_DeclList_tok; _temp2280.f1=({ struct Cyc_List_List*
_temp2281=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2281->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2282=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2282->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp2289=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2289[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp2290; _temp2290.tag= Cyc_Absyn_Using_d; _temp2290.f1=
Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2291= yyvs; void** _temp2293=
_temp2291.curr +( yyvsp_offset - 2); if( _temp2291.base == 0? 1:( _temp2293 <
_temp2291.base? 1: _temp2293 >= _temp2291.last_plus_one)){ _throw(
Null_Exception);}* _temp2293;})); _temp2290.f2= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2294= yyvs; void** _temp2296= _temp2294.curr + yyvsp_offset;
if( _temp2294.base == 0? 1:( _temp2296 < _temp2294.base? 1: _temp2296 >=
_temp2294.last_plus_one)){ _throw( Null_Exception);}* _temp2296;})); _temp2290;});
_temp2289;})); _temp2282->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2283= yyls; struct Cyc_Yyltype* _temp2285= _temp2283.curr +(
yylsp_offset - 2); if( _temp2283.base == 0? 1:( _temp2285 < _temp2283.base? 1:
_temp2285 >= _temp2283.last_plus_one)){ _throw( Null_Exception);}* _temp2285;}).first_line,({
struct _tagged_ptr3 _temp2286= yyls; struct Cyc_Yyltype* _temp2288= _temp2286.curr
+ yylsp_offset; if( _temp2286.base == 0? 1:( _temp2288 < _temp2286.base? 1:
_temp2288 >= _temp2286.last_plus_one)){ _throw( Null_Exception);}* _temp2288;}).last_line);
_temp2282;}); _temp2281->tl= 0; _temp2281;}); _temp2280;}); _temp2279;}); Cyc_Lex_leave_using();
break; case 4: _LL2278: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2298=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2298[ 0]=({ struct Cyc_DeclList_tok_struct _temp2299; _temp2299.tag= Cyc_DeclList_tok;
_temp2299.f1=({ struct Cyc_List_List* _temp2300=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2300->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2304=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2304->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2311=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2311[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2312; _temp2312.tag= Cyc_Absyn_Using_d;
_temp2312.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2313= yyvs; void**
_temp2315= _temp2313.curr +( yyvsp_offset - 4); if( _temp2313.base == 0? 1:(
_temp2315 < _temp2313.base? 1: _temp2315 >= _temp2313.last_plus_one)){ _throw(
Null_Exception);}* _temp2315;})); _temp2312.f2= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2316= yyvs; void** _temp2318= _temp2316.curr +( yyvsp_offset -
2); if( _temp2316.base == 0? 1:( _temp2318 < _temp2316.base? 1: _temp2318 >=
_temp2316.last_plus_one)){ _throw( Null_Exception);}* _temp2318;})); _temp2312;});
_temp2311;})); _temp2304->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2305= yyls; struct Cyc_Yyltype* _temp2307= _temp2305.curr +(
yylsp_offset - 4); if( _temp2305.base == 0? 1:( _temp2307 < _temp2305.base? 1:
_temp2307 >= _temp2305.last_plus_one)){ _throw( Null_Exception);}* _temp2307;}).first_line,({
struct _tagged_ptr3 _temp2308= yyls; struct Cyc_Yyltype* _temp2310= _temp2308.curr
+( yylsp_offset - 1); if( _temp2308.base == 0? 1:( _temp2310 < _temp2308.base? 1:
_temp2310 >= _temp2308.last_plus_one)){ _throw( Null_Exception);}* _temp2310;}).last_line);
_temp2304;}); _temp2300->tl= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2301= yyvs; void** _temp2303= _temp2301.curr + yyvsp_offset; if( _temp2301.base
== 0? 1:( _temp2303 < _temp2301.base? 1: _temp2303 >= _temp2301.last_plus_one)){
_throw( Null_Exception);}* _temp2303;})); _temp2300;}); _temp2299;}); _temp2298;});
break; case 5: _LL2297: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2320=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2320[ 0]=({ struct Cyc_DeclList_tok_struct _temp2321; _temp2321.tag= Cyc_DeclList_tok;
_temp2321.f1=({ struct Cyc_List_List* _temp2322=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2322->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2323=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2323->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2330=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2330[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2331; _temp2331.tag=
Cyc_Absyn_Namespace_d; _temp2331.f1=({ struct _tagged_string* _temp2332=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2332[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2333= yyvs; void** _temp2335=
_temp2333.curr +( yyvsp_offset - 2); if( _temp2333.base == 0? 1:( _temp2335 <
_temp2333.base? 1: _temp2335 >= _temp2333.last_plus_one)){ _throw(
Null_Exception);}* _temp2335;})); _temp2332;}); _temp2331.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2336= yyvs; void** _temp2338= _temp2336.curr +
yyvsp_offset; if( _temp2336.base == 0? 1:( _temp2338 < _temp2336.base? 1:
_temp2338 >= _temp2336.last_plus_one)){ _throw( Null_Exception);}* _temp2338;}));
_temp2331;}); _temp2330;})); _temp2323->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2324= yyls; struct Cyc_Yyltype* _temp2326= _temp2324.curr
+( yylsp_offset - 2); if( _temp2324.base == 0? 1:( _temp2326 < _temp2324.base? 1:
_temp2326 >= _temp2324.last_plus_one)){ _throw( Null_Exception);}* _temp2326;}).first_line,({
struct _tagged_ptr3 _temp2327= yyls; struct Cyc_Yyltype* _temp2329= _temp2327.curr
+ yylsp_offset; if( _temp2327.base == 0? 1:( _temp2329 < _temp2327.base? 1:
_temp2329 >= _temp2327.last_plus_one)){ _throw( Null_Exception);}* _temp2329;}).last_line);
_temp2323;}); _temp2322->tl= 0; _temp2322;}); _temp2321;}); _temp2320;}); Cyc_Lex_leave_namespace();
break; case 6: _LL2319: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2340=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2340[ 0]=({ struct Cyc_DeclList_tok_struct _temp2341; _temp2341.tag= Cyc_DeclList_tok;
_temp2341.f1=({ struct Cyc_List_List* _temp2342=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2342->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2346=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2346->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2353=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2353[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2354; _temp2354.tag=
Cyc_Absyn_Namespace_d; _temp2354.f1=({ struct _tagged_string* _temp2355=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2355[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2356= yyvs; void** _temp2358=
_temp2356.curr +( yyvsp_offset - 4); if( _temp2356.base == 0? 1:( _temp2358 <
_temp2356.base? 1: _temp2358 >= _temp2356.last_plus_one)){ _throw(
Null_Exception);}* _temp2358;})); _temp2355;}); _temp2354.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2359= yyvs; void** _temp2361= _temp2359.curr +(
yyvsp_offset - 2); if( _temp2359.base == 0? 1:( _temp2361 < _temp2359.base? 1:
_temp2361 >= _temp2359.last_plus_one)){ _throw( Null_Exception);}* _temp2361;}));
_temp2354;}); _temp2353;})); _temp2346->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2347= yyls; struct Cyc_Yyltype* _temp2349= _temp2347.curr
+( yylsp_offset - 4); if( _temp2347.base == 0? 1:( _temp2349 < _temp2347.base? 1:
_temp2349 >= _temp2347.last_plus_one)){ _throw( Null_Exception);}* _temp2349;}).first_line,({
struct _tagged_ptr3 _temp2350= yyls; struct Cyc_Yyltype* _temp2352= _temp2350.curr
+( yylsp_offset - 1); if( _temp2350.base == 0? 1:( _temp2352 < _temp2350.base? 1:
_temp2352 >= _temp2350.last_plus_one)){ _throw( Null_Exception);}* _temp2352;}).last_line);
_temp2346;}); _temp2342->tl= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2343= yyvs; void** _temp2345= _temp2343.curr + yyvsp_offset; if( _temp2343.base
== 0? 1:( _temp2345 < _temp2343.base? 1: _temp2345 >= _temp2343.last_plus_one)){
_throw( Null_Exception);}* _temp2345;})); _temp2342;}); _temp2341;}); _temp2340;});
break; case 7: _LL2339: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2363= yyvs; void** _temp2365= _temp2363.curr +( yyvsp_offset -
4); if( _temp2363.base == 0? 1:( _temp2365 < _temp2363.base? 1: _temp2365 >=
_temp2363.last_plus_one)){ _throw( Null_Exception);}* _temp2365;})),( struct
_tagged_string)({ char* _temp2366=( char*)"C"; struct _tagged_string _temp2367;
_temp2367.curr= _temp2366; _temp2367.base= _temp2366; _temp2367.last_plus_one=
_temp2366 + 2; _temp2367;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2368=( char*)"only extern \"C\" { ... } is allowed"; struct
_tagged_string _temp2369; _temp2369.curr= _temp2368; _temp2369.base= _temp2368;
_temp2369.last_plus_one= _temp2368 + 35; _temp2369;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2370= yyls; struct Cyc_Yyltype* _temp2372= _temp2370.curr
+( yylsp_offset - 5); if( _temp2370.base == 0? 1:( _temp2372 < _temp2370.base? 1:
_temp2372 >= _temp2370.last_plus_one)){ _throw( Null_Exception);}* _temp2372;}).first_line,({
struct _tagged_ptr3 _temp2373= yyls; struct Cyc_Yyltype* _temp2375= _temp2373.curr
+( yylsp_offset - 4); if( _temp2373.base == 0? 1:( _temp2375 < _temp2373.base? 1:
_temp2375 >= _temp2373.last_plus_one)){ _throw( Null_Exception);}* _temp2375;}).last_line));}
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2376=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2376[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2377; _temp2377.tag= Cyc_DeclList_tok; _temp2377.f1=({ struct Cyc_List_List*
_temp2378=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2378->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2382=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2382->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp2389=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp2389[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp2390; _temp2390.tag= Cyc_Absyn_ExternC_d;
_temp2390.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2391= yyvs;
void** _temp2393= _temp2391.curr +( yyvsp_offset - 2); if( _temp2391.base == 0?
1:( _temp2393 < _temp2391.base? 1: _temp2393 >= _temp2391.last_plus_one)){
_throw( Null_Exception);}* _temp2393;})); _temp2390;}); _temp2389;})); _temp2382->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2383= yyls; struct Cyc_Yyltype*
_temp2385= _temp2383.curr +( yylsp_offset - 5); if( _temp2383.base == 0? 1:(
_temp2385 < _temp2383.base? 1: _temp2385 >= _temp2383.last_plus_one)){ _throw(
Null_Exception);}* _temp2385;}).first_line,({ struct _tagged_ptr3 _temp2386=
yyls; struct Cyc_Yyltype* _temp2388= _temp2386.curr +( yylsp_offset - 1); if(
_temp2386.base == 0? 1:( _temp2388 < _temp2386.base? 1: _temp2388 >= _temp2386.last_plus_one)){
_throw( Null_Exception);}* _temp2388;}).last_line); _temp2382;}); _temp2378->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2379= yyvs; void** _temp2381=
_temp2379.curr + yyvsp_offset; if( _temp2379.base == 0? 1:( _temp2381 <
_temp2379.base? 1: _temp2381 >= _temp2379.last_plus_one)){ _throw(
Null_Exception);}* _temp2381;})); _temp2378;}); _temp2377;}); _temp2376;});
break; case 8: _LL2362: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2395=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2395[ 0]=({ struct Cyc_DeclList_tok_struct _temp2396; _temp2396.tag= Cyc_DeclList_tok;
_temp2396.f1= 0; _temp2396;}); _temp2395;}); break; case 9: _LL2394: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp2398=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2398[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2399; _temp2399.tag= Cyc_DeclList_tok; _temp2399.f1=({ struct Cyc_List_List*
_temp2400=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2400->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2401=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2401[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2402; _temp2402.tag= Cyc_Absyn_Fn_d;
_temp2402.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp2403= yyvs; void**
_temp2405= _temp2403.curr + yyvsp_offset; if( _temp2403.base == 0? 1:( _temp2405
< _temp2403.base? 1: _temp2405 >= _temp2403.last_plus_one)){ _throw(
Null_Exception);}* _temp2405;})); _temp2402;}); _temp2401;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2406= yyls; struct Cyc_Yyltype* _temp2408= _temp2406.curr
+ yylsp_offset; if( _temp2406.base == 0? 1:( _temp2408 < _temp2406.base? 1:
_temp2408 >= _temp2406.last_plus_one)){ _throw( Null_Exception);}* _temp2408;}).first_line,({
struct _tagged_ptr3 _temp2409= yyls; struct Cyc_Yyltype* _temp2411= _temp2409.curr
+ yylsp_offset; if( _temp2409.base == 0? 1:( _temp2411 < _temp2409.base? 1:
_temp2411 >= _temp2409.last_plus_one)){ _throw( Null_Exception);}* _temp2411;}).last_line));
_temp2400->tl= 0; _temp2400;}); _temp2399;}); _temp2398;}); break; case 10:
_LL2397: yyval=({ struct _tagged_ptr2 _temp2413= yyvs; void** _temp2415=
_temp2413.curr + yyvsp_offset; if( _temp2413.base == 0? 1:( _temp2415 <
_temp2413.base? 1: _temp2415 >= _temp2413.last_plus_one)){ _throw(
Null_Exception);}* _temp2415;}); break; case 11: _LL2412: yyval=( void*)({
struct Cyc_FnDecl_tok_struct* _temp2417=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2417[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2418; _temp2418.tag= Cyc_FnDecl_tok; _temp2418.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2419= yyvs; void**
_temp2421= _temp2419.curr +( yyvsp_offset - 1); if( _temp2419.base == 0? 1:(
_temp2421 < _temp2419.base? 1: _temp2421 >= _temp2419.last_plus_one)){ _throw(
Null_Exception);}* _temp2421;})), 0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2422= yyvs; void** _temp2424= _temp2422.curr + yyvsp_offset; if( _temp2422.base
== 0? 1:( _temp2424 < _temp2422.base? 1: _temp2424 >= _temp2422.last_plus_one)){
_throw( Null_Exception);}* _temp2424;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2425= yyls; struct Cyc_Yyltype* _temp2427= _temp2425.curr +(
yylsp_offset - 1); if( _temp2425.base == 0? 1:( _temp2427 < _temp2425.base? 1:
_temp2427 >= _temp2425.last_plus_one)){ _throw( Null_Exception);}* _temp2427;}).first_line,({
struct _tagged_ptr3 _temp2428= yyls; struct Cyc_Yyltype* _temp2430= _temp2428.curr
+ yylsp_offset; if( _temp2428.base == 0? 1:( _temp2430 < _temp2428.base? 1:
_temp2430 >= _temp2428.last_plus_one)){ _throw( Null_Exception);}* _temp2430;}).last_line));
_temp2418;}); _temp2417;}); break; case 12: _LL2416: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2432=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2432[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2433; _temp2433.tag= Cyc_FnDecl_tok;
_temp2433.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2434=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2434->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2435= yyvs; void** _temp2437=
_temp2435.curr +( yyvsp_offset - 2); if( _temp2435.base == 0? 1:( _temp2437 <
_temp2435.base? 1: _temp2437 >= _temp2435.last_plus_one)){ _throw(
Null_Exception);}* _temp2437;})); _temp2434;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2438= yyvs; void** _temp2440= _temp2438.curr +(
yyvsp_offset - 1); if( _temp2438.base == 0? 1:( _temp2440 < _temp2438.base? 1:
_temp2440 >= _temp2438.last_plus_one)){ _throw( Null_Exception);}* _temp2440;})),
0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2441= yyvs; void** _temp2443=
_temp2441.curr + yyvsp_offset; if( _temp2441.base == 0? 1:( _temp2443 <
_temp2441.base? 1: _temp2443 >= _temp2441.last_plus_one)){ _throw(
Null_Exception);}* _temp2443;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2444= yyls; struct Cyc_Yyltype* _temp2446= _temp2444.curr +(
yylsp_offset - 2); if( _temp2444.base == 0? 1:( _temp2446 < _temp2444.base? 1:
_temp2446 >= _temp2444.last_plus_one)){ _throw( Null_Exception);}* _temp2446;}).first_line,({
struct _tagged_ptr3 _temp2447= yyls; struct Cyc_Yyltype* _temp2449= _temp2447.curr
+ yylsp_offset; if( _temp2447.base == 0? 1:( _temp2449 < _temp2447.base? 1:
_temp2449 >= _temp2447.last_plus_one)){ _throw( Null_Exception);}* _temp2449;}).last_line));
_temp2433;}); _temp2432;}); break; case 13: _LL2431: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2451=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2451[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2452; _temp2452.tag= Cyc_FnDecl_tok;
_temp2452.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2453= yyvs; void** _temp2455= _temp2453.curr +( yyvsp_offset -
2); if( _temp2453.base == 0? 1:( _temp2455 < _temp2453.base? 1: _temp2455 >=
_temp2453.last_plus_one)){ _throw( Null_Exception);}* _temp2455;})), Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2456= yyvs; void** _temp2458= _temp2456.curr +(
yyvsp_offset - 1); if( _temp2456.base == 0? 1:( _temp2458 < _temp2456.base? 1:
_temp2458 >= _temp2456.last_plus_one)){ _throw( Null_Exception);}* _temp2458;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2459= yyvs; void** _temp2461=
_temp2459.curr + yyvsp_offset; if( _temp2459.base == 0? 1:( _temp2461 <
_temp2459.base? 1: _temp2461 >= _temp2459.last_plus_one)){ _throw(
Null_Exception);}* _temp2461;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2462= yyls; struct Cyc_Yyltype* _temp2464= _temp2462.curr +(
yylsp_offset - 2); if( _temp2462.base == 0? 1:( _temp2464 < _temp2462.base? 1:
_temp2464 >= _temp2462.last_plus_one)){ _throw( Null_Exception);}* _temp2464;}).first_line,({
struct _tagged_ptr3 _temp2465= yyls; struct Cyc_Yyltype* _temp2467= _temp2465.curr
+ yylsp_offset; if( _temp2465.base == 0? 1:( _temp2467 < _temp2465.base? 1:
_temp2467 >= _temp2465.last_plus_one)){ _throw( Null_Exception);}* _temp2467;}).last_line));
_temp2452;}); _temp2451;}); break; case 14: _LL2450: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2469=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2469[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2470; _temp2470.tag= Cyc_FnDecl_tok;
_temp2470.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2471=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2471->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2472= yyvs; void** _temp2474=
_temp2472.curr +( yyvsp_offset - 3); if( _temp2472.base == 0? 1:( _temp2474 <
_temp2472.base? 1: _temp2474 >= _temp2472.last_plus_one)){ _throw(
Null_Exception);}* _temp2474;})); _temp2471;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2475= yyvs; void** _temp2477= _temp2475.curr +(
yyvsp_offset - 2); if( _temp2475.base == 0? 1:( _temp2477 < _temp2475.base? 1:
_temp2477 >= _temp2475.last_plus_one)){ _throw( Null_Exception);}* _temp2477;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2478= yyvs; void** _temp2480=
_temp2478.curr +( yyvsp_offset - 1); if( _temp2478.base == 0? 1:( _temp2480 <
_temp2478.base? 1: _temp2480 >= _temp2478.last_plus_one)){ _throw(
Null_Exception);}* _temp2480;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2481= yyvs; void** _temp2483= _temp2481.curr + yyvsp_offset; if( _temp2481.base
== 0? 1:( _temp2483 < _temp2481.base? 1: _temp2483 >= _temp2481.last_plus_one)){
_throw( Null_Exception);}* _temp2483;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2484= yyls; struct Cyc_Yyltype* _temp2486= _temp2484.curr +(
yylsp_offset - 3); if( _temp2484.base == 0? 1:( _temp2486 < _temp2484.base? 1:
_temp2486 >= _temp2484.last_plus_one)){ _throw( Null_Exception);}* _temp2486;}).first_line,({
struct _tagged_ptr3 _temp2487= yyls; struct Cyc_Yyltype* _temp2489= _temp2487.curr
+ yylsp_offset; if( _temp2487.base == 0? 1:( _temp2489 < _temp2487.base? 1:
_temp2489 >= _temp2487.last_plus_one)){ _throw( Null_Exception);}* _temp2489;}).last_line));
_temp2470;}); _temp2469;}); break; case 15: _LL2468: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2491=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2491[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2492; _temp2492.tag= Cyc_FnDecl_tok;
_temp2492.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2493=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2493->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2494= yyvs; void** _temp2496=
_temp2494.curr +( yyvsp_offset - 2); if( _temp2494.base == 0? 1:( _temp2496 <
_temp2494.base? 1: _temp2496 >= _temp2494.last_plus_one)){ _throw(
Null_Exception);}* _temp2496;})); _temp2493;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2497= yyvs; void** _temp2499= _temp2497.curr +(
yyvsp_offset - 1); if( _temp2497.base == 0? 1:( _temp2499 < _temp2497.base? 1:
_temp2499 >= _temp2497.last_plus_one)){ _throw( Null_Exception);}* _temp2499;})),
0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2500= yyvs; void** _temp2502=
_temp2500.curr + yyvsp_offset; if( _temp2500.base == 0? 1:( _temp2502 <
_temp2500.base? 1: _temp2502 >= _temp2500.last_plus_one)){ _throw(
Null_Exception);}* _temp2502;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2503= yyls; struct Cyc_Yyltype* _temp2505= _temp2503.curr +(
yylsp_offset - 2); if( _temp2503.base == 0? 1:( _temp2505 < _temp2503.base? 1:
_temp2505 >= _temp2503.last_plus_one)){ _throw( Null_Exception);}* _temp2505;}).first_line,({
struct _tagged_ptr3 _temp2506= yyls; struct Cyc_Yyltype* _temp2508= _temp2506.curr
+ yylsp_offset; if( _temp2506.base == 0? 1:( _temp2508 < _temp2506.base? 1:
_temp2508 >= _temp2506.last_plus_one)){ _throw( Null_Exception);}* _temp2508;}).last_line));
_temp2492;}); _temp2491;}); break; case 16: _LL2490: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2510=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2510[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2511; _temp2511.tag= Cyc_FnDecl_tok;
_temp2511.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2512=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2512->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2513= yyvs; void** _temp2515=
_temp2513.curr +( yyvsp_offset - 3); if( _temp2513.base == 0? 1:( _temp2515 <
_temp2513.base? 1: _temp2515 >= _temp2513.last_plus_one)){ _throw(
Null_Exception);}* _temp2515;})); _temp2512;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2516= yyvs; void** _temp2518= _temp2516.curr +(
yyvsp_offset - 2); if( _temp2516.base == 0? 1:( _temp2518 < _temp2516.base? 1:
_temp2518 >= _temp2516.last_plus_one)){ _throw( Null_Exception);}* _temp2518;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2519= yyvs; void** _temp2521=
_temp2519.curr +( yyvsp_offset - 1); if( _temp2519.base == 0? 1:( _temp2521 <
_temp2519.base? 1: _temp2521 >= _temp2519.last_plus_one)){ _throw(
Null_Exception);}* _temp2521;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2522= yyvs; void** _temp2524= _temp2522.curr + yyvsp_offset; if( _temp2522.base
== 0? 1:( _temp2524 < _temp2522.base? 1: _temp2524 >= _temp2522.last_plus_one)){
_throw( Null_Exception);}* _temp2524;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2525= yyls; struct Cyc_Yyltype* _temp2527= _temp2525.curr +(
yylsp_offset - 3); if( _temp2525.base == 0? 1:( _temp2527 < _temp2525.base? 1:
_temp2527 >= _temp2525.last_plus_one)){ _throw( Null_Exception);}* _temp2527;}).first_line,({
struct _tagged_ptr3 _temp2528= yyls; struct Cyc_Yyltype* _temp2530= _temp2528.curr
+ yylsp_offset; if( _temp2528.base == 0? 1:( _temp2530 < _temp2528.base? 1:
_temp2530 >= _temp2528.last_plus_one)){ _throw( Null_Exception);}* _temp2530;}).last_line));
_temp2511;}); _temp2510;}); break; case 17: _LL2509: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2532= yyvs; void** _temp2534= _temp2532.curr +
yyvsp_offset; if( _temp2532.base == 0? 1:( _temp2534 < _temp2532.base? 1:
_temp2534 >= _temp2532.last_plus_one)){ _throw( Null_Exception);}* _temp2534;})));
yyval=({ struct _tagged_ptr2 _temp2535= yyvs; void** _temp2537= _temp2535.curr +
yyvsp_offset; if( _temp2535.base == 0? 1:( _temp2537 < _temp2535.base? 1:
_temp2537 >= _temp2535.last_plus_one)){ _throw( Null_Exception);}* _temp2537;});
break; case 18: _LL2531: Cyc_Lex_leave_using(); break; case 19: _LL2538: Cyc_Lex_enter_namespace(({
struct _tagged_string* _temp2540=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2540[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2541= yyvs; void** _temp2543= _temp2541.curr + yyvsp_offset;
if( _temp2541.base == 0? 1:( _temp2543 < _temp2541.base? 1: _temp2543 >=
_temp2541.last_plus_one)){ _throw( Null_Exception);}* _temp2543;})); _temp2540;}));
yyval=({ struct _tagged_ptr2 _temp2544= yyvs; void** _temp2546= _temp2544.curr +
yyvsp_offset; if( _temp2544.base == 0? 1:( _temp2546 < _temp2544.base? 1:
_temp2546 >= _temp2544.last_plus_one)){ _throw( Null_Exception);}* _temp2546;});
break; case 20: _LL2539: Cyc_Lex_leave_namespace(); break; case 21: _LL2547:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2549=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2549[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2550; _temp2550.tag= Cyc_DeclList_tok; _temp2550.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2551= yyvs; void** _temp2553=
_temp2551.curr +( yyvsp_offset - 1); if( _temp2551.base == 0? 1:( _temp2553 <
_temp2551.base? 1: _temp2553 >= _temp2551.last_plus_one)){ _throw(
Null_Exception);}* _temp2553;})), 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2554= yyls; struct Cyc_Yyltype* _temp2556= _temp2554.curr +(
yylsp_offset - 1); if( _temp2554.base == 0? 1:( _temp2556 < _temp2554.base? 1:
_temp2556 >= _temp2554.last_plus_one)){ _throw( Null_Exception);}* _temp2556;}).first_line,({
struct _tagged_ptr3 _temp2557= yyls; struct Cyc_Yyltype* _temp2559= _temp2557.curr
+( yylsp_offset - 1); if( _temp2557.base == 0? 1:( _temp2559 < _temp2557.base? 1:
_temp2559 >= _temp2557.last_plus_one)){ _throw( Null_Exception);}* _temp2559;}).last_line));
_temp2550;}); _temp2549;}); break; case 22: _LL2548: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2561=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2561[ 0]=({ struct Cyc_DeclList_tok_struct _temp2562; _temp2562.tag= Cyc_DeclList_tok;
_temp2562.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2563= yyvs; void** _temp2565= _temp2563.curr +( yyvsp_offset -
2); if( _temp2563.base == 0? 1:( _temp2565 < _temp2563.base? 1: _temp2565 >=
_temp2563.last_plus_one)){ _throw( Null_Exception);}* _temp2565;})), Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp2566= yyvs; void** _temp2568= _temp2566.curr +(
yyvsp_offset - 1); if( _temp2566.base == 0? 1:( _temp2568 < _temp2566.base? 1:
_temp2568 >= _temp2566.last_plus_one)){ _throw( Null_Exception);}* _temp2568;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2569= yyls; struct Cyc_Yyltype*
_temp2571= _temp2569.curr +( yylsp_offset - 2); if( _temp2569.base == 0? 1:(
_temp2571 < _temp2569.base? 1: _temp2571 >= _temp2569.last_plus_one)){ _throw(
Null_Exception);}* _temp2571;}).first_line,({ struct _tagged_ptr3 _temp2572=
yyls; struct Cyc_Yyltype* _temp2574= _temp2572.curr + yylsp_offset; if(
_temp2572.base == 0? 1:( _temp2574 < _temp2572.base? 1: _temp2574 >= _temp2572.last_plus_one)){
_throw( Null_Exception);}* _temp2574;}).last_line)); _temp2562;}); _temp2561;});
break; case 23: _LL2560: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2576=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2576[ 0]=({ struct Cyc_DeclList_tok_struct _temp2577; _temp2577.tag= Cyc_DeclList_tok;
_temp2577.f1=({ struct Cyc_List_List* _temp2578=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2578->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp2579= yyvs; void** _temp2581=
_temp2579.curr +( yyvsp_offset - 3); if( _temp2579.base == 0? 1:( _temp2581 <
_temp2579.base? 1: _temp2581 >= _temp2579.last_plus_one)){ _throw(
Null_Exception);}* _temp2581;})), 0, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp2582= yyvs; void** _temp2584= _temp2582.curr +( yyvsp_offset - 1); if(
_temp2582.base == 0? 1:( _temp2584 < _temp2582.base? 1: _temp2584 >= _temp2582.last_plus_one)){
_throw( Null_Exception);}* _temp2584;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2585= yyls; struct Cyc_Yyltype* _temp2587= _temp2585.curr +(
yylsp_offset - 4); if( _temp2585.base == 0? 1:( _temp2587 < _temp2585.base? 1:
_temp2587 >= _temp2585.last_plus_one)){ _throw( Null_Exception);}* _temp2587;}).first_line,({
struct _tagged_ptr3 _temp2588= yyls; struct Cyc_Yyltype* _temp2590= _temp2588.curr
+ yylsp_offset; if( _temp2588.base == 0? 1:( _temp2590 < _temp2588.base? 1:
_temp2590 >= _temp2588.last_plus_one)){ _throw( Null_Exception);}* _temp2590;}).last_line));
_temp2578->tl= 0; _temp2578;}); _temp2577;}); _temp2576;}); break; case 24:
_LL2575: yyval=({ struct _tagged_ptr2 _temp2592= yyvs; void** _temp2594=
_temp2592.curr + yyvsp_offset; if( _temp2592.base == 0? 1:( _temp2594 <
_temp2592.base? 1: _temp2594 >= _temp2592.last_plus_one)){ _throw(
Null_Exception);}* _temp2594;}); break; case 25: _LL2591: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp2596=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2596[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2597; _temp2597.tag= Cyc_DeclList_tok; _temp2597.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2598= yyvs; void** _temp2600= _temp2598.curr +(
yyvsp_offset - 1); if( _temp2598.base == 0? 1:( _temp2600 < _temp2598.base? 1:
_temp2600 >= _temp2598.last_plus_one)){ _throw( Null_Exception);}* _temp2600;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2601= yyvs; void** _temp2603=
_temp2601.curr + yyvsp_offset; if( _temp2601.base == 0? 1:( _temp2603 <
_temp2601.base? 1: _temp2603 >= _temp2601.last_plus_one)){ _throw(
Null_Exception);}* _temp2603;}))); _temp2597;}); _temp2596;}); break; case 26:
_LL2595: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2605=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2605[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2606; _temp2606.tag= Cyc_DeclSpec_tok; _temp2606.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2607=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2607->sc=({ struct Cyc_Core_Opt* _temp2611=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2611->v=( void*) Cyc_yyget_StorageClass_tok(({
struct _tagged_ptr2 _temp2612= yyvs; void** _temp2614= _temp2612.curr +(
yyvsp_offset - 1); if( _temp2612.base == 0? 1:( _temp2614 < _temp2612.base? 1:
_temp2614 >= _temp2612.last_plus_one)){ _throw( Null_Exception);}* _temp2614;}));
_temp2611;}); _temp2607->tq= Cyc_Absyn_empty_tqual(); _temp2607->type_specs= 0;
_temp2607->is_inline= 0; _temp2607->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2608= yyvs; void** _temp2610= _temp2608.curr +
yyvsp_offset; if( _temp2608.base == 0? 1:( _temp2610 < _temp2608.base? 1:
_temp2610 >= _temp2608.last_plus_one)){ _throw( Null_Exception);}* _temp2610;}));
_temp2607;}); _temp2606;}); _temp2605;}); break; case 27: _LL2604: if(( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2616= yyvs; void** _temp2618= _temp2616.curr +
yyvsp_offset; if( _temp2616.base == 0? 1:( _temp2618 < _temp2616.base? 1:
_temp2618 >= _temp2616.last_plus_one)){ _throw( Null_Exception);}* _temp2618;})))->sc
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp2619=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2620; _temp2620.curr= _temp2619; _temp2620.base=
_temp2619; _temp2620.last_plus_one= _temp2619 + 51; _temp2620;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2621= yyls; struct Cyc_Yyltype* _temp2623= _temp2621.curr
+( yylsp_offset - 2); if( _temp2621.base == 0? 1:( _temp2623 < _temp2621.base? 1:
_temp2623 >= _temp2621.last_plus_one)){ _throw( Null_Exception);}* _temp2623;}).first_line,({
struct _tagged_ptr3 _temp2624= yyls; struct Cyc_Yyltype* _temp2626= _temp2624.curr
+( yylsp_offset - 1); if( _temp2624.base == 0? 1:( _temp2626 < _temp2624.base? 1:
_temp2626 >= _temp2624.last_plus_one)){ _throw( Null_Exception);}* _temp2626;}).last_line));}
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2627=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2627[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2628; _temp2628.tag= Cyc_DeclSpec_tok; _temp2628.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2629=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2629->sc=({ struct Cyc_Core_Opt* _temp2645=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2645->v=( void*) Cyc_yyget_StorageClass_tok(({
struct _tagged_ptr2 _temp2646= yyvs; void** _temp2648= _temp2646.curr +(
yyvsp_offset - 2); if( _temp2646.base == 0? 1:( _temp2648 < _temp2646.base? 1:
_temp2648 >= _temp2646.last_plus_one)){ _throw( Null_Exception);}* _temp2648;}));
_temp2645;}); _temp2629->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2642= yyvs; void** _temp2644= _temp2642.curr + yyvsp_offset; if( _temp2642.base
== 0? 1:( _temp2644 < _temp2642.base? 1: _temp2644 >= _temp2642.last_plus_one)){
_throw( Null_Exception);}* _temp2644;})))->tq; _temp2629->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2639= yyvs; void** _temp2641= _temp2639.curr +
yyvsp_offset; if( _temp2639.base == 0? 1:( _temp2641 < _temp2639.base? 1:
_temp2641 >= _temp2639.last_plus_one)){ _throw( Null_Exception);}* _temp2641;})))->type_specs;
_temp2629->is_inline=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2636=
yyvs; void** _temp2638= _temp2636.curr + yyvsp_offset; if( _temp2636.base == 0?
1:( _temp2638 < _temp2636.base? 1: _temp2638 >= _temp2636.last_plus_one)){
_throw( Null_Exception);}* _temp2638;})))->is_inline; _temp2629->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2630= yyvs; void**
_temp2632= _temp2630.curr +( yyvsp_offset - 1); if( _temp2630.base == 0? 1:(
_temp2632 < _temp2630.base? 1: _temp2632 >= _temp2630.last_plus_one)){ _throw(
Null_Exception);}* _temp2632;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2633= yyvs; void** _temp2635= _temp2633.curr + yyvsp_offset; if( _temp2633.base
== 0? 1:( _temp2635 < _temp2633.base? 1: _temp2635 >= _temp2633.last_plus_one)){
_throw( Null_Exception);}* _temp2635;})))->attributes); _temp2629;}); _temp2628;});
_temp2627;}); break; case 28: _LL2615: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2650=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2650[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2651; _temp2651.tag= Cyc_DeclSpec_tok;
_temp2651.f1=({ struct Cyc_Parse_Declaration_spec* _temp2652=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2652->sc= 0;
_temp2652->tq= Cyc_Absyn_empty_tqual(); _temp2652->type_specs=({ struct Cyc_List_List*
_temp2656=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2656->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2657= yyvs; void** _temp2659= _temp2657.curr +( yyvsp_offset - 1); if(
_temp2657.base == 0? 1:( _temp2659 < _temp2657.base? 1: _temp2659 >= _temp2657.last_plus_one)){
_throw( Null_Exception);}* _temp2659;})); _temp2656->tl= 0; _temp2656;});
_temp2652->is_inline= 0; _temp2652->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2653= yyvs; void** _temp2655= _temp2653.curr +
yyvsp_offset; if( _temp2653.base == 0? 1:( _temp2655 < _temp2653.base? 1:
_temp2655 >= _temp2653.last_plus_one)){ _throw( Null_Exception);}* _temp2655;}));
_temp2652;}); _temp2651;}); _temp2650;}); break; case 29: _LL2649: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2661=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2661[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2662; _temp2662.tag= Cyc_DeclSpec_tok; _temp2662.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2663=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2663->sc=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2683= yyvs;
void** _temp2685= _temp2683.curr + yyvsp_offset; if( _temp2683.base == 0? 1:(
_temp2685 < _temp2683.base? 1: _temp2685 >= _temp2683.last_plus_one)){ _throw(
Null_Exception);}* _temp2685;})))->sc; _temp2663->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2680= yyvs; void** _temp2682= _temp2680.curr +
yyvsp_offset; if( _temp2680.base == 0? 1:( _temp2682 < _temp2680.base? 1:
_temp2682 >= _temp2680.last_plus_one)){ _throw( Null_Exception);}* _temp2682;})))->tq;
_temp2663->type_specs=({ struct Cyc_List_List* _temp2673=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2673->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp2677= yyvs; void** _temp2679= _temp2677.curr +(
yyvsp_offset - 2); if( _temp2677.base == 0? 1:( _temp2679 < _temp2677.base? 1:
_temp2679 >= _temp2677.last_plus_one)){ _throw( Null_Exception);}* _temp2679;}));
_temp2673->tl=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2674= yyvs;
void** _temp2676= _temp2674.curr + yyvsp_offset; if( _temp2674.base == 0? 1:(
_temp2676 < _temp2674.base? 1: _temp2676 >= _temp2674.last_plus_one)){ _throw(
Null_Exception);}* _temp2676;})))->type_specs; _temp2673;}); _temp2663->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2670= yyvs; void** _temp2672=
_temp2670.curr + yyvsp_offset; if( _temp2670.base == 0? 1:( _temp2672 <
_temp2670.base? 1: _temp2672 >= _temp2670.last_plus_one)){ _throw(
Null_Exception);}* _temp2672;})))->is_inline; _temp2663->attributes=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2664= yyvs; void**
_temp2666= _temp2664.curr +( yyvsp_offset - 1); if( _temp2664.base == 0? 1:(
_temp2666 < _temp2664.base? 1: _temp2666 >= _temp2664.last_plus_one)){ _throw(
Null_Exception);}* _temp2666;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2667= yyvs; void** _temp2669= _temp2667.curr + yyvsp_offset; if( _temp2667.base
== 0? 1:( _temp2669 < _temp2667.base? 1: _temp2669 >= _temp2667.last_plus_one)){
_throw( Null_Exception);}* _temp2669;})))->attributes); _temp2663;}); _temp2662;});
_temp2661;}); break; case 30: _LL2660: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2687=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2687[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2688; _temp2688.tag= Cyc_DeclSpec_tok;
_temp2688.f1=({ struct Cyc_Parse_Declaration_spec* _temp2689=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2689->sc= 0;
_temp2689->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2693= yyvs;
void** _temp2695= _temp2693.curr +( yyvsp_offset - 1); if( _temp2693.base == 0?
1:( _temp2695 < _temp2693.base? 1: _temp2695 >= _temp2693.last_plus_one)){
_throw( Null_Exception);}* _temp2695;})); _temp2689->type_specs= 0; _temp2689->is_inline=
0; _temp2689->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2690= yyvs; void** _temp2692= _temp2690.curr + yyvsp_offset; if( _temp2690.base
== 0? 1:( _temp2692 < _temp2690.base? 1: _temp2692 >= _temp2690.last_plus_one)){
_throw( Null_Exception);}* _temp2692;})); _temp2689;}); _temp2688;}); _temp2687;});
break; case 31: _LL2686: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2697=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2697[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2698; _temp2698.tag= Cyc_DeclSpec_tok;
_temp2698.f1=({ struct Cyc_Parse_Declaration_spec* _temp2699=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2699->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2718= yyvs; void** _temp2720= _temp2718.curr +
yyvsp_offset; if( _temp2718.base == 0? 1:( _temp2720 < _temp2718.base? 1:
_temp2720 >= _temp2718.last_plus_one)){ _throw( Null_Exception);}* _temp2720;})))->sc;
_temp2699->tq= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp2712= yyvs; void** _temp2714= _temp2712.curr +( yyvsp_offset -
2); if( _temp2712.base == 0? 1:( _temp2714 < _temp2712.base? 1: _temp2714 >=
_temp2712.last_plus_one)){ _throw( Null_Exception);}* _temp2714;})),( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2715= yyvs; void** _temp2717= _temp2715.curr +
yyvsp_offset; if( _temp2715.base == 0? 1:( _temp2717 < _temp2715.base? 1:
_temp2717 >= _temp2715.last_plus_one)){ _throw( Null_Exception);}* _temp2717;})))->tq);
_temp2699->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2709=
yyvs; void** _temp2711= _temp2709.curr + yyvsp_offset; if( _temp2709.base == 0?
1:( _temp2711 < _temp2709.base? 1: _temp2711 >= _temp2709.last_plus_one)){
_throw( Null_Exception);}* _temp2711;})))->type_specs; _temp2699->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2706= yyvs; void** _temp2708=
_temp2706.curr + yyvsp_offset; if( _temp2706.base == 0? 1:( _temp2708 <
_temp2706.base? 1: _temp2708 >= _temp2706.last_plus_one)){ _throw(
Null_Exception);}* _temp2708;})))->is_inline; _temp2699->attributes=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2700= yyvs; void**
_temp2702= _temp2700.curr +( yyvsp_offset - 1); if( _temp2700.base == 0? 1:(
_temp2702 < _temp2700.base? 1: _temp2702 >= _temp2700.last_plus_one)){ _throw(
Null_Exception);}* _temp2702;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2703= yyvs; void** _temp2705= _temp2703.curr + yyvsp_offset; if( _temp2703.base
== 0? 1:( _temp2705 < _temp2703.base? 1: _temp2705 >= _temp2703.last_plus_one)){
_throw( Null_Exception);}* _temp2705;})))->attributes); _temp2699;}); _temp2698;});
_temp2697;}); break; case 32: _LL2696: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2722=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2722[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2723; _temp2723.tag= Cyc_DeclSpec_tok;
_temp2723.f1=({ struct Cyc_Parse_Declaration_spec* _temp2724=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2724->sc= 0;
_temp2724->tq= Cyc_Absyn_empty_tqual(); _temp2724->type_specs= 0; _temp2724->is_inline=
1; _temp2724->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2725= yyvs; void** _temp2727= _temp2725.curr + yyvsp_offset; if( _temp2725.base
== 0? 1:( _temp2727 < _temp2725.base? 1: _temp2727 >= _temp2725.last_plus_one)){
_throw( Null_Exception);}* _temp2727;})); _temp2724;}); _temp2723;}); _temp2722;});
break; case 33: _LL2721: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2729=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2729[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2730; _temp2730.tag= Cyc_DeclSpec_tok;
_temp2730.f1=({ struct Cyc_Parse_Declaration_spec* _temp2731=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2731->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2744= yyvs; void** _temp2746= _temp2744.curr +
yyvsp_offset; if( _temp2744.base == 0? 1:( _temp2746 < _temp2744.base? 1:
_temp2746 >= _temp2744.last_plus_one)){ _throw( Null_Exception);}* _temp2746;})))->sc;
_temp2731->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2741= yyvs;
void** _temp2743= _temp2741.curr + yyvsp_offset; if( _temp2741.base == 0? 1:(
_temp2743 < _temp2741.base? 1: _temp2743 >= _temp2741.last_plus_one)){ _throw(
Null_Exception);}* _temp2743;})))->tq; _temp2731->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2738= yyvs; void** _temp2740= _temp2738.curr +
yyvsp_offset; if( _temp2738.base == 0? 1:( _temp2740 < _temp2738.base? 1:
_temp2740 >= _temp2738.last_plus_one)){ _throw( Null_Exception);}* _temp2740;})))->type_specs;
_temp2731->is_inline= 1; _temp2731->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2732= yyvs; void** _temp2734= _temp2732.curr +(
yyvsp_offset - 1); if( _temp2732.base == 0? 1:( _temp2734 < _temp2732.base? 1:
_temp2734 >= _temp2732.last_plus_one)){ _throw( Null_Exception);}* _temp2734;})),(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2735= yyvs; void** _temp2737=
_temp2735.curr + yyvsp_offset; if( _temp2735.base == 0? 1:( _temp2737 <
_temp2735.base? 1: _temp2737 >= _temp2735.last_plus_one)){ _throw(
Null_Exception);}* _temp2737;})))->attributes); _temp2731;}); _temp2730;});
_temp2729;}); break; case 34: _LL2728: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2748=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2748[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2749; _temp2749.tag=
Cyc_StorageClass_tok; _temp2749.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp2749;}); _temp2748;}); break; case 35: _LL2747: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2751=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2751[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2752; _temp2752.tag=
Cyc_StorageClass_tok; _temp2752.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp2752;}); _temp2751;}); break; case 36: _LL2750: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2754=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2754[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2755; _temp2755.tag=
Cyc_StorageClass_tok; _temp2755.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2755;}); _temp2754;}); break; case 37: _LL2753: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2757=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2757[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2758; _temp2758.tag=
Cyc_StorageClass_tok; _temp2758.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp2758;}); _temp2757;}); break; case 38: _LL2756: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2760= yyvs; void** _temp2762= _temp2760.curr +
yyvsp_offset; if( _temp2760.base == 0? 1:( _temp2762 < _temp2760.base? 1:
_temp2762 >= _temp2760.last_plus_one)){ _throw( Null_Exception);}* _temp2762;})),(
struct _tagged_string)({ char* _temp2763=( char*)"C"; struct _tagged_string
_temp2764; _temp2764.curr= _temp2763; _temp2764.base= _temp2763; _temp2764.last_plus_one=
_temp2763 + 2; _temp2764;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2765=( char*)"only extern or extern \"C\" is allowed"; struct
_tagged_string _temp2766; _temp2766.curr= _temp2765; _temp2766.base= _temp2765;
_temp2766.last_plus_one= _temp2765 + 37; _temp2766;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2767= yyls; struct Cyc_Yyltype* _temp2769= _temp2767.curr
+( yylsp_offset - 1); if( _temp2767.base == 0? 1:( _temp2769 < _temp2767.base? 1:
_temp2769 >= _temp2767.last_plus_one)){ _throw( Null_Exception);}* _temp2769;}).first_line,({
struct _tagged_ptr3 _temp2770= yyls; struct Cyc_Yyltype* _temp2772= _temp2770.curr
+ yylsp_offset; if( _temp2770.base == 0? 1:( _temp2772 < _temp2770.base? 1:
_temp2772 >= _temp2770.last_plus_one)){ _throw( Null_Exception);}* _temp2772;}).last_line));}
yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp2773=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2773[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2774; _temp2774.tag= Cyc_StorageClass_tok;
_temp2774.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp2774;}); _temp2773;});
break; case 39: _LL2759: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2776=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2776[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2777; _temp2777.tag=
Cyc_StorageClass_tok; _temp2777.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp2777;}); _temp2776;}); break; case 40: _LL2775: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2779=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2779[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2780; _temp2780.tag=
Cyc_StorageClass_tok; _temp2780.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2780;}); _temp2779;}); break; case 41: _LL2778: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2782=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2782[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2783; _temp2783.tag=
Cyc_AttributeList_tok; _temp2783.f1= 0; _temp2783;}); _temp2782;}); break; case
42: _LL2781: yyval=({ struct _tagged_ptr2 _temp2785= yyvs; void** _temp2787=
_temp2785.curr + yyvsp_offset; if( _temp2785.base == 0? 1:( _temp2787 <
_temp2785.base? 1: _temp2787 >= _temp2785.last_plus_one)){ _throw(
Null_Exception);}* _temp2787;}); break; case 43: _LL2784: yyval=( void*)({
struct Cyc_AttributeList_tok_struct* _temp2789=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2789[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2790; _temp2790.tag= Cyc_AttributeList_tok;
_temp2790.f1= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2791= yyvs;
void** _temp2793= _temp2791.curr +( yyvsp_offset - 2); if( _temp2791.base == 0?
1:( _temp2793 < _temp2791.base? 1: _temp2793 >= _temp2791.last_plus_one)){
_throw( Null_Exception);}* _temp2793;})); _temp2790;}); _temp2789;}); break;
case 44: _LL2788: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2795=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2795[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2796; _temp2796.tag=
Cyc_AttributeList_tok; _temp2796.f1=({ struct Cyc_List_List* _temp2797=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2797->hd=( void*)
Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2798= yyvs; void** _temp2800=
_temp2798.curr + yyvsp_offset; if( _temp2798.base == 0? 1:( _temp2800 <
_temp2798.base? 1: _temp2800 >= _temp2798.last_plus_one)){ _throw(
Null_Exception);}* _temp2800;})); _temp2797->tl= 0; _temp2797;}); _temp2796;});
_temp2795;}); break; case 45: _LL2794: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2802=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2802[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2803; _temp2803.tag=
Cyc_AttributeList_tok; _temp2803.f1=({ struct Cyc_List_List* _temp2804=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2804->hd=( void*)
Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2808= yyvs; void** _temp2810=
_temp2808.curr +( yyvsp_offset - 2); if( _temp2808.base == 0? 1:( _temp2810 <
_temp2808.base? 1: _temp2810 >= _temp2808.last_plus_one)){ _throw(
Null_Exception);}* _temp2810;})); _temp2804->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2805= yyvs; void** _temp2807= _temp2805.curr +
yyvsp_offset; if( _temp2805.base == 0? 1:( _temp2807 < _temp2805.base? 1:
_temp2807 >= _temp2805.last_plus_one)){ _throw( Null_Exception);}* _temp2807;}));
_temp2804;}); _temp2803;}); _temp2802;}); break; case 46: _LL2801: { struct
_tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2886= yyvs;
void** _temp2888= _temp2886.curr + yyvsp_offset; if( _temp2886.base == 0? 1:(
_temp2888 < _temp2886.base? 1: _temp2888 >= _temp2886.last_plus_one)){ _throw(
Null_Exception);}* _temp2888;})); void* a; if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2812=( char*)"stdcall"; struct _tagged_string
_temp2813; _temp2813.curr= _temp2812; _temp2813.base= _temp2812; _temp2813.last_plus_one=
_temp2812 + 8; _temp2813;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2814=( char*)"__stdcall__"; struct _tagged_string
_temp2815; _temp2815.curr= _temp2814; _temp2815.base= _temp2814; _temp2815.last_plus_one=
_temp2814 + 12; _temp2815;})) == 0){ a=( void*) Cyc_Absyn_Stdcall_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2816=( char*)"cdecl";
struct _tagged_string _temp2817; _temp2817.curr= _temp2816; _temp2817.base=
_temp2816; _temp2817.last_plus_one= _temp2816 + 6; _temp2817;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2818=( char*)"__cdecl__"; struct
_tagged_string _temp2819; _temp2819.curr= _temp2818; _temp2819.base= _temp2818;
_temp2819.last_plus_one= _temp2818 + 10; _temp2819;})) == 0){ a=( void*) Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2820=(
char*)"noreturn"; struct _tagged_string _temp2821; _temp2821.curr= _temp2820;
_temp2821.base= _temp2820; _temp2821.last_plus_one= _temp2820 + 9; _temp2821;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2822=( char*)"__noreturn__";
struct _tagged_string _temp2823; _temp2823.curr= _temp2822; _temp2823.base=
_temp2822; _temp2823.last_plus_one= _temp2822 + 13; _temp2823;})) == 0){ a=(
void*) Cyc_Absyn_Noreturn_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2824=( char*)"noreturn"; struct _tagged_string
_temp2825; _temp2825.curr= _temp2824; _temp2825.base= _temp2824; _temp2825.last_plus_one=
_temp2824 + 9; _temp2825;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2826=( char*)"__noreturn__"; struct _tagged_string
_temp2827; _temp2827.curr= _temp2826; _temp2827.base= _temp2826; _temp2827.last_plus_one=
_temp2826 + 13; _temp2827;})) == 0){ a=( void*) Cyc_Absyn_Noreturn_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2828=( char*)"__const__";
struct _tagged_string _temp2829; _temp2829.curr= _temp2828; _temp2829.base=
_temp2828; _temp2829.last_plus_one= _temp2828 + 10; _temp2829;})) == 0){ a=(
void*) Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2830=( char*)"aligned"; struct _tagged_string
_temp2831; _temp2831.curr= _temp2830; _temp2831.base= _temp2830; _temp2831.last_plus_one=
_temp2830 + 8; _temp2831;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2832=( char*)"__aligned__"; struct _tagged_string
_temp2833; _temp2833.curr= _temp2832; _temp2833.base= _temp2832; _temp2833.last_plus_one=
_temp2832 + 12; _temp2833;})) == 0){ a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp2834=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct));
_temp2834[ 0]=({ struct Cyc_Absyn_Aligned_att_struct _temp2835; _temp2835.tag=
Cyc_Absyn_Aligned_att; _temp2835.f1= - 1; _temp2835;}); _temp2834;});} else{ if(
Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2836=( char*)"packed";
struct _tagged_string _temp2837; _temp2837.curr= _temp2836; _temp2837.base=
_temp2836; _temp2837.last_plus_one= _temp2836 + 7; _temp2837;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2838=( char*)"__packed__"; struct
_tagged_string _temp2839; _temp2839.curr= _temp2838; _temp2839.base= _temp2838;
_temp2839.last_plus_one= _temp2838 + 11; _temp2839;})) == 0){ a=( void*) Cyc_Absyn_Packed_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2840=(
char*)"shared"; struct _tagged_string _temp2841; _temp2841.curr= _temp2840;
_temp2841.base= _temp2840; _temp2841.last_plus_one= _temp2840 + 7; _temp2841;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2842=( char*)"__shared__";
struct _tagged_string _temp2843; _temp2843.curr= _temp2842; _temp2843.base=
_temp2842; _temp2843.last_plus_one= _temp2842 + 11; _temp2843;})) == 0){ a=(
void*) Cyc_Absyn_Shared_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2844=( char*)"unused"; struct _tagged_string
_temp2845; _temp2845.curr= _temp2844; _temp2845.base= _temp2844; _temp2845.last_plus_one=
_temp2844 + 7; _temp2845;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2846=( char*)"__unused__"; struct _tagged_string
_temp2847; _temp2847.curr= _temp2846; _temp2847.base= _temp2846; _temp2847.last_plus_one=
_temp2846 + 11; _temp2847;})) == 0){ a=( void*) Cyc_Absyn_Unused_att;} else{ if(
Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2848=( char*)"weak";
struct _tagged_string _temp2849; _temp2849.curr= _temp2848; _temp2849.base=
_temp2848; _temp2849.last_plus_one= _temp2848 + 5; _temp2849;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2850=( char*)"__weak__"; struct
_tagged_string _temp2851; _temp2851.curr= _temp2850; _temp2851.base= _temp2850;
_temp2851.last_plus_one= _temp2850 + 9; _temp2851;})) == 0){ a=( void*) Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2852=(
char*)"dllimport"; struct _tagged_string _temp2853; _temp2853.curr= _temp2852;
_temp2853.base= _temp2852; _temp2853.last_plus_one= _temp2852 + 10; _temp2853;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2854=( char*)"__dllimport__";
struct _tagged_string _temp2855; _temp2855.curr= _temp2854; _temp2855.base=
_temp2854; _temp2855.last_plus_one= _temp2854 + 14; _temp2855;})) == 0){ a=(
void*) Cyc_Absyn_Dllimport_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2856=( char*)"dllexport"; struct _tagged_string
_temp2857; _temp2857.curr= _temp2856; _temp2857.base= _temp2856; _temp2857.last_plus_one=
_temp2856 + 10; _temp2857;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2858=( char*)"__dllexport__"; struct _tagged_string
_temp2859; _temp2859.curr= _temp2858; _temp2859.base= _temp2858; _temp2859.last_plus_one=
_temp2858 + 14; _temp2859;})) == 0){ a=( void*) Cyc_Absyn_Dllexport_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2860=( char*)"no_instrument_function";
struct _tagged_string _temp2861; _temp2861.curr= _temp2860; _temp2861.base=
_temp2860; _temp2861.last_plus_one= _temp2860 + 23; _temp2861;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2862=( char*)"__no_instrument_function__";
struct _tagged_string _temp2863; _temp2863.curr= _temp2862; _temp2863.base=
_temp2862; _temp2863.last_plus_one= _temp2862 + 27; _temp2863;})) == 0){ a=(
void*) Cyc_Absyn_No_instrument_function_att;} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2864=( char*)"constructor"; struct
_tagged_string _temp2865; _temp2865.curr= _temp2864; _temp2865.base= _temp2864;
_temp2865.last_plus_one= _temp2864 + 12; _temp2865;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2866=( char*)"__constructor__"; struct
_tagged_string _temp2867; _temp2867.curr= _temp2866; _temp2867.base= _temp2866;
_temp2867.last_plus_one= _temp2866 + 16; _temp2867;})) == 0){ a=( void*) Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2868=(
char*)"destructor"; struct _tagged_string _temp2869; _temp2869.curr= _temp2868;
_temp2869.base= _temp2868; _temp2869.last_plus_one= _temp2868 + 11; _temp2869;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2870=( char*)"__destructor__";
struct _tagged_string _temp2871; _temp2871.curr= _temp2870; _temp2871.base=
_temp2870; _temp2871.last_plus_one= _temp2870 + 15; _temp2871;})) == 0){ a=(
void*) Cyc_Absyn_Destructor_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2872=( char*)"no_check_memory_usage"; struct
_tagged_string _temp2873; _temp2873.curr= _temp2872; _temp2873.base= _temp2872;
_temp2873.last_plus_one= _temp2872 + 22; _temp2873;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2874=( char*)"__no_check_memory_usage__";
struct _tagged_string _temp2875; _temp2875.curr= _temp2874; _temp2875.base=
_temp2874; _temp2875.last_plus_one= _temp2874 + 26; _temp2875;})) == 0){ a=(
void*) Cyc_Absyn_No_check_memory_usage_att;} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2876=( char*)"unrecognized attribute"; struct
_tagged_string _temp2877; _temp2877.curr= _temp2876; _temp2877.base= _temp2876;
_temp2877.last_plus_one= _temp2876 + 23; _temp2877;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2878= yyls; struct Cyc_Yyltype* _temp2880= _temp2878.curr
+ yylsp_offset; if( _temp2878.base == 0? 1:( _temp2880 < _temp2878.base? 1:
_temp2880 >= _temp2878.last_plus_one)){ _throw( Null_Exception);}* _temp2880;}).first_line,({
struct _tagged_ptr3 _temp2881= yyls; struct Cyc_Yyltype* _temp2883= _temp2881.curr
+ yylsp_offset; if( _temp2881.base == 0? 1:( _temp2883 < _temp2881.base? 1:
_temp2883 >= _temp2881.last_plus_one)){ _throw( Null_Exception);}* _temp2883;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2884=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2884[ 0]=({ struct Cyc_Attribute_tok_struct _temp2885; _temp2885.tag= Cyc_Attribute_tok;
_temp2885.f1=( void*) a; _temp2885;}); _temp2884;}); break;} case 47: _LL2811:
yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2890=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2890[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2891; _temp2891.tag= Cyc_Attribute_tok; _temp2891.f1=(
void*)(( void*) Cyc_Absyn_Const_att); _temp2891;}); _temp2890;}); break; case 48:
_LL2889: { struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2949= yyvs; void** _temp2951= _temp2949.curr +( yyvsp_offset - 3); if(
_temp2949.base == 0? 1:( _temp2951 < _temp2949.base? 1: _temp2951 >= _temp2949.last_plus_one)){
_throw( Null_Exception);}* _temp2951;})); struct _tuple12 _temp2898; int
_temp2899; void* _temp2901; struct _tuple12* _temp2896= Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp2893= yyvs; void** _temp2895= _temp2893.curr +(
yyvsp_offset - 1); if( _temp2893.base == 0? 1:( _temp2895 < _temp2893.base? 1:
_temp2895 >= _temp2893.last_plus_one)){ _throw( Null_Exception);}* _temp2895;}));
_temp2898=* _temp2896; _LL2902: _temp2901= _temp2898.f1; goto _LL2900; _LL2900:
_temp2899= _temp2898.f2; goto _LL2897; _LL2897: { void* a; if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2903=( char*)"regparm"; struct
_tagged_string _temp2904; _temp2904.curr= _temp2903; _temp2904.base= _temp2903;
_temp2904.last_plus_one= _temp2903 + 8; _temp2904;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2905=( char*)"__regparm__"; struct
_tagged_string _temp2906; _temp2906.curr= _temp2905; _temp2906.base= _temp2905;
_temp2906.last_plus_one= _temp2905 + 12; _temp2906;})) == 0){ if( _temp2899 <= 0?
1: _temp2899 > 3){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2907=(
char*)"regparm requires value between 1 and 3"; struct _tagged_string _temp2908;
_temp2908.curr= _temp2907; _temp2908.base= _temp2907; _temp2908.last_plus_one=
_temp2907 + 39; _temp2908;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2909= yyls; struct Cyc_Yyltype* _temp2911= _temp2909.curr +( yylsp_offset -
1); if( _temp2909.base == 0? 1:( _temp2911 < _temp2909.base? 1: _temp2911 >=
_temp2909.last_plus_one)){ _throw( Null_Exception);}* _temp2911;}).first_line,({
struct _tagged_ptr3 _temp2912= yyls; struct Cyc_Yyltype* _temp2914= _temp2912.curr
+( yylsp_offset - 1); if( _temp2912.base == 0? 1:( _temp2914 < _temp2912.base? 1:
_temp2914 >= _temp2912.last_plus_one)){ _throw( Null_Exception);}* _temp2914;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2915=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2915[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2916; _temp2916.tag= Cyc_Absyn_Regparm_att;
_temp2916.f1= _temp2899; _temp2916;}); _temp2915;});} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2917=( char*)"aligned"; struct
_tagged_string _temp2918; _temp2918.curr= _temp2917; _temp2918.base= _temp2917;
_temp2918.last_plus_one= _temp2917 + 8; _temp2918;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2919=( char*)"__aligned__"; struct
_tagged_string _temp2920; _temp2920.curr= _temp2919; _temp2920.base= _temp2919;
_temp2920.last_plus_one= _temp2919 + 12; _temp2920;})) == 0){ if( _temp2899 < 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2921=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2922; _temp2922.curr= _temp2921; _temp2922.base=
_temp2921; _temp2922.last_plus_one= _temp2921 + 39; _temp2922;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2923= yyls; struct Cyc_Yyltype* _temp2925= _temp2923.curr
+( yylsp_offset - 1); if( _temp2923.base == 0? 1:( _temp2925 < _temp2923.base? 1:
_temp2925 >= _temp2923.last_plus_one)){ _throw( Null_Exception);}* _temp2925;}).first_line,({
struct _tagged_ptr3 _temp2926= yyls; struct Cyc_Yyltype* _temp2928= _temp2926.curr
+( yylsp_offset - 1); if( _temp2926.base == 0? 1:( _temp2928 < _temp2926.base? 1:
_temp2928 >= _temp2926.last_plus_one)){ _throw( Null_Exception);}* _temp2928;}).last_line));}{
unsigned int j=( unsigned int) _temp2899; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2929=(
char*)"aligned requires positive power of two"; struct _tagged_string _temp2930;
_temp2930.curr= _temp2929; _temp2930.base= _temp2929; _temp2930.last_plus_one=
_temp2929 + 39; _temp2930;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2931= yyls; struct Cyc_Yyltype* _temp2933= _temp2931.curr +( yylsp_offset -
1); if( _temp2931.base == 0? 1:( _temp2933 < _temp2931.base? 1: _temp2933 >=
_temp2931.last_plus_one)){ _throw( Null_Exception);}* _temp2933;}).first_line,({
struct _tagged_ptr3 _temp2934= yyls; struct Cyc_Yyltype* _temp2936= _temp2934.curr
+( yylsp_offset - 1); if( _temp2934.base == 0? 1:( _temp2936 < _temp2934.base? 1:
_temp2936 >= _temp2934.last_plus_one)){ _throw( Null_Exception);}* _temp2936;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2937=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2937[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2938; _temp2938.tag= Cyc_Absyn_Aligned_att;
_temp2938.f1= _temp2899; _temp2938;}); _temp2937;});}} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2939=( char*)"unrecognized attribute";
struct _tagged_string _temp2940; _temp2940.curr= _temp2939; _temp2940.base=
_temp2939; _temp2940.last_plus_one= _temp2939 + 23; _temp2940;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2941= yyls; struct Cyc_Yyltype* _temp2943= _temp2941.curr
+( yylsp_offset - 3); if( _temp2941.base == 0? 1:( _temp2943 < _temp2941.base? 1:
_temp2943 >= _temp2941.last_plus_one)){ _throw( Null_Exception);}* _temp2943;}).first_line,({
struct _tagged_ptr3 _temp2944= yyls; struct Cyc_Yyltype* _temp2946= _temp2944.curr
+( yylsp_offset - 3); if( _temp2944.base == 0? 1:( _temp2946 < _temp2944.base? 1:
_temp2946 >= _temp2944.last_plus_one)){ _throw( Null_Exception);}* _temp2946;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2947=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2947[ 0]=({ struct Cyc_Attribute_tok_struct _temp2948; _temp2948.tag= Cyc_Attribute_tok;
_temp2948.f1=( void*) a; _temp2948;}); _temp2947;}); break;}} case 49: _LL2892: {
struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2972=
yyvs; void** _temp2974= _temp2972.curr +( yyvsp_offset - 3); if( _temp2972.base
== 0? 1:( _temp2974 < _temp2972.base? 1: _temp2974 >= _temp2972.last_plus_one)){
_throw( Null_Exception);}* _temp2974;})); struct _tagged_string str= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2969= yyvs; void** _temp2971= _temp2969.curr +(
yyvsp_offset - 1); if( _temp2969.base == 0? 1:( _temp2971 < _temp2969.base? 1:
_temp2971 >= _temp2969.last_plus_one)){ _throw( Null_Exception);}* _temp2971;}));
void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2953=(
char*)"section"; struct _tagged_string _temp2954; _temp2954.curr= _temp2953;
_temp2954.base= _temp2953; _temp2954.last_plus_one= _temp2953 + 8; _temp2954;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2955=( char*)"__section__";
struct _tagged_string _temp2956; _temp2956.curr= _temp2955; _temp2956.base=
_temp2955; _temp2956.last_plus_one= _temp2955 + 12; _temp2956;}))){ a=( void*)({
struct Cyc_Absyn_Section_att_struct* _temp2957=( struct Cyc_Absyn_Section_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct)); _temp2957[ 0]=({
struct Cyc_Absyn_Section_att_struct _temp2958; _temp2958.tag= Cyc_Absyn_Section_att;
_temp2958.f1= str; _temp2958;}); _temp2957;});} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2959=( char*)"unrecognized attribute"; struct
_tagged_string _temp2960; _temp2960.curr= _temp2959; _temp2960.base= _temp2959;
_temp2960.last_plus_one= _temp2959 + 23; _temp2960;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2961= yyls; struct Cyc_Yyltype* _temp2963= _temp2961.curr
+( yylsp_offset - 3); if( _temp2961.base == 0? 1:( _temp2963 < _temp2961.base? 1:
_temp2963 >= _temp2961.last_plus_one)){ _throw( Null_Exception);}* _temp2963;}).first_line,({
struct _tagged_ptr3 _temp2964= yyls; struct Cyc_Yyltype* _temp2966= _temp2964.curr
+( yylsp_offset - 3); if( _temp2964.base == 0? 1:( _temp2966 < _temp2964.base? 1:
_temp2966 >= _temp2964.last_plus_one)){ _throw( Null_Exception);}* _temp2966;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2967=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2967[ 0]=({ struct Cyc_Attribute_tok_struct _temp2968; _temp2968.tag= Cyc_Attribute_tok;
_temp2968.f1=( void*) a; _temp2968;}); _temp2967;}); break;} case 50: _LL2952:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2976=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2976[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2977; _temp2977.tag= Cyc_TypeSpecifier_tok;
_temp2977.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2978= yyls; struct Cyc_Yyltype* _temp2980= _temp2978.curr
+ yylsp_offset; if( _temp2978.base == 0? 1:( _temp2980 < _temp2978.base? 1:
_temp2980 >= _temp2978.last_plus_one)){ _throw( Null_Exception);}* _temp2980;}).first_line,({
struct _tagged_ptr3 _temp2981= yyls; struct Cyc_Yyltype* _temp2983= _temp2981.curr
+ yylsp_offset; if( _temp2981.base == 0? 1:( _temp2983 < _temp2981.base? 1:
_temp2983 >= _temp2981.last_plus_one)){ _throw( Null_Exception);}* _temp2983;}).last_line));
_temp2977;}); _temp2976;}); break; case 51: _LL2975: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2985=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2985[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2986; _temp2986.tag=
Cyc_TypeSpecifier_tok; _temp2986.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2987= yyls; struct Cyc_Yyltype* _temp2989= _temp2987.curr + yylsp_offset;
if( _temp2987.base == 0? 1:( _temp2989 < _temp2987.base? 1: _temp2989 >=
_temp2987.last_plus_one)){ _throw( Null_Exception);}* _temp2989;}).first_line,({
struct _tagged_ptr3 _temp2990= yyls; struct Cyc_Yyltype* _temp2992= _temp2990.curr
+ yylsp_offset; if( _temp2990.base == 0? 1:( _temp2992 < _temp2990.base? 1:
_temp2992 >= _temp2990.last_plus_one)){ _throw( Null_Exception);}* _temp2992;}).last_line));
_temp2986;}); _temp2985;}); break; case 52: _LL2984: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2994=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2994[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2995; _temp2995.tag=
Cyc_TypeSpecifier_tok; _temp2995.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2996= yyls; struct Cyc_Yyltype*
_temp2998= _temp2996.curr + yylsp_offset; if( _temp2996.base == 0? 1:( _temp2998
< _temp2996.base? 1: _temp2998 >= _temp2996.last_plus_one)){ _throw(
Null_Exception);}* _temp2998;}).first_line,({ struct _tagged_ptr3 _temp2999=
yyls; struct Cyc_Yyltype* _temp3001= _temp2999.curr + yylsp_offset; if(
_temp2999.base == 0? 1:( _temp3001 < _temp2999.base? 1: _temp3001 >= _temp2999.last_plus_one)){
_throw( Null_Exception);}* _temp3001;}).last_line)); _temp2995;}); _temp2994;});
break; case 53: _LL2993: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3003=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3003[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3004; _temp3004.tag=
Cyc_TypeSpecifier_tok; _temp3004.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp3005=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp3005[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp3006; _temp3006.tag=
Cyc_Parse_Short_spec; _temp3006.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3007= yyls; struct Cyc_Yyltype* _temp3009= _temp3007.curr +
yylsp_offset; if( _temp3007.base == 0? 1:( _temp3009 < _temp3007.base? 1:
_temp3009 >= _temp3007.last_plus_one)){ _throw( Null_Exception);}* _temp3009;}).first_line,({
struct _tagged_ptr3 _temp3010= yyls; struct Cyc_Yyltype* _temp3012= _temp3010.curr
+ yylsp_offset; if( _temp3010.base == 0? 1:( _temp3012 < _temp3010.base? 1:
_temp3012 >= _temp3010.last_plus_one)){ _throw( Null_Exception);}* _temp3012;}).last_line);
_temp3006;}); _temp3005;})); _temp3004;}); _temp3003;}); break; case 54: _LL3002:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3014=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3014[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3015; _temp3015.tag= Cyc_TypeSpecifier_tok;
_temp3015.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3016= yyls; struct Cyc_Yyltype* _temp3018= _temp3016.curr
+ yylsp_offset; if( _temp3016.base == 0? 1:( _temp3018 < _temp3016.base? 1:
_temp3018 >= _temp3016.last_plus_one)){ _throw( Null_Exception);}* _temp3018;}).first_line,({
struct _tagged_ptr3 _temp3019= yyls; struct Cyc_Yyltype* _temp3021= _temp3019.curr
+ yylsp_offset; if( _temp3019.base == 0? 1:( _temp3021 < _temp3019.base? 1:
_temp3021 >= _temp3019.last_plus_one)){ _throw( Null_Exception);}* _temp3021;}).last_line));
_temp3015;}); _temp3014;}); break; case 55: _LL3013: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3023=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3023[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3024; _temp3024.tag=
Cyc_TypeSpecifier_tok; _temp3024.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp3025=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp3025[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp3026; _temp3026.tag= Cyc_Parse_Long_spec;
_temp3026.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3027= yyls;
struct Cyc_Yyltype* _temp3029= _temp3027.curr + yylsp_offset; if( _temp3027.base
== 0? 1:( _temp3029 < _temp3027.base? 1: _temp3029 >= _temp3027.last_plus_one)){
_throw( Null_Exception);}* _temp3029;}).first_line,({ struct _tagged_ptr3
_temp3030= yyls; struct Cyc_Yyltype* _temp3032= _temp3030.curr + yylsp_offset;
if( _temp3030.base == 0? 1:( _temp3032 < _temp3030.base? 1: _temp3032 >=
_temp3030.last_plus_one)){ _throw( Null_Exception);}* _temp3032;}).last_line);
_temp3026;}); _temp3025;})); _temp3024;}); _temp3023;}); break; case 56: _LL3022:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3034=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3034[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3035; _temp3035.tag= Cyc_TypeSpecifier_tok;
_temp3035.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3036= yyls; struct Cyc_Yyltype* _temp3038= _temp3036.curr
+ yylsp_offset; if( _temp3036.base == 0? 1:( _temp3038 < _temp3036.base? 1:
_temp3038 >= _temp3036.last_plus_one)){ _throw( Null_Exception);}* _temp3038;}).first_line,({
struct _tagged_ptr3 _temp3039= yyls; struct Cyc_Yyltype* _temp3041= _temp3039.curr
+ yylsp_offset; if( _temp3039.base == 0? 1:( _temp3041 < _temp3039.base? 1:
_temp3041 >= _temp3039.last_plus_one)){ _throw( Null_Exception);}* _temp3041;}).last_line));
_temp3035;}); _temp3034;}); break; case 57: _LL3033: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3043=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3043[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3044; _temp3044.tag=
Cyc_TypeSpecifier_tok; _temp3044.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3045= yyls; struct Cyc_Yyltype*
_temp3047= _temp3045.curr + yylsp_offset; if( _temp3045.base == 0? 1:( _temp3047
< _temp3045.base? 1: _temp3047 >= _temp3045.last_plus_one)){ _throw(
Null_Exception);}* _temp3047;}).first_line,({ struct _tagged_ptr3 _temp3048=
yyls; struct Cyc_Yyltype* _temp3050= _temp3048.curr + yylsp_offset; if(
_temp3048.base == 0? 1:( _temp3050 < _temp3048.base? 1: _temp3050 >= _temp3048.last_plus_one)){
_throw( Null_Exception);}* _temp3050;}).last_line)); _temp3044;}); _temp3043;});
break; case 58: _LL3042: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3052=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3052[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3053; _temp3053.tag=
Cyc_TypeSpecifier_tok; _temp3053.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp3054=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp3054[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp3055; _temp3055.tag=
Cyc_Parse_Signed_spec; _temp3055.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3056= yyls; struct Cyc_Yyltype* _temp3058= _temp3056.curr +
yylsp_offset; if( _temp3056.base == 0? 1:( _temp3058 < _temp3056.base? 1:
_temp3058 >= _temp3056.last_plus_one)){ _throw( Null_Exception);}* _temp3058;}).first_line,({
struct _tagged_ptr3 _temp3059= yyls; struct Cyc_Yyltype* _temp3061= _temp3059.curr
+ yylsp_offset; if( _temp3059.base == 0? 1:( _temp3061 < _temp3059.base? 1:
_temp3061 >= _temp3059.last_plus_one)){ _throw( Null_Exception);}* _temp3061;}).last_line);
_temp3055;}); _temp3054;})); _temp3053;}); _temp3052;}); break; case 59: _LL3051:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3063=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3063[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3064; _temp3064.tag= Cyc_TypeSpecifier_tok;
_temp3064.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp3065=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp3065[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp3066; _temp3066.tag= Cyc_Parse_Unsigned_spec; _temp3066.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3067= yyls; struct Cyc_Yyltype* _temp3069= _temp3067.curr
+ yylsp_offset; if( _temp3067.base == 0? 1:( _temp3069 < _temp3067.base? 1:
_temp3069 >= _temp3067.last_plus_one)){ _throw( Null_Exception);}* _temp3069;}).first_line,({
struct _tagged_ptr3 _temp3070= yyls; struct Cyc_Yyltype* _temp3072= _temp3070.curr
+ yylsp_offset; if( _temp3070.base == 0? 1:( _temp3072 < _temp3070.base? 1:
_temp3072 >= _temp3070.last_plus_one)){ _throw( Null_Exception);}* _temp3072;}).last_line);
_temp3066;}); _temp3065;})); _temp3064;}); _temp3063;}); break; case 60: _LL3062:
yyval=({ struct _tagged_ptr2 _temp3074= yyvs; void** _temp3076= _temp3074.curr +
yyvsp_offset; if( _temp3074.base == 0? 1:( _temp3076 < _temp3074.base? 1:
_temp3076 >= _temp3074.last_plus_one)){ _throw( Null_Exception);}* _temp3076;});
break; case 61: _LL3073: yyval=({ struct _tagged_ptr2 _temp3078= yyvs; void**
_temp3080= _temp3078.curr + yyvsp_offset; if( _temp3078.base == 0? 1:( _temp3080
< _temp3078.base? 1: _temp3080 >= _temp3078.last_plus_one)){ _throw(
Null_Exception);}* _temp3080;}); break; case 62: _LL3077: yyval=({ struct
_tagged_ptr2 _temp3082= yyvs; void** _temp3084= _temp3082.curr + yyvsp_offset;
if( _temp3082.base == 0? 1:( _temp3084 < _temp3082.base? 1: _temp3084 >=
_temp3082.last_plus_one)){ _throw( Null_Exception);}* _temp3084;}); break; case
63: _LL3081: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3086=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3086[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3087; _temp3087.tag=
Cyc_TypeSpecifier_tok; _temp3087.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3088= yyvs; void** _temp3090=
_temp3088.curr + yyvsp_offset; if( _temp3088.base == 0? 1:( _temp3090 <
_temp3088.base? 1: _temp3090 >= _temp3088.last_plus_one)){ _throw(
Null_Exception);}* _temp3090;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3091= yyls; struct Cyc_Yyltype*
_temp3093= _temp3091.curr + yylsp_offset; if( _temp3091.base == 0? 1:( _temp3093
< _temp3091.base? 1: _temp3093 >= _temp3091.last_plus_one)){ _throw(
Null_Exception);}* _temp3093;}).first_line,({ struct _tagged_ptr3 _temp3094=
yyls; struct Cyc_Yyltype* _temp3096= _temp3094.curr + yylsp_offset; if(
_temp3094.base == 0? 1:( _temp3096 < _temp3094.base? 1: _temp3096 >= _temp3094.last_plus_one)){
_throw( Null_Exception);}* _temp3096;}).last_line)); _temp3087;}); _temp3086;});
break; case 64: _LL3085: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3098=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3098[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3099; _temp3099.tag=
Cyc_TypeSpecifier_tok; _temp3099.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3100= yyvs; void** _temp3102=
_temp3100.curr +( yyvsp_offset - 2); if( _temp3100.base == 0? 1:( _temp3102 <
_temp3100.base? 1: _temp3102 >= _temp3100.last_plus_one)){ _throw(
Null_Exception);}* _temp3102;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp3103= yyvs; void** _temp3105=
_temp3103.curr + yyvsp_offset; if( _temp3103.base == 0? 1:( _temp3105 <
_temp3103.base? 1: _temp3105 >= _temp3103.last_plus_one)){ _throw(
Null_Exception);}* _temp3105;})))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3106= yyls; struct Cyc_Yyltype* _temp3108= _temp3106.curr +(
yylsp_offset - 2); if( _temp3106.base == 0? 1:( _temp3108 < _temp3106.base? 1:
_temp3108 >= _temp3106.last_plus_one)){ _throw( Null_Exception);}* _temp3108;}).first_line,({
struct _tagged_ptr3 _temp3109= yyls; struct Cyc_Yyltype* _temp3111= _temp3109.curr
+ yylsp_offset; if( _temp3109.base == 0? 1:( _temp3111 < _temp3109.base? 1:
_temp3111 >= _temp3109.last_plus_one)){ _throw( Null_Exception);}* _temp3111;}).last_line));
_temp3099;}); _temp3098;}); break; case 65: _LL3097: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3113=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3113[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3114; _temp3114.tag=
Cyc_TypeSpecifier_tok; _temp3114.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp3115=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp3115[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp3116; _temp3116.tag= Cyc_Absyn_TypedefType;
_temp3116.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3117= yyvs; void**
_temp3119= _temp3117.curr +( yyvsp_offset - 1); if( _temp3117.base == 0? 1:(
_temp3119 < _temp3117.base? 1: _temp3119 >= _temp3117.last_plus_one)){ _throw(
Null_Exception);}* _temp3119;})); _temp3116.f2= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3120= yyvs; void** _temp3122= _temp3120.curr + yyvsp_offset;
if( _temp3120.base == 0? 1:( _temp3122 < _temp3120.base? 1: _temp3122 >=
_temp3120.last_plus_one)){ _throw( Null_Exception);}* _temp3122;})); _temp3116.f3=
0; _temp3116;}); _temp3115;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3123= yyls; struct Cyc_Yyltype* _temp3125= _temp3123.curr +(
yylsp_offset - 1); if( _temp3123.base == 0? 1:( _temp3125 < _temp3123.base? 1:
_temp3125 >= _temp3123.last_plus_one)){ _throw( Null_Exception);}* _temp3125;}).first_line,({
struct _tagged_ptr3 _temp3126= yyls; struct Cyc_Yyltype* _temp3128= _temp3126.curr
+ yylsp_offset; if( _temp3126.base == 0? 1:( _temp3128 < _temp3126.base? 1:
_temp3128 >= _temp3126.last_plus_one)){ _throw( Null_Exception);}* _temp3128;}).last_line));
_temp3114;}); _temp3113;}); break; case 66: _LL3112: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3130=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3130[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3131; _temp3131.tag=
Cyc_TypeSpecifier_tok; _temp3131.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp3132=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp3132[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp3133; _temp3133.tag= Cyc_Absyn_TupleType;
_temp3133.f1=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3134= yyls; struct Cyc_Yyltype* _temp3136= _temp3134.curr +(
yylsp_offset - 1); if( _temp3134.base == 0? 1:( _temp3136 < _temp3134.base? 1:
_temp3136 >= _temp3134.last_plus_one)){ _throw( Null_Exception);}* _temp3136;}).first_line,({
struct _tagged_ptr3 _temp3137= yyls; struct Cyc_Yyltype* _temp3139= _temp3137.curr
+( yylsp_offset - 1); if( _temp3137.base == 0? 1:( _temp3139 < _temp3137.base? 1:
_temp3139 >= _temp3137.last_plus_one)){ _throw( Null_Exception);}* _temp3139;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3140= yyvs; void** _temp3142= _temp3140.curr +(
yyvsp_offset - 1); if( _temp3140.base == 0? 1:( _temp3142 < _temp3140.base? 1:
_temp3142 >= _temp3140.last_plus_one)){ _throw( Null_Exception);}* _temp3142;}))));
_temp3133;}); _temp3132;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3143= yyls; struct Cyc_Yyltype* _temp3145= _temp3143.curr +( yylsp_offset -
3); if( _temp3143.base == 0? 1:( _temp3145 < _temp3143.base? 1: _temp3145 >=
_temp3143.last_plus_one)){ _throw( Null_Exception);}* _temp3145;}).first_line,({
struct _tagged_ptr3 _temp3146= yyls; struct Cyc_Yyltype* _temp3148= _temp3146.curr
+ yylsp_offset; if( _temp3146.base == 0? 1:( _temp3148 < _temp3146.base? 1:
_temp3148 >= _temp3146.last_plus_one)){ _throw( Null_Exception);}* _temp3148;}).last_line));
_temp3131;}); _temp3130;}); break; case 67: _LL3129: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3150=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3150[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3151; _temp3151.tag=
Cyc_TypeSpecifier_tok; _temp3151.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp3152=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp3152[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp3153; _temp3153.tag= Cyc_Absyn_RgnHandleType;
_temp3153.f1=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp3154= yyvs;
void** _temp3156= _temp3154.curr +( yyvsp_offset - 1); if( _temp3154.base == 0?
1:( _temp3156 < _temp3154.base? 1: _temp3156 >= _temp3154.last_plus_one)){
_throw( Null_Exception);}* _temp3156;})); _temp3153;}); _temp3152;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3157= yyls; struct Cyc_Yyltype* _temp3159= _temp3157.curr
+( yylsp_offset - 3); if( _temp3157.base == 0? 1:( _temp3159 < _temp3157.base? 1:
_temp3159 >= _temp3157.last_plus_one)){ _throw( Null_Exception);}* _temp3159;}).first_line,({
struct _tagged_ptr3 _temp3160= yyls; struct Cyc_Yyltype* _temp3162= _temp3160.curr
+ yylsp_offset; if( _temp3160.base == 0? 1:( _temp3162 < _temp3160.base? 1:
_temp3162 >= _temp3160.last_plus_one)){ _throw( Null_Exception);}* _temp3162;}).last_line));
_temp3151;}); _temp3150;}); break; case 68: _LL3149: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp3164=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp3164[ 0]=({ struct Cyc_Kind_tok_struct _temp3165; _temp3165.tag= Cyc_Kind_tok;
_temp3165.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp3166= yyvs; void** _temp3168= _temp3166.curr + yyvsp_offset;
if( _temp3166.base == 0? 1:( _temp3168 < _temp3166.base? 1: _temp3168 >=
_temp3166.last_plus_one)){ _throw( Null_Exception);}* _temp3168;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3169= yyls; struct Cyc_Yyltype* _temp3171= _temp3169.curr
+ yylsp_offset; if( _temp3169.base == 0? 1:( _temp3171 < _temp3169.base? 1:
_temp3171 >= _temp3169.last_plus_one)){ _throw( Null_Exception);}* _temp3171;}).first_line,({
struct _tagged_ptr3 _temp3172= yyls; struct Cyc_Yyltype* _temp3174= _temp3172.curr
+ yylsp_offset; if( _temp3172.base == 0? 1:( _temp3174 < _temp3172.base? 1:
_temp3174 >= _temp3172.last_plus_one)){ _throw( Null_Exception);}* _temp3174;}).last_line));
_temp3165;}); _temp3164;}); break; case 69: _LL3163: { struct _tagged_string*
_temp3181; void* _temp3183; struct _tuple1 _temp3179=* Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3176= yyvs; void** _temp3178= _temp3176.curr +
yyvsp_offset; if( _temp3176.base == 0? 1:( _temp3178 < _temp3176.base? 1:
_temp3178 >= _temp3176.last_plus_one)){ _throw( Null_Exception);}* _temp3178;}));
_LL3184: _temp3183= _temp3179.f1; goto _LL3182; _LL3182: _temp3181= _temp3179.f2;
goto _LL3180; _LL3180: if( _temp3183 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3185=( char*)"bad kind in type specifier";
struct _tagged_string _temp3186; _temp3186.curr= _temp3185; _temp3186.base=
_temp3185; _temp3186.last_plus_one= _temp3185 + 27; _temp3186;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3187= yyls; struct Cyc_Yyltype* _temp3189= _temp3187.curr
+ yylsp_offset; if( _temp3187.base == 0? 1:( _temp3189 < _temp3187.base? 1:
_temp3189 >= _temp3187.last_plus_one)){ _throw( Null_Exception);}* _temp3189;}).first_line,({
struct _tagged_ptr3 _temp3190= yyls; struct Cyc_Yyltype* _temp3192= _temp3190.curr
+ yylsp_offset; if( _temp3190.base == 0? 1:( _temp3192 < _temp3190.base? 1:
_temp3192 >= _temp3190.last_plus_one)){ _throw( Null_Exception);}* _temp3192;}).last_line));}
yyval=( void*)({ struct Cyc_Kind_tok_struct* _temp3193=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp3193[ 0]=({ struct Cyc_Kind_tok_struct
_temp3194; _temp3194.tag= Cyc_Kind_tok; _temp3194.f1=( void*) Cyc_Parse_id_to_kind(*
_temp3181, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3195= yyls;
struct Cyc_Yyltype* _temp3197= _temp3195.curr + yylsp_offset; if( _temp3195.base
== 0? 1:( _temp3197 < _temp3195.base? 1: _temp3197 >= _temp3195.last_plus_one)){
_throw( Null_Exception);}* _temp3197;}).first_line,({ struct _tagged_ptr3
_temp3198= yyls; struct Cyc_Yyltype* _temp3200= _temp3198.curr + yylsp_offset;
if( _temp3198.base == 0? 1:( _temp3200 < _temp3198.base? 1: _temp3200 >=
_temp3198.last_plus_one)){ _throw( Null_Exception);}* _temp3200;}).last_line));
_temp3194;}); _temp3193;}); break;} case 70: _LL3175: yyval=( void*)({ struct
Cyc_TypeQual_tok_struct* _temp3202=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct)); _temp3202[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3203; _temp3203.tag= Cyc_TypeQual_tok; _temp3203.f1=({ struct Cyc_Absyn_Tqual
_temp3204; _temp3204.q_const= 1; _temp3204.q_volatile= 0; _temp3204.q_restrict=
0; _temp3204;}); _temp3203;}); _temp3202;}); break; case 71: _LL3201: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp3206=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3206[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3207; _temp3207.tag= Cyc_TypeQual_tok; _temp3207.f1=({ struct Cyc_Absyn_Tqual
_temp3208; _temp3208.q_const= 0; _temp3208.q_volatile= 1; _temp3208.q_restrict=
0; _temp3208;}); _temp3207;}); _temp3206;}); break; case 72: _LL3205: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp3210=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3210[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3211; _temp3211.tag= Cyc_TypeQual_tok; _temp3211.f1=({ struct Cyc_Absyn_Tqual
_temp3212; _temp3212.q_const= 0; _temp3212.q_volatile= 0; _temp3212.q_restrict=
1; _temp3212;}); _temp3211;}); _temp3210;}); break; case 73: _LL3209: yyval=(
void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3214=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3214[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3215; _temp3215.tag= Cyc_TypeSpecifier_tok;
_temp3215.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3216=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3216[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3217; _temp3217.tag= Cyc_Parse_Decl_spec;
_temp3217.f1=({ struct Cyc_Absyn_Decl* _temp3218=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp3218->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp3225=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp3225[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp3226; _temp3226.tag= Cyc_Absyn_Enum_d; _temp3226.f1=({ struct Cyc_Absyn_Enumdecl*
_temp3227=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp3227->sc=( void*)(( void*) Cyc_Absyn_Public); _temp3227->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3232= yyvs; void** _temp3234= _temp3232.curr +(
yyvsp_offset - 3); if( _temp3232.base == 0? 1:( _temp3234 < _temp3232.base? 1:
_temp3234 >= _temp3232.last_plus_one)){ _throw( Null_Exception);}* _temp3234;}));
_temp3227->fields=({ struct Cyc_Core_Opt* _temp3228=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3228->v=( void*) Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3229= yyvs; void** _temp3231= _temp3229.curr +(
yyvsp_offset - 1); if( _temp3229.base == 0? 1:( _temp3231 < _temp3229.base? 1:
_temp3231 >= _temp3229.last_plus_one)){ _throw( Null_Exception);}* _temp3231;}));
_temp3228;}); _temp3227;}); _temp3226;}); _temp3225;})); _temp3218->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3219= yyls; struct Cyc_Yyltype* _temp3221= _temp3219.curr
+( yylsp_offset - 4); if( _temp3219.base == 0? 1:( _temp3221 < _temp3219.base? 1:
_temp3221 >= _temp3219.last_plus_one)){ _throw( Null_Exception);}* _temp3221;}).first_line,({
struct _tagged_ptr3 _temp3222= yyls; struct Cyc_Yyltype* _temp3224= _temp3222.curr
+ yylsp_offset; if( _temp3222.base == 0? 1:( _temp3224 < _temp3222.base? 1:
_temp3224 >= _temp3222.last_plus_one)){ _throw( Null_Exception);}* _temp3224;}).last_line);
_temp3218;}); _temp3217;}); _temp3216;})); _temp3215;}); _temp3214;}); break;
case 74: _LL3213: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3236=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3236[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3237; _temp3237.tag=
Cyc_TypeSpecifier_tok; _temp3237.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp3238=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3238[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp3239; _temp3239.tag= Cyc_Absyn_EnumType;
_temp3239.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3240= yyvs; void**
_temp3242= _temp3240.curr + yyvsp_offset; if( _temp3240.base == 0? 1:( _temp3242
< _temp3240.base? 1: _temp3242 >= _temp3240.last_plus_one)){ _throw(
Null_Exception);}* _temp3242;})); _temp3239.f2= 0; _temp3239;}); _temp3238;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3243= yyls; struct Cyc_Yyltype*
_temp3245= _temp3243.curr +( yylsp_offset - 1); if( _temp3243.base == 0? 1:(
_temp3245 < _temp3243.base? 1: _temp3245 >= _temp3243.last_plus_one)){ _throw(
Null_Exception);}* _temp3245;}).first_line,({ struct _tagged_ptr3 _temp3246=
yyls; struct Cyc_Yyltype* _temp3248= _temp3246.curr + yylsp_offset; if(
_temp3246.base == 0? 1:( _temp3248 < _temp3246.base? 1: _temp3248 >= _temp3246.last_plus_one)){
_throw( Null_Exception);}* _temp3248;}).last_line)); _temp3237;}); _temp3236;});
break; case 75: _LL3235: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp3250=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp3250[ 0]=({ struct Cyc_Enumfield_tok_struct _temp3251; _temp3251.tag= Cyc_Enumfield_tok;
_temp3251.f1=({ struct Cyc_Absyn_Enumfield* _temp3252=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3252->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3259= yyvs; void** _temp3261= _temp3259.curr +
yyvsp_offset; if( _temp3259.base == 0? 1:( _temp3261 < _temp3259.base? 1:
_temp3261 >= _temp3259.last_plus_one)){ _throw( Null_Exception);}* _temp3261;}));
_temp3252->tag= 0; _temp3252->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3253= yyls; struct Cyc_Yyltype* _temp3255= _temp3253.curr +
yylsp_offset; if( _temp3253.base == 0? 1:( _temp3255 < _temp3253.base? 1:
_temp3255 >= _temp3253.last_plus_one)){ _throw( Null_Exception);}* _temp3255;}).first_line,({
struct _tagged_ptr3 _temp3256= yyls; struct Cyc_Yyltype* _temp3258= _temp3256.curr
+ yylsp_offset; if( _temp3256.base == 0? 1:( _temp3258 < _temp3256.base? 1:
_temp3258 >= _temp3256.last_plus_one)){ _throw( Null_Exception);}* _temp3258;}).last_line);
_temp3252;}); _temp3251;}); _temp3250;}); break; case 76: _LL3249: yyval=( void*)({
struct Cyc_Enumfield_tok_struct* _temp3263=( struct Cyc_Enumfield_tok_struct*)
GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp3263[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp3264; _temp3264.tag= Cyc_Enumfield_tok; _temp3264.f1=({
struct Cyc_Absyn_Enumfield* _temp3265=( struct Cyc_Absyn_Enumfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumfield)); _temp3265->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3275= yyvs; void** _temp3277= _temp3275.curr +(
yyvsp_offset - 2); if( _temp3275.base == 0? 1:( _temp3277 < _temp3275.base? 1:
_temp3277 >= _temp3275.last_plus_one)){ _throw( Null_Exception);}* _temp3277;}));
_temp3265->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp3272= yyvs; void** _temp3274= _temp3272.curr + yyvsp_offset; if( _temp3272.base
== 0? 1:( _temp3274 < _temp3272.base? 1: _temp3274 >= _temp3272.last_plus_one)){
_throw( Null_Exception);}* _temp3274;})); _temp3265->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3266= yyls; struct Cyc_Yyltype* _temp3268= _temp3266.curr
+( yylsp_offset - 2); if( _temp3266.base == 0? 1:( _temp3268 < _temp3266.base? 1:
_temp3268 >= _temp3266.last_plus_one)){ _throw( Null_Exception);}* _temp3268;}).first_line,({
struct _tagged_ptr3 _temp3269= yyls; struct Cyc_Yyltype* _temp3271= _temp3269.curr
+ yylsp_offset; if( _temp3269.base == 0? 1:( _temp3271 < _temp3269.base? 1:
_temp3271 >= _temp3269.last_plus_one)){ _throw( Null_Exception);}* _temp3271;}).last_line);
_temp3265;}); _temp3264;}); _temp3263;}); break; case 77: _LL3262: yyval=( void*)({
struct Cyc_EnumfieldList_tok_struct* _temp3279=( struct Cyc_EnumfieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct)); _temp3279[ 0]=({
struct Cyc_EnumfieldList_tok_struct _temp3280; _temp3280.tag= Cyc_EnumfieldList_tok;
_temp3280.f1=({ struct Cyc_List_List* _temp3281=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3281->hd=( void*) Cyc_yyget_Enumfield_tok(({
struct _tagged_ptr2 _temp3282= yyvs; void** _temp3284= _temp3282.curr +
yyvsp_offset; if( _temp3282.base == 0? 1:( _temp3284 < _temp3282.base? 1:
_temp3284 >= _temp3282.last_plus_one)){ _throw( Null_Exception);}* _temp3284;}));
_temp3281->tl= 0; _temp3281;}); _temp3280;}); _temp3279;}); break; case 78:
_LL3278: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct* _temp3286=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp3286[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp3287; _temp3287.tag=
Cyc_EnumfieldList_tok; _temp3287.f1=({ struct Cyc_List_List* _temp3288=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3288->hd=( void*)
Cyc_yyget_Enumfield_tok(({ struct _tagged_ptr2 _temp3292= yyvs; void** _temp3294=
_temp3292.curr +( yyvsp_offset - 2); if( _temp3292.base == 0? 1:( _temp3294 <
_temp3292.base? 1: _temp3294 >= _temp3292.last_plus_one)){ _throw(
Null_Exception);}* _temp3294;})); _temp3288->tl= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3289= yyvs; void** _temp3291= _temp3289.curr +
yyvsp_offset; if( _temp3289.base == 0? 1:( _temp3291 < _temp3289.base? 1:
_temp3291 >= _temp3289.last_plus_one)){ _throw( Null_Exception);}* _temp3291;}));
_temp3288;}); _temp3287;}); _temp3286;}); break; case 79: _LL3285: { void* t;{
void* _temp3299= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3296=
yyvs; void** _temp3298= _temp3296.curr +( yyvsp_offset - 3); if( _temp3296.base
== 0? 1:( _temp3298 < _temp3296.base? 1: _temp3298 >= _temp3296.last_plus_one)){
_throw( Null_Exception);}* _temp3298;})); _LL3301: if( _temp3299 ==( void*) Cyc_Parse_Struct_su){
goto _LL3302;} else{ goto _LL3303;} _LL3303: if( _temp3299 ==( void*) Cyc_Parse_Union_su){
goto _LL3304;} else{ goto _LL3300;} _LL3302: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp3305=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp3305[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp3306; _temp3306.tag= Cyc_Absyn_AnonStructType; _temp3306.f1= Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3307= yyvs; void** _temp3309= _temp3307.curr +(
yyvsp_offset - 1); if( _temp3307.base == 0? 1:( _temp3309 < _temp3307.base? 1:
_temp3309 >= _temp3307.last_plus_one)){ _throw( Null_Exception);}* _temp3309;}));
_temp3306;}); _temp3305;}); goto _LL3300; _LL3304: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp3310=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp3310[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp3311; _temp3311.tag= Cyc_Absyn_AnonUnionType; _temp3311.f1= Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3312= yyvs; void** _temp3314= _temp3312.curr +(
yyvsp_offset - 1); if( _temp3312.base == 0? 1:( _temp3314 < _temp3312.base? 1:
_temp3314 >= _temp3312.last_plus_one)){ _throw( Null_Exception);}* _temp3314;}));
_temp3311;}); _temp3310;}); goto _LL3300; _LL3300:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3315=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3315[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3316; _temp3316.tag=
Cyc_TypeSpecifier_tok; _temp3316.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp3317=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp3317[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp3318; _temp3318.tag= Cyc_Parse_Type_spec;
_temp3318.f1=( void*) t; _temp3318.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3319= yyls; struct Cyc_Yyltype* _temp3321= _temp3319.curr +(
yylsp_offset - 3); if( _temp3319.base == 0? 1:( _temp3321 < _temp3319.base? 1:
_temp3321 >= _temp3319.last_plus_one)){ _throw( Null_Exception);}* _temp3321;}).first_line,({
struct _tagged_ptr3 _temp3322= yyls; struct Cyc_Yyltype* _temp3324= _temp3322.curr
+ yylsp_offset; if( _temp3322.base == 0? 1:( _temp3324 < _temp3322.base? 1:
_temp3324 >= _temp3322.last_plus_one)){ _throw( Null_Exception);}* _temp3324;}).last_line);
_temp3318;}); _temp3317;})); _temp3316;}); _temp3315;}); break;} case 80:
_LL3295: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3367= yyls; struct Cyc_Yyltype* _temp3369= _temp3367.curr
+( yylsp_offset - 3); if( _temp3367.base == 0? 1:( _temp3369 < _temp3367.base? 1:
_temp3369 >= _temp3367.last_plus_one)){ _throw( Null_Exception);}* _temp3369;}).first_line,({
struct _tagged_ptr3 _temp3370= yyls; struct Cyc_Yyltype* _temp3372= _temp3370.curr
+( yylsp_offset - 3); if( _temp3370.base == 0? 1:( _temp3372 < _temp3370.base? 1:
_temp3372 >= _temp3370.last_plus_one)){ _throw( Null_Exception);}* _temp3372;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3373= yyvs; void** _temp3375=
_temp3373.curr +( yyvsp_offset - 3); if( _temp3373.base == 0? 1:( _temp3375 <
_temp3373.base? 1: _temp3375 >= _temp3373.last_plus_one)){ _throw(
Null_Exception);}* _temp3375;}))); struct Cyc_Absyn_Decl* d;{ void* _temp3329=
Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3326= yyvs; void**
_temp3328= _temp3326.curr +( yyvsp_offset - 5); if( _temp3326.base == 0? 1:(
_temp3328 < _temp3326.base? 1: _temp3328 >= _temp3326.last_plus_one)){ _throw(
Null_Exception);}* _temp3328;})); _LL3331: if( _temp3329 ==( void*) Cyc_Parse_Struct_su){
goto _LL3332;} else{ goto _LL3333;} _LL3333: if( _temp3329 ==( void*) Cyc_Parse_Union_su){
goto _LL3334;} else{ goto _LL3330;} _LL3332: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3335=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3335->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3336= yyvs; void** _temp3338= _temp3336.curr +(
yyvsp_offset - 4); if( _temp3336.base == 0? 1:( _temp3338 < _temp3336.base? 1:
_temp3338 >= _temp3336.last_plus_one)){ _throw( Null_Exception);}* _temp3338;}));
_temp3335;}), ts,({ struct Cyc_Core_Opt* _temp3339=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3339->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3340= yyvs; void** _temp3342= _temp3340.curr +(
yyvsp_offset - 1); if( _temp3340.base == 0? 1:( _temp3342 < _temp3340.base? 1:
_temp3342 >= _temp3340.last_plus_one)){ _throw( Null_Exception);}* _temp3342;}));
_temp3339;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3343=
yyls; struct Cyc_Yyltype* _temp3345= _temp3343.curr +( yylsp_offset - 5); if(
_temp3343.base == 0? 1:( _temp3345 < _temp3343.base? 1: _temp3345 >= _temp3343.last_plus_one)){
_throw( Null_Exception);}* _temp3345;}).first_line,({ struct _tagged_ptr3
_temp3346= yyls; struct Cyc_Yyltype* _temp3348= _temp3346.curr + yylsp_offset;
if( _temp3346.base == 0? 1:( _temp3348 < _temp3346.base? 1: _temp3348 >=
_temp3346.last_plus_one)){ _throw( Null_Exception);}* _temp3348;}).last_line));
goto _LL3330; _LL3334: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3349=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3349->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3350= yyvs; void** _temp3352= _temp3350.curr +( yyvsp_offset -
4); if( _temp3350.base == 0? 1:( _temp3352 < _temp3350.base? 1: _temp3352 >=
_temp3350.last_plus_one)){ _throw( Null_Exception);}* _temp3352;})); _temp3349;}),
ts,({ struct Cyc_Core_Opt* _temp3353=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp3353->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3354= yyvs; void** _temp3356= _temp3354.curr +(
yyvsp_offset - 1); if( _temp3354.base == 0? 1:( _temp3356 < _temp3354.base? 1:
_temp3356 >= _temp3354.last_plus_one)){ _throw( Null_Exception);}* _temp3356;}));
_temp3353;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3357=
yyls; struct Cyc_Yyltype* _temp3359= _temp3357.curr +( yylsp_offset - 5); if(
_temp3357.base == 0? 1:( _temp3359 < _temp3357.base? 1: _temp3359 >= _temp3357.last_plus_one)){
_throw( Null_Exception);}* _temp3359;}).first_line,({ struct _tagged_ptr3
_temp3360= yyls; struct Cyc_Yyltype* _temp3362= _temp3360.curr + yylsp_offset;
if( _temp3360.base == 0? 1:( _temp3362 < _temp3360.base? 1: _temp3362 >=
_temp3360.last_plus_one)){ _throw( Null_Exception);}* _temp3362;}).last_line));
goto _LL3330; _LL3330:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3363=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3363[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3364; _temp3364.tag=
Cyc_TypeSpecifier_tok; _temp3364.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3365=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3365[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3366; _temp3366.tag= Cyc_Parse_Decl_spec;
_temp3366.f1= d; _temp3366;}); _temp3365;})); _temp3364;}); _temp3363;}); break;}
case 81: _LL3325: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3418= yyls; struct Cyc_Yyltype* _temp3420= _temp3418.curr
+( yylsp_offset - 3); if( _temp3418.base == 0? 1:( _temp3420 < _temp3418.base? 1:
_temp3420 >= _temp3418.last_plus_one)){ _throw( Null_Exception);}* _temp3420;}).first_line,({
struct _tagged_ptr3 _temp3421= yyls; struct Cyc_Yyltype* _temp3423= _temp3421.curr
+( yylsp_offset - 3); if( _temp3421.base == 0? 1:( _temp3423 < _temp3421.base? 1:
_temp3423 >= _temp3421.last_plus_one)){ _throw( Null_Exception);}* _temp3423;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3424= yyvs; void** _temp3426=
_temp3424.curr +( yyvsp_offset - 3); if( _temp3424.base == 0? 1:( _temp3426 <
_temp3424.base? 1: _temp3426 >= _temp3424.last_plus_one)){ _throw(
Null_Exception);}* _temp3426;}))); struct Cyc_Absyn_Decl* d;{ void* _temp3380=
Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3377= yyvs; void**
_temp3379= _temp3377.curr +( yyvsp_offset - 5); if( _temp3377.base == 0? 1:(
_temp3379 < _temp3377.base? 1: _temp3379 >= _temp3377.last_plus_one)){ _throw(
Null_Exception);}* _temp3379;})); _LL3382: if( _temp3380 ==( void*) Cyc_Parse_Struct_su){
goto _LL3383;} else{ goto _LL3384;} _LL3384: if( _temp3380 ==( void*) Cyc_Parse_Union_su){
goto _LL3385;} else{ goto _LL3381;} _LL3383: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3386=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3386->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3387= yyvs; void** _temp3389= _temp3387.curr +(
yyvsp_offset - 4); if( _temp3387.base == 0? 1:( _temp3389 < _temp3387.base? 1:
_temp3389 >= _temp3387.last_plus_one)){ _throw( Null_Exception);}* _temp3389;}));
_temp3386;}), ts,({ struct Cyc_Core_Opt* _temp3390=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3390->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3391= yyvs; void** _temp3393= _temp3391.curr +(
yyvsp_offset - 1); if( _temp3391.base == 0? 1:( _temp3393 < _temp3391.base? 1:
_temp3393 >= _temp3391.last_plus_one)){ _throw( Null_Exception);}* _temp3393;}));
_temp3390;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3394=
yyls; struct Cyc_Yyltype* _temp3396= _temp3394.curr +( yylsp_offset - 5); if(
_temp3394.base == 0? 1:( _temp3396 < _temp3394.base? 1: _temp3396 >= _temp3394.last_plus_one)){
_throw( Null_Exception);}* _temp3396;}).first_line,({ struct _tagged_ptr3
_temp3397= yyls; struct Cyc_Yyltype* _temp3399= _temp3397.curr + yylsp_offset;
if( _temp3397.base == 0? 1:( _temp3399 < _temp3397.base? 1: _temp3399 >=
_temp3397.last_plus_one)){ _throw( Null_Exception);}* _temp3399;}).last_line));
goto _LL3381; _LL3385: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3400=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3400->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3401= yyvs; void** _temp3403= _temp3401.curr +( yyvsp_offset -
4); if( _temp3401.base == 0? 1:( _temp3403 < _temp3401.base? 1: _temp3403 >=
_temp3401.last_plus_one)){ _throw( Null_Exception);}* _temp3403;})); _temp3400;}),
ts,({ struct Cyc_Core_Opt* _temp3404=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp3404->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3405= yyvs; void** _temp3407= _temp3405.curr +(
yyvsp_offset - 1); if( _temp3405.base == 0? 1:( _temp3407 < _temp3405.base? 1:
_temp3407 >= _temp3405.last_plus_one)){ _throw( Null_Exception);}* _temp3407;}));
_temp3404;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3408=
yyls; struct Cyc_Yyltype* _temp3410= _temp3408.curr +( yylsp_offset - 5); if(
_temp3408.base == 0? 1:( _temp3410 < _temp3408.base? 1: _temp3410 >= _temp3408.last_plus_one)){
_throw( Null_Exception);}* _temp3410;}).first_line,({ struct _tagged_ptr3
_temp3411= yyls; struct Cyc_Yyltype* _temp3413= _temp3411.curr + yylsp_offset;
if( _temp3411.base == 0? 1:( _temp3413 < _temp3411.base? 1: _temp3413 >=
_temp3411.last_plus_one)){ _throw( Null_Exception);}* _temp3413;}).last_line));
goto _LL3381; _LL3381:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3414=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3414[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3415; _temp3415.tag=
Cyc_TypeSpecifier_tok; _temp3415.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3416=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3416[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3417; _temp3417.tag= Cyc_Parse_Decl_spec;
_temp3417.f1= d; _temp3417;}); _temp3416;})); _temp3415;}); _temp3414;}); break;}
case 82: _LL3376:{ void* _temp3431= Cyc_yyget_StructOrUnion_tok(({ struct
_tagged_ptr2 _temp3428= yyvs; void** _temp3430= _temp3428.curr +( yyvsp_offset -
2); if( _temp3428.base == 0? 1:( _temp3430 < _temp3428.base? 1: _temp3430 >=
_temp3428.last_plus_one)){ _throw( Null_Exception);}* _temp3430;})); _LL3433:
if( _temp3431 ==( void*) Cyc_Parse_Struct_su){ goto _LL3434;} else{ goto _LL3435;}
_LL3435: if( _temp3431 ==( void*) Cyc_Parse_Union_su){ goto _LL3436;} else{ goto
_LL3432;} _LL3434: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3437=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3437[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3438; _temp3438.tag=
Cyc_TypeSpecifier_tok; _temp3438.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3439=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3439[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3440; _temp3440.tag= Cyc_Absyn_StructType;
_temp3440.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3441= yyvs; void** _temp3443= _temp3441.curr +( yyvsp_offset - 1); if(
_temp3441.base == 0? 1:( _temp3443 < _temp3441.base? 1: _temp3443 >= _temp3441.last_plus_one)){
_throw( Null_Exception);}* _temp3443;})); _temp3440.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3444= yyvs; void** _temp3446= _temp3444.curr +
yyvsp_offset; if( _temp3444.base == 0? 1:( _temp3446 < _temp3444.base? 1:
_temp3446 >= _temp3444.last_plus_one)){ _throw( Null_Exception);}* _temp3446;}));
_temp3440.f3= 0; _temp3440;}); _temp3439;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3447= yyls; struct Cyc_Yyltype* _temp3449= _temp3447.curr
+( yylsp_offset - 2); if( _temp3447.base == 0? 1:( _temp3449 < _temp3447.base? 1:
_temp3449 >= _temp3447.last_plus_one)){ _throw( Null_Exception);}* _temp3449;}).first_line,({
struct _tagged_ptr3 _temp3450= yyls; struct Cyc_Yyltype* _temp3452= _temp3450.curr
+ yylsp_offset; if( _temp3450.base == 0? 1:( _temp3452 < _temp3450.base? 1:
_temp3452 >= _temp3450.last_plus_one)){ _throw( Null_Exception);}* _temp3452;}).last_line));
_temp3438;}); _temp3437;}); goto _LL3432; _LL3436: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3453=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3453[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3454; _temp3454.tag=
Cyc_TypeSpecifier_tok; _temp3454.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp3455=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3455[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp3456; _temp3456.tag= Cyc_Absyn_UnionType;
_temp3456.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3457= yyvs; void** _temp3459= _temp3457.curr +( yyvsp_offset - 1); if(
_temp3457.base == 0? 1:( _temp3459 < _temp3457.base? 1: _temp3459 >= _temp3457.last_plus_one)){
_throw( Null_Exception);}* _temp3459;})); _temp3456.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3460= yyvs; void** _temp3462= _temp3460.curr +
yyvsp_offset; if( _temp3460.base == 0? 1:( _temp3462 < _temp3460.base? 1:
_temp3462 >= _temp3460.last_plus_one)){ _throw( Null_Exception);}* _temp3462;}));
_temp3456.f3= 0; _temp3456;}); _temp3455;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3463= yyls; struct Cyc_Yyltype* _temp3465= _temp3463.curr
+( yylsp_offset - 2); if( _temp3463.base == 0? 1:( _temp3465 < _temp3463.base? 1:
_temp3465 >= _temp3463.last_plus_one)){ _throw( Null_Exception);}* _temp3465;}).first_line,({
struct _tagged_ptr3 _temp3466= yyls; struct Cyc_Yyltype* _temp3468= _temp3466.curr
+ yylsp_offset; if( _temp3466.base == 0? 1:( _temp3468 < _temp3466.base? 1:
_temp3468 >= _temp3466.last_plus_one)){ _throw( Null_Exception);}* _temp3468;}).last_line));
_temp3454;}); _temp3453;}); goto _LL3432; _LL3432:;} break; case 83: _LL3427:{
void* _temp3473= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3470=
yyvs; void** _temp3472= _temp3470.curr +( yyvsp_offset - 2); if( _temp3470.base
== 0? 1:( _temp3472 < _temp3470.base? 1: _temp3472 >= _temp3470.last_plus_one)){
_throw( Null_Exception);}* _temp3472;})); _LL3475: if( _temp3473 ==( void*) Cyc_Parse_Struct_su){
goto _LL3476;} else{ goto _LL3477;} _LL3477: if( _temp3473 ==( void*) Cyc_Parse_Union_su){
goto _LL3478;} else{ goto _LL3474;} _LL3476: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3479=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3479[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3480; _temp3480.tag=
Cyc_TypeSpecifier_tok; _temp3480.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3481=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3481[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3482; _temp3482.tag= Cyc_Absyn_StructType;
_temp3482.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3483= yyvs; void** _temp3485= _temp3483.curr +( yyvsp_offset - 1); if(
_temp3483.base == 0? 1:( _temp3485 < _temp3483.base? 1: _temp3485 >= _temp3483.last_plus_one)){
_throw( Null_Exception);}* _temp3485;})); _temp3482.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3486= yyvs; void** _temp3488= _temp3486.curr +
yyvsp_offset; if( _temp3486.base == 0? 1:( _temp3488 < _temp3486.base? 1:
_temp3488 >= _temp3486.last_plus_one)){ _throw( Null_Exception);}* _temp3488;}));
_temp3482.f3= 0; _temp3482;}); _temp3481;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3489= yyls; struct Cyc_Yyltype* _temp3491= _temp3489.curr
+( yylsp_offset - 2); if( _temp3489.base == 0? 1:( _temp3491 < _temp3489.base? 1:
_temp3491 >= _temp3489.last_plus_one)){ _throw( Null_Exception);}* _temp3491;}).first_line,({
struct _tagged_ptr3 _temp3492= yyls; struct Cyc_Yyltype* _temp3494= _temp3492.curr
+ yylsp_offset; if( _temp3492.base == 0? 1:( _temp3494 < _temp3492.base? 1:
_temp3494 >= _temp3492.last_plus_one)){ _throw( Null_Exception);}* _temp3494;}).last_line));
_temp3480;}); _temp3479;}); goto _LL3474; _LL3478: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3495=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3495[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3496; _temp3496.tag=
Cyc_TypeSpecifier_tok; _temp3496.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp3497=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3497[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp3498; _temp3498.tag= Cyc_Absyn_UnionType;
_temp3498.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3499= yyvs; void** _temp3501= _temp3499.curr +( yyvsp_offset - 1); if(
_temp3499.base == 0? 1:( _temp3501 < _temp3499.base? 1: _temp3501 >= _temp3499.last_plus_one)){
_throw( Null_Exception);}* _temp3501;})); _temp3498.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3502= yyvs; void** _temp3504= _temp3502.curr +
yyvsp_offset; if( _temp3502.base == 0? 1:( _temp3504 < _temp3502.base? 1:
_temp3504 >= _temp3502.last_plus_one)){ _throw( Null_Exception);}* _temp3504;}));
_temp3498.f3= 0; _temp3498;}); _temp3497;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3505= yyls; struct Cyc_Yyltype* _temp3507= _temp3505.curr
+( yylsp_offset - 2); if( _temp3505.base == 0? 1:( _temp3507 < _temp3505.base? 1:
_temp3507 >= _temp3505.last_plus_one)){ _throw( Null_Exception);}* _temp3507;}).first_line,({
struct _tagged_ptr3 _temp3508= yyls; struct Cyc_Yyltype* _temp3510= _temp3508.curr
+ yylsp_offset; if( _temp3508.base == 0? 1:( _temp3510 < _temp3508.base? 1:
_temp3510 >= _temp3508.last_plus_one)){ _throw( Null_Exception);}* _temp3510;}).last_line));
_temp3496;}); _temp3495;}); goto _LL3474; _LL3474:;} break; case 84: _LL3469:
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3512=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3512[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3513; _temp3513.tag= Cyc_TypeList_tok; _temp3513.f1= 0; _temp3513;});
_temp3512;}); break; case 85: _LL3511: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3515=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3515[ 0]=({ struct Cyc_TypeList_tok_struct _temp3516; _temp3516.tag= Cyc_TypeList_tok;
_temp3516.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3517= yyvs; void** _temp3519=
_temp3517.curr +( yyvsp_offset - 1); if( _temp3517.base == 0? 1:( _temp3519 <
_temp3517.base? 1: _temp3519 >= _temp3517.last_plus_one)){ _throw(
Null_Exception);}* _temp3519;}))); _temp3516;}); _temp3515;}); break; case 86:
_LL3514:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp3521= Cyc_Parse_lbuf;
if( _temp3521 == 0){ _throw( Null_Exception);} _temp3521->v;}))->lex_curr_pos -=
1; yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3522=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3522[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3523; _temp3523.tag= Cyc_TypeList_tok; _temp3523.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3524= yyvs; void** _temp3526= _temp3524.curr +( yyvsp_offset -
1); if( _temp3524.base == 0? 1:( _temp3526 < _temp3524.base? 1: _temp3526 >=
_temp3524.last_plus_one)){ _throw( Null_Exception);}* _temp3526;}))); _temp3523;});
_temp3522;}); break; case 87: _LL3520: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3528=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3528[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3529; _temp3529.tag=
Cyc_StructOrUnion_tok; _temp3529.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp3529;}); _temp3528;}); break; case 88: _LL3527: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3531=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3531[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3532; _temp3532.tag=
Cyc_StructOrUnion_tok; _temp3532.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp3532;}); _temp3531;}); break; case 89: _LL3530: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3534=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3534[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3535; _temp3535.tag= Cyc_StructFieldDeclList_tok; _temp3535.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3536= yyvs; void** _temp3538= _temp3536.curr +
yyvsp_offset; if( _temp3536.base == 0? 1:( _temp3538 < _temp3536.base? 1:
_temp3538 >= _temp3536.last_plus_one)){ _throw( Null_Exception);}* _temp3538;}))));
_temp3535;}); _temp3534;}); break; case 90: _LL3533: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3540=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3540[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3541; _temp3541.tag= Cyc_StructFieldDeclListList_tok; _temp3541.f1=({
struct Cyc_List_List* _temp3542=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3542->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3543= yyvs; void** _temp3545= _temp3543.curr +
yyvsp_offset; if( _temp3543.base == 0? 1:( _temp3545 < _temp3543.base? 1:
_temp3545 >= _temp3543.last_plus_one)){ _throw( Null_Exception);}* _temp3545;}));
_temp3542->tl= 0; _temp3542;}); _temp3541;}); _temp3540;}); break; case 91:
_LL3539: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3547=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3547[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3548; _temp3548.tag= Cyc_StructFieldDeclListList_tok; _temp3548.f1=({
struct Cyc_List_List* _temp3549=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3549->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3553= yyvs; void** _temp3555= _temp3553.curr +
yyvsp_offset; if( _temp3553.base == 0? 1:( _temp3555 < _temp3553.base? 1:
_temp3555 >= _temp3553.last_plus_one)){ _throw( Null_Exception);}* _temp3555;}));
_temp3549->tl= Cyc_yyget_StructFieldDeclListList_tok(({ struct _tagged_ptr2
_temp3550= yyvs; void** _temp3552= _temp3550.curr +( yyvsp_offset - 1); if(
_temp3550.base == 0? 1:( _temp3552 < _temp3550.base? 1: _temp3552 >= _temp3550.last_plus_one)){
_throw( Null_Exception);}* _temp3552;})); _temp3549;}); _temp3548;}); _temp3547;});
break; case 92: _LL3546: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp3557=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3557[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3558; _temp3558.tag=
Cyc_InitDeclList_tok; _temp3558.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2
_temp3559= yyvs; void** _temp3561= _temp3559.curr + yyvsp_offset; if( _temp3559.base
== 0? 1:( _temp3561 < _temp3559.base? 1: _temp3561 >= _temp3559.last_plus_one)){
_throw( Null_Exception);}* _temp3561;}))); _temp3558;}); _temp3557;}); break;
case 93: _LL3556: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp3563=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3563[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3564; _temp3564.tag=
Cyc_InitDeclList_tok; _temp3564.f1=({ struct Cyc_List_List* _temp3565=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3565->hd=( void*)
Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3566= yyvs; void** _temp3568=
_temp3566.curr + yyvsp_offset; if( _temp3566.base == 0? 1:( _temp3568 <
_temp3566.base? 1: _temp3568 >= _temp3566.last_plus_one)){ _throw(
Null_Exception);}* _temp3568;})); _temp3565->tl= 0; _temp3565;}); _temp3564;});
_temp3563;}); break; case 94: _LL3562: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp3570=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3570[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3571; _temp3571.tag=
Cyc_InitDeclList_tok; _temp3571.f1=({ struct Cyc_List_List* _temp3572=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3572->hd=( void*)
Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3576= yyvs; void** _temp3578=
_temp3576.curr + yyvsp_offset; if( _temp3576.base == 0? 1:( _temp3578 <
_temp3576.base? 1: _temp3578 >= _temp3576.last_plus_one)){ _throw(
Null_Exception);}* _temp3578;})); _temp3572->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3573= yyvs; void** _temp3575= _temp3573.curr +(
yyvsp_offset - 2); if( _temp3573.base == 0? 1:( _temp3575 < _temp3573.base? 1:
_temp3575 >= _temp3573.last_plus_one)){ _throw( Null_Exception);}* _temp3575;}));
_temp3572;}); _temp3571;}); _temp3570;}); break; case 95: _LL3569: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp3580=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp3580[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp3581; _temp3581.tag= Cyc_InitDecl_tok; _temp3581.f1=({ struct _tuple13*
_temp3582=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp3582->f1=
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3583= yyvs; void**
_temp3585= _temp3583.curr + yyvsp_offset; if( _temp3583.base == 0? 1:( _temp3585
< _temp3583.base? 1: _temp3585 >= _temp3583.last_plus_one)){ _throw(
Null_Exception);}* _temp3585;})); _temp3582->f2= 0; _temp3582;}); _temp3581;});
_temp3580;}); break; case 96: _LL3579: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp3587=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3587[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3588; _temp3588.tag= Cyc_InitDecl_tok;
_temp3588.f1=({ struct _tuple13* _temp3589=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3589->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3593= yyvs; void** _temp3595= _temp3593.curr +( yyvsp_offset -
2); if( _temp3593.base == 0? 1:( _temp3595 < _temp3593.base? 1: _temp3595 >=
_temp3593.last_plus_one)){ _throw( Null_Exception);}* _temp3595;})); _temp3589->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3590= yyvs;
void** _temp3592= _temp3590.curr + yyvsp_offset; if( _temp3590.base == 0? 1:(
_temp3592 < _temp3590.base? 1: _temp3592 >= _temp3590.last_plus_one)){ _throw(
Null_Exception);}* _temp3592;})); _temp3589;}); _temp3588;}); _temp3587;});
break; case 97: _LL3586: { struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3626= yyvs; void** _temp3628= _temp3626.curr +(
yyvsp_offset - 2); if( _temp3626.base == 0? 1:( _temp3628 < _temp3626.base? 1:
_temp3628 >= _temp3626.last_plus_one)){ _throw( Null_Exception);}* _temp3628;}))).f1;
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp3623= yyvs; void** _temp3625= _temp3623.curr +( yyvsp_offset - 2); if(
_temp3623.base == 0? 1:( _temp3625 < _temp3623.base? 1: _temp3625 >= _temp3623.last_plus_one)){
_throw( Null_Exception);}* _temp3625;}))).f3; void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3614= yyvs; void**
_temp3616= _temp3614.curr +( yyvsp_offset - 2); if( _temp3614.base == 0? 1:(
_temp3616 < _temp3614.base? 1: _temp3616 >= _temp3614.last_plus_one)){ _throw(
Null_Exception);}* _temp3616;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3617= yyls; struct Cyc_Yyltype* _temp3619= _temp3617.curr +(
yylsp_offset - 2); if( _temp3617.base == 0? 1:( _temp3619 < _temp3617.base? 1:
_temp3619 >= _temp3617.last_plus_one)){ _throw( Null_Exception);}* _temp3619;}).first_line,({
struct _tagged_ptr3 _temp3620= yyls; struct Cyc_Yyltype* _temp3622= _temp3620.curr
+( yylsp_offset - 2); if( _temp3620.base == 0? 1:( _temp3622 < _temp3620.base? 1:
_temp3622 >= _temp3620.last_plus_one)){ _throw( Null_Exception);}* _temp3622;}).last_line));
struct Cyc_List_List* _temp3602; struct Cyc_List_List* _temp3604; struct _tuple0
_temp3600=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3597= yyvs; void** _temp3599= _temp3597.curr +(
yyvsp_offset - 1); if( _temp3597.base == 0? 1:( _temp3599 < _temp3597.base? 1:
_temp3599 >= _temp3597.last_plus_one)){ _throw( Null_Exception);}* _temp3599;})));
_LL3605: _temp3604= _temp3600.f1; goto _LL3603; _LL3603: _temp3602= _temp3600.f2;
goto _LL3601; _LL3601: { struct Cyc_List_List* info=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, t, _temp3604, atts), _temp3602); yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3606=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3606[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3607; _temp3607.tag= Cyc_StructFieldDeclList_tok; _temp3607.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple8*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3608= yyls; struct Cyc_Yyltype* _temp3610= _temp3608.curr
+( yylsp_offset - 2); if( _temp3608.base == 0? 1:( _temp3610 < _temp3608.base? 1:
_temp3610 >= _temp3608.last_plus_one)){ _throw( Null_Exception);}* _temp3610;}).first_line,({
struct _tagged_ptr3 _temp3611= yyls; struct Cyc_Yyltype* _temp3613= _temp3611.curr
+( yylsp_offset - 1); if( _temp3611.base == 0? 1:( _temp3613 < _temp3611.base? 1:
_temp3613 >= _temp3611.last_plus_one)){ _throw( Null_Exception);}* _temp3613;}).last_line),
info); _temp3607;}); _temp3606;}); break;}} case 98: _LL3596: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp3630=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3630[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3631; _temp3631.tag= Cyc_QualSpecList_tok;
_temp3631.f1=({ struct _tuple14* _temp3632=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3632->f1= Cyc_Absyn_empty_tqual(); _temp3632->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp3636=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3636->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3637= yyvs; void** _temp3639= _temp3637.curr +(
yyvsp_offset - 1); if( _temp3637.base == 0? 1:( _temp3639 < _temp3637.base? 1:
_temp3639 >= _temp3637.last_plus_one)){ _throw( Null_Exception);}* _temp3639;}));
_temp3636->tl= 0; _temp3636;}); _temp3632->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3633= yyvs; void** _temp3635= _temp3633.curr +
yyvsp_offset; if( _temp3633.base == 0? 1:( _temp3635 < _temp3633.base? 1:
_temp3635 >= _temp3633.last_plus_one)){ _throw( Null_Exception);}* _temp3635;}));
_temp3632;}); _temp3631;}); _temp3630;}); break; case 99: _LL3629: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp3641=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3641[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3642; _temp3642.tag= Cyc_QualSpecList_tok;
_temp3642.f1=({ struct _tuple14* _temp3643=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3643->f1=(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3657= yyvs; void** _temp3659= _temp3657.curr + yyvsp_offset;
if( _temp3657.base == 0? 1:( _temp3659 < _temp3657.base? 1: _temp3659 >=
_temp3657.last_plus_one)){ _throw( Null_Exception);}* _temp3659;}))).f1;
_temp3643->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp3650=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3650->hd=(
void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2 _temp3654= yyvs; void**
_temp3656= _temp3654.curr +( yyvsp_offset - 2); if( _temp3654.base == 0? 1:(
_temp3656 < _temp3654.base? 1: _temp3656 >= _temp3654.last_plus_one)){ _throw(
Null_Exception);}* _temp3656;})); _temp3650->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3651= yyvs; void** _temp3653= _temp3651.curr +
yyvsp_offset; if( _temp3651.base == 0? 1:( _temp3653 < _temp3651.base? 1:
_temp3653 >= _temp3651.last_plus_one)){ _throw( Null_Exception);}* _temp3653;}))).f2;
_temp3650;}); _temp3643->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3644= yyvs; void** _temp3646= _temp3644.curr +(
yyvsp_offset - 1); if( _temp3644.base == 0? 1:( _temp3646 < _temp3644.base? 1:
_temp3646 >= _temp3644.last_plus_one)){ _throw( Null_Exception);}* _temp3646;})),(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3647= yyvs; void**
_temp3649= _temp3647.curr + yyvsp_offset; if( _temp3647.base == 0? 1:( _temp3649
< _temp3647.base? 1: _temp3649 >= _temp3647.last_plus_one)){ _throw(
Null_Exception);}* _temp3649;}))).f3); _temp3643;}); _temp3642;}); _temp3641;});
break; case 100: _LL3640: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp3661=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3661[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3662; _temp3662.tag=
Cyc_QualSpecList_tok; _temp3662.f1=({ struct _tuple14* _temp3663=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3663->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3667= yyvs; void** _temp3669= _temp3667.curr +(
yyvsp_offset - 1); if( _temp3667.base == 0? 1:( _temp3669 < _temp3667.base? 1:
_temp3669 >= _temp3667.last_plus_one)){ _throw( Null_Exception);}* _temp3669;}));
_temp3663->f2= 0; _temp3663->f3= Cyc_yyget_AttributeList_tok(({ struct
_tagged_ptr2 _temp3664= yyvs; void** _temp3666= _temp3664.curr + yyvsp_offset;
if( _temp3664.base == 0? 1:( _temp3666 < _temp3664.base? 1: _temp3666 >=
_temp3664.last_plus_one)){ _throw( Null_Exception);}* _temp3666;})); _temp3663;});
_temp3662;}); _temp3661;}); break; case 101: _LL3660: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp3671=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3671[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3672; _temp3672.tag= Cyc_QualSpecList_tok;
_temp3672.f1=({ struct _tuple14* _temp3673=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3673->f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3683= yyvs; void** _temp3685= _temp3683.curr +(
yyvsp_offset - 2); if( _temp3683.base == 0? 1:( _temp3685 < _temp3683.base? 1:
_temp3685 >= _temp3683.last_plus_one)){ _throw( Null_Exception);}* _temp3685;})),(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3686= yyvs; void**
_temp3688= _temp3686.curr + yyvsp_offset; if( _temp3686.base == 0? 1:( _temp3688
< _temp3686.base? 1: _temp3688 >= _temp3686.last_plus_one)){ _throw(
Null_Exception);}* _temp3688;}))).f1); _temp3673->f2=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3680= yyvs; void** _temp3682= _temp3680.curr +
yyvsp_offset; if( _temp3680.base == 0? 1:( _temp3682 < _temp3680.base? 1:
_temp3682 >= _temp3680.last_plus_one)){ _throw( Null_Exception);}* _temp3682;}))).f2;
_temp3673->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp3674= yyvs; void** _temp3676= _temp3674.curr +( yyvsp_offset - 1); if(
_temp3674.base == 0? 1:( _temp3676 < _temp3674.base? 1: _temp3676 >= _temp3674.last_plus_one)){
_throw( Null_Exception);}* _temp3676;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3677= yyvs; void** _temp3679= _temp3677.curr + yyvsp_offset;
if( _temp3677.base == 0? 1:( _temp3679 < _temp3677.base? 1: _temp3679 >=
_temp3677.last_plus_one)){ _throw( Null_Exception);}* _temp3679;}))).f3);
_temp3673;}); _temp3672;}); _temp3671;}); break; case 102: _LL3670: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp3690=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3690[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp3691; _temp3691.tag= Cyc_DeclaratorExpoptList_tok;
_temp3691.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(({ struct _tagged_ptr2 _temp3692= yyvs; void**
_temp3694= _temp3692.curr + yyvsp_offset; if( _temp3692.base == 0? 1:( _temp3694
< _temp3692.base? 1: _temp3694 >= _temp3692.last_plus_one)){ _throw(
Null_Exception);}* _temp3694;}))); _temp3691;}); _temp3690;}); break; case 103:
_LL3689: yyval=( void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp3696=(
struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp3696[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp3697; _temp3697.tag=
Cyc_DeclaratorExpoptList_tok; _temp3697.f1=({ struct Cyc_List_List* _temp3698=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3698->hd=(
void*) Cyc_yyget_DeclaratorExpopt_tok(({ struct _tagged_ptr2 _temp3699= yyvs;
void** _temp3701= _temp3699.curr + yyvsp_offset; if( _temp3699.base == 0? 1:(
_temp3701 < _temp3699.base? 1: _temp3701 >= _temp3699.last_plus_one)){ _throw(
Null_Exception);}* _temp3701;})); _temp3698->tl= 0; _temp3698;}); _temp3697;});
_temp3696;}); break; case 104: _LL3695: yyval=( void*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp3703=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3703[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp3704; _temp3704.tag= Cyc_DeclaratorExpoptList_tok; _temp3704.f1=({ struct
Cyc_List_List* _temp3705=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3705->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({ struct _tagged_ptr2
_temp3709= yyvs; void** _temp3711= _temp3709.curr + yyvsp_offset; if( _temp3709.base
== 0? 1:( _temp3711 < _temp3709.base? 1: _temp3711 >= _temp3709.last_plus_one)){
_throw( Null_Exception);}* _temp3711;})); _temp3705->tl= Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3706= yyvs; void** _temp3708= _temp3706.curr +(
yyvsp_offset - 2); if( _temp3706.base == 0? 1:( _temp3708 < _temp3706.base? 1:
_temp3708 >= _temp3706.last_plus_one)){ _throw( Null_Exception);}* _temp3708;}));
_temp3705;}); _temp3704;}); _temp3703;}); break; case 105: _LL3702: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp3713=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp3713[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp3714; _temp3714.tag= Cyc_DeclaratorExpopt_tok;
_temp3714.f1=({ struct _tuple15* _temp3715=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp3715->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3716= yyvs; void** _temp3718= _temp3716.curr + yyvsp_offset;
if( _temp3716.base == 0? 1:( _temp3718 < _temp3716.base? 1: _temp3718 >=
_temp3716.last_plus_one)){ _throw( Null_Exception);}* _temp3718;})); _temp3715->f2=
0; _temp3715;}); _temp3714;}); _temp3713;}); break; case 106: _LL3712: yyval=(
void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3720=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp3720[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp3721; _temp3721.tag= Cyc_DeclaratorExpopt_tok;
_temp3721.f1=({ struct _tuple15* _temp3722=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp3722->f1=({ struct Cyc_Parse_Declarator*
_temp3727=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3727->id=({ struct _tuple1* _temp3728=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3728->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp3732=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp3732[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp3733; _temp3733.tag= Cyc_Absyn_Rel_n;
_temp3733.f1= 0; _temp3733;}); _temp3732;}); _temp3728->f2=({ struct
_tagged_string* _temp3729=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3729[ 0]=( struct _tagged_string)({ char* _temp3730=(
char*)""; struct _tagged_string _temp3731; _temp3731.curr= _temp3730; _temp3731.base=
_temp3730; _temp3731.last_plus_one= _temp3730 + 1; _temp3731;}); _temp3729;});
_temp3728;}); _temp3727->tms= 0; _temp3727;}); _temp3722->f2=({ struct Cyc_Core_Opt*
_temp3723=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3723->v=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3724= yyvs;
void** _temp3726= _temp3724.curr + yyvsp_offset; if( _temp3724.base == 0? 1:(
_temp3726 < _temp3724.base? 1: _temp3726 >= _temp3724.last_plus_one)){ _throw(
Null_Exception);}* _temp3726;})); _temp3723;}); _temp3722;}); _temp3721;});
_temp3720;}); break; case 107: _LL3719: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp3735=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp3735[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp3736; _temp3736.tag= Cyc_DeclaratorExpopt_tok; _temp3736.f1=({ struct
_tuple15* _temp3737=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15));
_temp3737->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3742= yyvs;
void** _temp3744= _temp3742.curr +( yyvsp_offset - 2); if( _temp3742.base == 0?
1:( _temp3744 < _temp3742.base? 1: _temp3744 >= _temp3742.last_plus_one)){
_throw( Null_Exception);}* _temp3744;})); _temp3737->f2=({ struct Cyc_Core_Opt*
_temp3738=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3738->v=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3739= yyvs;
void** _temp3741= _temp3739.curr + yyvsp_offset; if( _temp3739.base == 0? 1:(
_temp3741 < _temp3739.base? 1: _temp3741 >= _temp3739.last_plus_one)){ _throw(
Null_Exception);}* _temp3741;})); _temp3738;}); _temp3737;}); _temp3736;});
_temp3735;}); break; case 108: _LL3734: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3766= yyls; struct Cyc_Yyltype* _temp3768= _temp3766.curr
+( yylsp_offset - 3); if( _temp3766.base == 0? 1:( _temp3768 < _temp3766.base? 1:
_temp3768 >= _temp3766.last_plus_one)){ _throw( Null_Exception);}* _temp3768;}).first_line,({
struct _tagged_ptr3 _temp3769= yyls; struct Cyc_Yyltype* _temp3771= _temp3769.curr
+( yylsp_offset - 3); if( _temp3769.base == 0? 1:( _temp3771 < _temp3769.base? 1:
_temp3771 >= _temp3769.last_plus_one)){ _throw( Null_Exception);}* _temp3771;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3772= yyvs; void** _temp3774=
_temp3772.curr +( yyvsp_offset - 3); if( _temp3772.base == 0? 1:( _temp3774 <
_temp3772.base? 1: _temp3774 >= _temp3772.last_plus_one)){ _throw(
Null_Exception);}* _temp3774;}))); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3746=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3746[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3747; _temp3747.tag=
Cyc_TypeSpecifier_tok; _temp3747.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3748=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3748[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3749; _temp3749.tag= Cyc_Parse_Decl_spec;
_temp3749.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3750= yyvs; void** _temp3752= _temp3750.curr +(
yyvsp_offset - 4); if( _temp3750.base == 0? 1:( _temp3752 < _temp3750.base? 1:
_temp3752 >= _temp3750.last_plus_one)){ _throw( Null_Exception);}* _temp3752;})),
ts,({ struct Cyc_Core_Opt* _temp3753=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp3753->v=( void*) Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3754= yyvs; void** _temp3756= _temp3754.curr +(
yyvsp_offset - 1); if( _temp3754.base == 0? 1:( _temp3756 < _temp3754.base? 1:
_temp3756 >= _temp3754.last_plus_one)){ _throw( Null_Exception);}* _temp3756;}));
_temp3753;}), Cyc_yyget_Bool_tok(({ struct _tagged_ptr2 _temp3757= yyvs; void**
_temp3759= _temp3757.curr +( yyvsp_offset - 5); if( _temp3757.base == 0? 1:(
_temp3759 < _temp3757.base? 1: _temp3759 >= _temp3757.last_plus_one)){ _throw(
Null_Exception);}* _temp3759;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3760= yyls; struct Cyc_Yyltype* _temp3762= _temp3760.curr +(
yylsp_offset - 5); if( _temp3760.base == 0? 1:( _temp3762 < _temp3760.base? 1:
_temp3762 >= _temp3760.last_plus_one)){ _throw( Null_Exception);}* _temp3762;}).first_line,({
struct _tagged_ptr3 _temp3763= yyls; struct Cyc_Yyltype* _temp3765= _temp3763.curr
+ yylsp_offset; if( _temp3763.base == 0? 1:( _temp3765 < _temp3763.base? 1:
_temp3765 >= _temp3763.last_plus_one)){ _throw( Null_Exception);}* _temp3765;}).last_line));
_temp3749;}); _temp3748;})); _temp3747;}); _temp3746;}); break;} case 109:
_LL3745: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3776=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3776[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3777; _temp3777.tag=
Cyc_TypeSpecifier_tok; _temp3777.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp3778=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3778[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3779; _temp3779.tag= Cyc_Absyn_TunionType;
_temp3779.f1=({ struct Cyc_Absyn_TunionInfo _temp3780; _temp3780.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp3784=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp3784[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp3785; _temp3785.tag= Cyc_Absyn_UnknownTunion;
_temp3785.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp3786; _temp3786.name=
Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3790= yyvs; void** _temp3792=
_temp3790.curr +( yyvsp_offset - 1); if( _temp3790.base == 0? 1:( _temp3792 <
_temp3790.base? 1: _temp3792 >= _temp3790.last_plus_one)){ _throw(
Null_Exception);}* _temp3792;})); _temp3786.is_xtunion= Cyc_yyget_Bool_tok(({
struct _tagged_ptr2 _temp3787= yyvs; void** _temp3789= _temp3787.curr +(
yyvsp_offset - 2); if( _temp3787.base == 0? 1:( _temp3789 < _temp3787.base? 1:
_temp3789 >= _temp3787.last_plus_one)){ _throw( Null_Exception);}* _temp3789;}));
_temp3786;}); _temp3785;}); _temp3784;})); _temp3780.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3781= yyvs; void** _temp3783= _temp3781.curr +
yyvsp_offset; if( _temp3781.base == 0? 1:( _temp3783 < _temp3781.base? 1:
_temp3783 >= _temp3781.last_plus_one)){ _throw( Null_Exception);}* _temp3783;}));
_temp3780.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp3780;}); _temp3779;});
_temp3778;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3793= yyls;
struct Cyc_Yyltype* _temp3795= _temp3793.curr +( yylsp_offset - 2); if(
_temp3793.base == 0? 1:( _temp3795 < _temp3793.base? 1: _temp3795 >= _temp3793.last_plus_one)){
_throw( Null_Exception);}* _temp3795;}).first_line,({ struct _tagged_ptr3
_temp3796= yyls; struct Cyc_Yyltype* _temp3798= _temp3796.curr + yylsp_offset;
if( _temp3796.base == 0? 1:( _temp3798 < _temp3796.base? 1: _temp3798 >=
_temp3796.last_plus_one)){ _throw( Null_Exception);}* _temp3798;}).last_line));
_temp3777;}); _temp3776;}); break; case 110: _LL3775: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp3800=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3800[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3801; _temp3801.tag= Cyc_TypeSpecifier_tok;
_temp3801.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp3802=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp3802[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp3803; _temp3803.tag=
Cyc_Absyn_TunionType; _temp3803.f1=({ struct Cyc_Absyn_TunionInfo _temp3804;
_temp3804.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp3811=( struct Cyc_Absyn_UnknownTunion_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp3811[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp3812; _temp3812.tag= Cyc_Absyn_UnknownTunion; _temp3812.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp3813; _temp3813.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3817=
yyvs; void** _temp3819= _temp3817.curr +( yyvsp_offset - 1); if( _temp3817.base
== 0? 1:( _temp3819 < _temp3817.base? 1: _temp3819 >= _temp3817.last_plus_one)){
_throw( Null_Exception);}* _temp3819;})); _temp3813.is_xtunion= Cyc_yyget_Bool_tok(({
struct _tagged_ptr2 _temp3814= yyvs; void** _temp3816= _temp3814.curr +(
yyvsp_offset - 3); if( _temp3814.base == 0? 1:( _temp3816 < _temp3814.base? 1:
_temp3816 >= _temp3814.last_plus_one)){ _throw( Null_Exception);}* _temp3816;}));
_temp3813;}); _temp3812;}); _temp3811;})); _temp3804.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3808= yyvs; void** _temp3810= _temp3808.curr +
yyvsp_offset; if( _temp3808.base == 0? 1:( _temp3810 < _temp3808.base? 1:
_temp3810 >= _temp3808.last_plus_one)){ _throw( Null_Exception);}* _temp3810;}));
_temp3804.rgn=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp3805= yyvs;
void** _temp3807= _temp3805.curr +( yyvsp_offset - 2); if( _temp3805.base == 0?
1:( _temp3807 < _temp3805.base? 1: _temp3807 >= _temp3805.last_plus_one)){
_throw( Null_Exception);}* _temp3807;})); _temp3804;}); _temp3803;}); _temp3802;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3820= yyls; struct Cyc_Yyltype*
_temp3822= _temp3820.curr +( yylsp_offset - 3); if( _temp3820.base == 0? 1:(
_temp3822 < _temp3820.base? 1: _temp3822 >= _temp3820.last_plus_one)){ _throw(
Null_Exception);}* _temp3822;}).first_line,({ struct _tagged_ptr3 _temp3823=
yyls; struct Cyc_Yyltype* _temp3825= _temp3823.curr + yylsp_offset; if(
_temp3823.base == 0? 1:( _temp3825 < _temp3823.base? 1: _temp3825 >= _temp3823.last_plus_one)){
_throw( Null_Exception);}* _temp3825;}).last_line)); _temp3801;}); _temp3800;});
break; case 111: _LL3799: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3827=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3827[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3828; _temp3828.tag=
Cyc_TypeSpecifier_tok; _temp3828.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp3829=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp3829[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp3830; _temp3830.tag= Cyc_Absyn_TunionFieldType;
_temp3830.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp3831; _temp3831.field_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct* _temp3835=( struct
Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunionfield_struct));
_temp3835[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct _temp3836; _temp3836.tag=
Cyc_Absyn_UnknownTunionfield; _temp3836.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp3837; _temp3837.tunion_name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3844= yyvs; void** _temp3846= _temp3844.curr +( yyvsp_offset - 3); if(
_temp3844.base == 0? 1:( _temp3846 < _temp3844.base? 1: _temp3846 >= _temp3844.last_plus_one)){
_throw( Null_Exception);}* _temp3846;})); _temp3837.field_name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3841= yyvs; void** _temp3843= _temp3841.curr +(
yyvsp_offset - 1); if( _temp3841.base == 0? 1:( _temp3843 < _temp3841.base? 1:
_temp3843 >= _temp3841.last_plus_one)){ _throw( Null_Exception);}* _temp3843;}));
_temp3837.is_xtunion= Cyc_yyget_Bool_tok(({ struct _tagged_ptr2 _temp3838= yyvs;
void** _temp3840= _temp3838.curr +( yyvsp_offset - 4); if( _temp3838.base == 0?
1:( _temp3840 < _temp3838.base? 1: _temp3840 >= _temp3838.last_plus_one)){
_throw( Null_Exception);}* _temp3840;})); _temp3837;}); _temp3836;}); _temp3835;}));
_temp3831.targs= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3832= yyvs;
void** _temp3834= _temp3832.curr + yyvsp_offset; if( _temp3832.base == 0? 1:(
_temp3834 < _temp3832.base? 1: _temp3834 >= _temp3832.last_plus_one)){ _throw(
Null_Exception);}* _temp3834;})); _temp3831;}); _temp3830;}); _temp3829;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3847= yyls; struct Cyc_Yyltype* _temp3849= _temp3847.curr
+( yylsp_offset - 4); if( _temp3847.base == 0? 1:( _temp3849 < _temp3847.base? 1:
_temp3849 >= _temp3847.last_plus_one)){ _throw( Null_Exception);}* _temp3849;}).first_line,({
struct _tagged_ptr3 _temp3850= yyls; struct Cyc_Yyltype* _temp3852= _temp3850.curr
+ yylsp_offset; if( _temp3850.base == 0? 1:( _temp3852 < _temp3850.base? 1:
_temp3852 >= _temp3850.last_plus_one)){ _throw( Null_Exception);}* _temp3852;}).last_line));
_temp3828;}); _temp3827;}); break; case 112: _LL3826: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp3854=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp3854[ 0]=({ struct Cyc_Bool_tok_struct
_temp3855; _temp3855.tag= Cyc_Bool_tok; _temp3855.f1= 0; _temp3855;}); _temp3854;});
break; case 113: _LL3853: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp3857=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp3857[ 0]=({ struct Cyc_Bool_tok_struct _temp3858; _temp3858.tag= Cyc_Bool_tok;
_temp3858.f1= 1; _temp3858;}); _temp3857;}); break; case 114: _LL3856: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp3860=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp3860[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp3861; _temp3861.tag= Cyc_TunionFieldList_tok;
_temp3861.f1=({ struct Cyc_List_List* _temp3862=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3862->hd=( void*) Cyc_yyget_TunionField_tok(({
struct _tagged_ptr2 _temp3863= yyvs; void** _temp3865= _temp3863.curr +
yyvsp_offset; if( _temp3863.base == 0? 1:( _temp3865 < _temp3863.base? 1:
_temp3865 >= _temp3863.last_plus_one)){ _throw( Null_Exception);}* _temp3865;}));
_temp3862->tl= 0; _temp3862;}); _temp3861;}); _temp3860;}); break; case 115:
_LL3859: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp3867=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3867[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3868; _temp3868.tag=
Cyc_TunionFieldList_tok; _temp3868.f1=({ struct Cyc_List_List* _temp3869=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3869->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3870= yyvs; void**
_temp3872= _temp3870.curr +( yyvsp_offset - 1); if( _temp3870.base == 0? 1:(
_temp3872 < _temp3870.base? 1: _temp3872 >= _temp3870.last_plus_one)){ _throw(
Null_Exception);}* _temp3872;})); _temp3869->tl= 0; _temp3869;}); _temp3868;});
_temp3867;}); break; case 116: _LL3866: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3874=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3874[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3875; _temp3875.tag= Cyc_TunionFieldList_tok; _temp3875.f1=({ struct Cyc_List_List*
_temp3876=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3876->hd=( void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2
_temp3880= yyvs; void** _temp3882= _temp3880.curr +( yyvsp_offset - 2); if(
_temp3880.base == 0? 1:( _temp3882 < _temp3880.base? 1: _temp3882 >= _temp3880.last_plus_one)){
_throw( Null_Exception);}* _temp3882;})); _temp3876->tl= Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3877= yyvs; void** _temp3879= _temp3877.curr +
yyvsp_offset; if( _temp3877.base == 0? 1:( _temp3879 < _temp3877.base? 1:
_temp3879 >= _temp3877.last_plus_one)){ _throw( Null_Exception);}* _temp3879;}));
_temp3876;}); _temp3875;}); _temp3874;}); break; case 117: _LL3873: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp3884=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp3884[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp3885; _temp3885.tag= Cyc_TunionFieldList_tok;
_temp3885.f1=({ struct Cyc_List_List* _temp3886=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3886->hd=( void*) Cyc_yyget_TunionField_tok(({
struct _tagged_ptr2 _temp3890= yyvs; void** _temp3892= _temp3890.curr +(
yyvsp_offset - 2); if( _temp3890.base == 0? 1:( _temp3892 < _temp3890.base? 1:
_temp3892 >= _temp3890.last_plus_one)){ _throw( Null_Exception);}* _temp3892;}));
_temp3886->tl= Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2 _temp3887=
yyvs; void** _temp3889= _temp3887.curr + yyvsp_offset; if( _temp3887.base == 0?
1:( _temp3889 < _temp3887.base? 1: _temp3889 >= _temp3887.last_plus_one)){
_throw( Null_Exception);}* _temp3889;})); _temp3886;}); _temp3885;}); _temp3884;});
break; case 118: _LL3883: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp3894=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp3894[ 0]=({ struct Cyc_Scope_tok_struct _temp3895; _temp3895.tag= Cyc_Scope_tok;
_temp3895.f1=( void*)(( void*) Cyc_Absyn_Public); _temp3895;}); _temp3894;});
break; case 119: _LL3893: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp3897=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp3897[ 0]=({ struct Cyc_Scope_tok_struct _temp3898; _temp3898.tag= Cyc_Scope_tok;
_temp3898.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp3898;}); _temp3897;});
break; case 120: _LL3896: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp3900=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp3900[ 0]=({ struct Cyc_Scope_tok_struct _temp3901; _temp3901.tag= Cyc_Scope_tok;
_temp3901.f1=( void*)(( void*) Cyc_Absyn_Static); _temp3901;}); _temp3900;});
break; case 121: _LL3899: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp3903=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3903[ 0]=({ struct Cyc_TunionField_tok_struct _temp3904; _temp3904.tag= Cyc_TunionField_tok;
_temp3904.f1=({ struct Cyc_Absyn_Tunionfield* _temp3905=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3905->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3915= yyvs; void** _temp3917= _temp3915.curr +
yyvsp_offset; if( _temp3915.base == 0? 1:( _temp3917 < _temp3915.base? 1:
_temp3917 >= _temp3915.last_plus_one)){ _throw( Null_Exception);}* _temp3917;}));
_temp3905->tvs= 0; _temp3905->typs= 0; _temp3905->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3909= yyls; struct Cyc_Yyltype* _temp3911= _temp3909.curr
+( yylsp_offset - 1); if( _temp3909.base == 0? 1:( _temp3911 < _temp3909.base? 1:
_temp3911 >= _temp3909.last_plus_one)){ _throw( Null_Exception);}* _temp3911;}).first_line,({
struct _tagged_ptr3 _temp3912= yyls; struct Cyc_Yyltype* _temp3914= _temp3912.curr
+ yylsp_offset; if( _temp3912.base == 0? 1:( _temp3914 < _temp3912.base? 1:
_temp3914 >= _temp3912.last_plus_one)){ _throw( Null_Exception);}* _temp3914;}).last_line);
_temp3905->sc=( void*) Cyc_yyget_Scope_tok(({ struct _tagged_ptr2 _temp3906=
yyvs; void** _temp3908= _temp3906.curr +( yyvsp_offset - 1); if( _temp3906.base
== 0? 1:( _temp3908 < _temp3906.base? 1: _temp3908 >= _temp3906.last_plus_one)){
_throw( Null_Exception);}* _temp3908;})); _temp3905;}); _temp3904;}); _temp3903;});
break; case 122: _LL3902: { struct Cyc_List_List* typs=(( struct Cyc_List_List*(*)(
struct _tuple10*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct
Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3943= yyls; struct Cyc_Yyltype*
_temp3945= _temp3943.curr +( yylsp_offset - 1); if( _temp3943.base == 0? 1:(
_temp3945 < _temp3943.base? 1: _temp3945 >= _temp3943.last_plus_one)){ _throw(
Null_Exception);}* _temp3945;}).first_line,({ struct _tagged_ptr3 _temp3946=
yyls; struct Cyc_Yyltype* _temp3948= _temp3946.curr +( yylsp_offset - 1); if(
_temp3946.base == 0? 1:( _temp3948 < _temp3946.base? 1: _temp3948 >= _temp3946.last_plus_one)){
_throw( Null_Exception);}* _temp3948;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3949= yyvs; void** _temp3951= _temp3949.curr +(
yyvsp_offset - 1); if( _temp3949.base == 0? 1:( _temp3951 < _temp3949.base? 1:
_temp3951 >= _temp3949.last_plus_one)){ _throw( Null_Exception);}* _temp3951;}))));
struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3934= yyls; struct Cyc_Yyltype* _temp3936= _temp3934.curr
+( yylsp_offset - 3); if( _temp3934.base == 0? 1:( _temp3936 < _temp3934.base? 1:
_temp3936 >= _temp3934.last_plus_one)){ _throw( Null_Exception);}* _temp3936;}).first_line,({
struct _tagged_ptr3 _temp3937= yyls; struct Cyc_Yyltype* _temp3939= _temp3937.curr
+( yylsp_offset - 3); if( _temp3937.base == 0? 1:( _temp3939 < _temp3937.base? 1:
_temp3939 >= _temp3937.last_plus_one)){ _throw( Null_Exception);}* _temp3939;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3940= yyvs; void** _temp3942=
_temp3940.curr +( yyvsp_offset - 3); if( _temp3940.base == 0? 1:( _temp3942 <
_temp3940.base? 1: _temp3942 >= _temp3940.last_plus_one)){ _throw(
Null_Exception);}* _temp3942;}))); yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp3919=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3919[ 0]=({ struct Cyc_TunionField_tok_struct _temp3920; _temp3920.tag= Cyc_TunionField_tok;
_temp3920.f1=({ struct Cyc_Absyn_Tunionfield* _temp3921=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3921->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3931= yyvs; void** _temp3933= _temp3931.curr +(
yyvsp_offset - 4); if( _temp3931.base == 0? 1:( _temp3933 < _temp3931.base? 1:
_temp3933 >= _temp3931.last_plus_one)){ _throw( Null_Exception);}* _temp3933;}));
_temp3921->tvs= tvs; _temp3921->typs= typs; _temp3921->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3925= yyls; struct Cyc_Yyltype* _temp3927= _temp3925.curr
+( yylsp_offset - 5); if( _temp3925.base == 0? 1:( _temp3927 < _temp3925.base? 1:
_temp3927 >= _temp3925.last_plus_one)){ _throw( Null_Exception);}* _temp3927;}).first_line,({
struct _tagged_ptr3 _temp3928= yyls; struct Cyc_Yyltype* _temp3930= _temp3928.curr
+ yylsp_offset; if( _temp3928.base == 0? 1:( _temp3930 < _temp3928.base? 1:
_temp3930 >= _temp3928.last_plus_one)){ _throw( Null_Exception);}* _temp3930;}).last_line);
_temp3921->sc=( void*) Cyc_yyget_Scope_tok(({ struct _tagged_ptr2 _temp3922=
yyvs; void** _temp3924= _temp3922.curr +( yyvsp_offset - 5); if( _temp3922.base
== 0? 1:( _temp3924 < _temp3922.base? 1: _temp3924 >= _temp3922.last_plus_one)){
_throw( Null_Exception);}* _temp3924;})); _temp3921;}); _temp3920;}); _temp3919;});
break;} case 123: _LL3918: yyval=({ struct _tagged_ptr2 _temp3953= yyvs; void**
_temp3955= _temp3953.curr + yyvsp_offset; if( _temp3953.base == 0? 1:( _temp3955
< _temp3953.base? 1: _temp3955 >= _temp3953.last_plus_one)){ _throw(
Null_Exception);}* _temp3955;}); break; case 124: _LL3952: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp3957=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3957[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3958; _temp3958.tag= Cyc_Declarator_tok;
_temp3958.f1=({ struct Cyc_Parse_Declarator* _temp3959=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3959->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3966= yyvs; void** _temp3968= _temp3966.curr +
yyvsp_offset; if( _temp3966.base == 0? 1:( _temp3968 < _temp3966.base? 1:
_temp3968 >= _temp3966.last_plus_one)){ _throw( Null_Exception);}* _temp3968;})))->id;
_temp3959->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp3960= yyvs; void** _temp3962= _temp3960.curr +( yyvsp_offset - 1); if(
_temp3960.base == 0? 1:( _temp3962 < _temp3960.base? 1: _temp3962 >= _temp3960.last_plus_one)){
_throw( Null_Exception);}* _temp3962;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3963= yyvs; void** _temp3965= _temp3963.curr + yyvsp_offset;
if( _temp3963.base == 0? 1:( _temp3965 < _temp3963.base? 1: _temp3965 >=
_temp3963.last_plus_one)){ _throw( Null_Exception);}* _temp3965;})))->tms);
_temp3959;}); _temp3958;}); _temp3957;}); break; case 125: _LL3956: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp3970=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3970[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3971; _temp3971.tag= Cyc_Declarator_tok;
_temp3971.f1=({ struct Cyc_Parse_Declarator* _temp3972=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3972->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3973= yyvs; void** _temp3975= _temp3973.curr +
yyvsp_offset; if( _temp3973.base == 0? 1:( _temp3975 < _temp3973.base? 1:
_temp3975 >= _temp3973.last_plus_one)){ _throw( Null_Exception);}* _temp3975;}));
_temp3972->tms= 0; _temp3972;}); _temp3971;}); _temp3970;}); break; case 126:
_LL3969: yyval=({ struct _tagged_ptr2 _temp3977= yyvs; void** _temp3979=
_temp3977.curr +( yyvsp_offset - 1); if( _temp3977.base == 0? 1:( _temp3979 <
_temp3977.base? 1: _temp3979 >= _temp3977.last_plus_one)){ _throw(
Null_Exception);}* _temp3979;}); break; case 127: _LL3976: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp3981=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3981[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3982; _temp3982.tag= Cyc_Declarator_tok;
_temp3982.f1=({ struct Cyc_Parse_Declarator* _temp3983=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3983->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3988= yyvs; void** _temp3990= _temp3988.curr +(
yyvsp_offset - 2); if( _temp3988.base == 0? 1:( _temp3990 < _temp3988.base? 1:
_temp3990 >= _temp3988.last_plus_one)){ _throw( Null_Exception);}* _temp3990;})))->id;
_temp3983->tms=({ struct Cyc_List_List* _temp3984=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3984->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3984->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3985= yyvs;
void** _temp3987= _temp3985.curr +( yyvsp_offset - 2); if( _temp3985.base == 0?
1:( _temp3987 < _temp3985.base? 1: _temp3987 >= _temp3985.last_plus_one)){
_throw( Null_Exception);}* _temp3987;})))->tms; _temp3984;}); _temp3983;});
_temp3982;}); _temp3981;}); break; case 128: _LL3980: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp3992=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3992[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3993; _temp3993.tag= Cyc_Declarator_tok;
_temp3993.f1=({ struct Cyc_Parse_Declarator* _temp3994=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3994->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4004= yyvs; void** _temp4006= _temp4004.curr +(
yyvsp_offset - 3); if( _temp4004.base == 0? 1:( _temp4006 < _temp4004.base? 1:
_temp4006 >= _temp4004.last_plus_one)){ _throw( Null_Exception);}* _temp4006;})))->id;
_temp3994->tms=({ struct Cyc_List_List* _temp3995=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3995->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3999=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3999[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp4000; _temp4000.tag= Cyc_Absyn_ConstArray_mod;
_temp4000.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4001= yyvs; void**
_temp4003= _temp4001.curr +( yyvsp_offset - 1); if( _temp4001.base == 0? 1:(
_temp4003 < _temp4001.base? 1: _temp4003 >= _temp4001.last_plus_one)){ _throw(
Null_Exception);}* _temp4003;})); _temp4000;}); _temp3999;})); _temp3995->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3996= yyvs; void**
_temp3998= _temp3996.curr +( yyvsp_offset - 3); if( _temp3996.base == 0? 1:(
_temp3998 < _temp3996.base? 1: _temp3998 >= _temp3996.last_plus_one)){ _throw(
Null_Exception);}* _temp3998;})))->tms; _temp3995;}); _temp3994;}); _temp3993;});
_temp3992;}); break; case 129: _LL3991: { struct _tuple16 _temp4013; struct Cyc_Core_Opt*
_temp4014; int _temp4016; struct Cyc_List_List* _temp4018; struct _tuple16*
_temp4011= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp4008=
yyvs; void** _temp4010= _temp4008.curr +( yyvsp_offset - 1); if( _temp4008.base
== 0? 1:( _temp4010 < _temp4008.base? 1: _temp4010 >= _temp4008.last_plus_one)){
_throw( Null_Exception);}* _temp4010;})); _temp4013=* _temp4011; _LL4019:
_temp4018= _temp4013.f1; goto _LL4017; _LL4017: _temp4016= _temp4013.f2; goto
_LL4015; _LL4015: _temp4014= _temp4013.f3; goto _LL4012; _LL4012: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp4020=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4020[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4021; _temp4021.tag= Cyc_Declarator_tok;
_temp4021.f1=({ struct Cyc_Parse_Declarator* _temp4022=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4022->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4031= yyvs; void** _temp4033= _temp4031.curr +(
yyvsp_offset - 3); if( _temp4031.base == 0? 1:( _temp4033 < _temp4031.base? 1:
_temp4033 >= _temp4031.last_plus_one)){ _throw( Null_Exception);}* _temp4033;})))->id;
_temp4022->tms=({ struct Cyc_List_List* _temp4023=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4023->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4027=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4027[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4028; _temp4028.tag= Cyc_Absyn_Function_mod;
_temp4028.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4029=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4029[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4030; _temp4030.tag= Cyc_Absyn_WithTypes;
_temp4030.f1= _temp4018; _temp4030.f2= _temp4016; _temp4030.f3= _temp4014;
_temp4030;}); _temp4029;})); _temp4028;}); _temp4027;})); _temp4023->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4024= yyvs; void** _temp4026= _temp4024.curr +(
yyvsp_offset - 3); if( _temp4024.base == 0? 1:( _temp4026 < _temp4024.base? 1:
_temp4026 >= _temp4024.last_plus_one)){ _throw( Null_Exception);}* _temp4026;})))->tms;
_temp4023;}); _temp4022;}); _temp4021;}); _temp4020;}); break;} case 130:
_LL4007: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4035=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4035[ 0]=({ struct Cyc_Declarator_tok_struct _temp4036; _temp4036.tag= Cyc_Declarator_tok;
_temp4036.f1=({ struct Cyc_Parse_Declarator* _temp4037=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4037->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4046= yyvs; void** _temp4048= _temp4046.curr +(
yyvsp_offset - 2); if( _temp4046.base == 0? 1:( _temp4048 < _temp4046.base? 1:
_temp4048 >= _temp4046.last_plus_one)){ _throw( Null_Exception);}* _temp4048;})))->id;
_temp4037->tms=({ struct Cyc_List_List* _temp4038=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4038->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4042=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4042[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4043; _temp4043.tag= Cyc_Absyn_Function_mod;
_temp4043.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4044=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4044[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4045; _temp4045.tag= Cyc_Absyn_WithTypes;
_temp4045.f1= 0; _temp4045.f2= 0; _temp4045.f3= 0; _temp4045;}); _temp4044;}));
_temp4043;}); _temp4042;})); _temp4038->tl=( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp4039= yyvs; void** _temp4041= _temp4039.curr +( yyvsp_offset -
2); if( _temp4039.base == 0? 1:( _temp4041 < _temp4039.base? 1: _temp4041 >=
_temp4039.last_plus_one)){ _throw( Null_Exception);}* _temp4041;})))->tms;
_temp4038;}); _temp4037;}); _temp4036;}); _temp4035;}); break; case 131: _LL4034:
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4050=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4050[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4051; _temp4051.tag= Cyc_Declarator_tok;
_temp4051.f1=({ struct Cyc_Parse_Declarator* _temp4052=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4052->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4067= yyvs; void** _temp4069= _temp4067.curr +(
yyvsp_offset - 4); if( _temp4067.base == 0? 1:( _temp4069 < _temp4067.base? 1:
_temp4069 >= _temp4067.last_plus_one)){ _throw( Null_Exception);}* _temp4069;})))->id;
_temp4052->tms=({ struct Cyc_List_List* _temp4053=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4053->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4057=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4057[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4058; _temp4058.tag= Cyc_Absyn_Function_mod;
_temp4058.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4059=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4059[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4060; _temp4060.tag= Cyc_Absyn_WithTypes;
_temp4060.f1= 0; _temp4060.f2= 0; _temp4060.f3=({ struct Cyc_Core_Opt* _temp4061=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4061->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4062=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4062[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4063; _temp4063.tag= Cyc_Absyn_JoinEff; _temp4063.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4064= yyvs; void** _temp4066=
_temp4064.curr +( yyvsp_offset - 1); if( _temp4064.base == 0? 1:( _temp4066 <
_temp4064.base? 1: _temp4066 >= _temp4064.last_plus_one)){ _throw(
Null_Exception);}* _temp4066;})); _temp4063;}); _temp4062;})); _temp4061;});
_temp4060;}); _temp4059;})); _temp4058;}); _temp4057;})); _temp4053->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4054= yyvs; void** _temp4056= _temp4054.curr +(
yyvsp_offset - 4); if( _temp4054.base == 0? 1:( _temp4056 < _temp4054.base? 1:
_temp4056 >= _temp4054.last_plus_one)){ _throw( Null_Exception);}* _temp4056;})))->tms;
_temp4053;}); _temp4052;}); _temp4051;}); _temp4050;}); break; case 132: _LL4049:
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4071=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4071[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4072; _temp4072.tag= Cyc_Declarator_tok;
_temp4072.f1=({ struct Cyc_Parse_Declarator* _temp4073=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4073->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4091= yyvs; void** _temp4093= _temp4091.curr +(
yyvsp_offset - 3); if( _temp4091.base == 0? 1:( _temp4093 < _temp4091.base? 1:
_temp4093 >= _temp4091.last_plus_one)){ _throw( Null_Exception);}* _temp4093;})))->id;
_temp4073->tms=({ struct Cyc_List_List* _temp4074=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4074->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4078=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4078[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4079; _temp4079.tag= Cyc_Absyn_Function_mod;
_temp4079.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp4080=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp4080[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp4081; _temp4081.tag= Cyc_Absyn_NoTypes;
_temp4081.f1= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4082= yyvs; void**
_temp4084= _temp4082.curr +( yyvsp_offset - 1); if( _temp4082.base == 0? 1:(
_temp4084 < _temp4082.base? 1: _temp4084 >= _temp4082.last_plus_one)){ _throw(
Null_Exception);}* _temp4084;})); _temp4081.f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4085= yyls; struct Cyc_Yyltype* _temp4087= _temp4085.curr
+( yylsp_offset - 3); if( _temp4085.base == 0? 1:( _temp4087 < _temp4085.base? 1:
_temp4087 >= _temp4085.last_plus_one)){ _throw( Null_Exception);}* _temp4087;}).first_line,({
struct _tagged_ptr3 _temp4088= yyls; struct Cyc_Yyltype* _temp4090= _temp4088.curr
+ yylsp_offset; if( _temp4088.base == 0? 1:( _temp4090 < _temp4088.base? 1:
_temp4090 >= _temp4088.last_plus_one)){ _throw( Null_Exception);}* _temp4090;}).last_line);
_temp4081;}); _temp4080;})); _temp4079;}); _temp4078;})); _temp4074->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4075= yyvs; void** _temp4077= _temp4075.curr +(
yyvsp_offset - 3); if( _temp4075.base == 0? 1:( _temp4077 < _temp4075.base? 1:
_temp4077 >= _temp4075.last_plus_one)){ _throw( Null_Exception);}* _temp4077;})))->tms;
_temp4074;}); _temp4073;}); _temp4072;}); _temp4071;}); break; case 133: _LL4070: {
struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4113= yyls; struct Cyc_Yyltype* _temp4115= _temp4113.curr
+( yylsp_offset - 2); if( _temp4113.base == 0? 1:( _temp4115 < _temp4113.base? 1:
_temp4115 >= _temp4113.last_plus_one)){ _throw( Null_Exception);}* _temp4115;}).first_line,({
struct _tagged_ptr3 _temp4116= yyls; struct Cyc_Yyltype* _temp4118= _temp4116.curr
+ yylsp_offset; if( _temp4116.base == 0? 1:( _temp4118 < _temp4116.base? 1:
_temp4118 >= _temp4116.last_plus_one)){ _throw( Null_Exception);}* _temp4118;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4119= yyvs; void** _temp4121= _temp4119.curr +(
yyvsp_offset - 1); if( _temp4119.base == 0? 1:( _temp4121 < _temp4119.base? 1:
_temp4121 >= _temp4119.last_plus_one)){ _throw( Null_Exception);}* _temp4121;}))));
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4095=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4095[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4096; _temp4096.tag= Cyc_Declarator_tok;
_temp4096.f1=({ struct Cyc_Parse_Declarator* _temp4097=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4097->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4110= yyvs; void** _temp4112= _temp4110.curr +(
yyvsp_offset - 3); if( _temp4110.base == 0? 1:( _temp4112 < _temp4110.base? 1:
_temp4112 >= _temp4110.last_plus_one)){ _throw( Null_Exception);}* _temp4112;})))->id;
_temp4097->tms=({ struct Cyc_List_List* _temp4098=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4098->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4102=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4102[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4103; _temp4103.tag= Cyc_Absyn_TypeParams_mod;
_temp4103.f1= ts; _temp4103.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4104= yyls; struct Cyc_Yyltype* _temp4106= _temp4104.curr +(
yylsp_offset - 3); if( _temp4104.base == 0? 1:( _temp4106 < _temp4104.base? 1:
_temp4106 >= _temp4104.last_plus_one)){ _throw( Null_Exception);}* _temp4106;}).first_line,({
struct _tagged_ptr3 _temp4107= yyls; struct Cyc_Yyltype* _temp4109= _temp4107.curr
+ yylsp_offset; if( _temp4107.base == 0? 1:( _temp4109 < _temp4107.base? 1:
_temp4109 >= _temp4107.last_plus_one)){ _throw( Null_Exception);}* _temp4109;}).last_line);
_temp4103.f3= 0; _temp4103;}); _temp4102;})); _temp4098->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4099= yyvs; void** _temp4101= _temp4099.curr +(
yyvsp_offset - 3); if( _temp4099.base == 0? 1:( _temp4101 < _temp4099.base? 1:
_temp4101 >= _temp4099.last_plus_one)){ _throw( Null_Exception);}* _temp4101;})))->tms;
_temp4098;}); _temp4097;}); _temp4096;}); _temp4095;}); break;} case 134:
_LL4094:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp4123= Cyc_Parse_lbuf;
if( _temp4123 == 0){ _throw( Null_Exception);} _temp4123->v;}))->lex_curr_pos -=
1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4142= yyls; struct Cyc_Yyltype* _temp4144= _temp4142.curr
+( yylsp_offset - 2); if( _temp4142.base == 0? 1:( _temp4144 < _temp4142.base? 1:
_temp4144 >= _temp4142.last_plus_one)){ _throw( Null_Exception);}* _temp4144;}).first_line,({
struct _tagged_ptr3 _temp4145= yyls; struct Cyc_Yyltype* _temp4147= _temp4145.curr
+ yylsp_offset; if( _temp4145.base == 0? 1:( _temp4147 < _temp4145.base? 1:
_temp4147 >= _temp4145.last_plus_one)){ _throw( Null_Exception);}* _temp4147;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4148= yyvs; void** _temp4150= _temp4148.curr +(
yyvsp_offset - 1); if( _temp4148.base == 0? 1:( _temp4150 < _temp4148.base? 1:
_temp4150 >= _temp4148.last_plus_one)){ _throw( Null_Exception);}* _temp4150;}))));
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4124=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4124[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4125; _temp4125.tag= Cyc_Declarator_tok;
_temp4125.f1=({ struct Cyc_Parse_Declarator* _temp4126=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4126->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4139= yyvs; void** _temp4141= _temp4139.curr +(
yyvsp_offset - 3); if( _temp4139.base == 0? 1:( _temp4141 < _temp4139.base? 1:
_temp4141 >= _temp4139.last_plus_one)){ _throw( Null_Exception);}* _temp4141;})))->id;
_temp4126->tms=({ struct Cyc_List_List* _temp4127=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4127->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4131=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4131[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4132; _temp4132.tag= Cyc_Absyn_TypeParams_mod;
_temp4132.f1= ts; _temp4132.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4133= yyls; struct Cyc_Yyltype* _temp4135= _temp4133.curr +(
yylsp_offset - 3); if( _temp4133.base == 0? 1:( _temp4135 < _temp4133.base? 1:
_temp4135 >= _temp4133.last_plus_one)){ _throw( Null_Exception);}* _temp4135;}).first_line,({
struct _tagged_ptr3 _temp4136= yyls; struct Cyc_Yyltype* _temp4138= _temp4136.curr
+ yylsp_offset; if( _temp4136.base == 0? 1:( _temp4138 < _temp4136.base? 1:
_temp4138 >= _temp4136.last_plus_one)){ _throw( Null_Exception);}* _temp4138;}).last_line);
_temp4132.f3= 0; _temp4132;}); _temp4131;})); _temp4127->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4128= yyvs; void** _temp4130= _temp4128.curr +(
yyvsp_offset - 3); if( _temp4128.base == 0? 1:( _temp4130 < _temp4128.base? 1:
_temp4130 >= _temp4128.last_plus_one)){ _throw( Null_Exception);}* _temp4130;})))->tms;
_temp4127;}); _temp4126;}); _temp4125;}); _temp4124;}); break;} case 135:
_LL4122: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4152=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4152[ 0]=({ struct Cyc_Declarator_tok_struct _temp4153; _temp4153.tag= Cyc_Declarator_tok;
_temp4153.f1=({ struct Cyc_Parse_Declarator* _temp4154=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4154->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4170= yyvs; void** _temp4172= _temp4170.curr +(
yyvsp_offset - 1); if( _temp4170.base == 0? 1:( _temp4172 < _temp4170.base? 1:
_temp4172 >= _temp4170.last_plus_one)){ _throw( Null_Exception);}* _temp4172;})))->id;
_temp4154->tms=({ struct Cyc_List_List* _temp4155=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4155->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp4159=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp4159[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp4160; _temp4160.tag= Cyc_Absyn_Attributes_mod;
_temp4160.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4161= yyls;
struct Cyc_Yyltype* _temp4163= _temp4161.curr + yylsp_offset; if( _temp4161.base
== 0? 1:( _temp4163 < _temp4161.base? 1: _temp4163 >= _temp4161.last_plus_one)){
_throw( Null_Exception);}* _temp4163;}).first_line,({ struct _tagged_ptr3
_temp4164= yyls; struct Cyc_Yyltype* _temp4166= _temp4164.curr + yylsp_offset;
if( _temp4164.base == 0? 1:( _temp4166 < _temp4164.base? 1: _temp4166 >=
_temp4164.last_plus_one)){ _throw( Null_Exception);}* _temp4166;}).last_line);
_temp4160.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4167= yyvs;
void** _temp4169= _temp4167.curr + yyvsp_offset; if( _temp4167.base == 0? 1:(
_temp4169 < _temp4167.base? 1: _temp4169 >= _temp4167.last_plus_one)){ _throw(
Null_Exception);}* _temp4169;})); _temp4160;}); _temp4159;})); _temp4155->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4156= yyvs; void**
_temp4158= _temp4156.curr +( yyvsp_offset - 1); if( _temp4156.base == 0? 1:(
_temp4158 < _temp4156.base? 1: _temp4158 >= _temp4156.last_plus_one)){ _throw(
Null_Exception);}* _temp4158;})))->tms; _temp4155;}); _temp4154;}); _temp4153;});
_temp4152;}); break; case 136: _LL4151: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4174=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4174[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4175; _temp4175.tag= Cyc_TypeModifierList_tok; _temp4175.f1=({ struct Cyc_List_List*
_temp4176=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4176->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4186=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4186[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4187; _temp4187.tag=
Cyc_Absyn_Pointer_mod; _temp4187.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4188= yyvs; void** _temp4190= _temp4188.curr +(
yyvsp_offset - 2); if( _temp4188.base == 0? 1:( _temp4190 < _temp4188.base? 1:
_temp4190 >= _temp4188.last_plus_one)){ _throw( Null_Exception);}* _temp4190;}));
_temp4187.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4191= yyvs;
void** _temp4193= _temp4191.curr +( yyvsp_offset - 1); if( _temp4191.base == 0?
1:( _temp4193 < _temp4191.base? 1: _temp4193 >= _temp4191.last_plus_one)){
_throw( Null_Exception);}* _temp4193;})); _temp4187.f3= Cyc_Absyn_empty_tqual();
_temp4187;}); _temp4186;})); _temp4176->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4177= yyls; struct Cyc_Yyltype* _temp4179= _temp4177.curr
+ yylsp_offset; if( _temp4177.base == 0? 1:( _temp4179 < _temp4177.base? 1:
_temp4179 >= _temp4177.last_plus_one)){ _throw( Null_Exception);}* _temp4179;}).first_line,({
struct _tagged_ptr3 _temp4180= yyls; struct Cyc_Yyltype* _temp4182= _temp4180.curr
+ yylsp_offset; if( _temp4180.base == 0? 1:( _temp4182 < _temp4180.base? 1:
_temp4182 >= _temp4180.last_plus_one)){ _throw( Null_Exception);}* _temp4182;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4183= yyvs; void**
_temp4185= _temp4183.curr + yyvsp_offset; if( _temp4183.base == 0? 1:( _temp4185
< _temp4183.base? 1: _temp4185 >= _temp4183.last_plus_one)){ _throw(
Null_Exception);}* _temp4185;})), 0); _temp4176;}); _temp4175;}); _temp4174;});
break; case 137: _LL4173: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4195=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4195[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4196; _temp4196.tag= Cyc_TypeModifierList_tok; _temp4196.f1=({ struct Cyc_List_List*
_temp4197=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4197->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4207=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4207[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4208; _temp4208.tag=
Cyc_Absyn_Pointer_mod; _temp4208.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4209= yyvs; void** _temp4211= _temp4209.curr +(
yyvsp_offset - 3); if( _temp4209.base == 0? 1:( _temp4211 < _temp4209.base? 1:
_temp4211 >= _temp4209.last_plus_one)){ _throw( Null_Exception);}* _temp4211;}));
_temp4208.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4212= yyvs;
void** _temp4214= _temp4212.curr +( yyvsp_offset - 2); if( _temp4212.base == 0?
1:( _temp4214 < _temp4212.base? 1: _temp4214 >= _temp4212.last_plus_one)){
_throw( Null_Exception);}* _temp4214;})); _temp4208.f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp4215= yyvs; void** _temp4217= _temp4215.curr +
yyvsp_offset; if( _temp4215.base == 0? 1:( _temp4217 < _temp4215.base? 1:
_temp4217 >= _temp4215.last_plus_one)){ _throw( Null_Exception);}* _temp4217;}));
_temp4208;}); _temp4207;})); _temp4197->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4198= yyls; struct Cyc_Yyltype* _temp4200= _temp4198.curr
+( yylsp_offset - 1); if( _temp4198.base == 0? 1:( _temp4200 < _temp4198.base? 1:
_temp4200 >= _temp4198.last_plus_one)){ _throw( Null_Exception);}* _temp4200;}).first_line,({
struct _tagged_ptr3 _temp4201= yyls; struct Cyc_Yyltype* _temp4203= _temp4201.curr
+( yylsp_offset - 1); if( _temp4201.base == 0? 1:( _temp4203 < _temp4201.base? 1:
_temp4203 >= _temp4201.last_plus_one)){ _throw( Null_Exception);}* _temp4203;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4204= yyvs; void**
_temp4206= _temp4204.curr +( yyvsp_offset - 1); if( _temp4204.base == 0? 1:(
_temp4206 < _temp4204.base? 1: _temp4206 >= _temp4204.last_plus_one)){ _throw(
Null_Exception);}* _temp4206;})), 0); _temp4197;}); _temp4196;}); _temp4195;});
break; case 138: _LL4194: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4219=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4219[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4220; _temp4220.tag= Cyc_TypeModifierList_tok; _temp4220.f1=({ struct Cyc_List_List*
_temp4221=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4221->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4234=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4234[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4235; _temp4235.tag=
Cyc_Absyn_Pointer_mod; _temp4235.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4236= yyvs; void** _temp4238= _temp4236.curr +(
yyvsp_offset - 3); if( _temp4236.base == 0? 1:( _temp4238 < _temp4236.base? 1:
_temp4238 >= _temp4236.last_plus_one)){ _throw( Null_Exception);}* _temp4238;}));
_temp4235.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4239= yyvs;
void** _temp4241= _temp4239.curr +( yyvsp_offset - 2); if( _temp4239.base == 0?
1:( _temp4241 < _temp4239.base? 1: _temp4241 >= _temp4239.last_plus_one)){
_throw( Null_Exception);}* _temp4241;})); _temp4235.f3= Cyc_Absyn_empty_tqual();
_temp4235;}); _temp4234;})); _temp4221->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4222= yyls; struct Cyc_Yyltype* _temp4224= _temp4222.curr
+( yylsp_offset - 1); if( _temp4222.base == 0? 1:( _temp4224 < _temp4222.base? 1:
_temp4224 >= _temp4222.last_plus_one)){ _throw( Null_Exception);}* _temp4224;}).first_line,({
struct _tagged_ptr3 _temp4225= yyls; struct Cyc_Yyltype* _temp4227= _temp4225.curr
+( yylsp_offset - 1); if( _temp4225.base == 0? 1:( _temp4227 < _temp4225.base? 1:
_temp4227 >= _temp4225.last_plus_one)){ _throw( Null_Exception);}* _temp4227;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4228= yyvs; void**
_temp4230= _temp4228.curr +( yyvsp_offset - 1); if( _temp4228.base == 0? 1:(
_temp4230 < _temp4228.base? 1: _temp4230 >= _temp4228.last_plus_one)){ _throw(
Null_Exception);}* _temp4230;})), Cyc_yyget_TypeModifierList_tok(({ struct
_tagged_ptr2 _temp4231= yyvs; void** _temp4233= _temp4231.curr + yyvsp_offset;
if( _temp4231.base == 0? 1:( _temp4233 < _temp4231.base? 1: _temp4233 >=
_temp4231.last_plus_one)){ _throw( Null_Exception);}* _temp4233;}))); _temp4221;});
_temp4220;}); _temp4219;}); break; case 139: _LL4218: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp4243=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4243[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4244; _temp4244.tag= Cyc_TypeModifierList_tok;
_temp4244.f1=({ struct Cyc_List_List* _temp4245=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4245->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4258=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4258[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4259; _temp4259.tag= Cyc_Absyn_Pointer_mod;
_temp4259.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4260= yyvs; void** _temp4262= _temp4260.curr +( yyvsp_offset - 4); if(
_temp4260.base == 0? 1:( _temp4262 < _temp4260.base? 1: _temp4262 >= _temp4260.last_plus_one)){
_throw( Null_Exception);}* _temp4262;})); _temp4259.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4263= yyvs; void** _temp4265= _temp4263.curr +(
yyvsp_offset - 3); if( _temp4263.base == 0? 1:( _temp4265 < _temp4263.base? 1:
_temp4265 >= _temp4263.last_plus_one)){ _throw( Null_Exception);}* _temp4265;}));
_temp4259.f3= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp4266= yyvs;
void** _temp4268= _temp4266.curr +( yyvsp_offset - 1); if( _temp4266.base == 0?
1:( _temp4268 < _temp4266.base? 1: _temp4268 >= _temp4266.last_plus_one)){
_throw( Null_Exception);}* _temp4268;})); _temp4259;}); _temp4258;})); _temp4245->tl=
Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4246= yyls; struct Cyc_Yyltype* _temp4248= _temp4246.curr +( yylsp_offset -
2); if( _temp4246.base == 0? 1:( _temp4248 < _temp4246.base? 1: _temp4248 >=
_temp4246.last_plus_one)){ _throw( Null_Exception);}* _temp4248;}).first_line,({
struct _tagged_ptr3 _temp4249= yyls; struct Cyc_Yyltype* _temp4251= _temp4249.curr
+( yylsp_offset - 2); if( _temp4249.base == 0? 1:( _temp4251 < _temp4249.base? 1:
_temp4251 >= _temp4249.last_plus_one)){ _throw( Null_Exception);}* _temp4251;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4252= yyvs; void**
_temp4254= _temp4252.curr +( yyvsp_offset - 2); if( _temp4252.base == 0? 1:(
_temp4254 < _temp4252.base? 1: _temp4254 >= _temp4252.last_plus_one)){ _throw(
Null_Exception);}* _temp4254;})), Cyc_yyget_TypeModifierList_tok(({ struct
_tagged_ptr2 _temp4255= yyvs; void** _temp4257= _temp4255.curr + yyvsp_offset;
if( _temp4255.base == 0? 1:( _temp4257 < _temp4255.base? 1: _temp4257 >=
_temp4255.last_plus_one)){ _throw( Null_Exception);}* _temp4257;}))); _temp4245;});
_temp4244;}); _temp4243;}); break; case 140: _LL4242: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp4270=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4270[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4271; _temp4271.tag= Cyc_Pointer_Sort_tok;
_temp4271.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp4272=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4272[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4273; _temp4273.tag=
Cyc_Absyn_Nullable_ps; _temp4273.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4274= yyls; struct Cyc_Yyltype* _temp4276= _temp4274.curr
+ yylsp_offset; if( _temp4274.base == 0? 1:( _temp4276 < _temp4274.base? 1:
_temp4276 >= _temp4274.last_plus_one)){ _throw( Null_Exception);}* _temp4276;}).first_line,({
struct _tagged_ptr3 _temp4277= yyls; struct Cyc_Yyltype* _temp4279= _temp4277.curr
+ yylsp_offset; if( _temp4277.base == 0? 1:( _temp4279 < _temp4277.base? 1:
_temp4279 >= _temp4277.last_plus_one)){ _throw( Null_Exception);}* _temp4279;}).last_line));
_temp4273;}); _temp4272;})); _temp4271;}); _temp4270;}); break; case 141:
_LL4269: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4281=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4281[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4282; _temp4282.tag=
Cyc_Pointer_Sort_tok; _temp4282.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4283=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4283[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4284; _temp4284.tag= Cyc_Absyn_NonNullable_ps; _temp4284.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4285= yyls; struct
Cyc_Yyltype* _temp4287= _temp4285.curr + yylsp_offset; if( _temp4285.base == 0?
1:( _temp4287 < _temp4285.base? 1: _temp4287 >= _temp4285.last_plus_one)){
_throw( Null_Exception);}* _temp4287;}).first_line,({ struct _tagged_ptr3
_temp4288= yyls; struct Cyc_Yyltype* _temp4290= _temp4288.curr + yylsp_offset;
if( _temp4288.base == 0? 1:( _temp4290 < _temp4288.base? 1: _temp4290 >=
_temp4288.last_plus_one)){ _throw( Null_Exception);}* _temp4290;}).last_line));
_temp4284;}); _temp4283;})); _temp4282;}); _temp4281;}); break; case 142:
_LL4280: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4292=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4292[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4293; _temp4293.tag=
Cyc_Pointer_Sort_tok; _temp4293.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4294=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4294[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4295; _temp4295.tag=
Cyc_Absyn_Nullable_ps; _temp4295.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4296= yyvs; void** _temp4298= _temp4296.curr +( yyvsp_offset - 1); if(
_temp4296.base == 0? 1:( _temp4298 < _temp4296.base? 1: _temp4298 >= _temp4296.last_plus_one)){
_throw( Null_Exception);}* _temp4298;})); _temp4295;}); _temp4294;})); _temp4293;});
_temp4292;}); break; case 143: _LL4291: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4300=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4300[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4301; _temp4301.tag=
Cyc_Pointer_Sort_tok; _temp4301.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4302=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4302[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4303; _temp4303.tag= Cyc_Absyn_NonNullable_ps; _temp4303.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4304= yyvs; void** _temp4306= _temp4304.curr +(
yyvsp_offset - 1); if( _temp4304.base == 0? 1:( _temp4306 < _temp4304.base? 1:
_temp4306 >= _temp4304.last_plus_one)){ _throw( Null_Exception);}* _temp4306;}));
_temp4303;}); _temp4302;})); _temp4301;}); _temp4300;}); break; case 144:
_LL4299: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4308=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4308[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4309; _temp4309.tag=
Cyc_Pointer_Sort_tok; _temp4309.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps);
_temp4309;}); _temp4308;}); break; case 145: _LL4307: yyval=( void*)({ struct
Cyc_Rgn_tok_struct* _temp4311=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp4311[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4312; _temp4312.tag= Cyc_Rgn_tok; _temp4312.f1=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp4312;}); _temp4311;}); break; case 146: _LL4310: yyval=({ struct
_tagged_ptr2 _temp4314= yyvs; void** _temp4316= _temp4314.curr + yyvsp_offset;
if( _temp4314.base == 0? 1:( _temp4316 < _temp4314.base? 1: _temp4316 >=
_temp4314.last_plus_one)){ _throw( Null_Exception);}* _temp4316;}); break; case
147: _LL4313: yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp4318=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4318[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4319; _temp4319.tag= Cyc_Rgn_tok; _temp4319.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4320= yyvs; void** _temp4322=
_temp4320.curr + yyvsp_offset; if( _temp4320.base == 0? 1:( _temp4322 <
_temp4320.base? 1: _temp4322 >= _temp4320.last_plus_one)){ _throw(
Null_Exception);}* _temp4322;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp4319;}); _temp4318;}); break; case 148: _LL4317:
if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp4324= yyvs; void** _temp4326=
_temp4324.curr + yyvsp_offset; if( _temp4324.base == 0? 1:( _temp4326 <
_temp4324.base? 1: _temp4326 >= _temp4324.last_plus_one)){ _throw(
Null_Exception);}* _temp4326;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4327=( char*)"expecting region kind\n";
struct _tagged_string _temp4328; _temp4328.curr= _temp4327; _temp4328.base=
_temp4327; _temp4328.last_plus_one= _temp4327 + 23; _temp4328;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4329= yyls; struct Cyc_Yyltype* _temp4331= _temp4329.curr
+ yylsp_offset; if( _temp4329.base == 0? 1:( _temp4331 < _temp4329.base? 1:
_temp4331 >= _temp4329.last_plus_one)){ _throw( Null_Exception);}* _temp4331;}).first_line,({
struct _tagged_ptr3 _temp4332= yyls; struct Cyc_Yyltype* _temp4334= _temp4332.curr
+ yylsp_offset; if( _temp4332.base == 0? 1:( _temp4334 < _temp4332.base? 1:
_temp4334 >= _temp4332.last_plus_one)){ _throw( Null_Exception);}* _temp4334;}).last_line));}
yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp4335=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4335[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4336; _temp4336.tag= Cyc_Rgn_tok; _temp4336.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4337= yyvs; void** _temp4339=
_temp4337.curr +( yyvsp_offset - 2); if( _temp4337.base == 0? 1:( _temp4339 <
_temp4337.base? 1: _temp4339 >= _temp4337.last_plus_one)){ _throw(
Null_Exception);}* _temp4339;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp4336;}); _temp4335;}); break; case 149: _LL4323:
yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp4341=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4341[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4342; _temp4342.tag= Cyc_Rgn_tok; _temp4342.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp4342;}); _temp4341;}); break; case 150: _LL4340:
yyval=({ struct _tagged_ptr2 _temp4344= yyvs; void** _temp4346= _temp4344.curr +
yyvsp_offset; if( _temp4344.base == 0? 1:( _temp4346 < _temp4344.base? 1:
_temp4346 >= _temp4344.last_plus_one)){ _throw( Null_Exception);}* _temp4346;});
break; case 151: _LL4343: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp4348=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp4348[ 0]=({ struct Cyc_TypeQual_tok_struct _temp4349; _temp4349.tag= Cyc_TypeQual_tok;
_temp4349.f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4350= yyvs; void** _temp4352= _temp4350.curr +( yyvsp_offset -
1); if( _temp4350.base == 0? 1:( _temp4352 < _temp4350.base? 1: _temp4352 >=
_temp4350.last_plus_one)){ _throw( Null_Exception);}* _temp4352;})), Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp4353= yyvs; void** _temp4355= _temp4353.curr +
yyvsp_offset; if( _temp4353.base == 0? 1:( _temp4355 < _temp4353.base? 1:
_temp4355 >= _temp4353.last_plus_one)){ _throw( Null_Exception);}* _temp4355;})));
_temp4349;}); _temp4348;}); break; case 152: _LL4347: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp4357=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp4357[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp4358; _temp4358.tag= Cyc_ParamDeclListBool_tok;
_temp4358.f1=({ struct _tuple16* _temp4359=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp4359->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4360= yyvs; void** _temp4362= _temp4360.curr + yyvsp_offset; if( _temp4360.base
== 0? 1:( _temp4362 < _temp4360.base? 1: _temp4362 >= _temp4360.last_plus_one)){
_throw( Null_Exception);}* _temp4362;}))); _temp4359->f2= 0; _temp4359->f3= 0;
_temp4359;}); _temp4358;}); _temp4357;}); break; case 153: _LL4356: yyval=( void*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp4364=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp4364[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp4365; _temp4365.tag= Cyc_ParamDeclListBool_tok;
_temp4365.f1=({ struct _tuple16* _temp4366=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp4366->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4367= yyvs; void** _temp4369= _temp4367.curr +( yyvsp_offset - 2); if(
_temp4367.base == 0? 1:( _temp4369 < _temp4367.base? 1: _temp4369 >= _temp4367.last_plus_one)){
_throw( Null_Exception);}* _temp4369;}))); _temp4366->f2= 1; _temp4366->f3= 0;
_temp4366;}); _temp4365;}); _temp4364;}); break; case 154: _LL4363: yyval=( void*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp4371=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp4371[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp4372; _temp4372.tag= Cyc_ParamDeclListBool_tok;
_temp4372.f1=({ struct _tuple16* _temp4373=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp4373->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4380= yyvs; void** _temp4382= _temp4380.curr +( yyvsp_offset - 2); if(
_temp4380.base == 0? 1:( _temp4382 < _temp4380.base? 1: _temp4382 >= _temp4380.last_plus_one)){
_throw( Null_Exception);}* _temp4382;}))); _temp4373->f2= 0; _temp4373->f3=(
struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp4374=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4374->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp4375=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4375[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4376; _temp4376.tag= Cyc_Absyn_JoinEff; _temp4376.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4377= yyvs; void** _temp4379=
_temp4377.curr + yyvsp_offset; if( _temp4377.base == 0? 1:( _temp4379 <
_temp4377.base? 1: _temp4379 >= _temp4377.last_plus_one)){ _throw(
Null_Exception);}* _temp4379;})); _temp4376;}); _temp4375;})); _temp4374;});
_temp4373;}); _temp4372;}); _temp4371;}); break; case 155: _LL4370: yyval=({
struct _tagged_ptr2 _temp4384= yyvs; void** _temp4386= _temp4384.curr +
yyvsp_offset; if( _temp4384.base == 0? 1:( _temp4386 < _temp4384.base? 1:
_temp4386 >= _temp4384.last_plus_one)){ _throw( Null_Exception);}* _temp4386;});
break; case 156: _LL4383: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4388=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4388[ 0]=({ struct Cyc_TypeList_tok_struct _temp4389; _temp4389.tag= Cyc_TypeList_tok;
_temp4389.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2
_temp4390= yyvs; void** _temp4392= _temp4390.curr +( yyvsp_offset - 2); if(
_temp4390.base == 0? 1:( _temp4392 < _temp4390.base? 1: _temp4392 >= _temp4390.last_plus_one)){
_throw( Null_Exception);}* _temp4392;})), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4393= yyvs; void** _temp4395= _temp4393.curr + yyvsp_offset;
if( _temp4393.base == 0? 1:( _temp4395 < _temp4393.base? 1: _temp4395 >=
_temp4393.last_plus_one)){ _throw( Null_Exception);}* _temp4395;}))); _temp4389;});
_temp4388;}); break; case 157: _LL4387: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4397=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4397[ 0]=({ struct Cyc_TypeList_tok_struct _temp4398; _temp4398.tag= Cyc_TypeList_tok;
_temp4398.f1= 0; _temp4398;}); _temp4397;}); break; case 158: _LL4396: yyval=({
struct _tagged_ptr2 _temp4400= yyvs; void** _temp4402= _temp4400.curr +(
yyvsp_offset - 1); if( _temp4400.base == 0? 1:( _temp4402 < _temp4400.base? 1:
_temp4402 >= _temp4400.last_plus_one)){ _throw( Null_Exception);}* _temp4402;});
break; case 159: _LL4399: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4404=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4404[ 0]=({ struct Cyc_TypeList_tok_struct _temp4405; _temp4405.tag= Cyc_TypeList_tok;
_temp4405.f1=({ struct Cyc_List_List* _temp4406=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4406->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4407= yyvs; void** _temp4409=
_temp4407.curr + yyvsp_offset; if( _temp4407.base == 0? 1:( _temp4409 <
_temp4407.base? 1: _temp4409 >= _temp4407.last_plus_one)){ _throw(
Null_Exception);}* _temp4409;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_EffKind)); _temp4406->tl= 0; _temp4406;}); _temp4405;});
_temp4404;}); break; case 160: _LL4403: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4411= yyvs; void** _temp4413= _temp4411.curr + yyvsp_offset;
if( _temp4411.base == 0? 1:( _temp4413 < _temp4411.base? 1: _temp4413 >=
_temp4411.last_plus_one)){ _throw( Null_Exception);}* _temp4413;})) !=( void*)
Cyc_Absyn_EffKind){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp4414=(
char*)"expecing effect kind (E)"; struct _tagged_string _temp4415; _temp4415.curr=
_temp4414; _temp4415.base= _temp4414; _temp4415.last_plus_one= _temp4414 + 25;
_temp4415;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4416= yyls;
struct Cyc_Yyltype* _temp4418= _temp4416.curr + yylsp_offset; if( _temp4416.base
== 0? 1:( _temp4418 < _temp4416.base? 1: _temp4418 >= _temp4416.last_plus_one)){
_throw( Null_Exception);}* _temp4418;}).first_line,({ struct _tagged_ptr3
_temp4419= yyls; struct Cyc_Yyltype* _temp4421= _temp4419.curr + yylsp_offset;
if( _temp4419.base == 0? 1:( _temp4421 < _temp4419.base? 1: _temp4421 >=
_temp4419.last_plus_one)){ _throw( Null_Exception);}* _temp4421;}).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4422=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4422[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4423; _temp4423.tag= Cyc_TypeList_tok; _temp4423.f1=({ struct Cyc_List_List*
_temp4424=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4424->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4425= yyvs; void** _temp4427= _temp4425.curr +( yyvsp_offset -
2); if( _temp4425.base == 0? 1:( _temp4427 < _temp4425.base? 1: _temp4427 >=
_temp4425.last_plus_one)){ _throw( Null_Exception);}* _temp4427;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp4424->tl= 0; _temp4424;}); _temp4423;}); _temp4422;}); break; case 161:
_LL4410: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4429=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4429[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4430; _temp4430.tag= Cyc_TypeList_tok; _temp4430.f1=({ struct Cyc_List_List*
_temp4431=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4431->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4432=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4432[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4433; _temp4433.tag= Cyc_Absyn_AccessEff;
_temp4433.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4434= yyvs; void** _temp4436= _temp4434.curr + yyvsp_offset;
if( _temp4434.base == 0? 1:( _temp4436 < _temp4434.base? 1: _temp4436 >=
_temp4434.last_plus_one)){ _throw( Null_Exception);}* _temp4436;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4433;}); _temp4432;})); _temp4431->tl= 0; _temp4431;}); _temp4430;});
_temp4429;}); break; case 162: _LL4428: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4438=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4438[ 0]=({ struct Cyc_TypeList_tok_struct _temp4439; _temp4439.tag= Cyc_TypeList_tok;
_temp4439.f1=({ struct Cyc_List_List* _temp4440=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4440->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4444=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4444[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4445; _temp4445.tag= Cyc_Absyn_AccessEff;
_temp4445.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4446= yyvs; void** _temp4448= _temp4446.curr +( yyvsp_offset -
2); if( _temp4446.base == 0? 1:( _temp4448 < _temp4446.base? 1: _temp4448 >=
_temp4446.last_plus_one)){ _throw( Null_Exception);}* _temp4448;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4445;}); _temp4444;})); _temp4440->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4441= yyvs; void** _temp4443= _temp4441.curr + yyvsp_offset;
if( _temp4441.base == 0? 1:( _temp4443 < _temp4441.base? 1: _temp4443 >=
_temp4441.last_plus_one)){ _throw( Null_Exception);}* _temp4443;})); _temp4440;});
_temp4439;}); _temp4438;}); break; case 163: _LL4437: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4450= yyvs; void** _temp4452= _temp4450.curr +
yyvsp_offset; if( _temp4450.base == 0? 1:( _temp4452 < _temp4450.base? 1:
_temp4452 >= _temp4450.last_plus_one)){ _throw( Null_Exception);}* _temp4452;}))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp4453=( char*)"expecting region kind (R)"; struct _tagged_string _temp4454;
_temp4454.curr= _temp4453; _temp4454.base= _temp4453; _temp4454.last_plus_one=
_temp4453 + 26; _temp4454;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4455= yyls; struct Cyc_Yyltype* _temp4457= _temp4455.curr + yylsp_offset;
if( _temp4455.base == 0? 1:( _temp4457 < _temp4455.base? 1: _temp4457 >=
_temp4455.last_plus_one)){ _throw( Null_Exception);}* _temp4457;}).first_line,({
struct _tagged_ptr3 _temp4458= yyls; struct Cyc_Yyltype* _temp4460= _temp4458.curr
+ yylsp_offset; if( _temp4458.base == 0? 1:( _temp4460 < _temp4458.base? 1:
_temp4460 >= _temp4458.last_plus_one)){ _throw( Null_Exception);}* _temp4460;}).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4461=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4461[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4462; _temp4462.tag= Cyc_TypeList_tok; _temp4462.f1=({ struct Cyc_List_List*
_temp4463=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4463->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4464=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4464[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4465; _temp4465.tag= Cyc_Absyn_AccessEff;
_temp4465.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4466= yyvs; void** _temp4468= _temp4466.curr +( yyvsp_offset -
2); if( _temp4466.base == 0? 1:( _temp4468 < _temp4466.base? 1: _temp4468 >=
_temp4466.last_plus_one)){ _throw( Null_Exception);}* _temp4468;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4465;}); _temp4464;})); _temp4463->tl= 0; _temp4463;}); _temp4462;});
_temp4461;}); break; case 164: _LL4449: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4470= yyvs; void** _temp4472= _temp4470.curr +( yyvsp_offset -
2); if( _temp4470.base == 0? 1:( _temp4472 < _temp4470.base? 1: _temp4472 >=
_temp4470.last_plus_one)){ _throw( Null_Exception);}* _temp4472;})) !=( void*)
Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp4473=(
char*)"expecting region kind (R)"; struct _tagged_string _temp4474; _temp4474.curr=
_temp4473; _temp4474.base= _temp4473; _temp4474.last_plus_one= _temp4473 + 26;
_temp4474;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4475= yyls;
struct Cyc_Yyltype* _temp4477= _temp4475.curr +( yylsp_offset - 2); if(
_temp4475.base == 0? 1:( _temp4477 < _temp4475.base? 1: _temp4477 >= _temp4475.last_plus_one)){
_throw( Null_Exception);}* _temp4477;}).first_line,({ struct _tagged_ptr3
_temp4478= yyls; struct Cyc_Yyltype* _temp4480= _temp4478.curr +( yylsp_offset -
2); if( _temp4478.base == 0? 1:( _temp4480 < _temp4478.base? 1: _temp4480 >=
_temp4478.last_plus_one)){ _throw( Null_Exception);}* _temp4480;}).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4481=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4481[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4482; _temp4482.tag= Cyc_TypeList_tok; _temp4482.f1=({ struct Cyc_List_List*
_temp4483=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4483->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4487=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4487[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4488; _temp4488.tag= Cyc_Absyn_AccessEff;
_temp4488.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4489= yyvs; void** _temp4491= _temp4489.curr +( yyvsp_offset -
4); if( _temp4489.base == 0? 1:( _temp4491 < _temp4489.base? 1: _temp4491 >=
_temp4489.last_plus_one)){ _throw( Null_Exception);}* _temp4491;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4488;}); _temp4487;})); _temp4483->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4484= yyvs; void** _temp4486= _temp4484.curr + yyvsp_offset;
if( _temp4484.base == 0? 1:( _temp4486 < _temp4484.base? 1: _temp4486 >=
_temp4484.last_plus_one)){ _throw( Null_Exception);}* _temp4486;})); _temp4483;});
_temp4482;}); _temp4481;}); break; case 165: _LL4469: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp4493=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp4493[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp4494; _temp4494.tag= Cyc_ParamDeclList_tok;
_temp4494.f1=({ struct Cyc_List_List* _temp4495=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4495->hd=( void*) Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp4496= yyvs; void** _temp4498= _temp4496.curr +
yyvsp_offset; if( _temp4496.base == 0? 1:( _temp4498 < _temp4496.base? 1:
_temp4498 >= _temp4496.last_plus_one)){ _throw( Null_Exception);}* _temp4498;}));
_temp4495->tl= 0; _temp4495;}); _temp4494;}); _temp4493;}); break; case 166:
_LL4492: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp4500=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4500[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4501; _temp4501.tag=
Cyc_ParamDeclList_tok; _temp4501.f1=({ struct Cyc_List_List* _temp4502=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4502->hd=( void*)
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4506= yyvs; void** _temp4508=
_temp4506.curr + yyvsp_offset; if( _temp4506.base == 0? 1:( _temp4508 <
_temp4506.base? 1: _temp4508 >= _temp4506.last_plus_one)){ _throw(
Null_Exception);}* _temp4508;})); _temp4502->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4503= yyvs; void** _temp4505= _temp4503.curr +(
yyvsp_offset - 2); if( _temp4503.base == 0? 1:( _temp4505 < _temp4503.base? 1:
_temp4505 >= _temp4503.last_plus_one)){ _throw( Null_Exception);}* _temp4505;}));
_temp4502;}); _temp4501;}); _temp4500;}); break; case 167: _LL4499: { void* t=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4572= yyvs; void** _temp4574= _temp4572.curr +( yyvsp_offset - 1); if(
_temp4572.base == 0? 1:( _temp4574 < _temp4572.base? 1: _temp4574 >= _temp4572.last_plus_one)){
_throw( Null_Exception);}* _temp4574;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4575= yyls; struct Cyc_Yyltype* _temp4577= _temp4575.curr
+( yylsp_offset - 1); if( _temp4575.base == 0? 1:( _temp4577 < _temp4575.base? 1:
_temp4577 >= _temp4575.last_plus_one)){ _throw( Null_Exception);}* _temp4577;}).first_line,({
struct _tagged_ptr3 _temp4578= yyls; struct Cyc_Yyltype* _temp4580= _temp4578.curr
+( yylsp_offset - 1); if( _temp4578.base == 0? 1:( _temp4580 < _temp4578.base? 1:
_temp4580 >= _temp4578.last_plus_one)){ _throw( Null_Exception);}* _temp4580;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4569= yyvs; void** _temp4571= _temp4569.curr +( yyvsp_offset - 1); if(
_temp4569.base == 0? 1:( _temp4571 < _temp4569.base? 1: _temp4571 >= _temp4569.last_plus_one)){
_throw( Null_Exception);}* _temp4571;}))).f3; struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4566= yyvs; void** _temp4568= _temp4566.curr +(
yyvsp_offset - 1); if( _temp4566.base == 0? 1:( _temp4568 < _temp4566.base? 1:
_temp4568 >= _temp4566.last_plus_one)){ _throw( Null_Exception);}* _temp4568;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4563= yyvs; void** _temp4565= _temp4563.curr + yyvsp_offset; if( _temp4563.base
== 0? 1:( _temp4565 < _temp4563.base? 1: _temp4565 >= _temp4563.last_plus_one)){
_throw( Null_Exception);}* _temp4565;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp4510=( char*)"parameter with bad type params"; struct _tagged_string
_temp4511; _temp4511.curr= _temp4510; _temp4511.base= _temp4510; _temp4511.last_plus_one=
_temp4510 + 31; _temp4511;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4512= yyls; struct Cyc_Yyltype* _temp4514= _temp4512.curr + yylsp_offset;
if( _temp4512.base == 0? 1:( _temp4514 < _temp4512.base? 1: _temp4514 >=
_temp4512.last_plus_one)){ _throw( Null_Exception);}* _temp4514;}).first_line,({
struct _tagged_ptr3 _temp4515= yyls; struct Cyc_Yyltype* _temp4517= _temp4515.curr
+ yylsp_offset; if( _temp4515.base == 0? 1:( _temp4517 < _temp4515.base? 1:
_temp4517 >= _temp4515.last_plus_one)){ _throw( Null_Exception);}* _temp4517;}).last_line));}{
struct _tuple1* q=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4560=
yyvs; void** _temp4562= _temp4560.curr + yyvsp_offset; if( _temp4560.base == 0?
1:( _temp4562 < _temp4560.base? 1: _temp4562 >= _temp4560.last_plus_one)){
_throw( Null_Exception);}* _temp4562;})))->id;{ void* _temp4518=(* q).f1; struct
Cyc_List_List* _temp4528; struct Cyc_List_List* _temp4530; _LL4520: if(
_temp4518 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4521;} else{ goto _LL4522;}
_LL4522: if(( unsigned int) _temp4518 > 1u?*(( int*) _temp4518) == Cyc_Absyn_Rel_n:
0){ _LL4529: _temp4528=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp4518)->f1; if( _temp4528 == 0){ goto _LL4523;} else{ goto _LL4524;}} else{
goto _LL4524;} _LL4524: if(( unsigned int) _temp4518 > 1u?*(( int*) _temp4518)
== Cyc_Absyn_Abs_n: 0){ _LL4531: _temp4530=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp4518)->f1; if( _temp4530 == 0){ goto _LL4525;} else{ goto _LL4526;}} else{
goto _LL4526;} _LL4526: goto _LL4527; _LL4521: goto _LL4519; _LL4523: goto
_LL4519; _LL4525: goto _LL4519; _LL4527: Cyc_Parse_err(( struct _tagged_string)({
char* _temp4532=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp4533; _temp4533.curr= _temp4532; _temp4533.base=
_temp4532; _temp4533.last_plus_one= _temp4532 + 49; _temp4533;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4534= yyls; struct Cyc_Yyltype* _temp4536= _temp4534.curr
+( yylsp_offset - 1); if( _temp4534.base == 0? 1:( _temp4536 < _temp4534.base? 1:
_temp4536 >= _temp4534.last_plus_one)){ _throw( Null_Exception);}* _temp4536;}).first_line,({
struct _tagged_ptr3 _temp4537= yyls; struct Cyc_Yyltype* _temp4539= _temp4537.curr
+( yylsp_offset - 1); if( _temp4537.base == 0? 1:( _temp4539 < _temp4537.base? 1:
_temp4539 >= _temp4537.last_plus_one)){ _throw( Null_Exception);}* _temp4539;}).last_line));
goto _LL4519; _LL4519:;}{ struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4559=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4559->v=( void*)(* q).f2; _temp4559;}); if( t_info.f4 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4540=( char*)"extra attributes on parameter, ignoring";
struct _tagged_string _temp4541; _temp4541.curr= _temp4540; _temp4541.base=
_temp4540; _temp4541.last_plus_one= _temp4540 + 40; _temp4541;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4542= yyls; struct Cyc_Yyltype* _temp4544= _temp4542.curr
+( yylsp_offset - 1); if( _temp4542.base == 0? 1:( _temp4544 < _temp4542.base? 1:
_temp4544 >= _temp4542.last_plus_one)){ _throw( Null_Exception);}* _temp4544;}).first_line,({
struct _tagged_ptr3 _temp4545= yyls; struct Cyc_Yyltype* _temp4547= _temp4545.curr
+ yylsp_offset; if( _temp4545.base == 0? 1:( _temp4547 < _temp4545.base? 1:
_temp4547 >= _temp4545.last_plus_one)){ _throw( Null_Exception);}* _temp4547;}).last_line));}
if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4548=(
char*)"extra type variables on parameter, ignoring"; struct _tagged_string
_temp4549; _temp4549.curr= _temp4548; _temp4549.base= _temp4548; _temp4549.last_plus_one=
_temp4548 + 44; _temp4549;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4550= yyls; struct Cyc_Yyltype* _temp4552= _temp4550.curr +( yylsp_offset -
1); if( _temp4550.base == 0? 1:( _temp4552 < _temp4550.base? 1: _temp4552 >=
_temp4550.last_plus_one)){ _throw( Null_Exception);}* _temp4552;}).first_line,({
struct _tagged_ptr3 _temp4553= yyls; struct Cyc_Yyltype* _temp4555= _temp4553.curr
+ yylsp_offset; if( _temp4553.base == 0? 1:( _temp4555 < _temp4553.base? 1:
_temp4555 >= _temp4553.last_plus_one)){ _throw( Null_Exception);}* _temp4555;}).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4556=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4556[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4557; _temp4557.tag= Cyc_ParamDecl_tok; _temp4557.f1=({
struct _tuple2* _temp4558=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4558->f1= idopt; _temp4558->f2= t_info.f1; _temp4558->f3= t_info.f2;
_temp4558;}); _temp4557;}); _temp4556;}); break;}}} case 168: _LL4509: { void* t=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4599= yyvs; void** _temp4601= _temp4599.curr + yyvsp_offset; if( _temp4599.base
== 0? 1:( _temp4601 < _temp4599.base? 1: _temp4601 >= _temp4599.last_plus_one)){
_throw( Null_Exception);}* _temp4601;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4602= yyls; struct Cyc_Yyltype* _temp4604= _temp4602.curr
+ yylsp_offset; if( _temp4602.base == 0? 1:( _temp4604 < _temp4602.base? 1:
_temp4604 >= _temp4602.last_plus_one)){ _throw( Null_Exception);}* _temp4604;}).first_line,({
struct _tagged_ptr3 _temp4605= yyls; struct Cyc_Yyltype* _temp4607= _temp4605.curr
+ yylsp_offset; if( _temp4605.base == 0? 1:( _temp4607 < _temp4605.base? 1:
_temp4607 >= _temp4605.last_plus_one)){ _throw( Null_Exception);}* _temp4607;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4596= yyvs; void** _temp4598= _temp4596.curr + yyvsp_offset; if( _temp4596.base
== 0? 1:( _temp4598 < _temp4596.base? 1: _temp4598 >= _temp4596.last_plus_one)){
_throw( Null_Exception);}* _temp4598;}))).f3; struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4593= yyvs; void** _temp4595= _temp4593.curr +
yyvsp_offset; if( _temp4593.base == 0? 1:( _temp4595 < _temp4593.base? 1:
_temp4595 >= _temp4593.last_plus_one)){ _throw( Null_Exception);}* _temp4595;}))).f1;
if( atts != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4582=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4583; _temp4583.curr= _temp4582; _temp4583.base=
_temp4582; _temp4583.last_plus_one= _temp4582 + 38; _temp4583;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4584= yyls; struct Cyc_Yyltype* _temp4586= _temp4584.curr
+ yylsp_offset; if( _temp4584.base == 0? 1:( _temp4586 < _temp4584.base? 1:
_temp4586 >= _temp4584.last_plus_one)){ _throw( Null_Exception);}* _temp4586;}).first_line,({
struct _tagged_ptr3 _temp4587= yyls; struct Cyc_Yyltype* _temp4589= _temp4587.curr
+ yylsp_offset; if( _temp4587.base == 0? 1:( _temp4589 < _temp4587.base? 1:
_temp4589 >= _temp4587.last_plus_one)){ _throw( Null_Exception);}* _temp4589;}).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4590=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4590[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4591; _temp4591.tag= Cyc_ParamDecl_tok; _temp4591.f1=({
struct _tuple2* _temp4592=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4592->f1= 0; _temp4592->f2= tq; _temp4592->f3= t; _temp4592;}); _temp4591;});
_temp4590;}); break;} case 169: _LL4581: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4637= yyvs; void**
_temp4639= _temp4637.curr +( yyvsp_offset - 1); if( _temp4637.base == 0? 1:(
_temp4639 < _temp4637.base? 1: _temp4639 >= _temp4637.last_plus_one)){ _throw(
Null_Exception);}* _temp4639;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4640= yyls; struct Cyc_Yyltype* _temp4642= _temp4640.curr +(
yylsp_offset - 1); if( _temp4640.base == 0? 1:( _temp4642 < _temp4640.base? 1:
_temp4642 >= _temp4640.last_plus_one)){ _throw( Null_Exception);}* _temp4642;}).first_line,({
struct _tagged_ptr3 _temp4643= yyls; struct Cyc_Yyltype* _temp4645= _temp4643.curr
+( yylsp_offset - 1); if( _temp4643.base == 0? 1:( _temp4645 < _temp4643.base? 1:
_temp4645 >= _temp4643.last_plus_one)){ _throw( Null_Exception);}* _temp4645;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4634= yyvs; void** _temp4636= _temp4634.curr +( yyvsp_offset - 1); if(
_temp4634.base == 0? 1:( _temp4636 < _temp4634.base? 1: _temp4636 >= _temp4634.last_plus_one)){
_throw( Null_Exception);}* _temp4636;}))).f3; struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4631= yyvs; void** _temp4633= _temp4631.curr +(
yyvsp_offset - 1); if( _temp4631.base == 0? 1:( _temp4633 < _temp4631.base? 1:
_temp4633 >= _temp4631.last_plus_one)){ _throw( Null_Exception);}* _temp4633;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4628= yyvs; void** _temp4630= _temp4628.curr + yyvsp_offset;
if( _temp4628.base == 0? 1:( _temp4630 < _temp4628.base? 1: _temp4630 >=
_temp4628.last_plus_one)){ _throw( Null_Exception);}* _temp4630;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4609=( char*)"bad type parameters on formal argument, ignoring";
struct _tagged_string _temp4610; _temp4610.curr= _temp4609; _temp4610.base=
_temp4609; _temp4610.last_plus_one= _temp4609 + 49; _temp4610;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4611= yyls; struct Cyc_Yyltype* _temp4613= _temp4611.curr
+( yylsp_offset - 1); if( _temp4611.base == 0? 1:( _temp4613 < _temp4611.base? 1:
_temp4613 >= _temp4611.last_plus_one)){ _throw( Null_Exception);}* _temp4613;}).first_line,({
struct _tagged_ptr3 _temp4614= yyls; struct Cyc_Yyltype* _temp4616= _temp4614.curr
+ yylsp_offset; if( _temp4614.base == 0? 1:( _temp4616 < _temp4614.base? 1:
_temp4616 >= _temp4614.last_plus_one)){ _throw( Null_Exception);}* _temp4616;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4617=(
char*)"bad attributes on parameter, ignoring"; struct _tagged_string _temp4618;
_temp4618.curr= _temp4617; _temp4618.base= _temp4617; _temp4618.last_plus_one=
_temp4617 + 38; _temp4618;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4619= yyls; struct Cyc_Yyltype* _temp4621= _temp4619.curr +( yylsp_offset -
1); if( _temp4619.base == 0? 1:( _temp4621 < _temp4619.base? 1: _temp4621 >=
_temp4619.last_plus_one)){ _throw( Null_Exception);}* _temp4621;}).first_line,({
struct _tagged_ptr3 _temp4622= yyls; struct Cyc_Yyltype* _temp4624= _temp4622.curr
+ yylsp_offset; if( _temp4622.base == 0? 1:( _temp4624 < _temp4622.base? 1:
_temp4624 >= _temp4622.last_plus_one)){ _throw( Null_Exception);}* _temp4624;}).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4625=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4625[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4626; _temp4626.tag= Cyc_ParamDecl_tok; _temp4626.f1=({
struct _tuple2* _temp4627=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4627->f1= 0; _temp4627->f2= t_info.f1; _temp4627->f3= t_info.f2; _temp4627;});
_temp4626;}); _temp4625;}); break;} case 170: _LL4608: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp4647=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp4647[ 0]=({ struct Cyc_IdList_tok_struct
_temp4648; _temp4648.tag= Cyc_IdList_tok; _temp4648.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(({ struct
_tagged_ptr2 _temp4649= yyvs; void** _temp4651= _temp4649.curr + yyvsp_offset;
if( _temp4649.base == 0? 1:( _temp4651 < _temp4649.base? 1: _temp4651 >=
_temp4649.last_plus_one)){ _throw( Null_Exception);}* _temp4651;}))); _temp4648;});
_temp4647;}); break; case 171: _LL4646: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp4653=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4653[ 0]=({ struct Cyc_IdList_tok_struct _temp4654; _temp4654.tag= Cyc_IdList_tok;
_temp4654.f1=({ struct Cyc_List_List* _temp4655=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4655->hd=( void*)({ struct
_tagged_string* _temp4656=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4656[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4657= yyvs; void** _temp4659= _temp4657.curr + yyvsp_offset; if( _temp4657.base
== 0? 1:( _temp4659 < _temp4657.base? 1: _temp4659 >= _temp4657.last_plus_one)){
_throw( Null_Exception);}* _temp4659;})); _temp4656;}); _temp4655->tl= 0;
_temp4655;}); _temp4654;}); _temp4653;}); break; case 172: _LL4652: yyval=( void*)({
struct Cyc_IdList_tok_struct* _temp4661=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp4661[ 0]=({ struct Cyc_IdList_tok_struct
_temp4662; _temp4662.tag= Cyc_IdList_tok; _temp4662.f1=({ struct Cyc_List_List*
_temp4663=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4663->hd=( void*)({ struct _tagged_string* _temp4667=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4667[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4668= yyvs; void** _temp4670=
_temp4668.curr + yyvsp_offset; if( _temp4668.base == 0? 1:( _temp4670 <
_temp4668.base? 1: _temp4670 >= _temp4668.last_plus_one)){ _throw(
Null_Exception);}* _temp4670;})); _temp4667;}); _temp4663->tl= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp4664= yyvs; void** _temp4666= _temp4664.curr +(
yyvsp_offset - 2); if( _temp4664.base == 0? 1:( _temp4666 < _temp4664.base? 1:
_temp4666 >= _temp4664.last_plus_one)){ _throw( Null_Exception);}* _temp4666;}));
_temp4663;}); _temp4662;}); _temp4661;}); break; case 173: _LL4660: yyval=({
struct _tagged_ptr2 _temp4672= yyvs; void** _temp4674= _temp4672.curr +
yyvsp_offset; if( _temp4672.base == 0? 1:( _temp4674 < _temp4672.base? 1:
_temp4674 >= _temp4672.last_plus_one)){ _throw( Null_Exception);}* _temp4674;});
break; case 174: _LL4671: yyval=({ struct _tagged_ptr2 _temp4676= yyvs; void**
_temp4678= _temp4676.curr + yyvsp_offset; if( _temp4676.base == 0? 1:( _temp4678
< _temp4676.base? 1: _temp4678 >= _temp4676.last_plus_one)){ _throw(
Null_Exception);}* _temp4678;}); break; case 175: _LL4675: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4680=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4680[ 0]=({ struct Cyc_Exp_tok_struct
_temp4681; _temp4681.tag= Cyc_Exp_tok; _temp4681.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4682=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4682[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4683; _temp4683.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4683.f1= 0; _temp4683.f2= 0; _temp4683;}); _temp4682;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4684= yyls; struct Cyc_Yyltype* _temp4686= _temp4684.curr
+( yylsp_offset - 1); if( _temp4684.base == 0? 1:( _temp4686 < _temp4684.base? 1:
_temp4686 >= _temp4684.last_plus_one)){ _throw( Null_Exception);}* _temp4686;}).first_line,({
struct _tagged_ptr3 _temp4687= yyls; struct Cyc_Yyltype* _temp4689= _temp4687.curr
+ yylsp_offset; if( _temp4687.base == 0? 1:( _temp4689 < _temp4687.base? 1:
_temp4689 >= _temp4687.last_plus_one)){ _throw( Null_Exception);}* _temp4689;}).last_line));
_temp4681;}); _temp4680;}); break; case 176: _LL4679: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4691=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4691[ 0]=({ struct Cyc_Exp_tok_struct
_temp4692; _temp4692.tag= Cyc_Exp_tok; _temp4692.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4693=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4693[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4694; _temp4694.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4694.f1= 0; _temp4694.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4695= yyvs; void** _temp4697= _temp4695.curr +( yyvsp_offset - 1); if(
_temp4695.base == 0? 1:( _temp4697 < _temp4695.base? 1: _temp4697 >= _temp4695.last_plus_one)){
_throw( Null_Exception);}* _temp4697;}))); _temp4694;}); _temp4693;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4698= yyls; struct Cyc_Yyltype* _temp4700= _temp4698.curr
+( yylsp_offset - 2); if( _temp4698.base == 0? 1:( _temp4700 < _temp4698.base? 1:
_temp4700 >= _temp4698.last_plus_one)){ _throw( Null_Exception);}* _temp4700;}).first_line,({
struct _tagged_ptr3 _temp4701= yyls; struct Cyc_Yyltype* _temp4703= _temp4701.curr
+ yylsp_offset; if( _temp4701.base == 0? 1:( _temp4703 < _temp4701.base? 1:
_temp4703 >= _temp4701.last_plus_one)){ _throw( Null_Exception);}* _temp4703;}).last_line));
_temp4692;}); _temp4691;}); break; case 177: _LL4690: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4705=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4705[ 0]=({ struct Cyc_Exp_tok_struct
_temp4706; _temp4706.tag= Cyc_Exp_tok; _temp4706.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4707=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4707[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4708; _temp4708.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4708.f1= 0; _temp4708.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4709= yyvs; void** _temp4711= _temp4709.curr +( yyvsp_offset - 2); if(
_temp4709.base == 0? 1:( _temp4711 < _temp4709.base? 1: _temp4711 >= _temp4709.last_plus_one)){
_throw( Null_Exception);}* _temp4711;}))); _temp4708;}); _temp4707;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4712= yyls; struct Cyc_Yyltype* _temp4714= _temp4712.curr
+( yylsp_offset - 3); if( _temp4712.base == 0? 1:( _temp4714 < _temp4712.base? 1:
_temp4714 >= _temp4712.last_plus_one)){ _throw( Null_Exception);}* _temp4714;}).first_line,({
struct _tagged_ptr3 _temp4715= yyls; struct Cyc_Yyltype* _temp4717= _temp4715.curr
+ yylsp_offset; if( _temp4715.base == 0? 1:( _temp4717 < _temp4715.base? 1:
_temp4717 >= _temp4715.last_plus_one)){ _throw( Null_Exception);}* _temp4717;}).last_line));
_temp4706;}); _temp4705;}); break; case 178: _LL4704: { struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp4736=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp4736->f1=( void*) Cyc_Absyn_Loc_n;
_temp4736->f2=({ struct _tagged_string* _temp4737=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4737[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4738= yyvs; void** _temp4740= _temp4738.curr +(
yyvsp_offset - 5); if( _temp4738.base == 0? 1:( _temp4740 < _temp4738.base? 1:
_temp4740 >= _temp4738.last_plus_one)){ _throw( Null_Exception);}* _temp4740;}));
_temp4737;}); _temp4736;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4741= yyls; struct
Cyc_Yyltype* _temp4743= _temp4741.curr +( yylsp_offset - 5); if( _temp4741.base
== 0? 1:( _temp4743 < _temp4741.base? 1: _temp4743 >= _temp4741.last_plus_one)){
_throw( Null_Exception);}* _temp4743;}).first_line,({ struct _tagged_ptr3
_temp4744= yyls; struct Cyc_Yyltype* _temp4746= _temp4744.curr +( yylsp_offset -
5); if( _temp4744.base == 0? 1:( _temp4746 < _temp4744.base? 1: _temp4746 >=
_temp4744.last_plus_one)){ _throw( Null_Exception);}* _temp4746;}).last_line)));
vd->tq=({ struct Cyc_Absyn_Tqual _temp4719; _temp4719.q_const= 1; _temp4719.q_volatile=
0; _temp4719.q_restrict= 1; _temp4719;}); yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4720=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4720[ 0]=({ struct Cyc_Exp_tok_struct _temp4721; _temp4721.tag= Cyc_Exp_tok;
_temp4721.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Comprehension_e_struct*
_temp4722=( struct Cyc_Absyn_Comprehension_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Comprehension_e_struct)); _temp4722[ 0]=({ struct Cyc_Absyn_Comprehension_e_struct
_temp4723; _temp4723.tag= Cyc_Absyn_Comprehension_e; _temp4723.f1= vd; _temp4723.f2=
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4724= yyvs; void** _temp4726=
_temp4724.curr +( yyvsp_offset - 3); if( _temp4724.base == 0? 1:( _temp4726 <
_temp4724.base? 1: _temp4726 >= _temp4724.last_plus_one)){ _throw(
Null_Exception);}* _temp4726;})); _temp4723.f3= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4727= yyvs; void** _temp4729= _temp4727.curr +( yyvsp_offset -
1); if( _temp4727.base == 0? 1:( _temp4729 < _temp4727.base? 1: _temp4729 >=
_temp4727.last_plus_one)){ _throw( Null_Exception);}* _temp4729;})); _temp4723;});
_temp4722;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4730= yyls;
struct Cyc_Yyltype* _temp4732= _temp4730.curr +( yylsp_offset - 7); if(
_temp4730.base == 0? 1:( _temp4732 < _temp4730.base? 1: _temp4732 >= _temp4730.last_plus_one)){
_throw( Null_Exception);}* _temp4732;}).first_line,({ struct _tagged_ptr3
_temp4733= yyls; struct Cyc_Yyltype* _temp4735= _temp4733.curr + yylsp_offset;
if( _temp4733.base == 0? 1:( _temp4735 < _temp4733.base? 1: _temp4735 >=
_temp4733.last_plus_one)){ _throw( Null_Exception);}* _temp4735;}).last_line));
_temp4721;}); _temp4720;}); break;} case 179: _LL4718: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp4748=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4748[ 0]=({
struct Cyc_InitializerList_tok_struct _temp4749; _temp4749.tag= Cyc_InitializerList_tok;
_temp4749.f1=({ struct Cyc_List_List* _temp4750=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4750->hd=( void*)({ struct
_tuple18* _temp4751=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4751->f1= 0; _temp4751->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4752= yyvs; void** _temp4754= _temp4752.curr + yyvsp_offset; if( _temp4752.base
== 0? 1:( _temp4754 < _temp4752.base? 1: _temp4754 >= _temp4752.last_plus_one)){
_throw( Null_Exception);}* _temp4754;})); _temp4751;}); _temp4750->tl= 0;
_temp4750;}); _temp4749;}); _temp4748;}); break; case 180: _LL4747: yyval=( void*)({
struct Cyc_InitializerList_tok_struct* _temp4756=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4756[ 0]=({
struct Cyc_InitializerList_tok_struct _temp4757; _temp4757.tag= Cyc_InitializerList_tok;
_temp4757.f1=({ struct Cyc_List_List* _temp4758=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4758->hd=( void*)({ struct
_tuple18* _temp4759=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4759->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4763=
yyvs; void** _temp4765= _temp4763.curr +( yyvsp_offset - 1); if( _temp4763.base
== 0? 1:( _temp4765 < _temp4763.base? 1: _temp4765 >= _temp4763.last_plus_one)){
_throw( Null_Exception);}* _temp4765;})); _temp4759->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4760= yyvs; void** _temp4762= _temp4760.curr +
yyvsp_offset; if( _temp4760.base == 0? 1:( _temp4762 < _temp4760.base? 1:
_temp4762 >= _temp4760.last_plus_one)){ _throw( Null_Exception);}* _temp4762;}));
_temp4759;}); _temp4758->tl= 0; _temp4758;}); _temp4757;}); _temp4756;}); break;
case 181: _LL4755: yyval=( void*)({ struct Cyc_InitializerList_tok_struct*
_temp4767=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4767[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4768; _temp4768.tag= Cyc_InitializerList_tok; _temp4768.f1=({ struct Cyc_List_List*
_temp4769=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4769->hd=( void*)({ struct _tuple18* _temp4773=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4773->f1= 0; _temp4773->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4774= yyvs; void** _temp4776= _temp4774.curr +
yyvsp_offset; if( _temp4774.base == 0? 1:( _temp4776 < _temp4774.base? 1:
_temp4776 >= _temp4774.last_plus_one)){ _throw( Null_Exception);}* _temp4776;}));
_temp4773;}); _temp4769->tl= Cyc_yyget_InitializerList_tok(({ struct
_tagged_ptr2 _temp4770= yyvs; void** _temp4772= _temp4770.curr +( yyvsp_offset -
2); if( _temp4770.base == 0? 1:( _temp4772 < _temp4770.base? 1: _temp4772 >=
_temp4770.last_plus_one)){ _throw( Null_Exception);}* _temp4772;})); _temp4769;});
_temp4768;}); _temp4767;}); break; case 182: _LL4766: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp4778=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4778[ 0]=({
struct Cyc_InitializerList_tok_struct _temp4779; _temp4779.tag= Cyc_InitializerList_tok;
_temp4779.f1=({ struct Cyc_List_List* _temp4780=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4780->hd=( void*)({ struct
_tuple18* _temp4784=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4784->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4788=
yyvs; void** _temp4790= _temp4788.curr +( yyvsp_offset - 1); if( _temp4788.base
== 0? 1:( _temp4790 < _temp4788.base? 1: _temp4790 >= _temp4788.last_plus_one)){
_throw( Null_Exception);}* _temp4790;})); _temp4784->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4785= yyvs; void** _temp4787= _temp4785.curr +
yyvsp_offset; if( _temp4785.base == 0? 1:( _temp4787 < _temp4785.base? 1:
_temp4787 >= _temp4785.last_plus_one)){ _throw( Null_Exception);}* _temp4787;}));
_temp4784;}); _temp4780->tl= Cyc_yyget_InitializerList_tok(({ struct
_tagged_ptr2 _temp4781= yyvs; void** _temp4783= _temp4781.curr +( yyvsp_offset -
3); if( _temp4781.base == 0? 1:( _temp4783 < _temp4781.base? 1: _temp4783 >=
_temp4781.last_plus_one)){ _throw( Null_Exception);}* _temp4783;})); _temp4780;});
_temp4779;}); _temp4778;}); break; case 183: _LL4777: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp4792=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4792[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4793; _temp4793.tag= Cyc_DesignatorList_tok;
_temp4793.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4794= yyvs; void**
_temp4796= _temp4794.curr +( yyvsp_offset - 1); if( _temp4794.base == 0? 1:(
_temp4796 < _temp4794.base? 1: _temp4796 >= _temp4794.last_plus_one)){ _throw(
Null_Exception);}* _temp4796;}))); _temp4793;}); _temp4792;}); break; case 184:
_LL4791: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct* _temp4798=(
struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4798[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4799; _temp4799.tag=
Cyc_DesignatorList_tok; _temp4799.f1=({ struct Cyc_List_List* _temp4800=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4800->hd=( void*)
Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4801= yyvs; void**
_temp4803= _temp4801.curr + yyvsp_offset; if( _temp4801.base == 0? 1:( _temp4803
< _temp4801.base? 1: _temp4803 >= _temp4801.last_plus_one)){ _throw(
Null_Exception);}* _temp4803;})); _temp4800->tl= 0; _temp4800;}); _temp4799;});
_temp4798;}); break; case 185: _LL4797: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp4805=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4805[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4806; _temp4806.tag=
Cyc_DesignatorList_tok; _temp4806.f1=({ struct Cyc_List_List* _temp4807=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4807->hd=( void*)
Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4811= yyvs; void**
_temp4813= _temp4811.curr + yyvsp_offset; if( _temp4811.base == 0? 1:( _temp4813
< _temp4811.base? 1: _temp4813 >= _temp4811.last_plus_one)){ _throw(
Null_Exception);}* _temp4813;})); _temp4807->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4808= yyvs; void** _temp4810= _temp4808.curr +(
yyvsp_offset - 1); if( _temp4808.base == 0? 1:( _temp4810 < _temp4808.base? 1:
_temp4810 >= _temp4808.last_plus_one)){ _throw( Null_Exception);}* _temp4810;}));
_temp4807;}); _temp4806;}); _temp4805;}); break; case 186: _LL4804: yyval=( void*)({
struct Cyc_Designator_tok_struct* _temp4815=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp4815[ 0]=({ struct
Cyc_Designator_tok_struct _temp4816; _temp4816.tag= Cyc_Designator_tok;
_temp4816.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp4817=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4817[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp4818; _temp4818.tag=
Cyc_Absyn_ArrayElement; _temp4818.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4819= yyvs; void** _temp4821= _temp4819.curr +( yyvsp_offset - 1); if(
_temp4819.base == 0? 1:( _temp4821 < _temp4819.base? 1: _temp4821 >= _temp4819.last_plus_one)){
_throw( Null_Exception);}* _temp4821;})); _temp4818;}); _temp4817;})); _temp4816;});
_temp4815;}); break; case 187: _LL4814: yyval=( void*)({ struct Cyc_Designator_tok_struct*
_temp4823=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4823[ 0]=({ struct Cyc_Designator_tok_struct _temp4824; _temp4824.tag= Cyc_Designator_tok;
_temp4824.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4825=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4825[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4826; _temp4826.tag= Cyc_Absyn_FieldName;
_temp4826.f1=({ struct _tagged_string* _temp4827=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4827[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4828= yyvs; void** _temp4830= _temp4828.curr +
yyvsp_offset; if( _temp4828.base == 0? 1:( _temp4830 < _temp4828.base? 1:
_temp4830 >= _temp4828.last_plus_one)){ _throw( Null_Exception);}* _temp4830;}));
_temp4827;}); _temp4826;}); _temp4825;})); _temp4824;}); _temp4823;}); break;
case 188: _LL4822: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4850= yyvs; void** _temp4852= _temp4850.curr +
yyvsp_offset; if( _temp4850.base == 0? 1:( _temp4852 < _temp4850.base? 1:
_temp4852 >= _temp4850.last_plus_one)){ _throw( Null_Exception);}* _temp4852;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4853= yyls; struct Cyc_Yyltype*
_temp4855= _temp4853.curr + yylsp_offset; if( _temp4853.base == 0? 1:( _temp4855
< _temp4853.base? 1: _temp4855 >= _temp4853.last_plus_one)){ _throw(
Null_Exception);}* _temp4855;}).first_line,({ struct _tagged_ptr3 _temp4856=
yyls; struct Cyc_Yyltype* _temp4858= _temp4856.curr + yylsp_offset; if(
_temp4856.base == 0? 1:( _temp4858 < _temp4856.base? 1: _temp4858 >= _temp4856.last_plus_one)){
_throw( Null_Exception);}* _temp4858;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4847= yyvs; void**
_temp4849= _temp4847.curr + yyvsp_offset; if( _temp4847.base == 0? 1:( _temp4849
< _temp4847.base? 1: _temp4849 >= _temp4847.last_plus_one)){ _throw(
Null_Exception);}* _temp4849;}))).f3; if( atts != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp4832=( char*)"ignoring attributes in type"; struct
_tagged_string _temp4833; _temp4833.curr= _temp4832; _temp4833.base= _temp4832;
_temp4833.last_plus_one= _temp4832 + 28; _temp4833;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4834= yyls; struct Cyc_Yyltype* _temp4836= _temp4834.curr
+ yylsp_offset; if( _temp4834.base == 0? 1:( _temp4836 < _temp4834.base? 1:
_temp4836 >= _temp4834.last_plus_one)){ _throw( Null_Exception);}* _temp4836;}).first_line,({
struct _tagged_ptr3 _temp4837= yyls; struct Cyc_Yyltype* _temp4839= _temp4837.curr
+ yylsp_offset; if( _temp4837.base == 0? 1:( _temp4839 < _temp4837.base? 1:
_temp4839 >= _temp4837.last_plus_one)){ _throw( Null_Exception);}* _temp4839;}).last_line));}{
struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4844= yyvs; void** _temp4846= _temp4844.curr + yyvsp_offset; if( _temp4844.base
== 0? 1:( _temp4846 < _temp4844.base? 1: _temp4846 >= _temp4844.last_plus_one)){
_throw( Null_Exception);}* _temp4846;}))).f1; yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp4840=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4840[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4841; _temp4841.tag= Cyc_ParamDecl_tok;
_temp4841.f1=({ struct _tuple2* _temp4842=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp4842[ 0]=({ struct _tuple2 _temp4843; _temp4843.f1= 0;
_temp4843.f2= tq; _temp4843.f3= t; _temp4843;}); _temp4842;}); _temp4841;});
_temp4840;}); break;}} case 189: _LL4831: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4888= yyvs; void**
_temp4890= _temp4888.curr +( yyvsp_offset - 1); if( _temp4888.base == 0? 1:(
_temp4890 < _temp4888.base? 1: _temp4890 >= _temp4888.last_plus_one)){ _throw(
Null_Exception);}* _temp4890;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4891= yyls; struct Cyc_Yyltype* _temp4893= _temp4891.curr +(
yylsp_offset - 1); if( _temp4891.base == 0? 1:( _temp4893 < _temp4891.base? 1:
_temp4893 >= _temp4891.last_plus_one)){ _throw( Null_Exception);}* _temp4893;}).first_line,({
struct _tagged_ptr3 _temp4894= yyls; struct Cyc_Yyltype* _temp4896= _temp4894.curr
+( yylsp_offset - 1); if( _temp4894.base == 0? 1:( _temp4896 < _temp4894.base? 1:
_temp4896 >= _temp4894.last_plus_one)){ _throw( Null_Exception);}* _temp4896;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4885= yyvs; void** _temp4887= _temp4885.curr +( yyvsp_offset - 1); if(
_temp4885.base == 0? 1:( _temp4887 < _temp4885.base? 1: _temp4887 >= _temp4885.last_plus_one)){
_throw( Null_Exception);}* _temp4887;}))).f3; struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4882= yyvs; void** _temp4884= _temp4882.curr +(
yyvsp_offset - 1); if( _temp4882.base == 0? 1:( _temp4884 < _temp4882.base? 1:
_temp4884 >= _temp4882.last_plus_one)){ _throw( Null_Exception);}* _temp4884;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4879= yyvs; void** _temp4881= _temp4879.curr + yyvsp_offset;
if( _temp4879.base == 0? 1:( _temp4881 < _temp4879.base? 1: _temp4881 >=
_temp4879.last_plus_one)){ _throw( Null_Exception);}* _temp4881;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4860=( char*)"bad type params, ignoring";
struct _tagged_string _temp4861; _temp4861.curr= _temp4860; _temp4861.base=
_temp4860; _temp4861.last_plus_one= _temp4860 + 26; _temp4861;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4862= yyls; struct Cyc_Yyltype* _temp4864= _temp4862.curr
+ yylsp_offset; if( _temp4862.base == 0? 1:( _temp4864 < _temp4862.base? 1:
_temp4864 >= _temp4862.last_plus_one)){ _throw( Null_Exception);}* _temp4864;}).first_line,({
struct _tagged_ptr3 _temp4865= yyls; struct Cyc_Yyltype* _temp4867= _temp4865.curr
+ yylsp_offset; if( _temp4865.base == 0? 1:( _temp4867 < _temp4865.base? 1:
_temp4867 >= _temp4865.last_plus_one)){ _throw( Null_Exception);}* _temp4867;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4868=(
char*)"bad specifiers, ignoring"; struct _tagged_string _temp4869; _temp4869.curr=
_temp4868; _temp4869.base= _temp4868; _temp4869.last_plus_one= _temp4868 + 25;
_temp4869;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4870= yyls;
struct Cyc_Yyltype* _temp4872= _temp4870.curr + yylsp_offset; if( _temp4870.base
== 0? 1:( _temp4872 < _temp4870.base? 1: _temp4872 >= _temp4870.last_plus_one)){
_throw( Null_Exception);}* _temp4872;}).first_line,({ struct _tagged_ptr3
_temp4873= yyls; struct Cyc_Yyltype* _temp4875= _temp4873.curr + yylsp_offset;
if( _temp4873.base == 0? 1:( _temp4875 < _temp4873.base? 1: _temp4875 >=
_temp4873.last_plus_one)){ _throw( Null_Exception);}* _temp4875;}).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4876=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4876[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4877; _temp4877.tag= Cyc_ParamDecl_tok; _temp4877.f1=({
struct _tuple2* _temp4878=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4878->f1= 0; _temp4878->f2= t_info.f1; _temp4878->f3= t_info.f2; _temp4878;});
_temp4877;}); _temp4876;}); break;} case 190: _LL4859: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp4898=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct)); _temp4898[ 0]=({ struct Cyc_Type_tok_struct
_temp4899; _temp4899.tag= Cyc_Type_tok; _temp4899.f1=( void*)(* Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp4900= yyvs; void** _temp4902= _temp4900.curr +
yyvsp_offset; if( _temp4900.base == 0? 1:( _temp4902 < _temp4900.base? 1:
_temp4902 >= _temp4900.last_plus_one)){ _throw( Null_Exception);}* _temp4902;}))).f3;
_temp4899;}); _temp4898;}); break; case 191: _LL4897: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp4904=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct)); _temp4904[ 0]=({ struct Cyc_Type_tok_struct
_temp4905; _temp4905.tag= Cyc_Type_tok; _temp4905.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp4906=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4906[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4907; _temp4907.tag= Cyc_Absyn_JoinEff; _temp4907.f1=
0; _temp4907;}); _temp4906;})); _temp4905;}); _temp4904;}); break; case 192:
_LL4903: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp4909=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp4909[ 0]=({ struct Cyc_Type_tok_struct
_temp4910; _temp4910.tag= Cyc_Type_tok; _temp4910.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp4911=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4911[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4912; _temp4912.tag= Cyc_Absyn_JoinEff; _temp4912.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4913= yyvs; void** _temp4915=
_temp4913.curr +( yyvsp_offset - 1); if( _temp4913.base == 0? 1:( _temp4915 <
_temp4913.base? 1: _temp4915 >= _temp4913.last_plus_one)){ _throw(
Null_Exception);}* _temp4915;})); _temp4912;}); _temp4911;})); _temp4910;});
_temp4909;}); break; case 193: _LL4908: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp4917=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4917[ 0]=({ struct Cyc_Type_tok_struct _temp4918; _temp4918.tag= Cyc_Type_tok;
_temp4918.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4919=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4919[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4920; _temp4920.tag= Cyc_Absyn_JoinEff;
_temp4920.f1=({ struct Cyc_List_List* _temp4921=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4921->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4925= yyvs; void** _temp4927= _temp4925.curr +(
yyvsp_offset - 2); if( _temp4925.base == 0? 1:( _temp4927 < _temp4925.base? 1:
_temp4927 >= _temp4925.last_plus_one)){ _throw( Null_Exception);}* _temp4927;}));
_temp4921->tl= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4922= yyvs;
void** _temp4924= _temp4922.curr + yyvsp_offset; if( _temp4922.base == 0? 1:(
_temp4924 < _temp4922.base? 1: _temp4924 >= _temp4922.last_plus_one)){ _throw(
Null_Exception);}* _temp4924;})); _temp4921;}); _temp4920;}); _temp4919;}));
_temp4918;}); _temp4917;}); break; case 194: _LL4916: yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp4929=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp4929[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4930; _temp4930.tag= Cyc_TypeList_tok; _temp4930.f1=({ struct Cyc_List_List*
_temp4931=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4931->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4932= yyvs;
void** _temp4934= _temp4932.curr + yyvsp_offset; if( _temp4932.base == 0? 1:(
_temp4934 < _temp4932.base? 1: _temp4934 >= _temp4932.last_plus_one)){ _throw(
Null_Exception);}* _temp4934;})); _temp4931->tl= 0; _temp4931;}); _temp4930;});
_temp4929;}); break; case 195: _LL4928: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4936=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4936[ 0]=({ struct Cyc_TypeList_tok_struct _temp4937; _temp4937.tag= Cyc_TypeList_tok;
_temp4937.f1=({ struct Cyc_List_List* _temp4938=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4938->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4942= yyvs; void** _temp4944= _temp4942.curr +
yyvsp_offset; if( _temp4942.base == 0? 1:( _temp4944 < _temp4942.base? 1:
_temp4944 >= _temp4942.last_plus_one)){ _throw( Null_Exception);}* _temp4944;}));
_temp4938->tl= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4939= yyvs;
void** _temp4941= _temp4939.curr +( yyvsp_offset - 2); if( _temp4939.base == 0?
1:( _temp4941 < _temp4939.base? 1: _temp4941 >= _temp4939.last_plus_one)){
_throw( Null_Exception);}* _temp4941;})); _temp4938;}); _temp4937;}); _temp4936;});
break; case 196: _LL4935: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4946=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4946[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4947; _temp4947.tag= Cyc_AbstractDeclarator_tok; _temp4947.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4948=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4948->tms= Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4949=
yyvs; void** _temp4951= _temp4949.curr + yyvsp_offset; if( _temp4949.base == 0?
1:( _temp4951 < _temp4949.base? 1: _temp4951 >= _temp4949.last_plus_one)){
_throw( Null_Exception);}* _temp4951;})); _temp4948;}); _temp4947;}); _temp4946;});
break; case 197: _LL4945: yyval=({ struct _tagged_ptr2 _temp4953= yyvs; void**
_temp4955= _temp4953.curr + yyvsp_offset; if( _temp4953.base == 0? 1:( _temp4955
< _temp4953.base? 1: _temp4955 >= _temp4953.last_plus_one)){ _throw(
Null_Exception);}* _temp4955;}); break; case 198: _LL4952: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp4957=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4957[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4958; _temp4958.tag= Cyc_AbstractDeclarator_tok;
_temp4958.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4959=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4959->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4960= yyvs; void**
_temp4962= _temp4960.curr +( yyvsp_offset - 1); if( _temp4960.base == 0? 1:(
_temp4962 < _temp4960.base? 1: _temp4962 >= _temp4960.last_plus_one)){ _throw(
Null_Exception);}* _temp4962;})),( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4963= yyvs; void** _temp4965= _temp4963.curr + yyvsp_offset;
if( _temp4963.base == 0? 1:( _temp4965 < _temp4963.base? 1: _temp4965 >=
_temp4963.last_plus_one)){ _throw( Null_Exception);}* _temp4965;})))->tms);
_temp4959;}); _temp4958;}); _temp4957;}); break; case 199: _LL4956: yyval=({
struct _tagged_ptr2 _temp4967= yyvs; void** _temp4969= _temp4967.curr +(
yyvsp_offset - 1); if( _temp4967.base == 0? 1:( _temp4969 < _temp4967.base? 1:
_temp4969 >= _temp4967.last_plus_one)){ _throw( Null_Exception);}* _temp4969;});
break; case 200: _LL4966: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4971=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4971[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4972; _temp4972.tag= Cyc_AbstractDeclarator_tok; _temp4972.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4973=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4973->tms=({ struct Cyc_List_List* _temp4974=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4974->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4974->tl= 0; _temp4974;}); _temp4973;}); _temp4972;}); _temp4971;}); break;
case 201: _LL4970: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4976=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4976[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4977; _temp4977.tag= Cyc_AbstractDeclarator_tok; _temp4977.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4978=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4978->tms=({ struct Cyc_List_List* _temp4979=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4979->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4979->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4980= yyvs; void** _temp4982= _temp4980.curr +( yyvsp_offset - 2); if(
_temp4980.base == 0? 1:( _temp4982 < _temp4980.base? 1: _temp4982 >= _temp4980.last_plus_one)){
_throw( Null_Exception);}* _temp4982;})))->tms; _temp4979;}); _temp4978;});
_temp4977;}); _temp4976;}); break; case 202: _LL4975: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4984=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4984[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4985; _temp4985.tag= Cyc_AbstractDeclarator_tok;
_temp4985.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4986=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4986->tms=({
struct Cyc_List_List* _temp4987=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4987->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4988=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4988[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4989; _temp4989.tag= Cyc_Absyn_ConstArray_mod; _temp4989.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4990= yyvs; void** _temp4992= _temp4990.curr +(
yyvsp_offset - 1); if( _temp4990.base == 0? 1:( _temp4992 < _temp4990.base? 1:
_temp4992 >= _temp4990.last_plus_one)){ _throw( Null_Exception);}* _temp4992;}));
_temp4989;}); _temp4988;})); _temp4987->tl= 0; _temp4987;}); _temp4986;});
_temp4985;}); _temp4984;}); break; case 203: _LL4983: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4994=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4994[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4995; _temp4995.tag= Cyc_AbstractDeclarator_tok;
_temp4995.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4996=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4996->tms=({
struct Cyc_List_List* _temp4997=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4997->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp5001=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp5001[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp5002; _temp5002.tag= Cyc_Absyn_ConstArray_mod; _temp5002.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5003= yyvs; void** _temp5005= _temp5003.curr +(
yyvsp_offset - 1); if( _temp5003.base == 0? 1:( _temp5005 < _temp5003.base? 1:
_temp5005 >= _temp5003.last_plus_one)){ _throw( Null_Exception);}* _temp5005;}));
_temp5002;}); _temp5001;})); _temp4997->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4998= yyvs; void** _temp5000= _temp4998.curr +(
yyvsp_offset - 3); if( _temp4998.base == 0? 1:( _temp5000 < _temp4998.base? 1:
_temp5000 >= _temp4998.last_plus_one)){ _throw( Null_Exception);}* _temp5000;})))->tms;
_temp4997;}); _temp4996;}); _temp4995;}); _temp4994;}); break; case 204: _LL4993:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5007=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5007[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5008; _temp5008.tag=
Cyc_AbstractDeclarator_tok; _temp5008.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5009=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5009->tms=({ struct Cyc_List_List* _temp5010=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5010->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5011=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5011[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5012; _temp5012.tag= Cyc_Absyn_Function_mod;
_temp5012.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5013=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5013[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5014; _temp5014.tag= Cyc_Absyn_WithTypes;
_temp5014.f1= 0; _temp5014.f2= 0; _temp5014.f3= 0; _temp5014;}); _temp5013;}));
_temp5012;}); _temp5011;})); _temp5010->tl= 0; _temp5010;}); _temp5009;});
_temp5008;}); _temp5007;}); break; case 205: _LL5006: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5016=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5016[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5017; _temp5017.tag= Cyc_AbstractDeclarator_tok;
_temp5017.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5018=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5018->tms=({
struct Cyc_List_List* _temp5019=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5019->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5020=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5020[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5021; _temp5021.tag=
Cyc_Absyn_Function_mod; _temp5021.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5022=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5022[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5023; _temp5023.tag= Cyc_Absyn_WithTypes;
_temp5023.f1= 0; _temp5023.f2= 0; _temp5023.f3=({ struct Cyc_Core_Opt* _temp5024=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5024->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp5025=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp5025[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp5026; _temp5026.tag= Cyc_Absyn_JoinEff; _temp5026.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5027= yyvs; void** _temp5029=
_temp5027.curr +( yyvsp_offset - 1); if( _temp5027.base == 0? 1:( _temp5029 <
_temp5027.base? 1: _temp5029 >= _temp5027.last_plus_one)){ _throw(
Null_Exception);}* _temp5029;})); _temp5026;}); _temp5025;})); _temp5024;});
_temp5023;}); _temp5022;})); _temp5021;}); _temp5020;})); _temp5019->tl= 0;
_temp5019;}); _temp5018;}); _temp5017;}); _temp5016;}); break; case 206: _LL5015: {
struct _tuple16 _temp5036; struct Cyc_Core_Opt* _temp5037; int _temp5039; struct
Cyc_List_List* _temp5041; struct _tuple16* _temp5034= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp5031= yyvs; void** _temp5033= _temp5031.curr +(
yyvsp_offset - 1); if( _temp5031.base == 0? 1:( _temp5033 < _temp5031.base? 1:
_temp5033 >= _temp5031.last_plus_one)){ _throw( Null_Exception);}* _temp5033;}));
_temp5036=* _temp5034; _LL5042: _temp5041= _temp5036.f1; goto _LL5040; _LL5040:
_temp5039= _temp5036.f2; goto _LL5038; _LL5038: _temp5037= _temp5036.f3; goto
_LL5035; _LL5035: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5043=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5043[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5044; _temp5044.tag= Cyc_AbstractDeclarator_tok; _temp5044.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5045=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5045->tms=({ struct Cyc_List_List* _temp5046=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5046->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5047=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5047[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5048; _temp5048.tag= Cyc_Absyn_Function_mod;
_temp5048.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5049=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5049[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5050; _temp5050.tag= Cyc_Absyn_WithTypes;
_temp5050.f1= _temp5041; _temp5050.f2= _temp5039; _temp5050.f3= _temp5037;
_temp5050;}); _temp5049;})); _temp5048;}); _temp5047;})); _temp5046->tl= 0;
_temp5046;}); _temp5045;}); _temp5044;}); _temp5043;}); break;} case 207:
_LL5030: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5052=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5052[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5053; _temp5053.tag=
Cyc_AbstractDeclarator_tok; _temp5053.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5054=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5054->tms=({ struct Cyc_List_List* _temp5055=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5055->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5059=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5059[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5060; _temp5060.tag= Cyc_Absyn_Function_mod;
_temp5060.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5061=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5061[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5062; _temp5062.tag= Cyc_Absyn_WithTypes;
_temp5062.f1= 0; _temp5062.f2= 0; _temp5062.f3= 0; _temp5062;}); _temp5061;}));
_temp5060;}); _temp5059;})); _temp5055->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5056= yyvs; void** _temp5058= _temp5056.curr +(
yyvsp_offset - 2); if( _temp5056.base == 0? 1:( _temp5058 < _temp5056.base? 1:
_temp5058 >= _temp5056.last_plus_one)){ _throw( Null_Exception);}* _temp5058;})))->tms;
_temp5055;}); _temp5054;}); _temp5053;}); _temp5052;}); break; case 208: _LL5051: {
struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt* _temp5075=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5075->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp5076=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp5076[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp5077; _temp5077.tag= Cyc_Absyn_JoinEff; _temp5077.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5078= yyvs; void** _temp5080=
_temp5078.curr +( yyvsp_offset - 1); if( _temp5078.base == 0? 1:( _temp5080 <
_temp5078.base? 1: _temp5080 >= _temp5078.last_plus_one)){ _throw(
Null_Exception);}* _temp5080;})); _temp5077;}); _temp5076;})); _temp5075;});
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5064=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5064[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5065; _temp5065.tag=
Cyc_AbstractDeclarator_tok; _temp5065.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5066=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5066->tms=({ struct Cyc_List_List* _temp5067=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5067->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5071=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5071[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5072; _temp5072.tag= Cyc_Absyn_Function_mod;
_temp5072.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5073=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5073[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5074; _temp5074.tag= Cyc_Absyn_WithTypes;
_temp5074.f1= 0; _temp5074.f2= 0; _temp5074.f3= eff; _temp5074;}); _temp5073;}));
_temp5072;}); _temp5071;})); _temp5067->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5068= yyvs; void** _temp5070= _temp5068.curr +(
yyvsp_offset - 4); if( _temp5068.base == 0? 1:( _temp5070 < _temp5068.base? 1:
_temp5070 >= _temp5068.last_plus_one)){ _throw( Null_Exception);}* _temp5070;})))->tms;
_temp5067;}); _temp5066;}); _temp5065;}); _temp5064;}); break;} case 209:
_LL5063: { struct _tuple16 _temp5087; struct Cyc_Core_Opt* _temp5088; int
_temp5090; struct Cyc_List_List* _temp5092; struct _tuple16* _temp5085= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp5082= yyvs; void** _temp5084= _temp5082.curr +(
yyvsp_offset - 1); if( _temp5082.base == 0? 1:( _temp5084 < _temp5082.base? 1:
_temp5084 >= _temp5082.last_plus_one)){ _throw( Null_Exception);}* _temp5084;}));
_temp5087=* _temp5085; _LL5093: _temp5092= _temp5087.f1; goto _LL5091; _LL5091:
_temp5090= _temp5087.f2; goto _LL5089; _LL5089: _temp5088= _temp5087.f3; goto
_LL5086; _LL5086: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5094=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5094[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5095; _temp5095.tag= Cyc_AbstractDeclarator_tok; _temp5095.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5096=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5096->tms=({ struct Cyc_List_List* _temp5097=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5097->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5101=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5101[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5102; _temp5102.tag= Cyc_Absyn_Function_mod;
_temp5102.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5103=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5103[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5104; _temp5104.tag= Cyc_Absyn_WithTypes;
_temp5104.f1= _temp5092; _temp5104.f2= _temp5090; _temp5104.f3= _temp5088;
_temp5104;}); _temp5103;})); _temp5102;}); _temp5101;})); _temp5097->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5098= yyvs; void** _temp5100= _temp5098.curr +(
yyvsp_offset - 3); if( _temp5098.base == 0? 1:( _temp5100 < _temp5098.base? 1:
_temp5100 >= _temp5098.last_plus_one)){ _throw( Null_Exception);}* _temp5100;})))->tms;
_temp5097;}); _temp5096;}); _temp5095;}); _temp5094;}); break;} case 210:
_LL5081: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5121= yyls; struct Cyc_Yyltype* _temp5123= _temp5121.curr
+( yylsp_offset - 2); if( _temp5121.base == 0? 1:( _temp5123 < _temp5121.base? 1:
_temp5123 >= _temp5121.last_plus_one)){ _throw( Null_Exception);}* _temp5123;}).first_line,({
struct _tagged_ptr3 _temp5124= yyls; struct Cyc_Yyltype* _temp5126= _temp5124.curr
+ yylsp_offset; if( _temp5124.base == 0? 1:( _temp5126 < _temp5124.base? 1:
_temp5126 >= _temp5124.last_plus_one)){ _throw( Null_Exception);}* _temp5126;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5127= yyvs; void** _temp5129= _temp5127.curr +(
yyvsp_offset - 1); if( _temp5127.base == 0? 1:( _temp5129 < _temp5127.base? 1:
_temp5129 >= _temp5127.last_plus_one)){ _throw( Null_Exception);}* _temp5129;}))));
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5106=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5106[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5107; _temp5107.tag=
Cyc_AbstractDeclarator_tok; _temp5107.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5108=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5108->tms=({ struct Cyc_List_List* _temp5109=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5109->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp5113=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp5113[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp5114; _temp5114.tag= Cyc_Absyn_TypeParams_mod;
_temp5114.f1= ts; _temp5114.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5115= yyls; struct Cyc_Yyltype* _temp5117= _temp5115.curr +(
yylsp_offset - 2); if( _temp5115.base == 0? 1:( _temp5117 < _temp5115.base? 1:
_temp5117 >= _temp5115.last_plus_one)){ _throw( Null_Exception);}* _temp5117;}).first_line,({
struct _tagged_ptr3 _temp5118= yyls; struct Cyc_Yyltype* _temp5120= _temp5118.curr
+ yylsp_offset; if( _temp5118.base == 0? 1:( _temp5120 < _temp5118.base? 1:
_temp5120 >= _temp5118.last_plus_one)){ _throw( Null_Exception);}* _temp5120;}).last_line);
_temp5114.f3= 0; _temp5114;}); _temp5113;})); _temp5109->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5110= yyvs; void** _temp5112= _temp5110.curr +(
yyvsp_offset - 3); if( _temp5110.base == 0? 1:( _temp5112 < _temp5110.base? 1:
_temp5112 >= _temp5110.last_plus_one)){ _throw( Null_Exception);}* _temp5112;})))->tms;
_temp5109;}); _temp5108;}); _temp5107;}); _temp5106;}); break;} case 211:
_LL5105:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp5131= Cyc_Parse_lbuf;
if( _temp5131 == 0){ _throw( Null_Exception);} _temp5131->v;}))->lex_curr_pos -=
1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5147= yyls; struct Cyc_Yyltype* _temp5149= _temp5147.curr
+( yylsp_offset - 2); if( _temp5147.base == 0? 1:( _temp5149 < _temp5147.base? 1:
_temp5149 >= _temp5147.last_plus_one)){ _throw( Null_Exception);}* _temp5149;}).first_line,({
struct _tagged_ptr3 _temp5150= yyls; struct Cyc_Yyltype* _temp5152= _temp5150.curr
+ yylsp_offset; if( _temp5150.base == 0? 1:( _temp5152 < _temp5150.base? 1:
_temp5152 >= _temp5150.last_plus_one)){ _throw( Null_Exception);}* _temp5152;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5153= yyvs; void** _temp5155= _temp5153.curr +(
yyvsp_offset - 1); if( _temp5153.base == 0? 1:( _temp5155 < _temp5153.base? 1:
_temp5155 >= _temp5153.last_plus_one)){ _throw( Null_Exception);}* _temp5155;}))));
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5132=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5132[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5133; _temp5133.tag=
Cyc_AbstractDeclarator_tok; _temp5133.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5134=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5134->tms=({ struct Cyc_List_List* _temp5135=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5135->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp5139=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp5139[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp5140; _temp5140.tag= Cyc_Absyn_TypeParams_mod;
_temp5140.f1= ts; _temp5140.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5141= yyls; struct Cyc_Yyltype* _temp5143= _temp5141.curr +(
yylsp_offset - 2); if( _temp5141.base == 0? 1:( _temp5143 < _temp5141.base? 1:
_temp5143 >= _temp5141.last_plus_one)){ _throw( Null_Exception);}* _temp5143;}).first_line,({
struct _tagged_ptr3 _temp5144= yyls; struct Cyc_Yyltype* _temp5146= _temp5144.curr
+ yylsp_offset; if( _temp5144.base == 0? 1:( _temp5146 < _temp5144.base? 1:
_temp5146 >= _temp5144.last_plus_one)){ _throw( Null_Exception);}* _temp5146;}).last_line);
_temp5140.f3= 0; _temp5140;}); _temp5139;})); _temp5135->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5136= yyvs; void** _temp5138= _temp5136.curr +(
yyvsp_offset - 3); if( _temp5136.base == 0? 1:( _temp5138 < _temp5136.base? 1:
_temp5138 >= _temp5136.last_plus_one)){ _throw( Null_Exception);}* _temp5138;})))->tms;
_temp5135;}); _temp5134;}); _temp5133;}); _temp5132;}); break;} case 212:
_LL5130: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5157=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5157[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5158; _temp5158.tag=
Cyc_AbstractDeclarator_tok; _temp5158.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5159=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5159->tms=({ struct Cyc_List_List* _temp5160=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5160->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp5164=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp5164[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp5165; _temp5165.tag= Cyc_Absyn_Attributes_mod;
_temp5165.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5166= yyls;
struct Cyc_Yyltype* _temp5168= _temp5166.curr + yylsp_offset; if( _temp5166.base
== 0? 1:( _temp5168 < _temp5166.base? 1: _temp5168 >= _temp5166.last_plus_one)){
_throw( Null_Exception);}* _temp5168;}).first_line,({ struct _tagged_ptr3
_temp5169= yyls; struct Cyc_Yyltype* _temp5171= _temp5169.curr + yylsp_offset;
if( _temp5169.base == 0? 1:( _temp5171 < _temp5169.base? 1: _temp5171 >=
_temp5169.last_plus_one)){ _throw( Null_Exception);}* _temp5171;}).last_line);
_temp5165.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp5172= yyvs;
void** _temp5174= _temp5172.curr + yyvsp_offset; if( _temp5172.base == 0? 1:(
_temp5174 < _temp5172.base? 1: _temp5174 >= _temp5172.last_plus_one)){ _throw(
Null_Exception);}* _temp5174;})); _temp5165;}); _temp5164;})); _temp5160->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp5161= yyvs; void**
_temp5163= _temp5161.curr +( yyvsp_offset - 1); if( _temp5161.base == 0? 1:(
_temp5163 < _temp5161.base? 1: _temp5163 >= _temp5161.last_plus_one)){ _throw(
Null_Exception);}* _temp5163;})))->tms; _temp5160;}); _temp5159;}); _temp5158;});
_temp5157;}); break; case 213: _LL5156: yyval=({ struct _tagged_ptr2 _temp5176=
yyvs; void** _temp5178= _temp5176.curr + yyvsp_offset; if( _temp5176.base == 0?
1:( _temp5178 < _temp5176.base? 1: _temp5178 >= _temp5176.last_plus_one)){
_throw( Null_Exception);}* _temp5178;}); break; case 214: _LL5175: yyval=({
struct _tagged_ptr2 _temp5180= yyvs; void** _temp5182= _temp5180.curr +
yyvsp_offset; if( _temp5180.base == 0? 1:( _temp5182 < _temp5180.base? 1:
_temp5182 >= _temp5180.last_plus_one)){ _throw( Null_Exception);}* _temp5182;});
break; case 215: _LL5179: yyval=({ struct _tagged_ptr2 _temp5184= yyvs; void**
_temp5186= _temp5184.curr + yyvsp_offset; if( _temp5184.base == 0? 1:( _temp5186
< _temp5184.base? 1: _temp5186 >= _temp5184.last_plus_one)){ _throw(
Null_Exception);}* _temp5186;}); break; case 216: _LL5183: yyval=({ struct
_tagged_ptr2 _temp5188= yyvs; void** _temp5190= _temp5188.curr + yyvsp_offset;
if( _temp5188.base == 0? 1:( _temp5190 < _temp5188.base? 1: _temp5190 >=
_temp5188.last_plus_one)){ _throw( Null_Exception);}* _temp5190;}); break; case
217: _LL5187: yyval=({ struct _tagged_ptr2 _temp5192= yyvs; void** _temp5194=
_temp5192.curr + yyvsp_offset; if( _temp5192.base == 0? 1:( _temp5194 <
_temp5192.base? 1: _temp5194 >= _temp5192.last_plus_one)){ _throw(
Null_Exception);}* _temp5194;}); break; case 218: _LL5191: yyval=({ struct
_tagged_ptr2 _temp5196= yyvs; void** _temp5198= _temp5196.curr + yyvsp_offset;
if( _temp5196.base == 0? 1:( _temp5198 < _temp5196.base? 1: _temp5198 >=
_temp5196.last_plus_one)){ _throw( Null_Exception);}* _temp5198;}); break; case
219: _LL5195: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5200= yyvs; void** _temp5202= _temp5200.curr +( yyvsp_offset -
3); if( _temp5200.base == 0? 1:( _temp5202 < _temp5200.base? 1: _temp5202 >=
_temp5200.last_plus_one)){ _throw( Null_Exception);}* _temp5202;})),( struct
_tagged_string)({ char* _temp5203=( char*)"`H"; struct _tagged_string _temp5204;
_temp5204.curr= _temp5203; _temp5204.base= _temp5203; _temp5204.last_plus_one=
_temp5203 + 3; _temp5204;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp5205=( char*)"bad occurrence of heap region `H"; struct
_tagged_string _temp5206; _temp5206.curr= _temp5205; _temp5206.base= _temp5205;
_temp5206.last_plus_one= _temp5205 + 33; _temp5206;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5207= yyls; struct Cyc_Yyltype* _temp5209= _temp5207.curr
+( yylsp_offset - 3); if( _temp5207.base == 0? 1:( _temp5209 < _temp5207.base? 1:
_temp5209 >= _temp5207.last_plus_one)){ _throw( Null_Exception);}* _temp5209;}).first_line,({
struct _tagged_ptr3 _temp5210= yyls; struct Cyc_Yyltype* _temp5212= _temp5210.curr
+( yylsp_offset - 3); if( _temp5210.base == 0? 1:( _temp5212 < _temp5210.base? 1:
_temp5212 >= _temp5210.last_plus_one)){ _throw( Null_Exception);}* _temp5212;}).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5235=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5235->name=({ struct
_tagged_string* _temp5236=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp5236[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp5237= yyvs; void** _temp5239= _temp5237.curr +( yyvsp_offset - 3); if(
_temp5237.base == 0? 1:( _temp5239 < _temp5237.base? 1: _temp5239 >= _temp5237.last_plus_one)){
_throw( Null_Exception);}* _temp5239;})); _temp5236;}); _temp5235->kind=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind);
_temp5235;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp5233=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp5233[ 0]=({ struct Cyc_Absyn_VarType_struct _temp5234; _temp5234.tag= Cyc_Absyn_VarType;
_temp5234.f1= tv; _temp5234;}); _temp5233;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5213=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5213[ 0]=({ struct Cyc_Stmt_tok_struct _temp5214; _temp5214.tag= Cyc_Stmt_tok;
_temp5214.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp5215=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp5215[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp5216; _temp5216.tag= Cyc_Absyn_Region_s;
_temp5216.f1= tv; _temp5216.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5217=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5217->f1=(
void*) Cyc_Absyn_Loc_n; _temp5217->f2=({ struct _tagged_string* _temp5218=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp5218[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5219= yyvs; void** _temp5221=
_temp5219.curr +( yyvsp_offset - 1); if( _temp5219.base == 0? 1:( _temp5221 <
_temp5219.base? 1: _temp5221 >= _temp5219.last_plus_one)){ _throw(
Null_Exception);}* _temp5221;})); _temp5218;}); _temp5217;}),( void*)({ struct
Cyc_Absyn_RgnHandleType_struct* _temp5222=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp5222[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp5223; _temp5223.tag= Cyc_Absyn_RgnHandleType;
_temp5223.f1=( void*) t; _temp5223;}); _temp5222;}), 0); _temp5216.f3= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5224= yyvs; void** _temp5226= _temp5224.curr +
yyvsp_offset; if( _temp5224.base == 0? 1:( _temp5226 < _temp5224.base? 1:
_temp5226 >= _temp5224.last_plus_one)){ _throw( Null_Exception);}* _temp5226;}));
_temp5216;}); _temp5215;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5227= yyls; struct Cyc_Yyltype* _temp5229= _temp5227.curr +( yylsp_offset -
5); if( _temp5227.base == 0? 1:( _temp5229 < _temp5227.base? 1: _temp5229 >=
_temp5227.last_plus_one)){ _throw( Null_Exception);}* _temp5229;}).first_line,({
struct _tagged_ptr3 _temp5230= yyls; struct Cyc_Yyltype* _temp5232= _temp5230.curr
+ yylsp_offset; if( _temp5230.base == 0? 1:( _temp5232 < _temp5230.base? 1:
_temp5232 >= _temp5230.last_plus_one)){ _throw( Null_Exception);}* _temp5232;}).last_line));
_temp5214;}); _temp5213;}); break;} case 220: _LL5199: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5241= yyvs; void** _temp5243=
_temp5241.curr +( yyvsp_offset - 1); if( _temp5241.base == 0? 1:( _temp5243 <
_temp5241.base? 1: _temp5243 >= _temp5241.last_plus_one)){ _throw(
Null_Exception);}* _temp5243;})),( struct _tagged_string)({ char* _temp5244=(
char*)"H"; struct _tagged_string _temp5245; _temp5245.curr= _temp5244; _temp5245.base=
_temp5244; _temp5245.last_plus_one= _temp5244 + 2; _temp5245;})) == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp5246=( char*)"bad occurrence of heap region `H";
struct _tagged_string _temp5247; _temp5247.curr= _temp5246; _temp5247.base=
_temp5246; _temp5247.last_plus_one= _temp5246 + 33; _temp5247;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5248= yyls; struct Cyc_Yyltype* _temp5250= _temp5248.curr
+ yylsp_offset; if( _temp5248.base == 0? 1:( _temp5250 < _temp5248.base? 1:
_temp5250 >= _temp5248.last_plus_one)){ _throw( Null_Exception);}* _temp5250;}).first_line,({
struct _tagged_ptr3 _temp5251= yyls; struct Cyc_Yyltype* _temp5253= _temp5251.curr
+ yylsp_offset; if( _temp5251.base == 0? 1:( _temp5253 < _temp5251.base? 1:
_temp5253 >= _temp5251.last_plus_one)){ _throw( Null_Exception);}* _temp5253;}).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5276=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5276->name=({ struct
_tagged_string* _temp5277=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp5277[ 0]=({ struct _tagged_string _temp5281= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5278= yyvs; void** _temp5280= _temp5278.curr +(
yyvsp_offset - 1); if( _temp5278.base == 0? 1:( _temp5280 < _temp5278.base? 1:
_temp5280 >= _temp5278.last_plus_one)){ _throw( Null_Exception);}* _temp5280;}));
xprintf("`%.*s", _temp5281.last_plus_one - _temp5281.curr, _temp5281.curr);});
_temp5277;}); _temp5276->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp5276;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp5274=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp5274[ 0]=({ struct Cyc_Absyn_VarType_struct _temp5275; _temp5275.tag= Cyc_Absyn_VarType;
_temp5275.f1= tv; _temp5275;}); _temp5274;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5254=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5254[ 0]=({ struct Cyc_Stmt_tok_struct _temp5255; _temp5255.tag= Cyc_Stmt_tok;
_temp5255.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp5256=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp5256[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp5257; _temp5257.tag= Cyc_Absyn_Region_s;
_temp5257.f1= tv; _temp5257.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5258=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5258->f1=(
void*) Cyc_Absyn_Loc_n; _temp5258->f2=({ struct _tagged_string* _temp5259=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp5259[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5260= yyvs; void** _temp5262=
_temp5260.curr +( yyvsp_offset - 1); if( _temp5260.base == 0? 1:( _temp5262 <
_temp5260.base? 1: _temp5262 >= _temp5260.last_plus_one)){ _throw(
Null_Exception);}* _temp5262;})); _temp5259;}); _temp5258;}),( void*)({ struct
Cyc_Absyn_RgnHandleType_struct* _temp5263=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp5263[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp5264; _temp5264.tag= Cyc_Absyn_RgnHandleType;
_temp5264.f1=( void*) t; _temp5264;}); _temp5263;}), 0); _temp5257.f3= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5265= yyvs; void** _temp5267= _temp5265.curr +
yyvsp_offset; if( _temp5265.base == 0? 1:( _temp5267 < _temp5265.base? 1:
_temp5267 >= _temp5265.last_plus_one)){ _throw( Null_Exception);}* _temp5267;}));
_temp5257;}); _temp5256;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5268= yyls; struct Cyc_Yyltype* _temp5270= _temp5268.curr +( yylsp_offset -
2); if( _temp5268.base == 0? 1:( _temp5270 < _temp5268.base? 1: _temp5270 >=
_temp5268.last_plus_one)){ _throw( Null_Exception);}* _temp5270;}).first_line,({
struct _tagged_ptr3 _temp5271= yyls; struct Cyc_Yyltype* _temp5273= _temp5271.curr
+ yylsp_offset; if( _temp5271.base == 0? 1:( _temp5273 < _temp5271.base? 1:
_temp5273 >= _temp5271.last_plus_one)){ _throw( Null_Exception);}* _temp5273;}).last_line));
_temp5255;}); _temp5254;}); break;} case 221: _LL5240: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5283=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5283[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5284; _temp5284.tag= Cyc_Stmt_tok; _temp5284.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Cut_s_struct* _temp5285=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp5285[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp5286; _temp5286.tag= Cyc_Absyn_Cut_s; _temp5286.f1= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5287= yyvs; void** _temp5289= _temp5287.curr +
yyvsp_offset; if( _temp5287.base == 0? 1:( _temp5289 < _temp5287.base? 1:
_temp5289 >= _temp5287.last_plus_one)){ _throw( Null_Exception);}* _temp5289;}));
_temp5286;}); _temp5285;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5290= yyls; struct Cyc_Yyltype* _temp5292= _temp5290.curr +( yylsp_offset -
1); if( _temp5290.base == 0? 1:( _temp5292 < _temp5290.base? 1: _temp5292 >=
_temp5290.last_plus_one)){ _throw( Null_Exception);}* _temp5292;}).first_line,({
struct _tagged_ptr3 _temp5293= yyls; struct Cyc_Yyltype* _temp5295= _temp5293.curr
+ yylsp_offset; if( _temp5293.base == 0? 1:( _temp5295 < _temp5293.base? 1:
_temp5295 >= _temp5293.last_plus_one)){ _throw( Null_Exception);}* _temp5295;}).last_line));
_temp5284;}); _temp5283;}); break; case 222: _LL5282: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5297=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5297[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5298; _temp5298.tag= Cyc_Stmt_tok; _temp5298.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Splice_s_struct* _temp5299=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp5299[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp5300; _temp5300.tag= Cyc_Absyn_Splice_s;
_temp5300.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5301= yyvs; void**
_temp5303= _temp5301.curr + yyvsp_offset; if( _temp5301.base == 0? 1:( _temp5303
< _temp5301.base? 1: _temp5303 >= _temp5301.last_plus_one)){ _throw(
Null_Exception);}* _temp5303;})); _temp5300;}); _temp5299;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5304= yyls; struct Cyc_Yyltype* _temp5306= _temp5304.curr
+( yylsp_offset - 1); if( _temp5304.base == 0? 1:( _temp5306 < _temp5304.base? 1:
_temp5306 >= _temp5304.last_plus_one)){ _throw( Null_Exception);}* _temp5306;}).first_line,({
struct _tagged_ptr3 _temp5307= yyls; struct Cyc_Yyltype* _temp5309= _temp5307.curr
+ yylsp_offset; if( _temp5307.base == 0? 1:( _temp5309 < _temp5307.base? 1:
_temp5309 >= _temp5307.last_plus_one)){ _throw( Null_Exception);}* _temp5309;}).last_line));
_temp5298;}); _temp5297;}); break; case 223: _LL5296: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5311=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5311[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5312; _temp5312.tag= Cyc_Stmt_tok; _temp5312.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Label_s_struct* _temp5313=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp5313[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp5314; _temp5314.tag= Cyc_Absyn_Label_s; _temp5314.f1=({
struct _tagged_string* _temp5315=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5315[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5316= yyvs; void** _temp5318= _temp5316.curr +( yyvsp_offset -
2); if( _temp5316.base == 0? 1:( _temp5318 < _temp5316.base? 1: _temp5318 >=
_temp5316.last_plus_one)){ _throw( Null_Exception);}* _temp5318;})); _temp5315;});
_temp5314.f2= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5319= yyvs; void**
_temp5321= _temp5319.curr + yyvsp_offset; if( _temp5319.base == 0? 1:( _temp5321
< _temp5319.base? 1: _temp5321 >= _temp5319.last_plus_one)){ _throw(
Null_Exception);}* _temp5321;})); _temp5314;}); _temp5313;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5322= yyls; struct Cyc_Yyltype* _temp5324= _temp5322.curr
+( yylsp_offset - 2); if( _temp5322.base == 0? 1:( _temp5324 < _temp5322.base? 1:
_temp5324 >= _temp5322.last_plus_one)){ _throw( Null_Exception);}* _temp5324;}).first_line,({
struct _tagged_ptr3 _temp5325= yyls; struct Cyc_Yyltype* _temp5327= _temp5325.curr
+ yylsp_offset; if( _temp5325.base == 0? 1:( _temp5327 < _temp5325.base? 1:
_temp5327 >= _temp5325.last_plus_one)){ _throw( Null_Exception);}* _temp5327;}).last_line));
_temp5312;}); _temp5311;}); break; case 224: _LL5310: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5329=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5329[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5330; _temp5330.tag= Cyc_Stmt_tok; _temp5330.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5331= yyls; struct Cyc_Yyltype* _temp5333= _temp5331.curr
+ yylsp_offset; if( _temp5331.base == 0? 1:( _temp5333 < _temp5331.base? 1:
_temp5333 >= _temp5331.last_plus_one)){ _throw( Null_Exception);}* _temp5333;}).first_line,({
struct _tagged_ptr3 _temp5334= yyls; struct Cyc_Yyltype* _temp5336= _temp5334.curr
+ yylsp_offset; if( _temp5334.base == 0? 1:( _temp5336 < _temp5334.base? 1:
_temp5336 >= _temp5334.last_plus_one)){ _throw( Null_Exception);}* _temp5336;}).last_line));
_temp5330;}); _temp5329;}); break; case 225: _LL5328: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5338=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5338[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5339; _temp5339.tag= Cyc_Stmt_tok; _temp5339.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5340= yyvs; void** _temp5342= _temp5340.curr +(
yyvsp_offset - 1); if( _temp5340.base == 0? 1:( _temp5342 < _temp5340.base? 1:
_temp5342 >= _temp5340.last_plus_one)){ _throw( Null_Exception);}* _temp5342;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5343= yyls; struct Cyc_Yyltype*
_temp5345= _temp5343.curr +( yylsp_offset - 1); if( _temp5343.base == 0? 1:(
_temp5345 < _temp5343.base? 1: _temp5345 >= _temp5343.last_plus_one)){ _throw(
Null_Exception);}* _temp5345;}).first_line,({ struct _tagged_ptr3 _temp5346=
yyls; struct Cyc_Yyltype* _temp5348= _temp5346.curr + yylsp_offset; if(
_temp5346.base == 0? 1:( _temp5348 < _temp5346.base? 1: _temp5348 >= _temp5346.last_plus_one)){
_throw( Null_Exception);}* _temp5348;}).last_line)); _temp5339;}); _temp5338;});
break; case 226: _LL5337: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5350=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5350[ 0]=({ struct Cyc_Stmt_tok_struct _temp5351; _temp5351.tag= Cyc_Stmt_tok;
_temp5351.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5352= yyls; struct Cyc_Yyltype* _temp5354= _temp5352.curr +(
yylsp_offset - 1); if( _temp5352.base == 0? 1:( _temp5354 < _temp5352.base? 1:
_temp5354 >= _temp5352.last_plus_one)){ _throw( Null_Exception);}* _temp5354;}).first_line,({
struct _tagged_ptr3 _temp5355= yyls; struct Cyc_Yyltype* _temp5357= _temp5355.curr
+ yylsp_offset; if( _temp5355.base == 0? 1:( _temp5357 < _temp5355.base? 1:
_temp5357 >= _temp5355.last_plus_one)){ _throw( Null_Exception);}* _temp5357;}).last_line));
_temp5351;}); _temp5350;}); break; case 227: _LL5349: yyval=({ struct
_tagged_ptr2 _temp5359= yyvs; void** _temp5361= _temp5359.curr +( yyvsp_offset -
1); if( _temp5359.base == 0? 1:( _temp5361 < _temp5359.base? 1: _temp5361 >=
_temp5359.last_plus_one)){ _throw( Null_Exception);}* _temp5361;}); break; case
228: _LL5358:{ void* _temp5366= Cyc_yyget_BlockItem_tok(({ struct _tagged_ptr2
_temp5363= yyvs; void** _temp5365= _temp5363.curr + yyvsp_offset; if( _temp5363.base
== 0? 1:( _temp5365 < _temp5363.base? 1: _temp5365 >= _temp5363.last_plus_one)){
_throw( Null_Exception);}* _temp5365;})); struct Cyc_List_List* _temp5374;
struct Cyc_Absyn_Fndecl* _temp5376; struct Cyc_Absyn_Stmt* _temp5378; _LL5368:
if(*(( int*) _temp5366) == Cyc_Parse_TopDecls_bl){ _LL5375: _temp5374=( struct
Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*) _temp5366)->f1; goto
_LL5369;} else{ goto _LL5370;} _LL5370: if(*(( int*) _temp5366) == Cyc_Parse_FnDecl_bl){
_LL5377: _temp5376=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp5366)->f1; goto _LL5371;} else{ goto _LL5372;} _LL5372: if(*(( int*)
_temp5366) == Cyc_Parse_Stmt_bl){ _LL5379: _temp5378=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5366)->f1; goto _LL5373;} else{ goto
_LL5367;} _LL5369: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5380=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5380[ 0]=({ struct Cyc_Stmt_tok_struct _temp5381; _temp5381.tag= Cyc_Stmt_tok;
_temp5381.f1= Cyc_Parse_flatten_declarations( _temp5374, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5382= yyls; struct Cyc_Yyltype*
_temp5384= _temp5382.curr + yylsp_offset; if( _temp5382.base == 0? 1:( _temp5384
< _temp5382.base? 1: _temp5384 >= _temp5382.last_plus_one)){ _throw(
Null_Exception);}* _temp5384;}).first_line,({ struct _tagged_ptr3 _temp5385=
yyls; struct Cyc_Yyltype* _temp5387= _temp5385.curr + yylsp_offset; if(
_temp5385.base == 0? 1:( _temp5387 < _temp5385.base? 1: _temp5387 >= _temp5385.last_plus_one)){
_throw( Null_Exception);}* _temp5387;}).last_line))); _temp5381;}); _temp5380;});
goto _LL5367; _LL5371: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5388=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5388[ 0]=({ struct Cyc_Stmt_tok_struct _temp5389; _temp5389.tag= Cyc_Stmt_tok;
_temp5389.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5390=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5390[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5391; _temp5391.tag= Cyc_Absyn_Fn_d;
_temp5391.f1= _temp5376; _temp5391;}); _temp5390;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5392= yyls; struct Cyc_Yyltype* _temp5394= _temp5392.curr
+ yylsp_offset; if( _temp5392.base == 0? 1:( _temp5394 < _temp5392.base? 1:
_temp5394 >= _temp5392.last_plus_one)){ _throw( Null_Exception);}* _temp5394;}).first_line,({
struct _tagged_ptr3 _temp5395= yyls; struct Cyc_Yyltype* _temp5397= _temp5395.curr
+ yylsp_offset; if( _temp5395.base == 0? 1:( _temp5397 < _temp5395.base? 1:
_temp5397 >= _temp5395.last_plus_one)){ _throw( Null_Exception);}* _temp5397;}).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5398= yyls; struct Cyc_Yyltype* _temp5400= _temp5398.curr + yylsp_offset;
if( _temp5398.base == 0? 1:( _temp5400 < _temp5398.base? 1: _temp5400 >=
_temp5398.last_plus_one)){ _throw( Null_Exception);}* _temp5400;}).first_line,({
struct _tagged_ptr3 _temp5401= yyls; struct Cyc_Yyltype* _temp5403= _temp5401.curr
+ yylsp_offset; if( _temp5401.base == 0? 1:( _temp5403 < _temp5401.base? 1:
_temp5403 >= _temp5401.last_plus_one)){ _throw( Null_Exception);}* _temp5403;}).last_line)));
_temp5389;}); _temp5388;}); goto _LL5367; _LL5373: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5404=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5404[ 0]=({ struct Cyc_Stmt_tok_struct _temp5405; _temp5405.tag= Cyc_Stmt_tok;
_temp5405.f1= _temp5378; _temp5405;}); _temp5404;}); goto _LL5367; _LL5367:;}
break; case 229: _LL5362:{ void* _temp5410= Cyc_yyget_BlockItem_tok(({ struct
_tagged_ptr2 _temp5407= yyvs; void** _temp5409= _temp5407.curr +( yyvsp_offset -
1); if( _temp5407.base == 0? 1:( _temp5409 < _temp5407.base? 1: _temp5409 >=
_temp5407.last_plus_one)){ _throw( Null_Exception);}* _temp5409;})); struct Cyc_List_List*
_temp5418; struct Cyc_Absyn_Fndecl* _temp5420; struct Cyc_Absyn_Stmt* _temp5422;
_LL5412: if(*(( int*) _temp5410) == Cyc_Parse_TopDecls_bl){ _LL5419: _temp5418=(
struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*) _temp5410)->f1;
goto _LL5413;} else{ goto _LL5414;} _LL5414: if(*(( int*) _temp5410) == Cyc_Parse_FnDecl_bl){
_LL5421: _temp5420=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp5410)->f1; goto _LL5415;} else{ goto _LL5416;} _LL5416: if(*(( int*)
_temp5410) == Cyc_Parse_Stmt_bl){ _LL5423: _temp5422=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5410)->f1; goto _LL5417;} else{ goto
_LL5411;} _LL5413: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5424=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5424[ 0]=({ struct Cyc_Stmt_tok_struct _temp5425; _temp5425.tag= Cyc_Stmt_tok;
_temp5425.f1= Cyc_Parse_flatten_declarations( _temp5418, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5426= yyvs; void** _temp5428= _temp5426.curr +
yyvsp_offset; if( _temp5426.base == 0? 1:( _temp5428 < _temp5426.base? 1:
_temp5428 >= _temp5426.last_plus_one)){ _throw( Null_Exception);}* _temp5428;})));
_temp5425;}); _temp5424;}); goto _LL5411; _LL5415: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5429=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5429[ 0]=({ struct Cyc_Stmt_tok_struct _temp5430; _temp5430.tag= Cyc_Stmt_tok;
_temp5430.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5431=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5431[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5432; _temp5432.tag= Cyc_Absyn_Fn_d;
_temp5432.f1= _temp5420; _temp5432;}); _temp5431;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5433= yyls; struct Cyc_Yyltype* _temp5435= _temp5433.curr
+( yylsp_offset - 1); if( _temp5433.base == 0? 1:( _temp5435 < _temp5433.base? 1:
_temp5435 >= _temp5433.last_plus_one)){ _throw( Null_Exception);}* _temp5435;}).first_line,({
struct _tagged_ptr3 _temp5436= yyls; struct Cyc_Yyltype* _temp5438= _temp5436.curr
+( yylsp_offset - 1); if( _temp5436.base == 0? 1:( _temp5438 < _temp5436.base? 1:
_temp5438 >= _temp5436.last_plus_one)){ _throw( Null_Exception);}* _temp5438;}).last_line)),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5439= yyvs; void** _temp5441=
_temp5439.curr + yyvsp_offset; if( _temp5439.base == 0? 1:( _temp5441 <
_temp5439.base? 1: _temp5441 >= _temp5439.last_plus_one)){ _throw(
Null_Exception);}* _temp5441;}))); _temp5430;}); _temp5429;}); goto _LL5411;
_LL5417: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5442=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5442[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5443; _temp5443.tag= Cyc_Stmt_tok; _temp5443.f1= Cyc_Absyn_seq_stmt(
_temp5422, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5444= yyvs; void**
_temp5446= _temp5444.curr + yyvsp_offset; if( _temp5444.base == 0? 1:( _temp5446
< _temp5444.base? 1: _temp5446 >= _temp5444.last_plus_one)){ _throw(
Null_Exception);}* _temp5446;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5447= yyls; struct Cyc_Yyltype* _temp5449= _temp5447.curr +(
yylsp_offset - 1); if( _temp5447.base == 0? 1:( _temp5449 < _temp5447.base? 1:
_temp5449 >= _temp5447.last_plus_one)){ _throw( Null_Exception);}* _temp5449;}).first_line,({
struct _tagged_ptr3 _temp5450= yyls; struct Cyc_Yyltype* _temp5452= _temp5450.curr
+ yylsp_offset; if( _temp5450.base == 0? 1:( _temp5452 < _temp5450.base? 1:
_temp5452 >= _temp5450.last_plus_one)){ _throw( Null_Exception);}* _temp5452;}).last_line));
_temp5443;}); _temp5442;}); goto _LL5411; _LL5411:;} break; case 230: _LL5406:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp5454=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5454[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5455; _temp5455.tag= Cyc_BlockItem_tok; _temp5455.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp5456=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp5456[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp5457; _temp5457.tag= Cyc_Parse_TopDecls_bl;
_temp5457.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5458= yyvs;
void** _temp5460= _temp5458.curr + yyvsp_offset; if( _temp5458.base == 0? 1:(
_temp5460 < _temp5458.base? 1: _temp5460 >= _temp5458.last_plus_one)){ _throw(
Null_Exception);}* _temp5460;})); _temp5457;}); _temp5456;})); _temp5455;});
_temp5454;}); break; case 231: _LL5453: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp5462=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5462[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5463; _temp5463.tag= Cyc_BlockItem_tok;
_temp5463.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp5464=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp5464[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp5465; _temp5465.tag= Cyc_Parse_Stmt_bl;
_temp5465.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5466= yyvs; void**
_temp5468= _temp5466.curr + yyvsp_offset; if( _temp5466.base == 0? 1:( _temp5468
< _temp5466.base? 1: _temp5468 >= _temp5466.last_plus_one)){ _throw(
Null_Exception);}* _temp5468;})); _temp5465;}); _temp5464;})); _temp5463;});
_temp5462;}); break; case 232: _LL5461: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp5470=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5470[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5471; _temp5471.tag= Cyc_BlockItem_tok;
_temp5471.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp5472=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp5472[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp5473; _temp5473.tag= Cyc_Parse_FnDecl_bl;
_temp5473.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp5474= yyvs; void**
_temp5476= _temp5474.curr + yyvsp_offset; if( _temp5474.base == 0? 1:( _temp5476
< _temp5474.base? 1: _temp5476 >= _temp5474.last_plus_one)){ _throw(
Null_Exception);}* _temp5476;})); _temp5473;}); _temp5472;})); _temp5471;});
_temp5470;}); break; case 233: _LL5469: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5478=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5478[ 0]=({ struct Cyc_Stmt_tok_struct _temp5479; _temp5479.tag= Cyc_Stmt_tok;
_temp5479.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5480= yyvs; void** _temp5482= _temp5480.curr +( yyvsp_offset -
2); if( _temp5480.base == 0? 1:( _temp5482 < _temp5480.base? 1: _temp5482 >=
_temp5480.last_plus_one)){ _throw( Null_Exception);}* _temp5482;})), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5483= yyvs; void** _temp5485= _temp5483.curr +
yyvsp_offset; if( _temp5483.base == 0? 1:( _temp5485 < _temp5483.base? 1:
_temp5485 >= _temp5483.last_plus_one)){ _throw( Null_Exception);}* _temp5485;})),
Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5486= yyls; struct Cyc_Yyltype* _temp5488= _temp5486.curr +( yylsp_offset -
4); if( _temp5486.base == 0? 1:( _temp5488 < _temp5486.base? 1: _temp5488 >=
_temp5486.last_plus_one)){ _throw( Null_Exception);}* _temp5488;}).first_line,({
struct _tagged_ptr3 _temp5489= yyls; struct Cyc_Yyltype* _temp5491= _temp5489.curr
+ yylsp_offset; if( _temp5489.base == 0? 1:( _temp5491 < _temp5489.base? 1:
_temp5491 >= _temp5489.last_plus_one)){ _throw( Null_Exception);}* _temp5491;}).last_line));
_temp5479;}); _temp5478;}); break; case 234: _LL5477: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5493=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5493[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5494; _temp5494.tag= Cyc_Stmt_tok; _temp5494.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5495= yyvs; void** _temp5497=
_temp5495.curr +( yyvsp_offset - 4); if( _temp5495.base == 0? 1:( _temp5497 <
_temp5495.base? 1: _temp5497 >= _temp5495.last_plus_one)){ _throw(
Null_Exception);}* _temp5497;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5498= yyvs; void** _temp5500= _temp5498.curr +( yyvsp_offset - 2); if(
_temp5498.base == 0? 1:( _temp5500 < _temp5498.base? 1: _temp5500 >= _temp5498.last_plus_one)){
_throw( Null_Exception);}* _temp5500;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5501= yyvs; void** _temp5503= _temp5501.curr + yyvsp_offset;
if( _temp5501.base == 0? 1:( _temp5503 < _temp5501.base? 1: _temp5503 >=
_temp5501.last_plus_one)){ _throw( Null_Exception);}* _temp5503;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5504= yyls; struct Cyc_Yyltype* _temp5506= _temp5504.curr
+( yylsp_offset - 6); if( _temp5504.base == 0? 1:( _temp5506 < _temp5504.base? 1:
_temp5506 >= _temp5504.last_plus_one)){ _throw( Null_Exception);}* _temp5506;}).first_line,({
struct _tagged_ptr3 _temp5507= yyls; struct Cyc_Yyltype* _temp5509= _temp5507.curr
+ yylsp_offset; if( _temp5507.base == 0? 1:( _temp5509 < _temp5507.base? 1:
_temp5509 >= _temp5507.last_plus_one)){ _throw( Null_Exception);}* _temp5509;}).last_line));
_temp5494;}); _temp5493;}); break; case 235: _LL5492: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5511=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5511[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5512; _temp5512.tag= Cyc_Stmt_tok; _temp5512.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5513= yyvs; void** _temp5515= _temp5513.curr +(
yyvsp_offset - 4); if( _temp5513.base == 0? 1:( _temp5515 < _temp5513.base? 1:
_temp5515 >= _temp5513.last_plus_one)){ _throw( Null_Exception);}* _temp5515;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp5516= yyvs; void**
_temp5518= _temp5516.curr +( yyvsp_offset - 1); if( _temp5516.base == 0? 1:(
_temp5518 < _temp5516.base? 1: _temp5518 >= _temp5516.last_plus_one)){ _throw(
Null_Exception);}* _temp5518;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5519= yyls; struct Cyc_Yyltype* _temp5521= _temp5519.curr +(
yylsp_offset - 6); if( _temp5519.base == 0? 1:( _temp5521 < _temp5519.base? 1:
_temp5521 >= _temp5519.last_plus_one)){ _throw( Null_Exception);}* _temp5521;}).first_line,({
struct _tagged_ptr3 _temp5522= yyls; struct Cyc_Yyltype* _temp5524= _temp5522.curr
+ yylsp_offset; if( _temp5522.base == 0? 1:( _temp5524 < _temp5522.base? 1:
_temp5524 >= _temp5522.last_plus_one)){ _throw( Null_Exception);}* _temp5524;}).last_line));
_temp5512;}); _temp5511;}); break; case 236: _LL5510: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5526=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5526[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5527; _temp5527.tag= Cyc_Stmt_tok; _temp5527.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5528= yyvs; void** _temp5530=
_temp5528.curr +( yyvsp_offset - 4); if( _temp5528.base == 0? 1:( _temp5530 <
_temp5528.base? 1: _temp5530 >= _temp5528.last_plus_one)){ _throw(
Null_Exception);}* _temp5530;})), Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5531= yyvs; void** _temp5533= _temp5531.curr +( yyvsp_offset -
1); if( _temp5531.base == 0? 1:( _temp5533 < _temp5531.base? 1: _temp5533 >=
_temp5531.last_plus_one)){ _throw( Null_Exception);}* _temp5533;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5534= yyls; struct Cyc_Yyltype* _temp5536= _temp5534.curr
+( yylsp_offset - 5); if( _temp5534.base == 0? 1:( _temp5536 < _temp5534.base? 1:
_temp5536 >= _temp5534.last_plus_one)){ _throw( Null_Exception);}* _temp5536;}).first_line,({
struct _tagged_ptr3 _temp5537= yyls; struct Cyc_Yyltype* _temp5539= _temp5537.curr
+ yylsp_offset; if( _temp5537.base == 0? 1:( _temp5539 < _temp5537.base? 1:
_temp5539 >= _temp5537.last_plus_one)){ _throw( Null_Exception);}* _temp5539;}).last_line));
_temp5527;}); _temp5526;}); break; case 237: _LL5525: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp5541=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5541[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5542; _temp5542.tag= Cyc_SwitchClauseList_tok;
_temp5542.f1= 0; _temp5542;}); _temp5541;}); break; case 238: _LL5540: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5544=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5544[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5545; _temp5545.tag= Cyc_SwitchClauseList_tok;
_temp5545.f1=({ struct Cyc_List_List* _temp5546=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5546->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp5547=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp5547->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5557= yyls; struct Cyc_Yyltype* _temp5559= _temp5557.curr
+( yylsp_offset - 2); if( _temp5557.base == 0? 1:( _temp5559 < _temp5557.base? 1:
_temp5559 >= _temp5557.last_plus_one)){ _throw( Null_Exception);}* _temp5559;}).first_line,({
struct _tagged_ptr3 _temp5560= yyls; struct Cyc_Yyltype* _temp5562= _temp5560.curr
+( yylsp_offset - 2); if( _temp5560.base == 0? 1:( _temp5562 < _temp5560.base? 1:
_temp5562 >= _temp5560.last_plus_one)){ _throw( Null_Exception);}* _temp5562;}).last_line));
_temp5547->pat_vars= 0; _temp5547->where_clause= 0; _temp5547->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5554= yyvs; void** _temp5556= _temp5554.curr +
yyvsp_offset; if( _temp5554.base == 0? 1:( _temp5556 < _temp5554.base? 1:
_temp5556 >= _temp5554.last_plus_one)){ _throw( Null_Exception);}* _temp5556;}));
_temp5547->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5548=
yyls; struct Cyc_Yyltype* _temp5550= _temp5548.curr +( yylsp_offset - 2); if(
_temp5548.base == 0? 1:( _temp5550 < _temp5548.base? 1: _temp5550 >= _temp5548.last_plus_one)){
_throw( Null_Exception);}* _temp5550;}).first_line,({ struct _tagged_ptr3
_temp5551= yyls; struct Cyc_Yyltype* _temp5553= _temp5551.curr + yylsp_offset;
if( _temp5551.base == 0? 1:( _temp5553 < _temp5551.base? 1: _temp5553 >=
_temp5551.last_plus_one)){ _throw( Null_Exception);}* _temp5553;}).last_line);
_temp5547;}); _temp5546->tl= 0; _temp5546;}); _temp5545;}); _temp5544;}); break;
case 239: _LL5543: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5564=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5564[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5565; _temp5565.tag= Cyc_SwitchClauseList_tok; _temp5565.f1=({ struct Cyc_List_List*
_temp5566=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5566->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5570=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5570->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5583= yyvs; void** _temp5585= _temp5583.curr +(
yyvsp_offset - 2); if( _temp5583.base == 0? 1:( _temp5585 < _temp5583.base? 1:
_temp5585 >= _temp5583.last_plus_one)){ _throw( Null_Exception);}* _temp5585;}));
_temp5570->pat_vars= 0; _temp5570->where_clause= 0; _temp5570->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5577= yyls; struct Cyc_Yyltype*
_temp5579= _temp5577.curr +( yylsp_offset - 1); if( _temp5577.base == 0? 1:(
_temp5579 < _temp5577.base? 1: _temp5579 >= _temp5577.last_plus_one)){ _throw(
Null_Exception);}* _temp5579;}).first_line,({ struct _tagged_ptr3 _temp5580=
yyls; struct Cyc_Yyltype* _temp5582= _temp5580.curr +( yylsp_offset - 1); if(
_temp5580.base == 0? 1:( _temp5582 < _temp5580.base? 1: _temp5582 >= _temp5580.last_plus_one)){
_throw( Null_Exception);}* _temp5582;}).last_line)); _temp5570->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5571= yyls; struct Cyc_Yyltype* _temp5573= _temp5571.curr
+( yylsp_offset - 3); if( _temp5571.base == 0? 1:( _temp5573 < _temp5571.base? 1:
_temp5573 >= _temp5571.last_plus_one)){ _throw( Null_Exception);}* _temp5573;}).first_line,({
struct _tagged_ptr3 _temp5574= yyls; struct Cyc_Yyltype* _temp5576= _temp5574.curr
+ yylsp_offset; if( _temp5574.base == 0? 1:( _temp5576 < _temp5574.base? 1:
_temp5576 >= _temp5574.last_plus_one)){ _throw( Null_Exception);}* _temp5576;}).last_line);
_temp5570;}); _temp5566->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5567= yyvs; void** _temp5569= _temp5567.curr + yyvsp_offset;
if( _temp5567.base == 0? 1:( _temp5569 < _temp5567.base? 1: _temp5569 >=
_temp5567.last_plus_one)){ _throw( Null_Exception);}* _temp5569;})); _temp5566;});
_temp5565;}); _temp5564;}); break; case 240: _LL5563: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp5587=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5587[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5588; _temp5588.tag= Cyc_SwitchClauseList_tok;
_temp5588.f1=({ struct Cyc_List_List* _temp5589=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5589->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp5593=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp5593->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5603= yyvs;
void** _temp5605= _temp5603.curr +( yyvsp_offset - 3); if( _temp5603.base == 0?
1:( _temp5605 < _temp5603.base? 1: _temp5605 >= _temp5603.last_plus_one)){
_throw( Null_Exception);}* _temp5605;})); _temp5593->pat_vars= 0; _temp5593->where_clause=
0; _temp5593->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5600= yyvs;
void** _temp5602= _temp5600.curr +( yyvsp_offset - 1); if( _temp5600.base == 0?
1:( _temp5602 < _temp5600.base? 1: _temp5602 >= _temp5600.last_plus_one)){
_throw( Null_Exception);}* _temp5602;})); _temp5593->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5594= yyls; struct Cyc_Yyltype* _temp5596= _temp5594.curr
+( yylsp_offset - 4); if( _temp5594.base == 0? 1:( _temp5596 < _temp5594.base? 1:
_temp5596 >= _temp5594.last_plus_one)){ _throw( Null_Exception);}* _temp5596;}).first_line,({
struct _tagged_ptr3 _temp5597= yyls; struct Cyc_Yyltype* _temp5599= _temp5597.curr
+( yylsp_offset - 1); if( _temp5597.base == 0? 1:( _temp5599 < _temp5597.base? 1:
_temp5599 >= _temp5597.last_plus_one)){ _throw( Null_Exception);}* _temp5599;}).last_line);
_temp5593;}); _temp5589->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5590= yyvs; void** _temp5592= _temp5590.curr + yyvsp_offset;
if( _temp5590.base == 0? 1:( _temp5592 < _temp5590.base? 1: _temp5592 >=
_temp5590.last_plus_one)){ _throw( Null_Exception);}* _temp5592;})); _temp5589;});
_temp5588;}); _temp5587;}); break; case 241: _LL5586: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp5607=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5607[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5608; _temp5608.tag= Cyc_SwitchClauseList_tok;
_temp5608.f1=({ struct Cyc_List_List* _temp5609=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5609->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp5613=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp5613->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5629= yyvs;
void** _temp5631= _temp5629.curr +( yyvsp_offset - 4); if( _temp5629.base == 0?
1:( _temp5631 < _temp5629.base? 1: _temp5631 >= _temp5629.last_plus_one)){
_throw( Null_Exception);}* _temp5631;})); _temp5613->pat_vars= 0; _temp5613->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5626= yyvs;
void** _temp5628= _temp5626.curr +( yyvsp_offset - 2); if( _temp5626.base == 0?
1:( _temp5628 < _temp5626.base? 1: _temp5628 >= _temp5626.last_plus_one)){
_throw( Null_Exception);}* _temp5628;})); _temp5613->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5620= yyls; struct Cyc_Yyltype*
_temp5622= _temp5620.curr +( yylsp_offset - 1); if( _temp5620.base == 0? 1:(
_temp5622 < _temp5620.base? 1: _temp5622 >= _temp5620.last_plus_one)){ _throw(
Null_Exception);}* _temp5622;}).first_line,({ struct _tagged_ptr3 _temp5623=
yyls; struct Cyc_Yyltype* _temp5625= _temp5623.curr +( yylsp_offset - 1); if(
_temp5623.base == 0? 1:( _temp5625 < _temp5623.base? 1: _temp5625 >= _temp5623.last_plus_one)){
_throw( Null_Exception);}* _temp5625;}).last_line)); _temp5613->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5614= yyls; struct Cyc_Yyltype* _temp5616= _temp5614.curr
+( yylsp_offset - 5); if( _temp5614.base == 0? 1:( _temp5616 < _temp5614.base? 1:
_temp5616 >= _temp5614.last_plus_one)){ _throw( Null_Exception);}* _temp5616;}).first_line,({
struct _tagged_ptr3 _temp5617= yyls; struct Cyc_Yyltype* _temp5619= _temp5617.curr
+ yylsp_offset; if( _temp5617.base == 0? 1:( _temp5619 < _temp5617.base? 1:
_temp5619 >= _temp5617.last_plus_one)){ _throw( Null_Exception);}* _temp5619;}).last_line);
_temp5613;}); _temp5609->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5610= yyvs; void** _temp5612= _temp5610.curr + yyvsp_offset;
if( _temp5610.base == 0? 1:( _temp5612 < _temp5610.base? 1: _temp5612 >=
_temp5610.last_plus_one)){ _throw( Null_Exception);}* _temp5612;})); _temp5609;});
_temp5608;}); _temp5607;}); break; case 242: _LL5606: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp5633=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5633[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5634; _temp5634.tag= Cyc_SwitchClauseList_tok;
_temp5634.f1=({ struct Cyc_List_List* _temp5635=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5635->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp5639=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp5639->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5652= yyvs;
void** _temp5654= _temp5652.curr +( yyvsp_offset - 5); if( _temp5652.base == 0?
1:( _temp5654 < _temp5652.base? 1: _temp5654 >= _temp5652.last_plus_one)){
_throw( Null_Exception);}* _temp5654;})); _temp5639->pat_vars= 0; _temp5639->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5649= yyvs;
void** _temp5651= _temp5649.curr +( yyvsp_offset - 3); if( _temp5649.base == 0?
1:( _temp5651 < _temp5649.base? 1: _temp5651 >= _temp5649.last_plus_one)){
_throw( Null_Exception);}* _temp5651;})); _temp5639->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5646= yyvs; void** _temp5648= _temp5646.curr +(
yyvsp_offset - 1); if( _temp5646.base == 0? 1:( _temp5648 < _temp5646.base? 1:
_temp5648 >= _temp5646.last_plus_one)){ _throw( Null_Exception);}* _temp5648;}));
_temp5639->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5640=
yyls; struct Cyc_Yyltype* _temp5642= _temp5640.curr +( yylsp_offset - 6); if(
_temp5640.base == 0? 1:( _temp5642 < _temp5640.base? 1: _temp5642 >= _temp5640.last_plus_one)){
_throw( Null_Exception);}* _temp5642;}).first_line,({ struct _tagged_ptr3
_temp5643= yyls; struct Cyc_Yyltype* _temp5645= _temp5643.curr + yylsp_offset;
if( _temp5643.base == 0? 1:( _temp5645 < _temp5643.base? 1: _temp5645 >=
_temp5643.last_plus_one)){ _throw( Null_Exception);}* _temp5645;}).last_line);
_temp5639;}); _temp5635->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5636= yyvs; void** _temp5638= _temp5636.curr + yyvsp_offset;
if( _temp5636.base == 0? 1:( _temp5638 < _temp5636.base? 1: _temp5638 >=
_temp5636.last_plus_one)){ _throw( Null_Exception);}* _temp5638;})); _temp5635;});
_temp5634;}); _temp5633;}); break; case 243: _LL5632: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5656=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5656[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5657; _temp5657.tag= Cyc_Stmt_tok; _temp5657.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5658= yyvs; void** _temp5660= _temp5658.curr +(
yyvsp_offset - 2); if( _temp5658.base == 0? 1:( _temp5660 < _temp5658.base? 1:
_temp5660 >= _temp5658.last_plus_one)){ _throw( Null_Exception);}* _temp5660;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5661= yyvs; void** _temp5663=
_temp5661.curr + yyvsp_offset; if( _temp5661.base == 0? 1:( _temp5663 <
_temp5661.base? 1: _temp5663 >= _temp5661.last_plus_one)){ _throw(
Null_Exception);}* _temp5663;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5664= yyls; struct Cyc_Yyltype* _temp5666= _temp5664.curr +(
yylsp_offset - 4); if( _temp5664.base == 0? 1:( _temp5666 < _temp5664.base? 1:
_temp5666 >= _temp5664.last_plus_one)){ _throw( Null_Exception);}* _temp5666;}).first_line,({
struct _tagged_ptr3 _temp5667= yyls; struct Cyc_Yyltype* _temp5669= _temp5667.curr
+ yylsp_offset; if( _temp5667.base == 0? 1:( _temp5669 < _temp5667.base? 1:
_temp5669 >= _temp5667.last_plus_one)){ _throw( Null_Exception);}* _temp5669;}).last_line));
_temp5657;}); _temp5656;}); break; case 244: _LL5655: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5671=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5671[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5672; _temp5672.tag= Cyc_Stmt_tok; _temp5672.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5673= yyvs; void** _temp5675= _temp5673.curr +(
yyvsp_offset - 5); if( _temp5673.base == 0? 1:( _temp5675 < _temp5673.base? 1:
_temp5675 >= _temp5673.last_plus_one)){ _throw( Null_Exception);}* _temp5675;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5676= yyvs; void** _temp5678=
_temp5676.curr +( yyvsp_offset - 2); if( _temp5676.base == 0? 1:( _temp5678 <
_temp5676.base? 1: _temp5678 >= _temp5676.last_plus_one)){ _throw(
Null_Exception);}* _temp5678;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5679= yyls; struct Cyc_Yyltype* _temp5681= _temp5679.curr +(
yylsp_offset - 6); if( _temp5679.base == 0? 1:( _temp5681 < _temp5679.base? 1:
_temp5681 >= _temp5679.last_plus_one)){ _throw( Null_Exception);}* _temp5681;}).first_line,({
struct _tagged_ptr3 _temp5682= yyls; struct Cyc_Yyltype* _temp5684= _temp5682.curr
+ yylsp_offset; if( _temp5682.base == 0? 1:( _temp5684 < _temp5682.base? 1:
_temp5684 >= _temp5682.last_plus_one)){ _throw( Null_Exception);}* _temp5684;}).last_line));
_temp5672;}); _temp5671;}); break; case 245: _LL5670: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5686=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5686[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5687; _temp5687.tag= Cyc_Stmt_tok; _temp5687.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5688= yyvs; void** _temp5690= _temp5688.curr +
yyvsp_offset; if( _temp5688.base == 0? 1:( _temp5690 < _temp5688.base? 1:
_temp5690 >= _temp5688.last_plus_one)){ _throw( Null_Exception);}* _temp5690;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5691= yyls; struct Cyc_Yyltype*
_temp5693= _temp5691.curr +( yylsp_offset - 5); if( _temp5691.base == 0? 1:(
_temp5693 < _temp5691.base? 1: _temp5693 >= _temp5691.last_plus_one)){ _throw(
Null_Exception);}* _temp5693;}).first_line,({ struct _tagged_ptr3 _temp5694=
yyls; struct Cyc_Yyltype* _temp5696= _temp5694.curr + yylsp_offset; if(
_temp5694.base == 0? 1:( _temp5696 < _temp5694.base? 1: _temp5696 >= _temp5694.last_plus_one)){
_throw( Null_Exception);}* _temp5696;}).last_line)); _temp5687;}); _temp5686;});
break; case 246: _LL5685: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5698=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5698[ 0]=({ struct Cyc_Stmt_tok_struct _temp5699; _temp5699.tag= Cyc_Stmt_tok;
_temp5699.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5700= yyvs; void** _temp5702=
_temp5700.curr +( yyvsp_offset - 2); if( _temp5700.base == 0? 1:( _temp5702 <
_temp5700.base? 1: _temp5702 >= _temp5700.last_plus_one)){ _throw(
Null_Exception);}* _temp5702;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5703= yyvs; void** _temp5705= _temp5703.curr + yyvsp_offset; if( _temp5703.base
== 0? 1:( _temp5705 < _temp5703.base? 1: _temp5705 >= _temp5703.last_plus_one)){
_throw( Null_Exception);}* _temp5705;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5706= yyls; struct Cyc_Yyltype* _temp5708= _temp5706.curr +(
yylsp_offset - 6); if( _temp5706.base == 0? 1:( _temp5708 < _temp5706.base? 1:
_temp5708 >= _temp5706.last_plus_one)){ _throw( Null_Exception);}* _temp5708;}).first_line,({
struct _tagged_ptr3 _temp5709= yyls; struct Cyc_Yyltype* _temp5711= _temp5709.curr
+ yylsp_offset; if( _temp5709.base == 0? 1:( _temp5711 < _temp5709.base? 1:
_temp5711 >= _temp5709.last_plus_one)){ _throw( Null_Exception);}* _temp5711;}).last_line));
_temp5699;}); _temp5698;}); break; case 247: _LL5697: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5713=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5713[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5714; _temp5714.tag= Cyc_Stmt_tok; _temp5714.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5715= yyvs; void** _temp5717=
_temp5715.curr +( yyvsp_offset - 3); if( _temp5715.base == 0? 1:( _temp5717 <
_temp5715.base? 1: _temp5717 >= _temp5715.last_plus_one)){ _throw(
Null_Exception);}* _temp5717;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5718= yyvs; void** _temp5720= _temp5718.curr +
yyvsp_offset; if( _temp5718.base == 0? 1:( _temp5720 < _temp5718.base? 1:
_temp5720 >= _temp5718.last_plus_one)){ _throw( Null_Exception);}* _temp5720;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5721= yyls; struct Cyc_Yyltype*
_temp5723= _temp5721.curr +( yylsp_offset - 6); if( _temp5721.base == 0? 1:(
_temp5723 < _temp5721.base? 1: _temp5723 >= _temp5721.last_plus_one)){ _throw(
Null_Exception);}* _temp5723;}).first_line,({ struct _tagged_ptr3 _temp5724=
yyls; struct Cyc_Yyltype* _temp5726= _temp5724.curr + yylsp_offset; if(
_temp5724.base == 0? 1:( _temp5726 < _temp5724.base? 1: _temp5726 >= _temp5724.last_plus_one)){
_throw( Null_Exception);}* _temp5726;}).last_line)); _temp5714;}); _temp5713;});
break; case 248: _LL5712: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5728=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5728[ 0]=({ struct Cyc_Stmt_tok_struct _temp5729; _temp5729.tag= Cyc_Stmt_tok;
_temp5729.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5730= yyvs; void** _temp5732= _temp5730.curr +(
yyvsp_offset - 4); if( _temp5730.base == 0? 1:( _temp5732 < _temp5730.base? 1:
_temp5732 >= _temp5730.last_plus_one)){ _throw( Null_Exception);}* _temp5732;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5733= yyvs; void** _temp5735=
_temp5733.curr +( yyvsp_offset - 2); if( _temp5733.base == 0? 1:( _temp5735 <
_temp5733.base? 1: _temp5735 >= _temp5733.last_plus_one)){ _throw(
Null_Exception);}* _temp5735;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5736= yyvs; void** _temp5738= _temp5736.curr + yyvsp_offset; if( _temp5736.base
== 0? 1:( _temp5738 < _temp5736.base? 1: _temp5738 >= _temp5736.last_plus_one)){
_throw( Null_Exception);}* _temp5738;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5739= yyls; struct Cyc_Yyltype* _temp5741= _temp5739.curr +(
yylsp_offset - 7); if( _temp5739.base == 0? 1:( _temp5741 < _temp5739.base? 1:
_temp5741 >= _temp5739.last_plus_one)){ _throw( Null_Exception);}* _temp5741;}).first_line,({
struct _tagged_ptr3 _temp5742= yyls; struct Cyc_Yyltype* _temp5744= _temp5742.curr
+( yylsp_offset - 1); if( _temp5742.base == 0? 1:( _temp5744 < _temp5742.base? 1:
_temp5744 >= _temp5742.last_plus_one)){ _throw( Null_Exception);}* _temp5744;}).last_line));
_temp5729;}); _temp5728;}); break; case 249: _LL5727: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5746=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5746[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5747; _temp5747.tag= Cyc_Stmt_tok; _temp5747.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5748= yyvs; void** _temp5750= _temp5748.curr +(
yyvsp_offset - 4); if( _temp5748.base == 0? 1:( _temp5750 < _temp5748.base? 1:
_temp5750 >= _temp5748.last_plus_one)){ _throw( Null_Exception);}* _temp5750;})),
Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5751= yyvs; void** _temp5753= _temp5751.curr + yyvsp_offset;
if( _temp5751.base == 0? 1:( _temp5753 < _temp5751.base? 1: _temp5753 >=
_temp5751.last_plus_one)){ _throw( Null_Exception);}* _temp5753;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5754= yyls; struct Cyc_Yyltype* _temp5756= _temp5754.curr
+( yylsp_offset - 6); if( _temp5754.base == 0? 1:( _temp5756 < _temp5754.base? 1:
_temp5756 >= _temp5754.last_plus_one)){ _throw( Null_Exception);}* _temp5756;}).first_line,({
struct _tagged_ptr3 _temp5757= yyls; struct Cyc_Yyltype* _temp5759= _temp5757.curr
+ yylsp_offset; if( _temp5757.base == 0? 1:( _temp5759 < _temp5757.base? 1:
_temp5759 >= _temp5757.last_plus_one)){ _throw( Null_Exception);}* _temp5759;}).last_line));
_temp5747;}); _temp5746;}); break; case 250: _LL5745: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5761=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5761[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5762; _temp5762.tag= Cyc_Stmt_tok; _temp5762.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5763= yyvs; void** _temp5765= _temp5763.curr +(
yyvsp_offset - 5); if( _temp5763.base == 0? 1:( _temp5765 < _temp5763.base? 1:
_temp5765 >= _temp5763.last_plus_one)){ _throw( Null_Exception);}* _temp5765;})),
Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5766= yyvs;
void** _temp5768= _temp5766.curr +( yyvsp_offset - 2); if( _temp5766.base == 0?
1:( _temp5768 < _temp5766.base? 1: _temp5768 >= _temp5766.last_plus_one)){
_throw( Null_Exception);}* _temp5768;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5769= yyvs; void** _temp5771= _temp5769.curr + yyvsp_offset;
if( _temp5769.base == 0? 1:( _temp5771 < _temp5769.base? 1: _temp5771 >=
_temp5769.last_plus_one)){ _throw( Null_Exception);}* _temp5771;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5772= yyls; struct Cyc_Yyltype* _temp5774= _temp5772.curr
+( yylsp_offset - 7); if( _temp5772.base == 0? 1:( _temp5774 < _temp5772.base? 1:
_temp5774 >= _temp5772.last_plus_one)){ _throw( Null_Exception);}* _temp5774;}).first_line,({
struct _tagged_ptr3 _temp5775= yyls; struct Cyc_Yyltype* _temp5777= _temp5775.curr
+ yylsp_offset; if( _temp5775.base == 0? 1:( _temp5777 < _temp5775.base? 1:
_temp5777 >= _temp5775.last_plus_one)){ _throw( Null_Exception);}* _temp5777;}).last_line));
_temp5762;}); _temp5761;}); break; case 251: _LL5760: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5779=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5779[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5780; _temp5780.tag= Cyc_Stmt_tok; _temp5780.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5781= yyvs; void** _temp5783= _temp5781.curr +(
yyvsp_offset - 5); if( _temp5781.base == 0? 1:( _temp5783 < _temp5781.base? 1:
_temp5783 >= _temp5781.last_plus_one)){ _throw( Null_Exception);}* _temp5783;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5784= yyvs; void** _temp5786=
_temp5784.curr +( yyvsp_offset - 3); if( _temp5784.base == 0? 1:( _temp5786 <
_temp5784.base? 1: _temp5786 >= _temp5784.last_plus_one)){ _throw(
Null_Exception);}* _temp5786;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5787= yyvs; void** _temp5789= _temp5787.curr +
yyvsp_offset; if( _temp5787.base == 0? 1:( _temp5789 < _temp5787.base? 1:
_temp5789 >= _temp5787.last_plus_one)){ _throw( Null_Exception);}* _temp5789;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5790= yyls; struct Cyc_Yyltype*
_temp5792= _temp5790.curr +( yylsp_offset - 7); if( _temp5790.base == 0? 1:(
_temp5792 < _temp5790.base? 1: _temp5792 >= _temp5790.last_plus_one)){ _throw(
Null_Exception);}* _temp5792;}).first_line,({ struct _tagged_ptr3 _temp5793=
yyls; struct Cyc_Yyltype* _temp5795= _temp5793.curr + yylsp_offset; if(
_temp5793.base == 0? 1:( _temp5795 < _temp5793.base? 1: _temp5795 >= _temp5793.last_plus_one)){
_throw( Null_Exception);}* _temp5795;}).last_line)); _temp5780;}); _temp5779;});
break; case 252: _LL5778: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5797=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5797[ 0]=({ struct Cyc_Stmt_tok_struct _temp5798; _temp5798.tag= Cyc_Stmt_tok;
_temp5798.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5799= yyvs; void** _temp5801= _temp5799.curr +( yyvsp_offset - 6); if(
_temp5799.base == 0? 1:( _temp5801 < _temp5799.base? 1: _temp5801 >= _temp5799.last_plus_one)){
_throw( Null_Exception);}* _temp5801;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5802= yyvs; void** _temp5804= _temp5802.curr +( yyvsp_offset -
4); if( _temp5802.base == 0? 1:( _temp5804 < _temp5802.base? 1: _temp5804 >=
_temp5802.last_plus_one)){ _throw( Null_Exception);}* _temp5804;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5805= yyvs; void** _temp5807= _temp5805.curr +(
yyvsp_offset - 2); if( _temp5805.base == 0? 1:( _temp5807 < _temp5805.base? 1:
_temp5807 >= _temp5805.last_plus_one)){ _throw( Null_Exception);}* _temp5807;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5808= yyvs; void** _temp5810=
_temp5808.curr + yyvsp_offset; if( _temp5808.base == 0? 1:( _temp5810 <
_temp5808.base? 1: _temp5810 >= _temp5808.last_plus_one)){ _throw(
Null_Exception);}* _temp5810;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5811= yyls; struct Cyc_Yyltype* _temp5813= _temp5811.curr +(
yylsp_offset - 8); if( _temp5811.base == 0? 1:( _temp5813 < _temp5811.base? 1:
_temp5813 >= _temp5811.last_plus_one)){ _throw( Null_Exception);}* _temp5813;}).first_line,({
struct _tagged_ptr3 _temp5814= yyls; struct Cyc_Yyltype* _temp5816= _temp5814.curr
+ yylsp_offset; if( _temp5814.base == 0? 1:( _temp5816 < _temp5814.base? 1:
_temp5816 >= _temp5814.last_plus_one)){ _throw( Null_Exception);}* _temp5816;}).last_line));
_temp5798;}); _temp5797;}); break; case 253: _LL5796: { struct Cyc_List_List*
decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5829= yyvs; void**
_temp5831= _temp5829.curr +( yyvsp_offset - 3); if( _temp5829.base == 0? 1:(
_temp5831 < _temp5829.base? 1: _temp5831 >= _temp5829.last_plus_one)){ _throw(
Null_Exception);}* _temp5831;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5820= yyvs; void** _temp5822= _temp5820.curr +
yyvsp_offset; if( _temp5820.base == 0? 1:( _temp5822 < _temp5820.base? 1:
_temp5822 >= _temp5820.last_plus_one)){ _throw( Null_Exception);}* _temp5822;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5823= yyls; struct Cyc_Yyltype*
_temp5825= _temp5823.curr +( yylsp_offset - 5); if( _temp5823.base == 0? 1:(
_temp5825 < _temp5823.base? 1: _temp5825 >= _temp5823.last_plus_one)){ _throw(
Null_Exception);}* _temp5825;}).first_line,({ struct _tagged_ptr3 _temp5826=
yyls; struct Cyc_Yyltype* _temp5828= _temp5826.curr + yylsp_offset; if(
_temp5826.base == 0? 1:( _temp5828 < _temp5826.base? 1: _temp5828 >= _temp5826.last_plus_one)){
_throw( Null_Exception);}* _temp5828;}).last_line)); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5818=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5818[ 0]=({ struct Cyc_Stmt_tok_struct _temp5819; _temp5819.tag= Cyc_Stmt_tok;
_temp5819.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5819;}); _temp5818;});
break;} case 254: _LL5817: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5847= yyvs; void** _temp5849= _temp5847.curr +(
yyvsp_offset - 4); if( _temp5847.base == 0? 1:( _temp5849 < _temp5847.base? 1:
_temp5849 >= _temp5847.last_plus_one)){ _throw( Null_Exception);}* _temp5849;}));
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5835= yyvs; void** _temp5837= _temp5835.curr +(
yyvsp_offset - 3); if( _temp5835.base == 0? 1:( _temp5837 < _temp5835.base? 1:
_temp5837 >= _temp5835.last_plus_one)){ _throw( Null_Exception);}* _temp5837;})),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5838=
yyvs; void** _temp5840= _temp5838.curr + yyvsp_offset; if( _temp5838.base == 0?
1:( _temp5840 < _temp5838.base? 1: _temp5840 >= _temp5838.last_plus_one)){
_throw( Null_Exception);}* _temp5840;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5841= yyls; struct Cyc_Yyltype* _temp5843= _temp5841.curr +(
yylsp_offset - 6); if( _temp5841.base == 0? 1:( _temp5843 < _temp5841.base? 1:
_temp5843 >= _temp5841.last_plus_one)){ _throw( Null_Exception);}* _temp5843;}).first_line,({
struct _tagged_ptr3 _temp5844= yyls; struct Cyc_Yyltype* _temp5846= _temp5844.curr
+ yylsp_offset; if( _temp5844.base == 0? 1:( _temp5846 < _temp5844.base? 1:
_temp5846 >= _temp5844.last_plus_one)){ _throw( Null_Exception);}* _temp5846;}).last_line));
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5833=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5833[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5834; _temp5834.tag= Cyc_Stmt_tok; _temp5834.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5834;}); _temp5833;}); break;} case 255: _LL5832: { struct Cyc_List_List*
decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5865= yyvs; void**
_temp5867= _temp5865.curr +( yyvsp_offset - 4); if( _temp5865.base == 0? 1:(
_temp5867 < _temp5865.base? 1: _temp5867 >= _temp5865.last_plus_one)){ _throw(
Null_Exception);}* _temp5867;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5853= yyvs; void** _temp5855= _temp5853.curr +( yyvsp_offset -
2); if( _temp5853.base == 0? 1:( _temp5855 < _temp5853.base? 1: _temp5855 >=
_temp5853.last_plus_one)){ _throw( Null_Exception);}* _temp5855;})), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5856= yyvs; void** _temp5858= _temp5856.curr +
yyvsp_offset; if( _temp5856.base == 0? 1:( _temp5858 < _temp5856.base? 1:
_temp5858 >= _temp5856.last_plus_one)){ _throw( Null_Exception);}* _temp5858;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5859= yyls; struct Cyc_Yyltype*
_temp5861= _temp5859.curr +( yylsp_offset - 6); if( _temp5859.base == 0? 1:(
_temp5861 < _temp5859.base? 1: _temp5861 >= _temp5859.last_plus_one)){ _throw(
Null_Exception);}* _temp5861;}).first_line,({ struct _tagged_ptr3 _temp5862=
yyls; struct Cyc_Yyltype* _temp5864= _temp5862.curr + yylsp_offset; if(
_temp5862.base == 0? 1:( _temp5864 < _temp5862.base? 1: _temp5864 >= _temp5862.last_plus_one)){
_throw( Null_Exception);}* _temp5864;}).last_line)); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5851=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5851[ 0]=({ struct Cyc_Stmt_tok_struct _temp5852; _temp5852.tag= Cyc_Stmt_tok;
_temp5852.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5852;}); _temp5851;});
break;} case 256: _LL5850: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5886= yyvs; void** _temp5888= _temp5886.curr +(
yyvsp_offset - 5); if( _temp5886.base == 0? 1:( _temp5888 < _temp5886.base? 1:
_temp5888 >= _temp5886.last_plus_one)){ _throw( Null_Exception);}* _temp5888;}));
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5871= yyvs; void** _temp5873= _temp5871.curr +(
yyvsp_offset - 4); if( _temp5871.base == 0? 1:( _temp5873 < _temp5871.base? 1:
_temp5873 >= _temp5871.last_plus_one)){ _throw( Null_Exception);}* _temp5873;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5874= yyvs; void** _temp5876=
_temp5874.curr +( yyvsp_offset - 2); if( _temp5874.base == 0? 1:( _temp5876 <
_temp5874.base? 1: _temp5876 >= _temp5874.last_plus_one)){ _throw(
Null_Exception);}* _temp5876;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5877= yyvs; void** _temp5879= _temp5877.curr + yyvsp_offset; if( _temp5877.base
== 0? 1:( _temp5879 < _temp5877.base? 1: _temp5879 >= _temp5877.last_plus_one)){
_throw( Null_Exception);}* _temp5879;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5880= yyls; struct Cyc_Yyltype* _temp5882= _temp5880.curr +(
yylsp_offset - 7); if( _temp5880.base == 0? 1:( _temp5882 < _temp5880.base? 1:
_temp5882 >= _temp5880.last_plus_one)){ _throw( Null_Exception);}* _temp5882;}).first_line,({
struct _tagged_ptr3 _temp5883= yyls; struct Cyc_Yyltype* _temp5885= _temp5883.curr
+ yylsp_offset; if( _temp5883.base == 0? 1:( _temp5885 < _temp5883.base? 1:
_temp5885 >= _temp5883.last_plus_one)){ _throw( Null_Exception);}* _temp5885;}).last_line));
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5869=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5869[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5870; _temp5870.tag= Cyc_Stmt_tok; _temp5870.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5870;}); _temp5869;}); break;} case 257: _LL5868: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp5890=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5890[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5891; _temp5891.tag= Cyc_Stmt_tok; _temp5891.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_string* _temp5892=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5892[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5893= yyvs; void** _temp5895= _temp5893.curr +( yyvsp_offset -
1); if( _temp5893.base == 0? 1:( _temp5895 < _temp5893.base? 1: _temp5895 >=
_temp5893.last_plus_one)){ _throw( Null_Exception);}* _temp5895;})); _temp5892;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5896= yyls; struct Cyc_Yyltype*
_temp5898= _temp5896.curr +( yylsp_offset - 2); if( _temp5896.base == 0? 1:(
_temp5898 < _temp5896.base? 1: _temp5898 >= _temp5896.last_plus_one)){ _throw(
Null_Exception);}* _temp5898;}).first_line,({ struct _tagged_ptr3 _temp5899=
yyls; struct Cyc_Yyltype* _temp5901= _temp5899.curr +( yylsp_offset - 1); if(
_temp5899.base == 0? 1:( _temp5901 < _temp5899.base? 1: _temp5901 >= _temp5899.last_plus_one)){
_throw( Null_Exception);}* _temp5901;}).last_line)); _temp5891;}); _temp5890;});
break; case 258: _LL5889: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5903=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5903[ 0]=({ struct Cyc_Stmt_tok_struct _temp5904; _temp5904.tag= Cyc_Stmt_tok;
_temp5904.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5905= yyls; struct Cyc_Yyltype* _temp5907= _temp5905.curr +(
yylsp_offset - 1); if( _temp5905.base == 0? 1:( _temp5907 < _temp5905.base? 1:
_temp5907 >= _temp5905.last_plus_one)){ _throw( Null_Exception);}* _temp5907;}).first_line,({
struct _tagged_ptr3 _temp5908= yyls; struct Cyc_Yyltype* _temp5910= _temp5908.curr
+( yylsp_offset - 1); if( _temp5908.base == 0? 1:( _temp5910 < _temp5908.base? 1:
_temp5910 >= _temp5908.last_plus_one)){ _throw( Null_Exception);}* _temp5910;}).last_line));
_temp5904;}); _temp5903;}); break; case 259: _LL5902: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5912=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5912[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5913; _temp5913.tag= Cyc_Stmt_tok; _temp5913.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5914= yyls; struct Cyc_Yyltype* _temp5916= _temp5914.curr
+( yylsp_offset - 1); if( _temp5914.base == 0? 1:( _temp5916 < _temp5914.base? 1:
_temp5916 >= _temp5914.last_plus_one)){ _throw( Null_Exception);}* _temp5916;}).first_line,({
struct _tagged_ptr3 _temp5917= yyls; struct Cyc_Yyltype* _temp5919= _temp5917.curr
+( yylsp_offset - 1); if( _temp5917.base == 0? 1:( _temp5919 < _temp5917.base? 1:
_temp5919 >= _temp5917.last_plus_one)){ _throw( Null_Exception);}* _temp5919;}).last_line));
_temp5913;}); _temp5912;}); break; case 260: _LL5911: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5921=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5921[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5922; _temp5922.tag= Cyc_Stmt_tok; _temp5922.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5923= yyls; struct Cyc_Yyltype*
_temp5925= _temp5923.curr +( yylsp_offset - 1); if( _temp5923.base == 0? 1:(
_temp5925 < _temp5923.base? 1: _temp5925 >= _temp5923.last_plus_one)){ _throw(
Null_Exception);}* _temp5925;}).first_line,({ struct _tagged_ptr3 _temp5926=
yyls; struct Cyc_Yyltype* _temp5928= _temp5926.curr +( yylsp_offset - 1); if(
_temp5926.base == 0? 1:( _temp5928 < _temp5926.base? 1: _temp5928 >= _temp5926.last_plus_one)){
_throw( Null_Exception);}* _temp5928;}).last_line)); _temp5922;}); _temp5921;});
break; case 261: _LL5920: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5930=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5930[ 0]=({ struct Cyc_Stmt_tok_struct _temp5931; _temp5931.tag= Cyc_Stmt_tok;
_temp5931.f1= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5932= yyvs; void** _temp5934= _temp5932.curr +(
yyvsp_offset - 1); if( _temp5932.base == 0? 1:( _temp5934 < _temp5932.base? 1:
_temp5934 >= _temp5932.last_plus_one)){ _throw( Null_Exception);}* _temp5934;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5935= yyls; struct Cyc_Yyltype*
_temp5937= _temp5935.curr +( yylsp_offset - 2); if( _temp5935.base == 0? 1:(
_temp5937 < _temp5935.base? 1: _temp5937 >= _temp5935.last_plus_one)){ _throw(
Null_Exception);}* _temp5937;}).first_line,({ struct _tagged_ptr3 _temp5938=
yyls; struct Cyc_Yyltype* _temp5940= _temp5938.curr +( yylsp_offset - 1); if(
_temp5938.base == 0? 1:( _temp5940 < _temp5938.base? 1: _temp5940 >= _temp5938.last_plus_one)){
_throw( Null_Exception);}* _temp5940;}).last_line)); _temp5931;}); _temp5930;});
break; case 262: _LL5929: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5942=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5942[ 0]=({ struct Cyc_Stmt_tok_struct _temp5943; _temp5943.tag= Cyc_Stmt_tok;
_temp5943.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5944= yyls; struct Cyc_Yyltype* _temp5946= _temp5944.curr +(
yylsp_offset - 1); if( _temp5944.base == 0? 1:( _temp5946 < _temp5944.base? 1:
_temp5946 >= _temp5944.last_plus_one)){ _throw( Null_Exception);}* _temp5946;}).first_line,({
struct _tagged_ptr3 _temp5947= yyls; struct Cyc_Yyltype* _temp5949= _temp5947.curr
+( yylsp_offset - 1); if( _temp5947.base == 0? 1:( _temp5949 < _temp5947.base? 1:
_temp5949 >= _temp5947.last_plus_one)){ _throw( Null_Exception);}* _temp5949;}).last_line));
_temp5943;}); _temp5942;}); break; case 263: _LL5941: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5951=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5951[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5952; _temp5952.tag= Cyc_Stmt_tok; _temp5952.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5953= yyls; struct Cyc_Yyltype*
_temp5955= _temp5953.curr +( yylsp_offset - 3); if( _temp5953.base == 0? 1:(
_temp5955 < _temp5953.base? 1: _temp5955 >= _temp5953.last_plus_one)){ _throw(
Null_Exception);}* _temp5955;}).first_line,({ struct _tagged_ptr3 _temp5956=
yyls; struct Cyc_Yyltype* _temp5958= _temp5956.curr +( yylsp_offset - 3); if(
_temp5956.base == 0? 1:( _temp5958 < _temp5956.base? 1: _temp5958 >= _temp5956.last_plus_one)){
_throw( Null_Exception);}* _temp5958;}).last_line)); _temp5952;}); _temp5951;});
break; case 264: _LL5950: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5960=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5960[ 0]=({ struct Cyc_Stmt_tok_struct _temp5961; _temp5961.tag= Cyc_Stmt_tok;
_temp5961.f1= Cyc_Absyn_fallthru_stmt( Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp5962= yyvs; void** _temp5964= _temp5962.curr +( yyvsp_offset -
2); if( _temp5962.base == 0? 1:( _temp5964 < _temp5962.base? 1: _temp5964 >=
_temp5962.last_plus_one)){ _throw( Null_Exception);}* _temp5964;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5965= yyls; struct Cyc_Yyltype* _temp5967= _temp5965.curr
+( yylsp_offset - 4); if( _temp5965.base == 0? 1:( _temp5967 < _temp5965.base? 1:
_temp5967 >= _temp5965.last_plus_one)){ _throw( Null_Exception);}* _temp5967;}).first_line,({
struct _tagged_ptr3 _temp5968= yyls; struct Cyc_Yyltype* _temp5970= _temp5968.curr
+( yylsp_offset - 4); if( _temp5968.base == 0? 1:( _temp5970 < _temp5968.base? 1:
_temp5970 >= _temp5968.last_plus_one)){ _throw( Null_Exception);}* _temp5970;}).last_line));
_temp5961;}); _temp5960;}); break; case 265: _LL5959: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp5972=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp5972[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5973; _temp5973.tag= Cyc_Pattern_tok; _temp5973.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5974= yyls; struct Cyc_Yyltype* _temp5976= _temp5974.curr + yylsp_offset;
if( _temp5974.base == 0? 1:( _temp5976 < _temp5974.base? 1: _temp5976 >=
_temp5974.last_plus_one)){ _throw( Null_Exception);}* _temp5976;}).first_line,({
struct _tagged_ptr3 _temp5977= yyls; struct Cyc_Yyltype* _temp5979= _temp5977.curr
+ yylsp_offset; if( _temp5977.base == 0? 1:( _temp5979 < _temp5977.base? 1:
_temp5979 >= _temp5977.last_plus_one)){ _throw( Null_Exception);}* _temp5979;}).last_line));
_temp5973;}); _temp5972;}); break; case 266: _LL5971: yyval=({ struct
_tagged_ptr2 _temp5981= yyvs; void** _temp5983= _temp5981.curr +( yyvsp_offset -
1); if( _temp5981.base == 0? 1:( _temp5983 < _temp5981.base? 1: _temp5983 >=
_temp5981.last_plus_one)){ _throw( Null_Exception);}* _temp5983;}); break; case
267: _LL5980: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp5985=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5985[ 0]=({ struct Cyc_Pattern_tok_struct _temp5986; _temp5986.tag= Cyc_Pattern_tok;
_temp5986.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp5987=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp5987[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp5988; _temp5988.tag= Cyc_Absyn_Int_p;
_temp5988.f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5989= yyvs;
void** _temp5991= _temp5989.curr + yyvsp_offset; if( _temp5989.base == 0? 1:(
_temp5991 < _temp5989.base? 1: _temp5991 >= _temp5989.last_plus_one)){ _throw(
Null_Exception);}* _temp5991;}))).f1; _temp5988.f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5992= yyvs; void** _temp5994= _temp5992.curr +
yyvsp_offset; if( _temp5992.base == 0? 1:( _temp5994 < _temp5992.base? 1:
_temp5994 >= _temp5992.last_plus_one)){ _throw( Null_Exception);}* _temp5994;}))).f2;
_temp5988;}); _temp5987;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5995= yyls; struct Cyc_Yyltype* _temp5997= _temp5995.curr + yylsp_offset;
if( _temp5995.base == 0? 1:( _temp5997 < _temp5995.base? 1: _temp5997 >=
_temp5995.last_plus_one)){ _throw( Null_Exception);}* _temp5997;}).first_line,({
struct _tagged_ptr3 _temp5998= yyls; struct Cyc_Yyltype* _temp6000= _temp5998.curr
+ yylsp_offset; if( _temp5998.base == 0? 1:( _temp6000 < _temp5998.base? 1:
_temp6000 >= _temp5998.last_plus_one)){ _throw( Null_Exception);}* _temp6000;}).last_line));
_temp5986;}); _temp5985;}); break; case 268: _LL5984: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6002=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6002[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6003; _temp6003.tag= Cyc_Pattern_tok; _temp6003.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp6004=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp6004[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp6005; _temp6005.tag= Cyc_Absyn_Int_p; _temp6005.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp6005.f2= -(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp6006= yyvs; void**
_temp6008= _temp6006.curr + yyvsp_offset; if( _temp6006.base == 0? 1:( _temp6008
< _temp6006.base? 1: _temp6008 >= _temp6006.last_plus_one)){ _throw(
Null_Exception);}* _temp6008;}))).f2; _temp6005;}); _temp6004;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6009= yyls; struct Cyc_Yyltype* _temp6011= _temp6009.curr
+( yylsp_offset - 1); if( _temp6009.base == 0? 1:( _temp6011 < _temp6009.base? 1:
_temp6011 >= _temp6009.last_plus_one)){ _throw( Null_Exception);}* _temp6011;}).first_line,({
struct _tagged_ptr3 _temp6012= yyls; struct Cyc_Yyltype* _temp6014= _temp6012.curr
+ yylsp_offset; if( _temp6012.base == 0? 1:( _temp6014 < _temp6012.base? 1:
_temp6014 >= _temp6012.last_plus_one)){ _throw( Null_Exception);}* _temp6014;}).last_line));
_temp6003;}); _temp6002;}); break; case 269: _LL6001: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6016=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6016[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6017; _temp6017.tag= Cyc_Pattern_tok; _temp6017.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp6018=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp6018[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp6019; _temp6019.tag= Cyc_Absyn_Float_p; _temp6019.f1=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6020= yyvs; void** _temp6022=
_temp6020.curr + yyvsp_offset; if( _temp6020.base == 0? 1:( _temp6022 <
_temp6020.base? 1: _temp6022 >= _temp6020.last_plus_one)){ _throw(
Null_Exception);}* _temp6022;})); _temp6019;}); _temp6018;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6023= yyls; struct Cyc_Yyltype* _temp6025= _temp6023.curr
+ yylsp_offset; if( _temp6023.base == 0? 1:( _temp6025 < _temp6023.base? 1:
_temp6025 >= _temp6023.last_plus_one)){ _throw( Null_Exception);}* _temp6025;}).first_line,({
struct _tagged_ptr3 _temp6026= yyls; struct Cyc_Yyltype* _temp6028= _temp6026.curr
+ yylsp_offset; if( _temp6026.base == 0? 1:( _temp6028 < _temp6026.base? 1:
_temp6028 >= _temp6026.last_plus_one)){ _throw( Null_Exception);}* _temp6028;}).last_line));
_temp6017;}); _temp6016;}); break; case 270: _LL6015: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6030=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6030[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6031; _temp6031.tag= Cyc_Pattern_tok; _temp6031.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Char_p_struct* _temp6032=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp6032[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp6033; _temp6033.tag= Cyc_Absyn_Char_p; _temp6033.f1= Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp6034= yyvs; void** _temp6036= _temp6034.curr +
yyvsp_offset; if( _temp6034.base == 0? 1:( _temp6036 < _temp6034.base? 1:
_temp6036 >= _temp6034.last_plus_one)){ _throw( Null_Exception);}* _temp6036;}));
_temp6033;}); _temp6032;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6037= yyls; struct Cyc_Yyltype* _temp6039= _temp6037.curr + yylsp_offset;
if( _temp6037.base == 0? 1:( _temp6039 < _temp6037.base? 1: _temp6039 >=
_temp6037.last_plus_one)){ _throw( Null_Exception);}* _temp6039;}).first_line,({
struct _tagged_ptr3 _temp6040= yyls; struct Cyc_Yyltype* _temp6042= _temp6040.curr
+ yylsp_offset; if( _temp6040.base == 0? 1:( _temp6042 < _temp6040.base? 1:
_temp6042 >= _temp6040.last_plus_one)){ _throw( Null_Exception);}* _temp6042;}).last_line));
_temp6031;}); _temp6030;}); break; case 271: _LL6029: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6044=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6044[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6045; _temp6045.tag= Cyc_Pattern_tok; _temp6045.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6046= yyls; struct Cyc_Yyltype* _temp6048= _temp6046.curr + yylsp_offset;
if( _temp6046.base == 0? 1:( _temp6048 < _temp6046.base? 1: _temp6048 >=
_temp6046.last_plus_one)){ _throw( Null_Exception);}* _temp6048;}).first_line,({
struct _tagged_ptr3 _temp6049= yyls; struct Cyc_Yyltype* _temp6051= _temp6049.curr
+ yylsp_offset; if( _temp6049.base == 0? 1:( _temp6051 < _temp6049.base? 1:
_temp6051 >= _temp6049.last_plus_one)){ _throw( Null_Exception);}* _temp6051;}).last_line));
_temp6045;}); _temp6044;}); break; case 272: _LL6043: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6053=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6053[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6054; _temp6054.tag= Cyc_Pattern_tok; _temp6054.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownId_p_struct* _temp6055=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp6055[ 0]=({
struct Cyc_Absyn_UnknownId_p_struct _temp6056; _temp6056.tag= Cyc_Absyn_UnknownId_p;
_temp6056.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6057= yyvs; void**
_temp6059= _temp6057.curr + yyvsp_offset; if( _temp6057.base == 0? 1:( _temp6059
< _temp6057.base? 1: _temp6059 >= _temp6057.last_plus_one)){ _throw(
Null_Exception);}* _temp6059;})); _temp6056;}); _temp6055;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6060= yyls; struct Cyc_Yyltype* _temp6062= _temp6060.curr
+ yylsp_offset; if( _temp6060.base == 0? 1:( _temp6062 < _temp6060.base? 1:
_temp6062 >= _temp6060.last_plus_one)){ _throw( Null_Exception);}* _temp6062;}).first_line,({
struct _tagged_ptr3 _temp6063= yyls; struct Cyc_Yyltype* _temp6065= _temp6063.curr
+ yylsp_offset; if( _temp6063.base == 0? 1:( _temp6065 < _temp6063.base? 1:
_temp6065 >= _temp6063.last_plus_one)){ _throw( Null_Exception);}* _temp6065;}).last_line));
_temp6054;}); _temp6053;}); break; case 273: _LL6052: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6083=
yyls; struct Cyc_Yyltype* _temp6085= _temp6083.curr +( yylsp_offset - 3); if(
_temp6083.base == 0? 1:( _temp6085 < _temp6083.base? 1: _temp6085 >= _temp6083.last_plus_one)){
_throw( Null_Exception);}* _temp6085;}).first_line,({ struct _tagged_ptr3
_temp6086= yyls; struct Cyc_Yyltype* _temp6088= _temp6086.curr +( yylsp_offset -
3); if( _temp6086.base == 0? 1:( _temp6088 < _temp6086.base? 1: _temp6088 >=
_temp6086.last_plus_one)){ _throw( Null_Exception);}* _temp6088;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6089= yyvs; void** _temp6091=
_temp6089.curr +( yyvsp_offset - 3); if( _temp6089.base == 0? 1:( _temp6091 <
_temp6089.base? 1: _temp6091 >= _temp6089.last_plus_one)){ _throw(
Null_Exception);}* _temp6091;}))); yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp6067=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6067[ 0]=({ struct Cyc_Pattern_tok_struct _temp6068; _temp6068.tag= Cyc_Pattern_tok;
_temp6068.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp6069=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp6069[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp6070; _temp6070.tag= Cyc_Absyn_UnknownCall_p; _temp6070.f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6071= yyvs; void** _temp6073= _temp6071.curr +(
yyvsp_offset - 4); if( _temp6071.base == 0? 1:( _temp6073 < _temp6071.base? 1:
_temp6073 >= _temp6071.last_plus_one)){ _throw( Null_Exception);}* _temp6073;}));
_temp6070.f2= tvs; _temp6070.f3= Cyc_yyget_PatternList_tok(({ struct
_tagged_ptr2 _temp6074= yyvs; void** _temp6076= _temp6074.curr +( yyvsp_offset -
1); if( _temp6074.base == 0? 1:( _temp6076 < _temp6074.base? 1: _temp6076 >=
_temp6074.last_plus_one)){ _throw( Null_Exception);}* _temp6076;})); _temp6070;});
_temp6069;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6077= yyls;
struct Cyc_Yyltype* _temp6079= _temp6077.curr +( yylsp_offset - 4); if(
_temp6077.base == 0? 1:( _temp6079 < _temp6077.base? 1: _temp6079 >= _temp6077.last_plus_one)){
_throw( Null_Exception);}* _temp6079;}).first_line,({ struct _tagged_ptr3
_temp6080= yyls; struct Cyc_Yyltype* _temp6082= _temp6080.curr + yylsp_offset;
if( _temp6080.base == 0? 1:( _temp6082 < _temp6080.base? 1: _temp6082 >=
_temp6080.last_plus_one)){ _throw( Null_Exception);}* _temp6082;}).last_line));
_temp6068;}); _temp6067;}); break;} case 274: _LL6066: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6093=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6093[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6094; _temp6094.tag= Cyc_Pattern_tok; _temp6094.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp6095=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp6095[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp6096; _temp6096.tag= Cyc_Absyn_Tuple_p; _temp6096.f1=
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6097= yyvs; void**
_temp6099= _temp6097.curr +( yyvsp_offset - 1); if( _temp6097.base == 0? 1:(
_temp6099 < _temp6097.base? 1: _temp6099 >= _temp6097.last_plus_one)){ _throw(
Null_Exception);}* _temp6099;})); _temp6096;}); _temp6095;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6100= yyls; struct Cyc_Yyltype* _temp6102= _temp6100.curr
+( yylsp_offset - 3); if( _temp6100.base == 0? 1:( _temp6102 < _temp6100.base? 1:
_temp6102 >= _temp6100.last_plus_one)){ _throw( Null_Exception);}* _temp6102;}).first_line,({
struct _tagged_ptr3 _temp6103= yyls; struct Cyc_Yyltype* _temp6105= _temp6103.curr
+ yylsp_offset; if( _temp6103.base == 0? 1:( _temp6105 < _temp6103.base? 1:
_temp6105 >= _temp6103.last_plus_one)){ _throw( Null_Exception);}* _temp6105;}).last_line));
_temp6094;}); _temp6093;}); break; case 275: _LL6092: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6120=
yyls; struct Cyc_Yyltype* _temp6122= _temp6120.curr +( yylsp_offset - 2); if(
_temp6120.base == 0? 1:( _temp6122 < _temp6120.base? 1: _temp6122 >= _temp6120.last_plus_one)){
_throw( Null_Exception);}* _temp6122;}).first_line,({ struct _tagged_ptr3
_temp6123= yyls; struct Cyc_Yyltype* _temp6125= _temp6123.curr +( yylsp_offset -
2); if( _temp6123.base == 0? 1:( _temp6125 < _temp6123.base? 1: _temp6125 >=
_temp6123.last_plus_one)){ _throw( Null_Exception);}* _temp6125;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6126= yyvs; void** _temp6128=
_temp6126.curr +( yyvsp_offset - 2); if( _temp6126.base == 0? 1:( _temp6128 <
_temp6126.base? 1: _temp6128 >= _temp6126.last_plus_one)){ _throw(
Null_Exception);}* _temp6128;}))); yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp6107=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6107[ 0]=({ struct Cyc_Pattern_tok_struct _temp6108; _temp6108.tag= Cyc_Pattern_tok;
_temp6108.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp6109=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp6109[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp6110; _temp6110.tag= Cyc_Absyn_UnknownFields_p; _temp6110.f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6111= yyvs; void** _temp6113= _temp6111.curr +(
yyvsp_offset - 3); if( _temp6111.base == 0? 1:( _temp6113 < _temp6111.base? 1:
_temp6113 >= _temp6111.last_plus_one)){ _throw( Null_Exception);}* _temp6113;}));
_temp6110.f2= tvs; _temp6110.f3= 0; _temp6110;}); _temp6109;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6114= yyls; struct Cyc_Yyltype* _temp6116= _temp6114.curr
+( yylsp_offset - 3); if( _temp6114.base == 0? 1:( _temp6116 < _temp6114.base? 1:
_temp6116 >= _temp6114.last_plus_one)){ _throw( Null_Exception);}* _temp6116;}).first_line,({
struct _tagged_ptr3 _temp6117= yyls; struct Cyc_Yyltype* _temp6119= _temp6117.curr
+ yylsp_offset; if( _temp6117.base == 0? 1:( _temp6119 < _temp6117.base? 1:
_temp6119 >= _temp6117.last_plus_one)){ _throw( Null_Exception);}* _temp6119;}).last_line));
_temp6108;}); _temp6107;}); break;} case 276: _LL6106: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6146=
yyls; struct Cyc_Yyltype* _temp6148= _temp6146.curr +( yylsp_offset - 3); if(
_temp6146.base == 0? 1:( _temp6148 < _temp6146.base? 1: _temp6148 >= _temp6146.last_plus_one)){
_throw( Null_Exception);}* _temp6148;}).first_line,({ struct _tagged_ptr3
_temp6149= yyls; struct Cyc_Yyltype* _temp6151= _temp6149.curr +( yylsp_offset -
3); if( _temp6149.base == 0? 1:( _temp6151 < _temp6149.base? 1: _temp6151 >=
_temp6149.last_plus_one)){ _throw( Null_Exception);}* _temp6151;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6152= yyvs; void** _temp6154=
_temp6152.curr +( yyvsp_offset - 3); if( _temp6152.base == 0? 1:( _temp6154 <
_temp6152.base? 1: _temp6154 >= _temp6152.last_plus_one)){ _throw(
Null_Exception);}* _temp6154;}))); yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp6130=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6130[ 0]=({ struct Cyc_Pattern_tok_struct _temp6131; _temp6131.tag= Cyc_Pattern_tok;
_temp6131.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp6132=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp6132[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp6133; _temp6133.tag= Cyc_Absyn_UnknownFields_p; _temp6133.f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6134= yyvs; void** _temp6136= _temp6134.curr +(
yyvsp_offset - 4); if( _temp6134.base == 0? 1:( _temp6136 < _temp6134.base? 1:
_temp6136 >= _temp6134.last_plus_one)){ _throw( Null_Exception);}* _temp6136;}));
_temp6133.f2= tvs; _temp6133.f3= Cyc_yyget_FieldPatternList_tok(({ struct
_tagged_ptr2 _temp6137= yyvs; void** _temp6139= _temp6137.curr +( yyvsp_offset -
1); if( _temp6137.base == 0? 1:( _temp6139 < _temp6137.base? 1: _temp6139 >=
_temp6137.last_plus_one)){ _throw( Null_Exception);}* _temp6139;})); _temp6133;});
_temp6132;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6140= yyls;
struct Cyc_Yyltype* _temp6142= _temp6140.curr +( yylsp_offset - 4); if(
_temp6140.base == 0? 1:( _temp6142 < _temp6140.base? 1: _temp6142 >= _temp6140.last_plus_one)){
_throw( Null_Exception);}* _temp6142;}).first_line,({ struct _tagged_ptr3
_temp6143= yyls; struct Cyc_Yyltype* _temp6145= _temp6143.curr + yylsp_offset;
if( _temp6143.base == 0? 1:( _temp6145 < _temp6143.base? 1: _temp6145 >=
_temp6143.last_plus_one)){ _throw( Null_Exception);}* _temp6145;}).last_line));
_temp6131;}); _temp6130;}); break;} case 277: _LL6129: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6156=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6156[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6157; _temp6157.tag= Cyc_Pattern_tok; _temp6157.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp6158=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp6158[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp6159; _temp6159.tag= Cyc_Absyn_Pointer_p;
_temp6159.f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6160= yyvs; void**
_temp6162= _temp6160.curr + yyvsp_offset; if( _temp6160.base == 0? 1:( _temp6162
< _temp6160.base? 1: _temp6162 >= _temp6160.last_plus_one)){ _throw(
Null_Exception);}* _temp6162;})); _temp6159;}); _temp6158;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6163= yyls; struct Cyc_Yyltype* _temp6165= _temp6163.curr
+( yylsp_offset - 1); if( _temp6163.base == 0? 1:( _temp6165 < _temp6163.base? 1:
_temp6165 >= _temp6163.last_plus_one)){ _throw( Null_Exception);}* _temp6165;}).first_line,({
struct _tagged_ptr3 _temp6166= yyls; struct Cyc_Yyltype* _temp6168= _temp6166.curr
+ yylsp_offset; if( _temp6166.base == 0? 1:( _temp6168 < _temp6166.base? 1:
_temp6168 >= _temp6166.last_plus_one)){ _throw( Null_Exception);}* _temp6168;}).last_line));
_temp6157;}); _temp6156;}); break; case 278: _LL6155: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6170=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6170[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6171; _temp6171.tag= Cyc_Pattern_tok; _temp6171.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp6172=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp6172[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp6173; _temp6173.tag= Cyc_Absyn_Reference_p;
_temp6173.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp6174=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6174->f1=( void*) Cyc_Absyn_Loc_n;
_temp6174->f2=({ struct _tagged_string* _temp6175=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp6175[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6176= yyvs; void** _temp6178= _temp6176.curr +
yyvsp_offset; if( _temp6176.base == 0? 1:( _temp6178 < _temp6176.base? 1:
_temp6178 >= _temp6176.last_plus_one)){ _throw( Null_Exception);}* _temp6178;}));
_temp6175;}); _temp6174;}),( void*) Cyc_Absyn_VoidType, 0); _temp6173;});
_temp6172;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6179= yyls;
struct Cyc_Yyltype* _temp6181= _temp6179.curr +( yylsp_offset - 1); if(
_temp6179.base == 0? 1:( _temp6181 < _temp6179.base? 1: _temp6181 >= _temp6179.last_plus_one)){
_throw( Null_Exception);}* _temp6181;}).first_line,({ struct _tagged_ptr3
_temp6182= yyls; struct Cyc_Yyltype* _temp6184= _temp6182.curr + yylsp_offset;
if( _temp6182.base == 0? 1:( _temp6184 < _temp6182.base? 1: _temp6184 >=
_temp6182.last_plus_one)){ _throw( Null_Exception);}* _temp6184;}).last_line));
_temp6171;}); _temp6170;}); break; case 279: _LL6169: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp6186=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6186[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6187; _temp6187.tag= Cyc_PatternList_tok;
_temp6187.f1= 0; _temp6187;}); _temp6186;}); break; case 280: _LL6185: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp6189=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6189[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6190; _temp6190.tag= Cyc_PatternList_tok;
_temp6190.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6191= yyvs; void**
_temp6193= _temp6191.curr + yyvsp_offset; if( _temp6191.base == 0? 1:( _temp6193
< _temp6191.base? 1: _temp6193 >= _temp6191.last_plus_one)){ _throw(
Null_Exception);}* _temp6193;}))); _temp6190;}); _temp6189;}); break; case 281:
_LL6188: yyval=( void*)({ struct Cyc_PatternList_tok_struct* _temp6195=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6195[ 0]=({ struct Cyc_PatternList_tok_struct _temp6196; _temp6196.tag= Cyc_PatternList_tok;
_temp6196.f1=({ struct Cyc_List_List* _temp6197=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6197->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6198= yyvs; void** _temp6200= _temp6198.curr +
yyvsp_offset; if( _temp6198.base == 0? 1:( _temp6200 < _temp6198.base? 1:
_temp6200 >= _temp6198.last_plus_one)){ _throw( Null_Exception);}* _temp6200;}));
_temp6197->tl= 0; _temp6197;}); _temp6196;}); _temp6195;}); break; case 282:
_LL6194: yyval=( void*)({ struct Cyc_PatternList_tok_struct* _temp6202=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6202[ 0]=({ struct Cyc_PatternList_tok_struct _temp6203; _temp6203.tag= Cyc_PatternList_tok;
_temp6203.f1=({ struct Cyc_List_List* _temp6204=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6204->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6208= yyvs; void** _temp6210= _temp6208.curr +
yyvsp_offset; if( _temp6208.base == 0? 1:( _temp6210 < _temp6208.base? 1:
_temp6210 >= _temp6208.last_plus_one)){ _throw( Null_Exception);}* _temp6210;}));
_temp6204->tl= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6205= yyvs;
void** _temp6207= _temp6205.curr +( yyvsp_offset - 2); if( _temp6205.base == 0?
1:( _temp6207 < _temp6205.base? 1: _temp6207 >= _temp6205.last_plus_one)){
_throw( Null_Exception);}* _temp6207;})); _temp6204;}); _temp6203;}); _temp6202;});
break; case 283: _LL6201: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct*
_temp6212=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6212[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6213; _temp6213.tag=
Cyc_FieldPattern_tok; _temp6213.f1=({ struct _tuple17* _temp6214=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6214->f1= 0; _temp6214->f2=
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6215= yyvs; void** _temp6217=
_temp6215.curr + yyvsp_offset; if( _temp6215.base == 0? 1:( _temp6217 <
_temp6215.base? 1: _temp6217 >= _temp6215.last_plus_one)){ _throw(
Null_Exception);}* _temp6217;})); _temp6214;}); _temp6213;}); _temp6212;});
break; case 284: _LL6211: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct*
_temp6219=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6219[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6220; _temp6220.tag=
Cyc_FieldPattern_tok; _temp6220.f1=({ struct _tuple17* _temp6221=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6221->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp6225= yyvs; void** _temp6227= _temp6225.curr +(
yyvsp_offset - 1); if( _temp6225.base == 0? 1:( _temp6227 < _temp6225.base? 1:
_temp6227 >= _temp6225.last_plus_one)){ _throw( Null_Exception);}* _temp6227;}));
_temp6221->f2= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6222= yyvs;
void** _temp6224= _temp6222.curr + yyvsp_offset; if( _temp6222.base == 0? 1:(
_temp6224 < _temp6222.base? 1: _temp6224 >= _temp6222.last_plus_one)){ _throw(
Null_Exception);}* _temp6224;})); _temp6221;}); _temp6220;}); _temp6219;});
break; case 285: _LL6218: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp6229=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp6229[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp6230; _temp6230.tag= Cyc_FieldPatternList_tok; _temp6230.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6231= yyvs; void** _temp6233= _temp6231.curr +
yyvsp_offset; if( _temp6231.base == 0? 1:( _temp6233 < _temp6231.base? 1:
_temp6233 >= _temp6231.last_plus_one)){ _throw( Null_Exception);}* _temp6233;})));
_temp6230;}); _temp6229;}); break; case 286: _LL6228: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp6235=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp6235[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp6236; _temp6236.tag= Cyc_FieldPatternList_tok;
_temp6236.f1=({ struct Cyc_List_List* _temp6237=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6237->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp6238= yyvs; void** _temp6240= _temp6238.curr +
yyvsp_offset; if( _temp6238.base == 0? 1:( _temp6240 < _temp6238.base? 1:
_temp6240 >= _temp6238.last_plus_one)){ _throw( Null_Exception);}* _temp6240;}));
_temp6237->tl= 0; _temp6237;}); _temp6236;}); _temp6235;}); break; case 287:
_LL6234: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp6242=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6242[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6243; _temp6243.tag=
Cyc_FieldPatternList_tok; _temp6243.f1=({ struct Cyc_List_List* _temp6244=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6244->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6248= yyvs; void**
_temp6250= _temp6248.curr + yyvsp_offset; if( _temp6248.base == 0? 1:( _temp6250
< _temp6248.base? 1: _temp6250 >= _temp6248.last_plus_one)){ _throw(
Null_Exception);}* _temp6250;})); _temp6244->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6245= yyvs; void** _temp6247= _temp6245.curr +(
yyvsp_offset - 2); if( _temp6245.base == 0? 1:( _temp6247 < _temp6245.base? 1:
_temp6247 >= _temp6245.last_plus_one)){ _throw( Null_Exception);}* _temp6247;}));
_temp6244;}); _temp6243;}); _temp6242;}); break; case 288: _LL6241: yyval=({
struct _tagged_ptr2 _temp6252= yyvs; void** _temp6254= _temp6252.curr +
yyvsp_offset; if( _temp6252.base == 0? 1:( _temp6254 < _temp6252.base? 1:
_temp6254 >= _temp6252.last_plus_one)){ _throw( Null_Exception);}* _temp6254;});
break; case 289: _LL6251: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6256=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6256[ 0]=({ struct Cyc_Exp_tok_struct _temp6257; _temp6257.tag= Cyc_Exp_tok;
_temp6257.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6258= yyvs; void** _temp6260= _temp6258.curr +( yyvsp_offset - 2); if(
_temp6258.base == 0? 1:( _temp6260 < _temp6258.base? 1: _temp6260 >= _temp6258.last_plus_one)){
_throw( Null_Exception);}* _temp6260;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6261= yyvs; void** _temp6263= _temp6261.curr + yyvsp_offset;
if( _temp6261.base == 0? 1:( _temp6263 < _temp6261.base? 1: _temp6263 >=
_temp6261.last_plus_one)){ _throw( Null_Exception);}* _temp6263;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6264= yyls; struct Cyc_Yyltype* _temp6266= _temp6264.curr
+( yylsp_offset - 2); if( _temp6264.base == 0? 1:( _temp6266 < _temp6264.base? 1:
_temp6266 >= _temp6264.last_plus_one)){ _throw( Null_Exception);}* _temp6266;}).first_line,({
struct _tagged_ptr3 _temp6267= yyls; struct Cyc_Yyltype* _temp6269= _temp6267.curr
+ yylsp_offset; if( _temp6267.base == 0? 1:( _temp6269 < _temp6267.base? 1:
_temp6269 >= _temp6267.last_plus_one)){ _throw( Null_Exception);}* _temp6269;}).last_line));
_temp6257;}); _temp6256;}); break; case 290: _LL6255: yyval=({ struct
_tagged_ptr2 _temp6271= yyvs; void** _temp6273= _temp6271.curr + yyvsp_offset;
if( _temp6271.base == 0? 1:( _temp6273 < _temp6271.base? 1: _temp6273 >=
_temp6271.last_plus_one)){ _throw( Null_Exception);}* _temp6273;}); break; case
291: _LL6270: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6275=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6275[ 0]=({ struct Cyc_Exp_tok_struct
_temp6276; _temp6276.tag= Cyc_Exp_tok; _temp6276.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6277= yyvs; void** _temp6279= _temp6277.curr +(
yyvsp_offset - 2); if( _temp6277.base == 0? 1:( _temp6279 < _temp6277.base? 1:
_temp6279 >= _temp6277.last_plus_one)){ _throw( Null_Exception);}* _temp6279;})),
Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2 _temp6280= yyvs; void** _temp6282=
_temp6280.curr +( yyvsp_offset - 1); if( _temp6280.base == 0? 1:( _temp6282 <
_temp6280.base? 1: _temp6282 >= _temp6280.last_plus_one)){ _throw(
Null_Exception);}* _temp6282;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6283= yyvs; void** _temp6285= _temp6283.curr + yyvsp_offset; if( _temp6283.base
== 0? 1:( _temp6285 < _temp6283.base? 1: _temp6285 >= _temp6283.last_plus_one)){
_throw( Null_Exception);}* _temp6285;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6286= yyls; struct Cyc_Yyltype* _temp6288= _temp6286.curr +(
yylsp_offset - 2); if( _temp6286.base == 0? 1:( _temp6288 < _temp6286.base? 1:
_temp6288 >= _temp6286.last_plus_one)){ _throw( Null_Exception);}* _temp6288;}).first_line,({
struct _tagged_ptr3 _temp6289= yyls; struct Cyc_Yyltype* _temp6291= _temp6289.curr
+ yylsp_offset; if( _temp6289.base == 0? 1:( _temp6291 < _temp6289.base? 1:
_temp6291 >= _temp6289.last_plus_one)){ _throw( Null_Exception);}* _temp6291;}).last_line));
_temp6276;}); _temp6275;}); break; case 292: _LL6274: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6293=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6293[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6294; _temp6294.tag= Cyc_Primopopt_tok; _temp6294.f1=
0; _temp6294;}); _temp6293;}); break; case 293: _LL6292: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6296=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6296[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6297; _temp6297.tag= Cyc_Primopopt_tok; _temp6297.f1=({
struct Cyc_Core_Opt* _temp6298=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6298->v=( void*)(( void*) Cyc_Absyn_Times); _temp6298;});
_temp6297;}); _temp6296;}); break; case 294: _LL6295: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6300=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6300[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6301; _temp6301.tag= Cyc_Primopopt_tok; _temp6301.f1=({
struct Cyc_Core_Opt* _temp6302=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6302->v=( void*)(( void*) Cyc_Absyn_Div); _temp6302;});
_temp6301;}); _temp6300;}); break; case 295: _LL6299: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6304=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6304[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6305; _temp6305.tag= Cyc_Primopopt_tok; _temp6305.f1=({
struct Cyc_Core_Opt* _temp6306=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6306->v=( void*)(( void*) Cyc_Absyn_Mod); _temp6306;});
_temp6305;}); _temp6304;}); break; case 296: _LL6303: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6308=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6308[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6309; _temp6309.tag= Cyc_Primopopt_tok; _temp6309.f1=({
struct Cyc_Core_Opt* _temp6310=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6310->v=( void*)(( void*) Cyc_Absyn_Plus); _temp6310;});
_temp6309;}); _temp6308;}); break; case 297: _LL6307: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6312=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6312[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6313; _temp6313.tag= Cyc_Primopopt_tok; _temp6313.f1=({
struct Cyc_Core_Opt* _temp6314=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6314->v=( void*)(( void*) Cyc_Absyn_Minus); _temp6314;});
_temp6313;}); _temp6312;}); break; case 298: _LL6311: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6316=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6316[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6317; _temp6317.tag= Cyc_Primopopt_tok; _temp6317.f1=({
struct Cyc_Core_Opt* _temp6318=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6318->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp6318;});
_temp6317;}); _temp6316;}); break; case 299: _LL6315: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6320=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6320[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6321; _temp6321.tag= Cyc_Primopopt_tok; _temp6321.f1=({
struct Cyc_Core_Opt* _temp6322=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6322->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp6322;});
_temp6321;}); _temp6320;}); break; case 300: _LL6319: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6324=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6324[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6325; _temp6325.tag= Cyc_Primopopt_tok; _temp6325.f1=({
struct Cyc_Core_Opt* _temp6326=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6326->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp6326;});
_temp6325;}); _temp6324;}); break; case 301: _LL6323: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6328=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6328[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6329; _temp6329.tag= Cyc_Primopopt_tok; _temp6329.f1=({
struct Cyc_Core_Opt* _temp6330=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6330->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp6330;});
_temp6329;}); _temp6328;}); break; case 302: _LL6327: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6332=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6332[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6333; _temp6333.tag= Cyc_Primopopt_tok; _temp6333.f1=({
struct Cyc_Core_Opt* _temp6334=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6334->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp6334;});
_temp6333;}); _temp6332;}); break; case 303: _LL6331: yyval=({ struct
_tagged_ptr2 _temp6336= yyvs; void** _temp6338= _temp6336.curr + yyvsp_offset;
if( _temp6336.base == 0? 1:( _temp6338 < _temp6336.base? 1: _temp6338 >=
_temp6336.last_plus_one)){ _throw( Null_Exception);}* _temp6338;}); break; case
304: _LL6335: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6340=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6340[ 0]=({ struct Cyc_Exp_tok_struct
_temp6341; _temp6341.tag= Cyc_Exp_tok; _temp6341.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6342= yyvs; void** _temp6344=
_temp6342.curr +( yyvsp_offset - 4); if( _temp6342.base == 0? 1:( _temp6344 <
_temp6342.base? 1: _temp6344 >= _temp6342.last_plus_one)){ _throw(
Null_Exception);}* _temp6344;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6345= yyvs; void** _temp6347= _temp6345.curr +( yyvsp_offset - 2); if(
_temp6345.base == 0? 1:( _temp6347 < _temp6345.base? 1: _temp6347 >= _temp6345.last_plus_one)){
_throw( Null_Exception);}* _temp6347;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6348= yyvs; void** _temp6350= _temp6348.curr + yyvsp_offset;
if( _temp6348.base == 0? 1:( _temp6350 < _temp6348.base? 1: _temp6350 >=
_temp6348.last_plus_one)){ _throw( Null_Exception);}* _temp6350;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6351= yyls; struct Cyc_Yyltype* _temp6353= _temp6351.curr
+( yylsp_offset - 4); if( _temp6351.base == 0? 1:( _temp6353 < _temp6351.base? 1:
_temp6353 >= _temp6351.last_plus_one)){ _throw( Null_Exception);}* _temp6353;}).first_line,({
struct _tagged_ptr3 _temp6354= yyls; struct Cyc_Yyltype* _temp6356= _temp6354.curr
+ yylsp_offset; if( _temp6354.base == 0? 1:( _temp6356 < _temp6354.base? 1:
_temp6356 >= _temp6354.last_plus_one)){ _throw( Null_Exception);}* _temp6356;}).last_line));
_temp6341;}); _temp6340;}); break; case 305: _LL6339: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6358=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6358[ 0]=({ struct Cyc_Exp_tok_struct
_temp6359; _temp6359.tag= Cyc_Exp_tok; _temp6359.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6360= yyvs; void** _temp6362= _temp6360.curr +
yyvsp_offset; if( _temp6360.base == 0? 1:( _temp6362 < _temp6360.base? 1:
_temp6362 >= _temp6360.last_plus_one)){ _throw( Null_Exception);}* _temp6362;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6363= yyls; struct Cyc_Yyltype*
_temp6365= _temp6363.curr +( yylsp_offset - 1); if( _temp6363.base == 0? 1:(
_temp6365 < _temp6363.base? 1: _temp6365 >= _temp6363.last_plus_one)){ _throw(
Null_Exception);}* _temp6365;}).first_line,({ struct _tagged_ptr3 _temp6366=
yyls; struct Cyc_Yyltype* _temp6368= _temp6366.curr + yylsp_offset; if(
_temp6366.base == 0? 1:( _temp6368 < _temp6366.base? 1: _temp6368 >= _temp6366.last_plus_one)){
_throw( Null_Exception);}* _temp6368;}).last_line)); _temp6359;}); _temp6358;});
break; case 306: _LL6357: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6370=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6370[ 0]=({ struct Cyc_Exp_tok_struct _temp6371; _temp6371.tag= Cyc_Exp_tok;
_temp6371.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6372= yyvs; void** _temp6374= _temp6372.curr + yyvsp_offset; if( _temp6372.base
== 0? 1:( _temp6374 < _temp6372.base? 1: _temp6374 >= _temp6372.last_plus_one)){
_throw( Null_Exception);}* _temp6374;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6375= yyls; struct Cyc_Yyltype* _temp6377= _temp6375.curr +(
yylsp_offset - 1); if( _temp6375.base == 0? 1:( _temp6377 < _temp6375.base? 1:
_temp6377 >= _temp6375.last_plus_one)){ _throw( Null_Exception);}* _temp6377;}).first_line,({
struct _tagged_ptr3 _temp6378= yyls; struct Cyc_Yyltype* _temp6380= _temp6378.curr
+( yylsp_offset + 1); if( _temp6378.base == 0? 1:( _temp6380 < _temp6378.base? 1:
_temp6380 >= _temp6378.last_plus_one)){ _throw( Null_Exception);}* _temp6380;}).last_line));
_temp6371;}); _temp6370;}); break; case 307: _LL6369: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6382=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6382[ 0]=({ struct Cyc_Exp_tok_struct
_temp6383; _temp6383.tag= Cyc_Exp_tok; _temp6383.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6384= yyvs; void** _temp6386= _temp6384.curr +
yyvsp_offset; if( _temp6384.base == 0? 1:( _temp6386 < _temp6384.base? 1:
_temp6386 >= _temp6384.last_plus_one)){ _throw( Null_Exception);}* _temp6386;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6387= yyls; struct Cyc_Yyltype*
_temp6389= _temp6387.curr +( yylsp_offset - 1); if( _temp6387.base == 0? 1:(
_temp6389 < _temp6387.base? 1: _temp6389 >= _temp6387.last_plus_one)){ _throw(
Null_Exception);}* _temp6389;}).first_line,({ struct _tagged_ptr3 _temp6390=
yyls; struct Cyc_Yyltype* _temp6392= _temp6390.curr +( yylsp_offset + 1); if(
_temp6390.base == 0? 1:( _temp6392 < _temp6390.base? 1: _temp6392 >= _temp6390.last_plus_one)){
_throw( Null_Exception);}* _temp6392;}).last_line)); _temp6383;}); _temp6382;});
break; case 308: _LL6381: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6394=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6394[ 0]=({ struct Cyc_Exp_tok_struct _temp6395; _temp6395.tag= Cyc_Exp_tok;
_temp6395.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6396= yyvs; void** _temp6398= _temp6396.curr +(
yyvsp_offset - 2); if( _temp6396.base == 0? 1:( _temp6398 < _temp6396.base? 1:
_temp6398 >= _temp6396.last_plus_one)){ _throw( Null_Exception);}* _temp6398;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6399= yyvs; void** _temp6401=
_temp6399.curr + yyvsp_offset; if( _temp6399.base == 0? 1:( _temp6401 <
_temp6399.base? 1: _temp6401 >= _temp6399.last_plus_one)){ _throw(
Null_Exception);}* _temp6401;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6402= yyls; struct Cyc_Yyltype* _temp6404= _temp6402.curr +(
yylsp_offset - 4); if( _temp6402.base == 0? 1:( _temp6404 < _temp6402.base? 1:
_temp6404 >= _temp6402.last_plus_one)){ _throw( Null_Exception);}* _temp6404;}).first_line,({
struct _tagged_ptr3 _temp6405= yyls; struct Cyc_Yyltype* _temp6407= _temp6405.curr
+ yylsp_offset; if( _temp6405.base == 0? 1:( _temp6407 < _temp6405.base? 1:
_temp6407 >= _temp6405.last_plus_one)){ _throw( Null_Exception);}* _temp6407;}).last_line));
_temp6395;}); _temp6394;}); break; case 309: _LL6393: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6409=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6409[ 0]=({ struct Cyc_Exp_tok_struct
_temp6410; _temp6410.tag= Cyc_Exp_tok; _temp6410.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6411= yyvs; void**
_temp6413= _temp6411.curr +( yyvsp_offset - 2); if( _temp6411.base == 0? 1:(
_temp6413 < _temp6411.base? 1: _temp6413 >= _temp6411.last_plus_one)){ _throw(
Null_Exception);}* _temp6413;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6414= yyvs; void** _temp6416= _temp6414.curr + yyvsp_offset; if( _temp6414.base
== 0? 1:( _temp6416 < _temp6414.base? 1: _temp6416 >= _temp6414.last_plus_one)){
_throw( Null_Exception);}* _temp6416;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6417= yyls; struct Cyc_Yyltype* _temp6419= _temp6417.curr +(
yylsp_offset - 4); if( _temp6417.base == 0? 1:( _temp6419 < _temp6417.base? 1:
_temp6419 >= _temp6417.last_plus_one)){ _throw( Null_Exception);}* _temp6419;}).first_line,({
struct _tagged_ptr3 _temp6420= yyls; struct Cyc_Yyltype* _temp6422= _temp6420.curr
+ yylsp_offset; if( _temp6420.base == 0? 1:( _temp6422 < _temp6420.base? 1:
_temp6422 >= _temp6420.last_plus_one)){ _throw( Null_Exception);}* _temp6422;}).last_line));
_temp6410;}); _temp6409;}); break; case 310: _LL6408: yyval=({ struct
_tagged_ptr2 _temp6424= yyvs; void** _temp6426= _temp6424.curr + yyvsp_offset;
if( _temp6424.base == 0? 1:( _temp6426 < _temp6424.base? 1: _temp6426 >=
_temp6424.last_plus_one)){ _throw( Null_Exception);}* _temp6426;}); break; case
311: _LL6423: yyval=({ struct _tagged_ptr2 _temp6428= yyvs; void** _temp6430=
_temp6428.curr + yyvsp_offset; if( _temp6428.base == 0? 1:( _temp6430 <
_temp6428.base? 1: _temp6430 >= _temp6428.last_plus_one)){ _throw(
Null_Exception);}* _temp6430;}); break; case 312: _LL6427: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp6432=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6432[ 0]=({ struct Cyc_Exp_tok_struct
_temp6433; _temp6433.tag= Cyc_Exp_tok; _temp6433.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6434= yyvs; void** _temp6436= _temp6434.curr +(
yyvsp_offset - 2); if( _temp6434.base == 0? 1:( _temp6436 < _temp6434.base? 1:
_temp6436 >= _temp6434.last_plus_one)){ _throw( Null_Exception);}* _temp6436;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6437= yyvs; void** _temp6439=
_temp6437.curr + yyvsp_offset; if( _temp6437.base == 0? 1:( _temp6439 <
_temp6437.base? 1: _temp6439 >= _temp6437.last_plus_one)){ _throw(
Null_Exception);}* _temp6439;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6440= yyls; struct Cyc_Yyltype* _temp6442= _temp6440.curr +(
yylsp_offset - 2); if( _temp6440.base == 0? 1:( _temp6442 < _temp6440.base? 1:
_temp6442 >= _temp6440.last_plus_one)){ _throw( Null_Exception);}* _temp6442;}).first_line,({
struct _tagged_ptr3 _temp6443= yyls; struct Cyc_Yyltype* _temp6445= _temp6443.curr
+ yylsp_offset; if( _temp6443.base == 0? 1:( _temp6445 < _temp6443.base? 1:
_temp6445 >= _temp6443.last_plus_one)){ _throw( Null_Exception);}* _temp6445;}).last_line));
_temp6433;}); _temp6432;}); break; case 313: _LL6431: yyval=({ struct
_tagged_ptr2 _temp6447= yyvs; void** _temp6449= _temp6447.curr + yyvsp_offset;
if( _temp6447.base == 0? 1:( _temp6449 < _temp6447.base? 1: _temp6449 >=
_temp6447.last_plus_one)){ _throw( Null_Exception);}* _temp6449;}); break; case
314: _LL6446: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6451=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6451[ 0]=({ struct Cyc_Exp_tok_struct
_temp6452; _temp6452.tag= Cyc_Exp_tok; _temp6452.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6453= yyvs; void** _temp6455= _temp6453.curr +(
yyvsp_offset - 2); if( _temp6453.base == 0? 1:( _temp6455 < _temp6453.base? 1:
_temp6455 >= _temp6453.last_plus_one)){ _throw( Null_Exception);}* _temp6455;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6456= yyvs; void** _temp6458=
_temp6456.curr + yyvsp_offset; if( _temp6456.base == 0? 1:( _temp6458 <
_temp6456.base? 1: _temp6458 >= _temp6456.last_plus_one)){ _throw(
Null_Exception);}* _temp6458;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6459= yyls; struct Cyc_Yyltype* _temp6461= _temp6459.curr +(
yylsp_offset - 2); if( _temp6459.base == 0? 1:( _temp6461 < _temp6459.base? 1:
_temp6461 >= _temp6459.last_plus_one)){ _throw( Null_Exception);}* _temp6461;}).first_line,({
struct _tagged_ptr3 _temp6462= yyls; struct Cyc_Yyltype* _temp6464= _temp6462.curr
+ yylsp_offset; if( _temp6462.base == 0? 1:( _temp6464 < _temp6462.base? 1:
_temp6464 >= _temp6462.last_plus_one)){ _throw( Null_Exception);}* _temp6464;}).last_line));
_temp6452;}); _temp6451;}); break; case 315: _LL6450: yyval=({ struct
_tagged_ptr2 _temp6466= yyvs; void** _temp6468= _temp6466.curr + yyvsp_offset;
if( _temp6466.base == 0? 1:( _temp6468 < _temp6466.base? 1: _temp6468 >=
_temp6466.last_plus_one)){ _throw( Null_Exception);}* _temp6468;}); break; case
316: _LL6465: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6470=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6470[ 0]=({ struct Cyc_Exp_tok_struct
_temp6471; _temp6471.tag= Cyc_Exp_tok; _temp6471.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6472= yyvs; void**
_temp6474= _temp6472.curr +( yyvsp_offset - 2); if( _temp6472.base == 0? 1:(
_temp6474 < _temp6472.base? 1: _temp6474 >= _temp6472.last_plus_one)){ _throw(
Null_Exception);}* _temp6474;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6475= yyvs; void** _temp6477= _temp6475.curr + yyvsp_offset; if( _temp6475.base
== 0? 1:( _temp6477 < _temp6475.base? 1: _temp6477 >= _temp6475.last_plus_one)){
_throw( Null_Exception);}* _temp6477;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6478= yyls; struct Cyc_Yyltype* _temp6480= _temp6478.curr +(
yylsp_offset - 2); if( _temp6478.base == 0? 1:( _temp6480 < _temp6478.base? 1:
_temp6480 >= _temp6478.last_plus_one)){ _throw( Null_Exception);}* _temp6480;}).first_line,({
struct _tagged_ptr3 _temp6481= yyls; struct Cyc_Yyltype* _temp6483= _temp6481.curr
+ yylsp_offset; if( _temp6481.base == 0? 1:( _temp6483 < _temp6481.base? 1:
_temp6483 >= _temp6481.last_plus_one)){ _throw( Null_Exception);}* _temp6483;}).last_line));
_temp6471;}); _temp6470;}); break; case 317: _LL6469: yyval=({ struct
_tagged_ptr2 _temp6485= yyvs; void** _temp6487= _temp6485.curr + yyvsp_offset;
if( _temp6485.base == 0? 1:( _temp6487 < _temp6485.base? 1: _temp6487 >=
_temp6485.last_plus_one)){ _throw( Null_Exception);}* _temp6487;}); break; case
318: _LL6484: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6489=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6489[ 0]=({ struct Cyc_Exp_tok_struct
_temp6490; _temp6490.tag= Cyc_Exp_tok; _temp6490.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6491= yyvs; void**
_temp6493= _temp6491.curr +( yyvsp_offset - 2); if( _temp6491.base == 0? 1:(
_temp6493 < _temp6491.base? 1: _temp6493 >= _temp6491.last_plus_one)){ _throw(
Null_Exception);}* _temp6493;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6494= yyvs; void** _temp6496= _temp6494.curr + yyvsp_offset; if( _temp6494.base
== 0? 1:( _temp6496 < _temp6494.base? 1: _temp6496 >= _temp6494.last_plus_one)){
_throw( Null_Exception);}* _temp6496;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6497= yyls; struct Cyc_Yyltype* _temp6499= _temp6497.curr +(
yylsp_offset - 2); if( _temp6497.base == 0? 1:( _temp6499 < _temp6497.base? 1:
_temp6499 >= _temp6497.last_plus_one)){ _throw( Null_Exception);}* _temp6499;}).first_line,({
struct _tagged_ptr3 _temp6500= yyls; struct Cyc_Yyltype* _temp6502= _temp6500.curr
+ yylsp_offset; if( _temp6500.base == 0? 1:( _temp6502 < _temp6500.base? 1:
_temp6502 >= _temp6500.last_plus_one)){ _throw( Null_Exception);}* _temp6502;}).last_line));
_temp6490;}); _temp6489;}); break; case 319: _LL6488: yyval=({ struct
_tagged_ptr2 _temp6504= yyvs; void** _temp6506= _temp6504.curr + yyvsp_offset;
if( _temp6504.base == 0? 1:( _temp6506 < _temp6504.base? 1: _temp6506 >=
_temp6504.last_plus_one)){ _throw( Null_Exception);}* _temp6506;}); break; case
320: _LL6503: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6508=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6508[ 0]=({ struct Cyc_Exp_tok_struct
_temp6509; _temp6509.tag= Cyc_Exp_tok; _temp6509.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6510= yyvs; void**
_temp6512= _temp6510.curr +( yyvsp_offset - 2); if( _temp6510.base == 0? 1:(
_temp6512 < _temp6510.base? 1: _temp6512 >= _temp6510.last_plus_one)){ _throw(
Null_Exception);}* _temp6512;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6513= yyvs; void** _temp6515= _temp6513.curr + yyvsp_offset; if( _temp6513.base
== 0? 1:( _temp6515 < _temp6513.base? 1: _temp6515 >= _temp6513.last_plus_one)){
_throw( Null_Exception);}* _temp6515;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6516= yyls; struct Cyc_Yyltype* _temp6518= _temp6516.curr +(
yylsp_offset - 2); if( _temp6516.base == 0? 1:( _temp6518 < _temp6516.base? 1:
_temp6518 >= _temp6516.last_plus_one)){ _throw( Null_Exception);}* _temp6518;}).first_line,({
struct _tagged_ptr3 _temp6519= yyls; struct Cyc_Yyltype* _temp6521= _temp6519.curr
+ yylsp_offset; if( _temp6519.base == 0? 1:( _temp6521 < _temp6519.base? 1:
_temp6521 >= _temp6519.last_plus_one)){ _throw( Null_Exception);}* _temp6521;}).last_line));
_temp6509;}); _temp6508;}); break; case 321: _LL6507: yyval=({ struct
_tagged_ptr2 _temp6523= yyvs; void** _temp6525= _temp6523.curr + yyvsp_offset;
if( _temp6523.base == 0? 1:( _temp6525 < _temp6523.base? 1: _temp6525 >=
_temp6523.last_plus_one)){ _throw( Null_Exception);}* _temp6525;}); break; case
322: _LL6522: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6527=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6527[ 0]=({ struct Cyc_Exp_tok_struct
_temp6528; _temp6528.tag= Cyc_Exp_tok; _temp6528.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6529= yyvs; void** _temp6531= _temp6529.curr +(
yyvsp_offset - 2); if( _temp6529.base == 0? 1:( _temp6531 < _temp6529.base? 1:
_temp6531 >= _temp6529.last_plus_one)){ _throw( Null_Exception);}* _temp6531;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6532= yyvs; void** _temp6534=
_temp6532.curr + yyvsp_offset; if( _temp6532.base == 0? 1:( _temp6534 <
_temp6532.base? 1: _temp6534 >= _temp6532.last_plus_one)){ _throw(
Null_Exception);}* _temp6534;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6535= yyls; struct Cyc_Yyltype* _temp6537= _temp6535.curr +(
yylsp_offset - 2); if( _temp6535.base == 0? 1:( _temp6537 < _temp6535.base? 1:
_temp6537 >= _temp6535.last_plus_one)){ _throw( Null_Exception);}* _temp6537;}).first_line,({
struct _tagged_ptr3 _temp6538= yyls; struct Cyc_Yyltype* _temp6540= _temp6538.curr
+ yylsp_offset; if( _temp6538.base == 0? 1:( _temp6540 < _temp6538.base? 1:
_temp6540 >= _temp6538.last_plus_one)){ _throw( Null_Exception);}* _temp6540;}).last_line));
_temp6528;}); _temp6527;}); break; case 323: _LL6526: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6542=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6542[ 0]=({ struct Cyc_Exp_tok_struct
_temp6543; _temp6543.tag= Cyc_Exp_tok; _temp6543.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6544= yyvs; void** _temp6546= _temp6544.curr +(
yyvsp_offset - 2); if( _temp6544.base == 0? 1:( _temp6546 < _temp6544.base? 1:
_temp6546 >= _temp6544.last_plus_one)){ _throw( Null_Exception);}* _temp6546;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6547= yyvs; void** _temp6549=
_temp6547.curr + yyvsp_offset; if( _temp6547.base == 0? 1:( _temp6549 <
_temp6547.base? 1: _temp6549 >= _temp6547.last_plus_one)){ _throw(
Null_Exception);}* _temp6549;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6550= yyls; struct Cyc_Yyltype* _temp6552= _temp6550.curr +(
yylsp_offset - 2); if( _temp6550.base == 0? 1:( _temp6552 < _temp6550.base? 1:
_temp6552 >= _temp6550.last_plus_one)){ _throw( Null_Exception);}* _temp6552;}).first_line,({
struct _tagged_ptr3 _temp6553= yyls; struct Cyc_Yyltype* _temp6555= _temp6553.curr
+ yylsp_offset; if( _temp6553.base == 0? 1:( _temp6555 < _temp6553.base? 1:
_temp6555 >= _temp6553.last_plus_one)){ _throw( Null_Exception);}* _temp6555;}).last_line));
_temp6543;}); _temp6542;}); break; case 324: _LL6541: yyval=({ struct
_tagged_ptr2 _temp6557= yyvs; void** _temp6559= _temp6557.curr + yyvsp_offset;
if( _temp6557.base == 0? 1:( _temp6559 < _temp6557.base? 1: _temp6559 >=
_temp6557.last_plus_one)){ _throw( Null_Exception);}* _temp6559;}); break; case
325: _LL6556: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6561=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6561[ 0]=({ struct Cyc_Exp_tok_struct
_temp6562; _temp6562.tag= Cyc_Exp_tok; _temp6562.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6563= yyvs; void** _temp6565= _temp6563.curr +(
yyvsp_offset - 2); if( _temp6563.base == 0? 1:( _temp6565 < _temp6563.base? 1:
_temp6565 >= _temp6563.last_plus_one)){ _throw( Null_Exception);}* _temp6565;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6566= yyvs; void** _temp6568=
_temp6566.curr + yyvsp_offset; if( _temp6566.base == 0? 1:( _temp6568 <
_temp6566.base? 1: _temp6568 >= _temp6566.last_plus_one)){ _throw(
Null_Exception);}* _temp6568;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6569= yyls; struct Cyc_Yyltype* _temp6571= _temp6569.curr +(
yylsp_offset - 2); if( _temp6569.base == 0? 1:( _temp6571 < _temp6569.base? 1:
_temp6571 >= _temp6569.last_plus_one)){ _throw( Null_Exception);}* _temp6571;}).first_line,({
struct _tagged_ptr3 _temp6572= yyls; struct Cyc_Yyltype* _temp6574= _temp6572.curr
+ yylsp_offset; if( _temp6572.base == 0? 1:( _temp6574 < _temp6572.base? 1:
_temp6574 >= _temp6572.last_plus_one)){ _throw( Null_Exception);}* _temp6574;}).last_line));
_temp6562;}); _temp6561;}); break; case 326: _LL6560: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6576=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6576[ 0]=({ struct Cyc_Exp_tok_struct
_temp6577; _temp6577.tag= Cyc_Exp_tok; _temp6577.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6578= yyvs; void** _temp6580= _temp6578.curr +(
yyvsp_offset - 2); if( _temp6578.base == 0? 1:( _temp6580 < _temp6578.base? 1:
_temp6580 >= _temp6578.last_plus_one)){ _throw( Null_Exception);}* _temp6580;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6581= yyvs; void** _temp6583=
_temp6581.curr + yyvsp_offset; if( _temp6581.base == 0? 1:( _temp6583 <
_temp6581.base? 1: _temp6583 >= _temp6581.last_plus_one)){ _throw(
Null_Exception);}* _temp6583;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6584= yyls; struct Cyc_Yyltype* _temp6586= _temp6584.curr +(
yylsp_offset - 2); if( _temp6584.base == 0? 1:( _temp6586 < _temp6584.base? 1:
_temp6586 >= _temp6584.last_plus_one)){ _throw( Null_Exception);}* _temp6586;}).first_line,({
struct _tagged_ptr3 _temp6587= yyls; struct Cyc_Yyltype* _temp6589= _temp6587.curr
+ yylsp_offset; if( _temp6587.base == 0? 1:( _temp6589 < _temp6587.base? 1:
_temp6589 >= _temp6587.last_plus_one)){ _throw( Null_Exception);}* _temp6589;}).last_line));
_temp6577;}); _temp6576;}); break; case 327: _LL6575: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6591=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6591[ 0]=({ struct Cyc_Exp_tok_struct
_temp6592; _temp6592.tag= Cyc_Exp_tok; _temp6592.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6593= yyvs; void** _temp6595= _temp6593.curr +(
yyvsp_offset - 2); if( _temp6593.base == 0? 1:( _temp6595 < _temp6593.base? 1:
_temp6595 >= _temp6593.last_plus_one)){ _throw( Null_Exception);}* _temp6595;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6596= yyvs; void** _temp6598=
_temp6596.curr + yyvsp_offset; if( _temp6596.base == 0? 1:( _temp6598 <
_temp6596.base? 1: _temp6598 >= _temp6596.last_plus_one)){ _throw(
Null_Exception);}* _temp6598;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6599= yyls; struct Cyc_Yyltype* _temp6601= _temp6599.curr +(
yylsp_offset - 2); if( _temp6599.base == 0? 1:( _temp6601 < _temp6599.base? 1:
_temp6601 >= _temp6599.last_plus_one)){ _throw( Null_Exception);}* _temp6601;}).first_line,({
struct _tagged_ptr3 _temp6602= yyls; struct Cyc_Yyltype* _temp6604= _temp6602.curr
+ yylsp_offset; if( _temp6602.base == 0? 1:( _temp6604 < _temp6602.base? 1:
_temp6604 >= _temp6602.last_plus_one)){ _throw( Null_Exception);}* _temp6604;}).last_line));
_temp6592;}); _temp6591;}); break; case 328: _LL6590: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6606=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6606[ 0]=({ struct Cyc_Exp_tok_struct
_temp6607; _temp6607.tag= Cyc_Exp_tok; _temp6607.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6608= yyvs; void** _temp6610= _temp6608.curr +(
yyvsp_offset - 2); if( _temp6608.base == 0? 1:( _temp6610 < _temp6608.base? 1:
_temp6610 >= _temp6608.last_plus_one)){ _throw( Null_Exception);}* _temp6610;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6611= yyvs; void** _temp6613=
_temp6611.curr + yyvsp_offset; if( _temp6611.base == 0? 1:( _temp6613 <
_temp6611.base? 1: _temp6613 >= _temp6611.last_plus_one)){ _throw(
Null_Exception);}* _temp6613;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6614= yyls; struct Cyc_Yyltype* _temp6616= _temp6614.curr +(
yylsp_offset - 2); if( _temp6614.base == 0? 1:( _temp6616 < _temp6614.base? 1:
_temp6616 >= _temp6614.last_plus_one)){ _throw( Null_Exception);}* _temp6616;}).first_line,({
struct _tagged_ptr3 _temp6617= yyls; struct Cyc_Yyltype* _temp6619= _temp6617.curr
+ yylsp_offset; if( _temp6617.base == 0? 1:( _temp6619 < _temp6617.base? 1:
_temp6619 >= _temp6617.last_plus_one)){ _throw( Null_Exception);}* _temp6619;}).last_line));
_temp6607;}); _temp6606;}); break; case 329: _LL6605: yyval=({ struct
_tagged_ptr2 _temp6621= yyvs; void** _temp6623= _temp6621.curr + yyvsp_offset;
if( _temp6621.base == 0? 1:( _temp6623 < _temp6621.base? 1: _temp6623 >=
_temp6621.last_plus_one)){ _throw( Null_Exception);}* _temp6623;}); break; case
330: _LL6620: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6625=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6625[ 0]=({ struct Cyc_Exp_tok_struct
_temp6626; _temp6626.tag= Cyc_Exp_tok; _temp6626.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6627= yyvs;
void** _temp6629= _temp6627.curr +( yyvsp_offset - 2); if( _temp6627.base == 0?
1:( _temp6629 < _temp6627.base? 1: _temp6629 >= _temp6627.last_plus_one)){
_throw( Null_Exception);}* _temp6629;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6630= yyvs; void** _temp6632= _temp6630.curr + yyvsp_offset;
if( _temp6630.base == 0? 1:( _temp6632 < _temp6630.base? 1: _temp6632 >=
_temp6630.last_plus_one)){ _throw( Null_Exception);}* _temp6632;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6633= yyls; struct Cyc_Yyltype* _temp6635= _temp6633.curr
+( yylsp_offset - 2); if( _temp6633.base == 0? 1:( _temp6635 < _temp6633.base? 1:
_temp6635 >= _temp6633.last_plus_one)){ _throw( Null_Exception);}* _temp6635;}).first_line,({
struct _tagged_ptr3 _temp6636= yyls; struct Cyc_Yyltype* _temp6638= _temp6636.curr
+ yylsp_offset; if( _temp6636.base == 0? 1:( _temp6638 < _temp6636.base? 1:
_temp6638 >= _temp6636.last_plus_one)){ _throw( Null_Exception);}* _temp6638;}).last_line));
_temp6626;}); _temp6625;}); break; case 331: _LL6624: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6640=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6640[ 0]=({ struct Cyc_Exp_tok_struct
_temp6641; _temp6641.tag= Cyc_Exp_tok; _temp6641.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6642= yyvs;
void** _temp6644= _temp6642.curr +( yyvsp_offset - 2); if( _temp6642.base == 0?
1:( _temp6644 < _temp6642.base? 1: _temp6644 >= _temp6642.last_plus_one)){
_throw( Null_Exception);}* _temp6644;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6645= yyvs; void** _temp6647= _temp6645.curr + yyvsp_offset;
if( _temp6645.base == 0? 1:( _temp6647 < _temp6645.base? 1: _temp6647 >=
_temp6645.last_plus_one)){ _throw( Null_Exception);}* _temp6647;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6648= yyls; struct Cyc_Yyltype* _temp6650= _temp6648.curr
+( yylsp_offset - 2); if( _temp6648.base == 0? 1:( _temp6650 < _temp6648.base? 1:
_temp6650 >= _temp6648.last_plus_one)){ _throw( Null_Exception);}* _temp6650;}).first_line,({
struct _tagged_ptr3 _temp6651= yyls; struct Cyc_Yyltype* _temp6653= _temp6651.curr
+ yylsp_offset; if( _temp6651.base == 0? 1:( _temp6653 < _temp6651.base? 1:
_temp6653 >= _temp6651.last_plus_one)){ _throw( Null_Exception);}* _temp6653;}).last_line));
_temp6641;}); _temp6640;}); break; case 332: _LL6639: yyval=({ struct
_tagged_ptr2 _temp6655= yyvs; void** _temp6657= _temp6655.curr + yyvsp_offset;
if( _temp6655.base == 0? 1:( _temp6657 < _temp6655.base? 1: _temp6657 >=
_temp6655.last_plus_one)){ _throw( Null_Exception);}* _temp6657;}); break; case
333: _LL6654: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6659=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6659[ 0]=({ struct Cyc_Exp_tok_struct
_temp6660; _temp6660.tag= Cyc_Exp_tok; _temp6660.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6661= yyvs; void**
_temp6663= _temp6661.curr +( yyvsp_offset - 2); if( _temp6661.base == 0? 1:(
_temp6663 < _temp6661.base? 1: _temp6663 >= _temp6661.last_plus_one)){ _throw(
Null_Exception);}* _temp6663;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6664= yyvs; void** _temp6666= _temp6664.curr + yyvsp_offset; if( _temp6664.base
== 0? 1:( _temp6666 < _temp6664.base? 1: _temp6666 >= _temp6664.last_plus_one)){
_throw( Null_Exception);}* _temp6666;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6667= yyls; struct Cyc_Yyltype* _temp6669= _temp6667.curr +(
yylsp_offset - 2); if( _temp6667.base == 0? 1:( _temp6669 < _temp6667.base? 1:
_temp6669 >= _temp6667.last_plus_one)){ _throw( Null_Exception);}* _temp6669;}).first_line,({
struct _tagged_ptr3 _temp6670= yyls; struct Cyc_Yyltype* _temp6672= _temp6670.curr
+ yylsp_offset; if( _temp6670.base == 0? 1:( _temp6672 < _temp6670.base? 1:
_temp6672 >= _temp6670.last_plus_one)){ _throw( Null_Exception);}* _temp6672;}).last_line));
_temp6660;}); _temp6659;}); break; case 334: _LL6658: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6674=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6674[ 0]=({ struct Cyc_Exp_tok_struct
_temp6675; _temp6675.tag= Cyc_Exp_tok; _temp6675.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6676= yyvs; void**
_temp6678= _temp6676.curr +( yyvsp_offset - 2); if( _temp6676.base == 0? 1:(
_temp6678 < _temp6676.base? 1: _temp6678 >= _temp6676.last_plus_one)){ _throw(
Null_Exception);}* _temp6678;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6679= yyvs; void** _temp6681= _temp6679.curr + yyvsp_offset; if( _temp6679.base
== 0? 1:( _temp6681 < _temp6679.base? 1: _temp6681 >= _temp6679.last_plus_one)){
_throw( Null_Exception);}* _temp6681;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6682= yyls; struct Cyc_Yyltype* _temp6684= _temp6682.curr +(
yylsp_offset - 2); if( _temp6682.base == 0? 1:( _temp6684 < _temp6682.base? 1:
_temp6684 >= _temp6682.last_plus_one)){ _throw( Null_Exception);}* _temp6684;}).first_line,({
struct _tagged_ptr3 _temp6685= yyls; struct Cyc_Yyltype* _temp6687= _temp6685.curr
+ yylsp_offset; if( _temp6685.base == 0? 1:( _temp6687 < _temp6685.base? 1:
_temp6687 >= _temp6685.last_plus_one)){ _throw( Null_Exception);}* _temp6687;}).last_line));
_temp6675;}); _temp6674;}); break; case 335: _LL6673: yyval=({ struct
_tagged_ptr2 _temp6689= yyvs; void** _temp6691= _temp6689.curr + yyvsp_offset;
if( _temp6689.base == 0? 1:( _temp6691 < _temp6689.base? 1: _temp6691 >=
_temp6689.last_plus_one)){ _throw( Null_Exception);}* _temp6691;}); break; case
336: _LL6688: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6693=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6693[ 0]=({ struct Cyc_Exp_tok_struct
_temp6694; _temp6694.tag= Cyc_Exp_tok; _temp6694.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6695= yyvs; void**
_temp6697= _temp6695.curr +( yyvsp_offset - 2); if( _temp6695.base == 0? 1:(
_temp6697 < _temp6695.base? 1: _temp6697 >= _temp6695.last_plus_one)){ _throw(
Null_Exception);}* _temp6697;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6698= yyvs; void** _temp6700= _temp6698.curr + yyvsp_offset; if( _temp6698.base
== 0? 1:( _temp6700 < _temp6698.base? 1: _temp6700 >= _temp6698.last_plus_one)){
_throw( Null_Exception);}* _temp6700;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6701= yyls; struct Cyc_Yyltype* _temp6703= _temp6701.curr +(
yylsp_offset - 2); if( _temp6701.base == 0? 1:( _temp6703 < _temp6701.base? 1:
_temp6703 >= _temp6701.last_plus_one)){ _throw( Null_Exception);}* _temp6703;}).first_line,({
struct _tagged_ptr3 _temp6704= yyls; struct Cyc_Yyltype* _temp6706= _temp6704.curr
+ yylsp_offset; if( _temp6704.base == 0? 1:( _temp6706 < _temp6704.base? 1:
_temp6706 >= _temp6704.last_plus_one)){ _throw( Null_Exception);}* _temp6706;}).last_line));
_temp6694;}); _temp6693;}); break; case 337: _LL6692: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6708=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6708[ 0]=({ struct Cyc_Exp_tok_struct
_temp6709; _temp6709.tag= Cyc_Exp_tok; _temp6709.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6710= yyvs; void**
_temp6712= _temp6710.curr +( yyvsp_offset - 2); if( _temp6710.base == 0? 1:(
_temp6712 < _temp6710.base? 1: _temp6712 >= _temp6710.last_plus_one)){ _throw(
Null_Exception);}* _temp6712;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6713= yyvs; void** _temp6715= _temp6713.curr + yyvsp_offset; if( _temp6713.base
== 0? 1:( _temp6715 < _temp6713.base? 1: _temp6715 >= _temp6713.last_plus_one)){
_throw( Null_Exception);}* _temp6715;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6716= yyls; struct Cyc_Yyltype* _temp6718= _temp6716.curr +(
yylsp_offset - 2); if( _temp6716.base == 0? 1:( _temp6718 < _temp6716.base? 1:
_temp6718 >= _temp6716.last_plus_one)){ _throw( Null_Exception);}* _temp6718;}).first_line,({
struct _tagged_ptr3 _temp6719= yyls; struct Cyc_Yyltype* _temp6721= _temp6719.curr
+ yylsp_offset; if( _temp6719.base == 0? 1:( _temp6721 < _temp6719.base? 1:
_temp6721 >= _temp6719.last_plus_one)){ _throw( Null_Exception);}* _temp6721;}).last_line));
_temp6709;}); _temp6708;}); break; case 338: _LL6707: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6723=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6723[ 0]=({ struct Cyc_Exp_tok_struct
_temp6724; _temp6724.tag= Cyc_Exp_tok; _temp6724.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6725= yyvs; void**
_temp6727= _temp6725.curr +( yyvsp_offset - 2); if( _temp6725.base == 0? 1:(
_temp6727 < _temp6725.base? 1: _temp6727 >= _temp6725.last_plus_one)){ _throw(
Null_Exception);}* _temp6727;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6728= yyvs; void** _temp6730= _temp6728.curr + yyvsp_offset; if( _temp6728.base
== 0? 1:( _temp6730 < _temp6728.base? 1: _temp6730 >= _temp6728.last_plus_one)){
_throw( Null_Exception);}* _temp6730;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6731= yyls; struct Cyc_Yyltype* _temp6733= _temp6731.curr +(
yylsp_offset - 2); if( _temp6731.base == 0? 1:( _temp6733 < _temp6731.base? 1:
_temp6733 >= _temp6731.last_plus_one)){ _throw( Null_Exception);}* _temp6733;}).first_line,({
struct _tagged_ptr3 _temp6734= yyls; struct Cyc_Yyltype* _temp6736= _temp6734.curr
+ yylsp_offset; if( _temp6734.base == 0? 1:( _temp6736 < _temp6734.base? 1:
_temp6736 >= _temp6734.last_plus_one)){ _throw( Null_Exception);}* _temp6736;}).last_line));
_temp6724;}); _temp6723;}); break; case 339: _LL6722: yyval=({ struct
_tagged_ptr2 _temp6738= yyvs; void** _temp6740= _temp6738.curr + yyvsp_offset;
if( _temp6738.base == 0? 1:( _temp6740 < _temp6738.base? 1: _temp6740 >=
_temp6738.last_plus_one)){ _throw( Null_Exception);}* _temp6740;}); break; case
340: _LL6737: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6742=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6742[ 0]=({ struct Cyc_Exp_tok_struct
_temp6743; _temp6743.tag= Cyc_Exp_tok; _temp6743.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp6744= yyvs; void** _temp6746= _temp6744.curr +(
yyvsp_offset - 2); if( _temp6744.base == 0? 1:( _temp6746 < _temp6744.base? 1:
_temp6746 >= _temp6744.last_plus_one)){ _throw( Null_Exception);}* _temp6746;}))).f3,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6747= yyvs; void** _temp6749=
_temp6747.curr + yyvsp_offset; if( _temp6747.base == 0? 1:( _temp6749 <
_temp6747.base? 1: _temp6749 >= _temp6747.last_plus_one)){ _throw(
Null_Exception);}* _temp6749;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6750= yyls; struct Cyc_Yyltype* _temp6752= _temp6750.curr +(
yylsp_offset - 3); if( _temp6750.base == 0? 1:( _temp6752 < _temp6750.base? 1:
_temp6752 >= _temp6750.last_plus_one)){ _throw( Null_Exception);}* _temp6752;}).first_line,({
struct _tagged_ptr3 _temp6753= yyls; struct Cyc_Yyltype* _temp6755= _temp6753.curr
+ yylsp_offset; if( _temp6753.base == 0? 1:( _temp6755 < _temp6753.base? 1:
_temp6755 >= _temp6753.last_plus_one)){ _throw( Null_Exception);}* _temp6755;}).last_line));
_temp6743;}); _temp6742;}); break; case 341: _LL6741: yyval=({ struct
_tagged_ptr2 _temp6757= yyvs; void** _temp6759= _temp6757.curr + yyvsp_offset;
if( _temp6757.base == 0? 1:( _temp6759 < _temp6757.base? 1: _temp6759 >=
_temp6757.last_plus_one)){ _throw( Null_Exception);}* _temp6759;}); break; case
342: _LL6756: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6761=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6761[ 0]=({ struct Cyc_Exp_tok_struct
_temp6762; _temp6762.tag= Cyc_Exp_tok; _temp6762.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6763= yyvs; void** _temp6765= _temp6763.curr +
yyvsp_offset; if( _temp6763.base == 0? 1:( _temp6765 < _temp6763.base? 1:
_temp6765 >= _temp6763.last_plus_one)){ _throw( Null_Exception);}* _temp6765;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6766= yyls; struct Cyc_Yyltype*
_temp6768= _temp6766.curr +( yylsp_offset - 1); if( _temp6766.base == 0? 1:(
_temp6768 < _temp6766.base? 1: _temp6768 >= _temp6766.last_plus_one)){ _throw(
Null_Exception);}* _temp6768;}).first_line,({ struct _tagged_ptr3 _temp6769=
yyls; struct Cyc_Yyltype* _temp6771= _temp6769.curr + yylsp_offset; if(
_temp6769.base == 0? 1:( _temp6771 < _temp6769.base? 1: _temp6771 >= _temp6769.last_plus_one)){
_throw( Null_Exception);}* _temp6771;}).last_line)); _temp6762;}); _temp6761;});
break; case 343: _LL6760: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6773=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6773[ 0]=({ struct Cyc_Exp_tok_struct _temp6774; _temp6774.tag= Cyc_Exp_tok;
_temp6774.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6775= yyvs; void** _temp6777= _temp6775.curr + yyvsp_offset; if( _temp6775.base
== 0? 1:( _temp6777 < _temp6775.base? 1: _temp6777 >= _temp6775.last_plus_one)){
_throw( Null_Exception);}* _temp6777;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6778= yyls; struct Cyc_Yyltype* _temp6780= _temp6778.curr +(
yylsp_offset - 1); if( _temp6778.base == 0? 1:( _temp6780 < _temp6778.base? 1:
_temp6780 >= _temp6778.last_plus_one)){ _throw( Null_Exception);}* _temp6780;}).first_line,({
struct _tagged_ptr3 _temp6781= yyls; struct Cyc_Yyltype* _temp6783= _temp6781.curr
+ yylsp_offset; if( _temp6781.base == 0? 1:( _temp6783 < _temp6781.base? 1:
_temp6783 >= _temp6781.last_plus_one)){ _throw( Null_Exception);}* _temp6783;}).last_line));
_temp6774;}); _temp6773;}); break; case 344: _LL6772: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6785=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6785[ 0]=({ struct Cyc_Exp_tok_struct
_temp6786; _temp6786.tag= Cyc_Exp_tok; _temp6786.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6787= yyvs; void** _temp6789= _temp6787.curr +
yyvsp_offset; if( _temp6787.base == 0? 1:( _temp6789 < _temp6787.base? 1:
_temp6789 >= _temp6787.last_plus_one)){ _throw( Null_Exception);}* _temp6789;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6790= yyls; struct Cyc_Yyltype*
_temp6792= _temp6790.curr +( yylsp_offset - 1); if( _temp6790.base == 0? 1:(
_temp6792 < _temp6790.base? 1: _temp6792 >= _temp6790.last_plus_one)){ _throw(
Null_Exception);}* _temp6792;}).first_line,({ struct _tagged_ptr3 _temp6793=
yyls; struct Cyc_Yyltype* _temp6795= _temp6793.curr + yylsp_offset; if(
_temp6793.base == 0? 1:( _temp6795 < _temp6793.base? 1: _temp6795 >= _temp6793.last_plus_one)){
_throw( Null_Exception);}* _temp6795;}).last_line)); _temp6786;}); _temp6785;});
break; case 345: _LL6784: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6797=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6797[ 0]=({ struct Cyc_Exp_tok_struct _temp6798; _temp6798.tag= Cyc_Exp_tok;
_temp6798.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6799= yyvs; void** _temp6801= _temp6799.curr + yyvsp_offset; if( _temp6799.base
== 0? 1:( _temp6801 < _temp6799.base? 1: _temp6801 >= _temp6799.last_plus_one)){
_throw( Null_Exception);}* _temp6801;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6802= yyls; struct Cyc_Yyltype* _temp6804= _temp6802.curr +(
yylsp_offset - 1); if( _temp6802.base == 0? 1:( _temp6804 < _temp6802.base? 1:
_temp6804 >= _temp6802.last_plus_one)){ _throw( Null_Exception);}* _temp6804;}).first_line,({
struct _tagged_ptr3 _temp6805= yyls; struct Cyc_Yyltype* _temp6807= _temp6805.curr
+ yylsp_offset; if( _temp6805.base == 0? 1:( _temp6807 < _temp6805.base? 1:
_temp6807 >= _temp6805.last_plus_one)){ _throw( Null_Exception);}* _temp6807;}).last_line));
_temp6798;}); _temp6797;}); break; case 346: _LL6796: yyval=({ struct
_tagged_ptr2 _temp6809= yyvs; void** _temp6811= _temp6809.curr + yyvsp_offset;
if( _temp6809.base == 0? 1:( _temp6811 < _temp6809.base? 1: _temp6811 >=
_temp6809.last_plus_one)){ _throw( Null_Exception);}* _temp6811;}); break; case
347: _LL6808: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6813=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6813[ 0]=({ struct Cyc_Exp_tok_struct
_temp6814; _temp6814.tag= Cyc_Exp_tok; _temp6814.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(({
struct _tagged_ptr2 _temp6815= yyvs; void** _temp6817= _temp6815.curr +(
yyvsp_offset - 1); if( _temp6815.base == 0? 1:( _temp6817 < _temp6815.base? 1:
_temp6817 >= _temp6815.last_plus_one)){ _throw( Null_Exception);}* _temp6817;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6818= yyvs; void** _temp6820=
_temp6818.curr + yyvsp_offset; if( _temp6818.base == 0? 1:( _temp6820 <
_temp6818.base? 1: _temp6820 >= _temp6818.last_plus_one)){ _throw(
Null_Exception);}* _temp6820;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6821= yyls; struct Cyc_Yyltype* _temp6823= _temp6821.curr +(
yylsp_offset - 1); if( _temp6821.base == 0? 1:( _temp6823 < _temp6821.base? 1:
_temp6823 >= _temp6821.last_plus_one)){ _throw( Null_Exception);}* _temp6823;}).first_line,({
struct _tagged_ptr3 _temp6824= yyls; struct Cyc_Yyltype* _temp6826= _temp6824.curr
+ yylsp_offset; if( _temp6824.base == 0? 1:( _temp6826 < _temp6824.base? 1:
_temp6826 >= _temp6824.last_plus_one)){ _throw( Null_Exception);}* _temp6826;}).last_line));
_temp6814;}); _temp6813;}); break; case 348: _LL6812: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6828=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6828[ 0]=({ struct Cyc_Exp_tok_struct
_temp6829; _temp6829.tag= Cyc_Exp_tok; _temp6829.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6830= yyvs; void** _temp6832=
_temp6830.curr +( yyvsp_offset - 1); if( _temp6830.base == 0? 1:( _temp6832 <
_temp6830.base? 1: _temp6832 >= _temp6830.last_plus_one)){ _throw(
Null_Exception);}* _temp6832;}))).f3, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6833= yyls; struct Cyc_Yyltype* _temp6835= _temp6833.curr +(
yylsp_offset - 3); if( _temp6833.base == 0? 1:( _temp6835 < _temp6833.base? 1:
_temp6835 >= _temp6833.last_plus_one)){ _throw( Null_Exception);}* _temp6835;}).first_line,({
struct _tagged_ptr3 _temp6836= yyls; struct Cyc_Yyltype* _temp6838= _temp6836.curr
+ yylsp_offset; if( _temp6836.base == 0? 1:( _temp6838 < _temp6836.base? 1:
_temp6838 >= _temp6836.last_plus_one)){ _throw( Null_Exception);}* _temp6838;}).last_line));
_temp6829;}); _temp6828;}); break; case 349: _LL6827: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6840=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6840[ 0]=({ struct Cyc_Exp_tok_struct
_temp6841; _temp6841.tag= Cyc_Exp_tok; _temp6841.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6842= yyvs; void** _temp6844=
_temp6842.curr + yyvsp_offset; if( _temp6842.base == 0? 1:( _temp6844 <
_temp6842.base? 1: _temp6844 >= _temp6842.last_plus_one)){ _throw(
Null_Exception);}* _temp6844;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6845= yyls; struct Cyc_Yyltype* _temp6847= _temp6845.curr +(
yylsp_offset - 1); if( _temp6845.base == 0? 1:( _temp6847 < _temp6845.base? 1:
_temp6847 >= _temp6845.last_plus_one)){ _throw( Null_Exception);}* _temp6847;}).first_line,({
struct _tagged_ptr3 _temp6848= yyls; struct Cyc_Yyltype* _temp6850= _temp6848.curr
+ yylsp_offset; if( _temp6848.base == 0? 1:( _temp6850 < _temp6848.base? 1:
_temp6850 >= _temp6848.last_plus_one)){ _throw( Null_Exception);}* _temp6850;}).last_line));
_temp6841;}); _temp6840;}); break; case 350: _LL6839: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6852=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6852[ 0]=({ struct Cyc_Exp_tok_struct
_temp6853; _temp6853.tag= Cyc_Exp_tok; _temp6853.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(({
struct _tagged_ptr2 _temp6854= yyvs; void** _temp6856= _temp6854.curr +(
yyvsp_offset - 3); if( _temp6854.base == 0? 1:( _temp6856 < _temp6854.base? 1:
_temp6856 >= _temp6854.last_plus_one)){ _throw( Null_Exception);}* _temp6856;})),
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp6857= yyvs; void** _temp6859=
_temp6857.curr +( yyvsp_offset - 1); if( _temp6857.base == 0? 1:( _temp6859 <
_temp6857.base? 1: _temp6859 >= _temp6857.last_plus_one)){ _throw(
Null_Exception);}* _temp6859;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6860= yyls; struct Cyc_Yyltype* _temp6862= _temp6860.curr +(
yylsp_offset - 3); if( _temp6860.base == 0? 1:( _temp6862 < _temp6860.base? 1:
_temp6862 >= _temp6860.last_plus_one)){ _throw( Null_Exception);}* _temp6862;}).first_line,({
struct _tagged_ptr3 _temp6863= yyls; struct Cyc_Yyltype* _temp6865= _temp6863.curr
+ yylsp_offset; if( _temp6863.base == 0? 1:( _temp6865 < _temp6863.base? 1:
_temp6865 >= _temp6863.last_plus_one)){ _throw( Null_Exception);}* _temp6865;}).last_line));
_temp6853;}); _temp6852;}); break; case 351: _LL6851: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6867=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6867[ 0]=({ struct Cyc_Exp_tok_struct
_temp6868; _temp6868.tag= Cyc_Exp_tok; _temp6868.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp6869=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp6869[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp6870; _temp6870.tag= Cyc_Absyn_Malloc_e;
_temp6870.f1= 0; _temp6870.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp6871= yyvs; void** _temp6873= _temp6871.curr +(
yyvsp_offset - 2); if( _temp6871.base == 0? 1:( _temp6873 < _temp6871.base? 1:
_temp6873 >= _temp6871.last_plus_one)){ _throw( Null_Exception);}* _temp6873;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6874= yyls; struct Cyc_Yyltype*
_temp6876= _temp6874.curr +( yylsp_offset - 2); if( _temp6874.base == 0? 1:(
_temp6876 < _temp6874.base? 1: _temp6876 >= _temp6874.last_plus_one)){ _throw(
Null_Exception);}* _temp6876;}).first_line,({ struct _tagged_ptr3 _temp6877=
yyls; struct Cyc_Yyltype* _temp6879= _temp6877.curr +( yylsp_offset - 2); if(
_temp6877.base == 0? 1:( _temp6879 < _temp6877.base? 1: _temp6879 >= _temp6877.last_plus_one)){
_throw( Null_Exception);}* _temp6879;}).last_line)); _temp6870;}); _temp6869;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6880= yyls; struct Cyc_Yyltype*
_temp6882= _temp6880.curr +( yylsp_offset - 6); if( _temp6880.base == 0? 1:(
_temp6882 < _temp6880.base? 1: _temp6882 >= _temp6880.last_plus_one)){ _throw(
Null_Exception);}* _temp6882;}).first_line,({ struct _tagged_ptr3 _temp6883=
yyls; struct Cyc_Yyltype* _temp6885= _temp6883.curr + yylsp_offset; if(
_temp6883.base == 0? 1:( _temp6885 < _temp6883.base? 1: _temp6885 >= _temp6883.last_plus_one)){
_throw( Null_Exception);}* _temp6885;}).last_line)); _temp6868;}); _temp6867;});
break; case 352: _LL6866: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6887=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6887[ 0]=({ struct Cyc_Exp_tok_struct _temp6888; _temp6888.tag= Cyc_Exp_tok;
_temp6888.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6889=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6889[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6890; _temp6890.tag= Cyc_Absyn_Malloc_e;
_temp6890.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6891= yyvs; void** _temp6893= _temp6891.curr +( yyvsp_offset - 6); if(
_temp6891.base == 0? 1:( _temp6893 < _temp6891.base? 1: _temp6893 >= _temp6891.last_plus_one)){
_throw( Null_Exception);}* _temp6893;})); _temp6890.f2=( void*) Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp6894= yyvs; void**
_temp6896= _temp6894.curr +( yyvsp_offset - 2); if( _temp6894.base == 0? 1:(
_temp6896 < _temp6894.base? 1: _temp6896 >= _temp6894.last_plus_one)){ _throw(
Null_Exception);}* _temp6896;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6897= yyls; struct Cyc_Yyltype* _temp6899= _temp6897.curr +(
yylsp_offset - 2); if( _temp6897.base == 0? 1:( _temp6899 < _temp6897.base? 1:
_temp6899 >= _temp6897.last_plus_one)){ _throw( Null_Exception);}* _temp6899;}).first_line,({
struct _tagged_ptr3 _temp6900= yyls; struct Cyc_Yyltype* _temp6902= _temp6900.curr
+( yylsp_offset - 2); if( _temp6900.base == 0? 1:( _temp6902 < _temp6900.base? 1:
_temp6902 >= _temp6900.last_plus_one)){ _throw( Null_Exception);}* _temp6902;}).last_line));
_temp6890;}); _temp6889;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6903= yyls; struct Cyc_Yyltype* _temp6905= _temp6903.curr +( yylsp_offset -
8); if( _temp6903.base == 0? 1:( _temp6905 < _temp6903.base? 1: _temp6905 >=
_temp6903.last_plus_one)){ _throw( Null_Exception);}* _temp6905;}).first_line,({
struct _tagged_ptr3 _temp6906= yyls; struct Cyc_Yyltype* _temp6908= _temp6906.curr
+ yylsp_offset; if( _temp6906.base == 0? 1:( _temp6908 < _temp6906.base? 1:
_temp6908 >= _temp6906.last_plus_one)){ _throw( Null_Exception);}* _temp6908;}).last_line));
_temp6888;}); _temp6887;}); break; case 353: _LL6886: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6910=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6910[ 0]=({ struct Cyc_Primop_tok_struct
_temp6911; _temp6911.tag= Cyc_Primop_tok; _temp6911.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp6911;}); _temp6910;}); break; case 354: _LL6909: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6913=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6913[ 0]=({ struct Cyc_Primop_tok_struct
_temp6914; _temp6914.tag= Cyc_Primop_tok; _temp6914.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp6914;}); _temp6913;}); break; case 355: _LL6912: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6916=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6916[ 0]=({ struct Cyc_Primop_tok_struct
_temp6917; _temp6917.tag= Cyc_Primop_tok; _temp6917.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp6917;}); _temp6916;}); break; case 356: _LL6915: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6919=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6919[ 0]=({ struct Cyc_Primop_tok_struct
_temp6920; _temp6920.tag= Cyc_Primop_tok; _temp6920.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp6920;}); _temp6919;}); break; case 357: _LL6918: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6922=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6922[ 0]=({ struct Cyc_Primop_tok_struct
_temp6923; _temp6923.tag= Cyc_Primop_tok; _temp6923.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp6923;}); _temp6922;}); break; case 358: _LL6921: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6925=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6925[ 0]=({ struct Cyc_Primop_tok_struct
_temp6926; _temp6926.tag= Cyc_Primop_tok; _temp6926.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp6926;}); _temp6925;}); break; case 359: _LL6924: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6928=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6928[ 0]=({ struct Cyc_Primop_tok_struct
_temp6929; _temp6929.tag= Cyc_Primop_tok; _temp6929.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp6929;}); _temp6928;}); break; case 360: _LL6927: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6931=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6931[ 0]=({ struct Cyc_Primop_tok_struct
_temp6932; _temp6932.tag= Cyc_Primop_tok; _temp6932.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp6932;}); _temp6931;}); break; case 361: _LL6930: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6934=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6934[ 0]=({ struct Cyc_Primop_tok_struct
_temp6935; _temp6935.tag= Cyc_Primop_tok; _temp6935.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp6935;}); _temp6934;}); break; case 362: _LL6933: yyval=({ struct
_tagged_ptr2 _temp6937= yyvs; void** _temp6939= _temp6937.curr + yyvsp_offset;
if( _temp6937.base == 0? 1:( _temp6939 < _temp6937.base? 1: _temp6939 >=
_temp6937.last_plus_one)){ _throw( Null_Exception);}* _temp6939;}); break; case
363: _LL6936: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6941=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6941[ 0]=({ struct Cyc_Exp_tok_struct
_temp6942; _temp6942.tag= Cyc_Exp_tok; _temp6942.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6943= yyvs; void** _temp6945=
_temp6943.curr +( yyvsp_offset - 3); if( _temp6943.base == 0? 1:( _temp6945 <
_temp6943.base? 1: _temp6945 >= _temp6943.last_plus_one)){ _throw(
Null_Exception);}* _temp6945;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6946= yyvs; void** _temp6948= _temp6946.curr +( yyvsp_offset - 1); if(
_temp6946.base == 0? 1:( _temp6948 < _temp6946.base? 1: _temp6948 >= _temp6946.last_plus_one)){
_throw( Null_Exception);}* _temp6948;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6949= yyls; struct Cyc_Yyltype* _temp6951= _temp6949.curr +(
yylsp_offset - 3); if( _temp6949.base == 0? 1:( _temp6951 < _temp6949.base? 1:
_temp6951 >= _temp6949.last_plus_one)){ _throw( Null_Exception);}* _temp6951;}).first_line,({
struct _tagged_ptr3 _temp6952= yyls; struct Cyc_Yyltype* _temp6954= _temp6952.curr
+ yylsp_offset; if( _temp6952.base == 0? 1:( _temp6954 < _temp6952.base? 1:
_temp6954 >= _temp6952.last_plus_one)){ _throw( Null_Exception);}* _temp6954;}).last_line));
_temp6942;}); _temp6941;}); break; case 364: _LL6940: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6956=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6956[ 0]=({ struct Cyc_Exp_tok_struct
_temp6957; _temp6957.tag= Cyc_Exp_tok; _temp6957.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6958= yyvs; void** _temp6960=
_temp6958.curr +( yyvsp_offset - 2); if( _temp6958.base == 0? 1:( _temp6960 <
_temp6958.base? 1: _temp6960 >= _temp6958.last_plus_one)){ _throw(
Null_Exception);}* _temp6960;})), 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6961= yyls; struct Cyc_Yyltype* _temp6963= _temp6961.curr +(
yylsp_offset - 2); if( _temp6961.base == 0? 1:( _temp6963 < _temp6961.base? 1:
_temp6963 >= _temp6961.last_plus_one)){ _throw( Null_Exception);}* _temp6963;}).first_line,({
struct _tagged_ptr3 _temp6964= yyls; struct Cyc_Yyltype* _temp6966= _temp6964.curr
+ yylsp_offset; if( _temp6964.base == 0? 1:( _temp6966 < _temp6964.base? 1:
_temp6966 >= _temp6964.last_plus_one)){ _throw( Null_Exception);}* _temp6966;}).last_line));
_temp6957;}); _temp6956;}); break; case 365: _LL6955: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6968=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6968[ 0]=({ struct Cyc_Exp_tok_struct
_temp6969; _temp6969.tag= Cyc_Exp_tok; _temp6969.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6970= yyvs; void** _temp6972=
_temp6970.curr +( yyvsp_offset - 3); if( _temp6970.base == 0? 1:( _temp6972 <
_temp6970.base? 1: _temp6972 >= _temp6970.last_plus_one)){ _throw(
Null_Exception);}* _temp6972;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp6973= yyvs; void** _temp6975= _temp6973.curr +( yyvsp_offset - 1); if(
_temp6973.base == 0? 1:( _temp6975 < _temp6973.base? 1: _temp6975 >= _temp6973.last_plus_one)){
_throw( Null_Exception);}* _temp6975;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6976= yyls; struct Cyc_Yyltype* _temp6978= _temp6976.curr +(
yylsp_offset - 3); if( _temp6976.base == 0? 1:( _temp6978 < _temp6976.base? 1:
_temp6978 >= _temp6976.last_plus_one)){ _throw( Null_Exception);}* _temp6978;}).first_line,({
struct _tagged_ptr3 _temp6979= yyls; struct Cyc_Yyltype* _temp6981= _temp6979.curr
+ yylsp_offset; if( _temp6979.base == 0? 1:( _temp6981 < _temp6979.base? 1:
_temp6981 >= _temp6979.last_plus_one)){ _throw( Null_Exception);}* _temp6981;}).last_line));
_temp6969;}); _temp6968;}); break; case 366: _LL6967: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6983=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6983[ 0]=({ struct Cyc_Exp_tok_struct
_temp6984; _temp6984.tag= Cyc_Exp_tok; _temp6984.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6985= yyvs; void** _temp6987=
_temp6985.curr +( yyvsp_offset - 2); if( _temp6985.base == 0? 1:( _temp6987 <
_temp6985.base? 1: _temp6987 >= _temp6985.last_plus_one)){ _throw(
Null_Exception);}* _temp6987;})),({ struct _tagged_string* _temp6988=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp6988[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6989= yyvs; void** _temp6991=
_temp6989.curr + yyvsp_offset; if( _temp6989.base == 0? 1:( _temp6991 <
_temp6989.base? 1: _temp6991 >= _temp6989.last_plus_one)){ _throw(
Null_Exception);}* _temp6991;})); _temp6988;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6992= yyls; struct Cyc_Yyltype* _temp6994= _temp6992.curr
+( yylsp_offset - 2); if( _temp6992.base == 0? 1:( _temp6994 < _temp6992.base? 1:
_temp6994 >= _temp6992.last_plus_one)){ _throw( Null_Exception);}* _temp6994;}).first_line,({
struct _tagged_ptr3 _temp6995= yyls; struct Cyc_Yyltype* _temp6997= _temp6995.curr
+ yylsp_offset; if( _temp6995.base == 0? 1:( _temp6997 < _temp6995.base? 1:
_temp6997 >= _temp6995.last_plus_one)){ _throw( Null_Exception);}* _temp6997;}).last_line));
_temp6984;}); _temp6983;}); break; case 367: _LL6982: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp7032= yyvs; void** _temp7034= _temp7032.curr +
yyvsp_offset; if( _temp7032.base == 0? 1:( _temp7034 < _temp7032.base? 1:
_temp7034 >= _temp7032.last_plus_one)){ _throw( Null_Exception);}* _temp7034;}));{
void* _temp6999=(* q).f1; struct Cyc_List_List* _temp7009; struct Cyc_List_List*
_temp7011; _LL7001: if( _temp6999 ==( void*) Cyc_Absyn_Loc_n){ goto _LL7002;}
else{ goto _LL7003;} _LL7003: if(( unsigned int) _temp6999 > 1u?*(( int*)
_temp6999) == Cyc_Absyn_Rel_n: 0){ _LL7010: _temp7009=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp6999)->f1; if( _temp7009 == 0){ goto
_LL7004;} else{ goto _LL7005;}} else{ goto _LL7005;} _LL7005: if(( unsigned int)
_temp6999 > 1u?*(( int*) _temp6999) == Cyc_Absyn_Abs_n: 0){ _LL7012: _temp7011=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp6999)->f1; if(
_temp7011 == 0){ goto _LL7006;} else{ goto _LL7007;}} else{ goto _LL7007;}
_LL7007: goto _LL7008; _LL7002: goto _LL7000; _LL7004: goto _LL7000; _LL7006:
goto _LL7000; _LL7008: Cyc_Parse_err(( struct _tagged_string)({ char* _temp7013=(
char*)"struct field name is qualified"; struct _tagged_string _temp7014;
_temp7014.curr= _temp7013; _temp7014.base= _temp7013; _temp7014.last_plus_one=
_temp7013 + 31; _temp7014;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7015= yyls; struct Cyc_Yyltype* _temp7017= _temp7015.curr + yylsp_offset;
if( _temp7015.base == 0? 1:( _temp7017 < _temp7015.base? 1: _temp7017 >=
_temp7015.last_plus_one)){ _throw( Null_Exception);}* _temp7017;}).first_line,({
struct _tagged_ptr3 _temp7018= yyls; struct Cyc_Yyltype* _temp7020= _temp7018.curr
+ yylsp_offset; if( _temp7018.base == 0? 1:( _temp7020 < _temp7018.base? 1:
_temp7020 >= _temp7018.last_plus_one)){ _throw( Null_Exception);}* _temp7020;}).last_line));
goto _LL7000; _LL7000:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7021=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7021[ 0]=({ struct Cyc_Exp_tok_struct _temp7022; _temp7022.tag= Cyc_Exp_tok;
_temp7022.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7023= yyvs; void** _temp7025= _temp7023.curr +( yyvsp_offset -
2); if( _temp7023.base == 0? 1:( _temp7025 < _temp7023.base? 1: _temp7025 >=
_temp7023.last_plus_one)){ _throw( Null_Exception);}* _temp7025;})),(* q).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7026= yyls; struct Cyc_Yyltype*
_temp7028= _temp7026.curr +( yylsp_offset - 2); if( _temp7026.base == 0? 1:(
_temp7028 < _temp7026.base? 1: _temp7028 >= _temp7026.last_plus_one)){ _throw(
Null_Exception);}* _temp7028;}).first_line,({ struct _tagged_ptr3 _temp7029=
yyls; struct Cyc_Yyltype* _temp7031= _temp7029.curr + yylsp_offset; if(
_temp7029.base == 0? 1:( _temp7031 < _temp7029.base? 1: _temp7031 >= _temp7029.last_plus_one)){
_throw( Null_Exception);}* _temp7031;}).last_line)); _temp7022;}); _temp7021;});
break;} case 368: _LL6998: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7036=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7036[ 0]=({ struct Cyc_Exp_tok_struct _temp7037; _temp7037.tag= Cyc_Exp_tok;
_temp7037.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7038= yyvs; void** _temp7040= _temp7038.curr +( yyvsp_offset -
2); if( _temp7038.base == 0? 1:( _temp7040 < _temp7038.base? 1: _temp7040 >=
_temp7038.last_plus_one)){ _throw( Null_Exception);}* _temp7040;})),({ struct
_tagged_string* _temp7041=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp7041[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7042= yyvs; void** _temp7044= _temp7042.curr + yyvsp_offset; if( _temp7042.base
== 0? 1:( _temp7044 < _temp7042.base? 1: _temp7044 >= _temp7042.last_plus_one)){
_throw( Null_Exception);}* _temp7044;})); _temp7041;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7045= yyls; struct Cyc_Yyltype* _temp7047= _temp7045.curr
+( yylsp_offset - 2); if( _temp7045.base == 0? 1:( _temp7047 < _temp7045.base? 1:
_temp7047 >= _temp7045.last_plus_one)){ _throw( Null_Exception);}* _temp7047;}).first_line,({
struct _tagged_ptr3 _temp7048= yyls; struct Cyc_Yyltype* _temp7050= _temp7048.curr
+ yylsp_offset; if( _temp7048.base == 0? 1:( _temp7050 < _temp7048.base? 1:
_temp7050 >= _temp7048.last_plus_one)){ _throw( Null_Exception);}* _temp7050;}).last_line));
_temp7037;}); _temp7036;}); break; case 369: _LL7035: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp7085= yyvs; void** _temp7087= _temp7085.curr +
yyvsp_offset; if( _temp7085.base == 0? 1:( _temp7087 < _temp7085.base? 1:
_temp7087 >= _temp7085.last_plus_one)){ _throw( Null_Exception);}* _temp7087;}));{
void* _temp7052=(* q).f1; struct Cyc_List_List* _temp7062; struct Cyc_List_List*
_temp7064; _LL7054: if( _temp7052 ==( void*) Cyc_Absyn_Loc_n){ goto _LL7055;}
else{ goto _LL7056;} _LL7056: if(( unsigned int) _temp7052 > 1u?*(( int*)
_temp7052) == Cyc_Absyn_Rel_n: 0){ _LL7063: _temp7062=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp7052)->f1; if( _temp7062 == 0){ goto
_LL7057;} else{ goto _LL7058;}} else{ goto _LL7058;} _LL7058: if(( unsigned int)
_temp7052 > 1u?*(( int*) _temp7052) == Cyc_Absyn_Abs_n: 0){ _LL7065: _temp7064=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp7052)->f1; if(
_temp7064 == 0){ goto _LL7059;} else{ goto _LL7060;}} else{ goto _LL7060;}
_LL7060: goto _LL7061; _LL7055: goto _LL7053; _LL7057: goto _LL7053; _LL7059:
goto _LL7053; _LL7061: Cyc_Parse_err(( struct _tagged_string)({ char* _temp7066=(
char*)"struct field is qualified with module name"; struct _tagged_string
_temp7067; _temp7067.curr= _temp7066; _temp7067.base= _temp7066; _temp7067.last_plus_one=
_temp7066 + 43; _temp7067;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7068= yyls; struct Cyc_Yyltype* _temp7070= _temp7068.curr + yylsp_offset;
if( _temp7068.base == 0? 1:( _temp7070 < _temp7068.base? 1: _temp7070 >=
_temp7068.last_plus_one)){ _throw( Null_Exception);}* _temp7070;}).first_line,({
struct _tagged_ptr3 _temp7071= yyls; struct Cyc_Yyltype* _temp7073= _temp7071.curr
+ yylsp_offset; if( _temp7071.base == 0? 1:( _temp7073 < _temp7071.base? 1:
_temp7073 >= _temp7071.last_plus_one)){ _throw( Null_Exception);}* _temp7073;}).last_line));
goto _LL7053; _LL7053:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7074=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7074[ 0]=({ struct Cyc_Exp_tok_struct _temp7075; _temp7075.tag= Cyc_Exp_tok;
_temp7075.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7076= yyvs; void** _temp7078= _temp7076.curr +( yyvsp_offset -
2); if( _temp7076.base == 0? 1:( _temp7078 < _temp7076.base? 1: _temp7078 >=
_temp7076.last_plus_one)){ _throw( Null_Exception);}* _temp7078;})),(* q).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7079= yyls; struct Cyc_Yyltype*
_temp7081= _temp7079.curr +( yylsp_offset - 2); if( _temp7079.base == 0? 1:(
_temp7081 < _temp7079.base? 1: _temp7081 >= _temp7079.last_plus_one)){ _throw(
Null_Exception);}* _temp7081;}).first_line,({ struct _tagged_ptr3 _temp7082=
yyls; struct Cyc_Yyltype* _temp7084= _temp7082.curr + yylsp_offset; if(
_temp7082.base == 0? 1:( _temp7084 < _temp7082.base? 1: _temp7084 >= _temp7082.last_plus_one)){
_throw( Null_Exception);}* _temp7084;}).last_line)); _temp7075;}); _temp7074;});
break;} case 370: _LL7051: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7089=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7089[ 0]=({ struct Cyc_Exp_tok_struct _temp7090; _temp7090.tag= Cyc_Exp_tok;
_temp7090.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp7091= yyvs; void** _temp7093= _temp7091.curr +( yyvsp_offset - 1); if(
_temp7091.base == 0? 1:( _temp7093 < _temp7091.base? 1: _temp7093 >= _temp7091.last_plus_one)){
_throw( Null_Exception);}* _temp7093;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7094= yyls; struct Cyc_Yyltype* _temp7096= _temp7094.curr +(
yylsp_offset - 1); if( _temp7094.base == 0? 1:( _temp7096 < _temp7094.base? 1:
_temp7096 >= _temp7094.last_plus_one)){ _throw( Null_Exception);}* _temp7096;}).first_line,({
struct _tagged_ptr3 _temp7097= yyls; struct Cyc_Yyltype* _temp7099= _temp7097.curr
+ yylsp_offset; if( _temp7097.base == 0? 1:( _temp7099 < _temp7097.base? 1:
_temp7099 >= _temp7097.last_plus_one)){ _throw( Null_Exception);}* _temp7099;}).last_line));
_temp7090;}); _temp7089;}); break; case 371: _LL7088: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7101=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7101[ 0]=({ struct Cyc_Exp_tok_struct
_temp7102; _temp7102.tag= Cyc_Exp_tok; _temp7102.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7103= yyvs; void** _temp7105= _temp7103.curr +(
yyvsp_offset - 1); if( _temp7103.base == 0? 1:( _temp7105 < _temp7103.base? 1:
_temp7105 >= _temp7103.last_plus_one)){ _throw( Null_Exception);}* _temp7105;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7106= yyls; struct Cyc_Yyltype*
_temp7108= _temp7106.curr +( yylsp_offset - 1); if( _temp7106.base == 0? 1:(
_temp7108 < _temp7106.base? 1: _temp7108 >= _temp7106.last_plus_one)){ _throw(
Null_Exception);}* _temp7108;}).first_line,({ struct _tagged_ptr3 _temp7109=
yyls; struct Cyc_Yyltype* _temp7111= _temp7109.curr + yylsp_offset; if(
_temp7109.base == 0? 1:( _temp7111 < _temp7109.base? 1: _temp7111 >= _temp7109.last_plus_one)){
_throw( Null_Exception);}* _temp7111;}).last_line)); _temp7102;}); _temp7101;});
break; case 372: _LL7100: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7113=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7113[ 0]=({ struct Cyc_Exp_tok_struct _temp7114; _temp7114.tag= Cyc_Exp_tok;
_temp7114.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_CompoundLit_e_struct*
_temp7115=( struct Cyc_Absyn_CompoundLit_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_CompoundLit_e_struct)); _temp7115[ 0]=({ struct Cyc_Absyn_CompoundLit_e_struct
_temp7116; _temp7116.tag= Cyc_Absyn_CompoundLit_e; _temp7116.f1= Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp7117= yyvs; void** _temp7119= _temp7117.curr +(
yyvsp_offset - 4); if( _temp7117.base == 0? 1:( _temp7119 < _temp7117.base? 1:
_temp7119 >= _temp7117.last_plus_one)){ _throw( Null_Exception);}* _temp7119;}));
_temp7116.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp7120= yyvs; void**
_temp7122= _temp7120.curr +( yyvsp_offset - 1); if( _temp7120.base == 0? 1:(
_temp7122 < _temp7120.base? 1: _temp7122 >= _temp7120.last_plus_one)){ _throw(
Null_Exception);}* _temp7122;}))); _temp7116;}); _temp7115;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7123= yyls; struct Cyc_Yyltype* _temp7125= _temp7123.curr
+( yylsp_offset - 5); if( _temp7123.base == 0? 1:( _temp7125 < _temp7123.base? 1:
_temp7125 >= _temp7123.last_plus_one)){ _throw( Null_Exception);}* _temp7125;}).first_line,({
struct _tagged_ptr3 _temp7126= yyls; struct Cyc_Yyltype* _temp7128= _temp7126.curr
+ yylsp_offset; if( _temp7126.base == 0? 1:( _temp7128 < _temp7126.base? 1:
_temp7128 >= _temp7126.last_plus_one)){ _throw( Null_Exception);}* _temp7128;}).last_line));
_temp7114;}); _temp7113;}); break; case 373: _LL7112: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7130=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7130[ 0]=({ struct Cyc_Exp_tok_struct
_temp7131; _temp7131.tag= Cyc_Exp_tok; _temp7131.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp7132=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7132[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7133; _temp7133.tag= Cyc_Absyn_CompoundLit_e;
_temp7133.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp7134= yyvs;
void** _temp7136= _temp7134.curr +( yyvsp_offset - 5); if( _temp7134.base == 0?
1:( _temp7136 < _temp7134.base? 1: _temp7136 >= _temp7134.last_plus_one)){
_throw( Null_Exception);}* _temp7136;})); _temp7133.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7137= yyvs; void** _temp7139= _temp7137.curr +(
yyvsp_offset - 2); if( _temp7137.base == 0? 1:( _temp7139 < _temp7137.base? 1:
_temp7139 >= _temp7137.last_plus_one)){ _throw( Null_Exception);}* _temp7139;})));
_temp7133;}); _temp7132;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7140= yyls; struct Cyc_Yyltype* _temp7142= _temp7140.curr +( yylsp_offset -
6); if( _temp7140.base == 0? 1:( _temp7142 < _temp7140.base? 1: _temp7142 >=
_temp7140.last_plus_one)){ _throw( Null_Exception);}* _temp7142;}).first_line,({
struct _tagged_ptr3 _temp7143= yyls; struct Cyc_Yyltype* _temp7145= _temp7143.curr
+ yylsp_offset; if( _temp7143.base == 0? 1:( _temp7145 < _temp7143.base? 1:
_temp7145 >= _temp7143.last_plus_one)){ _throw( Null_Exception);}* _temp7145;}).last_line));
_temp7131;}); _temp7130;}); break; case 374: _LL7129: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7147=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7147[ 0]=({ struct Cyc_Exp_tok_struct
_temp7148; _temp7148.tag= Cyc_Exp_tok; _temp7148.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp7149=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp7149[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp7150; _temp7150.tag= Cyc_Absyn_Fill_e; _temp7150.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7151= yyvs; void** _temp7153= _temp7151.curr +(
yyvsp_offset - 1); if( _temp7151.base == 0? 1:( _temp7153 < _temp7151.base? 1:
_temp7153 >= _temp7151.last_plus_one)){ _throw( Null_Exception);}* _temp7153;}));
_temp7150;}); _temp7149;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7154= yyls; struct Cyc_Yyltype* _temp7156= _temp7154.curr +( yylsp_offset -
3); if( _temp7154.base == 0? 1:( _temp7156 < _temp7154.base? 1: _temp7156 >=
_temp7154.last_plus_one)){ _throw( Null_Exception);}* _temp7156;}).first_line,({
struct _tagged_ptr3 _temp7157= yyls; struct Cyc_Yyltype* _temp7159= _temp7157.curr
+ yylsp_offset; if( _temp7157.base == 0? 1:( _temp7159 < _temp7157.base? 1:
_temp7159 >= _temp7157.last_plus_one)){ _throw( Null_Exception);}* _temp7159;}).last_line));
_temp7148;}); _temp7147;}); break; case 375: _LL7146: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7161=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7161[ 0]=({ struct Cyc_Exp_tok_struct
_temp7162; _temp7162.tag= Cyc_Exp_tok; _temp7162.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp7163=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp7163[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp7164; _temp7164.tag= Cyc_Absyn_Codegen_e;
_temp7164.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp7165= yyvs; void**
_temp7167= _temp7165.curr +( yyvsp_offset - 1); if( _temp7165.base == 0? 1:(
_temp7167 < _temp7165.base? 1: _temp7167 >= _temp7165.last_plus_one)){ _throw(
Null_Exception);}* _temp7167;})); _temp7164;}); _temp7163;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7168= yyls; struct Cyc_Yyltype* _temp7170= _temp7168.curr
+( yylsp_offset - 3); if( _temp7168.base == 0? 1:( _temp7170 < _temp7168.base? 1:
_temp7170 >= _temp7168.last_plus_one)){ _throw( Null_Exception);}* _temp7170;}).first_line,({
struct _tagged_ptr3 _temp7171= yyls; struct Cyc_Yyltype* _temp7173= _temp7171.curr
+ yylsp_offset; if( _temp7171.base == 0? 1:( _temp7173 < _temp7171.base? 1:
_temp7173 >= _temp7171.last_plus_one)){ _throw( Null_Exception);}* _temp7173;}).last_line));
_temp7162;}); _temp7161;}); break; case 376: _LL7160: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7175=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7175[ 0]=({ struct Cyc_Exp_tok_struct
_temp7176; _temp7176.tag= Cyc_Exp_tok; _temp7176.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp7177=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp7177[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp7178; _temp7178.tag= Cyc_Absyn_UnknownId_e;
_temp7178.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7179= yyvs; void**
_temp7181= _temp7179.curr + yyvsp_offset; if( _temp7179.base == 0? 1:( _temp7181
< _temp7179.base? 1: _temp7181 >= _temp7179.last_plus_one)){ _throw(
Null_Exception);}* _temp7181;})); _temp7178;}); _temp7177;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7182= yyls; struct Cyc_Yyltype* _temp7184= _temp7182.curr
+ yylsp_offset; if( _temp7182.base == 0? 1:( _temp7184 < _temp7182.base? 1:
_temp7184 >= _temp7182.last_plus_one)){ _throw( Null_Exception);}* _temp7184;}).first_line,({
struct _tagged_ptr3 _temp7185= yyls; struct Cyc_Yyltype* _temp7187= _temp7185.curr
+ yylsp_offset; if( _temp7185.base == 0? 1:( _temp7187 < _temp7185.base? 1:
_temp7187 >= _temp7185.last_plus_one)){ _throw( Null_Exception);}* _temp7187;}).last_line));
_temp7176;}); _temp7175;}); break; case 377: _LL7174: yyval=({ struct
_tagged_ptr2 _temp7189= yyvs; void** _temp7191= _temp7189.curr + yyvsp_offset;
if( _temp7189.base == 0? 1:( _temp7191 < _temp7189.base? 1: _temp7191 >=
_temp7189.last_plus_one)){ _throw( Null_Exception);}* _temp7191;}); break; case
378: _LL7188: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7193=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7193[ 0]=({ struct Cyc_Exp_tok_struct
_temp7194; _temp7194.tag= Cyc_Exp_tok; _temp7194.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp7195= yyvs; void** _temp7197= _temp7195.curr +
yyvsp_offset; if( _temp7195.base == 0? 1:( _temp7197 < _temp7195.base? 1:
_temp7197 >= _temp7195.last_plus_one)){ _throw( Null_Exception);}* _temp7197;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7198= yyls; struct Cyc_Yyltype*
_temp7200= _temp7198.curr + yylsp_offset; if( _temp7198.base == 0? 1:( _temp7200
< _temp7198.base? 1: _temp7200 >= _temp7198.last_plus_one)){ _throw(
Null_Exception);}* _temp7200;}).first_line,({ struct _tagged_ptr3 _temp7201=
yyls; struct Cyc_Yyltype* _temp7203= _temp7201.curr + yylsp_offset; if(
_temp7201.base == 0? 1:( _temp7203 < _temp7201.base? 1: _temp7203 >= _temp7201.last_plus_one)){
_throw( Null_Exception);}* _temp7203;}).last_line)); _temp7194;}); _temp7193;});
break; case 379: _LL7192: yyval=({ struct _tagged_ptr2 _temp7205= yyvs; void**
_temp7207= _temp7205.curr +( yyvsp_offset - 1); if( _temp7205.base == 0? 1:(
_temp7207 < _temp7205.base? 1: _temp7207 >= _temp7205.last_plus_one)){ _throw(
Null_Exception);}* _temp7207;}); break; case 380: _LL7204: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp7209=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp7209[ 0]=({ struct Cyc_Exp_tok_struct
_temp7210; _temp7210.tag= Cyc_Exp_tok; _temp7210.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7211=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7211[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7212; _temp7212.tag=
Cyc_Absyn_UnknownId_e; _temp7212.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp7213= yyvs; void** _temp7215= _temp7213.curr +( yyvsp_offset - 1); if(
_temp7213.base == 0? 1:( _temp7215 < _temp7213.base? 1: _temp7215 >= _temp7213.last_plus_one)){
_throw( Null_Exception);}* _temp7215;})); _temp7212;}); _temp7211;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7216= yyls; struct Cyc_Yyltype* _temp7218= _temp7216.curr
+( yylsp_offset - 1); if( _temp7216.base == 0? 1:( _temp7218 < _temp7216.base? 1:
_temp7218 >= _temp7216.last_plus_one)){ _throw( Null_Exception);}* _temp7218;}).first_line,({
struct _tagged_ptr3 _temp7219= yyls; struct Cyc_Yyltype* _temp7221= _temp7219.curr
+( yylsp_offset - 1); if( _temp7219.base == 0? 1:( _temp7221 < _temp7219.base? 1:
_temp7221 >= _temp7219.last_plus_one)){ _throw( Null_Exception);}* _temp7221;}).last_line)),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7222= yyls; struct Cyc_Yyltype*
_temp7224= _temp7222.curr +( yylsp_offset - 1); if( _temp7222.base == 0? 1:(
_temp7224 < _temp7222.base? 1: _temp7224 >= _temp7222.last_plus_one)){ _throw(
Null_Exception);}* _temp7224;}).first_line,({ struct _tagged_ptr3 _temp7225=
yyls; struct Cyc_Yyltype* _temp7227= _temp7225.curr + yylsp_offset; if(
_temp7225.base == 0? 1:( _temp7227 < _temp7225.base? 1: _temp7227 >= _temp7225.last_plus_one)){
_throw( Null_Exception);}* _temp7227;}).last_line)); _temp7210;}); _temp7209;});
break; case 381: _LL7208: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7229=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7229[ 0]=({ struct Cyc_Exp_tok_struct _temp7230; _temp7230.tag= Cyc_Exp_tok;
_temp7230.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp7231=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7231[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7232; _temp7232.tag=
Cyc_Absyn_UnknownId_e; _temp7232.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp7233= yyvs; void** _temp7235= _temp7233.curr +( yyvsp_offset - 4); if(
_temp7233.base == 0? 1:( _temp7235 < _temp7233.base? 1: _temp7235 >= _temp7233.last_plus_one)){
_throw( Null_Exception);}* _temp7235;})); _temp7232;}); _temp7231;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7236= yyls; struct Cyc_Yyltype* _temp7238= _temp7236.curr
+( yylsp_offset - 4); if( _temp7236.base == 0? 1:( _temp7238 < _temp7236.base? 1:
_temp7238 >= _temp7236.last_plus_one)){ _throw( Null_Exception);}* _temp7238;}).first_line,({
struct _tagged_ptr3 _temp7239= yyls; struct Cyc_Yyltype* _temp7241= _temp7239.curr
+( yylsp_offset - 4); if( _temp7239.base == 0? 1:( _temp7241 < _temp7239.base? 1:
_temp7241 >= _temp7239.last_plus_one)){ _throw( Null_Exception);}* _temp7241;}).last_line)),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp7242= yyvs; void** _temp7244= _temp7242.curr +(
yyvsp_offset - 1); if( _temp7242.base == 0? 1:( _temp7244 < _temp7242.base? 1:
_temp7244 >= _temp7242.last_plus_one)){ _throw( Null_Exception);}* _temp7244;}))),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7245= yyls; struct Cyc_Yyltype*
_temp7247= _temp7245.curr +( yylsp_offset - 4); if( _temp7245.base == 0? 1:(
_temp7247 < _temp7245.base? 1: _temp7247 >= _temp7245.last_plus_one)){ _throw(
Null_Exception);}* _temp7247;}).first_line,({ struct _tagged_ptr3 _temp7248=
yyls; struct Cyc_Yyltype* _temp7250= _temp7248.curr + yylsp_offset; if(
_temp7248.base == 0? 1:( _temp7250 < _temp7248.base? 1: _temp7250 >= _temp7248.last_plus_one)){
_throw( Null_Exception);}* _temp7250;}).last_line)); _temp7230;}); _temp7229;});
break; case 382: _LL7228: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7252=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7252[ 0]=({ struct Cyc_Exp_tok_struct _temp7253; _temp7253.tag= Cyc_Exp_tok;
_temp7253.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp7254= yyvs; void** _temp7256= _temp7254.curr +( yyvsp_offset - 1); if(
_temp7254.base == 0? 1:( _temp7256 < _temp7254.base? 1: _temp7256 >= _temp7254.last_plus_one)){
_throw( Null_Exception);}* _temp7256;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7257= yyls; struct Cyc_Yyltype* _temp7259= _temp7257.curr +(
yylsp_offset - 3); if( _temp7257.base == 0? 1:( _temp7259 < _temp7257.base? 1:
_temp7259 >= _temp7257.last_plus_one)){ _throw( Null_Exception);}* _temp7259;}).first_line,({
struct _tagged_ptr3 _temp7260= yyls; struct Cyc_Yyltype* _temp7262= _temp7260.curr
+ yylsp_offset; if( _temp7260.base == 0? 1:( _temp7262 < _temp7260.base? 1:
_temp7262 >= _temp7260.last_plus_one)){ _throw( Null_Exception);}* _temp7262;}).last_line));
_temp7253;}); _temp7252;}); break; case 383: _LL7251: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7264=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7264[ 0]=({ struct Cyc_Exp_tok_struct
_temp7265; _temp7265.tag= Cyc_Exp_tok; _temp7265.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp7266=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp7266[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp7267; _temp7267.tag= Cyc_Absyn_Struct_e;
_temp7267.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7268= yyvs; void**
_temp7270= _temp7268.curr +( yyvsp_offset - 3); if( _temp7268.base == 0? 1:(
_temp7270 < _temp7268.base? 1: _temp7270 >= _temp7268.last_plus_one)){ _throw(
Null_Exception);}* _temp7270;})); _temp7267.f2= 0; _temp7267.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7271= yyvs; void** _temp7273= _temp7271.curr +(
yyvsp_offset - 1); if( _temp7271.base == 0? 1:( _temp7273 < _temp7271.base? 1:
_temp7273 >= _temp7271.last_plus_one)){ _throw( Null_Exception);}* _temp7273;})));
_temp7267.f4= 0; _temp7267;}); _temp7266;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7274= yyls; struct Cyc_Yyltype* _temp7276= _temp7274.curr
+( yylsp_offset - 3); if( _temp7274.base == 0? 1:( _temp7276 < _temp7274.base? 1:
_temp7276 >= _temp7274.last_plus_one)){ _throw( Null_Exception);}* _temp7276;}).first_line,({
struct _tagged_ptr3 _temp7277= yyls; struct Cyc_Yyltype* _temp7279= _temp7277.curr
+ yylsp_offset; if( _temp7277.base == 0? 1:( _temp7279 < _temp7277.base? 1:
_temp7279 >= _temp7277.last_plus_one)){ _throw( Null_Exception);}* _temp7279;}).last_line));
_temp7265;}); _temp7264;}); break; case 384: _LL7263: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7281=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7281[ 0]=({ struct Cyc_Exp_tok_struct
_temp7282; _temp7282.tag= Cyc_Exp_tok; _temp7282.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp7283= yyvs; void** _temp7285= _temp7283.curr +(
yyvsp_offset - 2); if( _temp7283.base == 0? 1:( _temp7285 < _temp7283.base? 1:
_temp7285 >= _temp7283.last_plus_one)){ _throw( Null_Exception);}* _temp7285;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7286= yyls; struct Cyc_Yyltype*
_temp7288= _temp7286.curr +( yylsp_offset - 4); if( _temp7286.base == 0? 1:(
_temp7288 < _temp7286.base? 1: _temp7288 >= _temp7286.last_plus_one)){ _throw(
Null_Exception);}* _temp7288;}).first_line,({ struct _tagged_ptr3 _temp7289=
yyls; struct Cyc_Yyltype* _temp7291= _temp7289.curr + yylsp_offset; if(
_temp7289.base == 0? 1:( _temp7291 < _temp7289.base? 1: _temp7291 >= _temp7289.last_plus_one)){
_throw( Null_Exception);}* _temp7291;}).last_line)); _temp7282;}); _temp7281;});
break; case 385: _LL7280: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp7293=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7293[ 0]=({ struct Cyc_ExpList_tok_struct _temp7294; _temp7294.tag= Cyc_ExpList_tok;
_temp7294.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7295= yyvs; void** _temp7297=
_temp7295.curr + yyvsp_offset; if( _temp7295.base == 0? 1:( _temp7297 <
_temp7295.base? 1: _temp7297 >= _temp7295.last_plus_one)){ _throw(
Null_Exception);}* _temp7297;}))); _temp7294;}); _temp7293;}); break; case 386:
_LL7292: yyval=( void*)({ struct Cyc_ExpList_tok_struct* _temp7299=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7299[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7300; _temp7300.tag= Cyc_ExpList_tok; _temp7300.f1=({ struct Cyc_List_List*
_temp7301=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7301->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7302= yyvs;
void** _temp7304= _temp7302.curr + yyvsp_offset; if( _temp7302.base == 0? 1:(
_temp7304 < _temp7302.base? 1: _temp7304 >= _temp7302.last_plus_one)){ _throw(
Null_Exception);}* _temp7304;})); _temp7301->tl= 0; _temp7301;}); _temp7300;});
_temp7299;}); break; case 387: _LL7298: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp7306=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7306[ 0]=({ struct Cyc_ExpList_tok_struct _temp7307; _temp7307.tag= Cyc_ExpList_tok;
_temp7307.f1=({ struct Cyc_List_List* _temp7308=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp7308->hd=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7312= yyvs; void** _temp7314= _temp7312.curr +
yyvsp_offset; if( _temp7312.base == 0? 1:( _temp7314 < _temp7312.base? 1:
_temp7314 >= _temp7312.last_plus_one)){ _throw( Null_Exception);}* _temp7314;}));
_temp7308->tl= Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7309= yyvs;
void** _temp7311= _temp7309.curr +( yyvsp_offset - 2); if( _temp7309.base == 0?
1:( _temp7311 < _temp7309.base? 1: _temp7311 >= _temp7309.last_plus_one)){
_throw( Null_Exception);}* _temp7311;})); _temp7308;}); _temp7307;}); _temp7306;});
break; case 388: _LL7305: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7316=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7316[ 0]=({ struct Cyc_Exp_tok_struct _temp7317; _temp7317.tag= Cyc_Exp_tok;
_temp7317.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp7318= yyvs; void** _temp7320= _temp7318.curr + yyvsp_offset; if( _temp7318.base
== 0? 1:( _temp7320 < _temp7318.base? 1: _temp7320 >= _temp7318.last_plus_one)){
_throw( Null_Exception);}* _temp7320;}))).f1,(* Cyc_yyget_Int_tok(({ struct
_tagged_ptr2 _temp7321= yyvs; void** _temp7323= _temp7321.curr + yyvsp_offset;
if( _temp7321.base == 0? 1:( _temp7323 < _temp7321.base? 1: _temp7323 >=
_temp7321.last_plus_one)){ _throw( Null_Exception);}* _temp7323;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7324= yyls; struct Cyc_Yyltype* _temp7326= _temp7324.curr
+ yylsp_offset; if( _temp7324.base == 0? 1:( _temp7326 < _temp7324.base? 1:
_temp7326 >= _temp7324.last_plus_one)){ _throw( Null_Exception);}* _temp7326;}).first_line,({
struct _tagged_ptr3 _temp7327= yyls; struct Cyc_Yyltype* _temp7329= _temp7327.curr
+ yylsp_offset; if( _temp7327.base == 0? 1:( _temp7329 < _temp7327.base? 1:
_temp7329 >= _temp7327.last_plus_one)){ _throw( Null_Exception);}* _temp7329;}).last_line));
_temp7317;}); _temp7316;}); break; case 389: _LL7315: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7331=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7331[ 0]=({ struct Cyc_Exp_tok_struct
_temp7332; _temp7332.tag= Cyc_Exp_tok; _temp7332.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp7333= yyvs; void** _temp7335= _temp7333.curr +
yyvsp_offset; if( _temp7333.base == 0? 1:( _temp7335 < _temp7333.base? 1:
_temp7335 >= _temp7333.last_plus_one)){ _throw( Null_Exception);}* _temp7335;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7336= yyls; struct Cyc_Yyltype*
_temp7338= _temp7336.curr + yylsp_offset; if( _temp7336.base == 0? 1:( _temp7338
< _temp7336.base? 1: _temp7338 >= _temp7336.last_plus_one)){ _throw(
Null_Exception);}* _temp7338;}).first_line,({ struct _tagged_ptr3 _temp7339=
yyls; struct Cyc_Yyltype* _temp7341= _temp7339.curr + yylsp_offset; if(
_temp7339.base == 0? 1:( _temp7341 < _temp7339.base? 1: _temp7341 >= _temp7339.last_plus_one)){
_throw( Null_Exception);}* _temp7341;}).last_line)); _temp7332;}); _temp7331;});
break; case 390: _LL7330: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7343=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7343[ 0]=({ struct Cyc_Exp_tok_struct _temp7344; _temp7344.tag= Cyc_Exp_tok;
_temp7344.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7345= yyvs; void** _temp7347= _temp7345.curr + yyvsp_offset; if( _temp7345.base
== 0? 1:( _temp7347 < _temp7345.base? 1: _temp7347 >= _temp7345.last_plus_one)){
_throw( Null_Exception);}* _temp7347;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7348= yyls; struct Cyc_Yyltype* _temp7350= _temp7348.curr +
yylsp_offset; if( _temp7348.base == 0? 1:( _temp7350 < _temp7348.base? 1:
_temp7350 >= _temp7348.last_plus_one)){ _throw( Null_Exception);}* _temp7350;}).first_line,({
struct _tagged_ptr3 _temp7351= yyls; struct Cyc_Yyltype* _temp7353= _temp7351.curr
+ yylsp_offset; if( _temp7351.base == 0? 1:( _temp7353 < _temp7351.base? 1:
_temp7353 >= _temp7351.last_plus_one)){ _throw( Null_Exception);}* _temp7353;}).last_line));
_temp7344;}); _temp7343;}); break; case 391: _LL7342: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7355=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7355[ 0]=({ struct Cyc_Exp_tok_struct
_temp7356; _temp7356.tag= Cyc_Exp_tok; _temp7356.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7357= yyls; struct Cyc_Yyltype* _temp7359= _temp7357.curr
+ yylsp_offset; if( _temp7357.base == 0? 1:( _temp7359 < _temp7357.base? 1:
_temp7359 >= _temp7357.last_plus_one)){ _throw( Null_Exception);}* _temp7359;}).first_line,({
struct _tagged_ptr3 _temp7360= yyls; struct Cyc_Yyltype* _temp7362= _temp7360.curr
+ yylsp_offset; if( _temp7360.base == 0? 1:( _temp7362 < _temp7360.base? 1:
_temp7362 >= _temp7360.last_plus_one)){ _throw( Null_Exception);}* _temp7362;}).last_line));
_temp7356;}); _temp7355;}); break; case 392: _LL7354: yyval=( void*)({ struct
Cyc_QualId_tok_struct* _temp7364=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp7364[ 0]=({ struct Cyc_QualId_tok_struct
_temp7365; _temp7365.tag= Cyc_QualId_tok; _temp7365.f1=({ struct _tuple1*
_temp7366=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp7366->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp7371=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp7371[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp7372; _temp7372.tag= Cyc_Absyn_Rel_n; _temp7372.f1= 0; _temp7372;});
_temp7371;}); _temp7366->f2=({ struct _tagged_string* _temp7367=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7367[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7368= yyvs; void** _temp7370=
_temp7368.curr + yyvsp_offset; if( _temp7368.base == 0? 1:( _temp7370 <
_temp7368.base? 1: _temp7370 >= _temp7368.last_plus_one)){ _throw(
Null_Exception);}* _temp7370;})); _temp7367;}); _temp7366;}); _temp7365;});
_temp7364;}); break; case 393: _LL7363: yyval=({ struct _tagged_ptr2 _temp7374=
yyvs; void** _temp7376= _temp7374.curr + yyvsp_offset; if( _temp7374.base == 0?
1:( _temp7376 < _temp7374.base? 1: _temp7376 >= _temp7374.last_plus_one)){
_throw( Null_Exception);}* _temp7376;}); break; default: _LL7373:( void) _throw((
void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen; yyssp_offset -= yylen;
yylsp_offset -= yylen;({ struct _tagged_ptr2 _temp7378= yyvs; void** _temp7380=
_temp7378.curr +( ++ yyvsp_offset); if( _temp7378.base == 0? 1:( _temp7380 <
_temp7378.base? 1: _temp7380 >= _temp7378.last_plus_one)){ _throw(
Null_Exception);}* _temp7380= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp7381= yyls; struct Cyc_Yyltype* _temp7383= _temp7381.curr
+ yylsp_offset; if( _temp7381.base == 0? 1:( _temp7383 < _temp7381.base? 1:
_temp7383 >= _temp7381.last_plus_one)){ _throw( Null_Exception);}(* _temp7383).first_line=
Cyc_yylloc.first_line;});({ struct _tagged_ptr3 _temp7384= yyls; struct Cyc_Yyltype*
_temp7386= _temp7384.curr + yylsp_offset; if( _temp7384.base == 0? 1:( _temp7386
< _temp7384.base? 1: _temp7386 >= _temp7384.last_plus_one)){ _throw(
Null_Exception);}(* _temp7386).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp7387= yyls; struct Cyc_Yyltype* _temp7389= _temp7387.curr
+ yylsp_offset; if( _temp7387.base == 0? 1:( _temp7389 < _temp7387.base? 1:
_temp7389 >= _temp7387.last_plus_one)){ _throw( Null_Exception);}(* _temp7389).last_line=({
struct _tagged_ptr3 _temp7390= yyls; struct Cyc_Yyltype* _temp7392= _temp7390.curr
+( yylsp_offset - 1); if( _temp7390.base == 0? 1:( _temp7392 < _temp7390.base? 1:
_temp7392 >= _temp7390.last_plus_one)){ _throw( Null_Exception);}* _temp7392;}).last_line;});({
struct _tagged_ptr3 _temp7393= yyls; struct Cyc_Yyltype* _temp7395= _temp7393.curr
+ yylsp_offset; if( _temp7393.base == 0? 1:( _temp7395 < _temp7393.base? 1:
_temp7395 >= _temp7393.last_plus_one)){ _throw( Null_Exception);}(* _temp7395).last_column=({
struct _tagged_ptr3 _temp7396= yyls; struct Cyc_Yyltype* _temp7398= _temp7396.curr
+( yylsp_offset - 1); if( _temp7396.base == 0? 1:( _temp7398 < _temp7396.base? 1:
_temp7398 >= _temp7396.last_plus_one)){ _throw( Null_Exception);}* _temp7398;}).last_column;});({
struct _tagged_ptr3 _temp7399= yyls; struct Cyc_Yyltype* _temp7401= _temp7399.curr
+ yylsp_offset; if( _temp7399.base == 0? 1:( _temp7401 < _temp7399.base? 1:
_temp7401 >= _temp7399.last_plus_one)){ _throw( Null_Exception);}(* _temp7401).text=(
struct _tagged_string)({ char* _temp7402=( char*)""; struct _tagged_string
_temp7403; _temp7403.curr= _temp7402; _temp7403.base= _temp7402; _temp7403.last_plus_one=
_temp7402 + 1; _temp7403;});});} else{({ struct _tagged_ptr3 _temp7404= yyls;
struct Cyc_Yyltype* _temp7406= _temp7404.curr + yylsp_offset; if( _temp7404.base
== 0? 1:( _temp7406 < _temp7404.base? 1: _temp7406 >= _temp7404.last_plus_one)){
_throw( Null_Exception);}(* _temp7406).last_line=({ struct _tagged_ptr3
_temp7407= yyls; struct Cyc_Yyltype* _temp7409= _temp7407.curr +(( yylsp_offset
+ yylen) - 1); if( _temp7407.base == 0? 1:( _temp7409 < _temp7407.base? 1:
_temp7409 >= _temp7407.last_plus_one)){ _throw( Null_Exception);}* _temp7409;}).last_line;});({
struct _tagged_ptr3 _temp7410= yyls; struct Cyc_Yyltype* _temp7412= _temp7410.curr
+ yylsp_offset; if( _temp7410.base == 0? 1:( _temp7412 < _temp7410.base? 1:
_temp7412 >= _temp7410.last_plus_one)){ _throw( Null_Exception);}(* _temp7412).last_column=({
struct _tagged_ptr3 _temp7413= yyls; struct Cyc_Yyltype* _temp7415= _temp7413.curr
+(( yylsp_offset + yylen) - 1); if( _temp7413.base == 0? 1:( _temp7415 <
_temp7413.base? 1: _temp7415 >= _temp7413.last_plus_one)){ _throw(
Null_Exception);}* _temp7415;}).last_column;});} yyn=( int)({ short* _temp7416=(
short*) Cyc_yyr1; unsigned int _temp7417= yyn; if( _temp7417 >= 394u){ _throw(
Null_Exception);} _temp7416[ _temp7417];}); yystate=({ short* _temp7418=( short*)
Cyc_yypgoto; unsigned int _temp7419= yyn - 124; if( _temp7419 >= 108u){ _throw(
Null_Exception);} _temp7418[ _temp7419];}) +({ struct _tagged_ptr1 _temp7420=
yyss; short* _temp7422= _temp7420.curr + yyssp_offset; if( _temp7420.base == 0?
1:( _temp7422 < _temp7420.base? 1: _temp7422 >= _temp7420.last_plus_one)){
_throw( Null_Exception);}* _temp7422;}); if(( yystate >= 0? yystate <= 4222: 0)?({
short* _temp7423=( short*) Cyc_yycheck; unsigned int _temp7424= yystate; if(
_temp7424 >= 4223u){ _throw( Null_Exception);} _temp7423[ _temp7424];}) ==({
struct _tagged_ptr1 _temp7425= yyss; short* _temp7427= _temp7425.curr +
yyssp_offset; if( _temp7425.base == 0? 1:( _temp7427 < _temp7425.base? 1:
_temp7427 >= _temp7425.last_plus_one)){ _throw( Null_Exception);}* _temp7427;}):
0){ yystate=( int)({ short* _temp7428=( short*) Cyc_yytable; unsigned int
_temp7429= yystate; if( _temp7429 >= 4223u){ _throw( Null_Exception);} _temp7428[
_temp7429];});} else{ yystate=( int)({ short* _temp7430=( short*) Cyc_yydefgoto;
unsigned int _temp7431= yyn - 124; if( _temp7431 >= 108u){ _throw(
Null_Exception);} _temp7430[ _temp7431];});} goto yynewstate; yyerrlab: if(
yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short* _temp7432=( short*) Cyc_yypact;
unsigned int _temp7433= yystate; if( _temp7433 >= 751u){ _throw( Null_Exception);}
_temp7432[ _temp7433];}); if( yyn > - 32768? yyn < 4222: 0){ int sze= 0; struct
_tagged_string msg; int x; int count; count= 0; for( x= yyn < 0? - yyn: 0; x <
232u / sizeof( char*); x ++){ if(({ short* _temp7434=( short*) Cyc_yycheck;
unsigned int _temp7435= x + yyn; if( _temp7435 >= 4223u){ _throw( Null_Exception);}
_temp7434[ _temp7435];}) == x){ sze += Cyc_String_strlen(({ struct
_tagged_string* _temp7436=( struct _tagged_string*) Cyc_yytname; unsigned int
_temp7437= x; if( _temp7437 >= 232u){ _throw( Null_Exception);} _temp7436[
_temp7437];})) + 15, count ++;}} msg=({ unsigned int _temp7438=( unsigned int)(
sze + 15); char* _temp7439=( char*) GC_malloc_atomic( sizeof( char) * _temp7438);
struct _tagged_string _temp7442={ _temp7439, _temp7439, _temp7439 + _temp7438};{
unsigned int _temp7440= _temp7438; unsigned int i; for( i= 0; i < _temp7440; i
++){ _temp7439[ i]='\000';}}; _temp7442;}); Cyc_String_strcpy( msg,( struct
_tagged_string)({ char* _temp7443=( char*)"parse error"; struct _tagged_string
_temp7444; _temp7444.curr= _temp7443; _temp7444.base= _temp7443; _temp7444.last_plus_one=
_temp7443 + 12; _temp7444;})); if( count < 5){ count= 0; for( x= yyn < 0? - yyn:
0; x < 232u / sizeof( char*); x ++){ if(({ short* _temp7445=( short*) Cyc_yycheck;
unsigned int _temp7446= x + yyn; if( _temp7446 >= 4223u){ _throw( Null_Exception);}
_temp7445[ _temp7446];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp7447=( char*)", expecting `"; struct _tagged_string
_temp7448; _temp7448.curr= _temp7447; _temp7448.base= _temp7447; _temp7448.last_plus_one=
_temp7447 + 14; _temp7448;}):( struct _tagged_string)({ char* _temp7449=( char*)" or `";
struct _tagged_string _temp7450; _temp7450.curr= _temp7449; _temp7450.base=
_temp7449; _temp7450.last_plus_one= _temp7449 + 6; _temp7450;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp7451=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp7452= x; if( _temp7452 >= 232u){ _throw( Null_Exception);}
_temp7451[ _temp7452];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp7453=( char*)"'"; struct _tagged_string _temp7454; _temp7454.curr=
_temp7453; _temp7454.base= _temp7453; _temp7454.last_plus_one= _temp7453 + 2;
_temp7454;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp7455=( char*)"parse error"; struct _tagged_string
_temp7456; _temp7456.curr= _temp7455; _temp7456.base= _temp7455; _temp7456.last_plus_one=
_temp7455 + 12; _temp7456;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp7457= yyss; short*
_temp7459= _temp7457.curr +( -- yyssp_offset); if( _temp7457.base == 0? 1:(
_temp7459 < _temp7457.base? 1: _temp7459 >= _temp7457.last_plus_one)){ _throw(
Null_Exception);}* _temp7459;}); yylsp_offset --; yyerrhandle: yyn=( int)({
short* _temp7460=( short*) Cyc_yypact; unsigned int _temp7461= yystate; if(
_temp7461 >= 751u){ _throw( Null_Exception);} _temp7460[ _temp7461];}); if( yyn
== - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4222)? 1:({
short* _temp7462=( short*) Cyc_yycheck; unsigned int _temp7463= yyn; if(
_temp7463 >= 4223u){ _throw( Null_Exception);} _temp7462[ _temp7463];}) != 1){
goto yyerrdefault;} yyn=( int)({ short* _temp7464=( short*) Cyc_yytable;
unsigned int _temp7465= yyn; if( _temp7465 >= 4223u){ _throw( Null_Exception);}
_temp7464[ _temp7465];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 750){
return 0;}({ struct _tagged_ptr2 _temp7466= yyvs; void** _temp7468= _temp7466.curr
+( ++ yyvsp_offset); if( _temp7466.base == 0? 1:( _temp7468 < _temp7466.base? 1:
_temp7468 >= _temp7466.last_plus_one)){ _throw( Null_Exception);}* _temp7468=
Cyc_yylval;});({ struct _tagged_ptr3 _temp7469= yyls; struct Cyc_Yyltype*
_temp7471= _temp7469.curr +( ++ yylsp_offset); if( _temp7469.base == 0? 1:(
_temp7471 < _temp7469.base? 1: _temp7471 >= _temp7469.last_plus_one)){ _throw(
Null_Exception);}* _temp7471= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, void* v){ void* _temp7490= v; struct _tuple12* _temp7510; struct _tuple12
_temp7512; int _temp7513; void* _temp7515; char _temp7517; short _temp7519;
struct _tagged_string _temp7521; struct Cyc_Core_Opt* _temp7523; struct Cyc_Core_Opt*
_temp7525; struct Cyc_Core_Opt _temp7527; struct _tagged_string* _temp7528;
struct _tuple1* _temp7530; struct _tuple1 _temp7532; struct _tagged_string*
_temp7533; void* _temp7535; _LL7492: if( _temp7490 == Cyc_Okay_tok){ goto
_LL7493;} else{ goto _LL7494;} _LL7494: if(*(( void**) _temp7490) == Cyc_Int_tok){
_LL7511: _temp7510=( struct _tuple12*)(( struct Cyc_Int_tok_struct*) _temp7490)->f1;
_temp7512=* _temp7510; _LL7516: _temp7515= _temp7512.f1; goto _LL7514; _LL7514:
_temp7513= _temp7512.f2; goto _LL7495;} else{ goto _LL7496;} _LL7496: if(*((
void**) _temp7490) == Cyc_Char_tok){ _LL7518: _temp7517=( char)(( struct Cyc_Char_tok_struct*)
_temp7490)->f1; goto _LL7497;} else{ goto _LL7498;} _LL7498: if(*(( void**)
_temp7490) == Cyc_Short_tok){ _LL7520: _temp7519=( short)(( struct Cyc_Short_tok_struct*)
_temp7490)->f1; goto _LL7499;} else{ goto _LL7500;} _LL7500: if(*(( void**)
_temp7490) == Cyc_String_tok){ _LL7522: _temp7521=( struct _tagged_string)((
struct Cyc_String_tok_struct*) _temp7490)->f1; goto _LL7501;} else{ goto _LL7502;}
_LL7502: if(*(( void**) _temp7490) == Cyc_Stringopt_tok){ _LL7524: _temp7523=(
struct Cyc_Core_Opt*)(( struct Cyc_Stringopt_tok_struct*) _temp7490)->f1; if(
_temp7523 == 0){ goto _LL7503;} else{ goto _LL7504;}} else{ goto _LL7504;}
_LL7504: if(*(( void**) _temp7490) == Cyc_Stringopt_tok){ _LL7526: _temp7525=(
struct Cyc_Core_Opt*)(( struct Cyc_Stringopt_tok_struct*) _temp7490)->f1; if(
_temp7525 == 0){ goto _LL7506;} else{ _temp7527=* _temp7525; _LL7529: _temp7528=(
struct _tagged_string*) _temp7527.v; goto _LL7505;}} else{ goto _LL7506;}
_LL7506: if(*(( void**) _temp7490) == Cyc_QualId_tok){ _LL7531: _temp7530=(
struct _tuple1*)(( struct Cyc_QualId_tok_struct*) _temp7490)->f1; _temp7532=*
_temp7530; _LL7536: _temp7535= _temp7532.f1; goto _LL7534; _LL7534: _temp7533=
_temp7532.f2; goto _LL7507;} else{ goto _LL7508;} _LL7508: goto _LL7509; _LL7493:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL7491; _LL7495: fprintf( Cyc_Stdio_stderr,"%d",
_temp7513); goto _LL7491; _LL7497: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp7517); goto _LL7491; _LL7499: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp7519); goto _LL7491; _LL7501:({ struct _tagged_string _temp7537= _temp7521;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp7537.last_plus_one - _temp7537.curr,
_temp7537.curr);}); goto _LL7491; _LL7503: fprintf( Cyc_Stdio_stderr,"null");
goto _LL7491; _LL7505:({ struct _tagged_string _temp7538=* _temp7528; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp7538.last_plus_one - _temp7538.curr, _temp7538.curr);});
goto _LL7491; _LL7507: { struct Cyc_List_List* prefix= 0;{ void* _temp7539=
_temp7535; struct Cyc_List_List* _temp7547; struct Cyc_List_List* _temp7549;
_LL7541: if(( unsigned int) _temp7539 > 1u?*(( int*) _temp7539) == Cyc_Absyn_Rel_n:
0){ _LL7548: _temp7547=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp7539)->f1; goto _LL7542;} else{ goto _LL7543;} _LL7543: if(( unsigned int)
_temp7539 > 1u?*(( int*) _temp7539) == Cyc_Absyn_Abs_n: 0){ _LL7550: _temp7549=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp7539)->f1; goto
_LL7544;} else{ goto _LL7545;} _LL7545: if( _temp7539 ==( void*) Cyc_Absyn_Loc_n){
goto _LL7546;} else{ goto _LL7540;} _LL7542: prefix= _temp7547; goto _LL7540;
_LL7544: prefix= _temp7549; goto _LL7540; _LL7546: goto _LL7540; _LL7540:;} for(
0; prefix != 0; prefix=({ struct Cyc_List_List* _temp7551= prefix; if( _temp7551
== 0){ _throw( Null_Exception);} _temp7551->tl;})){({ struct _tagged_string
_temp7553=*(( struct _tagged_string*)({ struct Cyc_List_List* _temp7552= prefix;
if( _temp7552 == 0){ _throw( Null_Exception);} _temp7552->hd;})); fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7553.last_plus_one - _temp7553.curr, _temp7553.curr);});}({ struct
_tagged_string _temp7554=* _temp7533; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7554.last_plus_one - _temp7554.curr, _temp7554.curr);}); goto _LL7491;}
_LL7509: fprintf( Cyc_Stdio_stderr,"?"); goto _LL7491; _LL7491:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp7555=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp7555->v=( void*) Cyc_Lexing_from_file(
f); _temp7555;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}