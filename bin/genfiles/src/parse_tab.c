#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char;
typedef unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int; typedef
unsigned int Cyc_u_long; typedef unsigned short Cyc_ushort; typedef unsigned int
Cyc_uint; typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef
char* Cyc_caddr_t; typedef unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t;
typedef unsigned int Cyc_vm_size_t; typedef char Cyc_int8_t; typedef char Cyc_u_int8_t;
typedef short Cyc_int16_t; typedef unsigned short Cyc_u_int16_t; typedef int Cyc_int32_t;
typedef unsigned int Cyc_u_int32_t; typedef long long Cyc_int64_t; typedef
unsigned long long Cyc_u_int64_t; typedef int Cyc_register_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef char* Cyc_addr_t; typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t;
typedef int Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ;
typedef struct Cyc__types_fd_set Cyc__types_fd_set; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
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
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType= 11;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 12; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 13; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
14; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 15; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
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
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_Tunion_e= 27;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 28; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 29; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
30; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 31; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 32; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 33; struct Cyc_Absyn_Fill_e_struct{
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
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;
int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc;
struct _tuple1* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
char* tag; struct Cyc_List_List* f1; } ; static char _temp879[ 8u]="Int_tok";
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
16u]="TunionField_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1299, _temp1299, _temp1299 + 16u}};
struct Cyc_Absyn_Tunionfield* Cyc_yyget_TunionField_tok( void* yy1){ void*
_temp1300= yy1; struct Cyc_Absyn_Tunionfield* _temp1306; _LL1302: if(*(( void**)
_temp1300) == Cyc_TunionField_tok){ _LL1307: _temp1306=( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_TunionField_tok_struct*) _temp1300)->f1; goto _LL1303;} else{ goto
_LL1304;} _LL1304: goto _LL1305; _LL1303: return _temp1306; _LL1305:( void)
_throw(( void*)& Cyc_yyfail_TunionField_tok); _LL1301:;} static char _temp1311[
20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionFieldList_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1311, _temp1311, _temp1311 + 20u}};
struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok( void* yy1){ void* _temp1312=
yy1; struct Cyc_List_List* _temp1318; _LL1314: if(*(( void**) _temp1312) == Cyc_TunionFieldList_tok){
_LL1319: _temp1318=( struct Cyc_List_List*)(( struct Cyc_TunionFieldList_tok_struct*)
_temp1312)->f1; goto _LL1315;} else{ goto _LL1316;} _LL1316: goto _LL1317;
_LL1315: return _temp1318; _LL1317:( void) _throw(( void*)& Cyc_yyfail_TunionFieldList_tok);
_LL1313:;} static char _temp1323[ 17u]="QualSpecList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualSpecList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1323, _temp1323, _temp1323 + 17u}}; struct _tuple14* Cyc_yyget_QualSpecList_tok(
void* yy1){ void* _temp1324= yy1; struct _tuple14* _temp1330; _LL1326: if(*((
void**) _temp1324) == Cyc_QualSpecList_tok){ _LL1331: _temp1330=( struct
_tuple14*)(( struct Cyc_QualSpecList_tok_struct*) _temp1324)->f1; goto _LL1327;}
else{ goto _LL1328;} _LL1328: goto _LL1329; _LL1327: return _temp1330; _LL1329:(
void) _throw(( void*)& Cyc_yyfail_QualSpecList_tok); _LL1325:;} static char
_temp1335[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_IdList_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1335, _temp1335, _temp1335 + 11u}};
struct Cyc_List_List* Cyc_yyget_IdList_tok( void* yy1){ void* _temp1336= yy1;
struct Cyc_List_List* _temp1342; _LL1338: if(*(( void**) _temp1336) == Cyc_IdList_tok){
_LL1343: _temp1342=( struct Cyc_List_List*)(( struct Cyc_IdList_tok_struct*)
_temp1336)->f1; goto _LL1339;} else{ goto _LL1340;} _LL1340: goto _LL1341;
_LL1339: return _temp1342; _LL1341:( void) _throw(( void*)& Cyc_yyfail_IdList_tok);
_LL1337:;} static char _temp1347[ 14u]="ParamDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1347,
_temp1347, _temp1347 + 14u}}; struct _tuple2* Cyc_yyget_ParamDecl_tok( void* yy1){
void* _temp1348= yy1; struct _tuple2* _temp1354; _LL1350: if(*(( void**)
_temp1348) == Cyc_ParamDecl_tok){ _LL1355: _temp1354=( struct _tuple2*)(( struct
Cyc_ParamDecl_tok_struct*) _temp1348)->f1; goto _LL1351;} else{ goto _LL1352;}
_LL1352: goto _LL1353; _LL1351: return _temp1354; _LL1353:( void) _throw(( void*)&
Cyc_yyfail_ParamDecl_tok); _LL1349:;} static char _temp1359[ 18u]="ParamDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1359, _temp1359, _temp1359 + 18u}}; struct Cyc_List_List*
Cyc_yyget_ParamDeclList_tok( void* yy1){ void* _temp1360= yy1; struct Cyc_List_List*
_temp1366; _LL1362: if(*(( void**) _temp1360) == Cyc_ParamDeclList_tok){ _LL1367:
_temp1366=( struct Cyc_List_List*)(( struct Cyc_ParamDeclList_tok_struct*)
_temp1360)->f1; goto _LL1363;} else{ goto _LL1364;} _LL1364: goto _LL1365;
_LL1363: return _temp1366; _LL1365:( void) _throw(( void*)& Cyc_yyfail_ParamDeclList_tok);
_LL1361:;} static char _temp1371[ 22u]="ParamDeclListBool_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1371, _temp1371, _temp1371 + 22u}}; struct _tuple16*
Cyc_yyget_ParamDeclListBool_tok( void* yy1){ void* _temp1372= yy1; struct
_tuple16* _temp1378; _LL1374: if(*(( void**) _temp1372) == Cyc_ParamDeclListBool_tok){
_LL1379: _temp1378=( struct _tuple16*)(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp1372)->f1; goto _LL1375;} else{ goto _LL1376;} _LL1376: goto _LL1377;
_LL1375: return _temp1378; _LL1377:( void) _throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL1373:;} static char _temp1383[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1383,
_temp1383, _temp1383 + 13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void*
yy1){ void* _temp1384= yy1; struct Cyc_List_List* _temp1390; _LL1386: if(*((
void**) _temp1384) == Cyc_TypeList_tok){ _LL1391: _temp1390=( struct Cyc_List_List*)((
struct Cyc_TypeList_tok_struct*) _temp1384)->f1; goto _LL1387;} else{ goto
_LL1388;} _LL1388: goto _LL1389; _LL1387: return _temp1390; _LL1389:( void)
_throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1385:;} static char _temp1395[ 19u]="DesignatorList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1395, _temp1395, _temp1395 + 19u}}; struct Cyc_List_List*
Cyc_yyget_DesignatorList_tok( void* yy1){ void* _temp1396= yy1; struct Cyc_List_List*
_temp1402; _LL1398: if(*(( void**) _temp1396) == Cyc_DesignatorList_tok){
_LL1403: _temp1402=( struct Cyc_List_List*)(( struct Cyc_DesignatorList_tok_struct*)
_temp1396)->f1; goto _LL1399;} else{ goto _LL1400;} _LL1400: goto _LL1401;
_LL1399: return _temp1402; _LL1401:( void) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1397:;} static char _temp1407[ 15u]="Designator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1407,
_temp1407, _temp1407 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1408= yy1; void* _temp1414; _LL1410: if(*(( void**) _temp1408) == Cyc_Designator_tok){
_LL1415: _temp1414=( void*)(( struct Cyc_Designator_tok_struct*) _temp1408)->f1;
goto _LL1411;} else{ goto _LL1412;} _LL1412: goto _LL1413; _LL1411: return
_temp1414; _LL1413:( void) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1409:;}
static char _temp1419[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1419, _temp1419, _temp1419 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1420= yy1; void* _temp1426;
_LL1422: if(*(( void**) _temp1420) == Cyc_Kind_tok){ _LL1427: _temp1426=( void*)((
struct Cyc_Kind_tok_struct*) _temp1420)->f1; goto _LL1423;} else{ goto _LL1424;}
_LL1424: goto _LL1425; _LL1423: return _temp1426; _LL1425:( void) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1421:;} static char _temp1431[ 9u]="Type_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,( struct
_tagged_string){ _temp1431, _temp1431, _temp1431 + 9u}}; void* Cyc_yyget_Type_tok(
void* yy1){ void* _temp1432= yy1; void* _temp1438; _LL1434: if(*(( void**)
_temp1432) == Cyc_Type_tok){ _LL1439: _temp1438=( void*)(( struct Cyc_Type_tok_struct*)
_temp1432)->f1; goto _LL1435;} else{ goto _LL1436;} _LL1436: goto _LL1437;
_LL1435: return _temp1438; _LL1437:( void) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1433:;} static char _temp1443[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1443, _temp1443, _temp1443 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ void* _temp1444= yy1; struct Cyc_List_List* _temp1450; _LL1446: if(*((
void**) _temp1444) == Cyc_AttributeList_tok){ _LL1451: _temp1450=( struct Cyc_List_List*)((
struct Cyc_AttributeList_tok_struct*) _temp1444)->f1; goto _LL1447;} else{ goto
_LL1448;} _LL1448: goto _LL1449; _LL1447: return _temp1450; _LL1449:( void)
_throw(( void*)& Cyc_yyfail_AttributeList_tok); _LL1445:;} static char _temp1455[
14u]="Attribute_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1455, _temp1455, _temp1455 + 14u}};
void* Cyc_yyget_Attribute_tok( void* yy1){ void* _temp1456= yy1; void* _temp1462;
_LL1458: if(*(( void**) _temp1456) == Cyc_Attribute_tok){ _LL1463: _temp1462=(
void*)(( struct Cyc_Attribute_tok_struct*) _temp1456)->f1; goto _LL1459;} else{
goto _LL1460;} _LL1460: goto _LL1461; _LL1459: return _temp1462; _LL1461:( void)
_throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1457:;} static char _temp1467[ 14u]="Enumfield_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1467, _temp1467, _temp1467 + 14u}}; struct Cyc_Absyn_Enumfield*
Cyc_yyget_Enumfield_tok( void* yy1){ void* _temp1468= yy1; struct Cyc_Absyn_Enumfield*
_temp1474; _LL1470: if(*(( void**) _temp1468) == Cyc_Enumfield_tok){ _LL1475:
_temp1474=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Enumfield_tok_struct*)
_temp1468)->f1; goto _LL1471;} else{ goto _LL1472;} _LL1472: goto _LL1473;
_LL1471: return _temp1474; _LL1473:( void) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1469:;} static char _temp1479[ 18u]="EnumfieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1479, _temp1479, _temp1479 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1480= yy1; struct Cyc_List_List* _temp1486; _LL1482: if(*((
void**) _temp1480) == Cyc_EnumfieldList_tok){ _LL1487: _temp1486=( struct Cyc_List_List*)((
struct Cyc_EnumfieldList_tok_struct*) _temp1480)->f1; goto _LL1483;} else{ goto
_LL1484;} _LL1484: goto _LL1485; _LL1483: return _temp1486; _LL1485:( void)
_throw(( void*)& Cyc_yyfail_EnumfieldList_tok); _LL1481:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct
Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1488; _temp1488.timestamp=
0; _temp1488.first_line= 0; _temp1488.first_column= 0; _temp1488.last_line= 0;
_temp1488.last_column= 0; _temp1488.text=( struct _tagged_string)({ char*
_temp1489=( char*)""; struct _tagged_string _temp1490; _temp1490.curr= _temp1489;
_temp1490.base= _temp1489; _temp1490.last_plus_one= _temp1489 + 1; _temp1490;});
_temp1488;});} static char _temp1493[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc=(
struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column= 0,.last_line= 0,.last_column=
0,.text=( struct _tagged_string){ _temp1493, _temp1493, _temp1493 + 1u}}; static
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
(short)93, (short)94, (short)95, (short)96}; static char _temp1496[ 2u]="$";
static char _temp1499[ 6u]="error"; static char _temp1502[ 12u]="$undefined.";
static char _temp1505[ 5u]="AUTO"; static char _temp1508[ 9u]="REGISTER"; static
char _temp1511[ 7u]="STATIC"; static char _temp1514[ 7u]="EXTERN"; static char
_temp1517[ 8u]="TYPEDEF"; static char _temp1520[ 5u]="VOID"; static char
_temp1523[ 5u]="CHAR"; static char _temp1526[ 6u]="SHORT"; static char _temp1529[
4u]="INT"; static char _temp1532[ 5u]="LONG"; static char _temp1535[ 6u]="FLOAT";
static char _temp1538[ 7u]="DOUBLE"; static char _temp1541[ 7u]="SIGNED"; static
char _temp1544[ 9u]="UNSIGNED"; static char _temp1547[ 6u]="CONST"; static char
_temp1550[ 9u]="VOLATILE"; static char _temp1553[ 9u]="RESTRICT"; static char
_temp1556[ 7u]="STRUCT"; static char _temp1559[ 6u]="UNION"; static char
_temp1562[ 5u]="CASE"; static char _temp1565[ 8u]="DEFAULT"; static char
_temp1568[ 7u]="INLINE"; static char _temp1571[ 3u]="IF"; static char _temp1574[
5u]="ELSE"; static char _temp1577[ 7u]="SWITCH"; static char _temp1580[ 6u]="WHILE";
static char _temp1583[ 3u]="DO"; static char _temp1586[ 4u]="FOR"; static char
_temp1589[ 5u]="GOTO"; static char _temp1592[ 9u]="CONTINUE"; static char
_temp1595[ 6u]="BREAK"; static char _temp1598[ 7u]="RETURN"; static char
_temp1601[ 7u]="SIZEOF"; static char _temp1604[ 5u]="ENUM"; static char
_temp1607[ 8u]="NULL_kw"; static char _temp1610[ 4u]="LET"; static char
_temp1613[ 6u]="THROW"; static char _temp1616[ 4u]="TRY"; static char _temp1619[
6u]="CATCH"; static char _temp1622[ 4u]="NEW"; static char _temp1625[ 9u]="ABSTRACT";
static char _temp1628[ 9u]="FALLTHRU"; static char _temp1631[ 6u]="USING";
static char _temp1634[ 10u]="NAMESPACE"; static char _temp1637[ 7u]="TUNION";
static char _temp1640[ 8u]="XTUNION"; static char _temp1643[ 5u]="FILL"; static
char _temp1646[ 8u]="CODEGEN"; static char _temp1649[ 4u]="CUT"; static char
_temp1652[ 7u]="SPLICE"; static char _temp1655[ 7u]="PRINTF"; static char
_temp1658[ 8u]="FPRINTF"; static char _temp1661[ 8u]="XPRINTF"; static char
_temp1664[ 6u]="SCANF"; static char _temp1667[ 7u]="FSCANF"; static char
_temp1670[ 7u]="SSCANF"; static char _temp1673[ 7u]="MALLOC"; static char
_temp1676[ 9u]="REGION_T"; static char _temp1679[ 7u]="REGION"; static char
_temp1682[ 5u]="RNEW"; static char _temp1685[ 8u]="RMALLOC"; static char
_temp1688[ 7u]="PTR_OP"; static char _temp1691[ 7u]="INC_OP"; static char
_temp1694[ 7u]="DEC_OP"; static char _temp1697[ 8u]="LEFT_OP"; static char
_temp1700[ 9u]="RIGHT_OP"; static char _temp1703[ 6u]="LE_OP"; static char
_temp1706[ 6u]="GE_OP"; static char _temp1709[ 6u]="EQ_OP"; static char
_temp1712[ 6u]="NE_OP"; static char _temp1715[ 7u]="AND_OP"; static char
_temp1718[ 6u]="OR_OP"; static char _temp1721[ 11u]="MUL_ASSIGN"; static char
_temp1724[ 11u]="DIV_ASSIGN"; static char _temp1727[ 11u]="MOD_ASSIGN"; static
char _temp1730[ 11u]="ADD_ASSIGN"; static char _temp1733[ 11u]="SUB_ASSIGN";
static char _temp1736[ 12u]="LEFT_ASSIGN"; static char _temp1739[ 13u]="RIGHT_ASSIGN";
static char _temp1742[ 11u]="AND_ASSIGN"; static char _temp1745[ 11u]="XOR_ASSIGN";
static char _temp1748[ 10u]="OR_ASSIGN"; static char _temp1751[ 9u]="ELLIPSIS";
static char _temp1754[ 11u]="LEFT_RIGHT"; static char _temp1757[ 12u]="COLON_COLON";
static char _temp1760[ 11u]="IDENTIFIER"; static char _temp1763[ 17u]="INTEGER_CONSTANT";
static char _temp1766[ 7u]="STRING"; static char _temp1769[ 19u]="CHARACTER_CONSTANT";
static char _temp1772[ 18u]="FLOATING_CONSTANT"; static char _temp1775[ 9u]="TYPE_VAR";
static char _temp1778[ 16u]="QUAL_IDENTIFIER"; static char _temp1781[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1784[ 10u]="ATTRIBUTE"; static char _temp1787[ 4u]="';'";
static char _temp1790[ 4u]="'{'"; static char _temp1793[ 4u]="'}'"; static char
_temp1796[ 4u]="'='"; static char _temp1799[ 4u]="'('"; static char _temp1802[ 4u]="')'";
static char _temp1805[ 4u]="','"; static char _temp1808[ 4u]="'_'"; static char
_temp1811[ 4u]="'$'"; static char _temp1814[ 4u]="'<'"; static char _temp1817[ 4u]="'>'";
static char _temp1820[ 4u]="':'"; static char _temp1823[ 4u]="'.'"; static char
_temp1826[ 4u]="'['"; static char _temp1829[ 4u]="']'"; static char _temp1832[ 4u]="'*'";
static char _temp1835[ 4u]="'@'"; static char _temp1838[ 4u]="'?'"; static char
_temp1841[ 4u]="'+'"; static char _temp1844[ 4u]="'-'"; static char _temp1847[ 4u]="'&'";
static char _temp1850[ 4u]="'|'"; static char _temp1853[ 4u]="'^'"; static char
_temp1856[ 4u]="'/'"; static char _temp1859[ 4u]="'%'"; static char _temp1862[ 4u]="'~'";
static char _temp1865[ 4u]="'!'"; static char _temp1868[ 5u]="prog"; static char
_temp1871[ 17u]="translation_unit"; static char _temp1874[ 21u]="external_declaration";
static char _temp1877[ 20u]="function_definition"; static char _temp1880[ 21u]="function_definition2";
static char _temp1883[ 13u]="using_action"; static char _temp1886[ 15u]="unusing_action";
static char _temp1889[ 17u]="namespace_action"; static char _temp1892[ 19u]="unnamespace_action";
static char _temp1895[ 12u]="declaration"; static char _temp1898[ 17u]="declaration_list";
static char _temp1901[ 23u]="declaration_specifiers"; static char _temp1904[ 24u]="storage_class_specifier";
static char _temp1907[ 15u]="attributes_opt"; static char _temp1910[ 11u]="attributes";
static char _temp1913[ 15u]="attribute_list"; static char _temp1916[ 10u]="attribute";
static char _temp1919[ 15u]="type_specifier"; static char _temp1922[ 5u]="kind";
static char _temp1925[ 15u]="type_qualifier"; static char _temp1928[ 15u]="enum_specifier";
static char _temp1931[ 11u]="enum_field"; static char _temp1934[ 22u]="enum_declaration_list";
static char _temp1937[ 26u]="struct_or_union_specifier"; static char _temp1940[
16u]="type_params_opt"; static char _temp1943[ 16u]="struct_or_union"; static
char _temp1946[ 24u]="struct_declaration_list"; static char _temp1949[ 25u]="struct_declaration_list0";
static char _temp1952[ 21u]="init_declarator_list"; static char _temp1955[ 22u]="init_declarator_list0";
static char _temp1958[ 16u]="init_declarator"; static char _temp1961[ 19u]="struct_declaration";
static char _temp1964[ 25u]="specifier_qualifier_list"; static char _temp1967[
23u]="struct_declarator_list"; static char _temp1970[ 24u]="struct_declarator_list0";
static char _temp1973[ 18u]="struct_declarator"; static char _temp1976[ 17u]="tunion_specifier";
static char _temp1979[ 18u]="tunion_or_xtunion"; static char _temp1982[ 17u]="tunionfield_list";
static char _temp1985[ 12u]="tunionfield"; static char _temp1988[ 11u]="declarator";
static char _temp1991[ 18u]="direct_declarator"; static char _temp1994[ 8u]="pointer";
static char _temp1997[ 13u]="pointer_char"; static char _temp2000[ 8u]="rgn_opt";
static char _temp2003[ 4u]="rgn"; static char _temp2006[ 20u]="type_qualifier_list";
static char _temp2009[ 20u]="parameter_type_list"; static char _temp2012[ 11u]="effect_set";
static char _temp2015[ 14u]="atomic_effect"; static char _temp2018[ 11u]="region_set";
static char _temp2021[ 15u]="parameter_list"; static char _temp2024[ 22u]="parameter_declaration";
static char _temp2027[ 16u]="identifier_list"; static char _temp2030[ 17u]="identifier_list0";
static char _temp2033[ 12u]="initializer"; static char _temp2036[ 18u]="array_initializer";
static char _temp2039[ 17u]="initializer_list"; static char _temp2042[ 12u]="designation";
static char _temp2045[ 16u]="designator_list"; static char _temp2048[ 11u]="designator";
static char _temp2051[ 10u]="type_name"; static char _temp2054[ 14u]="any_type_name";
static char _temp2057[ 15u]="type_name_list"; static char _temp2060[ 20u]="abstract_declarator";
static char _temp2063[ 27u]="direct_abstract_declarator"; static char _temp2066[
10u]="statement"; static char _temp2069[ 18u]="labeled_statement"; static char
_temp2072[ 21u]="expression_statement"; static char _temp2075[ 19u]="compound_statement";
static char _temp2078[ 16u]="block_item_list"; static char _temp2081[ 11u]="block_item";
static char _temp2084[ 20u]="selection_statement"; static char _temp2087[ 15u]="switch_clauses";
static char _temp2090[ 20u]="iteration_statement"; static char _temp2093[ 15u]="jump_statement";
static char _temp2096[ 8u]="pattern"; static char _temp2099[ 19u]="tuple_pattern_list";
static char _temp2102[ 20u]="tuple_pattern_list0"; static char _temp2105[ 14u]="field_pattern";
static char _temp2108[ 19u]="field_pattern_list"; static char _temp2111[ 20u]="field_pattern_list0";
static char _temp2114[ 11u]="expression"; static char _temp2117[ 22u]="assignment_expression";
static char _temp2120[ 20u]="assignment_operator"; static char _temp2123[ 23u]="conditional_expression";
static char _temp2126[ 20u]="constant_expression"; static char _temp2129[ 22u]="logical_or_expression";
static char _temp2132[ 23u]="logical_and_expression"; static char _temp2135[ 24u]="inclusive_or_expression";
static char _temp2138[ 24u]="exclusive_or_expression"; static char _temp2141[ 15u]="and_expression";
static char _temp2144[ 20u]="equality_expression"; static char _temp2147[ 22u]="relational_expression";
static char _temp2150[ 17u]="shift_expression"; static char _temp2153[ 20u]="additive_expression";
static char _temp2156[ 26u]="multiplicative_expression"; static char _temp2159[
16u]="cast_expression"; static char _temp2162[ 17u]="unary_expression"; static
char _temp2165[ 14u]="format_primop"; static char _temp2168[ 15u]="unary_operator";
static char _temp2171[ 19u]="postfix_expression"; static char _temp2174[ 19u]="primary_expression";
static char _temp2177[ 25u]="argument_expression_list"; static char _temp2180[
26u]="argument_expression_list0"; static char _temp2183[ 9u]="constant"; static
char _temp2186[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
231u]={( struct _tagged_string){ _temp1496, _temp1496, _temp1496 + 2u},( struct
_tagged_string){ _temp1499, _temp1499, _temp1499 + 6u},( struct _tagged_string){
_temp1502, _temp1502, _temp1502 + 12u},( struct _tagged_string){ _temp1505,
_temp1505, _temp1505 + 5u},( struct _tagged_string){ _temp1508, _temp1508,
_temp1508 + 9u},( struct _tagged_string){ _temp1511, _temp1511, _temp1511 + 7u},(
struct _tagged_string){ _temp1514, _temp1514, _temp1514 + 7u},( struct
_tagged_string){ _temp1517, _temp1517, _temp1517 + 8u},( struct _tagged_string){
_temp1520, _temp1520, _temp1520 + 5u},( struct _tagged_string){ _temp1523,
_temp1523, _temp1523 + 5u},( struct _tagged_string){ _temp1526, _temp1526,
_temp1526 + 6u},( struct _tagged_string){ _temp1529, _temp1529, _temp1529 + 4u},(
struct _tagged_string){ _temp1532, _temp1532, _temp1532 + 5u},( struct
_tagged_string){ _temp1535, _temp1535, _temp1535 + 6u},( struct _tagged_string){
_temp1538, _temp1538, _temp1538 + 7u},( struct _tagged_string){ _temp1541,
_temp1541, _temp1541 + 7u},( struct _tagged_string){ _temp1544, _temp1544,
_temp1544 + 9u},( struct _tagged_string){ _temp1547, _temp1547, _temp1547 + 6u},(
struct _tagged_string){ _temp1550, _temp1550, _temp1550 + 9u},( struct
_tagged_string){ _temp1553, _temp1553, _temp1553 + 9u},( struct _tagged_string){
_temp1556, _temp1556, _temp1556 + 7u},( struct _tagged_string){ _temp1559,
_temp1559, _temp1559 + 6u},( struct _tagged_string){ _temp1562, _temp1562,
_temp1562 + 5u},( struct _tagged_string){ _temp1565, _temp1565, _temp1565 + 8u},(
struct _tagged_string){ _temp1568, _temp1568, _temp1568 + 7u},( struct
_tagged_string){ _temp1571, _temp1571, _temp1571 + 3u},( struct _tagged_string){
_temp1574, _temp1574, _temp1574 + 5u},( struct _tagged_string){ _temp1577,
_temp1577, _temp1577 + 7u},( struct _tagged_string){ _temp1580, _temp1580,
_temp1580 + 6u},( struct _tagged_string){ _temp1583, _temp1583, _temp1583 + 3u},(
struct _tagged_string){ _temp1586, _temp1586, _temp1586 + 4u},( struct
_tagged_string){ _temp1589, _temp1589, _temp1589 + 5u},( struct _tagged_string){
_temp1592, _temp1592, _temp1592 + 9u},( struct _tagged_string){ _temp1595,
_temp1595, _temp1595 + 6u},( struct _tagged_string){ _temp1598, _temp1598,
_temp1598 + 7u},( struct _tagged_string){ _temp1601, _temp1601, _temp1601 + 7u},(
struct _tagged_string){ _temp1604, _temp1604, _temp1604 + 5u},( struct
_tagged_string){ _temp1607, _temp1607, _temp1607 + 8u},( struct _tagged_string){
_temp1610, _temp1610, _temp1610 + 4u},( struct _tagged_string){ _temp1613,
_temp1613, _temp1613 + 6u},( struct _tagged_string){ _temp1616, _temp1616,
_temp1616 + 4u},( struct _tagged_string){ _temp1619, _temp1619, _temp1619 + 6u},(
struct _tagged_string){ _temp1622, _temp1622, _temp1622 + 4u},( struct
_tagged_string){ _temp1625, _temp1625, _temp1625 + 9u},( struct _tagged_string){
_temp1628, _temp1628, _temp1628 + 9u},( struct _tagged_string){ _temp1631,
_temp1631, _temp1631 + 6u},( struct _tagged_string){ _temp1634, _temp1634,
_temp1634 + 10u},( struct _tagged_string){ _temp1637, _temp1637, _temp1637 + 7u},(
struct _tagged_string){ _temp1640, _temp1640, _temp1640 + 8u},( struct
_tagged_string){ _temp1643, _temp1643, _temp1643 + 5u},( struct _tagged_string){
_temp1646, _temp1646, _temp1646 + 8u},( struct _tagged_string){ _temp1649,
_temp1649, _temp1649 + 4u},( struct _tagged_string){ _temp1652, _temp1652,
_temp1652 + 7u},( struct _tagged_string){ _temp1655, _temp1655, _temp1655 + 7u},(
struct _tagged_string){ _temp1658, _temp1658, _temp1658 + 8u},( struct
_tagged_string){ _temp1661, _temp1661, _temp1661 + 8u},( struct _tagged_string){
_temp1664, _temp1664, _temp1664 + 6u},( struct _tagged_string){ _temp1667,
_temp1667, _temp1667 + 7u},( struct _tagged_string){ _temp1670, _temp1670,
_temp1670 + 7u},( struct _tagged_string){ _temp1673, _temp1673, _temp1673 + 7u},(
struct _tagged_string){ _temp1676, _temp1676, _temp1676 + 9u},( struct
_tagged_string){ _temp1679, _temp1679, _temp1679 + 7u},( struct _tagged_string){
_temp1682, _temp1682, _temp1682 + 5u},( struct _tagged_string){ _temp1685,
_temp1685, _temp1685 + 8u},( struct _tagged_string){ _temp1688, _temp1688,
_temp1688 + 7u},( struct _tagged_string){ _temp1691, _temp1691, _temp1691 + 7u},(
struct _tagged_string){ _temp1694, _temp1694, _temp1694 + 7u},( struct
_tagged_string){ _temp1697, _temp1697, _temp1697 + 8u},( struct _tagged_string){
_temp1700, _temp1700, _temp1700 + 9u},( struct _tagged_string){ _temp1703,
_temp1703, _temp1703 + 6u},( struct _tagged_string){ _temp1706, _temp1706,
_temp1706 + 6u},( struct _tagged_string){ _temp1709, _temp1709, _temp1709 + 6u},(
struct _tagged_string){ _temp1712, _temp1712, _temp1712 + 6u},( struct
_tagged_string){ _temp1715, _temp1715, _temp1715 + 7u},( struct _tagged_string){
_temp1718, _temp1718, _temp1718 + 6u},( struct _tagged_string){ _temp1721,
_temp1721, _temp1721 + 11u},( struct _tagged_string){ _temp1724, _temp1724,
_temp1724 + 11u},( struct _tagged_string){ _temp1727, _temp1727, _temp1727 + 11u},(
struct _tagged_string){ _temp1730, _temp1730, _temp1730 + 11u},( struct
_tagged_string){ _temp1733, _temp1733, _temp1733 + 11u},( struct _tagged_string){
_temp1736, _temp1736, _temp1736 + 12u},( struct _tagged_string){ _temp1739,
_temp1739, _temp1739 + 13u},( struct _tagged_string){ _temp1742, _temp1742,
_temp1742 + 11u},( struct _tagged_string){ _temp1745, _temp1745, _temp1745 + 11u},(
struct _tagged_string){ _temp1748, _temp1748, _temp1748 + 10u},( struct
_tagged_string){ _temp1751, _temp1751, _temp1751 + 9u},( struct _tagged_string){
_temp1754, _temp1754, _temp1754 + 11u},( struct _tagged_string){ _temp1757,
_temp1757, _temp1757 + 12u},( struct _tagged_string){ _temp1760, _temp1760,
_temp1760 + 11u},( struct _tagged_string){ _temp1763, _temp1763, _temp1763 + 17u},(
struct _tagged_string){ _temp1766, _temp1766, _temp1766 + 7u},( struct
_tagged_string){ _temp1769, _temp1769, _temp1769 + 19u},( struct _tagged_string){
_temp1772, _temp1772, _temp1772 + 18u},( struct _tagged_string){ _temp1775,
_temp1775, _temp1775 + 9u},( struct _tagged_string){ _temp1778, _temp1778,
_temp1778 + 16u},( struct _tagged_string){ _temp1781, _temp1781, _temp1781 + 18u},(
struct _tagged_string){ _temp1784, _temp1784, _temp1784 + 10u},( struct
_tagged_string){ _temp1787, _temp1787, _temp1787 + 4u},( struct _tagged_string){
_temp1790, _temp1790, _temp1790 + 4u},( struct _tagged_string){ _temp1793,
_temp1793, _temp1793 + 4u},( struct _tagged_string){ _temp1796, _temp1796,
_temp1796 + 4u},( struct _tagged_string){ _temp1799, _temp1799, _temp1799 + 4u},(
struct _tagged_string){ _temp1802, _temp1802, _temp1802 + 4u},( struct
_tagged_string){ _temp1805, _temp1805, _temp1805 + 4u},( struct _tagged_string){
_temp1808, _temp1808, _temp1808 + 4u},( struct _tagged_string){ _temp1811,
_temp1811, _temp1811 + 4u},( struct _tagged_string){ _temp1814, _temp1814,
_temp1814 + 4u},( struct _tagged_string){ _temp1817, _temp1817, _temp1817 + 4u},(
struct _tagged_string){ _temp1820, _temp1820, _temp1820 + 4u},( struct
_tagged_string){ _temp1823, _temp1823, _temp1823 + 4u},( struct _tagged_string){
_temp1826, _temp1826, _temp1826 + 4u},( struct _tagged_string){ _temp1829,
_temp1829, _temp1829 + 4u},( struct _tagged_string){ _temp1832, _temp1832,
_temp1832 + 4u},( struct _tagged_string){ _temp1835, _temp1835, _temp1835 + 4u},(
struct _tagged_string){ _temp1838, _temp1838, _temp1838 + 4u},( struct
_tagged_string){ _temp1841, _temp1841, _temp1841 + 4u},( struct _tagged_string){
_temp1844, _temp1844, _temp1844 + 4u},( struct _tagged_string){ _temp1847,
_temp1847, _temp1847 + 4u},( struct _tagged_string){ _temp1850, _temp1850,
_temp1850 + 4u},( struct _tagged_string){ _temp1853, _temp1853, _temp1853 + 4u},(
struct _tagged_string){ _temp1856, _temp1856, _temp1856 + 4u},( struct
_tagged_string){ _temp1859, _temp1859, _temp1859 + 4u},( struct _tagged_string){
_temp1862, _temp1862, _temp1862 + 4u},( struct _tagged_string){ _temp1865,
_temp1865, _temp1865 + 4u},( struct _tagged_string){ _temp1868, _temp1868,
_temp1868 + 5u},( struct _tagged_string){ _temp1871, _temp1871, _temp1871 + 17u},(
struct _tagged_string){ _temp1874, _temp1874, _temp1874 + 21u},( struct
_tagged_string){ _temp1877, _temp1877, _temp1877 + 20u},( struct _tagged_string){
_temp1880, _temp1880, _temp1880 + 21u},( struct _tagged_string){ _temp1883,
_temp1883, _temp1883 + 13u},( struct _tagged_string){ _temp1886, _temp1886,
_temp1886 + 15u},( struct _tagged_string){ _temp1889, _temp1889, _temp1889 + 17u},(
struct _tagged_string){ _temp1892, _temp1892, _temp1892 + 19u},( struct
_tagged_string){ _temp1895, _temp1895, _temp1895 + 12u},( struct _tagged_string){
_temp1898, _temp1898, _temp1898 + 17u},( struct _tagged_string){ _temp1901,
_temp1901, _temp1901 + 23u},( struct _tagged_string){ _temp1904, _temp1904,
_temp1904 + 24u},( struct _tagged_string){ _temp1907, _temp1907, _temp1907 + 15u},(
struct _tagged_string){ _temp1910, _temp1910, _temp1910 + 11u},( struct
_tagged_string){ _temp1913, _temp1913, _temp1913 + 15u},( struct _tagged_string){
_temp1916, _temp1916, _temp1916 + 10u},( struct _tagged_string){ _temp1919,
_temp1919, _temp1919 + 15u},( struct _tagged_string){ _temp1922, _temp1922,
_temp1922 + 5u},( struct _tagged_string){ _temp1925, _temp1925, _temp1925 + 15u},(
struct _tagged_string){ _temp1928, _temp1928, _temp1928 + 15u},( struct
_tagged_string){ _temp1931, _temp1931, _temp1931 + 11u},( struct _tagged_string){
_temp1934, _temp1934, _temp1934 + 22u},( struct _tagged_string){ _temp1937,
_temp1937, _temp1937 + 26u},( struct _tagged_string){ _temp1940, _temp1940,
_temp1940 + 16u},( struct _tagged_string){ _temp1943, _temp1943, _temp1943 + 16u},(
struct _tagged_string){ _temp1946, _temp1946, _temp1946 + 24u},( struct
_tagged_string){ _temp1949, _temp1949, _temp1949 + 25u},( struct _tagged_string){
_temp1952, _temp1952, _temp1952 + 21u},( struct _tagged_string){ _temp1955,
_temp1955, _temp1955 + 22u},( struct _tagged_string){ _temp1958, _temp1958,
_temp1958 + 16u},( struct _tagged_string){ _temp1961, _temp1961, _temp1961 + 19u},(
struct _tagged_string){ _temp1964, _temp1964, _temp1964 + 25u},( struct
_tagged_string){ _temp1967, _temp1967, _temp1967 + 23u},( struct _tagged_string){
_temp1970, _temp1970, _temp1970 + 24u},( struct _tagged_string){ _temp1973,
_temp1973, _temp1973 + 18u},( struct _tagged_string){ _temp1976, _temp1976,
_temp1976 + 17u},( struct _tagged_string){ _temp1979, _temp1979, _temp1979 + 18u},(
struct _tagged_string){ _temp1982, _temp1982, _temp1982 + 17u},( struct
_tagged_string){ _temp1985, _temp1985, _temp1985 + 12u},( struct _tagged_string){
_temp1988, _temp1988, _temp1988 + 11u},( struct _tagged_string){ _temp1991,
_temp1991, _temp1991 + 18u},( struct _tagged_string){ _temp1994, _temp1994,
_temp1994 + 8u},( struct _tagged_string){ _temp1997, _temp1997, _temp1997 + 13u},(
struct _tagged_string){ _temp2000, _temp2000, _temp2000 + 8u},( struct
_tagged_string){ _temp2003, _temp2003, _temp2003 + 4u},( struct _tagged_string){
_temp2006, _temp2006, _temp2006 + 20u},( struct _tagged_string){ _temp2009,
_temp2009, _temp2009 + 20u},( struct _tagged_string){ _temp2012, _temp2012,
_temp2012 + 11u},( struct _tagged_string){ _temp2015, _temp2015, _temp2015 + 14u},(
struct _tagged_string){ _temp2018, _temp2018, _temp2018 + 11u},( struct
_tagged_string){ _temp2021, _temp2021, _temp2021 + 15u},( struct _tagged_string){
_temp2024, _temp2024, _temp2024 + 22u},( struct _tagged_string){ _temp2027,
_temp2027, _temp2027 + 16u},( struct _tagged_string){ _temp2030, _temp2030,
_temp2030 + 17u},( struct _tagged_string){ _temp2033, _temp2033, _temp2033 + 12u},(
struct _tagged_string){ _temp2036, _temp2036, _temp2036 + 18u},( struct
_tagged_string){ _temp2039, _temp2039, _temp2039 + 17u},( struct _tagged_string){
_temp2042, _temp2042, _temp2042 + 12u},( struct _tagged_string){ _temp2045,
_temp2045, _temp2045 + 16u},( struct _tagged_string){ _temp2048, _temp2048,
_temp2048 + 11u},( struct _tagged_string){ _temp2051, _temp2051, _temp2051 + 10u},(
struct _tagged_string){ _temp2054, _temp2054, _temp2054 + 14u},( struct
_tagged_string){ _temp2057, _temp2057, _temp2057 + 15u},( struct _tagged_string){
_temp2060, _temp2060, _temp2060 + 20u},( struct _tagged_string){ _temp2063,
_temp2063, _temp2063 + 27u},( struct _tagged_string){ _temp2066, _temp2066,
_temp2066 + 10u},( struct _tagged_string){ _temp2069, _temp2069, _temp2069 + 18u},(
struct _tagged_string){ _temp2072, _temp2072, _temp2072 + 21u},( struct
_tagged_string){ _temp2075, _temp2075, _temp2075 + 19u},( struct _tagged_string){
_temp2078, _temp2078, _temp2078 + 16u},( struct _tagged_string){ _temp2081,
_temp2081, _temp2081 + 11u},( struct _tagged_string){ _temp2084, _temp2084,
_temp2084 + 20u},( struct _tagged_string){ _temp2087, _temp2087, _temp2087 + 15u},(
struct _tagged_string){ _temp2090, _temp2090, _temp2090 + 20u},( struct
_tagged_string){ _temp2093, _temp2093, _temp2093 + 15u},( struct _tagged_string){
_temp2096, _temp2096, _temp2096 + 8u},( struct _tagged_string){ _temp2099,
_temp2099, _temp2099 + 19u},( struct _tagged_string){ _temp2102, _temp2102,
_temp2102 + 20u},( struct _tagged_string){ _temp2105, _temp2105, _temp2105 + 14u},(
struct _tagged_string){ _temp2108, _temp2108, _temp2108 + 19u},( struct
_tagged_string){ _temp2111, _temp2111, _temp2111 + 20u},( struct _tagged_string){
_temp2114, _temp2114, _temp2114 + 11u},( struct _tagged_string){ _temp2117,
_temp2117, _temp2117 + 22u},( struct _tagged_string){ _temp2120, _temp2120,
_temp2120 + 20u},( struct _tagged_string){ _temp2123, _temp2123, _temp2123 + 23u},(
struct _tagged_string){ _temp2126, _temp2126, _temp2126 + 20u},( struct
_tagged_string){ _temp2129, _temp2129, _temp2129 + 22u},( struct _tagged_string){
_temp2132, _temp2132, _temp2132 + 23u},( struct _tagged_string){ _temp2135,
_temp2135, _temp2135 + 24u},( struct _tagged_string){ _temp2138, _temp2138,
_temp2138 + 24u},( struct _tagged_string){ _temp2141, _temp2141, _temp2141 + 15u},(
struct _tagged_string){ _temp2144, _temp2144, _temp2144 + 20u},( struct
_tagged_string){ _temp2147, _temp2147, _temp2147 + 22u},( struct _tagged_string){
_temp2150, _temp2150, _temp2150 + 17u},( struct _tagged_string){ _temp2153,
_temp2153, _temp2153 + 20u},( struct _tagged_string){ _temp2156, _temp2156,
_temp2156 + 26u},( struct _tagged_string){ _temp2159, _temp2159, _temp2159 + 16u},(
struct _tagged_string){ _temp2162, _temp2162, _temp2162 + 17u},( struct
_tagged_string){ _temp2165, _temp2165, _temp2165 + 14u},( struct _tagged_string){
_temp2168, _temp2168, _temp2168 + 15u},( struct _tagged_string){ _temp2171,
_temp2171, _temp2171 + 19u},( struct _tagged_string){ _temp2174, _temp2174,
_temp2174 + 19u},( struct _tagged_string){ _temp2177, _temp2177, _temp2177 + 25u},(
struct _tagged_string){ _temp2180, _temp2180, _temp2180 + 26u},( struct
_tagged_string){ _temp2183, _temp2183, _temp2183 + 9u},( struct _tagged_string){
_temp2186, _temp2186, _temp2186 + 20u}}; static short Cyc_yyr1[ 391u]={ (short)0,
(short)124, (short)125, (short)125, (short)125, (short)125, (short)125, (short)125,
(short)125, (short)126, (short)126, (short)127, (short)127, (short)127, (short)127,
(short)128, (short)128, (short)129, (short)130, (short)131, (short)132, (short)133,
(short)133, (short)133, (short)134, (short)134, (short)135, (short)135, (short)135,
(short)135, (short)135, (short)135, (short)135, (short)135, (short)136, (short)136,
(short)136, (short)136, (short)136, (short)136, (short)136, (short)137, (short)137,
(short)138, (short)139, (short)139, (short)140, (short)140, (short)140, (short)140,
(short)141, (short)141, (short)141, (short)141, (short)141, (short)141, (short)141,
(short)141, (short)141, (short)141, (short)141, (short)141, (short)141, (short)141,
(short)141, (short)141, (short)141, (short)141, (short)142, (short)142, (short)143,
(short)143, (short)143, (short)144, (short)144, (short)145, (short)145, (short)146,
(short)146, (short)147, (short)147, (short)147, (short)147, (short)147, (short)148,
(short)148, (short)148, (short)149, (short)149, (short)150, (short)151, (short)151,
(short)152, (short)153, (short)153, (short)154, (short)154, (short)155, (short)156,
(short)156, (short)156, (short)156, (short)157, (short)158, (short)158, (short)159,
(short)159, (short)159, (short)160, (short)160, (short)160, (short)160, (short)161,
(short)161, (short)162, (short)162, (short)162, (short)162, (short)163, (short)163,
(short)164, (short)164, (short)165, (short)165, (short)165, (short)165, (short)165,
(short)165, (short)165, (short)165, (short)165, (short)165, (short)165, (short)166,
(short)166, (short)166, (short)166, (short)167, (short)167, (short)167, (short)167,
(short)167, (short)168, (short)168, (short)169, (short)169, (short)169, (short)170,
(short)170, (short)171, (short)171, (short)171, (short)172, (short)172, (short)173,
(short)173, (short)173, (short)173, (short)174, (short)174, (short)174, (short)174,
(short)175, (short)175, (short)176, (short)176, (short)176, (short)177, (short)178,
(short)178, (short)179, (short)179, (short)180, (short)180, (short)180, (short)180,
(short)181, (short)181, (short)181, (short)181, (short)182, (short)183, (short)183,
(short)184, (short)184, (short)185, (short)185, (short)186, (short)186, (short)186,
(short)186, (short)187, (short)187, (short)188, (short)188, (short)188, (short)189,
(short)189, (short)189, (short)189, (short)189, (short)189, (short)189, (short)189,
(short)189, (short)189, (short)189, (short)189, (short)189, (short)189, (short)190,
(short)190, (short)190, (short)190, (short)190, (short)190, (short)190, (short)190,
(short)190, (short)190, (short)191, (short)192, (short)192, (short)193, (short)193,
(short)194, (short)194, (short)195, (short)195, (short)195, (short)196, (short)196,
(short)196, (short)196, (short)197, (short)197, (short)197, (short)197, (short)197,
(short)197, (short)198, (short)198, (short)198, (short)198, (short)198, (short)198,
(short)198, (short)198, (short)198, (short)198, (short)198, (short)198, (short)198,
(short)198, (short)199, (short)199, (short)199, (short)199, (short)199, (short)199,
(short)199, (short)199, (short)200, (short)200, (short)200, (short)200, (short)200,
(short)200, (short)200, (short)200, (short)200, (short)200, (short)200, (short)200,
(short)200, (short)200, (short)201, (short)201, (short)202, (short)202, (short)203,
(short)203, (short)204, (short)205, (short)205, (short)206, (short)206, (short)207,
(short)207, (short)208, (short)208, (short)208, (short)208, (short)208, (short)208,
(short)208, (short)208, (short)208, (short)208, (short)208, (short)209, (short)209,
(short)209, (short)209, (short)209, (short)209, (short)209, (short)210, (short)211,
(short)211, (short)212, (short)212, (short)213, (short)213, (short)214, (short)214,
(short)215, (short)215, (short)216, (short)216, (short)216, (short)217, (short)217,
(short)217, (short)217, (short)217, (short)218, (short)218, (short)218, (short)219,
(short)219, (short)219, (short)220, (short)220, (short)220, (short)220, (short)221,
(short)221, (short)222, (short)222, (short)222, (short)222, (short)222, (short)222,
(short)222, (short)222, (short)222, (short)222, (short)222, (short)222, (short)223,
(short)223, (short)223, (short)223, (short)223, (short)223, (short)224, (short)224,
(short)224, (short)225, (short)225, (short)225, (short)225, (short)225, (short)225,
(short)225, (short)225, (short)225, (short)225, (short)225, (short)225, (short)225,
(short)225, (short)226, (short)226, (short)226, (short)226, (short)226, (short)226,
(short)226, (short)226, (short)226, (short)227, (short)228, (short)228, (short)229,
(short)229, (short)229, (short)229, (short)230, (short)230}; static short Cyc_yyr2[
391u]={ (short)0, (short)1, (short)2, (short)3, (short)5, (short)3, (short)5,
(short)6, (short)0, (short)1, (short)1, (short)2, (short)3, (short)3, (short)4,
(short)3, (short)4, (short)2, (short)1, (short)2, (short)1, (short)2, (short)3,
(short)5, (short)1, (short)2, (short)2, (short)3, (short)2, (short)3, (short)2,
(short)3, (short)2, (short)3, (short)1, (short)1, (short)1, (short)1, (short)2,
(short)1, (short)1, (short)0, (short)1, (short)6, (short)1, (short)3, (short)1,
(short)1, (short)4, (short)4, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)3, (short)2, (short)4, (short)4, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)5, (short)2, (short)1, (short)3, (short)1, (short)3,
(short)4, (short)6, (short)6, (short)3, (short)3, (short)0, (short)3, (short)3,
(short)1, (short)1, (short)1, (short)1, (short)2, (short)1, (short)1, (short)3,
(short)1, (short)3, (short)3, (short)2, (short)3, (short)2, (short)3, (short)1,
(short)1, (short)3, (short)1, (short)2, (short)3, (short)6, (short)3, (short)4,
(short)5, (short)1, (short)1, (short)1, (short)2, (short)3, (short)3, (short)1,
(short)5, (short)1, (short)2, (short)1, (short)3, (short)3, (short)4, (short)4,
(short)3, (short)5, (short)4, (short)4, (short)4, (short)2, (short)3, (short)4,
(short)4, (short)5, (short)1, (short)1, (short)4, (short)4, (short)1, (short)0,
(short)1, (short)1, (short)3, (short)1, (short)1, (short)2, (short)1, (short)3,
(short)3, (short)1, (short)3, (short)2, (short)3, (short)1, (short)3, (short)1,
(short)3, (short)3, (short)5, (short)1, (short)3, (short)2, (short)1, (short)2,
(short)1, (short)1, (short)3, (short)1, (short)1, (short)2, (short)3, (short)4,
(short)8, (short)1, (short)2, (short)3, (short)4, (short)2, (short)1, (short)2,
(short)3, (short)2, (short)1, (short)2, (short)1, (short)2, (short)3, (short)3,
(short)1, (short)3, (short)1, (short)1, (short)2, (short)3, (short)2, (short)3,
(short)3, (short)4, (short)2, (short)4, (short)3, (short)3, (short)5, (short)4,
(short)4, (short)4, (short)2, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)6, (short)3, (short)2, (short)2, (short)3, (short)1, (short)2,
(short)2, (short)3, (short)1, (short)2, (short)1, (short)1, (short)1, (short)5,
(short)7, (short)7, (short)6, (short)0, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)5, (short)7, (short)6, (short)7, (short)7, (short)8, (short)7,
(short)8, (short)8, (short)9, (short)6, (short)7, (short)7, (short)8, (short)3,
(short)2, (short)2, (short)2, (short)3, (short)2, (short)4, (short)5, (short)1,
(short)3, (short)1, (short)2, (short)1, (short)1, (short)1, (short)1, (short)5,
(short)4, (short)4, (short)5, (short)2, (short)2, (short)0, (short)1, (short)1,
(short)3, (short)1, (short)2, (short)1, (short)1, (short)3, (short)1, (short)3,
(short)1, (short)3, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)5, (short)2,
(short)2, (short)2, (short)5, (short)5, (short)1, (short)1, (short)3, (short)1,
(short)3, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)1,
(short)3, (short)3, (short)1, (short)3, (short)3, (short)3, (short)3, (short)1,
(short)3, (short)3, (short)1, (short)3, (short)3, (short)1, (short)3, (short)3,
(short)3, (short)1, (short)4, (short)1, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)4, (short)2, (short)4, (short)7, (short)9, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)4, (short)3, (short)4, (short)3, (short)3, (short)3, (short)3,
(short)2, (short)2, (short)6, (short)7, (short)4, (short)4, (short)1, (short)1,
(short)1, (short)3, (short)2, (short)5, (short)4, (short)4, (short)5, (short)1,
(short)1, (short)3, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1};
static short Cyc_yydefact[ 748u]={ (short)8, (short)34, (short)35, (short)36,
(short)37, (short)39, (short)50, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, (short)70, (short)71, (short)72,
(short)87, (short)88, (short)41, (short)0, (short)0, (short)40, (short)0,
(short)0, (short)112, (short)113, (short)0, (short)389, (short)63, (short)390,
(short)84, (short)0, (short)51, (short)0, (short)137, (short)138, (short)141,
(short)1, (short)8, (short)9, (short)0, (short)0, (short)10, (short)0, (short)41,
(short)41, (short)41, (short)60, (short)61, (short)0, (short)62, (short)0,
(short)0, (short)120, (short)0, (short)142, (short)122, (short)38, (short)0,
(short)32, (short)42, (short)74, (short)268, (short)264, (short)267, (short)266,
(short)0, (short)262, (short)0, (short)0, (short)0, (short)0, (short)0, (short)269,
(short)17, (short)19, (short)0, (short)0, (short)0, (short)65, (short)0, (short)0,
(short)0, (short)0, (short)2, (short)8, (short)8, (short)8, (short)8, (short)21,
(short)0, (short)92, (short)93, (short)95, (short)26, (short)28, (short)30,
(short)84, (short)0, (short)84, (short)144, (short)146, (short)0, (short)84,
(short)37, (short)0, (short)24, (short)0, (short)0, (short)11, (short)0, (short)0,
(short)0, (short)132, (short)121, (short)41, (short)143, (short)8, (short)0,
(short)33, (short)0, (short)0, (short)276, (short)275, (short)265, (short)274,
(short)0, (short)0, (short)0, (short)41, (short)41, (short)185, (short)187,
(short)0, (short)68, (short)69, (short)64, (short)191, (short)0, (short)123,
(short)165, (short)0, (short)162, (short)0, (short)388, (short)0, (short)0,
(short)0, (short)0, (short)350, (short)351, (short)352, (short)353, (short)354,
(short)355, (short)0, (short)0, (short)0, (short)0, (short)0, (short)385,
(short)375, (short)386, (short)387, (short)0, (short)0, (short)0, (short)0,
(short)358, (short)0, (short)356, (short)357, (short)0, (short)287, (short)300,
(short)308, (short)310, (short)312, (short)314, (short)316, (short)318, (short)321,
(short)326, (short)329, (short)332, (short)336, (short)0, (short)0, (short)338,
(short)359, (short)374, (short)373, (short)0, (short)3, (short)0, (short)5,
(short)0, (short)22, (short)0, (short)0, (short)0, (short)12, (short)27, (short)29,
(short)31, (short)83, (short)0, (short)89, (short)90, (short)0, (short)82,
(short)0, (short)84, (short)0, (short)109, (short)38, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)389, (short)221, (short)223,
(short)0, (short)229, (short)227, (short)0, (short)228, (short)210, (short)211,
(short)212, (short)0, (short)225, (short)213, (short)214, (short)215, (short)0,
(short)285, (short)25, (short)13, (short)95, (short)168, (short)0, (short)127,
(short)0, (short)149, (short)0, (short)167, (short)0, (short)124, (short)0,
(short)133, (short)0, (short)0, (short)77, (short)0, (short)75, (short)263,
(short)278, (short)0, (short)277, (short)0, (short)0, (short)276, (short)158,
(short)188, (short)0, (short)98, (short)100, (short)0, (short)0, (short)193,
(short)186, (short)194, (short)67, (short)0, (short)86, (short)0, (short)85,
(short)0, (short)164, (short)193, (short)166, (short)66, (short)0, (short)0,
(short)346, (short)302, (short)336, (short)0, (short)303, (short)304, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)339, (short)340,
(short)0, (short)0, (short)0, (short)0, (short)342, (short)343, (short)341,
(short)139, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)290, (short)291, (short)292,
(short)293, (short)294, (short)295, (short)296, (short)297, (short)298, (short)299,
(short)289, (short)0, (short)0, (short)344, (short)0, (short)367, (short)368,
(short)0, (short)0, (short)0, (short)377, (short)0, (short)0, (short)140,
(short)18, (short)8, (short)20, (short)8, (short)94, (short)96, (short)171,
(short)170, (short)14, (short)0, (short)79, (short)91, (short)0, (short)0,
(short)102, (short)103, (short)105, (short)0, (short)145, (short)110, (short)84,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)255,
(short)256, (short)257, (short)0, (short)0, (short)259, (short)0, (short)218,
(short)219, (short)0, (short)0, (short)0, (short)0, (short)95, (short)224,
(short)226, (short)222, (short)0, (short)156, (short)0, (short)0, (short)152,
(short)126, (short)0, (short)0, (short)129, (short)0, (short)131, (short)130,
(short)125, (short)147, (short)135, (short)134, (short)8, (short)47, (short)46,
(short)0, (short)44, (short)0, (short)73, (short)0, (short)271, (short)0,
(short)23, (short)272, (short)0, (short)0, (short)0, (short)0, (short)181,
(short)280, (short)283, (short)0, (short)282, (short)0, (short)0, (short)0,
(short)189, (short)99, (short)101, (short)0, (short)201, (short)0, (short)0,
(short)197, (short)0, (short)195, (short)0, (short)0, (short)0, (short)209,
(short)190, (short)192, (short)163, (short)0, (short)0, (short)172, (short)176,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)376, (short)383, (short)0, (short)382,
(short)309, (short)0, (short)311, (short)313, (short)315, (short)317, (short)319,
(short)320, (short)324, (short)325, (short)322, (short)323, (short)327, (short)328,
(short)330, (short)331, (short)333, (short)334, (short)335, (short)288, (short)0,
(short)365, (short)366, (short)361, (short)0, (short)363, (short)364, (short)0,
(short)0, (short)0, (short)4, (short)6, (short)0, (short)307, (short)106,
(short)97, (short)0, (short)0, (short)0, (short)111, (short)0, (short)114,
(short)118, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)254, (short)258, (short)0, (short)0, (short)0, (short)217, (short)0,
(short)220, (short)0, (short)15, (short)286, (short)0, (short)154, (short)0,
(short)128, (short)0, (short)151, (short)150, (short)169, (short)148, (short)136,
(short)7, (short)0, (short)0, (short)0, (short)78, (short)76, (short)279,
(short)184, (short)0, (short)281, (short)180, (short)182, (short)273, (short)0,
(short)270, (short)160, (short)159, (short)0, (short)203, (short)196, (short)199,
(short)0, (short)204, (short)0, (short)0, (short)198, (short)0, (short)345,
(short)0, (short)173, (short)0, (short)177, (short)371, (short)372, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)337, (short)379,
(short)0, (short)0, (short)347, (short)362, (short)360, (short)380, (short)0,
(short)0, (short)81, (short)104, (short)107, (short)80, (short)108, (short)115,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)234, (short)260, (short)0, (short)0,
(short)16, (short)157, (short)155, (short)153, (short)0, (short)0, (short)43,
(short)45, (short)183, (short)284, (short)0, (short)202, (short)0, (short)206,
(short)208, (short)207, (short)200, (short)0, (short)174, (short)178, (short)0,
(short)0, (short)305, (short)306, (short)0, (short)381, (short)0, (short)384,
(short)301, (short)378, (short)117, (short)116, (short)0, (short)230, (short)234,
(short)240, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)261, (short)0, (short)48,
(short)49, (short)161, (short)205, (short)0, (short)179, (short)0, (short)0,
(short)369, (short)0, (short)0, (short)0, (short)0, (short)0, (short)242,
(short)0, (short)0, (short)0, (short)250, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)233, (short)216, (short)0,
(short)348, (short)0, (short)370, (short)119, (short)231, (short)232, (short)241,
(short)243, (short)244, (short)0, (short)252, (short)251, (short)0, (short)246,
(short)0, (short)0, (short)0, (short)0, (short)234, (short)235, (short)0,
(short)0, (short)245, (short)253, (short)247, (short)248, (short)0, (short)0,
(short)234, (short)236, (short)175, (short)349, (short)249, (short)234, (short)237,
(short)234, (short)238, (short)239, (short)0, (short)0, (short)0}; static short
Cyc_yydefgoto[ 107u]={ (short)745, (short)39, (short)40, (short)41, (short)236,
(short)42, (short)364, (short)43, (short)366, (short)44, (short)202, (short)45,
(short)46, (short)61, (short)62, (short)427, (short)428, (short)47, (short)138,
(short)48, (short)49, (short)266, (short)267, (short)50, (short)81, (short)51,
(short)208, (short)209, (short)92, (short)93, (short)94, (short)210, (short)133,
(short)376, (short)377, (short)378, (short)52, (short)53, (short)524, (short)525,
(short)54, (short)55, (short)56, (short)57, (short)117, (short)104, (short)423,
(short)453, (short)411, (short)412, (short)278, (short)257, (short)144, (short)258,
(short)259, (short)468, (short)369, (short)469, (short)470, (short)439, (short)440,
(short)134, (short)139, (short)140, (short)454, (short)285, (short)239, (short)240,
(short)241, (short)242, (short)243, (short)244, (short)245, (short)674, (short)246,
(short)247, (short)270, (short)271, (short)272, (short)442, (short)443, (short)444,
(short)248, (short)249, (short)350, (short)175, (short)518, (short)176, (short)177,
(short)178, (short)179, (short)180, (short)181, (short)182, (short)183, (short)184,
(short)185, (short)186, (short)187, (short)188, (short)189, (short)190, (short)191,
(short)482, (short)483, (short)192, (short)193}; static short Cyc_yypact[ 748u]={
(short)1764, - (short)-32768, - (short)-32768, - (short)-32768, - (short)69, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)28, (short)185, (short)529, - (short)-32768, (short)185, (short)2, -
(short)-32768, - (short)-32768, (short)22, - (short)-32768, (short)52, - (short)-32768,
(short)36, (short)314, - (short)-32768, (short)64, (short)101, (short)124, -
(short)-32768, - (short)-32768, (short)1764, - (short)-32768, (short)137,
(short)205, - (short)-32768, (short)310, - (short)28, - (short)28, - (short)28,
- (short)-32768, - (short)-32768, (short)180, - (short)-32768, (short)104,
(short)2062, (short)111, (short)156, - (short)5, - (short)-32768, (short)158,
(short)182, (short)2122, - (short)-32768, (short)194, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)529, - (short)-32768, (short)199,
(short)229, (short)269, (short)529, (short)219, (short)166, - (short)-32768, -
(short)-32768, (short)2262, (short)145, (short)2262, - (short)-32768, (short)225,
(short)2329, (short)3840, (short)3840, - (short)-32768, (short)1764, (short)1764,
(short)1764, (short)1764, - (short)-32768, (short)282, (short)267, - (short)-32768,
(short)1999, (short)2122, (short)2122, (short)2122, (short)36, (short)2329,
(short)36, (short)303, - (short)-32768, (short)185, (short)140, (short)306,
(short)1169, - (short)-32768, (short)2062, (short)310, - (short)-32768, (short)577,
(short)2262, (short)2704, - (short)-32768, (short)111, - (short)28, - (short)-32768,
(short)1764, (short)292, - (short)-32768, (short)185, (short)299, (short)529, -
(short)-32768, - (short)-32768, - (short)-32768, (short)3840, - (short)47,
(short)187, - (short)28, - (short)28, (short)408, - (short)-32768, (short)94, -
(short)-32768, - (short)-32768, - (short)-32768, (short)291, - (short)22, -
(short)-32768, (short)224, (short)243, - (short)-32768, (short)4053, - (short)-32768,
(short)3840, (short)3911, (short)312, (short)318, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)320,
(short)324, (short)332, (short)4124, (short)4124, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)1527, (short)336, (short)4195, (short)4195,
- (short)-32768, (short)4195, - (short)-32768, - (short)-32768, (short)317, -
(short)-32768, - (short)35, (short)374, (short)334, (short)331, (short)351,
(short)261, (short)97, (short)342, (short)315, (short)65, - (short)-32768,
(short)627, (short)379, (short)4195, (short)105, - (short)-32768, - (short)-32768,
- (short)11, (short)383, - (short)-32768, (short)391, - (short)-32768, (short)397,
- (short)-32768, (short)314, (short)2775, (short)2062, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)406, (short)420, (short)2329,
- (short)-32768, (short)393, (short)435, (short)145, (short)36, (short)185,
(short)450, - (short)-32768, (short)437, (short)452, (short)455, (short)2146,
(short)457, (short)473, (short)472, (short)476, (short)2846, (short)2146,
(short)99, (short)2146, (short)2146, (short)44, (short)466, - (short)-32768, -
(short)-32768, (short)474, - (short)-32768, - (short)-32768, (short)310, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)477,
(short)1290, - (short)-32768, - (short)-32768, - (short)-32768, (short)184, -
(short)-32768, - (short)-32768, - (short)-32768, (short)479, - (short)-32768,
(short)89, - (short)-32768, (short)475, (short)191, (short)478, (short)480,
(short)46, - (short)-32768, (short)470, (short)10, (short)483, (short)6, (short)496,
(short)501, (short)502, - (short)-32768, - (short)-32768, (short)499, (short)500,
(short)202, (short)701, (short)529, - (short)55, - (short)-32768, (short)505,
(short)2329, (short)2329, (short)1939, (short)2917, - (short)27, - (short)-32768,
(short)122, - (short)-32768, (short)89, - (short)-32768, (short)2262, - (short)-32768,
(short)1879, - (short)-32768, (short)33, - (short)-32768, - (short)-32768,
(short)2329, (short)1527, - (short)-32768, - (short)-32768, - (short)-32768,
(short)2348, - (short)-32768, (short)531, (short)3840, (short)1824, (short)571,
(short)3840, (short)3840, (short)1527, - (short)-32768, - (short)-32768, (short)1290,
(short)506, (short)333, (short)3840, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)4195, (short)3840, (short)4195, (short)4195, (short)4195,
(short)4195, (short)4195, (short)4195, (short)4195, (short)4195, (short)4195,
(short)4195, (short)4195, (short)4195, (short)4195, (short)4195, (short)4195,
(short)4195, (short)4195, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)3840, (short)3840, -
(short)-32768, (short)150, - (short)-32768, - (short)-32768, (short)2988,
(short)160, (short)3840, - (short)-32768, (short)2615, (short)503, - (short)-32768,
- (short)-32768, (short)1764, - (short)-32768, (short)1764, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)2329, -
(short)-32768, - (short)-32768, (short)3840, (short)513, (short)508, - (short)-32768,
(short)507, (short)2329, - (short)-32768, - (short)-32768, (short)36, (short)185,
(short)3840, (short)3840, (short)3840, (short)586, (short)1411, (short)519, -
(short)-32768, - (short)-32768, - (short)-32768, (short)207, (short)578, -
(short)-32768, (short)3059, - (short)-32768, - (short)-32768, (short)2146,
(short)533, (short)2146, (short)1643, (short)1999, - (short)-32768, - (short)-32768,
- (short)-32768, (short)3840, (short)542, (short)230, (short)530, (short)516, -
(short)-32768, (short)89, (short)2276, - (short)-32768, (short)547, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)10,
(short)1764, - (short)-32768, (short)535, (short)536, (short)537, (short)185, -
(short)-32768, (short)3840, - (short)-32768, (short)529, - (short)-32768, -
(short)-32768, (short)554, (short)3840, (short)529, (short)9, - (short)-32768, -
(short)-32768, - (short)-32768, (short)544, (short)541, (short)545, (short)145,
(short)555, - (short)-32768, - (short)-32768, - (short)-32768, (short)89, -
(short)-32768, (short)548, (short)550, - (short)-32768, (short)543, (short)122,
(short)903, (short)2262, (short)3130, - (short)-32768, - (short)-32768, (short)291,
- (short)-32768, (short)553, (short)568, - (short)-32768, - (short)-32768,
(short)8, (short)2775, (short)356, (short)556, (short)314, (short)558, (short)363,
(short)557, (short)559, (short)563, (short)3982, - (short)-32768, - (short)-32768,
(short)561, (short)564, (short)374, (short)162, (short)334, (short)331, (short)351,
(short)261, (short)97, (short)97, (short)342, (short)342, (short)342, (short)342,
(short)315, (short)315, (short)65, (short)65, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)566, - (short)-32768, - (short)-32768,
- (short)-32768, (short)567, - (short)-32768, - (short)-32768, (short)102,
(short)121, (short)2262, - (short)-32768, - (short)-32768, (short)574, - (short)-32768,
- (short)-32768, - (short)-32768, (short)393, (short)3840, (short)579, - (short)-32768,
(short)581, (short)227, (short)214, (short)368, (short)375, (short)382, (short)576,
(short)3201, (short)3272, (short)246, - (short)-32768, - (short)-32768, (short)585,
(short)560, (short)582, - (short)-32768, (short)580, - (short)-32768, (short)2062,
- (short)-32768, - (short)-32768, (short)145, - (short)-32768, (short)590, -
(short)-32768, (short)89, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)409, (short)583, (short)6,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)584,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)844,
- (short)-32768, (short)588, - (short)-32768, (short)591, - (short)-32768, -
(short)-32768, - (short)-32768, (short)89, - (short)-32768, (short)592, (short)57,
- (short)-32768, (short)587, (short)594, (short)593, - (short)-32768, (short)2437,
- (short)-32768, - (short)-32768, - (short)-32768, (short)2062, (short)2329,
(short)3911, (short)665, (short)594, (short)610, (short)2615, - (short)-32768, -
(short)-32768, (short)3840, (short)3840, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)2615, (short)241, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)185,
(short)185, (short)612, (short)2146, (short)616, (short)2146, (short)3840,
(short)3343, (short)268, (short)3414, (short)271, (short)3485, (short)491, -
(short)-32768, (short)618, (short)628, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)619, (short)620, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)555, - (short)-32768,
(short)621, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)3840, - (short)-32768, - (short)-32768, (short)2775, (short)622, -
(short)-32768, (short)531, (short)617, - (short)-32768, (short)274, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)2329,
(short)699, (short)491, - (short)-32768, (short)414, (short)2146, (short)422,
(short)3556, (short)2146, (short)424, (short)3627, (short)3698, (short)272,
(short)529, (short)623, (short)629, - (short)-32768, (short)2146, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)258, - (short)-32768,
(short)624, (short)2329, - (short)-32768, (short)2526, (short)426, (short)2146,
(short)630, (short)633, - (short)-32768, (short)2146, (short)2146, (short)429, -
(short)-32768, (short)2146, (short)2146, (short)438, (short)2146, (short)440,
(short)3769, - (short)42, (short)1290, - (short)-32768, - (short)-32768, (short)3840,
- (short)-32768, (short)632, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)2146,
- (short)-32768, - (short)-32768, (short)2146, - (short)-32768, (short)2146,
(short)2146, (short)443, (short)3840, (short)1010, - (short)-32768, (short)289,
(short)634, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)2146, (short)273, (short)491, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)1010, - (short)-32768, (short)491, - (short)-32768, -
(short)-32768, (short)739, (short)742, - (short)-32768}; static short Cyc_yypgoto[
107u]={ - (short)-32768, (short)91, - (short)-32768, (short)439, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)50,
- (short)41, - (short)54, - (short)-32768, (short)30, - (short)43, (short)188, -
(short)-32768, (short)17, - (short)183, - (short)20, - (short)-32768, - (short)-32768,
(short)316, - (short)-32768, (short)11, - (short)-32768, - (short)161, - (short)-32768,
- (short)-32768, - (short)-32768, (short)549, (short)539, (short)76, - (short)-32768,
- (short)-32768, (short)231, - (short)-32768, - (short)-32768, - (short)49, -
(short)-32768, - (short)9, - (short)46, - (short)107, - (short)-32768, - (short)-32768,
(short)695, - (short)-32768, - (short)94, - (short)381, (short)467, - (short)385,
- (short)81, - (short)274, - (short)-32768, - (short)-32768, - (short)186, -
(short)145, - (short)354, - (short)257, - (short)-32768, (short)319, - (short)146,
- (short)59, - (short)108, (short)3, - (short)139, (short)157, - (short)-32768,
- (short)-32768, - (short)45, - (short)230, - (short)-32768, - (short)-32768, -
(short)627, - (short)-32768, - (short)-32768, - (short)21, (short)482, - (short)-32768,
(short)195, - (short)-32768, - (short)-32768, (short)165, (short)152, - (short)-32768,
- (short)136, - (short)366, - (short)140, (short)433, (short)442, (short)432,
(short)441, (short)434, (short)238, (short)113, (short)235, (short)237, (short)4,
(short)572, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)260, - (short)-32768, - (short)-32768, (short)16}; static short Cyc_yytable[
4319u]={ (short)110, (short)74, (short)143, (short)302, (short)108, (short)260,
(short)512, (short)121, (short)303, (short)111, (short)116, (short)299, (short)115,
(short)109, (short)406, (short)368, (short)58, (short)438, (short)256, (short)135,
(short)313, (short)59, (short)464, (short)425, (short)82, (short)547, (short)283,
(short)15, (short)16, (short)17, (short)381, (short)724, (short)446, (short)550,
(short)689, (short)293, (short)95, (short)63, (short)75, (short)320, (short)76,
(short)110, (short)204, (short)205, (short)206, (short)108, (short)288, (short)123,
(short)447, (short)58, (short)203, (short)274, (short)127, (short)238, (short)275,
(short)110, (short)58, (short)237, (short)132, (short)250, (short)132, (short)58,
(short)571, (short)132, (short)251, (short)560, (short)725, (short)101, (short)60,
(short)105, (short)572, (short)563, (short)58, (short)115, (short)281, (short)359,
(short)96, (short)97, (short)98, (short)321, (short)132, (short)289, (short)478,
(short)282, (short)75, (short)290, (short)129, (short)360, (short)102, (short)75,
(short)77, (short)504, (short)132, (short)132, (short)426, (short)131, (short)508,
(short)131, (short)736, (short)103, (short)131, (short)252, (short)361, (short)58,
(short)58, (short)58, (short)58, (short)584, (short)741, (short)565, (short)207,
(short)585, (short)212, (short)743, (short)418, (short)744, (short)216, (short)131,
(short)436, (short)437, (short)214, (short)29, (short)36, (short)37, (short)38,
(short)641, (short)58, (short)31, (short)78, (short)131, (short)131, (short)86,
(short)400, (short)292, (short)291, (short)58, (short)284, (short)538, (short)268,
(short)79, (short)75, (short)464, (short)80, (short)282, (short)457, (short)294,
(short)132, (short)263, (short)110, (short)289, (short)401, (short)465, (short)250,
(short)419, (short)457, (short)608, (short)422, (short)371, (short)58, (short)142,
(short)289, (short)279, (short)280, (short)477, (short)642, (short)83, (short)328,
(short)329, (short)630, (short)353, (short)354, (short)355, (short)316, (short)317,
(short)283, (short)318, (short)211, (short)336, (short)195, (short)196, (short)197,
(short)198, (short)409, (short)131, (short)293, (short)337, (short)338, (short)410,
(short)142, (short)132, (short)238, (short)252, (short)29, (short)352, (short)237,
(short)639, (short)396, (short)102, (short)31, (short)84, (short)397, (short)286,
(short)379, (short)330, (short)331, (short)408, (short)356, (short)60, (short)103,
(short)287, (short)264, (short)516, (short)112, (short)602, (short)357, (short)358,
(short)58, (short)113, (short)60, (short)522, (short)603, (short)114, (short)85,
(short)458, (short)604, (short)382, (short)131, (short)58, (short)459, (short)404,
(short)463, (short)383, (short)460, (short)136, (short)87, (short)88, (short)174,
(short)194, (short)505, (short)517, (short)137, (short)653, (short)461, (short)421,
(short)29, (short)506, (short)80, (short)116, (short)509, (short)215, (short)31,
(short)473, (short)679, (short)441, (short)58, (short)510, (short)119, (short)33,
(short)238, (short)132, (short)132, (short)132, (short)237, (short)570, -
(short)84, (short)408, (short)262, - (short)84, (short)29, (short)132, (short)599,
(short)132, (short)80, (short)29, (short)31, (short)99, (short)132, (short)132,
(short)100, (short)31, (short)276, (short)407, (short)82, (short)120, (short)586,
(short)211, (short)277, (short)408, (short)414, (short)132, (short)75, (short)75,
(short)122, (short)273, (short)415, (short)517, (short)131, (short)131, (short)131,
(short)434, (short)124, (short)517, (short)89, (short)90, (short)535, (short)408,
(short)131, (short)58, (short)131, (short)58, (short)408, (short)438, (short)29,
(short)131, (short)131, - (short)84, (short)554, (short)125, (short)31, (short)128,
(short)80, (short)58, (short)143, (short)276, (short)611, (short)291, (short)131,
(short)141, (short)647, (short)546, (short)612, (short)314, (short)326, (short)327,
(short)282, (short)110, (short)36, (short)37, (short)38, (short)532, (short)500,
(short)501, (short)502, (short)622, (short)289, (short)295, (short)296, (short)647,
(short)656, (short)408, (short)110, (short)579, (short)132, (short)370, (short)108,
(short)449, (short)450, (short)142, (short)126, (short)543, (short)132, (short)408,
(short)628, (short)542, (short)578, (short)666, (short)706, (short)142, (short)669,
(short)701, (short)200, (short)408, (short)142, (short)685, (short)408, (short)408,
(short)408, (short)686, (short)388, (short)199, (short)58, (short)740, (short)58,
(short)132, (short)395, (short)517, (short)398, (short)399, (short)737, (short)131,
(short)213, (short)394, (short)408, (short)265, (short)523, (short)132, (short)217,
(short)131, (short)29, (short)646, (short)526, (short)269, (short)29, (short)553,
(short)31, (short)605, (short)287, (short)91, (short)31, (short)332, (short)333,
(short)33, (short)561, (short)304, (short)461, (short)33, (short)319, (short)564,
(short)646, (short)305, (short)131, (short)306, (short)36, (short)37, (short)38,
(short)307, (short)36, (short)37, (short)38, (short)647, (short)334, (short)335,
(short)131, (short)308, (short)456, (short)480, (short)408, (short)315, (short)132,
(short)132, (short)58, (short)492, (short)493, (short)494, (short)495, (short)268,
(short)649, (short)322, (short)211, (short)75, (short)324, (short)650, (short)323,
(short)370, (short)75, (short)514, (short)211, (short)515, (short)587, (short)408,
(short)476, (short)682, (short)314, (short)655, (short)589, (short)591, (short)408,
(short)481, (short)325, (short)471, (short)614, (short)408, (short)475, (short)726,
(short)314, (short)131, (short)131, (short)615, (short)408, (short)142, (short)351,
(short)29, (short)362, (short)596, (short)616, (short)408, (short)485, (short)31,
(short)110, (short)58, (short)363, (short)142, (short)250, (short)132, (short)33,
(short)735, (short)365, (short)627, (short)631, (short)632, (short)646, (short)375,
(short)503, (short)481, (short)372, (short)36, (short)37, (short)38, (short)481,
(short)281, (short)742, (short)379, (short)370, (short)672, (short)673, (short)555,
(short)690, (short)408, (short)282, (short)373, (short)36, (short)37, (short)38,
(short)511, (short)692, (short)408, (short)696, (short)408, (short)710, (short)296,
(short)131, (short)716, (short)408, (short)380, (short)142, (short)110, (short)58,
(short)613, (short)385, (short)108, (short)719, (short)408, (short)721, (short)408,
(short)203, (short)733, (short)408, (short)441, (short)384, (short)481, (short)527,
(short)528, (short)529, (short)386, (short)533, (short)481, (short)387, (short)539,
(short)389, (short)541, (short)544, (short)390, (short)657, (short)658, (short)490,
(short)491, (short)64, (short)496, (short)497, (short)391, (short)132, (short)498,
(short)499, (short)392, (short)402, (short)403, (short)405, (short)413, (short)687,
(short)201, (short)416, (short)420, (short)424, (short)417, (short)75, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)429,
(short)430, (short)432, (short)431, (short)433, (short)448, (short)320, (short)474,
(short)131, (short)479, (short)513, (short)519, (short)520, (short)581, (short)21,
(short)530, (short)521, (short)534, (short)29, (short)65, (short)536, (short)66,
(short)67, (short)370, (short)31, (short)26, (short)27, (short)540, (short)526,
(short)526, (short)545, (short)68, (short)549, (short)548, (short)69, (short)70,
(short)552, (short)556, (short)28, (short)557, (short)132, (short)558, (short)71,
(short)562, (short)567, (short)568, (short)72, (short)73, (short)569, (short)276,
(short)238, (short)573, (short)702, (short)574, (short)237, (short)575, (short)582,
(short)583, (short)624, (short)588, (short)590, (short)592, (short)593, (short)594,
(short)597, (short)132, (short)253, (short)648, (short)598, (short)600, (short)601,
(short)30, (short)238, (short)32, (short)606, (short)254, (short)237, (short)131,
(short)617, (short)609, (short)255, (short)610, (short)34, (short)35, (short)623,
(short)625, (short)633, (short)238, (short)626, (short)75, (short)629, (short)237,
(short)637, (short)595, (short)638, (short)640, (short)635, (short)619, (short)621,
(short)643, (short)644, (short)651, (short)131, (short)339, (short)340, (short)341,
(short)342, (short)343, (short)344, (short)345, (short)346, (short)347, (short)348,
(short)652, (short)659, (short)661, (short)675, (short)676, (short)298, (short)684,
(short)300, (short)300, (short)677, (short)678, (short)680, (short)683, (short)688,
(short)707, (short)349, (short)704, (short)712, (short)713, (short)703, (short)310,
(short)311, (short)728, (short)142, (short)738, (short)370, (short)64, (short)746,
(short)300, (short)300, (short)747, (short)300, (short)472, (short)559, (short)634,
(short)370, (short)374, (short)367, (short)654, (short)607, (short)118, (short)484,
(short)462, (short)487, (short)370, (short)445, (short)566, (short)489, (short)708,
(short)300, (short)0, (short)636, (short)486, (short)488, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)660, (short)0, (short)662, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)663,
(short)665, (short)0, (short)668, (short)0, (short)671, (short)0, (short)29,
(short)65, (short)0, (short)66, (short)67, (short)0, (short)31, (short)0,
(short)0, (short)0, (short)370, (short)435, (short)0, (short)68, (short)0,
(short)0, (short)69, (short)70, (short)0, (short)0, (short)681, (short)436,
(short)437, (short)0, (short)71, (short)0, (short)0, (short)0, (short)72,
(short)73, (short)0, (short)0, (short)691, (short)0, (short)0, (short)695,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)694, (short)0,
(short)705, (short)698, (short)700, (short)0, (short)0, (short)370, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)711, (short)0, (short)0,
(short)0, (short)714, (short)715, (short)0, (short)0, (short)717, (short)718,
(short)0, (short)720, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)723, (short)0, (short)0, (short)0, (short)0,
(short)727, (short)0, (short)729, (short)0, (short)0, (short)730, (short)0,
(short)731, (short)732, (short)0, (short)64, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)734, (short)739, (short)0,
(short)300, (short)0, (short)300, (short)300, (short)300, (short)300, (short)300,
(short)300, (short)300, (short)300, (short)300, (short)300, (short)300, (short)300,
(short)300, (short)300, (short)300, (short)300, (short)300, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)65, (short)0,
(short)66, (short)67, (short)0, (short)31, (short)21, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)68, (short)0, (short)300, (short)69,
(short)70, (short)26, (short)27, (short)0, (short)436, (short)437, (short)0,
(short)71, (short)0, (short)0, (short)0, (short)72, (short)73, (short)0, (short)28,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)30, (short)0, (short)32, (short)0, (short)576, (short)0, (short)0,
(short)300, (short)0, (short)577, (short)0, (short)34, (short)35, (short)300,
(short)0, (short)0, (short)0, (short)1, (short)2, (short)3, (short)106, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)672,
(short)673, (short)20, (short)218, (short)0, (short)219, (short)220, (short)221,
(short)222, (short)223, (short)224, (short)225, (short)226, (short)145, (short)21,
(short)146, (short)22, (short)147, (short)227, (short)300, (short)148, (short)23,
(short)228, (short)0, (short)0, (short)26, (short)27, (short)149, (short)150,
(short)229, (short)230, (short)151, (short)152, (short)153, (short)154, (short)155,
(short)156, (short)157, (short)28, (short)231, (short)158, (short)159, (short)0,
(short)160, (short)161, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)300, (short)0, (short)0, (short)0, (short)0,
(short)232, (short)162, (short)163, (short)164, (short)165, (short)30, (short)31,
(short)32, (short)0, (short)233, (short)107, (short)0, (short)0, (short)166,
(short)0, (short)0, (short)34, (short)235, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)168, (short)0, (short)0, (short)169, (short)170,
(short)171, (short)0, (short)0, (short)0, (short)0, (short)172, (short)173,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)300, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)300, (short)1, (short)2,
(short)3, (short)106, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)218, (short)0,
(short)219, (short)220, (short)221, (short)222, (short)223, (short)224, (short)225,
(short)226, (short)145, (short)21, (short)146, (short)22, (short)147, (short)227,
(short)0, (short)148, (short)23, (short)228, (short)0, (short)0, (short)26,
(short)27, (short)149, (short)150, (short)229, (short)230, (short)151, (short)152,
(short)153, (short)154, (short)155, (short)156, (short)157, (short)28, (short)231,
(short)158, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)232, (short)162, (short)163, (short)164,
(short)165, (short)30, (short)31, (short)32, (short)0, (short)233, (short)107,
(short)234, (short)0, (short)166, (short)0, (short)0, (short)34, (short)235,
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
(short)31, (short)32, (short)0, (short)233, (short)107, (short)0, (short)0,
(short)166, (short)0, (short)0, (short)34, (short)235, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)168, (short)0, (short)0, (short)169,
(short)170, (short)171, (short)0, (short)0, (short)0, (short)0, (short)172,
(short)173, (short)1, (short)2, (short)3, (short)106, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)145, (short)21, (short)146,
(short)22, (short)147, (short)0, (short)0, (short)148, (short)23, (short)0,
(short)0, (short)0, (short)26, (short)27, (short)149, (short)150, (short)0,
(short)0, (short)151, (short)152, (short)153, (short)154, (short)155, (short)156,
(short)157, (short)28, (short)0, (short)158, (short)159, (short)0, (short)160,
(short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162,
(short)163, (short)164, (short)165, (short)30, (short)31, (short)32, (short)0,
(short)531, (short)0, (short)0, (short)0, (short)166, (short)0, (short)0,
(short)34, (short)235, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)168, (short)0, (short)0, (short)169, (short)170, (short)171, (short)0,
(short)0, (short)0, (short)0, (short)172, (short)173, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)145, (short)21, (short)146, (short)0,
(short)147, (short)0, (short)0, (short)148, (short)0, (short)0, (short)0,
(short)0, (short)26, (short)27, (short)149, (short)150, (short)0, (short)0,
(short)151, (short)152, (short)153, (short)154, (short)155, (short)156, (short)157,
(short)28, (short)0, (short)158, (short)159, (short)0, (short)160, (short)161,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162, (short)163,
(short)164, (short)165, (short)30, (short)31, (short)32, (short)0, (short)0,
(short)312, (short)0, (short)0, (short)166, (short)0, (short)0, (short)34,
(short)235, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)168,
(short)0, (short)0, (short)169, (short)170, (short)171, (short)0, (short)0,
(short)0, (short)0, (short)172, (short)173, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)145, (short)21, (short)146, (short)0, (short)147,
(short)0, (short)0, (short)148, (short)0, (short)0, (short)0, (short)0, (short)26,
(short)27, (short)149, (short)150, (short)0, (short)0, (short)151, (short)152,
(short)153, (short)154, (short)155, (short)156, (short)157, (short)28, (short)0,
(short)158, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)162, (short)163, (short)164,
(short)165, (short)30, (short)31, (short)32, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)166, (short)0, (short)0, (short)34, (short)235,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)168, (short)0,
(short)0, (short)169, (short)170, (short)171, (short)0, (short)0, (short)0,
(short)0, (short)172, (short)173, (short)1, (short)2, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)21, (short)0,
(short)22, (short)0, (short)0, (short)0, (short)0, (short)23, (short)0, (short)24,
(short)25, (short)26, (short)27, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)0,
(short)0, (short)1, (short)2, (short)3, (short)106, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)20, (short)0, (short)0, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)30, (short)31, (short)32, (short)21, (short)0, (short)0,
(short)0, (short)0, (short)33, (short)0, (short)23, (short)34, (short)35,
(short)0, (short)26, (short)27, (short)0, (short)0, (short)0, (short)36, (short)37,
(short)38, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)0,
(short)0, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)0, (short)0, (short)21, (short)0,
(short)30, (short)31, (short)32, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)33, (short)26, (short)27, (short)34, (short)35, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)36, (short)37, (short)38, (short)28,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)0, (short)30, (short)31, (short)32, (short)21, (short)451,
(short)0, (short)0, (short)0, (short)291, (short)452, (short)0, (short)34,
(short)35, (short)0, (short)26, (short)27, (short)0, (short)282, (short)0,
(short)36, (short)37, (short)38, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)0, (short)0, (short)1, (short)2, (short)3, (short)106, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)30, (short)0, (short)32, (short)21, (short)451,
(short)22, (short)0, (short)0, (short)281, (short)452, (short)23, (short)34,
(short)35, (short)0, (short)26, (short)27, (short)0, (short)282, (short)0,
(short)36, (short)37, (short)38, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)0, (short)0, (short)0, (short)0, (short)0, (short)1, (short)2,
(short)3, (short)106, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)30, (short)0, (short)32, (short)0, (short)0, (short)107,
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
(short)0, (short)166, (short)0, (short)0, (short)0, (short)167, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)168, (short)0, (short)0,
(short)169, (short)170, (short)171, (short)0, (short)0, (short)0, (short)0,
(short)172, (short)173, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)21, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)26, (short)27, (short)0, (short)21,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)26, (short)27, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)30, (short)0, (short)32, (short)0, (short)0,
(short)130, (short)551, (short)0, (short)0, (short)0, (short)21, (short)34,
(short)35, (short)0, (short)30, (short)0, (short)32, (short)0, (short)0, (short)0,
(short)0, (short)26, (short)27, (short)466, (short)0, (short)34, (short)35,
(short)0, (short)145, (short)0, (short)146, (short)0, (short)147, (short)0,
(short)28, (short)148, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)149, (short)150, (short)0, (short)0, (short)151, (short)152, (short)153,
(short)154, (short)155, (short)156, (short)157, (short)0, (short)0, (short)158,
(short)159, (short)0, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)30, (short)0, (short)32, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)34,
(short)35, (short)0, (short)29, (short)162, (short)163, (short)164, (short)165,
(short)0, (short)31, (short)0, (short)0, (short)0, (short)301, (short)467,
(short)0, (short)166, (short)0, (short)0, (short)0, (short)167, (short)0,
(short)0, (short)0, (short)436, (short)437, (short)0, (short)168, (short)0,
(short)0, (short)169, (short)170, (short)171, (short)0, (short)0, (short)0,
(short)0, (short)172, (short)173, (short)145, (short)0, (short)146, (short)0,
(short)147, (short)0, (short)0, (short)148, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)149, (short)150, (short)0, (short)0,
(short)151, (short)152, (short)153, (short)154, (short)155, (short)156, (short)157,
(short)0, (short)0, (short)158, (short)159, (short)0, (short)160, (short)161,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162, (short)163,
(short)164, (short)165, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)301, (short)645, (short)0, (short)166, (short)0, (short)0, (short)0,
(short)167, (short)0, (short)0, (short)0, (short)436, (short)437, (short)0,
(short)168, (short)0, (short)0, (short)169, (short)170, (short)171, (short)0,
(short)0, (short)0, (short)0, (short)172, (short)173, (short)145, (short)0,
(short)146, (short)0, (short)147, (short)0, (short)0, (short)148, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)149, (short)150,
(short)0, (short)0, (short)151, (short)152, (short)153, (short)154, (short)155,
(short)156, (short)157, (short)0, (short)0, (short)158, (short)159, (short)0,
(short)160, (short)161, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)162, (short)163, (short)164, (short)165, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)301, (short)709, (short)0, (short)166,
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
(short)0, (short)0, (short)0, (short)166, (short)537, (short)0, (short)0,
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
(short)0, (short)146, (short)0, (short)147, (short)580, (short)168, (short)148,
(short)0, (short)169, (short)170, (short)171, (short)0, (short)0, (short)149,
(short)150, (short)172, (short)173, (short)151, (short)152, (short)153, (short)154,
(short)155, (short)156, (short)157, (short)0, (short)0, (short)158, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)162, (short)163, (short)164, (short)165, (short)0,
(short)31, (short)0, (short)0, (short)618, (short)0, (short)0, (short)0, (short)166,
(short)0, (short)0, (short)0, (short)167, (short)145, (short)0, (short)146,
(short)0, (short)147, (short)0, (short)168, (short)148, (short)0, (short)169,
(short)170, (short)171, (short)0, (short)0, (short)149, (short)150, (short)172,
(short)173, (short)151, (short)152, (short)153, (short)154, (short)155, (short)156,
(short)157, (short)0, (short)0, (short)158, (short)159, (short)0, (short)160,
(short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162,
(short)163, (short)164, (short)165, (short)0, (short)31, (short)0, (short)0,
(short)620, (short)0, (short)0, (short)0, (short)166, (short)0, (short)0,
(short)0, (short)167, (short)145, (short)0, (short)146, (short)0, (short)147,
(short)0, (short)168, (short)148, (short)0, (short)169, (short)170, (short)171,
(short)0, (short)0, (short)149, (short)150, (short)172, (short)173, (short)151,
(short)152, (short)153, (short)154, (short)155, (short)156, (short)157, (short)0,
(short)0, (short)158, (short)159, (short)0, (short)160, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)162, (short)163,
(short)164, (short)165, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)166, (short)664, (short)0, (short)0,
(short)167, (short)145, (short)0, (short)146, (short)0, (short)147, (short)0,
(short)168, (short)148, (short)0, (short)169, (short)170, (short)171, (short)0,
(short)0, (short)149, (short)150, (short)172, (short)173, (short)151, (short)152,
(short)153, (short)154, (short)155, (short)156, (short)157, (short)0, (short)0,
(short)158, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)162, (short)163, (short)164,
(short)165, (short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)667, (short)0, (short)0, (short)167, (short)145,
(short)0, (short)146, (short)0, (short)147, (short)0, (short)168, (short)148,
(short)0, (short)169, (short)170, (short)171, (short)0, (short)0, (short)149,
(short)150, (short)172, (short)173, (short)151, (short)152, (short)153, (short)154,
(short)155, (short)156, (short)157, (short)0, (short)0, (short)158, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)162, (short)163, (short)164, (short)165, (short)0,
(short)31, (short)0, (short)0, (short)670, (short)0, (short)0, (short)0, (short)166,
(short)0, (short)0, (short)0, (short)167, (short)145, (short)0, (short)146,
(short)0, (short)147, (short)0, (short)168, (short)148, (short)0, (short)169,
(short)170, (short)171, (short)0, (short)0, (short)149, (short)150, (short)172,
(short)173, (short)151, (short)152, (short)153, (short)154, (short)155, (short)156,
(short)157, (short)0, (short)0, (short)158, (short)159, (short)0, (short)160,
(short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162,
(short)163, (short)164, (short)165, (short)0, (short)31, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)166, (short)693, (short)0,
(short)0, (short)167, (short)145, (short)0, (short)146, (short)0, (short)147,
(short)0, (short)168, (short)148, (short)0, (short)169, (short)170, (short)171,
(short)0, (short)0, (short)149, (short)150, (short)172, (short)173, (short)151,
(short)152, (short)153, (short)154, (short)155, (short)156, (short)157, (short)0,
(short)0, (short)158, (short)159, (short)0, (short)160, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)29, (short)162, (short)163,
(short)164, (short)165, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)166, (short)697, (short)0, (short)0,
(short)167, (short)145, (short)0, (short)146, (short)0, (short)147, (short)0,
(short)168, (short)148, (short)0, (short)169, (short)170, (short)171, (short)0,
(short)0, (short)149, (short)150, (short)172, (short)173, (short)151, (short)152,
(short)153, (short)154, (short)155, (short)156, (short)157, (short)0, (short)0,
(short)158, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)29, (short)162, (short)163, (short)164,
(short)165, (short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)699, (short)0, (short)0, (short)167, (short)145,
(short)0, (short)146, (short)0, (short)147, (short)0, (short)168, (short)148,
(short)0, (short)169, (short)170, (short)171, (short)0, (short)0, (short)149,
(short)150, (short)172, (short)173, (short)151, (short)152, (short)153, (short)154,
(short)155, (short)156, (short)157, (short)0, (short)0, (short)158, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)29, (short)162, (short)163, (short)164, (short)165, (short)0,
(short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)166,
(short)722, (short)0, (short)0, (short)167, (short)145, (short)0, (short)146,
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
(short)165, (short)0, (short)31, (short)0, (short)0, (short)0, (short)301,
(short)0, (short)0, (short)166, (short)0, (short)0, (short)0, (short)167,
(short)145, (short)0, (short)146, (short)0, (short)0, (short)0, (short)168,
(short)0, (short)0, (short)169, (short)170, (short)171, (short)0, (short)0,
(short)149, (short)150, (short)172, (short)173, (short)151, (short)152, (short)153,
(short)154, (short)155, (short)156, (short)157, (short)0, (short)0, (short)0,
(short)159, (short)0, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)162, (short)163, (short)164, (short)165,
(short)0, (short)31, (short)0, (short)0, (short)0, (short)595, (short)0, (short)0,
(short)166, (short)0, (short)0, (short)0, (short)167, (short)145, (short)0,
(short)146, (short)0, (short)0, (short)0, (short)168, (short)0, (short)0,
(short)169, (short)170, (short)171, (short)0, (short)0, (short)149, (short)150,
(short)172, (short)173, (short)151, (short)152, (short)153, (short)154, (short)155,
(short)156, (short)157, (short)0, (short)0, (short)0, (short)159, (short)0,
(short)160, (short)161, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)29, (short)162, (short)163, (short)164, (short)165, (short)0, (short)31,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)297, (short)0,
(short)0, (short)0, (short)167, (short)145, (short)0, (short)146, (short)0,
(short)0, (short)0, (short)168, (short)0, (short)0, (short)169, (short)170,
(short)171, (short)0, (short)0, (short)149, (short)150, (short)172, (short)173,
(short)151, (short)152, (short)153, (short)154, (short)155, (short)156, (short)157,
(short)0, (short)0, (short)0, (short)159, (short)0, (short)160, (short)161,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)29, (short)162, (short)163,
(short)164, (short)165, (short)0, (short)31, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)309, (short)0, (short)0, (short)0, (short)167,
(short)145, (short)0, (short)146, (short)0, (short)0, (short)0, (short)168,
(short)0, (short)0, (short)169, (short)170, (short)171, (short)0, (short)0,
(short)149, (short)150, (short)172, (short)173, (short)151, (short)152, (short)153,
(short)154, (short)155, (short)156, (short)157, (short)0, (short)0, (short)0,
(short)159, (short)0, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)29, (short)162, (short)163, (short)164, (short)165,
(short)0, (short)31, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)166, (short)0, (short)0, (short)0, (short)167, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)168, (short)0, (short)0, (short)169,
(short)170, (short)171, (short)0, (short)0, (short)0, (short)0, (short)172,
(short)173}; static short Cyc_yycheck[ 4319u]={ (short)54, (short)22, (short)83,
(short)148, (short)54, (short)113, (short)360, (short)61, (short)148, (short)54,
(short)56, (short)147, (short)55, (short)54, (short)244, (short)201, (short)0,
(short)274, (short)112, (short)78, (short)166, (short)90, (short)296, (short)17,
(short)33, (short)410, (short)133, (short)17, (short)18, (short)19, (short)213,
(short)73, (short)87, (short)414, (short)661, (short)142, (short)45, (short)21,
(short)22, (short)74, (short)24, (short)95, (short)96, (short)97, (short)98,
(short)95, (short)68, (short)68, (short)103, (short)33, (short)95, (short)98,
(short)73, (short)107, (short)101, (short)109, (short)40, (short)107, (short)78,
(short)109, (short)80, (short)45, (short)447, (short)83, (short)109, (short)431,
(short)108, (short)51, (short)96, (short)53, (short)451, (short)437, (short)56,
(short)116, (short)101, (short)86, (short)46, (short)47, (short)48, (short)114,
(short)100, (short)103, (short)312, (short)110, (short)68, (short)107, (short)75,
(short)98, (short)93, (short)73, (short)88, (short)351, (short)112, (short)113,
(short)88, (short)78, (short)356, (short)80, (short)725, (short)104, (short)83,
(short)110, (short)113, (short)87, (short)88, (short)89, (short)90, (short)99,
(short)735, (short)100, (short)99, (short)103, (short)101, (short)740, (short)68,
(short)742, (short)105, (short)100, (short)109, (short)110, (short)104, (short)88,
(short)112, (short)113, (short)114, (short)68, (short)110, (short)94, (short)106,
(short)112, (short)113, (short)40, (short)88, (short)142, (short)101, (short)119,
(short)133, (short)397, (short)122, (short)87, (short)124, (short)415, (short)106,
(short)110, (short)283, (short)142, (short)166, (short)117, (short)202, (short)103,
(short)106, (short)297, (short)202, (short)107, (short)293, (short)521, (short)263,
(short)202, (short)142, (short)83, (short)103, (short)131, (short)132, (short)309,
(short)107, (short)101, (short)69, (short)70, (short)549, (short)64, (short)65,
(short)66, (short)168, (short)169, (short)281, (short)171, (short)100, (short)112,
(short)87, (short)88, (short)89, (short)90, (short)93, (short)166, (short)291,
(short)120, (short)121, (short)98, (short)112, (short)209, (short)244, (short)200,
(short)88, (short)189, (short)244, (short)576, (short)97, (short)93, (short)94,
(short)98, (short)101, (short)107, (short)211, (short)106, (short)107, (short)103,
(short)101, (short)96, (short)104, (short)115, (short)119, (short)372, (short)101,
(short)111, (short)109, (short)110, (short)200, (short)106, (short)96, (short)380,
(short)99, (short)110, (short)98, (short)101, (short)103, (short)214, (short)209,
(short)211, (short)106, (short)238, (short)289, (short)215, (short)110, (short)88,
(short)97, (short)98, (short)84, (short)85, (short)88, (short)375, (short)95,
(short)595, (short)285, (short)263, (short)88, (short)95, (short)106, (short)293,
(short)88, (short)109, (short)94, (short)305, (short)637, (short)274, (short)238,
(short)95, (short)98, (short)101, (short)312, (short)279, (short)280, (short)281,
(short)312, (short)446, (short)98, (short)103, (short)114, (short)101, (short)88,
(short)289, (short)108, (short)291, (short)106, (short)88, (short)94, (short)95,
(short)296, (short)297, (short)98, (short)94, (short)93, (short)97, (short)291,
(short)101, (short)470, (short)209, (short)99, (short)103, (short)97, (short)309,
(short)274, (short)275, (short)98, (short)128, (short)103, (short)431, (short)279,
(short)280, (short)281, (short)97, (short)101, (short)437, (short)97, (short)98,
(short)97, (short)103, (short)289, (short)291, (short)291, (short)293, (short)103,
(short)568, (short)88, (short)296, (short)297, (short)101, (short)423, (short)88,
(short)94, (short)100, (short)106, (short)305, (short)403, (short)93, (short)97,
(short)101, (short)309, (short)102, (short)585, (short)99, (short)103, (short)166,
(short)71, (short)72, (short)110, (short)389, (short)112, (short)113, (short)114,
(short)389, (short)336, (short)337, (short)338, (short)97, (short)103, (short)102,
(short)103, (short)604, (short)107, (short)103, (short)404, (short)459, (short)372,
(short)201, (short)404, (short)279, (short)280, (short)281, (short)89, (short)404,
(short)380, (short)103, (short)545, (short)404, (short)458, (short)97, (short)108,
(short)291, (short)97, (short)97, (short)103, (short)103, (short)296, (short)99,
(short)103, (short)103, (short)103, (short)103, (short)221, (short)97, (short)364,
(short)108, (short)366, (short)403, (short)227, (short)521, (short)229, (short)230,
(short)99, (short)372, (short)87, (short)226, (short)103, (short)101, (short)383,
(short)415, (short)90, (short)380, (short)88, (short)585, (short)384, (short)102,
(short)88, (short)423, (short)94, (short)513, (short)115, (short)97, (short)94,
(short)67, (short)68, (short)101, (short)433, (short)101, (short)457, (short)101,
(short)99, (short)438, (short)604, (short)101, (short)403, (short)101, (short)112,
(short)113, (short)114, (short)101, (short)112, (short)113, (short)114, (short)686,
(short)115, (short)116, (short)415, (short)101, (short)282, (short)102, (short)103,
(short)101, (short)458, (short)459, (short)424, (short)328, (short)329, (short)330,
(short)331, (short)429, (short)591, (short)73, (short)372, (short)433, (short)119,
(short)591, (short)118, (short)301, (short)438, (short)364, (short)380, (short)366,
(short)102, (short)103, (short)308, (short)647, (short)297, (short)599, (short)473,
(short)102, (short)103, (short)315, (short)117, (short)304, (short)102, (short)103,
(short)307, (short)703, (short)309, (short)458, (short)459, (short)102, (short)103,
(short)403, (short)101, (short)88, (short)99, (short)479, (short)102, (short)103,
(short)321, (short)94, (short)542, (short)473, (short)99, (short)415, (short)542,
(short)513, (short)101, (short)725, (short)99, (short)542, (short)89, (short)90,
(short)686, (short)108, (short)350, (short)351, (short)98, (short)112, (short)113,
(short)114, (short)356, (short)101, (short)740, (short)520, (short)360, (short)22,
(short)23, (short)424, (short)102, (short)103, (short)110, (short)99, (short)112,
(short)113, (short)114, (short)358, (short)102, (short)103, (short)102, (short)103,
(short)102, (short)103, (short)513, (short)102, (short)103, (short)98, (short)458,
(short)589, (short)520, (short)526, (short)101, (short)589, (short)102, (short)103,
(short)102, (short)103, (short)589, (short)102, (short)103, (short)568, (short)98,
(short)397, (short)385, (short)386, (short)387, (short)101, (short)389, (short)403,
(short)101, (short)400, (short)101, (short)402, (short)408, (short)88, (short)611,
(short)612, (short)326, (short)327, (short)37, (short)332, (short)333, (short)97,
(short)590, (short)334, (short)335, (short)97, (short)108, (short)101, (short)99,
(short)102, (short)659, (short)100, (short)102, (short)111, (short)99, (short)103,
(short)568, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, (short)103, (short)99, (short)102, (short)100, (short)103, (short)99,
(short)74, (short)35, (short)590, (short)102, (short)106, (short)97, (short)103,
(short)460, (short)36, (short)28, (short)108, (short)97, (short)88, (short)89,
(short)41, (short)91, (short)92, (short)470, (short)94, (short)47, (short)48,
(short)93, (short)611, (short)612, (short)87, (short)101, (short)115, (short)102,
(short)104, (short)105, (short)88, (short)101, (short)60, (short)102, (short)659,
(short)103, (short)112, (short)88, (short)99, (short)103, (short)116, (short)117,
(short)102, (short)93, (short)703, (short)102, (short)672, (short)102, (short)703,
(short)111, (short)102, (short)88, (short)97, (short)102, (short)101, (short)103,
(short)102, (short)99, (short)102, (short)684, (short)88, (short)590, (short)103,
(short)102, (short)102, (short)93, (short)725, (short)95, (short)99, (short)97,
(short)725, (short)659, (short)101, (short)99, (short)102, (short)99, (short)104,
(short)105, (short)98, (short)102, (short)102, (short)740, (short)107, (short)672,
(short)99, (short)740, (short)103, (short)98, (short)102, (short)102, (short)111,
(short)531, (short)532, (short)111, (short)106, (short)35, (short)684, (short)75,
(short)76, (short)77, (short)78, (short)79, (short)80, (short)81, (short)82,
(short)83, (short)84, (short)102, (short)101, (short)98, (short)97, (short)88,
(short)145, (short)101, (short)147, (short)148, (short)102, (short)102, (short)102,
(short)102, (short)26, (short)102, (short)100, (short)99, (short)99, (short)97,
(short)108, (short)160, (short)161, (short)102, (short)659, (short)102, (short)585,
(short)37, (short)0, (short)168, (short)169, (short)0, (short)171, (short)305,
(short)429, (short)558, (short)595, (short)209, (short)200, (short)598, (short)520,
(short)57, (short)320, (short)287, (short)323, (short)604, (short)275, (short)439,
(short)325, (short)684, (short)189, - (short)1, (short)568, (short)322, (short)324,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)614, -
(short)1, (short)616, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)617, (short)618, - (short)1, (short)620,
- (short)1, (short)622, - (short)1, (short)88, (short)89, - (short)1, (short)91,
(short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1, (short)647,
(short)99, - (short)1, (short)101, - (short)1, - (short)1, (short)104, (short)105,
- (short)1, - (short)1, (short)644, (short)109, (short)110, - (short)1, (short)112,
- (short)1, - (short)1, - (short)1, (short)116, (short)117, - (short)1, -
(short)1, (short)664, - (short)1, - (short)1, (short)667, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)666, - (short)1, (short)676,
(short)669, (short)670, - (short)1, - (short)1, (short)686, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)688, - (short)1,
- (short)1, - (short)1, (short)692, (short)693, - (short)1, - (short)1, (short)696,
(short)697, - (short)1, (short)699, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)701,
- (short)1, - (short)1, - (short)1, - (short)1, (short)706, - (short)1, (short)716,
- (short)1, - (short)1, (short)719, - (short)1, (short)721, (short)722, -
(short)1, (short)37, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)724, (short)733, - (short)1, (short)320, -
(short)1, (short)322, (short)323, (short)324, (short)325, (short)326, (short)327,
(short)328, (short)329, (short)330, (short)331, (short)332, (short)333, (short)334,
(short)335, (short)336, (short)337, (short)338, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, - (short)1,
(short)91, (short)92, - (short)1, (short)94, (short)36, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, - (short)1, (short)375, (short)104,
(short)105, (short)47, (short)48, - (short)1, (short)109, (short)110, - (short)1,
(short)112, - (short)1, - (short)1, - (short)1, (short)116, (short)117, -
(short)1, (short)60, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)93, - (short)1, (short)95, - (short)1, (short)97, - (short)1,
- (short)1, (short)431, - (short)1, (short)102, - (short)1, (short)104, (short)105,
(short)437, - (short)1, - (short)1, - (short)1, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, (short)22, (short)23, (short)24, (short)25, - (short)1, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)36, (short)37, (short)38, (short)39, (short)40, (short)479,
(short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, (short)60, (short)61, (short)62,
(short)63, - (short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)521, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89,
(short)90, (short)91, (short)92, (short)93, (short)94, (short)95, - (short)1,
(short)97, (short)98, - (short)1, - (short)1, (short)101, - (short)1, - (short)1,
(short)104, (short)105, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116,
(short)117, - (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)591, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)599, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, (short)25, -
(short)1, (short)27, (short)28, (short)29, (short)30, (short)31, (short)32,
(short)33, (short)34, (short)35, (short)36, (short)37, (short)38, (short)39,
(short)40, - (short)1, (short)42, (short)43, (short)44, - (short)1, - (short)1,
(short)47, (short)48, (short)49, (short)50, (short)51, (short)52, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, (short)60,
(short)61, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, (short)93, (short)94,
(short)95, - (short)1, (short)97, (short)98, (short)99, - (short)1, (short)101,
- (short)1, - (short)1, (short)104, (short)105, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)112, - (short)1, - (short)1,
(short)115, (short)116, (short)117, - (short)1, - (short)1, - (short)1, -
(short)1, (short)122, (short)123, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
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
(short)97, (short)98, - (short)1, - (short)1, (short)101, - (short)1, - (short)1,
(short)104, (short)105, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116,
(short)117, - (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)35, (short)36, (short)37, (short)38,
(short)39, - (short)1, - (short)1, (short)42, (short)43, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, (short)49, (short)50, - (short)1, - (short)1,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, (short)59,
(short)60, - (short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, (short)93,
(short)94, (short)95, - (short)1, (short)97, - (short)1, - (short)1, - (short)1,
(short)101, - (short)1, - (short)1, (short)104, (short)105, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)112, - (short)1,
- (short)1, (short)115, (short)116, (short)117, - (short)1, - (short)1, -
(short)1, - (short)1, (short)122, (short)123, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)35, (short)36, (short)37, -
(short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1,
- (short)1, - (short)1, (short)47, (short)48, (short)49, (short)50, - (short)1,
- (short)1, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, (short)60, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92,
(short)93, (short)94, (short)95, - (short)1, - (short)1, (short)98, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, (short)104, (short)105, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)112,
- (short)1, - (short)1, (short)115, (short)116, (short)117, - (short)1, -
(short)1, - (short)1, - (short)1, (short)122, (short)123, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)35, (short)36, (short)37,
- (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1,
- (short)1, - (short)1, (short)47, (short)48, (short)49, (short)50, - (short)1,
- (short)1, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, (short)60, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92,
(short)93, (short)94, (short)95, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, (short)104, (short)105, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)112,
- (short)1, - (short)1, (short)115, (short)116, (short)117, - (short)1, -
(short)1, - (short)1, - (short)1, (short)122, (short)123, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)36, - (short)1, (short)38, - (short)1,
- (short)1, - (short)1, - (short)1, (short)43, - (short)1, (short)45, (short)46,
(short)47, (short)48, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)60,
- (short)1, - (short)1, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1, (short)88,
- (short)1, - (short)1, - (short)1, - (short)1, (short)93, (short)94, (short)95,
(short)36, - (short)1, - (short)1, - (short)1, - (short)1, (short)101, - (short)1,
(short)43, (short)104, (short)105, - (short)1, (short)47, (short)48, - (short)1,
- (short)1, - (short)1, (short)112, (short)113, (short)114, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)60, - (short)1, - (short)1,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, - (short)1, - (short)1,
(short)36, - (short)1, (short)93, (short)94, (short)95, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, (short)47, (short)48, (short)104,
(short)105, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)112, (short)113, (short)114, (short)60, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, (short)93, (short)94, (short)95, (short)36, (short)97, -
(short)1, - (short)1, - (short)1, (short)101, (short)102, - (short)1, (short)104,
(short)105, - (short)1, (short)47, (short)48, - (short)1, (short)110, - (short)1,
(short)112, (short)113, (short)114, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)60, - (short)1, - (short)1, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)93,
- (short)1, (short)95, (short)36, (short)97, (short)38, - (short)1, - (short)1,
(short)101, (short)102, (short)43, (short)104, (short)105, - (short)1, (short)47,
(short)48, - (short)1, (short)110, - (short)1, (short)112, (short)113, (short)114,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)60, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)93, - (short)1, (short)95, - (short)1, - (short)1,
(short)98, (short)36, (short)100, (short)38, - (short)1, - (short)1, (short)104,
(short)105, (short)43, - (short)1, - (short)1, - (short)1, (short)47, (short)48,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)60, - (short)1,
- (short)1, (short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1,
(short)24, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)93, - (short)1, (short)95, (short)36, - (short)1,
(short)98, - (short)1, - (short)1, - (short)1, - (short)1, (short)43, (short)104,
(short)105, - (short)1, (short)47, (short)48, (short)25, - (short)1, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)60, (short)37, - (short)1, (short)39, (short)40, - (short)1,
(short)42, - (short)1, (short)44, - (short)1, - (short)1, - (short)1, - (short)1,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, (short)61, (short)62,
(short)63, - (short)1, (short)65, (short)66, - (short)1, - (short)1, (short)93,
- (short)1, (short)95, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)104, (short)105, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89,
(short)90, (short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1,
(short)97, (short)98, - (short)1, - (short)1, (short)101, - (short)1, - (short)1,
- (short)1, (short)105, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116,
(short)117, - (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21,
(short)36, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)47, (short)48, - (short)1,
(short)36, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)60, (short)47, (short)48, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)60, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, -
(short)1, - (short)1, (short)93, - (short)1, (short)95, - (short)1, - (short)1,
(short)98, (short)85, - (short)1, - (short)1, - (short)1, (short)36, (short)104,
(short)105, - (short)1, (short)93, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, - (short)1, (short)47, (short)48, (short)30, - (short)1, (short)104,
(short)105, - (short)1, (short)35, - (short)1, (short)37, - (short)1, (short)39,
- (short)1, (short)60, (short)42, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)49, (short)50, - (short)1, - (short)1, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)93,
- (short)1, (short)95, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)104, (short)105, - (short)1, (short)88,
(short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, - (short)1, -
(short)1, - (short)1, (short)98, (short)99, - (short)1, (short)101, - (short)1,
- (short)1, - (short)1, (short)105, - (short)1, - (short)1, - (short)1, (short)109,
(short)110, - (short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116,
(short)117, - (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1,
(short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)49, (short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62,
(short)63, - (short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89,
(short)90, (short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1,
- (short)1, (short)98, (short)99, - (short)1, (short)101, - (short)1, - (short)1,
- (short)1, (short)105, - (short)1, - (short)1, - (short)1, (short)109, (short)110,
- (short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49,
(short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
(short)98, (short)99, - (short)1, (short)101, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, - (short)1, (short)109, (short)110, -
(short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49,
(short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
(short)98, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, - (short)1, (short)109, (short)110, -
(short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)49,
(short)50, - (short)1, - (short)1, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, (short)59, - (short)1, - (short)1, (short)62, (short)63, -
(short)1, (short)65, (short)66, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89, (short)90,
(short)91, (short)92, - (short)1, (short)94, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, -
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39,
(short)111, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, (short)62, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, - (short)1, (short)98, - (short)1, - (short)1, (short)101,
- (short)1, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, (short)112, (short)42, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, (short)62, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)35,
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
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39,
(short)111, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
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
(short)1, (short)105, (short)35, - (short)1, (short)37, - (short)1, (short)39,
(short)111, (short)112, (short)42, - (short)1, (short)115, (short)116, (short)117,
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
(short)1, (short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)35,
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
(short)1, (short)94, - (short)1, - (short)1, (short)97, - (short)1, - (short)1,
- (short)1, (short)101, - (short)1, - (short)1, - (short)1, (short)105, (short)35,
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
- (short)1, - (short)1, - (short)1, (short)105, (short)35, - (short)1, (short)37,
- (short)1, - (short)1, - (short)1, (short)112, - (short)1, - (short)1, (short)115,
(short)116, (short)117, - (short)1, - (short)1, (short)49, (short)50, (short)122,
(short)123, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58,
(short)59, - (short)1, - (short)1, - (short)1, (short)63, - (short)1, (short)65,
(short)66, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)91, (short)92, -
(short)1, (short)94, - (short)1, - (short)1, - (short)1, (short)98, - (short)1,
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
(short)98, - (short)1, - (short)1, (short)101, - (short)1, - (short)1, - (short)1,
(short)105, (short)35, - (short)1, (short)37, - (short)1, - (short)1, - (short)1,
(short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117, -
(short)1, - (short)1, (short)49, (short)50, (short)122, (short)123, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, - (short)1, -
(short)1, - (short)1, (short)63, - (short)1, (short)65, (short)66, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)91, (short)92, - (short)1, (short)94, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)101,
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
(short)1, (short)105, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)112, - (short)1, - (short)1, (short)115, (short)116, (short)117,
- (short)1, - (short)1, - (short)1, - (short)1, (short)122, (short)123}; char
Cyc_Yyimpossible[ 17u]="\000\000\000\000Yyimpossible"; char Cyc_Yystack_overflow[
21u]="\000\000\000\000Yystack_overflow"; extern void Cyc_yyerror( struct
_tagged_string); extern int Cyc_yylex(); char Cyc_YYINITIALSVAL[ 18u]="\000\000\000\000YYINITIALSVAL";
static int Cyc_yychar=( int)'\000'; void* Cyc_yylval=( void*) Cyc_YYINITIALSVAL;
static int Cyc_yynerrs= 0; struct _tagged_ptr1{ short* curr; short* base; short*
last_plus_one; } ; struct _tagged_ptr2{ void** curr; void** base; void**
last_plus_one; } ; struct _tagged_ptr3{ struct Cyc_Yyltype* curr; struct Cyc_Yyltype*
base; struct Cyc_Yyltype* last_plus_one; } ; struct _tuple18{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; int Cyc_yyparse(){ int yystate; int yyn= 0;
int yyerrstatus; int yychar1= 0; int yyssp_offset; struct _tagged_ptr1 yyss=(
struct _tagged_ptr1)({ short* _temp7469=( short*)({ unsigned int _temp7465= 200u;
short* _temp7466=( short*) GC_malloc_atomic( sizeof( short) * _temp7465);{
unsigned int _temp7467= _temp7465; unsigned int i; for( i= 0; i < _temp7467; i
++){ _temp7466[ i]= (short)0;}}; _temp7466;}); struct _tagged_ptr1 _temp7470;
_temp7470.curr= _temp7469; _temp7470.base= _temp7469; _temp7470.last_plus_one=
_temp7469 + 200; _temp7470;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=(
struct _tagged_ptr2)({ void** _temp7463=( void**)({ unsigned int _temp7459= 200u;
void** _temp7460=( void**) GC_malloc( sizeof( void*) * _temp7459);{ unsigned int
_temp7461= _temp7459; unsigned int i; for( i= 0; i < _temp7461; i ++){ _temp7460[
i]= Cyc_yylval;}}; _temp7460;}); struct _tagged_ptr2 _temp7464; _temp7464.curr=
_temp7463; _temp7464.base= _temp7463; _temp7464.last_plus_one= _temp7463 + 200;
_temp7464;}); int yylsp_offset; struct _tagged_ptr3 yyls=( struct _tagged_ptr3)({
struct Cyc_Yyltype* _temp7457=( struct Cyc_Yyltype*)({ unsigned int _temp7453=
200u; struct Cyc_Yyltype* _temp7454=( struct Cyc_Yyltype*) GC_malloc( sizeof(
struct Cyc_Yyltype) * _temp7453);{ unsigned int _temp7455= _temp7453;
unsigned int i; for( i= 0; i < _temp7455; i ++){ _temp7454[ i]= Cyc_yynewloc();}};
_temp7454;}); struct _tagged_ptr3 _temp7458; _temp7458.curr= _temp7457;
_temp7458.base= _temp7457; _temp7458.last_plus_one= _temp7457 + 200; _temp7458;});
int yystacksize= 200; void* yyval= Cyc_yylval; int yylen; yystate= 0;
yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset=
0; yylsp_offset= 0; yynewstate:({ struct _tagged_ptr1 _temp2187= yyss; short*
_temp2189= _temp2187.curr +( ++ yyssp_offset); if( _temp2187.base == 0? 1:(
_temp2189 < _temp2187.base? 1: _temp2189 >= _temp2187.last_plus_one)){ _throw(
Null_Exception);}* _temp2189=( short) yystate;}); if( yyssp_offset >=
yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror(( struct _tagged_string)({
char* _temp2190=( char*)"parser stack overflow"; struct _tagged_string _temp2191;
_temp2191.curr= _temp2190; _temp2191.base= _temp2190; _temp2191.last_plus_one=
_temp2190 + 22; _temp2191;}));( void) _throw(( void*) Cyc_Yystack_overflow);}
yystacksize *= 2; if( yystacksize > 10000){ yystacksize= 10000;}{ struct
_tagged_ptr1 yyss1=({ unsigned int _temp2220=( unsigned int) yystacksize; short*
_temp2221=( short*) GC_malloc_atomic( sizeof( short) * _temp2220); struct
_tagged_ptr1 _temp2224={ _temp2221, _temp2221, _temp2221 + _temp2220};{
unsigned int _temp2222= _temp2220; unsigned int i; for( i= 0; i < _temp2222; i
++){ _temp2221[ i]= (short)0;}}; _temp2224;}); struct _tagged_ptr2 yyvs1=({
unsigned int _temp2215=( unsigned int) yystacksize; void** _temp2216=( void**)
GC_malloc( sizeof( void*) * _temp2215); struct _tagged_ptr2 _temp2219={
_temp2216, _temp2216, _temp2216 + _temp2215};{ unsigned int _temp2217= _temp2215;
unsigned int i; for( i= 0; i < _temp2217; i ++){ _temp2216[ i]= Cyc_yylval;}};
_temp2219;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2210=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2211=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2210); struct _tagged_ptr3 _temp2214={
_temp2211, _temp2211, _temp2211 + _temp2210};{ unsigned int _temp2212= _temp2210;
unsigned int i; for( i= 0; i < _temp2212; i ++){ _temp2211[ i]= Cyc_yynewloc();}};
_temp2214;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1
_temp2192= yyss1; short* _temp2194= _temp2192.curr + i; if( _temp2192.base == 0?
1:( _temp2194 < _temp2192.base? 1: _temp2194 >= _temp2192.last_plus_one)){
_throw( Null_Exception);}* _temp2194=({ struct _tagged_ptr1 _temp2195= yyss;
short* _temp2197= _temp2195.curr + i; if( _temp2195.base == 0? 1:( _temp2197 <
_temp2195.base? 1: _temp2197 >= _temp2195.last_plus_one)){ _throw(
Null_Exception);}* _temp2197;});});({ struct _tagged_ptr2 _temp2198= yyvs1; void**
_temp2200= _temp2198.curr + i; if( _temp2198.base == 0? 1:( _temp2200 <
_temp2198.base? 1: _temp2200 >= _temp2198.last_plus_one)){ _throw(
Null_Exception);}* _temp2200=({ struct _tagged_ptr2 _temp2201= yyvs; void**
_temp2203= _temp2201.curr + i; if( _temp2201.base == 0? 1:( _temp2203 <
_temp2201.base? 1: _temp2203 >= _temp2201.last_plus_one)){ _throw(
Null_Exception);}* _temp2203;});});({ struct _tagged_ptr3 _temp2204= yyls1;
struct Cyc_Yyltype* _temp2206= _temp2204.curr + i; if( _temp2204.base == 0? 1:(
_temp2206 < _temp2204.base? 1: _temp2206 >= _temp2204.last_plus_one)){ _throw(
Null_Exception);}* _temp2206=({ struct _tagged_ptr3 _temp2207= yyls; struct Cyc_Yyltype*
_temp2209= _temp2207.curr + i; if( _temp2207.base == 0? 1:( _temp2209 <
_temp2207.base? 1: _temp2209 >= _temp2207.last_plus_one)){ _throw(
Null_Exception);}* _temp2209;});});}} yyss= yyss1; yyvs= yyvs1; yyls= yyls1;}}
goto yybackup; yybackup: yyn=( int)({ short* _temp2225=( short*) Cyc_yypact;
unsigned int _temp2226= yystate; if( _temp2226 >= 748u){ _throw( Null_Exception);}
_temp2225[ _temp2226];}); if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar
== - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar=
0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 351: 0)?( int)({ short*
_temp2227=( short*) Cyc_yytranslate; unsigned int _temp2228= Cyc_yychar; if(
_temp2228 >= 352u){ _throw( Null_Exception);} _temp2227[ _temp2228];}): 231;}
yyn += yychar1; if(( yyn < 0? 1: yyn > 4318)? 1:({ short* _temp2229=( short*)
Cyc_yycheck; unsigned int _temp2230= yyn; if( _temp2230 >= 4319u){ _throw(
Null_Exception);} _temp2229[ _temp2230];}) != yychar1){ goto yydefault;} yyn=(
int)({ short* _temp2231=( short*) Cyc_yytable; unsigned int _temp2232= yyn; if(
_temp2232 >= 4319u){ _throw( Null_Exception);} _temp2231[ _temp2232];}); if( yyn
< 0){ if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if(
yyn == 0){ goto yyerrlab;}} if( yyn == 747){ return 0;} if( Cyc_yychar != 0){
Cyc_yychar= - 2;}({ struct _tagged_ptr2 _temp2233= yyvs; void** _temp2235=
_temp2233.curr +( ++ yyvsp_offset); if( _temp2233.base == 0? 1:( _temp2235 <
_temp2233.base? 1: _temp2235 >= _temp2233.last_plus_one)){ _throw(
Null_Exception);}* _temp2235= Cyc_yylval;});({ struct _tagged_ptr3 _temp2236=
yyls; struct Cyc_Yyltype* _temp2238= _temp2236.curr +( ++ yylsp_offset); if(
_temp2236.base == 0? 1:( _temp2238 < _temp2236.base? 1: _temp2238 >= _temp2236.last_plus_one)){
_throw( Null_Exception);}* _temp2238= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp2239=( short*) Cyc_yydefact; unsigned int _temp2240= yystate; if( _temp2240
>= 748u){ _throw( Null_Exception);} _temp2239[ _temp2240];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp2241=( short*) Cyc_yyr2;
unsigned int _temp2242= yyn; if( _temp2242 >= 391u){ _throw( Null_Exception);}
_temp2241[ _temp2242];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp2243=
yyvs; void** _temp2245= _temp2243.curr +(( yyvsp_offset + 1) - yylen); if(
_temp2243.base == 0? 1:( _temp2245 < _temp2243.base? 1: _temp2245 >= _temp2243.last_plus_one)){
_throw( Null_Exception);}* _temp2245;});} switch( yyn){ case 1: _LL2246: yyval=({
struct _tagged_ptr2 _temp2248= yyvs; void** _temp2250= _temp2248.curr +
yyvsp_offset; if( _temp2248.base == 0? 1:( _temp2250 < _temp2248.base? 1:
_temp2250 >= _temp2248.last_plus_one)){ _throw( Null_Exception);}* _temp2250;});
Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2251=
yyvs; void** _temp2253= _temp2251.curr + yyvsp_offset; if( _temp2251.base == 0?
1:( _temp2253 < _temp2251.base? 1: _temp2253 >= _temp2251.last_plus_one)){
_throw( Null_Exception);}* _temp2253;})); break; case 2: _LL2247: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp2255=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2255[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2256; _temp2256.tag= Cyc_DeclList_tok; _temp2256.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2257= yyvs; void** _temp2259= _temp2257.curr +(
yyvsp_offset - 1); if( _temp2257.base == 0? 1:( _temp2259 < _temp2257.base? 1:
_temp2259 >= _temp2257.last_plus_one)){ _throw( Null_Exception);}* _temp2259;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2260= yyvs; void** _temp2262=
_temp2260.curr + yyvsp_offset; if( _temp2260.base == 0? 1:( _temp2262 <
_temp2260.base? 1: _temp2262 >= _temp2260.last_plus_one)){ _throw(
Null_Exception);}* _temp2262;}))); _temp2256;}); _temp2255;}); break; case 3:
_LL2254: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2264=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2264[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2265; _temp2265.tag= Cyc_DeclList_tok; _temp2265.f1=({ struct Cyc_List_List*
_temp2266=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2266->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2267=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2267->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp2274=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2274[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp2275; _temp2275.tag= Cyc_Absyn_Using_d; _temp2275.f1=
Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2276= yyvs; void** _temp2278=
_temp2276.curr +( yyvsp_offset - 2); if( _temp2276.base == 0? 1:( _temp2278 <
_temp2276.base? 1: _temp2278 >= _temp2276.last_plus_one)){ _throw(
Null_Exception);}* _temp2278;})); _temp2275.f2= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2279= yyvs; void** _temp2281= _temp2279.curr + yyvsp_offset;
if( _temp2279.base == 0? 1:( _temp2281 < _temp2279.base? 1: _temp2281 >=
_temp2279.last_plus_one)){ _throw( Null_Exception);}* _temp2281;})); _temp2275;});
_temp2274;})); _temp2267->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2268= yyls; struct Cyc_Yyltype* _temp2270= _temp2268.curr +(
yylsp_offset - 2); if( _temp2268.base == 0? 1:( _temp2270 < _temp2268.base? 1:
_temp2270 >= _temp2268.last_plus_one)){ _throw( Null_Exception);}* _temp2270;}).first_line,({
struct _tagged_ptr3 _temp2271= yyls; struct Cyc_Yyltype* _temp2273= _temp2271.curr
+ yylsp_offset; if( _temp2271.base == 0? 1:( _temp2273 < _temp2271.base? 1:
_temp2273 >= _temp2271.last_plus_one)){ _throw( Null_Exception);}* _temp2273;}).last_line);
_temp2267;}); _temp2266->tl= 0; _temp2266;}); _temp2265;}); _temp2264;}); Cyc_Lex_leave_using();
break; case 4: _LL2263: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2283=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2283[ 0]=({ struct Cyc_DeclList_tok_struct _temp2284; _temp2284.tag= Cyc_DeclList_tok;
_temp2284.f1=({ struct Cyc_List_List* _temp2285=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2285->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2289=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2289->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2296=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2296[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2297; _temp2297.tag= Cyc_Absyn_Using_d;
_temp2297.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2298= yyvs; void**
_temp2300= _temp2298.curr +( yyvsp_offset - 4); if( _temp2298.base == 0? 1:(
_temp2300 < _temp2298.base? 1: _temp2300 >= _temp2298.last_plus_one)){ _throw(
Null_Exception);}* _temp2300;})); _temp2297.f2= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2301= yyvs; void** _temp2303= _temp2301.curr +( yyvsp_offset -
2); if( _temp2301.base == 0? 1:( _temp2303 < _temp2301.base? 1: _temp2303 >=
_temp2301.last_plus_one)){ _throw( Null_Exception);}* _temp2303;})); _temp2297;});
_temp2296;})); _temp2289->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2290= yyls; struct Cyc_Yyltype* _temp2292= _temp2290.curr +(
yylsp_offset - 4); if( _temp2290.base == 0? 1:( _temp2292 < _temp2290.base? 1:
_temp2292 >= _temp2290.last_plus_one)){ _throw( Null_Exception);}* _temp2292;}).first_line,({
struct _tagged_ptr3 _temp2293= yyls; struct Cyc_Yyltype* _temp2295= _temp2293.curr
+( yylsp_offset - 1); if( _temp2293.base == 0? 1:( _temp2295 < _temp2293.base? 1:
_temp2295 >= _temp2293.last_plus_one)){ _throw( Null_Exception);}* _temp2295;}).last_line);
_temp2289;}); _temp2285->tl= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2286= yyvs; void** _temp2288= _temp2286.curr + yyvsp_offset; if( _temp2286.base
== 0? 1:( _temp2288 < _temp2286.base? 1: _temp2288 >= _temp2286.last_plus_one)){
_throw( Null_Exception);}* _temp2288;})); _temp2285;}); _temp2284;}); _temp2283;});
break; case 5: _LL2282: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2305=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2305[ 0]=({ struct Cyc_DeclList_tok_struct _temp2306; _temp2306.tag= Cyc_DeclList_tok;
_temp2306.f1=({ struct Cyc_List_List* _temp2307=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2307->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2308=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2308->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2315=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2315[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2316; _temp2316.tag=
Cyc_Absyn_Namespace_d; _temp2316.f1=({ struct _tagged_string* _temp2317=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2317[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2318= yyvs; void** _temp2320=
_temp2318.curr +( yyvsp_offset - 2); if( _temp2318.base == 0? 1:( _temp2320 <
_temp2318.base? 1: _temp2320 >= _temp2318.last_plus_one)){ _throw(
Null_Exception);}* _temp2320;})); _temp2317;}); _temp2316.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2321= yyvs; void** _temp2323= _temp2321.curr +
yyvsp_offset; if( _temp2321.base == 0? 1:( _temp2323 < _temp2321.base? 1:
_temp2323 >= _temp2321.last_plus_one)){ _throw( Null_Exception);}* _temp2323;}));
_temp2316;}); _temp2315;})); _temp2308->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2309= yyls; struct Cyc_Yyltype* _temp2311= _temp2309.curr
+( yylsp_offset - 2); if( _temp2309.base == 0? 1:( _temp2311 < _temp2309.base? 1:
_temp2311 >= _temp2309.last_plus_one)){ _throw( Null_Exception);}* _temp2311;}).first_line,({
struct _tagged_ptr3 _temp2312= yyls; struct Cyc_Yyltype* _temp2314= _temp2312.curr
+ yylsp_offset; if( _temp2312.base == 0? 1:( _temp2314 < _temp2312.base? 1:
_temp2314 >= _temp2312.last_plus_one)){ _throw( Null_Exception);}* _temp2314;}).last_line);
_temp2308;}); _temp2307->tl= 0; _temp2307;}); _temp2306;}); _temp2305;}); Cyc_Lex_leave_namespace();
break; case 6: _LL2304: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2325=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2325[ 0]=({ struct Cyc_DeclList_tok_struct _temp2326; _temp2326.tag= Cyc_DeclList_tok;
_temp2326.f1=({ struct Cyc_List_List* _temp2327=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2327->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2331=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2331->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2338=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2338[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2339; _temp2339.tag=
Cyc_Absyn_Namespace_d; _temp2339.f1=({ struct _tagged_string* _temp2340=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2340[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2341= yyvs; void** _temp2343=
_temp2341.curr +( yyvsp_offset - 4); if( _temp2341.base == 0? 1:( _temp2343 <
_temp2341.base? 1: _temp2343 >= _temp2341.last_plus_one)){ _throw(
Null_Exception);}* _temp2343;})); _temp2340;}); _temp2339.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2344= yyvs; void** _temp2346= _temp2344.curr +(
yyvsp_offset - 2); if( _temp2344.base == 0? 1:( _temp2346 < _temp2344.base? 1:
_temp2346 >= _temp2344.last_plus_one)){ _throw( Null_Exception);}* _temp2346;}));
_temp2339;}); _temp2338;})); _temp2331->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2332= yyls; struct Cyc_Yyltype* _temp2334= _temp2332.curr
+( yylsp_offset - 4); if( _temp2332.base == 0? 1:( _temp2334 < _temp2332.base? 1:
_temp2334 >= _temp2332.last_plus_one)){ _throw( Null_Exception);}* _temp2334;}).first_line,({
struct _tagged_ptr3 _temp2335= yyls; struct Cyc_Yyltype* _temp2337= _temp2335.curr
+( yylsp_offset - 1); if( _temp2335.base == 0? 1:( _temp2337 < _temp2335.base? 1:
_temp2337 >= _temp2335.last_plus_one)){ _throw( Null_Exception);}* _temp2337;}).last_line);
_temp2331;}); _temp2327->tl= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2328= yyvs; void** _temp2330= _temp2328.curr + yyvsp_offset; if( _temp2328.base
== 0? 1:( _temp2330 < _temp2328.base? 1: _temp2330 >= _temp2328.last_plus_one)){
_throw( Null_Exception);}* _temp2330;})); _temp2327;}); _temp2326;}); _temp2325;});
break; case 7: _LL2324: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2348= yyvs; void** _temp2350= _temp2348.curr +( yyvsp_offset -
4); if( _temp2348.base == 0? 1:( _temp2350 < _temp2348.base? 1: _temp2350 >=
_temp2348.last_plus_one)){ _throw( Null_Exception);}* _temp2350;})),( struct
_tagged_string)({ char* _temp2351=( char*)"C"; struct _tagged_string _temp2352;
_temp2352.curr= _temp2351; _temp2352.base= _temp2351; _temp2352.last_plus_one=
_temp2351 + 2; _temp2352;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2353=( char*)"only extern \"C\" { ... } is allowed"; struct
_tagged_string _temp2354; _temp2354.curr= _temp2353; _temp2354.base= _temp2353;
_temp2354.last_plus_one= _temp2353 + 35; _temp2354;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2355= yyls; struct Cyc_Yyltype* _temp2357= _temp2355.curr
+( yylsp_offset - 5); if( _temp2355.base == 0? 1:( _temp2357 < _temp2355.base? 1:
_temp2357 >= _temp2355.last_plus_one)){ _throw( Null_Exception);}* _temp2357;}).first_line,({
struct _tagged_ptr3 _temp2358= yyls; struct Cyc_Yyltype* _temp2360= _temp2358.curr
+( yylsp_offset - 4); if( _temp2358.base == 0? 1:( _temp2360 < _temp2358.base? 1:
_temp2360 >= _temp2358.last_plus_one)){ _throw( Null_Exception);}* _temp2360;}).last_line));}
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2361=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2361[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2362; _temp2362.tag= Cyc_DeclList_tok; _temp2362.f1=({ struct Cyc_List_List*
_temp2363=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2363->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2367=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2367->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp2374=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp2374[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp2375; _temp2375.tag= Cyc_Absyn_ExternC_d;
_temp2375.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2376= yyvs;
void** _temp2378= _temp2376.curr +( yyvsp_offset - 2); if( _temp2376.base == 0?
1:( _temp2378 < _temp2376.base? 1: _temp2378 >= _temp2376.last_plus_one)){
_throw( Null_Exception);}* _temp2378;})); _temp2375;}); _temp2374;})); _temp2367->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2368= yyls; struct Cyc_Yyltype*
_temp2370= _temp2368.curr +( yylsp_offset - 5); if( _temp2368.base == 0? 1:(
_temp2370 < _temp2368.base? 1: _temp2370 >= _temp2368.last_plus_one)){ _throw(
Null_Exception);}* _temp2370;}).first_line,({ struct _tagged_ptr3 _temp2371=
yyls; struct Cyc_Yyltype* _temp2373= _temp2371.curr +( yylsp_offset - 1); if(
_temp2371.base == 0? 1:( _temp2373 < _temp2371.base? 1: _temp2373 >= _temp2371.last_plus_one)){
_throw( Null_Exception);}* _temp2373;}).last_line); _temp2367;}); _temp2363->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2364= yyvs; void** _temp2366=
_temp2364.curr + yyvsp_offset; if( _temp2364.base == 0? 1:( _temp2366 <
_temp2364.base? 1: _temp2366 >= _temp2364.last_plus_one)){ _throw(
Null_Exception);}* _temp2366;})); _temp2363;}); _temp2362;}); _temp2361;});
break; case 8: _LL2347: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2380=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2380[ 0]=({ struct Cyc_DeclList_tok_struct _temp2381; _temp2381.tag= Cyc_DeclList_tok;
_temp2381.f1= 0; _temp2381;}); _temp2380;}); break; case 9: _LL2379: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp2383=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2383[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2384; _temp2384.tag= Cyc_DeclList_tok; _temp2384.f1=({ struct Cyc_List_List*
_temp2385=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2385->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2386=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2386[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2387; _temp2387.tag= Cyc_Absyn_Fn_d;
_temp2387.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp2388= yyvs; void**
_temp2390= _temp2388.curr + yyvsp_offset; if( _temp2388.base == 0? 1:( _temp2390
< _temp2388.base? 1: _temp2390 >= _temp2388.last_plus_one)){ _throw(
Null_Exception);}* _temp2390;})); _temp2387;}); _temp2386;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2391= yyls; struct Cyc_Yyltype* _temp2393= _temp2391.curr
+ yylsp_offset; if( _temp2391.base == 0? 1:( _temp2393 < _temp2391.base? 1:
_temp2393 >= _temp2391.last_plus_one)){ _throw( Null_Exception);}* _temp2393;}).first_line,({
struct _tagged_ptr3 _temp2394= yyls; struct Cyc_Yyltype* _temp2396= _temp2394.curr
+ yylsp_offset; if( _temp2394.base == 0? 1:( _temp2396 < _temp2394.base? 1:
_temp2396 >= _temp2394.last_plus_one)){ _throw( Null_Exception);}* _temp2396;}).last_line));
_temp2385->tl= 0; _temp2385;}); _temp2384;}); _temp2383;}); break; case 10:
_LL2382: yyval=({ struct _tagged_ptr2 _temp2398= yyvs; void** _temp2400=
_temp2398.curr + yyvsp_offset; if( _temp2398.base == 0? 1:( _temp2400 <
_temp2398.base? 1: _temp2400 >= _temp2398.last_plus_one)){ _throw(
Null_Exception);}* _temp2400;}); break; case 11: _LL2397: yyval=( void*)({
struct Cyc_FnDecl_tok_struct* _temp2402=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2402[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2403; _temp2403.tag= Cyc_FnDecl_tok; _temp2403.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2404= yyvs; void**
_temp2406= _temp2404.curr +( yyvsp_offset - 1); if( _temp2404.base == 0? 1:(
_temp2406 < _temp2404.base? 1: _temp2406 >= _temp2404.last_plus_one)){ _throw(
Null_Exception);}* _temp2406;})), 0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2407= yyvs; void** _temp2409= _temp2407.curr + yyvsp_offset; if( _temp2407.base
== 0? 1:( _temp2409 < _temp2407.base? 1: _temp2409 >= _temp2407.last_plus_one)){
_throw( Null_Exception);}* _temp2409;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2410= yyls; struct Cyc_Yyltype* _temp2412= _temp2410.curr +(
yylsp_offset - 1); if( _temp2410.base == 0? 1:( _temp2412 < _temp2410.base? 1:
_temp2412 >= _temp2410.last_plus_one)){ _throw( Null_Exception);}* _temp2412;}).first_line,({
struct _tagged_ptr3 _temp2413= yyls; struct Cyc_Yyltype* _temp2415= _temp2413.curr
+ yylsp_offset; if( _temp2413.base == 0? 1:( _temp2415 < _temp2413.base? 1:
_temp2415 >= _temp2413.last_plus_one)){ _throw( Null_Exception);}* _temp2415;}).last_line));
_temp2403;}); _temp2402;}); break; case 12: _LL2401: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2417=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2417[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2418; _temp2418.tag= Cyc_FnDecl_tok;
_temp2418.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2419=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2419->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2420= yyvs; void** _temp2422=
_temp2420.curr +( yyvsp_offset - 2); if( _temp2420.base == 0? 1:( _temp2422 <
_temp2420.base? 1: _temp2422 >= _temp2420.last_plus_one)){ _throw(
Null_Exception);}* _temp2422;})); _temp2419;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2423= yyvs; void** _temp2425= _temp2423.curr +(
yyvsp_offset - 1); if( _temp2423.base == 0? 1:( _temp2425 < _temp2423.base? 1:
_temp2425 >= _temp2423.last_plus_one)){ _throw( Null_Exception);}* _temp2425;})),
0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2426= yyvs; void** _temp2428=
_temp2426.curr + yyvsp_offset; if( _temp2426.base == 0? 1:( _temp2428 <
_temp2426.base? 1: _temp2428 >= _temp2426.last_plus_one)){ _throw(
Null_Exception);}* _temp2428;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2429= yyls; struct Cyc_Yyltype* _temp2431= _temp2429.curr +(
yylsp_offset - 2); if( _temp2429.base == 0? 1:( _temp2431 < _temp2429.base? 1:
_temp2431 >= _temp2429.last_plus_one)){ _throw( Null_Exception);}* _temp2431;}).first_line,({
struct _tagged_ptr3 _temp2432= yyls; struct Cyc_Yyltype* _temp2434= _temp2432.curr
+ yylsp_offset; if( _temp2432.base == 0? 1:( _temp2434 < _temp2432.base? 1:
_temp2434 >= _temp2432.last_plus_one)){ _throw( Null_Exception);}* _temp2434;}).last_line));
_temp2418;}); _temp2417;}); break; case 13: _LL2416: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2436=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2436[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2437; _temp2437.tag= Cyc_FnDecl_tok;
_temp2437.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2438= yyvs; void** _temp2440= _temp2438.curr +( yyvsp_offset -
2); if( _temp2438.base == 0? 1:( _temp2440 < _temp2438.base? 1: _temp2440 >=
_temp2438.last_plus_one)){ _throw( Null_Exception);}* _temp2440;})), Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2441= yyvs; void** _temp2443= _temp2441.curr +(
yyvsp_offset - 1); if( _temp2441.base == 0? 1:( _temp2443 < _temp2441.base? 1:
_temp2443 >= _temp2441.last_plus_one)){ _throw( Null_Exception);}* _temp2443;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2444= yyvs; void** _temp2446=
_temp2444.curr + yyvsp_offset; if( _temp2444.base == 0? 1:( _temp2446 <
_temp2444.base? 1: _temp2446 >= _temp2444.last_plus_one)){ _throw(
Null_Exception);}* _temp2446;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2447= yyls; struct Cyc_Yyltype* _temp2449= _temp2447.curr +(
yylsp_offset - 2); if( _temp2447.base == 0? 1:( _temp2449 < _temp2447.base? 1:
_temp2449 >= _temp2447.last_plus_one)){ _throw( Null_Exception);}* _temp2449;}).first_line,({
struct _tagged_ptr3 _temp2450= yyls; struct Cyc_Yyltype* _temp2452= _temp2450.curr
+ yylsp_offset; if( _temp2450.base == 0? 1:( _temp2452 < _temp2450.base? 1:
_temp2452 >= _temp2450.last_plus_one)){ _throw( Null_Exception);}* _temp2452;}).last_line));
_temp2437;}); _temp2436;}); break; case 14: _LL2435: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2454=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2454[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2455; _temp2455.tag= Cyc_FnDecl_tok;
_temp2455.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2456=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2456->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2457= yyvs; void** _temp2459=
_temp2457.curr +( yyvsp_offset - 3); if( _temp2457.base == 0? 1:( _temp2459 <
_temp2457.base? 1: _temp2459 >= _temp2457.last_plus_one)){ _throw(
Null_Exception);}* _temp2459;})); _temp2456;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2460= yyvs; void** _temp2462= _temp2460.curr +(
yyvsp_offset - 2); if( _temp2460.base == 0? 1:( _temp2462 < _temp2460.base? 1:
_temp2462 >= _temp2460.last_plus_one)){ _throw( Null_Exception);}* _temp2462;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2463= yyvs; void** _temp2465=
_temp2463.curr +( yyvsp_offset - 1); if( _temp2463.base == 0? 1:( _temp2465 <
_temp2463.base? 1: _temp2465 >= _temp2463.last_plus_one)){ _throw(
Null_Exception);}* _temp2465;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2466= yyvs; void** _temp2468= _temp2466.curr + yyvsp_offset; if( _temp2466.base
== 0? 1:( _temp2468 < _temp2466.base? 1: _temp2468 >= _temp2466.last_plus_one)){
_throw( Null_Exception);}* _temp2468;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2469= yyls; struct Cyc_Yyltype* _temp2471= _temp2469.curr +(
yylsp_offset - 3); if( _temp2469.base == 0? 1:( _temp2471 < _temp2469.base? 1:
_temp2471 >= _temp2469.last_plus_one)){ _throw( Null_Exception);}* _temp2471;}).first_line,({
struct _tagged_ptr3 _temp2472= yyls; struct Cyc_Yyltype* _temp2474= _temp2472.curr
+ yylsp_offset; if( _temp2472.base == 0? 1:( _temp2474 < _temp2472.base? 1:
_temp2474 >= _temp2472.last_plus_one)){ _throw( Null_Exception);}* _temp2474;}).last_line));
_temp2455;}); _temp2454;}); break; case 15: _LL2453: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2476=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2476[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2477; _temp2477.tag= Cyc_FnDecl_tok;
_temp2477.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2478=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2478->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2479= yyvs; void** _temp2481=
_temp2479.curr +( yyvsp_offset - 2); if( _temp2479.base == 0? 1:( _temp2481 <
_temp2479.base? 1: _temp2481 >= _temp2479.last_plus_one)){ _throw(
Null_Exception);}* _temp2481;})); _temp2478;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2482= yyvs; void** _temp2484= _temp2482.curr +(
yyvsp_offset - 1); if( _temp2482.base == 0? 1:( _temp2484 < _temp2482.base? 1:
_temp2484 >= _temp2482.last_plus_one)){ _throw( Null_Exception);}* _temp2484;})),
0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2485= yyvs; void** _temp2487=
_temp2485.curr + yyvsp_offset; if( _temp2485.base == 0? 1:( _temp2487 <
_temp2485.base? 1: _temp2487 >= _temp2485.last_plus_one)){ _throw(
Null_Exception);}* _temp2487;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2488= yyls; struct Cyc_Yyltype* _temp2490= _temp2488.curr +(
yylsp_offset - 2); if( _temp2488.base == 0? 1:( _temp2490 < _temp2488.base? 1:
_temp2490 >= _temp2488.last_plus_one)){ _throw( Null_Exception);}* _temp2490;}).first_line,({
struct _tagged_ptr3 _temp2491= yyls; struct Cyc_Yyltype* _temp2493= _temp2491.curr
+ yylsp_offset; if( _temp2491.base == 0? 1:( _temp2493 < _temp2491.base? 1:
_temp2493 >= _temp2491.last_plus_one)){ _throw( Null_Exception);}* _temp2493;}).last_line));
_temp2477;}); _temp2476;}); break; case 16: _LL2475: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2495=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2495[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2496; _temp2496.tag= Cyc_FnDecl_tok;
_temp2496.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2497=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2497->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2498= yyvs; void** _temp2500=
_temp2498.curr +( yyvsp_offset - 3); if( _temp2498.base == 0? 1:( _temp2500 <
_temp2498.base? 1: _temp2500 >= _temp2498.last_plus_one)){ _throw(
Null_Exception);}* _temp2500;})); _temp2497;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2501= yyvs; void** _temp2503= _temp2501.curr +(
yyvsp_offset - 2); if( _temp2501.base == 0? 1:( _temp2503 < _temp2501.base? 1:
_temp2503 >= _temp2501.last_plus_one)){ _throw( Null_Exception);}* _temp2503;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2504= yyvs; void** _temp2506=
_temp2504.curr +( yyvsp_offset - 1); if( _temp2504.base == 0? 1:( _temp2506 <
_temp2504.base? 1: _temp2506 >= _temp2504.last_plus_one)){ _throw(
Null_Exception);}* _temp2506;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2507= yyvs; void** _temp2509= _temp2507.curr + yyvsp_offset; if( _temp2507.base
== 0? 1:( _temp2509 < _temp2507.base? 1: _temp2509 >= _temp2507.last_plus_one)){
_throw( Null_Exception);}* _temp2509;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2510= yyls; struct Cyc_Yyltype* _temp2512= _temp2510.curr +(
yylsp_offset - 3); if( _temp2510.base == 0? 1:( _temp2512 < _temp2510.base? 1:
_temp2512 >= _temp2510.last_plus_one)){ _throw( Null_Exception);}* _temp2512;}).first_line,({
struct _tagged_ptr3 _temp2513= yyls; struct Cyc_Yyltype* _temp2515= _temp2513.curr
+ yylsp_offset; if( _temp2513.base == 0? 1:( _temp2515 < _temp2513.base? 1:
_temp2515 >= _temp2513.last_plus_one)){ _throw( Null_Exception);}* _temp2515;}).last_line));
_temp2496;}); _temp2495;}); break; case 17: _LL2494: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2517= yyvs; void** _temp2519= _temp2517.curr +
yyvsp_offset; if( _temp2517.base == 0? 1:( _temp2519 < _temp2517.base? 1:
_temp2519 >= _temp2517.last_plus_one)){ _throw( Null_Exception);}* _temp2519;})));
yyval=({ struct _tagged_ptr2 _temp2520= yyvs; void** _temp2522= _temp2520.curr +
yyvsp_offset; if( _temp2520.base == 0? 1:( _temp2522 < _temp2520.base? 1:
_temp2522 >= _temp2520.last_plus_one)){ _throw( Null_Exception);}* _temp2522;});
break; case 18: _LL2516: Cyc_Lex_leave_using(); break; case 19: _LL2523: Cyc_Lex_enter_namespace(({
struct _tagged_string* _temp2525=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2525[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2526= yyvs; void** _temp2528= _temp2526.curr + yyvsp_offset;
if( _temp2526.base == 0? 1:( _temp2528 < _temp2526.base? 1: _temp2528 >=
_temp2526.last_plus_one)){ _throw( Null_Exception);}* _temp2528;})); _temp2525;}));
yyval=({ struct _tagged_ptr2 _temp2529= yyvs; void** _temp2531= _temp2529.curr +
yyvsp_offset; if( _temp2529.base == 0? 1:( _temp2531 < _temp2529.base? 1:
_temp2531 >= _temp2529.last_plus_one)){ _throw( Null_Exception);}* _temp2531;});
break; case 20: _LL2524: Cyc_Lex_leave_namespace(); break; case 21: _LL2532:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2534=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2534[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2535; _temp2535.tag= Cyc_DeclList_tok; _temp2535.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2536= yyvs; void** _temp2538=
_temp2536.curr +( yyvsp_offset - 1); if( _temp2536.base == 0? 1:( _temp2538 <
_temp2536.base? 1: _temp2538 >= _temp2536.last_plus_one)){ _throw(
Null_Exception);}* _temp2538;})), 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2539= yyls; struct Cyc_Yyltype* _temp2541= _temp2539.curr +(
yylsp_offset - 1); if( _temp2539.base == 0? 1:( _temp2541 < _temp2539.base? 1:
_temp2541 >= _temp2539.last_plus_one)){ _throw( Null_Exception);}* _temp2541;}).first_line,({
struct _tagged_ptr3 _temp2542= yyls; struct Cyc_Yyltype* _temp2544= _temp2542.curr
+( yylsp_offset - 1); if( _temp2542.base == 0? 1:( _temp2544 < _temp2542.base? 1:
_temp2544 >= _temp2542.last_plus_one)){ _throw( Null_Exception);}* _temp2544;}).last_line));
_temp2535;}); _temp2534;}); break; case 22: _LL2533: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2546=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2546[ 0]=({ struct Cyc_DeclList_tok_struct _temp2547; _temp2547.tag= Cyc_DeclList_tok;
_temp2547.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2548= yyvs; void** _temp2550= _temp2548.curr +( yyvsp_offset -
2); if( _temp2548.base == 0? 1:( _temp2550 < _temp2548.base? 1: _temp2550 >=
_temp2548.last_plus_one)){ _throw( Null_Exception);}* _temp2550;})), Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp2551= yyvs; void** _temp2553= _temp2551.curr +(
yyvsp_offset - 1); if( _temp2551.base == 0? 1:( _temp2553 < _temp2551.base? 1:
_temp2553 >= _temp2551.last_plus_one)){ _throw( Null_Exception);}* _temp2553;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2554= yyls; struct Cyc_Yyltype*
_temp2556= _temp2554.curr +( yylsp_offset - 2); if( _temp2554.base == 0? 1:(
_temp2556 < _temp2554.base? 1: _temp2556 >= _temp2554.last_plus_one)){ _throw(
Null_Exception);}* _temp2556;}).first_line,({ struct _tagged_ptr3 _temp2557=
yyls; struct Cyc_Yyltype* _temp2559= _temp2557.curr + yylsp_offset; if(
_temp2557.base == 0? 1:( _temp2559 < _temp2557.base? 1: _temp2559 >= _temp2557.last_plus_one)){
_throw( Null_Exception);}* _temp2559;}).last_line)); _temp2547;}); _temp2546;});
break; case 23: _LL2545: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2561=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2561[ 0]=({ struct Cyc_DeclList_tok_struct _temp2562; _temp2562.tag= Cyc_DeclList_tok;
_temp2562.f1=({ struct Cyc_List_List* _temp2563=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2563->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp2564= yyvs; void** _temp2566=
_temp2564.curr +( yyvsp_offset - 3); if( _temp2564.base == 0? 1:( _temp2566 <
_temp2564.base? 1: _temp2566 >= _temp2564.last_plus_one)){ _throw(
Null_Exception);}* _temp2566;})), 0, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp2567= yyvs; void** _temp2569= _temp2567.curr +( yyvsp_offset - 1); if(
_temp2567.base == 0? 1:( _temp2569 < _temp2567.base? 1: _temp2569 >= _temp2567.last_plus_one)){
_throw( Null_Exception);}* _temp2569;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2570= yyls; struct Cyc_Yyltype* _temp2572= _temp2570.curr +(
yylsp_offset - 4); if( _temp2570.base == 0? 1:( _temp2572 < _temp2570.base? 1:
_temp2572 >= _temp2570.last_plus_one)){ _throw( Null_Exception);}* _temp2572;}).first_line,({
struct _tagged_ptr3 _temp2573= yyls; struct Cyc_Yyltype* _temp2575= _temp2573.curr
+ yylsp_offset; if( _temp2573.base == 0? 1:( _temp2575 < _temp2573.base? 1:
_temp2575 >= _temp2573.last_plus_one)){ _throw( Null_Exception);}* _temp2575;}).last_line));
_temp2563->tl= 0; _temp2563;}); _temp2562;}); _temp2561;}); break; case 24:
_LL2560: yyval=({ struct _tagged_ptr2 _temp2577= yyvs; void** _temp2579=
_temp2577.curr + yyvsp_offset; if( _temp2577.base == 0? 1:( _temp2579 <
_temp2577.base? 1: _temp2579 >= _temp2577.last_plus_one)){ _throw(
Null_Exception);}* _temp2579;}); break; case 25: _LL2576: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp2581=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2581[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2582; _temp2582.tag= Cyc_DeclList_tok; _temp2582.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2583= yyvs; void** _temp2585= _temp2583.curr +(
yyvsp_offset - 1); if( _temp2583.base == 0? 1:( _temp2585 < _temp2583.base? 1:
_temp2585 >= _temp2583.last_plus_one)){ _throw( Null_Exception);}* _temp2585;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2586= yyvs; void** _temp2588=
_temp2586.curr + yyvsp_offset; if( _temp2586.base == 0? 1:( _temp2588 <
_temp2586.base? 1: _temp2588 >= _temp2586.last_plus_one)){ _throw(
Null_Exception);}* _temp2588;}))); _temp2582;}); _temp2581;}); break; case 26:
_LL2580: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2590=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2590[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2591; _temp2591.tag= Cyc_DeclSpec_tok; _temp2591.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2592=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2592->sc=({ struct Cyc_Core_Opt* _temp2596=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2596->v=( void*) Cyc_yyget_StorageClass_tok(({
struct _tagged_ptr2 _temp2597= yyvs; void** _temp2599= _temp2597.curr +(
yyvsp_offset - 1); if( _temp2597.base == 0? 1:( _temp2599 < _temp2597.base? 1:
_temp2599 >= _temp2597.last_plus_one)){ _throw( Null_Exception);}* _temp2599;}));
_temp2596;}); _temp2592->tq= Cyc_Absyn_empty_tqual(); _temp2592->type_specs= 0;
_temp2592->is_inline= 0; _temp2592->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2593= yyvs; void** _temp2595= _temp2593.curr +
yyvsp_offset; if( _temp2593.base == 0? 1:( _temp2595 < _temp2593.base? 1:
_temp2595 >= _temp2593.last_plus_one)){ _throw( Null_Exception);}* _temp2595;}));
_temp2592;}); _temp2591;}); _temp2590;}); break; case 27: _LL2589: if(( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2601= yyvs; void** _temp2603= _temp2601.curr +
yyvsp_offset; if( _temp2601.base == 0? 1:( _temp2603 < _temp2601.base? 1:
_temp2603 >= _temp2601.last_plus_one)){ _throw( Null_Exception);}* _temp2603;})))->sc
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp2604=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2605; _temp2605.curr= _temp2604; _temp2605.base=
_temp2604; _temp2605.last_plus_one= _temp2604 + 51; _temp2605;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2606= yyls; struct Cyc_Yyltype* _temp2608= _temp2606.curr
+( yylsp_offset - 2); if( _temp2606.base == 0? 1:( _temp2608 < _temp2606.base? 1:
_temp2608 >= _temp2606.last_plus_one)){ _throw( Null_Exception);}* _temp2608;}).first_line,({
struct _tagged_ptr3 _temp2609= yyls; struct Cyc_Yyltype* _temp2611= _temp2609.curr
+( yylsp_offset - 1); if( _temp2609.base == 0? 1:( _temp2611 < _temp2609.base? 1:
_temp2611 >= _temp2609.last_plus_one)){ _throw( Null_Exception);}* _temp2611;}).last_line));}
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2612=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2612[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2613; _temp2613.tag= Cyc_DeclSpec_tok; _temp2613.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2614=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2614->sc=({ struct Cyc_Core_Opt* _temp2630=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2630->v=( void*) Cyc_yyget_StorageClass_tok(({
struct _tagged_ptr2 _temp2631= yyvs; void** _temp2633= _temp2631.curr +(
yyvsp_offset - 2); if( _temp2631.base == 0? 1:( _temp2633 < _temp2631.base? 1:
_temp2633 >= _temp2631.last_plus_one)){ _throw( Null_Exception);}* _temp2633;}));
_temp2630;}); _temp2614->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2627= yyvs; void** _temp2629= _temp2627.curr + yyvsp_offset; if( _temp2627.base
== 0? 1:( _temp2629 < _temp2627.base? 1: _temp2629 >= _temp2627.last_plus_one)){
_throw( Null_Exception);}* _temp2629;})))->tq; _temp2614->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2624= yyvs; void** _temp2626= _temp2624.curr +
yyvsp_offset; if( _temp2624.base == 0? 1:( _temp2626 < _temp2624.base? 1:
_temp2626 >= _temp2624.last_plus_one)){ _throw( Null_Exception);}* _temp2626;})))->type_specs;
_temp2614->is_inline=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2621=
yyvs; void** _temp2623= _temp2621.curr + yyvsp_offset; if( _temp2621.base == 0?
1:( _temp2623 < _temp2621.base? 1: _temp2623 >= _temp2621.last_plus_one)){
_throw( Null_Exception);}* _temp2623;})))->is_inline; _temp2614->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2615= yyvs; void**
_temp2617= _temp2615.curr +( yyvsp_offset - 1); if( _temp2615.base == 0? 1:(
_temp2617 < _temp2615.base? 1: _temp2617 >= _temp2615.last_plus_one)){ _throw(
Null_Exception);}* _temp2617;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2618= yyvs; void** _temp2620= _temp2618.curr + yyvsp_offset; if( _temp2618.base
== 0? 1:( _temp2620 < _temp2618.base? 1: _temp2620 >= _temp2618.last_plus_one)){
_throw( Null_Exception);}* _temp2620;})))->attributes); _temp2614;}); _temp2613;});
_temp2612;}); break; case 28: _LL2600: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2635=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2635[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2636; _temp2636.tag= Cyc_DeclSpec_tok;
_temp2636.f1=({ struct Cyc_Parse_Declaration_spec* _temp2637=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2637->sc= 0;
_temp2637->tq= Cyc_Absyn_empty_tqual(); _temp2637->type_specs=({ struct Cyc_List_List*
_temp2641=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2641->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2642= yyvs; void** _temp2644= _temp2642.curr +( yyvsp_offset - 1); if(
_temp2642.base == 0? 1:( _temp2644 < _temp2642.base? 1: _temp2644 >= _temp2642.last_plus_one)){
_throw( Null_Exception);}* _temp2644;})); _temp2641->tl= 0; _temp2641;});
_temp2637->is_inline= 0; _temp2637->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2638= yyvs; void** _temp2640= _temp2638.curr +
yyvsp_offset; if( _temp2638.base == 0? 1:( _temp2640 < _temp2638.base? 1:
_temp2640 >= _temp2638.last_plus_one)){ _throw( Null_Exception);}* _temp2640;}));
_temp2637;}); _temp2636;}); _temp2635;}); break; case 29: _LL2634: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2646=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2646[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2647; _temp2647.tag= Cyc_DeclSpec_tok; _temp2647.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2648=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2648->sc=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2668= yyvs;
void** _temp2670= _temp2668.curr + yyvsp_offset; if( _temp2668.base == 0? 1:(
_temp2670 < _temp2668.base? 1: _temp2670 >= _temp2668.last_plus_one)){ _throw(
Null_Exception);}* _temp2670;})))->sc; _temp2648->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2665= yyvs; void** _temp2667= _temp2665.curr +
yyvsp_offset; if( _temp2665.base == 0? 1:( _temp2667 < _temp2665.base? 1:
_temp2667 >= _temp2665.last_plus_one)){ _throw( Null_Exception);}* _temp2667;})))->tq;
_temp2648->type_specs=({ struct Cyc_List_List* _temp2658=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2658->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp2662= yyvs; void** _temp2664= _temp2662.curr +(
yyvsp_offset - 2); if( _temp2662.base == 0? 1:( _temp2664 < _temp2662.base? 1:
_temp2664 >= _temp2662.last_plus_one)){ _throw( Null_Exception);}* _temp2664;}));
_temp2658->tl=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2659= yyvs;
void** _temp2661= _temp2659.curr + yyvsp_offset; if( _temp2659.base == 0? 1:(
_temp2661 < _temp2659.base? 1: _temp2661 >= _temp2659.last_plus_one)){ _throw(
Null_Exception);}* _temp2661;})))->type_specs; _temp2658;}); _temp2648->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2655= yyvs; void** _temp2657=
_temp2655.curr + yyvsp_offset; if( _temp2655.base == 0? 1:( _temp2657 <
_temp2655.base? 1: _temp2657 >= _temp2655.last_plus_one)){ _throw(
Null_Exception);}* _temp2657;})))->is_inline; _temp2648->attributes=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2649= yyvs; void**
_temp2651= _temp2649.curr +( yyvsp_offset - 1); if( _temp2649.base == 0? 1:(
_temp2651 < _temp2649.base? 1: _temp2651 >= _temp2649.last_plus_one)){ _throw(
Null_Exception);}* _temp2651;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2652= yyvs; void** _temp2654= _temp2652.curr + yyvsp_offset; if( _temp2652.base
== 0? 1:( _temp2654 < _temp2652.base? 1: _temp2654 >= _temp2652.last_plus_one)){
_throw( Null_Exception);}* _temp2654;})))->attributes); _temp2648;}); _temp2647;});
_temp2646;}); break; case 30: _LL2645: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2672=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2672[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2673; _temp2673.tag= Cyc_DeclSpec_tok;
_temp2673.f1=({ struct Cyc_Parse_Declaration_spec* _temp2674=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2674->sc= 0;
_temp2674->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2678= yyvs;
void** _temp2680= _temp2678.curr +( yyvsp_offset - 1); if( _temp2678.base == 0?
1:( _temp2680 < _temp2678.base? 1: _temp2680 >= _temp2678.last_plus_one)){
_throw( Null_Exception);}* _temp2680;})); _temp2674->type_specs= 0; _temp2674->is_inline=
0; _temp2674->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2675= yyvs; void** _temp2677= _temp2675.curr + yyvsp_offset; if( _temp2675.base
== 0? 1:( _temp2677 < _temp2675.base? 1: _temp2677 >= _temp2675.last_plus_one)){
_throw( Null_Exception);}* _temp2677;})); _temp2674;}); _temp2673;}); _temp2672;});
break; case 31: _LL2671: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2682=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2682[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2683; _temp2683.tag= Cyc_DeclSpec_tok;
_temp2683.f1=({ struct Cyc_Parse_Declaration_spec* _temp2684=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2684->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2703= yyvs; void** _temp2705= _temp2703.curr +
yyvsp_offset; if( _temp2703.base == 0? 1:( _temp2705 < _temp2703.base? 1:
_temp2705 >= _temp2703.last_plus_one)){ _throw( Null_Exception);}* _temp2705;})))->sc;
_temp2684->tq= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp2697= yyvs; void** _temp2699= _temp2697.curr +( yyvsp_offset -
2); if( _temp2697.base == 0? 1:( _temp2699 < _temp2697.base? 1: _temp2699 >=
_temp2697.last_plus_one)){ _throw( Null_Exception);}* _temp2699;})),( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2700= yyvs; void** _temp2702= _temp2700.curr +
yyvsp_offset; if( _temp2700.base == 0? 1:( _temp2702 < _temp2700.base? 1:
_temp2702 >= _temp2700.last_plus_one)){ _throw( Null_Exception);}* _temp2702;})))->tq);
_temp2684->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2694=
yyvs; void** _temp2696= _temp2694.curr + yyvsp_offset; if( _temp2694.base == 0?
1:( _temp2696 < _temp2694.base? 1: _temp2696 >= _temp2694.last_plus_one)){
_throw( Null_Exception);}* _temp2696;})))->type_specs; _temp2684->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2691= yyvs; void** _temp2693=
_temp2691.curr + yyvsp_offset; if( _temp2691.base == 0? 1:( _temp2693 <
_temp2691.base? 1: _temp2693 >= _temp2691.last_plus_one)){ _throw(
Null_Exception);}* _temp2693;})))->is_inline; _temp2684->attributes=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2685= yyvs; void**
_temp2687= _temp2685.curr +( yyvsp_offset - 1); if( _temp2685.base == 0? 1:(
_temp2687 < _temp2685.base? 1: _temp2687 >= _temp2685.last_plus_one)){ _throw(
Null_Exception);}* _temp2687;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2688= yyvs; void** _temp2690= _temp2688.curr + yyvsp_offset; if( _temp2688.base
== 0? 1:( _temp2690 < _temp2688.base? 1: _temp2690 >= _temp2688.last_plus_one)){
_throw( Null_Exception);}* _temp2690;})))->attributes); _temp2684;}); _temp2683;});
_temp2682;}); break; case 32: _LL2681: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2707=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2707[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2708; _temp2708.tag= Cyc_DeclSpec_tok;
_temp2708.f1=({ struct Cyc_Parse_Declaration_spec* _temp2709=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2709->sc= 0;
_temp2709->tq= Cyc_Absyn_empty_tqual(); _temp2709->type_specs= 0; _temp2709->is_inline=
1; _temp2709->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2710= yyvs; void** _temp2712= _temp2710.curr + yyvsp_offset; if( _temp2710.base
== 0? 1:( _temp2712 < _temp2710.base? 1: _temp2712 >= _temp2710.last_plus_one)){
_throw( Null_Exception);}* _temp2712;})); _temp2709;}); _temp2708;}); _temp2707;});
break; case 33: _LL2706: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2714=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2714[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2715; _temp2715.tag= Cyc_DeclSpec_tok;
_temp2715.f1=({ struct Cyc_Parse_Declaration_spec* _temp2716=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2716->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2729= yyvs; void** _temp2731= _temp2729.curr +
yyvsp_offset; if( _temp2729.base == 0? 1:( _temp2731 < _temp2729.base? 1:
_temp2731 >= _temp2729.last_plus_one)){ _throw( Null_Exception);}* _temp2731;})))->sc;
_temp2716->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2726= yyvs;
void** _temp2728= _temp2726.curr + yyvsp_offset; if( _temp2726.base == 0? 1:(
_temp2728 < _temp2726.base? 1: _temp2728 >= _temp2726.last_plus_one)){ _throw(
Null_Exception);}* _temp2728;})))->tq; _temp2716->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2723= yyvs; void** _temp2725= _temp2723.curr +
yyvsp_offset; if( _temp2723.base == 0? 1:( _temp2725 < _temp2723.base? 1:
_temp2725 >= _temp2723.last_plus_one)){ _throw( Null_Exception);}* _temp2725;})))->type_specs;
_temp2716->is_inline= 1; _temp2716->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2717= yyvs; void** _temp2719= _temp2717.curr +(
yyvsp_offset - 1); if( _temp2717.base == 0? 1:( _temp2719 < _temp2717.base? 1:
_temp2719 >= _temp2717.last_plus_one)){ _throw( Null_Exception);}* _temp2719;})),(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2720= yyvs; void** _temp2722=
_temp2720.curr + yyvsp_offset; if( _temp2720.base == 0? 1:( _temp2722 <
_temp2720.base? 1: _temp2722 >= _temp2720.last_plus_one)){ _throw(
Null_Exception);}* _temp2722;})))->attributes); _temp2716;}); _temp2715;});
_temp2714;}); break; case 34: _LL2713: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2733=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2733[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2734; _temp2734.tag=
Cyc_StorageClass_tok; _temp2734.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp2734;}); _temp2733;}); break; case 35: _LL2732: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2736=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2736[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2737; _temp2737.tag=
Cyc_StorageClass_tok; _temp2737.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp2737;}); _temp2736;}); break; case 36: _LL2735: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2739=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2739[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2740; _temp2740.tag=
Cyc_StorageClass_tok; _temp2740.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2740;}); _temp2739;}); break; case 37: _LL2738: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2742=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2742[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2743; _temp2743.tag=
Cyc_StorageClass_tok; _temp2743.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp2743;}); _temp2742;}); break; case 38: _LL2741: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2745= yyvs; void** _temp2747= _temp2745.curr +
yyvsp_offset; if( _temp2745.base == 0? 1:( _temp2747 < _temp2745.base? 1:
_temp2747 >= _temp2745.last_plus_one)){ _throw( Null_Exception);}* _temp2747;})),(
struct _tagged_string)({ char* _temp2748=( char*)"C"; struct _tagged_string
_temp2749; _temp2749.curr= _temp2748; _temp2749.base= _temp2748; _temp2749.last_plus_one=
_temp2748 + 2; _temp2749;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2750=( char*)"only extern or extern \"C\" is allowed"; struct
_tagged_string _temp2751; _temp2751.curr= _temp2750; _temp2751.base= _temp2750;
_temp2751.last_plus_one= _temp2750 + 37; _temp2751;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2752= yyls; struct Cyc_Yyltype* _temp2754= _temp2752.curr
+( yylsp_offset - 1); if( _temp2752.base == 0? 1:( _temp2754 < _temp2752.base? 1:
_temp2754 >= _temp2752.last_plus_one)){ _throw( Null_Exception);}* _temp2754;}).first_line,({
struct _tagged_ptr3 _temp2755= yyls; struct Cyc_Yyltype* _temp2757= _temp2755.curr
+ yylsp_offset; if( _temp2755.base == 0? 1:( _temp2757 < _temp2755.base? 1:
_temp2757 >= _temp2755.last_plus_one)){ _throw( Null_Exception);}* _temp2757;}).last_line));}
yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp2758=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2758[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2759; _temp2759.tag= Cyc_StorageClass_tok;
_temp2759.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp2759;}); _temp2758;});
break; case 39: _LL2744: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2761=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2761[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2762; _temp2762.tag=
Cyc_StorageClass_tok; _temp2762.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp2762;}); _temp2761;}); break; case 40: _LL2760: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2764=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2764[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2765; _temp2765.tag=
Cyc_StorageClass_tok; _temp2765.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2765;}); _temp2764;}); break; case 41: _LL2763: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2767=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2767[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2768; _temp2768.tag=
Cyc_AttributeList_tok; _temp2768.f1= 0; _temp2768;}); _temp2767;}); break; case
42: _LL2766: yyval=({ struct _tagged_ptr2 _temp2770= yyvs; void** _temp2772=
_temp2770.curr + yyvsp_offset; if( _temp2770.base == 0? 1:( _temp2772 <
_temp2770.base? 1: _temp2772 >= _temp2770.last_plus_one)){ _throw(
Null_Exception);}* _temp2772;}); break; case 43: _LL2769: yyval=( void*)({
struct Cyc_AttributeList_tok_struct* _temp2774=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2774[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2775; _temp2775.tag= Cyc_AttributeList_tok;
_temp2775.f1= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2776= yyvs;
void** _temp2778= _temp2776.curr +( yyvsp_offset - 2); if( _temp2776.base == 0?
1:( _temp2778 < _temp2776.base? 1: _temp2778 >= _temp2776.last_plus_one)){
_throw( Null_Exception);}* _temp2778;})); _temp2775;}); _temp2774;}); break;
case 44: _LL2773: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2780=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2780[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2781; _temp2781.tag=
Cyc_AttributeList_tok; _temp2781.f1=({ struct Cyc_List_List* _temp2782=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2782->hd=( void*)
Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2783= yyvs; void** _temp2785=
_temp2783.curr + yyvsp_offset; if( _temp2783.base == 0? 1:( _temp2785 <
_temp2783.base? 1: _temp2785 >= _temp2783.last_plus_one)){ _throw(
Null_Exception);}* _temp2785;})); _temp2782->tl= 0; _temp2782;}); _temp2781;});
_temp2780;}); break; case 45: _LL2779: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2787=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2787[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2788; _temp2788.tag=
Cyc_AttributeList_tok; _temp2788.f1=({ struct Cyc_List_List* _temp2789=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2789->hd=( void*)
Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2793= yyvs; void** _temp2795=
_temp2793.curr +( yyvsp_offset - 2); if( _temp2793.base == 0? 1:( _temp2795 <
_temp2793.base? 1: _temp2795 >= _temp2793.last_plus_one)){ _throw(
Null_Exception);}* _temp2795;})); _temp2789->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2790= yyvs; void** _temp2792= _temp2790.curr +
yyvsp_offset; if( _temp2790.base == 0? 1:( _temp2792 < _temp2790.base? 1:
_temp2792 >= _temp2790.last_plus_one)){ _throw( Null_Exception);}* _temp2792;}));
_temp2789;}); _temp2788;}); _temp2787;}); break; case 46: _LL2786: { struct
_tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2871= yyvs;
void** _temp2873= _temp2871.curr + yyvsp_offset; if( _temp2871.base == 0? 1:(
_temp2873 < _temp2871.base? 1: _temp2873 >= _temp2871.last_plus_one)){ _throw(
Null_Exception);}* _temp2873;})); void* a; if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2797=( char*)"stdcall"; struct _tagged_string
_temp2798; _temp2798.curr= _temp2797; _temp2798.base= _temp2797; _temp2798.last_plus_one=
_temp2797 + 8; _temp2798;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2799=( char*)"__stdcall__"; struct _tagged_string
_temp2800; _temp2800.curr= _temp2799; _temp2800.base= _temp2799; _temp2800.last_plus_one=
_temp2799 + 12; _temp2800;})) == 0){ a=( void*) Cyc_Absyn_Stdcall_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2801=( char*)"cdecl";
struct _tagged_string _temp2802; _temp2802.curr= _temp2801; _temp2802.base=
_temp2801; _temp2802.last_plus_one= _temp2801 + 6; _temp2802;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2803=( char*)"__cdecl__"; struct
_tagged_string _temp2804; _temp2804.curr= _temp2803; _temp2804.base= _temp2803;
_temp2804.last_plus_one= _temp2803 + 10; _temp2804;})) == 0){ a=( void*) Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2805=(
char*)"noreturn"; struct _tagged_string _temp2806; _temp2806.curr= _temp2805;
_temp2806.base= _temp2805; _temp2806.last_plus_one= _temp2805 + 9; _temp2806;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2807=( char*)"__noreturn__";
struct _tagged_string _temp2808; _temp2808.curr= _temp2807; _temp2808.base=
_temp2807; _temp2808.last_plus_one= _temp2807 + 13; _temp2808;})) == 0){ a=(
void*) Cyc_Absyn_Noreturn_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2809=( char*)"noreturn"; struct _tagged_string
_temp2810; _temp2810.curr= _temp2809; _temp2810.base= _temp2809; _temp2810.last_plus_one=
_temp2809 + 9; _temp2810;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2811=( char*)"__noreturn__"; struct _tagged_string
_temp2812; _temp2812.curr= _temp2811; _temp2812.base= _temp2811; _temp2812.last_plus_one=
_temp2811 + 13; _temp2812;})) == 0){ a=( void*) Cyc_Absyn_Noreturn_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2813=( char*)"__const__";
struct _tagged_string _temp2814; _temp2814.curr= _temp2813; _temp2814.base=
_temp2813; _temp2814.last_plus_one= _temp2813 + 10; _temp2814;})) == 0){ a=(
void*) Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2815=( char*)"aligned"; struct _tagged_string
_temp2816; _temp2816.curr= _temp2815; _temp2816.base= _temp2815; _temp2816.last_plus_one=
_temp2815 + 8; _temp2816;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2817=( char*)"__aligned__"; struct _tagged_string
_temp2818; _temp2818.curr= _temp2817; _temp2818.base= _temp2817; _temp2818.last_plus_one=
_temp2817 + 12; _temp2818;})) == 0){ a=( void*)({ struct Cyc_Absyn_Aligned_att_struct*
_temp2819=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct));
_temp2819[ 0]=({ struct Cyc_Absyn_Aligned_att_struct _temp2820; _temp2820.tag=
Cyc_Absyn_Aligned_att; _temp2820.f1= - 1; _temp2820;}); _temp2819;});} else{ if(
Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2821=( char*)"packed";
struct _tagged_string _temp2822; _temp2822.curr= _temp2821; _temp2822.base=
_temp2821; _temp2822.last_plus_one= _temp2821 + 7; _temp2822;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2823=( char*)"__packed__"; struct
_tagged_string _temp2824; _temp2824.curr= _temp2823; _temp2824.base= _temp2823;
_temp2824.last_plus_one= _temp2823 + 11; _temp2824;})) == 0){ a=( void*) Cyc_Absyn_Packed_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2825=(
char*)"shared"; struct _tagged_string _temp2826; _temp2826.curr= _temp2825;
_temp2826.base= _temp2825; _temp2826.last_plus_one= _temp2825 + 7; _temp2826;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2827=( char*)"__shared__";
struct _tagged_string _temp2828; _temp2828.curr= _temp2827; _temp2828.base=
_temp2827; _temp2828.last_plus_one= _temp2827 + 11; _temp2828;})) == 0){ a=(
void*) Cyc_Absyn_Shared_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2829=( char*)"unused"; struct _tagged_string
_temp2830; _temp2830.curr= _temp2829; _temp2830.base= _temp2829; _temp2830.last_plus_one=
_temp2829 + 7; _temp2830;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2831=( char*)"__unused__"; struct _tagged_string
_temp2832; _temp2832.curr= _temp2831; _temp2832.base= _temp2831; _temp2832.last_plus_one=
_temp2831 + 11; _temp2832;})) == 0){ a=( void*) Cyc_Absyn_Unused_att;} else{ if(
Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2833=( char*)"weak";
struct _tagged_string _temp2834; _temp2834.curr= _temp2833; _temp2834.base=
_temp2833; _temp2834.last_plus_one= _temp2833 + 5; _temp2834;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2835=( char*)"__weak__"; struct
_tagged_string _temp2836; _temp2836.curr= _temp2835; _temp2836.base= _temp2835;
_temp2836.last_plus_one= _temp2835 + 9; _temp2836;})) == 0){ a=( void*) Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2837=(
char*)"dllimport"; struct _tagged_string _temp2838; _temp2838.curr= _temp2837;
_temp2838.base= _temp2837; _temp2838.last_plus_one= _temp2837 + 10; _temp2838;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2839=( char*)"__dllimport__";
struct _tagged_string _temp2840; _temp2840.curr= _temp2839; _temp2840.base=
_temp2839; _temp2840.last_plus_one= _temp2839 + 14; _temp2840;})) == 0){ a=(
void*) Cyc_Absyn_Dllimport_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2841=( char*)"dllexport"; struct _tagged_string
_temp2842; _temp2842.curr= _temp2841; _temp2842.base= _temp2841; _temp2842.last_plus_one=
_temp2841 + 10; _temp2842;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2843=( char*)"__dllexport__"; struct _tagged_string
_temp2844; _temp2844.curr= _temp2843; _temp2844.base= _temp2843; _temp2844.last_plus_one=
_temp2843 + 14; _temp2844;})) == 0){ a=( void*) Cyc_Absyn_Dllexport_att;} else{
if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2845=( char*)"no_instrument_function";
struct _tagged_string _temp2846; _temp2846.curr= _temp2845; _temp2846.base=
_temp2845; _temp2846.last_plus_one= _temp2845 + 23; _temp2846;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2847=( char*)"__no_instrument_function__";
struct _tagged_string _temp2848; _temp2848.curr= _temp2847; _temp2848.base=
_temp2847; _temp2848.last_plus_one= _temp2847 + 27; _temp2848;})) == 0){ a=(
void*) Cyc_Absyn_No_instrument_function_att;} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2849=( char*)"constructor"; struct
_tagged_string _temp2850; _temp2850.curr= _temp2849; _temp2850.base= _temp2849;
_temp2850.last_plus_one= _temp2849 + 12; _temp2850;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2851=( char*)"__constructor__"; struct
_tagged_string _temp2852; _temp2852.curr= _temp2851; _temp2852.base= _temp2851;
_temp2852.last_plus_one= _temp2851 + 16; _temp2852;})) == 0){ a=( void*) Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2853=(
char*)"destructor"; struct _tagged_string _temp2854; _temp2854.curr= _temp2853;
_temp2854.base= _temp2853; _temp2854.last_plus_one= _temp2853 + 11; _temp2854;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2855=( char*)"__destructor__";
struct _tagged_string _temp2856; _temp2856.curr= _temp2855; _temp2856.base=
_temp2855; _temp2856.last_plus_one= _temp2855 + 15; _temp2856;})) == 0){ a=(
void*) Cyc_Absyn_Destructor_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2857=( char*)"no_check_memory_usage"; struct
_tagged_string _temp2858; _temp2858.curr= _temp2857; _temp2858.base= _temp2857;
_temp2858.last_plus_one= _temp2857 + 22; _temp2858;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2859=( char*)"__no_check_memory_usage__";
struct _tagged_string _temp2860; _temp2860.curr= _temp2859; _temp2860.base=
_temp2859; _temp2860.last_plus_one= _temp2859 + 26; _temp2860;})) == 0){ a=(
void*) Cyc_Absyn_No_check_memory_usage_att;} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2861=( char*)"unrecognized attribute"; struct
_tagged_string _temp2862; _temp2862.curr= _temp2861; _temp2862.base= _temp2861;
_temp2862.last_plus_one= _temp2861 + 23; _temp2862;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2863= yyls; struct Cyc_Yyltype* _temp2865= _temp2863.curr
+ yylsp_offset; if( _temp2863.base == 0? 1:( _temp2865 < _temp2863.base? 1:
_temp2865 >= _temp2863.last_plus_one)){ _throw( Null_Exception);}* _temp2865;}).first_line,({
struct _tagged_ptr3 _temp2866= yyls; struct Cyc_Yyltype* _temp2868= _temp2866.curr
+ yylsp_offset; if( _temp2866.base == 0? 1:( _temp2868 < _temp2866.base? 1:
_temp2868 >= _temp2866.last_plus_one)){ _throw( Null_Exception);}* _temp2868;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2869=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2869[ 0]=({ struct Cyc_Attribute_tok_struct _temp2870; _temp2870.tag= Cyc_Attribute_tok;
_temp2870.f1=( void*) a; _temp2870;}); _temp2869;}); break;} case 47: _LL2796:
yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2875=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2875[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2876; _temp2876.tag= Cyc_Attribute_tok; _temp2876.f1=(
void*)(( void*) Cyc_Absyn_Const_att); _temp2876;}); _temp2875;}); break; case 48:
_LL2874: { struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2934= yyvs; void** _temp2936= _temp2934.curr +( yyvsp_offset - 3); if(
_temp2934.base == 0? 1:( _temp2936 < _temp2934.base? 1: _temp2936 >= _temp2934.last_plus_one)){
_throw( Null_Exception);}* _temp2936;})); struct _tuple12 _temp2883; int
_temp2884; void* _temp2886; struct _tuple12* _temp2881= Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp2878= yyvs; void** _temp2880= _temp2878.curr +(
yyvsp_offset - 1); if( _temp2878.base == 0? 1:( _temp2880 < _temp2878.base? 1:
_temp2880 >= _temp2878.last_plus_one)){ _throw( Null_Exception);}* _temp2880;}));
_temp2883=* _temp2881; _LL2887: _temp2886= _temp2883.f1; goto _LL2885; _LL2885:
_temp2884= _temp2883.f2; goto _LL2882; _LL2882: { void* a; if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2888=( char*)"regparm"; struct
_tagged_string _temp2889; _temp2889.curr= _temp2888; _temp2889.base= _temp2888;
_temp2889.last_plus_one= _temp2888 + 8; _temp2889;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2890=( char*)"__regparm__"; struct
_tagged_string _temp2891; _temp2891.curr= _temp2890; _temp2891.base= _temp2890;
_temp2891.last_plus_one= _temp2890 + 12; _temp2891;})) == 0){ if( _temp2884 <= 0?
1: _temp2884 > 3){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2892=(
char*)"regparm requires value between 1 and 3"; struct _tagged_string _temp2893;
_temp2893.curr= _temp2892; _temp2893.base= _temp2892; _temp2893.last_plus_one=
_temp2892 + 39; _temp2893;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2894= yyls; struct Cyc_Yyltype* _temp2896= _temp2894.curr +( yylsp_offset -
1); if( _temp2894.base == 0? 1:( _temp2896 < _temp2894.base? 1: _temp2896 >=
_temp2894.last_plus_one)){ _throw( Null_Exception);}* _temp2896;}).first_line,({
struct _tagged_ptr3 _temp2897= yyls; struct Cyc_Yyltype* _temp2899= _temp2897.curr
+( yylsp_offset - 1); if( _temp2897.base == 0? 1:( _temp2899 < _temp2897.base? 1:
_temp2899 >= _temp2897.last_plus_one)){ _throw( Null_Exception);}* _temp2899;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2900=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2900[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2901; _temp2901.tag= Cyc_Absyn_Regparm_att;
_temp2901.f1= _temp2884; _temp2901;}); _temp2900;});} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2902=( char*)"aligned"; struct
_tagged_string _temp2903; _temp2903.curr= _temp2902; _temp2903.base= _temp2902;
_temp2903.last_plus_one= _temp2902 + 8; _temp2903;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2904=( char*)"__aligned__"; struct
_tagged_string _temp2905; _temp2905.curr= _temp2904; _temp2905.base= _temp2904;
_temp2905.last_plus_one= _temp2904 + 12; _temp2905;})) == 0){ if( _temp2884 < 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2906=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2907; _temp2907.curr= _temp2906; _temp2907.base=
_temp2906; _temp2907.last_plus_one= _temp2906 + 39; _temp2907;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2908= yyls; struct Cyc_Yyltype* _temp2910= _temp2908.curr
+( yylsp_offset - 1); if( _temp2908.base == 0? 1:( _temp2910 < _temp2908.base? 1:
_temp2910 >= _temp2908.last_plus_one)){ _throw( Null_Exception);}* _temp2910;}).first_line,({
struct _tagged_ptr3 _temp2911= yyls; struct Cyc_Yyltype* _temp2913= _temp2911.curr
+( yylsp_offset - 1); if( _temp2911.base == 0? 1:( _temp2913 < _temp2911.base? 1:
_temp2913 >= _temp2911.last_plus_one)){ _throw( Null_Exception);}* _temp2913;}).last_line));}{
unsigned int j=( unsigned int) _temp2884; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2914=(
char*)"aligned requires positive power of two"; struct _tagged_string _temp2915;
_temp2915.curr= _temp2914; _temp2915.base= _temp2914; _temp2915.last_plus_one=
_temp2914 + 39; _temp2915;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2916= yyls; struct Cyc_Yyltype* _temp2918= _temp2916.curr +( yylsp_offset -
1); if( _temp2916.base == 0? 1:( _temp2918 < _temp2916.base? 1: _temp2918 >=
_temp2916.last_plus_one)){ _throw( Null_Exception);}* _temp2918;}).first_line,({
struct _tagged_ptr3 _temp2919= yyls; struct Cyc_Yyltype* _temp2921= _temp2919.curr
+( yylsp_offset - 1); if( _temp2919.base == 0? 1:( _temp2921 < _temp2919.base? 1:
_temp2921 >= _temp2919.last_plus_one)){ _throw( Null_Exception);}* _temp2921;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2922=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2922[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2923; _temp2923.tag= Cyc_Absyn_Aligned_att;
_temp2923.f1= _temp2884; _temp2923;}); _temp2922;});}} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2924=( char*)"unrecognized attribute";
struct _tagged_string _temp2925; _temp2925.curr= _temp2924; _temp2925.base=
_temp2924; _temp2925.last_plus_one= _temp2924 + 23; _temp2925;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2926= yyls; struct Cyc_Yyltype* _temp2928= _temp2926.curr
+( yylsp_offset - 3); if( _temp2926.base == 0? 1:( _temp2928 < _temp2926.base? 1:
_temp2928 >= _temp2926.last_plus_one)){ _throw( Null_Exception);}* _temp2928;}).first_line,({
struct _tagged_ptr3 _temp2929= yyls; struct Cyc_Yyltype* _temp2931= _temp2929.curr
+( yylsp_offset - 3); if( _temp2929.base == 0? 1:( _temp2931 < _temp2929.base? 1:
_temp2931 >= _temp2929.last_plus_one)){ _throw( Null_Exception);}* _temp2931;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2932=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2932[ 0]=({ struct Cyc_Attribute_tok_struct _temp2933; _temp2933.tag= Cyc_Attribute_tok;
_temp2933.f1=( void*) a; _temp2933;}); _temp2932;}); break;}} case 49: _LL2877: {
struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2957=
yyvs; void** _temp2959= _temp2957.curr +( yyvsp_offset - 3); if( _temp2957.base
== 0? 1:( _temp2959 < _temp2957.base? 1: _temp2959 >= _temp2957.last_plus_one)){
_throw( Null_Exception);}* _temp2959;})); struct _tagged_string str= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2954= yyvs; void** _temp2956= _temp2954.curr +(
yyvsp_offset - 1); if( _temp2954.base == 0? 1:( _temp2956 < _temp2954.base? 1:
_temp2956 >= _temp2954.last_plus_one)){ _throw( Null_Exception);}* _temp2956;}));
void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2938=(
char*)"section"; struct _tagged_string _temp2939; _temp2939.curr= _temp2938;
_temp2939.base= _temp2938; _temp2939.last_plus_one= _temp2938 + 8; _temp2939;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2940=( char*)"__section__";
struct _tagged_string _temp2941; _temp2941.curr= _temp2940; _temp2941.base=
_temp2940; _temp2941.last_plus_one= _temp2940 + 12; _temp2941;}))){ a=( void*)({
struct Cyc_Absyn_Section_att_struct* _temp2942=( struct Cyc_Absyn_Section_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct)); _temp2942[ 0]=({
struct Cyc_Absyn_Section_att_struct _temp2943; _temp2943.tag= Cyc_Absyn_Section_att;
_temp2943.f1= str; _temp2943;}); _temp2942;});} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2944=( char*)"unrecognized attribute"; struct
_tagged_string _temp2945; _temp2945.curr= _temp2944; _temp2945.base= _temp2944;
_temp2945.last_plus_one= _temp2944 + 23; _temp2945;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2946= yyls; struct Cyc_Yyltype* _temp2948= _temp2946.curr
+( yylsp_offset - 3); if( _temp2946.base == 0? 1:( _temp2948 < _temp2946.base? 1:
_temp2948 >= _temp2946.last_plus_one)){ _throw( Null_Exception);}* _temp2948;}).first_line,({
struct _tagged_ptr3 _temp2949= yyls; struct Cyc_Yyltype* _temp2951= _temp2949.curr
+( yylsp_offset - 3); if( _temp2949.base == 0? 1:( _temp2951 < _temp2949.base? 1:
_temp2951 >= _temp2949.last_plus_one)){ _throw( Null_Exception);}* _temp2951;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2952=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2952[ 0]=({ struct Cyc_Attribute_tok_struct _temp2953; _temp2953.tag= Cyc_Attribute_tok;
_temp2953.f1=( void*) a; _temp2953;}); _temp2952;}); break;} case 50: _LL2937:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2961=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2961[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2962; _temp2962.tag= Cyc_TypeSpecifier_tok;
_temp2962.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2963= yyls; struct Cyc_Yyltype* _temp2965= _temp2963.curr
+ yylsp_offset; if( _temp2963.base == 0? 1:( _temp2965 < _temp2963.base? 1:
_temp2965 >= _temp2963.last_plus_one)){ _throw( Null_Exception);}* _temp2965;}).first_line,({
struct _tagged_ptr3 _temp2966= yyls; struct Cyc_Yyltype* _temp2968= _temp2966.curr
+ yylsp_offset; if( _temp2966.base == 0? 1:( _temp2968 < _temp2966.base? 1:
_temp2968 >= _temp2966.last_plus_one)){ _throw( Null_Exception);}* _temp2968;}).last_line));
_temp2962;}); _temp2961;}); break; case 51: _LL2960: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2970=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2970[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2971; _temp2971.tag=
Cyc_TypeSpecifier_tok; _temp2971.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2972= yyls; struct Cyc_Yyltype* _temp2974= _temp2972.curr + yylsp_offset;
if( _temp2972.base == 0? 1:( _temp2974 < _temp2972.base? 1: _temp2974 >=
_temp2972.last_plus_one)){ _throw( Null_Exception);}* _temp2974;}).first_line,({
struct _tagged_ptr3 _temp2975= yyls; struct Cyc_Yyltype* _temp2977= _temp2975.curr
+ yylsp_offset; if( _temp2975.base == 0? 1:( _temp2977 < _temp2975.base? 1:
_temp2977 >= _temp2975.last_plus_one)){ _throw( Null_Exception);}* _temp2977;}).last_line));
_temp2971;}); _temp2970;}); break; case 52: _LL2969: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2979=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2979[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2980; _temp2980.tag=
Cyc_TypeSpecifier_tok; _temp2980.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2981= yyls; struct Cyc_Yyltype*
_temp2983= _temp2981.curr + yylsp_offset; if( _temp2981.base == 0? 1:( _temp2983
< _temp2981.base? 1: _temp2983 >= _temp2981.last_plus_one)){ _throw(
Null_Exception);}* _temp2983;}).first_line,({ struct _tagged_ptr3 _temp2984=
yyls; struct Cyc_Yyltype* _temp2986= _temp2984.curr + yylsp_offset; if(
_temp2984.base == 0? 1:( _temp2986 < _temp2984.base? 1: _temp2986 >= _temp2984.last_plus_one)){
_throw( Null_Exception);}* _temp2986;}).last_line)); _temp2980;}); _temp2979;});
break; case 53: _LL2978: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2988=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2988[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2989; _temp2989.tag=
Cyc_TypeSpecifier_tok; _temp2989.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp2990=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2990[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2991; _temp2991.tag=
Cyc_Parse_Short_spec; _temp2991.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2992= yyls; struct Cyc_Yyltype* _temp2994= _temp2992.curr +
yylsp_offset; if( _temp2992.base == 0? 1:( _temp2994 < _temp2992.base? 1:
_temp2994 >= _temp2992.last_plus_one)){ _throw( Null_Exception);}* _temp2994;}).first_line,({
struct _tagged_ptr3 _temp2995= yyls; struct Cyc_Yyltype* _temp2997= _temp2995.curr
+ yylsp_offset; if( _temp2995.base == 0? 1:( _temp2997 < _temp2995.base? 1:
_temp2997 >= _temp2995.last_plus_one)){ _throw( Null_Exception);}* _temp2997;}).last_line);
_temp2991;}); _temp2990;})); _temp2989;}); _temp2988;}); break; case 54: _LL2987:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2999=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2999[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3000; _temp3000.tag= Cyc_TypeSpecifier_tok;
_temp3000.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3001= yyls; struct Cyc_Yyltype* _temp3003= _temp3001.curr
+ yylsp_offset; if( _temp3001.base == 0? 1:( _temp3003 < _temp3001.base? 1:
_temp3003 >= _temp3001.last_plus_one)){ _throw( Null_Exception);}* _temp3003;}).first_line,({
struct _tagged_ptr3 _temp3004= yyls; struct Cyc_Yyltype* _temp3006= _temp3004.curr
+ yylsp_offset; if( _temp3004.base == 0? 1:( _temp3006 < _temp3004.base? 1:
_temp3006 >= _temp3004.last_plus_one)){ _throw( Null_Exception);}* _temp3006;}).last_line));
_temp3000;}); _temp2999;}); break; case 55: _LL2998: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3008=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3008[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3009; _temp3009.tag=
Cyc_TypeSpecifier_tok; _temp3009.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp3010=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp3010[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp3011; _temp3011.tag= Cyc_Parse_Long_spec;
_temp3011.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3012= yyls;
struct Cyc_Yyltype* _temp3014= _temp3012.curr + yylsp_offset; if( _temp3012.base
== 0? 1:( _temp3014 < _temp3012.base? 1: _temp3014 >= _temp3012.last_plus_one)){
_throw( Null_Exception);}* _temp3014;}).first_line,({ struct _tagged_ptr3
_temp3015= yyls; struct Cyc_Yyltype* _temp3017= _temp3015.curr + yylsp_offset;
if( _temp3015.base == 0? 1:( _temp3017 < _temp3015.base? 1: _temp3017 >=
_temp3015.last_plus_one)){ _throw( Null_Exception);}* _temp3017;}).last_line);
_temp3011;}); _temp3010;})); _temp3009;}); _temp3008;}); break; case 56: _LL3007:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3019=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3019[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3020; _temp3020.tag= Cyc_TypeSpecifier_tok;
_temp3020.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3021= yyls; struct Cyc_Yyltype* _temp3023= _temp3021.curr
+ yylsp_offset; if( _temp3021.base == 0? 1:( _temp3023 < _temp3021.base? 1:
_temp3023 >= _temp3021.last_plus_one)){ _throw( Null_Exception);}* _temp3023;}).first_line,({
struct _tagged_ptr3 _temp3024= yyls; struct Cyc_Yyltype* _temp3026= _temp3024.curr
+ yylsp_offset; if( _temp3024.base == 0? 1:( _temp3026 < _temp3024.base? 1:
_temp3026 >= _temp3024.last_plus_one)){ _throw( Null_Exception);}* _temp3026;}).last_line));
_temp3020;}); _temp3019;}); break; case 57: _LL3018: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3028=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3028[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3029; _temp3029.tag=
Cyc_TypeSpecifier_tok; _temp3029.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3030= yyls; struct Cyc_Yyltype*
_temp3032= _temp3030.curr + yylsp_offset; if( _temp3030.base == 0? 1:( _temp3032
< _temp3030.base? 1: _temp3032 >= _temp3030.last_plus_one)){ _throw(
Null_Exception);}* _temp3032;}).first_line,({ struct _tagged_ptr3 _temp3033=
yyls; struct Cyc_Yyltype* _temp3035= _temp3033.curr + yylsp_offset; if(
_temp3033.base == 0? 1:( _temp3035 < _temp3033.base? 1: _temp3035 >= _temp3033.last_plus_one)){
_throw( Null_Exception);}* _temp3035;}).last_line)); _temp3029;}); _temp3028;});
break; case 58: _LL3027: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3037=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3037[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3038; _temp3038.tag=
Cyc_TypeSpecifier_tok; _temp3038.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp3039=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp3039[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp3040; _temp3040.tag=
Cyc_Parse_Signed_spec; _temp3040.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3041= yyls; struct Cyc_Yyltype* _temp3043= _temp3041.curr +
yylsp_offset; if( _temp3041.base == 0? 1:( _temp3043 < _temp3041.base? 1:
_temp3043 >= _temp3041.last_plus_one)){ _throw( Null_Exception);}* _temp3043;}).first_line,({
struct _tagged_ptr3 _temp3044= yyls; struct Cyc_Yyltype* _temp3046= _temp3044.curr
+ yylsp_offset; if( _temp3044.base == 0? 1:( _temp3046 < _temp3044.base? 1:
_temp3046 >= _temp3044.last_plus_one)){ _throw( Null_Exception);}* _temp3046;}).last_line);
_temp3040;}); _temp3039;})); _temp3038;}); _temp3037;}); break; case 59: _LL3036:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3048=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3048[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3049; _temp3049.tag= Cyc_TypeSpecifier_tok;
_temp3049.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp3050=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp3050[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp3051; _temp3051.tag= Cyc_Parse_Unsigned_spec; _temp3051.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3052= yyls; struct Cyc_Yyltype* _temp3054= _temp3052.curr
+ yylsp_offset; if( _temp3052.base == 0? 1:( _temp3054 < _temp3052.base? 1:
_temp3054 >= _temp3052.last_plus_one)){ _throw( Null_Exception);}* _temp3054;}).first_line,({
struct _tagged_ptr3 _temp3055= yyls; struct Cyc_Yyltype* _temp3057= _temp3055.curr
+ yylsp_offset; if( _temp3055.base == 0? 1:( _temp3057 < _temp3055.base? 1:
_temp3057 >= _temp3055.last_plus_one)){ _throw( Null_Exception);}* _temp3057;}).last_line);
_temp3051;}); _temp3050;})); _temp3049;}); _temp3048;}); break; case 60: _LL3047:
yyval=({ struct _tagged_ptr2 _temp3059= yyvs; void** _temp3061= _temp3059.curr +
yyvsp_offset; if( _temp3059.base == 0? 1:( _temp3061 < _temp3059.base? 1:
_temp3061 >= _temp3059.last_plus_one)){ _throw( Null_Exception);}* _temp3061;});
break; case 61: _LL3058: yyval=({ struct _tagged_ptr2 _temp3063= yyvs; void**
_temp3065= _temp3063.curr + yyvsp_offset; if( _temp3063.base == 0? 1:( _temp3065
< _temp3063.base? 1: _temp3065 >= _temp3063.last_plus_one)){ _throw(
Null_Exception);}* _temp3065;}); break; case 62: _LL3062: yyval=({ struct
_tagged_ptr2 _temp3067= yyvs; void** _temp3069= _temp3067.curr + yyvsp_offset;
if( _temp3067.base == 0? 1:( _temp3069 < _temp3067.base? 1: _temp3069 >=
_temp3067.last_plus_one)){ _throw( Null_Exception);}* _temp3069;}); break; case
63: _LL3066: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3071=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3071[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3072; _temp3072.tag=
Cyc_TypeSpecifier_tok; _temp3072.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3073= yyvs; void** _temp3075=
_temp3073.curr + yyvsp_offset; if( _temp3073.base == 0? 1:( _temp3075 <
_temp3073.base? 1: _temp3075 >= _temp3073.last_plus_one)){ _throw(
Null_Exception);}* _temp3075;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3076= yyls; struct Cyc_Yyltype*
_temp3078= _temp3076.curr + yylsp_offset; if( _temp3076.base == 0? 1:( _temp3078
< _temp3076.base? 1: _temp3078 >= _temp3076.last_plus_one)){ _throw(
Null_Exception);}* _temp3078;}).first_line,({ struct _tagged_ptr3 _temp3079=
yyls; struct Cyc_Yyltype* _temp3081= _temp3079.curr + yylsp_offset; if(
_temp3079.base == 0? 1:( _temp3081 < _temp3079.base? 1: _temp3081 >= _temp3079.last_plus_one)){
_throw( Null_Exception);}* _temp3081;}).last_line)); _temp3072;}); _temp3071;});
break; case 64: _LL3070: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3083=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3083[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3084; _temp3084.tag=
Cyc_TypeSpecifier_tok; _temp3084.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3085= yyvs; void** _temp3087=
_temp3085.curr +( yyvsp_offset - 2); if( _temp3085.base == 0? 1:( _temp3087 <
_temp3085.base? 1: _temp3087 >= _temp3085.last_plus_one)){ _throw(
Null_Exception);}* _temp3087;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp3088= yyvs; void** _temp3090=
_temp3088.curr + yyvsp_offset; if( _temp3088.base == 0? 1:( _temp3090 <
_temp3088.base? 1: _temp3090 >= _temp3088.last_plus_one)){ _throw(
Null_Exception);}* _temp3090;})))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3091= yyls; struct Cyc_Yyltype* _temp3093= _temp3091.curr +(
yylsp_offset - 2); if( _temp3091.base == 0? 1:( _temp3093 < _temp3091.base? 1:
_temp3093 >= _temp3091.last_plus_one)){ _throw( Null_Exception);}* _temp3093;}).first_line,({
struct _tagged_ptr3 _temp3094= yyls; struct Cyc_Yyltype* _temp3096= _temp3094.curr
+ yylsp_offset; if( _temp3094.base == 0? 1:( _temp3096 < _temp3094.base? 1:
_temp3096 >= _temp3094.last_plus_one)){ _throw( Null_Exception);}* _temp3096;}).last_line));
_temp3084;}); _temp3083;}); break; case 65: _LL3082: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3098=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3098[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3099; _temp3099.tag=
Cyc_TypeSpecifier_tok; _temp3099.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp3100=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp3100[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp3101; _temp3101.tag= Cyc_Absyn_TypedefType;
_temp3101.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3102= yyvs; void**
_temp3104= _temp3102.curr +( yyvsp_offset - 1); if( _temp3102.base == 0? 1:(
_temp3104 < _temp3102.base? 1: _temp3104 >= _temp3102.last_plus_one)){ _throw(
Null_Exception);}* _temp3104;})); _temp3101.f2= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3105= yyvs; void** _temp3107= _temp3105.curr + yyvsp_offset;
if( _temp3105.base == 0? 1:( _temp3107 < _temp3105.base? 1: _temp3107 >=
_temp3105.last_plus_one)){ _throw( Null_Exception);}* _temp3107;})); _temp3101.f3=
0; _temp3101;}); _temp3100;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3108= yyls; struct Cyc_Yyltype* _temp3110= _temp3108.curr +(
yylsp_offset - 1); if( _temp3108.base == 0? 1:( _temp3110 < _temp3108.base? 1:
_temp3110 >= _temp3108.last_plus_one)){ _throw( Null_Exception);}* _temp3110;}).first_line,({
struct _tagged_ptr3 _temp3111= yyls; struct Cyc_Yyltype* _temp3113= _temp3111.curr
+ yylsp_offset; if( _temp3111.base == 0? 1:( _temp3113 < _temp3111.base? 1:
_temp3113 >= _temp3111.last_plus_one)){ _throw( Null_Exception);}* _temp3113;}).last_line));
_temp3099;}); _temp3098;}); break; case 66: _LL3097: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3115=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3115[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3116; _temp3116.tag=
Cyc_TypeSpecifier_tok; _temp3116.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp3117=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp3117[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp3118; _temp3118.tag= Cyc_Absyn_TupleType;
_temp3118.f1=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3119= yyls; struct Cyc_Yyltype* _temp3121= _temp3119.curr +(
yylsp_offset - 1); if( _temp3119.base == 0? 1:( _temp3121 < _temp3119.base? 1:
_temp3121 >= _temp3119.last_plus_one)){ _throw( Null_Exception);}* _temp3121;}).first_line,({
struct _tagged_ptr3 _temp3122= yyls; struct Cyc_Yyltype* _temp3124= _temp3122.curr
+( yylsp_offset - 1); if( _temp3122.base == 0? 1:( _temp3124 < _temp3122.base? 1:
_temp3124 >= _temp3122.last_plus_one)){ _throw( Null_Exception);}* _temp3124;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3125= yyvs; void** _temp3127= _temp3125.curr +(
yyvsp_offset - 1); if( _temp3125.base == 0? 1:( _temp3127 < _temp3125.base? 1:
_temp3127 >= _temp3125.last_plus_one)){ _throw( Null_Exception);}* _temp3127;}))));
_temp3118;}); _temp3117;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3128= yyls; struct Cyc_Yyltype* _temp3130= _temp3128.curr +( yylsp_offset -
3); if( _temp3128.base == 0? 1:( _temp3130 < _temp3128.base? 1: _temp3130 >=
_temp3128.last_plus_one)){ _throw( Null_Exception);}* _temp3130;}).first_line,({
struct _tagged_ptr3 _temp3131= yyls; struct Cyc_Yyltype* _temp3133= _temp3131.curr
+ yylsp_offset; if( _temp3131.base == 0? 1:( _temp3133 < _temp3131.base? 1:
_temp3133 >= _temp3131.last_plus_one)){ _throw( Null_Exception);}* _temp3133;}).last_line));
_temp3116;}); _temp3115;}); break; case 67: _LL3114: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3135=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3135[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3136; _temp3136.tag=
Cyc_TypeSpecifier_tok; _temp3136.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp3137=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp3137[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp3138; _temp3138.tag= Cyc_Absyn_RgnHandleType;
_temp3138.f1=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp3139= yyvs;
void** _temp3141= _temp3139.curr +( yyvsp_offset - 1); if( _temp3139.base == 0?
1:( _temp3141 < _temp3139.base? 1: _temp3141 >= _temp3139.last_plus_one)){
_throw( Null_Exception);}* _temp3141;})); _temp3138;}); _temp3137;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3142= yyls; struct Cyc_Yyltype* _temp3144= _temp3142.curr
+( yylsp_offset - 3); if( _temp3142.base == 0? 1:( _temp3144 < _temp3142.base? 1:
_temp3144 >= _temp3142.last_plus_one)){ _throw( Null_Exception);}* _temp3144;}).first_line,({
struct _tagged_ptr3 _temp3145= yyls; struct Cyc_Yyltype* _temp3147= _temp3145.curr
+ yylsp_offset; if( _temp3145.base == 0? 1:( _temp3147 < _temp3145.base? 1:
_temp3147 >= _temp3145.last_plus_one)){ _throw( Null_Exception);}* _temp3147;}).last_line));
_temp3136;}); _temp3135;}); break; case 68: _LL3134: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp3149=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp3149[ 0]=({ struct Cyc_Kind_tok_struct _temp3150; _temp3150.tag= Cyc_Kind_tok;
_temp3150.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp3151= yyvs; void** _temp3153= _temp3151.curr + yyvsp_offset;
if( _temp3151.base == 0? 1:( _temp3153 < _temp3151.base? 1: _temp3153 >=
_temp3151.last_plus_one)){ _throw( Null_Exception);}* _temp3153;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3154= yyls; struct Cyc_Yyltype* _temp3156= _temp3154.curr
+ yylsp_offset; if( _temp3154.base == 0? 1:( _temp3156 < _temp3154.base? 1:
_temp3156 >= _temp3154.last_plus_one)){ _throw( Null_Exception);}* _temp3156;}).first_line,({
struct _tagged_ptr3 _temp3157= yyls; struct Cyc_Yyltype* _temp3159= _temp3157.curr
+ yylsp_offset; if( _temp3157.base == 0? 1:( _temp3159 < _temp3157.base? 1:
_temp3159 >= _temp3157.last_plus_one)){ _throw( Null_Exception);}* _temp3159;}).last_line));
_temp3150;}); _temp3149;}); break; case 69: _LL3148: { struct _tagged_string*
_temp3166; void* _temp3168; struct _tuple1 _temp3164=* Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3161= yyvs; void** _temp3163= _temp3161.curr +
yyvsp_offset; if( _temp3161.base == 0? 1:( _temp3163 < _temp3161.base? 1:
_temp3163 >= _temp3161.last_plus_one)){ _throw( Null_Exception);}* _temp3163;}));
_LL3169: _temp3168= _temp3164.f1; goto _LL3167; _LL3167: _temp3166= _temp3164.f2;
goto _LL3165; _LL3165: if( _temp3168 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3170=( char*)"bad kind in type specifier";
struct _tagged_string _temp3171; _temp3171.curr= _temp3170; _temp3171.base=
_temp3170; _temp3171.last_plus_one= _temp3170 + 27; _temp3171;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3172= yyls; struct Cyc_Yyltype* _temp3174= _temp3172.curr
+ yylsp_offset; if( _temp3172.base == 0? 1:( _temp3174 < _temp3172.base? 1:
_temp3174 >= _temp3172.last_plus_one)){ _throw( Null_Exception);}* _temp3174;}).first_line,({
struct _tagged_ptr3 _temp3175= yyls; struct Cyc_Yyltype* _temp3177= _temp3175.curr
+ yylsp_offset; if( _temp3175.base == 0? 1:( _temp3177 < _temp3175.base? 1:
_temp3177 >= _temp3175.last_plus_one)){ _throw( Null_Exception);}* _temp3177;}).last_line));}
yyval=( void*)({ struct Cyc_Kind_tok_struct* _temp3178=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp3178[ 0]=({ struct Cyc_Kind_tok_struct
_temp3179; _temp3179.tag= Cyc_Kind_tok; _temp3179.f1=( void*) Cyc_Parse_id_to_kind(*
_temp3166, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3180= yyls;
struct Cyc_Yyltype* _temp3182= _temp3180.curr + yylsp_offset; if( _temp3180.base
== 0? 1:( _temp3182 < _temp3180.base? 1: _temp3182 >= _temp3180.last_plus_one)){
_throw( Null_Exception);}* _temp3182;}).first_line,({ struct _tagged_ptr3
_temp3183= yyls; struct Cyc_Yyltype* _temp3185= _temp3183.curr + yylsp_offset;
if( _temp3183.base == 0? 1:( _temp3185 < _temp3183.base? 1: _temp3185 >=
_temp3183.last_plus_one)){ _throw( Null_Exception);}* _temp3185;}).last_line));
_temp3179;}); _temp3178;}); break;} case 70: _LL3160: yyval=( void*)({ struct
Cyc_TypeQual_tok_struct* _temp3187=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct)); _temp3187[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3188; _temp3188.tag= Cyc_TypeQual_tok; _temp3188.f1=({ struct Cyc_Absyn_Tqual
_temp3189; _temp3189.q_const= 1; _temp3189.q_volatile= 0; _temp3189.q_restrict=
0; _temp3189;}); _temp3188;}); _temp3187;}); break; case 71: _LL3186: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp3191=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3191[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3192; _temp3192.tag= Cyc_TypeQual_tok; _temp3192.f1=({ struct Cyc_Absyn_Tqual
_temp3193; _temp3193.q_const= 0; _temp3193.q_volatile= 1; _temp3193.q_restrict=
0; _temp3193;}); _temp3192;}); _temp3191;}); break; case 72: _LL3190: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp3195=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3195[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3196; _temp3196.tag= Cyc_TypeQual_tok; _temp3196.f1=({ struct Cyc_Absyn_Tqual
_temp3197; _temp3197.q_const= 0; _temp3197.q_volatile= 0; _temp3197.q_restrict=
1; _temp3197;}); _temp3196;}); _temp3195;}); break; case 73: _LL3194: yyval=(
void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3199=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3199[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3200; _temp3200.tag= Cyc_TypeSpecifier_tok;
_temp3200.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3201=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3201[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3202; _temp3202.tag= Cyc_Parse_Decl_spec;
_temp3202.f1=({ struct Cyc_Absyn_Decl* _temp3203=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp3203->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp3210=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp3210[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp3211; _temp3211.tag= Cyc_Absyn_Enum_d; _temp3211.f1=({ struct Cyc_Absyn_Enumdecl*
_temp3212=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp3212->sc=( void*)(( void*) Cyc_Absyn_Public); _temp3212->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3216= yyvs; void** _temp3218= _temp3216.curr +(
yyvsp_offset - 3); if( _temp3216.base == 0? 1:( _temp3218 < _temp3216.base? 1:
_temp3218 >= _temp3216.last_plus_one)){ _throw( Null_Exception);}* _temp3218;}));
_temp3212->fields= Cyc_yyget_EnumfieldList_tok(({ struct _tagged_ptr2 _temp3213=
yyvs; void** _temp3215= _temp3213.curr +( yyvsp_offset - 1); if( _temp3213.base
== 0? 1:( _temp3215 < _temp3213.base? 1: _temp3215 >= _temp3213.last_plus_one)){
_throw( Null_Exception);}* _temp3215;})); _temp3212;}); _temp3211;}); _temp3210;}));
_temp3203->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3204=
yyls; struct Cyc_Yyltype* _temp3206= _temp3204.curr +( yylsp_offset - 4); if(
_temp3204.base == 0? 1:( _temp3206 < _temp3204.base? 1: _temp3206 >= _temp3204.last_plus_one)){
_throw( Null_Exception);}* _temp3206;}).first_line,({ struct _tagged_ptr3
_temp3207= yyls; struct Cyc_Yyltype* _temp3209= _temp3207.curr + yylsp_offset;
if( _temp3207.base == 0? 1:( _temp3209 < _temp3207.base? 1: _temp3209 >=
_temp3207.last_plus_one)){ _throw( Null_Exception);}* _temp3209;}).last_line);
_temp3203;}); _temp3202;}); _temp3201;})); _temp3200;}); _temp3199;}); break;
case 74: _LL3198: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3220=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3220[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3221; _temp3221.tag=
Cyc_TypeSpecifier_tok; _temp3221.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp3222=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3222[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp3223; _temp3223.tag= Cyc_Absyn_EnumType;
_temp3223.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3224= yyvs; void**
_temp3226= _temp3224.curr + yyvsp_offset; if( _temp3224.base == 0? 1:( _temp3226
< _temp3224.base? 1: _temp3226 >= _temp3224.last_plus_one)){ _throw(
Null_Exception);}* _temp3226;})); _temp3223.f2= 0; _temp3223;}); _temp3222;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3227= yyls; struct Cyc_Yyltype*
_temp3229= _temp3227.curr +( yylsp_offset - 1); if( _temp3227.base == 0? 1:(
_temp3229 < _temp3227.base? 1: _temp3229 >= _temp3227.last_plus_one)){ _throw(
Null_Exception);}* _temp3229;}).first_line,({ struct _tagged_ptr3 _temp3230=
yyls; struct Cyc_Yyltype* _temp3232= _temp3230.curr + yylsp_offset; if(
_temp3230.base == 0? 1:( _temp3232 < _temp3230.base? 1: _temp3232 >= _temp3230.last_plus_one)){
_throw( Null_Exception);}* _temp3232;}).last_line)); _temp3221;}); _temp3220;});
break; case 75: _LL3219: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp3234=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp3234[ 0]=({ struct Cyc_Enumfield_tok_struct _temp3235; _temp3235.tag= Cyc_Enumfield_tok;
_temp3235.f1=({ struct Cyc_Absyn_Enumfield* _temp3236=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3236->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3243= yyvs; void** _temp3245= _temp3243.curr +
yyvsp_offset; if( _temp3243.base == 0? 1:( _temp3245 < _temp3243.base? 1:
_temp3245 >= _temp3243.last_plus_one)){ _throw( Null_Exception);}* _temp3245;}));
_temp3236->tag= 0; _temp3236->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3237= yyls; struct Cyc_Yyltype* _temp3239= _temp3237.curr +
yylsp_offset; if( _temp3237.base == 0? 1:( _temp3239 < _temp3237.base? 1:
_temp3239 >= _temp3237.last_plus_one)){ _throw( Null_Exception);}* _temp3239;}).first_line,({
struct _tagged_ptr3 _temp3240= yyls; struct Cyc_Yyltype* _temp3242= _temp3240.curr
+ yylsp_offset; if( _temp3240.base == 0? 1:( _temp3242 < _temp3240.base? 1:
_temp3242 >= _temp3240.last_plus_one)){ _throw( Null_Exception);}* _temp3242;}).last_line);
_temp3236;}); _temp3235;}); _temp3234;}); break; case 76: _LL3233: yyval=( void*)({
struct Cyc_Enumfield_tok_struct* _temp3247=( struct Cyc_Enumfield_tok_struct*)
GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp3247[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp3248; _temp3248.tag= Cyc_Enumfield_tok; _temp3248.f1=({
struct Cyc_Absyn_Enumfield* _temp3249=( struct Cyc_Absyn_Enumfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumfield)); _temp3249->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3259= yyvs; void** _temp3261= _temp3259.curr +(
yyvsp_offset - 2); if( _temp3259.base == 0? 1:( _temp3261 < _temp3259.base? 1:
_temp3261 >= _temp3259.last_plus_one)){ _throw( Null_Exception);}* _temp3261;}));
_temp3249->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp3256= yyvs; void** _temp3258= _temp3256.curr + yyvsp_offset; if( _temp3256.base
== 0? 1:( _temp3258 < _temp3256.base? 1: _temp3258 >= _temp3256.last_plus_one)){
_throw( Null_Exception);}* _temp3258;})); _temp3249->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3250= yyls; struct Cyc_Yyltype* _temp3252= _temp3250.curr
+( yylsp_offset - 2); if( _temp3250.base == 0? 1:( _temp3252 < _temp3250.base? 1:
_temp3252 >= _temp3250.last_plus_one)){ _throw( Null_Exception);}* _temp3252;}).first_line,({
struct _tagged_ptr3 _temp3253= yyls; struct Cyc_Yyltype* _temp3255= _temp3253.curr
+ yylsp_offset; if( _temp3253.base == 0? 1:( _temp3255 < _temp3253.base? 1:
_temp3255 >= _temp3253.last_plus_one)){ _throw( Null_Exception);}* _temp3255;}).last_line);
_temp3249;}); _temp3248;}); _temp3247;}); break; case 77: _LL3246: yyval=( void*)({
struct Cyc_EnumfieldList_tok_struct* _temp3263=( struct Cyc_EnumfieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct)); _temp3263[ 0]=({
struct Cyc_EnumfieldList_tok_struct _temp3264; _temp3264.tag= Cyc_EnumfieldList_tok;
_temp3264.f1=({ struct Cyc_List_List* _temp3265=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3265->hd=( void*) Cyc_yyget_Enumfield_tok(({
struct _tagged_ptr2 _temp3266= yyvs; void** _temp3268= _temp3266.curr +
yyvsp_offset; if( _temp3266.base == 0? 1:( _temp3268 < _temp3266.base? 1:
_temp3268 >= _temp3266.last_plus_one)){ _throw( Null_Exception);}* _temp3268;}));
_temp3265->tl= 0; _temp3265;}); _temp3264;}); _temp3263;}); break; case 78:
_LL3262: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct* _temp3270=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp3270[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp3271; _temp3271.tag=
Cyc_EnumfieldList_tok; _temp3271.f1=({ struct Cyc_List_List* _temp3272=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3272->hd=( void*)
Cyc_yyget_Enumfield_tok(({ struct _tagged_ptr2 _temp3276= yyvs; void** _temp3278=
_temp3276.curr +( yyvsp_offset - 2); if( _temp3276.base == 0? 1:( _temp3278 <
_temp3276.base? 1: _temp3278 >= _temp3276.last_plus_one)){ _throw(
Null_Exception);}* _temp3278;})); _temp3272->tl= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3273= yyvs; void** _temp3275= _temp3273.curr +
yyvsp_offset; if( _temp3273.base == 0? 1:( _temp3275 < _temp3273.base? 1:
_temp3275 >= _temp3273.last_plus_one)){ _throw( Null_Exception);}* _temp3275;}));
_temp3272;}); _temp3271;}); _temp3270;}); break; case 79: _LL3269: { struct Cyc_Absyn_Decl*
d;{ void* _temp3283= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2
_temp3280= yyvs; void** _temp3282= _temp3280.curr +( yyvsp_offset - 3); if(
_temp3280.base == 0? 1:( _temp3282 < _temp3280.base? 1: _temp3282 >= _temp3280.last_plus_one)){
_throw( Null_Exception);}* _temp3282;})); _LL3285: if( _temp3283 ==( void*) Cyc_Parse_Struct_su){
goto _LL3286;} else{ goto _LL3287;} _LL3287: if( _temp3283 ==( void*) Cyc_Parse_Union_su){
goto _LL3288;} else{ goto _LL3284;} _LL3286: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public, 0, 0,({ struct Cyc_Core_Opt* _temp3289=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3289->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3290= yyvs; void** _temp3292= _temp3290.curr +(
yyvsp_offset - 1); if( _temp3290.base == 0? 1:( _temp3292 < _temp3290.base? 1:
_temp3292 >= _temp3290.last_plus_one)){ _throw( Null_Exception);}* _temp3292;}));
_temp3289;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3293=
yyls; struct Cyc_Yyltype* _temp3295= _temp3293.curr +( yylsp_offset - 3); if(
_temp3293.base == 0? 1:( _temp3295 < _temp3293.base? 1: _temp3295 >= _temp3293.last_plus_one)){
_throw( Null_Exception);}* _temp3295;}).first_line,({ struct _tagged_ptr3
_temp3296= yyls; struct Cyc_Yyltype* _temp3298= _temp3296.curr + yylsp_offset;
if( _temp3296.base == 0? 1:( _temp3298 < _temp3296.base? 1: _temp3298 >=
_temp3296.last_plus_one)){ _throw( Null_Exception);}* _temp3298;}).last_line));
goto _LL3284; _LL3288: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public, 0, 0,({
struct Cyc_Core_Opt* _temp3299=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3299->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3300= yyvs; void** _temp3302= _temp3300.curr +(
yyvsp_offset - 1); if( _temp3300.base == 0? 1:( _temp3302 < _temp3300.base? 1:
_temp3302 >= _temp3300.last_plus_one)){ _throw( Null_Exception);}* _temp3302;}));
_temp3299;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3303=
yyls; struct Cyc_Yyltype* _temp3305= _temp3303.curr +( yylsp_offset - 3); if(
_temp3303.base == 0? 1:( _temp3305 < _temp3303.base? 1: _temp3305 >= _temp3303.last_plus_one)){
_throw( Null_Exception);}* _temp3305;}).first_line,({ struct _tagged_ptr3
_temp3306= yyls; struct Cyc_Yyltype* _temp3308= _temp3306.curr + yylsp_offset;
if( _temp3306.base == 0? 1:( _temp3308 < _temp3306.base? 1: _temp3308 >=
_temp3306.last_plus_one)){ _throw( Null_Exception);}* _temp3308;}).last_line));
goto _LL3284; _LL3284:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3309=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3309[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3310; _temp3310.tag=
Cyc_TypeSpecifier_tok; _temp3310.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3311=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3311[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3312; _temp3312.tag= Cyc_Parse_Decl_spec;
_temp3312.f1= d; _temp3312;}); _temp3311;})); _temp3310;}); _temp3309;}); Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp3313=( char*)"anonymous structs/unions";
struct _tagged_string _temp3314; _temp3314.curr= _temp3313; _temp3314.base=
_temp3313; _temp3314.last_plus_one= _temp3313 + 25; _temp3314;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3315= yyls; struct Cyc_Yyltype* _temp3317= _temp3315.curr
+( yylsp_offset - 3); if( _temp3315.base == 0? 1:( _temp3317 < _temp3315.base? 1:
_temp3317 >= _temp3315.last_plus_one)){ _throw( Null_Exception);}* _temp3317;}).first_line,({
struct _tagged_ptr3 _temp3318= yyls; struct Cyc_Yyltype* _temp3320= _temp3318.curr
+ yylsp_offset; if( _temp3318.base == 0? 1:( _temp3320 < _temp3318.base? 1:
_temp3320 >= _temp3318.last_plus_one)){ _throw( Null_Exception);}* _temp3320;}).last_line));
break;} case 80: _LL3279: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3363= yyls; struct Cyc_Yyltype* _temp3365= _temp3363.curr
+( yylsp_offset - 3); if( _temp3363.base == 0? 1:( _temp3365 < _temp3363.base? 1:
_temp3365 >= _temp3363.last_plus_one)){ _throw( Null_Exception);}* _temp3365;}).first_line,({
struct _tagged_ptr3 _temp3366= yyls; struct Cyc_Yyltype* _temp3368= _temp3366.curr
+( yylsp_offset - 3); if( _temp3366.base == 0? 1:( _temp3368 < _temp3366.base? 1:
_temp3368 >= _temp3366.last_plus_one)){ _throw( Null_Exception);}* _temp3368;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3369= yyvs; void** _temp3371=
_temp3369.curr +( yyvsp_offset - 3); if( _temp3369.base == 0? 1:( _temp3371 <
_temp3369.base? 1: _temp3371 >= _temp3369.last_plus_one)){ _throw(
Null_Exception);}* _temp3371;}))); struct Cyc_Absyn_Decl* d;{ void* _temp3325=
Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3322= yyvs; void**
_temp3324= _temp3322.curr +( yyvsp_offset - 5); if( _temp3322.base == 0? 1:(
_temp3324 < _temp3322.base? 1: _temp3324 >= _temp3322.last_plus_one)){ _throw(
Null_Exception);}* _temp3324;})); _LL3327: if( _temp3325 ==( void*) Cyc_Parse_Struct_su){
goto _LL3328;} else{ goto _LL3329;} _LL3329: if( _temp3325 ==( void*) Cyc_Parse_Union_su){
goto _LL3330;} else{ goto _LL3326;} _LL3328: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3331=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3331->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3332= yyvs; void** _temp3334= _temp3332.curr +(
yyvsp_offset - 4); if( _temp3332.base == 0? 1:( _temp3334 < _temp3332.base? 1:
_temp3334 >= _temp3332.last_plus_one)){ _throw( Null_Exception);}* _temp3334;}));
_temp3331;}), ts,({ struct Cyc_Core_Opt* _temp3335=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3335->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3336= yyvs; void** _temp3338= _temp3336.curr +(
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
_tagged_ptr2 _temp3346= yyvs; void** _temp3348= _temp3346.curr +( yyvsp_offset -
4); if( _temp3346.base == 0? 1:( _temp3348 < _temp3346.base? 1: _temp3348 >=
_temp3346.last_plus_one)){ _throw( Null_Exception);}* _temp3348;})); _temp3345;}),
ts,({ struct Cyc_Core_Opt* _temp3349=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp3349->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3350= yyvs; void** _temp3352= _temp3350.curr +(
yyvsp_offset - 1); if( _temp3350.base == 0? 1:( _temp3352 < _temp3350.base? 1:
_temp3352 >= _temp3350.last_plus_one)){ _throw( Null_Exception);}* _temp3352;}));
_temp3349;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3353=
yyls; struct Cyc_Yyltype* _temp3355= _temp3353.curr +( yylsp_offset - 5); if(
_temp3353.base == 0? 1:( _temp3355 < _temp3353.base? 1: _temp3355 >= _temp3353.last_plus_one)){
_throw( Null_Exception);}* _temp3355;}).first_line,({ struct _tagged_ptr3
_temp3356= yyls; struct Cyc_Yyltype* _temp3358= _temp3356.curr + yylsp_offset;
if( _temp3356.base == 0? 1:( _temp3358 < _temp3356.base? 1: _temp3358 >=
_temp3356.last_plus_one)){ _throw( Null_Exception);}* _temp3358;}).last_line));
goto _LL3326; _LL3326:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3359=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3359[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3360; _temp3360.tag=
Cyc_TypeSpecifier_tok; _temp3360.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3361=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3361[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3362; _temp3362.tag= Cyc_Parse_Decl_spec;
_temp3362.f1= d; _temp3362;}); _temp3361;})); _temp3360;}); _temp3359;}); break;}
case 81: _LL3321: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3414= yyls; struct Cyc_Yyltype* _temp3416= _temp3414.curr
+( yylsp_offset - 3); if( _temp3414.base == 0? 1:( _temp3416 < _temp3414.base? 1:
_temp3416 >= _temp3414.last_plus_one)){ _throw( Null_Exception);}* _temp3416;}).first_line,({
struct _tagged_ptr3 _temp3417= yyls; struct Cyc_Yyltype* _temp3419= _temp3417.curr
+( yylsp_offset - 3); if( _temp3417.base == 0? 1:( _temp3419 < _temp3417.base? 1:
_temp3419 >= _temp3417.last_plus_one)){ _throw( Null_Exception);}* _temp3419;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3420= yyvs; void** _temp3422=
_temp3420.curr +( yyvsp_offset - 3); if( _temp3420.base == 0? 1:( _temp3422 <
_temp3420.base? 1: _temp3422 >= _temp3420.last_plus_one)){ _throw(
Null_Exception);}* _temp3422;}))); struct Cyc_Absyn_Decl* d;{ void* _temp3376=
Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3373= yyvs; void**
_temp3375= _temp3373.curr +( yyvsp_offset - 5); if( _temp3373.base == 0? 1:(
_temp3375 < _temp3373.base? 1: _temp3375 >= _temp3373.last_plus_one)){ _throw(
Null_Exception);}* _temp3375;})); _LL3378: if( _temp3376 ==( void*) Cyc_Parse_Struct_su){
goto _LL3379;} else{ goto _LL3380;} _LL3380: if( _temp3376 ==( void*) Cyc_Parse_Union_su){
goto _LL3381;} else{ goto _LL3377;} _LL3379: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3382=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3382->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3383= yyvs; void** _temp3385= _temp3383.curr +(
yyvsp_offset - 4); if( _temp3383.base == 0? 1:( _temp3385 < _temp3383.base? 1:
_temp3385 >= _temp3383.last_plus_one)){ _throw( Null_Exception);}* _temp3385;}));
_temp3382;}), ts,({ struct Cyc_Core_Opt* _temp3386=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3386->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3387= yyvs; void** _temp3389= _temp3387.curr +(
yyvsp_offset - 1); if( _temp3387.base == 0? 1:( _temp3389 < _temp3387.base? 1:
_temp3389 >= _temp3387.last_plus_one)){ _throw( Null_Exception);}* _temp3389;}));
_temp3386;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3390=
yyls; struct Cyc_Yyltype* _temp3392= _temp3390.curr +( yylsp_offset - 5); if(
_temp3390.base == 0? 1:( _temp3392 < _temp3390.base? 1: _temp3392 >= _temp3390.last_plus_one)){
_throw( Null_Exception);}* _temp3392;}).first_line,({ struct _tagged_ptr3
_temp3393= yyls; struct Cyc_Yyltype* _temp3395= _temp3393.curr + yylsp_offset;
if( _temp3393.base == 0? 1:( _temp3395 < _temp3393.base? 1: _temp3395 >=
_temp3393.last_plus_one)){ _throw( Null_Exception);}* _temp3395;}).last_line));
goto _LL3377; _LL3381: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3396=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3396->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3397= yyvs; void** _temp3399= _temp3397.curr +( yyvsp_offset -
4); if( _temp3397.base == 0? 1:( _temp3399 < _temp3397.base? 1: _temp3399 >=
_temp3397.last_plus_one)){ _throw( Null_Exception);}* _temp3399;})); _temp3396;}),
ts,({ struct Cyc_Core_Opt* _temp3400=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp3400->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3401= yyvs; void** _temp3403= _temp3401.curr +(
yyvsp_offset - 1); if( _temp3401.base == 0? 1:( _temp3403 < _temp3401.base? 1:
_temp3403 >= _temp3401.last_plus_one)){ _throw( Null_Exception);}* _temp3403;}));
_temp3400;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3404=
yyls; struct Cyc_Yyltype* _temp3406= _temp3404.curr +( yylsp_offset - 5); if(
_temp3404.base == 0? 1:( _temp3406 < _temp3404.base? 1: _temp3406 >= _temp3404.last_plus_one)){
_throw( Null_Exception);}* _temp3406;}).first_line,({ struct _tagged_ptr3
_temp3407= yyls; struct Cyc_Yyltype* _temp3409= _temp3407.curr + yylsp_offset;
if( _temp3407.base == 0? 1:( _temp3409 < _temp3407.base? 1: _temp3409 >=
_temp3407.last_plus_one)){ _throw( Null_Exception);}* _temp3409;}).last_line));
goto _LL3377; _LL3377:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3410=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3410[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3411; _temp3411.tag=
Cyc_TypeSpecifier_tok; _temp3411.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3412=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3412[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3413; _temp3413.tag= Cyc_Parse_Decl_spec;
_temp3413.f1= d; _temp3413;}); _temp3412;})); _temp3411;}); _temp3410;}); break;}
case 82: _LL3372:{ void* _temp3427= Cyc_yyget_StructOrUnion_tok(({ struct
_tagged_ptr2 _temp3424= yyvs; void** _temp3426= _temp3424.curr +( yyvsp_offset -
2); if( _temp3424.base == 0? 1:( _temp3426 < _temp3424.base? 1: _temp3426 >=
_temp3424.last_plus_one)){ _throw( Null_Exception);}* _temp3426;})); _LL3429:
if( _temp3427 ==( void*) Cyc_Parse_Struct_su){ goto _LL3430;} else{ goto _LL3431;}
_LL3431: if( _temp3427 ==( void*) Cyc_Parse_Union_su){ goto _LL3432;} else{ goto
_LL3428;} _LL3430: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3433=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3433[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3434; _temp3434.tag=
Cyc_TypeSpecifier_tok; _temp3434.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3435=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3435[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3436; _temp3436.tag= Cyc_Absyn_StructType;
_temp3436.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3437= yyvs; void** _temp3439= _temp3437.curr +( yyvsp_offset - 1); if(
_temp3437.base == 0? 1:( _temp3439 < _temp3437.base? 1: _temp3439 >= _temp3437.last_plus_one)){
_throw( Null_Exception);}* _temp3439;})); _temp3436.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3440= yyvs; void** _temp3442= _temp3440.curr +
yyvsp_offset; if( _temp3440.base == 0? 1:( _temp3442 < _temp3440.base? 1:
_temp3442 >= _temp3440.last_plus_one)){ _throw( Null_Exception);}* _temp3442;}));
_temp3436.f3= 0; _temp3436;}); _temp3435;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3443= yyls; struct Cyc_Yyltype* _temp3445= _temp3443.curr
+( yylsp_offset - 2); if( _temp3443.base == 0? 1:( _temp3445 < _temp3443.base? 1:
_temp3445 >= _temp3443.last_plus_one)){ _throw( Null_Exception);}* _temp3445;}).first_line,({
struct _tagged_ptr3 _temp3446= yyls; struct Cyc_Yyltype* _temp3448= _temp3446.curr
+ yylsp_offset; if( _temp3446.base == 0? 1:( _temp3448 < _temp3446.base? 1:
_temp3448 >= _temp3446.last_plus_one)){ _throw( Null_Exception);}* _temp3448;}).last_line));
_temp3434;}); _temp3433;}); goto _LL3428; _LL3432: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3449=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3449[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3450; _temp3450.tag=
Cyc_TypeSpecifier_tok; _temp3450.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp3451=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3451[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp3452; _temp3452.tag= Cyc_Absyn_UnionType;
_temp3452.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3453= yyvs; void** _temp3455= _temp3453.curr +( yyvsp_offset - 1); if(
_temp3453.base == 0? 1:( _temp3455 < _temp3453.base? 1: _temp3455 >= _temp3453.last_plus_one)){
_throw( Null_Exception);}* _temp3455;})); _temp3452.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3456= yyvs; void** _temp3458= _temp3456.curr +
yyvsp_offset; if( _temp3456.base == 0? 1:( _temp3458 < _temp3456.base? 1:
_temp3458 >= _temp3456.last_plus_one)){ _throw( Null_Exception);}* _temp3458;}));
_temp3452.f3= 0; _temp3452;}); _temp3451;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3459= yyls; struct Cyc_Yyltype* _temp3461= _temp3459.curr
+( yylsp_offset - 2); if( _temp3459.base == 0? 1:( _temp3461 < _temp3459.base? 1:
_temp3461 >= _temp3459.last_plus_one)){ _throw( Null_Exception);}* _temp3461;}).first_line,({
struct _tagged_ptr3 _temp3462= yyls; struct Cyc_Yyltype* _temp3464= _temp3462.curr
+ yylsp_offset; if( _temp3462.base == 0? 1:( _temp3464 < _temp3462.base? 1:
_temp3464 >= _temp3462.last_plus_one)){ _throw( Null_Exception);}* _temp3464;}).last_line));
_temp3450;}); _temp3449;}); goto _LL3428; _LL3428:;} break; case 83: _LL3423:{
void* _temp3469= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3466=
yyvs; void** _temp3468= _temp3466.curr +( yyvsp_offset - 2); if( _temp3466.base
== 0? 1:( _temp3468 < _temp3466.base? 1: _temp3468 >= _temp3466.last_plus_one)){
_throw( Null_Exception);}* _temp3468;})); _LL3471: if( _temp3469 ==( void*) Cyc_Parse_Struct_su){
goto _LL3472;} else{ goto _LL3473;} _LL3473: if( _temp3469 ==( void*) Cyc_Parse_Union_su){
goto _LL3474;} else{ goto _LL3470;} _LL3472: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3475=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3475[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3476; _temp3476.tag=
Cyc_TypeSpecifier_tok; _temp3476.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3477=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3477[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3478; _temp3478.tag= Cyc_Absyn_StructType;
_temp3478.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3479= yyvs; void** _temp3481= _temp3479.curr +( yyvsp_offset - 1); if(
_temp3479.base == 0? 1:( _temp3481 < _temp3479.base? 1: _temp3481 >= _temp3479.last_plus_one)){
_throw( Null_Exception);}* _temp3481;})); _temp3478.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3482= yyvs; void** _temp3484= _temp3482.curr +
yyvsp_offset; if( _temp3482.base == 0? 1:( _temp3484 < _temp3482.base? 1:
_temp3484 >= _temp3482.last_plus_one)){ _throw( Null_Exception);}* _temp3484;}));
_temp3478.f3= 0; _temp3478;}); _temp3477;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3485= yyls; struct Cyc_Yyltype* _temp3487= _temp3485.curr
+( yylsp_offset - 2); if( _temp3485.base == 0? 1:( _temp3487 < _temp3485.base? 1:
_temp3487 >= _temp3485.last_plus_one)){ _throw( Null_Exception);}* _temp3487;}).first_line,({
struct _tagged_ptr3 _temp3488= yyls; struct Cyc_Yyltype* _temp3490= _temp3488.curr
+ yylsp_offset; if( _temp3488.base == 0? 1:( _temp3490 < _temp3488.base? 1:
_temp3490 >= _temp3488.last_plus_one)){ _throw( Null_Exception);}* _temp3490;}).last_line));
_temp3476;}); _temp3475;}); goto _LL3470; _LL3474: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3491=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3491[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3492; _temp3492.tag=
Cyc_TypeSpecifier_tok; _temp3492.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp3493=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3493[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp3494; _temp3494.tag= Cyc_Absyn_UnionType;
_temp3494.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3495= yyvs; void** _temp3497= _temp3495.curr +( yyvsp_offset - 1); if(
_temp3495.base == 0? 1:( _temp3497 < _temp3495.base? 1: _temp3497 >= _temp3495.last_plus_one)){
_throw( Null_Exception);}* _temp3497;})); _temp3494.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3498= yyvs; void** _temp3500= _temp3498.curr +
yyvsp_offset; if( _temp3498.base == 0? 1:( _temp3500 < _temp3498.base? 1:
_temp3500 >= _temp3498.last_plus_one)){ _throw( Null_Exception);}* _temp3500;}));
_temp3494.f3= 0; _temp3494;}); _temp3493;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3501= yyls; struct Cyc_Yyltype* _temp3503= _temp3501.curr
+( yylsp_offset - 2); if( _temp3501.base == 0? 1:( _temp3503 < _temp3501.base? 1:
_temp3503 >= _temp3501.last_plus_one)){ _throw( Null_Exception);}* _temp3503;}).first_line,({
struct _tagged_ptr3 _temp3504= yyls; struct Cyc_Yyltype* _temp3506= _temp3504.curr
+ yylsp_offset; if( _temp3504.base == 0? 1:( _temp3506 < _temp3504.base? 1:
_temp3506 >= _temp3504.last_plus_one)){ _throw( Null_Exception);}* _temp3506;}).last_line));
_temp3492;}); _temp3491;}); goto _LL3470; _LL3470:;} break; case 84: _LL3465:
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3508=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3508[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3509; _temp3509.tag= Cyc_TypeList_tok; _temp3509.f1= 0; _temp3509;});
_temp3508;}); break; case 85: _LL3507: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3511=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3511[ 0]=({ struct Cyc_TypeList_tok_struct _temp3512; _temp3512.tag= Cyc_TypeList_tok;
_temp3512.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3513= yyvs; void** _temp3515=
_temp3513.curr +( yyvsp_offset - 1); if( _temp3513.base == 0? 1:( _temp3515 <
_temp3513.base? 1: _temp3515 >= _temp3513.last_plus_one)){ _throw(
Null_Exception);}* _temp3515;}))); _temp3512;}); _temp3511;}); break; case 86:
_LL3510:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp3517= Cyc_Parse_lbuf;
if( _temp3517 == 0){ _throw( Null_Exception);} _temp3517->v;}))->lex_curr_pos -=
1; yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3518=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3518[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3519; _temp3519.tag= Cyc_TypeList_tok; _temp3519.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3520= yyvs; void** _temp3522= _temp3520.curr +( yyvsp_offset -
1); if( _temp3520.base == 0? 1:( _temp3522 < _temp3520.base? 1: _temp3522 >=
_temp3520.last_plus_one)){ _throw( Null_Exception);}* _temp3522;}))); _temp3519;});
_temp3518;}); break; case 87: _LL3516: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3524=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3524[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3525; _temp3525.tag=
Cyc_StructOrUnion_tok; _temp3525.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp3525;}); _temp3524;}); break; case 88: _LL3523: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3527=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3527[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3528; _temp3528.tag=
Cyc_StructOrUnion_tok; _temp3528.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp3528;}); _temp3527;}); break; case 89: _LL3526: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3530=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3530[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3531; _temp3531.tag= Cyc_StructFieldDeclList_tok; _temp3531.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3532= yyvs; void** _temp3534= _temp3532.curr +
yyvsp_offset; if( _temp3532.base == 0? 1:( _temp3534 < _temp3532.base? 1:
_temp3534 >= _temp3532.last_plus_one)){ _throw( Null_Exception);}* _temp3534;}))));
_temp3531;}); _temp3530;}); break; case 90: _LL3529: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3536=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3536[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3537; _temp3537.tag= Cyc_StructFieldDeclListList_tok; _temp3537.f1=({
struct Cyc_List_List* _temp3538=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3538->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3539= yyvs; void** _temp3541= _temp3539.curr +
yyvsp_offset; if( _temp3539.base == 0? 1:( _temp3541 < _temp3539.base? 1:
_temp3541 >= _temp3539.last_plus_one)){ _throw( Null_Exception);}* _temp3541;}));
_temp3538->tl= 0; _temp3538;}); _temp3537;}); _temp3536;}); break; case 91:
_LL3535: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3543=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3543[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3544; _temp3544.tag= Cyc_StructFieldDeclListList_tok; _temp3544.f1=({
struct Cyc_List_List* _temp3545=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3545->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3549= yyvs; void** _temp3551= _temp3549.curr +
yyvsp_offset; if( _temp3549.base == 0? 1:( _temp3551 < _temp3549.base? 1:
_temp3551 >= _temp3549.last_plus_one)){ _throw( Null_Exception);}* _temp3551;}));
_temp3545->tl= Cyc_yyget_StructFieldDeclListList_tok(({ struct _tagged_ptr2
_temp3546= yyvs; void** _temp3548= _temp3546.curr +( yyvsp_offset - 1); if(
_temp3546.base == 0? 1:( _temp3548 < _temp3546.base? 1: _temp3548 >= _temp3546.last_plus_one)){
_throw( Null_Exception);}* _temp3548;})); _temp3545;}); _temp3544;}); _temp3543;});
break; case 92: _LL3542: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp3553=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3553[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3554; _temp3554.tag=
Cyc_InitDeclList_tok; _temp3554.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2
_temp3555= yyvs; void** _temp3557= _temp3555.curr + yyvsp_offset; if( _temp3555.base
== 0? 1:( _temp3557 < _temp3555.base? 1: _temp3557 >= _temp3555.last_plus_one)){
_throw( Null_Exception);}* _temp3557;}))); _temp3554;}); _temp3553;}); break;
case 93: _LL3552: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp3559=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3559[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3560; _temp3560.tag=
Cyc_InitDeclList_tok; _temp3560.f1=({ struct Cyc_List_List* _temp3561=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3561->hd=( void*)
Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3562= yyvs; void** _temp3564=
_temp3562.curr + yyvsp_offset; if( _temp3562.base == 0? 1:( _temp3564 <
_temp3562.base? 1: _temp3564 >= _temp3562.last_plus_one)){ _throw(
Null_Exception);}* _temp3564;})); _temp3561->tl= 0; _temp3561;}); _temp3560;});
_temp3559;}); break; case 94: _LL3558: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp3566=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3566[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3567; _temp3567.tag=
Cyc_InitDeclList_tok; _temp3567.f1=({ struct Cyc_List_List* _temp3568=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3568->hd=( void*)
Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3572= yyvs; void** _temp3574=
_temp3572.curr + yyvsp_offset; if( _temp3572.base == 0? 1:( _temp3574 <
_temp3572.base? 1: _temp3574 >= _temp3572.last_plus_one)){ _throw(
Null_Exception);}* _temp3574;})); _temp3568->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3569= yyvs; void** _temp3571= _temp3569.curr +(
yyvsp_offset - 2); if( _temp3569.base == 0? 1:( _temp3571 < _temp3569.base? 1:
_temp3571 >= _temp3569.last_plus_one)){ _throw( Null_Exception);}* _temp3571;}));
_temp3568;}); _temp3567;}); _temp3566;}); break; case 95: _LL3565: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp3576=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp3576[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp3577; _temp3577.tag= Cyc_InitDecl_tok; _temp3577.f1=({ struct _tuple13*
_temp3578=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp3578->f1=
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3579= yyvs; void**
_temp3581= _temp3579.curr + yyvsp_offset; if( _temp3579.base == 0? 1:( _temp3581
< _temp3579.base? 1: _temp3581 >= _temp3579.last_plus_one)){ _throw(
Null_Exception);}* _temp3581;})); _temp3578->f2= 0; _temp3578;}); _temp3577;});
_temp3576;}); break; case 96: _LL3575: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp3583=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3583[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3584; _temp3584.tag= Cyc_InitDecl_tok;
_temp3584.f1=({ struct _tuple13* _temp3585=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3585->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3589= yyvs; void** _temp3591= _temp3589.curr +( yyvsp_offset -
2); if( _temp3589.base == 0? 1:( _temp3591 < _temp3589.base? 1: _temp3591 >=
_temp3589.last_plus_one)){ _throw( Null_Exception);}* _temp3591;})); _temp3585->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3586= yyvs;
void** _temp3588= _temp3586.curr + yyvsp_offset; if( _temp3586.base == 0? 1:(
_temp3588 < _temp3586.base? 1: _temp3588 >= _temp3586.last_plus_one)){ _throw(
Null_Exception);}* _temp3588;})); _temp3585;}); _temp3584;}); _temp3583;});
break; case 97: _LL3582: { struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3622= yyvs; void** _temp3624= _temp3622.curr +(
yyvsp_offset - 2); if( _temp3622.base == 0? 1:( _temp3624 < _temp3622.base? 1:
_temp3624 >= _temp3622.last_plus_one)){ _throw( Null_Exception);}* _temp3624;}))).f1;
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp3619= yyvs; void** _temp3621= _temp3619.curr +( yyvsp_offset - 2); if(
_temp3619.base == 0? 1:( _temp3621 < _temp3619.base? 1: _temp3621 >= _temp3619.last_plus_one)){
_throw( Null_Exception);}* _temp3621;}))).f3; void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3610= yyvs; void**
_temp3612= _temp3610.curr +( yyvsp_offset - 2); if( _temp3610.base == 0? 1:(
_temp3612 < _temp3610.base? 1: _temp3612 >= _temp3610.last_plus_one)){ _throw(
Null_Exception);}* _temp3612;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3613= yyls; struct Cyc_Yyltype* _temp3615= _temp3613.curr +(
yylsp_offset - 2); if( _temp3613.base == 0? 1:( _temp3615 < _temp3613.base? 1:
_temp3615 >= _temp3613.last_plus_one)){ _throw( Null_Exception);}* _temp3615;}).first_line,({
struct _tagged_ptr3 _temp3616= yyls; struct Cyc_Yyltype* _temp3618= _temp3616.curr
+( yylsp_offset - 2); if( _temp3616.base == 0? 1:( _temp3618 < _temp3616.base? 1:
_temp3618 >= _temp3616.last_plus_one)){ _throw( Null_Exception);}* _temp3618;}).last_line));
struct Cyc_List_List* _temp3598; struct Cyc_List_List* _temp3600; struct _tuple0
_temp3596=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3593= yyvs; void** _temp3595= _temp3593.curr +(
yyvsp_offset - 1); if( _temp3593.base == 0? 1:( _temp3595 < _temp3593.base? 1:
_temp3595 >= _temp3593.last_plus_one)){ _throw( Null_Exception);}* _temp3595;})));
_LL3601: _temp3600= _temp3596.f1; goto _LL3599; _LL3599: _temp3598= _temp3596.f2;
goto _LL3597; _LL3597: { struct Cyc_List_List* info=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, t, _temp3600, atts), _temp3598); yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3602=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3602[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3603; _temp3603.tag= Cyc_StructFieldDeclList_tok; _temp3603.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple8*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3604= yyls; struct Cyc_Yyltype* _temp3606= _temp3604.curr
+( yylsp_offset - 2); if( _temp3604.base == 0? 1:( _temp3606 < _temp3604.base? 1:
_temp3606 >= _temp3604.last_plus_one)){ _throw( Null_Exception);}* _temp3606;}).first_line,({
struct _tagged_ptr3 _temp3607= yyls; struct Cyc_Yyltype* _temp3609= _temp3607.curr
+( yylsp_offset - 1); if( _temp3607.base == 0? 1:( _temp3609 < _temp3607.base? 1:
_temp3609 >= _temp3607.last_plus_one)){ _throw( Null_Exception);}* _temp3609;}).last_line),
info); _temp3603;}); _temp3602;}); break;}} case 98: _LL3592: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp3626=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3626[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3627; _temp3627.tag= Cyc_QualSpecList_tok;
_temp3627.f1=({ struct _tuple14* _temp3628=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3628->f1= Cyc_Absyn_empty_tqual(); _temp3628->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp3632=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3632->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3633= yyvs; void** _temp3635= _temp3633.curr +(
yyvsp_offset - 1); if( _temp3633.base == 0? 1:( _temp3635 < _temp3633.base? 1:
_temp3635 >= _temp3633.last_plus_one)){ _throw( Null_Exception);}* _temp3635;}));
_temp3632->tl= 0; _temp3632;}); _temp3628->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3629= yyvs; void** _temp3631= _temp3629.curr +
yyvsp_offset; if( _temp3629.base == 0? 1:( _temp3631 < _temp3629.base? 1:
_temp3631 >= _temp3629.last_plus_one)){ _throw( Null_Exception);}* _temp3631;}));
_temp3628;}); _temp3627;}); _temp3626;}); break; case 99: _LL3625: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp3637=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3637[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3638; _temp3638.tag= Cyc_QualSpecList_tok;
_temp3638.f1=({ struct _tuple14* _temp3639=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3639->f1=(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3653= yyvs; void** _temp3655= _temp3653.curr + yyvsp_offset;
if( _temp3653.base == 0? 1:( _temp3655 < _temp3653.base? 1: _temp3655 >=
_temp3653.last_plus_one)){ _throw( Null_Exception);}* _temp3655;}))).f1;
_temp3639->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp3646=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3646->hd=(
void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2 _temp3650= yyvs; void**
_temp3652= _temp3650.curr +( yyvsp_offset - 2); if( _temp3650.base == 0? 1:(
_temp3652 < _temp3650.base? 1: _temp3652 >= _temp3650.last_plus_one)){ _throw(
Null_Exception);}* _temp3652;})); _temp3646->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3647= yyvs; void** _temp3649= _temp3647.curr +
yyvsp_offset; if( _temp3647.base == 0? 1:( _temp3649 < _temp3647.base? 1:
_temp3649 >= _temp3647.last_plus_one)){ _throw( Null_Exception);}* _temp3649;}))).f2;
_temp3646;}); _temp3639->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3640= yyvs; void** _temp3642= _temp3640.curr +(
yyvsp_offset - 1); if( _temp3640.base == 0? 1:( _temp3642 < _temp3640.base? 1:
_temp3642 >= _temp3640.last_plus_one)){ _throw( Null_Exception);}* _temp3642;})),(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3643= yyvs; void**
_temp3645= _temp3643.curr + yyvsp_offset; if( _temp3643.base == 0? 1:( _temp3645
< _temp3643.base? 1: _temp3645 >= _temp3643.last_plus_one)){ _throw(
Null_Exception);}* _temp3645;}))).f3); _temp3639;}); _temp3638;}); _temp3637;});
break; case 100: _LL3636: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp3657=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3657[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3658; _temp3658.tag=
Cyc_QualSpecList_tok; _temp3658.f1=({ struct _tuple14* _temp3659=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3659->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3663= yyvs; void** _temp3665= _temp3663.curr +(
yyvsp_offset - 1); if( _temp3663.base == 0? 1:( _temp3665 < _temp3663.base? 1:
_temp3665 >= _temp3663.last_plus_one)){ _throw( Null_Exception);}* _temp3665;}));
_temp3659->f2= 0; _temp3659->f3= Cyc_yyget_AttributeList_tok(({ struct
_tagged_ptr2 _temp3660= yyvs; void** _temp3662= _temp3660.curr + yyvsp_offset;
if( _temp3660.base == 0? 1:( _temp3662 < _temp3660.base? 1: _temp3662 >=
_temp3660.last_plus_one)){ _throw( Null_Exception);}* _temp3662;})); _temp3659;});
_temp3658;}); _temp3657;}); break; case 101: _LL3656: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp3667=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3667[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3668; _temp3668.tag= Cyc_QualSpecList_tok;
_temp3668.f1=({ struct _tuple14* _temp3669=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3669->f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3679= yyvs; void** _temp3681= _temp3679.curr +(
yyvsp_offset - 2); if( _temp3679.base == 0? 1:( _temp3681 < _temp3679.base? 1:
_temp3681 >= _temp3679.last_plus_one)){ _throw( Null_Exception);}* _temp3681;})),(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3682= yyvs; void**
_temp3684= _temp3682.curr + yyvsp_offset; if( _temp3682.base == 0? 1:( _temp3684
< _temp3682.base? 1: _temp3684 >= _temp3682.last_plus_one)){ _throw(
Null_Exception);}* _temp3684;}))).f1); _temp3669->f2=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3676= yyvs; void** _temp3678= _temp3676.curr +
yyvsp_offset; if( _temp3676.base == 0? 1:( _temp3678 < _temp3676.base? 1:
_temp3678 >= _temp3676.last_plus_one)){ _throw( Null_Exception);}* _temp3678;}))).f2;
_temp3669->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp3670= yyvs; void** _temp3672= _temp3670.curr +( yyvsp_offset - 1); if(
_temp3670.base == 0? 1:( _temp3672 < _temp3670.base? 1: _temp3672 >= _temp3670.last_plus_one)){
_throw( Null_Exception);}* _temp3672;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3673= yyvs; void** _temp3675= _temp3673.curr + yyvsp_offset;
if( _temp3673.base == 0? 1:( _temp3675 < _temp3673.base? 1: _temp3675 >=
_temp3673.last_plus_one)){ _throw( Null_Exception);}* _temp3675;}))).f3);
_temp3669;}); _temp3668;}); _temp3667;}); break; case 102: _LL3666: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp3686=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3686[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp3687; _temp3687.tag= Cyc_DeclaratorExpoptList_tok;
_temp3687.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(({ struct _tagged_ptr2 _temp3688= yyvs; void**
_temp3690= _temp3688.curr + yyvsp_offset; if( _temp3688.base == 0? 1:( _temp3690
< _temp3688.base? 1: _temp3690 >= _temp3688.last_plus_one)){ _throw(
Null_Exception);}* _temp3690;}))); _temp3687;}); _temp3686;}); break; case 103:
_LL3685: yyval=( void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp3692=(
struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp3692[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp3693; _temp3693.tag=
Cyc_DeclaratorExpoptList_tok; _temp3693.f1=({ struct Cyc_List_List* _temp3694=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3694->hd=(
void*) Cyc_yyget_DeclaratorExpopt_tok(({ struct _tagged_ptr2 _temp3695= yyvs;
void** _temp3697= _temp3695.curr + yyvsp_offset; if( _temp3695.base == 0? 1:(
_temp3697 < _temp3695.base? 1: _temp3697 >= _temp3695.last_plus_one)){ _throw(
Null_Exception);}* _temp3697;})); _temp3694->tl= 0; _temp3694;}); _temp3693;});
_temp3692;}); break; case 104: _LL3691: yyval=( void*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp3699=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3699[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp3700; _temp3700.tag= Cyc_DeclaratorExpoptList_tok; _temp3700.f1=({ struct
Cyc_List_List* _temp3701=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3701->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({ struct _tagged_ptr2
_temp3705= yyvs; void** _temp3707= _temp3705.curr + yyvsp_offset; if( _temp3705.base
== 0? 1:( _temp3707 < _temp3705.base? 1: _temp3707 >= _temp3705.last_plus_one)){
_throw( Null_Exception);}* _temp3707;})); _temp3701->tl= Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3702= yyvs; void** _temp3704= _temp3702.curr +(
yyvsp_offset - 2); if( _temp3702.base == 0? 1:( _temp3704 < _temp3702.base? 1:
_temp3704 >= _temp3702.last_plus_one)){ _throw( Null_Exception);}* _temp3704;}));
_temp3701;}); _temp3700;}); _temp3699;}); break; case 105: _LL3698: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp3709=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp3709[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp3710; _temp3710.tag= Cyc_DeclaratorExpopt_tok;
_temp3710.f1=({ struct _tuple15* _temp3711=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp3711->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3712= yyvs; void** _temp3714= _temp3712.curr + yyvsp_offset;
if( _temp3712.base == 0? 1:( _temp3714 < _temp3712.base? 1: _temp3714 >=
_temp3712.last_plus_one)){ _throw( Null_Exception);}* _temp3714;})); _temp3711->f2=
0; _temp3711;}); _temp3710;}); _temp3709;}); break; case 106: _LL3708: yyval=(
void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3716=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp3716[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp3717; _temp3717.tag= Cyc_DeclaratorExpopt_tok;
_temp3717.f1=({ struct _tuple15* _temp3718=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp3718->f1=({ struct Cyc_Parse_Declarator*
_temp3723=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3723->id=({ struct _tuple1* _temp3724=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3724->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp3728=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp3728[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp3729; _temp3729.tag= Cyc_Absyn_Rel_n;
_temp3729.f1= 0; _temp3729;}); _temp3728;}); _temp3724->f2=({ struct
_tagged_string* _temp3725=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3725[ 0]=( struct _tagged_string)({ char* _temp3726=(
char*)""; struct _tagged_string _temp3727; _temp3727.curr= _temp3726; _temp3727.base=
_temp3726; _temp3727.last_plus_one= _temp3726 + 1; _temp3727;}); _temp3725;});
_temp3724;}); _temp3723->tms= 0; _temp3723;}); _temp3718->f2=({ struct Cyc_Core_Opt*
_temp3719=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3719->v=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3720= yyvs;
void** _temp3722= _temp3720.curr + yyvsp_offset; if( _temp3720.base == 0? 1:(
_temp3722 < _temp3720.base? 1: _temp3722 >= _temp3720.last_plus_one)){ _throw(
Null_Exception);}* _temp3722;})); _temp3719;}); _temp3718;}); _temp3717;});
_temp3716;}); break; case 107: _LL3715: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp3731=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp3731[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp3732; _temp3732.tag= Cyc_DeclaratorExpopt_tok; _temp3732.f1=({ struct
_tuple15* _temp3733=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15));
_temp3733->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3738= yyvs;
void** _temp3740= _temp3738.curr +( yyvsp_offset - 2); if( _temp3738.base == 0?
1:( _temp3740 < _temp3738.base? 1: _temp3740 >= _temp3738.last_plus_one)){
_throw( Null_Exception);}* _temp3740;})); _temp3733->f2=({ struct Cyc_Core_Opt*
_temp3734=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3734->v=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3735= yyvs;
void** _temp3737= _temp3735.curr + yyvsp_offset; if( _temp3735.base == 0? 1:(
_temp3737 < _temp3735.base? 1: _temp3737 >= _temp3735.last_plus_one)){ _throw(
Null_Exception);}* _temp3737;})); _temp3734;}); _temp3733;}); _temp3732;});
_temp3731;}); break; case 108: _LL3730: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3762= yyls; struct Cyc_Yyltype* _temp3764= _temp3762.curr
+( yylsp_offset - 3); if( _temp3762.base == 0? 1:( _temp3764 < _temp3762.base? 1:
_temp3764 >= _temp3762.last_plus_one)){ _throw( Null_Exception);}* _temp3764;}).first_line,({
struct _tagged_ptr3 _temp3765= yyls; struct Cyc_Yyltype* _temp3767= _temp3765.curr
+( yylsp_offset - 3); if( _temp3765.base == 0? 1:( _temp3767 < _temp3765.base? 1:
_temp3767 >= _temp3765.last_plus_one)){ _throw( Null_Exception);}* _temp3767;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3768= yyvs; void** _temp3770=
_temp3768.curr +( yyvsp_offset - 3); if( _temp3768.base == 0? 1:( _temp3770 <
_temp3768.base? 1: _temp3770 >= _temp3768.last_plus_one)){ _throw(
Null_Exception);}* _temp3770;}))); yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3742=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3742[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3743; _temp3743.tag=
Cyc_TypeSpecifier_tok; _temp3743.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3744=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3744[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3745; _temp3745.tag= Cyc_Parse_Decl_spec;
_temp3745.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3746= yyvs; void** _temp3748= _temp3746.curr +(
yyvsp_offset - 4); if( _temp3746.base == 0? 1:( _temp3748 < _temp3746.base? 1:
_temp3748 >= _temp3746.last_plus_one)){ _throw( Null_Exception);}* _temp3748;})),
ts,({ struct Cyc_Core_Opt* _temp3749=( struct Cyc_Core_Opt*) GC_malloc( sizeof(
struct Cyc_Core_Opt)); _temp3749->v=( void*) Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3750= yyvs; void** _temp3752= _temp3750.curr +(
yyvsp_offset - 1); if( _temp3750.base == 0? 1:( _temp3752 < _temp3750.base? 1:
_temp3752 >= _temp3750.last_plus_one)){ _throw( Null_Exception);}* _temp3752;}));
_temp3749;}), Cyc_yyget_Bool_tok(({ struct _tagged_ptr2 _temp3753= yyvs; void**
_temp3755= _temp3753.curr +( yyvsp_offset - 5); if( _temp3753.base == 0? 1:(
_temp3755 < _temp3753.base? 1: _temp3755 >= _temp3753.last_plus_one)){ _throw(
Null_Exception);}* _temp3755;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3756= yyls; struct Cyc_Yyltype* _temp3758= _temp3756.curr +(
yylsp_offset - 5); if( _temp3756.base == 0? 1:( _temp3758 < _temp3756.base? 1:
_temp3758 >= _temp3756.last_plus_one)){ _throw( Null_Exception);}* _temp3758;}).first_line,({
struct _tagged_ptr3 _temp3759= yyls; struct Cyc_Yyltype* _temp3761= _temp3759.curr
+ yylsp_offset; if( _temp3759.base == 0? 1:( _temp3761 < _temp3759.base? 1:
_temp3761 >= _temp3759.last_plus_one)){ _throw( Null_Exception);}* _temp3761;}).last_line));
_temp3745;}); _temp3744;})); _temp3743;}); _temp3742;}); break;} case 109:
_LL3741: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3772=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3772[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3773; _temp3773.tag=
Cyc_TypeSpecifier_tok; _temp3773.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp3774=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3774[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3775; _temp3775.tag= Cyc_Absyn_TunionType;
_temp3775.f1=({ struct Cyc_Absyn_TunionInfo _temp3776; _temp3776.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp3780=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp3780[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp3781; _temp3781.tag= Cyc_Absyn_UnknownTunion;
_temp3781.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp3782; _temp3782.name=
Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3786= yyvs; void** _temp3788=
_temp3786.curr +( yyvsp_offset - 1); if( _temp3786.base == 0? 1:( _temp3788 <
_temp3786.base? 1: _temp3788 >= _temp3786.last_plus_one)){ _throw(
Null_Exception);}* _temp3788;})); _temp3782.is_xtunion= Cyc_yyget_Bool_tok(({
struct _tagged_ptr2 _temp3783= yyvs; void** _temp3785= _temp3783.curr +(
yyvsp_offset - 2); if( _temp3783.base == 0? 1:( _temp3785 < _temp3783.base? 1:
_temp3785 >= _temp3783.last_plus_one)){ _throw( Null_Exception);}* _temp3785;}));
_temp3782;}); _temp3781;}); _temp3780;})); _temp3776.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3777= yyvs; void** _temp3779= _temp3777.curr +
yyvsp_offset; if( _temp3777.base == 0? 1:( _temp3779 < _temp3777.base? 1:
_temp3779 >= _temp3777.last_plus_one)){ _throw( Null_Exception);}* _temp3779;}));
_temp3776.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp3776;}); _temp3775;});
_temp3774;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3789= yyls;
struct Cyc_Yyltype* _temp3791= _temp3789.curr +( yylsp_offset - 2); if(
_temp3789.base == 0? 1:( _temp3791 < _temp3789.base? 1: _temp3791 >= _temp3789.last_plus_one)){
_throw( Null_Exception);}* _temp3791;}).first_line,({ struct _tagged_ptr3
_temp3792= yyls; struct Cyc_Yyltype* _temp3794= _temp3792.curr + yylsp_offset;
if( _temp3792.base == 0? 1:( _temp3794 < _temp3792.base? 1: _temp3794 >=
_temp3792.last_plus_one)){ _throw( Null_Exception);}* _temp3794;}).last_line));
_temp3773;}); _temp3772;}); break; case 110: _LL3771: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp3796=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3796[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3797; _temp3797.tag= Cyc_TypeSpecifier_tok;
_temp3797.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp3798=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp3798[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp3799; _temp3799.tag=
Cyc_Absyn_TunionType; _temp3799.f1=({ struct Cyc_Absyn_TunionInfo _temp3800;
_temp3800.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp3807=( struct Cyc_Absyn_UnknownTunion_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp3807[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp3808; _temp3808.tag= Cyc_Absyn_UnknownTunion; _temp3808.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp3809; _temp3809.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3813=
yyvs; void** _temp3815= _temp3813.curr +( yyvsp_offset - 1); if( _temp3813.base
== 0? 1:( _temp3815 < _temp3813.base? 1: _temp3815 >= _temp3813.last_plus_one)){
_throw( Null_Exception);}* _temp3815;})); _temp3809.is_xtunion= Cyc_yyget_Bool_tok(({
struct _tagged_ptr2 _temp3810= yyvs; void** _temp3812= _temp3810.curr +(
yyvsp_offset - 3); if( _temp3810.base == 0? 1:( _temp3812 < _temp3810.base? 1:
_temp3812 >= _temp3810.last_plus_one)){ _throw( Null_Exception);}* _temp3812;}));
_temp3809;}); _temp3808;}); _temp3807;})); _temp3800.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3804= yyvs; void** _temp3806= _temp3804.curr +
yyvsp_offset; if( _temp3804.base == 0? 1:( _temp3806 < _temp3804.base? 1:
_temp3806 >= _temp3804.last_plus_one)){ _throw( Null_Exception);}* _temp3806;}));
_temp3800.rgn=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp3801= yyvs;
void** _temp3803= _temp3801.curr +( yyvsp_offset - 2); if( _temp3801.base == 0?
1:( _temp3803 < _temp3801.base? 1: _temp3803 >= _temp3801.last_plus_one)){
_throw( Null_Exception);}* _temp3803;})); _temp3800;}); _temp3799;}); _temp3798;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3816= yyls; struct Cyc_Yyltype*
_temp3818= _temp3816.curr +( yylsp_offset - 3); if( _temp3816.base == 0? 1:(
_temp3818 < _temp3816.base? 1: _temp3818 >= _temp3816.last_plus_one)){ _throw(
Null_Exception);}* _temp3818;}).first_line,({ struct _tagged_ptr3 _temp3819=
yyls; struct Cyc_Yyltype* _temp3821= _temp3819.curr + yylsp_offset; if(
_temp3819.base == 0? 1:( _temp3821 < _temp3819.base? 1: _temp3821 >= _temp3819.last_plus_one)){
_throw( Null_Exception);}* _temp3821;}).last_line)); _temp3797;}); _temp3796;});
break; case 111: _LL3795: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3823=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3823[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3824; _temp3824.tag=
Cyc_TypeSpecifier_tok; _temp3824.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp3825=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp3825[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp3826; _temp3826.tag= Cyc_Absyn_TunionFieldType;
_temp3826.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp3827; _temp3827.field_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct* _temp3831=( struct
Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunionfield_struct));
_temp3831[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct _temp3832; _temp3832.tag=
Cyc_Absyn_UnknownTunionfield; _temp3832.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp3833; _temp3833.tunion_name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3840= yyvs; void** _temp3842= _temp3840.curr +( yyvsp_offset - 3); if(
_temp3840.base == 0? 1:( _temp3842 < _temp3840.base? 1: _temp3842 >= _temp3840.last_plus_one)){
_throw( Null_Exception);}* _temp3842;})); _temp3833.field_name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3837= yyvs; void** _temp3839= _temp3837.curr +(
yyvsp_offset - 1); if( _temp3837.base == 0? 1:( _temp3839 < _temp3837.base? 1:
_temp3839 >= _temp3837.last_plus_one)){ _throw( Null_Exception);}* _temp3839;}));
_temp3833.is_xtunion= Cyc_yyget_Bool_tok(({ struct _tagged_ptr2 _temp3834= yyvs;
void** _temp3836= _temp3834.curr +( yyvsp_offset - 4); if( _temp3834.base == 0?
1:( _temp3836 < _temp3834.base? 1: _temp3836 >= _temp3834.last_plus_one)){
_throw( Null_Exception);}* _temp3836;})); _temp3833;}); _temp3832;}); _temp3831;}));
_temp3827.targs= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3828= yyvs;
void** _temp3830= _temp3828.curr + yyvsp_offset; if( _temp3828.base == 0? 1:(
_temp3830 < _temp3828.base? 1: _temp3830 >= _temp3828.last_plus_one)){ _throw(
Null_Exception);}* _temp3830;})); _temp3827;}); _temp3826;}); _temp3825;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3843= yyls; struct Cyc_Yyltype* _temp3845= _temp3843.curr
+( yylsp_offset - 4); if( _temp3843.base == 0? 1:( _temp3845 < _temp3843.base? 1:
_temp3845 >= _temp3843.last_plus_one)){ _throw( Null_Exception);}* _temp3845;}).first_line,({
struct _tagged_ptr3 _temp3846= yyls; struct Cyc_Yyltype* _temp3848= _temp3846.curr
+ yylsp_offset; if( _temp3846.base == 0? 1:( _temp3848 < _temp3846.base? 1:
_temp3848 >= _temp3846.last_plus_one)){ _throw( Null_Exception);}* _temp3848;}).last_line));
_temp3824;}); _temp3823;}); break; case 112: _LL3822: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp3850=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp3850[ 0]=({ struct Cyc_Bool_tok_struct
_temp3851; _temp3851.tag= Cyc_Bool_tok; _temp3851.f1= 0; _temp3851;}); _temp3850;});
break; case 113: _LL3849: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp3853=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp3853[ 0]=({ struct Cyc_Bool_tok_struct _temp3854; _temp3854.tag= Cyc_Bool_tok;
_temp3854.f1= 1; _temp3854;}); _temp3853;}); break; case 114: _LL3852: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp3856=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp3856[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp3857; _temp3857.tag= Cyc_TunionFieldList_tok;
_temp3857.f1=({ struct Cyc_List_List* _temp3858=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3858->hd=( void*) Cyc_yyget_TunionField_tok(({
struct _tagged_ptr2 _temp3859= yyvs; void** _temp3861= _temp3859.curr +
yyvsp_offset; if( _temp3859.base == 0? 1:( _temp3861 < _temp3859.base? 1:
_temp3861 >= _temp3859.last_plus_one)){ _throw( Null_Exception);}* _temp3861;}));
_temp3858->tl= 0; _temp3858;}); _temp3857;}); _temp3856;}); break; case 115:
_LL3855: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp3863=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3863[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3864; _temp3864.tag=
Cyc_TunionFieldList_tok; _temp3864.f1=({ struct Cyc_List_List* _temp3865=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3865->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3866= yyvs; void**
_temp3868= _temp3866.curr +( yyvsp_offset - 1); if( _temp3866.base == 0? 1:(
_temp3868 < _temp3866.base? 1: _temp3868 >= _temp3866.last_plus_one)){ _throw(
Null_Exception);}* _temp3868;})); _temp3865->tl= 0; _temp3865;}); _temp3864;});
_temp3863;}); break; case 116: _LL3862: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3870=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3870[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3871; _temp3871.tag= Cyc_TunionFieldList_tok; _temp3871.f1=({ struct Cyc_List_List*
_temp3872=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3872->hd=( void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2
_temp3876= yyvs; void** _temp3878= _temp3876.curr +( yyvsp_offset - 2); if(
_temp3876.base == 0? 1:( _temp3878 < _temp3876.base? 1: _temp3878 >= _temp3876.last_plus_one)){
_throw( Null_Exception);}* _temp3878;})); _temp3872->tl= Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3873= yyvs; void** _temp3875= _temp3873.curr +
yyvsp_offset; if( _temp3873.base == 0? 1:( _temp3875 < _temp3873.base? 1:
_temp3875 >= _temp3873.last_plus_one)){ _throw( Null_Exception);}* _temp3875;}));
_temp3872;}); _temp3871;}); _temp3870;}); break; case 117: _LL3869: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp3880=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp3880[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp3881; _temp3881.tag= Cyc_TunionFieldList_tok;
_temp3881.f1=({ struct Cyc_List_List* _temp3882=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3882->hd=( void*) Cyc_yyget_TunionField_tok(({
struct _tagged_ptr2 _temp3886= yyvs; void** _temp3888= _temp3886.curr +(
yyvsp_offset - 2); if( _temp3886.base == 0? 1:( _temp3888 < _temp3886.base? 1:
_temp3888 >= _temp3886.last_plus_one)){ _throw( Null_Exception);}* _temp3888;}));
_temp3882->tl= Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2 _temp3883=
yyvs; void** _temp3885= _temp3883.curr + yyvsp_offset; if( _temp3883.base == 0?
1:( _temp3885 < _temp3883.base? 1: _temp3885 >= _temp3883.last_plus_one)){
_throw( Null_Exception);}* _temp3885;})); _temp3882;}); _temp3881;}); _temp3880;});
break; case 118: _LL3879: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp3890=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3890[ 0]=({ struct Cyc_TunionField_tok_struct _temp3891; _temp3891.tag= Cyc_TunionField_tok;
_temp3891.f1=({ struct Cyc_Absyn_Tunionfield* _temp3892=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3892->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3899= yyvs; void** _temp3901= _temp3899.curr +
yyvsp_offset; if( _temp3899.base == 0? 1:( _temp3901 < _temp3899.base? 1:
_temp3901 >= _temp3899.last_plus_one)){ _throw( Null_Exception);}* _temp3901;}));
_temp3892->tvs= 0; _temp3892->typs= 0; _temp3892->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3893= yyls; struct Cyc_Yyltype* _temp3895= _temp3893.curr
+ yylsp_offset; if( _temp3893.base == 0? 1:( _temp3895 < _temp3893.base? 1:
_temp3895 >= _temp3893.last_plus_one)){ _throw( Null_Exception);}* _temp3895;}).first_line,({
struct _tagged_ptr3 _temp3896= yyls; struct Cyc_Yyltype* _temp3898= _temp3896.curr
+ yylsp_offset; if( _temp3896.base == 0? 1:( _temp3898 < _temp3896.base? 1:
_temp3898 >= _temp3896.last_plus_one)){ _throw( Null_Exception);}* _temp3898;}).last_line);
_temp3892;}); _temp3891;}); _temp3890;}); break; case 119: _LL3889: { struct Cyc_List_List*
typs=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3924= yyls; struct Cyc_Yyltype* _temp3926= _temp3924.curr +(
yylsp_offset - 1); if( _temp3924.base == 0? 1:( _temp3926 < _temp3924.base? 1:
_temp3926 >= _temp3924.last_plus_one)){ _throw( Null_Exception);}* _temp3926;}).first_line,({
struct _tagged_ptr3 _temp3927= yyls; struct Cyc_Yyltype* _temp3929= _temp3927.curr
+( yylsp_offset - 1); if( _temp3927.base == 0? 1:( _temp3929 < _temp3927.base? 1:
_temp3929 >= _temp3927.last_plus_one)){ _throw( Null_Exception);}* _temp3929;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3930= yyvs; void** _temp3932= _temp3930.curr +(
yyvsp_offset - 1); if( _temp3930.base == 0? 1:( _temp3932 < _temp3930.base? 1:
_temp3932 >= _temp3930.last_plus_one)){ _throw( Null_Exception);}* _temp3932;}))));
struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3915= yyls; struct Cyc_Yyltype* _temp3917= _temp3915.curr
+( yylsp_offset - 3); if( _temp3915.base == 0? 1:( _temp3917 < _temp3915.base? 1:
_temp3917 >= _temp3915.last_plus_one)){ _throw( Null_Exception);}* _temp3917;}).first_line,({
struct _tagged_ptr3 _temp3918= yyls; struct Cyc_Yyltype* _temp3920= _temp3918.curr
+( yylsp_offset - 3); if( _temp3918.base == 0? 1:( _temp3920 < _temp3918.base? 1:
_temp3920 >= _temp3918.last_plus_one)){ _throw( Null_Exception);}* _temp3920;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3921= yyvs; void** _temp3923=
_temp3921.curr +( yyvsp_offset - 3); if( _temp3921.base == 0? 1:( _temp3923 <
_temp3921.base? 1: _temp3923 >= _temp3921.last_plus_one)){ _throw(
Null_Exception);}* _temp3923;}))); yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp3903=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3903[ 0]=({ struct Cyc_TunionField_tok_struct _temp3904; _temp3904.tag= Cyc_TunionField_tok;
_temp3904.f1=({ struct Cyc_Absyn_Tunionfield* _temp3905=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3905->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3912= yyvs; void** _temp3914= _temp3912.curr +(
yyvsp_offset - 4); if( _temp3912.base == 0? 1:( _temp3914 < _temp3912.base? 1:
_temp3914 >= _temp3912.last_plus_one)){ _throw( Null_Exception);}* _temp3914;}));
_temp3905->tvs= tvs; _temp3905->typs= typs; _temp3905->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3906= yyls; struct Cyc_Yyltype* _temp3908= _temp3906.curr
+( yylsp_offset - 4); if( _temp3906.base == 0? 1:( _temp3908 < _temp3906.base? 1:
_temp3908 >= _temp3906.last_plus_one)){ _throw( Null_Exception);}* _temp3908;}).first_line,({
struct _tagged_ptr3 _temp3909= yyls; struct Cyc_Yyltype* _temp3911= _temp3909.curr
+ yylsp_offset; if( _temp3909.base == 0? 1:( _temp3911 < _temp3909.base? 1:
_temp3911 >= _temp3909.last_plus_one)){ _throw( Null_Exception);}* _temp3911;}).last_line);
_temp3905;}); _temp3904;}); _temp3903;}); break;} case 120: _LL3902: yyval=({
struct _tagged_ptr2 _temp3934= yyvs; void** _temp3936= _temp3934.curr +
yyvsp_offset; if( _temp3934.base == 0? 1:( _temp3936 < _temp3934.base? 1:
_temp3936 >= _temp3934.last_plus_one)){ _throw( Null_Exception);}* _temp3936;});
break; case 121: _LL3933: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp3938=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3938[ 0]=({ struct Cyc_Declarator_tok_struct _temp3939; _temp3939.tag= Cyc_Declarator_tok;
_temp3939.f1=({ struct Cyc_Parse_Declarator* _temp3940=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3940->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3947= yyvs; void** _temp3949= _temp3947.curr +
yyvsp_offset; if( _temp3947.base == 0? 1:( _temp3949 < _temp3947.base? 1:
_temp3949 >= _temp3947.last_plus_one)){ _throw( Null_Exception);}* _temp3949;})))->id;
_temp3940->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp3941= yyvs; void** _temp3943= _temp3941.curr +( yyvsp_offset - 1); if(
_temp3941.base == 0? 1:( _temp3943 < _temp3941.base? 1: _temp3943 >= _temp3941.last_plus_one)){
_throw( Null_Exception);}* _temp3943;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3944= yyvs; void** _temp3946= _temp3944.curr + yyvsp_offset;
if( _temp3944.base == 0? 1:( _temp3946 < _temp3944.base? 1: _temp3946 >=
_temp3944.last_plus_one)){ _throw( Null_Exception);}* _temp3946;})))->tms);
_temp3940;}); _temp3939;}); _temp3938;}); break; case 122: _LL3937: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp3951=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3951[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3952; _temp3952.tag= Cyc_Declarator_tok;
_temp3952.f1=({ struct Cyc_Parse_Declarator* _temp3953=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3953->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3954= yyvs; void** _temp3956= _temp3954.curr +
yyvsp_offset; if( _temp3954.base == 0? 1:( _temp3956 < _temp3954.base? 1:
_temp3956 >= _temp3954.last_plus_one)){ _throw( Null_Exception);}* _temp3956;}));
_temp3953->tms= 0; _temp3953;}); _temp3952;}); _temp3951;}); break; case 123:
_LL3950: yyval=({ struct _tagged_ptr2 _temp3958= yyvs; void** _temp3960=
_temp3958.curr +( yyvsp_offset - 1); if( _temp3958.base == 0? 1:( _temp3960 <
_temp3958.base? 1: _temp3960 >= _temp3958.last_plus_one)){ _throw(
Null_Exception);}* _temp3960;}); break; case 124: _LL3957: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp3962=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3962[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3963; _temp3963.tag= Cyc_Declarator_tok;
_temp3963.f1=({ struct Cyc_Parse_Declarator* _temp3964=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3964->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3969= yyvs; void** _temp3971= _temp3969.curr +(
yyvsp_offset - 2); if( _temp3969.base == 0? 1:( _temp3971 < _temp3969.base? 1:
_temp3971 >= _temp3969.last_plus_one)){ _throw( Null_Exception);}* _temp3971;})))->id;
_temp3964->tms=({ struct Cyc_List_List* _temp3965=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3965->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3965->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3966= yyvs;
void** _temp3968= _temp3966.curr +( yyvsp_offset - 2); if( _temp3966.base == 0?
1:( _temp3968 < _temp3966.base? 1: _temp3968 >= _temp3966.last_plus_one)){
_throw( Null_Exception);}* _temp3968;})))->tms; _temp3965;}); _temp3964;});
_temp3963;}); _temp3962;}); break; case 125: _LL3961: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp3973=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3973[ 0]=({ struct
Cyc_Declarator_tok_struct _temp3974; _temp3974.tag= Cyc_Declarator_tok;
_temp3974.f1=({ struct Cyc_Parse_Declarator* _temp3975=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3975->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3985= yyvs; void** _temp3987= _temp3985.curr +(
yyvsp_offset - 3); if( _temp3985.base == 0? 1:( _temp3987 < _temp3985.base? 1:
_temp3987 >= _temp3985.last_plus_one)){ _throw( Null_Exception);}* _temp3987;})))->id;
_temp3975->tms=({ struct Cyc_List_List* _temp3976=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3976->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3980=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3980[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3981; _temp3981.tag= Cyc_Absyn_ConstArray_mod;
_temp3981.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3982= yyvs; void**
_temp3984= _temp3982.curr +( yyvsp_offset - 1); if( _temp3982.base == 0? 1:(
_temp3984 < _temp3982.base? 1: _temp3984 >= _temp3982.last_plus_one)){ _throw(
Null_Exception);}* _temp3984;})); _temp3981;}); _temp3980;})); _temp3976->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3977= yyvs; void**
_temp3979= _temp3977.curr +( yyvsp_offset - 3); if( _temp3977.base == 0? 1:(
_temp3979 < _temp3977.base? 1: _temp3979 >= _temp3977.last_plus_one)){ _throw(
Null_Exception);}* _temp3979;})))->tms; _temp3976;}); _temp3975;}); _temp3974;});
_temp3973;}); break; case 126: _LL3972: { struct _tuple16 _temp3994; struct Cyc_Core_Opt*
_temp3995; int _temp3997; struct Cyc_List_List* _temp3999; struct _tuple16*
_temp3992= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp3989=
yyvs; void** _temp3991= _temp3989.curr +( yyvsp_offset - 1); if( _temp3989.base
== 0? 1:( _temp3991 < _temp3989.base? 1: _temp3991 >= _temp3989.last_plus_one)){
_throw( Null_Exception);}* _temp3991;})); _temp3994=* _temp3992; _LL4000:
_temp3999= _temp3994.f1; goto _LL3998; _LL3998: _temp3997= _temp3994.f2; goto
_LL3996; _LL3996: _temp3995= _temp3994.f3; goto _LL3993; _LL3993: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp4001=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4001[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4002; _temp4002.tag= Cyc_Declarator_tok;
_temp4002.f1=({ struct Cyc_Parse_Declarator* _temp4003=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4003->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4012= yyvs; void** _temp4014= _temp4012.curr +(
yyvsp_offset - 3); if( _temp4012.base == 0? 1:( _temp4014 < _temp4012.base? 1:
_temp4014 >= _temp4012.last_plus_one)){ _throw( Null_Exception);}* _temp4014;})))->id;
_temp4003->tms=({ struct Cyc_List_List* _temp4004=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4004->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4008=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4008[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4009; _temp4009.tag= Cyc_Absyn_Function_mod;
_temp4009.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4010=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4010[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4011; _temp4011.tag= Cyc_Absyn_WithTypes;
_temp4011.f1= _temp3999; _temp4011.f2= _temp3997; _temp4011.f3= _temp3995;
_temp4011;}); _temp4010;})); _temp4009;}); _temp4008;})); _temp4004->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4005= yyvs; void** _temp4007= _temp4005.curr +(
yyvsp_offset - 3); if( _temp4005.base == 0? 1:( _temp4007 < _temp4005.base? 1:
_temp4007 >= _temp4005.last_plus_one)){ _throw( Null_Exception);}* _temp4007;})))->tms;
_temp4004;}); _temp4003;}); _temp4002;}); _temp4001;}); break;} case 127:
_LL3988: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4016=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4016[ 0]=({ struct Cyc_Declarator_tok_struct _temp4017; _temp4017.tag= Cyc_Declarator_tok;
_temp4017.f1=({ struct Cyc_Parse_Declarator* _temp4018=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4018->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4027= yyvs; void** _temp4029= _temp4027.curr +(
yyvsp_offset - 2); if( _temp4027.base == 0? 1:( _temp4029 < _temp4027.base? 1:
_temp4029 >= _temp4027.last_plus_one)){ _throw( Null_Exception);}* _temp4029;})))->id;
_temp4018->tms=({ struct Cyc_List_List* _temp4019=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4019->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4023=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4023[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4024; _temp4024.tag= Cyc_Absyn_Function_mod;
_temp4024.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4025=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4025[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4026; _temp4026.tag= Cyc_Absyn_WithTypes;
_temp4026.f1= 0; _temp4026.f2= 0; _temp4026.f3= 0; _temp4026;}); _temp4025;}));
_temp4024;}); _temp4023;})); _temp4019->tl=( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp4020= yyvs; void** _temp4022= _temp4020.curr +( yyvsp_offset -
2); if( _temp4020.base == 0? 1:( _temp4022 < _temp4020.base? 1: _temp4022 >=
_temp4020.last_plus_one)){ _throw( Null_Exception);}* _temp4022;})))->tms;
_temp4019;}); _temp4018;}); _temp4017;}); _temp4016;}); break; case 128: _LL4015:
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4031=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4031[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4032; _temp4032.tag= Cyc_Declarator_tok;
_temp4032.f1=({ struct Cyc_Parse_Declarator* _temp4033=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4033->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4048= yyvs; void** _temp4050= _temp4048.curr +(
yyvsp_offset - 4); if( _temp4048.base == 0? 1:( _temp4050 < _temp4048.base? 1:
_temp4050 >= _temp4048.last_plus_one)){ _throw( Null_Exception);}* _temp4050;})))->id;
_temp4033->tms=({ struct Cyc_List_List* _temp4034=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4034->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4038=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4038[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4039; _temp4039.tag= Cyc_Absyn_Function_mod;
_temp4039.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4040=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4040[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4041; _temp4041.tag= Cyc_Absyn_WithTypes;
_temp4041.f1= 0; _temp4041.f2= 0; _temp4041.f3=({ struct Cyc_Core_Opt* _temp4042=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4042->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4043=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4043[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4044; _temp4044.tag= Cyc_Absyn_JoinEff; _temp4044.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4045= yyvs; void** _temp4047=
_temp4045.curr +( yyvsp_offset - 1); if( _temp4045.base == 0? 1:( _temp4047 <
_temp4045.base? 1: _temp4047 >= _temp4045.last_plus_one)){ _throw(
Null_Exception);}* _temp4047;})); _temp4044;}); _temp4043;})); _temp4042;});
_temp4041;}); _temp4040;})); _temp4039;}); _temp4038;})); _temp4034->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4035= yyvs; void** _temp4037= _temp4035.curr +(
yyvsp_offset - 4); if( _temp4035.base == 0? 1:( _temp4037 < _temp4035.base? 1:
_temp4037 >= _temp4035.last_plus_one)){ _throw( Null_Exception);}* _temp4037;})))->tms;
_temp4034;}); _temp4033;}); _temp4032;}); _temp4031;}); break; case 129: _LL4030:
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4052=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4052[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4053; _temp4053.tag= Cyc_Declarator_tok;
_temp4053.f1=({ struct Cyc_Parse_Declarator* _temp4054=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4054->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4072= yyvs; void** _temp4074= _temp4072.curr +(
yyvsp_offset - 3); if( _temp4072.base == 0? 1:( _temp4074 < _temp4072.base? 1:
_temp4074 >= _temp4072.last_plus_one)){ _throw( Null_Exception);}* _temp4074;})))->id;
_temp4054->tms=({ struct Cyc_List_List* _temp4055=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4055->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4059=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4059[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4060; _temp4060.tag= Cyc_Absyn_Function_mod;
_temp4060.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp4061=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp4061[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp4062; _temp4062.tag= Cyc_Absyn_NoTypes;
_temp4062.f1= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4063= yyvs; void**
_temp4065= _temp4063.curr +( yyvsp_offset - 1); if( _temp4063.base == 0? 1:(
_temp4065 < _temp4063.base? 1: _temp4065 >= _temp4063.last_plus_one)){ _throw(
Null_Exception);}* _temp4065;})); _temp4062.f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4066= yyls; struct Cyc_Yyltype* _temp4068= _temp4066.curr
+( yylsp_offset - 3); if( _temp4066.base == 0? 1:( _temp4068 < _temp4066.base? 1:
_temp4068 >= _temp4066.last_plus_one)){ _throw( Null_Exception);}* _temp4068;}).first_line,({
struct _tagged_ptr3 _temp4069= yyls; struct Cyc_Yyltype* _temp4071= _temp4069.curr
+ yylsp_offset; if( _temp4069.base == 0? 1:( _temp4071 < _temp4069.base? 1:
_temp4071 >= _temp4069.last_plus_one)){ _throw( Null_Exception);}* _temp4071;}).last_line);
_temp4062;}); _temp4061;})); _temp4060;}); _temp4059;})); _temp4055->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4056= yyvs; void** _temp4058= _temp4056.curr +(
yyvsp_offset - 3); if( _temp4056.base == 0? 1:( _temp4058 < _temp4056.base? 1:
_temp4058 >= _temp4056.last_plus_one)){ _throw( Null_Exception);}* _temp4058;})))->tms;
_temp4055;}); _temp4054;}); _temp4053;}); _temp4052;}); break; case 130: _LL4051: {
struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4094= yyls; struct Cyc_Yyltype* _temp4096= _temp4094.curr
+( yylsp_offset - 2); if( _temp4094.base == 0? 1:( _temp4096 < _temp4094.base? 1:
_temp4096 >= _temp4094.last_plus_one)){ _throw( Null_Exception);}* _temp4096;}).first_line,({
struct _tagged_ptr3 _temp4097= yyls; struct Cyc_Yyltype* _temp4099= _temp4097.curr
+ yylsp_offset; if( _temp4097.base == 0? 1:( _temp4099 < _temp4097.base? 1:
_temp4099 >= _temp4097.last_plus_one)){ _throw( Null_Exception);}* _temp4099;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4100= yyvs; void** _temp4102= _temp4100.curr +(
yyvsp_offset - 1); if( _temp4100.base == 0? 1:( _temp4102 < _temp4100.base? 1:
_temp4102 >= _temp4100.last_plus_one)){ _throw( Null_Exception);}* _temp4102;}))));
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4076=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4076[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4077; _temp4077.tag= Cyc_Declarator_tok;
_temp4077.f1=({ struct Cyc_Parse_Declarator* _temp4078=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4078->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4091= yyvs; void** _temp4093= _temp4091.curr +(
yyvsp_offset - 3); if( _temp4091.base == 0? 1:( _temp4093 < _temp4091.base? 1:
_temp4093 >= _temp4091.last_plus_one)){ _throw( Null_Exception);}* _temp4093;})))->id;
_temp4078->tms=({ struct Cyc_List_List* _temp4079=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4079->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4083=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4083[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4084; _temp4084.tag= Cyc_Absyn_TypeParams_mod;
_temp4084.f1= ts; _temp4084.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4085= yyls; struct Cyc_Yyltype* _temp4087= _temp4085.curr +(
yylsp_offset - 3); if( _temp4085.base == 0? 1:( _temp4087 < _temp4085.base? 1:
_temp4087 >= _temp4085.last_plus_one)){ _throw( Null_Exception);}* _temp4087;}).first_line,({
struct _tagged_ptr3 _temp4088= yyls; struct Cyc_Yyltype* _temp4090= _temp4088.curr
+ yylsp_offset; if( _temp4088.base == 0? 1:( _temp4090 < _temp4088.base? 1:
_temp4090 >= _temp4088.last_plus_one)){ _throw( Null_Exception);}* _temp4090;}).last_line);
_temp4084.f3= 0; _temp4084;}); _temp4083;})); _temp4079->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4080= yyvs; void** _temp4082= _temp4080.curr +(
yyvsp_offset - 3); if( _temp4080.base == 0? 1:( _temp4082 < _temp4080.base? 1:
_temp4082 >= _temp4080.last_plus_one)){ _throw( Null_Exception);}* _temp4082;})))->tms;
_temp4079;}); _temp4078;}); _temp4077;}); _temp4076;}); break;} case 131:
_LL4075:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp4104= Cyc_Parse_lbuf;
if( _temp4104 == 0){ _throw( Null_Exception);} _temp4104->v;}))->lex_curr_pos -=
1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4123= yyls; struct Cyc_Yyltype* _temp4125= _temp4123.curr
+( yylsp_offset - 2); if( _temp4123.base == 0? 1:( _temp4125 < _temp4123.base? 1:
_temp4125 >= _temp4123.last_plus_one)){ _throw( Null_Exception);}* _temp4125;}).first_line,({
struct _tagged_ptr3 _temp4126= yyls; struct Cyc_Yyltype* _temp4128= _temp4126.curr
+ yylsp_offset; if( _temp4126.base == 0? 1:( _temp4128 < _temp4126.base? 1:
_temp4128 >= _temp4126.last_plus_one)){ _throw( Null_Exception);}* _temp4128;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4129= yyvs; void** _temp4131= _temp4129.curr +(
yyvsp_offset - 1); if( _temp4129.base == 0? 1:( _temp4131 < _temp4129.base? 1:
_temp4131 >= _temp4129.last_plus_one)){ _throw( Null_Exception);}* _temp4131;}))));
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4105=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4105[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4106; _temp4106.tag= Cyc_Declarator_tok;
_temp4106.f1=({ struct Cyc_Parse_Declarator* _temp4107=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4107->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4120= yyvs; void** _temp4122= _temp4120.curr +(
yyvsp_offset - 3); if( _temp4120.base == 0? 1:( _temp4122 < _temp4120.base? 1:
_temp4122 >= _temp4120.last_plus_one)){ _throw( Null_Exception);}* _temp4122;})))->id;
_temp4107->tms=({ struct Cyc_List_List* _temp4108=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4108->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4112=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4112[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4113; _temp4113.tag= Cyc_Absyn_TypeParams_mod;
_temp4113.f1= ts; _temp4113.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4114= yyls; struct Cyc_Yyltype* _temp4116= _temp4114.curr +(
yylsp_offset - 3); if( _temp4114.base == 0? 1:( _temp4116 < _temp4114.base? 1:
_temp4116 >= _temp4114.last_plus_one)){ _throw( Null_Exception);}* _temp4116;}).first_line,({
struct _tagged_ptr3 _temp4117= yyls; struct Cyc_Yyltype* _temp4119= _temp4117.curr
+ yylsp_offset; if( _temp4117.base == 0? 1:( _temp4119 < _temp4117.base? 1:
_temp4119 >= _temp4117.last_plus_one)){ _throw( Null_Exception);}* _temp4119;}).last_line);
_temp4113.f3= 0; _temp4113;}); _temp4112;})); _temp4108->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4109= yyvs; void** _temp4111= _temp4109.curr +(
yyvsp_offset - 3); if( _temp4109.base == 0? 1:( _temp4111 < _temp4109.base? 1:
_temp4111 >= _temp4109.last_plus_one)){ _throw( Null_Exception);}* _temp4111;})))->tms;
_temp4108;}); _temp4107;}); _temp4106;}); _temp4105;}); break;} case 132:
_LL4103: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4133=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4133[ 0]=({ struct Cyc_Declarator_tok_struct _temp4134; _temp4134.tag= Cyc_Declarator_tok;
_temp4134.f1=({ struct Cyc_Parse_Declarator* _temp4135=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4135->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4151= yyvs; void** _temp4153= _temp4151.curr +(
yyvsp_offset - 1); if( _temp4151.base == 0? 1:( _temp4153 < _temp4151.base? 1:
_temp4153 >= _temp4151.last_plus_one)){ _throw( Null_Exception);}* _temp4153;})))->id;
_temp4135->tms=({ struct Cyc_List_List* _temp4136=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4136->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp4140=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp4140[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp4141; _temp4141.tag= Cyc_Absyn_Attributes_mod;
_temp4141.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4142= yyls;
struct Cyc_Yyltype* _temp4144= _temp4142.curr + yylsp_offset; if( _temp4142.base
== 0? 1:( _temp4144 < _temp4142.base? 1: _temp4144 >= _temp4142.last_plus_one)){
_throw( Null_Exception);}* _temp4144;}).first_line,({ struct _tagged_ptr3
_temp4145= yyls; struct Cyc_Yyltype* _temp4147= _temp4145.curr + yylsp_offset;
if( _temp4145.base == 0? 1:( _temp4147 < _temp4145.base? 1: _temp4147 >=
_temp4145.last_plus_one)){ _throw( Null_Exception);}* _temp4147;}).last_line);
_temp4141.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4148= yyvs;
void** _temp4150= _temp4148.curr + yyvsp_offset; if( _temp4148.base == 0? 1:(
_temp4150 < _temp4148.base? 1: _temp4150 >= _temp4148.last_plus_one)){ _throw(
Null_Exception);}* _temp4150;})); _temp4141;}); _temp4140;})); _temp4136->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4137= yyvs; void**
_temp4139= _temp4137.curr +( yyvsp_offset - 1); if( _temp4137.base == 0? 1:(
_temp4139 < _temp4137.base? 1: _temp4139 >= _temp4137.last_plus_one)){ _throw(
Null_Exception);}* _temp4139;})))->tms; _temp4136;}); _temp4135;}); _temp4134;});
_temp4133;}); break; case 133: _LL4132: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4155=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4155[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4156; _temp4156.tag= Cyc_TypeModifierList_tok; _temp4156.f1=({ struct Cyc_List_List*
_temp4157=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4157->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4167=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4167[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4168; _temp4168.tag=
Cyc_Absyn_Pointer_mod; _temp4168.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4169= yyvs; void** _temp4171= _temp4169.curr +(
yyvsp_offset - 2); if( _temp4169.base == 0? 1:( _temp4171 < _temp4169.base? 1:
_temp4171 >= _temp4169.last_plus_one)){ _throw( Null_Exception);}* _temp4171;}));
_temp4168.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4172= yyvs;
void** _temp4174= _temp4172.curr +( yyvsp_offset - 1); if( _temp4172.base == 0?
1:( _temp4174 < _temp4172.base? 1: _temp4174 >= _temp4172.last_plus_one)){
_throw( Null_Exception);}* _temp4174;})); _temp4168.f3= Cyc_Absyn_empty_tqual();
_temp4168;}); _temp4167;})); _temp4157->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4158= yyls; struct Cyc_Yyltype* _temp4160= _temp4158.curr
+ yylsp_offset; if( _temp4158.base == 0? 1:( _temp4160 < _temp4158.base? 1:
_temp4160 >= _temp4158.last_plus_one)){ _throw( Null_Exception);}* _temp4160;}).first_line,({
struct _tagged_ptr3 _temp4161= yyls; struct Cyc_Yyltype* _temp4163= _temp4161.curr
+ yylsp_offset; if( _temp4161.base == 0? 1:( _temp4163 < _temp4161.base? 1:
_temp4163 >= _temp4161.last_plus_one)){ _throw( Null_Exception);}* _temp4163;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4164= yyvs; void**
_temp4166= _temp4164.curr + yyvsp_offset; if( _temp4164.base == 0? 1:( _temp4166
< _temp4164.base? 1: _temp4166 >= _temp4164.last_plus_one)){ _throw(
Null_Exception);}* _temp4166;})), 0); _temp4157;}); _temp4156;}); _temp4155;});
break; case 134: _LL4154: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4176=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4176[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4177; _temp4177.tag= Cyc_TypeModifierList_tok; _temp4177.f1=({ struct Cyc_List_List*
_temp4178=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4178->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4188=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4188[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4189; _temp4189.tag=
Cyc_Absyn_Pointer_mod; _temp4189.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4190= yyvs; void** _temp4192= _temp4190.curr +(
yyvsp_offset - 3); if( _temp4190.base == 0? 1:( _temp4192 < _temp4190.base? 1:
_temp4192 >= _temp4190.last_plus_one)){ _throw( Null_Exception);}* _temp4192;}));
_temp4189.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4193= yyvs;
void** _temp4195= _temp4193.curr +( yyvsp_offset - 2); if( _temp4193.base == 0?
1:( _temp4195 < _temp4193.base? 1: _temp4195 >= _temp4193.last_plus_one)){
_throw( Null_Exception);}* _temp4195;})); _temp4189.f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp4196= yyvs; void** _temp4198= _temp4196.curr +
yyvsp_offset; if( _temp4196.base == 0? 1:( _temp4198 < _temp4196.base? 1:
_temp4198 >= _temp4196.last_plus_one)){ _throw( Null_Exception);}* _temp4198;}));
_temp4189;}); _temp4188;})); _temp4178->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4179= yyls; struct Cyc_Yyltype* _temp4181= _temp4179.curr
+( yylsp_offset - 1); if( _temp4179.base == 0? 1:( _temp4181 < _temp4179.base? 1:
_temp4181 >= _temp4179.last_plus_one)){ _throw( Null_Exception);}* _temp4181;}).first_line,({
struct _tagged_ptr3 _temp4182= yyls; struct Cyc_Yyltype* _temp4184= _temp4182.curr
+( yylsp_offset - 1); if( _temp4182.base == 0? 1:( _temp4184 < _temp4182.base? 1:
_temp4184 >= _temp4182.last_plus_one)){ _throw( Null_Exception);}* _temp4184;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4185= yyvs; void**
_temp4187= _temp4185.curr +( yyvsp_offset - 1); if( _temp4185.base == 0? 1:(
_temp4187 < _temp4185.base? 1: _temp4187 >= _temp4185.last_plus_one)){ _throw(
Null_Exception);}* _temp4187;})), 0); _temp4178;}); _temp4177;}); _temp4176;});
break; case 135: _LL4175: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4200=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4200[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4201; _temp4201.tag= Cyc_TypeModifierList_tok; _temp4201.f1=({ struct Cyc_List_List*
_temp4202=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4202->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4215=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4215[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4216; _temp4216.tag=
Cyc_Absyn_Pointer_mod; _temp4216.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4217= yyvs; void** _temp4219= _temp4217.curr +(
yyvsp_offset - 3); if( _temp4217.base == 0? 1:( _temp4219 < _temp4217.base? 1:
_temp4219 >= _temp4217.last_plus_one)){ _throw( Null_Exception);}* _temp4219;}));
_temp4216.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4220= yyvs;
void** _temp4222= _temp4220.curr +( yyvsp_offset - 2); if( _temp4220.base == 0?
1:( _temp4222 < _temp4220.base? 1: _temp4222 >= _temp4220.last_plus_one)){
_throw( Null_Exception);}* _temp4222;})); _temp4216.f3= Cyc_Absyn_empty_tqual();
_temp4216;}); _temp4215;})); _temp4202->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4203= yyls; struct Cyc_Yyltype* _temp4205= _temp4203.curr
+( yylsp_offset - 1); if( _temp4203.base == 0? 1:( _temp4205 < _temp4203.base? 1:
_temp4205 >= _temp4203.last_plus_one)){ _throw( Null_Exception);}* _temp4205;}).first_line,({
struct _tagged_ptr3 _temp4206= yyls; struct Cyc_Yyltype* _temp4208= _temp4206.curr
+( yylsp_offset - 1); if( _temp4206.base == 0? 1:( _temp4208 < _temp4206.base? 1:
_temp4208 >= _temp4206.last_plus_one)){ _throw( Null_Exception);}* _temp4208;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4209= yyvs; void**
_temp4211= _temp4209.curr +( yyvsp_offset - 1); if( _temp4209.base == 0? 1:(
_temp4211 < _temp4209.base? 1: _temp4211 >= _temp4209.last_plus_one)){ _throw(
Null_Exception);}* _temp4211;})), Cyc_yyget_TypeModifierList_tok(({ struct
_tagged_ptr2 _temp4212= yyvs; void** _temp4214= _temp4212.curr + yyvsp_offset;
if( _temp4212.base == 0? 1:( _temp4214 < _temp4212.base? 1: _temp4214 >=
_temp4212.last_plus_one)){ _throw( Null_Exception);}* _temp4214;}))); _temp4202;});
_temp4201;}); _temp4200;}); break; case 136: _LL4199: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp4224=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4224[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4225; _temp4225.tag= Cyc_TypeModifierList_tok;
_temp4225.f1=({ struct Cyc_List_List* _temp4226=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4226->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4239=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4239[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4240; _temp4240.tag= Cyc_Absyn_Pointer_mod;
_temp4240.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4241= yyvs; void** _temp4243= _temp4241.curr +( yyvsp_offset - 4); if(
_temp4241.base == 0? 1:( _temp4243 < _temp4241.base? 1: _temp4243 >= _temp4241.last_plus_one)){
_throw( Null_Exception);}* _temp4243;})); _temp4240.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4244= yyvs; void** _temp4246= _temp4244.curr +(
yyvsp_offset - 3); if( _temp4244.base == 0? 1:( _temp4246 < _temp4244.base? 1:
_temp4246 >= _temp4244.last_plus_one)){ _throw( Null_Exception);}* _temp4246;}));
_temp4240.f3= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp4247= yyvs;
void** _temp4249= _temp4247.curr +( yyvsp_offset - 1); if( _temp4247.base == 0?
1:( _temp4249 < _temp4247.base? 1: _temp4249 >= _temp4247.last_plus_one)){
_throw( Null_Exception);}* _temp4249;})); _temp4240;}); _temp4239;})); _temp4226->tl=
Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4227= yyls; struct Cyc_Yyltype* _temp4229= _temp4227.curr +( yylsp_offset -
2); if( _temp4227.base == 0? 1:( _temp4229 < _temp4227.base? 1: _temp4229 >=
_temp4227.last_plus_one)){ _throw( Null_Exception);}* _temp4229;}).first_line,({
struct _tagged_ptr3 _temp4230= yyls; struct Cyc_Yyltype* _temp4232= _temp4230.curr
+( yylsp_offset - 2); if( _temp4230.base == 0? 1:( _temp4232 < _temp4230.base? 1:
_temp4232 >= _temp4230.last_plus_one)){ _throw( Null_Exception);}* _temp4232;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4233= yyvs; void**
_temp4235= _temp4233.curr +( yyvsp_offset - 2); if( _temp4233.base == 0? 1:(
_temp4235 < _temp4233.base? 1: _temp4235 >= _temp4233.last_plus_one)){ _throw(
Null_Exception);}* _temp4235;})), Cyc_yyget_TypeModifierList_tok(({ struct
_tagged_ptr2 _temp4236= yyvs; void** _temp4238= _temp4236.curr + yyvsp_offset;
if( _temp4236.base == 0? 1:( _temp4238 < _temp4236.base? 1: _temp4238 >=
_temp4236.last_plus_one)){ _throw( Null_Exception);}* _temp4238;}))); _temp4226;});
_temp4225;}); _temp4224;}); break; case 137: _LL4223: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp4251=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4251[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4252; _temp4252.tag= Cyc_Pointer_Sort_tok;
_temp4252.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp4253=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4253[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4254; _temp4254.tag=
Cyc_Absyn_Nullable_ps; _temp4254.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4255= yyls; struct Cyc_Yyltype* _temp4257= _temp4255.curr
+ yylsp_offset; if( _temp4255.base == 0? 1:( _temp4257 < _temp4255.base? 1:
_temp4257 >= _temp4255.last_plus_one)){ _throw( Null_Exception);}* _temp4257;}).first_line,({
struct _tagged_ptr3 _temp4258= yyls; struct Cyc_Yyltype* _temp4260= _temp4258.curr
+ yylsp_offset; if( _temp4258.base == 0? 1:( _temp4260 < _temp4258.base? 1:
_temp4260 >= _temp4258.last_plus_one)){ _throw( Null_Exception);}* _temp4260;}).last_line));
_temp4254;}); _temp4253;})); _temp4252;}); _temp4251;}); break; case 138:
_LL4250: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4262=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4262[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4263; _temp4263.tag=
Cyc_Pointer_Sort_tok; _temp4263.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4264=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4264[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4265; _temp4265.tag= Cyc_Absyn_NonNullable_ps; _temp4265.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4266= yyls; struct
Cyc_Yyltype* _temp4268= _temp4266.curr + yylsp_offset; if( _temp4266.base == 0?
1:( _temp4268 < _temp4266.base? 1: _temp4268 >= _temp4266.last_plus_one)){
_throw( Null_Exception);}* _temp4268;}).first_line,({ struct _tagged_ptr3
_temp4269= yyls; struct Cyc_Yyltype* _temp4271= _temp4269.curr + yylsp_offset;
if( _temp4269.base == 0? 1:( _temp4271 < _temp4269.base? 1: _temp4271 >=
_temp4269.last_plus_one)){ _throw( Null_Exception);}* _temp4271;}).last_line));
_temp4265;}); _temp4264;})); _temp4263;}); _temp4262;}); break; case 139:
_LL4261: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4273=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4273[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4274; _temp4274.tag=
Cyc_Pointer_Sort_tok; _temp4274.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4275=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4275[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4276; _temp4276.tag=
Cyc_Absyn_Nullable_ps; _temp4276.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4277= yyvs; void** _temp4279= _temp4277.curr +( yyvsp_offset - 1); if(
_temp4277.base == 0? 1:( _temp4279 < _temp4277.base? 1: _temp4279 >= _temp4277.last_plus_one)){
_throw( Null_Exception);}* _temp4279;})); _temp4276;}); _temp4275;})); _temp4274;});
_temp4273;}); break; case 140: _LL4272: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4281=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4281[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4282; _temp4282.tag=
Cyc_Pointer_Sort_tok; _temp4282.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4283=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4283[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4284; _temp4284.tag= Cyc_Absyn_NonNullable_ps; _temp4284.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4285= yyvs; void** _temp4287= _temp4285.curr +(
yyvsp_offset - 1); if( _temp4285.base == 0? 1:( _temp4287 < _temp4285.base? 1:
_temp4287 >= _temp4285.last_plus_one)){ _throw( Null_Exception);}* _temp4287;}));
_temp4284;}); _temp4283;})); _temp4282;}); _temp4281;}); break; case 141:
_LL4280: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4289=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4289[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4290; _temp4290.tag=
Cyc_Pointer_Sort_tok; _temp4290.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps);
_temp4290;}); _temp4289;}); break; case 142: _LL4288: yyval=( void*)({ struct
Cyc_Rgn_tok_struct* _temp4292=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp4292[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4293; _temp4293.tag= Cyc_Rgn_tok; _temp4293.f1=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp4293;}); _temp4292;}); break; case 143: _LL4291: yyval=({ struct
_tagged_ptr2 _temp4295= yyvs; void** _temp4297= _temp4295.curr + yyvsp_offset;
if( _temp4295.base == 0? 1:( _temp4297 < _temp4295.base? 1: _temp4297 >=
_temp4295.last_plus_one)){ _throw( Null_Exception);}* _temp4297;}); break; case
144: _LL4294: yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp4299=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4299[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4300; _temp4300.tag= Cyc_Rgn_tok; _temp4300.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4301= yyvs; void** _temp4303=
_temp4301.curr + yyvsp_offset; if( _temp4301.base == 0? 1:( _temp4303 <
_temp4301.base? 1: _temp4303 >= _temp4301.last_plus_one)){ _throw(
Null_Exception);}* _temp4303;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp4300;}); _temp4299;}); break; case 145: _LL4298:
if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp4305= yyvs; void** _temp4307=
_temp4305.curr + yyvsp_offset; if( _temp4305.base == 0? 1:( _temp4307 <
_temp4305.base? 1: _temp4307 >= _temp4305.last_plus_one)){ _throw(
Null_Exception);}* _temp4307;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4308=( char*)"expecting region kind\n";
struct _tagged_string _temp4309; _temp4309.curr= _temp4308; _temp4309.base=
_temp4308; _temp4309.last_plus_one= _temp4308 + 23; _temp4309;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4310= yyls; struct Cyc_Yyltype* _temp4312= _temp4310.curr
+ yylsp_offset; if( _temp4310.base == 0? 1:( _temp4312 < _temp4310.base? 1:
_temp4312 >= _temp4310.last_plus_one)){ _throw( Null_Exception);}* _temp4312;}).first_line,({
struct _tagged_ptr3 _temp4313= yyls; struct Cyc_Yyltype* _temp4315= _temp4313.curr
+ yylsp_offset; if( _temp4313.base == 0? 1:( _temp4315 < _temp4313.base? 1:
_temp4315 >= _temp4313.last_plus_one)){ _throw( Null_Exception);}* _temp4315;}).last_line));}
yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp4316=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4316[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4317; _temp4317.tag= Cyc_Rgn_tok; _temp4317.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4318= yyvs; void** _temp4320=
_temp4318.curr +( yyvsp_offset - 2); if( _temp4318.base == 0? 1:( _temp4320 <
_temp4318.base? 1: _temp4320 >= _temp4318.last_plus_one)){ _throw(
Null_Exception);}* _temp4320;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp4317;}); _temp4316;}); break; case 146: _LL4304:
yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp4322=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4322[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4323; _temp4323.tag= Cyc_Rgn_tok; _temp4323.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp4323;}); _temp4322;}); break; case 147: _LL4321:
yyval=({ struct _tagged_ptr2 _temp4325= yyvs; void** _temp4327= _temp4325.curr +
yyvsp_offset; if( _temp4325.base == 0? 1:( _temp4327 < _temp4325.base? 1:
_temp4327 >= _temp4325.last_plus_one)){ _throw( Null_Exception);}* _temp4327;});
break; case 148: _LL4324: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp4329=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp4329[ 0]=({ struct Cyc_TypeQual_tok_struct _temp4330; _temp4330.tag= Cyc_TypeQual_tok;
_temp4330.f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4331= yyvs; void** _temp4333= _temp4331.curr +( yyvsp_offset -
1); if( _temp4331.base == 0? 1:( _temp4333 < _temp4331.base? 1: _temp4333 >=
_temp4331.last_plus_one)){ _throw( Null_Exception);}* _temp4333;})), Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp4334= yyvs; void** _temp4336= _temp4334.curr +
yyvsp_offset; if( _temp4334.base == 0? 1:( _temp4336 < _temp4334.base? 1:
_temp4336 >= _temp4334.last_plus_one)){ _throw( Null_Exception);}* _temp4336;})));
_temp4330;}); _temp4329;}); break; case 149: _LL4328: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp4338=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp4338[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp4339; _temp4339.tag= Cyc_ParamDeclListBool_tok;
_temp4339.f1=({ struct _tuple16* _temp4340=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp4340->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4341= yyvs; void** _temp4343= _temp4341.curr + yyvsp_offset; if( _temp4341.base
== 0? 1:( _temp4343 < _temp4341.base? 1: _temp4343 >= _temp4341.last_plus_one)){
_throw( Null_Exception);}* _temp4343;}))); _temp4340->f2= 0; _temp4340->f3= 0;
_temp4340;}); _temp4339;}); _temp4338;}); break; case 150: _LL4337: yyval=( void*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp4345=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp4345[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp4346; _temp4346.tag= Cyc_ParamDeclListBool_tok;
_temp4346.f1=({ struct _tuple16* _temp4347=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp4347->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4348= yyvs; void** _temp4350= _temp4348.curr +( yyvsp_offset - 2); if(
_temp4348.base == 0? 1:( _temp4350 < _temp4348.base? 1: _temp4350 >= _temp4348.last_plus_one)){
_throw( Null_Exception);}* _temp4350;}))); _temp4347->f2= 1; _temp4347->f3= 0;
_temp4347;}); _temp4346;}); _temp4345;}); break; case 151: _LL4344: yyval=( void*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp4352=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp4352[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp4353; _temp4353.tag= Cyc_ParamDeclListBool_tok;
_temp4353.f1=({ struct _tuple16* _temp4354=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp4354->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4361= yyvs; void** _temp4363= _temp4361.curr +( yyvsp_offset - 2); if(
_temp4361.base == 0? 1:( _temp4363 < _temp4361.base? 1: _temp4363 >= _temp4361.last_plus_one)){
_throw( Null_Exception);}* _temp4363;}))); _temp4354->f2= 0; _temp4354->f3=(
struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp4355=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4355->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp4356=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4356[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4357; _temp4357.tag= Cyc_Absyn_JoinEff; _temp4357.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4358= yyvs; void** _temp4360=
_temp4358.curr + yyvsp_offset; if( _temp4358.base == 0? 1:( _temp4360 <
_temp4358.base? 1: _temp4360 >= _temp4358.last_plus_one)){ _throw(
Null_Exception);}* _temp4360;})); _temp4357;}); _temp4356;})); _temp4355;});
_temp4354;}); _temp4353;}); _temp4352;}); break; case 152: _LL4351: yyval=({
struct _tagged_ptr2 _temp4365= yyvs; void** _temp4367= _temp4365.curr +
yyvsp_offset; if( _temp4365.base == 0? 1:( _temp4367 < _temp4365.base? 1:
_temp4367 >= _temp4365.last_plus_one)){ _throw( Null_Exception);}* _temp4367;});
break; case 153: _LL4364: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4369=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4369[ 0]=({ struct Cyc_TypeList_tok_struct _temp4370; _temp4370.tag= Cyc_TypeList_tok;
_temp4370.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2
_temp4371= yyvs; void** _temp4373= _temp4371.curr +( yyvsp_offset - 2); if(
_temp4371.base == 0? 1:( _temp4373 < _temp4371.base? 1: _temp4373 >= _temp4371.last_plus_one)){
_throw( Null_Exception);}* _temp4373;})), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4374= yyvs; void** _temp4376= _temp4374.curr + yyvsp_offset;
if( _temp4374.base == 0? 1:( _temp4376 < _temp4374.base? 1: _temp4376 >=
_temp4374.last_plus_one)){ _throw( Null_Exception);}* _temp4376;}))); _temp4370;});
_temp4369;}); break; case 154: _LL4368: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4378=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4378[ 0]=({ struct Cyc_TypeList_tok_struct _temp4379; _temp4379.tag= Cyc_TypeList_tok;
_temp4379.f1= 0; _temp4379;}); _temp4378;}); break; case 155: _LL4377: yyval=({
struct _tagged_ptr2 _temp4381= yyvs; void** _temp4383= _temp4381.curr +(
yyvsp_offset - 1); if( _temp4381.base == 0? 1:( _temp4383 < _temp4381.base? 1:
_temp4383 >= _temp4381.last_plus_one)){ _throw( Null_Exception);}* _temp4383;});
break; case 156: _LL4380: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4385=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4385[ 0]=({ struct Cyc_TypeList_tok_struct _temp4386; _temp4386.tag= Cyc_TypeList_tok;
_temp4386.f1=({ struct Cyc_List_List* _temp4387=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4387->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4388= yyvs; void** _temp4390=
_temp4388.curr + yyvsp_offset; if( _temp4388.base == 0? 1:( _temp4390 <
_temp4388.base? 1: _temp4390 >= _temp4388.last_plus_one)){ _throw(
Null_Exception);}* _temp4390;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_EffKind)); _temp4387->tl= 0; _temp4387;}); _temp4386;});
_temp4385;}); break; case 157: _LL4384: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4392= yyvs; void** _temp4394= _temp4392.curr + yyvsp_offset;
if( _temp4392.base == 0? 1:( _temp4394 < _temp4392.base? 1: _temp4394 >=
_temp4392.last_plus_one)){ _throw( Null_Exception);}* _temp4394;})) !=( void*)
Cyc_Absyn_EffKind){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp4395=(
char*)"expecing effect kind (E)"; struct _tagged_string _temp4396; _temp4396.curr=
_temp4395; _temp4396.base= _temp4395; _temp4396.last_plus_one= _temp4395 + 25;
_temp4396;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4397= yyls;
struct Cyc_Yyltype* _temp4399= _temp4397.curr + yylsp_offset; if( _temp4397.base
== 0? 1:( _temp4399 < _temp4397.base? 1: _temp4399 >= _temp4397.last_plus_one)){
_throw( Null_Exception);}* _temp4399;}).first_line,({ struct _tagged_ptr3
_temp4400= yyls; struct Cyc_Yyltype* _temp4402= _temp4400.curr + yylsp_offset;
if( _temp4400.base == 0? 1:( _temp4402 < _temp4400.base? 1: _temp4402 >=
_temp4400.last_plus_one)){ _throw( Null_Exception);}* _temp4402;}).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4403=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4403[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4404; _temp4404.tag= Cyc_TypeList_tok; _temp4404.f1=({ struct Cyc_List_List*
_temp4405=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4405->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4406= yyvs; void** _temp4408= _temp4406.curr +( yyvsp_offset -
2); if( _temp4406.base == 0? 1:( _temp4408 < _temp4406.base? 1: _temp4408 >=
_temp4406.last_plus_one)){ _throw( Null_Exception);}* _temp4408;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp4405->tl= 0; _temp4405;}); _temp4404;}); _temp4403;}); break; case 158:
_LL4391: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4410=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4410[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4411; _temp4411.tag= Cyc_TypeList_tok; _temp4411.f1=({ struct Cyc_List_List*
_temp4412=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4412->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4413=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4413[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4414; _temp4414.tag= Cyc_Absyn_AccessEff;
_temp4414.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4415= yyvs; void** _temp4417= _temp4415.curr + yyvsp_offset;
if( _temp4415.base == 0? 1:( _temp4417 < _temp4415.base? 1: _temp4417 >=
_temp4415.last_plus_one)){ _throw( Null_Exception);}* _temp4417;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4414;}); _temp4413;})); _temp4412->tl= 0; _temp4412;}); _temp4411;});
_temp4410;}); break; case 159: _LL4409: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4419=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4419[ 0]=({ struct Cyc_TypeList_tok_struct _temp4420; _temp4420.tag= Cyc_TypeList_tok;
_temp4420.f1=({ struct Cyc_List_List* _temp4421=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4421->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4425=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4425[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4426; _temp4426.tag= Cyc_Absyn_AccessEff;
_temp4426.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4427= yyvs; void** _temp4429= _temp4427.curr +( yyvsp_offset -
2); if( _temp4427.base == 0? 1:( _temp4429 < _temp4427.base? 1: _temp4429 >=
_temp4427.last_plus_one)){ _throw( Null_Exception);}* _temp4429;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4426;}); _temp4425;})); _temp4421->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4422= yyvs; void** _temp4424= _temp4422.curr + yyvsp_offset;
if( _temp4422.base == 0? 1:( _temp4424 < _temp4422.base? 1: _temp4424 >=
_temp4422.last_plus_one)){ _throw( Null_Exception);}* _temp4424;})); _temp4421;});
_temp4420;}); _temp4419;}); break; case 160: _LL4418: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4431= yyvs; void** _temp4433= _temp4431.curr +
yyvsp_offset; if( _temp4431.base == 0? 1:( _temp4433 < _temp4431.base? 1:
_temp4433 >= _temp4431.last_plus_one)){ _throw( Null_Exception);}* _temp4433;}))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp4434=( char*)"expecting region kind (R)"; struct _tagged_string _temp4435;
_temp4435.curr= _temp4434; _temp4435.base= _temp4434; _temp4435.last_plus_one=
_temp4434 + 26; _temp4435;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4436= yyls; struct Cyc_Yyltype* _temp4438= _temp4436.curr + yylsp_offset;
if( _temp4436.base == 0? 1:( _temp4438 < _temp4436.base? 1: _temp4438 >=
_temp4436.last_plus_one)){ _throw( Null_Exception);}* _temp4438;}).first_line,({
struct _tagged_ptr3 _temp4439= yyls; struct Cyc_Yyltype* _temp4441= _temp4439.curr
+ yylsp_offset; if( _temp4439.base == 0? 1:( _temp4441 < _temp4439.base? 1:
_temp4441 >= _temp4439.last_plus_one)){ _throw( Null_Exception);}* _temp4441;}).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4442=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4442[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4443; _temp4443.tag= Cyc_TypeList_tok; _temp4443.f1=({ struct Cyc_List_List*
_temp4444=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4444->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4445=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4445[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4446; _temp4446.tag= Cyc_Absyn_AccessEff;
_temp4446.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4447= yyvs; void** _temp4449= _temp4447.curr +( yyvsp_offset -
2); if( _temp4447.base == 0? 1:( _temp4449 < _temp4447.base? 1: _temp4449 >=
_temp4447.last_plus_one)){ _throw( Null_Exception);}* _temp4449;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4446;}); _temp4445;})); _temp4444->tl= 0; _temp4444;}); _temp4443;});
_temp4442;}); break; case 161: _LL4430: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4451= yyvs; void** _temp4453= _temp4451.curr +( yyvsp_offset -
2); if( _temp4451.base == 0? 1:( _temp4453 < _temp4451.base? 1: _temp4453 >=
_temp4451.last_plus_one)){ _throw( Null_Exception);}* _temp4453;})) !=( void*)
Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp4454=(
char*)"expecting region kind (R)"; struct _tagged_string _temp4455; _temp4455.curr=
_temp4454; _temp4455.base= _temp4454; _temp4455.last_plus_one= _temp4454 + 26;
_temp4455;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4456= yyls;
struct Cyc_Yyltype* _temp4458= _temp4456.curr +( yylsp_offset - 2); if(
_temp4456.base == 0? 1:( _temp4458 < _temp4456.base? 1: _temp4458 >= _temp4456.last_plus_one)){
_throw( Null_Exception);}* _temp4458;}).first_line,({ struct _tagged_ptr3
_temp4459= yyls; struct Cyc_Yyltype* _temp4461= _temp4459.curr +( yylsp_offset -
2); if( _temp4459.base == 0? 1:( _temp4461 < _temp4459.base? 1: _temp4461 >=
_temp4459.last_plus_one)){ _throw( Null_Exception);}* _temp4461;}).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4462=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4462[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4463; _temp4463.tag= Cyc_TypeList_tok; _temp4463.f1=({ struct Cyc_List_List*
_temp4464=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4464->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4468=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4468[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4469; _temp4469.tag= Cyc_Absyn_AccessEff;
_temp4469.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4470= yyvs; void** _temp4472= _temp4470.curr +( yyvsp_offset -
4); if( _temp4470.base == 0? 1:( _temp4472 < _temp4470.base? 1: _temp4472 >=
_temp4470.last_plus_one)){ _throw( Null_Exception);}* _temp4472;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4469;}); _temp4468;})); _temp4464->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4465= yyvs; void** _temp4467= _temp4465.curr + yyvsp_offset;
if( _temp4465.base == 0? 1:( _temp4467 < _temp4465.base? 1: _temp4467 >=
_temp4465.last_plus_one)){ _throw( Null_Exception);}* _temp4467;})); _temp4464;});
_temp4463;}); _temp4462;}); break; case 162: _LL4450: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp4474=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp4474[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp4475; _temp4475.tag= Cyc_ParamDeclList_tok;
_temp4475.f1=({ struct Cyc_List_List* _temp4476=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4476->hd=( void*) Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp4477= yyvs; void** _temp4479= _temp4477.curr +
yyvsp_offset; if( _temp4477.base == 0? 1:( _temp4479 < _temp4477.base? 1:
_temp4479 >= _temp4477.last_plus_one)){ _throw( Null_Exception);}* _temp4479;}));
_temp4476->tl= 0; _temp4476;}); _temp4475;}); _temp4474;}); break; case 163:
_LL4473: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp4481=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4481[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4482; _temp4482.tag=
Cyc_ParamDeclList_tok; _temp4482.f1=({ struct Cyc_List_List* _temp4483=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4483->hd=( void*)
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4487= yyvs; void** _temp4489=
_temp4487.curr + yyvsp_offset; if( _temp4487.base == 0? 1:( _temp4489 <
_temp4487.base? 1: _temp4489 >= _temp4487.last_plus_one)){ _throw(
Null_Exception);}* _temp4489;})); _temp4483->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4484= yyvs; void** _temp4486= _temp4484.curr +(
yyvsp_offset - 2); if( _temp4484.base == 0? 1:( _temp4486 < _temp4484.base? 1:
_temp4486 >= _temp4484.last_plus_one)){ _throw( Null_Exception);}* _temp4486;}));
_temp4483;}); _temp4482;}); _temp4481;}); break; case 164: _LL4480: { void* t=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4553= yyvs; void** _temp4555= _temp4553.curr +( yyvsp_offset - 1); if(
_temp4553.base == 0? 1:( _temp4555 < _temp4553.base? 1: _temp4555 >= _temp4553.last_plus_one)){
_throw( Null_Exception);}* _temp4555;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4556= yyls; struct Cyc_Yyltype* _temp4558= _temp4556.curr
+( yylsp_offset - 1); if( _temp4556.base == 0? 1:( _temp4558 < _temp4556.base? 1:
_temp4558 >= _temp4556.last_plus_one)){ _throw( Null_Exception);}* _temp4558;}).first_line,({
struct _tagged_ptr3 _temp4559= yyls; struct Cyc_Yyltype* _temp4561= _temp4559.curr
+( yylsp_offset - 1); if( _temp4559.base == 0? 1:( _temp4561 < _temp4559.base? 1:
_temp4561 >= _temp4559.last_plus_one)){ _throw( Null_Exception);}* _temp4561;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4550= yyvs; void** _temp4552= _temp4550.curr +( yyvsp_offset - 1); if(
_temp4550.base == 0? 1:( _temp4552 < _temp4550.base? 1: _temp4552 >= _temp4550.last_plus_one)){
_throw( Null_Exception);}* _temp4552;}))).f3; struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4547= yyvs; void** _temp4549= _temp4547.curr +(
yyvsp_offset - 1); if( _temp4547.base == 0? 1:( _temp4549 < _temp4547.base? 1:
_temp4549 >= _temp4547.last_plus_one)){ _throw( Null_Exception);}* _temp4549;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4544= yyvs; void** _temp4546= _temp4544.curr + yyvsp_offset; if( _temp4544.base
== 0? 1:( _temp4546 < _temp4544.base? 1: _temp4546 >= _temp4544.last_plus_one)){
_throw( Null_Exception);}* _temp4546;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp4491=( char*)"parameter with bad type params"; struct _tagged_string
_temp4492; _temp4492.curr= _temp4491; _temp4492.base= _temp4491; _temp4492.last_plus_one=
_temp4491 + 31; _temp4492;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4493= yyls; struct Cyc_Yyltype* _temp4495= _temp4493.curr + yylsp_offset;
if( _temp4493.base == 0? 1:( _temp4495 < _temp4493.base? 1: _temp4495 >=
_temp4493.last_plus_one)){ _throw( Null_Exception);}* _temp4495;}).first_line,({
struct _tagged_ptr3 _temp4496= yyls; struct Cyc_Yyltype* _temp4498= _temp4496.curr
+ yylsp_offset; if( _temp4496.base == 0? 1:( _temp4498 < _temp4496.base? 1:
_temp4498 >= _temp4496.last_plus_one)){ _throw( Null_Exception);}* _temp4498;}).last_line));}{
struct _tuple1* q=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4541=
yyvs; void** _temp4543= _temp4541.curr + yyvsp_offset; if( _temp4541.base == 0?
1:( _temp4543 < _temp4541.base? 1: _temp4543 >= _temp4541.last_plus_one)){
_throw( Null_Exception);}* _temp4543;})))->id;{ void* _temp4499=(* q).f1; struct
Cyc_List_List* _temp4509; struct Cyc_List_List* _temp4511; _LL4501: if(
_temp4499 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4502;} else{ goto _LL4503;}
_LL4503: if(( unsigned int) _temp4499 > 1u?*(( int*) _temp4499) == Cyc_Absyn_Rel_n:
0){ _LL4510: _temp4509=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp4499)->f1; if( _temp4509 == 0){ goto _LL4504;} else{ goto _LL4505;}} else{
goto _LL4505;} _LL4505: if(( unsigned int) _temp4499 > 1u?*(( int*) _temp4499)
== Cyc_Absyn_Abs_n: 0){ _LL4512: _temp4511=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp4499)->f1; if( _temp4511 == 0){ goto _LL4506;} else{ goto _LL4507;}} else{
goto _LL4507;} _LL4507: goto _LL4508; _LL4502: goto _LL4500; _LL4504: goto
_LL4500; _LL4506: goto _LL4500; _LL4508: Cyc_Parse_err(( struct _tagged_string)({
char* _temp4513=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp4514; _temp4514.curr= _temp4513; _temp4514.base=
_temp4513; _temp4514.last_plus_one= _temp4513 + 49; _temp4514;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4515= yyls; struct Cyc_Yyltype* _temp4517= _temp4515.curr
+( yylsp_offset - 1); if( _temp4515.base == 0? 1:( _temp4517 < _temp4515.base? 1:
_temp4517 >= _temp4515.last_plus_one)){ _throw( Null_Exception);}* _temp4517;}).first_line,({
struct _tagged_ptr3 _temp4518= yyls; struct Cyc_Yyltype* _temp4520= _temp4518.curr
+( yylsp_offset - 1); if( _temp4518.base == 0? 1:( _temp4520 < _temp4518.base? 1:
_temp4520 >= _temp4518.last_plus_one)){ _throw( Null_Exception);}* _temp4520;}).last_line));
goto _LL4500; _LL4500:;}{ struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4540=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4540->v=( void*)(* q).f2; _temp4540;}); if( t_info.f4 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4521=( char*)"extra attributes on parameter, ignoring";
struct _tagged_string _temp4522; _temp4522.curr= _temp4521; _temp4522.base=
_temp4521; _temp4522.last_plus_one= _temp4521 + 40; _temp4522;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4523= yyls; struct Cyc_Yyltype* _temp4525= _temp4523.curr
+( yylsp_offset - 1); if( _temp4523.base == 0? 1:( _temp4525 < _temp4523.base? 1:
_temp4525 >= _temp4523.last_plus_one)){ _throw( Null_Exception);}* _temp4525;}).first_line,({
struct _tagged_ptr3 _temp4526= yyls; struct Cyc_Yyltype* _temp4528= _temp4526.curr
+ yylsp_offset; if( _temp4526.base == 0? 1:( _temp4528 < _temp4526.base? 1:
_temp4528 >= _temp4526.last_plus_one)){ _throw( Null_Exception);}* _temp4528;}).last_line));}
if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4529=(
char*)"extra type variables on parameter, ignoring"; struct _tagged_string
_temp4530; _temp4530.curr= _temp4529; _temp4530.base= _temp4529; _temp4530.last_plus_one=
_temp4529 + 44; _temp4530;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4531= yyls; struct Cyc_Yyltype* _temp4533= _temp4531.curr +( yylsp_offset -
1); if( _temp4531.base == 0? 1:( _temp4533 < _temp4531.base? 1: _temp4533 >=
_temp4531.last_plus_one)){ _throw( Null_Exception);}* _temp4533;}).first_line,({
struct _tagged_ptr3 _temp4534= yyls; struct Cyc_Yyltype* _temp4536= _temp4534.curr
+ yylsp_offset; if( _temp4534.base == 0? 1:( _temp4536 < _temp4534.base? 1:
_temp4536 >= _temp4534.last_plus_one)){ _throw( Null_Exception);}* _temp4536;}).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4537=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4537[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4538; _temp4538.tag= Cyc_ParamDecl_tok; _temp4538.f1=({
struct _tuple2* _temp4539=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4539->f1= idopt; _temp4539->f2= t_info.f1; _temp4539->f3= t_info.f2;
_temp4539;}); _temp4538;}); _temp4537;}); break;}}} case 165: _LL4490: { void* t=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4580= yyvs; void** _temp4582= _temp4580.curr + yyvsp_offset; if( _temp4580.base
== 0? 1:( _temp4582 < _temp4580.base? 1: _temp4582 >= _temp4580.last_plus_one)){
_throw( Null_Exception);}* _temp4582;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4583= yyls; struct Cyc_Yyltype* _temp4585= _temp4583.curr
+ yylsp_offset; if( _temp4583.base == 0? 1:( _temp4585 < _temp4583.base? 1:
_temp4585 >= _temp4583.last_plus_one)){ _throw( Null_Exception);}* _temp4585;}).first_line,({
struct _tagged_ptr3 _temp4586= yyls; struct Cyc_Yyltype* _temp4588= _temp4586.curr
+ yylsp_offset; if( _temp4586.base == 0? 1:( _temp4588 < _temp4586.base? 1:
_temp4588 >= _temp4586.last_plus_one)){ _throw( Null_Exception);}* _temp4588;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4577= yyvs; void** _temp4579= _temp4577.curr + yyvsp_offset; if( _temp4577.base
== 0? 1:( _temp4579 < _temp4577.base? 1: _temp4579 >= _temp4577.last_plus_one)){
_throw( Null_Exception);}* _temp4579;}))).f3; struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4574= yyvs; void** _temp4576= _temp4574.curr +
yyvsp_offset; if( _temp4574.base == 0? 1:( _temp4576 < _temp4574.base? 1:
_temp4576 >= _temp4574.last_plus_one)){ _throw( Null_Exception);}* _temp4576;}))).f1;
if( atts != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4563=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4564; _temp4564.curr= _temp4563; _temp4564.base=
_temp4563; _temp4564.last_plus_one= _temp4563 + 38; _temp4564;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4565= yyls; struct Cyc_Yyltype* _temp4567= _temp4565.curr
+ yylsp_offset; if( _temp4565.base == 0? 1:( _temp4567 < _temp4565.base? 1:
_temp4567 >= _temp4565.last_plus_one)){ _throw( Null_Exception);}* _temp4567;}).first_line,({
struct _tagged_ptr3 _temp4568= yyls; struct Cyc_Yyltype* _temp4570= _temp4568.curr
+ yylsp_offset; if( _temp4568.base == 0? 1:( _temp4570 < _temp4568.base? 1:
_temp4570 >= _temp4568.last_plus_one)){ _throw( Null_Exception);}* _temp4570;}).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4571=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4571[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4572; _temp4572.tag= Cyc_ParamDecl_tok; _temp4572.f1=({
struct _tuple2* _temp4573=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4573->f1= 0; _temp4573->f2= tq; _temp4573->f3= t; _temp4573;}); _temp4572;});
_temp4571;}); break;} case 166: _LL4562: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4618= yyvs; void**
_temp4620= _temp4618.curr +( yyvsp_offset - 1); if( _temp4618.base == 0? 1:(
_temp4620 < _temp4618.base? 1: _temp4620 >= _temp4618.last_plus_one)){ _throw(
Null_Exception);}* _temp4620;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4621= yyls; struct Cyc_Yyltype* _temp4623= _temp4621.curr +(
yylsp_offset - 1); if( _temp4621.base == 0? 1:( _temp4623 < _temp4621.base? 1:
_temp4623 >= _temp4621.last_plus_one)){ _throw( Null_Exception);}* _temp4623;}).first_line,({
struct _tagged_ptr3 _temp4624= yyls; struct Cyc_Yyltype* _temp4626= _temp4624.curr
+( yylsp_offset - 1); if( _temp4624.base == 0? 1:( _temp4626 < _temp4624.base? 1:
_temp4626 >= _temp4624.last_plus_one)){ _throw( Null_Exception);}* _temp4626;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4615= yyvs; void** _temp4617= _temp4615.curr +( yyvsp_offset - 1); if(
_temp4615.base == 0? 1:( _temp4617 < _temp4615.base? 1: _temp4617 >= _temp4615.last_plus_one)){
_throw( Null_Exception);}* _temp4617;}))).f3; struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4612= yyvs; void** _temp4614= _temp4612.curr +(
yyvsp_offset - 1); if( _temp4612.base == 0? 1:( _temp4614 < _temp4612.base? 1:
_temp4614 >= _temp4612.last_plus_one)){ _throw( Null_Exception);}* _temp4614;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4609= yyvs; void** _temp4611= _temp4609.curr + yyvsp_offset;
if( _temp4609.base == 0? 1:( _temp4611 < _temp4609.base? 1: _temp4611 >=
_temp4609.last_plus_one)){ _throw( Null_Exception);}* _temp4611;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4590=( char*)"bad type parameters on formal argument, ignoring";
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
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4606=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4606[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4607; _temp4607.tag= Cyc_ParamDecl_tok; _temp4607.f1=({
struct _tuple2* _temp4608=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4608->f1= 0; _temp4608->f2= t_info.f1; _temp4608->f3= t_info.f2; _temp4608;});
_temp4607;}); _temp4606;}); break;} case 167: _LL4589: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp4628=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp4628[ 0]=({ struct Cyc_IdList_tok_struct
_temp4629; _temp4629.tag= Cyc_IdList_tok; _temp4629.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(({ struct
_tagged_ptr2 _temp4630= yyvs; void** _temp4632= _temp4630.curr + yyvsp_offset;
if( _temp4630.base == 0? 1:( _temp4632 < _temp4630.base? 1: _temp4632 >=
_temp4630.last_plus_one)){ _throw( Null_Exception);}* _temp4632;}))); _temp4629;});
_temp4628;}); break; case 168: _LL4627: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp4634=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4634[ 0]=({ struct Cyc_IdList_tok_struct _temp4635; _temp4635.tag= Cyc_IdList_tok;
_temp4635.f1=({ struct Cyc_List_List* _temp4636=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4636->hd=( void*)({ struct
_tagged_string* _temp4637=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4637[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4638= yyvs; void** _temp4640= _temp4638.curr + yyvsp_offset; if( _temp4638.base
== 0? 1:( _temp4640 < _temp4638.base? 1: _temp4640 >= _temp4638.last_plus_one)){
_throw( Null_Exception);}* _temp4640;})); _temp4637;}); _temp4636->tl= 0;
_temp4636;}); _temp4635;}); _temp4634;}); break; case 169: _LL4633: yyval=( void*)({
struct Cyc_IdList_tok_struct* _temp4642=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp4642[ 0]=({ struct Cyc_IdList_tok_struct
_temp4643; _temp4643.tag= Cyc_IdList_tok; _temp4643.f1=({ struct Cyc_List_List*
_temp4644=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4644->hd=( void*)({ struct _tagged_string* _temp4648=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4648[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4649= yyvs; void** _temp4651=
_temp4649.curr + yyvsp_offset; if( _temp4649.base == 0? 1:( _temp4651 <
_temp4649.base? 1: _temp4651 >= _temp4649.last_plus_one)){ _throw(
Null_Exception);}* _temp4651;})); _temp4648;}); _temp4644->tl= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp4645= yyvs; void** _temp4647= _temp4645.curr +(
yyvsp_offset - 2); if( _temp4645.base == 0? 1:( _temp4647 < _temp4645.base? 1:
_temp4647 >= _temp4645.last_plus_one)){ _throw( Null_Exception);}* _temp4647;}));
_temp4644;}); _temp4643;}); _temp4642;}); break; case 170: _LL4641: yyval=({
struct _tagged_ptr2 _temp4653= yyvs; void** _temp4655= _temp4653.curr +
yyvsp_offset; if( _temp4653.base == 0? 1:( _temp4655 < _temp4653.base? 1:
_temp4655 >= _temp4653.last_plus_one)){ _throw( Null_Exception);}* _temp4655;});
break; case 171: _LL4652: yyval=({ struct _tagged_ptr2 _temp4657= yyvs; void**
_temp4659= _temp4657.curr + yyvsp_offset; if( _temp4657.base == 0? 1:( _temp4659
< _temp4657.base? 1: _temp4659 >= _temp4657.last_plus_one)){ _throw(
Null_Exception);}* _temp4659;}); break; case 172: _LL4656: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4661=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4661[ 0]=({ struct Cyc_Exp_tok_struct
_temp4662; _temp4662.tag= Cyc_Exp_tok; _temp4662.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4663=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4663[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4664; _temp4664.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4664.f1= 0; _temp4664.f2= 0; _temp4664;}); _temp4663;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4665= yyls; struct Cyc_Yyltype* _temp4667= _temp4665.curr
+( yylsp_offset - 1); if( _temp4665.base == 0? 1:( _temp4667 < _temp4665.base? 1:
_temp4667 >= _temp4665.last_plus_one)){ _throw( Null_Exception);}* _temp4667;}).first_line,({
struct _tagged_ptr3 _temp4668= yyls; struct Cyc_Yyltype* _temp4670= _temp4668.curr
+ yylsp_offset; if( _temp4668.base == 0? 1:( _temp4670 < _temp4668.base? 1:
_temp4670 >= _temp4668.last_plus_one)){ _throw( Null_Exception);}* _temp4670;}).last_line));
_temp4662;}); _temp4661;}); break; case 173: _LL4660: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4672=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4672[ 0]=({ struct Cyc_Exp_tok_struct
_temp4673; _temp4673.tag= Cyc_Exp_tok; _temp4673.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4674=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4674[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4675; _temp4675.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4675.f1= 0; _temp4675.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4676= yyvs; void** _temp4678= _temp4676.curr +( yyvsp_offset - 1); if(
_temp4676.base == 0? 1:( _temp4678 < _temp4676.base? 1: _temp4678 >= _temp4676.last_plus_one)){
_throw( Null_Exception);}* _temp4678;}))); _temp4675;}); _temp4674;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4679= yyls; struct Cyc_Yyltype* _temp4681= _temp4679.curr
+( yylsp_offset - 2); if( _temp4679.base == 0? 1:( _temp4681 < _temp4679.base? 1:
_temp4681 >= _temp4679.last_plus_one)){ _throw( Null_Exception);}* _temp4681;}).first_line,({
struct _tagged_ptr3 _temp4682= yyls; struct Cyc_Yyltype* _temp4684= _temp4682.curr
+ yylsp_offset; if( _temp4682.base == 0? 1:( _temp4684 < _temp4682.base? 1:
_temp4684 >= _temp4682.last_plus_one)){ _throw( Null_Exception);}* _temp4684;}).last_line));
_temp4673;}); _temp4672;}); break; case 174: _LL4671: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4686=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4686[ 0]=({ struct Cyc_Exp_tok_struct
_temp4687; _temp4687.tag= Cyc_Exp_tok; _temp4687.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4688=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4688[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4689; _temp4689.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4689.f1= 0; _temp4689.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4690= yyvs; void** _temp4692= _temp4690.curr +( yyvsp_offset - 2); if(
_temp4690.base == 0? 1:( _temp4692 < _temp4690.base? 1: _temp4692 >= _temp4690.last_plus_one)){
_throw( Null_Exception);}* _temp4692;}))); _temp4689;}); _temp4688;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4693= yyls; struct Cyc_Yyltype* _temp4695= _temp4693.curr
+( yylsp_offset - 3); if( _temp4693.base == 0? 1:( _temp4695 < _temp4693.base? 1:
_temp4695 >= _temp4693.last_plus_one)){ _throw( Null_Exception);}* _temp4695;}).first_line,({
struct _tagged_ptr3 _temp4696= yyls; struct Cyc_Yyltype* _temp4698= _temp4696.curr
+ yylsp_offset; if( _temp4696.base == 0? 1:( _temp4698 < _temp4696.base? 1:
_temp4698 >= _temp4696.last_plus_one)){ _throw( Null_Exception);}* _temp4698;}).last_line));
_temp4687;}); _temp4686;}); break; case 175: _LL4685: { struct Cyc_Absyn_Vardecl*
vd= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp4717=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp4717->f1=( void*) Cyc_Absyn_Loc_n;
_temp4717->f2=({ struct _tagged_string* _temp4718=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4718[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4719= yyvs; void** _temp4721= _temp4719.curr +(
yyvsp_offset - 5); if( _temp4719.base == 0? 1:( _temp4721 < _temp4719.base? 1:
_temp4721 >= _temp4719.last_plus_one)){ _throw( Null_Exception);}* _temp4721;}));
_temp4718;}); _temp4717;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4722= yyls; struct
Cyc_Yyltype* _temp4724= _temp4722.curr +( yylsp_offset - 5); if( _temp4722.base
== 0? 1:( _temp4724 < _temp4722.base? 1: _temp4724 >= _temp4722.last_plus_one)){
_throw( Null_Exception);}* _temp4724;}).first_line,({ struct _tagged_ptr3
_temp4725= yyls; struct Cyc_Yyltype* _temp4727= _temp4725.curr +( yylsp_offset -
5); if( _temp4725.base == 0? 1:( _temp4727 < _temp4725.base? 1: _temp4727 >=
_temp4725.last_plus_one)){ _throw( Null_Exception);}* _temp4727;}).last_line)));
vd->tq=({ struct Cyc_Absyn_Tqual _temp4700; _temp4700.q_const= 1; _temp4700.q_volatile=
0; _temp4700.q_restrict= 1; _temp4700;}); yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4701=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4701[ 0]=({ struct Cyc_Exp_tok_struct _temp4702; _temp4702.tag= Cyc_Exp_tok;
_temp4702.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Comprehension_e_struct*
_temp4703=( struct Cyc_Absyn_Comprehension_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Comprehension_e_struct)); _temp4703[ 0]=({ struct Cyc_Absyn_Comprehension_e_struct
_temp4704; _temp4704.tag= Cyc_Absyn_Comprehension_e; _temp4704.f1= vd; _temp4704.f2=
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4705= yyvs; void** _temp4707=
_temp4705.curr +( yyvsp_offset - 3); if( _temp4705.base == 0? 1:( _temp4707 <
_temp4705.base? 1: _temp4707 >= _temp4705.last_plus_one)){ _throw(
Null_Exception);}* _temp4707;})); _temp4704.f3= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4708= yyvs; void** _temp4710= _temp4708.curr +( yyvsp_offset -
1); if( _temp4708.base == 0? 1:( _temp4710 < _temp4708.base? 1: _temp4710 >=
_temp4708.last_plus_one)){ _throw( Null_Exception);}* _temp4710;})); _temp4704;});
_temp4703;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4711= yyls;
struct Cyc_Yyltype* _temp4713= _temp4711.curr +( yylsp_offset - 7); if(
_temp4711.base == 0? 1:( _temp4713 < _temp4711.base? 1: _temp4713 >= _temp4711.last_plus_one)){
_throw( Null_Exception);}* _temp4713;}).first_line,({ struct _tagged_ptr3
_temp4714= yyls; struct Cyc_Yyltype* _temp4716= _temp4714.curr + yylsp_offset;
if( _temp4714.base == 0? 1:( _temp4716 < _temp4714.base? 1: _temp4716 >=
_temp4714.last_plus_one)){ _throw( Null_Exception);}* _temp4716;}).last_line));
_temp4702;}); _temp4701;}); break;} case 176: _LL4699: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp4729=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4729[ 0]=({
struct Cyc_InitializerList_tok_struct _temp4730; _temp4730.tag= Cyc_InitializerList_tok;
_temp4730.f1=({ struct Cyc_List_List* _temp4731=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4731->hd=( void*)({ struct
_tuple18* _temp4732=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4732->f1= 0; _temp4732->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4733= yyvs; void** _temp4735= _temp4733.curr + yyvsp_offset; if( _temp4733.base
== 0? 1:( _temp4735 < _temp4733.base? 1: _temp4735 >= _temp4733.last_plus_one)){
_throw( Null_Exception);}* _temp4735;})); _temp4732;}); _temp4731->tl= 0;
_temp4731;}); _temp4730;}); _temp4729;}); break; case 177: _LL4728: yyval=( void*)({
struct Cyc_InitializerList_tok_struct* _temp4737=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4737[ 0]=({
struct Cyc_InitializerList_tok_struct _temp4738; _temp4738.tag= Cyc_InitializerList_tok;
_temp4738.f1=({ struct Cyc_List_List* _temp4739=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4739->hd=( void*)({ struct
_tuple18* _temp4740=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4740->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4744=
yyvs; void** _temp4746= _temp4744.curr +( yyvsp_offset - 1); if( _temp4744.base
== 0? 1:( _temp4746 < _temp4744.base? 1: _temp4746 >= _temp4744.last_plus_one)){
_throw( Null_Exception);}* _temp4746;})); _temp4740->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4741= yyvs; void** _temp4743= _temp4741.curr +
yyvsp_offset; if( _temp4741.base == 0? 1:( _temp4743 < _temp4741.base? 1:
_temp4743 >= _temp4741.last_plus_one)){ _throw( Null_Exception);}* _temp4743;}));
_temp4740;}); _temp4739->tl= 0; _temp4739;}); _temp4738;}); _temp4737;}); break;
case 178: _LL4736: yyval=( void*)({ struct Cyc_InitializerList_tok_struct*
_temp4748=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4748[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4749; _temp4749.tag= Cyc_InitializerList_tok; _temp4749.f1=({ struct Cyc_List_List*
_temp4750=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4750->hd=( void*)({ struct _tuple18* _temp4754=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4754->f1= 0; _temp4754->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4755= yyvs; void** _temp4757= _temp4755.curr +
yyvsp_offset; if( _temp4755.base == 0? 1:( _temp4757 < _temp4755.base? 1:
_temp4757 >= _temp4755.last_plus_one)){ _throw( Null_Exception);}* _temp4757;}));
_temp4754;}); _temp4750->tl= Cyc_yyget_InitializerList_tok(({ struct
_tagged_ptr2 _temp4751= yyvs; void** _temp4753= _temp4751.curr +( yyvsp_offset -
2); if( _temp4751.base == 0? 1:( _temp4753 < _temp4751.base? 1: _temp4753 >=
_temp4751.last_plus_one)){ _throw( Null_Exception);}* _temp4753;})); _temp4750;});
_temp4749;}); _temp4748;}); break; case 179: _LL4747: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp4759=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4759[ 0]=({
struct Cyc_InitializerList_tok_struct _temp4760; _temp4760.tag= Cyc_InitializerList_tok;
_temp4760.f1=({ struct Cyc_List_List* _temp4761=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4761->hd=( void*)({ struct
_tuple18* _temp4765=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4765->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4769=
yyvs; void** _temp4771= _temp4769.curr +( yyvsp_offset - 1); if( _temp4769.base
== 0? 1:( _temp4771 < _temp4769.base? 1: _temp4771 >= _temp4769.last_plus_one)){
_throw( Null_Exception);}* _temp4771;})); _temp4765->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4766= yyvs; void** _temp4768= _temp4766.curr +
yyvsp_offset; if( _temp4766.base == 0? 1:( _temp4768 < _temp4766.base? 1:
_temp4768 >= _temp4766.last_plus_one)){ _throw( Null_Exception);}* _temp4768;}));
_temp4765;}); _temp4761->tl= Cyc_yyget_InitializerList_tok(({ struct
_tagged_ptr2 _temp4762= yyvs; void** _temp4764= _temp4762.curr +( yyvsp_offset -
3); if( _temp4762.base == 0? 1:( _temp4764 < _temp4762.base? 1: _temp4764 >=
_temp4762.last_plus_one)){ _throw( Null_Exception);}* _temp4764;})); _temp4761;});
_temp4760;}); _temp4759;}); break; case 180: _LL4758: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp4773=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4773[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4774; _temp4774.tag= Cyc_DesignatorList_tok;
_temp4774.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4775= yyvs; void**
_temp4777= _temp4775.curr +( yyvsp_offset - 1); if( _temp4775.base == 0? 1:(
_temp4777 < _temp4775.base? 1: _temp4777 >= _temp4775.last_plus_one)){ _throw(
Null_Exception);}* _temp4777;}))); _temp4774;}); _temp4773;}); break; case 181:
_LL4772: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct* _temp4779=(
struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4779[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4780; _temp4780.tag=
Cyc_DesignatorList_tok; _temp4780.f1=({ struct Cyc_List_List* _temp4781=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4781->hd=( void*)
Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4782= yyvs; void**
_temp4784= _temp4782.curr + yyvsp_offset; if( _temp4782.base == 0? 1:( _temp4784
< _temp4782.base? 1: _temp4784 >= _temp4782.last_plus_one)){ _throw(
Null_Exception);}* _temp4784;})); _temp4781->tl= 0; _temp4781;}); _temp4780;});
_temp4779;}); break; case 182: _LL4778: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp4786=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4786[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4787; _temp4787.tag=
Cyc_DesignatorList_tok; _temp4787.f1=({ struct Cyc_List_List* _temp4788=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4788->hd=( void*)
Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4792= yyvs; void**
_temp4794= _temp4792.curr + yyvsp_offset; if( _temp4792.base == 0? 1:( _temp4794
< _temp4792.base? 1: _temp4794 >= _temp4792.last_plus_one)){ _throw(
Null_Exception);}* _temp4794;})); _temp4788->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4789= yyvs; void** _temp4791= _temp4789.curr +(
yyvsp_offset - 1); if( _temp4789.base == 0? 1:( _temp4791 < _temp4789.base? 1:
_temp4791 >= _temp4789.last_plus_one)){ _throw( Null_Exception);}* _temp4791;}));
_temp4788;}); _temp4787;}); _temp4786;}); break; case 183: _LL4785: yyval=( void*)({
struct Cyc_Designator_tok_struct* _temp4796=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp4796[ 0]=({ struct
Cyc_Designator_tok_struct _temp4797; _temp4797.tag= Cyc_Designator_tok;
_temp4797.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp4798=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4798[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp4799; _temp4799.tag=
Cyc_Absyn_ArrayElement; _temp4799.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4800= yyvs; void** _temp4802= _temp4800.curr +( yyvsp_offset - 1); if(
_temp4800.base == 0? 1:( _temp4802 < _temp4800.base? 1: _temp4802 >= _temp4800.last_plus_one)){
_throw( Null_Exception);}* _temp4802;})); _temp4799;}); _temp4798;})); _temp4797;});
_temp4796;}); break; case 184: _LL4795: yyval=( void*)({ struct Cyc_Designator_tok_struct*
_temp4804=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4804[ 0]=({ struct Cyc_Designator_tok_struct _temp4805; _temp4805.tag= Cyc_Designator_tok;
_temp4805.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4806=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4806[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4807; _temp4807.tag= Cyc_Absyn_FieldName;
_temp4807.f1=({ struct _tagged_string* _temp4808=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4808[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4809= yyvs; void** _temp4811= _temp4809.curr +
yyvsp_offset; if( _temp4809.base == 0? 1:( _temp4811 < _temp4809.base? 1:
_temp4811 >= _temp4809.last_plus_one)){ _throw( Null_Exception);}* _temp4811;}));
_temp4808;}); _temp4807;}); _temp4806;})); _temp4805;}); _temp4804;}); break;
case 185: _LL4803: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4831= yyvs; void** _temp4833= _temp4831.curr +
yyvsp_offset; if( _temp4831.base == 0? 1:( _temp4833 < _temp4831.base? 1:
_temp4833 >= _temp4831.last_plus_one)){ _throw( Null_Exception);}* _temp4833;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4834= yyls; struct Cyc_Yyltype*
_temp4836= _temp4834.curr + yylsp_offset; if( _temp4834.base == 0? 1:( _temp4836
< _temp4834.base? 1: _temp4836 >= _temp4834.last_plus_one)){ _throw(
Null_Exception);}* _temp4836;}).first_line,({ struct _tagged_ptr3 _temp4837=
yyls; struct Cyc_Yyltype* _temp4839= _temp4837.curr + yylsp_offset; if(
_temp4837.base == 0? 1:( _temp4839 < _temp4837.base? 1: _temp4839 >= _temp4837.last_plus_one)){
_throw( Null_Exception);}* _temp4839;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4828= yyvs; void**
_temp4830= _temp4828.curr + yyvsp_offset; if( _temp4828.base == 0? 1:( _temp4830
< _temp4828.base? 1: _temp4830 >= _temp4828.last_plus_one)){ _throw(
Null_Exception);}* _temp4830;}))).f3; if( atts != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp4813=( char*)"ignoring attributes in type"; struct
_tagged_string _temp4814; _temp4814.curr= _temp4813; _temp4814.base= _temp4813;
_temp4814.last_plus_one= _temp4813 + 28; _temp4814;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4815= yyls; struct Cyc_Yyltype* _temp4817= _temp4815.curr
+ yylsp_offset; if( _temp4815.base == 0? 1:( _temp4817 < _temp4815.base? 1:
_temp4817 >= _temp4815.last_plus_one)){ _throw( Null_Exception);}* _temp4817;}).first_line,({
struct _tagged_ptr3 _temp4818= yyls; struct Cyc_Yyltype* _temp4820= _temp4818.curr
+ yylsp_offset; if( _temp4818.base == 0? 1:( _temp4820 < _temp4818.base? 1:
_temp4820 >= _temp4818.last_plus_one)){ _throw( Null_Exception);}* _temp4820;}).last_line));}{
struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4825= yyvs; void** _temp4827= _temp4825.curr + yyvsp_offset; if( _temp4825.base
== 0? 1:( _temp4827 < _temp4825.base? 1: _temp4827 >= _temp4825.last_plus_one)){
_throw( Null_Exception);}* _temp4827;}))).f1; yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp4821=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4821[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4822; _temp4822.tag= Cyc_ParamDecl_tok;
_temp4822.f1=({ struct _tuple2* _temp4823=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp4823[ 0]=({ struct _tuple2 _temp4824; _temp4824.f1= 0;
_temp4824.f2= tq; _temp4824.f3= t; _temp4824;}); _temp4823;}); _temp4822;});
_temp4821;}); break;}} case 186: _LL4812: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4869= yyvs; void**
_temp4871= _temp4869.curr +( yyvsp_offset - 1); if( _temp4869.base == 0? 1:(
_temp4871 < _temp4869.base? 1: _temp4871 >= _temp4869.last_plus_one)){ _throw(
Null_Exception);}* _temp4871;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4872= yyls; struct Cyc_Yyltype* _temp4874= _temp4872.curr +(
yylsp_offset - 1); if( _temp4872.base == 0? 1:( _temp4874 < _temp4872.base? 1:
_temp4874 >= _temp4872.last_plus_one)){ _throw( Null_Exception);}* _temp4874;}).first_line,({
struct _tagged_ptr3 _temp4875= yyls; struct Cyc_Yyltype* _temp4877= _temp4875.curr
+( yylsp_offset - 1); if( _temp4875.base == 0? 1:( _temp4877 < _temp4875.base? 1:
_temp4877 >= _temp4875.last_plus_one)){ _throw( Null_Exception);}* _temp4877;}).last_line));
struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4866= yyvs; void** _temp4868= _temp4866.curr +( yyvsp_offset - 1); if(
_temp4866.base == 0? 1:( _temp4868 < _temp4866.base? 1: _temp4868 >= _temp4866.last_plus_one)){
_throw( Null_Exception);}* _temp4868;}))).f3; struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4863= yyvs; void** _temp4865= _temp4863.curr +(
yyvsp_offset - 1); if( _temp4863.base == 0? 1:( _temp4865 < _temp4863.base? 1:
_temp4865 >= _temp4863.last_plus_one)){ _throw( Null_Exception);}* _temp4865;}))).f1;
struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4860= yyvs; void** _temp4862= _temp4860.curr + yyvsp_offset;
if( _temp4860.base == 0? 1:( _temp4862 < _temp4860.base? 1: _temp4862 >=
_temp4860.last_plus_one)){ _throw( Null_Exception);}* _temp4862;})))->tms;
struct _tuple6 t_info= Cyc_Parse_apply_tms( tq, t, atts, tms); if( t_info.f3 !=
0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4841=( char*)"bad type params, ignoring";
struct _tagged_string _temp4842; _temp4842.curr= _temp4841; _temp4842.base=
_temp4841; _temp4842.last_plus_one= _temp4841 + 26; _temp4842;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4843= yyls; struct Cyc_Yyltype* _temp4845= _temp4843.curr
+ yylsp_offset; if( _temp4843.base == 0? 1:( _temp4845 < _temp4843.base? 1:
_temp4845 >= _temp4843.last_plus_one)){ _throw( Null_Exception);}* _temp4845;}).first_line,({
struct _tagged_ptr3 _temp4846= yyls; struct Cyc_Yyltype* _temp4848= _temp4846.curr
+ yylsp_offset; if( _temp4846.base == 0? 1:( _temp4848 < _temp4846.base? 1:
_temp4848 >= _temp4846.last_plus_one)){ _throw( Null_Exception);}* _temp4848;}).last_line));}
if( t_info.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4849=(
char*)"bad specifiers, ignoring"; struct _tagged_string _temp4850; _temp4850.curr=
_temp4849; _temp4850.base= _temp4849; _temp4850.last_plus_one= _temp4849 + 25;
_temp4850;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4851= yyls;
struct Cyc_Yyltype* _temp4853= _temp4851.curr + yylsp_offset; if( _temp4851.base
== 0? 1:( _temp4853 < _temp4851.base? 1: _temp4853 >= _temp4851.last_plus_one)){
_throw( Null_Exception);}* _temp4853;}).first_line,({ struct _tagged_ptr3
_temp4854= yyls; struct Cyc_Yyltype* _temp4856= _temp4854.curr + yylsp_offset;
if( _temp4854.base == 0? 1:( _temp4856 < _temp4854.base? 1: _temp4856 >=
_temp4854.last_plus_one)){ _throw( Null_Exception);}* _temp4856;}).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4857=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4857[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4858; _temp4858.tag= Cyc_ParamDecl_tok; _temp4858.f1=({
struct _tuple2* _temp4859=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4859->f1= 0; _temp4859->f2= t_info.f1; _temp4859->f3= t_info.f2; _temp4859;});
_temp4858;}); _temp4857;}); break;} case 187: _LL4840: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp4879=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct)); _temp4879[ 0]=({ struct Cyc_Type_tok_struct
_temp4880; _temp4880.tag= Cyc_Type_tok; _temp4880.f1=( void*)(* Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp4881= yyvs; void** _temp4883= _temp4881.curr +
yyvsp_offset; if( _temp4881.base == 0? 1:( _temp4883 < _temp4881.base? 1:
_temp4883 >= _temp4881.last_plus_one)){ _throw( Null_Exception);}* _temp4883;}))).f3;
_temp4880;}); _temp4879;}); break; case 188: _LL4878: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp4885=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct)); _temp4885[ 0]=({ struct Cyc_Type_tok_struct
_temp4886; _temp4886.tag= Cyc_Type_tok; _temp4886.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp4887=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4887[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4888; _temp4888.tag= Cyc_Absyn_JoinEff; _temp4888.f1=
0; _temp4888;}); _temp4887;})); _temp4886;}); _temp4885;}); break; case 189:
_LL4884: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp4890=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp4890[ 0]=({ struct Cyc_Type_tok_struct
_temp4891; _temp4891.tag= Cyc_Type_tok; _temp4891.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp4892=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4892[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4893; _temp4893.tag= Cyc_Absyn_JoinEff; _temp4893.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4894= yyvs; void** _temp4896=
_temp4894.curr +( yyvsp_offset - 1); if( _temp4894.base == 0? 1:( _temp4896 <
_temp4894.base? 1: _temp4896 >= _temp4894.last_plus_one)){ _throw(
Null_Exception);}* _temp4896;})); _temp4893;}); _temp4892;})); _temp4891;});
_temp4890;}); break; case 190: _LL4889: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp4898=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4898[ 0]=({ struct Cyc_Type_tok_struct _temp4899; _temp4899.tag= Cyc_Type_tok;
_temp4899.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4900=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4900[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp4901; _temp4901.tag= Cyc_Absyn_JoinEff;
_temp4901.f1=({ struct Cyc_List_List* _temp4902=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4902->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4906= yyvs; void** _temp4908= _temp4906.curr +(
yyvsp_offset - 2); if( _temp4906.base == 0? 1:( _temp4908 < _temp4906.base? 1:
_temp4908 >= _temp4906.last_plus_one)){ _throw( Null_Exception);}* _temp4908;}));
_temp4902->tl= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4903= yyvs;
void** _temp4905= _temp4903.curr + yyvsp_offset; if( _temp4903.base == 0? 1:(
_temp4905 < _temp4903.base? 1: _temp4905 >= _temp4903.last_plus_one)){ _throw(
Null_Exception);}* _temp4905;})); _temp4902;}); _temp4901;}); _temp4900;}));
_temp4899;}); _temp4898;}); break; case 191: _LL4897: yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp4910=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp4910[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4911; _temp4911.tag= Cyc_TypeList_tok; _temp4911.f1=({ struct Cyc_List_List*
_temp4912=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4912->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4913= yyvs;
void** _temp4915= _temp4913.curr + yyvsp_offset; if( _temp4913.base == 0? 1:(
_temp4915 < _temp4913.base? 1: _temp4915 >= _temp4913.last_plus_one)){ _throw(
Null_Exception);}* _temp4915;})); _temp4912->tl= 0; _temp4912;}); _temp4911;});
_temp4910;}); break; case 192: _LL4909: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4917=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4917[ 0]=({ struct Cyc_TypeList_tok_struct _temp4918; _temp4918.tag= Cyc_TypeList_tok;
_temp4918.f1=({ struct Cyc_List_List* _temp4919=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4919->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4923= yyvs; void** _temp4925= _temp4923.curr +
yyvsp_offset; if( _temp4923.base == 0? 1:( _temp4925 < _temp4923.base? 1:
_temp4925 >= _temp4923.last_plus_one)){ _throw( Null_Exception);}* _temp4925;}));
_temp4919->tl= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4920= yyvs;
void** _temp4922= _temp4920.curr +( yyvsp_offset - 2); if( _temp4920.base == 0?
1:( _temp4922 < _temp4920.base? 1: _temp4922 >= _temp4920.last_plus_one)){
_throw( Null_Exception);}* _temp4922;})); _temp4919;}); _temp4918;}); _temp4917;});
break; case 193: _LL4916: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4927=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4927[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4928; _temp4928.tag= Cyc_AbstractDeclarator_tok; _temp4928.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4929=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4929->tms= Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4930=
yyvs; void** _temp4932= _temp4930.curr + yyvsp_offset; if( _temp4930.base == 0?
1:( _temp4932 < _temp4930.base? 1: _temp4932 >= _temp4930.last_plus_one)){
_throw( Null_Exception);}* _temp4932;})); _temp4929;}); _temp4928;}); _temp4927;});
break; case 194: _LL4926: yyval=({ struct _tagged_ptr2 _temp4934= yyvs; void**
_temp4936= _temp4934.curr + yyvsp_offset; if( _temp4934.base == 0? 1:( _temp4936
< _temp4934.base? 1: _temp4936 >= _temp4934.last_plus_one)){ _throw(
Null_Exception);}* _temp4936;}); break; case 195: _LL4933: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp4938=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4938[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4939; _temp4939.tag= Cyc_AbstractDeclarator_tok;
_temp4939.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4940=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4940->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4941= yyvs; void**
_temp4943= _temp4941.curr +( yyvsp_offset - 1); if( _temp4941.base == 0? 1:(
_temp4943 < _temp4941.base? 1: _temp4943 >= _temp4941.last_plus_one)){ _throw(
Null_Exception);}* _temp4943;})),( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4944= yyvs; void** _temp4946= _temp4944.curr + yyvsp_offset;
if( _temp4944.base == 0? 1:( _temp4946 < _temp4944.base? 1: _temp4946 >=
_temp4944.last_plus_one)){ _throw( Null_Exception);}* _temp4946;})))->tms);
_temp4940;}); _temp4939;}); _temp4938;}); break; case 196: _LL4937: yyval=({
struct _tagged_ptr2 _temp4948= yyvs; void** _temp4950= _temp4948.curr +(
yyvsp_offset - 1); if( _temp4948.base == 0? 1:( _temp4950 < _temp4948.base? 1:
_temp4950 >= _temp4948.last_plus_one)){ _throw( Null_Exception);}* _temp4950;});
break; case 197: _LL4947: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4952=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4952[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4953; _temp4953.tag= Cyc_AbstractDeclarator_tok; _temp4953.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4954=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4954->tms=({ struct Cyc_List_List* _temp4955=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4955->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4955->tl= 0; _temp4955;}); _temp4954;}); _temp4953;}); _temp4952;}); break;
case 198: _LL4951: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4957=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4957[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp4958; _temp4958.tag= Cyc_AbstractDeclarator_tok; _temp4958.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4959=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4959->tms=({ struct Cyc_List_List* _temp4960=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4960->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4960->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4961= yyvs; void** _temp4963= _temp4961.curr +( yyvsp_offset - 2); if(
_temp4961.base == 0? 1:( _temp4963 < _temp4961.base? 1: _temp4963 >= _temp4961.last_plus_one)){
_throw( Null_Exception);}* _temp4963;})))->tms; _temp4960;}); _temp4959;});
_temp4958;}); _temp4957;}); break; case 199: _LL4956: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4965=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4965[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4966; _temp4966.tag= Cyc_AbstractDeclarator_tok;
_temp4966.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4967=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4967->tms=({
struct Cyc_List_List* _temp4968=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4968->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4969=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4969[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4970; _temp4970.tag= Cyc_Absyn_ConstArray_mod; _temp4970.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4971= yyvs; void** _temp4973= _temp4971.curr +(
yyvsp_offset - 1); if( _temp4971.base == 0? 1:( _temp4973 < _temp4971.base? 1:
_temp4973 >= _temp4971.last_plus_one)){ _throw( Null_Exception);}* _temp4973;}));
_temp4970;}); _temp4969;})); _temp4968->tl= 0; _temp4968;}); _temp4967;});
_temp4966;}); _temp4965;}); break; case 200: _LL4964: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4975=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4975[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4976; _temp4976.tag= Cyc_AbstractDeclarator_tok;
_temp4976.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4977=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4977->tms=({
struct Cyc_List_List* _temp4978=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4978->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4982=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4982[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp4983; _temp4983.tag= Cyc_Absyn_ConstArray_mod; _temp4983.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4984= yyvs; void** _temp4986= _temp4984.curr +(
yyvsp_offset - 1); if( _temp4984.base == 0? 1:( _temp4986 < _temp4984.base? 1:
_temp4986 >= _temp4984.last_plus_one)){ _throw( Null_Exception);}* _temp4986;}));
_temp4983;}); _temp4982;})); _temp4978->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4979= yyvs; void** _temp4981= _temp4979.curr +(
yyvsp_offset - 3); if( _temp4979.base == 0? 1:( _temp4981 < _temp4979.base? 1:
_temp4981 >= _temp4979.last_plus_one)){ _throw( Null_Exception);}* _temp4981;})))->tms;
_temp4978;}); _temp4977;}); _temp4976;}); _temp4975;}); break; case 201: _LL4974:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp4988=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4988[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp4989; _temp4989.tag=
Cyc_AbstractDeclarator_tok; _temp4989.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4990=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4990->tms=({ struct Cyc_List_List* _temp4991=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4991->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4992=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4992[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4993; _temp4993.tag= Cyc_Absyn_Function_mod;
_temp4993.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4994=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4994[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4995; _temp4995.tag= Cyc_Absyn_WithTypes;
_temp4995.f1= 0; _temp4995.f2= 0; _temp4995.f3= 0; _temp4995;}); _temp4994;}));
_temp4993;}); _temp4992;})); _temp4991->tl= 0; _temp4991;}); _temp4990;});
_temp4989;}); _temp4988;}); break; case 202: _LL4987: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4997=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4997[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp4998; _temp4998.tag= Cyc_AbstractDeclarator_tok;
_temp4998.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4999=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4999->tms=({
struct Cyc_List_List* _temp5000=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5000->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5001=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5001[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5002; _temp5002.tag=
Cyc_Absyn_Function_mod; _temp5002.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5003=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5003[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5004; _temp5004.tag= Cyc_Absyn_WithTypes;
_temp5004.f1= 0; _temp5004.f2= 0; _temp5004.f3=({ struct Cyc_Core_Opt* _temp5005=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5005->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp5006=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp5006[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp5007; _temp5007.tag= Cyc_Absyn_JoinEff; _temp5007.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5008= yyvs; void** _temp5010=
_temp5008.curr +( yyvsp_offset - 1); if( _temp5008.base == 0? 1:( _temp5010 <
_temp5008.base? 1: _temp5010 >= _temp5008.last_plus_one)){ _throw(
Null_Exception);}* _temp5010;})); _temp5007;}); _temp5006;})); _temp5005;});
_temp5004;}); _temp5003;})); _temp5002;}); _temp5001;})); _temp5000->tl= 0;
_temp5000;}); _temp4999;}); _temp4998;}); _temp4997;}); break; case 203: _LL4996: {
struct _tuple16 _temp5017; struct Cyc_Core_Opt* _temp5018; int _temp5020; struct
Cyc_List_List* _temp5022; struct _tuple16* _temp5015= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp5012= yyvs; void** _temp5014= _temp5012.curr +(
yyvsp_offset - 1); if( _temp5012.base == 0? 1:( _temp5014 < _temp5012.base? 1:
_temp5014 >= _temp5012.last_plus_one)){ _throw( Null_Exception);}* _temp5014;}));
_temp5017=* _temp5015; _LL5023: _temp5022= _temp5017.f1; goto _LL5021; _LL5021:
_temp5020= _temp5017.f2; goto _LL5019; _LL5019: _temp5018= _temp5017.f3; goto
_LL5016; _LL5016: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5024=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5024[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5025; _temp5025.tag= Cyc_AbstractDeclarator_tok; _temp5025.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5026=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5026->tms=({ struct Cyc_List_List* _temp5027=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5027->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5028=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5028[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5029; _temp5029.tag= Cyc_Absyn_Function_mod;
_temp5029.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5030=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5030[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5031; _temp5031.tag= Cyc_Absyn_WithTypes;
_temp5031.f1= _temp5022; _temp5031.f2= _temp5020; _temp5031.f3= _temp5018;
_temp5031;}); _temp5030;})); _temp5029;}); _temp5028;})); _temp5027->tl= 0;
_temp5027;}); _temp5026;}); _temp5025;}); _temp5024;}); break;} case 204:
_LL5011: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5033=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5033[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5034; _temp5034.tag=
Cyc_AbstractDeclarator_tok; _temp5034.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5035=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5035->tms=({ struct Cyc_List_List* _temp5036=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5036->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5040=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5040[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5041; _temp5041.tag= Cyc_Absyn_Function_mod;
_temp5041.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5042=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5042[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5043; _temp5043.tag= Cyc_Absyn_WithTypes;
_temp5043.f1= 0; _temp5043.f2= 0; _temp5043.f3= 0; _temp5043;}); _temp5042;}));
_temp5041;}); _temp5040;})); _temp5036->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5037= yyvs; void** _temp5039= _temp5037.curr +(
yyvsp_offset - 2); if( _temp5037.base == 0? 1:( _temp5039 < _temp5037.base? 1:
_temp5039 >= _temp5037.last_plus_one)){ _throw( Null_Exception);}* _temp5039;})))->tms;
_temp5036;}); _temp5035;}); _temp5034;}); _temp5033;}); break; case 205: _LL5032: {
struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt* _temp5056=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5056->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp5057=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp5057[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp5058; _temp5058.tag= Cyc_Absyn_JoinEff; _temp5058.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5059= yyvs; void** _temp5061=
_temp5059.curr +( yyvsp_offset - 1); if( _temp5059.base == 0? 1:( _temp5061 <
_temp5059.base? 1: _temp5061 >= _temp5059.last_plus_one)){ _throw(
Null_Exception);}* _temp5061;})); _temp5058;}); _temp5057;})); _temp5056;});
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5045=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5045[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5046; _temp5046.tag=
Cyc_AbstractDeclarator_tok; _temp5046.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5047=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5047->tms=({ struct Cyc_List_List* _temp5048=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5048->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5052=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5052[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5053; _temp5053.tag= Cyc_Absyn_Function_mod;
_temp5053.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5054=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5054[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5055; _temp5055.tag= Cyc_Absyn_WithTypes;
_temp5055.f1= 0; _temp5055.f2= 0; _temp5055.f3= eff; _temp5055;}); _temp5054;}));
_temp5053;}); _temp5052;})); _temp5048->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5049= yyvs; void** _temp5051= _temp5049.curr +(
yyvsp_offset - 4); if( _temp5049.base == 0? 1:( _temp5051 < _temp5049.base? 1:
_temp5051 >= _temp5049.last_plus_one)){ _throw( Null_Exception);}* _temp5051;})))->tms;
_temp5048;}); _temp5047;}); _temp5046;}); _temp5045;}); break;} case 206:
_LL5044: { struct _tuple16 _temp5068; struct Cyc_Core_Opt* _temp5069; int
_temp5071; struct Cyc_List_List* _temp5073; struct _tuple16* _temp5066= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp5063= yyvs; void** _temp5065= _temp5063.curr +(
yyvsp_offset - 1); if( _temp5063.base == 0? 1:( _temp5065 < _temp5063.base? 1:
_temp5065 >= _temp5063.last_plus_one)){ _throw( Null_Exception);}* _temp5065;}));
_temp5068=* _temp5066; _LL5074: _temp5073= _temp5068.f1; goto _LL5072; _LL5072:
_temp5071= _temp5068.f2; goto _LL5070; _LL5070: _temp5069= _temp5068.f3; goto
_LL5067; _LL5067: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5075=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5075[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5076; _temp5076.tag= Cyc_AbstractDeclarator_tok; _temp5076.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5077=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5077->tms=({ struct Cyc_List_List* _temp5078=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5078->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5082=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5082[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5083; _temp5083.tag= Cyc_Absyn_Function_mod;
_temp5083.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5084=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5084[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5085; _temp5085.tag= Cyc_Absyn_WithTypes;
_temp5085.f1= _temp5073; _temp5085.f2= _temp5071; _temp5085.f3= _temp5069;
_temp5085;}); _temp5084;})); _temp5083;}); _temp5082;})); _temp5078->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5079= yyvs; void** _temp5081= _temp5079.curr +(
yyvsp_offset - 3); if( _temp5079.base == 0? 1:( _temp5081 < _temp5079.base? 1:
_temp5081 >= _temp5079.last_plus_one)){ _throw( Null_Exception);}* _temp5081;})))->tms;
_temp5078;}); _temp5077;}); _temp5076;}); _temp5075;}); break;} case 207:
_LL5062: { struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5102= yyls; struct Cyc_Yyltype* _temp5104= _temp5102.curr
+( yylsp_offset - 2); if( _temp5102.base == 0? 1:( _temp5104 < _temp5102.base? 1:
_temp5104 >= _temp5102.last_plus_one)){ _throw( Null_Exception);}* _temp5104;}).first_line,({
struct _tagged_ptr3 _temp5105= yyls; struct Cyc_Yyltype* _temp5107= _temp5105.curr
+ yylsp_offset; if( _temp5105.base == 0? 1:( _temp5107 < _temp5105.base? 1:
_temp5107 >= _temp5105.last_plus_one)){ _throw( Null_Exception);}* _temp5107;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5108= yyvs; void** _temp5110= _temp5108.curr +(
yyvsp_offset - 1); if( _temp5108.base == 0? 1:( _temp5110 < _temp5108.base? 1:
_temp5110 >= _temp5108.last_plus_one)){ _throw( Null_Exception);}* _temp5110;}))));
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5087=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5087[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5088; _temp5088.tag=
Cyc_AbstractDeclarator_tok; _temp5088.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5089=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5089->tms=({ struct Cyc_List_List* _temp5090=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5090->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp5094=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp5094[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp5095; _temp5095.tag= Cyc_Absyn_TypeParams_mod;
_temp5095.f1= ts; _temp5095.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5096= yyls; struct Cyc_Yyltype* _temp5098= _temp5096.curr +(
yylsp_offset - 2); if( _temp5096.base == 0? 1:( _temp5098 < _temp5096.base? 1:
_temp5098 >= _temp5096.last_plus_one)){ _throw( Null_Exception);}* _temp5098;}).first_line,({
struct _tagged_ptr3 _temp5099= yyls; struct Cyc_Yyltype* _temp5101= _temp5099.curr
+ yylsp_offset; if( _temp5099.base == 0? 1:( _temp5101 < _temp5099.base? 1:
_temp5101 >= _temp5099.last_plus_one)){ _throw( Null_Exception);}* _temp5101;}).last_line);
_temp5095.f3= 0; _temp5095;}); _temp5094;})); _temp5090->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5091= yyvs; void** _temp5093= _temp5091.curr +(
yyvsp_offset - 3); if( _temp5091.base == 0? 1:( _temp5093 < _temp5091.base? 1:
_temp5093 >= _temp5091.last_plus_one)){ _throw( Null_Exception);}* _temp5093;})))->tms;
_temp5090;}); _temp5089;}); _temp5088;}); _temp5087;}); break;} case 208:
_LL5086:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp5112= Cyc_Parse_lbuf;
if( _temp5112 == 0){ _throw( Null_Exception);} _temp5112->v;}))->lex_curr_pos -=
1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5128= yyls; struct Cyc_Yyltype* _temp5130= _temp5128.curr
+( yylsp_offset - 2); if( _temp5128.base == 0? 1:( _temp5130 < _temp5128.base? 1:
_temp5130 >= _temp5128.last_plus_one)){ _throw( Null_Exception);}* _temp5130;}).first_line,({
struct _tagged_ptr3 _temp5131= yyls; struct Cyc_Yyltype* _temp5133= _temp5131.curr
+ yylsp_offset; if( _temp5131.base == 0? 1:( _temp5133 < _temp5131.base? 1:
_temp5133 >= _temp5131.last_plus_one)){ _throw( Null_Exception);}* _temp5133;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5134= yyvs; void** _temp5136= _temp5134.curr +(
yyvsp_offset - 1); if( _temp5134.base == 0? 1:( _temp5136 < _temp5134.base? 1:
_temp5136 >= _temp5134.last_plus_one)){ _throw( Null_Exception);}* _temp5136;}))));
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5113=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5113[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5114; _temp5114.tag=
Cyc_AbstractDeclarator_tok; _temp5114.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5115=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5115->tms=({ struct Cyc_List_List* _temp5116=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5116->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp5120=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp5120[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp5121; _temp5121.tag= Cyc_Absyn_TypeParams_mod;
_temp5121.f1= ts; _temp5121.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5122= yyls; struct Cyc_Yyltype* _temp5124= _temp5122.curr +(
yylsp_offset - 2); if( _temp5122.base == 0? 1:( _temp5124 < _temp5122.base? 1:
_temp5124 >= _temp5122.last_plus_one)){ _throw( Null_Exception);}* _temp5124;}).first_line,({
struct _tagged_ptr3 _temp5125= yyls; struct Cyc_Yyltype* _temp5127= _temp5125.curr
+ yylsp_offset; if( _temp5125.base == 0? 1:( _temp5127 < _temp5125.base? 1:
_temp5127 >= _temp5125.last_plus_one)){ _throw( Null_Exception);}* _temp5127;}).last_line);
_temp5121.f3= 0; _temp5121;}); _temp5120;})); _temp5116->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5117= yyvs; void** _temp5119= _temp5117.curr +(
yyvsp_offset - 3); if( _temp5117.base == 0? 1:( _temp5119 < _temp5117.base? 1:
_temp5119 >= _temp5117.last_plus_one)){ _throw( Null_Exception);}* _temp5119;})))->tms;
_temp5116;}); _temp5115;}); _temp5114;}); _temp5113;}); break;} case 209:
_LL5111: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5138=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5138[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5139; _temp5139.tag=
Cyc_AbstractDeclarator_tok; _temp5139.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5140=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5140->tms=({ struct Cyc_List_List* _temp5141=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5141->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp5145=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp5145[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp5146; _temp5146.tag= Cyc_Absyn_Attributes_mod;
_temp5146.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5147= yyls;
struct Cyc_Yyltype* _temp5149= _temp5147.curr + yylsp_offset; if( _temp5147.base
== 0? 1:( _temp5149 < _temp5147.base? 1: _temp5149 >= _temp5147.last_plus_one)){
_throw( Null_Exception);}* _temp5149;}).first_line,({ struct _tagged_ptr3
_temp5150= yyls; struct Cyc_Yyltype* _temp5152= _temp5150.curr + yylsp_offset;
if( _temp5150.base == 0? 1:( _temp5152 < _temp5150.base? 1: _temp5152 >=
_temp5150.last_plus_one)){ _throw( Null_Exception);}* _temp5152;}).last_line);
_temp5146.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp5153= yyvs;
void** _temp5155= _temp5153.curr + yyvsp_offset; if( _temp5153.base == 0? 1:(
_temp5155 < _temp5153.base? 1: _temp5155 >= _temp5153.last_plus_one)){ _throw(
Null_Exception);}* _temp5155;})); _temp5146;}); _temp5145;})); _temp5141->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp5142= yyvs; void**
_temp5144= _temp5142.curr +( yyvsp_offset - 1); if( _temp5142.base == 0? 1:(
_temp5144 < _temp5142.base? 1: _temp5144 >= _temp5142.last_plus_one)){ _throw(
Null_Exception);}* _temp5144;})))->tms; _temp5141;}); _temp5140;}); _temp5139;});
_temp5138;}); break; case 210: _LL5137: yyval=({ struct _tagged_ptr2 _temp5157=
yyvs; void** _temp5159= _temp5157.curr + yyvsp_offset; if( _temp5157.base == 0?
1:( _temp5159 < _temp5157.base? 1: _temp5159 >= _temp5157.last_plus_one)){
_throw( Null_Exception);}* _temp5159;}); break; case 211: _LL5156: yyval=({
struct _tagged_ptr2 _temp5161= yyvs; void** _temp5163= _temp5161.curr +
yyvsp_offset; if( _temp5161.base == 0? 1:( _temp5163 < _temp5161.base? 1:
_temp5163 >= _temp5161.last_plus_one)){ _throw( Null_Exception);}* _temp5163;});
break; case 212: _LL5160: yyval=({ struct _tagged_ptr2 _temp5165= yyvs; void**
_temp5167= _temp5165.curr + yyvsp_offset; if( _temp5165.base == 0? 1:( _temp5167
< _temp5165.base? 1: _temp5167 >= _temp5165.last_plus_one)){ _throw(
Null_Exception);}* _temp5167;}); break; case 213: _LL5164: yyval=({ struct
_tagged_ptr2 _temp5169= yyvs; void** _temp5171= _temp5169.curr + yyvsp_offset;
if( _temp5169.base == 0? 1:( _temp5171 < _temp5169.base? 1: _temp5171 >=
_temp5169.last_plus_one)){ _throw( Null_Exception);}* _temp5171;}); break; case
214: _LL5168: yyval=({ struct _tagged_ptr2 _temp5173= yyvs; void** _temp5175=
_temp5173.curr + yyvsp_offset; if( _temp5173.base == 0? 1:( _temp5175 <
_temp5173.base? 1: _temp5175 >= _temp5173.last_plus_one)){ _throw(
Null_Exception);}* _temp5175;}); break; case 215: _LL5172: yyval=({ struct
_tagged_ptr2 _temp5177= yyvs; void** _temp5179= _temp5177.curr + yyvsp_offset;
if( _temp5177.base == 0? 1:( _temp5179 < _temp5177.base? 1: _temp5179 >=
_temp5177.last_plus_one)){ _throw( Null_Exception);}* _temp5179;}); break; case
216: _LL5176: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5181= yyvs; void** _temp5183= _temp5181.curr +( yyvsp_offset -
3); if( _temp5181.base == 0? 1:( _temp5183 < _temp5181.base? 1: _temp5183 >=
_temp5181.last_plus_one)){ _throw( Null_Exception);}* _temp5183;})),( struct
_tagged_string)({ char* _temp5184=( char*)"`H"; struct _tagged_string _temp5185;
_temp5185.curr= _temp5184; _temp5185.base= _temp5184; _temp5185.last_plus_one=
_temp5184 + 3; _temp5185;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp5186=( char*)"bad occurrence of heap region `H"; struct
_tagged_string _temp5187; _temp5187.curr= _temp5186; _temp5187.base= _temp5186;
_temp5187.last_plus_one= _temp5186 + 33; _temp5187;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5188= yyls; struct Cyc_Yyltype* _temp5190= _temp5188.curr
+( yylsp_offset - 3); if( _temp5188.base == 0? 1:( _temp5190 < _temp5188.base? 1:
_temp5190 >= _temp5188.last_plus_one)){ _throw( Null_Exception);}* _temp5190;}).first_line,({
struct _tagged_ptr3 _temp5191= yyls; struct Cyc_Yyltype* _temp5193= _temp5191.curr
+( yylsp_offset - 3); if( _temp5191.base == 0? 1:( _temp5193 < _temp5191.base? 1:
_temp5193 >= _temp5191.last_plus_one)){ _throw( Null_Exception);}* _temp5193;}).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5216=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5216->name=({ struct
_tagged_string* _temp5217=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp5217[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp5218= yyvs; void** _temp5220= _temp5218.curr +( yyvsp_offset - 3); if(
_temp5218.base == 0? 1:( _temp5220 < _temp5218.base? 1: _temp5220 >= _temp5218.last_plus_one)){
_throw( Null_Exception);}* _temp5220;})); _temp5217;}); _temp5216->kind=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind);
_temp5216;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp5214=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp5214[ 0]=({ struct Cyc_Absyn_VarType_struct _temp5215; _temp5215.tag= Cyc_Absyn_VarType;
_temp5215.f1= tv; _temp5215;}); _temp5214;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5194=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5194[ 0]=({ struct Cyc_Stmt_tok_struct _temp5195; _temp5195.tag= Cyc_Stmt_tok;
_temp5195.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp5196=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp5196[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp5197; _temp5197.tag= Cyc_Absyn_Region_s;
_temp5197.f1= tv; _temp5197.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5198=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5198->f1=(
void*) Cyc_Absyn_Loc_n; _temp5198->f2=({ struct _tagged_string* _temp5199=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp5199[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5200= yyvs; void** _temp5202=
_temp5200.curr +( yyvsp_offset - 1); if( _temp5200.base == 0? 1:( _temp5202 <
_temp5200.base? 1: _temp5202 >= _temp5200.last_plus_one)){ _throw(
Null_Exception);}* _temp5202;})); _temp5199;}); _temp5198;}),( void*)({ struct
Cyc_Absyn_RgnHandleType_struct* _temp5203=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp5203[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp5204; _temp5204.tag= Cyc_Absyn_RgnHandleType;
_temp5204.f1=( void*) t; _temp5204;}); _temp5203;}), 0); _temp5197.f3= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5205= yyvs; void** _temp5207= _temp5205.curr +
yyvsp_offset; if( _temp5205.base == 0? 1:( _temp5207 < _temp5205.base? 1:
_temp5207 >= _temp5205.last_plus_one)){ _throw( Null_Exception);}* _temp5207;}));
_temp5197;}); _temp5196;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5208= yyls; struct Cyc_Yyltype* _temp5210= _temp5208.curr +( yylsp_offset -
5); if( _temp5208.base == 0? 1:( _temp5210 < _temp5208.base? 1: _temp5210 >=
_temp5208.last_plus_one)){ _throw( Null_Exception);}* _temp5210;}).first_line,({
struct _tagged_ptr3 _temp5211= yyls; struct Cyc_Yyltype* _temp5213= _temp5211.curr
+ yylsp_offset; if( _temp5211.base == 0? 1:( _temp5213 < _temp5211.base? 1:
_temp5213 >= _temp5211.last_plus_one)){ _throw( Null_Exception);}* _temp5213;}).last_line));
_temp5195;}); _temp5194;}); break;} case 217: _LL5180: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5222= yyvs; void** _temp5224=
_temp5222.curr +( yyvsp_offset - 1); if( _temp5222.base == 0? 1:( _temp5224 <
_temp5222.base? 1: _temp5224 >= _temp5222.last_plus_one)){ _throw(
Null_Exception);}* _temp5224;})),( struct _tagged_string)({ char* _temp5225=(
char*)"H"; struct _tagged_string _temp5226; _temp5226.curr= _temp5225; _temp5226.base=
_temp5225; _temp5226.last_plus_one= _temp5225 + 2; _temp5226;})) == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp5227=( char*)"bad occurrence of heap region `H";
struct _tagged_string _temp5228; _temp5228.curr= _temp5227; _temp5228.base=
_temp5227; _temp5228.last_plus_one= _temp5227 + 33; _temp5228;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5229= yyls; struct Cyc_Yyltype* _temp5231= _temp5229.curr
+ yylsp_offset; if( _temp5229.base == 0? 1:( _temp5231 < _temp5229.base? 1:
_temp5231 >= _temp5229.last_plus_one)){ _throw( Null_Exception);}* _temp5231;}).first_line,({
struct _tagged_ptr3 _temp5232= yyls; struct Cyc_Yyltype* _temp5234= _temp5232.curr
+ yylsp_offset; if( _temp5232.base == 0? 1:( _temp5234 < _temp5232.base? 1:
_temp5234 >= _temp5232.last_plus_one)){ _throw( Null_Exception);}* _temp5234;}).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5257=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5257->name=({ struct
_tagged_string* _temp5258=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp5258[ 0]=({ struct _tagged_string _temp5262= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5259= yyvs; void** _temp5261= _temp5259.curr +(
yyvsp_offset - 1); if( _temp5259.base == 0? 1:( _temp5261 < _temp5259.base? 1:
_temp5261 >= _temp5259.last_plus_one)){ _throw( Null_Exception);}* _temp5261;}));
xprintf("`%.*s", _temp5262.last_plus_one - _temp5262.curr, _temp5262.curr);});
_temp5258;}); _temp5257->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp5257;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp5255=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp5255[ 0]=({ struct Cyc_Absyn_VarType_struct _temp5256; _temp5256.tag= Cyc_Absyn_VarType;
_temp5256.f1= tv; _temp5256;}); _temp5255;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5235=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5235[ 0]=({ struct Cyc_Stmt_tok_struct _temp5236; _temp5236.tag= Cyc_Stmt_tok;
_temp5236.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp5237=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp5237[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp5238; _temp5238.tag= Cyc_Absyn_Region_s;
_temp5238.f1= tv; _temp5238.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5239=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5239->f1=(
void*) Cyc_Absyn_Loc_n; _temp5239->f2=({ struct _tagged_string* _temp5240=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp5240[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5241= yyvs; void** _temp5243=
_temp5241.curr +( yyvsp_offset - 1); if( _temp5241.base == 0? 1:( _temp5243 <
_temp5241.base? 1: _temp5243 >= _temp5241.last_plus_one)){ _throw(
Null_Exception);}* _temp5243;})); _temp5240;}); _temp5239;}),( void*)({ struct
Cyc_Absyn_RgnHandleType_struct* _temp5244=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp5244[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp5245; _temp5245.tag= Cyc_Absyn_RgnHandleType;
_temp5245.f1=( void*) t; _temp5245;}); _temp5244;}), 0); _temp5238.f3= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5246= yyvs; void** _temp5248= _temp5246.curr +
yyvsp_offset; if( _temp5246.base == 0? 1:( _temp5248 < _temp5246.base? 1:
_temp5248 >= _temp5246.last_plus_one)){ _throw( Null_Exception);}* _temp5248;}));
_temp5238;}); _temp5237;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5249= yyls; struct Cyc_Yyltype* _temp5251= _temp5249.curr +( yylsp_offset -
2); if( _temp5249.base == 0? 1:( _temp5251 < _temp5249.base? 1: _temp5251 >=
_temp5249.last_plus_one)){ _throw( Null_Exception);}* _temp5251;}).first_line,({
struct _tagged_ptr3 _temp5252= yyls; struct Cyc_Yyltype* _temp5254= _temp5252.curr
+ yylsp_offset; if( _temp5252.base == 0? 1:( _temp5254 < _temp5252.base? 1:
_temp5254 >= _temp5252.last_plus_one)){ _throw( Null_Exception);}* _temp5254;}).last_line));
_temp5236;}); _temp5235;}); break;} case 218: _LL5221: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5264=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5264[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5265; _temp5265.tag= Cyc_Stmt_tok; _temp5265.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Cut_s_struct* _temp5266=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp5266[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp5267; _temp5267.tag= Cyc_Absyn_Cut_s; _temp5267.f1= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5268= yyvs; void** _temp5270= _temp5268.curr +
yyvsp_offset; if( _temp5268.base == 0? 1:( _temp5270 < _temp5268.base? 1:
_temp5270 >= _temp5268.last_plus_one)){ _throw( Null_Exception);}* _temp5270;}));
_temp5267;}); _temp5266;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5271= yyls; struct Cyc_Yyltype* _temp5273= _temp5271.curr +( yylsp_offset -
1); if( _temp5271.base == 0? 1:( _temp5273 < _temp5271.base? 1: _temp5273 >=
_temp5271.last_plus_one)){ _throw( Null_Exception);}* _temp5273;}).first_line,({
struct _tagged_ptr3 _temp5274= yyls; struct Cyc_Yyltype* _temp5276= _temp5274.curr
+ yylsp_offset; if( _temp5274.base == 0? 1:( _temp5276 < _temp5274.base? 1:
_temp5276 >= _temp5274.last_plus_one)){ _throw( Null_Exception);}* _temp5276;}).last_line));
_temp5265;}); _temp5264;}); break; case 219: _LL5263: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5278=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5278[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5279; _temp5279.tag= Cyc_Stmt_tok; _temp5279.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Splice_s_struct* _temp5280=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp5280[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp5281; _temp5281.tag= Cyc_Absyn_Splice_s;
_temp5281.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5282= yyvs; void**
_temp5284= _temp5282.curr + yyvsp_offset; if( _temp5282.base == 0? 1:( _temp5284
< _temp5282.base? 1: _temp5284 >= _temp5282.last_plus_one)){ _throw(
Null_Exception);}* _temp5284;})); _temp5281;}); _temp5280;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5285= yyls; struct Cyc_Yyltype* _temp5287= _temp5285.curr
+( yylsp_offset - 1); if( _temp5285.base == 0? 1:( _temp5287 < _temp5285.base? 1:
_temp5287 >= _temp5285.last_plus_one)){ _throw( Null_Exception);}* _temp5287;}).first_line,({
struct _tagged_ptr3 _temp5288= yyls; struct Cyc_Yyltype* _temp5290= _temp5288.curr
+ yylsp_offset; if( _temp5288.base == 0? 1:( _temp5290 < _temp5288.base? 1:
_temp5290 >= _temp5288.last_plus_one)){ _throw( Null_Exception);}* _temp5290;}).last_line));
_temp5279;}); _temp5278;}); break; case 220: _LL5277: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5292=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5292[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5293; _temp5293.tag= Cyc_Stmt_tok; _temp5293.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Label_s_struct* _temp5294=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp5294[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp5295; _temp5295.tag= Cyc_Absyn_Label_s; _temp5295.f1=({
struct _tagged_string* _temp5296=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5296[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5297= yyvs; void** _temp5299= _temp5297.curr +( yyvsp_offset -
2); if( _temp5297.base == 0? 1:( _temp5299 < _temp5297.base? 1: _temp5299 >=
_temp5297.last_plus_one)){ _throw( Null_Exception);}* _temp5299;})); _temp5296;});
_temp5295.f2= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5300= yyvs; void**
_temp5302= _temp5300.curr + yyvsp_offset; if( _temp5300.base == 0? 1:( _temp5302
< _temp5300.base? 1: _temp5302 >= _temp5300.last_plus_one)){ _throw(
Null_Exception);}* _temp5302;})); _temp5295;}); _temp5294;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5303= yyls; struct Cyc_Yyltype* _temp5305= _temp5303.curr
+( yylsp_offset - 2); if( _temp5303.base == 0? 1:( _temp5305 < _temp5303.base? 1:
_temp5305 >= _temp5303.last_plus_one)){ _throw( Null_Exception);}* _temp5305;}).first_line,({
struct _tagged_ptr3 _temp5306= yyls; struct Cyc_Yyltype* _temp5308= _temp5306.curr
+ yylsp_offset; if( _temp5306.base == 0? 1:( _temp5308 < _temp5306.base? 1:
_temp5308 >= _temp5306.last_plus_one)){ _throw( Null_Exception);}* _temp5308;}).last_line));
_temp5293;}); _temp5292;}); break; case 221: _LL5291: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5310=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5310[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5311; _temp5311.tag= Cyc_Stmt_tok; _temp5311.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5312= yyls; struct Cyc_Yyltype* _temp5314= _temp5312.curr
+ yylsp_offset; if( _temp5312.base == 0? 1:( _temp5314 < _temp5312.base? 1:
_temp5314 >= _temp5312.last_plus_one)){ _throw( Null_Exception);}* _temp5314;}).first_line,({
struct _tagged_ptr3 _temp5315= yyls; struct Cyc_Yyltype* _temp5317= _temp5315.curr
+ yylsp_offset; if( _temp5315.base == 0? 1:( _temp5317 < _temp5315.base? 1:
_temp5317 >= _temp5315.last_plus_one)){ _throw( Null_Exception);}* _temp5317;}).last_line));
_temp5311;}); _temp5310;}); break; case 222: _LL5309: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5319=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5319[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5320; _temp5320.tag= Cyc_Stmt_tok; _temp5320.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5321= yyvs; void** _temp5323= _temp5321.curr +(
yyvsp_offset - 1); if( _temp5321.base == 0? 1:( _temp5323 < _temp5321.base? 1:
_temp5323 >= _temp5321.last_plus_one)){ _throw( Null_Exception);}* _temp5323;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5324= yyls; struct Cyc_Yyltype*
_temp5326= _temp5324.curr +( yylsp_offset - 1); if( _temp5324.base == 0? 1:(
_temp5326 < _temp5324.base? 1: _temp5326 >= _temp5324.last_plus_one)){ _throw(
Null_Exception);}* _temp5326;}).first_line,({ struct _tagged_ptr3 _temp5327=
yyls; struct Cyc_Yyltype* _temp5329= _temp5327.curr + yylsp_offset; if(
_temp5327.base == 0? 1:( _temp5329 < _temp5327.base? 1: _temp5329 >= _temp5327.last_plus_one)){
_throw( Null_Exception);}* _temp5329;}).last_line)); _temp5320;}); _temp5319;});
break; case 223: _LL5318: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5331=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5331[ 0]=({ struct Cyc_Stmt_tok_struct _temp5332; _temp5332.tag= Cyc_Stmt_tok;
_temp5332.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5333= yyls; struct Cyc_Yyltype* _temp5335= _temp5333.curr +(
yylsp_offset - 1); if( _temp5333.base == 0? 1:( _temp5335 < _temp5333.base? 1:
_temp5335 >= _temp5333.last_plus_one)){ _throw( Null_Exception);}* _temp5335;}).first_line,({
struct _tagged_ptr3 _temp5336= yyls; struct Cyc_Yyltype* _temp5338= _temp5336.curr
+ yylsp_offset; if( _temp5336.base == 0? 1:( _temp5338 < _temp5336.base? 1:
_temp5338 >= _temp5336.last_plus_one)){ _throw( Null_Exception);}* _temp5338;}).last_line));
_temp5332;}); _temp5331;}); break; case 224: _LL5330: yyval=({ struct
_tagged_ptr2 _temp5340= yyvs; void** _temp5342= _temp5340.curr +( yyvsp_offset -
1); if( _temp5340.base == 0? 1:( _temp5342 < _temp5340.base? 1: _temp5342 >=
_temp5340.last_plus_one)){ _throw( Null_Exception);}* _temp5342;}); break; case
225: _LL5339:{ void* _temp5347= Cyc_yyget_BlockItem_tok(({ struct _tagged_ptr2
_temp5344= yyvs; void** _temp5346= _temp5344.curr + yyvsp_offset; if( _temp5344.base
== 0? 1:( _temp5346 < _temp5344.base? 1: _temp5346 >= _temp5344.last_plus_one)){
_throw( Null_Exception);}* _temp5346;})); struct Cyc_List_List* _temp5355;
struct Cyc_Absyn_Fndecl* _temp5357; struct Cyc_Absyn_Stmt* _temp5359; _LL5349:
if(*(( int*) _temp5347) == Cyc_Parse_TopDecls_bl){ _LL5356: _temp5355=( struct
Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*) _temp5347)->f1; goto
_LL5350;} else{ goto _LL5351;} _LL5351: if(*(( int*) _temp5347) == Cyc_Parse_FnDecl_bl){
_LL5358: _temp5357=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp5347)->f1; goto _LL5352;} else{ goto _LL5353;} _LL5353: if(*(( int*)
_temp5347) == Cyc_Parse_Stmt_bl){ _LL5360: _temp5359=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5347)->f1; goto _LL5354;} else{ goto
_LL5348;} _LL5350: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5361=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5361[ 0]=({ struct Cyc_Stmt_tok_struct _temp5362; _temp5362.tag= Cyc_Stmt_tok;
_temp5362.f1= Cyc_Parse_flatten_declarations( _temp5355, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5363= yyls; struct Cyc_Yyltype*
_temp5365= _temp5363.curr + yylsp_offset; if( _temp5363.base == 0? 1:( _temp5365
< _temp5363.base? 1: _temp5365 >= _temp5363.last_plus_one)){ _throw(
Null_Exception);}* _temp5365;}).first_line,({ struct _tagged_ptr3 _temp5366=
yyls; struct Cyc_Yyltype* _temp5368= _temp5366.curr + yylsp_offset; if(
_temp5366.base == 0? 1:( _temp5368 < _temp5366.base? 1: _temp5368 >= _temp5366.last_plus_one)){
_throw( Null_Exception);}* _temp5368;}).last_line))); _temp5362;}); _temp5361;});
goto _LL5348; _LL5352: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5369=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5369[ 0]=({ struct Cyc_Stmt_tok_struct _temp5370; _temp5370.tag= Cyc_Stmt_tok;
_temp5370.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5371=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5371[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5372; _temp5372.tag= Cyc_Absyn_Fn_d;
_temp5372.f1= _temp5357; _temp5372;}); _temp5371;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5373= yyls; struct Cyc_Yyltype* _temp5375= _temp5373.curr
+ yylsp_offset; if( _temp5373.base == 0? 1:( _temp5375 < _temp5373.base? 1:
_temp5375 >= _temp5373.last_plus_one)){ _throw( Null_Exception);}* _temp5375;}).first_line,({
struct _tagged_ptr3 _temp5376= yyls; struct Cyc_Yyltype* _temp5378= _temp5376.curr
+ yylsp_offset; if( _temp5376.base == 0? 1:( _temp5378 < _temp5376.base? 1:
_temp5378 >= _temp5376.last_plus_one)){ _throw( Null_Exception);}* _temp5378;}).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5379= yyls; struct Cyc_Yyltype* _temp5381= _temp5379.curr + yylsp_offset;
if( _temp5379.base == 0? 1:( _temp5381 < _temp5379.base? 1: _temp5381 >=
_temp5379.last_plus_one)){ _throw( Null_Exception);}* _temp5381;}).first_line,({
struct _tagged_ptr3 _temp5382= yyls; struct Cyc_Yyltype* _temp5384= _temp5382.curr
+ yylsp_offset; if( _temp5382.base == 0? 1:( _temp5384 < _temp5382.base? 1:
_temp5384 >= _temp5382.last_plus_one)){ _throw( Null_Exception);}* _temp5384;}).last_line)));
_temp5370;}); _temp5369;}); goto _LL5348; _LL5354: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5385=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5385[ 0]=({ struct Cyc_Stmt_tok_struct _temp5386; _temp5386.tag= Cyc_Stmt_tok;
_temp5386.f1= _temp5359; _temp5386;}); _temp5385;}); goto _LL5348; _LL5348:;}
break; case 226: _LL5343:{ void* _temp5391= Cyc_yyget_BlockItem_tok(({ struct
_tagged_ptr2 _temp5388= yyvs; void** _temp5390= _temp5388.curr +( yyvsp_offset -
1); if( _temp5388.base == 0? 1:( _temp5390 < _temp5388.base? 1: _temp5390 >=
_temp5388.last_plus_one)){ _throw( Null_Exception);}* _temp5390;})); struct Cyc_List_List*
_temp5399; struct Cyc_Absyn_Fndecl* _temp5401; struct Cyc_Absyn_Stmt* _temp5403;
_LL5393: if(*(( int*) _temp5391) == Cyc_Parse_TopDecls_bl){ _LL5400: _temp5399=(
struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*) _temp5391)->f1;
goto _LL5394;} else{ goto _LL5395;} _LL5395: if(*(( int*) _temp5391) == Cyc_Parse_FnDecl_bl){
_LL5402: _temp5401=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp5391)->f1; goto _LL5396;} else{ goto _LL5397;} _LL5397: if(*(( int*)
_temp5391) == Cyc_Parse_Stmt_bl){ _LL5404: _temp5403=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5391)->f1; goto _LL5398;} else{ goto
_LL5392;} _LL5394: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5405=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5405[ 0]=({ struct Cyc_Stmt_tok_struct _temp5406; _temp5406.tag= Cyc_Stmt_tok;
_temp5406.f1= Cyc_Parse_flatten_declarations( _temp5399, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5407= yyvs; void** _temp5409= _temp5407.curr +
yyvsp_offset; if( _temp5407.base == 0? 1:( _temp5409 < _temp5407.base? 1:
_temp5409 >= _temp5407.last_plus_one)){ _throw( Null_Exception);}* _temp5409;})));
_temp5406;}); _temp5405;}); goto _LL5392; _LL5396: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5410=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5410[ 0]=({ struct Cyc_Stmt_tok_struct _temp5411; _temp5411.tag= Cyc_Stmt_tok;
_temp5411.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5412=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5412[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5413; _temp5413.tag= Cyc_Absyn_Fn_d;
_temp5413.f1= _temp5401; _temp5413;}); _temp5412;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5414= yyls; struct Cyc_Yyltype* _temp5416= _temp5414.curr
+( yylsp_offset - 1); if( _temp5414.base == 0? 1:( _temp5416 < _temp5414.base? 1:
_temp5416 >= _temp5414.last_plus_one)){ _throw( Null_Exception);}* _temp5416;}).first_line,({
struct _tagged_ptr3 _temp5417= yyls; struct Cyc_Yyltype* _temp5419= _temp5417.curr
+( yylsp_offset - 1); if( _temp5417.base == 0? 1:( _temp5419 < _temp5417.base? 1:
_temp5419 >= _temp5417.last_plus_one)){ _throw( Null_Exception);}* _temp5419;}).last_line)),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5420= yyvs; void** _temp5422=
_temp5420.curr + yyvsp_offset; if( _temp5420.base == 0? 1:( _temp5422 <
_temp5420.base? 1: _temp5422 >= _temp5420.last_plus_one)){ _throw(
Null_Exception);}* _temp5422;}))); _temp5411;}); _temp5410;}); goto _LL5392;
_LL5398: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5423=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5423[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5424; _temp5424.tag= Cyc_Stmt_tok; _temp5424.f1= Cyc_Absyn_seq_stmt(
_temp5403, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5425= yyvs; void**
_temp5427= _temp5425.curr + yyvsp_offset; if( _temp5425.base == 0? 1:( _temp5427
< _temp5425.base? 1: _temp5427 >= _temp5425.last_plus_one)){ _throw(
Null_Exception);}* _temp5427;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5428= yyls; struct Cyc_Yyltype* _temp5430= _temp5428.curr +(
yylsp_offset - 1); if( _temp5428.base == 0? 1:( _temp5430 < _temp5428.base? 1:
_temp5430 >= _temp5428.last_plus_one)){ _throw( Null_Exception);}* _temp5430;}).first_line,({
struct _tagged_ptr3 _temp5431= yyls; struct Cyc_Yyltype* _temp5433= _temp5431.curr
+ yylsp_offset; if( _temp5431.base == 0? 1:( _temp5433 < _temp5431.base? 1:
_temp5433 >= _temp5431.last_plus_one)){ _throw( Null_Exception);}* _temp5433;}).last_line));
_temp5424;}); _temp5423;}); goto _LL5392; _LL5392:;} break; case 227: _LL5387:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp5435=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5435[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5436; _temp5436.tag= Cyc_BlockItem_tok; _temp5436.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp5437=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp5437[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp5438; _temp5438.tag= Cyc_Parse_TopDecls_bl;
_temp5438.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5439= yyvs;
void** _temp5441= _temp5439.curr + yyvsp_offset; if( _temp5439.base == 0? 1:(
_temp5441 < _temp5439.base? 1: _temp5441 >= _temp5439.last_plus_one)){ _throw(
Null_Exception);}* _temp5441;})); _temp5438;}); _temp5437;})); _temp5436;});
_temp5435;}); break; case 228: _LL5434: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp5443=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5443[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5444; _temp5444.tag= Cyc_BlockItem_tok;
_temp5444.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp5445=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp5445[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp5446; _temp5446.tag= Cyc_Parse_Stmt_bl;
_temp5446.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5447= yyvs; void**
_temp5449= _temp5447.curr + yyvsp_offset; if( _temp5447.base == 0? 1:( _temp5449
< _temp5447.base? 1: _temp5449 >= _temp5447.last_plus_one)){ _throw(
Null_Exception);}* _temp5449;})); _temp5446;}); _temp5445;})); _temp5444;});
_temp5443;}); break; case 229: _LL5442: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp5451=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5451[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5452; _temp5452.tag= Cyc_BlockItem_tok;
_temp5452.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp5453=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp5453[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp5454; _temp5454.tag= Cyc_Parse_FnDecl_bl;
_temp5454.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp5455= yyvs; void**
_temp5457= _temp5455.curr + yyvsp_offset; if( _temp5455.base == 0? 1:( _temp5457
< _temp5455.base? 1: _temp5457 >= _temp5455.last_plus_one)){ _throw(
Null_Exception);}* _temp5457;})); _temp5454;}); _temp5453;})); _temp5452;});
_temp5451;}); break; case 230: _LL5450: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5459=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5459[ 0]=({ struct Cyc_Stmt_tok_struct _temp5460; _temp5460.tag= Cyc_Stmt_tok;
_temp5460.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5461= yyvs; void** _temp5463= _temp5461.curr +( yyvsp_offset -
2); if( _temp5461.base == 0? 1:( _temp5463 < _temp5461.base? 1: _temp5463 >=
_temp5461.last_plus_one)){ _throw( Null_Exception);}* _temp5463;})), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5464= yyvs; void** _temp5466= _temp5464.curr +
yyvsp_offset; if( _temp5464.base == 0? 1:( _temp5466 < _temp5464.base? 1:
_temp5466 >= _temp5464.last_plus_one)){ _throw( Null_Exception);}* _temp5466;})),
Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5467= yyls; struct Cyc_Yyltype* _temp5469= _temp5467.curr +( yylsp_offset -
4); if( _temp5467.base == 0? 1:( _temp5469 < _temp5467.base? 1: _temp5469 >=
_temp5467.last_plus_one)){ _throw( Null_Exception);}* _temp5469;}).first_line,({
struct _tagged_ptr3 _temp5470= yyls; struct Cyc_Yyltype* _temp5472= _temp5470.curr
+ yylsp_offset; if( _temp5470.base == 0? 1:( _temp5472 < _temp5470.base? 1:
_temp5472 >= _temp5470.last_plus_one)){ _throw( Null_Exception);}* _temp5472;}).last_line));
_temp5460;}); _temp5459;}); break; case 231: _LL5458: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5474=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5474[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5475; _temp5475.tag= Cyc_Stmt_tok; _temp5475.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5476= yyvs; void** _temp5478=
_temp5476.curr +( yyvsp_offset - 4); if( _temp5476.base == 0? 1:( _temp5478 <
_temp5476.base? 1: _temp5478 >= _temp5476.last_plus_one)){ _throw(
Null_Exception);}* _temp5478;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5479= yyvs; void** _temp5481= _temp5479.curr +( yyvsp_offset - 2); if(
_temp5479.base == 0? 1:( _temp5481 < _temp5479.base? 1: _temp5481 >= _temp5479.last_plus_one)){
_throw( Null_Exception);}* _temp5481;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5482= yyvs; void** _temp5484= _temp5482.curr + yyvsp_offset;
if( _temp5482.base == 0? 1:( _temp5484 < _temp5482.base? 1: _temp5484 >=
_temp5482.last_plus_one)){ _throw( Null_Exception);}* _temp5484;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5485= yyls; struct Cyc_Yyltype* _temp5487= _temp5485.curr
+( yylsp_offset - 6); if( _temp5485.base == 0? 1:( _temp5487 < _temp5485.base? 1:
_temp5487 >= _temp5485.last_plus_one)){ _throw( Null_Exception);}* _temp5487;}).first_line,({
struct _tagged_ptr3 _temp5488= yyls; struct Cyc_Yyltype* _temp5490= _temp5488.curr
+ yylsp_offset; if( _temp5488.base == 0? 1:( _temp5490 < _temp5488.base? 1:
_temp5490 >= _temp5488.last_plus_one)){ _throw( Null_Exception);}* _temp5490;}).last_line));
_temp5475;}); _temp5474;}); break; case 232: _LL5473: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5492=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5492[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5493; _temp5493.tag= Cyc_Stmt_tok; _temp5493.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5494= yyvs; void** _temp5496= _temp5494.curr +(
yyvsp_offset - 4); if( _temp5494.base == 0? 1:( _temp5496 < _temp5494.base? 1:
_temp5496 >= _temp5494.last_plus_one)){ _throw( Null_Exception);}* _temp5496;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp5497= yyvs; void**
_temp5499= _temp5497.curr +( yyvsp_offset - 1); if( _temp5497.base == 0? 1:(
_temp5499 < _temp5497.base? 1: _temp5499 >= _temp5497.last_plus_one)){ _throw(
Null_Exception);}* _temp5499;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5500= yyls; struct Cyc_Yyltype* _temp5502= _temp5500.curr +(
yylsp_offset - 6); if( _temp5500.base == 0? 1:( _temp5502 < _temp5500.base? 1:
_temp5502 >= _temp5500.last_plus_one)){ _throw( Null_Exception);}* _temp5502;}).first_line,({
struct _tagged_ptr3 _temp5503= yyls; struct Cyc_Yyltype* _temp5505= _temp5503.curr
+ yylsp_offset; if( _temp5503.base == 0? 1:( _temp5505 < _temp5503.base? 1:
_temp5505 >= _temp5503.last_plus_one)){ _throw( Null_Exception);}* _temp5505;}).last_line));
_temp5493;}); _temp5492;}); break; case 233: _LL5491: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5507=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5507[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5508; _temp5508.tag= Cyc_Stmt_tok; _temp5508.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5509= yyvs; void** _temp5511=
_temp5509.curr +( yyvsp_offset - 4); if( _temp5509.base == 0? 1:( _temp5511 <
_temp5509.base? 1: _temp5511 >= _temp5509.last_plus_one)){ _throw(
Null_Exception);}* _temp5511;})), Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5512= yyvs; void** _temp5514= _temp5512.curr +( yyvsp_offset -
1); if( _temp5512.base == 0? 1:( _temp5514 < _temp5512.base? 1: _temp5514 >=
_temp5512.last_plus_one)){ _throw( Null_Exception);}* _temp5514;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5515= yyls; struct Cyc_Yyltype* _temp5517= _temp5515.curr
+( yylsp_offset - 5); if( _temp5515.base == 0? 1:( _temp5517 < _temp5515.base? 1:
_temp5517 >= _temp5515.last_plus_one)){ _throw( Null_Exception);}* _temp5517;}).first_line,({
struct _tagged_ptr3 _temp5518= yyls; struct Cyc_Yyltype* _temp5520= _temp5518.curr
+ yylsp_offset; if( _temp5518.base == 0? 1:( _temp5520 < _temp5518.base? 1:
_temp5520 >= _temp5518.last_plus_one)){ _throw( Null_Exception);}* _temp5520;}).last_line));
_temp5508;}); _temp5507;}); break; case 234: _LL5506: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp5522=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5522[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5523; _temp5523.tag= Cyc_SwitchClauseList_tok;
_temp5523.f1= 0; _temp5523;}); _temp5522;}); break; case 235: _LL5521: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5525=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5525[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5526; _temp5526.tag= Cyc_SwitchClauseList_tok;
_temp5526.f1=({ struct Cyc_List_List* _temp5527=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5527->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp5528=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp5528->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5538= yyls; struct Cyc_Yyltype* _temp5540= _temp5538.curr
+( yylsp_offset - 2); if( _temp5538.base == 0? 1:( _temp5540 < _temp5538.base? 1:
_temp5540 >= _temp5538.last_plus_one)){ _throw( Null_Exception);}* _temp5540;}).first_line,({
struct _tagged_ptr3 _temp5541= yyls; struct Cyc_Yyltype* _temp5543= _temp5541.curr
+( yylsp_offset - 2); if( _temp5541.base == 0? 1:( _temp5543 < _temp5541.base? 1:
_temp5543 >= _temp5541.last_plus_one)){ _throw( Null_Exception);}* _temp5543;}).last_line));
_temp5528->pat_vars= 0; _temp5528->where_clause= 0; _temp5528->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5535= yyvs; void** _temp5537= _temp5535.curr +
yyvsp_offset; if( _temp5535.base == 0? 1:( _temp5537 < _temp5535.base? 1:
_temp5537 >= _temp5535.last_plus_one)){ _throw( Null_Exception);}* _temp5537;}));
_temp5528->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5529=
yyls; struct Cyc_Yyltype* _temp5531= _temp5529.curr +( yylsp_offset - 2); if(
_temp5529.base == 0? 1:( _temp5531 < _temp5529.base? 1: _temp5531 >= _temp5529.last_plus_one)){
_throw( Null_Exception);}* _temp5531;}).first_line,({ struct _tagged_ptr3
_temp5532= yyls; struct Cyc_Yyltype* _temp5534= _temp5532.curr + yylsp_offset;
if( _temp5532.base == 0? 1:( _temp5534 < _temp5532.base? 1: _temp5534 >=
_temp5532.last_plus_one)){ _throw( Null_Exception);}* _temp5534;}).last_line);
_temp5528;}); _temp5527->tl= 0; _temp5527;}); _temp5526;}); _temp5525;}); break;
case 236: _LL5524: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5545=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5545[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5546; _temp5546.tag= Cyc_SwitchClauseList_tok; _temp5546.f1=({ struct Cyc_List_List*
_temp5547=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5547->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5551=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5551->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5564= yyvs; void** _temp5566= _temp5564.curr +(
yyvsp_offset - 2); if( _temp5564.base == 0? 1:( _temp5566 < _temp5564.base? 1:
_temp5566 >= _temp5564.last_plus_one)){ _throw( Null_Exception);}* _temp5566;}));
_temp5551->pat_vars= 0; _temp5551->where_clause= 0; _temp5551->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5558= yyls; struct Cyc_Yyltype*
_temp5560= _temp5558.curr +( yylsp_offset - 1); if( _temp5558.base == 0? 1:(
_temp5560 < _temp5558.base? 1: _temp5560 >= _temp5558.last_plus_one)){ _throw(
Null_Exception);}* _temp5560;}).first_line,({ struct _tagged_ptr3 _temp5561=
yyls; struct Cyc_Yyltype* _temp5563= _temp5561.curr +( yylsp_offset - 1); if(
_temp5561.base == 0? 1:( _temp5563 < _temp5561.base? 1: _temp5563 >= _temp5561.last_plus_one)){
_throw( Null_Exception);}* _temp5563;}).last_line)); _temp5551->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5552= yyls; struct Cyc_Yyltype* _temp5554= _temp5552.curr
+( yylsp_offset - 3); if( _temp5552.base == 0? 1:( _temp5554 < _temp5552.base? 1:
_temp5554 >= _temp5552.last_plus_one)){ _throw( Null_Exception);}* _temp5554;}).first_line,({
struct _tagged_ptr3 _temp5555= yyls; struct Cyc_Yyltype* _temp5557= _temp5555.curr
+ yylsp_offset; if( _temp5555.base == 0? 1:( _temp5557 < _temp5555.base? 1:
_temp5557 >= _temp5555.last_plus_one)){ _throw( Null_Exception);}* _temp5557;}).last_line);
_temp5551;}); _temp5547->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5548= yyvs; void** _temp5550= _temp5548.curr + yyvsp_offset;
if( _temp5548.base == 0? 1:( _temp5550 < _temp5548.base? 1: _temp5550 >=
_temp5548.last_plus_one)){ _throw( Null_Exception);}* _temp5550;})); _temp5547;});
_temp5546;}); _temp5545;}); break; case 237: _LL5544: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp5568=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5568[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5569; _temp5569.tag= Cyc_SwitchClauseList_tok;
_temp5569.f1=({ struct Cyc_List_List* _temp5570=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5570->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp5574=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp5574->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5584= yyvs;
void** _temp5586= _temp5584.curr +( yyvsp_offset - 3); if( _temp5584.base == 0?
1:( _temp5586 < _temp5584.base? 1: _temp5586 >= _temp5584.last_plus_one)){
_throw( Null_Exception);}* _temp5586;})); _temp5574->pat_vars= 0; _temp5574->where_clause=
0; _temp5574->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5581= yyvs;
void** _temp5583= _temp5581.curr +( yyvsp_offset - 1); if( _temp5581.base == 0?
1:( _temp5583 < _temp5581.base? 1: _temp5583 >= _temp5581.last_plus_one)){
_throw( Null_Exception);}* _temp5583;})); _temp5574->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5575= yyls; struct Cyc_Yyltype* _temp5577= _temp5575.curr
+( yylsp_offset - 4); if( _temp5575.base == 0? 1:( _temp5577 < _temp5575.base? 1:
_temp5577 >= _temp5575.last_plus_one)){ _throw( Null_Exception);}* _temp5577;}).first_line,({
struct _tagged_ptr3 _temp5578= yyls; struct Cyc_Yyltype* _temp5580= _temp5578.curr
+( yylsp_offset - 1); if( _temp5578.base == 0? 1:( _temp5580 < _temp5578.base? 1:
_temp5580 >= _temp5578.last_plus_one)){ _throw( Null_Exception);}* _temp5580;}).last_line);
_temp5574;}); _temp5570->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5571= yyvs; void** _temp5573= _temp5571.curr + yyvsp_offset;
if( _temp5571.base == 0? 1:( _temp5573 < _temp5571.base? 1: _temp5573 >=
_temp5571.last_plus_one)){ _throw( Null_Exception);}* _temp5573;})); _temp5570;});
_temp5569;}); _temp5568;}); break; case 238: _LL5567: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp5588=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5588[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5589; _temp5589.tag= Cyc_SwitchClauseList_tok;
_temp5589.f1=({ struct Cyc_List_List* _temp5590=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5590->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp5594=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp5594->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5610= yyvs;
void** _temp5612= _temp5610.curr +( yyvsp_offset - 4); if( _temp5610.base == 0?
1:( _temp5612 < _temp5610.base? 1: _temp5612 >= _temp5610.last_plus_one)){
_throw( Null_Exception);}* _temp5612;})); _temp5594->pat_vars= 0; _temp5594->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5607= yyvs;
void** _temp5609= _temp5607.curr +( yyvsp_offset - 2); if( _temp5607.base == 0?
1:( _temp5609 < _temp5607.base? 1: _temp5609 >= _temp5607.last_plus_one)){
_throw( Null_Exception);}* _temp5609;})); _temp5594->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5601= yyls; struct Cyc_Yyltype*
_temp5603= _temp5601.curr +( yylsp_offset - 1); if( _temp5601.base == 0? 1:(
_temp5603 < _temp5601.base? 1: _temp5603 >= _temp5601.last_plus_one)){ _throw(
Null_Exception);}* _temp5603;}).first_line,({ struct _tagged_ptr3 _temp5604=
yyls; struct Cyc_Yyltype* _temp5606= _temp5604.curr +( yylsp_offset - 1); if(
_temp5604.base == 0? 1:( _temp5606 < _temp5604.base? 1: _temp5606 >= _temp5604.last_plus_one)){
_throw( Null_Exception);}* _temp5606;}).last_line)); _temp5594->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5595= yyls; struct Cyc_Yyltype* _temp5597= _temp5595.curr
+( yylsp_offset - 5); if( _temp5595.base == 0? 1:( _temp5597 < _temp5595.base? 1:
_temp5597 >= _temp5595.last_plus_one)){ _throw( Null_Exception);}* _temp5597;}).first_line,({
struct _tagged_ptr3 _temp5598= yyls; struct Cyc_Yyltype* _temp5600= _temp5598.curr
+ yylsp_offset; if( _temp5598.base == 0? 1:( _temp5600 < _temp5598.base? 1:
_temp5600 >= _temp5598.last_plus_one)){ _throw( Null_Exception);}* _temp5600;}).last_line);
_temp5594;}); _temp5590->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5591= yyvs; void** _temp5593= _temp5591.curr + yyvsp_offset;
if( _temp5591.base == 0? 1:( _temp5593 < _temp5591.base? 1: _temp5593 >=
_temp5591.last_plus_one)){ _throw( Null_Exception);}* _temp5593;})); _temp5590;});
_temp5589;}); _temp5588;}); break; case 239: _LL5587: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp5614=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5614[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5615; _temp5615.tag= Cyc_SwitchClauseList_tok;
_temp5615.f1=({ struct Cyc_List_List* _temp5616=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5616->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp5620=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp5620->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5633= yyvs;
void** _temp5635= _temp5633.curr +( yyvsp_offset - 5); if( _temp5633.base == 0?
1:( _temp5635 < _temp5633.base? 1: _temp5635 >= _temp5633.last_plus_one)){
_throw( Null_Exception);}* _temp5635;})); _temp5620->pat_vars= 0; _temp5620->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5630= yyvs;
void** _temp5632= _temp5630.curr +( yyvsp_offset - 3); if( _temp5630.base == 0?
1:( _temp5632 < _temp5630.base? 1: _temp5632 >= _temp5630.last_plus_one)){
_throw( Null_Exception);}* _temp5632;})); _temp5620->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5627= yyvs; void** _temp5629= _temp5627.curr +(
yyvsp_offset - 1); if( _temp5627.base == 0? 1:( _temp5629 < _temp5627.base? 1:
_temp5629 >= _temp5627.last_plus_one)){ _throw( Null_Exception);}* _temp5629;}));
_temp5620->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5621=
yyls; struct Cyc_Yyltype* _temp5623= _temp5621.curr +( yylsp_offset - 6); if(
_temp5621.base == 0? 1:( _temp5623 < _temp5621.base? 1: _temp5623 >= _temp5621.last_plus_one)){
_throw( Null_Exception);}* _temp5623;}).first_line,({ struct _tagged_ptr3
_temp5624= yyls; struct Cyc_Yyltype* _temp5626= _temp5624.curr + yylsp_offset;
if( _temp5624.base == 0? 1:( _temp5626 < _temp5624.base? 1: _temp5626 >=
_temp5624.last_plus_one)){ _throw( Null_Exception);}* _temp5626;}).last_line);
_temp5620;}); _temp5616->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5617= yyvs; void** _temp5619= _temp5617.curr + yyvsp_offset;
if( _temp5617.base == 0? 1:( _temp5619 < _temp5617.base? 1: _temp5619 >=
_temp5617.last_plus_one)){ _throw( Null_Exception);}* _temp5619;})); _temp5616;});
_temp5615;}); _temp5614;}); break; case 240: _LL5613: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5637=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5637[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5638; _temp5638.tag= Cyc_Stmt_tok; _temp5638.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5639= yyvs; void** _temp5641= _temp5639.curr +(
yyvsp_offset - 2); if( _temp5639.base == 0? 1:( _temp5641 < _temp5639.base? 1:
_temp5641 >= _temp5639.last_plus_one)){ _throw( Null_Exception);}* _temp5641;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5642= yyvs; void** _temp5644=
_temp5642.curr + yyvsp_offset; if( _temp5642.base == 0? 1:( _temp5644 <
_temp5642.base? 1: _temp5644 >= _temp5642.last_plus_one)){ _throw(
Null_Exception);}* _temp5644;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5645= yyls; struct Cyc_Yyltype* _temp5647= _temp5645.curr +(
yylsp_offset - 4); if( _temp5645.base == 0? 1:( _temp5647 < _temp5645.base? 1:
_temp5647 >= _temp5645.last_plus_one)){ _throw( Null_Exception);}* _temp5647;}).first_line,({
struct _tagged_ptr3 _temp5648= yyls; struct Cyc_Yyltype* _temp5650= _temp5648.curr
+ yylsp_offset; if( _temp5648.base == 0? 1:( _temp5650 < _temp5648.base? 1:
_temp5650 >= _temp5648.last_plus_one)){ _throw( Null_Exception);}* _temp5650;}).last_line));
_temp5638;}); _temp5637;}); break; case 241: _LL5636: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5652=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5652[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5653; _temp5653.tag= Cyc_Stmt_tok; _temp5653.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5654= yyvs; void** _temp5656= _temp5654.curr +(
yyvsp_offset - 5); if( _temp5654.base == 0? 1:( _temp5656 < _temp5654.base? 1:
_temp5656 >= _temp5654.last_plus_one)){ _throw( Null_Exception);}* _temp5656;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5657= yyvs; void** _temp5659=
_temp5657.curr +( yyvsp_offset - 2); if( _temp5657.base == 0? 1:( _temp5659 <
_temp5657.base? 1: _temp5659 >= _temp5657.last_plus_one)){ _throw(
Null_Exception);}* _temp5659;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5660= yyls; struct Cyc_Yyltype* _temp5662= _temp5660.curr +(
yylsp_offset - 6); if( _temp5660.base == 0? 1:( _temp5662 < _temp5660.base? 1:
_temp5662 >= _temp5660.last_plus_one)){ _throw( Null_Exception);}* _temp5662;}).first_line,({
struct _tagged_ptr3 _temp5663= yyls; struct Cyc_Yyltype* _temp5665= _temp5663.curr
+ yylsp_offset; if( _temp5663.base == 0? 1:( _temp5665 < _temp5663.base? 1:
_temp5665 >= _temp5663.last_plus_one)){ _throw( Null_Exception);}* _temp5665;}).last_line));
_temp5653;}); _temp5652;}); break; case 242: _LL5651: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5667=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5667[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5668; _temp5668.tag= Cyc_Stmt_tok; _temp5668.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5669= yyvs; void** _temp5671= _temp5669.curr +
yyvsp_offset; if( _temp5669.base == 0? 1:( _temp5671 < _temp5669.base? 1:
_temp5671 >= _temp5669.last_plus_one)){ _throw( Null_Exception);}* _temp5671;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5672= yyls; struct Cyc_Yyltype*
_temp5674= _temp5672.curr +( yylsp_offset - 5); if( _temp5672.base == 0? 1:(
_temp5674 < _temp5672.base? 1: _temp5674 >= _temp5672.last_plus_one)){ _throw(
Null_Exception);}* _temp5674;}).first_line,({ struct _tagged_ptr3 _temp5675=
yyls; struct Cyc_Yyltype* _temp5677= _temp5675.curr + yylsp_offset; if(
_temp5675.base == 0? 1:( _temp5677 < _temp5675.base? 1: _temp5677 >= _temp5675.last_plus_one)){
_throw( Null_Exception);}* _temp5677;}).last_line)); _temp5668;}); _temp5667;});
break; case 243: _LL5666: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5679=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5679[ 0]=({ struct Cyc_Stmt_tok_struct _temp5680; _temp5680.tag= Cyc_Stmt_tok;
_temp5680.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5681= yyvs; void** _temp5683=
_temp5681.curr +( yyvsp_offset - 2); if( _temp5681.base == 0? 1:( _temp5683 <
_temp5681.base? 1: _temp5683 >= _temp5681.last_plus_one)){ _throw(
Null_Exception);}* _temp5683;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5684= yyvs; void** _temp5686= _temp5684.curr + yyvsp_offset; if( _temp5684.base
== 0? 1:( _temp5686 < _temp5684.base? 1: _temp5686 >= _temp5684.last_plus_one)){
_throw( Null_Exception);}* _temp5686;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5687= yyls; struct Cyc_Yyltype* _temp5689= _temp5687.curr +(
yylsp_offset - 6); if( _temp5687.base == 0? 1:( _temp5689 < _temp5687.base? 1:
_temp5689 >= _temp5687.last_plus_one)){ _throw( Null_Exception);}* _temp5689;}).first_line,({
struct _tagged_ptr3 _temp5690= yyls; struct Cyc_Yyltype* _temp5692= _temp5690.curr
+ yylsp_offset; if( _temp5690.base == 0? 1:( _temp5692 < _temp5690.base? 1:
_temp5692 >= _temp5690.last_plus_one)){ _throw( Null_Exception);}* _temp5692;}).last_line));
_temp5680;}); _temp5679;}); break; case 244: _LL5678: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5694=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5694[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5695; _temp5695.tag= Cyc_Stmt_tok; _temp5695.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5696= yyvs; void** _temp5698=
_temp5696.curr +( yyvsp_offset - 3); if( _temp5696.base == 0? 1:( _temp5698 <
_temp5696.base? 1: _temp5698 >= _temp5696.last_plus_one)){ _throw(
Null_Exception);}* _temp5698;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5699= yyvs; void** _temp5701= _temp5699.curr +
yyvsp_offset; if( _temp5699.base == 0? 1:( _temp5701 < _temp5699.base? 1:
_temp5701 >= _temp5699.last_plus_one)){ _throw( Null_Exception);}* _temp5701;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5702= yyls; struct Cyc_Yyltype*
_temp5704= _temp5702.curr +( yylsp_offset - 6); if( _temp5702.base == 0? 1:(
_temp5704 < _temp5702.base? 1: _temp5704 >= _temp5702.last_plus_one)){ _throw(
Null_Exception);}* _temp5704;}).first_line,({ struct _tagged_ptr3 _temp5705=
yyls; struct Cyc_Yyltype* _temp5707= _temp5705.curr + yylsp_offset; if(
_temp5705.base == 0? 1:( _temp5707 < _temp5705.base? 1: _temp5707 >= _temp5705.last_plus_one)){
_throw( Null_Exception);}* _temp5707;}).last_line)); _temp5695;}); _temp5694;});
break; case 245: _LL5693: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5709=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5709[ 0]=({ struct Cyc_Stmt_tok_struct _temp5710; _temp5710.tag= Cyc_Stmt_tok;
_temp5710.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5711= yyvs; void** _temp5713= _temp5711.curr +(
yyvsp_offset - 4); if( _temp5711.base == 0? 1:( _temp5713 < _temp5711.base? 1:
_temp5713 >= _temp5711.last_plus_one)){ _throw( Null_Exception);}* _temp5713;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5714= yyvs; void** _temp5716=
_temp5714.curr +( yyvsp_offset - 2); if( _temp5714.base == 0? 1:( _temp5716 <
_temp5714.base? 1: _temp5716 >= _temp5714.last_plus_one)){ _throw(
Null_Exception);}* _temp5716;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5717= yyvs; void** _temp5719= _temp5717.curr + yyvsp_offset; if( _temp5717.base
== 0? 1:( _temp5719 < _temp5717.base? 1: _temp5719 >= _temp5717.last_plus_one)){
_throw( Null_Exception);}* _temp5719;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5720= yyls; struct Cyc_Yyltype* _temp5722= _temp5720.curr +(
yylsp_offset - 7); if( _temp5720.base == 0? 1:( _temp5722 < _temp5720.base? 1:
_temp5722 >= _temp5720.last_plus_one)){ _throw( Null_Exception);}* _temp5722;}).first_line,({
struct _tagged_ptr3 _temp5723= yyls; struct Cyc_Yyltype* _temp5725= _temp5723.curr
+( yylsp_offset - 1); if( _temp5723.base == 0? 1:( _temp5725 < _temp5723.base? 1:
_temp5725 >= _temp5723.last_plus_one)){ _throw( Null_Exception);}* _temp5725;}).last_line));
_temp5710;}); _temp5709;}); break; case 246: _LL5708: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5727=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5727[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5728; _temp5728.tag= Cyc_Stmt_tok; _temp5728.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5729= yyvs; void** _temp5731= _temp5729.curr +(
yyvsp_offset - 4); if( _temp5729.base == 0? 1:( _temp5731 < _temp5729.base? 1:
_temp5731 >= _temp5729.last_plus_one)){ _throw( Null_Exception);}* _temp5731;})),
Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5732= yyvs; void** _temp5734= _temp5732.curr + yyvsp_offset;
if( _temp5732.base == 0? 1:( _temp5734 < _temp5732.base? 1: _temp5734 >=
_temp5732.last_plus_one)){ _throw( Null_Exception);}* _temp5734;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5735= yyls; struct Cyc_Yyltype* _temp5737= _temp5735.curr
+( yylsp_offset - 6); if( _temp5735.base == 0? 1:( _temp5737 < _temp5735.base? 1:
_temp5737 >= _temp5735.last_plus_one)){ _throw( Null_Exception);}* _temp5737;}).first_line,({
struct _tagged_ptr3 _temp5738= yyls; struct Cyc_Yyltype* _temp5740= _temp5738.curr
+ yylsp_offset; if( _temp5738.base == 0? 1:( _temp5740 < _temp5738.base? 1:
_temp5740 >= _temp5738.last_plus_one)){ _throw( Null_Exception);}* _temp5740;}).last_line));
_temp5728;}); _temp5727;}); break; case 247: _LL5726: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5742=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5742[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5743; _temp5743.tag= Cyc_Stmt_tok; _temp5743.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5744= yyvs; void** _temp5746= _temp5744.curr +(
yyvsp_offset - 5); if( _temp5744.base == 0? 1:( _temp5746 < _temp5744.base? 1:
_temp5746 >= _temp5744.last_plus_one)){ _throw( Null_Exception);}* _temp5746;})),
Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5747= yyvs;
void** _temp5749= _temp5747.curr +( yyvsp_offset - 2); if( _temp5747.base == 0?
1:( _temp5749 < _temp5747.base? 1: _temp5749 >= _temp5747.last_plus_one)){
_throw( Null_Exception);}* _temp5749;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5750= yyvs; void** _temp5752= _temp5750.curr + yyvsp_offset;
if( _temp5750.base == 0? 1:( _temp5752 < _temp5750.base? 1: _temp5752 >=
_temp5750.last_plus_one)){ _throw( Null_Exception);}* _temp5752;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5753= yyls; struct Cyc_Yyltype* _temp5755= _temp5753.curr
+( yylsp_offset - 7); if( _temp5753.base == 0? 1:( _temp5755 < _temp5753.base? 1:
_temp5755 >= _temp5753.last_plus_one)){ _throw( Null_Exception);}* _temp5755;}).first_line,({
struct _tagged_ptr3 _temp5756= yyls; struct Cyc_Yyltype* _temp5758= _temp5756.curr
+ yylsp_offset; if( _temp5756.base == 0? 1:( _temp5758 < _temp5756.base? 1:
_temp5758 >= _temp5756.last_plus_one)){ _throw( Null_Exception);}* _temp5758;}).last_line));
_temp5743;}); _temp5742;}); break; case 248: _LL5741: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5760=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5760[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5761; _temp5761.tag= Cyc_Stmt_tok; _temp5761.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5762= yyvs; void** _temp5764= _temp5762.curr +(
yyvsp_offset - 5); if( _temp5762.base == 0? 1:( _temp5764 < _temp5762.base? 1:
_temp5764 >= _temp5762.last_plus_one)){ _throw( Null_Exception);}* _temp5764;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5765= yyvs; void** _temp5767=
_temp5765.curr +( yyvsp_offset - 3); if( _temp5765.base == 0? 1:( _temp5767 <
_temp5765.base? 1: _temp5767 >= _temp5765.last_plus_one)){ _throw(
Null_Exception);}* _temp5767;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5768= yyvs; void** _temp5770= _temp5768.curr +
yyvsp_offset; if( _temp5768.base == 0? 1:( _temp5770 < _temp5768.base? 1:
_temp5770 >= _temp5768.last_plus_one)){ _throw( Null_Exception);}* _temp5770;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5771= yyls; struct Cyc_Yyltype*
_temp5773= _temp5771.curr +( yylsp_offset - 7); if( _temp5771.base == 0? 1:(
_temp5773 < _temp5771.base? 1: _temp5773 >= _temp5771.last_plus_one)){ _throw(
Null_Exception);}* _temp5773;}).first_line,({ struct _tagged_ptr3 _temp5774=
yyls; struct Cyc_Yyltype* _temp5776= _temp5774.curr + yylsp_offset; if(
_temp5774.base == 0? 1:( _temp5776 < _temp5774.base? 1: _temp5776 >= _temp5774.last_plus_one)){
_throw( Null_Exception);}* _temp5776;}).last_line)); _temp5761;}); _temp5760;});
break; case 249: _LL5759: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5778=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5778[ 0]=({ struct Cyc_Stmt_tok_struct _temp5779; _temp5779.tag= Cyc_Stmt_tok;
_temp5779.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5780= yyvs; void** _temp5782= _temp5780.curr +( yyvsp_offset - 6); if(
_temp5780.base == 0? 1:( _temp5782 < _temp5780.base? 1: _temp5782 >= _temp5780.last_plus_one)){
_throw( Null_Exception);}* _temp5782;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5783= yyvs; void** _temp5785= _temp5783.curr +( yyvsp_offset -
4); if( _temp5783.base == 0? 1:( _temp5785 < _temp5783.base? 1: _temp5785 >=
_temp5783.last_plus_one)){ _throw( Null_Exception);}* _temp5785;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5786= yyvs; void** _temp5788= _temp5786.curr +(
yyvsp_offset - 2); if( _temp5786.base == 0? 1:( _temp5788 < _temp5786.base? 1:
_temp5788 >= _temp5786.last_plus_one)){ _throw( Null_Exception);}* _temp5788;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5789= yyvs; void** _temp5791=
_temp5789.curr + yyvsp_offset; if( _temp5789.base == 0? 1:( _temp5791 <
_temp5789.base? 1: _temp5791 >= _temp5789.last_plus_one)){ _throw(
Null_Exception);}* _temp5791;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5792= yyls; struct Cyc_Yyltype* _temp5794= _temp5792.curr +(
yylsp_offset - 8); if( _temp5792.base == 0? 1:( _temp5794 < _temp5792.base? 1:
_temp5794 >= _temp5792.last_plus_one)){ _throw( Null_Exception);}* _temp5794;}).first_line,({
struct _tagged_ptr3 _temp5795= yyls; struct Cyc_Yyltype* _temp5797= _temp5795.curr
+ yylsp_offset; if( _temp5795.base == 0? 1:( _temp5797 < _temp5795.base? 1:
_temp5797 >= _temp5795.last_plus_one)){ _throw( Null_Exception);}* _temp5797;}).last_line));
_temp5779;}); _temp5778;}); break; case 250: _LL5777: { struct Cyc_List_List*
decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5810= yyvs; void**
_temp5812= _temp5810.curr +( yyvsp_offset - 3); if( _temp5810.base == 0? 1:(
_temp5812 < _temp5810.base? 1: _temp5812 >= _temp5810.last_plus_one)){ _throw(
Null_Exception);}* _temp5812;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5801= yyvs; void** _temp5803= _temp5801.curr +
yyvsp_offset; if( _temp5801.base == 0? 1:( _temp5803 < _temp5801.base? 1:
_temp5803 >= _temp5801.last_plus_one)){ _throw( Null_Exception);}* _temp5803;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5804= yyls; struct Cyc_Yyltype*
_temp5806= _temp5804.curr +( yylsp_offset - 5); if( _temp5804.base == 0? 1:(
_temp5806 < _temp5804.base? 1: _temp5806 >= _temp5804.last_plus_one)){ _throw(
Null_Exception);}* _temp5806;}).first_line,({ struct _tagged_ptr3 _temp5807=
yyls; struct Cyc_Yyltype* _temp5809= _temp5807.curr + yylsp_offset; if(
_temp5807.base == 0? 1:( _temp5809 < _temp5807.base? 1: _temp5809 >= _temp5807.last_plus_one)){
_throw( Null_Exception);}* _temp5809;}).last_line)); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5799=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5799[ 0]=({ struct Cyc_Stmt_tok_struct _temp5800; _temp5800.tag= Cyc_Stmt_tok;
_temp5800.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5800;}); _temp5799;});
break;} case 251: _LL5798: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5828= yyvs; void** _temp5830= _temp5828.curr +(
yyvsp_offset - 4); if( _temp5828.base == 0? 1:( _temp5830 < _temp5828.base? 1:
_temp5830 >= _temp5828.last_plus_one)){ _throw( Null_Exception);}* _temp5830;}));
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5816= yyvs; void** _temp5818= _temp5816.curr +(
yyvsp_offset - 3); if( _temp5816.base == 0? 1:( _temp5818 < _temp5816.base? 1:
_temp5818 >= _temp5816.last_plus_one)){ _throw( Null_Exception);}* _temp5818;})),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5819=
yyvs; void** _temp5821= _temp5819.curr + yyvsp_offset; if( _temp5819.base == 0?
1:( _temp5821 < _temp5819.base? 1: _temp5821 >= _temp5819.last_plus_one)){
_throw( Null_Exception);}* _temp5821;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5822= yyls; struct Cyc_Yyltype* _temp5824= _temp5822.curr +(
yylsp_offset - 6); if( _temp5822.base == 0? 1:( _temp5824 < _temp5822.base? 1:
_temp5824 >= _temp5822.last_plus_one)){ _throw( Null_Exception);}* _temp5824;}).first_line,({
struct _tagged_ptr3 _temp5825= yyls; struct Cyc_Yyltype* _temp5827= _temp5825.curr
+ yylsp_offset; if( _temp5825.base == 0? 1:( _temp5827 < _temp5825.base? 1:
_temp5827 >= _temp5825.last_plus_one)){ _throw( Null_Exception);}* _temp5827;}).last_line));
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5814=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5814[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5815; _temp5815.tag= Cyc_Stmt_tok; _temp5815.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5815;}); _temp5814;}); break;} case 252: _LL5813: { struct Cyc_List_List*
decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5846= yyvs; void**
_temp5848= _temp5846.curr +( yyvsp_offset - 4); if( _temp5846.base == 0? 1:(
_temp5848 < _temp5846.base? 1: _temp5848 >= _temp5846.last_plus_one)){ _throw(
Null_Exception);}* _temp5848;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5834= yyvs; void** _temp5836= _temp5834.curr +( yyvsp_offset -
2); if( _temp5834.base == 0? 1:( _temp5836 < _temp5834.base? 1: _temp5836 >=
_temp5834.last_plus_one)){ _throw( Null_Exception);}* _temp5836;})), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5837= yyvs; void** _temp5839= _temp5837.curr +
yyvsp_offset; if( _temp5837.base == 0? 1:( _temp5839 < _temp5837.base? 1:
_temp5839 >= _temp5837.last_plus_one)){ _throw( Null_Exception);}* _temp5839;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5840= yyls; struct Cyc_Yyltype*
_temp5842= _temp5840.curr +( yylsp_offset - 6); if( _temp5840.base == 0? 1:(
_temp5842 < _temp5840.base? 1: _temp5842 >= _temp5840.last_plus_one)){ _throw(
Null_Exception);}* _temp5842;}).first_line,({ struct _tagged_ptr3 _temp5843=
yyls; struct Cyc_Yyltype* _temp5845= _temp5843.curr + yylsp_offset; if(
_temp5843.base == 0? 1:( _temp5845 < _temp5843.base? 1: _temp5845 >= _temp5843.last_plus_one)){
_throw( Null_Exception);}* _temp5845;}).last_line)); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5832=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5832[ 0]=({ struct Cyc_Stmt_tok_struct _temp5833; _temp5833.tag= Cyc_Stmt_tok;
_temp5833.f1= Cyc_Parse_flatten_declarations( decls, s); _temp5833;}); _temp5832;});
break;} case 253: _LL5831: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5867= yyvs; void** _temp5869= _temp5867.curr +(
yyvsp_offset - 5); if( _temp5867.base == 0? 1:( _temp5869 < _temp5867.base? 1:
_temp5869 >= _temp5867.last_plus_one)){ _throw( Null_Exception);}* _temp5869;}));
struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5852= yyvs; void** _temp5854= _temp5852.curr +(
yyvsp_offset - 4); if( _temp5852.base == 0? 1:( _temp5854 < _temp5852.base? 1:
_temp5854 >= _temp5852.last_plus_one)){ _throw( Null_Exception);}* _temp5854;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5855= yyvs; void** _temp5857=
_temp5855.curr +( yyvsp_offset - 2); if( _temp5855.base == 0? 1:( _temp5857 <
_temp5855.base? 1: _temp5857 >= _temp5855.last_plus_one)){ _throw(
Null_Exception);}* _temp5857;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5858= yyvs; void** _temp5860= _temp5858.curr + yyvsp_offset; if( _temp5858.base
== 0? 1:( _temp5860 < _temp5858.base? 1: _temp5860 >= _temp5858.last_plus_one)){
_throw( Null_Exception);}* _temp5860;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5861= yyls; struct Cyc_Yyltype* _temp5863= _temp5861.curr +(
yylsp_offset - 7); if( _temp5861.base == 0? 1:( _temp5863 < _temp5861.base? 1:
_temp5863 >= _temp5861.last_plus_one)){ _throw( Null_Exception);}* _temp5863;}).first_line,({
struct _tagged_ptr3 _temp5864= yyls; struct Cyc_Yyltype* _temp5866= _temp5864.curr
+ yylsp_offset; if( _temp5864.base == 0? 1:( _temp5866 < _temp5864.base? 1:
_temp5866 >= _temp5864.last_plus_one)){ _throw( Null_Exception);}* _temp5866;}).last_line));
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5850=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5850[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5851; _temp5851.tag= Cyc_Stmt_tok; _temp5851.f1= Cyc_Parse_flatten_declarations(
decls, s); _temp5851;}); _temp5850;}); break;} case 254: _LL5849: yyval=( void*)({
struct Cyc_Stmt_tok_struct* _temp5871=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp5871[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5872; _temp5872.tag= Cyc_Stmt_tok; _temp5872.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_string* _temp5873=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5873[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5874= yyvs; void** _temp5876= _temp5874.curr +( yyvsp_offset -
1); if( _temp5874.base == 0? 1:( _temp5876 < _temp5874.base? 1: _temp5876 >=
_temp5874.last_plus_one)){ _throw( Null_Exception);}* _temp5876;})); _temp5873;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5877= yyls; struct Cyc_Yyltype*
_temp5879= _temp5877.curr +( yylsp_offset - 2); if( _temp5877.base == 0? 1:(
_temp5879 < _temp5877.base? 1: _temp5879 >= _temp5877.last_plus_one)){ _throw(
Null_Exception);}* _temp5879;}).first_line,({ struct _tagged_ptr3 _temp5880=
yyls; struct Cyc_Yyltype* _temp5882= _temp5880.curr +( yylsp_offset - 1); if(
_temp5880.base == 0? 1:( _temp5882 < _temp5880.base? 1: _temp5882 >= _temp5880.last_plus_one)){
_throw( Null_Exception);}* _temp5882;}).last_line)); _temp5872;}); _temp5871;});
break; case 255: _LL5870: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5884=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5884[ 0]=({ struct Cyc_Stmt_tok_struct _temp5885; _temp5885.tag= Cyc_Stmt_tok;
_temp5885.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5886= yyls; struct Cyc_Yyltype* _temp5888= _temp5886.curr +(
yylsp_offset - 1); if( _temp5886.base == 0? 1:( _temp5888 < _temp5886.base? 1:
_temp5888 >= _temp5886.last_plus_one)){ _throw( Null_Exception);}* _temp5888;}).first_line,({
struct _tagged_ptr3 _temp5889= yyls; struct Cyc_Yyltype* _temp5891= _temp5889.curr
+( yylsp_offset - 1); if( _temp5889.base == 0? 1:( _temp5891 < _temp5889.base? 1:
_temp5891 >= _temp5889.last_plus_one)){ _throw( Null_Exception);}* _temp5891;}).last_line));
_temp5885;}); _temp5884;}); break; case 256: _LL5883: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5893=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5893[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5894; _temp5894.tag= Cyc_Stmt_tok; _temp5894.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5895= yyls; struct Cyc_Yyltype* _temp5897= _temp5895.curr
+( yylsp_offset - 1); if( _temp5895.base == 0? 1:( _temp5897 < _temp5895.base? 1:
_temp5897 >= _temp5895.last_plus_one)){ _throw( Null_Exception);}* _temp5897;}).first_line,({
struct _tagged_ptr3 _temp5898= yyls; struct Cyc_Yyltype* _temp5900= _temp5898.curr
+( yylsp_offset - 1); if( _temp5898.base == 0? 1:( _temp5900 < _temp5898.base? 1:
_temp5900 >= _temp5898.last_plus_one)){ _throw( Null_Exception);}* _temp5900;}).last_line));
_temp5894;}); _temp5893;}); break; case 257: _LL5892: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5902=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5902[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5903; _temp5903.tag= Cyc_Stmt_tok; _temp5903.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5904= yyls; struct Cyc_Yyltype*
_temp5906= _temp5904.curr +( yylsp_offset - 1); if( _temp5904.base == 0? 1:(
_temp5906 < _temp5904.base? 1: _temp5906 >= _temp5904.last_plus_one)){ _throw(
Null_Exception);}* _temp5906;}).first_line,({ struct _tagged_ptr3 _temp5907=
yyls; struct Cyc_Yyltype* _temp5909= _temp5907.curr +( yylsp_offset - 1); if(
_temp5907.base == 0? 1:( _temp5909 < _temp5907.base? 1: _temp5909 >= _temp5907.last_plus_one)){
_throw( Null_Exception);}* _temp5909;}).last_line)); _temp5903;}); _temp5902;});
break; case 258: _LL5901: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5911=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5911[ 0]=({ struct Cyc_Stmt_tok_struct _temp5912; _temp5912.tag= Cyc_Stmt_tok;
_temp5912.f1= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5913= yyvs; void** _temp5915= _temp5913.curr +(
yyvsp_offset - 1); if( _temp5913.base == 0? 1:( _temp5915 < _temp5913.base? 1:
_temp5915 >= _temp5913.last_plus_one)){ _throw( Null_Exception);}* _temp5915;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5916= yyls; struct Cyc_Yyltype*
_temp5918= _temp5916.curr +( yylsp_offset - 2); if( _temp5916.base == 0? 1:(
_temp5918 < _temp5916.base? 1: _temp5918 >= _temp5916.last_plus_one)){ _throw(
Null_Exception);}* _temp5918;}).first_line,({ struct _tagged_ptr3 _temp5919=
yyls; struct Cyc_Yyltype* _temp5921= _temp5919.curr +( yylsp_offset - 1); if(
_temp5919.base == 0? 1:( _temp5921 < _temp5919.base? 1: _temp5921 >= _temp5919.last_plus_one)){
_throw( Null_Exception);}* _temp5921;}).last_line)); _temp5912;}); _temp5911;});
break; case 259: _LL5910: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5923=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5923[ 0]=({ struct Cyc_Stmt_tok_struct _temp5924; _temp5924.tag= Cyc_Stmt_tok;
_temp5924.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5925= yyls; struct Cyc_Yyltype* _temp5927= _temp5925.curr +(
yylsp_offset - 1); if( _temp5925.base == 0? 1:( _temp5927 < _temp5925.base? 1:
_temp5927 >= _temp5925.last_plus_one)){ _throw( Null_Exception);}* _temp5927;}).first_line,({
struct _tagged_ptr3 _temp5928= yyls; struct Cyc_Yyltype* _temp5930= _temp5928.curr
+( yylsp_offset - 1); if( _temp5928.base == 0? 1:( _temp5930 < _temp5928.base? 1:
_temp5930 >= _temp5928.last_plus_one)){ _throw( Null_Exception);}* _temp5930;}).last_line));
_temp5924;}); _temp5923;}); break; case 260: _LL5922: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5932=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5932[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5933; _temp5933.tag= Cyc_Stmt_tok; _temp5933.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5934= yyls; struct Cyc_Yyltype*
_temp5936= _temp5934.curr +( yylsp_offset - 3); if( _temp5934.base == 0? 1:(
_temp5936 < _temp5934.base? 1: _temp5936 >= _temp5934.last_plus_one)){ _throw(
Null_Exception);}* _temp5936;}).first_line,({ struct _tagged_ptr3 _temp5937=
yyls; struct Cyc_Yyltype* _temp5939= _temp5937.curr +( yylsp_offset - 3); if(
_temp5937.base == 0? 1:( _temp5939 < _temp5937.base? 1: _temp5939 >= _temp5937.last_plus_one)){
_throw( Null_Exception);}* _temp5939;}).last_line)); _temp5933;}); _temp5932;});
break; case 261: _LL5931: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5941=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5941[ 0]=({ struct Cyc_Stmt_tok_struct _temp5942; _temp5942.tag= Cyc_Stmt_tok;
_temp5942.f1= Cyc_Absyn_fallthru_stmt( Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp5943= yyvs; void** _temp5945= _temp5943.curr +( yyvsp_offset -
2); if( _temp5943.base == 0? 1:( _temp5945 < _temp5943.base? 1: _temp5945 >=
_temp5943.last_plus_one)){ _throw( Null_Exception);}* _temp5945;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5946= yyls; struct Cyc_Yyltype* _temp5948= _temp5946.curr
+( yylsp_offset - 4); if( _temp5946.base == 0? 1:( _temp5948 < _temp5946.base? 1:
_temp5948 >= _temp5946.last_plus_one)){ _throw( Null_Exception);}* _temp5948;}).first_line,({
struct _tagged_ptr3 _temp5949= yyls; struct Cyc_Yyltype* _temp5951= _temp5949.curr
+( yylsp_offset - 4); if( _temp5949.base == 0? 1:( _temp5951 < _temp5949.base? 1:
_temp5951 >= _temp5949.last_plus_one)){ _throw( Null_Exception);}* _temp5951;}).last_line));
_temp5942;}); _temp5941;}); break; case 262: _LL5940: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp5953=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp5953[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5954; _temp5954.tag= Cyc_Pattern_tok; _temp5954.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5955= yyls; struct Cyc_Yyltype* _temp5957= _temp5955.curr + yylsp_offset;
if( _temp5955.base == 0? 1:( _temp5957 < _temp5955.base? 1: _temp5957 >=
_temp5955.last_plus_one)){ _throw( Null_Exception);}* _temp5957;}).first_line,({
struct _tagged_ptr3 _temp5958= yyls; struct Cyc_Yyltype* _temp5960= _temp5958.curr
+ yylsp_offset; if( _temp5958.base == 0? 1:( _temp5960 < _temp5958.base? 1:
_temp5960 >= _temp5958.last_plus_one)){ _throw( Null_Exception);}* _temp5960;}).last_line));
_temp5954;}); _temp5953;}); break; case 263: _LL5952: yyval=({ struct
_tagged_ptr2 _temp5962= yyvs; void** _temp5964= _temp5962.curr +( yyvsp_offset -
1); if( _temp5962.base == 0? 1:( _temp5964 < _temp5962.base? 1: _temp5964 >=
_temp5962.last_plus_one)){ _throw( Null_Exception);}* _temp5964;}); break; case
264: _LL5961: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp5966=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5966[ 0]=({ struct Cyc_Pattern_tok_struct _temp5967; _temp5967.tag= Cyc_Pattern_tok;
_temp5967.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp5968=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp5968[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp5969; _temp5969.tag= Cyc_Absyn_Int_p;
_temp5969.f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5970= yyvs;
void** _temp5972= _temp5970.curr + yyvsp_offset; if( _temp5970.base == 0? 1:(
_temp5972 < _temp5970.base? 1: _temp5972 >= _temp5970.last_plus_one)){ _throw(
Null_Exception);}* _temp5972;}))).f1; _temp5969.f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5973= yyvs; void** _temp5975= _temp5973.curr +
yyvsp_offset; if( _temp5973.base == 0? 1:( _temp5975 < _temp5973.base? 1:
_temp5975 >= _temp5973.last_plus_one)){ _throw( Null_Exception);}* _temp5975;}))).f2;
_temp5969;}); _temp5968;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5976= yyls; struct Cyc_Yyltype* _temp5978= _temp5976.curr + yylsp_offset;
if( _temp5976.base == 0? 1:( _temp5978 < _temp5976.base? 1: _temp5978 >=
_temp5976.last_plus_one)){ _throw( Null_Exception);}* _temp5978;}).first_line,({
struct _tagged_ptr3 _temp5979= yyls; struct Cyc_Yyltype* _temp5981= _temp5979.curr
+ yylsp_offset; if( _temp5979.base == 0? 1:( _temp5981 < _temp5979.base? 1:
_temp5981 >= _temp5979.last_plus_one)){ _throw( Null_Exception);}* _temp5981;}).last_line));
_temp5967;}); _temp5966;}); break; case 265: _LL5965: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp5983=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp5983[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5984; _temp5984.tag= Cyc_Pattern_tok; _temp5984.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp5985=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5985[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp5986; _temp5986.tag= Cyc_Absyn_Int_p; _temp5986.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp5986.f2= -(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5987= yyvs; void**
_temp5989= _temp5987.curr + yyvsp_offset; if( _temp5987.base == 0? 1:( _temp5989
< _temp5987.base? 1: _temp5989 >= _temp5987.last_plus_one)){ _throw(
Null_Exception);}* _temp5989;}))).f2; _temp5986;}); _temp5985;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5990= yyls; struct Cyc_Yyltype* _temp5992= _temp5990.curr
+( yylsp_offset - 1); if( _temp5990.base == 0? 1:( _temp5992 < _temp5990.base? 1:
_temp5992 >= _temp5990.last_plus_one)){ _throw( Null_Exception);}* _temp5992;}).first_line,({
struct _tagged_ptr3 _temp5993= yyls; struct Cyc_Yyltype* _temp5995= _temp5993.curr
+ yylsp_offset; if( _temp5993.base == 0? 1:( _temp5995 < _temp5993.base? 1:
_temp5995 >= _temp5993.last_plus_one)){ _throw( Null_Exception);}* _temp5995;}).last_line));
_temp5984;}); _temp5983;}); break; case 266: _LL5982: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp5997=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp5997[ 0]=({ struct Cyc_Pattern_tok_struct
_temp5998; _temp5998.tag= Cyc_Pattern_tok; _temp5998.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp5999=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp5999[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp6000; _temp6000.tag= Cyc_Absyn_Float_p; _temp6000.f1=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6001= yyvs; void** _temp6003=
_temp6001.curr + yyvsp_offset; if( _temp6001.base == 0? 1:( _temp6003 <
_temp6001.base? 1: _temp6003 >= _temp6001.last_plus_one)){ _throw(
Null_Exception);}* _temp6003;})); _temp6000;}); _temp5999;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6004= yyls; struct Cyc_Yyltype* _temp6006= _temp6004.curr
+ yylsp_offset; if( _temp6004.base == 0? 1:( _temp6006 < _temp6004.base? 1:
_temp6006 >= _temp6004.last_plus_one)){ _throw( Null_Exception);}* _temp6006;}).first_line,({
struct _tagged_ptr3 _temp6007= yyls; struct Cyc_Yyltype* _temp6009= _temp6007.curr
+ yylsp_offset; if( _temp6007.base == 0? 1:( _temp6009 < _temp6007.base? 1:
_temp6009 >= _temp6007.last_plus_one)){ _throw( Null_Exception);}* _temp6009;}).last_line));
_temp5998;}); _temp5997;}); break; case 267: _LL5996: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6011=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6011[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6012; _temp6012.tag= Cyc_Pattern_tok; _temp6012.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Char_p_struct* _temp6013=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp6013[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp6014; _temp6014.tag= Cyc_Absyn_Char_p; _temp6014.f1= Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp6015= yyvs; void** _temp6017= _temp6015.curr +
yyvsp_offset; if( _temp6015.base == 0? 1:( _temp6017 < _temp6015.base? 1:
_temp6017 >= _temp6015.last_plus_one)){ _throw( Null_Exception);}* _temp6017;}));
_temp6014;}); _temp6013;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6018= yyls; struct Cyc_Yyltype* _temp6020= _temp6018.curr + yylsp_offset;
if( _temp6018.base == 0? 1:( _temp6020 < _temp6018.base? 1: _temp6020 >=
_temp6018.last_plus_one)){ _throw( Null_Exception);}* _temp6020;}).first_line,({
struct _tagged_ptr3 _temp6021= yyls; struct Cyc_Yyltype* _temp6023= _temp6021.curr
+ yylsp_offset; if( _temp6021.base == 0? 1:( _temp6023 < _temp6021.base? 1:
_temp6023 >= _temp6021.last_plus_one)){ _throw( Null_Exception);}* _temp6023;}).last_line));
_temp6012;}); _temp6011;}); break; case 268: _LL6010: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6025=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6025[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6026; _temp6026.tag= Cyc_Pattern_tok; _temp6026.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6027= yyls; struct Cyc_Yyltype* _temp6029= _temp6027.curr + yylsp_offset;
if( _temp6027.base == 0? 1:( _temp6029 < _temp6027.base? 1: _temp6029 >=
_temp6027.last_plus_one)){ _throw( Null_Exception);}* _temp6029;}).first_line,({
struct _tagged_ptr3 _temp6030= yyls; struct Cyc_Yyltype* _temp6032= _temp6030.curr
+ yylsp_offset; if( _temp6030.base == 0? 1:( _temp6032 < _temp6030.base? 1:
_temp6032 >= _temp6030.last_plus_one)){ _throw( Null_Exception);}* _temp6032;}).last_line));
_temp6026;}); _temp6025;}); break; case 269: _LL6024: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6034=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6034[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6035; _temp6035.tag= Cyc_Pattern_tok; _temp6035.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownId_p_struct* _temp6036=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp6036[ 0]=({
struct Cyc_Absyn_UnknownId_p_struct _temp6037; _temp6037.tag= Cyc_Absyn_UnknownId_p;
_temp6037.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6038= yyvs; void**
_temp6040= _temp6038.curr + yyvsp_offset; if( _temp6038.base == 0? 1:( _temp6040
< _temp6038.base? 1: _temp6040 >= _temp6038.last_plus_one)){ _throw(
Null_Exception);}* _temp6040;})); _temp6037;}); _temp6036;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6041= yyls; struct Cyc_Yyltype* _temp6043= _temp6041.curr
+ yylsp_offset; if( _temp6041.base == 0? 1:( _temp6043 < _temp6041.base? 1:
_temp6043 >= _temp6041.last_plus_one)){ _throw( Null_Exception);}* _temp6043;}).first_line,({
struct _tagged_ptr3 _temp6044= yyls; struct Cyc_Yyltype* _temp6046= _temp6044.curr
+ yylsp_offset; if( _temp6044.base == 0? 1:( _temp6046 < _temp6044.base? 1:
_temp6046 >= _temp6044.last_plus_one)){ _throw( Null_Exception);}* _temp6046;}).last_line));
_temp6035;}); _temp6034;}); break; case 270: _LL6033: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6064=
yyls; struct Cyc_Yyltype* _temp6066= _temp6064.curr +( yylsp_offset - 3); if(
_temp6064.base == 0? 1:( _temp6066 < _temp6064.base? 1: _temp6066 >= _temp6064.last_plus_one)){
_throw( Null_Exception);}* _temp6066;}).first_line,({ struct _tagged_ptr3
_temp6067= yyls; struct Cyc_Yyltype* _temp6069= _temp6067.curr +( yylsp_offset -
3); if( _temp6067.base == 0? 1:( _temp6069 < _temp6067.base? 1: _temp6069 >=
_temp6067.last_plus_one)){ _throw( Null_Exception);}* _temp6069;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6070= yyvs; void** _temp6072=
_temp6070.curr +( yyvsp_offset - 3); if( _temp6070.base == 0? 1:( _temp6072 <
_temp6070.base? 1: _temp6072 >= _temp6070.last_plus_one)){ _throw(
Null_Exception);}* _temp6072;}))); yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp6048=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6048[ 0]=({ struct Cyc_Pattern_tok_struct _temp6049; _temp6049.tag= Cyc_Pattern_tok;
_temp6049.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp6050=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp6050[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp6051; _temp6051.tag= Cyc_Absyn_UnknownCall_p; _temp6051.f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6052= yyvs; void** _temp6054= _temp6052.curr +(
yyvsp_offset - 4); if( _temp6052.base == 0? 1:( _temp6054 < _temp6052.base? 1:
_temp6054 >= _temp6052.last_plus_one)){ _throw( Null_Exception);}* _temp6054;}));
_temp6051.f2= tvs; _temp6051.f3= Cyc_yyget_PatternList_tok(({ struct
_tagged_ptr2 _temp6055= yyvs; void** _temp6057= _temp6055.curr +( yyvsp_offset -
1); if( _temp6055.base == 0? 1:( _temp6057 < _temp6055.base? 1: _temp6057 >=
_temp6055.last_plus_one)){ _throw( Null_Exception);}* _temp6057;})); _temp6051;});
_temp6050;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6058= yyls;
struct Cyc_Yyltype* _temp6060= _temp6058.curr +( yylsp_offset - 4); if(
_temp6058.base == 0? 1:( _temp6060 < _temp6058.base? 1: _temp6060 >= _temp6058.last_plus_one)){
_throw( Null_Exception);}* _temp6060;}).first_line,({ struct _tagged_ptr3
_temp6061= yyls; struct Cyc_Yyltype* _temp6063= _temp6061.curr + yylsp_offset;
if( _temp6061.base == 0? 1:( _temp6063 < _temp6061.base? 1: _temp6063 >=
_temp6061.last_plus_one)){ _throw( Null_Exception);}* _temp6063;}).last_line));
_temp6049;}); _temp6048;}); break;} case 271: _LL6047: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6074=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6074[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6075; _temp6075.tag= Cyc_Pattern_tok; _temp6075.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp6076=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp6076[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp6077; _temp6077.tag= Cyc_Absyn_Tuple_p; _temp6077.f1=
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6078= yyvs; void**
_temp6080= _temp6078.curr +( yyvsp_offset - 1); if( _temp6078.base == 0? 1:(
_temp6080 < _temp6078.base? 1: _temp6080 >= _temp6078.last_plus_one)){ _throw(
Null_Exception);}* _temp6080;})); _temp6077;}); _temp6076;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6081= yyls; struct Cyc_Yyltype* _temp6083= _temp6081.curr
+( yylsp_offset - 3); if( _temp6081.base == 0? 1:( _temp6083 < _temp6081.base? 1:
_temp6083 >= _temp6081.last_plus_one)){ _throw( Null_Exception);}* _temp6083;}).first_line,({
struct _tagged_ptr3 _temp6084= yyls; struct Cyc_Yyltype* _temp6086= _temp6084.curr
+ yylsp_offset; if( _temp6084.base == 0? 1:( _temp6086 < _temp6084.base? 1:
_temp6086 >= _temp6084.last_plus_one)){ _throw( Null_Exception);}* _temp6086;}).last_line));
_temp6075;}); _temp6074;}); break; case 272: _LL6073: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6101=
yyls; struct Cyc_Yyltype* _temp6103= _temp6101.curr +( yylsp_offset - 2); if(
_temp6101.base == 0? 1:( _temp6103 < _temp6101.base? 1: _temp6103 >= _temp6101.last_plus_one)){
_throw( Null_Exception);}* _temp6103;}).first_line,({ struct _tagged_ptr3
_temp6104= yyls; struct Cyc_Yyltype* _temp6106= _temp6104.curr +( yylsp_offset -
2); if( _temp6104.base == 0? 1:( _temp6106 < _temp6104.base? 1: _temp6106 >=
_temp6104.last_plus_one)){ _throw( Null_Exception);}* _temp6106;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6107= yyvs; void** _temp6109=
_temp6107.curr +( yyvsp_offset - 2); if( _temp6107.base == 0? 1:( _temp6109 <
_temp6107.base? 1: _temp6109 >= _temp6107.last_plus_one)){ _throw(
Null_Exception);}* _temp6109;}))); yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp6088=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6088[ 0]=({ struct Cyc_Pattern_tok_struct _temp6089; _temp6089.tag= Cyc_Pattern_tok;
_temp6089.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp6090=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp6090[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp6091; _temp6091.tag= Cyc_Absyn_UnknownFields_p; _temp6091.f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6092= yyvs; void** _temp6094= _temp6092.curr +(
yyvsp_offset - 3); if( _temp6092.base == 0? 1:( _temp6094 < _temp6092.base? 1:
_temp6094 >= _temp6092.last_plus_one)){ _throw( Null_Exception);}* _temp6094;}));
_temp6091.f2= tvs; _temp6091.f3= 0; _temp6091;}); _temp6090;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6095= yyls; struct Cyc_Yyltype* _temp6097= _temp6095.curr
+( yylsp_offset - 3); if( _temp6095.base == 0? 1:( _temp6097 < _temp6095.base? 1:
_temp6097 >= _temp6095.last_plus_one)){ _throw( Null_Exception);}* _temp6097;}).first_line,({
struct _tagged_ptr3 _temp6098= yyls; struct Cyc_Yyltype* _temp6100= _temp6098.curr
+ yylsp_offset; if( _temp6098.base == 0? 1:( _temp6100 < _temp6098.base? 1:
_temp6100 >= _temp6098.last_plus_one)){ _throw( Null_Exception);}* _temp6100;}).last_line));
_temp6089;}); _temp6088;}); break;} case 273: _LL6087: { struct Cyc_List_List*
tvs=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6127=
yyls; struct Cyc_Yyltype* _temp6129= _temp6127.curr +( yylsp_offset - 3); if(
_temp6127.base == 0? 1:( _temp6129 < _temp6127.base? 1: _temp6129 >= _temp6127.last_plus_one)){
_throw( Null_Exception);}* _temp6129;}).first_line,({ struct _tagged_ptr3
_temp6130= yyls; struct Cyc_Yyltype* _temp6132= _temp6130.curr +( yylsp_offset -
3); if( _temp6130.base == 0? 1:( _temp6132 < _temp6130.base? 1: _temp6132 >=
_temp6130.last_plus_one)){ _throw( Null_Exception);}* _temp6132;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6133= yyvs; void** _temp6135=
_temp6133.curr +( yyvsp_offset - 3); if( _temp6133.base == 0? 1:( _temp6135 <
_temp6133.base? 1: _temp6135 >= _temp6133.last_plus_one)){ _throw(
Null_Exception);}* _temp6135;}))); yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp6111=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6111[ 0]=({ struct Cyc_Pattern_tok_struct _temp6112; _temp6112.tag= Cyc_Pattern_tok;
_temp6112.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp6113=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp6113[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp6114; _temp6114.tag= Cyc_Absyn_UnknownFields_p; _temp6114.f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6115= yyvs; void** _temp6117= _temp6115.curr +(
yyvsp_offset - 4); if( _temp6115.base == 0? 1:( _temp6117 < _temp6115.base? 1:
_temp6117 >= _temp6115.last_plus_one)){ _throw( Null_Exception);}* _temp6117;}));
_temp6114.f2= tvs; _temp6114.f3= Cyc_yyget_FieldPatternList_tok(({ struct
_tagged_ptr2 _temp6118= yyvs; void** _temp6120= _temp6118.curr +( yyvsp_offset -
1); if( _temp6118.base == 0? 1:( _temp6120 < _temp6118.base? 1: _temp6120 >=
_temp6118.last_plus_one)){ _throw( Null_Exception);}* _temp6120;})); _temp6114;});
_temp6113;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6121= yyls;
struct Cyc_Yyltype* _temp6123= _temp6121.curr +( yylsp_offset - 4); if(
_temp6121.base == 0? 1:( _temp6123 < _temp6121.base? 1: _temp6123 >= _temp6121.last_plus_one)){
_throw( Null_Exception);}* _temp6123;}).first_line,({ struct _tagged_ptr3
_temp6124= yyls; struct Cyc_Yyltype* _temp6126= _temp6124.curr + yylsp_offset;
if( _temp6124.base == 0? 1:( _temp6126 < _temp6124.base? 1: _temp6126 >=
_temp6124.last_plus_one)){ _throw( Null_Exception);}* _temp6126;}).last_line));
_temp6112;}); _temp6111;}); break;} case 274: _LL6110: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6137=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6137[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6138; _temp6138.tag= Cyc_Pattern_tok; _temp6138.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp6139=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp6139[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp6140; _temp6140.tag= Cyc_Absyn_Pointer_p;
_temp6140.f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6141= yyvs; void**
_temp6143= _temp6141.curr + yyvsp_offset; if( _temp6141.base == 0? 1:( _temp6143
< _temp6141.base? 1: _temp6143 >= _temp6141.last_plus_one)){ _throw(
Null_Exception);}* _temp6143;})); _temp6140;}); _temp6139;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6144= yyls; struct Cyc_Yyltype* _temp6146= _temp6144.curr
+( yylsp_offset - 1); if( _temp6144.base == 0? 1:( _temp6146 < _temp6144.base? 1:
_temp6146 >= _temp6144.last_plus_one)){ _throw( Null_Exception);}* _temp6146;}).first_line,({
struct _tagged_ptr3 _temp6147= yyls; struct Cyc_Yyltype* _temp6149= _temp6147.curr
+ yylsp_offset; if( _temp6147.base == 0? 1:( _temp6149 < _temp6147.base? 1:
_temp6149 >= _temp6147.last_plus_one)){ _throw( Null_Exception);}* _temp6149;}).last_line));
_temp6138;}); _temp6137;}); break; case 275: _LL6136: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6151=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6151[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6152; _temp6152.tag= Cyc_Pattern_tok; _temp6152.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp6153=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp6153[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp6154; _temp6154.tag= Cyc_Absyn_Reference_p;
_temp6154.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp6155=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6155->f1=( void*) Cyc_Absyn_Loc_n;
_temp6155->f2=({ struct _tagged_string* _temp6156=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp6156[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6157= yyvs; void** _temp6159= _temp6157.curr +
yyvsp_offset; if( _temp6157.base == 0? 1:( _temp6159 < _temp6157.base? 1:
_temp6159 >= _temp6157.last_plus_one)){ _throw( Null_Exception);}* _temp6159;}));
_temp6156;}); _temp6155;}),( void*) Cyc_Absyn_VoidType, 0); _temp6154;});
_temp6153;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6160= yyls;
struct Cyc_Yyltype* _temp6162= _temp6160.curr +( yylsp_offset - 1); if(
_temp6160.base == 0? 1:( _temp6162 < _temp6160.base? 1: _temp6162 >= _temp6160.last_plus_one)){
_throw( Null_Exception);}* _temp6162;}).first_line,({ struct _tagged_ptr3
_temp6163= yyls; struct Cyc_Yyltype* _temp6165= _temp6163.curr + yylsp_offset;
if( _temp6163.base == 0? 1:( _temp6165 < _temp6163.base? 1: _temp6165 >=
_temp6163.last_plus_one)){ _throw( Null_Exception);}* _temp6165;}).last_line));
_temp6152;}); _temp6151;}); break; case 276: _LL6150: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp6167=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6167[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6168; _temp6168.tag= Cyc_PatternList_tok;
_temp6168.f1= 0; _temp6168;}); _temp6167;}); break; case 277: _LL6166: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp6170=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6170[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6171; _temp6171.tag= Cyc_PatternList_tok;
_temp6171.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6172= yyvs; void**
_temp6174= _temp6172.curr + yyvsp_offset; if( _temp6172.base == 0? 1:( _temp6174
< _temp6172.base? 1: _temp6174 >= _temp6172.last_plus_one)){ _throw(
Null_Exception);}* _temp6174;}))); _temp6171;}); _temp6170;}); break; case 278:
_LL6169: yyval=( void*)({ struct Cyc_PatternList_tok_struct* _temp6176=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6176[ 0]=({ struct Cyc_PatternList_tok_struct _temp6177; _temp6177.tag= Cyc_PatternList_tok;
_temp6177.f1=({ struct Cyc_List_List* _temp6178=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6178->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6179= yyvs; void** _temp6181= _temp6179.curr +
yyvsp_offset; if( _temp6179.base == 0? 1:( _temp6181 < _temp6179.base? 1:
_temp6181 >= _temp6179.last_plus_one)){ _throw( Null_Exception);}* _temp6181;}));
_temp6178->tl= 0; _temp6178;}); _temp6177;}); _temp6176;}); break; case 279:
_LL6175: yyval=( void*)({ struct Cyc_PatternList_tok_struct* _temp6183=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6183[ 0]=({ struct Cyc_PatternList_tok_struct _temp6184; _temp6184.tag= Cyc_PatternList_tok;
_temp6184.f1=({ struct Cyc_List_List* _temp6185=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6185->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6189= yyvs; void** _temp6191= _temp6189.curr +
yyvsp_offset; if( _temp6189.base == 0? 1:( _temp6191 < _temp6189.base? 1:
_temp6191 >= _temp6189.last_plus_one)){ _throw( Null_Exception);}* _temp6191;}));
_temp6185->tl= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6186= yyvs;
void** _temp6188= _temp6186.curr +( yyvsp_offset - 2); if( _temp6186.base == 0?
1:( _temp6188 < _temp6186.base? 1: _temp6188 >= _temp6186.last_plus_one)){
_throw( Null_Exception);}* _temp6188;})); _temp6185;}); _temp6184;}); _temp6183;});
break; case 280: _LL6182: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct*
_temp6193=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6193[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6194; _temp6194.tag=
Cyc_FieldPattern_tok; _temp6194.f1=({ struct _tuple17* _temp6195=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6195->f1= 0; _temp6195->f2=
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6196= yyvs; void** _temp6198=
_temp6196.curr + yyvsp_offset; if( _temp6196.base == 0? 1:( _temp6198 <
_temp6196.base? 1: _temp6198 >= _temp6196.last_plus_one)){ _throw(
Null_Exception);}* _temp6198;})); _temp6195;}); _temp6194;}); _temp6193;});
break; case 281: _LL6192: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct*
_temp6200=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6200[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6201; _temp6201.tag=
Cyc_FieldPattern_tok; _temp6201.f1=({ struct _tuple17* _temp6202=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp6202->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp6206= yyvs; void** _temp6208= _temp6206.curr +(
yyvsp_offset - 1); if( _temp6206.base == 0? 1:( _temp6208 < _temp6206.base? 1:
_temp6208 >= _temp6206.last_plus_one)){ _throw( Null_Exception);}* _temp6208;}));
_temp6202->f2= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6203= yyvs;
void** _temp6205= _temp6203.curr + yyvsp_offset; if( _temp6203.base == 0? 1:(
_temp6205 < _temp6203.base? 1: _temp6205 >= _temp6203.last_plus_one)){ _throw(
Null_Exception);}* _temp6205;})); _temp6202;}); _temp6201;}); _temp6200;});
break; case 282: _LL6199: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp6210=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp6210[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp6211; _temp6211.tag= Cyc_FieldPatternList_tok; _temp6211.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6212= yyvs; void** _temp6214= _temp6212.curr +
yyvsp_offset; if( _temp6212.base == 0? 1:( _temp6214 < _temp6212.base? 1:
_temp6214 >= _temp6212.last_plus_one)){ _throw( Null_Exception);}* _temp6214;})));
_temp6211;}); _temp6210;}); break; case 283: _LL6209: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp6216=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp6216[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp6217; _temp6217.tag= Cyc_FieldPatternList_tok;
_temp6217.f1=({ struct Cyc_List_List* _temp6218=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6218->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp6219= yyvs; void** _temp6221= _temp6219.curr +
yyvsp_offset; if( _temp6219.base == 0? 1:( _temp6221 < _temp6219.base? 1:
_temp6221 >= _temp6219.last_plus_one)){ _throw( Null_Exception);}* _temp6221;}));
_temp6218->tl= 0; _temp6218;}); _temp6217;}); _temp6216;}); break; case 284:
_LL6215: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp6223=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6223[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6224; _temp6224.tag=
Cyc_FieldPatternList_tok; _temp6224.f1=({ struct Cyc_List_List* _temp6225=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6225->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6229= yyvs; void**
_temp6231= _temp6229.curr + yyvsp_offset; if( _temp6229.base == 0? 1:( _temp6231
< _temp6229.base? 1: _temp6231 >= _temp6229.last_plus_one)){ _throw(
Null_Exception);}* _temp6231;})); _temp6225->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6226= yyvs; void** _temp6228= _temp6226.curr +(
yyvsp_offset - 2); if( _temp6226.base == 0? 1:( _temp6228 < _temp6226.base? 1:
_temp6228 >= _temp6226.last_plus_one)){ _throw( Null_Exception);}* _temp6228;}));
_temp6225;}); _temp6224;}); _temp6223;}); break; case 285: _LL6222: yyval=({
struct _tagged_ptr2 _temp6233= yyvs; void** _temp6235= _temp6233.curr +
yyvsp_offset; if( _temp6233.base == 0? 1:( _temp6235 < _temp6233.base? 1:
_temp6235 >= _temp6233.last_plus_one)){ _throw( Null_Exception);}* _temp6235;});
break; case 286: _LL6232: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6237=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6237[ 0]=({ struct Cyc_Exp_tok_struct _temp6238; _temp6238.tag= Cyc_Exp_tok;
_temp6238.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6239= yyvs; void** _temp6241= _temp6239.curr +( yyvsp_offset - 2); if(
_temp6239.base == 0? 1:( _temp6241 < _temp6239.base? 1: _temp6241 >= _temp6239.last_plus_one)){
_throw( Null_Exception);}* _temp6241;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6242= yyvs; void** _temp6244= _temp6242.curr + yyvsp_offset;
if( _temp6242.base == 0? 1:( _temp6244 < _temp6242.base? 1: _temp6244 >=
_temp6242.last_plus_one)){ _throw( Null_Exception);}* _temp6244;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6245= yyls; struct Cyc_Yyltype* _temp6247= _temp6245.curr
+( yylsp_offset - 2); if( _temp6245.base == 0? 1:( _temp6247 < _temp6245.base? 1:
_temp6247 >= _temp6245.last_plus_one)){ _throw( Null_Exception);}* _temp6247;}).first_line,({
struct _tagged_ptr3 _temp6248= yyls; struct Cyc_Yyltype* _temp6250= _temp6248.curr
+ yylsp_offset; if( _temp6248.base == 0? 1:( _temp6250 < _temp6248.base? 1:
_temp6250 >= _temp6248.last_plus_one)){ _throw( Null_Exception);}* _temp6250;}).last_line));
_temp6238;}); _temp6237;}); break; case 287: _LL6236: yyval=({ struct
_tagged_ptr2 _temp6252= yyvs; void** _temp6254= _temp6252.curr + yyvsp_offset;
if( _temp6252.base == 0? 1:( _temp6254 < _temp6252.base? 1: _temp6254 >=
_temp6252.last_plus_one)){ _throw( Null_Exception);}* _temp6254;}); break; case
288: _LL6251: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6256=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6256[ 0]=({ struct Cyc_Exp_tok_struct
_temp6257; _temp6257.tag= Cyc_Exp_tok; _temp6257.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6258= yyvs; void** _temp6260= _temp6258.curr +(
yyvsp_offset - 2); if( _temp6258.base == 0? 1:( _temp6260 < _temp6258.base? 1:
_temp6260 >= _temp6258.last_plus_one)){ _throw( Null_Exception);}* _temp6260;})),
Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2 _temp6261= yyvs; void** _temp6263=
_temp6261.curr +( yyvsp_offset - 1); if( _temp6261.base == 0? 1:( _temp6263 <
_temp6261.base? 1: _temp6263 >= _temp6261.last_plus_one)){ _throw(
Null_Exception);}* _temp6263;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6264= yyvs; void** _temp6266= _temp6264.curr + yyvsp_offset; if( _temp6264.base
== 0? 1:( _temp6266 < _temp6264.base? 1: _temp6266 >= _temp6264.last_plus_one)){
_throw( Null_Exception);}* _temp6266;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6267= yyls; struct Cyc_Yyltype* _temp6269= _temp6267.curr +(
yylsp_offset - 2); if( _temp6267.base == 0? 1:( _temp6269 < _temp6267.base? 1:
_temp6269 >= _temp6267.last_plus_one)){ _throw( Null_Exception);}* _temp6269;}).first_line,({
struct _tagged_ptr3 _temp6270= yyls; struct Cyc_Yyltype* _temp6272= _temp6270.curr
+ yylsp_offset; if( _temp6270.base == 0? 1:( _temp6272 < _temp6270.base? 1:
_temp6272 >= _temp6270.last_plus_one)){ _throw( Null_Exception);}* _temp6272;}).last_line));
_temp6257;}); _temp6256;}); break; case 289: _LL6255: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6274=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6274[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6275; _temp6275.tag= Cyc_Primopopt_tok; _temp6275.f1=
0; _temp6275;}); _temp6274;}); break; case 290: _LL6273: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6277=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6277[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6278; _temp6278.tag= Cyc_Primopopt_tok; _temp6278.f1=({
struct Cyc_Core_Opt* _temp6279=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6279->v=( void*)(( void*) Cyc_Absyn_Times); _temp6279;});
_temp6278;}); _temp6277;}); break; case 291: _LL6276: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6281=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6281[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6282; _temp6282.tag= Cyc_Primopopt_tok; _temp6282.f1=({
struct Cyc_Core_Opt* _temp6283=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6283->v=( void*)(( void*) Cyc_Absyn_Div); _temp6283;});
_temp6282;}); _temp6281;}); break; case 292: _LL6280: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6285=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6285[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6286; _temp6286.tag= Cyc_Primopopt_tok; _temp6286.f1=({
struct Cyc_Core_Opt* _temp6287=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6287->v=( void*)(( void*) Cyc_Absyn_Mod); _temp6287;});
_temp6286;}); _temp6285;}); break; case 293: _LL6284: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6289=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6289[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6290; _temp6290.tag= Cyc_Primopopt_tok; _temp6290.f1=({
struct Cyc_Core_Opt* _temp6291=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6291->v=( void*)(( void*) Cyc_Absyn_Plus); _temp6291;});
_temp6290;}); _temp6289;}); break; case 294: _LL6288: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6293=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6293[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6294; _temp6294.tag= Cyc_Primopopt_tok; _temp6294.f1=({
struct Cyc_Core_Opt* _temp6295=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6295->v=( void*)(( void*) Cyc_Absyn_Minus); _temp6295;});
_temp6294;}); _temp6293;}); break; case 295: _LL6292: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6297=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6297[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6298; _temp6298.tag= Cyc_Primopopt_tok; _temp6298.f1=({
struct Cyc_Core_Opt* _temp6299=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6299->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp6299;});
_temp6298;}); _temp6297;}); break; case 296: _LL6296: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6301=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6301[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6302; _temp6302.tag= Cyc_Primopopt_tok; _temp6302.f1=({
struct Cyc_Core_Opt* _temp6303=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6303->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp6303;});
_temp6302;}); _temp6301;}); break; case 297: _LL6300: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6305=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6305[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6306; _temp6306.tag= Cyc_Primopopt_tok; _temp6306.f1=({
struct Cyc_Core_Opt* _temp6307=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6307->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp6307;});
_temp6306;}); _temp6305;}); break; case 298: _LL6304: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6309=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6309[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6310; _temp6310.tag= Cyc_Primopopt_tok; _temp6310.f1=({
struct Cyc_Core_Opt* _temp6311=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6311->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp6311;});
_temp6310;}); _temp6309;}); break; case 299: _LL6308: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6313=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6313[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6314; _temp6314.tag= Cyc_Primopopt_tok; _temp6314.f1=({
struct Cyc_Core_Opt* _temp6315=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6315->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp6315;});
_temp6314;}); _temp6313;}); break; case 300: _LL6312: yyval=({ struct
_tagged_ptr2 _temp6317= yyvs; void** _temp6319= _temp6317.curr + yyvsp_offset;
if( _temp6317.base == 0? 1:( _temp6319 < _temp6317.base? 1: _temp6319 >=
_temp6317.last_plus_one)){ _throw( Null_Exception);}* _temp6319;}); break; case
301: _LL6316: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6321=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6321[ 0]=({ struct Cyc_Exp_tok_struct
_temp6322; _temp6322.tag= Cyc_Exp_tok; _temp6322.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6323= yyvs; void** _temp6325=
_temp6323.curr +( yyvsp_offset - 4); if( _temp6323.base == 0? 1:( _temp6325 <
_temp6323.base? 1: _temp6325 >= _temp6323.last_plus_one)){ _throw(
Null_Exception);}* _temp6325;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6326= yyvs; void** _temp6328= _temp6326.curr +( yyvsp_offset - 2); if(
_temp6326.base == 0? 1:( _temp6328 < _temp6326.base? 1: _temp6328 >= _temp6326.last_plus_one)){
_throw( Null_Exception);}* _temp6328;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6329= yyvs; void** _temp6331= _temp6329.curr + yyvsp_offset;
if( _temp6329.base == 0? 1:( _temp6331 < _temp6329.base? 1: _temp6331 >=
_temp6329.last_plus_one)){ _throw( Null_Exception);}* _temp6331;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6332= yyls; struct Cyc_Yyltype* _temp6334= _temp6332.curr
+( yylsp_offset - 4); if( _temp6332.base == 0? 1:( _temp6334 < _temp6332.base? 1:
_temp6334 >= _temp6332.last_plus_one)){ _throw( Null_Exception);}* _temp6334;}).first_line,({
struct _tagged_ptr3 _temp6335= yyls; struct Cyc_Yyltype* _temp6337= _temp6335.curr
+ yylsp_offset; if( _temp6335.base == 0? 1:( _temp6337 < _temp6335.base? 1:
_temp6337 >= _temp6335.last_plus_one)){ _throw( Null_Exception);}* _temp6337;}).last_line));
_temp6322;}); _temp6321;}); break; case 302: _LL6320: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6339=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6339[ 0]=({ struct Cyc_Exp_tok_struct
_temp6340; _temp6340.tag= Cyc_Exp_tok; _temp6340.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6341= yyvs; void** _temp6343= _temp6341.curr +
yyvsp_offset; if( _temp6341.base == 0? 1:( _temp6343 < _temp6341.base? 1:
_temp6343 >= _temp6341.last_plus_one)){ _throw( Null_Exception);}* _temp6343;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6344= yyls; struct Cyc_Yyltype*
_temp6346= _temp6344.curr +( yylsp_offset - 1); if( _temp6344.base == 0? 1:(
_temp6346 < _temp6344.base? 1: _temp6346 >= _temp6344.last_plus_one)){ _throw(
Null_Exception);}* _temp6346;}).first_line,({ struct _tagged_ptr3 _temp6347=
yyls; struct Cyc_Yyltype* _temp6349= _temp6347.curr + yylsp_offset; if(
_temp6347.base == 0? 1:( _temp6349 < _temp6347.base? 1: _temp6349 >= _temp6347.last_plus_one)){
_throw( Null_Exception);}* _temp6349;}).last_line)); _temp6340;}); _temp6339;});
break; case 303: _LL6338: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6351=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6351[ 0]=({ struct Cyc_Exp_tok_struct _temp6352; _temp6352.tag= Cyc_Exp_tok;
_temp6352.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6353= yyvs; void** _temp6355= _temp6353.curr + yyvsp_offset; if( _temp6353.base
== 0? 1:( _temp6355 < _temp6353.base? 1: _temp6355 >= _temp6353.last_plus_one)){
_throw( Null_Exception);}* _temp6355;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6356= yyls; struct Cyc_Yyltype* _temp6358= _temp6356.curr +(
yylsp_offset - 1); if( _temp6356.base == 0? 1:( _temp6358 < _temp6356.base? 1:
_temp6358 >= _temp6356.last_plus_one)){ _throw( Null_Exception);}* _temp6358;}).first_line,({
struct _tagged_ptr3 _temp6359= yyls; struct Cyc_Yyltype* _temp6361= _temp6359.curr
+( yylsp_offset + 1); if( _temp6359.base == 0? 1:( _temp6361 < _temp6359.base? 1:
_temp6361 >= _temp6359.last_plus_one)){ _throw( Null_Exception);}* _temp6361;}).last_line));
_temp6352;}); _temp6351;}); break; case 304: _LL6350: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6363=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6363[ 0]=({ struct Cyc_Exp_tok_struct
_temp6364; _temp6364.tag= Cyc_Exp_tok; _temp6364.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6365= yyvs; void** _temp6367= _temp6365.curr +
yyvsp_offset; if( _temp6365.base == 0? 1:( _temp6367 < _temp6365.base? 1:
_temp6367 >= _temp6365.last_plus_one)){ _throw( Null_Exception);}* _temp6367;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6368= yyls; struct Cyc_Yyltype*
_temp6370= _temp6368.curr +( yylsp_offset - 1); if( _temp6368.base == 0? 1:(
_temp6370 < _temp6368.base? 1: _temp6370 >= _temp6368.last_plus_one)){ _throw(
Null_Exception);}* _temp6370;}).first_line,({ struct _tagged_ptr3 _temp6371=
yyls; struct Cyc_Yyltype* _temp6373= _temp6371.curr +( yylsp_offset + 1); if(
_temp6371.base == 0? 1:( _temp6373 < _temp6371.base? 1: _temp6373 >= _temp6371.last_plus_one)){
_throw( Null_Exception);}* _temp6373;}).last_line)); _temp6364;}); _temp6363;});
break; case 305: _LL6362: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6375=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6375[ 0]=({ struct Cyc_Exp_tok_struct _temp6376; _temp6376.tag= Cyc_Exp_tok;
_temp6376.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6377= yyvs; void** _temp6379= _temp6377.curr +(
yyvsp_offset - 2); if( _temp6377.base == 0? 1:( _temp6379 < _temp6377.base? 1:
_temp6379 >= _temp6377.last_plus_one)){ _throw( Null_Exception);}* _temp6379;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6380= yyvs; void** _temp6382=
_temp6380.curr + yyvsp_offset; if( _temp6380.base == 0? 1:( _temp6382 <
_temp6380.base? 1: _temp6382 >= _temp6380.last_plus_one)){ _throw(
Null_Exception);}* _temp6382;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6383= yyls; struct Cyc_Yyltype* _temp6385= _temp6383.curr +(
yylsp_offset - 4); if( _temp6383.base == 0? 1:( _temp6385 < _temp6383.base? 1:
_temp6385 >= _temp6383.last_plus_one)){ _throw( Null_Exception);}* _temp6385;}).first_line,({
struct _tagged_ptr3 _temp6386= yyls; struct Cyc_Yyltype* _temp6388= _temp6386.curr
+ yylsp_offset; if( _temp6386.base == 0? 1:( _temp6388 < _temp6386.base? 1:
_temp6388 >= _temp6386.last_plus_one)){ _throw( Null_Exception);}* _temp6388;}).last_line));
_temp6376;}); _temp6375;}); break; case 306: _LL6374: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6390=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6390[ 0]=({ struct Cyc_Exp_tok_struct
_temp6391; _temp6391.tag= Cyc_Exp_tok; _temp6391.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6392= yyvs; void**
_temp6394= _temp6392.curr +( yyvsp_offset - 2); if( _temp6392.base == 0? 1:(
_temp6394 < _temp6392.base? 1: _temp6394 >= _temp6392.last_plus_one)){ _throw(
Null_Exception);}* _temp6394;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6395= yyvs; void** _temp6397= _temp6395.curr + yyvsp_offset; if( _temp6395.base
== 0? 1:( _temp6397 < _temp6395.base? 1: _temp6397 >= _temp6395.last_plus_one)){
_throw( Null_Exception);}* _temp6397;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6398= yyls; struct Cyc_Yyltype* _temp6400= _temp6398.curr +(
yylsp_offset - 4); if( _temp6398.base == 0? 1:( _temp6400 < _temp6398.base? 1:
_temp6400 >= _temp6398.last_plus_one)){ _throw( Null_Exception);}* _temp6400;}).first_line,({
struct _tagged_ptr3 _temp6401= yyls; struct Cyc_Yyltype* _temp6403= _temp6401.curr
+ yylsp_offset; if( _temp6401.base == 0? 1:( _temp6403 < _temp6401.base? 1:
_temp6403 >= _temp6401.last_plus_one)){ _throw( Null_Exception);}* _temp6403;}).last_line));
_temp6391;}); _temp6390;}); break; case 307: _LL6389: yyval=({ struct
_tagged_ptr2 _temp6405= yyvs; void** _temp6407= _temp6405.curr + yyvsp_offset;
if( _temp6405.base == 0? 1:( _temp6407 < _temp6405.base? 1: _temp6407 >=
_temp6405.last_plus_one)){ _throw( Null_Exception);}* _temp6407;}); break; case
308: _LL6404: yyval=({ struct _tagged_ptr2 _temp6409= yyvs; void** _temp6411=
_temp6409.curr + yyvsp_offset; if( _temp6409.base == 0? 1:( _temp6411 <
_temp6409.base? 1: _temp6411 >= _temp6409.last_plus_one)){ _throw(
Null_Exception);}* _temp6411;}); break; case 309: _LL6408: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp6413=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6413[ 0]=({ struct Cyc_Exp_tok_struct
_temp6414; _temp6414.tag= Cyc_Exp_tok; _temp6414.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6415= yyvs; void** _temp6417= _temp6415.curr +(
yyvsp_offset - 2); if( _temp6415.base == 0? 1:( _temp6417 < _temp6415.base? 1:
_temp6417 >= _temp6415.last_plus_one)){ _throw( Null_Exception);}* _temp6417;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6418= yyvs; void** _temp6420=
_temp6418.curr + yyvsp_offset; if( _temp6418.base == 0? 1:( _temp6420 <
_temp6418.base? 1: _temp6420 >= _temp6418.last_plus_one)){ _throw(
Null_Exception);}* _temp6420;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6421= yyls; struct Cyc_Yyltype* _temp6423= _temp6421.curr +(
yylsp_offset - 2); if( _temp6421.base == 0? 1:( _temp6423 < _temp6421.base? 1:
_temp6423 >= _temp6421.last_plus_one)){ _throw( Null_Exception);}* _temp6423;}).first_line,({
struct _tagged_ptr3 _temp6424= yyls; struct Cyc_Yyltype* _temp6426= _temp6424.curr
+ yylsp_offset; if( _temp6424.base == 0? 1:( _temp6426 < _temp6424.base? 1:
_temp6426 >= _temp6424.last_plus_one)){ _throw( Null_Exception);}* _temp6426;}).last_line));
_temp6414;}); _temp6413;}); break; case 310: _LL6412: yyval=({ struct
_tagged_ptr2 _temp6428= yyvs; void** _temp6430= _temp6428.curr + yyvsp_offset;
if( _temp6428.base == 0? 1:( _temp6430 < _temp6428.base? 1: _temp6430 >=
_temp6428.last_plus_one)){ _throw( Null_Exception);}* _temp6430;}); break; case
311: _LL6427: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6432=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6432[ 0]=({ struct Cyc_Exp_tok_struct
_temp6433; _temp6433.tag= Cyc_Exp_tok; _temp6433.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(({
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
_temp6433;}); _temp6432;}); break; case 312: _LL6431: yyval=({ struct
_tagged_ptr2 _temp6447= yyvs; void** _temp6449= _temp6447.curr + yyvsp_offset;
if( _temp6447.base == 0? 1:( _temp6449 < _temp6447.base? 1: _temp6449 >=
_temp6447.last_plus_one)){ _throw( Null_Exception);}* _temp6449;}); break; case
313: _LL6446: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6451=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6451[ 0]=({ struct Cyc_Exp_tok_struct
_temp6452; _temp6452.tag= Cyc_Exp_tok; _temp6452.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6453= yyvs; void**
_temp6455= _temp6453.curr +( yyvsp_offset - 2); if( _temp6453.base == 0? 1:(
_temp6455 < _temp6453.base? 1: _temp6455 >= _temp6453.last_plus_one)){ _throw(
Null_Exception);}* _temp6455;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6456= yyvs; void** _temp6458= _temp6456.curr + yyvsp_offset; if( _temp6456.base
== 0? 1:( _temp6458 < _temp6456.base? 1: _temp6458 >= _temp6456.last_plus_one)){
_throw( Null_Exception);}* _temp6458;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6459= yyls; struct Cyc_Yyltype* _temp6461= _temp6459.curr +(
yylsp_offset - 2); if( _temp6459.base == 0? 1:( _temp6461 < _temp6459.base? 1:
_temp6461 >= _temp6459.last_plus_one)){ _throw( Null_Exception);}* _temp6461;}).first_line,({
struct _tagged_ptr3 _temp6462= yyls; struct Cyc_Yyltype* _temp6464= _temp6462.curr
+ yylsp_offset; if( _temp6462.base == 0? 1:( _temp6464 < _temp6462.base? 1:
_temp6464 >= _temp6462.last_plus_one)){ _throw( Null_Exception);}* _temp6464;}).last_line));
_temp6452;}); _temp6451;}); break; case 314: _LL6450: yyval=({ struct
_tagged_ptr2 _temp6466= yyvs; void** _temp6468= _temp6466.curr + yyvsp_offset;
if( _temp6466.base == 0? 1:( _temp6468 < _temp6466.base? 1: _temp6468 >=
_temp6466.last_plus_one)){ _throw( Null_Exception);}* _temp6468;}); break; case
315: _LL6465: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6470=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6470[ 0]=({ struct Cyc_Exp_tok_struct
_temp6471; _temp6471.tag= Cyc_Exp_tok; _temp6471.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6472= yyvs; void**
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
_temp6471;}); _temp6470;}); break; case 316: _LL6469: yyval=({ struct
_tagged_ptr2 _temp6485= yyvs; void** _temp6487= _temp6485.curr + yyvsp_offset;
if( _temp6485.base == 0? 1:( _temp6487 < _temp6485.base? 1: _temp6487 >=
_temp6485.last_plus_one)){ _throw( Null_Exception);}* _temp6487;}); break; case
317: _LL6484: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6489=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6489[ 0]=({ struct Cyc_Exp_tok_struct
_temp6490; _temp6490.tag= Cyc_Exp_tok; _temp6490.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6491= yyvs; void**
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
_temp6490;}); _temp6489;}); break; case 318: _LL6488: yyval=({ struct
_tagged_ptr2 _temp6504= yyvs; void** _temp6506= _temp6504.curr + yyvsp_offset;
if( _temp6504.base == 0? 1:( _temp6506 < _temp6504.base? 1: _temp6506 >=
_temp6504.last_plus_one)){ _throw( Null_Exception);}* _temp6506;}); break; case
319: _LL6503: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6508=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6508[ 0]=({ struct Cyc_Exp_tok_struct
_temp6509; _temp6509.tag= Cyc_Exp_tok; _temp6509.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6510= yyvs; void** _temp6512= _temp6510.curr +(
yyvsp_offset - 2); if( _temp6510.base == 0? 1:( _temp6512 < _temp6510.base? 1:
_temp6512 >= _temp6510.last_plus_one)){ _throw( Null_Exception);}* _temp6512;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6513= yyvs; void** _temp6515=
_temp6513.curr + yyvsp_offset; if( _temp6513.base == 0? 1:( _temp6515 <
_temp6513.base? 1: _temp6515 >= _temp6513.last_plus_one)){ _throw(
Null_Exception);}* _temp6515;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6516= yyls; struct Cyc_Yyltype* _temp6518= _temp6516.curr +(
yylsp_offset - 2); if( _temp6516.base == 0? 1:( _temp6518 < _temp6516.base? 1:
_temp6518 >= _temp6516.last_plus_one)){ _throw( Null_Exception);}* _temp6518;}).first_line,({
struct _tagged_ptr3 _temp6519= yyls; struct Cyc_Yyltype* _temp6521= _temp6519.curr
+ yylsp_offset; if( _temp6519.base == 0? 1:( _temp6521 < _temp6519.base? 1:
_temp6521 >= _temp6519.last_plus_one)){ _throw( Null_Exception);}* _temp6521;}).last_line));
_temp6509;}); _temp6508;}); break; case 320: _LL6507: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6523=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6523[ 0]=({ struct Cyc_Exp_tok_struct
_temp6524; _temp6524.tag= Cyc_Exp_tok; _temp6524.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6525= yyvs; void** _temp6527= _temp6525.curr +(
yyvsp_offset - 2); if( _temp6525.base == 0? 1:( _temp6527 < _temp6525.base? 1:
_temp6527 >= _temp6525.last_plus_one)){ _throw( Null_Exception);}* _temp6527;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6528= yyvs; void** _temp6530=
_temp6528.curr + yyvsp_offset; if( _temp6528.base == 0? 1:( _temp6530 <
_temp6528.base? 1: _temp6530 >= _temp6528.last_plus_one)){ _throw(
Null_Exception);}* _temp6530;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6531= yyls; struct Cyc_Yyltype* _temp6533= _temp6531.curr +(
yylsp_offset - 2); if( _temp6531.base == 0? 1:( _temp6533 < _temp6531.base? 1:
_temp6533 >= _temp6531.last_plus_one)){ _throw( Null_Exception);}* _temp6533;}).first_line,({
struct _tagged_ptr3 _temp6534= yyls; struct Cyc_Yyltype* _temp6536= _temp6534.curr
+ yylsp_offset; if( _temp6534.base == 0? 1:( _temp6536 < _temp6534.base? 1:
_temp6536 >= _temp6534.last_plus_one)){ _throw( Null_Exception);}* _temp6536;}).last_line));
_temp6524;}); _temp6523;}); break; case 321: _LL6522: yyval=({ struct
_tagged_ptr2 _temp6538= yyvs; void** _temp6540= _temp6538.curr + yyvsp_offset;
if( _temp6538.base == 0? 1:( _temp6540 < _temp6538.base? 1: _temp6540 >=
_temp6538.last_plus_one)){ _throw( Null_Exception);}* _temp6540;}); break; case
322: _LL6537: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6542=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6542[ 0]=({ struct Cyc_Exp_tok_struct
_temp6543; _temp6543.tag= Cyc_Exp_tok; _temp6543.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(({
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
_temp6543;}); _temp6542;}); break; case 323: _LL6541: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6557=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6557[ 0]=({ struct Cyc_Exp_tok_struct
_temp6558; _temp6558.tag= Cyc_Exp_tok; _temp6558.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6559= yyvs; void** _temp6561= _temp6559.curr +(
yyvsp_offset - 2); if( _temp6559.base == 0? 1:( _temp6561 < _temp6559.base? 1:
_temp6561 >= _temp6559.last_plus_one)){ _throw( Null_Exception);}* _temp6561;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6562= yyvs; void** _temp6564=
_temp6562.curr + yyvsp_offset; if( _temp6562.base == 0? 1:( _temp6564 <
_temp6562.base? 1: _temp6564 >= _temp6562.last_plus_one)){ _throw(
Null_Exception);}* _temp6564;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6565= yyls; struct Cyc_Yyltype* _temp6567= _temp6565.curr +(
yylsp_offset - 2); if( _temp6565.base == 0? 1:( _temp6567 < _temp6565.base? 1:
_temp6567 >= _temp6565.last_plus_one)){ _throw( Null_Exception);}* _temp6567;}).first_line,({
struct _tagged_ptr3 _temp6568= yyls; struct Cyc_Yyltype* _temp6570= _temp6568.curr
+ yylsp_offset; if( _temp6568.base == 0? 1:( _temp6570 < _temp6568.base? 1:
_temp6570 >= _temp6568.last_plus_one)){ _throw( Null_Exception);}* _temp6570;}).last_line));
_temp6558;}); _temp6557;}); break; case 324: _LL6556: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6572=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6572[ 0]=({ struct Cyc_Exp_tok_struct
_temp6573; _temp6573.tag= Cyc_Exp_tok; _temp6573.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6574= yyvs; void** _temp6576= _temp6574.curr +(
yyvsp_offset - 2); if( _temp6574.base == 0? 1:( _temp6576 < _temp6574.base? 1:
_temp6576 >= _temp6574.last_plus_one)){ _throw( Null_Exception);}* _temp6576;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6577= yyvs; void** _temp6579=
_temp6577.curr + yyvsp_offset; if( _temp6577.base == 0? 1:( _temp6579 <
_temp6577.base? 1: _temp6579 >= _temp6577.last_plus_one)){ _throw(
Null_Exception);}* _temp6579;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6580= yyls; struct Cyc_Yyltype* _temp6582= _temp6580.curr +(
yylsp_offset - 2); if( _temp6580.base == 0? 1:( _temp6582 < _temp6580.base? 1:
_temp6582 >= _temp6580.last_plus_one)){ _throw( Null_Exception);}* _temp6582;}).first_line,({
struct _tagged_ptr3 _temp6583= yyls; struct Cyc_Yyltype* _temp6585= _temp6583.curr
+ yylsp_offset; if( _temp6583.base == 0? 1:( _temp6585 < _temp6583.base? 1:
_temp6585 >= _temp6583.last_plus_one)){ _throw( Null_Exception);}* _temp6585;}).last_line));
_temp6573;}); _temp6572;}); break; case 325: _LL6571: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6587=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6587[ 0]=({ struct Cyc_Exp_tok_struct
_temp6588; _temp6588.tag= Cyc_Exp_tok; _temp6588.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6589= yyvs; void** _temp6591= _temp6589.curr +(
yyvsp_offset - 2); if( _temp6589.base == 0? 1:( _temp6591 < _temp6589.base? 1:
_temp6591 >= _temp6589.last_plus_one)){ _throw( Null_Exception);}* _temp6591;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6592= yyvs; void** _temp6594=
_temp6592.curr + yyvsp_offset; if( _temp6592.base == 0? 1:( _temp6594 <
_temp6592.base? 1: _temp6594 >= _temp6592.last_plus_one)){ _throw(
Null_Exception);}* _temp6594;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6595= yyls; struct Cyc_Yyltype* _temp6597= _temp6595.curr +(
yylsp_offset - 2); if( _temp6595.base == 0? 1:( _temp6597 < _temp6595.base? 1:
_temp6597 >= _temp6595.last_plus_one)){ _throw( Null_Exception);}* _temp6597;}).first_line,({
struct _tagged_ptr3 _temp6598= yyls; struct Cyc_Yyltype* _temp6600= _temp6598.curr
+ yylsp_offset; if( _temp6598.base == 0? 1:( _temp6600 < _temp6598.base? 1:
_temp6600 >= _temp6598.last_plus_one)){ _throw( Null_Exception);}* _temp6600;}).last_line));
_temp6588;}); _temp6587;}); break; case 326: _LL6586: yyval=({ struct
_tagged_ptr2 _temp6602= yyvs; void** _temp6604= _temp6602.curr + yyvsp_offset;
if( _temp6602.base == 0? 1:( _temp6604 < _temp6602.base? 1: _temp6604 >=
_temp6602.last_plus_one)){ _throw( Null_Exception);}* _temp6604;}); break; case
327: _LL6601: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6606=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6606[ 0]=({ struct Cyc_Exp_tok_struct
_temp6607; _temp6607.tag= Cyc_Exp_tok; _temp6607.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6608= yyvs;
void** _temp6610= _temp6608.curr +( yyvsp_offset - 2); if( _temp6608.base == 0?
1:( _temp6610 < _temp6608.base? 1: _temp6610 >= _temp6608.last_plus_one)){
_throw( Null_Exception);}* _temp6610;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6611= yyvs; void** _temp6613= _temp6611.curr + yyvsp_offset;
if( _temp6611.base == 0? 1:( _temp6613 < _temp6611.base? 1: _temp6613 >=
_temp6611.last_plus_one)){ _throw( Null_Exception);}* _temp6613;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6614= yyls; struct Cyc_Yyltype* _temp6616= _temp6614.curr
+( yylsp_offset - 2); if( _temp6614.base == 0? 1:( _temp6616 < _temp6614.base? 1:
_temp6616 >= _temp6614.last_plus_one)){ _throw( Null_Exception);}* _temp6616;}).first_line,({
struct _tagged_ptr3 _temp6617= yyls; struct Cyc_Yyltype* _temp6619= _temp6617.curr
+ yylsp_offset; if( _temp6617.base == 0? 1:( _temp6619 < _temp6617.base? 1:
_temp6619 >= _temp6617.last_plus_one)){ _throw( Null_Exception);}* _temp6619;}).last_line));
_temp6607;}); _temp6606;}); break; case 328: _LL6605: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6621=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6621[ 0]=({ struct Cyc_Exp_tok_struct
_temp6622; _temp6622.tag= Cyc_Exp_tok; _temp6622.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6623= yyvs;
void** _temp6625= _temp6623.curr +( yyvsp_offset - 2); if( _temp6623.base == 0?
1:( _temp6625 < _temp6623.base? 1: _temp6625 >= _temp6623.last_plus_one)){
_throw( Null_Exception);}* _temp6625;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6626= yyvs; void** _temp6628= _temp6626.curr + yyvsp_offset;
if( _temp6626.base == 0? 1:( _temp6628 < _temp6626.base? 1: _temp6628 >=
_temp6626.last_plus_one)){ _throw( Null_Exception);}* _temp6628;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6629= yyls; struct Cyc_Yyltype* _temp6631= _temp6629.curr
+( yylsp_offset - 2); if( _temp6629.base == 0? 1:( _temp6631 < _temp6629.base? 1:
_temp6631 >= _temp6629.last_plus_one)){ _throw( Null_Exception);}* _temp6631;}).first_line,({
struct _tagged_ptr3 _temp6632= yyls; struct Cyc_Yyltype* _temp6634= _temp6632.curr
+ yylsp_offset; if( _temp6632.base == 0? 1:( _temp6634 < _temp6632.base? 1:
_temp6634 >= _temp6632.last_plus_one)){ _throw( Null_Exception);}* _temp6634;}).last_line));
_temp6622;}); _temp6621;}); break; case 329: _LL6620: yyval=({ struct
_tagged_ptr2 _temp6636= yyvs; void** _temp6638= _temp6636.curr + yyvsp_offset;
if( _temp6636.base == 0? 1:( _temp6638 < _temp6636.base? 1: _temp6638 >=
_temp6636.last_plus_one)){ _throw( Null_Exception);}* _temp6638;}); break; case
330: _LL6635: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6640=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6640[ 0]=({ struct Cyc_Exp_tok_struct
_temp6641; _temp6641.tag= Cyc_Exp_tok; _temp6641.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6642= yyvs; void**
_temp6644= _temp6642.curr +( yyvsp_offset - 2); if( _temp6642.base == 0? 1:(
_temp6644 < _temp6642.base? 1: _temp6644 >= _temp6642.last_plus_one)){ _throw(
Null_Exception);}* _temp6644;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6645= yyvs; void** _temp6647= _temp6645.curr + yyvsp_offset; if( _temp6645.base
== 0? 1:( _temp6647 < _temp6645.base? 1: _temp6647 >= _temp6645.last_plus_one)){
_throw( Null_Exception);}* _temp6647;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6648= yyls; struct Cyc_Yyltype* _temp6650= _temp6648.curr +(
yylsp_offset - 2); if( _temp6648.base == 0? 1:( _temp6650 < _temp6648.base? 1:
_temp6650 >= _temp6648.last_plus_one)){ _throw( Null_Exception);}* _temp6650;}).first_line,({
struct _tagged_ptr3 _temp6651= yyls; struct Cyc_Yyltype* _temp6653= _temp6651.curr
+ yylsp_offset; if( _temp6651.base == 0? 1:( _temp6653 < _temp6651.base? 1:
_temp6653 >= _temp6651.last_plus_one)){ _throw( Null_Exception);}* _temp6653;}).last_line));
_temp6641;}); _temp6640;}); break; case 331: _LL6639: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6655=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6655[ 0]=({ struct Cyc_Exp_tok_struct
_temp6656; _temp6656.tag= Cyc_Exp_tok; _temp6656.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6657= yyvs; void**
_temp6659= _temp6657.curr +( yyvsp_offset - 2); if( _temp6657.base == 0? 1:(
_temp6659 < _temp6657.base? 1: _temp6659 >= _temp6657.last_plus_one)){ _throw(
Null_Exception);}* _temp6659;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6660= yyvs; void** _temp6662= _temp6660.curr + yyvsp_offset; if( _temp6660.base
== 0? 1:( _temp6662 < _temp6660.base? 1: _temp6662 >= _temp6660.last_plus_one)){
_throw( Null_Exception);}* _temp6662;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6663= yyls; struct Cyc_Yyltype* _temp6665= _temp6663.curr +(
yylsp_offset - 2); if( _temp6663.base == 0? 1:( _temp6665 < _temp6663.base? 1:
_temp6665 >= _temp6663.last_plus_one)){ _throw( Null_Exception);}* _temp6665;}).first_line,({
struct _tagged_ptr3 _temp6666= yyls; struct Cyc_Yyltype* _temp6668= _temp6666.curr
+ yylsp_offset; if( _temp6666.base == 0? 1:( _temp6668 < _temp6666.base? 1:
_temp6668 >= _temp6666.last_plus_one)){ _throw( Null_Exception);}* _temp6668;}).last_line));
_temp6656;}); _temp6655;}); break; case 332: _LL6654: yyval=({ struct
_tagged_ptr2 _temp6670= yyvs; void** _temp6672= _temp6670.curr + yyvsp_offset;
if( _temp6670.base == 0? 1:( _temp6672 < _temp6670.base? 1: _temp6672 >=
_temp6670.last_plus_one)){ _throw( Null_Exception);}* _temp6672;}); break; case
333: _LL6669: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6674=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6674[ 0]=({ struct Cyc_Exp_tok_struct
_temp6675; _temp6675.tag= Cyc_Exp_tok; _temp6675.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6676= yyvs; void**
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
_temp6675;}); _temp6674;}); break; case 334: _LL6673: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6689=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6689[ 0]=({ struct Cyc_Exp_tok_struct
_temp6690; _temp6690.tag= Cyc_Exp_tok; _temp6690.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6691= yyvs; void**
_temp6693= _temp6691.curr +( yyvsp_offset - 2); if( _temp6691.base == 0? 1:(
_temp6693 < _temp6691.base? 1: _temp6693 >= _temp6691.last_plus_one)){ _throw(
Null_Exception);}* _temp6693;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6694= yyvs; void** _temp6696= _temp6694.curr + yyvsp_offset; if( _temp6694.base
== 0? 1:( _temp6696 < _temp6694.base? 1: _temp6696 >= _temp6694.last_plus_one)){
_throw( Null_Exception);}* _temp6696;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6697= yyls; struct Cyc_Yyltype* _temp6699= _temp6697.curr +(
yylsp_offset - 2); if( _temp6697.base == 0? 1:( _temp6699 < _temp6697.base? 1:
_temp6699 >= _temp6697.last_plus_one)){ _throw( Null_Exception);}* _temp6699;}).first_line,({
struct _tagged_ptr3 _temp6700= yyls; struct Cyc_Yyltype* _temp6702= _temp6700.curr
+ yylsp_offset; if( _temp6700.base == 0? 1:( _temp6702 < _temp6700.base? 1:
_temp6702 >= _temp6700.last_plus_one)){ _throw( Null_Exception);}* _temp6702;}).last_line));
_temp6690;}); _temp6689;}); break; case 335: _LL6688: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6704=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6704[ 0]=({ struct Cyc_Exp_tok_struct
_temp6705; _temp6705.tag= Cyc_Exp_tok; _temp6705.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6706= yyvs; void**
_temp6708= _temp6706.curr +( yyvsp_offset - 2); if( _temp6706.base == 0? 1:(
_temp6708 < _temp6706.base? 1: _temp6708 >= _temp6706.last_plus_one)){ _throw(
Null_Exception);}* _temp6708;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6709= yyvs; void** _temp6711= _temp6709.curr + yyvsp_offset; if( _temp6709.base
== 0? 1:( _temp6711 < _temp6709.base? 1: _temp6711 >= _temp6709.last_plus_one)){
_throw( Null_Exception);}* _temp6711;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6712= yyls; struct Cyc_Yyltype* _temp6714= _temp6712.curr +(
yylsp_offset - 2); if( _temp6712.base == 0? 1:( _temp6714 < _temp6712.base? 1:
_temp6714 >= _temp6712.last_plus_one)){ _throw( Null_Exception);}* _temp6714;}).first_line,({
struct _tagged_ptr3 _temp6715= yyls; struct Cyc_Yyltype* _temp6717= _temp6715.curr
+ yylsp_offset; if( _temp6715.base == 0? 1:( _temp6717 < _temp6715.base? 1:
_temp6717 >= _temp6715.last_plus_one)){ _throw( Null_Exception);}* _temp6717;}).last_line));
_temp6705;}); _temp6704;}); break; case 336: _LL6703: yyval=({ struct
_tagged_ptr2 _temp6719= yyvs; void** _temp6721= _temp6719.curr + yyvsp_offset;
if( _temp6719.base == 0? 1:( _temp6721 < _temp6719.base? 1: _temp6721 >=
_temp6719.last_plus_one)){ _throw( Null_Exception);}* _temp6721;}); break; case
337: _LL6718: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6723=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6723[ 0]=({ struct Cyc_Exp_tok_struct
_temp6724; _temp6724.tag= Cyc_Exp_tok; _temp6724.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp6725= yyvs; void** _temp6727= _temp6725.curr +(
yyvsp_offset - 2); if( _temp6725.base == 0? 1:( _temp6727 < _temp6725.base? 1:
_temp6727 >= _temp6725.last_plus_one)){ _throw( Null_Exception);}* _temp6727;}))).f3,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6728= yyvs; void** _temp6730=
_temp6728.curr + yyvsp_offset; if( _temp6728.base == 0? 1:( _temp6730 <
_temp6728.base? 1: _temp6730 >= _temp6728.last_plus_one)){ _throw(
Null_Exception);}* _temp6730;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6731= yyls; struct Cyc_Yyltype* _temp6733= _temp6731.curr +(
yylsp_offset - 3); if( _temp6731.base == 0? 1:( _temp6733 < _temp6731.base? 1:
_temp6733 >= _temp6731.last_plus_one)){ _throw( Null_Exception);}* _temp6733;}).first_line,({
struct _tagged_ptr3 _temp6734= yyls; struct Cyc_Yyltype* _temp6736= _temp6734.curr
+ yylsp_offset; if( _temp6734.base == 0? 1:( _temp6736 < _temp6734.base? 1:
_temp6736 >= _temp6734.last_plus_one)){ _throw( Null_Exception);}* _temp6736;}).last_line));
_temp6724;}); _temp6723;}); break; case 338: _LL6722: yyval=({ struct
_tagged_ptr2 _temp6738= yyvs; void** _temp6740= _temp6738.curr + yyvsp_offset;
if( _temp6738.base == 0? 1:( _temp6740 < _temp6738.base? 1: _temp6740 >=
_temp6738.last_plus_one)){ _throw( Null_Exception);}* _temp6740;}); break; case
339: _LL6737: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6742=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6742[ 0]=({ struct Cyc_Exp_tok_struct
_temp6743; _temp6743.tag= Cyc_Exp_tok; _temp6743.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6744= yyvs; void** _temp6746= _temp6744.curr +
yyvsp_offset; if( _temp6744.base == 0? 1:( _temp6746 < _temp6744.base? 1:
_temp6746 >= _temp6744.last_plus_one)){ _throw( Null_Exception);}* _temp6746;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6747= yyls; struct Cyc_Yyltype*
_temp6749= _temp6747.curr +( yylsp_offset - 1); if( _temp6747.base == 0? 1:(
_temp6749 < _temp6747.base? 1: _temp6749 >= _temp6747.last_plus_one)){ _throw(
Null_Exception);}* _temp6749;}).first_line,({ struct _tagged_ptr3 _temp6750=
yyls; struct Cyc_Yyltype* _temp6752= _temp6750.curr + yylsp_offset; if(
_temp6750.base == 0? 1:( _temp6752 < _temp6750.base? 1: _temp6752 >= _temp6750.last_plus_one)){
_throw( Null_Exception);}* _temp6752;}).last_line)); _temp6743;}); _temp6742;});
break; case 340: _LL6741: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6754=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6754[ 0]=({ struct Cyc_Exp_tok_struct _temp6755; _temp6755.tag= Cyc_Exp_tok;
_temp6755.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6756= yyvs; void** _temp6758= _temp6756.curr + yyvsp_offset; if( _temp6756.base
== 0? 1:( _temp6758 < _temp6756.base? 1: _temp6758 >= _temp6756.last_plus_one)){
_throw( Null_Exception);}* _temp6758;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6759= yyls; struct Cyc_Yyltype* _temp6761= _temp6759.curr +(
yylsp_offset - 1); if( _temp6759.base == 0? 1:( _temp6761 < _temp6759.base? 1:
_temp6761 >= _temp6759.last_plus_one)){ _throw( Null_Exception);}* _temp6761;}).first_line,({
struct _tagged_ptr3 _temp6762= yyls; struct Cyc_Yyltype* _temp6764= _temp6762.curr
+ yylsp_offset; if( _temp6762.base == 0? 1:( _temp6764 < _temp6762.base? 1:
_temp6764 >= _temp6762.last_plus_one)){ _throw( Null_Exception);}* _temp6764;}).last_line));
_temp6755;}); _temp6754;}); break; case 341: _LL6753: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6766=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6766[ 0]=({ struct Cyc_Exp_tok_struct
_temp6767; _temp6767.tag= Cyc_Exp_tok; _temp6767.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6768= yyvs; void** _temp6770= _temp6768.curr +
yyvsp_offset; if( _temp6768.base == 0? 1:( _temp6770 < _temp6768.base? 1:
_temp6770 >= _temp6768.last_plus_one)){ _throw( Null_Exception);}* _temp6770;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6771= yyls; struct Cyc_Yyltype*
_temp6773= _temp6771.curr +( yylsp_offset - 1); if( _temp6771.base == 0? 1:(
_temp6773 < _temp6771.base? 1: _temp6773 >= _temp6771.last_plus_one)){ _throw(
Null_Exception);}* _temp6773;}).first_line,({ struct _tagged_ptr3 _temp6774=
yyls; struct Cyc_Yyltype* _temp6776= _temp6774.curr + yylsp_offset; if(
_temp6774.base == 0? 1:( _temp6776 < _temp6774.base? 1: _temp6776 >= _temp6774.last_plus_one)){
_throw( Null_Exception);}* _temp6776;}).last_line)); _temp6767;}); _temp6766;});
break; case 342: _LL6765: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6778=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6778[ 0]=({ struct Cyc_Exp_tok_struct _temp6779; _temp6779.tag= Cyc_Exp_tok;
_temp6779.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6780= yyvs; void** _temp6782= _temp6780.curr + yyvsp_offset; if( _temp6780.base
== 0? 1:( _temp6782 < _temp6780.base? 1: _temp6782 >= _temp6780.last_plus_one)){
_throw( Null_Exception);}* _temp6782;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6783= yyls; struct Cyc_Yyltype* _temp6785= _temp6783.curr +(
yylsp_offset - 1); if( _temp6783.base == 0? 1:( _temp6785 < _temp6783.base? 1:
_temp6785 >= _temp6783.last_plus_one)){ _throw( Null_Exception);}* _temp6785;}).first_line,({
struct _tagged_ptr3 _temp6786= yyls; struct Cyc_Yyltype* _temp6788= _temp6786.curr
+ yylsp_offset; if( _temp6786.base == 0? 1:( _temp6788 < _temp6786.base? 1:
_temp6788 >= _temp6786.last_plus_one)){ _throw( Null_Exception);}* _temp6788;}).last_line));
_temp6779;}); _temp6778;}); break; case 343: _LL6777: yyval=({ struct
_tagged_ptr2 _temp6790= yyvs; void** _temp6792= _temp6790.curr + yyvsp_offset;
if( _temp6790.base == 0? 1:( _temp6792 < _temp6790.base? 1: _temp6792 >=
_temp6790.last_plus_one)){ _throw( Null_Exception);}* _temp6792;}); break; case
344: _LL6789: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6794=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6794[ 0]=({ struct Cyc_Exp_tok_struct
_temp6795; _temp6795.tag= Cyc_Exp_tok; _temp6795.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(({
struct _tagged_ptr2 _temp6796= yyvs; void** _temp6798= _temp6796.curr +(
yyvsp_offset - 1); if( _temp6796.base == 0? 1:( _temp6798 < _temp6796.base? 1:
_temp6798 >= _temp6796.last_plus_one)){ _throw( Null_Exception);}* _temp6798;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6799= yyvs; void** _temp6801=
_temp6799.curr + yyvsp_offset; if( _temp6799.base == 0? 1:( _temp6801 <
_temp6799.base? 1: _temp6801 >= _temp6799.last_plus_one)){ _throw(
Null_Exception);}* _temp6801;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6802= yyls; struct Cyc_Yyltype* _temp6804= _temp6802.curr +(
yylsp_offset - 1); if( _temp6802.base == 0? 1:( _temp6804 < _temp6802.base? 1:
_temp6804 >= _temp6802.last_plus_one)){ _throw( Null_Exception);}* _temp6804;}).first_line,({
struct _tagged_ptr3 _temp6805= yyls; struct Cyc_Yyltype* _temp6807= _temp6805.curr
+ yylsp_offset; if( _temp6805.base == 0? 1:( _temp6807 < _temp6805.base? 1:
_temp6807 >= _temp6805.last_plus_one)){ _throw( Null_Exception);}* _temp6807;}).last_line));
_temp6795;}); _temp6794;}); break; case 345: _LL6793: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6809=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6809[ 0]=({ struct Cyc_Exp_tok_struct
_temp6810; _temp6810.tag= Cyc_Exp_tok; _temp6810.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6811= yyvs; void** _temp6813=
_temp6811.curr +( yyvsp_offset - 1); if( _temp6811.base == 0? 1:( _temp6813 <
_temp6811.base? 1: _temp6813 >= _temp6811.last_plus_one)){ _throw(
Null_Exception);}* _temp6813;}))).f3, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6814= yyls; struct Cyc_Yyltype* _temp6816= _temp6814.curr +(
yylsp_offset - 3); if( _temp6814.base == 0? 1:( _temp6816 < _temp6814.base? 1:
_temp6816 >= _temp6814.last_plus_one)){ _throw( Null_Exception);}* _temp6816;}).first_line,({
struct _tagged_ptr3 _temp6817= yyls; struct Cyc_Yyltype* _temp6819= _temp6817.curr
+ yylsp_offset; if( _temp6817.base == 0? 1:( _temp6819 < _temp6817.base? 1:
_temp6819 >= _temp6817.last_plus_one)){ _throw( Null_Exception);}* _temp6819;}).last_line));
_temp6810;}); _temp6809;}); break; case 346: _LL6808: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6821=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6821[ 0]=({ struct Cyc_Exp_tok_struct
_temp6822; _temp6822.tag= Cyc_Exp_tok; _temp6822.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6823= yyvs; void** _temp6825=
_temp6823.curr + yyvsp_offset; if( _temp6823.base == 0? 1:( _temp6825 <
_temp6823.base? 1: _temp6825 >= _temp6823.last_plus_one)){ _throw(
Null_Exception);}* _temp6825;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6826= yyls; struct Cyc_Yyltype* _temp6828= _temp6826.curr +(
yylsp_offset - 1); if( _temp6826.base == 0? 1:( _temp6828 < _temp6826.base? 1:
_temp6828 >= _temp6826.last_plus_one)){ _throw( Null_Exception);}* _temp6828;}).first_line,({
struct _tagged_ptr3 _temp6829= yyls; struct Cyc_Yyltype* _temp6831= _temp6829.curr
+ yylsp_offset; if( _temp6829.base == 0? 1:( _temp6831 < _temp6829.base? 1:
_temp6831 >= _temp6829.last_plus_one)){ _throw( Null_Exception);}* _temp6831;}).last_line));
_temp6822;}); _temp6821;}); break; case 347: _LL6820: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6833=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6833[ 0]=({ struct Cyc_Exp_tok_struct
_temp6834; _temp6834.tag= Cyc_Exp_tok; _temp6834.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(({
struct _tagged_ptr2 _temp6835= yyvs; void** _temp6837= _temp6835.curr +(
yyvsp_offset - 3); if( _temp6835.base == 0? 1:( _temp6837 < _temp6835.base? 1:
_temp6837 >= _temp6835.last_plus_one)){ _throw( Null_Exception);}* _temp6837;})),
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp6838= yyvs; void** _temp6840=
_temp6838.curr +( yyvsp_offset - 1); if( _temp6838.base == 0? 1:( _temp6840 <
_temp6838.base? 1: _temp6840 >= _temp6838.last_plus_one)){ _throw(
Null_Exception);}* _temp6840;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6841= yyls; struct Cyc_Yyltype* _temp6843= _temp6841.curr +(
yylsp_offset - 3); if( _temp6841.base == 0? 1:( _temp6843 < _temp6841.base? 1:
_temp6843 >= _temp6841.last_plus_one)){ _throw( Null_Exception);}* _temp6843;}).first_line,({
struct _tagged_ptr3 _temp6844= yyls; struct Cyc_Yyltype* _temp6846= _temp6844.curr
+ yylsp_offset; if( _temp6844.base == 0? 1:( _temp6846 < _temp6844.base? 1:
_temp6846 >= _temp6844.last_plus_one)){ _throw( Null_Exception);}* _temp6846;}).last_line));
_temp6834;}); _temp6833;}); break; case 348: _LL6832: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6848=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6848[ 0]=({ struct Cyc_Exp_tok_struct
_temp6849; _temp6849.tag= Cyc_Exp_tok; _temp6849.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp6850=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp6850[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp6851; _temp6851.tag= Cyc_Absyn_Malloc_e;
_temp6851.f1= 0; _temp6851.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp6852= yyvs; void** _temp6854= _temp6852.curr +(
yyvsp_offset - 2); if( _temp6852.base == 0? 1:( _temp6854 < _temp6852.base? 1:
_temp6854 >= _temp6852.last_plus_one)){ _throw( Null_Exception);}* _temp6854;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6855= yyls; struct Cyc_Yyltype*
_temp6857= _temp6855.curr +( yylsp_offset - 2); if( _temp6855.base == 0? 1:(
_temp6857 < _temp6855.base? 1: _temp6857 >= _temp6855.last_plus_one)){ _throw(
Null_Exception);}* _temp6857;}).first_line,({ struct _tagged_ptr3 _temp6858=
yyls; struct Cyc_Yyltype* _temp6860= _temp6858.curr +( yylsp_offset - 2); if(
_temp6858.base == 0? 1:( _temp6860 < _temp6858.base? 1: _temp6860 >= _temp6858.last_plus_one)){
_throw( Null_Exception);}* _temp6860;}).last_line)); _temp6851;}); _temp6850;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6861= yyls; struct Cyc_Yyltype*
_temp6863= _temp6861.curr +( yylsp_offset - 6); if( _temp6861.base == 0? 1:(
_temp6863 < _temp6861.base? 1: _temp6863 >= _temp6861.last_plus_one)){ _throw(
Null_Exception);}* _temp6863;}).first_line,({ struct _tagged_ptr3 _temp6864=
yyls; struct Cyc_Yyltype* _temp6866= _temp6864.curr + yylsp_offset; if(
_temp6864.base == 0? 1:( _temp6866 < _temp6864.base? 1: _temp6866 >= _temp6864.last_plus_one)){
_throw( Null_Exception);}* _temp6866;}).last_line)); _temp6849;}); _temp6848;});
break; case 349: _LL6847: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6868=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6868[ 0]=({ struct Cyc_Exp_tok_struct _temp6869; _temp6869.tag= Cyc_Exp_tok;
_temp6869.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp6870=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp6870[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp6871; _temp6871.tag= Cyc_Absyn_Malloc_e;
_temp6871.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6872= yyvs; void** _temp6874= _temp6872.curr +( yyvsp_offset - 6); if(
_temp6872.base == 0? 1:( _temp6874 < _temp6872.base? 1: _temp6874 >= _temp6872.last_plus_one)){
_throw( Null_Exception);}* _temp6874;})); _temp6871.f2=( void*) Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp6875= yyvs; void**
_temp6877= _temp6875.curr +( yyvsp_offset - 2); if( _temp6875.base == 0? 1:(
_temp6877 < _temp6875.base? 1: _temp6877 >= _temp6875.last_plus_one)){ _throw(
Null_Exception);}* _temp6877;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6878= yyls; struct Cyc_Yyltype* _temp6880= _temp6878.curr +(
yylsp_offset - 2); if( _temp6878.base == 0? 1:( _temp6880 < _temp6878.base? 1:
_temp6880 >= _temp6878.last_plus_one)){ _throw( Null_Exception);}* _temp6880;}).first_line,({
struct _tagged_ptr3 _temp6881= yyls; struct Cyc_Yyltype* _temp6883= _temp6881.curr
+( yylsp_offset - 2); if( _temp6881.base == 0? 1:( _temp6883 < _temp6881.base? 1:
_temp6883 >= _temp6881.last_plus_one)){ _throw( Null_Exception);}* _temp6883;}).last_line));
_temp6871;}); _temp6870;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6884= yyls; struct Cyc_Yyltype* _temp6886= _temp6884.curr +( yylsp_offset -
8); if( _temp6884.base == 0? 1:( _temp6886 < _temp6884.base? 1: _temp6886 >=
_temp6884.last_plus_one)){ _throw( Null_Exception);}* _temp6886;}).first_line,({
struct _tagged_ptr3 _temp6887= yyls; struct Cyc_Yyltype* _temp6889= _temp6887.curr
+ yylsp_offset; if( _temp6887.base == 0? 1:( _temp6889 < _temp6887.base? 1:
_temp6889 >= _temp6887.last_plus_one)){ _throw( Null_Exception);}* _temp6889;}).last_line));
_temp6869;}); _temp6868;}); break; case 350: _LL6867: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6891=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6891[ 0]=({ struct Cyc_Primop_tok_struct
_temp6892; _temp6892.tag= Cyc_Primop_tok; _temp6892.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp6892;}); _temp6891;}); break; case 351: _LL6890: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6894=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6894[ 0]=({ struct Cyc_Primop_tok_struct
_temp6895; _temp6895.tag= Cyc_Primop_tok; _temp6895.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp6895;}); _temp6894;}); break; case 352: _LL6893: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6897=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6897[ 0]=({ struct Cyc_Primop_tok_struct
_temp6898; _temp6898.tag= Cyc_Primop_tok; _temp6898.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp6898;}); _temp6897;}); break; case 353: _LL6896: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6900=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6900[ 0]=({ struct Cyc_Primop_tok_struct
_temp6901; _temp6901.tag= Cyc_Primop_tok; _temp6901.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp6901;}); _temp6900;}); break; case 354: _LL6899: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6903=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6903[ 0]=({ struct Cyc_Primop_tok_struct
_temp6904; _temp6904.tag= Cyc_Primop_tok; _temp6904.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp6904;}); _temp6903;}); break; case 355: _LL6902: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6906=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6906[ 0]=({ struct Cyc_Primop_tok_struct
_temp6907; _temp6907.tag= Cyc_Primop_tok; _temp6907.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp6907;}); _temp6906;}); break; case 356: _LL6905: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6909=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6909[ 0]=({ struct Cyc_Primop_tok_struct
_temp6910; _temp6910.tag= Cyc_Primop_tok; _temp6910.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp6910;}); _temp6909;}); break; case 357: _LL6908: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6912=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6912[ 0]=({ struct Cyc_Primop_tok_struct
_temp6913; _temp6913.tag= Cyc_Primop_tok; _temp6913.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp6913;}); _temp6912;}); break; case 358: _LL6911: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp6915=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6915[ 0]=({ struct Cyc_Primop_tok_struct
_temp6916; _temp6916.tag= Cyc_Primop_tok; _temp6916.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp6916;}); _temp6915;}); break; case 359: _LL6914: yyval=({ struct
_tagged_ptr2 _temp6918= yyvs; void** _temp6920= _temp6918.curr + yyvsp_offset;
if( _temp6918.base == 0? 1:( _temp6920 < _temp6918.base? 1: _temp6920 >=
_temp6918.last_plus_one)){ _throw( Null_Exception);}* _temp6920;}); break; case
360: _LL6917: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6922=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6922[ 0]=({ struct Cyc_Exp_tok_struct
_temp6923; _temp6923.tag= Cyc_Exp_tok; _temp6923.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6924= yyvs; void** _temp6926=
_temp6924.curr +( yyvsp_offset - 3); if( _temp6924.base == 0? 1:( _temp6926 <
_temp6924.base? 1: _temp6926 >= _temp6924.last_plus_one)){ _throw(
Null_Exception);}* _temp6926;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6927= yyvs; void** _temp6929= _temp6927.curr +( yyvsp_offset - 1); if(
_temp6927.base == 0? 1:( _temp6929 < _temp6927.base? 1: _temp6929 >= _temp6927.last_plus_one)){
_throw( Null_Exception);}* _temp6929;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6930= yyls; struct Cyc_Yyltype* _temp6932= _temp6930.curr +(
yylsp_offset - 3); if( _temp6930.base == 0? 1:( _temp6932 < _temp6930.base? 1:
_temp6932 >= _temp6930.last_plus_one)){ _throw( Null_Exception);}* _temp6932;}).first_line,({
struct _tagged_ptr3 _temp6933= yyls; struct Cyc_Yyltype* _temp6935= _temp6933.curr
+ yylsp_offset; if( _temp6933.base == 0? 1:( _temp6935 < _temp6933.base? 1:
_temp6935 >= _temp6933.last_plus_one)){ _throw( Null_Exception);}* _temp6935;}).last_line));
_temp6923;}); _temp6922;}); break; case 361: _LL6921: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6937=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6937[ 0]=({ struct Cyc_Exp_tok_struct
_temp6938; _temp6938.tag= Cyc_Exp_tok; _temp6938.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6939= yyvs; void** _temp6941=
_temp6939.curr +( yyvsp_offset - 2); if( _temp6939.base == 0? 1:( _temp6941 <
_temp6939.base? 1: _temp6941 >= _temp6939.last_plus_one)){ _throw(
Null_Exception);}* _temp6941;})), 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6942= yyls; struct Cyc_Yyltype* _temp6944= _temp6942.curr +(
yylsp_offset - 2); if( _temp6942.base == 0? 1:( _temp6944 < _temp6942.base? 1:
_temp6944 >= _temp6942.last_plus_one)){ _throw( Null_Exception);}* _temp6944;}).first_line,({
struct _tagged_ptr3 _temp6945= yyls; struct Cyc_Yyltype* _temp6947= _temp6945.curr
+ yylsp_offset; if( _temp6945.base == 0? 1:( _temp6947 < _temp6945.base? 1:
_temp6947 >= _temp6945.last_plus_one)){ _throw( Null_Exception);}* _temp6947;}).last_line));
_temp6938;}); _temp6937;}); break; case 362: _LL6936: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6949=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6949[ 0]=({ struct Cyc_Exp_tok_struct
_temp6950; _temp6950.tag= Cyc_Exp_tok; _temp6950.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6951= yyvs; void** _temp6953=
_temp6951.curr +( yyvsp_offset - 3); if( _temp6951.base == 0? 1:( _temp6953 <
_temp6951.base? 1: _temp6953 >= _temp6951.last_plus_one)){ _throw(
Null_Exception);}* _temp6953;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp6954= yyvs; void** _temp6956= _temp6954.curr +( yyvsp_offset - 1); if(
_temp6954.base == 0? 1:( _temp6956 < _temp6954.base? 1: _temp6956 >= _temp6954.last_plus_one)){
_throw( Null_Exception);}* _temp6956;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6957= yyls; struct Cyc_Yyltype* _temp6959= _temp6957.curr +(
yylsp_offset - 3); if( _temp6957.base == 0? 1:( _temp6959 < _temp6957.base? 1:
_temp6959 >= _temp6957.last_plus_one)){ _throw( Null_Exception);}* _temp6959;}).first_line,({
struct _tagged_ptr3 _temp6960= yyls; struct Cyc_Yyltype* _temp6962= _temp6960.curr
+ yylsp_offset; if( _temp6960.base == 0? 1:( _temp6962 < _temp6960.base? 1:
_temp6962 >= _temp6960.last_plus_one)){ _throw( Null_Exception);}* _temp6962;}).last_line));
_temp6950;}); _temp6949;}); break; case 363: _LL6948: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6964=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6964[ 0]=({ struct Cyc_Exp_tok_struct
_temp6965; _temp6965.tag= Cyc_Exp_tok; _temp6965.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6966= yyvs; void** _temp6968=
_temp6966.curr +( yyvsp_offset - 2); if( _temp6966.base == 0? 1:( _temp6968 <
_temp6966.base? 1: _temp6968 >= _temp6966.last_plus_one)){ _throw(
Null_Exception);}* _temp6968;})),({ struct _tagged_string* _temp6969=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp6969[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6970= yyvs; void** _temp6972=
_temp6970.curr + yyvsp_offset; if( _temp6970.base == 0? 1:( _temp6972 <
_temp6970.base? 1: _temp6972 >= _temp6970.last_plus_one)){ _throw(
Null_Exception);}* _temp6972;})); _temp6969;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6973= yyls; struct Cyc_Yyltype* _temp6975= _temp6973.curr
+( yylsp_offset - 2); if( _temp6973.base == 0? 1:( _temp6975 < _temp6973.base? 1:
_temp6975 >= _temp6973.last_plus_one)){ _throw( Null_Exception);}* _temp6975;}).first_line,({
struct _tagged_ptr3 _temp6976= yyls; struct Cyc_Yyltype* _temp6978= _temp6976.curr
+ yylsp_offset; if( _temp6976.base == 0? 1:( _temp6978 < _temp6976.base? 1:
_temp6978 >= _temp6976.last_plus_one)){ _throw( Null_Exception);}* _temp6978;}).last_line));
_temp6965;}); _temp6964;}); break; case 364: _LL6963: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp7013= yyvs; void** _temp7015= _temp7013.curr +
yyvsp_offset; if( _temp7013.base == 0? 1:( _temp7015 < _temp7013.base? 1:
_temp7015 >= _temp7013.last_plus_one)){ _throw( Null_Exception);}* _temp7015;}));{
void* _temp6980=(* q).f1; struct Cyc_List_List* _temp6990; struct Cyc_List_List*
_temp6992; _LL6982: if( _temp6980 ==( void*) Cyc_Absyn_Loc_n){ goto _LL6983;}
else{ goto _LL6984;} _LL6984: if(( unsigned int) _temp6980 > 1u?*(( int*)
_temp6980) == Cyc_Absyn_Rel_n: 0){ _LL6991: _temp6990=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp6980)->f1; if( _temp6990 == 0){ goto
_LL6985;} else{ goto _LL6986;}} else{ goto _LL6986;} _LL6986: if(( unsigned int)
_temp6980 > 1u?*(( int*) _temp6980) == Cyc_Absyn_Abs_n: 0){ _LL6993: _temp6992=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp6980)->f1; if(
_temp6992 == 0){ goto _LL6987;} else{ goto _LL6988;}} else{ goto _LL6988;}
_LL6988: goto _LL6989; _LL6983: goto _LL6981; _LL6985: goto _LL6981; _LL6987:
goto _LL6981; _LL6989: Cyc_Parse_err(( struct _tagged_string)({ char* _temp6994=(
char*)"struct field name is qualified"; struct _tagged_string _temp6995;
_temp6995.curr= _temp6994; _temp6995.base= _temp6994; _temp6995.last_plus_one=
_temp6994 + 31; _temp6995;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6996= yyls; struct Cyc_Yyltype* _temp6998= _temp6996.curr + yylsp_offset;
if( _temp6996.base == 0? 1:( _temp6998 < _temp6996.base? 1: _temp6998 >=
_temp6996.last_plus_one)){ _throw( Null_Exception);}* _temp6998;}).first_line,({
struct _tagged_ptr3 _temp6999= yyls; struct Cyc_Yyltype* _temp7001= _temp6999.curr
+ yylsp_offset; if( _temp6999.base == 0? 1:( _temp7001 < _temp6999.base? 1:
_temp7001 >= _temp6999.last_plus_one)){ _throw( Null_Exception);}* _temp7001;}).last_line));
goto _LL6981; _LL6981:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7002=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7002[ 0]=({ struct Cyc_Exp_tok_struct _temp7003; _temp7003.tag= Cyc_Exp_tok;
_temp7003.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7004= yyvs; void** _temp7006= _temp7004.curr +( yyvsp_offset -
2); if( _temp7004.base == 0? 1:( _temp7006 < _temp7004.base? 1: _temp7006 >=
_temp7004.last_plus_one)){ _throw( Null_Exception);}* _temp7006;})),(* q).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7007= yyls; struct Cyc_Yyltype*
_temp7009= _temp7007.curr +( yylsp_offset - 2); if( _temp7007.base == 0? 1:(
_temp7009 < _temp7007.base? 1: _temp7009 >= _temp7007.last_plus_one)){ _throw(
Null_Exception);}* _temp7009;}).first_line,({ struct _tagged_ptr3 _temp7010=
yyls; struct Cyc_Yyltype* _temp7012= _temp7010.curr + yylsp_offset; if(
_temp7010.base == 0? 1:( _temp7012 < _temp7010.base? 1: _temp7012 >= _temp7010.last_plus_one)){
_throw( Null_Exception);}* _temp7012;}).last_line)); _temp7003;}); _temp7002;});
break;} case 365: _LL6979: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7017=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7017[ 0]=({ struct Cyc_Exp_tok_struct _temp7018; _temp7018.tag= Cyc_Exp_tok;
_temp7018.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7019= yyvs; void** _temp7021= _temp7019.curr +( yyvsp_offset -
2); if( _temp7019.base == 0? 1:( _temp7021 < _temp7019.base? 1: _temp7021 >=
_temp7019.last_plus_one)){ _throw( Null_Exception);}* _temp7021;})),({ struct
_tagged_string* _temp7022=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp7022[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7023= yyvs; void** _temp7025= _temp7023.curr + yyvsp_offset; if( _temp7023.base
== 0? 1:( _temp7025 < _temp7023.base? 1: _temp7025 >= _temp7023.last_plus_one)){
_throw( Null_Exception);}* _temp7025;})); _temp7022;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7026= yyls; struct Cyc_Yyltype* _temp7028= _temp7026.curr
+( yylsp_offset - 2); if( _temp7026.base == 0? 1:( _temp7028 < _temp7026.base? 1:
_temp7028 >= _temp7026.last_plus_one)){ _throw( Null_Exception);}* _temp7028;}).first_line,({
struct _tagged_ptr3 _temp7029= yyls; struct Cyc_Yyltype* _temp7031= _temp7029.curr
+ yylsp_offset; if( _temp7029.base == 0? 1:( _temp7031 < _temp7029.base? 1:
_temp7031 >= _temp7029.last_plus_one)){ _throw( Null_Exception);}* _temp7031;}).last_line));
_temp7018;}); _temp7017;}); break; case 366: _LL7016: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp7066= yyvs; void** _temp7068= _temp7066.curr +
yyvsp_offset; if( _temp7066.base == 0? 1:( _temp7068 < _temp7066.base? 1:
_temp7068 >= _temp7066.last_plus_one)){ _throw( Null_Exception);}* _temp7068;}));{
void* _temp7033=(* q).f1; struct Cyc_List_List* _temp7043; struct Cyc_List_List*
_temp7045; _LL7035: if( _temp7033 ==( void*) Cyc_Absyn_Loc_n){ goto _LL7036;}
else{ goto _LL7037;} _LL7037: if(( unsigned int) _temp7033 > 1u?*(( int*)
_temp7033) == Cyc_Absyn_Rel_n: 0){ _LL7044: _temp7043=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp7033)->f1; if( _temp7043 == 0){ goto
_LL7038;} else{ goto _LL7039;}} else{ goto _LL7039;} _LL7039: if(( unsigned int)
_temp7033 > 1u?*(( int*) _temp7033) == Cyc_Absyn_Abs_n: 0){ _LL7046: _temp7045=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp7033)->f1; if(
_temp7045 == 0){ goto _LL7040;} else{ goto _LL7041;}} else{ goto _LL7041;}
_LL7041: goto _LL7042; _LL7036: goto _LL7034; _LL7038: goto _LL7034; _LL7040:
goto _LL7034; _LL7042: Cyc_Parse_err(( struct _tagged_string)({ char* _temp7047=(
char*)"struct field is qualified with module name"; struct _tagged_string
_temp7048; _temp7048.curr= _temp7047; _temp7048.base= _temp7047; _temp7048.last_plus_one=
_temp7047 + 43; _temp7048;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7049= yyls; struct Cyc_Yyltype* _temp7051= _temp7049.curr + yylsp_offset;
if( _temp7049.base == 0? 1:( _temp7051 < _temp7049.base? 1: _temp7051 >=
_temp7049.last_plus_one)){ _throw( Null_Exception);}* _temp7051;}).first_line,({
struct _tagged_ptr3 _temp7052= yyls; struct Cyc_Yyltype* _temp7054= _temp7052.curr
+ yylsp_offset; if( _temp7052.base == 0? 1:( _temp7054 < _temp7052.base? 1:
_temp7054 >= _temp7052.last_plus_one)){ _throw( Null_Exception);}* _temp7054;}).last_line));
goto _LL7034; _LL7034:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7055=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7055[ 0]=({ struct Cyc_Exp_tok_struct _temp7056; _temp7056.tag= Cyc_Exp_tok;
_temp7056.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7057= yyvs; void** _temp7059= _temp7057.curr +( yyvsp_offset -
2); if( _temp7057.base == 0? 1:( _temp7059 < _temp7057.base? 1: _temp7059 >=
_temp7057.last_plus_one)){ _throw( Null_Exception);}* _temp7059;})),(* q).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7060= yyls; struct Cyc_Yyltype*
_temp7062= _temp7060.curr +( yylsp_offset - 2); if( _temp7060.base == 0? 1:(
_temp7062 < _temp7060.base? 1: _temp7062 >= _temp7060.last_plus_one)){ _throw(
Null_Exception);}* _temp7062;}).first_line,({ struct _tagged_ptr3 _temp7063=
yyls; struct Cyc_Yyltype* _temp7065= _temp7063.curr + yylsp_offset; if(
_temp7063.base == 0? 1:( _temp7065 < _temp7063.base? 1: _temp7065 >= _temp7063.last_plus_one)){
_throw( Null_Exception);}* _temp7065;}).last_line)); _temp7056;}); _temp7055;});
break;} case 367: _LL7032: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7070=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7070[ 0]=({ struct Cyc_Exp_tok_struct _temp7071; _temp7071.tag= Cyc_Exp_tok;
_temp7071.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp7072= yyvs; void** _temp7074= _temp7072.curr +( yyvsp_offset - 1); if(
_temp7072.base == 0? 1:( _temp7074 < _temp7072.base? 1: _temp7074 >= _temp7072.last_plus_one)){
_throw( Null_Exception);}* _temp7074;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7075= yyls; struct Cyc_Yyltype* _temp7077= _temp7075.curr +(
yylsp_offset - 1); if( _temp7075.base == 0? 1:( _temp7077 < _temp7075.base? 1:
_temp7077 >= _temp7075.last_plus_one)){ _throw( Null_Exception);}* _temp7077;}).first_line,({
struct _tagged_ptr3 _temp7078= yyls; struct Cyc_Yyltype* _temp7080= _temp7078.curr
+ yylsp_offset; if( _temp7078.base == 0? 1:( _temp7080 < _temp7078.base? 1:
_temp7080 >= _temp7078.last_plus_one)){ _throw( Null_Exception);}* _temp7080;}).last_line));
_temp7071;}); _temp7070;}); break; case 368: _LL7069: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7082=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7082[ 0]=({ struct Cyc_Exp_tok_struct
_temp7083; _temp7083.tag= Cyc_Exp_tok; _temp7083.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7084= yyvs; void** _temp7086= _temp7084.curr +(
yyvsp_offset - 1); if( _temp7084.base == 0? 1:( _temp7086 < _temp7084.base? 1:
_temp7086 >= _temp7084.last_plus_one)){ _throw( Null_Exception);}* _temp7086;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7087= yyls; struct Cyc_Yyltype*
_temp7089= _temp7087.curr +( yylsp_offset - 1); if( _temp7087.base == 0? 1:(
_temp7089 < _temp7087.base? 1: _temp7089 >= _temp7087.last_plus_one)){ _throw(
Null_Exception);}* _temp7089;}).first_line,({ struct _tagged_ptr3 _temp7090=
yyls; struct Cyc_Yyltype* _temp7092= _temp7090.curr + yylsp_offset; if(
_temp7090.base == 0? 1:( _temp7092 < _temp7090.base? 1: _temp7092 >= _temp7090.last_plus_one)){
_throw( Null_Exception);}* _temp7092;}).last_line)); _temp7083;}); _temp7082;});
break; case 369: _LL7081: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7094=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7094[ 0]=({ struct Cyc_Exp_tok_struct _temp7095; _temp7095.tag= Cyc_Exp_tok;
_temp7095.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_CompoundLit_e_struct*
_temp7096=( struct Cyc_Absyn_CompoundLit_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_CompoundLit_e_struct)); _temp7096[ 0]=({ struct Cyc_Absyn_CompoundLit_e_struct
_temp7097; _temp7097.tag= Cyc_Absyn_CompoundLit_e; _temp7097.f1= Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp7098= yyvs; void** _temp7100= _temp7098.curr +(
yyvsp_offset - 4); if( _temp7098.base == 0? 1:( _temp7100 < _temp7098.base? 1:
_temp7100 >= _temp7098.last_plus_one)){ _throw( Null_Exception);}* _temp7100;}));
_temp7097.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp7101= yyvs; void**
_temp7103= _temp7101.curr +( yyvsp_offset - 1); if( _temp7101.base == 0? 1:(
_temp7103 < _temp7101.base? 1: _temp7103 >= _temp7101.last_plus_one)){ _throw(
Null_Exception);}* _temp7103;}))); _temp7097;}); _temp7096;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7104= yyls; struct Cyc_Yyltype* _temp7106= _temp7104.curr
+( yylsp_offset - 5); if( _temp7104.base == 0? 1:( _temp7106 < _temp7104.base? 1:
_temp7106 >= _temp7104.last_plus_one)){ _throw( Null_Exception);}* _temp7106;}).first_line,({
struct _tagged_ptr3 _temp7107= yyls; struct Cyc_Yyltype* _temp7109= _temp7107.curr
+ yylsp_offset; if( _temp7107.base == 0? 1:( _temp7109 < _temp7107.base? 1:
_temp7109 >= _temp7107.last_plus_one)){ _throw( Null_Exception);}* _temp7109;}).last_line));
_temp7095;}); _temp7094;}); break; case 370: _LL7093: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7111=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7111[ 0]=({ struct Cyc_Exp_tok_struct
_temp7112; _temp7112.tag= Cyc_Exp_tok; _temp7112.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp7113=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7113[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7114; _temp7114.tag= Cyc_Absyn_CompoundLit_e;
_temp7114.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp7115= yyvs;
void** _temp7117= _temp7115.curr +( yyvsp_offset - 5); if( _temp7115.base == 0?
1:( _temp7117 < _temp7115.base? 1: _temp7117 >= _temp7115.last_plus_one)){
_throw( Null_Exception);}* _temp7117;})); _temp7114.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7118= yyvs; void** _temp7120= _temp7118.curr +(
yyvsp_offset - 2); if( _temp7118.base == 0? 1:( _temp7120 < _temp7118.base? 1:
_temp7120 >= _temp7118.last_plus_one)){ _throw( Null_Exception);}* _temp7120;})));
_temp7114;}); _temp7113;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7121= yyls; struct Cyc_Yyltype* _temp7123= _temp7121.curr +( yylsp_offset -
6); if( _temp7121.base == 0? 1:( _temp7123 < _temp7121.base? 1: _temp7123 >=
_temp7121.last_plus_one)){ _throw( Null_Exception);}* _temp7123;}).first_line,({
struct _tagged_ptr3 _temp7124= yyls; struct Cyc_Yyltype* _temp7126= _temp7124.curr
+ yylsp_offset; if( _temp7124.base == 0? 1:( _temp7126 < _temp7124.base? 1:
_temp7126 >= _temp7124.last_plus_one)){ _throw( Null_Exception);}* _temp7126;}).last_line));
_temp7112;}); _temp7111;}); break; case 371: _LL7110: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7128=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7128[ 0]=({ struct Cyc_Exp_tok_struct
_temp7129; _temp7129.tag= Cyc_Exp_tok; _temp7129.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp7130=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp7130[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp7131; _temp7131.tag= Cyc_Absyn_Fill_e; _temp7131.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7132= yyvs; void** _temp7134= _temp7132.curr +(
yyvsp_offset - 1); if( _temp7132.base == 0? 1:( _temp7134 < _temp7132.base? 1:
_temp7134 >= _temp7132.last_plus_one)){ _throw( Null_Exception);}* _temp7134;}));
_temp7131;}); _temp7130;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7135= yyls; struct Cyc_Yyltype* _temp7137= _temp7135.curr +( yylsp_offset -
3); if( _temp7135.base == 0? 1:( _temp7137 < _temp7135.base? 1: _temp7137 >=
_temp7135.last_plus_one)){ _throw( Null_Exception);}* _temp7137;}).first_line,({
struct _tagged_ptr3 _temp7138= yyls; struct Cyc_Yyltype* _temp7140= _temp7138.curr
+ yylsp_offset; if( _temp7138.base == 0? 1:( _temp7140 < _temp7138.base? 1:
_temp7140 >= _temp7138.last_plus_one)){ _throw( Null_Exception);}* _temp7140;}).last_line));
_temp7129;}); _temp7128;}); break; case 372: _LL7127: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7142=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7142[ 0]=({ struct Cyc_Exp_tok_struct
_temp7143; _temp7143.tag= Cyc_Exp_tok; _temp7143.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp7144=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp7144[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp7145; _temp7145.tag= Cyc_Absyn_Codegen_e;
_temp7145.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp7146= yyvs; void**
_temp7148= _temp7146.curr +( yyvsp_offset - 1); if( _temp7146.base == 0? 1:(
_temp7148 < _temp7146.base? 1: _temp7148 >= _temp7146.last_plus_one)){ _throw(
Null_Exception);}* _temp7148;})); _temp7145;}); _temp7144;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7149= yyls; struct Cyc_Yyltype* _temp7151= _temp7149.curr
+( yylsp_offset - 3); if( _temp7149.base == 0? 1:( _temp7151 < _temp7149.base? 1:
_temp7151 >= _temp7149.last_plus_one)){ _throw( Null_Exception);}* _temp7151;}).first_line,({
struct _tagged_ptr3 _temp7152= yyls; struct Cyc_Yyltype* _temp7154= _temp7152.curr
+ yylsp_offset; if( _temp7152.base == 0? 1:( _temp7154 < _temp7152.base? 1:
_temp7154 >= _temp7152.last_plus_one)){ _throw( Null_Exception);}* _temp7154;}).last_line));
_temp7143;}); _temp7142;}); break; case 373: _LL7141: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7156=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7156[ 0]=({ struct Cyc_Exp_tok_struct
_temp7157; _temp7157.tag= Cyc_Exp_tok; _temp7157.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp7158=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp7158[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp7159; _temp7159.tag= Cyc_Absyn_UnknownId_e;
_temp7159.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7160= yyvs; void**
_temp7162= _temp7160.curr + yyvsp_offset; if( _temp7160.base == 0? 1:( _temp7162
< _temp7160.base? 1: _temp7162 >= _temp7160.last_plus_one)){ _throw(
Null_Exception);}* _temp7162;})); _temp7159;}); _temp7158;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7163= yyls; struct Cyc_Yyltype* _temp7165= _temp7163.curr
+ yylsp_offset; if( _temp7163.base == 0? 1:( _temp7165 < _temp7163.base? 1:
_temp7165 >= _temp7163.last_plus_one)){ _throw( Null_Exception);}* _temp7165;}).first_line,({
struct _tagged_ptr3 _temp7166= yyls; struct Cyc_Yyltype* _temp7168= _temp7166.curr
+ yylsp_offset; if( _temp7166.base == 0? 1:( _temp7168 < _temp7166.base? 1:
_temp7168 >= _temp7166.last_plus_one)){ _throw( Null_Exception);}* _temp7168;}).last_line));
_temp7157;}); _temp7156;}); break; case 374: _LL7155: yyval=({ struct
_tagged_ptr2 _temp7170= yyvs; void** _temp7172= _temp7170.curr + yyvsp_offset;
if( _temp7170.base == 0? 1:( _temp7172 < _temp7170.base? 1: _temp7172 >=
_temp7170.last_plus_one)){ _throw( Null_Exception);}* _temp7172;}); break; case
375: _LL7169: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7174=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7174[ 0]=({ struct Cyc_Exp_tok_struct
_temp7175; _temp7175.tag= Cyc_Exp_tok; _temp7175.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp7176= yyvs; void** _temp7178= _temp7176.curr +
yyvsp_offset; if( _temp7176.base == 0? 1:( _temp7178 < _temp7176.base? 1:
_temp7178 >= _temp7176.last_plus_one)){ _throw( Null_Exception);}* _temp7178;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7179= yyls; struct Cyc_Yyltype*
_temp7181= _temp7179.curr + yylsp_offset; if( _temp7179.base == 0? 1:( _temp7181
< _temp7179.base? 1: _temp7181 >= _temp7179.last_plus_one)){ _throw(
Null_Exception);}* _temp7181;}).first_line,({ struct _tagged_ptr3 _temp7182=
yyls; struct Cyc_Yyltype* _temp7184= _temp7182.curr + yylsp_offset; if(
_temp7182.base == 0? 1:( _temp7184 < _temp7182.base? 1: _temp7184 >= _temp7182.last_plus_one)){
_throw( Null_Exception);}* _temp7184;}).last_line)); _temp7175;}); _temp7174;});
break; case 376: _LL7173: yyval=({ struct _tagged_ptr2 _temp7186= yyvs; void**
_temp7188= _temp7186.curr +( yyvsp_offset - 1); if( _temp7186.base == 0? 1:(
_temp7188 < _temp7186.base? 1: _temp7188 >= _temp7186.last_plus_one)){ _throw(
Null_Exception);}* _temp7188;}); break; case 377: _LL7185: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp7190=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp7190[ 0]=({ struct Cyc_Exp_tok_struct
_temp7191; _temp7191.tag= Cyc_Exp_tok; _temp7191.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7192=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7192[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7193; _temp7193.tag=
Cyc_Absyn_UnknownId_e; _temp7193.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp7194= yyvs; void** _temp7196= _temp7194.curr +( yyvsp_offset - 1); if(
_temp7194.base == 0? 1:( _temp7196 < _temp7194.base? 1: _temp7196 >= _temp7194.last_plus_one)){
_throw( Null_Exception);}* _temp7196;})); _temp7193;}); _temp7192;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7197= yyls; struct Cyc_Yyltype* _temp7199= _temp7197.curr
+( yylsp_offset - 1); if( _temp7197.base == 0? 1:( _temp7199 < _temp7197.base? 1:
_temp7199 >= _temp7197.last_plus_one)){ _throw( Null_Exception);}* _temp7199;}).first_line,({
struct _tagged_ptr3 _temp7200= yyls; struct Cyc_Yyltype* _temp7202= _temp7200.curr
+( yylsp_offset - 1); if( _temp7200.base == 0? 1:( _temp7202 < _temp7200.base? 1:
_temp7202 >= _temp7200.last_plus_one)){ _throw( Null_Exception);}* _temp7202;}).last_line)),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7203= yyls; struct Cyc_Yyltype*
_temp7205= _temp7203.curr +( yylsp_offset - 1); if( _temp7203.base == 0? 1:(
_temp7205 < _temp7203.base? 1: _temp7205 >= _temp7203.last_plus_one)){ _throw(
Null_Exception);}* _temp7205;}).first_line,({ struct _tagged_ptr3 _temp7206=
yyls; struct Cyc_Yyltype* _temp7208= _temp7206.curr + yylsp_offset; if(
_temp7206.base == 0? 1:( _temp7208 < _temp7206.base? 1: _temp7208 >= _temp7206.last_plus_one)){
_throw( Null_Exception);}* _temp7208;}).last_line)); _temp7191;}); _temp7190;});
break; case 378: _LL7189: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7210=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7210[ 0]=({ struct Cyc_Exp_tok_struct _temp7211; _temp7211.tag= Cyc_Exp_tok;
_temp7211.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp7212=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7212[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7213; _temp7213.tag=
Cyc_Absyn_UnknownId_e; _temp7213.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp7214= yyvs; void** _temp7216= _temp7214.curr +( yyvsp_offset - 4); if(
_temp7214.base == 0? 1:( _temp7216 < _temp7214.base? 1: _temp7216 >= _temp7214.last_plus_one)){
_throw( Null_Exception);}* _temp7216;})); _temp7213;}); _temp7212;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7217= yyls; struct Cyc_Yyltype* _temp7219= _temp7217.curr
+( yylsp_offset - 4); if( _temp7217.base == 0? 1:( _temp7219 < _temp7217.base? 1:
_temp7219 >= _temp7217.last_plus_one)){ _throw( Null_Exception);}* _temp7219;}).first_line,({
struct _tagged_ptr3 _temp7220= yyls; struct Cyc_Yyltype* _temp7222= _temp7220.curr
+( yylsp_offset - 4); if( _temp7220.base == 0? 1:( _temp7222 < _temp7220.base? 1:
_temp7222 >= _temp7220.last_plus_one)){ _throw( Null_Exception);}* _temp7222;}).last_line)),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp7223= yyvs; void** _temp7225= _temp7223.curr +(
yyvsp_offset - 1); if( _temp7223.base == 0? 1:( _temp7225 < _temp7223.base? 1:
_temp7225 >= _temp7223.last_plus_one)){ _throw( Null_Exception);}* _temp7225;}))),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7226= yyls; struct Cyc_Yyltype*
_temp7228= _temp7226.curr +( yylsp_offset - 4); if( _temp7226.base == 0? 1:(
_temp7228 < _temp7226.base? 1: _temp7228 >= _temp7226.last_plus_one)){ _throw(
Null_Exception);}* _temp7228;}).first_line,({ struct _tagged_ptr3 _temp7229=
yyls; struct Cyc_Yyltype* _temp7231= _temp7229.curr + yylsp_offset; if(
_temp7229.base == 0? 1:( _temp7231 < _temp7229.base? 1: _temp7231 >= _temp7229.last_plus_one)){
_throw( Null_Exception);}* _temp7231;}).last_line)); _temp7211;}); _temp7210;});
break; case 379: _LL7209: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7233=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7233[ 0]=({ struct Cyc_Exp_tok_struct _temp7234; _temp7234.tag= Cyc_Exp_tok;
_temp7234.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp7235= yyvs; void** _temp7237= _temp7235.curr +( yyvsp_offset - 1); if(
_temp7235.base == 0? 1:( _temp7237 < _temp7235.base? 1: _temp7237 >= _temp7235.last_plus_one)){
_throw( Null_Exception);}* _temp7237;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7238= yyls; struct Cyc_Yyltype* _temp7240= _temp7238.curr +(
yylsp_offset - 3); if( _temp7238.base == 0? 1:( _temp7240 < _temp7238.base? 1:
_temp7240 >= _temp7238.last_plus_one)){ _throw( Null_Exception);}* _temp7240;}).first_line,({
struct _tagged_ptr3 _temp7241= yyls; struct Cyc_Yyltype* _temp7243= _temp7241.curr
+ yylsp_offset; if( _temp7241.base == 0? 1:( _temp7243 < _temp7241.base? 1:
_temp7243 >= _temp7241.last_plus_one)){ _throw( Null_Exception);}* _temp7243;}).last_line));
_temp7234;}); _temp7233;}); break; case 380: _LL7232: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7245=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7245[ 0]=({ struct Cyc_Exp_tok_struct
_temp7246; _temp7246.tag= Cyc_Exp_tok; _temp7246.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp7247=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp7247[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp7248; _temp7248.tag= Cyc_Absyn_Struct_e;
_temp7248.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7249= yyvs; void**
_temp7251= _temp7249.curr +( yyvsp_offset - 3); if( _temp7249.base == 0? 1:(
_temp7251 < _temp7249.base? 1: _temp7251 >= _temp7249.last_plus_one)){ _throw(
Null_Exception);}* _temp7251;})); _temp7248.f2= 0; _temp7248.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7252= yyvs; void** _temp7254= _temp7252.curr +(
yyvsp_offset - 1); if( _temp7252.base == 0? 1:( _temp7254 < _temp7252.base? 1:
_temp7254 >= _temp7252.last_plus_one)){ _throw( Null_Exception);}* _temp7254;})));
_temp7248.f4= 0; _temp7248;}); _temp7247;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7255= yyls; struct Cyc_Yyltype* _temp7257= _temp7255.curr
+( yylsp_offset - 3); if( _temp7255.base == 0? 1:( _temp7257 < _temp7255.base? 1:
_temp7257 >= _temp7255.last_plus_one)){ _throw( Null_Exception);}* _temp7257;}).first_line,({
struct _tagged_ptr3 _temp7258= yyls; struct Cyc_Yyltype* _temp7260= _temp7258.curr
+ yylsp_offset; if( _temp7258.base == 0? 1:( _temp7260 < _temp7258.base? 1:
_temp7260 >= _temp7258.last_plus_one)){ _throw( Null_Exception);}* _temp7260;}).last_line));
_temp7246;}); _temp7245;}); break; case 381: _LL7244: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7262=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7262[ 0]=({ struct Cyc_Exp_tok_struct
_temp7263; _temp7263.tag= Cyc_Exp_tok; _temp7263.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp7264= yyvs; void** _temp7266= _temp7264.curr +(
yyvsp_offset - 2); if( _temp7264.base == 0? 1:( _temp7266 < _temp7264.base? 1:
_temp7266 >= _temp7264.last_plus_one)){ _throw( Null_Exception);}* _temp7266;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7267= yyls; struct Cyc_Yyltype*
_temp7269= _temp7267.curr +( yylsp_offset - 4); if( _temp7267.base == 0? 1:(
_temp7269 < _temp7267.base? 1: _temp7269 >= _temp7267.last_plus_one)){ _throw(
Null_Exception);}* _temp7269;}).first_line,({ struct _tagged_ptr3 _temp7270=
yyls; struct Cyc_Yyltype* _temp7272= _temp7270.curr + yylsp_offset; if(
_temp7270.base == 0? 1:( _temp7272 < _temp7270.base? 1: _temp7272 >= _temp7270.last_plus_one)){
_throw( Null_Exception);}* _temp7272;}).last_line)); _temp7263;}); _temp7262;});
break; case 382: _LL7261: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp7274=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7274[ 0]=({ struct Cyc_ExpList_tok_struct _temp7275; _temp7275.tag= Cyc_ExpList_tok;
_temp7275.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7276= yyvs; void** _temp7278=
_temp7276.curr + yyvsp_offset; if( _temp7276.base == 0? 1:( _temp7278 <
_temp7276.base? 1: _temp7278 >= _temp7276.last_plus_one)){ _throw(
Null_Exception);}* _temp7278;}))); _temp7275;}); _temp7274;}); break; case 383:
_LL7273: yyval=( void*)({ struct Cyc_ExpList_tok_struct* _temp7280=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7280[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7281; _temp7281.tag= Cyc_ExpList_tok; _temp7281.f1=({ struct Cyc_List_List*
_temp7282=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7282->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7283= yyvs;
void** _temp7285= _temp7283.curr + yyvsp_offset; if( _temp7283.base == 0? 1:(
_temp7285 < _temp7283.base? 1: _temp7285 >= _temp7283.last_plus_one)){ _throw(
Null_Exception);}* _temp7285;})); _temp7282->tl= 0; _temp7282;}); _temp7281;});
_temp7280;}); break; case 384: _LL7279: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp7287=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7287[ 0]=({ struct Cyc_ExpList_tok_struct _temp7288; _temp7288.tag= Cyc_ExpList_tok;
_temp7288.f1=({ struct Cyc_List_List* _temp7289=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp7289->hd=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7293= yyvs; void** _temp7295= _temp7293.curr +
yyvsp_offset; if( _temp7293.base == 0? 1:( _temp7295 < _temp7293.base? 1:
_temp7295 >= _temp7293.last_plus_one)){ _throw( Null_Exception);}* _temp7295;}));
_temp7289->tl= Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7290= yyvs;
void** _temp7292= _temp7290.curr +( yyvsp_offset - 2); if( _temp7290.base == 0?
1:( _temp7292 < _temp7290.base? 1: _temp7292 >= _temp7290.last_plus_one)){
_throw( Null_Exception);}* _temp7292;})); _temp7289;}); _temp7288;}); _temp7287;});
break; case 385: _LL7286: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7297=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7297[ 0]=({ struct Cyc_Exp_tok_struct _temp7298; _temp7298.tag= Cyc_Exp_tok;
_temp7298.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp7299= yyvs; void** _temp7301= _temp7299.curr + yyvsp_offset; if( _temp7299.base
== 0? 1:( _temp7301 < _temp7299.base? 1: _temp7301 >= _temp7299.last_plus_one)){
_throw( Null_Exception);}* _temp7301;}))).f1,(* Cyc_yyget_Int_tok(({ struct
_tagged_ptr2 _temp7302= yyvs; void** _temp7304= _temp7302.curr + yyvsp_offset;
if( _temp7302.base == 0? 1:( _temp7304 < _temp7302.base? 1: _temp7304 >=
_temp7302.last_plus_one)){ _throw( Null_Exception);}* _temp7304;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7305= yyls; struct Cyc_Yyltype* _temp7307= _temp7305.curr
+ yylsp_offset; if( _temp7305.base == 0? 1:( _temp7307 < _temp7305.base? 1:
_temp7307 >= _temp7305.last_plus_one)){ _throw( Null_Exception);}* _temp7307;}).first_line,({
struct _tagged_ptr3 _temp7308= yyls; struct Cyc_Yyltype* _temp7310= _temp7308.curr
+ yylsp_offset; if( _temp7308.base == 0? 1:( _temp7310 < _temp7308.base? 1:
_temp7310 >= _temp7308.last_plus_one)){ _throw( Null_Exception);}* _temp7310;}).last_line));
_temp7298;}); _temp7297;}); break; case 386: _LL7296: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7312=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7312[ 0]=({ struct Cyc_Exp_tok_struct
_temp7313; _temp7313.tag= Cyc_Exp_tok; _temp7313.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp7314= yyvs; void** _temp7316= _temp7314.curr +
yyvsp_offset; if( _temp7314.base == 0? 1:( _temp7316 < _temp7314.base? 1:
_temp7316 >= _temp7314.last_plus_one)){ _throw( Null_Exception);}* _temp7316;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7317= yyls; struct Cyc_Yyltype*
_temp7319= _temp7317.curr + yylsp_offset; if( _temp7317.base == 0? 1:( _temp7319
< _temp7317.base? 1: _temp7319 >= _temp7317.last_plus_one)){ _throw(
Null_Exception);}* _temp7319;}).first_line,({ struct _tagged_ptr3 _temp7320=
yyls; struct Cyc_Yyltype* _temp7322= _temp7320.curr + yylsp_offset; if(
_temp7320.base == 0? 1:( _temp7322 < _temp7320.base? 1: _temp7322 >= _temp7320.last_plus_one)){
_throw( Null_Exception);}* _temp7322;}).last_line)); _temp7313;}); _temp7312;});
break; case 387: _LL7311: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7324=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7324[ 0]=({ struct Cyc_Exp_tok_struct _temp7325; _temp7325.tag= Cyc_Exp_tok;
_temp7325.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7326= yyvs; void** _temp7328= _temp7326.curr + yyvsp_offset; if( _temp7326.base
== 0? 1:( _temp7328 < _temp7326.base? 1: _temp7328 >= _temp7326.last_plus_one)){
_throw( Null_Exception);}* _temp7328;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7329= yyls; struct Cyc_Yyltype* _temp7331= _temp7329.curr +
yylsp_offset; if( _temp7329.base == 0? 1:( _temp7331 < _temp7329.base? 1:
_temp7331 >= _temp7329.last_plus_one)){ _throw( Null_Exception);}* _temp7331;}).first_line,({
struct _tagged_ptr3 _temp7332= yyls; struct Cyc_Yyltype* _temp7334= _temp7332.curr
+ yylsp_offset; if( _temp7332.base == 0? 1:( _temp7334 < _temp7332.base? 1:
_temp7334 >= _temp7332.last_plus_one)){ _throw( Null_Exception);}* _temp7334;}).last_line));
_temp7325;}); _temp7324;}); break; case 388: _LL7323: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7336=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7336[ 0]=({ struct Cyc_Exp_tok_struct
_temp7337; _temp7337.tag= Cyc_Exp_tok; _temp7337.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7338= yyls; struct Cyc_Yyltype* _temp7340= _temp7338.curr
+ yylsp_offset; if( _temp7338.base == 0? 1:( _temp7340 < _temp7338.base? 1:
_temp7340 >= _temp7338.last_plus_one)){ _throw( Null_Exception);}* _temp7340;}).first_line,({
struct _tagged_ptr3 _temp7341= yyls; struct Cyc_Yyltype* _temp7343= _temp7341.curr
+ yylsp_offset; if( _temp7341.base == 0? 1:( _temp7343 < _temp7341.base? 1:
_temp7343 >= _temp7341.last_plus_one)){ _throw( Null_Exception);}* _temp7343;}).last_line));
_temp7337;}); _temp7336;}); break; case 389: _LL7335: yyval=( void*)({ struct
Cyc_QualId_tok_struct* _temp7345=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp7345[ 0]=({ struct Cyc_QualId_tok_struct
_temp7346; _temp7346.tag= Cyc_QualId_tok; _temp7346.f1=({ struct _tuple1*
_temp7347=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp7347->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp7352=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp7352[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp7353; _temp7353.tag= Cyc_Absyn_Rel_n; _temp7353.f1= 0; _temp7353;});
_temp7352;}); _temp7347->f2=({ struct _tagged_string* _temp7348=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7348[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7349= yyvs; void** _temp7351=
_temp7349.curr + yyvsp_offset; if( _temp7349.base == 0? 1:( _temp7351 <
_temp7349.base? 1: _temp7351 >= _temp7349.last_plus_one)){ _throw(
Null_Exception);}* _temp7351;})); _temp7348;}); _temp7347;}); _temp7346;});
_temp7345;}); break; case 390: _LL7344: yyval=({ struct _tagged_ptr2 _temp7355=
yyvs; void** _temp7357= _temp7355.curr + yyvsp_offset; if( _temp7355.base == 0?
1:( _temp7357 < _temp7355.base? 1: _temp7357 >= _temp7355.last_plus_one)){
_throw( Null_Exception);}* _temp7357;}); break; default: _LL7354:( void) _throw((
void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen; yyssp_offset -= yylen;
yylsp_offset -= yylen;({ struct _tagged_ptr2 _temp7359= yyvs; void** _temp7361=
_temp7359.curr +( ++ yyvsp_offset); if( _temp7359.base == 0? 1:( _temp7361 <
_temp7359.base? 1: _temp7361 >= _temp7359.last_plus_one)){ _throw(
Null_Exception);}* _temp7361= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp7362= yyls; struct Cyc_Yyltype* _temp7364= _temp7362.curr
+ yylsp_offset; if( _temp7362.base == 0? 1:( _temp7364 < _temp7362.base? 1:
_temp7364 >= _temp7362.last_plus_one)){ _throw( Null_Exception);}(* _temp7364).first_line=
Cyc_yylloc.first_line;});({ struct _tagged_ptr3 _temp7365= yyls; struct Cyc_Yyltype*
_temp7367= _temp7365.curr + yylsp_offset; if( _temp7365.base == 0? 1:( _temp7367
< _temp7365.base? 1: _temp7367 >= _temp7365.last_plus_one)){ _throw(
Null_Exception);}(* _temp7367).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp7368= yyls; struct Cyc_Yyltype* _temp7370= _temp7368.curr
+ yylsp_offset; if( _temp7368.base == 0? 1:( _temp7370 < _temp7368.base? 1:
_temp7370 >= _temp7368.last_plus_one)){ _throw( Null_Exception);}(* _temp7370).last_line=({
struct _tagged_ptr3 _temp7371= yyls; struct Cyc_Yyltype* _temp7373= _temp7371.curr
+( yylsp_offset - 1); if( _temp7371.base == 0? 1:( _temp7373 < _temp7371.base? 1:
_temp7373 >= _temp7371.last_plus_one)){ _throw( Null_Exception);}* _temp7373;}).last_line;});({
struct _tagged_ptr3 _temp7374= yyls; struct Cyc_Yyltype* _temp7376= _temp7374.curr
+ yylsp_offset; if( _temp7374.base == 0? 1:( _temp7376 < _temp7374.base? 1:
_temp7376 >= _temp7374.last_plus_one)){ _throw( Null_Exception);}(* _temp7376).last_column=({
struct _tagged_ptr3 _temp7377= yyls; struct Cyc_Yyltype* _temp7379= _temp7377.curr
+( yylsp_offset - 1); if( _temp7377.base == 0? 1:( _temp7379 < _temp7377.base? 1:
_temp7379 >= _temp7377.last_plus_one)){ _throw( Null_Exception);}* _temp7379;}).last_column;});({
struct _tagged_ptr3 _temp7380= yyls; struct Cyc_Yyltype* _temp7382= _temp7380.curr
+ yylsp_offset; if( _temp7380.base == 0? 1:( _temp7382 < _temp7380.base? 1:
_temp7382 >= _temp7380.last_plus_one)){ _throw( Null_Exception);}(* _temp7382).text=(
struct _tagged_string)({ char* _temp7383=( char*)""; struct _tagged_string
_temp7384; _temp7384.curr= _temp7383; _temp7384.base= _temp7383; _temp7384.last_plus_one=
_temp7383 + 1; _temp7384;});});} else{({ struct _tagged_ptr3 _temp7385= yyls;
struct Cyc_Yyltype* _temp7387= _temp7385.curr + yylsp_offset; if( _temp7385.base
== 0? 1:( _temp7387 < _temp7385.base? 1: _temp7387 >= _temp7385.last_plus_one)){
_throw( Null_Exception);}(* _temp7387).last_line=({ struct _tagged_ptr3
_temp7388= yyls; struct Cyc_Yyltype* _temp7390= _temp7388.curr +(( yylsp_offset
+ yylen) - 1); if( _temp7388.base == 0? 1:( _temp7390 < _temp7388.base? 1:
_temp7390 >= _temp7388.last_plus_one)){ _throw( Null_Exception);}* _temp7390;}).last_line;});({
struct _tagged_ptr3 _temp7391= yyls; struct Cyc_Yyltype* _temp7393= _temp7391.curr
+ yylsp_offset; if( _temp7391.base == 0? 1:( _temp7393 < _temp7391.base? 1:
_temp7393 >= _temp7391.last_plus_one)){ _throw( Null_Exception);}(* _temp7393).last_column=({
struct _tagged_ptr3 _temp7394= yyls; struct Cyc_Yyltype* _temp7396= _temp7394.curr
+(( yylsp_offset + yylen) - 1); if( _temp7394.base == 0? 1:( _temp7396 <
_temp7394.base? 1: _temp7396 >= _temp7394.last_plus_one)){ _throw(
Null_Exception);}* _temp7396;}).last_column;});} yyn=( int)({ short* _temp7397=(
short*) Cyc_yyr1; unsigned int _temp7398= yyn; if( _temp7398 >= 391u){ _throw(
Null_Exception);} _temp7397[ _temp7398];}); yystate=({ short* _temp7399=( short*)
Cyc_yypgoto; unsigned int _temp7400= yyn - 124; if( _temp7400 >= 107u){ _throw(
Null_Exception);} _temp7399[ _temp7400];}) +({ struct _tagged_ptr1 _temp7401=
yyss; short* _temp7403= _temp7401.curr + yyssp_offset; if( _temp7401.base == 0?
1:( _temp7403 < _temp7401.base? 1: _temp7403 >= _temp7401.last_plus_one)){
_throw( Null_Exception);}* _temp7403;}); if(( yystate >= 0? yystate <= 4318: 0)?({
short* _temp7404=( short*) Cyc_yycheck; unsigned int _temp7405= yystate; if(
_temp7405 >= 4319u){ _throw( Null_Exception);} _temp7404[ _temp7405];}) ==({
struct _tagged_ptr1 _temp7406= yyss; short* _temp7408= _temp7406.curr +
yyssp_offset; if( _temp7406.base == 0? 1:( _temp7408 < _temp7406.base? 1:
_temp7408 >= _temp7406.last_plus_one)){ _throw( Null_Exception);}* _temp7408;}):
0){ yystate=( int)({ short* _temp7409=( short*) Cyc_yytable; unsigned int
_temp7410= yystate; if( _temp7410 >= 4319u){ _throw( Null_Exception);} _temp7409[
_temp7410];});} else{ yystate=( int)({ short* _temp7411=( short*) Cyc_yydefgoto;
unsigned int _temp7412= yyn - 124; if( _temp7412 >= 107u){ _throw(
Null_Exception);} _temp7411[ _temp7412];});} goto yynewstate; yyerrlab: if(
yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short* _temp7413=( short*) Cyc_yypact;
unsigned int _temp7414= yystate; if( _temp7414 >= 748u){ _throw( Null_Exception);}
_temp7413[ _temp7414];}); if( yyn > - 32768? yyn < 4318: 0){ int sze= 0; struct
_tagged_string msg; int x; int count; count= 0; for( x= yyn < 0? - yyn: 0; x <
231u / sizeof( char*); x ++){ if(({ short* _temp7415=( short*) Cyc_yycheck;
unsigned int _temp7416= x + yyn; if( _temp7416 >= 4319u){ _throw( Null_Exception);}
_temp7415[ _temp7416];}) == x){ sze += Cyc_String_strlen(({ struct
_tagged_string* _temp7417=( struct _tagged_string*) Cyc_yytname; unsigned int
_temp7418= x; if( _temp7418 >= 231u){ _throw( Null_Exception);} _temp7417[
_temp7418];})) + 15, count ++;}} msg=({ unsigned int _temp7419=( unsigned int)(
sze + 15); char* _temp7420=( char*) GC_malloc_atomic( sizeof( char) * _temp7419);
struct _tagged_string _temp7423={ _temp7420, _temp7420, _temp7420 + _temp7419};{
unsigned int _temp7421= _temp7419; unsigned int i; for( i= 0; i < _temp7421; i
++){ _temp7420[ i]='\000';}}; _temp7423;}); Cyc_String_strcpy( msg,( struct
_tagged_string)({ char* _temp7424=( char*)"parse error"; struct _tagged_string
_temp7425; _temp7425.curr= _temp7424; _temp7425.base= _temp7424; _temp7425.last_plus_one=
_temp7424 + 12; _temp7425;})); if( count < 5){ count= 0; for( x= yyn < 0? - yyn:
0; x < 231u / sizeof( char*); x ++){ if(({ short* _temp7426=( short*) Cyc_yycheck;
unsigned int _temp7427= x + yyn; if( _temp7427 >= 4319u){ _throw( Null_Exception);}
_temp7426[ _temp7427];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp7428=( char*)", expecting `"; struct _tagged_string
_temp7429; _temp7429.curr= _temp7428; _temp7429.base= _temp7428; _temp7429.last_plus_one=
_temp7428 + 14; _temp7429;}):( struct _tagged_string)({ char* _temp7430=( char*)" or `";
struct _tagged_string _temp7431; _temp7431.curr= _temp7430; _temp7431.base=
_temp7430; _temp7431.last_plus_one= _temp7430 + 6; _temp7431;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp7432=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp7433= x; if( _temp7433 >= 231u){ _throw( Null_Exception);}
_temp7432[ _temp7433];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp7434=( char*)"'"; struct _tagged_string _temp7435; _temp7435.curr=
_temp7434; _temp7435.base= _temp7434; _temp7435.last_plus_one= _temp7434 + 2;
_temp7435;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp7436=( char*)"parse error"; struct _tagged_string
_temp7437; _temp7437.curr= _temp7436; _temp7437.base= _temp7436; _temp7437.last_plus_one=
_temp7436 + 12; _temp7437;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp7438= yyss; short*
_temp7440= _temp7438.curr +( -- yyssp_offset); if( _temp7438.base == 0? 1:(
_temp7440 < _temp7438.base? 1: _temp7440 >= _temp7438.last_plus_one)){ _throw(
Null_Exception);}* _temp7440;}); yylsp_offset --; yyerrhandle: yyn=( int)({
short* _temp7441=( short*) Cyc_yypact; unsigned int _temp7442= yystate; if(
_temp7442 >= 748u){ _throw( Null_Exception);} _temp7441[ _temp7442];}); if( yyn
== - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4318)? 1:({
short* _temp7443=( short*) Cyc_yycheck; unsigned int _temp7444= yyn; if(
_temp7444 >= 4319u){ _throw( Null_Exception);} _temp7443[ _temp7444];}) != 1){
goto yyerrdefault;} yyn=( int)({ short* _temp7445=( short*) Cyc_yytable;
unsigned int _temp7446= yyn; if( _temp7446 >= 4319u){ _throw( Null_Exception);}
_temp7445[ _temp7446];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 747){
return 0;}({ struct _tagged_ptr2 _temp7447= yyvs; void** _temp7449= _temp7447.curr
+( ++ yyvsp_offset); if( _temp7447.base == 0? 1:( _temp7449 < _temp7447.base? 1:
_temp7449 >= _temp7447.last_plus_one)){ _throw( Null_Exception);}* _temp7449=
Cyc_yylval;});({ struct _tagged_ptr3 _temp7450= yyls; struct Cyc_Yyltype*
_temp7452= _temp7450.curr +( ++ yylsp_offset); if( _temp7450.base == 0? 1:(
_temp7452 < _temp7450.base? 1: _temp7452 >= _temp7450.last_plus_one)){ _throw(
Null_Exception);}* _temp7452= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, void* v){ void* _temp7471= v; struct _tuple12* _temp7491; struct _tuple12
_temp7493; int _temp7494; void* _temp7496; char _temp7498; short _temp7500;
struct _tagged_string _temp7502; struct Cyc_Core_Opt* _temp7504; struct Cyc_Core_Opt*
_temp7506; struct Cyc_Core_Opt _temp7508; struct _tagged_string* _temp7509;
struct _tuple1* _temp7511; struct _tuple1 _temp7513; struct _tagged_string*
_temp7514; void* _temp7516; _LL7473: if( _temp7471 == Cyc_Okay_tok){ goto
_LL7474;} else{ goto _LL7475;} _LL7475: if(*(( void**) _temp7471) == Cyc_Int_tok){
_LL7492: _temp7491=( struct _tuple12*)(( struct Cyc_Int_tok_struct*) _temp7471)->f1;
_temp7493=* _temp7491; _LL7497: _temp7496= _temp7493.f1; goto _LL7495; _LL7495:
_temp7494= _temp7493.f2; goto _LL7476;} else{ goto _LL7477;} _LL7477: if(*((
void**) _temp7471) == Cyc_Char_tok){ _LL7499: _temp7498=( char)(( struct Cyc_Char_tok_struct*)
_temp7471)->f1; goto _LL7478;} else{ goto _LL7479;} _LL7479: if(*(( void**)
_temp7471) == Cyc_Short_tok){ _LL7501: _temp7500=( short)(( struct Cyc_Short_tok_struct*)
_temp7471)->f1; goto _LL7480;} else{ goto _LL7481;} _LL7481: if(*(( void**)
_temp7471) == Cyc_String_tok){ _LL7503: _temp7502=( struct _tagged_string)((
struct Cyc_String_tok_struct*) _temp7471)->f1; goto _LL7482;} else{ goto _LL7483;}
_LL7483: if(*(( void**) _temp7471) == Cyc_Stringopt_tok){ _LL7505: _temp7504=(
struct Cyc_Core_Opt*)(( struct Cyc_Stringopt_tok_struct*) _temp7471)->f1; if(
_temp7504 == 0){ goto _LL7484;} else{ goto _LL7485;}} else{ goto _LL7485;}
_LL7485: if(*(( void**) _temp7471) == Cyc_Stringopt_tok){ _LL7507: _temp7506=(
struct Cyc_Core_Opt*)(( struct Cyc_Stringopt_tok_struct*) _temp7471)->f1; if(
_temp7506 == 0){ goto _LL7487;} else{ _temp7508=* _temp7506; _LL7510: _temp7509=(
struct _tagged_string*) _temp7508.v; goto _LL7486;}} else{ goto _LL7487;}
_LL7487: if(*(( void**) _temp7471) == Cyc_QualId_tok){ _LL7512: _temp7511=(
struct _tuple1*)(( struct Cyc_QualId_tok_struct*) _temp7471)->f1; _temp7513=*
_temp7511; _LL7517: _temp7516= _temp7513.f1; goto _LL7515; _LL7515: _temp7514=
_temp7513.f2; goto _LL7488;} else{ goto _LL7489;} _LL7489: goto _LL7490; _LL7474:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL7472; _LL7476: fprintf( Cyc_Stdio_stderr,"%d",
_temp7494); goto _LL7472; _LL7478: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp7498); goto _LL7472; _LL7480: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp7500); goto _LL7472; _LL7482:({ struct _tagged_string _temp7518= _temp7502;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp7518.last_plus_one - _temp7518.curr,
_temp7518.curr);}); goto _LL7472; _LL7484: fprintf( Cyc_Stdio_stderr,"null");
goto _LL7472; _LL7486:({ struct _tagged_string _temp7519=* _temp7509; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp7519.last_plus_one - _temp7519.curr, _temp7519.curr);});
goto _LL7472; _LL7488: { struct Cyc_List_List* prefix= 0;{ void* _temp7520=
_temp7516; struct Cyc_List_List* _temp7528; struct Cyc_List_List* _temp7530;
_LL7522: if(( unsigned int) _temp7520 > 1u?*(( int*) _temp7520) == Cyc_Absyn_Rel_n:
0){ _LL7529: _temp7528=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp7520)->f1; goto _LL7523;} else{ goto _LL7524;} _LL7524: if(( unsigned int)
_temp7520 > 1u?*(( int*) _temp7520) == Cyc_Absyn_Abs_n: 0){ _LL7531: _temp7530=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp7520)->f1; goto
_LL7525;} else{ goto _LL7526;} _LL7526: if( _temp7520 ==( void*) Cyc_Absyn_Loc_n){
goto _LL7527;} else{ goto _LL7521;} _LL7523: prefix= _temp7528; goto _LL7521;
_LL7525: prefix= _temp7530; goto _LL7521; _LL7527: goto _LL7521; _LL7521:;} for(
0; prefix != 0; prefix=({ struct Cyc_List_List* _temp7532= prefix; if( _temp7532
== 0){ _throw( Null_Exception);} _temp7532->tl;})){({ struct _tagged_string
_temp7534=*(( struct _tagged_string*)({ struct Cyc_List_List* _temp7533= prefix;
if( _temp7533 == 0){ _throw( Null_Exception);} _temp7533->hd;})); fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7534.last_plus_one - _temp7534.curr, _temp7534.curr);});}({ struct
_tagged_string _temp7535=* _temp7514; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7535.last_plus_one - _temp7535.curr, _temp7535.curr);}); goto _LL7472;}
_LL7490: fprintf( Cyc_Stdio_stderr,"?"); goto _LL7472; _LL7472:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp7536=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp7536->v=( void*) Cyc_Lexing_from_file(
f); _temp7536;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}