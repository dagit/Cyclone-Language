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
extern char Cyc_Stdio_FileCloseError[ 19u]; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{ char*
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
_temp12.last_plus_one= _temp11 + 32; _temp12;}), loc);}{ struct _tuple1* _temp13=(*
_temp9).f1; goto _LL14; _LL14:{ void* _temp15=(* _temp13).f1; struct Cyc_List_List*
_temp25; struct Cyc_List_List* _temp27; _LL17: if(( unsigned int) _temp15 > 1u?*((
int*) _temp15) == Cyc_Absyn_Rel_n: 0){ _LL26: _temp25=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp15)->f1; if( _temp25 == 0){ goto _LL18;}
else{ goto _LL19;}} else{ goto _LL19;} _LL19: if(( unsigned int) _temp15 > 1u?*((
int*) _temp15) == Cyc_Absyn_Abs_n: 0){ _LL28: _temp27=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp15)->f1; if( _temp27 == 0){ goto _LL20;}
else{ goto _LL21;}} else{ goto _LL21;} _LL21: if( _temp15 ==( void*) Cyc_Absyn_Loc_n){
goto _LL22;} else{ goto _LL23;} _LL23: goto _LL24; _LL18: goto _LL16; _LL20:
goto _LL16; _LL22: goto _LL16; _LL24: Cyc_Parse_err(( struct _tagged_string)({
char* _temp29=( char*)"struct field cannot be qualified with a module name";
struct _tagged_string _temp30; _temp30.curr= _temp29; _temp30.base= _temp29;
_temp30.last_plus_one= _temp29 + 52; _temp30;}), loc); goto _LL16; _LL16:;}{
struct Cyc_List_List* _temp31=(* _temp9).f5; goto _LL32; _LL32: return({ struct
Cyc_Absyn_Structfield* _temp33=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp33->name=(* _temp13).f2; _temp33->tq=(*
_temp9).f2; _temp33->type=( void*)(* _temp9).f3; _temp33->width= _temp7; _temp33->attributes=
_temp31; _temp33;});}}} struct _tuple9{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual
f2; void* f3; struct Cyc_List_List* f4; } ; static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple9* field){ struct _tuple9 _temp36;
struct Cyc_List_List* _temp37; void* _temp39; struct Cyc_Absyn_Tqual _temp41;
struct Cyc_Core_Opt* _temp43; struct _tuple9* _temp34= field; _temp36=* _temp34;
_LL44: _temp43= _temp36.f1; goto _LL42; _LL42: _temp41= _temp36.f2; goto _LL40;
_LL40: _temp39= _temp36.f3; goto _LL38; _LL38: _temp37= _temp36.f4; goto _LL35;
_LL35: { struct Cyc_Core_Opt* _temp45= 0; goto _LL46; _LL46: if( _temp43 != 0){
_temp45=({ struct Cyc_Core_Opt* _temp47=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp47->v=( void*)(*(( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp48= _temp43; if( _temp48 == 0){ _throw( Null_Exception);}
_temp48->v;}))).f2; _temp47;});{ void* _temp50=(*(( struct _tuple1*)({ struct
Cyc_Core_Opt* _temp49= _temp43; if( _temp49 == 0){ _throw( Null_Exception);}
_temp49->v;}))).f1; struct Cyc_List_List* _temp60; struct Cyc_List_List* _temp62;
_LL52: if(( unsigned int) _temp50 > 1u?*(( int*) _temp50) == Cyc_Absyn_Rel_n: 0){
_LL61: _temp60=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp50)->f1; if( _temp60 == 0){ goto _LL53;} else{ goto _LL54;}} else{ goto
_LL54;} _LL54: if(( unsigned int) _temp50 > 1u?*(( int*) _temp50) == Cyc_Absyn_Abs_n:
0){ _LL63: _temp62=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp50)->f1; if( _temp62 == 0){ goto _LL55;} else{ goto _LL56;}} else{ goto
_LL56;} _LL56: if( _temp50 ==( void*) Cyc_Absyn_Loc_n){ goto _LL57;} else{ goto
_LL58;} _LL58: goto _LL59; _LL53: goto _LL51; _LL55: goto _LL51; _LL57: goto
_LL51; _LL59: Cyc_Parse_err(( struct _tagged_string)({ char* _temp64=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp65; _temp65.curr= _temp64; _temp65.base= _temp64;
_temp65.last_plus_one= _temp64 + 49; _temp65;}), loc); goto _LL51; _LL51:;}} if(
_temp37 != 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp66=( char*)"parameter should have no type parameters";
struct _tagged_string _temp67; _temp67.curr= _temp66; _temp67.base= _temp66;
_temp67.last_plus_one= _temp66 + 41; _temp67;}), loc);} return({ struct _tuple2*
_temp68=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp68->f1=
_temp45; _temp68->f2= _temp41; _temp68->f3= _temp39; _temp68;});}} static void*
Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Parse_Type_spec_struct* _temp69=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp69[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp70; _temp70.tag= Cyc_Parse_Type_spec; _temp70.f1=(
void*) t; _temp70.f2= loc; _temp70;}); _temp69;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp71= t; struct Cyc_Absyn_Exp* _temp77; struct Cyc_Absyn_Tqual
_temp79; void* _temp81; _LL73: if(( unsigned int) _temp71 > 4u?*(( int*) _temp71)
== Cyc_Absyn_ArrayType: 0){ _LL82: _temp81=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f1; goto _LL80; _LL80: _temp79=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f2; goto _LL78; _LL78: _temp77=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp71)->f3; goto _LL74;} else{ goto _LL75;} _LL75: goto _LL76; _LL74: return
Cyc_Absyn_starb_typ( _temp81,( void*) Cyc_Absyn_HeapRgn, _temp79, _temp77 == 0?(
void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp83=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp83[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp84; _temp84.tag= Cyc_Absyn_Upper_b;
_temp84.f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp85= _temp77;
if( _temp85 == 0){ _throw( Null_Exception);} _temp85;}); _temp84;}); _temp83;}));
_LL76: return t; _LL72:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple10{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct _tuple10* Cyc_Parse_get_tqual_typ( struct Cyc_Position_Segment*
loc, struct _tuple2* t){ return({ struct _tuple10* _temp86=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp86->f1=(* t).f2; _temp86->f2=(* t).f3;
_temp86;});} static void Cyc_Parse_only_vardecl( struct Cyc_List_List* params,
struct Cyc_Absyn_Decl* x){ struct _tagged_string decl_kind;{ void* _temp87=(
void*) x->r; struct Cyc_Absyn_Vardecl* _temp111; int _temp113; struct Cyc_Absyn_Exp*
_temp115; struct Cyc_Core_Opt* _temp117; struct Cyc_Core_Opt* _temp119; struct
Cyc_Absyn_Pat* _temp121; struct Cyc_Absyn_Fndecl* _temp123; struct Cyc_Absyn_Structdecl*
_temp125; struct Cyc_Absyn_Uniondecl* _temp127; struct Cyc_Absyn_Tuniondecl*
_temp129; struct Cyc_Absyn_Typedefdecl* _temp131; struct Cyc_Absyn_Enumdecl*
_temp133; struct Cyc_List_List* _temp135; struct _tagged_string* _temp137;
struct Cyc_List_List* _temp139; struct _tuple1* _temp141; struct Cyc_List_List*
_temp143; _LL89: if(*(( int*) _temp87) == Cyc_Absyn_Var_d){ _LL112: _temp111=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp87)->f1; goto
_LL90;} else{ goto _LL91;} _LL91: if(*(( int*) _temp87) == Cyc_Absyn_Let_d){
_LL122: _temp121=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp87)->f1; goto _LL120; _LL120: _temp119=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp87)->f2; goto _LL118; _LL118: _temp117=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp87)->f3; goto _LL116; _LL116: _temp115=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Let_d_struct*) _temp87)->f4; goto _LL114; _LL114: _temp113=( int)((
struct Cyc_Absyn_Let_d_struct*) _temp87)->f5; goto _LL92;} else{ goto _LL93;}
_LL93: if(*(( int*) _temp87) == Cyc_Absyn_Fn_d){ _LL124: _temp123=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp87)->f1; goto _LL94;} else{ goto _LL95;}
_LL95: if(*(( int*) _temp87) == Cyc_Absyn_Struct_d){ _LL126: _temp125=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp87)->f1; goto
_LL96;} else{ goto _LL97;} _LL97: if(*(( int*) _temp87) == Cyc_Absyn_Union_d){
_LL128: _temp127=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp87)->f1; goto _LL98;} else{ goto _LL99;} _LL99: if(*(( int*) _temp87) ==
Cyc_Absyn_Tunion_d){ _LL130: _temp129=( struct Cyc_Absyn_Tuniondecl*)(( struct
Cyc_Absyn_Tunion_d_struct*) _temp87)->f1; goto _LL100;} else{ goto _LL101;}
_LL101: if(*(( int*) _temp87) == Cyc_Absyn_Typedef_d){ _LL132: _temp131=( struct
Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp87)->f1; goto
_LL102;} else{ goto _LL103;} _LL103: if(*(( int*) _temp87) == Cyc_Absyn_Enum_d){
_LL134: _temp133=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp87)->f1; goto _LL104;} else{ goto _LL105;} _LL105: if(*(( int*) _temp87) ==
Cyc_Absyn_Namespace_d){ _LL138: _temp137=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp87)->f1; goto _LL136; _LL136: _temp135=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp87)->f2; goto _LL106;} else{ goto _LL107;}
_LL107: if(*(( int*) _temp87) == Cyc_Absyn_Using_d){ _LL142: _temp141=( struct
_tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp87)->f1; goto _LL140; _LL140:
_temp139=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp87)->f2;
goto _LL108;} else{ goto _LL109;} _LL109: if(*(( int*) _temp87) == Cyc_Absyn_ExternC_d){
_LL144: _temp143=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp87)->f1; goto _LL110;} else{ goto _LL88;} _LL90: if( _temp111->initializer
!= 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg))
Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp145=( char*)"initializers are not allowed in parameter declarations";
struct _tagged_string _temp146; _temp146.curr= _temp145; _temp146.base= _temp145;
_temp146.last_plus_one= _temp145 + 55; _temp146;}), x->loc);}{ void* _temp147=(*
_temp111->name).f1; struct Cyc_List_List* _temp157; struct Cyc_List_List*
_temp159; _LL149: if( _temp147 ==( void*) Cyc_Absyn_Loc_n){ goto _LL150;} else{
goto _LL151;} _LL151: if(( unsigned int) _temp147 > 1u?*(( int*) _temp147) ==
Cyc_Absyn_Rel_n: 0){ _LL158: _temp157=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp147)->f1; if( _temp157 == 0){ goto _LL152;} else{ goto _LL153;}} else{ goto
_LL153;} _LL153: if(( unsigned int) _temp147 > 1u?*(( int*) _temp147) == Cyc_Absyn_Abs_n:
0){ _LL160: _temp159=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp147)->f1; if( _temp159 == 0){ goto _LL154;} else{ goto _LL155;}} else{ goto
_LL155;} _LL155: goto _LL156; _LL150: goto _LL148; _LL152: goto _LL148; _LL154:
goto _LL148; _LL156: Cyc_Parse_err(( struct _tagged_string)({ char* _temp161=(
char*)"module names not allowed on parameter declarations"; struct
_tagged_string _temp162; _temp162.curr= _temp161; _temp162.base= _temp161;
_temp162.last_plus_one= _temp161 + 51; _temp162;}), x->loc); goto _LL148; _LL148:;}{
int found= 0; for( 0; params != 0; params=({ struct Cyc_List_List* _temp163=
params; if( _temp163 == 0){ _throw( Null_Exception);} _temp163->tl;})){ if( Cyc_String_zstrptrcmp((*
_temp111->name).f2,( struct _tagged_string*)({ struct Cyc_List_List* _temp164=
params; if( _temp164 == 0){ _throw( Null_Exception);} _temp164->hd;})) == 0){
found= 1; break;}} if( ! found){(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp165=*(*
_temp111->name).f2; xprintf("%.*s is not listed as a parameter", _temp165.last_plus_one
- _temp165.curr, _temp165.curr);}), x->loc);} return;} _LL92: decl_kind=( struct
_tagged_string)({ char* _temp166=( char*)"let declaration"; struct
_tagged_string _temp167; _temp167.curr= _temp166; _temp167.base= _temp166;
_temp167.last_plus_one= _temp166 + 16; _temp167;}); goto _LL88; _LL94: decl_kind=(
struct _tagged_string)({ char* _temp168=( char*)"function declaration"; struct
_tagged_string _temp169; _temp169.curr= _temp168; _temp169.base= _temp168;
_temp169.last_plus_one= _temp168 + 21; _temp169;}); goto _LL88; _LL96: decl_kind=(
struct _tagged_string)({ char* _temp170=( char*)"struct declaration"; struct
_tagged_string _temp171; _temp171.curr= _temp170; _temp171.base= _temp170;
_temp171.last_plus_one= _temp170 + 19; _temp171;}); goto _LL88; _LL98: decl_kind=(
struct _tagged_string)({ char* _temp172=( char*)"union declaration"; struct
_tagged_string _temp173; _temp173.curr= _temp172; _temp173.base= _temp172;
_temp173.last_plus_one= _temp172 + 18; _temp173;}); goto _LL88; _LL100:
decl_kind=( struct _tagged_string)({ char* _temp174=( char*)"tunion declaration";
struct _tagged_string _temp175; _temp175.curr= _temp174; _temp175.base= _temp174;
_temp175.last_plus_one= _temp174 + 19; _temp175;}); goto _LL88; _LL102:
decl_kind=( struct _tagged_string)({ char* _temp176=( char*)"typedef"; struct
_tagged_string _temp177; _temp177.curr= _temp176; _temp177.base= _temp176;
_temp177.last_plus_one= _temp176 + 8; _temp177;}); goto _LL88; _LL104: decl_kind=(
struct _tagged_string)({ char* _temp178=( char*)"enum declaration"; struct
_tagged_string _temp179; _temp179.curr= _temp178; _temp179.base= _temp178;
_temp179.last_plus_one= _temp178 + 17; _temp179;}); goto _LL88; _LL106:
decl_kind=( struct _tagged_string)({ char* _temp180=( char*)"namespace declaration";
struct _tagged_string _temp181; _temp181.curr= _temp180; _temp181.base= _temp180;
_temp181.last_plus_one= _temp180 + 22; _temp181;}); goto _LL88; _LL108:
decl_kind=( struct _tagged_string)({ char* _temp182=( char*)"using declaration";
struct _tagged_string _temp183; _temp183.curr= _temp182; _temp183.base= _temp182;
_temp183.last_plus_one= _temp182 + 18; _temp183;}); goto _LL88; _LL110:
decl_kind=( struct _tagged_string)({ char* _temp184=( char*)"extern C declaration";
struct _tagged_string _temp185; _temp185.curr= _temp184; _temp185.base= _temp184;
_temp185.last_plus_one= _temp184 + 21; _temp185;}); goto _LL88; _LL88:;}(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp186= decl_kind; xprintf("%.*s appears in parameter type",
_temp186.last_plus_one - _temp186.curr, _temp186.curr);}), x->loc); return;}
struct _tuple11{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ;
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple11* env, struct
_tagged_string* x){ struct _tuple11 _temp189; struct Cyc_Position_Segment*
_temp190; struct Cyc_List_List* _temp192; struct _tuple11* _temp187= env;
_temp189=* _temp187; _LL193: _temp192= _temp189.f1; goto _LL191; _LL191:
_temp190= _temp189.f2; goto _LL188; _LL188: if( _temp192 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp194=* x; xprintf("missing type for parameter %.*s",
_temp194.last_plus_one - _temp194.curr, _temp194.curr);}), _temp190);}{ void*
_temp196=( void*)(( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List* _temp195=
_temp192; if( _temp195 == 0){ _throw( Null_Exception);} _temp195->hd;}))->r;
struct Cyc_Absyn_Vardecl* _temp202; _LL198: if(*(( int*) _temp196) == Cyc_Absyn_Var_d){
_LL203: _temp202=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp196)->f1; goto _LL199;} else{ goto _LL200;} _LL200: goto _LL201; _LL199:{
void* _temp204=(* _temp202->name).f1; struct Cyc_List_List* _temp214; struct Cyc_List_List*
_temp216; _LL206: if( _temp204 ==( void*) Cyc_Absyn_Loc_n){ goto _LL207;} else{
goto _LL208;} _LL208: if(( unsigned int) _temp204 > 1u?*(( int*) _temp204) ==
Cyc_Absyn_Rel_n: 0){ _LL215: _temp214=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp204)->f1; if( _temp214 == 0){ goto _LL209;} else{ goto _LL210;}} else{ goto
_LL210;} _LL210: if(( unsigned int) _temp204 > 1u?*(( int*) _temp204) == Cyc_Absyn_Abs_n:
0){ _LL217: _temp216=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp204)->f1; if( _temp216 == 0){ goto _LL211;} else{ goto _LL212;}} else{ goto
_LL212;} _LL212: goto _LL213; _LL207: goto _LL205; _LL209: goto _LL205; _LL211:
goto _LL205; _LL213: Cyc_Parse_err(( struct _tagged_string)({ char* _temp218=(
char*)"module name not allowed on parameter"; struct _tagged_string _temp219;
_temp219.curr= _temp218; _temp219.base= _temp218; _temp219.last_plus_one=
_temp218 + 37; _temp219;}), _temp190); goto _LL205; _LL205:;} if( Cyc_String_zstrptrcmp((*
_temp202->name).f2, x) == 0){ return({ struct _tuple2* _temp220=( struct _tuple2*)
GC_malloc( sizeof( struct _tuple2) * 1); _temp220[ 0]=({ struct _tuple2 _temp221;
_temp221.f1=({ struct Cyc_Core_Opt* _temp222=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt) * 1); _temp222[ 0]=({ struct Cyc_Core_Opt _temp223;
_temp223.v=( void*)(* _temp202->name).f2; _temp223;}); _temp222;}); _temp221.f2=
_temp202->tq; _temp221.f3=( void*) _temp202->type; _temp221;}); _temp220;});}
else{ return Cyc_Parse_get_param_type(({ struct _tuple11* _temp224=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp224->f1=({ struct Cyc_List_List*
_temp225= _temp192; if( _temp225 == 0){ _throw( Null_Exception);} _temp225->tl;});
_temp224->f2= _temp190; _temp224;}), x);} _LL201: return(( struct _tuple2*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp226=( char*)"non-variable declaration";
struct _tagged_string _temp227; _temp227.curr= _temp226; _temp227.base= _temp226;
_temp227.last_plus_one= _temp226 + 25; _temp227;}),(( struct Cyc_Absyn_Decl*)({
struct Cyc_List_List* _temp228= _temp192; if( _temp228 == 0){ _throw(
Null_Exception);} _temp228->hd;}))->loc); _LL197:;}} static int Cyc_Parse_is_typeparam(
void* tm){ void* _temp229= tm; int _temp235; struct Cyc_Position_Segment*
_temp237; struct Cyc_List_List* _temp239; _LL231: if(( unsigned int) _temp229 >
1u?*(( int*) _temp229) == Cyc_Absyn_TypeParams_mod: 0){ _LL240: _temp239=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp229)->f1;
goto _LL238; _LL238: _temp237=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp229)->f2; goto _LL236; _LL236: _temp235=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp229)->f3; goto _LL232;} else{ goto _LL233;} _LL233: goto _LL234; _LL232:
return 1; _LL234: return 0; _LL230:;} static void* Cyc_Parse_id2type( struct
_tagged_string s, struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp241=( char*)"`H"; struct _tagged_string
_temp242; _temp242.curr= _temp241; _temp242.base= _temp241; _temp242.last_plus_one=
_temp241 + 3; _temp242;})) == 0){ return( void*) Cyc_Absyn_HeapRgn;} else{
return( void*)({ struct Cyc_Absyn_VarType_struct* _temp243=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp243[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp244; _temp244.tag= Cyc_Absyn_VarType; _temp244.f1=({ struct Cyc_Absyn_Tvar*
_temp245=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp245->name=({ struct _tagged_string* _temp246=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp246[ 0]= s; _temp246;});
_temp245->kind= k; _temp245;}); _temp244;}); _temp243;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp247=
t; struct Cyc_Absyn_Tvar* _temp253; _LL249: if(( unsigned int) _temp247 > 4u?*((
int*) _temp247) == Cyc_Absyn_VarType: 0){ _LL254: _temp253=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp247)->f1; goto _LL250;} else{ goto _LL251;}
_LL251: goto _LL252; _LL250: return _temp253; _LL252: return(( struct Cyc_Absyn_Tvar*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp255=( char*)"expecting a list of type variables, not types";
struct _tagged_string _temp256; _temp256.curr= _temp255; _temp256.base= _temp255;
_temp256.last_plus_one= _temp255 + 46; _temp256;}), loc); _LL248:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp257=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp257[ 0]=({ struct Cyc_Absyn_VarType_struct _temp258; _temp258.tag= Cyc_Absyn_VarType;
_temp258.f1= pr; _temp258;}); _temp257;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp260=(
void*)({ struct Cyc_List_List* _temp259= tms; if( _temp259 == 0){ _throw(
Null_Exception);} _temp259->hd;}); void* _temp266; _LL262: if(( unsigned int)
_temp260 > 1u?*(( int*) _temp260) == Cyc_Absyn_Function_mod: 0){ _LL267:
_temp266=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp260)->f1; goto
_LL263;} else{ goto _LL264;} _LL264: goto _LL265; _LL263: if(({ struct Cyc_List_List*
_temp268= tms; if( _temp268 == 0){ _throw( Null_Exception);} _temp268->tl;}) ==
0? 1:( Cyc_Parse_is_typeparam(( void*)({ struct Cyc_List_List* _temp270=({
struct Cyc_List_List* _temp269= tms; if( _temp269 == 0){ _throw( Null_Exception);}
_temp269->tl;}); if( _temp270 == 0){ _throw( Null_Exception);} _temp270->hd;}))?({
struct Cyc_List_List* _temp272=({ struct Cyc_List_List* _temp271= tms; if(
_temp271 == 0){ _throw( Null_Exception);} _temp271->tl;}); if( _temp272 == 0){
_throw( Null_Exception);} _temp272->tl;}) == 0: 0)){ void* _temp273= _temp266;
struct Cyc_Core_Opt* _temp279; int _temp281; struct Cyc_List_List* _temp283;
struct Cyc_Position_Segment* _temp285; struct Cyc_List_List* _temp287; _LL275:
if(*(( int*) _temp273) == Cyc_Absyn_WithTypes){ _LL284: _temp283=( struct Cyc_List_List*)((
struct Cyc_Absyn_WithTypes_struct*) _temp273)->f1; goto _LL282; _LL282: _temp281=(
int)(( struct Cyc_Absyn_WithTypes_struct*) _temp273)->f2; goto _LL280; _LL280:
_temp279=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp273)->f3;
goto _LL276;} else{ goto _LL277;} _LL277: if(*(( int*) _temp273) == Cyc_Absyn_NoTypes){
_LL288: _temp287=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp273)->f1; goto _LL286; _LL286: _temp285=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp273)->f2; goto _LL278;} else{ goto _LL274;}
_LL276: Cyc_Parse_warn(( struct _tagged_string)({ char* _temp289=( char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp290; _temp290.curr= _temp289; _temp290.base= _temp289;
_temp290.last_plus_one= _temp289 + 92; _temp290;}), loc); return tms; _LL278:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp287, tds);{ struct _tuple11* _temp292=({ struct _tuple11* _temp291=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp291->f1= tds; _temp291->f2=
loc; _temp291;}); goto _LL293; _LL293: return({ struct Cyc_List_List* _temp294=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp294->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp295=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp295[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp296; _temp296.tag= Cyc_Absyn_Function_mod;
_temp296.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp297=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp297[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp298; _temp298.tag= Cyc_Absyn_WithTypes;
_temp298.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple11*,
struct _tagged_string*), struct _tuple11* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp292, _temp287); _temp298.f2= 0; _temp298.f3= 0;
_temp298;}); _temp297;})); _temp296;}); _temp295;})); _temp294->tl= 0; _temp294;});}
_LL274:;} else{ return({ struct Cyc_List_List* _temp299=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp299->hd=( void*)(( void*)({
struct Cyc_List_List* _temp301= tms; if( _temp301 == 0){ _throw( Null_Exception);}
_temp301->hd;})); _temp299->tl= Cyc_Parse_oldstyle2newstyle(({ struct Cyc_List_List*
_temp300= tms; if( _temp300 == 0){ _throw( Null_Exception);} _temp300->tl;}),
tds, loc); _temp299;});} _LL265: return({ struct Cyc_List_List* _temp302=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp302->hd=(
void*)(( void*)({ struct Cyc_List_List* _temp304= tms; if( _temp304 == 0){
_throw( Null_Exception);} _temp304->hd;})); _temp302->tl= Cyc_Parse_oldstyle2newstyle(({
struct Cyc_List_List* _temp303= tms; if( _temp303 == 0){ _throw( Null_Exception);}
_temp303->tl;}), tds, loc); _temp302;}); _LL261:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp305=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp305->id= d->id; _temp305->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp305;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
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
_temp312 == 0){ _throw( Null_Exception);} _temp312->v;}); _LL315: if( _temp313
==( void*) Cyc_Parse_Extern_sc){ goto _LL316;} else{ goto _LL317;} _LL317: if(
_temp313 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL318;} else{ goto _LL319;}
_LL319: if( _temp313 ==( void*) Cyc_Parse_Static_sc){ goto _LL320;} else{ goto
_LL321;} _LL321: goto _LL322; _LL316: sc=( void*) Cyc_Absyn_Extern; goto _LL314;
_LL318: sc=( void*) Cyc_Absyn_ExternC; goto _LL314; _LL320: sc=( void*) Cyc_Absyn_Static;
goto _LL314; _LL322: Cyc_Parse_err(( struct _tagged_string)({ char* _temp323=(
char*)"bad storage class on function"; struct _tagged_string _temp324; _temp324.curr=
_temp323; _temp324.base= _temp323; _temp324.last_plus_one= _temp323 + 30;
_temp324;}), loc); goto _LL314; _LL314:;}}{ struct Cyc_Core_Opt* _temp327; void*
_temp329; struct _tuple5 _temp325= Cyc_Parse_collapse_type_specifiers( tss, loc);
_LL330: _temp329= _temp325.f1; goto _LL328; _LL328: _temp327= _temp325.f2; goto
_LL326; _LL326: { struct Cyc_List_List* _temp333; struct Cyc_List_List* _temp335;
void* _temp337; struct Cyc_Absyn_Tqual _temp339; struct _tuple6 _temp331= Cyc_Parse_apply_tms(
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
4u?*(( int*) _temp345) == Cyc_Absyn_FnType: 0){ _LL352: _temp351=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp345)->f1; _LL364: _temp363=( struct Cyc_List_List*)
_temp351.tvars; goto _LL362; _LL362: _temp361=( struct Cyc_Core_Opt*) _temp351.effect;
goto _LL360; _LL360: _temp359=( void*) _temp351.ret_typ; goto _LL358; _LL358:
_temp357=( struct Cyc_List_List*) _temp351.args; goto _LL356; _LL356: _temp355=(
int) _temp351.varargs; goto _LL354; _LL354: _temp353=( struct Cyc_List_List*)
_temp351.attributes; goto _LL348;} else{ goto _LL349;} _LL349: goto _LL350;
_LL348: { struct Cyc_List_List* _temp365=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc,
_temp357); goto _LL366; _LL366: return({ struct Cyc_Absyn_Fndecl* _temp367=(
struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl)); _temp367->sc=(
void*) sc; _temp367->name= d->id; _temp367->tvs= _temp363; _temp367->is_inline=
is_inline; _temp367->effect= _temp361; _temp367->ret_type=( void*) _temp359;
_temp367->args= _temp365; _temp367->varargs= _temp355; _temp367->body= body;
_temp367->cached_typ= 0; _temp367->param_vardecls= 0; _temp367->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp353, _temp333); _temp367;});} _LL350: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp368=( char*)"declarator is not a function prototype";
struct _tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 39; _temp369;}), loc); _LL346:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp370=( char*)"missing argument variable in function prototype"; struct
_tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 48; _temp371;}), loc); return({ struct
_tuple4* _temp372=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp372->f1=({ struct _tagged_string* _temp373=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp373[ 0]=( struct
_tagged_string)({ char* _temp374=( char*)"?"; struct _tagged_string _temp375;
_temp375.curr= _temp374; _temp375.base= _temp374; _temp375.last_plus_one=
_temp374 + 2; _temp375;}); _temp373;}); _temp372->f2=(* t).f2; _temp372->f3=(* t).f3;
_temp372;});} else{ return({ struct _tuple4* _temp376=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp376->f1=( struct _tagged_string*)({
struct Cyc_Core_Opt* _temp377=(* t).f1; if( _temp377 == 0){ _throw(
Null_Exception);} _temp377->v;}); _temp376->f2=(* t).f2; _temp376->f3=(* t).f3;
_temp376;});}} static char _temp380[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_string Cyc_Parse_msg1=( struct _tagged_string){ _temp380,
_temp380, _temp380 + 52u}; static char _temp383[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2=( struct _tagged_string){ _temp383,
_temp383, _temp383 + 63u}; static char _temp386[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3=( struct _tagged_string){ _temp386,
_temp386, _temp386 + 50u}; static char _temp389[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4=( struct _tagged_string){ _temp389,
_temp389, _temp389 + 60u}; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
declopt= 0; int seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*)
Cyc_Absyn_VoidType; void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed;
struct Cyc_Position_Segment* last_loc= loc; for( 0; ts != 0; ts=({ struct Cyc_List_List*
_temp390= ts; if( _temp390 == 0){ _throw( Null_Exception);} _temp390->tl;})){
void* _temp392=( void*)({ struct Cyc_List_List* _temp391= ts; if( _temp391 == 0){
_throw( Null_Exception);} _temp391->hd;}); struct Cyc_Position_Segment* _temp406;
void* _temp408; struct Cyc_Position_Segment* _temp410; struct Cyc_Position_Segment*
_temp412; struct Cyc_Position_Segment* _temp414; struct Cyc_Position_Segment*
_temp416; struct Cyc_Absyn_Decl* _temp418; _LL394: if(*(( int*) _temp392) == Cyc_Parse_Type_spec){
_LL409: _temp408=( void*)(( struct Cyc_Parse_Type_spec_struct*) _temp392)->f1;
goto _LL407; _LL407: _temp406=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Type_spec_struct*)
_temp392)->f2; goto _LL395;} else{ goto _LL396;} _LL396: if(*(( int*) _temp392)
== Cyc_Parse_Signed_spec){ _LL411: _temp410=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Signed_spec_struct*) _temp392)->f1; goto _LL397;} else{ goto
_LL398;} _LL398: if(*(( int*) _temp392) == Cyc_Parse_Unsigned_spec){ _LL413:
_temp412=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Unsigned_spec_struct*)
_temp392)->f1; goto _LL399;} else{ goto _LL400;} _LL400: if(*(( int*) _temp392)
== Cyc_Parse_Short_spec){ _LL415: _temp414=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Short_spec_struct*) _temp392)->f1; goto _LL401;} else{ goto
_LL402;} _LL402: if(*(( int*) _temp392) == Cyc_Parse_Long_spec){ _LL417:
_temp416=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Long_spec_struct*)
_temp392)->f1; goto _LL403;} else{ goto _LL404;} _LL404: if(*(( int*) _temp392)
== Cyc_Parse_Decl_spec){ _LL419: _temp418=( struct Cyc_Absyn_Decl*)(( struct Cyc_Parse_Decl_spec_struct*)
_temp392)->f1; goto _LL405;} else{ goto _LL393;} _LL395: if( seen_type){ Cyc_Parse_err(
Cyc_Parse_msg1, _temp406);} last_loc= _temp406; seen_type= 1; t= _temp408; goto
_LL393; _LL397: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp410);} if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp420=( char*)"signed qualifier must come before type";
struct _tagged_string _temp421; _temp421.curr= _temp420; _temp421.base= _temp420;
_temp421.last_plus_one= _temp420 + 39; _temp421;}), _temp410);} last_loc=
_temp410; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed; goto _LL393; _LL399: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp412);} if( seen_type){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp422=( char*)"signed qualifier must come before type";
struct _tagged_string _temp423; _temp423.curr= _temp422; _temp423.base= _temp422;
_temp423.last_plus_one= _temp422 + 39; _temp423;}), _temp412);} last_loc=
_temp412; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL393; _LL401:
if( seen_size){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp424=( char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp425; _temp425.curr= _temp424; _temp425.base= _temp424;
_temp425.last_plus_one= _temp424 + 59; _temp425;}), _temp414);} if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp426=( char*)"short modifier must come before base type";
struct _tagged_string _temp427; _temp427.curr= _temp426; _temp427.base= _temp426;
_temp427.last_plus_one= _temp426 + 42; _temp427;}), _temp414);} last_loc=
_temp414; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL393; _LL403: if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp428=( char*)"long modifier must come before base type";
struct _tagged_string _temp429; _temp429.curr= _temp428; _temp429.base= _temp428;
_temp429.last_plus_one= _temp428 + 41; _temp429;}), _temp416);} if( seen_size){
void* _temp430= sz; _LL432: if( _temp430 ==( void*) Cyc_Absyn_B4){ goto _LL433;}
else{ goto _LL434;} _LL434: goto _LL435; _LL433: sz=( void*) Cyc_Absyn_B8; goto
_LL431; _LL435: Cyc_Parse_err(( struct _tagged_string)({ char* _temp436=( char*)"extra long in type specifier";
struct _tagged_string _temp437; _temp437.curr= _temp436; _temp437.base= _temp436;
_temp437.last_plus_one= _temp436 + 29; _temp437;}), _temp416); goto _LL431;
_LL431:;} last_loc= _temp416; seen_size= 1; goto _LL393; _LL405: last_loc=
_temp418->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp438=(
void*) _temp418->r; struct Cyc_Absyn_Structdecl* _temp450; struct Cyc_Absyn_Tuniondecl*
_temp452; struct Cyc_Absyn_Uniondecl* _temp454; struct Cyc_Absyn_Enumdecl*
_temp456; _LL440: if(*(( int*) _temp438) == Cyc_Absyn_Struct_d){ _LL451:
_temp450=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp438)->f1; goto _LL441;} else{ goto _LL442;} _LL442: if(*(( int*) _temp438)
== Cyc_Absyn_Tunion_d){ _LL453: _temp452=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp438)->f1; goto _LL443;} else{ goto
_LL444;} _LL444: if(*(( int*) _temp438) == Cyc_Absyn_Union_d){ _LL455: _temp454=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp438)->f1;
goto _LL445;} else{ goto _LL446;} _LL446: if(*(( int*) _temp438) == Cyc_Absyn_Enum_d){
_LL457: _temp456=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp438)->f1; goto _LL447;} else{ goto _LL448;} _LL448: goto _LL449; _LL441: {
struct Cyc_List_List* _temp458=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ,
_temp450->tvs); goto _LL459; _LL459: t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp460=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp460[ 0]=({ struct Cyc_Absyn_StructType_struct _temp461; _temp461.tag= Cyc_Absyn_StructType;
_temp461.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp462= _temp450->name; if( _temp462 == 0){ _throw( Null_Exception);} _temp462->v;}));
_temp461.f2= _temp458; _temp461.f3= 0; _temp461;}); _temp460;}); if( _temp450->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp463=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp463->v=( void*) _temp418;
_temp463;});} goto _LL439;} _LL443: { struct Cyc_List_List* _temp464=(( struct
Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x))
Cyc_List_map)( Cyc_Parse_tvar2typ, _temp452->tvs); goto _LL465; _LL465: t=( void*)({
struct Cyc_Absyn_TunionType_struct* _temp466=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp466[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp467; _temp467.tag= Cyc_Absyn_TunionType;
_temp467.f1=({ struct Cyc_Absyn_TunionInfo _temp468; _temp468.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp469=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp469[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp470; _temp470.tag= Cyc_Absyn_KnownTunion;
_temp470.f1= _temp452; _temp470;}); _temp469;})); _temp468.targs= _temp464;
_temp468.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp468;}); _temp467;});
_temp466;}); if( _temp452->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp471=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp471->v=( void*) _temp418; _temp471;});} goto _LL439;} _LL445: { struct Cyc_List_List*
_temp472=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp454->tvs);
goto _LL473; _LL473: t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp474=(
struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp474[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp475; _temp475.tag= Cyc_Absyn_UnionType;
_temp475.f1=( struct _tuple1*)(( struct _tuple1*)({ struct Cyc_Core_Opt*
_temp476= _temp454->name; if( _temp476 == 0){ _throw( Null_Exception);} _temp476->v;}));
_temp475.f2= _temp472; _temp475.f3= 0; _temp475;}); _temp474;}); if( _temp454->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp477=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp477->v=( void*) _temp418;
_temp477;});} goto _LL439;} _LL447: t=( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp478=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp478[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp479; _temp479.tag= Cyc_Absyn_EnumType;
_temp479.f1= _temp456->name; _temp479.f2= 0; _temp479;}); _temp478;}); declopt=({
struct Cyc_Core_Opt* _temp480=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp480->v=( void*) _temp418; _temp480;}); goto _LL439; _LL449:((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp481=( char*)"bad declaration within type specifier";
struct _tagged_string _temp482; _temp482.curr= _temp481; _temp482.base= _temp481;
_temp482.last_plus_one= _temp481 + 38; _temp482;}), _temp418->loc); goto _LL439;
_LL439:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp418->loc);} goto _LL393;
_LL393:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp483=( char*)"missing type withing specifier";
struct _tagged_string _temp484; _temp484.curr= _temp483; _temp484.base= _temp483;
_temp484.last_plus_one= _temp483 + 31; _temp484;}), last_loc);} t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp485=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp485[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp486; _temp486.tag= Cyc_Absyn_IntType; _temp486.f1=( void*) sgn; _temp486.f2=(
void*) sz; _temp486;}); _temp485;});} else{ if( seen_sign){ void* _temp487= t;
void* _temp493; void* _temp495; _LL489: if(( unsigned int) _temp487 > 4u?*(( int*)
_temp487) == Cyc_Absyn_IntType: 0){ _LL496: _temp495=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp487)->f1; goto _LL494; _LL494: _temp493=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp487)->f2; goto _LL490;} else{ goto _LL491;} _LL491: goto _LL492; _LL490: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp497=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp497[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp498; _temp498.tag= Cyc_Absyn_IntType; _temp498.f1=( void*) sgn; _temp498.f2=(
void*) _temp493; _temp498;}); _temp497;}); goto _LL488; _LL492: Cyc_Parse_err((
struct _tagged_string)({ char* _temp499=( char*)"sign specification on non-integral type";
struct _tagged_string _temp500; _temp500.curr= _temp499; _temp500.base= _temp499;
_temp500.last_plus_one= _temp499 + 40; _temp500;}), last_loc); goto _LL488;
_LL488:;} if( seen_size){ void* _temp501= t; void* _temp507; void* _temp509;
_LL503: if(( unsigned int) _temp501 > 4u?*(( int*) _temp501) == Cyc_Absyn_IntType:
0){ _LL510: _temp509=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp501)->f1;
goto _LL508; _LL508: _temp507=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp501)->f2; goto _LL504;} else{ goto _LL505;} _LL505: goto _LL506; _LL504: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp511=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp511[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp512; _temp512.tag= Cyc_Absyn_IntType; _temp512.f1=( void*) _temp509;
_temp512.f2=( void*) sz; _temp512;}); _temp511;}); goto _LL502; _LL506: Cyc_Parse_err((
struct _tagged_string)({ char* _temp513=( char*)"size qualifier on non-integral type";
struct _tagged_string _temp514; _temp514.curr= _temp513; _temp514.base= _temp513;
_temp514.last_plus_one= _temp513 + 36; _temp514;}), last_loc); goto _LL502;
_LL502:;}} return({ struct _tuple5 _temp515; _temp515.f1= t; _temp515.f2=
declopt; _temp515;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp517=(
struct Cyc_Parse_Declarator*)({ struct Cyc_List_List* _temp516= ds; if( _temp516
== 0){ _throw( Null_Exception);} _temp516->hd;}); goto _LL518; _LL518: { struct
_tuple1* _temp519= _temp517->id; goto _LL520; _LL520: { struct Cyc_List_List*
_temp523; struct Cyc_List_List* _temp525; void* _temp527; struct Cyc_Absyn_Tqual
_temp529; struct _tuple6 _temp521= Cyc_Parse_apply_tms( tq, t, shared_atts,
_temp517->tms); _LL530: _temp529= _temp521.f1; goto _LL528; _LL528: _temp527=
_temp521.f2; goto _LL526; _LL526: _temp525= _temp521.f3; goto _LL524; _LL524:
_temp523= _temp521.f4; goto _LL522; _LL522: return({ struct Cyc_List_List*
_temp531=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp531->hd=( void*)({ struct _tuple7* _temp533=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp533->f1= _temp519; _temp533->f2= _temp529;
_temp533->f3= _temp527; _temp533->f4= _temp525; _temp533->f5= _temp523; _temp533;});
_temp531->tl= Cyc_Parse_apply_tmss( _temp529, t,({ struct Cyc_List_List*
_temp532= ds; if( _temp532 == 0){ _throw( Null_Exception);} _temp532->tl;}),
shared_atts); _temp531;});}}}} static int Cyc_Parse_fn_type_att( void* a){ void*
_temp534= a; int _temp548; _LL536: if(( unsigned int) _temp534 > 15u?*(( int*)
_temp534) == Cyc_Absyn_Regparm_att: 0){ _LL549: _temp548=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp534)->f1; goto _LL537;} else{ goto _LL538;} _LL538: if( _temp534 ==( void*)
Cyc_Absyn_Stdcall_att){ goto _LL539;} else{ goto _LL540;} _LL540: if( _temp534
==( void*) Cyc_Absyn_Cdecl_att){ goto _LL541;} else{ goto _LL542;} _LL542: if(
_temp534 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL543;} else{ goto _LL544;}
_LL544: if( _temp534 ==( void*) Cyc_Absyn_Const_att){ goto _LL545;} else{ goto
_LL546;} _LL546: goto _LL547; _LL537: goto _LL539; _LL539: goto _LL541; _LL541:
goto _LL543; _LL543: goto _LL545; _LL545: return 1; _LL547: return 0; _LL535:;}
static struct _tuple6 Cyc_Parse_apply_tms( struct Cyc_Absyn_Tqual tq, void* t,
struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( tms == 0){ return({
struct _tuple6 _temp550; _temp550.f1= tq; _temp550.f2= t; _temp550.f3= 0;
_temp550.f4= atts; _temp550;});}{ void* _temp552=( void*)({ struct Cyc_List_List*
_temp551= tms; if( _temp551 == 0){ _throw( Null_Exception);} _temp551->hd;});
struct Cyc_Absyn_Exp* _temp566; void* _temp568; int _temp570; struct Cyc_Position_Segment*
_temp572; struct Cyc_List_List* _temp574; struct Cyc_Absyn_Tqual _temp576; void*
_temp578; void* _temp580; struct Cyc_List_List* _temp582; struct Cyc_Position_Segment*
_temp584; _LL554: if( _temp552 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL555;}
else{ goto _LL556;} _LL556: if(( unsigned int) _temp552 > 1u?*(( int*) _temp552)
== Cyc_Absyn_ConstArray_mod: 0){ _LL567: _temp566=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_ConstArray_mod_struct*) _temp552)->f1; goto _LL557;} else{ goto
_LL558;} _LL558: if(( unsigned int) _temp552 > 1u?*(( int*) _temp552) == Cyc_Absyn_Function_mod:
0){ _LL569: _temp568=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp552)->f1;
goto _LL559;} else{ goto _LL560;} _LL560: if(( unsigned int) _temp552 > 1u?*((
int*) _temp552) == Cyc_Absyn_TypeParams_mod: 0){ _LL575: _temp574=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp552)->f1; goto _LL573; _LL573:
_temp572=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp552)->f2; goto _LL571; _LL571: _temp570=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp552)->f3; goto _LL561;} else{ goto _LL562;} _LL562: if(( unsigned int)
_temp552 > 1u?*(( int*) _temp552) == Cyc_Absyn_Pointer_mod: 0){ _LL581: _temp580=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp552)->f1; goto _LL579;
_LL579: _temp578=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp552)->f2;
goto _LL577; _LL577: _temp576=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp552)->f3; goto _LL563;} else{ goto _LL564;} _LL564: if(( unsigned int)
_temp552 > 1u?*(( int*) _temp552) == Cyc_Absyn_Attributes_mod: 0){ _LL585:
_temp584=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp552)->f1; goto _LL583; _LL583: _temp582=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp552)->f2; goto _LL565;} else{ goto _LL553;}
_LL555: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp586=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp586[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp587; _temp587.tag= Cyc_Absyn_ArrayType; _temp587.f1=(
void*) t; _temp587.f2= tq; _temp587.f3= 0; _temp587;}); _temp586;}), atts,({
struct Cyc_List_List* _temp588= tms; if( _temp588 == 0){ _throw( Null_Exception);}
_temp588->tl;})); _LL557: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_ArrayType_struct* _temp589=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp589[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp590; _temp590.tag= Cyc_Absyn_ArrayType; _temp590.f1=(
void*) t; _temp590.f2= tq; _temp590.f3=( struct Cyc_Absyn_Exp*) _temp566;
_temp590;}); _temp589;}), atts,({ struct Cyc_List_List* _temp591= tms; if(
_temp591 == 0){ _throw( Null_Exception);} _temp591->tl;})); _LL559: { void*
_temp592= _temp568; struct Cyc_Core_Opt* _temp598; int _temp600; struct Cyc_List_List*
_temp602; struct Cyc_Position_Segment* _temp604; struct Cyc_List_List* _temp606;
_LL594: if(*(( int*) _temp592) == Cyc_Absyn_WithTypes){ _LL603: _temp602=(
struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*) _temp592)->f1; goto
_LL601; _LL601: _temp600=( int)(( struct Cyc_Absyn_WithTypes_struct*) _temp592)->f2;
goto _LL599; _LL599: _temp598=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp592)->f3; goto _LL595;} else{ goto _LL596;} _LL596: if(*(( int*) _temp592)
== Cyc_Absyn_NoTypes){ _LL607: _temp606=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp592)->f1; goto _LL605; _LL605: _temp604=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp592)->f2; goto _LL597;} else{ goto _LL593;}
_LL595: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as=({ struct Cyc_List_List* _temp608= as; if( _temp608 == 0){ _throw(
Null_Exception);} _temp608->tl;})){ if( Cyc_Parse_fn_type_att(( void*)({ struct
Cyc_List_List* _temp609= as; if( _temp609 == 0){ _throw( Null_Exception);}
_temp609->hd;}))){ fn_atts=({ struct Cyc_List_List* _temp610=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp610->hd=( void*)(( void*)({
struct Cyc_List_List* _temp611= as; if( _temp611 == 0){ _throw( Null_Exception);}
_temp611->hd;})); _temp610->tl= fn_atts; _temp610;});} else{ new_atts=({ struct
Cyc_List_List* _temp612=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp612->hd=( void*)(( void*)({ struct Cyc_List_List* _temp613= as; if(
_temp613 == 0){ _throw( Null_Exception);} _temp613->hd;})); _temp612->tl=
new_atts; _temp612;});}}} if(({ struct Cyc_List_List* _temp614= tms; if(
_temp614 == 0){ _throw( Null_Exception);} _temp614->tl;}) != 0){ void* _temp617=(
void*)({ struct Cyc_List_List* _temp616=({ struct Cyc_List_List* _temp615= tms;
if( _temp615 == 0){ _throw( Null_Exception);} _temp615->tl;}); if( _temp616 == 0){
_throw( Null_Exception);} _temp616->hd;}); int _temp623; struct Cyc_Position_Segment*
_temp625; struct Cyc_List_List* _temp627; _LL619: if(( unsigned int) _temp617 >
1u?*(( int*) _temp617) == Cyc_Absyn_TypeParams_mod: 0){ _LL628: _temp627=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp617)->f1;
goto _LL626; _LL626: _temp625=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp617)->f2; goto _LL624; _LL624: _temp623=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp617)->f3; goto _LL620;} else{ goto _LL621;} _LL621: goto _LL622; _LL620:
typvars= _temp627; tms=({ struct Cyc_List_List* _temp629= tms; if( _temp629 == 0){
_throw( Null_Exception);} _temp629->tl;}); goto _LL618; _LL622: goto _LL618;
_LL618:;} if(((( ! _temp600? _temp602 != 0: 0)?({ struct Cyc_List_List* _temp630=
_temp602; if( _temp630 == 0){ _throw( Null_Exception);} _temp630->tl;}) == 0: 0)?(*((
struct _tuple2*)({ struct Cyc_List_List* _temp631= _temp602; if( _temp631 == 0){
_throw( Null_Exception);} _temp631->hd;}))).f1 == 0: 0)?(*(( struct _tuple2*)({
struct Cyc_List_List* _temp632= _temp602; if( _temp632 == 0){ _throw(
Null_Exception);} _temp632->hd;}))).f3 ==( void*) Cyc_Absyn_VoidType: 0){
_temp602= 0; _temp600= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp602); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp598, t, _temp602, _temp600, fn_atts), new_atts,({ struct Cyc_List_List*
_temp633= tms; if( _temp633 == 0){ _throw( Null_Exception);} _temp633->tl;}));}
_LL597:( void) _throw((( void*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp634=( char*)"function declaration without parameter types";
struct _tagged_string _temp635; _temp635.curr= _temp634; _temp635.base= _temp634;
_temp635.last_plus_one= _temp634 + 45; _temp635;}), _temp604)); _LL593:;} _LL561:
if(({ struct Cyc_List_List* _temp636= tms; if( _temp636 == 0){ _throw(
Null_Exception);} _temp636->tl;}) == 0){ return({ struct _tuple6 _temp637;
_temp637.f1= tq; _temp637.f2= t; _temp637.f3= _temp574; _temp637.f4= atts;
_temp637;});}( void) _throw((( void*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp638=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp639; _temp639.curr= _temp638; _temp639.base= _temp638;
_temp639.last_plus_one= _temp638 + 68; _temp639;}), _temp572)); _LL563: { void*
_temp640= _temp580; struct Cyc_Absyn_Exp* _temp648; struct Cyc_Absyn_Exp*
_temp650; _LL642: if(( unsigned int) _temp640 > 1u?*(( int*) _temp640) == Cyc_Absyn_NonNullable_ps:
0){ _LL649: _temp648=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp640)->f1; goto _LL643;} else{ goto _LL644;} _LL644: if(( unsigned int)
_temp640 > 1u?*(( int*) _temp640) == Cyc_Absyn_Nullable_ps: 0){ _LL651: _temp650=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp640)->f1;
goto _LL645;} else{ goto _LL646;} _LL646: if( _temp640 ==( void*) Cyc_Absyn_TaggedArray_ps){
goto _LL647;} else{ goto _LL641;} _LL643: return Cyc_Parse_apply_tms( _temp576,
Cyc_Absyn_atb_typ( t, _temp578, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp652=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp652[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp653; _temp653.tag= Cyc_Absyn_Upper_b;
_temp653.f1= _temp648; _temp653;}); _temp652;})), atts,({ struct Cyc_List_List*
_temp654= tms; if( _temp654 == 0){ _throw( Null_Exception);} _temp654->tl;}));
_LL645: return Cyc_Parse_apply_tms( _temp576, Cyc_Absyn_starb_typ( t, _temp578,
tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp655=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp655[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp656; _temp656.tag= Cyc_Absyn_Upper_b; _temp656.f1= _temp650; _temp656;});
_temp655;})), atts,({ struct Cyc_List_List* _temp657= tms; if( _temp657 == 0){
_throw( Null_Exception);} _temp657->tl;})); _LL647: return Cyc_Parse_apply_tms(
_temp576, Cyc_Absyn_tagged_typ( t, _temp578, tq), atts,({ struct Cyc_List_List*
_temp658= tms; if( _temp658 == 0){ _throw( Null_Exception);} _temp658->tl;}));
_LL641:;} _LL565: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts,
_temp582),({ struct Cyc_List_List* _temp659= tms; if( _temp659 == 0){ _throw(
Null_Exception);} _temp659->tl;})); _LL553:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp662; void* _temp664; struct _tuple5 _temp660= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL665: _temp664= _temp660.f1; goto _LL663; _LL663: _temp662=
_temp660.f2; goto _LL661; _LL661: if( _temp662 != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp666=( char*)"ignoring nested type declaration(s) in specifier list";
struct _tagged_string _temp667; _temp667.curr= _temp666; _temp667.base= _temp666;
_temp667.last_plus_one= _temp666 + 54; _temp667;}), loc);} return _temp664;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp668=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp668[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp669; _temp669.tag= Cyc_Absyn_Decl_s;
_temp669.f1= d; _temp669.f2= s; _temp669;}); _temp668;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp670=( char*)"inline is only allowed on function definitions"; struct
_tagged_string _temp671; _temp671.curr= _temp670; _temp671.base= _temp670;
_temp671.last_plus_one= _temp670 + 47; _temp671;}), loc);} if( tss == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp672=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp673; _temp673.curr= _temp672; _temp673.base= _temp672;
_temp673.last_plus_one= _temp672 + 39; _temp673;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp675=( void*)({ struct Cyc_Core_Opt* _temp674= ds->sc; if(
_temp674 == 0){ _throw( Null_Exception);} _temp674->v;}); _LL677: if( _temp675
==( void*) Cyc_Parse_Typedef_sc){ goto _LL678;} else{ goto _LL679;} _LL679: if(
_temp675 ==( void*) Cyc_Parse_Extern_sc){ goto _LL680;} else{ goto _LL681;}
_LL681: if( _temp675 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL682;} else{ goto
_LL683;} _LL683: if( _temp675 ==( void*) Cyc_Parse_Static_sc){ goto _LL684;}
else{ goto _LL685;} _LL685: if( _temp675 ==( void*) Cyc_Parse_Auto_sc){ goto
_LL686;} else{ goto _LL687;} _LL687: if( _temp675 ==( void*) Cyc_Parse_Register_sc){
goto _LL688;} else{ goto _LL689;} _LL689: if( _temp675 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL690;} else{ goto _LL676;} _LL678: istypedef= 1; goto _LL676; _LL680: s=(
void*) Cyc_Absyn_Extern; goto _LL676; _LL682: s=( void*) Cyc_Absyn_ExternC; goto
_LL676; _LL684: s=( void*) Cyc_Absyn_Static; goto _LL676; _LL686: s=( void*) Cyc_Absyn_Public;
goto _LL676; _LL688: s=( void*) Cyc_Absyn_Public; goto _LL676; _LL690: s=( void*)
Cyc_Absyn_Abstract; goto _LL676; _LL676:;}{ struct Cyc_List_List* _temp693;
struct Cyc_List_List* _temp695; struct _tuple0 _temp691=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL696: _temp695= _temp691.f1;
goto _LL694; _LL694: _temp693= _temp691.f2; goto _LL692; _LL692: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp693; for( 0; es != 0; es=({
struct Cyc_List_List* _temp697= es; if( _temp697 == 0){ _throw( Null_Exception);}
_temp697->tl;})){ if(( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp698=
es; if( _temp698 == 0){ _throw( Null_Exception);} _temp698->hd;}) != 0){
exps_empty= 0; break;}}}{ struct _tuple5 _temp699= Cyc_Parse_collapse_type_specifiers(
tss, loc); goto _LL700; _LL700: if( _temp695 == 0){ struct Cyc_Core_Opt*
_temp703; void* _temp705; struct _tuple5 _temp701= _temp699; _LL706: _temp705=
_temp701.f1; goto _LL704; _LL704: _temp703= _temp701.f2; goto _LL702; _LL702:
if( _temp703 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct
Cyc_Core_Opt* _temp734= _temp703; if( _temp734 == 0){ _throw( Null_Exception);}
_temp734->v;});{ void* _temp707=( void*) d->r; struct Cyc_Absyn_Enumdecl*
_temp719; struct Cyc_Absyn_Structdecl* _temp721; struct Cyc_Absyn_Uniondecl*
_temp723; struct Cyc_Absyn_Tuniondecl* _temp725; _LL709: if(*(( int*) _temp707)
== Cyc_Absyn_Enum_d){ _LL720: _temp719=( struct Cyc_Absyn_Enumdecl*)(( struct
Cyc_Absyn_Enum_d_struct*) _temp707)->f1; goto _LL710;} else{ goto _LL711;}
_LL711: if(*(( int*) _temp707) == Cyc_Absyn_Struct_d){ _LL722: _temp721=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp707)->f1; goto
_LL712;} else{ goto _LL713;} _LL713: if(*(( int*) _temp707) == Cyc_Absyn_Union_d){
_LL724: _temp723=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp707)->f1; goto _LL714;} else{ goto _LL715;} _LL715: if(*(( int*) _temp707)
== Cyc_Absyn_Tunion_d){ _LL726: _temp725=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp707)->f1; goto _LL716;} else{ goto
_LL717;} _LL717: goto _LL718; _LL710:( void*)( _temp719->sc=( void*) s); if(
atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp727=( char*)"bad attributes on enum";
struct _tagged_string _temp728; _temp728.curr= _temp727; _temp728.base= _temp727;
_temp728.last_plus_one= _temp727 + 23; _temp728;}), loc);} goto _LL708; _LL712:(
void*)( _temp721->sc=( void*) s); _temp721->attributes= atts; goto _LL708;
_LL714:( void*)( _temp723->sc=( void*) s); _temp723->attributes= atts; goto
_LL708; _LL716:( void*)( _temp725->sc=( void*) s); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp729=( char*)"bad attributes on tunion";
struct _tagged_string _temp730; _temp730.curr= _temp729; _temp730.base= _temp729;
_temp730.last_plus_one= _temp729 + 25; _temp730;}), loc);} goto _LL708; _LL718:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp731=( char*)"bad declaration";
struct _tagged_string _temp732; _temp732.curr= _temp731; _temp732.base= _temp731;
_temp732.last_plus_one= _temp731 + 16; _temp732;}), loc); return 0; _LL708:;}
return({ struct Cyc_List_List* _temp733=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp733->hd=( void*) d; _temp733->tl= 0;
_temp733;});} else{ void* _temp735= _temp705; struct Cyc_Absyn_Structdecl**
_temp749; struct Cyc_List_List* _temp751; struct _tuple1* _temp753; struct Cyc_Absyn_TunionInfo
_temp755; void* _temp757; struct Cyc_List_List* _temp759; void* _temp761; struct
Cyc_Absyn_Tuniondecl* _temp763; struct Cyc_Absyn_TunionInfo _temp765; void*
_temp767; struct Cyc_List_List* _temp769; void* _temp771; struct Cyc_Absyn_UnknownTunionInfo
_temp773; int _temp775; struct _tuple1* _temp777; struct Cyc_Absyn_Uniondecl**
_temp779; struct Cyc_List_List* _temp781; struct _tuple1* _temp783; struct Cyc_Absyn_Enumdecl*
_temp785; struct _tuple1* _temp787; _LL737: if(( unsigned int) _temp735 > 4u?*((
int*) _temp735) == Cyc_Absyn_StructType: 0){ _LL754: _temp753=( struct _tuple1*)((
struct Cyc_Absyn_StructType_struct*) _temp735)->f1; goto _LL752; _LL752:
_temp751=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp735)->f2; goto _LL750; _LL750: _temp749=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp735)->f3; goto _LL738;} else{ goto
_LL739;} _LL739: if(( unsigned int) _temp735 > 4u?*(( int*) _temp735) == Cyc_Absyn_TunionType:
0){ _LL756: _temp755=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp735)->f1; _LL762: _temp761=( void*) _temp755.tunion_info; if(*(( int*)
_temp761) == Cyc_Absyn_KnownTunion){ _LL764: _temp763=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp761)->f1; goto _LL760;} else{ goto
_LL741;} _LL760: _temp759=( struct Cyc_List_List*) _temp755.targs; goto _LL758;
_LL758: _temp757=( void*) _temp755.rgn; goto _LL740;} else{ goto _LL741;} _LL741:
if(( unsigned int) _temp735 > 4u?*(( int*) _temp735) == Cyc_Absyn_TunionType: 0){
_LL766: _temp765=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp735)->f1; _LL772: _temp771=( void*) _temp765.tunion_info; if(*(( int*)
_temp771) == Cyc_Absyn_UnknownTunion){ _LL774: _temp773=( struct Cyc_Absyn_UnknownTunionInfo)((
struct Cyc_Absyn_UnknownTunion_struct*) _temp771)->f1; _LL778: _temp777=( struct
_tuple1*) _temp773.name; goto _LL776; _LL776: _temp775=( int) _temp773.is_xtunion;
goto _LL770;} else{ goto _LL743;} _LL770: _temp769=( struct Cyc_List_List*)
_temp765.targs; goto _LL768; _LL768: _temp767=( void*) _temp765.rgn; goto _LL742;}
else{ goto _LL743;} _LL743: if(( unsigned int) _temp735 > 4u?*(( int*) _temp735)
== Cyc_Absyn_UnionType: 0){ _LL784: _temp783=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp735)->f1; goto _LL782; _LL782: _temp781=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp735)->f2; goto _LL780; _LL780: _temp779=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp735)->f3;
goto _LL744;} else{ goto _LL745;} _LL745: if(( unsigned int) _temp735 > 4u?*((
int*) _temp735) == Cyc_Absyn_EnumType: 0){ _LL788: _temp787=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp735)->f1; goto _LL786; _LL786: _temp785=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp735)->f2;
goto _LL746;} else{ goto _LL747;} _LL747: goto _LL748; _LL738: { struct Cyc_List_List*
_temp789=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp751); goto _LL790; _LL790: { struct Cyc_Absyn_Structdecl*
_temp794=({ struct Cyc_Absyn_Structdecl* _temp791=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp791->sc=( void*) s;
_temp791->name=({ struct Cyc_Core_Opt* _temp792=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp792->v=( void*)(( struct _tuple1*)({
struct _tuple1* _temp793= _temp753; if( _temp793 == 0){ _throw( Null_Exception);}
_temp793;})); _temp792;}); _temp791->tvs= _temp789; _temp791->fields= 0;
_temp791->attributes= 0; _temp791;}); goto _LL795; _LL795: if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp796=( char*)"bad attributes on struct";
struct _tagged_string _temp797; _temp797.curr= _temp796; _temp797.base= _temp796;
_temp797.last_plus_one= _temp796 + 25; _temp797;}), loc);} return({ struct Cyc_List_List*
_temp798=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp798->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp799=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp799[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp800; _temp800.tag= Cyc_Absyn_Struct_d;
_temp800.f1= _temp794; _temp800;}); _temp799;}), loc); _temp798->tl= 0; _temp798;});}}
_LL740: if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp801=(
char*)"bad attributes on tunion"; struct _tagged_string _temp802; _temp802.curr=
_temp801; _temp802.base= _temp801; _temp802.last_plus_one= _temp801 + 25;
_temp802;}), loc);} return({ struct Cyc_List_List* _temp803=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp803->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp804=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp804[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp805; _temp805.tag= Cyc_Absyn_Tunion_d; _temp805.f1=
_temp763; _temp805;}); _temp804;}), loc); _temp803->tl= 0; _temp803;}); _LL742: {
struct Cyc_List_List* _temp806=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp769);
goto _LL807; _LL807: { struct Cyc_Absyn_Decl* _temp808= Cyc_Absyn_tunion_decl( s,
_temp777, _temp806, 0, _temp775, loc); goto _LL809; _LL809: if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp810=( char*)"bad attributes on tunion";
struct _tagged_string _temp811; _temp811.curr= _temp810; _temp811.base= _temp810;
_temp811.last_plus_one= _temp810 + 25; _temp811;}), loc);} return({ struct Cyc_List_List*
_temp812=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp812->hd=( void*) _temp808; _temp812->tl= 0; _temp812;});}} _LL744: { struct
Cyc_List_List* _temp813=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp781); goto
_LL814; _LL814: { struct Cyc_Absyn_Uniondecl* _temp818=({ struct Cyc_Absyn_Uniondecl*
_temp815=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp815->sc=( void*) s; _temp815->name=({ struct Cyc_Core_Opt* _temp816=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp816->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp817= _temp783; if( _temp817 ==
0){ _throw( Null_Exception);} _temp817;})); _temp816;}); _temp815->tvs= _temp813;
_temp815->fields= 0; _temp815->attributes= 0; _temp815;}); goto _LL819; _LL819:
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp820=( char*)"bad attributes on union";
struct _tagged_string _temp821; _temp821.curr= _temp820; _temp821.base= _temp820;
_temp821.last_plus_one= _temp820 + 24; _temp821;}), loc);} return({ struct Cyc_List_List*
_temp822=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp822->hd=( void*)({ struct Cyc_Absyn_Decl* _temp823=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp823->r=( void*)(( void*)({
struct Cyc_Absyn_Union_d_struct* _temp824=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp824[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp825; _temp825.tag= Cyc_Absyn_Union_d; _temp825.f1= _temp818; _temp825;});
_temp824;})); _temp823->loc= loc; _temp823;}); _temp822->tl= 0; _temp822;});}}
_LL746: { struct Cyc_Absyn_Enumdecl* _temp827=({ struct Cyc_Absyn_Enumdecl*
_temp826=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp826->sc=( void*) s; _temp826->name= _temp787; _temp826->fields= 0; _temp826;});
goto _LL828; _LL828: if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp829=( char*)"bad attributes on enum"; struct _tagged_string _temp830;
_temp830.curr= _temp829; _temp830.base= _temp829; _temp830.last_plus_one=
_temp829 + 23; _temp830;}), loc);} return({ struct Cyc_List_List* _temp831=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp831->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp832=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp832->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_d_struct*
_temp833=( struct Cyc_Absyn_Enum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct));
_temp833[ 0]=({ struct Cyc_Absyn_Enum_d_struct _temp834; _temp834.tag= Cyc_Absyn_Enum_d;
_temp834.f1= _temp827; _temp834;}); _temp833;})); _temp832->loc= loc; _temp832;});
_temp831->tl= 0; _temp831;});} _LL748: Cyc_Parse_err(( struct _tagged_string)({
char* _temp835=( char*)"missing declarator"; struct _tagged_string _temp836;
_temp836.curr= _temp835; _temp836.base= _temp835; _temp836.last_plus_one=
_temp835 + 19; _temp836;}), loc); return 0; _LL736:;}} else{ void* t= _temp699.f1;
struct Cyc_List_List* _temp837= Cyc_Parse_apply_tmss( tq, t, _temp695, atts);
goto _LL838; _LL838: if( istypedef){ if( ! exps_empty){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp839=( char*)"initializer in typedef declaration";
struct _tagged_string _temp840; _temp840.curr= _temp839; _temp840.base= _temp839;
_temp840.last_plus_one= _temp839 + 35; _temp840;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, _temp837); if( _temp699.f2 !=
0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_Core_Opt*
_temp864= _temp699.f2; if( _temp864 == 0){ _throw( Null_Exception);} _temp864->v;});{
void* _temp841=( void*) d->r; struct Cyc_Absyn_Structdecl* _temp853; struct Cyc_Absyn_Tuniondecl*
_temp855; struct Cyc_Absyn_Uniondecl* _temp857; struct Cyc_Absyn_Enumdecl*
_temp859; _LL843: if(*(( int*) _temp841) == Cyc_Absyn_Struct_d){ _LL854:
_temp853=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp841)->f1; goto _LL844;} else{ goto _LL845;} _LL845: if(*(( int*) _temp841)
== Cyc_Absyn_Tunion_d){ _LL856: _temp855=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp841)->f1; goto _LL846;} else{ goto
_LL847;} _LL847: if(*(( int*) _temp841) == Cyc_Absyn_Union_d){ _LL858: _temp857=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp841)->f1;
goto _LL848;} else{ goto _LL849;} _LL849: if(*(( int*) _temp841) == Cyc_Absyn_Enum_d){
_LL860: _temp859=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp841)->f1; goto _LL850;} else{ goto _LL851;} _LL851: goto _LL852; _LL844:(
void*)( _temp853->sc=( void*) s); _temp853->attributes= atts; atts= 0; goto
_LL842; _LL846:( void*)( _temp855->sc=( void*) s); goto _LL842; _LL848:( void*)(
_temp857->sc=( void*) s); goto _LL842; _LL850:( void*)( _temp859->sc=( void*) s);
goto _LL842; _LL852: Cyc_Parse_err(( struct _tagged_string)({ char* _temp861=(
char*)"declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp862; _temp862.curr= _temp861; _temp862.base= _temp861;
_temp862.last_plus_one= _temp861 + 70; _temp862;}), loc); goto _LL842; _LL842:;}
decls=({ struct Cyc_List_List* _temp863=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp863->hd=( void*) d; _temp863->tl= decls;
_temp863;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp866=
Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List* _temp865= atts; if(
_temp865 == 0){ _throw( Null_Exception);} _temp865->hd;})); xprintf("bad attribute %.*s in typedef",
_temp866.last_plus_one - _temp866.curr, _temp866.curr);}), loc);} return decls;}}
else{ if( _temp699.f2 != 0){ Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp867=( char*)"nested type declaration within declarator"; struct
_tagged_string _temp868; _temp868.curr= _temp867; _temp868.base= _temp867;
_temp868.last_plus_one= _temp867 + 42; _temp868;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* _temp869= _temp837; goto _LL870; _LL870: for( 0;
_temp869 != 0; _temp869=({ struct Cyc_List_List* _temp871= _temp869; if(
_temp871 == 0){ _throw( Null_Exception);} _temp871->tl;}), _temp693=({ struct
Cyc_List_List* _temp872= _temp693; if( _temp872 == 0){ _throw( Null_Exception);}
_temp872->tl;})){ struct _tuple7 _temp876; struct Cyc_List_List* _temp877;
struct Cyc_List_List* _temp879; void* _temp881; struct Cyc_Absyn_Tqual _temp883;
struct _tuple1* _temp885; struct _tuple7* _temp874=( struct _tuple7*)({ struct
Cyc_List_List* _temp873= _temp869; if( _temp873 == 0){ _throw( Null_Exception);}
_temp873->hd;}); _temp876=* _temp874; _LL886: _temp885= _temp876.f1; goto _LL884;
_LL884: _temp883= _temp876.f2; goto _LL882; _LL882: _temp881= _temp876.f3; goto
_LL880; _LL880: _temp879= _temp876.f4; goto _LL878; _LL878: _temp877= _temp876.f5;
goto _LL875; _LL875: if( _temp879 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp887=( char*)"bad type params, ignoring"; struct _tagged_string
_temp888; _temp888.curr= _temp887; _temp888.base= _temp887; _temp888.last_plus_one=
_temp887 + 26; _temp888;}), loc);} if( _temp693 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp889=( char*)"unexpected null in parse!"; struct
_tagged_string _temp890; _temp890.curr= _temp889; _temp890.base= _temp889;
_temp890.last_plus_one= _temp889 + 26; _temp890;}), loc);}{ struct Cyc_Absyn_Exp*
_temp892=( struct Cyc_Absyn_Exp*)({ struct Cyc_List_List* _temp891= _temp693;
if( _temp891 == 0){ _throw( Null_Exception);} _temp891->hd;}); goto _LL893;
_LL893: { struct Cyc_Absyn_Vardecl* _temp894= Cyc_Absyn_new_vardecl( _temp885,
_temp881, _temp892); goto _LL895; _LL895: _temp894->tq= _temp883;( void*)(
_temp894->sc=( void*) s); _temp894->attributes= _temp877;{ struct Cyc_Absyn_Decl*
_temp899=({ struct Cyc_Absyn_Decl* _temp896=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp896->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp897=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp897[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp898; _temp898.tag= Cyc_Absyn_Var_d;
_temp898.f1= _temp894; _temp898;}); _temp897;})); _temp896->loc= loc; _temp896;});
goto _LL900; _LL900: decls=({ struct Cyc_List_List* _temp901=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp901->hd=( void*) _temp899;
_temp901->tl= decls; _temp901;});}}}}} return(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen(
s) != 1){ Cyc_Parse_err(({ struct _tagged_string _temp902= s; xprintf("bad kind: %.*s",
_temp902.last_plus_one - _temp902.curr, _temp902.curr);}), loc); return( void*)
Cyc_Absyn_BoxKind;} else{ switch(({ struct _tagged_string _temp903= s; char*
_temp905= _temp903.curr + 0; if( _temp903.base == 0? 1:( _temp905 < _temp903.base?
1: _temp905 >= _temp903.last_plus_one)){ _throw( Null_Exception);}* _temp905;})){
case 'A': _LL906: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL907: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL908: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL909: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL910: return(
void*) Cyc_Absyn_EffKind; default: _LL911: Cyc_Parse_err(({ struct
_tagged_string _temp913= s; xprintf("bad kind: %.*s", _temp913.last_plus_one -
_temp913.curr, _temp913.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp914=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp914->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp915=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp915[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp916; _temp916.tag= Cyc_Absyn_Attributes_mod;
_temp916.f1= loc; _temp916.f2= atts; _temp916;}); _temp915;})); _temp914->tl=
tms; _temp914;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(({ struct
_tagged_string _temp918= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp917=(* t).f5; if( _temp917 == 0){ _throw( Null_Exception);} _temp917->hd;}));
xprintf("bad attribute %.*s within typedef", _temp918.last_plus_one - _temp918.curr,
_temp918.curr);}), loc);} return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Typedef_d_struct*
_temp919=( struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp919[ 0]=({ struct Cyc_Absyn_Typedef_d_struct _temp920; _temp920.tag= Cyc_Absyn_Typedef_d;
_temp920.f1=({ struct Cyc_Absyn_Typedefdecl* _temp921=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp921->name= x; _temp921->tvs=(*
t).f4; _temp921->defn=( void*)(* t).f3; _temp921;}); _temp920;}); _temp919;}),
loc);} char Cyc_AbstractDeclarator_tok[ 27u]="\000\000\000\000AbstractDeclarator_tok";
struct Cyc_AbstractDeclarator_tok_struct{ char* tag; struct Cyc_Parse_Abstractdeclarator*
f1; } ; char Cyc_AttributeList_tok[ 22u]="\000\000\000\000AttributeList_tok";
struct Cyc_AttributeList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_Attribute_tok[ 18u]="\000\000\000\000Attribute_tok"; struct Cyc_Attribute_tok_struct{
char* tag; void* f1; } ; char Cyc_BlockItem_tok[ 18u]="\000\000\000\000BlockItem_tok";
struct Cyc_BlockItem_tok_struct{ char* tag; void* f1; } ; char Cyc_Bool_tok[ 13u]="\000\000\000\000Bool_tok";
struct Cyc_Bool_tok_struct{ char* tag; int f1; } ; char Cyc_Char_tok[ 13u]="\000\000\000\000Char_tok";
struct Cyc_Char_tok_struct{ char* tag; char f1; } ; char Cyc_DeclList_tok[ 17u]="\000\000\000\000DeclList_tok";
struct Cyc_DeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char
Cyc_DeclSpec_tok[ 17u]="\000\000\000\000DeclSpec_tok"; struct Cyc_DeclSpec_tok_struct{
char* tag; struct Cyc_Parse_Declaration_spec* f1; } ; char Cyc_DeclaratorExpoptList_tok[
29u]="\000\000\000\000DeclaratorExpoptList_tok"; struct Cyc_DeclaratorExpoptList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_DeclaratorExpopt_tok[ 25u]="\000\000\000\000DeclaratorExpopt_tok";
struct _tuple12{ struct Cyc_Parse_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ char* tag; struct _tuple12* f1; } ; char
Cyc_Declarator_tok[ 19u]="\000\000\000\000Declarator_tok"; struct Cyc_Declarator_tok_struct{
char* tag; struct Cyc_Parse_Declarator* f1; } ; char Cyc_DesignatorList_tok[ 23u]="\000\000\000\000DesignatorList_tok";
struct Cyc_DesignatorList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_Designator_tok[ 19u]="\000\000\000\000Designator_tok"; struct Cyc_Designator_tok_struct{
char* tag; void* f1; } ; char Cyc_EnumfieldList_tok[ 22u]="\000\000\000\000EnumfieldList_tok";
struct Cyc_EnumfieldList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_Enumfield_tok[ 18u]="\000\000\000\000Enumfield_tok"; struct Cyc_Enumfield_tok_struct{
char* tag; struct Cyc_Absyn_Enumfield* f1; } ; char Cyc_ExpList_tok[ 16u]="\000\000\000\000ExpList_tok";
struct Cyc_ExpList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char Cyc_Exp_tok[
12u]="\000\000\000\000Exp_tok"; struct Cyc_Exp_tok_struct{ char* tag; struct Cyc_Absyn_Exp*
f1; } ; char Cyc_FieldPatternList_tok[ 25u]="\000\000\000\000FieldPatternList_tok";
struct Cyc_FieldPatternList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_FieldPattern_tok[ 21u]="\000\000\000\000FieldPattern_tok"; struct
_tuple13{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
char* tag; struct _tuple13* f1; } ; char Cyc_FnDecl_tok[ 15u]="\000\000\000\000FnDecl_tok";
struct Cyc_FnDecl_tok_struct{ char* tag; struct Cyc_Absyn_Fndecl* f1; } ; char
Cyc_IdList_tok[ 15u]="\000\000\000\000IdList_tok"; struct Cyc_IdList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_InitDeclList_tok[ 21u]="\000\000\000\000InitDeclList_tok";
struct Cyc_InitDeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_InitDecl_tok[ 17u]="\000\000\000\000InitDecl_tok"; struct _tuple14{
struct Cyc_Parse_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{
char* tag; struct _tuple14* f1; } ; char Cyc_InitializerList_tok[ 24u]="\000\000\000\000InitializerList_tok";
struct Cyc_InitializerList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_Int_tok[ 12u]="\000\000\000\000Int_tok"; struct _tuple15{ void* f1; int
f2; } ; struct Cyc_Int_tok_struct{ char* tag; struct _tuple15* f1; } ; char Cyc_Kind_tok[
13u]="\000\000\000\000Kind_tok"; struct Cyc_Kind_tok_struct{ char* tag; void* f1;
} ; char Cyc_Okay_tok[ 13u]="\000\000\000\000Okay_tok"; char Cyc_ParamDeclListBool_tok[
26u]="\000\000\000\000ParamDeclListBool_tok"; struct _tuple16{ struct Cyc_List_List*
f1; int f2; struct Cyc_Core_Opt* f3; } ; struct Cyc_ParamDeclListBool_tok_struct{
char* tag; struct _tuple16* f1; } ; char Cyc_ParamDeclList_tok[ 22u]="\000\000\000\000ParamDeclList_tok";
struct Cyc_ParamDeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_ParamDecl_tok[ 18u]="\000\000\000\000ParamDecl_tok"; struct Cyc_ParamDecl_tok_struct{
char* tag; struct _tuple2* f1; } ; char Cyc_PatternList_tok[ 20u]="\000\000\000\000PatternList_tok";
struct Cyc_PatternList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char
Cyc_Pattern_tok[ 16u]="\000\000\000\000Pattern_tok"; struct Cyc_Pattern_tok_struct{
char* tag; struct Cyc_Absyn_Pat* f1; } ; char Cyc_Pointer_Sort_tok[ 21u]="\000\000\000\000Pointer_Sort_tok";
struct Cyc_Pointer_Sort_tok_struct{ char* tag; void* f1; } ; char Cyc_Primop_tok[
15u]="\000\000\000\000Primop_tok"; struct Cyc_Primop_tok_struct{ char* tag; void*
f1; } ; char Cyc_Primopopt_tok[ 18u]="\000\000\000\000Primopopt_tok"; struct Cyc_Primopopt_tok_struct{
char* tag; struct Cyc_Core_Opt* f1; } ; char Cyc_QualId_tok[ 15u]="\000\000\000\000QualId_tok";
struct Cyc_QualId_tok_struct{ char* tag; struct _tuple1* f1; } ; char Cyc_QualSpecList_tok[
21u]="\000\000\000\000QualSpecList_tok"; struct _tuple17{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
char* tag; struct _tuple17* f1; } ; char Cyc_Rgn_tok[ 12u]="\000\000\000\000Rgn_tok";
struct Cyc_Rgn_tok_struct{ char* tag; void* f1; } ; char Cyc_Scope_tok[ 14u]="\000\000\000\000Scope_tok";
struct Cyc_Scope_tok_struct{ char* tag; void* f1; } ; char Cyc_Short_tok[ 14u]="\000\000\000\000Short_tok";
struct Cyc_Short_tok_struct{ char* tag; short f1; } ; char Cyc_Stmt_tok[ 13u]="\000\000\000\000Stmt_tok";
struct Cyc_Stmt_tok_struct{ char* tag; struct Cyc_Absyn_Stmt* f1; } ; char Cyc_StorageClass_tok[
21u]="\000\000\000\000StorageClass_tok"; struct Cyc_StorageClass_tok_struct{
char* tag; void* f1; } ; char Cyc_String_tok[ 15u]="\000\000\000\000String_tok";
struct Cyc_String_tok_struct{ char* tag; struct _tagged_string f1; } ; char Cyc_Stringopt_tok[
18u]="\000\000\000\000Stringopt_tok"; struct Cyc_Stringopt_tok_struct{ char* tag;
struct Cyc_Core_Opt* f1; } ; char Cyc_StructFieldDeclListList_tok[ 32u]="\000\000\000\000StructFieldDeclListList_tok";
struct Cyc_StructFieldDeclListList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; char Cyc_StructFieldDeclList_tok[ 28u]="\000\000\000\000StructFieldDeclList_tok";
struct Cyc_StructFieldDeclList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; char Cyc_StructOrUnion_tok[ 22u]="\000\000\000\000StructOrUnion_tok"; struct
Cyc_StructOrUnion_tok_struct{ char* tag; void* f1; } ; char Cyc_SwitchClauseList_tok[
25u]="\000\000\000\000SwitchClauseList_tok"; struct Cyc_SwitchClauseList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_TunionFieldList_tok[ 24u]="\000\000\000\000TunionFieldList_tok";
struct Cyc_TunionFieldList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_TunionField_tok[ 20u]="\000\000\000\000TunionField_tok"; struct Cyc_TunionField_tok_struct{
char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; char Cyc_TypeList_tok[ 17u]="\000\000\000\000TypeList_tok";
struct Cyc_TypeList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char
Cyc_TypeModifierList_tok[ 25u]="\000\000\000\000TypeModifierList_tok"; struct
Cyc_TypeModifierList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char
Cyc_TypeQual_tok[ 17u]="\000\000\000\000TypeQual_tok"; struct Cyc_TypeQual_tok_struct{
char* tag; struct Cyc_Absyn_Tqual f1; } ; char Cyc_TypeSpecifier_tok[ 22u]="\000\000\000\000TypeSpecifier_tok";
struct Cyc_TypeSpecifier_tok_struct{ char* tag; void* f1; } ; char Cyc_Type_tok[
13u]="\000\000\000\000Type_tok"; struct Cyc_Type_tok_struct{ char* tag; void* f1;
} ; static char _temp925[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp925,
_temp925, _temp925 + 8u}}; struct _tuple15* Cyc_yyget_Int_tok( void* yy1){ void*
_temp926= yy1; struct _tuple15* _temp932; _LL928: if(*(( void**) _temp926) ==
Cyc_Int_tok){ _LL933: _temp932=( struct _tuple15*)(( struct Cyc_Int_tok_struct*)
_temp926)->f1; goto _LL929;} else{ goto _LL930;} _LL930: goto _LL931; _LL929:
return _temp932; _LL931:( void) _throw(( void*)& Cyc_yyfail_Int_tok); _LL927:;}
static char _temp937[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp937,
_temp937, _temp937 + 11u}}; struct _tagged_string Cyc_yyget_String_tok( void*
yy1){ void* _temp938= yy1; struct _tagged_string _temp944; _LL940: if(*(( void**)
_temp938) == Cyc_String_tok){ _LL945: _temp944=( struct _tagged_string)(( struct
Cyc_String_tok_struct*) _temp938)->f1; goto _LL941;} else{ goto _LL942;} _LL942:
goto _LL943; _LL941: return _temp944; _LL943:( void) _throw(( void*)& Cyc_yyfail_String_tok);
_LL939:;} static char _temp949[ 9u]="Char_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp949,
_temp949, _temp949 + 9u}}; char Cyc_yyget_Char_tok( void* yy1){ void* _temp950=
yy1; char _temp956; _LL952: if(*(( void**) _temp950) == Cyc_Char_tok){ _LL957:
_temp956=( char)(( struct Cyc_Char_tok_struct*) _temp950)->f1; goto _LL953;}
else{ goto _LL954;} _LL954: goto _LL955; _LL953: return _temp956; _LL955:( void)
_throw(( void*)& Cyc_yyfail_Char_tok); _LL951:;} static char _temp961[ 17u]="Pointer_Sort_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp961, _temp961, _temp961 + 17u}}; void* Cyc_yyget_Pointer_Sort_tok(
void* yy1){ void* _temp962= yy1; void* _temp968; _LL964: if(*(( void**) _temp962)
== Cyc_Pointer_Sort_tok){ _LL969: _temp968=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp962)->f1; goto _LL965;} else{ goto _LL966;} _LL966: goto _LL967; _LL965:
return _temp968; _LL967:( void) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL963:;} static char _temp973[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp973,
_temp973, _temp973 + 8u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
void* _temp974= yy1; struct Cyc_Absyn_Exp* _temp980; _LL976: if(*(( void**)
_temp974) == Cyc_Exp_tok){ _LL981: _temp980=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Exp_tok_struct*) _temp974)->f1; goto _LL977;} else{ goto _LL978;} _LL978:
goto _LL979; _LL977: return _temp980; _LL979:( void) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL975:;} static char _temp985[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp985,
_temp985, _temp985 + 12u}}; struct Cyc_List_List* Cyc_yyget_ExpList_tok( void*
yy1){ void* _temp986= yy1; struct Cyc_List_List* _temp992; _LL988: if(*(( void**)
_temp986) == Cyc_ExpList_tok){ _LL993: _temp992=( struct Cyc_List_List*)((
struct Cyc_ExpList_tok_struct*) _temp986)->f1; goto _LL989;} else{ goto _LL990;}
_LL990: goto _LL991; _LL989: return _temp992; _LL991:( void) _throw(( void*)&
Cyc_yyfail_ExpList_tok); _LL987:;} static char _temp997[ 20u]="InitializerList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp997, _temp997, _temp997 + 20u}}; struct Cyc_List_List*
Cyc_yyget_InitializerList_tok( void* yy1){ void* _temp998= yy1; struct Cyc_List_List*
_temp1004; _LL1000: if(*(( void**) _temp998) == Cyc_InitializerList_tok){
_LL1005: _temp1004=( struct Cyc_List_List*)(( struct Cyc_InitializerList_tok_struct*)
_temp998)->f1; goto _LL1001;} else{ goto _LL1002;} _LL1002: goto _LL1003;
_LL1001: return _temp1004; _LL1003:( void) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL999:;} static char _temp1009[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1009,
_temp1009, _temp1009 + 11u}}; void* Cyc_yyget_Primop_tok( void* yy1){ void*
_temp1010= yy1; void* _temp1016; _LL1012: if(*(( void**) _temp1010) == Cyc_Primop_tok){
_LL1017: _temp1016=( void*)(( struct Cyc_Primop_tok_struct*) _temp1010)->f1;
goto _LL1013;} else{ goto _LL1014;} _LL1014: goto _LL1015; _LL1013: return
_temp1016; _LL1015:( void) _throw(( void*)& Cyc_yyfail_Primop_tok); _LL1011:;}
static char _temp1021[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primopopt_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1021,
_temp1021, _temp1021 + 14u}}; struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void*
yy1){ void* _temp1022= yy1; struct Cyc_Core_Opt* _temp1028; _LL1024: if(*(( void**)
_temp1022) == Cyc_Primopopt_tok){ _LL1029: _temp1028=( struct Cyc_Core_Opt*)((
struct Cyc_Primopopt_tok_struct*) _temp1022)->f1; goto _LL1025;} else{ goto
_LL1026;} _LL1026: goto _LL1027; _LL1025: return _temp1028; _LL1027:( void)
_throw(( void*)& Cyc_yyfail_Primopopt_tok); _LL1023:;} static char _temp1033[ 11u]="QualId_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1033, _temp1033, _temp1033 + 11u}}; struct _tuple1*
Cyc_yyget_QualId_tok( void* yy1){ void* _temp1034= yy1; struct _tuple1*
_temp1040; _LL1036: if(*(( void**) _temp1034) == Cyc_QualId_tok){ _LL1041:
_temp1040=( struct _tuple1*)(( struct Cyc_QualId_tok_struct*) _temp1034)->f1;
goto _LL1037;} else{ goto _LL1038;} _LL1038: goto _LL1039; _LL1037: return
_temp1040; _LL1039:( void) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL1035:;}
static char _temp1045[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1045, _temp1045, _temp1045 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp1046= yy1;
struct Cyc_Absyn_Stmt* _temp1052; _LL1048: if(*(( void**) _temp1046) == Cyc_Stmt_tok){
_LL1053: _temp1052=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Stmt_tok_struct*)
_temp1046)->f1; goto _LL1049;} else{ goto _LL1050;} _LL1050: goto _LL1051;
_LL1049: return _temp1052; _LL1051:( void) _throw(( void*)& Cyc_yyfail_Stmt_tok);
_LL1047:;} static char _temp1057[ 14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_BlockItem_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1057,
_temp1057, _temp1057 + 14u}}; void* Cyc_yyget_BlockItem_tok( void* yy1){ void*
_temp1058= yy1; void* _temp1064; _LL1060: if(*(( void**) _temp1058) == Cyc_BlockItem_tok){
_LL1065: _temp1064=( void*)(( struct Cyc_BlockItem_tok_struct*) _temp1058)->f1;
goto _LL1061;} else{ goto _LL1062;} _LL1062: goto _LL1063; _LL1061: return
_temp1064; _LL1063:( void) _throw(( void*)& Cyc_yyfail_BlockItem_tok); _LL1059:;}
static char _temp1069[ 21u]="SwitchClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1069, _temp1069, _temp1069 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
void* yy1){ void* _temp1070= yy1; struct Cyc_List_List* _temp1076; _LL1072: if(*((
void**) _temp1070) == Cyc_SwitchClauseList_tok){ _LL1077: _temp1076=( struct Cyc_List_List*)((
struct Cyc_SwitchClauseList_tok_struct*) _temp1070)->f1; goto _LL1073;} else{
goto _LL1074;} _LL1074: goto _LL1075; _LL1073: return _temp1076; _LL1075:( void)
_throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL1071:;} static char
_temp1081[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1081, _temp1081, _temp1081 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp1082= yy1;
struct Cyc_Absyn_Pat* _temp1088; _LL1084: if(*(( void**) _temp1082) == Cyc_Pattern_tok){
_LL1089: _temp1088=( struct Cyc_Absyn_Pat*)(( struct Cyc_Pattern_tok_struct*)
_temp1082)->f1; goto _LL1085;} else{ goto _LL1086;} _LL1086: goto _LL1087;
_LL1085: return _temp1088; _LL1087:( void) _throw(( void*)& Cyc_yyfail_Pattern_tok);
_LL1083:;} static char _temp1093[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1093, _temp1093, _temp1093 + 16u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok(
void* yy1){ void* _temp1094= yy1; struct Cyc_List_List* _temp1100; _LL1096: if(*((
void**) _temp1094) == Cyc_PatternList_tok){ _LL1101: _temp1100=( struct Cyc_List_List*)((
struct Cyc_PatternList_tok_struct*) _temp1094)->f1; goto _LL1097;} else{ goto
_LL1098;} _LL1098: goto _LL1099; _LL1097: return _temp1100; _LL1099:( void)
_throw(( void*)& Cyc_yyfail_PatternList_tok); _LL1095:;} static char _temp1105[
17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1105, _temp1105, _temp1105 + 17u}};
struct _tuple13* Cyc_yyget_FieldPattern_tok( void* yy1){ void* _temp1106= yy1;
struct _tuple13* _temp1112; _LL1108: if(*(( void**) _temp1106) == Cyc_FieldPattern_tok){
_LL1113: _temp1112=( struct _tuple13*)(( struct Cyc_FieldPattern_tok_struct*)
_temp1106)->f1; goto _LL1109;} else{ goto _LL1110;} _LL1110: goto _LL1111;
_LL1109: return _temp1112; _LL1111:( void) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL1107:;} static char _temp1117[ 21u]="FieldPatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1117, _temp1117, _temp1117 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp1118= yy1; struct Cyc_List_List* _temp1124; _LL1120: if(*((
void**) _temp1118) == Cyc_FieldPatternList_tok){ _LL1125: _temp1124=( struct Cyc_List_List*)((
struct Cyc_FieldPatternList_tok_struct*) _temp1118)->f1; goto _LL1121;} else{
goto _LL1122;} _LL1122: goto _LL1123; _LL1121: return _temp1124; _LL1123:( void)
_throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL1119:;} static char
_temp1129[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FnDecl_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1129, _temp1129, _temp1129 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp1130= yy1;
struct Cyc_Absyn_Fndecl* _temp1136; _LL1132: if(*(( void**) _temp1130) == Cyc_FnDecl_tok){
_LL1137: _temp1136=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_FnDecl_tok_struct*)
_temp1130)->f1; goto _LL1133;} else{ goto _LL1134;} _LL1134: goto _LL1135;
_LL1133: return _temp1136; _LL1135:( void) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL1131:;} static char _temp1141[ 13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1141,
_temp1141, _temp1141 + 13u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok( void*
yy1){ void* _temp1142= yy1; struct Cyc_List_List* _temp1148; _LL1144: if(*((
void**) _temp1142) == Cyc_DeclList_tok){ _LL1149: _temp1148=( struct Cyc_List_List*)((
struct Cyc_DeclList_tok_struct*) _temp1142)->f1; goto _LL1145;} else{ goto
_LL1146;} _LL1146: goto _LL1147; _LL1145: return _temp1148; _LL1147:( void)
_throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1143:;} static char _temp1153[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1153, _temp1153, _temp1153 + 13u}}; struct Cyc_Parse_Declaration_spec*
Cyc_yyget_DeclSpec_tok( void* yy1){ void* _temp1154= yy1; struct Cyc_Parse_Declaration_spec*
_temp1160; _LL1156: if(*(( void**) _temp1154) == Cyc_DeclSpec_tok){ _LL1161:
_temp1160=( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_DeclSpec_tok_struct*)
_temp1154)->f1; goto _LL1157;} else{ goto _LL1158;} _LL1158: goto _LL1159;
_LL1157: return _temp1160; _LL1159:( void) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1155:;} static char _temp1165[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1165,
_temp1165, _temp1165 + 13u}}; struct _tuple14* Cyc_yyget_InitDecl_tok( void* yy1){
void* _temp1166= yy1; struct _tuple14* _temp1172; _LL1168: if(*(( void**)
_temp1166) == Cyc_InitDecl_tok){ _LL1173: _temp1172=( struct _tuple14*)(( struct
Cyc_InitDecl_tok_struct*) _temp1166)->f1; goto _LL1169;} else{ goto _LL1170;}
_LL1170: goto _LL1171; _LL1169: return _temp1172; _LL1171:( void) _throw(( void*)&
Cyc_yyfail_InitDecl_tok); _LL1167:;} static char _temp1177[ 17u]="InitDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1177, _temp1177, _temp1177 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( void* yy1){ void* _temp1178= yy1; struct Cyc_List_List*
_temp1184; _LL1180: if(*(( void**) _temp1178) == Cyc_InitDeclList_tok){ _LL1185:
_temp1184=( struct Cyc_List_List*)(( struct Cyc_InitDeclList_tok_struct*)
_temp1178)->f1; goto _LL1181;} else{ goto _LL1182;} _LL1182: goto _LL1183;
_LL1181: return _temp1184; _LL1183:( void) _throw(( void*)& Cyc_yyfail_InitDeclList_tok);
_LL1179:;} static char _temp1189[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1189, _temp1189, _temp1189 + 17u}}; void* Cyc_yyget_StorageClass_tok( void*
yy1){ void* _temp1190= yy1; void* _temp1196; _LL1192: if(*(( void**) _temp1190)
== Cyc_StorageClass_tok){ _LL1197: _temp1196=( void*)(( struct Cyc_StorageClass_tok_struct*)
_temp1190)->f1; goto _LL1193;} else{ goto _LL1194;} _LL1194: goto _LL1195;
_LL1193: return _temp1196; _LL1195:( void) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL1191:;} static char _temp1201[ 18u]="TypeSpecifier_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1201, _temp1201, _temp1201 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* _temp1202= yy1; void* _temp1208; _LL1204: if(*(( void**) _temp1202)
== Cyc_TypeSpecifier_tok){ _LL1209: _temp1208=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp1202)->f1; goto _LL1205;} else{ goto _LL1206;} _LL1206: goto _LL1207;
_LL1205: return _temp1208; _LL1207:( void) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1203:;} static char _temp1213[ 18u]="StructOrUnion_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1213, _temp1213, _temp1213 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void*
yy1){ void* _temp1214= yy1; void* _temp1220; _LL1216: if(*(( void**) _temp1214)
== Cyc_StructOrUnion_tok){ _LL1221: _temp1220=( void*)(( struct Cyc_StructOrUnion_tok_struct*)
_temp1214)->f1; goto _LL1217;} else{ goto _LL1218;} _LL1218: goto _LL1219;
_LL1217: return _temp1220; _LL1219:( void) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL1215:;} static char _temp1225[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1225,
_temp1225, _temp1225 + 13u}}; struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(
void* yy1){ void* _temp1226= yy1; struct Cyc_Absyn_Tqual _temp1232; _LL1228: if(*((
void**) _temp1226) == Cyc_TypeQual_tok){ _LL1233: _temp1232=( struct Cyc_Absyn_Tqual)((
struct Cyc_TypeQual_tok_struct*) _temp1226)->f1; goto _LL1229;} else{ goto
_LL1230;} _LL1230: goto _LL1231; _LL1229: return _temp1232; _LL1231:( void)
_throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL1227:;} static char _temp1237[ 24u]="StructFieldDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1237, _temp1237, _temp1237 + 24u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclList_tok( void* yy1){ void* _temp1238= yy1; struct Cyc_List_List*
_temp1244; _LL1240: if(*(( void**) _temp1238) == Cyc_StructFieldDeclList_tok){
_LL1245: _temp1244=( struct Cyc_List_List*)(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp1238)->f1; goto _LL1241;} else{ goto _LL1242;} _LL1242: goto _LL1243;
_LL1241: return _temp1244; _LL1243:( void) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL1239:;} static char _temp1249[ 28u]="StructFieldDeclListList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1249, _temp1249, _temp1249 + 28u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ void* _temp1250= yy1; struct
Cyc_List_List* _temp1256; _LL1252: if(*(( void**) _temp1250) == Cyc_StructFieldDeclListList_tok){
_LL1257: _temp1256=( struct Cyc_List_List*)(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp1250)->f1; goto _LL1253;} else{ goto _LL1254;} _LL1254: goto _LL1255;
_LL1253: return _temp1256; _LL1255:( void) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL1251:;} static char _temp1261[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1261, _temp1261, _temp1261 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ void* _temp1262= yy1; struct Cyc_List_List* _temp1268; _LL1264: if(*((
void**) _temp1262) == Cyc_TypeModifierList_tok){ _LL1269: _temp1268=( struct Cyc_List_List*)((
struct Cyc_TypeModifierList_tok_struct*) _temp1262)->f1; goto _LL1265;} else{
goto _LL1266;} _LL1266: goto _LL1267; _LL1265: return _temp1268; _LL1267:( void)
_throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL1263:;} static char
_temp1273[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgn_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1273, _temp1273, _temp1273 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp1274= yy1; void* _temp1280;
_LL1276: if(*(( void**) _temp1274) == Cyc_Rgn_tok){ _LL1281: _temp1280=( void*)((
struct Cyc_Rgn_tok_struct*) _temp1274)->f1; goto _LL1277;} else{ goto _LL1278;}
_LL1278: goto _LL1279; _LL1277: return _temp1280; _LL1279:( void) _throw(( void*)&
Cyc_yyfail_Rgn_tok); _LL1275:;} static char _temp1285[ 15u]="Declarator_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1285, _temp1285, _temp1285 + 15u}}; struct Cyc_Parse_Declarator*
Cyc_yyget_Declarator_tok( void* yy1){ void* _temp1286= yy1; struct Cyc_Parse_Declarator*
_temp1292; _LL1288: if(*(( void**) _temp1286) == Cyc_Declarator_tok){ _LL1293:
_temp1292=( struct Cyc_Parse_Declarator*)(( struct Cyc_Declarator_tok_struct*)
_temp1286)->f1; goto _LL1289;} else{ goto _LL1290;} _LL1290: goto _LL1291;
_LL1289: return _temp1292; _LL1291:( void) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL1287:;} static char _temp1297[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1297, _temp1297, _temp1297 + 21u}}; struct _tuple12* Cyc_yyget_DeclaratorExpopt_tok(
void* yy1){ void* _temp1298= yy1; struct _tuple12* _temp1304; _LL1300: if(*((
void**) _temp1298) == Cyc_DeclaratorExpopt_tok){ _LL1305: _temp1304=( struct
_tuple12*)(( struct Cyc_DeclaratorExpopt_tok_struct*) _temp1298)->f1; goto
_LL1301;} else{ goto _LL1302;} _LL1302: goto _LL1303; _LL1301: return _temp1304;
_LL1303:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok); _LL1299:;}
static char _temp1309[ 25u]="DeclaratorExpoptList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1309, _temp1309, _temp1309 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1310= yy1; struct Cyc_List_List* _temp1316; _LL1312: if(*((
void**) _temp1310) == Cyc_DeclaratorExpoptList_tok){ _LL1317: _temp1316=( struct
Cyc_List_List*)(( struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1310)->f1;
goto _LL1313;} else{ goto _LL1314;} _LL1314: goto _LL1315; _LL1313: return
_temp1316; _LL1315:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok);
_LL1311:;} static char _temp1321[ 23u]="AbstractDeclarator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1321, _temp1321, _temp1321 + 23u}}; struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_AbstractDeclarator_tok( void* yy1){ void* _temp1322= yy1; struct Cyc_Parse_Abstractdeclarator*
_temp1328; _LL1324: if(*(( void**) _temp1322) == Cyc_AbstractDeclarator_tok){
_LL1329: _temp1328=( struct Cyc_Parse_Abstractdeclarator*)(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp1322)->f1; goto _LL1325;} else{ goto _LL1326;} _LL1326: goto _LL1327;
_LL1325: return _temp1328; _LL1327:( void) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL1323:;} static char _temp1333[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1333,
_temp1333, _temp1333 + 9u}}; int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1334=
yy1; int _temp1340; _LL1336: if(*(( void**) _temp1334) == Cyc_Bool_tok){ _LL1341:
_temp1340=( int)(( struct Cyc_Bool_tok_struct*) _temp1334)->f1; goto _LL1337;}
else{ goto _LL1338;} _LL1338: goto _LL1339; _LL1337: return _temp1340; _LL1339:(
void) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL1335:;} static char _temp1345[
10u]="Scope_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1345, _temp1345, _temp1345 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1346= yy1; void* _temp1352;
_LL1348: if(*(( void**) _temp1346) == Cyc_Scope_tok){ _LL1353: _temp1352=( void*)((
struct Cyc_Scope_tok_struct*) _temp1346)->f1; goto _LL1349;} else{ goto _LL1350;}
_LL1350: goto _LL1351; _LL1349: return _temp1352; _LL1351:( void) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1347:;} static char _temp1357[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1357, _temp1357, _temp1357 + 16u}}; struct Cyc_Absyn_Tunionfield*
Cyc_yyget_TunionField_tok( void* yy1){ void* _temp1358= yy1; struct Cyc_Absyn_Tunionfield*
_temp1364; _LL1360: if(*(( void**) _temp1358) == Cyc_TunionField_tok){ _LL1365:
_temp1364=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_TunionField_tok_struct*)
_temp1358)->f1; goto _LL1361;} else{ goto _LL1362;} _LL1362: goto _LL1363;
_LL1361: return _temp1364; _LL1363:( void) _throw(( void*)& Cyc_yyfail_TunionField_tok);
_LL1359:;} static char _temp1369[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1369, _temp1369, _temp1369 + 20u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ void* _temp1370= yy1; struct Cyc_List_List* _temp1376; _LL1372: if(*((
void**) _temp1370) == Cyc_TunionFieldList_tok){ _LL1377: _temp1376=( struct Cyc_List_List*)((
struct Cyc_TunionFieldList_tok_struct*) _temp1370)->f1; goto _LL1373;} else{
goto _LL1374;} _LL1374: goto _LL1375; _LL1373: return _temp1376; _LL1375:( void)
_throw(( void*)& Cyc_yyfail_TunionFieldList_tok); _LL1371:;} static char
_temp1381[ 17u]="QualSpecList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1381, _temp1381, _temp1381 + 17u}};
struct _tuple17* Cyc_yyget_QualSpecList_tok( void* yy1){ void* _temp1382= yy1;
struct _tuple17* _temp1388; _LL1384: if(*(( void**) _temp1382) == Cyc_QualSpecList_tok){
_LL1389: _temp1388=( struct _tuple17*)(( struct Cyc_QualSpecList_tok_struct*)
_temp1382)->f1; goto _LL1385;} else{ goto _LL1386;} _LL1386: goto _LL1387;
_LL1385: return _temp1388; _LL1387:( void) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1383:;} static char _temp1393[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1393,
_temp1393, _temp1393 + 11u}}; struct Cyc_List_List* Cyc_yyget_IdList_tok( void*
yy1){ void* _temp1394= yy1; struct Cyc_List_List* _temp1400; _LL1396: if(*((
void**) _temp1394) == Cyc_IdList_tok){ _LL1401: _temp1400=( struct Cyc_List_List*)((
struct Cyc_IdList_tok_struct*) _temp1394)->f1; goto _LL1397;} else{ goto _LL1398;}
_LL1398: goto _LL1399; _LL1397: return _temp1400; _LL1399:( void) _throw(( void*)&
Cyc_yyfail_IdList_tok); _LL1395:;} static char _temp1405[ 14u]="ParamDecl_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1405, _temp1405, _temp1405 + 14u}}; struct _tuple2*
Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1406= yy1; struct _tuple2*
_temp1412; _LL1408: if(*(( void**) _temp1406) == Cyc_ParamDecl_tok){ _LL1413:
_temp1412=( struct _tuple2*)(( struct Cyc_ParamDecl_tok_struct*) _temp1406)->f1;
goto _LL1409;} else{ goto _LL1410;} _LL1410: goto _LL1411; _LL1409: return
_temp1412; _LL1411:( void) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1407:;}
static char _temp1417[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1417, _temp1417, _temp1417 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ void* _temp1418= yy1; struct Cyc_List_List* _temp1424; _LL1420: if(*((
void**) _temp1418) == Cyc_ParamDeclList_tok){ _LL1425: _temp1424=( struct Cyc_List_List*)((
struct Cyc_ParamDeclList_tok_struct*) _temp1418)->f1; goto _LL1421;} else{ goto
_LL1422;} _LL1422: goto _LL1423; _LL1421: return _temp1424; _LL1423:( void)
_throw(( void*)& Cyc_yyfail_ParamDeclList_tok); _LL1419:;} static char _temp1429[
22u]="ParamDeclListBool_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1429, _temp1429, _temp1429 + 22u}};
struct _tuple16* Cyc_yyget_ParamDeclListBool_tok( void* yy1){ void* _temp1430=
yy1; struct _tuple16* _temp1436; _LL1432: if(*(( void**) _temp1430) == Cyc_ParamDeclListBool_tok){
_LL1437: _temp1436=( struct _tuple16*)(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp1430)->f1; goto _LL1433;} else{ goto _LL1434;} _LL1434: goto _LL1435;
_LL1433: return _temp1436; _LL1435:( void) _throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL1431:;} static char _temp1441[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1441,
_temp1441, _temp1441 + 13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void*
yy1){ void* _temp1442= yy1; struct Cyc_List_List* _temp1448; _LL1444: if(*((
void**) _temp1442) == Cyc_TypeList_tok){ _LL1449: _temp1448=( struct Cyc_List_List*)((
struct Cyc_TypeList_tok_struct*) _temp1442)->f1; goto _LL1445;} else{ goto
_LL1446;} _LL1446: goto _LL1447; _LL1445: return _temp1448; _LL1447:( void)
_throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1443:;} static char _temp1453[ 19u]="DesignatorList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1453, _temp1453, _temp1453 + 19u}}; struct Cyc_List_List*
Cyc_yyget_DesignatorList_tok( void* yy1){ void* _temp1454= yy1; struct Cyc_List_List*
_temp1460; _LL1456: if(*(( void**) _temp1454) == Cyc_DesignatorList_tok){
_LL1461: _temp1460=( struct Cyc_List_List*)(( struct Cyc_DesignatorList_tok_struct*)
_temp1454)->f1; goto _LL1457;} else{ goto _LL1458;} _LL1458: goto _LL1459;
_LL1457: return _temp1460; _LL1459:( void) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1455:;} static char _temp1465[ 15u]="Designator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1465,
_temp1465, _temp1465 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1466= yy1; void* _temp1472; _LL1468: if(*(( void**) _temp1466) == Cyc_Designator_tok){
_LL1473: _temp1472=( void*)(( struct Cyc_Designator_tok_struct*) _temp1466)->f1;
goto _LL1469;} else{ goto _LL1470;} _LL1470: goto _LL1471; _LL1469: return
_temp1472; _LL1471:( void) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1467:;}
static char _temp1477[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1477, _temp1477, _temp1477 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1478= yy1; void* _temp1484;
_LL1480: if(*(( void**) _temp1478) == Cyc_Kind_tok){ _LL1485: _temp1484=( void*)((
struct Cyc_Kind_tok_struct*) _temp1478)->f1; goto _LL1481;} else{ goto _LL1482;}
_LL1482: goto _LL1483; _LL1481: return _temp1484; _LL1483:( void) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1479:;} static char _temp1489[ 9u]="Type_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,( struct
_tagged_string){ _temp1489, _temp1489, _temp1489 + 9u}}; void* Cyc_yyget_Type_tok(
void* yy1){ void* _temp1490= yy1; void* _temp1496; _LL1492: if(*(( void**)
_temp1490) == Cyc_Type_tok){ _LL1497: _temp1496=( void*)(( struct Cyc_Type_tok_struct*)
_temp1490)->f1; goto _LL1493;} else{ goto _LL1494;} _LL1494: goto _LL1495;
_LL1493: return _temp1496; _LL1495:( void) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1491:;} static char _temp1501[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1501, _temp1501, _temp1501 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ void* _temp1502= yy1; struct Cyc_List_List* _temp1508; _LL1504: if(*((
void**) _temp1502) == Cyc_AttributeList_tok){ _LL1509: _temp1508=( struct Cyc_List_List*)((
struct Cyc_AttributeList_tok_struct*) _temp1502)->f1; goto _LL1505;} else{ goto
_LL1506;} _LL1506: goto _LL1507; _LL1505: return _temp1508; _LL1507:( void)
_throw(( void*)& Cyc_yyfail_AttributeList_tok); _LL1503:;} static char _temp1513[
14u]="Attribute_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1513, _temp1513, _temp1513 + 14u}};
void* Cyc_yyget_Attribute_tok( void* yy1){ void* _temp1514= yy1; void* _temp1520;
_LL1516: if(*(( void**) _temp1514) == Cyc_Attribute_tok){ _LL1521: _temp1520=(
void*)(( struct Cyc_Attribute_tok_struct*) _temp1514)->f1; goto _LL1517;} else{
goto _LL1518;} _LL1518: goto _LL1519; _LL1517: return _temp1520; _LL1519:( void)
_throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1515:;} static char _temp1525[ 14u]="Enumfield_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1525, _temp1525, _temp1525 + 14u}}; struct Cyc_Absyn_Enumfield*
Cyc_yyget_Enumfield_tok( void* yy1){ void* _temp1526= yy1; struct Cyc_Absyn_Enumfield*
_temp1532; _LL1528: if(*(( void**) _temp1526) == Cyc_Enumfield_tok){ _LL1533:
_temp1532=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Enumfield_tok_struct*)
_temp1526)->f1; goto _LL1529;} else{ goto _LL1530;} _LL1530: goto _LL1531;
_LL1529: return _temp1532; _LL1531:( void) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1527:;} static char _temp1537[ 18u]="EnumfieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1537, _temp1537, _temp1537 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1538= yy1; struct Cyc_List_List* _temp1544; _LL1540: if(*((
void**) _temp1538) == Cyc_EnumfieldList_tok){ _LL1545: _temp1544=( struct Cyc_List_List*)((
struct Cyc_EnumfieldList_tok_struct*) _temp1538)->f1; goto _LL1541;} else{ goto
_LL1542;} _LL1542: goto _LL1543; _LL1541: return _temp1544; _LL1543:( void)
_throw(( void*)& Cyc_yyfail_EnumfieldList_tok); _LL1539:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct
Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1546; _temp1546.timestamp=
0; _temp1546.first_line= 0; _temp1546.first_column= 0; _temp1546.last_line= 0;
_temp1546.last_column= 0; _temp1546.text=( struct _tagged_string)({ char*
_temp1547=( char*)""; struct _tagged_string _temp1548; _temp1548.curr= _temp1547;
_temp1548.base= _temp1547; _temp1548.last_plus_one= _temp1547 + 1; _temp1548;});
_temp1546;});} static char _temp1551[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc=(
struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column= 0,.last_line= 0,.last_column=
0,.text=( struct _tagged_string){ _temp1551, _temp1551, _temp1551 + 1u}}; static
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
(short)93, (short)94, (short)95, (short)96}; static char _temp1554[ 2u]="$";
static char _temp1557[ 6u]="error"; static char _temp1560[ 12u]="$undefined.";
static char _temp1563[ 5u]="AUTO"; static char _temp1566[ 9u]="REGISTER"; static
char _temp1569[ 7u]="STATIC"; static char _temp1572[ 7u]="EXTERN"; static char
_temp1575[ 8u]="TYPEDEF"; static char _temp1578[ 5u]="VOID"; static char
_temp1581[ 5u]="CHAR"; static char _temp1584[ 6u]="SHORT"; static char _temp1587[
4u]="INT"; static char _temp1590[ 5u]="LONG"; static char _temp1593[ 6u]="FLOAT";
static char _temp1596[ 7u]="DOUBLE"; static char _temp1599[ 7u]="SIGNED"; static
char _temp1602[ 9u]="UNSIGNED"; static char _temp1605[ 6u]="CONST"; static char
_temp1608[ 9u]="VOLATILE"; static char _temp1611[ 9u]="RESTRICT"; static char
_temp1614[ 7u]="STRUCT"; static char _temp1617[ 6u]="UNION"; static char
_temp1620[ 5u]="CASE"; static char _temp1623[ 8u]="DEFAULT"; static char
_temp1626[ 7u]="INLINE"; static char _temp1629[ 3u]="IF"; static char _temp1632[
5u]="ELSE"; static char _temp1635[ 7u]="SWITCH"; static char _temp1638[ 6u]="WHILE";
static char _temp1641[ 3u]="DO"; static char _temp1644[ 4u]="FOR"; static char
_temp1647[ 5u]="GOTO"; static char _temp1650[ 9u]="CONTINUE"; static char
_temp1653[ 6u]="BREAK"; static char _temp1656[ 7u]="RETURN"; static char
_temp1659[ 7u]="SIZEOF"; static char _temp1662[ 5u]="ENUM"; static char
_temp1665[ 8u]="NULL_kw"; static char _temp1668[ 4u]="LET"; static char
_temp1671[ 6u]="THROW"; static char _temp1674[ 4u]="TRY"; static char _temp1677[
6u]="CATCH"; static char _temp1680[ 4u]="NEW"; static char _temp1683[ 9u]="ABSTRACT";
static char _temp1686[ 9u]="FALLTHRU"; static char _temp1689[ 6u]="USING";
static char _temp1692[ 10u]="NAMESPACE"; static char _temp1695[ 7u]="TUNION";
static char _temp1698[ 8u]="XTUNION"; static char _temp1701[ 5u]="FILL"; static
char _temp1704[ 8u]="CODEGEN"; static char _temp1707[ 4u]="CUT"; static char
_temp1710[ 7u]="SPLICE"; static char _temp1713[ 7u]="PRINTF"; static char
_temp1716[ 8u]="FPRINTF"; static char _temp1719[ 8u]="XPRINTF"; static char
_temp1722[ 6u]="SCANF"; static char _temp1725[ 7u]="FSCANF"; static char
_temp1728[ 7u]="SSCANF"; static char _temp1731[ 7u]="MALLOC"; static char
_temp1734[ 9u]="REGION_T"; static char _temp1737[ 7u]="REGION"; static char
_temp1740[ 5u]="RNEW"; static char _temp1743[ 8u]="RMALLOC"; static char
_temp1746[ 7u]="PTR_OP"; static char _temp1749[ 7u]="INC_OP"; static char
_temp1752[ 7u]="DEC_OP"; static char _temp1755[ 8u]="LEFT_OP"; static char
_temp1758[ 9u]="RIGHT_OP"; static char _temp1761[ 6u]="LE_OP"; static char
_temp1764[ 6u]="GE_OP"; static char _temp1767[ 6u]="EQ_OP"; static char
_temp1770[ 6u]="NE_OP"; static char _temp1773[ 7u]="AND_OP"; static char
_temp1776[ 6u]="OR_OP"; static char _temp1779[ 11u]="MUL_ASSIGN"; static char
_temp1782[ 11u]="DIV_ASSIGN"; static char _temp1785[ 11u]="MOD_ASSIGN"; static
char _temp1788[ 11u]="ADD_ASSIGN"; static char _temp1791[ 11u]="SUB_ASSIGN";
static char _temp1794[ 12u]="LEFT_ASSIGN"; static char _temp1797[ 13u]="RIGHT_ASSIGN";
static char _temp1800[ 11u]="AND_ASSIGN"; static char _temp1803[ 11u]="XOR_ASSIGN";
static char _temp1806[ 10u]="OR_ASSIGN"; static char _temp1809[ 9u]="ELLIPSIS";
static char _temp1812[ 11u]="LEFT_RIGHT"; static char _temp1815[ 12u]="COLON_COLON";
static char _temp1818[ 11u]="IDENTIFIER"; static char _temp1821[ 17u]="INTEGER_CONSTANT";
static char _temp1824[ 7u]="STRING"; static char _temp1827[ 19u]="CHARACTER_CONSTANT";
static char _temp1830[ 18u]="FLOATING_CONSTANT"; static char _temp1833[ 9u]="TYPE_VAR";
static char _temp1836[ 16u]="QUAL_IDENTIFIER"; static char _temp1839[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1842[ 10u]="ATTRIBUTE"; static char _temp1845[ 4u]="';'";
static char _temp1848[ 4u]="'{'"; static char _temp1851[ 4u]="'}'"; static char
_temp1854[ 4u]="'='"; static char _temp1857[ 4u]="'('"; static char _temp1860[ 4u]="')'";
static char _temp1863[ 4u]="','"; static char _temp1866[ 4u]="'_'"; static char
_temp1869[ 4u]="'$'"; static char _temp1872[ 4u]="'<'"; static char _temp1875[ 4u]="'>'";
static char _temp1878[ 4u]="':'"; static char _temp1881[ 4u]="'.'"; static char
_temp1884[ 4u]="'['"; static char _temp1887[ 4u]="']'"; static char _temp1890[ 4u]="'*'";
static char _temp1893[ 4u]="'@'"; static char _temp1896[ 4u]="'?'"; static char
_temp1899[ 4u]="'+'"; static char _temp1902[ 4u]="'-'"; static char _temp1905[ 4u]="'&'";
static char _temp1908[ 4u]="'|'"; static char _temp1911[ 4u]="'^'"; static char
_temp1914[ 4u]="'/'"; static char _temp1917[ 4u]="'%'"; static char _temp1920[ 4u]="'~'";
static char _temp1923[ 4u]="'!'"; static char _temp1926[ 5u]="prog"; static char
_temp1929[ 17u]="translation_unit"; static char _temp1932[ 21u]="external_declaration";
static char _temp1935[ 20u]="function_definition"; static char _temp1938[ 21u]="function_definition2";
static char _temp1941[ 13u]="using_action"; static char _temp1944[ 15u]="unusing_action";
static char _temp1947[ 17u]="namespace_action"; static char _temp1950[ 19u]="unnamespace_action";
static char _temp1953[ 12u]="declaration"; static char _temp1956[ 17u]="declaration_list";
static char _temp1959[ 23u]="declaration_specifiers"; static char _temp1962[ 24u]="storage_class_specifier";
static char _temp1965[ 15u]="attributes_opt"; static char _temp1968[ 11u]="attributes";
static char _temp1971[ 15u]="attribute_list"; static char _temp1974[ 10u]="attribute";
static char _temp1977[ 15u]="type_specifier"; static char _temp1980[ 5u]="kind";
static char _temp1983[ 15u]="type_qualifier"; static char _temp1986[ 15u]="enum_specifier";
static char _temp1989[ 11u]="enum_field"; static char _temp1992[ 22u]="enum_declaration_list";
static char _temp1995[ 26u]="struct_or_union_specifier"; static char _temp1998[
16u]="type_params_opt"; static char _temp2001[ 16u]="struct_or_union"; static
char _temp2004[ 24u]="struct_declaration_list"; static char _temp2007[ 25u]="struct_declaration_list0";
static char _temp2010[ 21u]="init_declarator_list"; static char _temp2013[ 22u]="init_declarator_list0";
static char _temp2016[ 16u]="init_declarator"; static char _temp2019[ 19u]="struct_declaration";
static char _temp2022[ 25u]="specifier_qualifier_list"; static char _temp2025[
23u]="struct_declarator_list"; static char _temp2028[ 24u]="struct_declarator_list0";
static char _temp2031[ 18u]="struct_declarator"; static char _temp2034[ 17u]="tunion_specifier";
static char _temp2037[ 18u]="tunion_or_xtunion"; static char _temp2040[ 17u]="tunionfield_list";
static char _temp2043[ 18u]="tunionfield_scope"; static char _temp2046[ 12u]="tunionfield";
static char _temp2049[ 11u]="declarator"; static char _temp2052[ 18u]="direct_declarator";
static char _temp2055[ 8u]="pointer"; static char _temp2058[ 13u]="pointer_char";
static char _temp2061[ 8u]="rgn_opt"; static char _temp2064[ 4u]="rgn"; static
char _temp2067[ 20u]="type_qualifier_list"; static char _temp2070[ 20u]="parameter_type_list";
static char _temp2073[ 11u]="effect_set"; static char _temp2076[ 14u]="atomic_effect";
static char _temp2079[ 11u]="region_set"; static char _temp2082[ 15u]="parameter_list";
static char _temp2085[ 22u]="parameter_declaration"; static char _temp2088[ 16u]="identifier_list";
static char _temp2091[ 17u]="identifier_list0"; static char _temp2094[ 12u]="initializer";
static char _temp2097[ 18u]="array_initializer"; static char _temp2100[ 17u]="initializer_list";
static char _temp2103[ 12u]="designation"; static char _temp2106[ 16u]="designator_list";
static char _temp2109[ 11u]="designator"; static char _temp2112[ 10u]="type_name";
static char _temp2115[ 14u]="any_type_name"; static char _temp2118[ 15u]="type_name_list";
static char _temp2121[ 20u]="abstract_declarator"; static char _temp2124[ 27u]="direct_abstract_declarator";
static char _temp2127[ 10u]="statement"; static char _temp2130[ 18u]="labeled_statement";
static char _temp2133[ 21u]="expression_statement"; static char _temp2136[ 19u]="compound_statement";
static char _temp2139[ 16u]="block_item_list"; static char _temp2142[ 11u]="block_item";
static char _temp2145[ 20u]="selection_statement"; static char _temp2148[ 15u]="switch_clauses";
static char _temp2151[ 20u]="iteration_statement"; static char _temp2154[ 15u]="jump_statement";
static char _temp2157[ 8u]="pattern"; static char _temp2160[ 19u]="tuple_pattern_list";
static char _temp2163[ 20u]="tuple_pattern_list0"; static char _temp2166[ 14u]="field_pattern";
static char _temp2169[ 19u]="field_pattern_list"; static char _temp2172[ 20u]="field_pattern_list0";
static char _temp2175[ 11u]="expression"; static char _temp2178[ 22u]="assignment_expression";
static char _temp2181[ 20u]="assignment_operator"; static char _temp2184[ 23u]="conditional_expression";
static char _temp2187[ 20u]="constant_expression"; static char _temp2190[ 22u]="logical_or_expression";
static char _temp2193[ 23u]="logical_and_expression"; static char _temp2196[ 24u]="inclusive_or_expression";
static char _temp2199[ 24u]="exclusive_or_expression"; static char _temp2202[ 15u]="and_expression";
static char _temp2205[ 20u]="equality_expression"; static char _temp2208[ 22u]="relational_expression";
static char _temp2211[ 17u]="shift_expression"; static char _temp2214[ 20u]="additive_expression";
static char _temp2217[ 26u]="multiplicative_expression"; static char _temp2220[
16u]="cast_expression"; static char _temp2223[ 17u]="unary_expression"; static
char _temp2226[ 14u]="format_primop"; static char _temp2229[ 15u]="unary_operator";
static char _temp2232[ 19u]="postfix_expression"; static char _temp2235[ 19u]="primary_expression";
static char _temp2238[ 25u]="argument_expression_list"; static char _temp2241[
26u]="argument_expression_list0"; static char _temp2244[ 9u]="constant"; static
char _temp2247[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
232u]={( struct _tagged_string){ _temp1554, _temp1554, _temp1554 + 2u},( struct
_tagged_string){ _temp1557, _temp1557, _temp1557 + 6u},( struct _tagged_string){
_temp1560, _temp1560, _temp1560 + 12u},( struct _tagged_string){ _temp1563,
_temp1563, _temp1563 + 5u},( struct _tagged_string){ _temp1566, _temp1566,
_temp1566 + 9u},( struct _tagged_string){ _temp1569, _temp1569, _temp1569 + 7u},(
struct _tagged_string){ _temp1572, _temp1572, _temp1572 + 7u},( struct
_tagged_string){ _temp1575, _temp1575, _temp1575 + 8u},( struct _tagged_string){
_temp1578, _temp1578, _temp1578 + 5u},( struct _tagged_string){ _temp1581,
_temp1581, _temp1581 + 5u},( struct _tagged_string){ _temp1584, _temp1584,
_temp1584 + 6u},( struct _tagged_string){ _temp1587, _temp1587, _temp1587 + 4u},(
struct _tagged_string){ _temp1590, _temp1590, _temp1590 + 5u},( struct
_tagged_string){ _temp1593, _temp1593, _temp1593 + 6u},( struct _tagged_string){
_temp1596, _temp1596, _temp1596 + 7u},( struct _tagged_string){ _temp1599,
_temp1599, _temp1599 + 7u},( struct _tagged_string){ _temp1602, _temp1602,
_temp1602 + 9u},( struct _tagged_string){ _temp1605, _temp1605, _temp1605 + 6u},(
struct _tagged_string){ _temp1608, _temp1608, _temp1608 + 9u},( struct
_tagged_string){ _temp1611, _temp1611, _temp1611 + 9u},( struct _tagged_string){
_temp1614, _temp1614, _temp1614 + 7u},( struct _tagged_string){ _temp1617,
_temp1617, _temp1617 + 6u},( struct _tagged_string){ _temp1620, _temp1620,
_temp1620 + 5u},( struct _tagged_string){ _temp1623, _temp1623, _temp1623 + 8u},(
struct _tagged_string){ _temp1626, _temp1626, _temp1626 + 7u},( struct
_tagged_string){ _temp1629, _temp1629, _temp1629 + 3u},( struct _tagged_string){
_temp1632, _temp1632, _temp1632 + 5u},( struct _tagged_string){ _temp1635,
_temp1635, _temp1635 + 7u},( struct _tagged_string){ _temp1638, _temp1638,
_temp1638 + 6u},( struct _tagged_string){ _temp1641, _temp1641, _temp1641 + 3u},(
struct _tagged_string){ _temp1644, _temp1644, _temp1644 + 4u},( struct
_tagged_string){ _temp1647, _temp1647, _temp1647 + 5u},( struct _tagged_string){
_temp1650, _temp1650, _temp1650 + 9u},( struct _tagged_string){ _temp1653,
_temp1653, _temp1653 + 6u},( struct _tagged_string){ _temp1656, _temp1656,
_temp1656 + 7u},( struct _tagged_string){ _temp1659, _temp1659, _temp1659 + 7u},(
struct _tagged_string){ _temp1662, _temp1662, _temp1662 + 5u},( struct
_tagged_string){ _temp1665, _temp1665, _temp1665 + 8u},( struct _tagged_string){
_temp1668, _temp1668, _temp1668 + 4u},( struct _tagged_string){ _temp1671,
_temp1671, _temp1671 + 6u},( struct _tagged_string){ _temp1674, _temp1674,
_temp1674 + 4u},( struct _tagged_string){ _temp1677, _temp1677, _temp1677 + 6u},(
struct _tagged_string){ _temp1680, _temp1680, _temp1680 + 4u},( struct
_tagged_string){ _temp1683, _temp1683, _temp1683 + 9u},( struct _tagged_string){
_temp1686, _temp1686, _temp1686 + 9u},( struct _tagged_string){ _temp1689,
_temp1689, _temp1689 + 6u},( struct _tagged_string){ _temp1692, _temp1692,
_temp1692 + 10u},( struct _tagged_string){ _temp1695, _temp1695, _temp1695 + 7u},(
struct _tagged_string){ _temp1698, _temp1698, _temp1698 + 8u},( struct
_tagged_string){ _temp1701, _temp1701, _temp1701 + 5u},( struct _tagged_string){
_temp1704, _temp1704, _temp1704 + 8u},( struct _tagged_string){ _temp1707,
_temp1707, _temp1707 + 4u},( struct _tagged_string){ _temp1710, _temp1710,
_temp1710 + 7u},( struct _tagged_string){ _temp1713, _temp1713, _temp1713 + 7u},(
struct _tagged_string){ _temp1716, _temp1716, _temp1716 + 8u},( struct
_tagged_string){ _temp1719, _temp1719, _temp1719 + 8u},( struct _tagged_string){
_temp1722, _temp1722, _temp1722 + 6u},( struct _tagged_string){ _temp1725,
_temp1725, _temp1725 + 7u},( struct _tagged_string){ _temp1728, _temp1728,
_temp1728 + 7u},( struct _tagged_string){ _temp1731, _temp1731, _temp1731 + 7u},(
struct _tagged_string){ _temp1734, _temp1734, _temp1734 + 9u},( struct
_tagged_string){ _temp1737, _temp1737, _temp1737 + 7u},( struct _tagged_string){
_temp1740, _temp1740, _temp1740 + 5u},( struct _tagged_string){ _temp1743,
_temp1743, _temp1743 + 8u},( struct _tagged_string){ _temp1746, _temp1746,
_temp1746 + 7u},( struct _tagged_string){ _temp1749, _temp1749, _temp1749 + 7u},(
struct _tagged_string){ _temp1752, _temp1752, _temp1752 + 7u},( struct
_tagged_string){ _temp1755, _temp1755, _temp1755 + 8u},( struct _tagged_string){
_temp1758, _temp1758, _temp1758 + 9u},( struct _tagged_string){ _temp1761,
_temp1761, _temp1761 + 6u},( struct _tagged_string){ _temp1764, _temp1764,
_temp1764 + 6u},( struct _tagged_string){ _temp1767, _temp1767, _temp1767 + 6u},(
struct _tagged_string){ _temp1770, _temp1770, _temp1770 + 6u},( struct
_tagged_string){ _temp1773, _temp1773, _temp1773 + 7u},( struct _tagged_string){
_temp1776, _temp1776, _temp1776 + 6u},( struct _tagged_string){ _temp1779,
_temp1779, _temp1779 + 11u},( struct _tagged_string){ _temp1782, _temp1782,
_temp1782 + 11u},( struct _tagged_string){ _temp1785, _temp1785, _temp1785 + 11u},(
struct _tagged_string){ _temp1788, _temp1788, _temp1788 + 11u},( struct
_tagged_string){ _temp1791, _temp1791, _temp1791 + 11u},( struct _tagged_string){
_temp1794, _temp1794, _temp1794 + 12u},( struct _tagged_string){ _temp1797,
_temp1797, _temp1797 + 13u},( struct _tagged_string){ _temp1800, _temp1800,
_temp1800 + 11u},( struct _tagged_string){ _temp1803, _temp1803, _temp1803 + 11u},(
struct _tagged_string){ _temp1806, _temp1806, _temp1806 + 10u},( struct
_tagged_string){ _temp1809, _temp1809, _temp1809 + 9u},( struct _tagged_string){
_temp1812, _temp1812, _temp1812 + 11u},( struct _tagged_string){ _temp1815,
_temp1815, _temp1815 + 12u},( struct _tagged_string){ _temp1818, _temp1818,
_temp1818 + 11u},( struct _tagged_string){ _temp1821, _temp1821, _temp1821 + 17u},(
struct _tagged_string){ _temp1824, _temp1824, _temp1824 + 7u},( struct
_tagged_string){ _temp1827, _temp1827, _temp1827 + 19u},( struct _tagged_string){
_temp1830, _temp1830, _temp1830 + 18u},( struct _tagged_string){ _temp1833,
_temp1833, _temp1833 + 9u},( struct _tagged_string){ _temp1836, _temp1836,
_temp1836 + 16u},( struct _tagged_string){ _temp1839, _temp1839, _temp1839 + 18u},(
struct _tagged_string){ _temp1842, _temp1842, _temp1842 + 10u},( struct
_tagged_string){ _temp1845, _temp1845, _temp1845 + 4u},( struct _tagged_string){
_temp1848, _temp1848, _temp1848 + 4u},( struct _tagged_string){ _temp1851,
_temp1851, _temp1851 + 4u},( struct _tagged_string){ _temp1854, _temp1854,
_temp1854 + 4u},( struct _tagged_string){ _temp1857, _temp1857, _temp1857 + 4u},(
struct _tagged_string){ _temp1860, _temp1860, _temp1860 + 4u},( struct
_tagged_string){ _temp1863, _temp1863, _temp1863 + 4u},( struct _tagged_string){
_temp1866, _temp1866, _temp1866 + 4u},( struct _tagged_string){ _temp1869,
_temp1869, _temp1869 + 4u},( struct _tagged_string){ _temp1872, _temp1872,
_temp1872 + 4u},( struct _tagged_string){ _temp1875, _temp1875, _temp1875 + 4u},(
struct _tagged_string){ _temp1878, _temp1878, _temp1878 + 4u},( struct
_tagged_string){ _temp1881, _temp1881, _temp1881 + 4u},( struct _tagged_string){
_temp1884, _temp1884, _temp1884 + 4u},( struct _tagged_string){ _temp1887,
_temp1887, _temp1887 + 4u},( struct _tagged_string){ _temp1890, _temp1890,
_temp1890 + 4u},( struct _tagged_string){ _temp1893, _temp1893, _temp1893 + 4u},(
struct _tagged_string){ _temp1896, _temp1896, _temp1896 + 4u},( struct
_tagged_string){ _temp1899, _temp1899, _temp1899 + 4u},( struct _tagged_string){
_temp1902, _temp1902, _temp1902 + 4u},( struct _tagged_string){ _temp1905,
_temp1905, _temp1905 + 4u},( struct _tagged_string){ _temp1908, _temp1908,
_temp1908 + 4u},( struct _tagged_string){ _temp1911, _temp1911, _temp1911 + 4u},(
struct _tagged_string){ _temp1914, _temp1914, _temp1914 + 4u},( struct
_tagged_string){ _temp1917, _temp1917, _temp1917 + 4u},( struct _tagged_string){
_temp1920, _temp1920, _temp1920 + 4u},( struct _tagged_string){ _temp1923,
_temp1923, _temp1923 + 4u},( struct _tagged_string){ _temp1926, _temp1926,
_temp1926 + 5u},( struct _tagged_string){ _temp1929, _temp1929, _temp1929 + 17u},(
struct _tagged_string){ _temp1932, _temp1932, _temp1932 + 21u},( struct
_tagged_string){ _temp1935, _temp1935, _temp1935 + 20u},( struct _tagged_string){
_temp1938, _temp1938, _temp1938 + 21u},( struct _tagged_string){ _temp1941,
_temp1941, _temp1941 + 13u},( struct _tagged_string){ _temp1944, _temp1944,
_temp1944 + 15u},( struct _tagged_string){ _temp1947, _temp1947, _temp1947 + 17u},(
struct _tagged_string){ _temp1950, _temp1950, _temp1950 + 19u},( struct
_tagged_string){ _temp1953, _temp1953, _temp1953 + 12u},( struct _tagged_string){
_temp1956, _temp1956, _temp1956 + 17u},( struct _tagged_string){ _temp1959,
_temp1959, _temp1959 + 23u},( struct _tagged_string){ _temp1962, _temp1962,
_temp1962 + 24u},( struct _tagged_string){ _temp1965, _temp1965, _temp1965 + 15u},(
struct _tagged_string){ _temp1968, _temp1968, _temp1968 + 11u},( struct
_tagged_string){ _temp1971, _temp1971, _temp1971 + 15u},( struct _tagged_string){
_temp1974, _temp1974, _temp1974 + 10u},( struct _tagged_string){ _temp1977,
_temp1977, _temp1977 + 15u},( struct _tagged_string){ _temp1980, _temp1980,
_temp1980 + 5u},( struct _tagged_string){ _temp1983, _temp1983, _temp1983 + 15u},(
struct _tagged_string){ _temp1986, _temp1986, _temp1986 + 15u},( struct
_tagged_string){ _temp1989, _temp1989, _temp1989 + 11u},( struct _tagged_string){
_temp1992, _temp1992, _temp1992 + 22u},( struct _tagged_string){ _temp1995,
_temp1995, _temp1995 + 26u},( struct _tagged_string){ _temp1998, _temp1998,
_temp1998 + 16u},( struct _tagged_string){ _temp2001, _temp2001, _temp2001 + 16u},(
struct _tagged_string){ _temp2004, _temp2004, _temp2004 + 24u},( struct
_tagged_string){ _temp2007, _temp2007, _temp2007 + 25u},( struct _tagged_string){
_temp2010, _temp2010, _temp2010 + 21u},( struct _tagged_string){ _temp2013,
_temp2013, _temp2013 + 22u},( struct _tagged_string){ _temp2016, _temp2016,
_temp2016 + 16u},( struct _tagged_string){ _temp2019, _temp2019, _temp2019 + 19u},(
struct _tagged_string){ _temp2022, _temp2022, _temp2022 + 25u},( struct
_tagged_string){ _temp2025, _temp2025, _temp2025 + 23u},( struct _tagged_string){
_temp2028, _temp2028, _temp2028 + 24u},( struct _tagged_string){ _temp2031,
_temp2031, _temp2031 + 18u},( struct _tagged_string){ _temp2034, _temp2034,
_temp2034 + 17u},( struct _tagged_string){ _temp2037, _temp2037, _temp2037 + 18u},(
struct _tagged_string){ _temp2040, _temp2040, _temp2040 + 17u},( struct
_tagged_string){ _temp2043, _temp2043, _temp2043 + 18u},( struct _tagged_string){
_temp2046, _temp2046, _temp2046 + 12u},( struct _tagged_string){ _temp2049,
_temp2049, _temp2049 + 11u},( struct _tagged_string){ _temp2052, _temp2052,
_temp2052 + 18u},( struct _tagged_string){ _temp2055, _temp2055, _temp2055 + 8u},(
struct _tagged_string){ _temp2058, _temp2058, _temp2058 + 13u},( struct
_tagged_string){ _temp2061, _temp2061, _temp2061 + 8u},( struct _tagged_string){
_temp2064, _temp2064, _temp2064 + 4u},( struct _tagged_string){ _temp2067,
_temp2067, _temp2067 + 20u},( struct _tagged_string){ _temp2070, _temp2070,
_temp2070 + 20u},( struct _tagged_string){ _temp2073, _temp2073, _temp2073 + 11u},(
struct _tagged_string){ _temp2076, _temp2076, _temp2076 + 14u},( struct
_tagged_string){ _temp2079, _temp2079, _temp2079 + 11u},( struct _tagged_string){
_temp2082, _temp2082, _temp2082 + 15u},( struct _tagged_string){ _temp2085,
_temp2085, _temp2085 + 22u},( struct _tagged_string){ _temp2088, _temp2088,
_temp2088 + 16u},( struct _tagged_string){ _temp2091, _temp2091, _temp2091 + 17u},(
struct _tagged_string){ _temp2094, _temp2094, _temp2094 + 12u},( struct
_tagged_string){ _temp2097, _temp2097, _temp2097 + 18u},( struct _tagged_string){
_temp2100, _temp2100, _temp2100 + 17u},( struct _tagged_string){ _temp2103,
_temp2103, _temp2103 + 12u},( struct _tagged_string){ _temp2106, _temp2106,
_temp2106 + 16u},( struct _tagged_string){ _temp2109, _temp2109, _temp2109 + 11u},(
struct _tagged_string){ _temp2112, _temp2112, _temp2112 + 10u},( struct
_tagged_string){ _temp2115, _temp2115, _temp2115 + 14u},( struct _tagged_string){
_temp2118, _temp2118, _temp2118 + 15u},( struct _tagged_string){ _temp2121,
_temp2121, _temp2121 + 20u},( struct _tagged_string){ _temp2124, _temp2124,
_temp2124 + 27u},( struct _tagged_string){ _temp2127, _temp2127, _temp2127 + 10u},(
struct _tagged_string){ _temp2130, _temp2130, _temp2130 + 18u},( struct
_tagged_string){ _temp2133, _temp2133, _temp2133 + 21u},( struct _tagged_string){
_temp2136, _temp2136, _temp2136 + 19u},( struct _tagged_string){ _temp2139,
_temp2139, _temp2139 + 16u},( struct _tagged_string){ _temp2142, _temp2142,
_temp2142 + 11u},( struct _tagged_string){ _temp2145, _temp2145, _temp2145 + 20u},(
struct _tagged_string){ _temp2148, _temp2148, _temp2148 + 15u},( struct
_tagged_string){ _temp2151, _temp2151, _temp2151 + 20u},( struct _tagged_string){
_temp2154, _temp2154, _temp2154 + 15u},( struct _tagged_string){ _temp2157,
_temp2157, _temp2157 + 8u},( struct _tagged_string){ _temp2160, _temp2160,
_temp2160 + 19u},( struct _tagged_string){ _temp2163, _temp2163, _temp2163 + 20u},(
struct _tagged_string){ _temp2166, _temp2166, _temp2166 + 14u},( struct
_tagged_string){ _temp2169, _temp2169, _temp2169 + 19u},( struct _tagged_string){
_temp2172, _temp2172, _temp2172 + 20u},( struct _tagged_string){ _temp2175,
_temp2175, _temp2175 + 11u},( struct _tagged_string){ _temp2178, _temp2178,
_temp2178 + 22u},( struct _tagged_string){ _temp2181, _temp2181, _temp2181 + 20u},(
struct _tagged_string){ _temp2184, _temp2184, _temp2184 + 23u},( struct
_tagged_string){ _temp2187, _temp2187, _temp2187 + 20u},( struct _tagged_string){
_temp2190, _temp2190, _temp2190 + 22u},( struct _tagged_string){ _temp2193,
_temp2193, _temp2193 + 23u},( struct _tagged_string){ _temp2196, _temp2196,
_temp2196 + 24u},( struct _tagged_string){ _temp2199, _temp2199, _temp2199 + 24u},(
struct _tagged_string){ _temp2202, _temp2202, _temp2202 + 15u},( struct
_tagged_string){ _temp2205, _temp2205, _temp2205 + 20u},( struct _tagged_string){
_temp2208, _temp2208, _temp2208 + 22u},( struct _tagged_string){ _temp2211,
_temp2211, _temp2211 + 17u},( struct _tagged_string){ _temp2214, _temp2214,
_temp2214 + 20u},( struct _tagged_string){ _temp2217, _temp2217, _temp2217 + 26u},(
struct _tagged_string){ _temp2220, _temp2220, _temp2220 + 16u},( struct
_tagged_string){ _temp2223, _temp2223, _temp2223 + 17u},( struct _tagged_string){
_temp2226, _temp2226, _temp2226 + 14u},( struct _tagged_string){ _temp2229,
_temp2229, _temp2229 + 15u},( struct _tagged_string){ _temp2232, _temp2232,
_temp2232 + 19u},( struct _tagged_string){ _temp2235, _temp2235, _temp2235 + 19u},(
struct _tagged_string){ _temp2238, _temp2238, _temp2238 + 25u},( struct
_tagged_string){ _temp2241, _temp2241, _temp2241 + 26u},( struct _tagged_string){
_temp2244, _temp2244, _temp2244 + 9u},( struct _tagged_string){ _temp2247,
_temp2247, _temp2247 + 20u}}; static short Cyc_yyr1[ 394u]={ (short)0, (short)124,
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
_temp7638=( short*)({ unsigned int _temp7634= 200u; short* _temp7635=( short*)
GC_malloc_atomic( sizeof( short) * _temp7634);{ unsigned int _temp7636=
_temp7634; unsigned int i; for( i= 0; i < _temp7636; i ++){ _temp7635[ i]=
(short)0;}}; _temp7635;}); struct _tagged_ptr1 _temp7639; _temp7639.curr=
_temp7638; _temp7639.base= _temp7638; _temp7639.last_plus_one= _temp7638 + 200;
_temp7639;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=( struct _tagged_ptr2)({
void** _temp7632=( void**)({ unsigned int _temp7628= 200u; void** _temp7629=(
void**) GC_malloc( sizeof( void*) * _temp7628);{ unsigned int _temp7630=
_temp7628; unsigned int i; for( i= 0; i < _temp7630; i ++){ _temp7629[ i]= Cyc_yylval;}};
_temp7629;}); struct _tagged_ptr2 _temp7633; _temp7633.curr= _temp7632;
_temp7633.base= _temp7632; _temp7633.last_plus_one= _temp7632 + 200; _temp7633;});
int yylsp_offset; struct _tagged_ptr3 yyls=( struct _tagged_ptr3)({ struct Cyc_Yyltype*
_temp7626=( struct Cyc_Yyltype*)({ unsigned int _temp7622= 200u; struct Cyc_Yyltype*
_temp7623=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp7622);{ unsigned int _temp7624= _temp7622; unsigned int i; for( i= 0; i <
_temp7624; i ++){ _temp7623[ i]= Cyc_yynewloc();}}; _temp7623;}); struct
_tagged_ptr3 _temp7627; _temp7627.curr= _temp7626; _temp7627.base= _temp7626;
_temp7627.last_plus_one= _temp7626 + 200; _temp7627;}); int yystacksize= 200;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:({
struct _tagged_ptr1 _temp2248= yyss; short* _temp2250= _temp2248.curr +( ++
yyssp_offset); if( _temp2248.base == 0? 1:( _temp2250 < _temp2248.base? 1:
_temp2250 >= _temp2248.last_plus_one)){ _throw( Null_Exception);}* _temp2250=(
short) yystate;}); if( yyssp_offset >= yystacksize - 1){ if( yystacksize >=
10000){ Cyc_yyerror(( struct _tagged_string)({ char* _temp2251=( char*)"parser stack overflow";
struct _tagged_string _temp2252; _temp2252.curr= _temp2251; _temp2252.base=
_temp2251; _temp2252.last_plus_one= _temp2251 + 22; _temp2252;}));( void) _throw((
void*) Cyc_Yystack_overflow);} yystacksize *= 2; if( yystacksize > 10000){
yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp2281=(
unsigned int) yystacksize; short* _temp2282=( short*) GC_malloc_atomic( sizeof(
short) * _temp2281); struct _tagged_ptr1 _temp2285={ _temp2282, _temp2282,
_temp2282 + _temp2281};{ unsigned int _temp2283= _temp2281; unsigned int i; for(
i= 0; i < _temp2283; i ++){ _temp2282[ i]= (short)0;}}; _temp2285;}); struct
_tagged_ptr2 yyvs1=({ unsigned int _temp2276=( unsigned int) yystacksize; void**
_temp2277=( void**) GC_malloc( sizeof( void*) * _temp2276); struct _tagged_ptr2
_temp2280={ _temp2277, _temp2277, _temp2277 + _temp2276};{ unsigned int
_temp2278= _temp2276; unsigned int i; for( i= 0; i < _temp2278; i ++){ _temp2277[
i]= Cyc_yylval;}}; _temp2280;}); struct _tagged_ptr3 yyls1=({ unsigned int
_temp2271=( unsigned int) yystacksize; struct Cyc_Yyltype* _temp2272=( struct
Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) * _temp2271); struct
_tagged_ptr3 _temp2275={ _temp2272, _temp2272, _temp2272 + _temp2271};{
unsigned int _temp2273= _temp2271; unsigned int i; for( i= 0; i < _temp2273; i
++){ _temp2272[ i]= Cyc_yynewloc();}}; _temp2275;});{ int i= 0; for( 0; i <=
yyssp_offset; i ++){({ struct _tagged_ptr1 _temp2253= yyss1; short* _temp2255=
_temp2253.curr + i; if( _temp2253.base == 0? 1:( _temp2255 < _temp2253.base? 1:
_temp2255 >= _temp2253.last_plus_one)){ _throw( Null_Exception);}* _temp2255=({
struct _tagged_ptr1 _temp2256= yyss; short* _temp2258= _temp2256.curr + i; if(
_temp2256.base == 0? 1:( _temp2258 < _temp2256.base? 1: _temp2258 >= _temp2256.last_plus_one)){
_throw( Null_Exception);}* _temp2258;});});({ struct _tagged_ptr2 _temp2259=
yyvs1; void** _temp2261= _temp2259.curr + i; if( _temp2259.base == 0? 1:(
_temp2261 < _temp2259.base? 1: _temp2261 >= _temp2259.last_plus_one)){ _throw(
Null_Exception);}* _temp2261=({ struct _tagged_ptr2 _temp2262= yyvs; void**
_temp2264= _temp2262.curr + i; if( _temp2262.base == 0? 1:( _temp2264 <
_temp2262.base? 1: _temp2264 >= _temp2262.last_plus_one)){ _throw(
Null_Exception);}* _temp2264;});});({ struct _tagged_ptr3 _temp2265= yyls1;
struct Cyc_Yyltype* _temp2267= _temp2265.curr + i; if( _temp2265.base == 0? 1:(
_temp2267 < _temp2265.base? 1: _temp2267 >= _temp2265.last_plus_one)){ _throw(
Null_Exception);}* _temp2267=({ struct _tagged_ptr3 _temp2268= yyls; struct Cyc_Yyltype*
_temp2270= _temp2268.curr + i; if( _temp2268.base == 0? 1:( _temp2270 <
_temp2268.base? 1: _temp2270 >= _temp2268.last_plus_one)){ _throw(
Null_Exception);}* _temp2270;});});}} yyss= yyss1; yyvs= yyvs1; yyls= yyls1;}}
goto yybackup; yybackup: yyn=( int)({ short* _temp2286=( short*) Cyc_yypact;
unsigned int _temp2287= yystate; if( _temp2287 >= 751u){ _throw( Null_Exception);}
_temp2286[ _temp2287];}); if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar
== - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar=
0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 351: 0)?( int)({ short*
_temp2288=( short*) Cyc_yytranslate; unsigned int _temp2289= Cyc_yychar; if(
_temp2289 >= 352u){ _throw( Null_Exception);} _temp2288[ _temp2289];}): 232;}
yyn += yychar1; if(( yyn < 0? 1: yyn > 4222)? 1:({ short* _temp2290=( short*)
Cyc_yycheck; unsigned int _temp2291= yyn; if( _temp2291 >= 4223u){ _throw(
Null_Exception);} _temp2290[ _temp2291];}) != yychar1){ goto yydefault;} yyn=(
int)({ short* _temp2292=( short*) Cyc_yytable; unsigned int _temp2293= yyn; if(
_temp2293 >= 4223u){ _throw( Null_Exception);} _temp2292[ _temp2293];}); if( yyn
< 0){ if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if(
yyn == 0){ goto yyerrlab;}} if( yyn == 750){ return 0;} if( Cyc_yychar != 0){
Cyc_yychar= - 2;}({ struct _tagged_ptr2 _temp2294= yyvs; void** _temp2296=
_temp2294.curr +( ++ yyvsp_offset); if( _temp2294.base == 0? 1:( _temp2296 <
_temp2294.base? 1: _temp2296 >= _temp2294.last_plus_one)){ _throw(
Null_Exception);}* _temp2296= Cyc_yylval;});({ struct _tagged_ptr3 _temp2297=
yyls; struct Cyc_Yyltype* _temp2299= _temp2297.curr +( ++ yylsp_offset); if(
_temp2297.base == 0? 1:( _temp2299 < _temp2297.base? 1: _temp2299 >= _temp2297.last_plus_one)){
_throw( Null_Exception);}* _temp2299= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp2300=( short*) Cyc_yydefact; unsigned int _temp2301= yystate; if( _temp2301
>= 751u){ _throw( Null_Exception);} _temp2300[ _temp2301];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp2302=( short*) Cyc_yyr2;
unsigned int _temp2303= yyn; if( _temp2303 >= 394u){ _throw( Null_Exception);}
_temp2302[ _temp2303];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp2304=
yyvs; void** _temp2306= _temp2304.curr +(( yyvsp_offset + 1) - yylen); if(
_temp2304.base == 0? 1:( _temp2306 < _temp2304.base? 1: _temp2306 >= _temp2304.last_plus_one)){
_throw( Null_Exception);}* _temp2306;});} switch( yyn){ case 1: _LL2307: yyval=({
struct _tagged_ptr2 _temp2309= yyvs; void** _temp2311= _temp2309.curr +
yyvsp_offset; if( _temp2309.base == 0? 1:( _temp2311 < _temp2309.base? 1:
_temp2311 >= _temp2309.last_plus_one)){ _throw( Null_Exception);}* _temp2311;});
Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2312=
yyvs; void** _temp2314= _temp2312.curr + yyvsp_offset; if( _temp2312.base == 0?
1:( _temp2314 < _temp2312.base? 1: _temp2314 >= _temp2312.last_plus_one)){
_throw( Null_Exception);}* _temp2314;})); break; case 2: _LL2308: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp2316=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2316[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2317; _temp2317.tag= Cyc_DeclList_tok; _temp2317.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2318= yyvs; void** _temp2320= _temp2318.curr +(
yyvsp_offset - 1); if( _temp2318.base == 0? 1:( _temp2320 < _temp2318.base? 1:
_temp2320 >= _temp2318.last_plus_one)){ _throw( Null_Exception);}* _temp2320;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2321= yyvs; void** _temp2323=
_temp2321.curr + yyvsp_offset; if( _temp2321.base == 0? 1:( _temp2323 <
_temp2321.base? 1: _temp2323 >= _temp2321.last_plus_one)){ _throw(
Null_Exception);}* _temp2323;}))); _temp2317;}); _temp2316;}); break; case 3:
_LL2315: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2325=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2325[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2326; _temp2326.tag= Cyc_DeclList_tok; _temp2326.f1=({ struct Cyc_List_List*
_temp2327=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2327->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2328=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2328->r=( void*)(( void*)({
struct Cyc_Absyn_Using_d_struct* _temp2335=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2335[ 0]=({ struct
Cyc_Absyn_Using_d_struct _temp2336; _temp2336.tag= Cyc_Absyn_Using_d; _temp2336.f1=
Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2337= yyvs; void** _temp2339=
_temp2337.curr +( yyvsp_offset - 2); if( _temp2337.base == 0? 1:( _temp2339 <
_temp2337.base? 1: _temp2339 >= _temp2337.last_plus_one)){ _throw(
Null_Exception);}* _temp2339;})); _temp2336.f2= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2340= yyvs; void** _temp2342= _temp2340.curr + yyvsp_offset;
if( _temp2340.base == 0? 1:( _temp2342 < _temp2340.base? 1: _temp2342 >=
_temp2340.last_plus_one)){ _throw( Null_Exception);}* _temp2342;})); _temp2336;});
_temp2335;})); _temp2328->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2329= yyls; struct Cyc_Yyltype* _temp2331= _temp2329.curr +(
yylsp_offset - 2); if( _temp2329.base == 0? 1:( _temp2331 < _temp2329.base? 1:
_temp2331 >= _temp2329.last_plus_one)){ _throw( Null_Exception);}* _temp2331;}).first_line,({
struct _tagged_ptr3 _temp2332= yyls; struct Cyc_Yyltype* _temp2334= _temp2332.curr
+ yylsp_offset; if( _temp2332.base == 0? 1:( _temp2334 < _temp2332.base? 1:
_temp2334 >= _temp2332.last_plus_one)){ _throw( Null_Exception);}* _temp2334;}).last_line);
_temp2328;}); _temp2327->tl= 0; _temp2327;}); _temp2326;}); _temp2325;}); Cyc_Lex_leave_using();
break; case 4: _LL2324: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2344=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2344[ 0]=({ struct Cyc_DeclList_tok_struct _temp2345; _temp2345.tag= Cyc_DeclList_tok;
_temp2345.f1=({ struct Cyc_List_List* _temp2346=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2346->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2350=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2350->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2357=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2357[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2358; _temp2358.tag= Cyc_Absyn_Using_d;
_temp2358.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2359= yyvs; void**
_temp2361= _temp2359.curr +( yyvsp_offset - 4); if( _temp2359.base == 0? 1:(
_temp2361 < _temp2359.base? 1: _temp2361 >= _temp2359.last_plus_one)){ _throw(
Null_Exception);}* _temp2361;})); _temp2358.f2= Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2362= yyvs; void** _temp2364= _temp2362.curr +( yyvsp_offset -
2); if( _temp2362.base == 0? 1:( _temp2364 < _temp2362.base? 1: _temp2364 >=
_temp2362.last_plus_one)){ _throw( Null_Exception);}* _temp2364;})); _temp2358;});
_temp2357;})); _temp2350->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2351= yyls; struct Cyc_Yyltype* _temp2353= _temp2351.curr +(
yylsp_offset - 4); if( _temp2351.base == 0? 1:( _temp2353 < _temp2351.base? 1:
_temp2353 >= _temp2351.last_plus_one)){ _throw( Null_Exception);}* _temp2353;}).first_line,({
struct _tagged_ptr3 _temp2354= yyls; struct Cyc_Yyltype* _temp2356= _temp2354.curr
+( yylsp_offset - 1); if( _temp2354.base == 0? 1:( _temp2356 < _temp2354.base? 1:
_temp2356 >= _temp2354.last_plus_one)){ _throw( Null_Exception);}* _temp2356;}).last_line);
_temp2350;}); _temp2346->tl= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2347= yyvs; void** _temp2349= _temp2347.curr + yyvsp_offset; if( _temp2347.base
== 0? 1:( _temp2349 < _temp2347.base? 1: _temp2349 >= _temp2347.last_plus_one)){
_throw( Null_Exception);}* _temp2349;})); _temp2346;}); _temp2345;}); _temp2344;});
break; case 5: _LL2343: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2366=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2366[ 0]=({ struct Cyc_DeclList_tok_struct _temp2367; _temp2367.tag= Cyc_DeclList_tok;
_temp2367.f1=({ struct Cyc_List_List* _temp2368=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2368->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2369=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2369->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2376=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2376[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2377; _temp2377.tag=
Cyc_Absyn_Namespace_d; _temp2377.f1=({ struct _tagged_string* _temp2378=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2378[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2379= yyvs; void** _temp2381=
_temp2379.curr +( yyvsp_offset - 2); if( _temp2379.base == 0? 1:( _temp2381 <
_temp2379.base? 1: _temp2381 >= _temp2379.last_plus_one)){ _throw(
Null_Exception);}* _temp2381;})); _temp2378;}); _temp2377.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2382= yyvs; void** _temp2384= _temp2382.curr +
yyvsp_offset; if( _temp2382.base == 0? 1:( _temp2384 < _temp2382.base? 1:
_temp2384 >= _temp2382.last_plus_one)){ _throw( Null_Exception);}* _temp2384;}));
_temp2377;}); _temp2376;})); _temp2369->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2370= yyls; struct Cyc_Yyltype* _temp2372= _temp2370.curr
+( yylsp_offset - 2); if( _temp2370.base == 0? 1:( _temp2372 < _temp2370.base? 1:
_temp2372 >= _temp2370.last_plus_one)){ _throw( Null_Exception);}* _temp2372;}).first_line,({
struct _tagged_ptr3 _temp2373= yyls; struct Cyc_Yyltype* _temp2375= _temp2373.curr
+ yylsp_offset; if( _temp2373.base == 0? 1:( _temp2375 < _temp2373.base? 1:
_temp2375 >= _temp2373.last_plus_one)){ _throw( Null_Exception);}* _temp2375;}).last_line);
_temp2369;}); _temp2368->tl= 0; _temp2368;}); _temp2367;}); _temp2366;}); Cyc_Lex_leave_namespace();
break; case 6: _LL2365: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2386=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2386[ 0]=({ struct Cyc_DeclList_tok_struct _temp2387; _temp2387.tag= Cyc_DeclList_tok;
_temp2387.f1=({ struct Cyc_List_List* _temp2388=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2388->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2392=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2392->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2399=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2399[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2400; _temp2400.tag=
Cyc_Absyn_Namespace_d; _temp2400.f1=({ struct _tagged_string* _temp2401=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2401[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2402= yyvs; void** _temp2404=
_temp2402.curr +( yyvsp_offset - 4); if( _temp2402.base == 0? 1:( _temp2404 <
_temp2402.base? 1: _temp2404 >= _temp2402.last_plus_one)){ _throw(
Null_Exception);}* _temp2404;})); _temp2401;}); _temp2400.f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2405= yyvs; void** _temp2407= _temp2405.curr +(
yyvsp_offset - 2); if( _temp2405.base == 0? 1:( _temp2407 < _temp2405.base? 1:
_temp2407 >= _temp2405.last_plus_one)){ _throw( Null_Exception);}* _temp2407;}));
_temp2400;}); _temp2399;})); _temp2392->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2393= yyls; struct Cyc_Yyltype* _temp2395= _temp2393.curr
+( yylsp_offset - 4); if( _temp2393.base == 0? 1:( _temp2395 < _temp2393.base? 1:
_temp2395 >= _temp2393.last_plus_one)){ _throw( Null_Exception);}* _temp2395;}).first_line,({
struct _tagged_ptr3 _temp2396= yyls; struct Cyc_Yyltype* _temp2398= _temp2396.curr
+( yylsp_offset - 1); if( _temp2396.base == 0? 1:( _temp2398 < _temp2396.base? 1:
_temp2398 >= _temp2396.last_plus_one)){ _throw( Null_Exception);}* _temp2398;}).last_line);
_temp2392;}); _temp2388->tl= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2389= yyvs; void** _temp2391= _temp2389.curr + yyvsp_offset; if( _temp2389.base
== 0? 1:( _temp2391 < _temp2389.base? 1: _temp2391 >= _temp2389.last_plus_one)){
_throw( Null_Exception);}* _temp2391;})); _temp2388;}); _temp2387;}); _temp2386;});
break; case 7: _LL2385: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2409= yyvs; void** _temp2411= _temp2409.curr +( yyvsp_offset -
4); if( _temp2409.base == 0? 1:( _temp2411 < _temp2409.base? 1: _temp2411 >=
_temp2409.last_plus_one)){ _throw( Null_Exception);}* _temp2411;})),( struct
_tagged_string)({ char* _temp2412=( char*)"C"; struct _tagged_string _temp2413;
_temp2413.curr= _temp2412; _temp2413.base= _temp2412; _temp2413.last_plus_one=
_temp2412 + 2; _temp2413;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2414=( char*)"only extern \"C\" { ... } is allowed"; struct
_tagged_string _temp2415; _temp2415.curr= _temp2414; _temp2415.base= _temp2414;
_temp2415.last_plus_one= _temp2414 + 35; _temp2415;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2416= yyls; struct Cyc_Yyltype* _temp2418= _temp2416.curr
+( yylsp_offset - 5); if( _temp2416.base == 0? 1:( _temp2418 < _temp2416.base? 1:
_temp2418 >= _temp2416.last_plus_one)){ _throw( Null_Exception);}* _temp2418;}).first_line,({
struct _tagged_ptr3 _temp2419= yyls; struct Cyc_Yyltype* _temp2421= _temp2419.curr
+( yylsp_offset - 4); if( _temp2419.base == 0? 1:( _temp2421 < _temp2419.base? 1:
_temp2421 >= _temp2419.last_plus_one)){ _throw( Null_Exception);}* _temp2421;}).last_line));}
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2422=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2422[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2423; _temp2423.tag= Cyc_DeclList_tok; _temp2423.f1=({ struct Cyc_List_List*
_temp2424=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2424->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2428=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2428->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp2435=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp2435[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp2436; _temp2436.tag= Cyc_Absyn_ExternC_d;
_temp2436.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2437= yyvs;
void** _temp2439= _temp2437.curr +( yyvsp_offset - 2); if( _temp2437.base == 0?
1:( _temp2439 < _temp2437.base? 1: _temp2439 >= _temp2437.last_plus_one)){
_throw( Null_Exception);}* _temp2439;})); _temp2436;}); _temp2435;})); _temp2428->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2429= yyls; struct Cyc_Yyltype*
_temp2431= _temp2429.curr +( yylsp_offset - 5); if( _temp2429.base == 0? 1:(
_temp2431 < _temp2429.base? 1: _temp2431 >= _temp2429.last_plus_one)){ _throw(
Null_Exception);}* _temp2431;}).first_line,({ struct _tagged_ptr3 _temp2432=
yyls; struct Cyc_Yyltype* _temp2434= _temp2432.curr +( yylsp_offset - 1); if(
_temp2432.base == 0? 1:( _temp2434 < _temp2432.base? 1: _temp2434 >= _temp2432.last_plus_one)){
_throw( Null_Exception);}* _temp2434;}).last_line); _temp2428;}); _temp2424->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2425= yyvs; void** _temp2427=
_temp2425.curr + yyvsp_offset; if( _temp2425.base == 0? 1:( _temp2427 <
_temp2425.base? 1: _temp2427 >= _temp2425.last_plus_one)){ _throw(
Null_Exception);}* _temp2427;})); _temp2424;}); _temp2423;}); _temp2422;});
break; case 8: _LL2408: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2441=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2441[ 0]=({ struct Cyc_DeclList_tok_struct _temp2442; _temp2442.tag= Cyc_DeclList_tok;
_temp2442.f1= 0; _temp2442;}); _temp2441;}); break; case 9: _LL2440: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp2444=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2444[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2445; _temp2445.tag= Cyc_DeclList_tok; _temp2445.f1=({ struct Cyc_List_List*
_temp2446=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2446->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2447=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2447[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2448; _temp2448.tag= Cyc_Absyn_Fn_d;
_temp2448.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp2449= yyvs; void**
_temp2451= _temp2449.curr + yyvsp_offset; if( _temp2449.base == 0? 1:( _temp2451
< _temp2449.base? 1: _temp2451 >= _temp2449.last_plus_one)){ _throw(
Null_Exception);}* _temp2451;})); _temp2448;}); _temp2447;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2452= yyls; struct Cyc_Yyltype* _temp2454= _temp2452.curr
+ yylsp_offset; if( _temp2452.base == 0? 1:( _temp2454 < _temp2452.base? 1:
_temp2454 >= _temp2452.last_plus_one)){ _throw( Null_Exception);}* _temp2454;}).first_line,({
struct _tagged_ptr3 _temp2455= yyls; struct Cyc_Yyltype* _temp2457= _temp2455.curr
+ yylsp_offset; if( _temp2455.base == 0? 1:( _temp2457 < _temp2455.base? 1:
_temp2457 >= _temp2455.last_plus_one)){ _throw( Null_Exception);}* _temp2457;}).last_line));
_temp2446->tl= 0; _temp2446;}); _temp2445;}); _temp2444;}); break; case 10:
_LL2443: yyval=({ struct _tagged_ptr2 _temp2459= yyvs; void** _temp2461=
_temp2459.curr + yyvsp_offset; if( _temp2459.base == 0? 1:( _temp2461 <
_temp2459.base? 1: _temp2461 >= _temp2459.last_plus_one)){ _throw(
Null_Exception);}* _temp2461;}); break; case 11: _LL2458: yyval=( void*)({
struct Cyc_FnDecl_tok_struct* _temp2463=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2463[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2464; _temp2464.tag= Cyc_FnDecl_tok; _temp2464.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2465= yyvs; void**
_temp2467= _temp2465.curr +( yyvsp_offset - 1); if( _temp2465.base == 0? 1:(
_temp2467 < _temp2465.base? 1: _temp2467 >= _temp2465.last_plus_one)){ _throw(
Null_Exception);}* _temp2467;})), 0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2468= yyvs; void** _temp2470= _temp2468.curr + yyvsp_offset; if( _temp2468.base
== 0? 1:( _temp2470 < _temp2468.base? 1: _temp2470 >= _temp2468.last_plus_one)){
_throw( Null_Exception);}* _temp2470;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2471= yyls; struct Cyc_Yyltype* _temp2473= _temp2471.curr +(
yylsp_offset - 1); if( _temp2471.base == 0? 1:( _temp2473 < _temp2471.base? 1:
_temp2473 >= _temp2471.last_plus_one)){ _throw( Null_Exception);}* _temp2473;}).first_line,({
struct _tagged_ptr3 _temp2474= yyls; struct Cyc_Yyltype* _temp2476= _temp2474.curr
+ yylsp_offset; if( _temp2474.base == 0? 1:( _temp2476 < _temp2474.base? 1:
_temp2476 >= _temp2474.last_plus_one)){ _throw( Null_Exception);}* _temp2476;}).last_line));
_temp2464;}); _temp2463;}); break; case 12: _LL2462: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2478=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2478[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2479; _temp2479.tag= Cyc_FnDecl_tok;
_temp2479.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2480=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2480->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2481= yyvs; void** _temp2483=
_temp2481.curr +( yyvsp_offset - 2); if( _temp2481.base == 0? 1:( _temp2483 <
_temp2481.base? 1: _temp2483 >= _temp2481.last_plus_one)){ _throw(
Null_Exception);}* _temp2483;})); _temp2480;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2484= yyvs; void** _temp2486= _temp2484.curr +(
yyvsp_offset - 1); if( _temp2484.base == 0? 1:( _temp2486 < _temp2484.base? 1:
_temp2486 >= _temp2484.last_plus_one)){ _throw( Null_Exception);}* _temp2486;})),
0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2487= yyvs; void** _temp2489=
_temp2487.curr + yyvsp_offset; if( _temp2487.base == 0? 1:( _temp2489 <
_temp2487.base? 1: _temp2489 >= _temp2487.last_plus_one)){ _throw(
Null_Exception);}* _temp2489;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2490= yyls; struct Cyc_Yyltype* _temp2492= _temp2490.curr +(
yylsp_offset - 2); if( _temp2490.base == 0? 1:( _temp2492 < _temp2490.base? 1:
_temp2492 >= _temp2490.last_plus_one)){ _throw( Null_Exception);}* _temp2492;}).first_line,({
struct _tagged_ptr3 _temp2493= yyls; struct Cyc_Yyltype* _temp2495= _temp2493.curr
+ yylsp_offset; if( _temp2493.base == 0? 1:( _temp2495 < _temp2493.base? 1:
_temp2495 >= _temp2493.last_plus_one)){ _throw( Null_Exception);}* _temp2495;}).last_line));
_temp2479;}); _temp2478;}); break; case 13: _LL2477: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2497=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2497[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2498; _temp2498.tag= Cyc_FnDecl_tok;
_temp2498.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2499= yyvs; void** _temp2501= _temp2499.curr +( yyvsp_offset -
2); if( _temp2499.base == 0? 1:( _temp2501 < _temp2499.base? 1: _temp2501 >=
_temp2499.last_plus_one)){ _throw( Null_Exception);}* _temp2501;})), Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2502= yyvs; void** _temp2504= _temp2502.curr +(
yyvsp_offset - 1); if( _temp2502.base == 0? 1:( _temp2504 < _temp2502.base? 1:
_temp2504 >= _temp2502.last_plus_one)){ _throw( Null_Exception);}* _temp2504;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2505= yyvs; void** _temp2507=
_temp2505.curr + yyvsp_offset; if( _temp2505.base == 0? 1:( _temp2507 <
_temp2505.base? 1: _temp2507 >= _temp2505.last_plus_one)){ _throw(
Null_Exception);}* _temp2507;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2508= yyls; struct Cyc_Yyltype* _temp2510= _temp2508.curr +(
yylsp_offset - 2); if( _temp2508.base == 0? 1:( _temp2510 < _temp2508.base? 1:
_temp2510 >= _temp2508.last_plus_one)){ _throw( Null_Exception);}* _temp2510;}).first_line,({
struct _tagged_ptr3 _temp2511= yyls; struct Cyc_Yyltype* _temp2513= _temp2511.curr
+ yylsp_offset; if( _temp2511.base == 0? 1:( _temp2513 < _temp2511.base? 1:
_temp2513 >= _temp2511.last_plus_one)){ _throw( Null_Exception);}* _temp2513;}).last_line));
_temp2498;}); _temp2497;}); break; case 14: _LL2496: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2515=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2515[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2516; _temp2516.tag= Cyc_FnDecl_tok;
_temp2516.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2517=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2517->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2518= yyvs; void** _temp2520=
_temp2518.curr +( yyvsp_offset - 3); if( _temp2518.base == 0? 1:( _temp2520 <
_temp2518.base? 1: _temp2520 >= _temp2518.last_plus_one)){ _throw(
Null_Exception);}* _temp2520;})); _temp2517;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2521= yyvs; void** _temp2523= _temp2521.curr +(
yyvsp_offset - 2); if( _temp2521.base == 0? 1:( _temp2523 < _temp2521.base? 1:
_temp2523 >= _temp2521.last_plus_one)){ _throw( Null_Exception);}* _temp2523;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2524= yyvs; void** _temp2526=
_temp2524.curr +( yyvsp_offset - 1); if( _temp2524.base == 0? 1:( _temp2526 <
_temp2524.base? 1: _temp2526 >= _temp2524.last_plus_one)){ _throw(
Null_Exception);}* _temp2526;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2527= yyvs; void** _temp2529= _temp2527.curr + yyvsp_offset; if( _temp2527.base
== 0? 1:( _temp2529 < _temp2527.base? 1: _temp2529 >= _temp2527.last_plus_one)){
_throw( Null_Exception);}* _temp2529;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2530= yyls; struct Cyc_Yyltype* _temp2532= _temp2530.curr +(
yylsp_offset - 3); if( _temp2530.base == 0? 1:( _temp2532 < _temp2530.base? 1:
_temp2532 >= _temp2530.last_plus_one)){ _throw( Null_Exception);}* _temp2532;}).first_line,({
struct _tagged_ptr3 _temp2533= yyls; struct Cyc_Yyltype* _temp2535= _temp2533.curr
+ yylsp_offset; if( _temp2533.base == 0? 1:( _temp2535 < _temp2533.base? 1:
_temp2535 >= _temp2533.last_plus_one)){ _throw( Null_Exception);}* _temp2535;}).last_line));
_temp2516;}); _temp2515;}); break; case 15: _LL2514: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2537=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2537[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2538; _temp2538.tag= Cyc_FnDecl_tok;
_temp2538.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2539=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2539->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2540= yyvs; void** _temp2542=
_temp2540.curr +( yyvsp_offset - 2); if( _temp2540.base == 0? 1:( _temp2542 <
_temp2540.base? 1: _temp2542 >= _temp2540.last_plus_one)){ _throw(
Null_Exception);}* _temp2542;})); _temp2539;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2543= yyvs; void** _temp2545= _temp2543.curr +(
yyvsp_offset - 1); if( _temp2543.base == 0? 1:( _temp2545 < _temp2543.base? 1:
_temp2545 >= _temp2543.last_plus_one)){ _throw( Null_Exception);}* _temp2545;})),
0, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp2546= yyvs; void** _temp2548=
_temp2546.curr + yyvsp_offset; if( _temp2546.base == 0? 1:( _temp2548 <
_temp2546.base? 1: _temp2548 >= _temp2546.last_plus_one)){ _throw(
Null_Exception);}* _temp2548;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2549= yyls; struct Cyc_Yyltype* _temp2551= _temp2549.curr +(
yylsp_offset - 2); if( _temp2549.base == 0? 1:( _temp2551 < _temp2549.base? 1:
_temp2551 >= _temp2549.last_plus_one)){ _throw( Null_Exception);}* _temp2551;}).first_line,({
struct _tagged_ptr3 _temp2552= yyls; struct Cyc_Yyltype* _temp2554= _temp2552.curr
+ yylsp_offset; if( _temp2552.base == 0? 1:( _temp2554 < _temp2552.base? 1:
_temp2554 >= _temp2552.last_plus_one)){ _throw( Null_Exception);}* _temp2554;}).last_line));
_temp2538;}); _temp2537;}); break; case 16: _LL2536: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2556=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2556[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2557; _temp2557.tag= Cyc_FnDecl_tok;
_temp2557.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2558=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2558->v=( void*)
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2559= yyvs; void** _temp2561=
_temp2559.curr +( yyvsp_offset - 3); if( _temp2559.base == 0? 1:( _temp2561 <
_temp2559.base? 1: _temp2561 >= _temp2559.last_plus_one)){ _throw(
Null_Exception);}* _temp2561;})); _temp2558;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2562= yyvs; void** _temp2564= _temp2562.curr +(
yyvsp_offset - 2); if( _temp2562.base == 0? 1:( _temp2564 < _temp2562.base? 1:
_temp2564 >= _temp2562.last_plus_one)){ _throw( Null_Exception);}* _temp2564;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2565= yyvs; void** _temp2567=
_temp2565.curr +( yyvsp_offset - 1); if( _temp2565.base == 0? 1:( _temp2567 <
_temp2565.base? 1: _temp2567 >= _temp2565.last_plus_one)){ _throw(
Null_Exception);}* _temp2567;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2568= yyvs; void** _temp2570= _temp2568.curr + yyvsp_offset; if( _temp2568.base
== 0? 1:( _temp2570 < _temp2568.base? 1: _temp2570 >= _temp2568.last_plus_one)){
_throw( Null_Exception);}* _temp2570;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2571= yyls; struct Cyc_Yyltype* _temp2573= _temp2571.curr +(
yylsp_offset - 3); if( _temp2571.base == 0? 1:( _temp2573 < _temp2571.base? 1:
_temp2573 >= _temp2571.last_plus_one)){ _throw( Null_Exception);}* _temp2573;}).first_line,({
struct _tagged_ptr3 _temp2574= yyls; struct Cyc_Yyltype* _temp2576= _temp2574.curr
+ yylsp_offset; if( _temp2574.base == 0? 1:( _temp2576 < _temp2574.base? 1:
_temp2576 >= _temp2574.last_plus_one)){ _throw( Null_Exception);}* _temp2576;}).last_line));
_temp2557;}); _temp2556;}); break; case 17: _LL2555: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2578= yyvs; void** _temp2580= _temp2578.curr +
yyvsp_offset; if( _temp2578.base == 0? 1:( _temp2580 < _temp2578.base? 1:
_temp2580 >= _temp2578.last_plus_one)){ _throw( Null_Exception);}* _temp2580;})));
yyval=({ struct _tagged_ptr2 _temp2581= yyvs; void** _temp2583= _temp2581.curr +
yyvsp_offset; if( _temp2581.base == 0? 1:( _temp2583 < _temp2581.base? 1:
_temp2583 >= _temp2581.last_plus_one)){ _throw( Null_Exception);}* _temp2583;});
break; case 18: _LL2577: Cyc_Lex_leave_using(); break; case 19: _LL2584: Cyc_Lex_enter_namespace(({
struct _tagged_string* _temp2586=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp2586[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2587= yyvs; void** _temp2589= _temp2587.curr + yyvsp_offset;
if( _temp2587.base == 0? 1:( _temp2589 < _temp2587.base? 1: _temp2589 >=
_temp2587.last_plus_one)){ _throw( Null_Exception);}* _temp2589;})); _temp2586;}));
yyval=({ struct _tagged_ptr2 _temp2590= yyvs; void** _temp2592= _temp2590.curr +
yyvsp_offset; if( _temp2590.base == 0? 1:( _temp2592 < _temp2590.base? 1:
_temp2592 >= _temp2590.last_plus_one)){ _throw( Null_Exception);}* _temp2592;});
break; case 20: _LL2585: Cyc_Lex_leave_namespace(); break; case 21: _LL2593:
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2595=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2595[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2596; _temp2596.tag= Cyc_DeclList_tok; _temp2596.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2597= yyvs; void** _temp2599=
_temp2597.curr +( yyvsp_offset - 1); if( _temp2597.base == 0? 1:( _temp2599 <
_temp2597.base? 1: _temp2599 >= _temp2597.last_plus_one)){ _throw(
Null_Exception);}* _temp2599;})), 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2600= yyls; struct Cyc_Yyltype* _temp2602= _temp2600.curr +(
yylsp_offset - 1); if( _temp2600.base == 0? 1:( _temp2602 < _temp2600.base? 1:
_temp2602 >= _temp2600.last_plus_one)){ _throw( Null_Exception);}* _temp2602;}).first_line,({
struct _tagged_ptr3 _temp2603= yyls; struct Cyc_Yyltype* _temp2605= _temp2603.curr
+( yylsp_offset - 1); if( _temp2603.base == 0? 1:( _temp2605 < _temp2603.base? 1:
_temp2605 >= _temp2603.last_plus_one)){ _throw( Null_Exception);}* _temp2605;}).last_line));
_temp2596;}); _temp2595;}); break; case 22: _LL2594: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2607=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2607[ 0]=({ struct Cyc_DeclList_tok_struct _temp2608; _temp2608.tag= Cyc_DeclList_tok;
_temp2608.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2609= yyvs; void** _temp2611= _temp2609.curr +( yyvsp_offset -
2); if( _temp2609.base == 0? 1:( _temp2611 < _temp2609.base? 1: _temp2611 >=
_temp2609.last_plus_one)){ _throw( Null_Exception);}* _temp2611;})), Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp2612= yyvs; void** _temp2614= _temp2612.curr +(
yyvsp_offset - 1); if( _temp2612.base == 0? 1:( _temp2614 < _temp2612.base? 1:
_temp2614 >= _temp2612.last_plus_one)){ _throw( Null_Exception);}* _temp2614;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2615= yyls; struct Cyc_Yyltype*
_temp2617= _temp2615.curr +( yylsp_offset - 2); if( _temp2615.base == 0? 1:(
_temp2617 < _temp2615.base? 1: _temp2617 >= _temp2615.last_plus_one)){ _throw(
Null_Exception);}* _temp2617;}).first_line,({ struct _tagged_ptr3 _temp2618=
yyls; struct Cyc_Yyltype* _temp2620= _temp2618.curr + yylsp_offset; if(
_temp2618.base == 0? 1:( _temp2620 < _temp2618.base? 1: _temp2620 >= _temp2618.last_plus_one)){
_throw( Null_Exception);}* _temp2620;}).last_line)); _temp2608;}); _temp2607;});
break; case 23: _LL2606: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2622=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2622[ 0]=({ struct Cyc_DeclList_tok_struct _temp2623; _temp2623.tag= Cyc_DeclList_tok;
_temp2623.f1=({ struct Cyc_List_List* _temp2624=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2624->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp2625= yyvs; void** _temp2627=
_temp2625.curr +( yyvsp_offset - 3); if( _temp2625.base == 0? 1:( _temp2627 <
_temp2625.base? 1: _temp2627 >= _temp2625.last_plus_one)){ _throw(
Null_Exception);}* _temp2627;})), 0, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp2628= yyvs; void** _temp2630= _temp2628.curr +( yyvsp_offset - 1); if(
_temp2628.base == 0? 1:( _temp2630 < _temp2628.base? 1: _temp2630 >= _temp2628.last_plus_one)){
_throw( Null_Exception);}* _temp2630;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2631= yyls; struct Cyc_Yyltype* _temp2633= _temp2631.curr +(
yylsp_offset - 4); if( _temp2631.base == 0? 1:( _temp2633 < _temp2631.base? 1:
_temp2633 >= _temp2631.last_plus_one)){ _throw( Null_Exception);}* _temp2633;}).first_line,({
struct _tagged_ptr3 _temp2634= yyls; struct Cyc_Yyltype* _temp2636= _temp2634.curr
+ yylsp_offset; if( _temp2634.base == 0? 1:( _temp2636 < _temp2634.base? 1:
_temp2636 >= _temp2634.last_plus_one)){ _throw( Null_Exception);}* _temp2636;}).last_line));
_temp2624->tl= 0; _temp2624;}); _temp2623;}); _temp2622;}); break; case 24:
_LL2621: yyval=({ struct _tagged_ptr2 _temp2638= yyvs; void** _temp2640=
_temp2638.curr + yyvsp_offset; if( _temp2638.base == 0? 1:( _temp2640 <
_temp2638.base? 1: _temp2640 >= _temp2638.last_plus_one)){ _throw(
Null_Exception);}* _temp2640;}); break; case 25: _LL2637: yyval=( void*)({
struct Cyc_DeclList_tok_struct* _temp2642=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2642[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2643; _temp2643.tag= Cyc_DeclList_tok; _temp2643.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2644= yyvs; void** _temp2646= _temp2644.curr +(
yyvsp_offset - 1); if( _temp2644.base == 0? 1:( _temp2646 < _temp2644.base? 1:
_temp2646 >= _temp2644.last_plus_one)){ _throw( Null_Exception);}* _temp2646;})),
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2647= yyvs; void** _temp2649=
_temp2647.curr + yyvsp_offset; if( _temp2647.base == 0? 1:( _temp2649 <
_temp2647.base? 1: _temp2649 >= _temp2647.last_plus_one)){ _throw(
Null_Exception);}* _temp2649;}))); _temp2643;}); _temp2642;}); break; case 26:
_LL2641: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2651=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2651[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2652; _temp2652.tag= Cyc_DeclSpec_tok; _temp2652.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2653=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2653->sc=({ struct Cyc_Core_Opt* _temp2657=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2657->v=( void*) Cyc_yyget_StorageClass_tok(({
struct _tagged_ptr2 _temp2658= yyvs; void** _temp2660= _temp2658.curr +(
yyvsp_offset - 1); if( _temp2658.base == 0? 1:( _temp2660 < _temp2658.base? 1:
_temp2660 >= _temp2658.last_plus_one)){ _throw( Null_Exception);}* _temp2660;}));
_temp2657;}); _temp2653->tq= Cyc_Absyn_empty_tqual(); _temp2653->type_specs= 0;
_temp2653->is_inline= 0; _temp2653->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2654= yyvs; void** _temp2656= _temp2654.curr +
yyvsp_offset; if( _temp2654.base == 0? 1:( _temp2656 < _temp2654.base? 1:
_temp2656 >= _temp2654.last_plus_one)){ _throw( Null_Exception);}* _temp2656;}));
_temp2653;}); _temp2652;}); _temp2651;}); break; case 27: _LL2650: if(( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2662= yyvs; void** _temp2664= _temp2662.curr +
yyvsp_offset; if( _temp2662.base == 0? 1:( _temp2664 < _temp2662.base? 1:
_temp2664 >= _temp2662.last_plus_one)){ _throw( Null_Exception);}* _temp2664;})))->sc
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp2665=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2666; _temp2666.curr= _temp2665; _temp2666.base=
_temp2665; _temp2666.last_plus_one= _temp2665 + 51; _temp2666;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2667= yyls; struct Cyc_Yyltype* _temp2669= _temp2667.curr
+( yylsp_offset - 2); if( _temp2667.base == 0? 1:( _temp2669 < _temp2667.base? 1:
_temp2669 >= _temp2667.last_plus_one)){ _throw( Null_Exception);}* _temp2669;}).first_line,({
struct _tagged_ptr3 _temp2670= yyls; struct Cyc_Yyltype* _temp2672= _temp2670.curr
+( yylsp_offset - 1); if( _temp2670.base == 0? 1:( _temp2672 < _temp2670.base? 1:
_temp2672 >= _temp2670.last_plus_one)){ _throw( Null_Exception);}* _temp2672;}).last_line));}
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2673=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2673[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2674; _temp2674.tag= Cyc_DeclSpec_tok; _temp2674.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2675=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2675->sc=({ struct Cyc_Core_Opt* _temp2691=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2691->v=( void*) Cyc_yyget_StorageClass_tok(({
struct _tagged_ptr2 _temp2692= yyvs; void** _temp2694= _temp2692.curr +(
yyvsp_offset - 2); if( _temp2692.base == 0? 1:( _temp2694 < _temp2692.base? 1:
_temp2694 >= _temp2692.last_plus_one)){ _throw( Null_Exception);}* _temp2694;}));
_temp2691;}); _temp2675->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2688= yyvs; void** _temp2690= _temp2688.curr + yyvsp_offset; if( _temp2688.base
== 0? 1:( _temp2690 < _temp2688.base? 1: _temp2690 >= _temp2688.last_plus_one)){
_throw( Null_Exception);}* _temp2690;})))->tq; _temp2675->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2685= yyvs; void** _temp2687= _temp2685.curr +
yyvsp_offset; if( _temp2685.base == 0? 1:( _temp2687 < _temp2685.base? 1:
_temp2687 >= _temp2685.last_plus_one)){ _throw( Null_Exception);}* _temp2687;})))->type_specs;
_temp2675->is_inline=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2682=
yyvs; void** _temp2684= _temp2682.curr + yyvsp_offset; if( _temp2682.base == 0?
1:( _temp2684 < _temp2682.base? 1: _temp2684 >= _temp2682.last_plus_one)){
_throw( Null_Exception);}* _temp2684;})))->is_inline; _temp2675->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2676= yyvs; void**
_temp2678= _temp2676.curr +( yyvsp_offset - 1); if( _temp2676.base == 0? 1:(
_temp2678 < _temp2676.base? 1: _temp2678 >= _temp2676.last_plus_one)){ _throw(
Null_Exception);}* _temp2678;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2679= yyvs; void** _temp2681= _temp2679.curr + yyvsp_offset; if( _temp2679.base
== 0? 1:( _temp2681 < _temp2679.base? 1: _temp2681 >= _temp2679.last_plus_one)){
_throw( Null_Exception);}* _temp2681;})))->attributes); _temp2675;}); _temp2674;});
_temp2673;}); break; case 28: _LL2661: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2696=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2696[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2697; _temp2697.tag= Cyc_DeclSpec_tok;
_temp2697.f1=({ struct Cyc_Parse_Declaration_spec* _temp2698=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2698->sc= 0;
_temp2698->tq= Cyc_Absyn_empty_tqual(); _temp2698->type_specs=({ struct Cyc_List_List*
_temp2702=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2702->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2703= yyvs; void** _temp2705= _temp2703.curr +( yyvsp_offset - 1); if(
_temp2703.base == 0? 1:( _temp2705 < _temp2703.base? 1: _temp2705 >= _temp2703.last_plus_one)){
_throw( Null_Exception);}* _temp2705;})); _temp2702->tl= 0; _temp2702;});
_temp2698->is_inline= 0; _temp2698->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2699= yyvs; void** _temp2701= _temp2699.curr +
yyvsp_offset; if( _temp2699.base == 0? 1:( _temp2701 < _temp2699.base? 1:
_temp2701 >= _temp2699.last_plus_one)){ _throw( Null_Exception);}* _temp2701;}));
_temp2698;}); _temp2697;}); _temp2696;}); break; case 29: _LL2695: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2707=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2707[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2708; _temp2708.tag= Cyc_DeclSpec_tok; _temp2708.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2709=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2709->sc=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2729= yyvs;
void** _temp2731= _temp2729.curr + yyvsp_offset; if( _temp2729.base == 0? 1:(
_temp2731 < _temp2729.base? 1: _temp2731 >= _temp2729.last_plus_one)){ _throw(
Null_Exception);}* _temp2731;})))->sc; _temp2709->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2726= yyvs; void** _temp2728= _temp2726.curr +
yyvsp_offset; if( _temp2726.base == 0? 1:( _temp2728 < _temp2726.base? 1:
_temp2728 >= _temp2726.last_plus_one)){ _throw( Null_Exception);}* _temp2728;})))->tq;
_temp2709->type_specs=({ struct Cyc_List_List* _temp2719=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2719->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp2723= yyvs; void** _temp2725= _temp2723.curr +(
yyvsp_offset - 2); if( _temp2723.base == 0? 1:( _temp2725 < _temp2723.base? 1:
_temp2725 >= _temp2723.last_plus_one)){ _throw( Null_Exception);}* _temp2725;}));
_temp2719->tl=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2720= yyvs;
void** _temp2722= _temp2720.curr + yyvsp_offset; if( _temp2720.base == 0? 1:(
_temp2722 < _temp2720.base? 1: _temp2722 >= _temp2720.last_plus_one)){ _throw(
Null_Exception);}* _temp2722;})))->type_specs; _temp2719;}); _temp2709->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2716= yyvs; void** _temp2718=
_temp2716.curr + yyvsp_offset; if( _temp2716.base == 0? 1:( _temp2718 <
_temp2716.base? 1: _temp2718 >= _temp2716.last_plus_one)){ _throw(
Null_Exception);}* _temp2718;})))->is_inline; _temp2709->attributes=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2710= yyvs; void**
_temp2712= _temp2710.curr +( yyvsp_offset - 1); if( _temp2710.base == 0? 1:(
_temp2712 < _temp2710.base? 1: _temp2712 >= _temp2710.last_plus_one)){ _throw(
Null_Exception);}* _temp2712;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2713= yyvs; void** _temp2715= _temp2713.curr + yyvsp_offset; if( _temp2713.base
== 0? 1:( _temp2715 < _temp2713.base? 1: _temp2715 >= _temp2713.last_plus_one)){
_throw( Null_Exception);}* _temp2715;})))->attributes); _temp2709;}); _temp2708;});
_temp2707;}); break; case 30: _LL2706: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2733=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2733[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2734; _temp2734.tag= Cyc_DeclSpec_tok;
_temp2734.f1=({ struct Cyc_Parse_Declaration_spec* _temp2735=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2735->sc= 0;
_temp2735->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2739= yyvs;
void** _temp2741= _temp2739.curr +( yyvsp_offset - 1); if( _temp2739.base == 0?
1:( _temp2741 < _temp2739.base? 1: _temp2741 >= _temp2739.last_plus_one)){
_throw( Null_Exception);}* _temp2741;})); _temp2735->type_specs= 0; _temp2735->is_inline=
0; _temp2735->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2736= yyvs; void** _temp2738= _temp2736.curr + yyvsp_offset; if( _temp2736.base
== 0? 1:( _temp2738 < _temp2736.base? 1: _temp2738 >= _temp2736.last_plus_one)){
_throw( Null_Exception);}* _temp2738;})); _temp2735;}); _temp2734;}); _temp2733;});
break; case 31: _LL2732: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2743=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2743[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2744; _temp2744.tag= Cyc_DeclSpec_tok;
_temp2744.f1=({ struct Cyc_Parse_Declaration_spec* _temp2745=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2745->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2764= yyvs; void** _temp2766= _temp2764.curr +
yyvsp_offset; if( _temp2764.base == 0? 1:( _temp2766 < _temp2764.base? 1:
_temp2766 >= _temp2764.last_plus_one)){ _throw( Null_Exception);}* _temp2766;})))->sc;
_temp2745->tq= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp2758= yyvs; void** _temp2760= _temp2758.curr +( yyvsp_offset -
2); if( _temp2758.base == 0? 1:( _temp2760 < _temp2758.base? 1: _temp2760 >=
_temp2758.last_plus_one)){ _throw( Null_Exception);}* _temp2760;})),( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2761= yyvs; void** _temp2763= _temp2761.curr +
yyvsp_offset; if( _temp2761.base == 0? 1:( _temp2763 < _temp2761.base? 1:
_temp2763 >= _temp2761.last_plus_one)){ _throw( Null_Exception);}* _temp2763;})))->tq);
_temp2745->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2755=
yyvs; void** _temp2757= _temp2755.curr + yyvsp_offset; if( _temp2755.base == 0?
1:( _temp2757 < _temp2755.base? 1: _temp2757 >= _temp2755.last_plus_one)){
_throw( Null_Exception);}* _temp2757;})))->type_specs; _temp2745->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2752= yyvs; void** _temp2754=
_temp2752.curr + yyvsp_offset; if( _temp2752.base == 0? 1:( _temp2754 <
_temp2752.base? 1: _temp2754 >= _temp2752.last_plus_one)){ _throw(
Null_Exception);}* _temp2754;})))->is_inline; _temp2745->attributes=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2746= yyvs; void**
_temp2748= _temp2746.curr +( yyvsp_offset - 1); if( _temp2746.base == 0? 1:(
_temp2748 < _temp2746.base? 1: _temp2748 >= _temp2746.last_plus_one)){ _throw(
Null_Exception);}* _temp2748;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2
_temp2749= yyvs; void** _temp2751= _temp2749.curr + yyvsp_offset; if( _temp2749.base
== 0? 1:( _temp2751 < _temp2749.base? 1: _temp2751 >= _temp2749.last_plus_one)){
_throw( Null_Exception);}* _temp2751;})))->attributes); _temp2745;}); _temp2744;});
_temp2743;}); break; case 32: _LL2742: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2768=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2768[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2769; _temp2769.tag= Cyc_DeclSpec_tok;
_temp2769.f1=({ struct Cyc_Parse_Declaration_spec* _temp2770=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2770->sc= 0;
_temp2770->tq= Cyc_Absyn_empty_tqual(); _temp2770->type_specs= 0; _temp2770->is_inline=
1; _temp2770->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2771= yyvs; void** _temp2773= _temp2771.curr + yyvsp_offset; if( _temp2771.base
== 0? 1:( _temp2773 < _temp2771.base? 1: _temp2773 >= _temp2771.last_plus_one)){
_throw( Null_Exception);}* _temp2773;})); _temp2770;}); _temp2769;}); _temp2768;});
break; case 33: _LL2767: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2775=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2775[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2776; _temp2776.tag= Cyc_DeclSpec_tok;
_temp2776.f1=({ struct Cyc_Parse_Declaration_spec* _temp2777=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2777->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2790= yyvs; void** _temp2792= _temp2790.curr +
yyvsp_offset; if( _temp2790.base == 0? 1:( _temp2792 < _temp2790.base? 1:
_temp2792 >= _temp2790.last_plus_one)){ _throw( Null_Exception);}* _temp2792;})))->sc;
_temp2777->tq=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2787= yyvs;
void** _temp2789= _temp2787.curr + yyvsp_offset; if( _temp2787.base == 0? 1:(
_temp2789 < _temp2787.base? 1: _temp2789 >= _temp2787.last_plus_one)){ _throw(
Null_Exception);}* _temp2789;})))->tq; _temp2777->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2784= yyvs; void** _temp2786= _temp2784.curr +
yyvsp_offset; if( _temp2784.base == 0? 1:( _temp2786 < _temp2784.base? 1:
_temp2786 >= _temp2784.last_plus_one)){ _throw( Null_Exception);}* _temp2786;})))->type_specs;
_temp2777->is_inline= 1; _temp2777->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2778= yyvs; void** _temp2780= _temp2778.curr +(
yyvsp_offset - 1); if( _temp2778.base == 0? 1:( _temp2780 < _temp2778.base? 1:
_temp2780 >= _temp2778.last_plus_one)){ _throw( Null_Exception);}* _temp2780;})),(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2781= yyvs; void** _temp2783=
_temp2781.curr + yyvsp_offset; if( _temp2781.base == 0? 1:( _temp2783 <
_temp2781.base? 1: _temp2783 >= _temp2781.last_plus_one)){ _throw(
Null_Exception);}* _temp2783;})))->attributes); _temp2777;}); _temp2776;});
_temp2775;}); break; case 34: _LL2774: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2794=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2794[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2795; _temp2795.tag=
Cyc_StorageClass_tok; _temp2795.f1=( void*)(( void*) Cyc_Parse_Auto_sc);
_temp2795;}); _temp2794;}); break; case 35: _LL2793: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2797=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2797[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2798; _temp2798.tag=
Cyc_StorageClass_tok; _temp2798.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp2798;}); _temp2797;}); break; case 36: _LL2796: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2800=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2800[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2801; _temp2801.tag=
Cyc_StorageClass_tok; _temp2801.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2801;}); _temp2800;}); break; case 37: _LL2799: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2803=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2803[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2804; _temp2804.tag=
Cyc_StorageClass_tok; _temp2804.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp2804;}); _temp2803;}); break; case 38: _LL2802: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2806= yyvs; void** _temp2808= _temp2806.curr +
yyvsp_offset; if( _temp2806.base == 0? 1:( _temp2808 < _temp2806.base? 1:
_temp2808 >= _temp2806.last_plus_one)){ _throw( Null_Exception);}* _temp2808;})),(
struct _tagged_string)({ char* _temp2809=( char*)"C"; struct _tagged_string
_temp2810; _temp2810.curr= _temp2809; _temp2810.base= _temp2809; _temp2810.last_plus_one=
_temp2809 + 2; _temp2810;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2811=( char*)"only extern or extern \"C\" is allowed"; struct
_tagged_string _temp2812; _temp2812.curr= _temp2811; _temp2812.base= _temp2811;
_temp2812.last_plus_one= _temp2811 + 37; _temp2812;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2813= yyls; struct Cyc_Yyltype* _temp2815= _temp2813.curr
+( yylsp_offset - 1); if( _temp2813.base == 0? 1:( _temp2815 < _temp2813.base? 1:
_temp2815 >= _temp2813.last_plus_one)){ _throw( Null_Exception);}* _temp2815;}).first_line,({
struct _tagged_ptr3 _temp2816= yyls; struct Cyc_Yyltype* _temp2818= _temp2816.curr
+ yylsp_offset; if( _temp2816.base == 0? 1:( _temp2818 < _temp2816.base? 1:
_temp2818 >= _temp2816.last_plus_one)){ _throw( Null_Exception);}* _temp2818;}).last_line));}
yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp2819=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2819[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2820; _temp2820.tag= Cyc_StorageClass_tok;
_temp2820.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp2820;}); _temp2819;});
break; case 39: _LL2805: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2822=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2822[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2823; _temp2823.tag=
Cyc_StorageClass_tok; _temp2823.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp2823;}); _temp2822;}); break; case 40: _LL2821: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2825=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2825[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2826; _temp2826.tag=
Cyc_StorageClass_tok; _temp2826.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2826;}); _temp2825;}); break; case 41: _LL2824: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2828=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2828[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2829; _temp2829.tag=
Cyc_AttributeList_tok; _temp2829.f1= 0; _temp2829;}); _temp2828;}); break; case
42: _LL2827: yyval=({ struct _tagged_ptr2 _temp2831= yyvs; void** _temp2833=
_temp2831.curr + yyvsp_offset; if( _temp2831.base == 0? 1:( _temp2833 <
_temp2831.base? 1: _temp2833 >= _temp2831.last_plus_one)){ _throw(
Null_Exception);}* _temp2833;}); break; case 43: _LL2830: yyval=( void*)({
struct Cyc_AttributeList_tok_struct* _temp2835=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2835[ 0]=({
struct Cyc_AttributeList_tok_struct _temp2836; _temp2836.tag= Cyc_AttributeList_tok;
_temp2836.f1= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp2837= yyvs;
void** _temp2839= _temp2837.curr +( yyvsp_offset - 2); if( _temp2837.base == 0?
1:( _temp2839 < _temp2837.base? 1: _temp2839 >= _temp2837.last_plus_one)){
_throw( Null_Exception);}* _temp2839;})); _temp2836;}); _temp2835;}); break;
case 44: _LL2834: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2841=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2841[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2842; _temp2842.tag=
Cyc_AttributeList_tok; _temp2842.f1=({ struct Cyc_List_List* _temp2843=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2843->hd=( void*)
Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2844= yyvs; void** _temp2846=
_temp2844.curr + yyvsp_offset; if( _temp2844.base == 0? 1:( _temp2846 <
_temp2844.base? 1: _temp2846 >= _temp2844.last_plus_one)){ _throw(
Null_Exception);}* _temp2846;})); _temp2843->tl= 0; _temp2843;}); _temp2842;});
_temp2841;}); break; case 45: _LL2840: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2848=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2848[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2849; _temp2849.tag=
Cyc_AttributeList_tok; _temp2849.f1=({ struct Cyc_List_List* _temp2850=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2850->hd=( void*)
Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2854= yyvs; void** _temp2856=
_temp2854.curr +( yyvsp_offset - 2); if( _temp2854.base == 0? 1:( _temp2856 <
_temp2854.base? 1: _temp2856 >= _temp2854.last_plus_one)){ _throw(
Null_Exception);}* _temp2856;})); _temp2850->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2851= yyvs; void** _temp2853= _temp2851.curr +
yyvsp_offset; if( _temp2851.base == 0? 1:( _temp2853 < _temp2851.base? 1:
_temp2853 >= _temp2851.last_plus_one)){ _throw( Null_Exception);}* _temp2853;}));
_temp2850;}); _temp2849;}); _temp2848;}); break; case 46: _LL2847: { struct
_tagged_string _temp2861= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2858=
yyvs; void** _temp2860= _temp2858.curr + yyvsp_offset; if( _temp2858.base == 0?
1:( _temp2860 < _temp2858.base? 1: _temp2860 >= _temp2858.last_plus_one)){
_throw( Null_Exception);}* _temp2860;})); goto _LL2862; _LL2862: { void* a; if(
Cyc_String_zstrcmp( _temp2861,( struct _tagged_string)({ char* _temp2863=( char*)"stdcall";
struct _tagged_string _temp2864; _temp2864.curr= _temp2863; _temp2864.base=
_temp2863; _temp2864.last_plus_one= _temp2863 + 8; _temp2864;})) == 0? 1: Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2865=( char*)"__stdcall__";
struct _tagged_string _temp2866; _temp2866.curr= _temp2865; _temp2866.base=
_temp2865; _temp2866.last_plus_one= _temp2865 + 12; _temp2866;})) == 0){ a=(
void*) Cyc_Absyn_Stdcall_att;} else{ if( Cyc_String_zstrcmp( _temp2861,( struct
_tagged_string)({ char* _temp2867=( char*)"cdecl"; struct _tagged_string
_temp2868; _temp2868.curr= _temp2867; _temp2868.base= _temp2867; _temp2868.last_plus_one=
_temp2867 + 6; _temp2868;})) == 0? 1: Cyc_String_zstrcmp( _temp2861,( struct
_tagged_string)({ char* _temp2869=( char*)"__cdecl__"; struct _tagged_string
_temp2870; _temp2870.curr= _temp2869; _temp2870.base= _temp2869; _temp2870.last_plus_one=
_temp2869 + 10; _temp2870;})) == 0){ a=( void*) Cyc_Absyn_Cdecl_att;} else{ if(
Cyc_String_zstrcmp( _temp2861,( struct _tagged_string)({ char* _temp2871=( char*)"noreturn";
struct _tagged_string _temp2872; _temp2872.curr= _temp2871; _temp2872.base=
_temp2871; _temp2872.last_plus_one= _temp2871 + 9; _temp2872;})) == 0? 1: Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2873=( char*)"__noreturn__";
struct _tagged_string _temp2874; _temp2874.curr= _temp2873; _temp2874.base=
_temp2873; _temp2874.last_plus_one= _temp2873 + 13; _temp2874;})) == 0){ a=(
void*) Cyc_Absyn_Noreturn_att;} else{ if( Cyc_String_zstrcmp( _temp2861,( struct
_tagged_string)({ char* _temp2875=( char*)"noreturn"; struct _tagged_string
_temp2876; _temp2876.curr= _temp2875; _temp2876.base= _temp2875; _temp2876.last_plus_one=
_temp2875 + 9; _temp2876;})) == 0? 1: Cyc_String_zstrcmp( _temp2861,( struct
_tagged_string)({ char* _temp2877=( char*)"__noreturn__"; struct _tagged_string
_temp2878; _temp2878.curr= _temp2877; _temp2878.base= _temp2877; _temp2878.last_plus_one=
_temp2877 + 13; _temp2878;})) == 0){ a=( void*) Cyc_Absyn_Noreturn_att;} else{
if( Cyc_String_zstrcmp( _temp2861,( struct _tagged_string)({ char* _temp2879=(
char*)"__const__"; struct _tagged_string _temp2880; _temp2880.curr= _temp2879;
_temp2880.base= _temp2879; _temp2880.last_plus_one= _temp2879 + 10; _temp2880;}))
== 0){ a=( void*) Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( _temp2861,(
struct _tagged_string)({ char* _temp2881=( char*)"aligned"; struct
_tagged_string _temp2882; _temp2882.curr= _temp2881; _temp2882.base= _temp2881;
_temp2882.last_plus_one= _temp2881 + 8; _temp2882;})) == 0? 1: Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2883=( char*)"__aligned__";
struct _tagged_string _temp2884; _temp2884.curr= _temp2883; _temp2884.base=
_temp2883; _temp2884.last_plus_one= _temp2883 + 12; _temp2884;})) == 0){ a=(
void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2885=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2885[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2886; _temp2886.tag= Cyc_Absyn_Aligned_att;
_temp2886.f1= - 1; _temp2886;}); _temp2885;});} else{ if( Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2887=( char*)"packed"; struct
_tagged_string _temp2888; _temp2888.curr= _temp2887; _temp2888.base= _temp2887;
_temp2888.last_plus_one= _temp2887 + 7; _temp2888;})) == 0? 1: Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2889=( char*)"__packed__";
struct _tagged_string _temp2890; _temp2890.curr= _temp2889; _temp2890.base=
_temp2889; _temp2890.last_plus_one= _temp2889 + 11; _temp2890;})) == 0){ a=(
void*) Cyc_Absyn_Packed_att;} else{ if( Cyc_String_zstrcmp( _temp2861,( struct
_tagged_string)({ char* _temp2891=( char*)"shared"; struct _tagged_string
_temp2892; _temp2892.curr= _temp2891; _temp2892.base= _temp2891; _temp2892.last_plus_one=
_temp2891 + 7; _temp2892;})) == 0? 1: Cyc_String_zstrcmp( _temp2861,( struct
_tagged_string)({ char* _temp2893=( char*)"__shared__"; struct _tagged_string
_temp2894; _temp2894.curr= _temp2893; _temp2894.base= _temp2893; _temp2894.last_plus_one=
_temp2893 + 11; _temp2894;})) == 0){ a=( void*) Cyc_Absyn_Shared_att;} else{ if(
Cyc_String_zstrcmp( _temp2861,( struct _tagged_string)({ char* _temp2895=( char*)"unused";
struct _tagged_string _temp2896; _temp2896.curr= _temp2895; _temp2896.base=
_temp2895; _temp2896.last_plus_one= _temp2895 + 7; _temp2896;})) == 0? 1: Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2897=( char*)"__unused__";
struct _tagged_string _temp2898; _temp2898.curr= _temp2897; _temp2898.base=
_temp2897; _temp2898.last_plus_one= _temp2897 + 11; _temp2898;})) == 0){ a=(
void*) Cyc_Absyn_Unused_att;} else{ if( Cyc_String_zstrcmp( _temp2861,( struct
_tagged_string)({ char* _temp2899=( char*)"weak"; struct _tagged_string
_temp2900; _temp2900.curr= _temp2899; _temp2900.base= _temp2899; _temp2900.last_plus_one=
_temp2899 + 5; _temp2900;})) == 0? 1: Cyc_String_zstrcmp( _temp2861,( struct
_tagged_string)({ char* _temp2901=( char*)"__weak__"; struct _tagged_string
_temp2902; _temp2902.curr= _temp2901; _temp2902.base= _temp2901; _temp2902.last_plus_one=
_temp2901 + 9; _temp2902;})) == 0){ a=( void*) Cyc_Absyn_Weak_att;} else{ if(
Cyc_String_zstrcmp( _temp2861,( struct _tagged_string)({ char* _temp2903=( char*)"dllimport";
struct _tagged_string _temp2904; _temp2904.curr= _temp2903; _temp2904.base=
_temp2903; _temp2904.last_plus_one= _temp2903 + 10; _temp2904;})) == 0? 1: Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2905=( char*)"__dllimport__";
struct _tagged_string _temp2906; _temp2906.curr= _temp2905; _temp2906.base=
_temp2905; _temp2906.last_plus_one= _temp2905 + 14; _temp2906;})) == 0){ a=(
void*) Cyc_Absyn_Dllimport_att;} else{ if( Cyc_String_zstrcmp( _temp2861,(
struct _tagged_string)({ char* _temp2907=( char*)"dllexport"; struct
_tagged_string _temp2908; _temp2908.curr= _temp2907; _temp2908.base= _temp2907;
_temp2908.last_plus_one= _temp2907 + 10; _temp2908;})) == 0? 1: Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2909=( char*)"__dllexport__";
struct _tagged_string _temp2910; _temp2910.curr= _temp2909; _temp2910.base=
_temp2909; _temp2910.last_plus_one= _temp2909 + 14; _temp2910;})) == 0){ a=(
void*) Cyc_Absyn_Dllexport_att;} else{ if( Cyc_String_zstrcmp( _temp2861,(
struct _tagged_string)({ char* _temp2911=( char*)"no_instrument_function";
struct _tagged_string _temp2912; _temp2912.curr= _temp2911; _temp2912.base=
_temp2911; _temp2912.last_plus_one= _temp2911 + 23; _temp2912;})) == 0? 1: Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2913=( char*)"__no_instrument_function__";
struct _tagged_string _temp2914; _temp2914.curr= _temp2913; _temp2914.base=
_temp2913; _temp2914.last_plus_one= _temp2913 + 27; _temp2914;})) == 0){ a=(
void*) Cyc_Absyn_No_instrument_function_att;} else{ if( Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2915=( char*)"constructor";
struct _tagged_string _temp2916; _temp2916.curr= _temp2915; _temp2916.base=
_temp2915; _temp2916.last_plus_one= _temp2915 + 12; _temp2916;})) == 0? 1: Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2917=( char*)"__constructor__";
struct _tagged_string _temp2918; _temp2918.curr= _temp2917; _temp2918.base=
_temp2917; _temp2918.last_plus_one= _temp2917 + 16; _temp2918;})) == 0){ a=(
void*) Cyc_Absyn_Constructor_att;} else{ if( Cyc_String_zstrcmp( _temp2861,(
struct _tagged_string)({ char* _temp2919=( char*)"destructor"; struct
_tagged_string _temp2920; _temp2920.curr= _temp2919; _temp2920.base= _temp2919;
_temp2920.last_plus_one= _temp2919 + 11; _temp2920;})) == 0? 1: Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2921=( char*)"__destructor__";
struct _tagged_string _temp2922; _temp2922.curr= _temp2921; _temp2922.base=
_temp2921; _temp2922.last_plus_one= _temp2921 + 15; _temp2922;})) == 0){ a=(
void*) Cyc_Absyn_Destructor_att;} else{ if( Cyc_String_zstrcmp( _temp2861,(
struct _tagged_string)({ char* _temp2923=( char*)"no_check_memory_usage"; struct
_tagged_string _temp2924; _temp2924.curr= _temp2923; _temp2924.base= _temp2923;
_temp2924.last_plus_one= _temp2923 + 22; _temp2924;})) == 0? 1: Cyc_String_zstrcmp(
_temp2861,( struct _tagged_string)({ char* _temp2925=( char*)"__no_check_memory_usage__";
struct _tagged_string _temp2926; _temp2926.curr= _temp2925; _temp2926.base=
_temp2925; _temp2926.last_plus_one= _temp2925 + 26; _temp2926;})) == 0){ a=(
void*) Cyc_Absyn_No_check_memory_usage_att;} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2927=( char*)"unrecognized attribute"; struct
_tagged_string _temp2928; _temp2928.curr= _temp2927; _temp2928.base= _temp2927;
_temp2928.last_plus_one= _temp2927 + 23; _temp2928;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2929= yyls; struct Cyc_Yyltype* _temp2931= _temp2929.curr
+ yylsp_offset; if( _temp2929.base == 0? 1:( _temp2931 < _temp2929.base? 1:
_temp2931 >= _temp2929.last_plus_one)){ _throw( Null_Exception);}* _temp2931;}).first_line,({
struct _tagged_ptr3 _temp2932= yyls; struct Cyc_Yyltype* _temp2934= _temp2932.curr
+ yylsp_offset; if( _temp2932.base == 0? 1:( _temp2934 < _temp2932.base? 1:
_temp2934 >= _temp2932.last_plus_one)){ _throw( Null_Exception);}* _temp2934;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2935=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2935[ 0]=({ struct Cyc_Attribute_tok_struct _temp2936; _temp2936.tag= Cyc_Attribute_tok;
_temp2936.f1=( void*) a; _temp2936;}); _temp2935;}); break;}} case 47: _LL2857:
yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2938=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2938[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2939; _temp2939.tag= Cyc_Attribute_tok; _temp2939.f1=(
void*)(( void*) Cyc_Absyn_Const_att); _temp2939;}); _temp2938;}); break; case 48:
_LL2937: { struct _tagged_string _temp2944= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2941= yyvs; void** _temp2943= _temp2941.curr +( yyvsp_offset -
3); if( _temp2941.base == 0? 1:( _temp2943 < _temp2941.base? 1: _temp2943 >=
_temp2941.last_plus_one)){ _throw( Null_Exception);}* _temp2943;})); goto
_LL2945; _LL2945: { struct _tuple15 _temp2951; int _temp2952; void* _temp2954;
struct _tuple15* _temp2949= Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp2946=
yyvs; void** _temp2948= _temp2946.curr +( yyvsp_offset - 1); if( _temp2946.base
== 0? 1:( _temp2948 < _temp2946.base? 1: _temp2948 >= _temp2946.last_plus_one)){
_throw( Null_Exception);}* _temp2948;})); _temp2951=* _temp2949; _LL2955:
_temp2954= _temp2951.f1; goto _LL2953; _LL2953: _temp2952= _temp2951.f2; goto
_LL2950; _LL2950: { void* a; if( Cyc_String_zstrcmp( _temp2944,( struct
_tagged_string)({ char* _temp2956=( char*)"regparm"; struct _tagged_string
_temp2957; _temp2957.curr= _temp2956; _temp2957.base= _temp2956; _temp2957.last_plus_one=
_temp2956 + 8; _temp2957;})) == 0? 1: Cyc_String_zstrcmp( _temp2944,( struct
_tagged_string)({ char* _temp2958=( char*)"__regparm__"; struct _tagged_string
_temp2959; _temp2959.curr= _temp2958; _temp2959.base= _temp2958; _temp2959.last_plus_one=
_temp2958 + 12; _temp2959;})) == 0){ if( _temp2952 <= 0? 1: _temp2952 > 3){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2960=( char*)"regparm requires value between 1 and 3";
struct _tagged_string _temp2961; _temp2961.curr= _temp2960; _temp2961.base=
_temp2960; _temp2961.last_plus_one= _temp2960 + 39; _temp2961;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2962= yyls; struct Cyc_Yyltype* _temp2964= _temp2962.curr
+( yylsp_offset - 1); if( _temp2962.base == 0? 1:( _temp2964 < _temp2962.base? 1:
_temp2964 >= _temp2962.last_plus_one)){ _throw( Null_Exception);}* _temp2964;}).first_line,({
struct _tagged_ptr3 _temp2965= yyls; struct Cyc_Yyltype* _temp2967= _temp2965.curr
+( yylsp_offset - 1); if( _temp2965.base == 0? 1:( _temp2967 < _temp2965.base? 1:
_temp2967 >= _temp2965.last_plus_one)){ _throw( Null_Exception);}* _temp2967;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2968=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2968[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2969; _temp2969.tag= Cyc_Absyn_Regparm_att;
_temp2969.f1= _temp2952; _temp2969;}); _temp2968;});} else{ if( Cyc_String_zstrcmp(
_temp2944,( struct _tagged_string)({ char* _temp2970=( char*)"aligned"; struct
_tagged_string _temp2971; _temp2971.curr= _temp2970; _temp2971.base= _temp2970;
_temp2971.last_plus_one= _temp2970 + 8; _temp2971;})) == 0? 1: Cyc_String_zstrcmp(
_temp2944,( struct _tagged_string)({ char* _temp2972=( char*)"__aligned__";
struct _tagged_string _temp2973; _temp2973.curr= _temp2972; _temp2973.base=
_temp2972; _temp2973.last_plus_one= _temp2972 + 12; _temp2973;})) == 0){ if(
_temp2952 < 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2974=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2975; _temp2975.curr= _temp2974; _temp2975.base=
_temp2974; _temp2975.last_plus_one= _temp2974 + 39; _temp2975;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2976= yyls; struct Cyc_Yyltype* _temp2978= _temp2976.curr
+( yylsp_offset - 1); if( _temp2976.base == 0? 1:( _temp2978 < _temp2976.base? 1:
_temp2978 >= _temp2976.last_plus_one)){ _throw( Null_Exception);}* _temp2978;}).first_line,({
struct _tagged_ptr3 _temp2979= yyls; struct Cyc_Yyltype* _temp2981= _temp2979.curr
+( yylsp_offset - 1); if( _temp2979.base == 0? 1:( _temp2981 < _temp2979.base? 1:
_temp2981 >= _temp2979.last_plus_one)){ _throw( Null_Exception);}* _temp2981;}).last_line));}{
unsigned int j=( unsigned int) _temp2952; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2982=(
char*)"aligned requires positive power of two"; struct _tagged_string _temp2983;
_temp2983.curr= _temp2982; _temp2983.base= _temp2982; _temp2983.last_plus_one=
_temp2982 + 39; _temp2983;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2984= yyls; struct Cyc_Yyltype* _temp2986= _temp2984.curr +( yylsp_offset -
1); if( _temp2984.base == 0? 1:( _temp2986 < _temp2984.base? 1: _temp2986 >=
_temp2984.last_plus_one)){ _throw( Null_Exception);}* _temp2986;}).first_line,({
struct _tagged_ptr3 _temp2987= yyls; struct Cyc_Yyltype* _temp2989= _temp2987.curr
+( yylsp_offset - 1); if( _temp2987.base == 0? 1:( _temp2989 < _temp2987.base? 1:
_temp2989 >= _temp2987.last_plus_one)){ _throw( Null_Exception);}* _temp2989;}).last_line));}
a=( void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2990=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2990[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2991; _temp2991.tag= Cyc_Absyn_Aligned_att;
_temp2991.f1= _temp2952; _temp2991;}); _temp2990;});}} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2992=( char*)"unrecognized attribute";
struct _tagged_string _temp2993; _temp2993.curr= _temp2992; _temp2993.base=
_temp2992; _temp2993.last_plus_one= _temp2992 + 23; _temp2993;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2994= yyls; struct Cyc_Yyltype* _temp2996= _temp2994.curr
+( yylsp_offset - 3); if( _temp2994.base == 0? 1:( _temp2996 < _temp2994.base? 1:
_temp2996 >= _temp2994.last_plus_one)){ _throw( Null_Exception);}* _temp2996;}).first_line,({
struct _tagged_ptr3 _temp2997= yyls; struct Cyc_Yyltype* _temp2999= _temp2997.curr
+( yylsp_offset - 3); if( _temp2997.base == 0? 1:( _temp2999 < _temp2997.base? 1:
_temp2999 >= _temp2997.last_plus_one)){ _throw( Null_Exception);}* _temp2999;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp3000=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp3000[ 0]=({ struct Cyc_Attribute_tok_struct _temp3001; _temp3001.tag= Cyc_Attribute_tok;
_temp3001.f1=( void*) a; _temp3001;}); _temp3000;}); break;}}} case 49: _LL2940: {
struct _tagged_string _temp3006= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp3003= yyvs; void** _temp3005= _temp3003.curr +( yyvsp_offset - 3); if(
_temp3003.base == 0? 1:( _temp3005 < _temp3003.base? 1: _temp3005 >= _temp3003.last_plus_one)){
_throw( Null_Exception);}* _temp3005;})); goto _LL3007; _LL3007: { struct
_tagged_string _temp3011= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3008=
yyvs; void** _temp3010= _temp3008.curr +( yyvsp_offset - 1); if( _temp3008.base
== 0? 1:( _temp3010 < _temp3008.base? 1: _temp3010 >= _temp3008.last_plus_one)){
_throw( Null_Exception);}* _temp3010;})); goto _LL3012; _LL3012: { void* a; if(
Cyc_String_zstrcmp( _temp3006,( struct _tagged_string)({ char* _temp3013=( char*)"section";
struct _tagged_string _temp3014; _temp3014.curr= _temp3013; _temp3014.base=
_temp3013; _temp3014.last_plus_one= _temp3013 + 8; _temp3014;})) == 0? 1: Cyc_String_zstrcmp(
_temp3006,( struct _tagged_string)({ char* _temp3015=( char*)"__section__";
struct _tagged_string _temp3016; _temp3016.curr= _temp3015; _temp3016.base=
_temp3015; _temp3016.last_plus_one= _temp3015 + 12; _temp3016;}))){ a=( void*)({
struct Cyc_Absyn_Section_att_struct* _temp3017=( struct Cyc_Absyn_Section_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct)); _temp3017[ 0]=({
struct Cyc_Absyn_Section_att_struct _temp3018; _temp3018.tag= Cyc_Absyn_Section_att;
_temp3018.f1= _temp3011; _temp3018;}); _temp3017;});} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3019=( char*)"unrecognized attribute";
struct _tagged_string _temp3020; _temp3020.curr= _temp3019; _temp3020.base=
_temp3019; _temp3020.last_plus_one= _temp3019 + 23; _temp3020;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3021= yyls; struct Cyc_Yyltype* _temp3023= _temp3021.curr
+( yylsp_offset - 3); if( _temp3021.base == 0? 1:( _temp3023 < _temp3021.base? 1:
_temp3023 >= _temp3021.last_plus_one)){ _throw( Null_Exception);}* _temp3023;}).first_line,({
struct _tagged_ptr3 _temp3024= yyls; struct Cyc_Yyltype* _temp3026= _temp3024.curr
+( yylsp_offset - 3); if( _temp3024.base == 0? 1:( _temp3026 < _temp3024.base? 1:
_temp3026 >= _temp3024.last_plus_one)){ _throw( Null_Exception);}* _temp3026;}).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp3027=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp3027[ 0]=({ struct Cyc_Attribute_tok_struct _temp3028; _temp3028.tag= Cyc_Attribute_tok;
_temp3028.f1=( void*) a; _temp3028;}); _temp3027;}); break;}}} case 50: _LL3002:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3030=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3030[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3031; _temp3031.tag= Cyc_TypeSpecifier_tok;
_temp3031.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3032= yyls; struct Cyc_Yyltype* _temp3034= _temp3032.curr
+ yylsp_offset; if( _temp3032.base == 0? 1:( _temp3034 < _temp3032.base? 1:
_temp3034 >= _temp3032.last_plus_one)){ _throw( Null_Exception);}* _temp3034;}).first_line,({
struct _tagged_ptr3 _temp3035= yyls; struct Cyc_Yyltype* _temp3037= _temp3035.curr
+ yylsp_offset; if( _temp3035.base == 0? 1:( _temp3037 < _temp3035.base? 1:
_temp3037 >= _temp3035.last_plus_one)){ _throw( Null_Exception);}* _temp3037;}).last_line));
_temp3031;}); _temp3030;}); break; case 51: _LL3029: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3039=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3039[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3040; _temp3040.tag=
Cyc_TypeSpecifier_tok; _temp3040.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3041= yyls; struct Cyc_Yyltype* _temp3043= _temp3041.curr + yylsp_offset;
if( _temp3041.base == 0? 1:( _temp3043 < _temp3041.base? 1: _temp3043 >=
_temp3041.last_plus_one)){ _throw( Null_Exception);}* _temp3043;}).first_line,({
struct _tagged_ptr3 _temp3044= yyls; struct Cyc_Yyltype* _temp3046= _temp3044.curr
+ yylsp_offset; if( _temp3044.base == 0? 1:( _temp3046 < _temp3044.base? 1:
_temp3046 >= _temp3044.last_plus_one)){ _throw( Null_Exception);}* _temp3046;}).last_line));
_temp3040;}); _temp3039;}); break; case 52: _LL3038: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3048=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3048[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3049; _temp3049.tag=
Cyc_TypeSpecifier_tok; _temp3049.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3050= yyls; struct Cyc_Yyltype*
_temp3052= _temp3050.curr + yylsp_offset; if( _temp3050.base == 0? 1:( _temp3052
< _temp3050.base? 1: _temp3052 >= _temp3050.last_plus_one)){ _throw(
Null_Exception);}* _temp3052;}).first_line,({ struct _tagged_ptr3 _temp3053=
yyls; struct Cyc_Yyltype* _temp3055= _temp3053.curr + yylsp_offset; if(
_temp3053.base == 0? 1:( _temp3055 < _temp3053.base? 1: _temp3055 >= _temp3053.last_plus_one)){
_throw( Null_Exception);}* _temp3055;}).last_line)); _temp3049;}); _temp3048;});
break; case 53: _LL3047: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3057=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3057[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3058; _temp3058.tag=
Cyc_TypeSpecifier_tok; _temp3058.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp3059=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp3059[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp3060; _temp3060.tag=
Cyc_Parse_Short_spec; _temp3060.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3061= yyls; struct Cyc_Yyltype* _temp3063= _temp3061.curr +
yylsp_offset; if( _temp3061.base == 0? 1:( _temp3063 < _temp3061.base? 1:
_temp3063 >= _temp3061.last_plus_one)){ _throw( Null_Exception);}* _temp3063;}).first_line,({
struct _tagged_ptr3 _temp3064= yyls; struct Cyc_Yyltype* _temp3066= _temp3064.curr
+ yylsp_offset; if( _temp3064.base == 0? 1:( _temp3066 < _temp3064.base? 1:
_temp3066 >= _temp3064.last_plus_one)){ _throw( Null_Exception);}* _temp3066;}).last_line);
_temp3060;}); _temp3059;})); _temp3058;}); _temp3057;}); break; case 54: _LL3056:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3068=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3068[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3069; _temp3069.tag= Cyc_TypeSpecifier_tok;
_temp3069.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3070= yyls; struct Cyc_Yyltype* _temp3072= _temp3070.curr
+ yylsp_offset; if( _temp3070.base == 0? 1:( _temp3072 < _temp3070.base? 1:
_temp3072 >= _temp3070.last_plus_one)){ _throw( Null_Exception);}* _temp3072;}).first_line,({
struct _tagged_ptr3 _temp3073= yyls; struct Cyc_Yyltype* _temp3075= _temp3073.curr
+ yylsp_offset; if( _temp3073.base == 0? 1:( _temp3075 < _temp3073.base? 1:
_temp3075 >= _temp3073.last_plus_one)){ _throw( Null_Exception);}* _temp3075;}).last_line));
_temp3069;}); _temp3068;}); break; case 55: _LL3067: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3077=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3077[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3078; _temp3078.tag=
Cyc_TypeSpecifier_tok; _temp3078.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp3079=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp3079[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp3080; _temp3080.tag= Cyc_Parse_Long_spec;
_temp3080.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3081= yyls;
struct Cyc_Yyltype* _temp3083= _temp3081.curr + yylsp_offset; if( _temp3081.base
== 0? 1:( _temp3083 < _temp3081.base? 1: _temp3083 >= _temp3081.last_plus_one)){
_throw( Null_Exception);}* _temp3083;}).first_line,({ struct _tagged_ptr3
_temp3084= yyls; struct Cyc_Yyltype* _temp3086= _temp3084.curr + yylsp_offset;
if( _temp3084.base == 0? 1:( _temp3086 < _temp3084.base? 1: _temp3086 >=
_temp3084.last_plus_one)){ _throw( Null_Exception);}* _temp3086;}).last_line);
_temp3080;}); _temp3079;})); _temp3078;}); _temp3077;}); break; case 56: _LL3076:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3088=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3088[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3089; _temp3089.tag= Cyc_TypeSpecifier_tok;
_temp3089.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3090= yyls; struct Cyc_Yyltype* _temp3092= _temp3090.curr
+ yylsp_offset; if( _temp3090.base == 0? 1:( _temp3092 < _temp3090.base? 1:
_temp3092 >= _temp3090.last_plus_one)){ _throw( Null_Exception);}* _temp3092;}).first_line,({
struct _tagged_ptr3 _temp3093= yyls; struct Cyc_Yyltype* _temp3095= _temp3093.curr
+ yylsp_offset; if( _temp3093.base == 0? 1:( _temp3095 < _temp3093.base? 1:
_temp3095 >= _temp3093.last_plus_one)){ _throw( Null_Exception);}* _temp3095;}).last_line));
_temp3089;}); _temp3088;}); break; case 57: _LL3087: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3097=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3097[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3098; _temp3098.tag=
Cyc_TypeSpecifier_tok; _temp3098.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3099= yyls; struct Cyc_Yyltype*
_temp3101= _temp3099.curr + yylsp_offset; if( _temp3099.base == 0? 1:( _temp3101
< _temp3099.base? 1: _temp3101 >= _temp3099.last_plus_one)){ _throw(
Null_Exception);}* _temp3101;}).first_line,({ struct _tagged_ptr3 _temp3102=
yyls; struct Cyc_Yyltype* _temp3104= _temp3102.curr + yylsp_offset; if(
_temp3102.base == 0? 1:( _temp3104 < _temp3102.base? 1: _temp3104 >= _temp3102.last_plus_one)){
_throw( Null_Exception);}* _temp3104;}).last_line)); _temp3098;}); _temp3097;});
break; case 58: _LL3096: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3106=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3106[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3107; _temp3107.tag=
Cyc_TypeSpecifier_tok; _temp3107.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp3108=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp3108[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp3109; _temp3109.tag=
Cyc_Parse_Signed_spec; _temp3109.f1= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3110= yyls; struct Cyc_Yyltype* _temp3112= _temp3110.curr +
yylsp_offset; if( _temp3110.base == 0? 1:( _temp3112 < _temp3110.base? 1:
_temp3112 >= _temp3110.last_plus_one)){ _throw( Null_Exception);}* _temp3112;}).first_line,({
struct _tagged_ptr3 _temp3113= yyls; struct Cyc_Yyltype* _temp3115= _temp3113.curr
+ yylsp_offset; if( _temp3113.base == 0? 1:( _temp3115 < _temp3113.base? 1:
_temp3115 >= _temp3113.last_plus_one)){ _throw( Null_Exception);}* _temp3115;}).last_line);
_temp3109;}); _temp3108;})); _temp3107;}); _temp3106;}); break; case 59: _LL3105:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3117=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3117[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3118; _temp3118.tag= Cyc_TypeSpecifier_tok;
_temp3118.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp3119=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp3119[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp3120; _temp3120.tag= Cyc_Parse_Unsigned_spec; _temp3120.f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3121= yyls; struct Cyc_Yyltype* _temp3123= _temp3121.curr
+ yylsp_offset; if( _temp3121.base == 0? 1:( _temp3123 < _temp3121.base? 1:
_temp3123 >= _temp3121.last_plus_one)){ _throw( Null_Exception);}* _temp3123;}).first_line,({
struct _tagged_ptr3 _temp3124= yyls; struct Cyc_Yyltype* _temp3126= _temp3124.curr
+ yylsp_offset; if( _temp3124.base == 0? 1:( _temp3126 < _temp3124.base? 1:
_temp3126 >= _temp3124.last_plus_one)){ _throw( Null_Exception);}* _temp3126;}).last_line);
_temp3120;}); _temp3119;})); _temp3118;}); _temp3117;}); break; case 60: _LL3116:
yyval=({ struct _tagged_ptr2 _temp3128= yyvs; void** _temp3130= _temp3128.curr +
yyvsp_offset; if( _temp3128.base == 0? 1:( _temp3130 < _temp3128.base? 1:
_temp3130 >= _temp3128.last_plus_one)){ _throw( Null_Exception);}* _temp3130;});
break; case 61: _LL3127: yyval=({ struct _tagged_ptr2 _temp3132= yyvs; void**
_temp3134= _temp3132.curr + yyvsp_offset; if( _temp3132.base == 0? 1:( _temp3134
< _temp3132.base? 1: _temp3134 >= _temp3132.last_plus_one)){ _throw(
Null_Exception);}* _temp3134;}); break; case 62: _LL3131: yyval=({ struct
_tagged_ptr2 _temp3136= yyvs; void** _temp3138= _temp3136.curr + yyvsp_offset;
if( _temp3136.base == 0? 1:( _temp3138 < _temp3136.base? 1: _temp3138 >=
_temp3136.last_plus_one)){ _throw( Null_Exception);}* _temp3138;}); break; case
63: _LL3135: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3140=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3140[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3141; _temp3141.tag=
Cyc_TypeSpecifier_tok; _temp3141.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3142= yyvs; void** _temp3144=
_temp3142.curr + yyvsp_offset; if( _temp3142.base == 0? 1:( _temp3144 <
_temp3142.base? 1: _temp3144 >= _temp3142.last_plus_one)){ _throw(
Null_Exception);}* _temp3144;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3145= yyls; struct Cyc_Yyltype*
_temp3147= _temp3145.curr + yylsp_offset; if( _temp3145.base == 0? 1:( _temp3147
< _temp3145.base? 1: _temp3147 >= _temp3145.last_plus_one)){ _throw(
Null_Exception);}* _temp3147;}).first_line,({ struct _tagged_ptr3 _temp3148=
yyls; struct Cyc_Yyltype* _temp3150= _temp3148.curr + yylsp_offset; if(
_temp3148.base == 0? 1:( _temp3150 < _temp3148.base? 1: _temp3150 >= _temp3148.last_plus_one)){
_throw( Null_Exception);}* _temp3150;}).last_line)); _temp3141;}); _temp3140;});
break; case 64: _LL3139: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3152=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3152[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3153; _temp3153.tag=
Cyc_TypeSpecifier_tok; _temp3153.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3154= yyvs; void** _temp3156=
_temp3154.curr +( yyvsp_offset - 2); if( _temp3154.base == 0? 1:( _temp3156 <
_temp3154.base? 1: _temp3156 >= _temp3154.last_plus_one)){ _throw(
Null_Exception);}* _temp3156;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp3157= yyvs; void** _temp3159=
_temp3157.curr + yyvsp_offset; if( _temp3157.base == 0? 1:( _temp3159 <
_temp3157.base? 1: _temp3159 >= _temp3157.last_plus_one)){ _throw(
Null_Exception);}* _temp3159;})))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3160= yyls; struct Cyc_Yyltype* _temp3162= _temp3160.curr +(
yylsp_offset - 2); if( _temp3160.base == 0? 1:( _temp3162 < _temp3160.base? 1:
_temp3162 >= _temp3160.last_plus_one)){ _throw( Null_Exception);}* _temp3162;}).first_line,({
struct _tagged_ptr3 _temp3163= yyls; struct Cyc_Yyltype* _temp3165= _temp3163.curr
+ yylsp_offset; if( _temp3163.base == 0? 1:( _temp3165 < _temp3163.base? 1:
_temp3165 >= _temp3163.last_plus_one)){ _throw( Null_Exception);}* _temp3165;}).last_line));
_temp3153;}); _temp3152;}); break; case 65: _LL3151: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3167=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3167[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3168; _temp3168.tag=
Cyc_TypeSpecifier_tok; _temp3168.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp3169=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp3169[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp3170; _temp3170.tag= Cyc_Absyn_TypedefType;
_temp3170.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3171= yyvs; void**
_temp3173= _temp3171.curr +( yyvsp_offset - 1); if( _temp3171.base == 0? 1:(
_temp3173 < _temp3171.base? 1: _temp3173 >= _temp3171.last_plus_one)){ _throw(
Null_Exception);}* _temp3173;})); _temp3170.f2= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3174= yyvs; void** _temp3176= _temp3174.curr + yyvsp_offset;
if( _temp3174.base == 0? 1:( _temp3176 < _temp3174.base? 1: _temp3176 >=
_temp3174.last_plus_one)){ _throw( Null_Exception);}* _temp3176;})); _temp3170.f3=
0; _temp3170;}); _temp3169;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3177= yyls; struct Cyc_Yyltype* _temp3179= _temp3177.curr +(
yylsp_offset - 1); if( _temp3177.base == 0? 1:( _temp3179 < _temp3177.base? 1:
_temp3179 >= _temp3177.last_plus_one)){ _throw( Null_Exception);}* _temp3179;}).first_line,({
struct _tagged_ptr3 _temp3180= yyls; struct Cyc_Yyltype* _temp3182= _temp3180.curr
+ yylsp_offset; if( _temp3180.base == 0? 1:( _temp3182 < _temp3180.base? 1:
_temp3182 >= _temp3180.last_plus_one)){ _throw( Null_Exception);}* _temp3182;}).last_line));
_temp3168;}); _temp3167;}); break; case 66: _LL3166: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3184=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3184[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3185; _temp3185.tag=
Cyc_TypeSpecifier_tok; _temp3185.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp3186=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp3186[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp3187; _temp3187.tag= Cyc_Absyn_TupleType;
_temp3187.f1=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3188= yyls; struct Cyc_Yyltype* _temp3190= _temp3188.curr +(
yylsp_offset - 1); if( _temp3188.base == 0? 1:( _temp3190 < _temp3188.base? 1:
_temp3190 >= _temp3188.last_plus_one)){ _throw( Null_Exception);}* _temp3190;}).first_line,({
struct _tagged_ptr3 _temp3191= yyls; struct Cyc_Yyltype* _temp3193= _temp3191.curr
+( yylsp_offset - 1); if( _temp3191.base == 0? 1:( _temp3193 < _temp3191.base? 1:
_temp3193 >= _temp3191.last_plus_one)){ _throw( Null_Exception);}* _temp3193;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3194= yyvs; void** _temp3196= _temp3194.curr +(
yyvsp_offset - 1); if( _temp3194.base == 0? 1:( _temp3196 < _temp3194.base? 1:
_temp3196 >= _temp3194.last_plus_one)){ _throw( Null_Exception);}* _temp3196;}))));
_temp3187;}); _temp3186;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3197= yyls; struct Cyc_Yyltype* _temp3199= _temp3197.curr +( yylsp_offset -
3); if( _temp3197.base == 0? 1:( _temp3199 < _temp3197.base? 1: _temp3199 >=
_temp3197.last_plus_one)){ _throw( Null_Exception);}* _temp3199;}).first_line,({
struct _tagged_ptr3 _temp3200= yyls; struct Cyc_Yyltype* _temp3202= _temp3200.curr
+ yylsp_offset; if( _temp3200.base == 0? 1:( _temp3202 < _temp3200.base? 1:
_temp3202 >= _temp3200.last_plus_one)){ _throw( Null_Exception);}* _temp3202;}).last_line));
_temp3185;}); _temp3184;}); break; case 67: _LL3183: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3204=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3204[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3205; _temp3205.tag=
Cyc_TypeSpecifier_tok; _temp3205.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp3206=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp3206[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp3207; _temp3207.tag= Cyc_Absyn_RgnHandleType;
_temp3207.f1=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp3208= yyvs;
void** _temp3210= _temp3208.curr +( yyvsp_offset - 1); if( _temp3208.base == 0?
1:( _temp3210 < _temp3208.base? 1: _temp3210 >= _temp3208.last_plus_one)){
_throw( Null_Exception);}* _temp3210;})); _temp3207;}); _temp3206;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3211= yyls; struct Cyc_Yyltype* _temp3213= _temp3211.curr
+( yylsp_offset - 3); if( _temp3211.base == 0? 1:( _temp3213 < _temp3211.base? 1:
_temp3213 >= _temp3211.last_plus_one)){ _throw( Null_Exception);}* _temp3213;}).first_line,({
struct _tagged_ptr3 _temp3214= yyls; struct Cyc_Yyltype* _temp3216= _temp3214.curr
+ yylsp_offset; if( _temp3214.base == 0? 1:( _temp3216 < _temp3214.base? 1:
_temp3216 >= _temp3214.last_plus_one)){ _throw( Null_Exception);}* _temp3216;}).last_line));
_temp3205;}); _temp3204;}); break; case 68: _LL3203: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp3218=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp3218[ 0]=({ struct Cyc_Kind_tok_struct _temp3219; _temp3219.tag= Cyc_Kind_tok;
_temp3219.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp3220= yyvs; void** _temp3222= _temp3220.curr + yyvsp_offset;
if( _temp3220.base == 0? 1:( _temp3222 < _temp3220.base? 1: _temp3222 >=
_temp3220.last_plus_one)){ _throw( Null_Exception);}* _temp3222;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3223= yyls; struct Cyc_Yyltype* _temp3225= _temp3223.curr
+ yylsp_offset; if( _temp3223.base == 0? 1:( _temp3225 < _temp3223.base? 1:
_temp3225 >= _temp3223.last_plus_one)){ _throw( Null_Exception);}* _temp3225;}).first_line,({
struct _tagged_ptr3 _temp3226= yyls; struct Cyc_Yyltype* _temp3228= _temp3226.curr
+ yylsp_offset; if( _temp3226.base == 0? 1:( _temp3228 < _temp3226.base? 1:
_temp3228 >= _temp3226.last_plus_one)){ _throw( Null_Exception);}* _temp3228;}).last_line));
_temp3219;}); _temp3218;}); break; case 69: _LL3217: { struct _tagged_string*
_temp3235; void* _temp3237; struct _tuple1 _temp3233=* Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3230= yyvs; void** _temp3232= _temp3230.curr +
yyvsp_offset; if( _temp3230.base == 0? 1:( _temp3232 < _temp3230.base? 1:
_temp3232 >= _temp3230.last_plus_one)){ _throw( Null_Exception);}* _temp3232;}));
_LL3238: _temp3237= _temp3233.f1; goto _LL3236; _LL3236: _temp3235= _temp3233.f2;
goto _LL3234; _LL3234: if( _temp3237 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3239=( char*)"bad kind in type specifier";
struct _tagged_string _temp3240; _temp3240.curr= _temp3239; _temp3240.base=
_temp3239; _temp3240.last_plus_one= _temp3239 + 27; _temp3240;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3241= yyls; struct Cyc_Yyltype* _temp3243= _temp3241.curr
+ yylsp_offset; if( _temp3241.base == 0? 1:( _temp3243 < _temp3241.base? 1:
_temp3243 >= _temp3241.last_plus_one)){ _throw( Null_Exception);}* _temp3243;}).first_line,({
struct _tagged_ptr3 _temp3244= yyls; struct Cyc_Yyltype* _temp3246= _temp3244.curr
+ yylsp_offset; if( _temp3244.base == 0? 1:( _temp3246 < _temp3244.base? 1:
_temp3246 >= _temp3244.last_plus_one)){ _throw( Null_Exception);}* _temp3246;}).last_line));}
yyval=( void*)({ struct Cyc_Kind_tok_struct* _temp3247=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp3247[ 0]=({ struct Cyc_Kind_tok_struct
_temp3248; _temp3248.tag= Cyc_Kind_tok; _temp3248.f1=( void*) Cyc_Parse_id_to_kind(*
_temp3235, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3249= yyls;
struct Cyc_Yyltype* _temp3251= _temp3249.curr + yylsp_offset; if( _temp3249.base
== 0? 1:( _temp3251 < _temp3249.base? 1: _temp3251 >= _temp3249.last_plus_one)){
_throw( Null_Exception);}* _temp3251;}).first_line,({ struct _tagged_ptr3
_temp3252= yyls; struct Cyc_Yyltype* _temp3254= _temp3252.curr + yylsp_offset;
if( _temp3252.base == 0? 1:( _temp3254 < _temp3252.base? 1: _temp3254 >=
_temp3252.last_plus_one)){ _throw( Null_Exception);}* _temp3254;}).last_line));
_temp3248;}); _temp3247;}); break;} case 70: _LL3229: yyval=( void*)({ struct
Cyc_TypeQual_tok_struct* _temp3256=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct)); _temp3256[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3257; _temp3257.tag= Cyc_TypeQual_tok; _temp3257.f1=({ struct Cyc_Absyn_Tqual
_temp3258; _temp3258.q_const= 1; _temp3258.q_volatile= 0; _temp3258.q_restrict=
0; _temp3258;}); _temp3257;}); _temp3256;}); break; case 71: _LL3255: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp3260=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3260[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3261; _temp3261.tag= Cyc_TypeQual_tok; _temp3261.f1=({ struct Cyc_Absyn_Tqual
_temp3262; _temp3262.q_const= 0; _temp3262.q_volatile= 1; _temp3262.q_restrict=
0; _temp3262;}); _temp3261;}); _temp3260;}); break; case 72: _LL3259: yyval=(
void*)({ struct Cyc_TypeQual_tok_struct* _temp3264=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3264[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3265; _temp3265.tag= Cyc_TypeQual_tok; _temp3265.f1=({ struct Cyc_Absyn_Tqual
_temp3266; _temp3266.q_const= 0; _temp3266.q_volatile= 0; _temp3266.q_restrict=
1; _temp3266;}); _temp3265;}); _temp3264;}); break; case 73: _LL3263: yyval=(
void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3268=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3268[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3269; _temp3269.tag= Cyc_TypeSpecifier_tok;
_temp3269.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3270=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3270[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3271; _temp3271.tag= Cyc_Parse_Decl_spec;
_temp3271.f1=({ struct Cyc_Absyn_Decl* _temp3272=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp3272->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp3279=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp3279[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp3280; _temp3280.tag= Cyc_Absyn_Enum_d; _temp3280.f1=({ struct Cyc_Absyn_Enumdecl*
_temp3281=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp3281->sc=( void*)(( void*) Cyc_Absyn_Public); _temp3281->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3286= yyvs; void** _temp3288= _temp3286.curr +(
yyvsp_offset - 3); if( _temp3286.base == 0? 1:( _temp3288 < _temp3286.base? 1:
_temp3288 >= _temp3286.last_plus_one)){ _throw( Null_Exception);}* _temp3288;}));
_temp3281->fields=({ struct Cyc_Core_Opt* _temp3282=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3282->v=( void*) Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3283= yyvs; void** _temp3285= _temp3283.curr +(
yyvsp_offset - 1); if( _temp3283.base == 0? 1:( _temp3285 < _temp3283.base? 1:
_temp3285 >= _temp3283.last_plus_one)){ _throw( Null_Exception);}* _temp3285;}));
_temp3282;}); _temp3281;}); _temp3280;}); _temp3279;})); _temp3272->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3273= yyls; struct Cyc_Yyltype* _temp3275= _temp3273.curr
+( yylsp_offset - 4); if( _temp3273.base == 0? 1:( _temp3275 < _temp3273.base? 1:
_temp3275 >= _temp3273.last_plus_one)){ _throw( Null_Exception);}* _temp3275;}).first_line,({
struct _tagged_ptr3 _temp3276= yyls; struct Cyc_Yyltype* _temp3278= _temp3276.curr
+ yylsp_offset; if( _temp3276.base == 0? 1:( _temp3278 < _temp3276.base? 1:
_temp3278 >= _temp3276.last_plus_one)){ _throw( Null_Exception);}* _temp3278;}).last_line);
_temp3272;}); _temp3271;}); _temp3270;})); _temp3269;}); _temp3268;}); break;
case 74: _LL3267: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3290=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3290[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3291; _temp3291.tag=
Cyc_TypeSpecifier_tok; _temp3291.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_EnumType_struct* _temp3292=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3292[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp3293; _temp3293.tag= Cyc_Absyn_EnumType;
_temp3293.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3294= yyvs; void**
_temp3296= _temp3294.curr + yyvsp_offset; if( _temp3294.base == 0? 1:( _temp3296
< _temp3294.base? 1: _temp3296 >= _temp3294.last_plus_one)){ _throw(
Null_Exception);}* _temp3296;})); _temp3293.f2= 0; _temp3293;}); _temp3292;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3297= yyls; struct Cyc_Yyltype*
_temp3299= _temp3297.curr +( yylsp_offset - 1); if( _temp3297.base == 0? 1:(
_temp3299 < _temp3297.base? 1: _temp3299 >= _temp3297.last_plus_one)){ _throw(
Null_Exception);}* _temp3299;}).first_line,({ struct _tagged_ptr3 _temp3300=
yyls; struct Cyc_Yyltype* _temp3302= _temp3300.curr + yylsp_offset; if(
_temp3300.base == 0? 1:( _temp3302 < _temp3300.base? 1: _temp3302 >= _temp3300.last_plus_one)){
_throw( Null_Exception);}* _temp3302;}).last_line)); _temp3291;}); _temp3290;});
break; case 75: _LL3289: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp3304=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp3304[ 0]=({ struct Cyc_Enumfield_tok_struct _temp3305; _temp3305.tag= Cyc_Enumfield_tok;
_temp3305.f1=({ struct Cyc_Absyn_Enumfield* _temp3306=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3306->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3313= yyvs; void** _temp3315= _temp3313.curr +
yyvsp_offset; if( _temp3313.base == 0? 1:( _temp3315 < _temp3313.base? 1:
_temp3315 >= _temp3313.last_plus_one)){ _throw( Null_Exception);}* _temp3315;}));
_temp3306->tag= 0; _temp3306->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3307= yyls; struct Cyc_Yyltype* _temp3309= _temp3307.curr +
yylsp_offset; if( _temp3307.base == 0? 1:( _temp3309 < _temp3307.base? 1:
_temp3309 >= _temp3307.last_plus_one)){ _throw( Null_Exception);}* _temp3309;}).first_line,({
struct _tagged_ptr3 _temp3310= yyls; struct Cyc_Yyltype* _temp3312= _temp3310.curr
+ yylsp_offset; if( _temp3310.base == 0? 1:( _temp3312 < _temp3310.base? 1:
_temp3312 >= _temp3310.last_plus_one)){ _throw( Null_Exception);}* _temp3312;}).last_line);
_temp3306;}); _temp3305;}); _temp3304;}); break; case 76: _LL3303: yyval=( void*)({
struct Cyc_Enumfield_tok_struct* _temp3317=( struct Cyc_Enumfield_tok_struct*)
GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct)); _temp3317[ 0]=({ struct
Cyc_Enumfield_tok_struct _temp3318; _temp3318.tag= Cyc_Enumfield_tok; _temp3318.f1=({
struct Cyc_Absyn_Enumfield* _temp3319=( struct Cyc_Absyn_Enumfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumfield)); _temp3319->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3329= yyvs; void** _temp3331= _temp3329.curr +(
yyvsp_offset - 2); if( _temp3329.base == 0? 1:( _temp3331 < _temp3329.base? 1:
_temp3331 >= _temp3329.last_plus_one)){ _throw( Null_Exception);}* _temp3331;}));
_temp3319->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp3326= yyvs; void** _temp3328= _temp3326.curr + yyvsp_offset; if( _temp3326.base
== 0? 1:( _temp3328 < _temp3326.base? 1: _temp3328 >= _temp3326.last_plus_one)){
_throw( Null_Exception);}* _temp3328;})); _temp3319->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3320= yyls; struct Cyc_Yyltype* _temp3322= _temp3320.curr
+( yylsp_offset - 2); if( _temp3320.base == 0? 1:( _temp3322 < _temp3320.base? 1:
_temp3322 >= _temp3320.last_plus_one)){ _throw( Null_Exception);}* _temp3322;}).first_line,({
struct _tagged_ptr3 _temp3323= yyls; struct Cyc_Yyltype* _temp3325= _temp3323.curr
+ yylsp_offset; if( _temp3323.base == 0? 1:( _temp3325 < _temp3323.base? 1:
_temp3325 >= _temp3323.last_plus_one)){ _throw( Null_Exception);}* _temp3325;}).last_line);
_temp3319;}); _temp3318;}); _temp3317;}); break; case 77: _LL3316: yyval=( void*)({
struct Cyc_EnumfieldList_tok_struct* _temp3333=( struct Cyc_EnumfieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct)); _temp3333[ 0]=({
struct Cyc_EnumfieldList_tok_struct _temp3334; _temp3334.tag= Cyc_EnumfieldList_tok;
_temp3334.f1=({ struct Cyc_List_List* _temp3335=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3335->hd=( void*) Cyc_yyget_Enumfield_tok(({
struct _tagged_ptr2 _temp3336= yyvs; void** _temp3338= _temp3336.curr +
yyvsp_offset; if( _temp3336.base == 0? 1:( _temp3338 < _temp3336.base? 1:
_temp3338 >= _temp3336.last_plus_one)){ _throw( Null_Exception);}* _temp3338;}));
_temp3335->tl= 0; _temp3335;}); _temp3334;}); _temp3333;}); break; case 78:
_LL3332: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct* _temp3340=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp3340[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp3341; _temp3341.tag=
Cyc_EnumfieldList_tok; _temp3341.f1=({ struct Cyc_List_List* _temp3342=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3342->hd=( void*)
Cyc_yyget_Enumfield_tok(({ struct _tagged_ptr2 _temp3346= yyvs; void** _temp3348=
_temp3346.curr +( yyvsp_offset - 2); if( _temp3346.base == 0? 1:( _temp3348 <
_temp3346.base? 1: _temp3348 >= _temp3346.last_plus_one)){ _throw(
Null_Exception);}* _temp3348;})); _temp3342->tl= Cyc_yyget_EnumfieldList_tok(({
struct _tagged_ptr2 _temp3343= yyvs; void** _temp3345= _temp3343.curr +
yyvsp_offset; if( _temp3343.base == 0? 1:( _temp3345 < _temp3343.base? 1:
_temp3345 >= _temp3343.last_plus_one)){ _throw( Null_Exception);}* _temp3345;}));
_temp3342;}); _temp3341;}); _temp3340;}); break; case 79: _LL3339: { void* t;{
void* _temp3353= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3350=
yyvs; void** _temp3352= _temp3350.curr +( yyvsp_offset - 3); if( _temp3350.base
== 0? 1:( _temp3352 < _temp3350.base? 1: _temp3352 >= _temp3350.last_plus_one)){
_throw( Null_Exception);}* _temp3352;})); _LL3355: if( _temp3353 ==( void*) Cyc_Parse_Struct_su){
goto _LL3356;} else{ goto _LL3357;} _LL3357: if( _temp3353 ==( void*) Cyc_Parse_Union_su){
goto _LL3358;} else{ goto _LL3354;} _LL3356: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp3359=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp3359[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp3360; _temp3360.tag= Cyc_Absyn_AnonStructType; _temp3360.f1= Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3361= yyvs; void** _temp3363= _temp3361.curr +(
yyvsp_offset - 1); if( _temp3361.base == 0? 1:( _temp3363 < _temp3361.base? 1:
_temp3363 >= _temp3361.last_plus_one)){ _throw( Null_Exception);}* _temp3363;}));
_temp3360;}); _temp3359;}); goto _LL3354; _LL3358: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp3364=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp3364[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp3365; _temp3365.tag= Cyc_Absyn_AnonUnionType; _temp3365.f1= Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3366= yyvs; void** _temp3368= _temp3366.curr +(
yyvsp_offset - 1); if( _temp3366.base == 0? 1:( _temp3368 < _temp3366.base? 1:
_temp3368 >= _temp3366.last_plus_one)){ _throw( Null_Exception);}* _temp3368;}));
_temp3365;}); _temp3364;}); goto _LL3354; _LL3354:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3369=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3369[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3370; _temp3370.tag=
Cyc_TypeSpecifier_tok; _temp3370.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp3371=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp3371[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp3372; _temp3372.tag= Cyc_Parse_Type_spec;
_temp3372.f1=( void*) t; _temp3372.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3373= yyls; struct Cyc_Yyltype* _temp3375= _temp3373.curr +(
yylsp_offset - 3); if( _temp3373.base == 0? 1:( _temp3375 < _temp3373.base? 1:
_temp3375 >= _temp3373.last_plus_one)){ _throw( Null_Exception);}* _temp3375;}).first_line,({
struct _tagged_ptr3 _temp3376= yyls; struct Cyc_Yyltype* _temp3378= _temp3376.curr
+ yylsp_offset; if( _temp3376.base == 0? 1:( _temp3378 < _temp3376.base? 1:
_temp3378 >= _temp3376.last_plus_one)){ _throw( Null_Exception);}* _temp3378;}).last_line);
_temp3372;}); _temp3371;})); _temp3370;}); _temp3369;}); break;} case 80:
_LL3349: { struct Cyc_List_List* _temp3389=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3380= yyls; struct Cyc_Yyltype* _temp3382= _temp3380.curr
+( yylsp_offset - 3); if( _temp3380.base == 0? 1:( _temp3382 < _temp3380.base? 1:
_temp3382 >= _temp3380.last_plus_one)){ _throw( Null_Exception);}* _temp3382;}).first_line,({
struct _tagged_ptr3 _temp3383= yyls; struct Cyc_Yyltype* _temp3385= _temp3383.curr
+( yylsp_offset - 3); if( _temp3383.base == 0? 1:( _temp3385 < _temp3383.base? 1:
_temp3385 >= _temp3383.last_plus_one)){ _throw( Null_Exception);}* _temp3385;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3386= yyvs; void** _temp3388=
_temp3386.curr +( yyvsp_offset - 3); if( _temp3386.base == 0? 1:( _temp3388 <
_temp3386.base? 1: _temp3388 >= _temp3386.last_plus_one)){ _throw(
Null_Exception);}* _temp3388;}))); goto _LL3390; _LL3390: { struct Cyc_Absyn_Decl*
d;{ void* _temp3394= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2
_temp3391= yyvs; void** _temp3393= _temp3391.curr +( yyvsp_offset - 5); if(
_temp3391.base == 0? 1:( _temp3393 < _temp3391.base? 1: _temp3393 >= _temp3391.last_plus_one)){
_throw( Null_Exception);}* _temp3393;})); _LL3396: if( _temp3394 ==( void*) Cyc_Parse_Struct_su){
goto _LL3397;} else{ goto _LL3398;} _LL3398: if( _temp3394 ==( void*) Cyc_Parse_Union_su){
goto _LL3399;} else{ goto _LL3395;} _LL3397: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3400=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3400->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3401= yyvs; void** _temp3403= _temp3401.curr +(
yyvsp_offset - 4); if( _temp3401.base == 0? 1:( _temp3403 < _temp3401.base? 1:
_temp3403 >= _temp3401.last_plus_one)){ _throw( Null_Exception);}* _temp3403;}));
_temp3400;}), _temp3389,({ struct Cyc_Core_Opt* _temp3404=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3404->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
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
goto _LL3395; _LL3399: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3414=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3414->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3415= yyvs; void** _temp3417= _temp3415.curr +( yyvsp_offset -
4); if( _temp3415.base == 0? 1:( _temp3417 < _temp3415.base? 1: _temp3417 >=
_temp3415.last_plus_one)){ _throw( Null_Exception);}* _temp3417;})); _temp3414;}),
_temp3389,({ struct Cyc_Core_Opt* _temp3418=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3418->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3419= yyvs; void** _temp3421= _temp3419.curr +(
yyvsp_offset - 1); if( _temp3419.base == 0? 1:( _temp3421 < _temp3419.base? 1:
_temp3421 >= _temp3419.last_plus_one)){ _throw( Null_Exception);}* _temp3421;}));
_temp3418;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3422=
yyls; struct Cyc_Yyltype* _temp3424= _temp3422.curr +( yylsp_offset - 5); if(
_temp3422.base == 0? 1:( _temp3424 < _temp3422.base? 1: _temp3424 >= _temp3422.last_plus_one)){
_throw( Null_Exception);}* _temp3424;}).first_line,({ struct _tagged_ptr3
_temp3425= yyls; struct Cyc_Yyltype* _temp3427= _temp3425.curr + yylsp_offset;
if( _temp3425.base == 0? 1:( _temp3427 < _temp3425.base? 1: _temp3427 >=
_temp3425.last_plus_one)){ _throw( Null_Exception);}* _temp3427;}).last_line));
goto _LL3395; _LL3395:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3428=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3428[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3429; _temp3429.tag=
Cyc_TypeSpecifier_tok; _temp3429.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3430=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3430[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3431; _temp3431.tag= Cyc_Parse_Decl_spec;
_temp3431.f1= d; _temp3431;}); _temp3430;})); _temp3429;}); _temp3428;}); break;}}
case 81: _LL3379: { struct Cyc_List_List* _temp3442=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3433= yyls; struct Cyc_Yyltype* _temp3435= _temp3433.curr
+( yylsp_offset - 3); if( _temp3433.base == 0? 1:( _temp3435 < _temp3433.base? 1:
_temp3435 >= _temp3433.last_plus_one)){ _throw( Null_Exception);}* _temp3435;}).first_line,({
struct _tagged_ptr3 _temp3436= yyls; struct Cyc_Yyltype* _temp3438= _temp3436.curr
+( yylsp_offset - 3); if( _temp3436.base == 0? 1:( _temp3438 < _temp3436.base? 1:
_temp3438 >= _temp3436.last_plus_one)){ _throw( Null_Exception);}* _temp3438;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3439= yyvs; void** _temp3441=
_temp3439.curr +( yyvsp_offset - 3); if( _temp3439.base == 0? 1:( _temp3441 <
_temp3439.base? 1: _temp3441 >= _temp3439.last_plus_one)){ _throw(
Null_Exception);}* _temp3441;}))); goto _LL3443; _LL3443: { struct Cyc_Absyn_Decl*
d;{ void* _temp3447= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2
_temp3444= yyvs; void** _temp3446= _temp3444.curr +( yyvsp_offset - 5); if(
_temp3444.base == 0? 1:( _temp3446 < _temp3444.base? 1: _temp3446 >= _temp3444.last_plus_one)){
_throw( Null_Exception);}* _temp3446;})); _LL3449: if( _temp3447 ==( void*) Cyc_Parse_Struct_su){
goto _LL3450;} else{ goto _LL3451;} _LL3451: if( _temp3447 ==( void*) Cyc_Parse_Union_su){
goto _LL3452;} else{ goto _LL3448;} _LL3450: d= Cyc_Absyn_struct_decl(( void*)
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3453=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3453->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3454= yyvs; void** _temp3456= _temp3454.curr +(
yyvsp_offset - 4); if( _temp3454.base == 0? 1:( _temp3456 < _temp3454.base? 1:
_temp3456 >= _temp3454.last_plus_one)){ _throw( Null_Exception);}* _temp3456;}));
_temp3453;}), _temp3442,({ struct Cyc_Core_Opt* _temp3457=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3457->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3458= yyvs; void** _temp3460= _temp3458.curr +(
yyvsp_offset - 1); if( _temp3458.base == 0? 1:( _temp3460 < _temp3458.base? 1:
_temp3460 >= _temp3458.last_plus_one)){ _throw( Null_Exception);}* _temp3460;}));
_temp3457;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3461=
yyls; struct Cyc_Yyltype* _temp3463= _temp3461.curr +( yylsp_offset - 5); if(
_temp3461.base == 0? 1:( _temp3463 < _temp3461.base? 1: _temp3463 >= _temp3461.last_plus_one)){
_throw( Null_Exception);}* _temp3463;}).first_line,({ struct _tagged_ptr3
_temp3464= yyls; struct Cyc_Yyltype* _temp3466= _temp3464.curr + yylsp_offset;
if( _temp3464.base == 0? 1:( _temp3466 < _temp3464.base? 1: _temp3466 >=
_temp3464.last_plus_one)){ _throw( Null_Exception);}* _temp3466;}).last_line));
goto _LL3448; _LL3452: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3467=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3467->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3468= yyvs; void** _temp3470= _temp3468.curr +( yyvsp_offset -
4); if( _temp3468.base == 0? 1:( _temp3470 < _temp3468.base? 1: _temp3470 >=
_temp3468.last_plus_one)){ _throw( Null_Exception);}* _temp3470;})); _temp3467;}),
_temp3442,({ struct Cyc_Core_Opt* _temp3471=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3471->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3472= yyvs; void** _temp3474= _temp3472.curr +(
yyvsp_offset - 1); if( _temp3472.base == 0? 1:( _temp3474 < _temp3472.base? 1:
_temp3474 >= _temp3472.last_plus_one)){ _throw( Null_Exception);}* _temp3474;}));
_temp3471;}), 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3475=
yyls; struct Cyc_Yyltype* _temp3477= _temp3475.curr +( yylsp_offset - 5); if(
_temp3475.base == 0? 1:( _temp3477 < _temp3475.base? 1: _temp3477 >= _temp3475.last_plus_one)){
_throw( Null_Exception);}* _temp3477;}).first_line,({ struct _tagged_ptr3
_temp3478= yyls; struct Cyc_Yyltype* _temp3480= _temp3478.curr + yylsp_offset;
if( _temp3478.base == 0? 1:( _temp3480 < _temp3478.base? 1: _temp3480 >=
_temp3478.last_plus_one)){ _throw( Null_Exception);}* _temp3480;}).last_line));
goto _LL3448; _LL3448:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3481=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3481[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3482; _temp3482.tag=
Cyc_TypeSpecifier_tok; _temp3482.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3483=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3483[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3484; _temp3484.tag= Cyc_Parse_Decl_spec;
_temp3484.f1= d; _temp3484;}); _temp3483;})); _temp3482;}); _temp3481;}); break;}}
case 82: _LL3432:{ void* _temp3489= Cyc_yyget_StructOrUnion_tok(({ struct
_tagged_ptr2 _temp3486= yyvs; void** _temp3488= _temp3486.curr +( yyvsp_offset -
2); if( _temp3486.base == 0? 1:( _temp3488 < _temp3486.base? 1: _temp3488 >=
_temp3486.last_plus_one)){ _throw( Null_Exception);}* _temp3488;})); _LL3491:
if( _temp3489 ==( void*) Cyc_Parse_Struct_su){ goto _LL3492;} else{ goto _LL3493;}
_LL3493: if( _temp3489 ==( void*) Cyc_Parse_Union_su){ goto _LL3494;} else{ goto
_LL3490;} _LL3492: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3495=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3495[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3496; _temp3496.tag=
Cyc_TypeSpecifier_tok; _temp3496.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3497=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3497[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3498; _temp3498.tag= Cyc_Absyn_StructType;
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
_temp3496;}); _temp3495;}); goto _LL3490; _LL3494: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3511=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3511[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3512; _temp3512.tag=
Cyc_TypeSpecifier_tok; _temp3512.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp3513=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3513[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp3514; _temp3514.tag= Cyc_Absyn_UnionType;
_temp3514.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3515= yyvs; void** _temp3517= _temp3515.curr +( yyvsp_offset - 1); if(
_temp3515.base == 0? 1:( _temp3517 < _temp3515.base? 1: _temp3517 >= _temp3515.last_plus_one)){
_throw( Null_Exception);}* _temp3517;})); _temp3514.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3518= yyvs; void** _temp3520= _temp3518.curr +
yyvsp_offset; if( _temp3518.base == 0? 1:( _temp3520 < _temp3518.base? 1:
_temp3520 >= _temp3518.last_plus_one)){ _throw( Null_Exception);}* _temp3520;}));
_temp3514.f3= 0; _temp3514;}); _temp3513;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3521= yyls; struct Cyc_Yyltype* _temp3523= _temp3521.curr
+( yylsp_offset - 2); if( _temp3521.base == 0? 1:( _temp3523 < _temp3521.base? 1:
_temp3523 >= _temp3521.last_plus_one)){ _throw( Null_Exception);}* _temp3523;}).first_line,({
struct _tagged_ptr3 _temp3524= yyls; struct Cyc_Yyltype* _temp3526= _temp3524.curr
+ yylsp_offset; if( _temp3524.base == 0? 1:( _temp3526 < _temp3524.base? 1:
_temp3526 >= _temp3524.last_plus_one)){ _throw( Null_Exception);}* _temp3526;}).last_line));
_temp3512;}); _temp3511;}); goto _LL3490; _LL3490:;} break; case 83: _LL3485:{
void* _temp3531= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3528=
yyvs; void** _temp3530= _temp3528.curr +( yyvsp_offset - 2); if( _temp3528.base
== 0? 1:( _temp3530 < _temp3528.base? 1: _temp3530 >= _temp3528.last_plus_one)){
_throw( Null_Exception);}* _temp3530;})); _LL3533: if( _temp3531 ==( void*) Cyc_Parse_Struct_su){
goto _LL3534;} else{ goto _LL3535;} _LL3535: if( _temp3531 ==( void*) Cyc_Parse_Union_su){
goto _LL3536;} else{ goto _LL3532;} _LL3534: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3537=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3537[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3538; _temp3538.tag=
Cyc_TypeSpecifier_tok; _temp3538.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp3539=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3539[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp3540; _temp3540.tag= Cyc_Absyn_StructType;
_temp3540.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3541= yyvs; void** _temp3543= _temp3541.curr +( yyvsp_offset - 1); if(
_temp3541.base == 0? 1:( _temp3543 < _temp3541.base? 1: _temp3543 >= _temp3541.last_plus_one)){
_throw( Null_Exception);}* _temp3543;})); _temp3540.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3544= yyvs; void** _temp3546= _temp3544.curr +
yyvsp_offset; if( _temp3544.base == 0? 1:( _temp3546 < _temp3544.base? 1:
_temp3546 >= _temp3544.last_plus_one)){ _throw( Null_Exception);}* _temp3546;}));
_temp3540.f3= 0; _temp3540;}); _temp3539;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3547= yyls; struct Cyc_Yyltype* _temp3549= _temp3547.curr
+( yylsp_offset - 2); if( _temp3547.base == 0? 1:( _temp3549 < _temp3547.base? 1:
_temp3549 >= _temp3547.last_plus_one)){ _throw( Null_Exception);}* _temp3549;}).first_line,({
struct _tagged_ptr3 _temp3550= yyls; struct Cyc_Yyltype* _temp3552= _temp3550.curr
+ yylsp_offset; if( _temp3550.base == 0? 1:( _temp3552 < _temp3550.base? 1:
_temp3552 >= _temp3550.last_plus_one)){ _throw( Null_Exception);}* _temp3552;}).last_line));
_temp3538;}); _temp3537;}); goto _LL3532; _LL3536: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3553=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3553[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3554; _temp3554.tag=
Cyc_TypeSpecifier_tok; _temp3554.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp3555=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3555[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp3556; _temp3556.tag= Cyc_Absyn_UnionType;
_temp3556.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3557= yyvs; void** _temp3559= _temp3557.curr +( yyvsp_offset - 1); if(
_temp3557.base == 0? 1:( _temp3559 < _temp3557.base? 1: _temp3559 >= _temp3557.last_plus_one)){
_throw( Null_Exception);}* _temp3559;})); _temp3556.f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3560= yyvs; void** _temp3562= _temp3560.curr +
yyvsp_offset; if( _temp3560.base == 0? 1:( _temp3562 < _temp3560.base? 1:
_temp3562 >= _temp3560.last_plus_one)){ _throw( Null_Exception);}* _temp3562;}));
_temp3556.f3= 0; _temp3556;}); _temp3555;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3563= yyls; struct Cyc_Yyltype* _temp3565= _temp3563.curr
+( yylsp_offset - 2); if( _temp3563.base == 0? 1:( _temp3565 < _temp3563.base? 1:
_temp3565 >= _temp3563.last_plus_one)){ _throw( Null_Exception);}* _temp3565;}).first_line,({
struct _tagged_ptr3 _temp3566= yyls; struct Cyc_Yyltype* _temp3568= _temp3566.curr
+ yylsp_offset; if( _temp3566.base == 0? 1:( _temp3568 < _temp3566.base? 1:
_temp3568 >= _temp3566.last_plus_one)){ _throw( Null_Exception);}* _temp3568;}).last_line));
_temp3554;}); _temp3553;}); goto _LL3532; _LL3532:;} break; case 84: _LL3527:
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3570=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3570[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3571; _temp3571.tag= Cyc_TypeList_tok; _temp3571.f1= 0; _temp3571;});
_temp3570;}); break; case 85: _LL3569: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3573=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3573[ 0]=({ struct Cyc_TypeList_tok_struct _temp3574; _temp3574.tag= Cyc_TypeList_tok;
_temp3574.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3575= yyvs; void** _temp3577=
_temp3575.curr +( yyvsp_offset - 1); if( _temp3575.base == 0? 1:( _temp3577 <
_temp3575.base? 1: _temp3577 >= _temp3575.last_plus_one)){ _throw(
Null_Exception);}* _temp3577;}))); _temp3574;}); _temp3573;}); break; case 86:
_LL3572:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp3579= Cyc_Parse_lbuf;
if( _temp3579 == 0){ _throw( Null_Exception);} _temp3579->v;}))->lex_curr_pos -=
1; yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3580=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3580[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3581; _temp3581.tag= Cyc_TypeList_tok; _temp3581.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3582= yyvs; void** _temp3584= _temp3582.curr +( yyvsp_offset -
1); if( _temp3582.base == 0? 1:( _temp3584 < _temp3582.base? 1: _temp3584 >=
_temp3582.last_plus_one)){ _throw( Null_Exception);}* _temp3584;}))); _temp3581;});
_temp3580;}); break; case 87: _LL3578: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3586=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3586[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3587; _temp3587.tag=
Cyc_StructOrUnion_tok; _temp3587.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp3587;}); _temp3586;}); break; case 88: _LL3585: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp3589=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3589[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp3590; _temp3590.tag=
Cyc_StructOrUnion_tok; _temp3590.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp3590;}); _temp3589;}); break; case 89: _LL3588: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3592=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3592[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp3593; _temp3593.tag= Cyc_StructFieldDeclList_tok; _temp3593.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3594= yyvs; void** _temp3596= _temp3594.curr +
yyvsp_offset; if( _temp3594.base == 0? 1:( _temp3596 < _temp3594.base? 1:
_temp3596 >= _temp3594.last_plus_one)){ _throw( Null_Exception);}* _temp3596;}))));
_temp3593;}); _temp3592;}); break; case 90: _LL3591: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3598=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3598[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3599; _temp3599.tag= Cyc_StructFieldDeclListList_tok; _temp3599.f1=({
struct Cyc_List_List* _temp3600=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3600->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3601= yyvs; void** _temp3603= _temp3601.curr +
yyvsp_offset; if( _temp3601.base == 0? 1:( _temp3603 < _temp3601.base? 1:
_temp3603 >= _temp3601.last_plus_one)){ _throw( Null_Exception);}* _temp3603;}));
_temp3600->tl= 0; _temp3600;}); _temp3599;}); _temp3598;}); break; case 91:
_LL3597: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3605=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3605[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp3606; _temp3606.tag= Cyc_StructFieldDeclListList_tok; _temp3606.f1=({
struct Cyc_List_List* _temp3607=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3607->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3611= yyvs; void** _temp3613= _temp3611.curr +
yyvsp_offset; if( _temp3611.base == 0? 1:( _temp3613 < _temp3611.base? 1:
_temp3613 >= _temp3611.last_plus_one)){ _throw( Null_Exception);}* _temp3613;}));
_temp3607->tl= Cyc_yyget_StructFieldDeclListList_tok(({ struct _tagged_ptr2
_temp3608= yyvs; void** _temp3610= _temp3608.curr +( yyvsp_offset - 1); if(
_temp3608.base == 0? 1:( _temp3610 < _temp3608.base? 1: _temp3610 >= _temp3608.last_plus_one)){
_throw( Null_Exception);}* _temp3610;})); _temp3607;}); _temp3606;}); _temp3605;});
break; case 92: _LL3604: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp3615=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3615[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3616; _temp3616.tag=
Cyc_InitDeclList_tok; _temp3616.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2
_temp3617= yyvs; void** _temp3619= _temp3617.curr + yyvsp_offset; if( _temp3617.base
== 0? 1:( _temp3619 < _temp3617.base? 1: _temp3619 >= _temp3617.last_plus_one)){
_throw( Null_Exception);}* _temp3619;}))); _temp3616;}); _temp3615;}); break;
case 93: _LL3614: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp3621=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3621[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3622; _temp3622.tag=
Cyc_InitDeclList_tok; _temp3622.f1=({ struct Cyc_List_List* _temp3623=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3623->hd=( void*)
Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3624= yyvs; void** _temp3626=
_temp3624.curr + yyvsp_offset; if( _temp3624.base == 0? 1:( _temp3626 <
_temp3624.base? 1: _temp3626 >= _temp3624.last_plus_one)){ _throw(
Null_Exception);}* _temp3626;})); _temp3623->tl= 0; _temp3623;}); _temp3622;});
_temp3621;}); break; case 94: _LL3620: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct*
_temp3628=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3628[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp3629; _temp3629.tag=
Cyc_InitDeclList_tok; _temp3629.f1=({ struct Cyc_List_List* _temp3630=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3630->hd=( void*)
Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3634= yyvs; void** _temp3636=
_temp3634.curr + yyvsp_offset; if( _temp3634.base == 0? 1:( _temp3636 <
_temp3634.base? 1: _temp3636 >= _temp3634.last_plus_one)){ _throw(
Null_Exception);}* _temp3636;})); _temp3630->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3631= yyvs; void** _temp3633= _temp3631.curr +(
yyvsp_offset - 2); if( _temp3631.base == 0? 1:( _temp3633 < _temp3631.base? 1:
_temp3633 >= _temp3631.last_plus_one)){ _throw( Null_Exception);}* _temp3633;}));
_temp3630;}); _temp3629;}); _temp3628;}); break; case 95: _LL3627: yyval=( void*)({
struct Cyc_InitDecl_tok_struct* _temp3638=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp3638[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp3639; _temp3639.tag= Cyc_InitDecl_tok; _temp3639.f1=({ struct _tuple14*
_temp3640=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3640->f1=
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3641= yyvs; void**
_temp3643= _temp3641.curr + yyvsp_offset; if( _temp3641.base == 0? 1:( _temp3643
< _temp3641.base? 1: _temp3643 >= _temp3641.last_plus_one)){ _throw(
Null_Exception);}* _temp3643;})); _temp3640->f2= 0; _temp3640;}); _temp3639;});
_temp3638;}); break; case 96: _LL3637: yyval=( void*)({ struct Cyc_InitDecl_tok_struct*
_temp3645=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3645[ 0]=({ struct Cyc_InitDecl_tok_struct _temp3646; _temp3646.tag= Cyc_InitDecl_tok;
_temp3646.f1=({ struct _tuple14* _temp3647=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3647->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3651= yyvs; void** _temp3653= _temp3651.curr +( yyvsp_offset -
2); if( _temp3651.base == 0? 1:( _temp3653 < _temp3651.base? 1: _temp3653 >=
_temp3651.last_plus_one)){ _throw( Null_Exception);}* _temp3653;})); _temp3647->f2=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3648= yyvs;
void** _temp3650= _temp3648.curr + yyvsp_offset; if( _temp3648.base == 0? 1:(
_temp3650 < _temp3648.base? 1: _temp3650 >= _temp3648.last_plus_one)){ _throw(
Null_Exception);}* _temp3650;})); _temp3647;}); _temp3646;}); _temp3645;});
break; case 97: _LL3644: { struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3690= yyvs; void** _temp3692= _temp3690.curr +(
yyvsp_offset - 2); if( _temp3690.base == 0? 1:( _temp3692 < _temp3690.base? 1:
_temp3692 >= _temp3690.last_plus_one)){ _throw( Null_Exception);}* _temp3692;}))).f1;
struct Cyc_List_List* _temp3658=(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3655= yyvs; void** _temp3657= _temp3655.curr +( yyvsp_offset -
2); if( _temp3655.base == 0? 1:( _temp3657 < _temp3655.base? 1: _temp3657 >=
_temp3655.last_plus_one)){ _throw( Null_Exception);}* _temp3657;}))).f3; goto
_LL3659; _LL3659: { void* _temp3669= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3660= yyvs; void** _temp3662= _temp3660.curr +(
yyvsp_offset - 2); if( _temp3660.base == 0? 1:( _temp3662 < _temp3660.base? 1:
_temp3662 >= _temp3660.last_plus_one)){ _throw( Null_Exception);}* _temp3662;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3663= yyls; struct Cyc_Yyltype*
_temp3665= _temp3663.curr +( yylsp_offset - 2); if( _temp3663.base == 0? 1:(
_temp3665 < _temp3663.base? 1: _temp3665 >= _temp3663.last_plus_one)){ _throw(
Null_Exception);}* _temp3665;}).first_line,({ struct _tagged_ptr3 _temp3666=
yyls; struct Cyc_Yyltype* _temp3668= _temp3666.curr +( yylsp_offset - 2); if(
_temp3666.base == 0? 1:( _temp3668 < _temp3666.base? 1: _temp3668 >= _temp3666.last_plus_one)){
_throw( Null_Exception);}* _temp3668;}).last_line)); goto _LL3670; _LL3670: {
struct Cyc_List_List* _temp3676; struct Cyc_List_List* _temp3678; struct _tuple0
_temp3674=(( struct _tuple0(*)( struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3671= yyvs; void** _temp3673= _temp3671.curr +(
yyvsp_offset - 1); if( _temp3671.base == 0? 1:( _temp3673 < _temp3671.base? 1:
_temp3673 >= _temp3671.last_plus_one)){ _throw( Null_Exception);}* _temp3673;})));
_LL3679: _temp3678= _temp3674.f1; goto _LL3677; _LL3677: _temp3676= _temp3674.f2;
goto _LL3675; _LL3675: { struct Cyc_List_List* _temp3680=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss(
tq, _temp3669, _temp3678, _temp3658), _temp3676); goto _LL3681; _LL3681: yyval=(
void*)({ struct Cyc_StructFieldDeclList_tok_struct* _temp3682=( struct Cyc_StructFieldDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct)); _temp3682[ 0]=({
struct Cyc_StructFieldDeclList_tok_struct _temp3683; _temp3683.tag= Cyc_StructFieldDeclList_tok;
_temp3683.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Position_Segment*, struct _tuple8*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3684= yyls; struct Cyc_Yyltype* _temp3686= _temp3684.curr
+( yylsp_offset - 2); if( _temp3684.base == 0? 1:( _temp3686 < _temp3684.base? 1:
_temp3686 >= _temp3684.last_plus_one)){ _throw( Null_Exception);}* _temp3686;}).first_line,({
struct _tagged_ptr3 _temp3687= yyls; struct Cyc_Yyltype* _temp3689= _temp3687.curr
+( yylsp_offset - 1); if( _temp3687.base == 0? 1:( _temp3689 < _temp3687.base? 1:
_temp3689 >= _temp3687.last_plus_one)){ _throw( Null_Exception);}* _temp3689;}).last_line),
_temp3680); _temp3683;}); _temp3682;}); break;}}}} case 98: _LL3654: yyval=(
void*)({ struct Cyc_QualSpecList_tok_struct* _temp3694=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3694[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3695; _temp3695.tag= Cyc_QualSpecList_tok;
_temp3695.f1=({ struct _tuple17* _temp3696=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3696->f1= Cyc_Absyn_empty_tqual(); _temp3696->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp3700=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3700->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3701= yyvs; void** _temp3703= _temp3701.curr +(
yyvsp_offset - 1); if( _temp3701.base == 0? 1:( _temp3703 < _temp3701.base? 1:
_temp3703 >= _temp3701.last_plus_one)){ _throw( Null_Exception);}* _temp3703;}));
_temp3700->tl= 0; _temp3700;}); _temp3696->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3697= yyvs; void** _temp3699= _temp3697.curr +
yyvsp_offset; if( _temp3697.base == 0? 1:( _temp3699 < _temp3697.base? 1:
_temp3699 >= _temp3697.last_plus_one)){ _throw( Null_Exception);}* _temp3699;}));
_temp3696;}); _temp3695;}); _temp3694;}); break; case 99: _LL3693: yyval=( void*)({
struct Cyc_QualSpecList_tok_struct* _temp3705=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3705[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3706; _temp3706.tag= Cyc_QualSpecList_tok;
_temp3706.f1=({ struct _tuple17* _temp3707=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3707->f1=(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3721= yyvs; void** _temp3723= _temp3721.curr + yyvsp_offset;
if( _temp3721.base == 0? 1:( _temp3723 < _temp3721.base? 1: _temp3723 >=
_temp3721.last_plus_one)){ _throw( Null_Exception);}* _temp3723;}))).f1;
_temp3707->f2=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp3714=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3714->hd=(
void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2 _temp3718= yyvs; void**
_temp3720= _temp3718.curr +( yyvsp_offset - 2); if( _temp3718.base == 0? 1:(
_temp3720 < _temp3718.base? 1: _temp3720 >= _temp3718.last_plus_one)){ _throw(
Null_Exception);}* _temp3720;})); _temp3714->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3715= yyvs; void** _temp3717= _temp3715.curr +
yyvsp_offset; if( _temp3715.base == 0? 1:( _temp3717 < _temp3715.base? 1:
_temp3717 >= _temp3715.last_plus_one)){ _throw( Null_Exception);}* _temp3717;}))).f2;
_temp3714;}); _temp3707->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3708= yyvs; void** _temp3710= _temp3708.curr +(
yyvsp_offset - 1); if( _temp3708.base == 0? 1:( _temp3710 < _temp3708.base? 1:
_temp3710 >= _temp3708.last_plus_one)){ _throw( Null_Exception);}* _temp3710;})),(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3711= yyvs; void**
_temp3713= _temp3711.curr + yyvsp_offset; if( _temp3711.base == 0? 1:( _temp3713
< _temp3711.base? 1: _temp3713 >= _temp3711.last_plus_one)){ _throw(
Null_Exception);}* _temp3713;}))).f3); _temp3707;}); _temp3706;}); _temp3705;});
break; case 100: _LL3704: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp3725=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3725[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp3726; _temp3726.tag=
Cyc_QualSpecList_tok; _temp3726.f1=({ struct _tuple17* _temp3727=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp3727->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3731= yyvs; void** _temp3733= _temp3731.curr +(
yyvsp_offset - 1); if( _temp3731.base == 0? 1:( _temp3733 < _temp3731.base? 1:
_temp3733 >= _temp3731.last_plus_one)){ _throw( Null_Exception);}* _temp3733;}));
_temp3727->f2= 0; _temp3727->f3= Cyc_yyget_AttributeList_tok(({ struct
_tagged_ptr2 _temp3728= yyvs; void** _temp3730= _temp3728.curr + yyvsp_offset;
if( _temp3728.base == 0? 1:( _temp3730 < _temp3728.base? 1: _temp3730 >=
_temp3728.last_plus_one)){ _throw( Null_Exception);}* _temp3730;})); _temp3727;});
_temp3726;}); _temp3725;}); break; case 101: _LL3724: yyval=( void*)({ struct
Cyc_QualSpecList_tok_struct* _temp3735=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp3735[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp3736; _temp3736.tag= Cyc_QualSpecList_tok;
_temp3736.f1=({ struct _tuple17* _temp3737=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3737->f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3747= yyvs; void** _temp3749= _temp3747.curr +(
yyvsp_offset - 2); if( _temp3747.base == 0? 1:( _temp3749 < _temp3747.base? 1:
_temp3749 >= _temp3747.last_plus_one)){ _throw( Null_Exception);}* _temp3749;})),(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3750= yyvs; void**
_temp3752= _temp3750.curr + yyvsp_offset; if( _temp3750.base == 0? 1:( _temp3752
< _temp3750.base? 1: _temp3752 >= _temp3750.last_plus_one)){ _throw(
Null_Exception);}* _temp3752;}))).f1); _temp3737->f2=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3744= yyvs; void** _temp3746= _temp3744.curr +
yyvsp_offset; if( _temp3744.base == 0? 1:( _temp3746 < _temp3744.base? 1:
_temp3746 >= _temp3744.last_plus_one)){ _throw( Null_Exception);}* _temp3746;}))).f2;
_temp3737->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp3738= yyvs; void** _temp3740= _temp3738.curr +( yyvsp_offset - 1); if(
_temp3738.base == 0? 1:( _temp3740 < _temp3738.base? 1: _temp3740 >= _temp3738.last_plus_one)){
_throw( Null_Exception);}* _temp3740;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3741= yyvs; void** _temp3743= _temp3741.curr + yyvsp_offset;
if( _temp3741.base == 0? 1:( _temp3743 < _temp3741.base? 1: _temp3743 >=
_temp3741.last_plus_one)){ _throw( Null_Exception);}* _temp3743;}))).f3);
_temp3737;}); _temp3736;}); _temp3735;}); break; case 102: _LL3734: yyval=( void*)({
struct Cyc_DeclaratorExpoptList_tok_struct* _temp3754=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3754[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp3755; _temp3755.tag= Cyc_DeclaratorExpoptList_tok;
_temp3755.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(({ struct _tagged_ptr2 _temp3756= yyvs; void**
_temp3758= _temp3756.curr + yyvsp_offset; if( _temp3756.base == 0? 1:( _temp3758
< _temp3756.base? 1: _temp3758 >= _temp3756.last_plus_one)){ _throw(
Null_Exception);}* _temp3758;}))); _temp3755;}); _temp3754;}); break; case 103:
_LL3753: yyval=( void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp3760=(
struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct));
_temp3760[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct _temp3761; _temp3761.tag=
Cyc_DeclaratorExpoptList_tok; _temp3761.f1=({ struct Cyc_List_List* _temp3762=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3762->hd=(
void*) Cyc_yyget_DeclaratorExpopt_tok(({ struct _tagged_ptr2 _temp3763= yyvs;
void** _temp3765= _temp3763.curr + yyvsp_offset; if( _temp3763.base == 0? 1:(
_temp3765 < _temp3763.base? 1: _temp3765 >= _temp3763.last_plus_one)){ _throw(
Null_Exception);}* _temp3765;})); _temp3762->tl= 0; _temp3762;}); _temp3761;});
_temp3760;}); break; case 104: _LL3759: yyval=( void*)({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp3767=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3767[ 0]=({ struct Cyc_DeclaratorExpoptList_tok_struct
_temp3768; _temp3768.tag= Cyc_DeclaratorExpoptList_tok; _temp3768.f1=({ struct
Cyc_List_List* _temp3769=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3769->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({ struct _tagged_ptr2
_temp3773= yyvs; void** _temp3775= _temp3773.curr + yyvsp_offset; if( _temp3773.base
== 0? 1:( _temp3775 < _temp3773.base? 1: _temp3775 >= _temp3773.last_plus_one)){
_throw( Null_Exception);}* _temp3775;})); _temp3769->tl= Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3770= yyvs; void** _temp3772= _temp3770.curr +(
yyvsp_offset - 2); if( _temp3770.base == 0? 1:( _temp3772 < _temp3770.base? 1:
_temp3772 >= _temp3770.last_plus_one)){ _throw( Null_Exception);}* _temp3772;}));
_temp3769;}); _temp3768;}); _temp3767;}); break; case 105: _LL3766: yyval=( void*)({
struct Cyc_DeclaratorExpopt_tok_struct* _temp3777=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp3777[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp3778; _temp3778.tag= Cyc_DeclaratorExpopt_tok;
_temp3778.f1=({ struct _tuple12* _temp3779=( struct _tuple12*) GC_malloc(
sizeof( struct _tuple12)); _temp3779->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3780= yyvs; void** _temp3782= _temp3780.curr + yyvsp_offset;
if( _temp3780.base == 0? 1:( _temp3782 < _temp3780.base? 1: _temp3782 >=
_temp3780.last_plus_one)){ _throw( Null_Exception);}* _temp3782;})); _temp3779->f2=
0; _temp3779;}); _temp3778;}); _temp3777;}); break; case 106: _LL3776: yyval=(
void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp3784=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp3784[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp3785; _temp3785.tag= Cyc_DeclaratorExpopt_tok;
_temp3785.f1=({ struct _tuple12* _temp3786=( struct _tuple12*) GC_malloc(
sizeof( struct _tuple12)); _temp3786->f1=({ struct Cyc_Parse_Declarator*
_temp3791=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3791->id=({ struct _tuple1* _temp3792=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3792->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp3796=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp3796[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp3797; _temp3797.tag= Cyc_Absyn_Rel_n;
_temp3797.f1= 0; _temp3797;}); _temp3796;}); _temp3792->f2=({ struct
_tagged_string* _temp3793=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3793[ 0]=( struct _tagged_string)({ char* _temp3794=(
char*)""; struct _tagged_string _temp3795; _temp3795.curr= _temp3794; _temp3795.base=
_temp3794; _temp3795.last_plus_one= _temp3794 + 1; _temp3795;}); _temp3793;});
_temp3792;}); _temp3791->tms= 0; _temp3791;}); _temp3786->f2=({ struct Cyc_Core_Opt*
_temp3787=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3787->v=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3788= yyvs;
void** _temp3790= _temp3788.curr + yyvsp_offset; if( _temp3788.base == 0? 1:(
_temp3790 < _temp3788.base? 1: _temp3790 >= _temp3788.last_plus_one)){ _throw(
Null_Exception);}* _temp3790;})); _temp3787;}); _temp3786;}); _temp3785;});
_temp3784;}); break; case 107: _LL3783: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp3799=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp3799[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp3800; _temp3800.tag= Cyc_DeclaratorExpopt_tok; _temp3800.f1=({ struct
_tuple12* _temp3801=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp3801->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3806= yyvs;
void** _temp3808= _temp3806.curr +( yyvsp_offset - 2); if( _temp3806.base == 0?
1:( _temp3808 < _temp3806.base? 1: _temp3808 >= _temp3806.last_plus_one)){
_throw( Null_Exception);}* _temp3808;})); _temp3801->f2=({ struct Cyc_Core_Opt*
_temp3802=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3802->v=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3803= yyvs;
void** _temp3805= _temp3803.curr + yyvsp_offset; if( _temp3803.base == 0? 1:(
_temp3805 < _temp3803.base? 1: _temp3805 >= _temp3803.last_plus_one)){ _throw(
Null_Exception);}* _temp3805;})); _temp3802;}); _temp3801;}); _temp3800;});
_temp3799;}); break; case 108: _LL3798: { struct Cyc_List_List* _temp3819=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3810=
yyls; struct Cyc_Yyltype* _temp3812= _temp3810.curr +( yylsp_offset - 3); if(
_temp3810.base == 0? 1:( _temp3812 < _temp3810.base? 1: _temp3812 >= _temp3810.last_plus_one)){
_throw( Null_Exception);}* _temp3812;}).first_line,({ struct _tagged_ptr3
_temp3813= yyls; struct Cyc_Yyltype* _temp3815= _temp3813.curr +( yylsp_offset -
3); if( _temp3813.base == 0? 1:( _temp3815 < _temp3813.base? 1: _temp3815 >=
_temp3813.last_plus_one)){ _throw( Null_Exception);}* _temp3815;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3816= yyvs; void** _temp3818=
_temp3816.curr +( yyvsp_offset - 3); if( _temp3816.base == 0? 1:( _temp3818 <
_temp3816.base? 1: _temp3818 >= _temp3816.last_plus_one)){ _throw(
Null_Exception);}* _temp3818;}))); goto _LL3820; _LL3820: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp3821=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3821[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3822; _temp3822.tag= Cyc_TypeSpecifier_tok;
_temp3822.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3823=(
struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3823[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp3824; _temp3824.tag= Cyc_Parse_Decl_spec;
_temp3824.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3825= yyvs; void** _temp3827= _temp3825.curr +(
yyvsp_offset - 4); if( _temp3825.base == 0? 1:( _temp3827 < _temp3825.base? 1:
_temp3827 >= _temp3825.last_plus_one)){ _throw( Null_Exception);}* _temp3827;})),
_temp3819,({ struct Cyc_Core_Opt* _temp3828=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3828->v=( void*) Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3829= yyvs; void** _temp3831= _temp3829.curr +(
yyvsp_offset - 1); if( _temp3829.base == 0? 1:( _temp3831 < _temp3829.base? 1:
_temp3831 >= _temp3829.last_plus_one)){ _throw( Null_Exception);}* _temp3831;}));
_temp3828;}), Cyc_yyget_Bool_tok(({ struct _tagged_ptr2 _temp3832= yyvs; void**
_temp3834= _temp3832.curr +( yyvsp_offset - 5); if( _temp3832.base == 0? 1:(
_temp3834 < _temp3832.base? 1: _temp3834 >= _temp3832.last_plus_one)){ _throw(
Null_Exception);}* _temp3834;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3835= yyls; struct Cyc_Yyltype* _temp3837= _temp3835.curr +(
yylsp_offset - 5); if( _temp3835.base == 0? 1:( _temp3837 < _temp3835.base? 1:
_temp3837 >= _temp3835.last_plus_one)){ _throw( Null_Exception);}* _temp3837;}).first_line,({
struct _tagged_ptr3 _temp3838= yyls; struct Cyc_Yyltype* _temp3840= _temp3838.curr
+ yylsp_offset; if( _temp3838.base == 0? 1:( _temp3840 < _temp3838.base? 1:
_temp3840 >= _temp3838.last_plus_one)){ _throw( Null_Exception);}* _temp3840;}).last_line));
_temp3824;}); _temp3823;})); _temp3822;}); _temp3821;}); break;} case 109:
_LL3809: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp3842=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3842[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3843; _temp3843.tag=
Cyc_TypeSpecifier_tok; _temp3843.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp3844=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp3844[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp3845; _temp3845.tag= Cyc_Absyn_TunionType;
_temp3845.f1=({ struct Cyc_Absyn_TunionInfo _temp3846; _temp3846.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp3850=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp3850[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp3851; _temp3851.tag= Cyc_Absyn_UnknownTunion;
_temp3851.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp3852; _temp3852.name=
Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3856= yyvs; void** _temp3858=
_temp3856.curr +( yyvsp_offset - 1); if( _temp3856.base == 0? 1:( _temp3858 <
_temp3856.base? 1: _temp3858 >= _temp3856.last_plus_one)){ _throw(
Null_Exception);}* _temp3858;})); _temp3852.is_xtunion= Cyc_yyget_Bool_tok(({
struct _tagged_ptr2 _temp3853= yyvs; void** _temp3855= _temp3853.curr +(
yyvsp_offset - 2); if( _temp3853.base == 0? 1:( _temp3855 < _temp3853.base? 1:
_temp3855 >= _temp3853.last_plus_one)){ _throw( Null_Exception);}* _temp3855;}));
_temp3852;}); _temp3851;}); _temp3850;})); _temp3846.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3847= yyvs; void** _temp3849= _temp3847.curr +
yyvsp_offset; if( _temp3847.base == 0? 1:( _temp3849 < _temp3847.base? 1:
_temp3849 >= _temp3847.last_plus_one)){ _throw( Null_Exception);}* _temp3849;}));
_temp3846.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp3846;}); _temp3845;});
_temp3844;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3859= yyls;
struct Cyc_Yyltype* _temp3861= _temp3859.curr +( yylsp_offset - 2); if(
_temp3859.base == 0? 1:( _temp3861 < _temp3859.base? 1: _temp3861 >= _temp3859.last_plus_one)){
_throw( Null_Exception);}* _temp3861;}).first_line,({ struct _tagged_ptr3
_temp3862= yyls; struct Cyc_Yyltype* _temp3864= _temp3862.curr + yylsp_offset;
if( _temp3862.base == 0? 1:( _temp3864 < _temp3862.base? 1: _temp3864 >=
_temp3862.last_plus_one)){ _throw( Null_Exception);}* _temp3864;}).last_line));
_temp3843;}); _temp3842;}); break; case 110: _LL3841: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp3866=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3866[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp3867; _temp3867.tag= Cyc_TypeSpecifier_tok;
_temp3867.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp3868=( struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp3868[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp3869; _temp3869.tag=
Cyc_Absyn_TunionType; _temp3869.f1=({ struct Cyc_Absyn_TunionInfo _temp3870;
_temp3870.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct*
_temp3877=( struct Cyc_Absyn_UnknownTunion_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownTunion_struct)); _temp3877[ 0]=({ struct Cyc_Absyn_UnknownTunion_struct
_temp3878; _temp3878.tag= Cyc_Absyn_UnknownTunion; _temp3878.f1=({ struct Cyc_Absyn_UnknownTunionInfo
_temp3879; _temp3879.name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3883=
yyvs; void** _temp3885= _temp3883.curr +( yyvsp_offset - 1); if( _temp3883.base
== 0? 1:( _temp3885 < _temp3883.base? 1: _temp3885 >= _temp3883.last_plus_one)){
_throw( Null_Exception);}* _temp3885;})); _temp3879.is_xtunion= Cyc_yyget_Bool_tok(({
struct _tagged_ptr2 _temp3880= yyvs; void** _temp3882= _temp3880.curr +(
yyvsp_offset - 3); if( _temp3880.base == 0? 1:( _temp3882 < _temp3880.base? 1:
_temp3882 >= _temp3880.last_plus_one)){ _throw( Null_Exception);}* _temp3882;}));
_temp3879;}); _temp3878;}); _temp3877;})); _temp3870.targs= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3874= yyvs; void** _temp3876= _temp3874.curr +
yyvsp_offset; if( _temp3874.base == 0? 1:( _temp3876 < _temp3874.base? 1:
_temp3876 >= _temp3874.last_plus_one)){ _throw( Null_Exception);}* _temp3876;}));
_temp3870.rgn=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp3871= yyvs;
void** _temp3873= _temp3871.curr +( yyvsp_offset - 2); if( _temp3871.base == 0?
1:( _temp3873 < _temp3871.base? 1: _temp3873 >= _temp3871.last_plus_one)){
_throw( Null_Exception);}* _temp3873;})); _temp3870;}); _temp3869;}); _temp3868;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3886= yyls; struct Cyc_Yyltype*
_temp3888= _temp3886.curr +( yylsp_offset - 3); if( _temp3886.base == 0? 1:(
_temp3888 < _temp3886.base? 1: _temp3888 >= _temp3886.last_plus_one)){ _throw(
Null_Exception);}* _temp3888;}).first_line,({ struct _tagged_ptr3 _temp3889=
yyls; struct Cyc_Yyltype* _temp3891= _temp3889.curr + yylsp_offset; if(
_temp3889.base == 0? 1:( _temp3891 < _temp3889.base? 1: _temp3891 >= _temp3889.last_plus_one)){
_throw( Null_Exception);}* _temp3891;}).last_line)); _temp3867;}); _temp3866;});
break; case 111: _LL3865: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp3893=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3893[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp3894; _temp3894.tag=
Cyc_TypeSpecifier_tok; _temp3894.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionFieldType_struct* _temp3895=( struct Cyc_Absyn_TunionFieldType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct)); _temp3895[ 0]=({
struct Cyc_Absyn_TunionFieldType_struct _temp3896; _temp3896.tag= Cyc_Absyn_TunionFieldType;
_temp3896.f1=({ struct Cyc_Absyn_TunionFieldInfo _temp3897; _temp3897.field_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct* _temp3901=( struct
Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunionfield_struct));
_temp3901[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct _temp3902; _temp3902.tag=
Cyc_Absyn_UnknownTunionfield; _temp3902.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp3903; _temp3903.tunion_name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3910= yyvs; void** _temp3912= _temp3910.curr +( yyvsp_offset - 3); if(
_temp3910.base == 0? 1:( _temp3912 < _temp3910.base? 1: _temp3912 >= _temp3910.last_plus_one)){
_throw( Null_Exception);}* _temp3912;})); _temp3903.field_name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3907= yyvs; void** _temp3909= _temp3907.curr +(
yyvsp_offset - 1); if( _temp3907.base == 0? 1:( _temp3909 < _temp3907.base? 1:
_temp3909 >= _temp3907.last_plus_one)){ _throw( Null_Exception);}* _temp3909;}));
_temp3903.is_xtunion= Cyc_yyget_Bool_tok(({ struct _tagged_ptr2 _temp3904= yyvs;
void** _temp3906= _temp3904.curr +( yyvsp_offset - 4); if( _temp3904.base == 0?
1:( _temp3906 < _temp3904.base? 1: _temp3906 >= _temp3904.last_plus_one)){
_throw( Null_Exception);}* _temp3906;})); _temp3903;}); _temp3902;}); _temp3901;}));
_temp3897.targs= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3898= yyvs;
void** _temp3900= _temp3898.curr + yyvsp_offset; if( _temp3898.base == 0? 1:(
_temp3900 < _temp3898.base? 1: _temp3900 >= _temp3898.last_plus_one)){ _throw(
Null_Exception);}* _temp3900;})); _temp3897;}); _temp3896;}); _temp3895;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3913= yyls; struct Cyc_Yyltype* _temp3915= _temp3913.curr
+( yylsp_offset - 4); if( _temp3913.base == 0? 1:( _temp3915 < _temp3913.base? 1:
_temp3915 >= _temp3913.last_plus_one)){ _throw( Null_Exception);}* _temp3915;}).first_line,({
struct _tagged_ptr3 _temp3916= yyls; struct Cyc_Yyltype* _temp3918= _temp3916.curr
+ yylsp_offset; if( _temp3916.base == 0? 1:( _temp3918 < _temp3916.base? 1:
_temp3918 >= _temp3916.last_plus_one)){ _throw( Null_Exception);}* _temp3918;}).last_line));
_temp3894;}); _temp3893;}); break; case 112: _LL3892: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp3920=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp3920[ 0]=({ struct Cyc_Bool_tok_struct
_temp3921; _temp3921.tag= Cyc_Bool_tok; _temp3921.f1= 0; _temp3921;}); _temp3920;});
break; case 113: _LL3919: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp3923=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp3923[ 0]=({ struct Cyc_Bool_tok_struct _temp3924; _temp3924.tag= Cyc_Bool_tok;
_temp3924.f1= 1; _temp3924;}); _temp3923;}); break; case 114: _LL3922: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp3926=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp3926[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp3927; _temp3927.tag= Cyc_TunionFieldList_tok;
_temp3927.f1=({ struct Cyc_List_List* _temp3928=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3928->hd=( void*) Cyc_yyget_TunionField_tok(({
struct _tagged_ptr2 _temp3929= yyvs; void** _temp3931= _temp3929.curr +
yyvsp_offset; if( _temp3929.base == 0? 1:( _temp3931 < _temp3929.base? 1:
_temp3931 >= _temp3929.last_plus_one)){ _throw( Null_Exception);}* _temp3931;}));
_temp3928->tl= 0; _temp3928;}); _temp3927;}); _temp3926;}); break; case 115:
_LL3925: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct* _temp3933=(
struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct));
_temp3933[ 0]=({ struct Cyc_TunionFieldList_tok_struct _temp3934; _temp3934.tag=
Cyc_TunionFieldList_tok; _temp3934.f1=({ struct Cyc_List_List* _temp3935=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3935->hd=(
void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2 _temp3936= yyvs; void**
_temp3938= _temp3936.curr +( yyvsp_offset - 1); if( _temp3936.base == 0? 1:(
_temp3938 < _temp3936.base? 1: _temp3938 >= _temp3936.last_plus_one)){ _throw(
Null_Exception);}* _temp3938;})); _temp3935->tl= 0; _temp3935;}); _temp3934;});
_temp3933;}); break; case 116: _LL3932: yyval=( void*)({ struct Cyc_TunionFieldList_tok_struct*
_temp3940=( struct Cyc_TunionFieldList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TunionFieldList_tok_struct)); _temp3940[ 0]=({ struct Cyc_TunionFieldList_tok_struct
_temp3941; _temp3941.tag= Cyc_TunionFieldList_tok; _temp3941.f1=({ struct Cyc_List_List*
_temp3942=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3942->hd=( void*) Cyc_yyget_TunionField_tok(({ struct _tagged_ptr2
_temp3946= yyvs; void** _temp3948= _temp3946.curr +( yyvsp_offset - 2); if(
_temp3946.base == 0? 1:( _temp3948 < _temp3946.base? 1: _temp3948 >= _temp3946.last_plus_one)){
_throw( Null_Exception);}* _temp3948;})); _temp3942->tl= Cyc_yyget_TunionFieldList_tok(({
struct _tagged_ptr2 _temp3943= yyvs; void** _temp3945= _temp3943.curr +
yyvsp_offset; if( _temp3943.base == 0? 1:( _temp3945 < _temp3943.base? 1:
_temp3945 >= _temp3943.last_plus_one)){ _throw( Null_Exception);}* _temp3945;}));
_temp3942;}); _temp3941;}); _temp3940;}); break; case 117: _LL3939: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp3950=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp3950[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp3951; _temp3951.tag= Cyc_TunionFieldList_tok;
_temp3951.f1=({ struct Cyc_List_List* _temp3952=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3952->hd=( void*) Cyc_yyget_TunionField_tok(({
struct _tagged_ptr2 _temp3956= yyvs; void** _temp3958= _temp3956.curr +(
yyvsp_offset - 2); if( _temp3956.base == 0? 1:( _temp3958 < _temp3956.base? 1:
_temp3958 >= _temp3956.last_plus_one)){ _throw( Null_Exception);}* _temp3958;}));
_temp3952->tl= Cyc_yyget_TunionFieldList_tok(({ struct _tagged_ptr2 _temp3953=
yyvs; void** _temp3955= _temp3953.curr + yyvsp_offset; if( _temp3953.base == 0?
1:( _temp3955 < _temp3953.base? 1: _temp3955 >= _temp3953.last_plus_one)){
_throw( Null_Exception);}* _temp3955;})); _temp3952;}); _temp3951;}); _temp3950;});
break; case 118: _LL3949: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp3960=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp3960[ 0]=({ struct Cyc_Scope_tok_struct _temp3961; _temp3961.tag= Cyc_Scope_tok;
_temp3961.f1=( void*)(( void*) Cyc_Absyn_Public); _temp3961;}); _temp3960;});
break; case 119: _LL3959: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp3963=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp3963[ 0]=({ struct Cyc_Scope_tok_struct _temp3964; _temp3964.tag= Cyc_Scope_tok;
_temp3964.f1=( void*)(( void*) Cyc_Absyn_Extern); _temp3964;}); _temp3963;});
break; case 120: _LL3962: yyval=( void*)({ struct Cyc_Scope_tok_struct*
_temp3966=( struct Cyc_Scope_tok_struct*) GC_malloc( sizeof( struct Cyc_Scope_tok_struct));
_temp3966[ 0]=({ struct Cyc_Scope_tok_struct _temp3967; _temp3967.tag= Cyc_Scope_tok;
_temp3967.f1=( void*)(( void*) Cyc_Absyn_Static); _temp3967;}); _temp3966;});
break; case 121: _LL3965: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp3969=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp3969[ 0]=({ struct Cyc_TunionField_tok_struct _temp3970; _temp3970.tag= Cyc_TunionField_tok;
_temp3970.f1=({ struct Cyc_Absyn_Tunionfield* _temp3971=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp3971->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3981= yyvs; void** _temp3983= _temp3981.curr +
yyvsp_offset; if( _temp3981.base == 0? 1:( _temp3983 < _temp3981.base? 1:
_temp3983 >= _temp3981.last_plus_one)){ _throw( Null_Exception);}* _temp3983;}));
_temp3971->tvs= 0; _temp3971->typs= 0; _temp3971->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3975= yyls; struct Cyc_Yyltype* _temp3977= _temp3975.curr
+( yylsp_offset - 1); if( _temp3975.base == 0? 1:( _temp3977 < _temp3975.base? 1:
_temp3977 >= _temp3975.last_plus_one)){ _throw( Null_Exception);}* _temp3977;}).first_line,({
struct _tagged_ptr3 _temp3978= yyls; struct Cyc_Yyltype* _temp3980= _temp3978.curr
+ yylsp_offset; if( _temp3978.base == 0? 1:( _temp3980 < _temp3978.base? 1:
_temp3980 >= _temp3978.last_plus_one)){ _throw( Null_Exception);}* _temp3980;}).last_line);
_temp3971->sc=( void*) Cyc_yyget_Scope_tok(({ struct _tagged_ptr2 _temp3972=
yyvs; void** _temp3974= _temp3972.curr +( yyvsp_offset - 1); if( _temp3972.base
== 0? 1:( _temp3974 < _temp3972.base? 1: _temp3974 >= _temp3972.last_plus_one)){
_throw( Null_Exception);}* _temp3974;})); _temp3971;}); _temp3970;}); _temp3969;});
break; case 122: _LL3968: { struct Cyc_List_List* _temp3994=(( struct Cyc_List_List*(*)(
struct _tuple10*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct
Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3985= yyls; struct Cyc_Yyltype*
_temp3987= _temp3985.curr +( yylsp_offset - 1); if( _temp3985.base == 0? 1:(
_temp3987 < _temp3985.base? 1: _temp3987 >= _temp3985.last_plus_one)){ _throw(
Null_Exception);}* _temp3987;}).first_line,({ struct _tagged_ptr3 _temp3988=
yyls; struct Cyc_Yyltype* _temp3990= _temp3988.curr +( yylsp_offset - 1); if(
_temp3988.base == 0? 1:( _temp3990 < _temp3988.base? 1: _temp3990 >= _temp3988.last_plus_one)){
_throw( Null_Exception);}* _temp3990;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3991= yyvs; void** _temp3993= _temp3991.curr +(
yyvsp_offset - 1); if( _temp3991.base == 0? 1:( _temp3993 < _temp3991.base? 1:
_temp3993 >= _temp3991.last_plus_one)){ _throw( Null_Exception);}* _temp3993;}))));
goto _LL3995; _LL3995: { struct Cyc_List_List* _temp4005=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3996= yyls; struct Cyc_Yyltype* _temp3998= _temp3996.curr
+( yylsp_offset - 3); if( _temp3996.base == 0? 1:( _temp3998 < _temp3996.base? 1:
_temp3998 >= _temp3996.last_plus_one)){ _throw( Null_Exception);}* _temp3998;}).first_line,({
struct _tagged_ptr3 _temp3999= yyls; struct Cyc_Yyltype* _temp4001= _temp3999.curr
+( yylsp_offset - 3); if( _temp3999.base == 0? 1:( _temp4001 < _temp3999.base? 1:
_temp4001 >= _temp3999.last_plus_one)){ _throw( Null_Exception);}* _temp4001;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4002= yyvs; void** _temp4004=
_temp4002.curr +( yyvsp_offset - 3); if( _temp4002.base == 0? 1:( _temp4004 <
_temp4002.base? 1: _temp4004 >= _temp4002.last_plus_one)){ _throw(
Null_Exception);}* _temp4004;}))); goto _LL4006; _LL4006: yyval=( void*)({
struct Cyc_TunionField_tok_struct* _temp4007=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp4007[ 0]=({ struct
Cyc_TunionField_tok_struct _temp4008; _temp4008.tag= Cyc_TunionField_tok;
_temp4008.f1=({ struct Cyc_Absyn_Tunionfield* _temp4009=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp4009->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp4019= yyvs; void** _temp4021= _temp4019.curr +(
yyvsp_offset - 4); if( _temp4019.base == 0? 1:( _temp4021 < _temp4019.base? 1:
_temp4021 >= _temp4019.last_plus_one)){ _throw( Null_Exception);}* _temp4021;}));
_temp4009->tvs= _temp4005; _temp4009->typs= _temp3994; _temp4009->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4013= yyls; struct Cyc_Yyltype* _temp4015= _temp4013.curr
+( yylsp_offset - 5); if( _temp4013.base == 0? 1:( _temp4015 < _temp4013.base? 1:
_temp4015 >= _temp4013.last_plus_one)){ _throw( Null_Exception);}* _temp4015;}).first_line,({
struct _tagged_ptr3 _temp4016= yyls; struct Cyc_Yyltype* _temp4018= _temp4016.curr
+ yylsp_offset; if( _temp4016.base == 0? 1:( _temp4018 < _temp4016.base? 1:
_temp4018 >= _temp4016.last_plus_one)){ _throw( Null_Exception);}* _temp4018;}).last_line);
_temp4009->sc=( void*) Cyc_yyget_Scope_tok(({ struct _tagged_ptr2 _temp4010=
yyvs; void** _temp4012= _temp4010.curr +( yyvsp_offset - 5); if( _temp4010.base
== 0? 1:( _temp4012 < _temp4010.base? 1: _temp4012 >= _temp4010.last_plus_one)){
_throw( Null_Exception);}* _temp4012;})); _temp4009;}); _temp4008;}); _temp4007;});
break;}} case 123: _LL3984: yyval=({ struct _tagged_ptr2 _temp4023= yyvs; void**
_temp4025= _temp4023.curr + yyvsp_offset; if( _temp4023.base == 0? 1:( _temp4025
< _temp4023.base? 1: _temp4025 >= _temp4023.last_plus_one)){ _throw(
Null_Exception);}* _temp4025;}); break; case 124: _LL4022: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp4027=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4027[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4028; _temp4028.tag= Cyc_Declarator_tok;
_temp4028.f1=({ struct Cyc_Parse_Declarator* _temp4029=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4029->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4036= yyvs; void** _temp4038= _temp4036.curr +
yyvsp_offset; if( _temp4036.base == 0? 1:( _temp4038 < _temp4036.base? 1:
_temp4038 >= _temp4036.last_plus_one)){ _throw( Null_Exception);}* _temp4038;})))->id;
_temp4029->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp4030= yyvs; void** _temp4032= _temp4030.curr +( yyvsp_offset - 1); if(
_temp4030.base == 0? 1:( _temp4032 < _temp4030.base? 1: _temp4032 >= _temp4030.last_plus_one)){
_throw( Null_Exception);}* _temp4032;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp4033= yyvs; void** _temp4035= _temp4033.curr + yyvsp_offset;
if( _temp4033.base == 0? 1:( _temp4035 < _temp4033.base? 1: _temp4035 >=
_temp4033.last_plus_one)){ _throw( Null_Exception);}* _temp4035;})))->tms);
_temp4029;}); _temp4028;}); _temp4027;}); break; case 125: _LL4026: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp4040=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4040[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4041; _temp4041.tag= Cyc_Declarator_tok;
_temp4041.f1=({ struct Cyc_Parse_Declarator* _temp4042=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4042->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp4043= yyvs; void** _temp4045= _temp4043.curr +
yyvsp_offset; if( _temp4043.base == 0? 1:( _temp4045 < _temp4043.base? 1:
_temp4045 >= _temp4043.last_plus_one)){ _throw( Null_Exception);}* _temp4045;}));
_temp4042->tms= 0; _temp4042;}); _temp4041;}); _temp4040;}); break; case 126:
_LL4039: yyval=({ struct _tagged_ptr2 _temp4047= yyvs; void** _temp4049=
_temp4047.curr +( yyvsp_offset - 1); if( _temp4047.base == 0? 1:( _temp4049 <
_temp4047.base? 1: _temp4049 >= _temp4047.last_plus_one)){ _throw(
Null_Exception);}* _temp4049;}); break; case 127: _LL4046: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp4051=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4051[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4052; _temp4052.tag= Cyc_Declarator_tok;
_temp4052.f1=({ struct Cyc_Parse_Declarator* _temp4053=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4053->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4058= yyvs; void** _temp4060= _temp4058.curr +(
yyvsp_offset - 2); if( _temp4058.base == 0? 1:( _temp4060 < _temp4058.base? 1:
_temp4060 >= _temp4058.last_plus_one)){ _throw( Null_Exception);}* _temp4060;})))->id;
_temp4053->tms=({ struct Cyc_List_List* _temp4054=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4054->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp4054->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4055= yyvs;
void** _temp4057= _temp4055.curr +( yyvsp_offset - 2); if( _temp4055.base == 0?
1:( _temp4057 < _temp4055.base? 1: _temp4057 >= _temp4055.last_plus_one)){
_throw( Null_Exception);}* _temp4057;})))->tms; _temp4054;}); _temp4053;});
_temp4052;}); _temp4051;}); break; case 128: _LL4050: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp4062=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4062[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4063; _temp4063.tag= Cyc_Declarator_tok;
_temp4063.f1=({ struct Cyc_Parse_Declarator* _temp4064=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4064->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4074= yyvs; void** _temp4076= _temp4074.curr +(
yyvsp_offset - 3); if( _temp4074.base == 0? 1:( _temp4076 < _temp4074.base? 1:
_temp4076 >= _temp4074.last_plus_one)){ _throw( Null_Exception);}* _temp4076;})))->id;
_temp4064->tms=({ struct Cyc_List_List* _temp4065=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4065->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp4069=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp4069[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp4070; _temp4070.tag= Cyc_Absyn_ConstArray_mod;
_temp4070.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4071= yyvs; void**
_temp4073= _temp4071.curr +( yyvsp_offset - 1); if( _temp4071.base == 0? 1:(
_temp4073 < _temp4071.base? 1: _temp4073 >= _temp4071.last_plus_one)){ _throw(
Null_Exception);}* _temp4073;})); _temp4070;}); _temp4069;})); _temp4065->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4066= yyvs; void**
_temp4068= _temp4066.curr +( yyvsp_offset - 3); if( _temp4066.base == 0? 1:(
_temp4068 < _temp4066.base? 1: _temp4068 >= _temp4066.last_plus_one)){ _throw(
Null_Exception);}* _temp4068;})))->tms; _temp4065;}); _temp4064;}); _temp4063;});
_temp4062;}); break; case 129: _LL4061: { struct _tuple16 _temp4083; struct Cyc_Core_Opt*
_temp4084; int _temp4086; struct Cyc_List_List* _temp4088; struct _tuple16*
_temp4081= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp4078=
yyvs; void** _temp4080= _temp4078.curr +( yyvsp_offset - 1); if( _temp4078.base
== 0? 1:( _temp4080 < _temp4078.base? 1: _temp4080 >= _temp4078.last_plus_one)){
_throw( Null_Exception);}* _temp4080;})); _temp4083=* _temp4081; _LL4089:
_temp4088= _temp4083.f1; goto _LL4087; _LL4087: _temp4086= _temp4083.f2; goto
_LL4085; _LL4085: _temp4084= _temp4083.f3; goto _LL4082; _LL4082: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp4090=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4090[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4091; _temp4091.tag= Cyc_Declarator_tok;
_temp4091.f1=({ struct Cyc_Parse_Declarator* _temp4092=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4092->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4101= yyvs; void** _temp4103= _temp4101.curr +(
yyvsp_offset - 3); if( _temp4101.base == 0? 1:( _temp4103 < _temp4101.base? 1:
_temp4103 >= _temp4101.last_plus_one)){ _throw( Null_Exception);}* _temp4103;})))->id;
_temp4092->tms=({ struct Cyc_List_List* _temp4093=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4093->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4097=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4097[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4098; _temp4098.tag= Cyc_Absyn_Function_mod;
_temp4098.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4099=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4099[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4100; _temp4100.tag= Cyc_Absyn_WithTypes;
_temp4100.f1= _temp4088; _temp4100.f2= _temp4086; _temp4100.f3= _temp4084;
_temp4100;}); _temp4099;})); _temp4098;}); _temp4097;})); _temp4093->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4094= yyvs; void** _temp4096= _temp4094.curr +(
yyvsp_offset - 3); if( _temp4094.base == 0? 1:( _temp4096 < _temp4094.base? 1:
_temp4096 >= _temp4094.last_plus_one)){ _throw( Null_Exception);}* _temp4096;})))->tms;
_temp4093;}); _temp4092;}); _temp4091;}); _temp4090;}); break;} case 130:
_LL4077: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4105=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4105[ 0]=({ struct Cyc_Declarator_tok_struct _temp4106; _temp4106.tag= Cyc_Declarator_tok;
_temp4106.f1=({ struct Cyc_Parse_Declarator* _temp4107=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4107->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4116= yyvs; void** _temp4118= _temp4116.curr +(
yyvsp_offset - 2); if( _temp4116.base == 0? 1:( _temp4118 < _temp4116.base? 1:
_temp4118 >= _temp4116.last_plus_one)){ _throw( Null_Exception);}* _temp4118;})))->id;
_temp4107->tms=({ struct Cyc_List_List* _temp4108=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4108->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4112=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4112[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4113; _temp4113.tag= Cyc_Absyn_Function_mod;
_temp4113.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4114=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4114[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4115; _temp4115.tag= Cyc_Absyn_WithTypes;
_temp4115.f1= 0; _temp4115.f2= 0; _temp4115.f3= 0; _temp4115;}); _temp4114;}));
_temp4113;}); _temp4112;})); _temp4108->tl=( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp4109= yyvs; void** _temp4111= _temp4109.curr +( yyvsp_offset -
2); if( _temp4109.base == 0? 1:( _temp4111 < _temp4109.base? 1: _temp4111 >=
_temp4109.last_plus_one)){ _throw( Null_Exception);}* _temp4111;})))->tms;
_temp4108;}); _temp4107;}); _temp4106;}); _temp4105;}); break; case 131: _LL4104:
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4120=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4120[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4121; _temp4121.tag= Cyc_Declarator_tok;
_temp4121.f1=({ struct Cyc_Parse_Declarator* _temp4122=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4122->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4137= yyvs; void** _temp4139= _temp4137.curr +(
yyvsp_offset - 4); if( _temp4137.base == 0? 1:( _temp4139 < _temp4137.base? 1:
_temp4139 >= _temp4137.last_plus_one)){ _throw( Null_Exception);}* _temp4139;})))->id;
_temp4122->tms=({ struct Cyc_List_List* _temp4123=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4123->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4127=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4127[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4128; _temp4128.tag= Cyc_Absyn_Function_mod;
_temp4128.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp4129=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4129[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp4130; _temp4130.tag= Cyc_Absyn_WithTypes;
_temp4130.f1= 0; _temp4130.f2= 0; _temp4130.f3=({ struct Cyc_Core_Opt* _temp4131=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4131->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4132=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4132[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4133; _temp4133.tag= Cyc_Absyn_JoinEff; _temp4133.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4134= yyvs; void** _temp4136=
_temp4134.curr +( yyvsp_offset - 1); if( _temp4134.base == 0? 1:( _temp4136 <
_temp4134.base? 1: _temp4136 >= _temp4134.last_plus_one)){ _throw(
Null_Exception);}* _temp4136;})); _temp4133;}); _temp4132;})); _temp4131;});
_temp4130;}); _temp4129;})); _temp4128;}); _temp4127;})); _temp4123->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4124= yyvs; void** _temp4126= _temp4124.curr +(
yyvsp_offset - 4); if( _temp4124.base == 0? 1:( _temp4126 < _temp4124.base? 1:
_temp4126 >= _temp4124.last_plus_one)){ _throw( Null_Exception);}* _temp4126;})))->tms;
_temp4123;}); _temp4122;}); _temp4121;}); _temp4120;}); break; case 132: _LL4119:
yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4141=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp4141[ 0]=({ struct
Cyc_Declarator_tok_struct _temp4142; _temp4142.tag= Cyc_Declarator_tok;
_temp4142.f1=({ struct Cyc_Parse_Declarator* _temp4143=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4143->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4161= yyvs; void** _temp4163= _temp4161.curr +(
yyvsp_offset - 3); if( _temp4161.base == 0? 1:( _temp4163 < _temp4161.base? 1:
_temp4163 >= _temp4161.last_plus_one)){ _throw( Null_Exception);}* _temp4163;})))->id;
_temp4143->tms=({ struct Cyc_List_List* _temp4144=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4144->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp4148=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp4148[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp4149; _temp4149.tag= Cyc_Absyn_Function_mod;
_temp4149.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp4150=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp4150[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp4151; _temp4151.tag= Cyc_Absyn_NoTypes;
_temp4151.f1= Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4152= yyvs; void**
_temp4154= _temp4152.curr +( yyvsp_offset - 1); if( _temp4152.base == 0? 1:(
_temp4154 < _temp4152.base? 1: _temp4154 >= _temp4152.last_plus_one)){ _throw(
Null_Exception);}* _temp4154;})); _temp4151.f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4155= yyls; struct Cyc_Yyltype* _temp4157= _temp4155.curr
+( yylsp_offset - 3); if( _temp4155.base == 0? 1:( _temp4157 < _temp4155.base? 1:
_temp4157 >= _temp4155.last_plus_one)){ _throw( Null_Exception);}* _temp4157;}).first_line,({
struct _tagged_ptr3 _temp4158= yyls; struct Cyc_Yyltype* _temp4160= _temp4158.curr
+ yylsp_offset; if( _temp4158.base == 0? 1:( _temp4160 < _temp4158.base? 1:
_temp4160 >= _temp4158.last_plus_one)){ _throw( Null_Exception);}* _temp4160;}).last_line);
_temp4151;}); _temp4150;})); _temp4149;}); _temp4148;})); _temp4144->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4145= yyvs; void** _temp4147= _temp4145.curr +(
yyvsp_offset - 3); if( _temp4145.base == 0? 1:( _temp4147 < _temp4145.base? 1:
_temp4147 >= _temp4145.last_plus_one)){ _throw( Null_Exception);}* _temp4147;})))->tms;
_temp4144;}); _temp4143;}); _temp4142;}); _temp4141;}); break; case 133: _LL4140: {
struct Cyc_List_List* _temp4174=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4165= yyls; struct Cyc_Yyltype* _temp4167= _temp4165.curr
+( yylsp_offset - 2); if( _temp4165.base == 0? 1:( _temp4167 < _temp4165.base? 1:
_temp4167 >= _temp4165.last_plus_one)){ _throw( Null_Exception);}* _temp4167;}).first_line,({
struct _tagged_ptr3 _temp4168= yyls; struct Cyc_Yyltype* _temp4170= _temp4168.curr
+ yylsp_offset; if( _temp4168.base == 0? 1:( _temp4170 < _temp4168.base? 1:
_temp4170 >= _temp4168.last_plus_one)){ _throw( Null_Exception);}* _temp4170;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4171= yyvs; void** _temp4173= _temp4171.curr +(
yyvsp_offset - 1); if( _temp4171.base == 0? 1:( _temp4173 < _temp4171.base? 1:
_temp4173 >= _temp4171.last_plus_one)){ _throw( Null_Exception);}* _temp4173;}))));
goto _LL4175; _LL4175: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp4176=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4176[ 0]=({ struct Cyc_Declarator_tok_struct _temp4177; _temp4177.tag= Cyc_Declarator_tok;
_temp4177.f1=({ struct Cyc_Parse_Declarator* _temp4178=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4178->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4191= yyvs; void** _temp4193= _temp4191.curr +(
yyvsp_offset - 3); if( _temp4191.base == 0? 1:( _temp4193 < _temp4191.base? 1:
_temp4193 >= _temp4191.last_plus_one)){ _throw( Null_Exception);}* _temp4193;})))->id;
_temp4178->tms=({ struct Cyc_List_List* _temp4179=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4179->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4183=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4183[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4184; _temp4184.tag= Cyc_Absyn_TypeParams_mod;
_temp4184.f1= _temp4174; _temp4184.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4185= yyls; struct Cyc_Yyltype* _temp4187= _temp4185.curr +(
yylsp_offset - 3); if( _temp4185.base == 0? 1:( _temp4187 < _temp4185.base? 1:
_temp4187 >= _temp4185.last_plus_one)){ _throw( Null_Exception);}* _temp4187;}).first_line,({
struct _tagged_ptr3 _temp4188= yyls; struct Cyc_Yyltype* _temp4190= _temp4188.curr
+ yylsp_offset; if( _temp4188.base == 0? 1:( _temp4190 < _temp4188.base? 1:
_temp4190 >= _temp4188.last_plus_one)){ _throw( Null_Exception);}* _temp4190;}).last_line);
_temp4184.f3= 0; _temp4184;}); _temp4183;})); _temp4179->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4180= yyvs; void** _temp4182= _temp4180.curr +(
yyvsp_offset - 3); if( _temp4180.base == 0? 1:( _temp4182 < _temp4180.base? 1:
_temp4182 >= _temp4180.last_plus_one)){ _throw( Null_Exception);}* _temp4182;})))->tms;
_temp4179;}); _temp4178;}); _temp4177;}); _temp4176;}); break;} case 134:
_LL4164:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp4195= Cyc_Parse_lbuf;
if( _temp4195 == 0){ _throw( Null_Exception);} _temp4195->v;}))->lex_curr_pos -=
1;{ struct Cyc_List_List* _temp4205=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4196= yyls; struct Cyc_Yyltype* _temp4198= _temp4196.curr
+( yylsp_offset - 2); if( _temp4196.base == 0? 1:( _temp4198 < _temp4196.base? 1:
_temp4198 >= _temp4196.last_plus_one)){ _throw( Null_Exception);}* _temp4198;}).first_line,({
struct _tagged_ptr3 _temp4199= yyls; struct Cyc_Yyltype* _temp4201= _temp4199.curr
+ yylsp_offset; if( _temp4199.base == 0? 1:( _temp4201 < _temp4199.base? 1:
_temp4201 >= _temp4199.last_plus_one)){ _throw( Null_Exception);}* _temp4201;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4202= yyvs; void** _temp4204= _temp4202.curr +(
yyvsp_offset - 1); if( _temp4202.base == 0? 1:( _temp4204 < _temp4202.base? 1:
_temp4204 >= _temp4202.last_plus_one)){ _throw( Null_Exception);}* _temp4204;}))));
goto _LL4206; _LL4206: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp4207=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4207[ 0]=({ struct Cyc_Declarator_tok_struct _temp4208; _temp4208.tag= Cyc_Declarator_tok;
_temp4208.f1=({ struct Cyc_Parse_Declarator* _temp4209=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4209->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4222= yyvs; void** _temp4224= _temp4222.curr +(
yyvsp_offset - 3); if( _temp4222.base == 0? 1:( _temp4224 < _temp4222.base? 1:
_temp4224 >= _temp4222.last_plus_one)){ _throw( Null_Exception);}* _temp4224;})))->id;
_temp4209->tms=({ struct Cyc_List_List* _temp4210=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4210->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp4214=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp4214[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp4215; _temp4215.tag= Cyc_Absyn_TypeParams_mod;
_temp4215.f1= _temp4205; _temp4215.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4216= yyls; struct Cyc_Yyltype* _temp4218= _temp4216.curr +(
yylsp_offset - 3); if( _temp4216.base == 0? 1:( _temp4218 < _temp4216.base? 1:
_temp4218 >= _temp4216.last_plus_one)){ _throw( Null_Exception);}* _temp4218;}).first_line,({
struct _tagged_ptr3 _temp4219= yyls; struct Cyc_Yyltype* _temp4221= _temp4219.curr
+ yylsp_offset; if( _temp4219.base == 0? 1:( _temp4221 < _temp4219.base? 1:
_temp4221 >= _temp4219.last_plus_one)){ _throw( Null_Exception);}* _temp4221;}).last_line);
_temp4215.f3= 0; _temp4215;}); _temp4214;})); _temp4210->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4211= yyvs; void** _temp4213= _temp4211.curr +(
yyvsp_offset - 3); if( _temp4211.base == 0? 1:( _temp4213 < _temp4211.base? 1:
_temp4213 >= _temp4211.last_plus_one)){ _throw( Null_Exception);}* _temp4213;})))->tms;
_temp4210;}); _temp4209;}); _temp4208;}); _temp4207;}); break;} case 135:
_LL4194: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp4226=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp4226[ 0]=({ struct Cyc_Declarator_tok_struct _temp4227; _temp4227.tag= Cyc_Declarator_tok;
_temp4227.f1=({ struct Cyc_Parse_Declarator* _temp4228=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp4228->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4244= yyvs; void** _temp4246= _temp4244.curr +(
yyvsp_offset - 1); if( _temp4244.base == 0? 1:( _temp4246 < _temp4244.base? 1:
_temp4246 >= _temp4244.last_plus_one)){ _throw( Null_Exception);}* _temp4246;})))->id;
_temp4228->tms=({ struct Cyc_List_List* _temp4229=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4229->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp4233=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp4233[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp4234; _temp4234.tag= Cyc_Absyn_Attributes_mod;
_temp4234.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4235= yyls;
struct Cyc_Yyltype* _temp4237= _temp4235.curr + yylsp_offset; if( _temp4235.base
== 0? 1:( _temp4237 < _temp4235.base? 1: _temp4237 >= _temp4235.last_plus_one)){
_throw( Null_Exception);}* _temp4237;}).first_line,({ struct _tagged_ptr3
_temp4238= yyls; struct Cyc_Yyltype* _temp4240= _temp4238.curr + yylsp_offset;
if( _temp4238.base == 0? 1:( _temp4240 < _temp4238.base? 1: _temp4240 >=
_temp4238.last_plus_one)){ _throw( Null_Exception);}* _temp4240;}).last_line);
_temp4234.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4241= yyvs;
void** _temp4243= _temp4241.curr + yyvsp_offset; if( _temp4241.base == 0? 1:(
_temp4243 < _temp4241.base? 1: _temp4243 >= _temp4241.last_plus_one)){ _throw(
Null_Exception);}* _temp4243;})); _temp4234;}); _temp4233;})); _temp4229->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp4230= yyvs; void**
_temp4232= _temp4230.curr +( yyvsp_offset - 1); if( _temp4230.base == 0? 1:(
_temp4232 < _temp4230.base? 1: _temp4232 >= _temp4230.last_plus_one)){ _throw(
Null_Exception);}* _temp4232;})))->tms; _temp4229;}); _temp4228;}); _temp4227;});
_temp4226;}); break; case 136: _LL4225: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4248=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4248[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4249; _temp4249.tag= Cyc_TypeModifierList_tok; _temp4249.f1=({ struct Cyc_List_List*
_temp4250=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4250->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4260=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4260[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4261; _temp4261.tag=
Cyc_Absyn_Pointer_mod; _temp4261.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4262= yyvs; void** _temp4264= _temp4262.curr +(
yyvsp_offset - 2); if( _temp4262.base == 0? 1:( _temp4264 < _temp4262.base? 1:
_temp4264 >= _temp4262.last_plus_one)){ _throw( Null_Exception);}* _temp4264;}));
_temp4261.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4265= yyvs;
void** _temp4267= _temp4265.curr +( yyvsp_offset - 1); if( _temp4265.base == 0?
1:( _temp4267 < _temp4265.base? 1: _temp4267 >= _temp4265.last_plus_one)){
_throw( Null_Exception);}* _temp4267;})); _temp4261.f3= Cyc_Absyn_empty_tqual();
_temp4261;}); _temp4260;})); _temp4250->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4251= yyls; struct Cyc_Yyltype* _temp4253= _temp4251.curr
+ yylsp_offset; if( _temp4251.base == 0? 1:( _temp4253 < _temp4251.base? 1:
_temp4253 >= _temp4251.last_plus_one)){ _throw( Null_Exception);}* _temp4253;}).first_line,({
struct _tagged_ptr3 _temp4254= yyls; struct Cyc_Yyltype* _temp4256= _temp4254.curr
+ yylsp_offset; if( _temp4254.base == 0? 1:( _temp4256 < _temp4254.base? 1:
_temp4256 >= _temp4254.last_plus_one)){ _throw( Null_Exception);}* _temp4256;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4257= yyvs; void**
_temp4259= _temp4257.curr + yyvsp_offset; if( _temp4257.base == 0? 1:( _temp4259
< _temp4257.base? 1: _temp4259 >= _temp4257.last_plus_one)){ _throw(
Null_Exception);}* _temp4259;})), 0); _temp4250;}); _temp4249;}); _temp4248;});
break; case 137: _LL4247: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4269=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4269[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4270; _temp4270.tag= Cyc_TypeModifierList_tok; _temp4270.f1=({ struct Cyc_List_List*
_temp4271=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4271->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4281=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4281[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4282; _temp4282.tag=
Cyc_Absyn_Pointer_mod; _temp4282.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4283= yyvs; void** _temp4285= _temp4283.curr +(
yyvsp_offset - 3); if( _temp4283.base == 0? 1:( _temp4285 < _temp4283.base? 1:
_temp4285 >= _temp4283.last_plus_one)){ _throw( Null_Exception);}* _temp4285;}));
_temp4282.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4286= yyvs;
void** _temp4288= _temp4286.curr +( yyvsp_offset - 2); if( _temp4286.base == 0?
1:( _temp4288 < _temp4286.base? 1: _temp4288 >= _temp4286.last_plus_one)){
_throw( Null_Exception);}* _temp4288;})); _temp4282.f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp4289= yyvs; void** _temp4291= _temp4289.curr +
yyvsp_offset; if( _temp4289.base == 0? 1:( _temp4291 < _temp4289.base? 1:
_temp4291 >= _temp4289.last_plus_one)){ _throw( Null_Exception);}* _temp4291;}));
_temp4282;}); _temp4281;})); _temp4271->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4272= yyls; struct Cyc_Yyltype* _temp4274= _temp4272.curr
+( yylsp_offset - 1); if( _temp4272.base == 0? 1:( _temp4274 < _temp4272.base? 1:
_temp4274 >= _temp4272.last_plus_one)){ _throw( Null_Exception);}* _temp4274;}).first_line,({
struct _tagged_ptr3 _temp4275= yyls; struct Cyc_Yyltype* _temp4277= _temp4275.curr
+( yylsp_offset - 1); if( _temp4275.base == 0? 1:( _temp4277 < _temp4275.base? 1:
_temp4277 >= _temp4275.last_plus_one)){ _throw( Null_Exception);}* _temp4277;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4278= yyvs; void**
_temp4280= _temp4278.curr +( yyvsp_offset - 1); if( _temp4278.base == 0? 1:(
_temp4280 < _temp4278.base? 1: _temp4280 >= _temp4278.last_plus_one)){ _throw(
Null_Exception);}* _temp4280;})), 0); _temp4271;}); _temp4270;}); _temp4269;});
break; case 138: _LL4268: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp4293=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp4293[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp4294; _temp4294.tag= Cyc_TypeModifierList_tok; _temp4294.f1=({ struct Cyc_List_List*
_temp4295=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4295->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp4308=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp4308[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp4309; _temp4309.tag=
Cyc_Absyn_Pointer_mod; _temp4309.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp4310= yyvs; void** _temp4312= _temp4310.curr +(
yyvsp_offset - 3); if( _temp4310.base == 0? 1:( _temp4312 < _temp4310.base? 1:
_temp4312 >= _temp4310.last_plus_one)){ _throw( Null_Exception);}* _temp4312;}));
_temp4309.f2=( void*) Cyc_yyget_Rgn_tok(({ struct _tagged_ptr2 _temp4313= yyvs;
void** _temp4315= _temp4313.curr +( yyvsp_offset - 2); if( _temp4313.base == 0?
1:( _temp4315 < _temp4313.base? 1: _temp4315 >= _temp4313.last_plus_one)){
_throw( Null_Exception);}* _temp4315;})); _temp4309.f3= Cyc_Absyn_empty_tqual();
_temp4309;}); _temp4308;})); _temp4295->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4296= yyls; struct Cyc_Yyltype* _temp4298= _temp4296.curr
+( yylsp_offset - 1); if( _temp4296.base == 0? 1:( _temp4298 < _temp4296.base? 1:
_temp4298 >= _temp4296.last_plus_one)){ _throw( Null_Exception);}* _temp4298;}).first_line,({
struct _tagged_ptr3 _temp4299= yyls; struct Cyc_Yyltype* _temp4301= _temp4299.curr
+( yylsp_offset - 1); if( _temp4299.base == 0? 1:( _temp4301 < _temp4299.base? 1:
_temp4301 >= _temp4299.last_plus_one)){ _throw( Null_Exception);}* _temp4301;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4302= yyvs; void**
_temp4304= _temp4302.curr +( yyvsp_offset - 1); if( _temp4302.base == 0? 1:(
_temp4304 < _temp4302.base? 1: _temp4304 >= _temp4302.last_plus_one)){ _throw(
Null_Exception);}* _temp4304;})), Cyc_yyget_TypeModifierList_tok(({ struct
_tagged_ptr2 _temp4305= yyvs; void** _temp4307= _temp4305.curr + yyvsp_offset;
if( _temp4305.base == 0? 1:( _temp4307 < _temp4305.base? 1: _temp4307 >=
_temp4305.last_plus_one)){ _throw( Null_Exception);}* _temp4307;}))); _temp4295;});
_temp4294;}); _temp4293;}); break; case 139: _LL4292: yyval=( void*)({ struct
Cyc_TypeModifierList_tok_struct* _temp4317=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp4317[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp4318; _temp4318.tag= Cyc_TypeModifierList_tok;
_temp4318.f1=({ struct Cyc_List_List* _temp4319=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4319->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp4332=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp4332[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp4333; _temp4333.tag= Cyc_Absyn_Pointer_mod;
_temp4333.f1=( void*) Cyc_yyget_Pointer_Sort_tok(({ struct _tagged_ptr2
_temp4334= yyvs; void** _temp4336= _temp4334.curr +( yyvsp_offset - 4); if(
_temp4334.base == 0? 1:( _temp4336 < _temp4334.base? 1: _temp4336 >= _temp4334.last_plus_one)){
_throw( Null_Exception);}* _temp4336;})); _temp4333.f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp4337= yyvs; void** _temp4339= _temp4337.curr +(
yyvsp_offset - 3); if( _temp4337.base == 0? 1:( _temp4339 < _temp4337.base? 1:
_temp4339 >= _temp4337.last_plus_one)){ _throw( Null_Exception);}* _temp4339;}));
_temp4333.f3= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp4340= yyvs;
void** _temp4342= _temp4340.curr +( yyvsp_offset - 1); if( _temp4340.base == 0?
1:( _temp4342 < _temp4340.base? 1: _temp4342 >= _temp4340.last_plus_one)){
_throw( Null_Exception);}* _temp4342;})); _temp4333;}); _temp4332;})); _temp4319->tl=
Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4320= yyls; struct Cyc_Yyltype* _temp4322= _temp4320.curr +( yylsp_offset -
2); if( _temp4320.base == 0? 1:( _temp4322 < _temp4320.base? 1: _temp4322 >=
_temp4320.last_plus_one)){ _throw( Null_Exception);}* _temp4322;}).first_line,({
struct _tagged_ptr3 _temp4323= yyls; struct Cyc_Yyltype* _temp4325= _temp4323.curr
+( yylsp_offset - 2); if( _temp4323.base == 0? 1:( _temp4325 < _temp4323.base? 1:
_temp4325 >= _temp4323.last_plus_one)){ _throw( Null_Exception);}* _temp4325;}).last_line),
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp4326= yyvs; void**
_temp4328= _temp4326.curr +( yyvsp_offset - 2); if( _temp4326.base == 0? 1:(
_temp4328 < _temp4326.base? 1: _temp4328 >= _temp4326.last_plus_one)){ _throw(
Null_Exception);}* _temp4328;})), Cyc_yyget_TypeModifierList_tok(({ struct
_tagged_ptr2 _temp4329= yyvs; void** _temp4331= _temp4329.curr + yyvsp_offset;
if( _temp4329.base == 0? 1:( _temp4331 < _temp4329.base? 1: _temp4331 >=
_temp4329.last_plus_one)){ _throw( Null_Exception);}* _temp4331;}))); _temp4319;});
_temp4318;}); _temp4317;}); break; case 140: _LL4316: yyval=( void*)({ struct
Cyc_Pointer_Sort_tok_struct* _temp4344=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp4344[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp4345; _temp4345.tag= Cyc_Pointer_Sort_tok;
_temp4345.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp4346=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4346[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4347; _temp4347.tag=
Cyc_Absyn_Nullable_ps; _temp4347.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4348= yyls; struct Cyc_Yyltype* _temp4350= _temp4348.curr
+ yylsp_offset; if( _temp4348.base == 0? 1:( _temp4350 < _temp4348.base? 1:
_temp4350 >= _temp4348.last_plus_one)){ _throw( Null_Exception);}* _temp4350;}).first_line,({
struct _tagged_ptr3 _temp4351= yyls; struct Cyc_Yyltype* _temp4353= _temp4351.curr
+ yylsp_offset; if( _temp4351.base == 0? 1:( _temp4353 < _temp4351.base? 1:
_temp4353 >= _temp4351.last_plus_one)){ _throw( Null_Exception);}* _temp4353;}).last_line));
_temp4347;}); _temp4346;})); _temp4345;}); _temp4344;}); break; case 141:
_LL4343: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4355=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4355[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4356; _temp4356.tag=
Cyc_Pointer_Sort_tok; _temp4356.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4357=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4357[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4358; _temp4358.tag= Cyc_Absyn_NonNullable_ps; _temp4358.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4359= yyls; struct
Cyc_Yyltype* _temp4361= _temp4359.curr + yylsp_offset; if( _temp4359.base == 0?
1:( _temp4361 < _temp4359.base? 1: _temp4361 >= _temp4359.last_plus_one)){
_throw( Null_Exception);}* _temp4361;}).first_line,({ struct _tagged_ptr3
_temp4362= yyls; struct Cyc_Yyltype* _temp4364= _temp4362.curr + yylsp_offset;
if( _temp4362.base == 0? 1:( _temp4364 < _temp4362.base? 1: _temp4364 >=
_temp4362.last_plus_one)){ _throw( Null_Exception);}* _temp4364;}).last_line));
_temp4358;}); _temp4357;})); _temp4356;}); _temp4355;}); break; case 142:
_LL4354: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4366=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4366[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4367; _temp4367.tag=
Cyc_Pointer_Sort_tok; _temp4367.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp4368=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp4368[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp4369; _temp4369.tag=
Cyc_Absyn_Nullable_ps; _temp4369.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4370= yyvs; void** _temp4372= _temp4370.curr +( yyvsp_offset - 1); if(
_temp4370.base == 0? 1:( _temp4372 < _temp4370.base? 1: _temp4372 >= _temp4370.last_plus_one)){
_throw( Null_Exception);}* _temp4372;})); _temp4369;}); _temp4368;})); _temp4367;});
_temp4366;}); break; case 143: _LL4365: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp4374=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4374[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4375; _temp4375.tag=
Cyc_Pointer_Sort_tok; _temp4375.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp4376=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp4376[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp4377; _temp4377.tag= Cyc_Absyn_NonNullable_ps; _temp4377.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4378= yyvs; void** _temp4380= _temp4378.curr +(
yyvsp_offset - 1); if( _temp4378.base == 0? 1:( _temp4380 < _temp4378.base? 1:
_temp4380 >= _temp4378.last_plus_one)){ _throw( Null_Exception);}* _temp4380;}));
_temp4377;}); _temp4376;})); _temp4375;}); _temp4374;}); break; case 144:
_LL4373: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp4382=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp4382[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp4383; _temp4383.tag=
Cyc_Pointer_Sort_tok; _temp4383.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps);
_temp4383;}); _temp4382;}); break; case 145: _LL4381: yyval=( void*)({ struct
Cyc_Rgn_tok_struct* _temp4385=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp4385[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4386; _temp4386.tag= Cyc_Rgn_tok; _temp4386.f1=( void*)(( void*) Cyc_Absyn_HeapRgn);
_temp4386;}); _temp4385;}); break; case 146: _LL4384: yyval=({ struct
_tagged_ptr2 _temp4388= yyvs; void** _temp4390= _temp4388.curr + yyvsp_offset;
if( _temp4388.base == 0? 1:( _temp4390 < _temp4388.base? 1: _temp4390 >=
_temp4388.last_plus_one)){ _throw( Null_Exception);}* _temp4390;}); break; case
147: _LL4387: yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp4392=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4392[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4393; _temp4393.tag= Cyc_Rgn_tok; _temp4393.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4394= yyvs; void** _temp4396=
_temp4394.curr + yyvsp_offset; if( _temp4394.base == 0? 1:( _temp4396 <
_temp4394.base? 1: _temp4396 >= _temp4394.last_plus_one)){ _throw(
Null_Exception);}* _temp4396;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp4393;}); _temp4392;}); break; case 148: _LL4391:
if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp4398= yyvs; void** _temp4400=
_temp4398.curr + yyvsp_offset; if( _temp4398.base == 0? 1:( _temp4400 <
_temp4398.base? 1: _temp4400 >= _temp4398.last_plus_one)){ _throw(
Null_Exception);}* _temp4400;})) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp4401=( char*)"expecting region kind\n";
struct _tagged_string _temp4402; _temp4402.curr= _temp4401; _temp4402.base=
_temp4401; _temp4402.last_plus_one= _temp4401 + 23; _temp4402;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4403= yyls; struct Cyc_Yyltype* _temp4405= _temp4403.curr
+ yylsp_offset; if( _temp4403.base == 0? 1:( _temp4405 < _temp4403.base? 1:
_temp4405 >= _temp4403.last_plus_one)){ _throw( Null_Exception);}* _temp4405;}).first_line,({
struct _tagged_ptr3 _temp4406= yyls; struct Cyc_Yyltype* _temp4408= _temp4406.curr
+ yylsp_offset; if( _temp4406.base == 0? 1:( _temp4408 < _temp4406.base? 1:
_temp4408 >= _temp4406.last_plus_one)){ _throw( Null_Exception);}* _temp4408;}).last_line));}
yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp4409=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4409[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4410; _temp4410.tag= Cyc_Rgn_tok; _temp4410.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4411= yyvs; void** _temp4413=
_temp4411.curr +( yyvsp_offset - 2); if( _temp4411.base == 0? 1:( _temp4413 <
_temp4411.base? 1: _temp4413 >= _temp4411.last_plus_one)){ _throw(
Null_Exception);}* _temp4413;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind)); _temp4410;}); _temp4409;}); break; case 149: _LL4397:
yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp4415=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp4415[ 0]=({ struct Cyc_Rgn_tok_struct
_temp4416; _temp4416.tag= Cyc_Rgn_tok; _temp4416.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp4416;}); _temp4415;}); break; case 150: _LL4414:
yyval=({ struct _tagged_ptr2 _temp4418= yyvs; void** _temp4420= _temp4418.curr +
yyvsp_offset; if( _temp4418.base == 0? 1:( _temp4420 < _temp4418.base? 1:
_temp4420 >= _temp4418.last_plus_one)){ _throw( Null_Exception);}* _temp4420;});
break; case 151: _LL4417: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp4422=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp4422[ 0]=({ struct Cyc_TypeQual_tok_struct _temp4423; _temp4423.tag= Cyc_TypeQual_tok;
_temp4423.f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp4424= yyvs; void** _temp4426= _temp4424.curr +( yyvsp_offset -
1); if( _temp4424.base == 0? 1:( _temp4426 < _temp4424.base? 1: _temp4426 >=
_temp4424.last_plus_one)){ _throw( Null_Exception);}* _temp4426;})), Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp4427= yyvs; void** _temp4429= _temp4427.curr +
yyvsp_offset; if( _temp4427.base == 0? 1:( _temp4429 < _temp4427.base? 1:
_temp4429 >= _temp4427.last_plus_one)){ _throw( Null_Exception);}* _temp4429;})));
_temp4423;}); _temp4422;}); break; case 152: _LL4421: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp4431=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp4431[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp4432; _temp4432.tag= Cyc_ParamDeclListBool_tok;
_temp4432.f1=({ struct _tuple16* _temp4433=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp4433->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4434= yyvs; void** _temp4436= _temp4434.curr + yyvsp_offset; if( _temp4434.base
== 0? 1:( _temp4436 < _temp4434.base? 1: _temp4436 >= _temp4434.last_plus_one)){
_throw( Null_Exception);}* _temp4436;}))); _temp4433->f2= 0; _temp4433->f3= 0;
_temp4433;}); _temp4432;}); _temp4431;}); break; case 153: _LL4430: yyval=( void*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp4438=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp4438[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp4439; _temp4439.tag= Cyc_ParamDeclListBool_tok;
_temp4439.f1=({ struct _tuple16* _temp4440=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp4440->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4441= yyvs; void** _temp4443= _temp4441.curr +( yyvsp_offset - 2); if(
_temp4441.base == 0? 1:( _temp4443 < _temp4441.base? 1: _temp4443 >= _temp4441.last_plus_one)){
_throw( Null_Exception);}* _temp4443;}))); _temp4440->f2= 1; _temp4440->f3= 0;
_temp4440;}); _temp4439;}); _temp4438;}); break; case 154: _LL4437: yyval=( void*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp4445=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp4445[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp4446; _temp4446.tag= Cyc_ParamDeclListBool_tok;
_temp4446.f1=({ struct _tuple16* _temp4447=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp4447->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp4454= yyvs; void** _temp4456= _temp4454.curr +( yyvsp_offset - 2); if(
_temp4454.base == 0? 1:( _temp4456 < _temp4454.base? 1: _temp4456 >= _temp4454.last_plus_one)){
_throw( Null_Exception);}* _temp4456;}))); _temp4447->f2= 0; _temp4447->f3=(
struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp4448=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4448->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp4449=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4449[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp4450; _temp4450.tag= Cyc_Absyn_JoinEff; _temp4450.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4451= yyvs; void** _temp4453=
_temp4451.curr + yyvsp_offset; if( _temp4451.base == 0? 1:( _temp4453 <
_temp4451.base? 1: _temp4453 >= _temp4451.last_plus_one)){ _throw(
Null_Exception);}* _temp4453;})); _temp4450;}); _temp4449;})); _temp4448;});
_temp4447;}); _temp4446;}); _temp4445;}); break; case 155: _LL4444: yyval=({
struct _tagged_ptr2 _temp4458= yyvs; void** _temp4460= _temp4458.curr +
yyvsp_offset; if( _temp4458.base == 0? 1:( _temp4460 < _temp4458.base? 1:
_temp4460 >= _temp4458.last_plus_one)){ _throw( Null_Exception);}* _temp4460;});
break; case 156: _LL4457: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4462=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4462[ 0]=({ struct Cyc_TypeList_tok_struct _temp4463; _temp4463.tag= Cyc_TypeList_tok;
_temp4463.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2
_temp4464= yyvs; void** _temp4466= _temp4464.curr +( yyvsp_offset - 2); if(
_temp4464.base == 0? 1:( _temp4466 < _temp4464.base? 1: _temp4466 >= _temp4464.last_plus_one)){
_throw( Null_Exception);}* _temp4466;})), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4467= yyvs; void** _temp4469= _temp4467.curr + yyvsp_offset;
if( _temp4467.base == 0? 1:( _temp4469 < _temp4467.base? 1: _temp4469 >=
_temp4467.last_plus_one)){ _throw( Null_Exception);}* _temp4469;}))); _temp4463;});
_temp4462;}); break; case 157: _LL4461: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4471=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4471[ 0]=({ struct Cyc_TypeList_tok_struct _temp4472; _temp4472.tag= Cyc_TypeList_tok;
_temp4472.f1= 0; _temp4472;}); _temp4471;}); break; case 158: _LL4470: yyval=({
struct _tagged_ptr2 _temp4474= yyvs; void** _temp4476= _temp4474.curr +(
yyvsp_offset - 1); if( _temp4474.base == 0? 1:( _temp4476 < _temp4474.base? 1:
_temp4476 >= _temp4474.last_plus_one)){ _throw( Null_Exception);}* _temp4476;});
break; case 159: _LL4473: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4478=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4478[ 0]=({ struct Cyc_TypeList_tok_struct _temp4479; _temp4479.tag= Cyc_TypeList_tok;
_temp4479.f1=({ struct Cyc_List_List* _temp4480=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4480->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4481= yyvs; void** _temp4483=
_temp4481.curr + yyvsp_offset; if( _temp4481.base == 0? 1:( _temp4483 <
_temp4481.base? 1: _temp4483 >= _temp4481.last_plus_one)){ _throw(
Null_Exception);}* _temp4483;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_EffKind)); _temp4480->tl= 0; _temp4480;}); _temp4479;});
_temp4478;}); break; case 160: _LL4477: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4485= yyvs; void** _temp4487= _temp4485.curr + yyvsp_offset;
if( _temp4485.base == 0? 1:( _temp4487 < _temp4485.base? 1: _temp4487 >=
_temp4485.last_plus_one)){ _throw( Null_Exception);}* _temp4487;})) !=( void*)
Cyc_Absyn_EffKind){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp4488=(
char*)"expecing effect kind (E)"; struct _tagged_string _temp4489; _temp4489.curr=
_temp4488; _temp4489.base= _temp4488; _temp4489.last_plus_one= _temp4488 + 25;
_temp4489;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4490= yyls;
struct Cyc_Yyltype* _temp4492= _temp4490.curr + yylsp_offset; if( _temp4490.base
== 0? 1:( _temp4492 < _temp4490.base? 1: _temp4492 >= _temp4490.last_plus_one)){
_throw( Null_Exception);}* _temp4492;}).first_line,({ struct _tagged_ptr3
_temp4493= yyls; struct Cyc_Yyltype* _temp4495= _temp4493.curr + yylsp_offset;
if( _temp4493.base == 0? 1:( _temp4495 < _temp4493.base? 1: _temp4495 >=
_temp4493.last_plus_one)){ _throw( Null_Exception);}* _temp4495;}).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4496=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4496[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4497; _temp4497.tag= Cyc_TypeList_tok; _temp4497.f1=({ struct Cyc_List_List*
_temp4498=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4498->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4499= yyvs; void** _temp4501= _temp4499.curr +( yyvsp_offset -
2); if( _temp4499.base == 0? 1:( _temp4501 < _temp4499.base? 1: _temp4501 >=
_temp4499.last_plus_one)){ _throw( Null_Exception);}* _temp4501;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp4498->tl= 0; _temp4498;}); _temp4497;}); _temp4496;}); break; case 161:
_LL4484: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4503=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4503[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4504; _temp4504.tag= Cyc_TypeList_tok; _temp4504.f1=({ struct Cyc_List_List*
_temp4505=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4505->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4506=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4506[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4507; _temp4507.tag= Cyc_Absyn_AccessEff;
_temp4507.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4508= yyvs; void** _temp4510= _temp4508.curr + yyvsp_offset;
if( _temp4508.base == 0? 1:( _temp4510 < _temp4508.base? 1: _temp4510 >=
_temp4508.last_plus_one)){ _throw( Null_Exception);}* _temp4510;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4507;}); _temp4506;})); _temp4505->tl= 0; _temp4505;}); _temp4504;});
_temp4503;}); break; case 162: _LL4502: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp4512=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp4512[ 0]=({ struct Cyc_TypeList_tok_struct _temp4513; _temp4513.tag= Cyc_TypeList_tok;
_temp4513.f1=({ struct Cyc_List_List* _temp4514=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4514->hd=( void*)(( void*)({
struct Cyc_Absyn_AccessEff_struct* _temp4518=( struct Cyc_Absyn_AccessEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct)); _temp4518[ 0]=({ struct
Cyc_Absyn_AccessEff_struct _temp4519; _temp4519.tag= Cyc_Absyn_AccessEff;
_temp4519.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4520= yyvs; void** _temp4522= _temp4520.curr +( yyvsp_offset -
2); if( _temp4520.base == 0? 1:( _temp4522 < _temp4520.base? 1: _temp4522 >=
_temp4520.last_plus_one)){ _throw( Null_Exception);}* _temp4522;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4519;}); _temp4518;})); _temp4514->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4515= yyvs; void** _temp4517= _temp4515.curr + yyvsp_offset;
if( _temp4515.base == 0? 1:( _temp4517 < _temp4515.base? 1: _temp4517 >=
_temp4515.last_plus_one)){ _throw( Null_Exception);}* _temp4517;})); _temp4514;});
_temp4513;}); _temp4512;}); break; case 163: _LL4511: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp4524= yyvs; void** _temp4526= _temp4524.curr +
yyvsp_offset; if( _temp4524.base == 0? 1:( _temp4526 < _temp4524.base? 1:
_temp4526 >= _temp4524.last_plus_one)){ _throw( Null_Exception);}* _temp4526;}))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp4527=( char*)"expecting region kind (R)"; struct _tagged_string _temp4528;
_temp4528.curr= _temp4527; _temp4528.base= _temp4527; _temp4528.last_plus_one=
_temp4527 + 26; _temp4528;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4529= yyls; struct Cyc_Yyltype* _temp4531= _temp4529.curr + yylsp_offset;
if( _temp4529.base == 0? 1:( _temp4531 < _temp4529.base? 1: _temp4531 >=
_temp4529.last_plus_one)){ _throw( Null_Exception);}* _temp4531;}).first_line,({
struct _tagged_ptr3 _temp4532= yyls; struct Cyc_Yyltype* _temp4534= _temp4532.curr
+ yylsp_offset; if( _temp4532.base == 0? 1:( _temp4534 < _temp4532.base? 1:
_temp4534 >= _temp4532.last_plus_one)){ _throw( Null_Exception);}* _temp4534;}).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4535=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4535[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4536; _temp4536.tag= Cyc_TypeList_tok; _temp4536.f1=({ struct Cyc_List_List*
_temp4537=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4537->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4538=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4538[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4539; _temp4539.tag= Cyc_Absyn_AccessEff;
_temp4539.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4540= yyvs; void** _temp4542= _temp4540.curr +( yyvsp_offset -
2); if( _temp4540.base == 0? 1:( _temp4542 < _temp4540.base? 1: _temp4542 >=
_temp4540.last_plus_one)){ _throw( Null_Exception);}* _temp4542;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4539;}); _temp4538;})); _temp4537->tl= 0; _temp4537;}); _temp4536;});
_temp4535;}); break; case 164: _LL4523: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp4544= yyvs; void** _temp4546= _temp4544.curr +( yyvsp_offset -
2); if( _temp4544.base == 0? 1:( _temp4546 < _temp4544.base? 1: _temp4546 >=
_temp4544.last_plus_one)){ _throw( Null_Exception);}* _temp4546;})) !=( void*)
Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp4547=(
char*)"expecting region kind (R)"; struct _tagged_string _temp4548; _temp4548.curr=
_temp4547; _temp4548.base= _temp4547; _temp4548.last_plus_one= _temp4547 + 26;
_temp4548;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4549= yyls;
struct Cyc_Yyltype* _temp4551= _temp4549.curr +( yylsp_offset - 2); if(
_temp4549.base == 0? 1:( _temp4551 < _temp4549.base? 1: _temp4551 >= _temp4549.last_plus_one)){
_throw( Null_Exception);}* _temp4551;}).first_line,({ struct _tagged_ptr3
_temp4552= yyls; struct Cyc_Yyltype* _temp4554= _temp4552.curr +( yylsp_offset -
2); if( _temp4552.base == 0? 1:( _temp4554 < _temp4552.base? 1: _temp4554 >=
_temp4552.last_plus_one)){ _throw( Null_Exception);}* _temp4554;}).last_line));}
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp4555=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp4555[ 0]=({ struct Cyc_TypeList_tok_struct
_temp4556; _temp4556.tag= Cyc_TypeList_tok; _temp4556.f1=({ struct Cyc_List_List*
_temp4557=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4557->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp4561=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp4561[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp4562; _temp4562.tag= Cyc_Absyn_AccessEff;
_temp4562.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp4563= yyvs; void** _temp4565= _temp4563.curr +( yyvsp_offset -
4); if( _temp4563.base == 0? 1:( _temp4565 < _temp4563.base? 1: _temp4565 >=
_temp4563.last_plus_one)){ _throw( Null_Exception);}* _temp4565;})),(( struct
Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp4562;}); _temp4561;})); _temp4557->tl= Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4558= yyvs; void** _temp4560= _temp4558.curr + yyvsp_offset;
if( _temp4558.base == 0? 1:( _temp4560 < _temp4558.base? 1: _temp4560 >=
_temp4558.last_plus_one)){ _throw( Null_Exception);}* _temp4560;})); _temp4557;});
_temp4556;}); _temp4555;}); break; case 165: _LL4543: yyval=( void*)({ struct
Cyc_ParamDeclList_tok_struct* _temp4567=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct)); _temp4567[ 0]=({
struct Cyc_ParamDeclList_tok_struct _temp4568; _temp4568.tag= Cyc_ParamDeclList_tok;
_temp4568.f1=({ struct Cyc_List_List* _temp4569=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4569->hd=( void*) Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp4570= yyvs; void** _temp4572= _temp4570.curr +
yyvsp_offset; if( _temp4570.base == 0? 1:( _temp4572 < _temp4570.base? 1:
_temp4572 >= _temp4570.last_plus_one)){ _throw( Null_Exception);}* _temp4572;}));
_temp4569->tl= 0; _temp4569;}); _temp4568;}); _temp4567;}); break; case 166:
_LL4566: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp4574=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp4574[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp4575; _temp4575.tag=
Cyc_ParamDeclList_tok; _temp4575.f1=({ struct Cyc_List_List* _temp4576=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4576->hd=( void*)
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4580= yyvs; void** _temp4582=
_temp4580.curr + yyvsp_offset; if( _temp4580.base == 0? 1:( _temp4582 <
_temp4580.base? 1: _temp4582 >= _temp4580.last_plus_one)){ _throw(
Null_Exception);}* _temp4582;})); _temp4576->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp4577= yyvs; void** _temp4579= _temp4577.curr +(
yyvsp_offset - 2); if( _temp4577.base == 0? 1:( _temp4579 < _temp4577.base? 1:
_temp4579 >= _temp4577.last_plus_one)){ _throw( Null_Exception);}* _temp4579;}));
_temp4576;}); _temp4575;}); _temp4574;}); break; case 167: _LL4573: { void*
_temp4593= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp4584= yyvs; void** _temp4586= _temp4584.curr +( yyvsp_offset -
1); if( _temp4584.base == 0? 1:( _temp4586 < _temp4584.base? 1: _temp4586 >=
_temp4584.last_plus_one)){ _throw( Null_Exception);}* _temp4586;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4587= yyls; struct Cyc_Yyltype* _temp4589= _temp4587.curr
+( yylsp_offset - 1); if( _temp4587.base == 0? 1:( _temp4589 < _temp4587.base? 1:
_temp4589 >= _temp4587.last_plus_one)){ _throw( Null_Exception);}* _temp4589;}).first_line,({
struct _tagged_ptr3 _temp4590= yyls; struct Cyc_Yyltype* _temp4592= _temp4590.curr
+( yylsp_offset - 1); if( _temp4590.base == 0? 1:( _temp4592 < _temp4590.base? 1:
_temp4592 >= _temp4590.last_plus_one)){ _throw( Null_Exception);}* _temp4592;}).last_line));
goto _LL4594; _LL4594: { struct Cyc_List_List* _temp4598=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4595= yyvs; void** _temp4597= _temp4595.curr +(
yyvsp_offset - 1); if( _temp4595.base == 0? 1:( _temp4597 < _temp4595.base? 1:
_temp4597 >= _temp4595.last_plus_one)){ _throw( Null_Exception);}* _temp4597;}))).f3;
goto _LL4599; _LL4599: { struct Cyc_Absyn_Tqual _temp4603=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4600= yyvs; void** _temp4602= _temp4600.curr +(
yyvsp_offset - 1); if( _temp4600.base == 0? 1:( _temp4602 < _temp4600.base? 1:
_temp4602 >= _temp4600.last_plus_one)){ _throw( Null_Exception);}* _temp4602;}))).f1;
goto _LL4604; _LL4604: { struct Cyc_List_List* _temp4608=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp4605= yyvs; void** _temp4607= _temp4605.curr +
yyvsp_offset; if( _temp4605.base == 0? 1:( _temp4607 < _temp4605.base? 1:
_temp4607 >= _temp4605.last_plus_one)){ _throw( Null_Exception);}* _temp4607;})))->tms;
goto _LL4609; _LL4609: { struct _tuple6 _temp4610= Cyc_Parse_apply_tms(
_temp4603, _temp4593, _temp4598, _temp4608); goto _LL4611; _LL4611: if(
_temp4610.f3 != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp4612=(
char*)"parameter with bad type params"; struct _tagged_string _temp4613;
_temp4613.curr= _temp4612; _temp4613.base= _temp4612; _temp4613.last_plus_one=
_temp4612 + 31; _temp4613;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4614= yyls; struct Cyc_Yyltype* _temp4616= _temp4614.curr + yylsp_offset;
if( _temp4614.base == 0? 1:( _temp4616 < _temp4614.base? 1: _temp4616 >=
_temp4614.last_plus_one)){ _throw( Null_Exception);}* _temp4616;}).first_line,({
struct _tagged_ptr3 _temp4617= yyls; struct Cyc_Yyltype* _temp4619= _temp4617.curr
+ yylsp_offset; if( _temp4617.base == 0? 1:( _temp4619 < _temp4617.base? 1:
_temp4619 >= _temp4617.last_plus_one)){ _throw( Null_Exception);}* _temp4619;}).last_line));}{
struct _tuple1* _temp4623=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp4620= yyvs; void** _temp4622= _temp4620.curr + yyvsp_offset; if( _temp4620.base
== 0? 1:( _temp4622 < _temp4620.base? 1: _temp4622 >= _temp4620.last_plus_one)){
_throw( Null_Exception);}* _temp4622;})))->id; goto _LL4624; _LL4624:{ void*
_temp4625=(* _temp4623).f1; struct Cyc_List_List* _temp4635; struct Cyc_List_List*
_temp4637; _LL4627: if( _temp4625 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4628;}
else{ goto _LL4629;} _LL4629: if(( unsigned int) _temp4625 > 1u?*(( int*)
_temp4625) == Cyc_Absyn_Rel_n: 0){ _LL4636: _temp4635=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp4625)->f1; if( _temp4635 == 0){ goto
_LL4630;} else{ goto _LL4631;}} else{ goto _LL4631;} _LL4631: if(( unsigned int)
_temp4625 > 1u?*(( int*) _temp4625) == Cyc_Absyn_Abs_n: 0){ _LL4638: _temp4637=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp4625)->f1; if(
_temp4637 == 0){ goto _LL4632;} else{ goto _LL4633;}} else{ goto _LL4633;}
_LL4633: goto _LL4634; _LL4628: goto _LL4626; _LL4630: goto _LL4626; _LL4632:
goto _LL4626; _LL4634: Cyc_Parse_err(( struct _tagged_string)({ char* _temp4639=(
char*)"parameter cannot be qualified with a module name"; struct _tagged_string
_temp4640; _temp4640.curr= _temp4639; _temp4640.base= _temp4639; _temp4640.last_plus_one=
_temp4639 + 49; _temp4640;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4641= yyls; struct Cyc_Yyltype* _temp4643= _temp4641.curr +( yylsp_offset -
1); if( _temp4641.base == 0? 1:( _temp4643 < _temp4641.base? 1: _temp4643 >=
_temp4641.last_plus_one)){ _throw( Null_Exception);}* _temp4643;}).first_line,({
struct _tagged_ptr3 _temp4644= yyls; struct Cyc_Yyltype* _temp4646= _temp4644.curr
+( yylsp_offset - 1); if( _temp4644.base == 0? 1:( _temp4646 < _temp4644.base? 1:
_temp4646 >= _temp4644.last_plus_one)){ _throw( Null_Exception);}* _temp4646;}).last_line));
goto _LL4626; _LL4626:;}{ struct Cyc_Core_Opt* _temp4648=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp4647=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp4647->v=( void*)(* _temp4623).f2; _temp4647;}); goto
_LL4649; _LL4649: if( _temp4610.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4650=( char*)"extra attributes on parameter, ignoring"; struct
_tagged_string _temp4651; _temp4651.curr= _temp4650; _temp4651.base= _temp4650;
_temp4651.last_plus_one= _temp4650 + 40; _temp4651;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4652= yyls; struct Cyc_Yyltype* _temp4654= _temp4652.curr
+( yylsp_offset - 1); if( _temp4652.base == 0? 1:( _temp4654 < _temp4652.base? 1:
_temp4654 >= _temp4652.last_plus_one)){ _throw( Null_Exception);}* _temp4654;}).first_line,({
struct _tagged_ptr3 _temp4655= yyls; struct Cyc_Yyltype* _temp4657= _temp4655.curr
+ yylsp_offset; if( _temp4655.base == 0? 1:( _temp4657 < _temp4655.base? 1:
_temp4657 >= _temp4655.last_plus_one)){ _throw( Null_Exception);}* _temp4657;}).last_line));}
if( _temp4610.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp4658=( char*)"extra type variables on parameter, ignoring"; struct
_tagged_string _temp4659; _temp4659.curr= _temp4658; _temp4659.base= _temp4658;
_temp4659.last_plus_one= _temp4658 + 44; _temp4659;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4660= yyls; struct Cyc_Yyltype* _temp4662= _temp4660.curr
+( yylsp_offset - 1); if( _temp4660.base == 0? 1:( _temp4662 < _temp4660.base? 1:
_temp4662 >= _temp4660.last_plus_one)){ _throw( Null_Exception);}* _temp4662;}).first_line,({
struct _tagged_ptr3 _temp4663= yyls; struct Cyc_Yyltype* _temp4665= _temp4663.curr
+ yylsp_offset; if( _temp4663.base == 0? 1:( _temp4665 < _temp4663.base? 1:
_temp4665 >= _temp4663.last_plus_one)){ _throw( Null_Exception);}* _temp4665;}).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4666=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4666[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4667; _temp4667.tag= Cyc_ParamDecl_tok; _temp4667.f1=({
struct _tuple2* _temp4668=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4668->f1= _temp4648; _temp4668->f2= _temp4610.f1; _temp4668->f3= _temp4610.f2;
_temp4668;}); _temp4667;}); _temp4666;}); break;}}}}}}} case 168: _LL4583: {
void* _temp4679= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp4670= yyvs; void** _temp4672= _temp4670.curr + yyvsp_offset;
if( _temp4670.base == 0? 1:( _temp4672 < _temp4670.base? 1: _temp4672 >=
_temp4670.last_plus_one)){ _throw( Null_Exception);}* _temp4672;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4673= yyls; struct Cyc_Yyltype* _temp4675= _temp4673.curr
+ yylsp_offset; if( _temp4673.base == 0? 1:( _temp4675 < _temp4673.base? 1:
_temp4675 >= _temp4673.last_plus_one)){ _throw( Null_Exception);}* _temp4675;}).first_line,({
struct _tagged_ptr3 _temp4676= yyls; struct Cyc_Yyltype* _temp4678= _temp4676.curr
+ yylsp_offset; if( _temp4676.base == 0? 1:( _temp4678 < _temp4676.base? 1:
_temp4678 >= _temp4676.last_plus_one)){ _throw( Null_Exception);}* _temp4678;}).last_line));
goto _LL4680; _LL4680: { struct Cyc_List_List* _temp4684=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4681= yyvs; void** _temp4683= _temp4681.curr +
yyvsp_offset; if( _temp4681.base == 0? 1:( _temp4683 < _temp4681.base? 1:
_temp4683 >= _temp4681.last_plus_one)){ _throw( Null_Exception);}* _temp4683;}))).f3;
goto _LL4685; _LL4685: { struct Cyc_Absyn_Tqual _temp4689=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4686= yyvs; void** _temp4688= _temp4686.curr +
yyvsp_offset; if( _temp4686.base == 0? 1:( _temp4688 < _temp4686.base? 1:
_temp4688 >= _temp4686.last_plus_one)){ _throw( Null_Exception);}* _temp4688;}))).f1;
goto _LL4690; _LL4690: if( _temp4684 != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp4691=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4692; _temp4692.curr= _temp4691; _temp4692.base=
_temp4691; _temp4692.last_plus_one= _temp4691 + 38; _temp4692;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4693= yyls; struct Cyc_Yyltype* _temp4695= _temp4693.curr
+ yylsp_offset; if( _temp4693.base == 0? 1:( _temp4695 < _temp4693.base? 1:
_temp4695 >= _temp4693.last_plus_one)){ _throw( Null_Exception);}* _temp4695;}).first_line,({
struct _tagged_ptr3 _temp4696= yyls; struct Cyc_Yyltype* _temp4698= _temp4696.curr
+ yylsp_offset; if( _temp4696.base == 0? 1:( _temp4698 < _temp4696.base? 1:
_temp4698 >= _temp4696.last_plus_one)){ _throw( Null_Exception);}* _temp4698;}).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4699=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4699[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4700; _temp4700.tag= Cyc_ParamDecl_tok; _temp4700.f1=({
struct _tuple2* _temp4701=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4701->f1= 0; _temp4701->f2= _temp4689; _temp4701->f3= _temp4679; _temp4701;});
_temp4700;}); _temp4699;}); break;}}} case 169: _LL4669: { void* _temp4712= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4703= yyvs; void**
_temp4705= _temp4703.curr +( yyvsp_offset - 1); if( _temp4703.base == 0? 1:(
_temp4705 < _temp4703.base? 1: _temp4705 >= _temp4703.last_plus_one)){ _throw(
Null_Exception);}* _temp4705;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4706= yyls; struct Cyc_Yyltype* _temp4708= _temp4706.curr +(
yylsp_offset - 1); if( _temp4706.base == 0? 1:( _temp4708 < _temp4706.base? 1:
_temp4708 >= _temp4706.last_plus_one)){ _throw( Null_Exception);}* _temp4708;}).first_line,({
struct _tagged_ptr3 _temp4709= yyls; struct Cyc_Yyltype* _temp4711= _temp4709.curr
+( yylsp_offset - 1); if( _temp4709.base == 0? 1:( _temp4711 < _temp4709.base? 1:
_temp4711 >= _temp4709.last_plus_one)){ _throw( Null_Exception);}* _temp4711;}).last_line));
goto _LL4713; _LL4713: { struct Cyc_List_List* _temp4717=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4714= yyvs; void** _temp4716= _temp4714.curr +(
yyvsp_offset - 1); if( _temp4714.base == 0? 1:( _temp4716 < _temp4714.base? 1:
_temp4716 >= _temp4714.last_plus_one)){ _throw( Null_Exception);}* _temp4716;}))).f3;
goto _LL4718; _LL4718: { struct Cyc_Absyn_Tqual _temp4722=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4719= yyvs; void** _temp4721= _temp4719.curr +(
yyvsp_offset - 1); if( _temp4719.base == 0? 1:( _temp4721 < _temp4719.base? 1:
_temp4721 >= _temp4719.last_plus_one)){ _throw( Null_Exception);}* _temp4721;}))).f1;
goto _LL4723; _LL4723: { struct Cyc_List_List* _temp4727=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4724= yyvs; void** _temp4726= _temp4724.curr +
yyvsp_offset; if( _temp4724.base == 0? 1:( _temp4726 < _temp4724.base? 1:
_temp4726 >= _temp4724.last_plus_one)){ _throw( Null_Exception);}* _temp4726;})))->tms;
goto _LL4728; _LL4728: { struct _tuple6 _temp4729= Cyc_Parse_apply_tms(
_temp4722, _temp4712, _temp4717, _temp4727); goto _LL4730; _LL4730: if(
_temp4729.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4731=(
char*)"bad type parameters on formal argument, ignoring"; struct _tagged_string
_temp4732; _temp4732.curr= _temp4731; _temp4732.base= _temp4731; _temp4732.last_plus_one=
_temp4731 + 49; _temp4732;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4733= yyls; struct Cyc_Yyltype* _temp4735= _temp4733.curr +( yylsp_offset -
1); if( _temp4733.base == 0? 1:( _temp4735 < _temp4733.base? 1: _temp4735 >=
_temp4733.last_plus_one)){ _throw( Null_Exception);}* _temp4735;}).first_line,({
struct _tagged_ptr3 _temp4736= yyls; struct Cyc_Yyltype* _temp4738= _temp4736.curr
+ yylsp_offset; if( _temp4736.base == 0? 1:( _temp4738 < _temp4736.base? 1:
_temp4738 >= _temp4736.last_plus_one)){ _throw( Null_Exception);}* _temp4738;}).last_line));}
if( _temp4729.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp4739=( char*)"bad attributes on parameter, ignoring"; struct _tagged_string
_temp4740; _temp4740.curr= _temp4739; _temp4740.base= _temp4739; _temp4740.last_plus_one=
_temp4739 + 38; _temp4740;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4741= yyls; struct Cyc_Yyltype* _temp4743= _temp4741.curr +( yylsp_offset -
1); if( _temp4741.base == 0? 1:( _temp4743 < _temp4741.base? 1: _temp4743 >=
_temp4741.last_plus_one)){ _throw( Null_Exception);}* _temp4743;}).first_line,({
struct _tagged_ptr3 _temp4744= yyls; struct Cyc_Yyltype* _temp4746= _temp4744.curr
+ yylsp_offset; if( _temp4744.base == 0? 1:( _temp4746 < _temp4744.base? 1:
_temp4746 >= _temp4744.last_plus_one)){ _throw( Null_Exception);}* _temp4746;}).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4747=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp4747[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp4748; _temp4748.tag= Cyc_ParamDecl_tok; _temp4748.f1=({
struct _tuple2* _temp4749=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4749->f1= 0; _temp4749->f2= _temp4729.f1; _temp4749->f3= _temp4729.f2;
_temp4749;}); _temp4748;}); _temp4747;}); break;}}}}} case 170: _LL4702: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp4751=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp4751[ 0]=({ struct Cyc_IdList_tok_struct
_temp4752; _temp4752.tag= Cyc_IdList_tok; _temp4752.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(({ struct
_tagged_ptr2 _temp4753= yyvs; void** _temp4755= _temp4753.curr + yyvsp_offset;
if( _temp4753.base == 0? 1:( _temp4755 < _temp4753.base? 1: _temp4755 >=
_temp4753.last_plus_one)){ _throw( Null_Exception);}* _temp4755;}))); _temp4752;});
_temp4751;}); break; case 171: _LL4750: yyval=( void*)({ struct Cyc_IdList_tok_struct*
_temp4757=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4757[ 0]=({ struct Cyc_IdList_tok_struct _temp4758; _temp4758.tag= Cyc_IdList_tok;
_temp4758.f1=({ struct Cyc_List_List* _temp4759=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4759->hd=( void*)({ struct
_tagged_string* _temp4760=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4760[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4761= yyvs; void** _temp4763= _temp4761.curr + yyvsp_offset; if( _temp4761.base
== 0? 1:( _temp4763 < _temp4761.base? 1: _temp4763 >= _temp4761.last_plus_one)){
_throw( Null_Exception);}* _temp4763;})); _temp4760;}); _temp4759->tl= 0;
_temp4759;}); _temp4758;}); _temp4757;}); break; case 172: _LL4756: yyval=( void*)({
struct Cyc_IdList_tok_struct* _temp4765=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp4765[ 0]=({ struct Cyc_IdList_tok_struct
_temp4766; _temp4766.tag= Cyc_IdList_tok; _temp4766.f1=({ struct Cyc_List_List*
_temp4767=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4767->hd=( void*)({ struct _tagged_string* _temp4771=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4771[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4772= yyvs; void** _temp4774=
_temp4772.curr + yyvsp_offset; if( _temp4772.base == 0? 1:( _temp4774 <
_temp4772.base? 1: _temp4774 >= _temp4772.last_plus_one)){ _throw(
Null_Exception);}* _temp4774;})); _temp4771;}); _temp4767->tl= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp4768= yyvs; void** _temp4770= _temp4768.curr +(
yyvsp_offset - 2); if( _temp4768.base == 0? 1:( _temp4770 < _temp4768.base? 1:
_temp4770 >= _temp4768.last_plus_one)){ _throw( Null_Exception);}* _temp4770;}));
_temp4767;}); _temp4766;}); _temp4765;}); break; case 173: _LL4764: yyval=({
struct _tagged_ptr2 _temp4776= yyvs; void** _temp4778= _temp4776.curr +
yyvsp_offset; if( _temp4776.base == 0? 1:( _temp4778 < _temp4776.base? 1:
_temp4778 >= _temp4776.last_plus_one)){ _throw( Null_Exception);}* _temp4778;});
break; case 174: _LL4775: yyval=({ struct _tagged_ptr2 _temp4780= yyvs; void**
_temp4782= _temp4780.curr + yyvsp_offset; if( _temp4780.base == 0? 1:( _temp4782
< _temp4780.base? 1: _temp4782 >= _temp4780.last_plus_one)){ _throw(
Null_Exception);}* _temp4782;}); break; case 175: _LL4779: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4784=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4784[ 0]=({ struct Cyc_Exp_tok_struct
_temp4785; _temp4785.tag= Cyc_Exp_tok; _temp4785.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4786=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4786[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4787; _temp4787.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4787.f1= 0; _temp4787.f2= 0; _temp4787;}); _temp4786;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4788= yyls; struct Cyc_Yyltype* _temp4790= _temp4788.curr
+( yylsp_offset - 1); if( _temp4788.base == 0? 1:( _temp4790 < _temp4788.base? 1:
_temp4790 >= _temp4788.last_plus_one)){ _throw( Null_Exception);}* _temp4790;}).first_line,({
struct _tagged_ptr3 _temp4791= yyls; struct Cyc_Yyltype* _temp4793= _temp4791.curr
+ yylsp_offset; if( _temp4791.base == 0? 1:( _temp4793 < _temp4791.base? 1:
_temp4793 >= _temp4791.last_plus_one)){ _throw( Null_Exception);}* _temp4793;}).last_line));
_temp4785;}); _temp4784;}); break; case 176: _LL4783: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4795=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4795[ 0]=({ struct Cyc_Exp_tok_struct
_temp4796; _temp4796.tag= Cyc_Exp_tok; _temp4796.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4797=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4797[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4798; _temp4798.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4798.f1= 0; _temp4798.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4799= yyvs; void** _temp4801= _temp4799.curr +( yyvsp_offset - 1); if(
_temp4799.base == 0? 1:( _temp4801 < _temp4799.base? 1: _temp4801 >= _temp4799.last_plus_one)){
_throw( Null_Exception);}* _temp4801;}))); _temp4798;}); _temp4797;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4802= yyls; struct Cyc_Yyltype* _temp4804= _temp4802.curr
+( yylsp_offset - 2); if( _temp4802.base == 0? 1:( _temp4804 < _temp4802.base? 1:
_temp4804 >= _temp4802.last_plus_one)){ _throw( Null_Exception);}* _temp4804;}).first_line,({
struct _tagged_ptr3 _temp4805= yyls; struct Cyc_Yyltype* _temp4807= _temp4805.curr
+ yylsp_offset; if( _temp4805.base == 0? 1:( _temp4807 < _temp4805.base? 1:
_temp4807 >= _temp4805.last_plus_one)){ _throw( Null_Exception);}* _temp4807;}).last_line));
_temp4796;}); _temp4795;}); break; case 177: _LL4794: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4809=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4809[ 0]=({ struct Cyc_Exp_tok_struct
_temp4810; _temp4810.tag= Cyc_Exp_tok; _temp4810.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4811=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4811[ 0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _temp4812; _temp4812.tag= Cyc_Absyn_UnresolvedMem_e;
_temp4812.f1= 0; _temp4812.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4813= yyvs; void** _temp4815= _temp4813.curr +( yyvsp_offset - 2); if(
_temp4813.base == 0? 1:( _temp4815 < _temp4813.base? 1: _temp4815 >= _temp4813.last_plus_one)){
_throw( Null_Exception);}* _temp4815;}))); _temp4812;}); _temp4811;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4816= yyls; struct Cyc_Yyltype* _temp4818= _temp4816.curr
+( yylsp_offset - 3); if( _temp4816.base == 0? 1:( _temp4818 < _temp4816.base? 1:
_temp4818 >= _temp4816.last_plus_one)){ _throw( Null_Exception);}* _temp4818;}).first_line,({
struct _tagged_ptr3 _temp4819= yyls; struct Cyc_Yyltype* _temp4821= _temp4819.curr
+ yylsp_offset; if( _temp4819.base == 0? 1:( _temp4821 < _temp4819.base? 1:
_temp4821 >= _temp4819.last_plus_one)){ _throw( Null_Exception);}* _temp4821;}).last_line));
_temp4810;}); _temp4809;}); break; case 178: _LL4808: { struct Cyc_Absyn_Vardecl*
_temp4834= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp4823=( struct _tuple1*)
GC_malloc( sizeof( struct _tuple1)); _temp4823->f1=( void*) Cyc_Absyn_Loc_n;
_temp4823->f2=({ struct _tagged_string* _temp4824=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4824[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4825= yyvs; void** _temp4827= _temp4825.curr +(
yyvsp_offset - 5); if( _temp4825.base == 0? 1:( _temp4827 < _temp4825.base? 1:
_temp4827 >= _temp4825.last_plus_one)){ _throw( Null_Exception);}* _temp4827;}));
_temp4824;}); _temp4823;}), Cyc_Absyn_uint_t,( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4828= yyls; struct
Cyc_Yyltype* _temp4830= _temp4828.curr +( yylsp_offset - 5); if( _temp4828.base
== 0? 1:( _temp4830 < _temp4828.base? 1: _temp4830 >= _temp4828.last_plus_one)){
_throw( Null_Exception);}* _temp4830;}).first_line,({ struct _tagged_ptr3
_temp4831= yyls; struct Cyc_Yyltype* _temp4833= _temp4831.curr +( yylsp_offset -
5); if( _temp4831.base == 0? 1:( _temp4833 < _temp4831.base? 1: _temp4833 >=
_temp4831.last_plus_one)){ _throw( Null_Exception);}* _temp4833;}).last_line)));
goto _LL4835; _LL4835: _temp4834->tq=({ struct Cyc_Absyn_Tqual _temp4836;
_temp4836.q_const= 1; _temp4836.q_volatile= 0; _temp4836.q_restrict= 1;
_temp4836;}); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4837=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4837[ 0]=({ struct Cyc_Exp_tok_struct
_temp4838; _temp4838.tag= Cyc_Exp_tok; _temp4838.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp4839=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp4839[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp4840; _temp4840.tag= Cyc_Absyn_Comprehension_e;
_temp4840.f1= _temp4834; _temp4840.f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4841= yyvs; void** _temp4843= _temp4841.curr +( yyvsp_offset - 3); if(
_temp4841.base == 0? 1:( _temp4843 < _temp4841.base? 1: _temp4843 >= _temp4841.last_plus_one)){
_throw( Null_Exception);}* _temp4843;})); _temp4840.f3= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4844= yyvs; void** _temp4846= _temp4844.curr +(
yyvsp_offset - 1); if( _temp4844.base == 0? 1:( _temp4846 < _temp4844.base? 1:
_temp4846 >= _temp4844.last_plus_one)){ _throw( Null_Exception);}* _temp4846;}));
_temp4840;}); _temp4839;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4847= yyls; struct Cyc_Yyltype* _temp4849= _temp4847.curr +( yylsp_offset -
7); if( _temp4847.base == 0? 1:( _temp4849 < _temp4847.base? 1: _temp4849 >=
_temp4847.last_plus_one)){ _throw( Null_Exception);}* _temp4849;}).first_line,({
struct _tagged_ptr3 _temp4850= yyls; struct Cyc_Yyltype* _temp4852= _temp4850.curr
+ yylsp_offset; if( _temp4850.base == 0? 1:( _temp4852 < _temp4850.base? 1:
_temp4852 >= _temp4850.last_plus_one)){ _throw( Null_Exception);}* _temp4852;}).last_line));
_temp4838;}); _temp4837;}); break;} case 179: _LL4822: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp4854=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4854[ 0]=({
struct Cyc_InitializerList_tok_struct _temp4855; _temp4855.tag= Cyc_InitializerList_tok;
_temp4855.f1=({ struct Cyc_List_List* _temp4856=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4856->hd=( void*)({ struct
_tuple18* _temp4857=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4857->f1= 0; _temp4857->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4858= yyvs; void** _temp4860= _temp4858.curr + yyvsp_offset; if( _temp4858.base
== 0? 1:( _temp4860 < _temp4858.base? 1: _temp4860 >= _temp4858.last_plus_one)){
_throw( Null_Exception);}* _temp4860;})); _temp4857;}); _temp4856->tl= 0;
_temp4856;}); _temp4855;}); _temp4854;}); break; case 180: _LL4853: yyval=( void*)({
struct Cyc_InitializerList_tok_struct* _temp4862=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4862[ 0]=({
struct Cyc_InitializerList_tok_struct _temp4863; _temp4863.tag= Cyc_InitializerList_tok;
_temp4863.f1=({ struct Cyc_List_List* _temp4864=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4864->hd=( void*)({ struct
_tuple18* _temp4865=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4865->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4869=
yyvs; void** _temp4871= _temp4869.curr +( yyvsp_offset - 1); if( _temp4869.base
== 0? 1:( _temp4871 < _temp4869.base? 1: _temp4871 >= _temp4869.last_plus_one)){
_throw( Null_Exception);}* _temp4871;})); _temp4865->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4866= yyvs; void** _temp4868= _temp4866.curr +
yyvsp_offset; if( _temp4866.base == 0? 1:( _temp4868 < _temp4866.base? 1:
_temp4868 >= _temp4866.last_plus_one)){ _throw( Null_Exception);}* _temp4868;}));
_temp4865;}); _temp4864->tl= 0; _temp4864;}); _temp4863;}); _temp4862;}); break;
case 181: _LL4861: yyval=( void*)({ struct Cyc_InitializerList_tok_struct*
_temp4873=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4873[ 0]=({ struct Cyc_InitializerList_tok_struct
_temp4874; _temp4874.tag= Cyc_InitializerList_tok; _temp4874.f1=({ struct Cyc_List_List*
_temp4875=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4875->hd=( void*)({ struct _tuple18* _temp4879=( struct _tuple18*)
GC_malloc( sizeof( struct _tuple18)); _temp4879->f1= 0; _temp4879->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4880= yyvs; void** _temp4882= _temp4880.curr +
yyvsp_offset; if( _temp4880.base == 0? 1:( _temp4882 < _temp4880.base? 1:
_temp4882 >= _temp4880.last_plus_one)){ _throw( Null_Exception);}* _temp4882;}));
_temp4879;}); _temp4875->tl= Cyc_yyget_InitializerList_tok(({ struct
_tagged_ptr2 _temp4876= yyvs; void** _temp4878= _temp4876.curr +( yyvsp_offset -
2); if( _temp4876.base == 0? 1:( _temp4878 < _temp4876.base? 1: _temp4878 >=
_temp4876.last_plus_one)){ _throw( Null_Exception);}* _temp4878;})); _temp4875;});
_temp4874;}); _temp4873;}); break; case 182: _LL4872: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp4884=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4884[ 0]=({
struct Cyc_InitializerList_tok_struct _temp4885; _temp4885.tag= Cyc_InitializerList_tok;
_temp4885.f1=({ struct Cyc_List_List* _temp4886=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4886->hd=( void*)({ struct
_tuple18* _temp4890=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4890->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4894=
yyvs; void** _temp4896= _temp4894.curr +( yyvsp_offset - 1); if( _temp4894.base
== 0? 1:( _temp4896 < _temp4894.base? 1: _temp4896 >= _temp4894.last_plus_one)){
_throw( Null_Exception);}* _temp4896;})); _temp4890->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4891= yyvs; void** _temp4893= _temp4891.curr +
yyvsp_offset; if( _temp4891.base == 0? 1:( _temp4893 < _temp4891.base? 1:
_temp4893 >= _temp4891.last_plus_one)){ _throw( Null_Exception);}* _temp4893;}));
_temp4890;}); _temp4886->tl= Cyc_yyget_InitializerList_tok(({ struct
_tagged_ptr2 _temp4887= yyvs; void** _temp4889= _temp4887.curr +( yyvsp_offset -
3); if( _temp4887.base == 0? 1:( _temp4889 < _temp4887.base? 1: _temp4889 >=
_temp4887.last_plus_one)){ _throw( Null_Exception);}* _temp4889;})); _temp4886;});
_temp4885;}); _temp4884;}); break; case 183: _LL4883: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp4898=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp4898[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp4899; _temp4899.tag= Cyc_DesignatorList_tok;
_temp4899.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4900= yyvs; void**
_temp4902= _temp4900.curr +( yyvsp_offset - 1); if( _temp4900.base == 0? 1:(
_temp4902 < _temp4900.base? 1: _temp4902 >= _temp4900.last_plus_one)){ _throw(
Null_Exception);}* _temp4902;}))); _temp4899;}); _temp4898;}); break; case 184:
_LL4897: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct* _temp4904=(
struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4904[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4905; _temp4905.tag=
Cyc_DesignatorList_tok; _temp4905.f1=({ struct Cyc_List_List* _temp4906=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4906->hd=( void*)
Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4907= yyvs; void**
_temp4909= _temp4907.curr + yyvsp_offset; if( _temp4907.base == 0? 1:( _temp4909
< _temp4907.base? 1: _temp4909 >= _temp4907.last_plus_one)){ _throw(
Null_Exception);}* _temp4909;})); _temp4906->tl= 0; _temp4906;}); _temp4905;});
_temp4904;}); break; case 185: _LL4903: yyval=( void*)({ struct Cyc_DesignatorList_tok_struct*
_temp4911=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4911[ 0]=({ struct Cyc_DesignatorList_tok_struct _temp4912; _temp4912.tag=
Cyc_DesignatorList_tok; _temp4912.f1=({ struct Cyc_List_List* _temp4913=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp4913->hd=( void*)
Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4917= yyvs; void**
_temp4919= _temp4917.curr + yyvsp_offset; if( _temp4917.base == 0? 1:( _temp4919
< _temp4917.base? 1: _temp4919 >= _temp4917.last_plus_one)){ _throw(
Null_Exception);}* _temp4919;})); _temp4913->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4914= yyvs; void** _temp4916= _temp4914.curr +(
yyvsp_offset - 1); if( _temp4914.base == 0? 1:( _temp4916 < _temp4914.base? 1:
_temp4916 >= _temp4914.last_plus_one)){ _throw( Null_Exception);}* _temp4916;}));
_temp4913;}); _temp4912;}); _temp4911;}); break; case 186: _LL4910: yyval=( void*)({
struct Cyc_Designator_tok_struct* _temp4921=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp4921[ 0]=({ struct
Cyc_Designator_tok_struct _temp4922; _temp4922.tag= Cyc_Designator_tok;
_temp4922.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp4923=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4923[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp4924; _temp4924.tag=
Cyc_Absyn_ArrayElement; _temp4924.f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4925= yyvs; void** _temp4927= _temp4925.curr +( yyvsp_offset - 1); if(
_temp4925.base == 0? 1:( _temp4927 < _temp4925.base? 1: _temp4927 >= _temp4925.last_plus_one)){
_throw( Null_Exception);}* _temp4927;})); _temp4924;}); _temp4923;})); _temp4922;});
_temp4921;}); break; case 187: _LL4920: yyval=( void*)({ struct Cyc_Designator_tok_struct*
_temp4929=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4929[ 0]=({ struct Cyc_Designator_tok_struct _temp4930; _temp4930.tag= Cyc_Designator_tok;
_temp4930.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp4931=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4931[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp4932; _temp4932.tag= Cyc_Absyn_FieldName;
_temp4932.f1=({ struct _tagged_string* _temp4933=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4933[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4934= yyvs; void** _temp4936= _temp4934.curr +
yyvsp_offset; if( _temp4934.base == 0? 1:( _temp4936 < _temp4934.base? 1:
_temp4936 >= _temp4934.last_plus_one)){ _throw( Null_Exception);}* _temp4936;}));
_temp4933;}); _temp4932;}); _temp4931;})); _temp4930;}); _temp4929;}); break;
case 188: _LL4928: { void* _temp4947= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4938= yyvs; void** _temp4940= _temp4938.curr +
yyvsp_offset; if( _temp4938.base == 0? 1:( _temp4940 < _temp4938.base? 1:
_temp4940 >= _temp4938.last_plus_one)){ _throw( Null_Exception);}* _temp4940;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4941= yyls; struct Cyc_Yyltype*
_temp4943= _temp4941.curr + yylsp_offset; if( _temp4941.base == 0? 1:( _temp4943
< _temp4941.base? 1: _temp4943 >= _temp4941.last_plus_one)){ _throw(
Null_Exception);}* _temp4943;}).first_line,({ struct _tagged_ptr3 _temp4944=
yyls; struct Cyc_Yyltype* _temp4946= _temp4944.curr + yylsp_offset; if(
_temp4944.base == 0? 1:( _temp4946 < _temp4944.base? 1: _temp4946 >= _temp4944.last_plus_one)){
_throw( Null_Exception);}* _temp4946;}).last_line)); goto _LL4948; _LL4948: {
struct Cyc_List_List* _temp4952=(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp4949= yyvs; void** _temp4951= _temp4949.curr + yyvsp_offset;
if( _temp4949.base == 0? 1:( _temp4951 < _temp4949.base? 1: _temp4951 >=
_temp4949.last_plus_one)){ _throw( Null_Exception);}* _temp4951;}))).f3; goto
_LL4953; _LL4953: if( _temp4952 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4954=( char*)"ignoring attributes in type"; struct _tagged_string
_temp4955; _temp4955.curr= _temp4954; _temp4955.base= _temp4954; _temp4955.last_plus_one=
_temp4954 + 28; _temp4955;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4956= yyls; struct Cyc_Yyltype* _temp4958= _temp4956.curr + yylsp_offset;
if( _temp4956.base == 0? 1:( _temp4958 < _temp4956.base? 1: _temp4958 >=
_temp4956.last_plus_one)){ _throw( Null_Exception);}* _temp4958;}).first_line,({
struct _tagged_ptr3 _temp4959= yyls; struct Cyc_Yyltype* _temp4961= _temp4959.curr
+ yylsp_offset; if( _temp4959.base == 0? 1:( _temp4961 < _temp4959.base? 1:
_temp4961 >= _temp4959.last_plus_one)){ _throw( Null_Exception);}* _temp4961;}).last_line));}{
struct Cyc_Absyn_Tqual _temp4965=(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp4962= yyvs; void** _temp4964= _temp4962.curr + yyvsp_offset;
if( _temp4962.base == 0? 1:( _temp4964 < _temp4962.base? 1: _temp4964 >=
_temp4962.last_plus_one)){ _throw( Null_Exception);}* _temp4964;}))).f1; goto
_LL4966; _LL4966: yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp4967=(
struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4967[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp4968; _temp4968.tag= Cyc_ParamDecl_tok;
_temp4968.f1=({ struct _tuple2* _temp4969=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp4969[ 0]=({ struct _tuple2 _temp4970; _temp4970.f1= 0;
_temp4970.f2= _temp4965; _temp4970.f3= _temp4947; _temp4970;}); _temp4969;});
_temp4968;}); _temp4967;}); break;}}} case 189: _LL4937: { void* _temp4981= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4972= yyvs; void**
_temp4974= _temp4972.curr +( yyvsp_offset - 1); if( _temp4972.base == 0? 1:(
_temp4974 < _temp4972.base? 1: _temp4974 >= _temp4972.last_plus_one)){ _throw(
Null_Exception);}* _temp4974;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4975= yyls; struct Cyc_Yyltype* _temp4977= _temp4975.curr +(
yylsp_offset - 1); if( _temp4975.base == 0? 1:( _temp4977 < _temp4975.base? 1:
_temp4977 >= _temp4975.last_plus_one)){ _throw( Null_Exception);}* _temp4977;}).first_line,({
struct _tagged_ptr3 _temp4978= yyls; struct Cyc_Yyltype* _temp4980= _temp4978.curr
+( yylsp_offset - 1); if( _temp4978.base == 0? 1:( _temp4980 < _temp4978.base? 1:
_temp4980 >= _temp4978.last_plus_one)){ _throw( Null_Exception);}* _temp4980;}).last_line));
goto _LL4982; _LL4982: { struct Cyc_List_List* _temp4986=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4983= yyvs; void** _temp4985= _temp4983.curr +(
yyvsp_offset - 1); if( _temp4983.base == 0? 1:( _temp4985 < _temp4983.base? 1:
_temp4985 >= _temp4983.last_plus_one)){ _throw( Null_Exception);}* _temp4985;}))).f3;
goto _LL4987; _LL4987: { struct Cyc_Absyn_Tqual _temp4991=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4988= yyvs; void** _temp4990= _temp4988.curr +(
yyvsp_offset - 1); if( _temp4988.base == 0? 1:( _temp4990 < _temp4988.base? 1:
_temp4990 >= _temp4988.last_plus_one)){ _throw( Null_Exception);}* _temp4990;}))).f1;
goto _LL4992; _LL4992: { struct Cyc_List_List* _temp4996=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4993= yyvs; void** _temp4995= _temp4993.curr +
yyvsp_offset; if( _temp4993.base == 0? 1:( _temp4995 < _temp4993.base? 1:
_temp4995 >= _temp4993.last_plus_one)){ _throw( Null_Exception);}* _temp4995;})))->tms;
goto _LL4997; _LL4997: { struct _tuple6 _temp4998= Cyc_Parse_apply_tms(
_temp4991, _temp4981, _temp4986, _temp4996); goto _LL4999; _LL4999: if(
_temp4998.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp5000=(
char*)"bad type params, ignoring"; struct _tagged_string _temp5001; _temp5001.curr=
_temp5000; _temp5001.base= _temp5000; _temp5001.last_plus_one= _temp5000 + 26;
_temp5001;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5002= yyls;
struct Cyc_Yyltype* _temp5004= _temp5002.curr + yylsp_offset; if( _temp5002.base
== 0? 1:( _temp5004 < _temp5002.base? 1: _temp5004 >= _temp5002.last_plus_one)){
_throw( Null_Exception);}* _temp5004;}).first_line,({ struct _tagged_ptr3
_temp5005= yyls; struct Cyc_Yyltype* _temp5007= _temp5005.curr + yylsp_offset;
if( _temp5005.base == 0? 1:( _temp5007 < _temp5005.base? 1: _temp5007 >=
_temp5005.last_plus_one)){ _throw( Null_Exception);}* _temp5007;}).last_line));}
if( _temp4998.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp5008=( char*)"bad specifiers, ignoring"; struct _tagged_string _temp5009;
_temp5009.curr= _temp5008; _temp5009.base= _temp5008; _temp5009.last_plus_one=
_temp5008 + 25; _temp5009;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5010= yyls; struct Cyc_Yyltype* _temp5012= _temp5010.curr + yylsp_offset;
if( _temp5010.base == 0? 1:( _temp5012 < _temp5010.base? 1: _temp5012 >=
_temp5010.last_plus_one)){ _throw( Null_Exception);}* _temp5012;}).first_line,({
struct _tagged_ptr3 _temp5013= yyls; struct Cyc_Yyltype* _temp5015= _temp5013.curr
+ yylsp_offset; if( _temp5013.base == 0? 1:( _temp5015 < _temp5013.base? 1:
_temp5015 >= _temp5013.last_plus_one)){ _throw( Null_Exception);}* _temp5015;}).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp5016=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp5016[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp5017; _temp5017.tag= Cyc_ParamDecl_tok; _temp5017.f1=({
struct _tuple2* _temp5018=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp5018->f1= 0; _temp5018->f2= _temp4998.f1; _temp5018->f3= _temp4998.f2;
_temp5018;}); _temp5017;}); _temp5016;}); break;}}}}} case 190: _LL4971: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp5020=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp5020[ 0]=({ struct Cyc_Type_tok_struct
_temp5021; _temp5021.tag= Cyc_Type_tok; _temp5021.f1=( void*)(* Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp5022= yyvs; void** _temp5024= _temp5022.curr +
yyvsp_offset; if( _temp5022.base == 0? 1:( _temp5024 < _temp5022.base? 1:
_temp5024 >= _temp5022.last_plus_one)){ _throw( Null_Exception);}* _temp5024;}))).f3;
_temp5021;}); _temp5020;}); break; case 191: _LL5019: yyval=( void*)({ struct
Cyc_Type_tok_struct* _temp5026=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct)); _temp5026[ 0]=({ struct Cyc_Type_tok_struct
_temp5027; _temp5027.tag= Cyc_Type_tok; _temp5027.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp5028=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp5028[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp5029; _temp5029.tag= Cyc_Absyn_JoinEff; _temp5029.f1=
0; _temp5029;}); _temp5028;})); _temp5027;}); _temp5026;}); break; case 192:
_LL5025: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp5031=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp5031[ 0]=({ struct Cyc_Type_tok_struct
_temp5032; _temp5032.tag= Cyc_Type_tok; _temp5032.f1=( void*)(( void*)({ struct
Cyc_Absyn_JoinEff_struct* _temp5033=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp5033[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp5034; _temp5034.tag= Cyc_Absyn_JoinEff; _temp5034.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5035= yyvs; void** _temp5037=
_temp5035.curr +( yyvsp_offset - 1); if( _temp5035.base == 0? 1:( _temp5037 <
_temp5035.base? 1: _temp5037 >= _temp5035.last_plus_one)){ _throw(
Null_Exception);}* _temp5037;})); _temp5034;}); _temp5033;})); _temp5032;});
_temp5031;}); break; case 193: _LL5030: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp5039=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp5039[ 0]=({ struct Cyc_Type_tok_struct _temp5040; _temp5040.tag= Cyc_Type_tok;
_temp5040.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp5041=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp5041[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp5042; _temp5042.tag= Cyc_Absyn_JoinEff;
_temp5042.f1=({ struct Cyc_List_List* _temp5043=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5043->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp5047= yyvs; void** _temp5049= _temp5047.curr +(
yyvsp_offset - 2); if( _temp5047.base == 0? 1:( _temp5049 < _temp5047.base? 1:
_temp5049 >= _temp5047.last_plus_one)){ _throw( Null_Exception);}* _temp5049;}));
_temp5043->tl= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5044= yyvs;
void** _temp5046= _temp5044.curr + yyvsp_offset; if( _temp5044.base == 0? 1:(
_temp5046 < _temp5044.base? 1: _temp5046 >= _temp5044.last_plus_one)){ _throw(
Null_Exception);}* _temp5046;})); _temp5043;}); _temp5042;}); _temp5041;}));
_temp5040;}); _temp5039;}); break; case 194: _LL5038: yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp5051=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp5051[ 0]=({ struct Cyc_TypeList_tok_struct
_temp5052; _temp5052.tag= Cyc_TypeList_tok; _temp5052.f1=({ struct Cyc_List_List*
_temp5053=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5053->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp5054= yyvs;
void** _temp5056= _temp5054.curr + yyvsp_offset; if( _temp5054.base == 0? 1:(
_temp5056 < _temp5054.base? 1: _temp5056 >= _temp5054.last_plus_one)){ _throw(
Null_Exception);}* _temp5056;})); _temp5053->tl= 0; _temp5053;}); _temp5052;});
_temp5051;}); break; case 195: _LL5050: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp5058=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp5058[ 0]=({ struct Cyc_TypeList_tok_struct _temp5059; _temp5059.tag= Cyc_TypeList_tok;
_temp5059.f1=({ struct Cyc_List_List* _temp5060=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5060->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp5064= yyvs; void** _temp5066= _temp5064.curr +
yyvsp_offset; if( _temp5064.base == 0? 1:( _temp5066 < _temp5064.base? 1:
_temp5066 >= _temp5064.last_plus_one)){ _throw( Null_Exception);}* _temp5066;}));
_temp5060->tl= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5061= yyvs;
void** _temp5063= _temp5061.curr +( yyvsp_offset - 2); if( _temp5061.base == 0?
1:( _temp5063 < _temp5061.base? 1: _temp5063 >= _temp5061.last_plus_one)){
_throw( Null_Exception);}* _temp5063;})); _temp5060;}); _temp5059;}); _temp5058;});
break; case 196: _LL5057: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5068=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5068[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5069; _temp5069.tag= Cyc_AbstractDeclarator_tok; _temp5069.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5070=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5070->tms= Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp5071=
yyvs; void** _temp5073= _temp5071.curr + yyvsp_offset; if( _temp5071.base == 0?
1:( _temp5073 < _temp5071.base? 1: _temp5073 >= _temp5071.last_plus_one)){
_throw( Null_Exception);}* _temp5073;})); _temp5070;}); _temp5069;}); _temp5068;});
break; case 197: _LL5067: yyval=({ struct _tagged_ptr2 _temp5075= yyvs; void**
_temp5077= _temp5075.curr + yyvsp_offset; if( _temp5075.base == 0? 1:( _temp5077
< _temp5075.base? 1: _temp5077 >= _temp5075.last_plus_one)){ _throw(
Null_Exception);}* _temp5077;}); break; case 198: _LL5074: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp5079=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5079[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5080; _temp5080.tag= Cyc_AbstractDeclarator_tok;
_temp5080.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5081=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5081->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp5082= yyvs; void**
_temp5084= _temp5082.curr +( yyvsp_offset - 1); if( _temp5082.base == 0? 1:(
_temp5084 < _temp5082.base? 1: _temp5084 >= _temp5082.last_plus_one)){ _throw(
Null_Exception);}* _temp5084;})),( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp5085= yyvs; void** _temp5087= _temp5085.curr + yyvsp_offset;
if( _temp5085.base == 0? 1:( _temp5087 < _temp5085.base? 1: _temp5087 >=
_temp5085.last_plus_one)){ _throw( Null_Exception);}* _temp5087;})))->tms);
_temp5081;}); _temp5080;}); _temp5079;}); break; case 199: _LL5078: yyval=({
struct _tagged_ptr2 _temp5089= yyvs; void** _temp5091= _temp5089.curr +(
yyvsp_offset - 1); if( _temp5089.base == 0? 1:( _temp5091 < _temp5089.base? 1:
_temp5091 >= _temp5089.last_plus_one)){ _throw( Null_Exception);}* _temp5091;});
break; case 200: _LL5088: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5093=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5093[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5094; _temp5094.tag= Cyc_AbstractDeclarator_tok; _temp5094.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5095=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5095->tms=({ struct Cyc_List_List* _temp5096=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5096->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp5096->tl= 0; _temp5096;}); _temp5095;}); _temp5094;}); _temp5093;}); break;
case 201: _LL5092: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5098=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5098[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5099; _temp5099.tag= Cyc_AbstractDeclarator_tok; _temp5099.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5100=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5100->tms=({ struct Cyc_List_List* _temp5101=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5101->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp5101->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp5102= yyvs; void** _temp5104= _temp5102.curr +( yyvsp_offset - 2); if(
_temp5102.base == 0? 1:( _temp5104 < _temp5102.base? 1: _temp5104 >= _temp5102.last_plus_one)){
_throw( Null_Exception);}* _temp5104;})))->tms; _temp5101;}); _temp5100;});
_temp5099;}); _temp5098;}); break; case 202: _LL5097: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5106=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5106[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5107; _temp5107.tag= Cyc_AbstractDeclarator_tok;
_temp5107.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5108=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5108->tms=({
struct Cyc_List_List* _temp5109=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5109->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp5110=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp5110[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp5111; _temp5111.tag= Cyc_Absyn_ConstArray_mod; _temp5111.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5112= yyvs; void** _temp5114= _temp5112.curr +(
yyvsp_offset - 1); if( _temp5112.base == 0? 1:( _temp5114 < _temp5112.base? 1:
_temp5114 >= _temp5112.last_plus_one)){ _throw( Null_Exception);}* _temp5114;}));
_temp5111;}); _temp5110;})); _temp5109->tl= 0; _temp5109;}); _temp5108;});
_temp5107;}); _temp5106;}); break; case 203: _LL5105: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5116=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5116[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5117; _temp5117.tag= Cyc_AbstractDeclarator_tok;
_temp5117.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5118=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5118->tms=({
struct Cyc_List_List* _temp5119=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5119->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp5123=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp5123[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp5124; _temp5124.tag= Cyc_Absyn_ConstArray_mod; _temp5124.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5125= yyvs; void** _temp5127= _temp5125.curr +(
yyvsp_offset - 1); if( _temp5125.base == 0? 1:( _temp5127 < _temp5125.base? 1:
_temp5127 >= _temp5125.last_plus_one)){ _throw( Null_Exception);}* _temp5127;}));
_temp5124;}); _temp5123;})); _temp5119->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5120= yyvs; void** _temp5122= _temp5120.curr +(
yyvsp_offset - 3); if( _temp5120.base == 0? 1:( _temp5122 < _temp5120.base? 1:
_temp5122 >= _temp5120.last_plus_one)){ _throw( Null_Exception);}* _temp5122;})))->tms;
_temp5119;}); _temp5118;}); _temp5117;}); _temp5116;}); break; case 204: _LL5115:
yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5129=( struct
Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5129[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5130; _temp5130.tag=
Cyc_AbstractDeclarator_tok; _temp5130.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5131=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5131->tms=({ struct Cyc_List_List* _temp5132=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5132->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5133=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5133[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5134; _temp5134.tag= Cyc_Absyn_Function_mod;
_temp5134.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5135=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5135[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5136; _temp5136.tag= Cyc_Absyn_WithTypes;
_temp5136.f1= 0; _temp5136.f2= 0; _temp5136.f3= 0; _temp5136;}); _temp5135;}));
_temp5134;}); _temp5133;})); _temp5132->tl= 0; _temp5132;}); _temp5131;});
_temp5130;}); _temp5129;}); break; case 205: _LL5128: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp5138=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp5138[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp5139; _temp5139.tag= Cyc_AbstractDeclarator_tok;
_temp5139.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp5140=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp5140->tms=({
struct Cyc_List_List* _temp5141=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5141->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp5142=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp5142[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp5143; _temp5143.tag=
Cyc_Absyn_Function_mod; _temp5143.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp5144=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5144[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5145; _temp5145.tag= Cyc_Absyn_WithTypes;
_temp5145.f1= 0; _temp5145.f2= 0; _temp5145.f3=({ struct Cyc_Core_Opt* _temp5146=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5146->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp5147=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp5147[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp5148; _temp5148.tag= Cyc_Absyn_JoinEff; _temp5148.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5149= yyvs; void** _temp5151=
_temp5149.curr +( yyvsp_offset - 1); if( _temp5149.base == 0? 1:( _temp5151 <
_temp5149.base? 1: _temp5151 >= _temp5149.last_plus_one)){ _throw(
Null_Exception);}* _temp5151;})); _temp5148;}); _temp5147;})); _temp5146;});
_temp5145;}); _temp5144;})); _temp5143;}); _temp5142;})); _temp5141->tl= 0;
_temp5141;}); _temp5140;}); _temp5139;}); _temp5138;}); break; case 206: _LL5137: {
struct _tuple16 _temp5158; struct Cyc_Core_Opt* _temp5159; int _temp5161; struct
Cyc_List_List* _temp5163; struct _tuple16* _temp5156= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp5153= yyvs; void** _temp5155= _temp5153.curr +(
yyvsp_offset - 1); if( _temp5153.base == 0? 1:( _temp5155 < _temp5153.base? 1:
_temp5155 >= _temp5153.last_plus_one)){ _throw( Null_Exception);}* _temp5155;}));
_temp5158=* _temp5156; _LL5164: _temp5163= _temp5158.f1; goto _LL5162; _LL5162:
_temp5161= _temp5158.f2; goto _LL5160; _LL5160: _temp5159= _temp5158.f3; goto
_LL5157; _LL5157: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5165=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5165[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5166; _temp5166.tag= Cyc_AbstractDeclarator_tok; _temp5166.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5167=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5167->tms=({ struct Cyc_List_List* _temp5168=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5168->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5169=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5169[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5170; _temp5170.tag= Cyc_Absyn_Function_mod;
_temp5170.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5171=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5171[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5172; _temp5172.tag= Cyc_Absyn_WithTypes;
_temp5172.f1= _temp5163; _temp5172.f2= _temp5161; _temp5172.f3= _temp5159;
_temp5172;}); _temp5171;})); _temp5170;}); _temp5169;})); _temp5168->tl= 0;
_temp5168;}); _temp5167;}); _temp5166;}); _temp5165;}); break;} case 207:
_LL5152: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5174=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5174[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5175; _temp5175.tag=
Cyc_AbstractDeclarator_tok; _temp5175.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5176=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5176->tms=({ struct Cyc_List_List* _temp5177=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5177->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5181=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5181[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5182; _temp5182.tag= Cyc_Absyn_Function_mod;
_temp5182.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5183=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5183[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5184; _temp5184.tag= Cyc_Absyn_WithTypes;
_temp5184.f1= 0; _temp5184.f2= 0; _temp5184.f3= 0; _temp5184;}); _temp5183;}));
_temp5182;}); _temp5181;})); _temp5177->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5178= yyvs; void** _temp5180= _temp5178.curr +(
yyvsp_offset - 2); if( _temp5178.base == 0? 1:( _temp5180 < _temp5178.base? 1:
_temp5180 >= _temp5178.last_plus_one)){ _throw( Null_Exception);}* _temp5180;})))->tms;
_temp5177;}); _temp5176;}); _temp5175;}); _temp5174;}); break; case 208: _LL5173: {
struct Cyc_Core_Opt* _temp5192=({ struct Cyc_Core_Opt* _temp5186=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5186->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp5187=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp5187[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp5188; _temp5188.tag= Cyc_Absyn_JoinEff; _temp5188.f1=
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp5189= yyvs; void** _temp5191=
_temp5189.curr +( yyvsp_offset - 1); if( _temp5189.base == 0? 1:( _temp5191 <
_temp5189.base? 1: _temp5191 >= _temp5189.last_plus_one)){ _throw(
Null_Exception);}* _temp5191;})); _temp5188;}); _temp5187;})); _temp5186;});
goto _LL5193; _LL5193: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5194=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5194[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5195; _temp5195.tag= Cyc_AbstractDeclarator_tok; _temp5195.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5196=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5196->tms=({ struct Cyc_List_List* _temp5197=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5197->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5201=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5201[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5202; _temp5202.tag= Cyc_Absyn_Function_mod;
_temp5202.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5203=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5203[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5204; _temp5204.tag= Cyc_Absyn_WithTypes;
_temp5204.f1= 0; _temp5204.f2= 0; _temp5204.f3= _temp5192; _temp5204;});
_temp5203;})); _temp5202;}); _temp5201;})); _temp5197->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5198= yyvs; void** _temp5200= _temp5198.curr +(
yyvsp_offset - 4); if( _temp5198.base == 0? 1:( _temp5200 < _temp5198.base? 1:
_temp5200 >= _temp5198.last_plus_one)){ _throw( Null_Exception);}* _temp5200;})))->tms;
_temp5197;}); _temp5196;}); _temp5195;}); _temp5194;}); break;} case 209:
_LL5185: { struct _tuple16 _temp5211; struct Cyc_Core_Opt* _temp5212; int
_temp5214; struct Cyc_List_List* _temp5216; struct _tuple16* _temp5209= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp5206= yyvs; void** _temp5208= _temp5206.curr +(
yyvsp_offset - 1); if( _temp5206.base == 0? 1:( _temp5208 < _temp5206.base? 1:
_temp5208 >= _temp5206.last_plus_one)){ _throw( Null_Exception);}* _temp5208;}));
_temp5211=* _temp5209; _LL5217: _temp5216= _temp5211.f1; goto _LL5215; _LL5215:
_temp5214= _temp5211.f2; goto _LL5213; _LL5213: _temp5212= _temp5211.f3; goto
_LL5210; _LL5210: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5218=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5218[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5219; _temp5219.tag= Cyc_AbstractDeclarator_tok; _temp5219.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5220=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5220->tms=({ struct Cyc_List_List* _temp5221=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5221->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp5225=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp5225[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp5226; _temp5226.tag= Cyc_Absyn_Function_mod;
_temp5226.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp5227=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp5227[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp5228; _temp5228.tag= Cyc_Absyn_WithTypes;
_temp5228.f1= _temp5216; _temp5228.f2= _temp5214; _temp5228.f3= _temp5212;
_temp5228;}); _temp5227;})); _temp5226;}); _temp5225;})); _temp5221->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5222= yyvs; void** _temp5224= _temp5222.curr +(
yyvsp_offset - 3); if( _temp5222.base == 0? 1:( _temp5224 < _temp5222.base? 1:
_temp5224 >= _temp5222.last_plus_one)){ _throw( Null_Exception);}* _temp5224;})))->tms;
_temp5221;}); _temp5220;}); _temp5219;}); _temp5218;}); break;} case 210:
_LL5205: { struct Cyc_List_List* _temp5239=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5230= yyls; struct Cyc_Yyltype* _temp5232= _temp5230.curr
+( yylsp_offset - 2); if( _temp5230.base == 0? 1:( _temp5232 < _temp5230.base? 1:
_temp5232 >= _temp5230.last_plus_one)){ _throw( Null_Exception);}* _temp5232;}).first_line,({
struct _tagged_ptr3 _temp5233= yyls; struct Cyc_Yyltype* _temp5235= _temp5233.curr
+ yylsp_offset; if( _temp5233.base == 0? 1:( _temp5235 < _temp5233.base? 1:
_temp5235 >= _temp5233.last_plus_one)){ _throw( Null_Exception);}* _temp5235;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5236= yyvs; void** _temp5238= _temp5236.curr +(
yyvsp_offset - 1); if( _temp5236.base == 0? 1:( _temp5238 < _temp5236.base? 1:
_temp5238 >= _temp5236.last_plus_one)){ _throw( Null_Exception);}* _temp5238;}))));
goto _LL5240; _LL5240: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5241=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5241[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5242; _temp5242.tag= Cyc_AbstractDeclarator_tok; _temp5242.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5243=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5243->tms=({ struct Cyc_List_List* _temp5244=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5244->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp5248=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp5248[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp5249; _temp5249.tag= Cyc_Absyn_TypeParams_mod;
_temp5249.f1= _temp5239; _temp5249.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5250= yyls; struct Cyc_Yyltype* _temp5252= _temp5250.curr +(
yylsp_offset - 2); if( _temp5250.base == 0? 1:( _temp5252 < _temp5250.base? 1:
_temp5252 >= _temp5250.last_plus_one)){ _throw( Null_Exception);}* _temp5252;}).first_line,({
struct _tagged_ptr3 _temp5253= yyls; struct Cyc_Yyltype* _temp5255= _temp5253.curr
+ yylsp_offset; if( _temp5253.base == 0? 1:( _temp5255 < _temp5253.base? 1:
_temp5255 >= _temp5253.last_plus_one)){ _throw( Null_Exception);}* _temp5255;}).last_line);
_temp5249.f3= 0; _temp5249;}); _temp5248;})); _temp5244->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5245= yyvs; void** _temp5247= _temp5245.curr +(
yyvsp_offset - 3); if( _temp5245.base == 0? 1:( _temp5247 < _temp5245.base? 1:
_temp5247 >= _temp5245.last_plus_one)){ _throw( Null_Exception);}* _temp5247;})))->tms;
_temp5244;}); _temp5243;}); _temp5242;}); _temp5241;}); break;} case 211:
_LL5229:(( struct Cyc_Lexing_lexbuf*)({ struct Cyc_Core_Opt* _temp5257= Cyc_Parse_lbuf;
if( _temp5257 == 0){ _throw( Null_Exception);} _temp5257->v;}))->lex_curr_pos -=
1;{ struct Cyc_List_List* _temp5267=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5258= yyls; struct Cyc_Yyltype* _temp5260= _temp5258.curr
+( yylsp_offset - 2); if( _temp5258.base == 0? 1:( _temp5260 < _temp5258.base? 1:
_temp5260 >= _temp5258.last_plus_one)){ _throw( Null_Exception);}* _temp5260;}).first_line,({
struct _tagged_ptr3 _temp5261= yyls; struct Cyc_Yyltype* _temp5263= _temp5261.curr
+ yylsp_offset; if( _temp5261.base == 0? 1:( _temp5263 < _temp5261.base? 1:
_temp5263 >= _temp5261.last_plus_one)){ _throw( Null_Exception);}* _temp5263;}).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5264= yyvs; void** _temp5266= _temp5264.curr +(
yyvsp_offset - 1); if( _temp5264.base == 0? 1:( _temp5266 < _temp5264.base? 1:
_temp5266 >= _temp5264.last_plus_one)){ _throw( Null_Exception);}* _temp5266;}))));
goto _LL5268; _LL5268: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp5269=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp5269[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp5270; _temp5270.tag= Cyc_AbstractDeclarator_tok; _temp5270.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5271=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5271->tms=({ struct Cyc_List_List* _temp5272=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5272->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp5276=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp5276[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp5277; _temp5277.tag= Cyc_Absyn_TypeParams_mod;
_temp5277.f1= _temp5267; _temp5277.f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5278= yyls; struct Cyc_Yyltype* _temp5280= _temp5278.curr +(
yylsp_offset - 2); if( _temp5278.base == 0? 1:( _temp5280 < _temp5278.base? 1:
_temp5280 >= _temp5278.last_plus_one)){ _throw( Null_Exception);}* _temp5280;}).first_line,({
struct _tagged_ptr3 _temp5281= yyls; struct Cyc_Yyltype* _temp5283= _temp5281.curr
+ yylsp_offset; if( _temp5281.base == 0? 1:( _temp5283 < _temp5281.base? 1:
_temp5283 >= _temp5281.last_plus_one)){ _throw( Null_Exception);}* _temp5283;}).last_line);
_temp5277.f3= 0; _temp5277;}); _temp5276;})); _temp5272->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp5273= yyvs; void** _temp5275= _temp5273.curr +(
yyvsp_offset - 3); if( _temp5273.base == 0? 1:( _temp5275 < _temp5273.base? 1:
_temp5275 >= _temp5273.last_plus_one)){ _throw( Null_Exception);}* _temp5275;})))->tms;
_temp5272;}); _temp5271;}); _temp5270;}); _temp5269;}); break;} case 212:
_LL5256: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp5285=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp5285[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp5286; _temp5286.tag=
Cyc_AbstractDeclarator_tok; _temp5286.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp5287=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp5287->tms=({ struct Cyc_List_List* _temp5288=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5288->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp5292=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp5292[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp5293; _temp5293.tag= Cyc_Absyn_Attributes_mod;
_temp5293.f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5294= yyls;
struct Cyc_Yyltype* _temp5296= _temp5294.curr + yylsp_offset; if( _temp5294.base
== 0? 1:( _temp5296 < _temp5294.base? 1: _temp5296 >= _temp5294.last_plus_one)){
_throw( Null_Exception);}* _temp5296;}).first_line,({ struct _tagged_ptr3
_temp5297= yyls; struct Cyc_Yyltype* _temp5299= _temp5297.curr + yylsp_offset;
if( _temp5297.base == 0? 1:( _temp5299 < _temp5297.base? 1: _temp5299 >=
_temp5297.last_plus_one)){ _throw( Null_Exception);}* _temp5299;}).last_line);
_temp5293.f2= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp5300= yyvs;
void** _temp5302= _temp5300.curr + yyvsp_offset; if( _temp5300.base == 0? 1:(
_temp5302 < _temp5300.base? 1: _temp5302 >= _temp5300.last_plus_one)){ _throw(
Null_Exception);}* _temp5302;})); _temp5293;}); _temp5292;})); _temp5288->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp5289= yyvs; void**
_temp5291= _temp5289.curr +( yyvsp_offset - 1); if( _temp5289.base == 0? 1:(
_temp5291 < _temp5289.base? 1: _temp5291 >= _temp5289.last_plus_one)){ _throw(
Null_Exception);}* _temp5291;})))->tms; _temp5288;}); _temp5287;}); _temp5286;});
_temp5285;}); break; case 213: _LL5284: yyval=({ struct _tagged_ptr2 _temp5304=
yyvs; void** _temp5306= _temp5304.curr + yyvsp_offset; if( _temp5304.base == 0?
1:( _temp5306 < _temp5304.base? 1: _temp5306 >= _temp5304.last_plus_one)){
_throw( Null_Exception);}* _temp5306;}); break; case 214: _LL5303: yyval=({
struct _tagged_ptr2 _temp5308= yyvs; void** _temp5310= _temp5308.curr +
yyvsp_offset; if( _temp5308.base == 0? 1:( _temp5310 < _temp5308.base? 1:
_temp5310 >= _temp5308.last_plus_one)){ _throw( Null_Exception);}* _temp5310;});
break; case 215: _LL5307: yyval=({ struct _tagged_ptr2 _temp5312= yyvs; void**
_temp5314= _temp5312.curr + yyvsp_offset; if( _temp5312.base == 0? 1:( _temp5314
< _temp5312.base? 1: _temp5314 >= _temp5312.last_plus_one)){ _throw(
Null_Exception);}* _temp5314;}); break; case 216: _LL5311: yyval=({ struct
_tagged_ptr2 _temp5316= yyvs; void** _temp5318= _temp5316.curr + yyvsp_offset;
if( _temp5316.base == 0? 1:( _temp5318 < _temp5316.base? 1: _temp5318 >=
_temp5316.last_plus_one)){ _throw( Null_Exception);}* _temp5318;}); break; case
217: _LL5315: yyval=({ struct _tagged_ptr2 _temp5320= yyvs; void** _temp5322=
_temp5320.curr + yyvsp_offset; if( _temp5320.base == 0? 1:( _temp5322 <
_temp5320.base? 1: _temp5322 >= _temp5320.last_plus_one)){ _throw(
Null_Exception);}* _temp5322;}); break; case 218: _LL5319: yyval=({ struct
_tagged_ptr2 _temp5324= yyvs; void** _temp5326= _temp5324.curr + yyvsp_offset;
if( _temp5324.base == 0? 1:( _temp5326 < _temp5324.base? 1: _temp5326 >=
_temp5324.last_plus_one)){ _throw( Null_Exception);}* _temp5326;}); break; case
219: _LL5323: if( Cyc_String_zstrcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5328= yyvs; void** _temp5330= _temp5328.curr +( yyvsp_offset -
3); if( _temp5328.base == 0? 1:( _temp5330 < _temp5328.base? 1: _temp5330 >=
_temp5328.last_plus_one)){ _throw( Null_Exception);}* _temp5330;})),( struct
_tagged_string)({ char* _temp5331=( char*)"`H"; struct _tagged_string _temp5332;
_temp5332.curr= _temp5331; _temp5332.base= _temp5331; _temp5332.last_plus_one=
_temp5331 + 3; _temp5332;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp5333=( char*)"bad occurrence of heap region `H"; struct
_tagged_string _temp5334; _temp5334.curr= _temp5333; _temp5334.base= _temp5333;
_temp5334.last_plus_one= _temp5333 + 33; _temp5334;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5335= yyls; struct Cyc_Yyltype* _temp5337= _temp5335.curr
+( yylsp_offset - 3); if( _temp5335.base == 0? 1:( _temp5337 < _temp5335.base? 1:
_temp5337 >= _temp5335.last_plus_one)){ _throw( Null_Exception);}* _temp5337;}).first_line,({
struct _tagged_ptr3 _temp5338= yyls; struct Cyc_Yyltype* _temp5340= _temp5338.curr
+( yylsp_offset - 3); if( _temp5338.base == 0? 1:( _temp5340 < _temp5338.base? 1:
_temp5340 >= _temp5338.last_plus_one)){ _throw( Null_Exception);}* _temp5340;}).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5363=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5363->name=({ struct
_tagged_string* _temp5364=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp5364[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp5365= yyvs; void** _temp5367= _temp5365.curr +( yyvsp_offset - 3); if(
_temp5365.base == 0? 1:( _temp5367 < _temp5365.base? 1: _temp5367 >= _temp5365.last_plus_one)){
_throw( Null_Exception);}* _temp5367;})); _temp5364;}); _temp5363->kind=((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind);
_temp5363;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp5361=(
struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp5361[ 0]=({ struct Cyc_Absyn_VarType_struct _temp5362; _temp5362.tag= Cyc_Absyn_VarType;
_temp5362.f1= tv; _temp5362;}); _temp5361;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5341=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5341[ 0]=({ struct Cyc_Stmt_tok_struct _temp5342; _temp5342.tag= Cyc_Stmt_tok;
_temp5342.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp5343=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp5343[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp5344; _temp5344.tag= Cyc_Absyn_Region_s;
_temp5344.f1= tv; _temp5344.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5345=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5345->f1=(
void*) Cyc_Absyn_Loc_n; _temp5345->f2=({ struct _tagged_string* _temp5346=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp5346[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5347= yyvs; void** _temp5349=
_temp5347.curr +( yyvsp_offset - 1); if( _temp5347.base == 0? 1:( _temp5349 <
_temp5347.base? 1: _temp5349 >= _temp5347.last_plus_one)){ _throw(
Null_Exception);}* _temp5349;})); _temp5346;}); _temp5345;}),( void*)({ struct
Cyc_Absyn_RgnHandleType_struct* _temp5350=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp5350[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp5351; _temp5351.tag= Cyc_Absyn_RgnHandleType;
_temp5351.f1=( void*) t; _temp5351;}); _temp5350;}), 0); _temp5344.f3= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5352= yyvs; void** _temp5354= _temp5352.curr +
yyvsp_offset; if( _temp5352.base == 0? 1:( _temp5354 < _temp5352.base? 1:
_temp5354 >= _temp5352.last_plus_one)){ _throw( Null_Exception);}* _temp5354;}));
_temp5344;}); _temp5343;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5355= yyls; struct Cyc_Yyltype* _temp5357= _temp5355.curr +( yylsp_offset -
5); if( _temp5355.base == 0? 1:( _temp5357 < _temp5355.base? 1: _temp5357 >=
_temp5355.last_plus_one)){ _throw( Null_Exception);}* _temp5357;}).first_line,({
struct _tagged_ptr3 _temp5358= yyls; struct Cyc_Yyltype* _temp5360= _temp5358.curr
+ yylsp_offset; if( _temp5358.base == 0? 1:( _temp5360 < _temp5358.base? 1:
_temp5360 >= _temp5358.last_plus_one)){ _throw( Null_Exception);}* _temp5360;}).last_line));
_temp5342;}); _temp5341;}); break;} case 220: _LL5327: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5369= yyvs; void** _temp5371=
_temp5369.curr +( yyvsp_offset - 1); if( _temp5369.base == 0? 1:( _temp5371 <
_temp5369.base? 1: _temp5371 >= _temp5369.last_plus_one)){ _throw(
Null_Exception);}* _temp5371;})),( struct _tagged_string)({ char* _temp5372=(
char*)"H"; struct _tagged_string _temp5373; _temp5373.curr= _temp5372; _temp5373.base=
_temp5372; _temp5373.last_plus_one= _temp5372 + 2; _temp5373;})) == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp5374=( char*)"bad occurrence of heap region `H";
struct _tagged_string _temp5375; _temp5375.curr= _temp5374; _temp5375.base=
_temp5374; _temp5375.last_plus_one= _temp5374 + 33; _temp5375;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5376= yyls; struct Cyc_Yyltype* _temp5378= _temp5376.curr
+ yylsp_offset; if( _temp5376.base == 0? 1:( _temp5378 < _temp5376.base? 1:
_temp5378 >= _temp5376.last_plus_one)){ _throw( Null_Exception);}* _temp5378;}).first_line,({
struct _tagged_ptr3 _temp5379= yyls; struct Cyc_Yyltype* _temp5381= _temp5379.curr
+ yylsp_offset; if( _temp5379.base == 0? 1:( _temp5381 < _temp5379.base? 1:
_temp5381 >= _temp5379.last_plus_one)){ _throw( Null_Exception);}* _temp5381;}).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp5404=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp5404->name=({ struct
_tagged_string* _temp5405=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp5405[ 0]=({ struct _tagged_string _temp5409= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5406= yyvs; void** _temp5408= _temp5406.curr +(
yyvsp_offset - 1); if( _temp5406.base == 0? 1:( _temp5408 < _temp5406.base? 1:
_temp5408 >= _temp5406.last_plus_one)){ _throw( Null_Exception);}* _temp5408;}));
xprintf("`%.*s", _temp5409.last_plus_one - _temp5409.curr, _temp5409.curr);});
_temp5405;}); _temp5404->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp5404;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp5402=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp5402[ 0]=({ struct Cyc_Absyn_VarType_struct _temp5403; _temp5403.tag= Cyc_Absyn_VarType;
_temp5403.f1= tv; _temp5403;}); _temp5402;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5382=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5382[ 0]=({ struct Cyc_Stmt_tok_struct _temp5383; _temp5383.tag= Cyc_Stmt_tok;
_temp5383.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp5384=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp5384[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp5385; _temp5385.tag= Cyc_Absyn_Region_s;
_temp5385.f1= tv; _temp5385.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp5386=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5386->f1=(
void*) Cyc_Absyn_Loc_n; _temp5386->f2=({ struct _tagged_string* _temp5387=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp5387[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5388= yyvs; void** _temp5390=
_temp5388.curr +( yyvsp_offset - 1); if( _temp5388.base == 0? 1:( _temp5390 <
_temp5388.base? 1: _temp5390 >= _temp5388.last_plus_one)){ _throw(
Null_Exception);}* _temp5390;})); _temp5387;}); _temp5386;}),( void*)({ struct
Cyc_Absyn_RgnHandleType_struct* _temp5391=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp5391[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp5392; _temp5392.tag= Cyc_Absyn_RgnHandleType;
_temp5392.f1=( void*) t; _temp5392;}); _temp5391;}), 0); _temp5385.f3= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5393= yyvs; void** _temp5395= _temp5393.curr +
yyvsp_offset; if( _temp5393.base == 0? 1:( _temp5395 < _temp5393.base? 1:
_temp5395 >= _temp5393.last_plus_one)){ _throw( Null_Exception);}* _temp5395;}));
_temp5385;}); _temp5384;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5396= yyls; struct Cyc_Yyltype* _temp5398= _temp5396.curr +( yylsp_offset -
2); if( _temp5396.base == 0? 1:( _temp5398 < _temp5396.base? 1: _temp5398 >=
_temp5396.last_plus_one)){ _throw( Null_Exception);}* _temp5398;}).first_line,({
struct _tagged_ptr3 _temp5399= yyls; struct Cyc_Yyltype* _temp5401= _temp5399.curr
+ yylsp_offset; if( _temp5399.base == 0? 1:( _temp5401 < _temp5399.base? 1:
_temp5401 >= _temp5399.last_plus_one)){ _throw( Null_Exception);}* _temp5401;}).last_line));
_temp5383;}); _temp5382;}); break;} case 221: _LL5368: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5411=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5411[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5412; _temp5412.tag= Cyc_Stmt_tok; _temp5412.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Cut_s_struct* _temp5413=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp5413[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp5414; _temp5414.tag= Cyc_Absyn_Cut_s; _temp5414.f1= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5415= yyvs; void** _temp5417= _temp5415.curr +
yyvsp_offset; if( _temp5415.base == 0? 1:( _temp5417 < _temp5415.base? 1:
_temp5417 >= _temp5415.last_plus_one)){ _throw( Null_Exception);}* _temp5417;}));
_temp5414;}); _temp5413;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5418= yyls; struct Cyc_Yyltype* _temp5420= _temp5418.curr +( yylsp_offset -
1); if( _temp5418.base == 0? 1:( _temp5420 < _temp5418.base? 1: _temp5420 >=
_temp5418.last_plus_one)){ _throw( Null_Exception);}* _temp5420;}).first_line,({
struct _tagged_ptr3 _temp5421= yyls; struct Cyc_Yyltype* _temp5423= _temp5421.curr
+ yylsp_offset; if( _temp5421.base == 0? 1:( _temp5423 < _temp5421.base? 1:
_temp5423 >= _temp5421.last_plus_one)){ _throw( Null_Exception);}* _temp5423;}).last_line));
_temp5412;}); _temp5411;}); break; case 222: _LL5410: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5425=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5425[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5426; _temp5426.tag= Cyc_Stmt_tok; _temp5426.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Splice_s_struct* _temp5427=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp5427[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp5428; _temp5428.tag= Cyc_Absyn_Splice_s;
_temp5428.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5429= yyvs; void**
_temp5431= _temp5429.curr + yyvsp_offset; if( _temp5429.base == 0? 1:( _temp5431
< _temp5429.base? 1: _temp5431 >= _temp5429.last_plus_one)){ _throw(
Null_Exception);}* _temp5431;})); _temp5428;}); _temp5427;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5432= yyls; struct Cyc_Yyltype* _temp5434= _temp5432.curr
+( yylsp_offset - 1); if( _temp5432.base == 0? 1:( _temp5434 < _temp5432.base? 1:
_temp5434 >= _temp5432.last_plus_one)){ _throw( Null_Exception);}* _temp5434;}).first_line,({
struct _tagged_ptr3 _temp5435= yyls; struct Cyc_Yyltype* _temp5437= _temp5435.curr
+ yylsp_offset; if( _temp5435.base == 0? 1:( _temp5437 < _temp5435.base? 1:
_temp5437 >= _temp5435.last_plus_one)){ _throw( Null_Exception);}* _temp5437;}).last_line));
_temp5426;}); _temp5425;}); break; case 223: _LL5424: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5439=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5439[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5440; _temp5440.tag= Cyc_Stmt_tok; _temp5440.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Label_s_struct* _temp5441=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp5441[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp5442; _temp5442.tag= Cyc_Absyn_Label_s; _temp5442.f1=({
struct _tagged_string* _temp5443=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5443[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5444= yyvs; void** _temp5446= _temp5444.curr +( yyvsp_offset -
2); if( _temp5444.base == 0? 1:( _temp5446 < _temp5444.base? 1: _temp5446 >=
_temp5444.last_plus_one)){ _throw( Null_Exception);}* _temp5446;})); _temp5443;});
_temp5442.f2= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5447= yyvs; void**
_temp5449= _temp5447.curr + yyvsp_offset; if( _temp5447.base == 0? 1:( _temp5449
< _temp5447.base? 1: _temp5449 >= _temp5447.last_plus_one)){ _throw(
Null_Exception);}* _temp5449;})); _temp5442;}); _temp5441;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5450= yyls; struct Cyc_Yyltype* _temp5452= _temp5450.curr
+( yylsp_offset - 2); if( _temp5450.base == 0? 1:( _temp5452 < _temp5450.base? 1:
_temp5452 >= _temp5450.last_plus_one)){ _throw( Null_Exception);}* _temp5452;}).first_line,({
struct _tagged_ptr3 _temp5453= yyls; struct Cyc_Yyltype* _temp5455= _temp5453.curr
+ yylsp_offset; if( _temp5453.base == 0? 1:( _temp5455 < _temp5453.base? 1:
_temp5455 >= _temp5453.last_plus_one)){ _throw( Null_Exception);}* _temp5455;}).last_line));
_temp5440;}); _temp5439;}); break; case 224: _LL5438: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5457=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5457[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5458; _temp5458.tag= Cyc_Stmt_tok; _temp5458.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5459= yyls; struct Cyc_Yyltype* _temp5461= _temp5459.curr
+ yylsp_offset; if( _temp5459.base == 0? 1:( _temp5461 < _temp5459.base? 1:
_temp5461 >= _temp5459.last_plus_one)){ _throw( Null_Exception);}* _temp5461;}).first_line,({
struct _tagged_ptr3 _temp5462= yyls; struct Cyc_Yyltype* _temp5464= _temp5462.curr
+ yylsp_offset; if( _temp5462.base == 0? 1:( _temp5464 < _temp5462.base? 1:
_temp5464 >= _temp5462.last_plus_one)){ _throw( Null_Exception);}* _temp5464;}).last_line));
_temp5458;}); _temp5457;}); break; case 225: _LL5456: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5466=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5466[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5467; _temp5467.tag= Cyc_Stmt_tok; _temp5467.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5468= yyvs; void** _temp5470= _temp5468.curr +(
yyvsp_offset - 1); if( _temp5468.base == 0? 1:( _temp5470 < _temp5468.base? 1:
_temp5470 >= _temp5468.last_plus_one)){ _throw( Null_Exception);}* _temp5470;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5471= yyls; struct Cyc_Yyltype*
_temp5473= _temp5471.curr +( yylsp_offset - 1); if( _temp5471.base == 0? 1:(
_temp5473 < _temp5471.base? 1: _temp5473 >= _temp5471.last_plus_one)){ _throw(
Null_Exception);}* _temp5473;}).first_line,({ struct _tagged_ptr3 _temp5474=
yyls; struct Cyc_Yyltype* _temp5476= _temp5474.curr + yylsp_offset; if(
_temp5474.base == 0? 1:( _temp5476 < _temp5474.base? 1: _temp5476 >= _temp5474.last_plus_one)){
_throw( Null_Exception);}* _temp5476;}).last_line)); _temp5467;}); _temp5466;});
break; case 226: _LL5465: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5478=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5478[ 0]=({ struct Cyc_Stmt_tok_struct _temp5479; _temp5479.tag= Cyc_Stmt_tok;
_temp5479.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5480= yyls; struct Cyc_Yyltype* _temp5482= _temp5480.curr +(
yylsp_offset - 1); if( _temp5480.base == 0? 1:( _temp5482 < _temp5480.base? 1:
_temp5482 >= _temp5480.last_plus_one)){ _throw( Null_Exception);}* _temp5482;}).first_line,({
struct _tagged_ptr3 _temp5483= yyls; struct Cyc_Yyltype* _temp5485= _temp5483.curr
+ yylsp_offset; if( _temp5483.base == 0? 1:( _temp5485 < _temp5483.base? 1:
_temp5485 >= _temp5483.last_plus_one)){ _throw( Null_Exception);}* _temp5485;}).last_line));
_temp5479;}); _temp5478;}); break; case 227: _LL5477: yyval=({ struct
_tagged_ptr2 _temp5487= yyvs; void** _temp5489= _temp5487.curr +( yyvsp_offset -
1); if( _temp5487.base == 0? 1:( _temp5489 < _temp5487.base? 1: _temp5489 >=
_temp5487.last_plus_one)){ _throw( Null_Exception);}* _temp5489;}); break; case
228: _LL5486:{ void* _temp5494= Cyc_yyget_BlockItem_tok(({ struct _tagged_ptr2
_temp5491= yyvs; void** _temp5493= _temp5491.curr + yyvsp_offset; if( _temp5491.base
== 0? 1:( _temp5493 < _temp5491.base? 1: _temp5493 >= _temp5491.last_plus_one)){
_throw( Null_Exception);}* _temp5493;})); struct Cyc_List_List* _temp5502;
struct Cyc_Absyn_Fndecl* _temp5504; struct Cyc_Absyn_Stmt* _temp5506; _LL5496:
if(*(( int*) _temp5494) == Cyc_Parse_TopDecls_bl){ _LL5503: _temp5502=( struct
Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*) _temp5494)->f1; goto
_LL5497;} else{ goto _LL5498;} _LL5498: if(*(( int*) _temp5494) == Cyc_Parse_FnDecl_bl){
_LL5505: _temp5504=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp5494)->f1; goto _LL5499;} else{ goto _LL5500;} _LL5500: if(*(( int*)
_temp5494) == Cyc_Parse_Stmt_bl){ _LL5507: _temp5506=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5494)->f1; goto _LL5501;} else{ goto
_LL5495;} _LL5497: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5508=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5508[ 0]=({ struct Cyc_Stmt_tok_struct _temp5509; _temp5509.tag= Cyc_Stmt_tok;
_temp5509.f1= Cyc_Parse_flatten_declarations( _temp5502, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5510= yyls; struct Cyc_Yyltype*
_temp5512= _temp5510.curr + yylsp_offset; if( _temp5510.base == 0? 1:( _temp5512
< _temp5510.base? 1: _temp5512 >= _temp5510.last_plus_one)){ _throw(
Null_Exception);}* _temp5512;}).first_line,({ struct _tagged_ptr3 _temp5513=
yyls; struct Cyc_Yyltype* _temp5515= _temp5513.curr + yylsp_offset; if(
_temp5513.base == 0? 1:( _temp5515 < _temp5513.base? 1: _temp5515 >= _temp5513.last_plus_one)){
_throw( Null_Exception);}* _temp5515;}).last_line))); _temp5509;}); _temp5508;});
goto _LL5495; _LL5499: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5516=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5516[ 0]=({ struct Cyc_Stmt_tok_struct _temp5517; _temp5517.tag= Cyc_Stmt_tok;
_temp5517.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5518=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5518[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5519; _temp5519.tag= Cyc_Absyn_Fn_d;
_temp5519.f1= _temp5504; _temp5519;}); _temp5518;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5520= yyls; struct Cyc_Yyltype* _temp5522= _temp5520.curr
+ yylsp_offset; if( _temp5520.base == 0? 1:( _temp5522 < _temp5520.base? 1:
_temp5522 >= _temp5520.last_plus_one)){ _throw( Null_Exception);}* _temp5522;}).first_line,({
struct _tagged_ptr3 _temp5523= yyls; struct Cyc_Yyltype* _temp5525= _temp5523.curr
+ yylsp_offset; if( _temp5523.base == 0? 1:( _temp5525 < _temp5523.base? 1:
_temp5525 >= _temp5523.last_plus_one)){ _throw( Null_Exception);}* _temp5525;}).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5526= yyls; struct Cyc_Yyltype* _temp5528= _temp5526.curr + yylsp_offset;
if( _temp5526.base == 0? 1:( _temp5528 < _temp5526.base? 1: _temp5528 >=
_temp5526.last_plus_one)){ _throw( Null_Exception);}* _temp5528;}).first_line,({
struct _tagged_ptr3 _temp5529= yyls; struct Cyc_Yyltype* _temp5531= _temp5529.curr
+ yylsp_offset; if( _temp5529.base == 0? 1:( _temp5531 < _temp5529.base? 1:
_temp5531 >= _temp5529.last_plus_one)){ _throw( Null_Exception);}* _temp5531;}).last_line)));
_temp5517;}); _temp5516;}); goto _LL5495; _LL5501: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5532=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5532[ 0]=({ struct Cyc_Stmt_tok_struct _temp5533; _temp5533.tag= Cyc_Stmt_tok;
_temp5533.f1= _temp5506; _temp5533;}); _temp5532;}); goto _LL5495; _LL5495:;}
break; case 229: _LL5490:{ void* _temp5538= Cyc_yyget_BlockItem_tok(({ struct
_tagged_ptr2 _temp5535= yyvs; void** _temp5537= _temp5535.curr +( yyvsp_offset -
1); if( _temp5535.base == 0? 1:( _temp5537 < _temp5535.base? 1: _temp5537 >=
_temp5535.last_plus_one)){ _throw( Null_Exception);}* _temp5537;})); struct Cyc_List_List*
_temp5546; struct Cyc_Absyn_Fndecl* _temp5548; struct Cyc_Absyn_Stmt* _temp5550;
_LL5540: if(*(( int*) _temp5538) == Cyc_Parse_TopDecls_bl){ _LL5547: _temp5546=(
struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*) _temp5538)->f1;
goto _LL5541;} else{ goto _LL5542;} _LL5542: if(*(( int*) _temp5538) == Cyc_Parse_FnDecl_bl){
_LL5549: _temp5548=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp5538)->f1; goto _LL5543;} else{ goto _LL5544;} _LL5544: if(*(( int*)
_temp5538) == Cyc_Parse_Stmt_bl){ _LL5551: _temp5550=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp5538)->f1; goto _LL5545;} else{ goto
_LL5539;} _LL5541: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5552=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5552[ 0]=({ struct Cyc_Stmt_tok_struct _temp5553; _temp5553.tag= Cyc_Stmt_tok;
_temp5553.f1= Cyc_Parse_flatten_declarations( _temp5546, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5554= yyvs; void** _temp5556= _temp5554.curr +
yyvsp_offset; if( _temp5554.base == 0? 1:( _temp5556 < _temp5554.base? 1:
_temp5556 >= _temp5554.last_plus_one)){ _throw( Null_Exception);}* _temp5556;})));
_temp5553;}); _temp5552;}); goto _LL5539; _LL5543: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5557=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5557[ 0]=({ struct Cyc_Stmt_tok_struct _temp5558; _temp5558.tag= Cyc_Stmt_tok;
_temp5558.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp5559=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp5559[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp5560; _temp5560.tag= Cyc_Absyn_Fn_d;
_temp5560.f1= _temp5548; _temp5560;}); _temp5559;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5561= yyls; struct Cyc_Yyltype* _temp5563= _temp5561.curr
+( yylsp_offset - 1); if( _temp5561.base == 0? 1:( _temp5563 < _temp5561.base? 1:
_temp5563 >= _temp5561.last_plus_one)){ _throw( Null_Exception);}* _temp5563;}).first_line,({
struct _tagged_ptr3 _temp5564= yyls; struct Cyc_Yyltype* _temp5566= _temp5564.curr
+( yylsp_offset - 1); if( _temp5564.base == 0? 1:( _temp5566 < _temp5564.base? 1:
_temp5566 >= _temp5564.last_plus_one)){ _throw( Null_Exception);}* _temp5566;}).last_line)),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5567= yyvs; void** _temp5569=
_temp5567.curr + yyvsp_offset; if( _temp5567.base == 0? 1:( _temp5569 <
_temp5567.base? 1: _temp5569 >= _temp5567.last_plus_one)){ _throw(
Null_Exception);}* _temp5569;}))); _temp5558;}); _temp5557;}); goto _LL5539;
_LL5545: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5570=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5570[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5571; _temp5571.tag= Cyc_Stmt_tok; _temp5571.f1= Cyc_Absyn_seq_stmt(
_temp5550, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5572= yyvs; void**
_temp5574= _temp5572.curr + yyvsp_offset; if( _temp5572.base == 0? 1:( _temp5574
< _temp5572.base? 1: _temp5574 >= _temp5572.last_plus_one)){ _throw(
Null_Exception);}* _temp5574;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5575= yyls; struct Cyc_Yyltype* _temp5577= _temp5575.curr +(
yylsp_offset - 1); if( _temp5575.base == 0? 1:( _temp5577 < _temp5575.base? 1:
_temp5577 >= _temp5575.last_plus_one)){ _throw( Null_Exception);}* _temp5577;}).first_line,({
struct _tagged_ptr3 _temp5578= yyls; struct Cyc_Yyltype* _temp5580= _temp5578.curr
+ yylsp_offset; if( _temp5578.base == 0? 1:( _temp5580 < _temp5578.base? 1:
_temp5580 >= _temp5578.last_plus_one)){ _throw( Null_Exception);}* _temp5580;}).last_line));
_temp5571;}); _temp5570;}); goto _LL5539; _LL5539:;} break; case 230: _LL5534:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp5582=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp5582[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp5583; _temp5583.tag= Cyc_BlockItem_tok; _temp5583.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp5584=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp5584[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp5585; _temp5585.tag= Cyc_Parse_TopDecls_bl;
_temp5585.f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5586= yyvs;
void** _temp5588= _temp5586.curr + yyvsp_offset; if( _temp5586.base == 0? 1:(
_temp5588 < _temp5586.base? 1: _temp5588 >= _temp5586.last_plus_one)){ _throw(
Null_Exception);}* _temp5588;})); _temp5585;}); _temp5584;})); _temp5583;});
_temp5582;}); break; case 231: _LL5581: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp5590=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5590[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5591; _temp5591.tag= Cyc_BlockItem_tok;
_temp5591.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp5592=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp5592[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp5593; _temp5593.tag= Cyc_Parse_Stmt_bl;
_temp5593.f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5594= yyvs; void**
_temp5596= _temp5594.curr + yyvsp_offset; if( _temp5594.base == 0? 1:( _temp5596
< _temp5594.base? 1: _temp5596 >= _temp5594.last_plus_one)){ _throw(
Null_Exception);}* _temp5596;})); _temp5593;}); _temp5592;})); _temp5591;});
_temp5590;}); break; case 232: _LL5589: yyval=( void*)({ struct Cyc_BlockItem_tok_struct*
_temp5598=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp5598[ 0]=({ struct Cyc_BlockItem_tok_struct _temp5599; _temp5599.tag= Cyc_BlockItem_tok;
_temp5599.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp5600=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp5600[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp5601; _temp5601.tag= Cyc_Parse_FnDecl_bl;
_temp5601.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp5602= yyvs; void**
_temp5604= _temp5602.curr + yyvsp_offset; if( _temp5602.base == 0? 1:( _temp5604
< _temp5602.base? 1: _temp5604 >= _temp5602.last_plus_one)){ _throw(
Null_Exception);}* _temp5604;})); _temp5601;}); _temp5600;})); _temp5599;});
_temp5598;}); break; case 233: _LL5597: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp5606=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5606[ 0]=({ struct Cyc_Stmt_tok_struct _temp5607; _temp5607.tag= Cyc_Stmt_tok;
_temp5607.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5608= yyvs; void** _temp5610= _temp5608.curr +( yyvsp_offset -
2); if( _temp5608.base == 0? 1:( _temp5610 < _temp5608.base? 1: _temp5610 >=
_temp5608.last_plus_one)){ _throw( Null_Exception);}* _temp5610;})), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5611= yyvs; void** _temp5613= _temp5611.curr +
yyvsp_offset; if( _temp5611.base == 0? 1:( _temp5613 < _temp5611.base? 1:
_temp5613 >= _temp5611.last_plus_one)){ _throw( Null_Exception);}* _temp5613;})),
Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5614= yyls; struct Cyc_Yyltype* _temp5616= _temp5614.curr +( yylsp_offset -
4); if( _temp5614.base == 0? 1:( _temp5616 < _temp5614.base? 1: _temp5616 >=
_temp5614.last_plus_one)){ _throw( Null_Exception);}* _temp5616;}).first_line,({
struct _tagged_ptr3 _temp5617= yyls; struct Cyc_Yyltype* _temp5619= _temp5617.curr
+ yylsp_offset; if( _temp5617.base == 0? 1:( _temp5619 < _temp5617.base? 1:
_temp5619 >= _temp5617.last_plus_one)){ _throw( Null_Exception);}* _temp5619;}).last_line));
_temp5607;}); _temp5606;}); break; case 234: _LL5605: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5621=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5621[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5622; _temp5622.tag= Cyc_Stmt_tok; _temp5622.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5623= yyvs; void** _temp5625=
_temp5623.curr +( yyvsp_offset - 4); if( _temp5623.base == 0? 1:( _temp5625 <
_temp5623.base? 1: _temp5625 >= _temp5623.last_plus_one)){ _throw(
Null_Exception);}* _temp5625;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5626= yyvs; void** _temp5628= _temp5626.curr +( yyvsp_offset - 2); if(
_temp5626.base == 0? 1:( _temp5628 < _temp5626.base? 1: _temp5628 >= _temp5626.last_plus_one)){
_throw( Null_Exception);}* _temp5628;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5629= yyvs; void** _temp5631= _temp5629.curr + yyvsp_offset;
if( _temp5629.base == 0? 1:( _temp5631 < _temp5629.base? 1: _temp5631 >=
_temp5629.last_plus_one)){ _throw( Null_Exception);}* _temp5631;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5632= yyls; struct Cyc_Yyltype* _temp5634= _temp5632.curr
+( yylsp_offset - 6); if( _temp5632.base == 0? 1:( _temp5634 < _temp5632.base? 1:
_temp5634 >= _temp5632.last_plus_one)){ _throw( Null_Exception);}* _temp5634;}).first_line,({
struct _tagged_ptr3 _temp5635= yyls; struct Cyc_Yyltype* _temp5637= _temp5635.curr
+ yylsp_offset; if( _temp5635.base == 0? 1:( _temp5637 < _temp5635.base? 1:
_temp5637 >= _temp5635.last_plus_one)){ _throw( Null_Exception);}* _temp5637;}).last_line));
_temp5622;}); _temp5621;}); break; case 235: _LL5620: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5639=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5639[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5640; _temp5640.tag= Cyc_Stmt_tok; _temp5640.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5641= yyvs; void** _temp5643= _temp5641.curr +(
yyvsp_offset - 4); if( _temp5641.base == 0? 1:( _temp5643 < _temp5641.base? 1:
_temp5643 >= _temp5641.last_plus_one)){ _throw( Null_Exception);}* _temp5643;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp5644= yyvs; void**
_temp5646= _temp5644.curr +( yyvsp_offset - 1); if( _temp5644.base == 0? 1:(
_temp5646 < _temp5644.base? 1: _temp5646 >= _temp5644.last_plus_one)){ _throw(
Null_Exception);}* _temp5646;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5647= yyls; struct Cyc_Yyltype* _temp5649= _temp5647.curr +(
yylsp_offset - 6); if( _temp5647.base == 0? 1:( _temp5649 < _temp5647.base? 1:
_temp5649 >= _temp5647.last_plus_one)){ _throw( Null_Exception);}* _temp5649;}).first_line,({
struct _tagged_ptr3 _temp5650= yyls; struct Cyc_Yyltype* _temp5652= _temp5650.curr
+ yylsp_offset; if( _temp5650.base == 0? 1:( _temp5652 < _temp5650.base? 1:
_temp5652 >= _temp5650.last_plus_one)){ _throw( Null_Exception);}* _temp5652;}).last_line));
_temp5640;}); _temp5639;}); break; case 236: _LL5638: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5654=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5654[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5655; _temp5655.tag= Cyc_Stmt_tok; _temp5655.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5656= yyvs; void** _temp5658=
_temp5656.curr +( yyvsp_offset - 4); if( _temp5656.base == 0? 1:( _temp5658 <
_temp5656.base? 1: _temp5658 >= _temp5656.last_plus_one)){ _throw(
Null_Exception);}* _temp5658;})), Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5659= yyvs; void** _temp5661= _temp5659.curr +( yyvsp_offset -
1); if( _temp5659.base == 0? 1:( _temp5661 < _temp5659.base? 1: _temp5661 >=
_temp5659.last_plus_one)){ _throw( Null_Exception);}* _temp5661;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5662= yyls; struct Cyc_Yyltype* _temp5664= _temp5662.curr
+( yylsp_offset - 5); if( _temp5662.base == 0? 1:( _temp5664 < _temp5662.base? 1:
_temp5664 >= _temp5662.last_plus_one)){ _throw( Null_Exception);}* _temp5664;}).first_line,({
struct _tagged_ptr3 _temp5665= yyls; struct Cyc_Yyltype* _temp5667= _temp5665.curr
+ yylsp_offset; if( _temp5665.base == 0? 1:( _temp5667 < _temp5665.base? 1:
_temp5667 >= _temp5665.last_plus_one)){ _throw( Null_Exception);}* _temp5667;}).last_line));
_temp5655;}); _temp5654;}); break; case 237: _LL5653: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp5669=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5669[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5670; _temp5670.tag= Cyc_SwitchClauseList_tok;
_temp5670.f1= 0; _temp5670;}); _temp5669;}); break; case 238: _LL5668: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp5672=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5672[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5673; _temp5673.tag= Cyc_SwitchClauseList_tok;
_temp5673.f1=({ struct Cyc_List_List* _temp5674=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5674->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp5675=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp5675->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5685= yyls; struct Cyc_Yyltype* _temp5687= _temp5685.curr
+( yylsp_offset - 2); if( _temp5685.base == 0? 1:( _temp5687 < _temp5685.base? 1:
_temp5687 >= _temp5685.last_plus_one)){ _throw( Null_Exception);}* _temp5687;}).first_line,({
struct _tagged_ptr3 _temp5688= yyls; struct Cyc_Yyltype* _temp5690= _temp5688.curr
+( yylsp_offset - 2); if( _temp5688.base == 0? 1:( _temp5690 < _temp5688.base? 1:
_temp5690 >= _temp5688.last_plus_one)){ _throw( Null_Exception);}* _temp5690;}).last_line));
_temp5675->pat_vars= 0; _temp5675->where_clause= 0; _temp5675->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5682= yyvs; void** _temp5684= _temp5682.curr +
yyvsp_offset; if( _temp5682.base == 0? 1:( _temp5684 < _temp5682.base? 1:
_temp5684 >= _temp5682.last_plus_one)){ _throw( Null_Exception);}* _temp5684;}));
_temp5675->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5676=
yyls; struct Cyc_Yyltype* _temp5678= _temp5676.curr +( yylsp_offset - 2); if(
_temp5676.base == 0? 1:( _temp5678 < _temp5676.base? 1: _temp5678 >= _temp5676.last_plus_one)){
_throw( Null_Exception);}* _temp5678;}).first_line,({ struct _tagged_ptr3
_temp5679= yyls; struct Cyc_Yyltype* _temp5681= _temp5679.curr + yylsp_offset;
if( _temp5679.base == 0? 1:( _temp5681 < _temp5679.base? 1: _temp5681 >=
_temp5679.last_plus_one)){ _throw( Null_Exception);}* _temp5681;}).last_line);
_temp5675;}); _temp5674->tl= 0; _temp5674;}); _temp5673;}); _temp5672;}); break;
case 239: _LL5671: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp5692=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp5692[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp5693; _temp5693.tag= Cyc_SwitchClauseList_tok; _temp5693.f1=({ struct Cyc_List_List*
_temp5694=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5694->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp5698=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp5698->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5711= yyvs; void** _temp5713= _temp5711.curr +(
yyvsp_offset - 2); if( _temp5711.base == 0? 1:( _temp5713 < _temp5711.base? 1:
_temp5713 >= _temp5711.last_plus_one)){ _throw( Null_Exception);}* _temp5713;}));
_temp5698->pat_vars= 0; _temp5698->where_clause= 0; _temp5698->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5705= yyls; struct Cyc_Yyltype*
_temp5707= _temp5705.curr +( yylsp_offset - 1); if( _temp5705.base == 0? 1:(
_temp5707 < _temp5705.base? 1: _temp5707 >= _temp5705.last_plus_one)){ _throw(
Null_Exception);}* _temp5707;}).first_line,({ struct _tagged_ptr3 _temp5708=
yyls; struct Cyc_Yyltype* _temp5710= _temp5708.curr +( yylsp_offset - 1); if(
_temp5708.base == 0? 1:( _temp5710 < _temp5708.base? 1: _temp5710 >= _temp5708.last_plus_one)){
_throw( Null_Exception);}* _temp5710;}).last_line)); _temp5698->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5699= yyls; struct Cyc_Yyltype* _temp5701= _temp5699.curr
+( yylsp_offset - 3); if( _temp5699.base == 0? 1:( _temp5701 < _temp5699.base? 1:
_temp5701 >= _temp5699.last_plus_one)){ _throw( Null_Exception);}* _temp5701;}).first_line,({
struct _tagged_ptr3 _temp5702= yyls; struct Cyc_Yyltype* _temp5704= _temp5702.curr
+ yylsp_offset; if( _temp5702.base == 0? 1:( _temp5704 < _temp5702.base? 1:
_temp5704 >= _temp5702.last_plus_one)){ _throw( Null_Exception);}* _temp5704;}).last_line);
_temp5698;}); _temp5694->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5695= yyvs; void** _temp5697= _temp5695.curr + yyvsp_offset;
if( _temp5695.base == 0? 1:( _temp5697 < _temp5695.base? 1: _temp5697 >=
_temp5695.last_plus_one)){ _throw( Null_Exception);}* _temp5697;})); _temp5694;});
_temp5693;}); _temp5692;}); break; case 240: _LL5691: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp5715=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5715[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5716; _temp5716.tag= Cyc_SwitchClauseList_tok;
_temp5716.f1=({ struct Cyc_List_List* _temp5717=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5717->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp5721=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp5721->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5731= yyvs;
void** _temp5733= _temp5731.curr +( yyvsp_offset - 3); if( _temp5731.base == 0?
1:( _temp5733 < _temp5731.base? 1: _temp5733 >= _temp5731.last_plus_one)){
_throw( Null_Exception);}* _temp5733;})); _temp5721->pat_vars= 0; _temp5721->where_clause=
0; _temp5721->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5728= yyvs;
void** _temp5730= _temp5728.curr +( yyvsp_offset - 1); if( _temp5728.base == 0?
1:( _temp5730 < _temp5728.base? 1: _temp5730 >= _temp5728.last_plus_one)){
_throw( Null_Exception);}* _temp5730;})); _temp5721->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5722= yyls; struct Cyc_Yyltype* _temp5724= _temp5722.curr
+( yylsp_offset - 4); if( _temp5722.base == 0? 1:( _temp5724 < _temp5722.base? 1:
_temp5724 >= _temp5722.last_plus_one)){ _throw( Null_Exception);}* _temp5724;}).first_line,({
struct _tagged_ptr3 _temp5725= yyls; struct Cyc_Yyltype* _temp5727= _temp5725.curr
+( yylsp_offset - 1); if( _temp5725.base == 0? 1:( _temp5727 < _temp5725.base? 1:
_temp5727 >= _temp5725.last_plus_one)){ _throw( Null_Exception);}* _temp5727;}).last_line);
_temp5721;}); _temp5717->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5718= yyvs; void** _temp5720= _temp5718.curr + yyvsp_offset;
if( _temp5718.base == 0? 1:( _temp5720 < _temp5718.base? 1: _temp5720 >=
_temp5718.last_plus_one)){ _throw( Null_Exception);}* _temp5720;})); _temp5717;});
_temp5716;}); _temp5715;}); break; case 241: _LL5714: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp5735=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5735[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5736; _temp5736.tag= Cyc_SwitchClauseList_tok;
_temp5736.f1=({ struct Cyc_List_List* _temp5737=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5737->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp5741=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp5741->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5757= yyvs;
void** _temp5759= _temp5757.curr +( yyvsp_offset - 4); if( _temp5757.base == 0?
1:( _temp5759 < _temp5757.base? 1: _temp5759 >= _temp5757.last_plus_one)){
_throw( Null_Exception);}* _temp5759;})); _temp5741->pat_vars= 0; _temp5741->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5754= yyvs;
void** _temp5756= _temp5754.curr +( yyvsp_offset - 2); if( _temp5754.base == 0?
1:( _temp5756 < _temp5754.base? 1: _temp5756 >= _temp5754.last_plus_one)){
_throw( Null_Exception);}* _temp5756;})); _temp5741->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5748= yyls; struct Cyc_Yyltype*
_temp5750= _temp5748.curr +( yylsp_offset - 1); if( _temp5748.base == 0? 1:(
_temp5750 < _temp5748.base? 1: _temp5750 >= _temp5748.last_plus_one)){ _throw(
Null_Exception);}* _temp5750;}).first_line,({ struct _tagged_ptr3 _temp5751=
yyls; struct Cyc_Yyltype* _temp5753= _temp5751.curr +( yylsp_offset - 1); if(
_temp5751.base == 0? 1:( _temp5753 < _temp5751.base? 1: _temp5753 >= _temp5751.last_plus_one)){
_throw( Null_Exception);}* _temp5753;}).last_line)); _temp5741->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5742= yyls; struct Cyc_Yyltype* _temp5744= _temp5742.curr
+( yylsp_offset - 5); if( _temp5742.base == 0? 1:( _temp5744 < _temp5742.base? 1:
_temp5744 >= _temp5742.last_plus_one)){ _throw( Null_Exception);}* _temp5744;}).first_line,({
struct _tagged_ptr3 _temp5745= yyls; struct Cyc_Yyltype* _temp5747= _temp5745.curr
+ yylsp_offset; if( _temp5745.base == 0? 1:( _temp5747 < _temp5745.base? 1:
_temp5747 >= _temp5745.last_plus_one)){ _throw( Null_Exception);}* _temp5747;}).last_line);
_temp5741;}); _temp5737->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5738= yyvs; void** _temp5740= _temp5738.curr + yyvsp_offset;
if( _temp5738.base == 0? 1:( _temp5740 < _temp5738.base? 1: _temp5740 >=
_temp5738.last_plus_one)){ _throw( Null_Exception);}* _temp5740;})); _temp5737;});
_temp5736;}); _temp5735;}); break; case 242: _LL5734: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp5761=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp5761[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp5762; _temp5762.tag= Cyc_SwitchClauseList_tok;
_temp5762.f1=({ struct Cyc_List_List* _temp5763=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5763->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp5767=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp5767->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5780= yyvs;
void** _temp5782= _temp5780.curr +( yyvsp_offset - 5); if( _temp5780.base == 0?
1:( _temp5782 < _temp5780.base? 1: _temp5782 >= _temp5780.last_plus_one)){
_throw( Null_Exception);}* _temp5782;})); _temp5767->pat_vars= 0; _temp5767->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5777= yyvs;
void** _temp5779= _temp5777.curr +( yyvsp_offset - 3); if( _temp5777.base == 0?
1:( _temp5779 < _temp5777.base? 1: _temp5779 >= _temp5777.last_plus_one)){
_throw( Null_Exception);}* _temp5779;})); _temp5767->body= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5774= yyvs; void** _temp5776= _temp5774.curr +(
yyvsp_offset - 1); if( _temp5774.base == 0? 1:( _temp5776 < _temp5774.base? 1:
_temp5776 >= _temp5774.last_plus_one)){ _throw( Null_Exception);}* _temp5776;}));
_temp5767->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5768=
yyls; struct Cyc_Yyltype* _temp5770= _temp5768.curr +( yylsp_offset - 6); if(
_temp5768.base == 0? 1:( _temp5770 < _temp5768.base? 1: _temp5770 >= _temp5768.last_plus_one)){
_throw( Null_Exception);}* _temp5770;}).first_line,({ struct _tagged_ptr3
_temp5771= yyls; struct Cyc_Yyltype* _temp5773= _temp5771.curr + yylsp_offset;
if( _temp5771.base == 0? 1:( _temp5773 < _temp5771.base? 1: _temp5773 >=
_temp5771.last_plus_one)){ _throw( Null_Exception);}* _temp5773;}).last_line);
_temp5767;}); _temp5763->tl= Cyc_yyget_SwitchClauseList_tok(({ struct
_tagged_ptr2 _temp5764= yyvs; void** _temp5766= _temp5764.curr + yyvsp_offset;
if( _temp5764.base == 0? 1:( _temp5766 < _temp5764.base? 1: _temp5766 >=
_temp5764.last_plus_one)){ _throw( Null_Exception);}* _temp5766;})); _temp5763;});
_temp5762;}); _temp5761;}); break; case 243: _LL5760: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5784=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5784[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5785; _temp5785.tag= Cyc_Stmt_tok; _temp5785.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5786= yyvs; void** _temp5788= _temp5786.curr +(
yyvsp_offset - 2); if( _temp5786.base == 0? 1:( _temp5788 < _temp5786.base? 1:
_temp5788 >= _temp5786.last_plus_one)){ _throw( Null_Exception);}* _temp5788;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5789= yyvs; void** _temp5791=
_temp5789.curr + yyvsp_offset; if( _temp5789.base == 0? 1:( _temp5791 <
_temp5789.base? 1: _temp5791 >= _temp5789.last_plus_one)){ _throw(
Null_Exception);}* _temp5791;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5792= yyls; struct Cyc_Yyltype* _temp5794= _temp5792.curr +(
yylsp_offset - 4); if( _temp5792.base == 0? 1:( _temp5794 < _temp5792.base? 1:
_temp5794 >= _temp5792.last_plus_one)){ _throw( Null_Exception);}* _temp5794;}).first_line,({
struct _tagged_ptr3 _temp5795= yyls; struct Cyc_Yyltype* _temp5797= _temp5795.curr
+ yylsp_offset; if( _temp5795.base == 0? 1:( _temp5797 < _temp5795.base? 1:
_temp5797 >= _temp5795.last_plus_one)){ _throw( Null_Exception);}* _temp5797;}).last_line));
_temp5785;}); _temp5784;}); break; case 244: _LL5783: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5799=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5799[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5800; _temp5800.tag= Cyc_Stmt_tok; _temp5800.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5801= yyvs; void** _temp5803= _temp5801.curr +(
yyvsp_offset - 5); if( _temp5801.base == 0? 1:( _temp5803 < _temp5801.base? 1:
_temp5803 >= _temp5801.last_plus_one)){ _throw( Null_Exception);}* _temp5803;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5804= yyvs; void** _temp5806=
_temp5804.curr +( yyvsp_offset - 2); if( _temp5804.base == 0? 1:( _temp5806 <
_temp5804.base? 1: _temp5806 >= _temp5804.last_plus_one)){ _throw(
Null_Exception);}* _temp5806;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5807= yyls; struct Cyc_Yyltype* _temp5809= _temp5807.curr +(
yylsp_offset - 6); if( _temp5807.base == 0? 1:( _temp5809 < _temp5807.base? 1:
_temp5809 >= _temp5807.last_plus_one)){ _throw( Null_Exception);}* _temp5809;}).first_line,({
struct _tagged_ptr3 _temp5810= yyls; struct Cyc_Yyltype* _temp5812= _temp5810.curr
+ yylsp_offset; if( _temp5810.base == 0? 1:( _temp5812 < _temp5810.base? 1:
_temp5812 >= _temp5810.last_plus_one)){ _throw( Null_Exception);}* _temp5812;}).last_line));
_temp5800;}); _temp5799;}); break; case 245: _LL5798: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5814=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5814[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5815; _temp5815.tag= Cyc_Stmt_tok; _temp5815.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5816= yyvs; void** _temp5818= _temp5816.curr +
yyvsp_offset; if( _temp5816.base == 0? 1:( _temp5818 < _temp5816.base? 1:
_temp5818 >= _temp5816.last_plus_one)){ _throw( Null_Exception);}* _temp5818;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5819= yyls; struct Cyc_Yyltype*
_temp5821= _temp5819.curr +( yylsp_offset - 5); if( _temp5819.base == 0? 1:(
_temp5821 < _temp5819.base? 1: _temp5821 >= _temp5819.last_plus_one)){ _throw(
Null_Exception);}* _temp5821;}).first_line,({ struct _tagged_ptr3 _temp5822=
yyls; struct Cyc_Yyltype* _temp5824= _temp5822.curr + yylsp_offset; if(
_temp5822.base == 0? 1:( _temp5824 < _temp5822.base? 1: _temp5824 >= _temp5822.last_plus_one)){
_throw( Null_Exception);}* _temp5824;}).last_line)); _temp5815;}); _temp5814;});
break; case 246: _LL5813: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5826=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5826[ 0]=({ struct Cyc_Stmt_tok_struct _temp5827; _temp5827.tag= Cyc_Stmt_tok;
_temp5827.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5828= yyvs; void** _temp5830=
_temp5828.curr +( yyvsp_offset - 2); if( _temp5828.base == 0? 1:( _temp5830 <
_temp5828.base? 1: _temp5830 >= _temp5828.last_plus_one)){ _throw(
Null_Exception);}* _temp5830;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5831= yyvs; void** _temp5833= _temp5831.curr + yyvsp_offset; if( _temp5831.base
== 0? 1:( _temp5833 < _temp5831.base? 1: _temp5833 >= _temp5831.last_plus_one)){
_throw( Null_Exception);}* _temp5833;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5834= yyls; struct Cyc_Yyltype* _temp5836= _temp5834.curr +(
yylsp_offset - 6); if( _temp5834.base == 0? 1:( _temp5836 < _temp5834.base? 1:
_temp5836 >= _temp5834.last_plus_one)){ _throw( Null_Exception);}* _temp5836;}).first_line,({
struct _tagged_ptr3 _temp5837= yyls; struct Cyc_Yyltype* _temp5839= _temp5837.curr
+ yylsp_offset; if( _temp5837.base == 0? 1:( _temp5839 < _temp5837.base? 1:
_temp5839 >= _temp5837.last_plus_one)){ _throw( Null_Exception);}* _temp5839;}).last_line));
_temp5827;}); _temp5826;}); break; case 247: _LL5825: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5841=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5841[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5842; _temp5842.tag= Cyc_Stmt_tok; _temp5842.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5843= yyvs; void** _temp5845=
_temp5843.curr +( yyvsp_offset - 3); if( _temp5843.base == 0? 1:( _temp5845 <
_temp5843.base? 1: _temp5845 >= _temp5843.last_plus_one)){ _throw(
Null_Exception);}* _temp5845;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5846= yyvs; void** _temp5848= _temp5846.curr +
yyvsp_offset; if( _temp5846.base == 0? 1:( _temp5848 < _temp5846.base? 1:
_temp5848 >= _temp5846.last_plus_one)){ _throw( Null_Exception);}* _temp5848;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5849= yyls; struct Cyc_Yyltype*
_temp5851= _temp5849.curr +( yylsp_offset - 6); if( _temp5849.base == 0? 1:(
_temp5851 < _temp5849.base? 1: _temp5851 >= _temp5849.last_plus_one)){ _throw(
Null_Exception);}* _temp5851;}).first_line,({ struct _tagged_ptr3 _temp5852=
yyls; struct Cyc_Yyltype* _temp5854= _temp5852.curr + yylsp_offset; if(
_temp5852.base == 0? 1:( _temp5854 < _temp5852.base? 1: _temp5854 >= _temp5852.last_plus_one)){
_throw( Null_Exception);}* _temp5854;}).last_line)); _temp5842;}); _temp5841;});
break; case 248: _LL5840: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5856=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5856[ 0]=({ struct Cyc_Stmt_tok_struct _temp5857; _temp5857.tag= Cyc_Stmt_tok;
_temp5857.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5858= yyvs; void** _temp5860= _temp5858.curr +(
yyvsp_offset - 4); if( _temp5858.base == 0? 1:( _temp5860 < _temp5858.base? 1:
_temp5860 >= _temp5858.last_plus_one)){ _throw( Null_Exception);}* _temp5860;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5861= yyvs; void** _temp5863=
_temp5861.curr +( yyvsp_offset - 2); if( _temp5861.base == 0? 1:( _temp5863 <
_temp5861.base? 1: _temp5863 >= _temp5861.last_plus_one)){ _throw(
Null_Exception);}* _temp5863;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5864= yyvs; void** _temp5866= _temp5864.curr + yyvsp_offset; if( _temp5864.base
== 0? 1:( _temp5866 < _temp5864.base? 1: _temp5866 >= _temp5864.last_plus_one)){
_throw( Null_Exception);}* _temp5866;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5867= yyls; struct Cyc_Yyltype* _temp5869= _temp5867.curr +(
yylsp_offset - 7); if( _temp5867.base == 0? 1:( _temp5869 < _temp5867.base? 1:
_temp5869 >= _temp5867.last_plus_one)){ _throw( Null_Exception);}* _temp5869;}).first_line,({
struct _tagged_ptr3 _temp5870= yyls; struct Cyc_Yyltype* _temp5872= _temp5870.curr
+( yylsp_offset - 1); if( _temp5870.base == 0? 1:( _temp5872 < _temp5870.base? 1:
_temp5872 >= _temp5870.last_plus_one)){ _throw( Null_Exception);}* _temp5872;}).last_line));
_temp5857;}); _temp5856;}); break; case 249: _LL5855: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5874=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5874[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5875; _temp5875.tag= Cyc_Stmt_tok; _temp5875.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5876= yyvs; void** _temp5878= _temp5876.curr +(
yyvsp_offset - 4); if( _temp5876.base == 0? 1:( _temp5878 < _temp5876.base? 1:
_temp5878 >= _temp5876.last_plus_one)){ _throw( Null_Exception);}* _temp5878;})),
Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5879= yyvs; void** _temp5881= _temp5879.curr + yyvsp_offset;
if( _temp5879.base == 0? 1:( _temp5881 < _temp5879.base? 1: _temp5881 >=
_temp5879.last_plus_one)){ _throw( Null_Exception);}* _temp5881;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5882= yyls; struct Cyc_Yyltype* _temp5884= _temp5882.curr
+( yylsp_offset - 6); if( _temp5882.base == 0? 1:( _temp5884 < _temp5882.base? 1:
_temp5884 >= _temp5882.last_plus_one)){ _throw( Null_Exception);}* _temp5884;}).first_line,({
struct _tagged_ptr3 _temp5885= yyls; struct Cyc_Yyltype* _temp5887= _temp5885.curr
+ yylsp_offset; if( _temp5885.base == 0? 1:( _temp5887 < _temp5885.base? 1:
_temp5887 >= _temp5885.last_plus_one)){ _throw( Null_Exception);}* _temp5887;}).last_line));
_temp5875;}); _temp5874;}); break; case 250: _LL5873: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5889=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5889[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5890; _temp5890.tag= Cyc_Stmt_tok; _temp5890.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5891= yyvs; void** _temp5893= _temp5891.curr +(
yyvsp_offset - 5); if( _temp5891.base == 0? 1:( _temp5893 < _temp5891.base? 1:
_temp5893 >= _temp5891.last_plus_one)){ _throw( Null_Exception);}* _temp5893;})),
Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5894= yyvs;
void** _temp5896= _temp5894.curr +( yyvsp_offset - 2); if( _temp5894.base == 0?
1:( _temp5896 < _temp5894.base? 1: _temp5896 >= _temp5894.last_plus_one)){
_throw( Null_Exception);}* _temp5896;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5897= yyvs; void** _temp5899= _temp5897.curr + yyvsp_offset;
if( _temp5897.base == 0? 1:( _temp5899 < _temp5897.base? 1: _temp5899 >=
_temp5897.last_plus_one)){ _throw( Null_Exception);}* _temp5899;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5900= yyls; struct Cyc_Yyltype* _temp5902= _temp5900.curr
+( yylsp_offset - 7); if( _temp5900.base == 0? 1:( _temp5902 < _temp5900.base? 1:
_temp5902 >= _temp5900.last_plus_one)){ _throw( Null_Exception);}* _temp5902;}).first_line,({
struct _tagged_ptr3 _temp5903= yyls; struct Cyc_Yyltype* _temp5905= _temp5903.curr
+ yylsp_offset; if( _temp5903.base == 0? 1:( _temp5905 < _temp5903.base? 1:
_temp5905 >= _temp5903.last_plus_one)){ _throw( Null_Exception);}* _temp5905;}).last_line));
_temp5890;}); _temp5889;}); break; case 251: _LL5888: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp5907=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp5907[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5908; _temp5908.tag= Cyc_Stmt_tok; _temp5908.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5909= yyvs; void** _temp5911= _temp5909.curr +(
yyvsp_offset - 5); if( _temp5909.base == 0? 1:( _temp5911 < _temp5909.base? 1:
_temp5911 >= _temp5909.last_plus_one)){ _throw( Null_Exception);}* _temp5911;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5912= yyvs; void** _temp5914=
_temp5912.curr +( yyvsp_offset - 3); if( _temp5912.base == 0? 1:( _temp5914 <
_temp5912.base? 1: _temp5914 >= _temp5912.last_plus_one)){ _throw(
Null_Exception);}* _temp5914;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5915= yyvs; void** _temp5917= _temp5915.curr +
yyvsp_offset; if( _temp5915.base == 0? 1:( _temp5917 < _temp5915.base? 1:
_temp5917 >= _temp5915.last_plus_one)){ _throw( Null_Exception);}* _temp5917;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5918= yyls; struct Cyc_Yyltype*
_temp5920= _temp5918.curr +( yylsp_offset - 7); if( _temp5918.base == 0? 1:(
_temp5920 < _temp5918.base? 1: _temp5920 >= _temp5918.last_plus_one)){ _throw(
Null_Exception);}* _temp5920;}).first_line,({ struct _tagged_ptr3 _temp5921=
yyls; struct Cyc_Yyltype* _temp5923= _temp5921.curr + yylsp_offset; if(
_temp5921.base == 0? 1:( _temp5923 < _temp5921.base? 1: _temp5923 >= _temp5921.last_plus_one)){
_throw( Null_Exception);}* _temp5923;}).last_line)); _temp5908;}); _temp5907;});
break; case 252: _LL5906: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5925=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5925[ 0]=({ struct Cyc_Stmt_tok_struct _temp5926; _temp5926.tag= Cyc_Stmt_tok;
_temp5926.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5927= yyvs; void** _temp5929= _temp5927.curr +( yyvsp_offset - 6); if(
_temp5927.base == 0? 1:( _temp5929 < _temp5927.base? 1: _temp5929 >= _temp5927.last_plus_one)){
_throw( Null_Exception);}* _temp5929;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5930= yyvs; void** _temp5932= _temp5930.curr +( yyvsp_offset -
4); if( _temp5930.base == 0? 1:( _temp5932 < _temp5930.base? 1: _temp5932 >=
_temp5930.last_plus_one)){ _throw( Null_Exception);}* _temp5932;})), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5933= yyvs; void** _temp5935= _temp5933.curr +(
yyvsp_offset - 2); if( _temp5933.base == 0? 1:( _temp5935 < _temp5933.base? 1:
_temp5935 >= _temp5933.last_plus_one)){ _throw( Null_Exception);}* _temp5935;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5936= yyvs; void** _temp5938=
_temp5936.curr + yyvsp_offset; if( _temp5936.base == 0? 1:( _temp5938 <
_temp5936.base? 1: _temp5938 >= _temp5936.last_plus_one)){ _throw(
Null_Exception);}* _temp5938;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5939= yyls; struct Cyc_Yyltype* _temp5941= _temp5939.curr +(
yylsp_offset - 8); if( _temp5939.base == 0? 1:( _temp5941 < _temp5939.base? 1:
_temp5941 >= _temp5939.last_plus_one)){ _throw( Null_Exception);}* _temp5941;}).first_line,({
struct _tagged_ptr3 _temp5942= yyls; struct Cyc_Yyltype* _temp5944= _temp5942.curr
+ yylsp_offset; if( _temp5942.base == 0? 1:( _temp5944 < _temp5942.base? 1:
_temp5944 >= _temp5942.last_plus_one)){ _throw( Null_Exception);}* _temp5944;}).last_line));
_temp5926;}); _temp5925;}); break; case 253: _LL5924: { struct Cyc_List_List*
_temp5949= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp5946= yyvs; void**
_temp5948= _temp5946.curr +( yyvsp_offset - 3); if( _temp5946.base == 0? 1:(
_temp5948 < _temp5946.base? 1: _temp5948 >= _temp5946.last_plus_one)){ _throw(
Null_Exception);}* _temp5948;})); goto _LL5950; _LL5950: { struct Cyc_Absyn_Stmt*
_temp5960= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5951=
yyvs; void** _temp5953= _temp5951.curr + yyvsp_offset; if( _temp5951.base == 0?
1:( _temp5953 < _temp5951.base? 1: _temp5953 >= _temp5951.last_plus_one)){
_throw( Null_Exception);}* _temp5953;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5954= yyls; struct Cyc_Yyltype* _temp5956= _temp5954.curr +(
yylsp_offset - 5); if( _temp5954.base == 0? 1:( _temp5956 < _temp5954.base? 1:
_temp5956 >= _temp5954.last_plus_one)){ _throw( Null_Exception);}* _temp5956;}).first_line,({
struct _tagged_ptr3 _temp5957= yyls; struct Cyc_Yyltype* _temp5959= _temp5957.curr
+ yylsp_offset; if( _temp5957.base == 0? 1:( _temp5959 < _temp5957.base? 1:
_temp5959 >= _temp5957.last_plus_one)){ _throw( Null_Exception);}* _temp5959;}).last_line));
goto _LL5961; _LL5961: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5962=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5962[ 0]=({ struct Cyc_Stmt_tok_struct _temp5963; _temp5963.tag= Cyc_Stmt_tok;
_temp5963.f1= Cyc_Parse_flatten_declarations( _temp5949, _temp5960); _temp5963;});
_temp5962;}); break;}} case 254: _LL5945: { struct Cyc_List_List* _temp5968= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5965= yyvs; void** _temp5967= _temp5965.curr +(
yyvsp_offset - 4); if( _temp5965.base == 0? 1:( _temp5967 < _temp5965.base? 1:
_temp5967 >= _temp5965.last_plus_one)){ _throw( Null_Exception);}* _temp5967;}));
goto _LL5969; _LL5969: { struct Cyc_Absyn_Stmt* _temp5982= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5970=
yyvs; void** _temp5972= _temp5970.curr +( yyvsp_offset - 3); if( _temp5970.base
== 0? 1:( _temp5972 < _temp5970.base? 1: _temp5972 >= _temp5970.last_plus_one)){
_throw( Null_Exception);}* _temp5972;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5973= yyvs; void** _temp5975= _temp5973.curr +
yyvsp_offset; if( _temp5973.base == 0? 1:( _temp5975 < _temp5973.base? 1:
_temp5975 >= _temp5973.last_plus_one)){ _throw( Null_Exception);}* _temp5975;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5976= yyls; struct Cyc_Yyltype*
_temp5978= _temp5976.curr +( yylsp_offset - 6); if( _temp5976.base == 0? 1:(
_temp5978 < _temp5976.base? 1: _temp5978 >= _temp5976.last_plus_one)){ _throw(
Null_Exception);}* _temp5978;}).first_line,({ struct _tagged_ptr3 _temp5979=
yyls; struct Cyc_Yyltype* _temp5981= _temp5979.curr + yylsp_offset; if(
_temp5979.base == 0? 1:( _temp5981 < _temp5979.base? 1: _temp5981 >= _temp5979.last_plus_one)){
_throw( Null_Exception);}* _temp5981;}).last_line)); goto _LL5983; _LL5983:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp5984=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5984[ 0]=({ struct Cyc_Stmt_tok_struct
_temp5985; _temp5985.tag= Cyc_Stmt_tok; _temp5985.f1= Cyc_Parse_flatten_declarations(
_temp5968, _temp5982); _temp5985;}); _temp5984;}); break;}} case 255: _LL5964: {
struct Cyc_List_List* _temp5990= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp5987= yyvs; void** _temp5989= _temp5987.curr +( yyvsp_offset - 4); if(
_temp5987.base == 0? 1:( _temp5989 < _temp5987.base? 1: _temp5989 >= _temp5987.last_plus_one)){
_throw( Null_Exception);}* _temp5989;})); goto _LL5991; _LL5991: { struct Cyc_Absyn_Stmt*
_temp6004= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5992= yyvs; void** _temp5994=
_temp5992.curr +( yyvsp_offset - 2); if( _temp5992.base == 0? 1:( _temp5994 <
_temp5992.base? 1: _temp5994 >= _temp5992.last_plus_one)){ _throw(
Null_Exception);}* _temp5994;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp5995= yyvs; void** _temp5997= _temp5995.curr + yyvsp_offset; if( _temp5995.base
== 0? 1:( _temp5997 < _temp5995.base? 1: _temp5997 >= _temp5995.last_plus_one)){
_throw( Null_Exception);}* _temp5997;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5998= yyls; struct Cyc_Yyltype* _temp6000= _temp5998.curr +(
yylsp_offset - 6); if( _temp5998.base == 0? 1:( _temp6000 < _temp5998.base? 1:
_temp6000 >= _temp5998.last_plus_one)){ _throw( Null_Exception);}* _temp6000;}).first_line,({
struct _tagged_ptr3 _temp6001= yyls; struct Cyc_Yyltype* _temp6003= _temp6001.curr
+ yylsp_offset; if( _temp6001.base == 0? 1:( _temp6003 < _temp6001.base? 1:
_temp6003 >= _temp6001.last_plus_one)){ _throw( Null_Exception);}* _temp6003;}).last_line));
goto _LL6005; _LL6005: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp6006=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp6006[ 0]=({ struct Cyc_Stmt_tok_struct _temp6007; _temp6007.tag= Cyc_Stmt_tok;
_temp6007.f1= Cyc_Parse_flatten_declarations( _temp5990, _temp6004); _temp6007;});
_temp6006;}); break;}} case 256: _LL5986: { struct Cyc_List_List* _temp6012= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp6009= yyvs; void** _temp6011= _temp6009.curr +(
yyvsp_offset - 5); if( _temp6009.base == 0? 1:( _temp6011 < _temp6009.base? 1:
_temp6011 >= _temp6009.last_plus_one)){ _throw( Null_Exception);}* _temp6011;}));
goto _LL6013; _LL6013: { struct Cyc_Absyn_Stmt* _temp6029= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6014=
yyvs; void** _temp6016= _temp6014.curr +( yyvsp_offset - 4); if( _temp6014.base
== 0? 1:( _temp6016 < _temp6014.base? 1: _temp6016 >= _temp6014.last_plus_one)){
_throw( Null_Exception);}* _temp6016;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6017= yyvs; void** _temp6019= _temp6017.curr +( yyvsp_offset -
2); if( _temp6017.base == 0? 1:( _temp6019 < _temp6017.base? 1: _temp6019 >=
_temp6017.last_plus_one)){ _throw( Null_Exception);}* _temp6019;})), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp6020= yyvs; void** _temp6022= _temp6020.curr +
yyvsp_offset; if( _temp6020.base == 0? 1:( _temp6022 < _temp6020.base? 1:
_temp6022 >= _temp6020.last_plus_one)){ _throw( Null_Exception);}* _temp6022;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6023= yyls; struct Cyc_Yyltype*
_temp6025= _temp6023.curr +( yylsp_offset - 7); if( _temp6023.base == 0? 1:(
_temp6025 < _temp6023.base? 1: _temp6025 >= _temp6023.last_plus_one)){ _throw(
Null_Exception);}* _temp6025;}).first_line,({ struct _tagged_ptr3 _temp6026=
yyls; struct Cyc_Yyltype* _temp6028= _temp6026.curr + yylsp_offset; if(
_temp6026.base == 0? 1:( _temp6028 < _temp6026.base? 1: _temp6028 >= _temp6026.last_plus_one)){
_throw( Null_Exception);}* _temp6028;}).last_line)); goto _LL6030; _LL6030:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp6031=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp6031[ 0]=({ struct Cyc_Stmt_tok_struct
_temp6032; _temp6032.tag= Cyc_Stmt_tok; _temp6032.f1= Cyc_Parse_flatten_declarations(
_temp6012, _temp6029); _temp6032;}); _temp6031;}); break;}} case 257: _LL6008:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp6034=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp6034[ 0]=({ struct Cyc_Stmt_tok_struct
_temp6035; _temp6035.tag= Cyc_Stmt_tok; _temp6035.f1= Cyc_Absyn_goto_stmt(({
struct _tagged_string* _temp6036=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp6036[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp6037= yyvs; void** _temp6039= _temp6037.curr +( yyvsp_offset -
1); if( _temp6037.base == 0? 1:( _temp6039 < _temp6037.base? 1: _temp6039 >=
_temp6037.last_plus_one)){ _throw( Null_Exception);}* _temp6039;})); _temp6036;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6040= yyls; struct Cyc_Yyltype*
_temp6042= _temp6040.curr +( yylsp_offset - 2); if( _temp6040.base == 0? 1:(
_temp6042 < _temp6040.base? 1: _temp6042 >= _temp6040.last_plus_one)){ _throw(
Null_Exception);}* _temp6042;}).first_line,({ struct _tagged_ptr3 _temp6043=
yyls; struct Cyc_Yyltype* _temp6045= _temp6043.curr +( yylsp_offset - 1); if(
_temp6043.base == 0? 1:( _temp6045 < _temp6043.base? 1: _temp6045 >= _temp6043.last_plus_one)){
_throw( Null_Exception);}* _temp6045;}).last_line)); _temp6035;}); _temp6034;});
break; case 258: _LL6033: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp6047=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp6047[ 0]=({ struct Cyc_Stmt_tok_struct _temp6048; _temp6048.tag= Cyc_Stmt_tok;
_temp6048.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6049= yyls; struct Cyc_Yyltype* _temp6051= _temp6049.curr +(
yylsp_offset - 1); if( _temp6049.base == 0? 1:( _temp6051 < _temp6049.base? 1:
_temp6051 >= _temp6049.last_plus_one)){ _throw( Null_Exception);}* _temp6051;}).first_line,({
struct _tagged_ptr3 _temp6052= yyls; struct Cyc_Yyltype* _temp6054= _temp6052.curr
+( yylsp_offset - 1); if( _temp6052.base == 0? 1:( _temp6054 < _temp6052.base? 1:
_temp6054 >= _temp6052.last_plus_one)){ _throw( Null_Exception);}* _temp6054;}).last_line));
_temp6048;}); _temp6047;}); break; case 259: _LL6046: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp6056=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp6056[ 0]=({ struct Cyc_Stmt_tok_struct
_temp6057; _temp6057.tag= Cyc_Stmt_tok; _temp6057.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6058= yyls; struct Cyc_Yyltype* _temp6060= _temp6058.curr
+( yylsp_offset - 1); if( _temp6058.base == 0? 1:( _temp6060 < _temp6058.base? 1:
_temp6060 >= _temp6058.last_plus_one)){ _throw( Null_Exception);}* _temp6060;}).first_line,({
struct _tagged_ptr3 _temp6061= yyls; struct Cyc_Yyltype* _temp6063= _temp6061.curr
+( yylsp_offset - 1); if( _temp6061.base == 0? 1:( _temp6063 < _temp6061.base? 1:
_temp6063 >= _temp6061.last_plus_one)){ _throw( Null_Exception);}* _temp6063;}).last_line));
_temp6057;}); _temp6056;}); break; case 260: _LL6055: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp6065=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp6065[ 0]=({ struct Cyc_Stmt_tok_struct
_temp6066; _temp6066.tag= Cyc_Stmt_tok; _temp6066.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6067= yyls; struct Cyc_Yyltype*
_temp6069= _temp6067.curr +( yylsp_offset - 1); if( _temp6067.base == 0? 1:(
_temp6069 < _temp6067.base? 1: _temp6069 >= _temp6067.last_plus_one)){ _throw(
Null_Exception);}* _temp6069;}).first_line,({ struct _tagged_ptr3 _temp6070=
yyls; struct Cyc_Yyltype* _temp6072= _temp6070.curr +( yylsp_offset - 1); if(
_temp6070.base == 0? 1:( _temp6072 < _temp6070.base? 1: _temp6072 >= _temp6070.last_plus_one)){
_throw( Null_Exception);}* _temp6072;}).last_line)); _temp6066;}); _temp6065;});
break; case 261: _LL6064: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp6074=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp6074[ 0]=({ struct Cyc_Stmt_tok_struct _temp6075; _temp6075.tag= Cyc_Stmt_tok;
_temp6075.f1= Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6076= yyvs; void** _temp6078= _temp6076.curr +(
yyvsp_offset - 1); if( _temp6076.base == 0? 1:( _temp6078 < _temp6076.base? 1:
_temp6078 >= _temp6076.last_plus_one)){ _throw( Null_Exception);}* _temp6078;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6079= yyls; struct Cyc_Yyltype*
_temp6081= _temp6079.curr +( yylsp_offset - 2); if( _temp6079.base == 0? 1:(
_temp6081 < _temp6079.base? 1: _temp6081 >= _temp6079.last_plus_one)){ _throw(
Null_Exception);}* _temp6081;}).first_line,({ struct _tagged_ptr3 _temp6082=
yyls; struct Cyc_Yyltype* _temp6084= _temp6082.curr +( yylsp_offset - 1); if(
_temp6082.base == 0? 1:( _temp6084 < _temp6082.base? 1: _temp6084 >= _temp6082.last_plus_one)){
_throw( Null_Exception);}* _temp6084;}).last_line)); _temp6075;}); _temp6074;});
break; case 262: _LL6073: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp6086=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp6086[ 0]=({ struct Cyc_Stmt_tok_struct _temp6087; _temp6087.tag= Cyc_Stmt_tok;
_temp6087.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6088= yyls; struct Cyc_Yyltype* _temp6090= _temp6088.curr +(
yylsp_offset - 1); if( _temp6088.base == 0? 1:( _temp6090 < _temp6088.base? 1:
_temp6090 >= _temp6088.last_plus_one)){ _throw( Null_Exception);}* _temp6090;}).first_line,({
struct _tagged_ptr3 _temp6091= yyls; struct Cyc_Yyltype* _temp6093= _temp6091.curr
+( yylsp_offset - 1); if( _temp6091.base == 0? 1:( _temp6093 < _temp6091.base? 1:
_temp6093 >= _temp6091.last_plus_one)){ _throw( Null_Exception);}* _temp6093;}).last_line));
_temp6087;}); _temp6086;}); break; case 263: _LL6085: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp6095=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp6095[ 0]=({ struct Cyc_Stmt_tok_struct
_temp6096; _temp6096.tag= Cyc_Stmt_tok; _temp6096.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6097= yyls; struct Cyc_Yyltype*
_temp6099= _temp6097.curr +( yylsp_offset - 3); if( _temp6097.base == 0? 1:(
_temp6099 < _temp6097.base? 1: _temp6099 >= _temp6097.last_plus_one)){ _throw(
Null_Exception);}* _temp6099;}).first_line,({ struct _tagged_ptr3 _temp6100=
yyls; struct Cyc_Yyltype* _temp6102= _temp6100.curr +( yylsp_offset - 3); if(
_temp6100.base == 0? 1:( _temp6102 < _temp6100.base? 1: _temp6102 >= _temp6100.last_plus_one)){
_throw( Null_Exception);}* _temp6102;}).last_line)); _temp6096;}); _temp6095;});
break; case 264: _LL6094: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp6104=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp6104[ 0]=({ struct Cyc_Stmt_tok_struct _temp6105; _temp6105.tag= Cyc_Stmt_tok;
_temp6105.f1= Cyc_Absyn_fallthru_stmt( Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6106= yyvs; void** _temp6108= _temp6106.curr +( yyvsp_offset -
2); if( _temp6106.base == 0? 1:( _temp6108 < _temp6106.base? 1: _temp6108 >=
_temp6106.last_plus_one)){ _throw( Null_Exception);}* _temp6108;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6109= yyls; struct Cyc_Yyltype* _temp6111= _temp6109.curr
+( yylsp_offset - 4); if( _temp6109.base == 0? 1:( _temp6111 < _temp6109.base? 1:
_temp6111 >= _temp6109.last_plus_one)){ _throw( Null_Exception);}* _temp6111;}).first_line,({
struct _tagged_ptr3 _temp6112= yyls; struct Cyc_Yyltype* _temp6114= _temp6112.curr
+( yylsp_offset - 4); if( _temp6112.base == 0? 1:( _temp6114 < _temp6112.base? 1:
_temp6114 >= _temp6112.last_plus_one)){ _throw( Null_Exception);}* _temp6114;}).last_line));
_temp6105;}); _temp6104;}); break; case 265: _LL6103: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6116=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6116[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6117; _temp6117.tag= Cyc_Pattern_tok; _temp6117.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6118= yyls; struct Cyc_Yyltype* _temp6120= _temp6118.curr + yylsp_offset;
if( _temp6118.base == 0? 1:( _temp6120 < _temp6118.base? 1: _temp6120 >=
_temp6118.last_plus_one)){ _throw( Null_Exception);}* _temp6120;}).first_line,({
struct _tagged_ptr3 _temp6121= yyls; struct Cyc_Yyltype* _temp6123= _temp6121.curr
+ yylsp_offset; if( _temp6121.base == 0? 1:( _temp6123 < _temp6121.base? 1:
_temp6123 >= _temp6121.last_plus_one)){ _throw( Null_Exception);}* _temp6123;}).last_line));
_temp6117;}); _temp6116;}); break; case 266: _LL6115: yyval=({ struct
_tagged_ptr2 _temp6125= yyvs; void** _temp6127= _temp6125.curr +( yyvsp_offset -
1); if( _temp6125.base == 0? 1:( _temp6127 < _temp6125.base? 1: _temp6127 >=
_temp6125.last_plus_one)){ _throw( Null_Exception);}* _temp6127;}); break; case
267: _LL6124: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp6129=( struct
Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp6129[ 0]=({ struct Cyc_Pattern_tok_struct _temp6130; _temp6130.tag= Cyc_Pattern_tok;
_temp6130.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp6131=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp6131[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp6132; _temp6132.tag= Cyc_Absyn_Int_p;
_temp6132.f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp6133= yyvs;
void** _temp6135= _temp6133.curr + yyvsp_offset; if( _temp6133.base == 0? 1:(
_temp6135 < _temp6133.base? 1: _temp6135 >= _temp6133.last_plus_one)){ _throw(
Null_Exception);}* _temp6135;}))).f1; _temp6132.f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp6136= yyvs; void** _temp6138= _temp6136.curr +
yyvsp_offset; if( _temp6136.base == 0? 1:( _temp6138 < _temp6136.base? 1:
_temp6138 >= _temp6136.last_plus_one)){ _throw( Null_Exception);}* _temp6138;}))).f2;
_temp6132;}); _temp6131;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6139= yyls; struct Cyc_Yyltype* _temp6141= _temp6139.curr + yylsp_offset;
if( _temp6139.base == 0? 1:( _temp6141 < _temp6139.base? 1: _temp6141 >=
_temp6139.last_plus_one)){ _throw( Null_Exception);}* _temp6141;}).first_line,({
struct _tagged_ptr3 _temp6142= yyls; struct Cyc_Yyltype* _temp6144= _temp6142.curr
+ yylsp_offset; if( _temp6142.base == 0? 1:( _temp6144 < _temp6142.base? 1:
_temp6144 >= _temp6142.last_plus_one)){ _throw( Null_Exception);}* _temp6144;}).last_line));
_temp6130;}); _temp6129;}); break; case 268: _LL6128: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6146=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6146[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6147; _temp6147.tag= Cyc_Pattern_tok; _temp6147.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp6148=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp6148[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp6149; _temp6149.tag= Cyc_Absyn_Int_p; _temp6149.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp6149.f2= -(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp6150= yyvs; void**
_temp6152= _temp6150.curr + yyvsp_offset; if( _temp6150.base == 0? 1:( _temp6152
< _temp6150.base? 1: _temp6152 >= _temp6150.last_plus_one)){ _throw(
Null_Exception);}* _temp6152;}))).f2; _temp6149;}); _temp6148;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6153= yyls; struct Cyc_Yyltype* _temp6155= _temp6153.curr
+( yylsp_offset - 1); if( _temp6153.base == 0? 1:( _temp6155 < _temp6153.base? 1:
_temp6155 >= _temp6153.last_plus_one)){ _throw( Null_Exception);}* _temp6155;}).first_line,({
struct _tagged_ptr3 _temp6156= yyls; struct Cyc_Yyltype* _temp6158= _temp6156.curr
+ yylsp_offset; if( _temp6156.base == 0? 1:( _temp6158 < _temp6156.base? 1:
_temp6158 >= _temp6156.last_plus_one)){ _throw( Null_Exception);}* _temp6158;}).last_line));
_temp6147;}); _temp6146;}); break; case 269: _LL6145: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6160=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6160[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6161; _temp6161.tag= Cyc_Pattern_tok; _temp6161.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp6162=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp6162[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp6163; _temp6163.tag= Cyc_Absyn_Float_p; _temp6163.f1=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6164= yyvs; void** _temp6166=
_temp6164.curr + yyvsp_offset; if( _temp6164.base == 0? 1:( _temp6166 <
_temp6164.base? 1: _temp6166 >= _temp6164.last_plus_one)){ _throw(
Null_Exception);}* _temp6166;})); _temp6163;}); _temp6162;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6167= yyls; struct Cyc_Yyltype* _temp6169= _temp6167.curr
+ yylsp_offset; if( _temp6167.base == 0? 1:( _temp6169 < _temp6167.base? 1:
_temp6169 >= _temp6167.last_plus_one)){ _throw( Null_Exception);}* _temp6169;}).first_line,({
struct _tagged_ptr3 _temp6170= yyls; struct Cyc_Yyltype* _temp6172= _temp6170.curr
+ yylsp_offset; if( _temp6170.base == 0? 1:( _temp6172 < _temp6170.base? 1:
_temp6172 >= _temp6170.last_plus_one)){ _throw( Null_Exception);}* _temp6172;}).last_line));
_temp6161;}); _temp6160;}); break; case 270: _LL6159: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6174=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6174[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6175; _temp6175.tag= Cyc_Pattern_tok; _temp6175.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Char_p_struct* _temp6176=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp6176[ 0]=({ struct Cyc_Absyn_Char_p_struct
_temp6177; _temp6177.tag= Cyc_Absyn_Char_p; _temp6177.f1= Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp6178= yyvs; void** _temp6180= _temp6178.curr +
yyvsp_offset; if( _temp6178.base == 0? 1:( _temp6180 < _temp6178.base? 1:
_temp6180 >= _temp6178.last_plus_one)){ _throw( Null_Exception);}* _temp6180;}));
_temp6177;}); _temp6176;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6181= yyls; struct Cyc_Yyltype* _temp6183= _temp6181.curr + yylsp_offset;
if( _temp6181.base == 0? 1:( _temp6183 < _temp6181.base? 1: _temp6183 >=
_temp6181.last_plus_one)){ _throw( Null_Exception);}* _temp6183;}).first_line,({
struct _tagged_ptr3 _temp6184= yyls; struct Cyc_Yyltype* _temp6186= _temp6184.curr
+ yylsp_offset; if( _temp6184.base == 0? 1:( _temp6186 < _temp6184.base? 1:
_temp6186 >= _temp6184.last_plus_one)){ _throw( Null_Exception);}* _temp6186;}).last_line));
_temp6175;}); _temp6174;}); break; case 271: _LL6173: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6188=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6188[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6189; _temp6189.tag= Cyc_Pattern_tok; _temp6189.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6190= yyls; struct Cyc_Yyltype* _temp6192= _temp6190.curr + yylsp_offset;
if( _temp6190.base == 0? 1:( _temp6192 < _temp6190.base? 1: _temp6192 >=
_temp6190.last_plus_one)){ _throw( Null_Exception);}* _temp6192;}).first_line,({
struct _tagged_ptr3 _temp6193= yyls; struct Cyc_Yyltype* _temp6195= _temp6193.curr
+ yylsp_offset; if( _temp6193.base == 0? 1:( _temp6195 < _temp6193.base? 1:
_temp6195 >= _temp6193.last_plus_one)){ _throw( Null_Exception);}* _temp6195;}).last_line));
_temp6189;}); _temp6188;}); break; case 272: _LL6187: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6197=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6197[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6198; _temp6198.tag= Cyc_Pattern_tok; _temp6198.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownId_p_struct* _temp6199=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp6199[ 0]=({
struct Cyc_Absyn_UnknownId_p_struct _temp6200; _temp6200.tag= Cyc_Absyn_UnknownId_p;
_temp6200.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6201= yyvs; void**
_temp6203= _temp6201.curr + yyvsp_offset; if( _temp6201.base == 0? 1:( _temp6203
< _temp6201.base? 1: _temp6203 >= _temp6201.last_plus_one)){ _throw(
Null_Exception);}* _temp6203;})); _temp6200;}); _temp6199;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6204= yyls; struct Cyc_Yyltype* _temp6206= _temp6204.curr
+ yylsp_offset; if( _temp6204.base == 0? 1:( _temp6206 < _temp6204.base? 1:
_temp6206 >= _temp6204.last_plus_one)){ _throw( Null_Exception);}* _temp6206;}).first_line,({
struct _tagged_ptr3 _temp6207= yyls; struct Cyc_Yyltype* _temp6209= _temp6207.curr
+ yylsp_offset; if( _temp6207.base == 0? 1:( _temp6209 < _temp6207.base? 1:
_temp6209 >= _temp6207.last_plus_one)){ _throw( Null_Exception);}* _temp6209;}).last_line));
_temp6198;}); _temp6197;}); break; case 273: _LL6196: { struct Cyc_List_List*
_temp6220=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6211=
yyls; struct Cyc_Yyltype* _temp6213= _temp6211.curr +( yylsp_offset - 3); if(
_temp6211.base == 0? 1:( _temp6213 < _temp6211.base? 1: _temp6213 >= _temp6211.last_plus_one)){
_throw( Null_Exception);}* _temp6213;}).first_line,({ struct _tagged_ptr3
_temp6214= yyls; struct Cyc_Yyltype* _temp6216= _temp6214.curr +( yylsp_offset -
3); if( _temp6214.base == 0? 1:( _temp6216 < _temp6214.base? 1: _temp6216 >=
_temp6214.last_plus_one)){ _throw( Null_Exception);}* _temp6216;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6217= yyvs; void** _temp6219=
_temp6217.curr +( yyvsp_offset - 3); if( _temp6217.base == 0? 1:( _temp6219 <
_temp6217.base? 1: _temp6219 >= _temp6217.last_plus_one)){ _throw(
Null_Exception);}* _temp6219;}))); goto _LL6221; _LL6221: yyval=( void*)({
struct Cyc_Pattern_tok_struct* _temp6222=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6222[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6223; _temp6223.tag= Cyc_Pattern_tok; _temp6223.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownCall_p_struct* _temp6224=( struct Cyc_Absyn_UnknownCall_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_p_struct)); _temp6224[ 0]=({
struct Cyc_Absyn_UnknownCall_p_struct _temp6225; _temp6225.tag= Cyc_Absyn_UnknownCall_p;
_temp6225.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6226= yyvs; void**
_temp6228= _temp6226.curr +( yyvsp_offset - 4); if( _temp6226.base == 0? 1:(
_temp6228 < _temp6226.base? 1: _temp6228 >= _temp6226.last_plus_one)){ _throw(
Null_Exception);}* _temp6228;})); _temp6225.f2= _temp6220; _temp6225.f3= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp6229= yyvs; void** _temp6231= _temp6229.curr +(
yyvsp_offset - 1); if( _temp6229.base == 0? 1:( _temp6231 < _temp6229.base? 1:
_temp6231 >= _temp6229.last_plus_one)){ _throw( Null_Exception);}* _temp6231;}));
_temp6225;}); _temp6224;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6232= yyls; struct Cyc_Yyltype* _temp6234= _temp6232.curr +( yylsp_offset -
4); if( _temp6232.base == 0? 1:( _temp6234 < _temp6232.base? 1: _temp6234 >=
_temp6232.last_plus_one)){ _throw( Null_Exception);}* _temp6234;}).first_line,({
struct _tagged_ptr3 _temp6235= yyls; struct Cyc_Yyltype* _temp6237= _temp6235.curr
+ yylsp_offset; if( _temp6235.base == 0? 1:( _temp6237 < _temp6235.base? 1:
_temp6237 >= _temp6235.last_plus_one)){ _throw( Null_Exception);}* _temp6237;}).last_line));
_temp6223;}); _temp6222;}); break;} case 274: _LL6210: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6239=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6239[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6240; _temp6240.tag= Cyc_Pattern_tok; _temp6240.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp6241=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp6241[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp6242; _temp6242.tag= Cyc_Absyn_Tuple_p; _temp6242.f1=
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6243= yyvs; void**
_temp6245= _temp6243.curr +( yyvsp_offset - 1); if( _temp6243.base == 0? 1:(
_temp6245 < _temp6243.base? 1: _temp6245 >= _temp6243.last_plus_one)){ _throw(
Null_Exception);}* _temp6245;})); _temp6242;}); _temp6241;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6246= yyls; struct Cyc_Yyltype* _temp6248= _temp6246.curr
+( yylsp_offset - 3); if( _temp6246.base == 0? 1:( _temp6248 < _temp6246.base? 1:
_temp6248 >= _temp6246.last_plus_one)){ _throw( Null_Exception);}* _temp6248;}).first_line,({
struct _tagged_ptr3 _temp6249= yyls; struct Cyc_Yyltype* _temp6251= _temp6249.curr
+ yylsp_offset; if( _temp6249.base == 0? 1:( _temp6251 < _temp6249.base? 1:
_temp6251 >= _temp6249.last_plus_one)){ _throw( Null_Exception);}* _temp6251;}).last_line));
_temp6240;}); _temp6239;}); break; case 275: _LL6238: { struct Cyc_List_List*
_temp6262=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6253=
yyls; struct Cyc_Yyltype* _temp6255= _temp6253.curr +( yylsp_offset - 2); if(
_temp6253.base == 0? 1:( _temp6255 < _temp6253.base? 1: _temp6255 >= _temp6253.last_plus_one)){
_throw( Null_Exception);}* _temp6255;}).first_line,({ struct _tagged_ptr3
_temp6256= yyls; struct Cyc_Yyltype* _temp6258= _temp6256.curr +( yylsp_offset -
2); if( _temp6256.base == 0? 1:( _temp6258 < _temp6256.base? 1: _temp6258 >=
_temp6256.last_plus_one)){ _throw( Null_Exception);}* _temp6258;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6259= yyvs; void** _temp6261=
_temp6259.curr +( yyvsp_offset - 2); if( _temp6259.base == 0? 1:( _temp6261 <
_temp6259.base? 1: _temp6261 >= _temp6259.last_plus_one)){ _throw(
Null_Exception);}* _temp6261;}))); goto _LL6263; _LL6263: yyval=( void*)({
struct Cyc_Pattern_tok_struct* _temp6264=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6264[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6265; _temp6265.tag= Cyc_Pattern_tok; _temp6265.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp6266=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp6266[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp6267; _temp6267.tag= Cyc_Absyn_UnknownFields_p;
_temp6267.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6268= yyvs; void**
_temp6270= _temp6268.curr +( yyvsp_offset - 3); if( _temp6268.base == 0? 1:(
_temp6270 < _temp6268.base? 1: _temp6270 >= _temp6268.last_plus_one)){ _throw(
Null_Exception);}* _temp6270;})); _temp6267.f2= _temp6262; _temp6267.f3= 0;
_temp6267;}); _temp6266;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6271= yyls; struct Cyc_Yyltype* _temp6273= _temp6271.curr +( yylsp_offset -
3); if( _temp6271.base == 0? 1:( _temp6273 < _temp6271.base? 1: _temp6273 >=
_temp6271.last_plus_one)){ _throw( Null_Exception);}* _temp6273;}).first_line,({
struct _tagged_ptr3 _temp6274= yyls; struct Cyc_Yyltype* _temp6276= _temp6274.curr
+ yylsp_offset; if( _temp6274.base == 0? 1:( _temp6276 < _temp6274.base? 1:
_temp6276 >= _temp6274.last_plus_one)){ _throw( Null_Exception);}* _temp6276;}).last_line));
_temp6265;}); _temp6264;}); break;} case 276: _LL6252: { struct Cyc_List_List*
_temp6287=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6278=
yyls; struct Cyc_Yyltype* _temp6280= _temp6278.curr +( yylsp_offset - 3); if(
_temp6278.base == 0? 1:( _temp6280 < _temp6278.base? 1: _temp6280 >= _temp6278.last_plus_one)){
_throw( Null_Exception);}* _temp6280;}).first_line,({ struct _tagged_ptr3
_temp6281= yyls; struct Cyc_Yyltype* _temp6283= _temp6281.curr +( yylsp_offset -
3); if( _temp6281.base == 0? 1:( _temp6283 < _temp6281.base? 1: _temp6283 >=
_temp6281.last_plus_one)){ _throw( Null_Exception);}* _temp6283;}).last_line),
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp6284= yyvs; void** _temp6286=
_temp6284.curr +( yyvsp_offset - 3); if( _temp6284.base == 0? 1:( _temp6286 <
_temp6284.base? 1: _temp6286 >= _temp6284.last_plus_one)){ _throw(
Null_Exception);}* _temp6286;}))); goto _LL6288; _LL6288: yyval=( void*)({
struct Cyc_Pattern_tok_struct* _temp6289=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp6289[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6290; _temp6290.tag= Cyc_Pattern_tok; _temp6290.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_UnknownFields_p_struct* _temp6291=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp6291[ 0]=({
struct Cyc_Absyn_UnknownFields_p_struct _temp6292; _temp6292.tag= Cyc_Absyn_UnknownFields_p;
_temp6292.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6293= yyvs; void**
_temp6295= _temp6293.curr +( yyvsp_offset - 4); if( _temp6293.base == 0? 1:(
_temp6295 < _temp6293.base? 1: _temp6295 >= _temp6293.last_plus_one)){ _throw(
Null_Exception);}* _temp6295;})); _temp6292.f2= _temp6287; _temp6292.f3= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6296= yyvs; void** _temp6298= _temp6296.curr +(
yyvsp_offset - 1); if( _temp6296.base == 0? 1:( _temp6298 < _temp6296.base? 1:
_temp6298 >= _temp6296.last_plus_one)){ _throw( Null_Exception);}* _temp6298;}));
_temp6292;}); _temp6291;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6299= yyls; struct Cyc_Yyltype* _temp6301= _temp6299.curr +( yylsp_offset -
4); if( _temp6299.base == 0? 1:( _temp6301 < _temp6299.base? 1: _temp6301 >=
_temp6299.last_plus_one)){ _throw( Null_Exception);}* _temp6301;}).first_line,({
struct _tagged_ptr3 _temp6302= yyls; struct Cyc_Yyltype* _temp6304= _temp6302.curr
+ yylsp_offset; if( _temp6302.base == 0? 1:( _temp6304 < _temp6302.base? 1:
_temp6304 >= _temp6302.last_plus_one)){ _throw( Null_Exception);}* _temp6304;}).last_line));
_temp6290;}); _temp6289;}); break;} case 277: _LL6277: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6306=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6306[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6307; _temp6307.tag= Cyc_Pattern_tok; _temp6307.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp6308=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp6308[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp6309; _temp6309.tag= Cyc_Absyn_Pointer_p;
_temp6309.f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6310= yyvs; void**
_temp6312= _temp6310.curr + yyvsp_offset; if( _temp6310.base == 0? 1:( _temp6312
< _temp6310.base? 1: _temp6312 >= _temp6310.last_plus_one)){ _throw(
Null_Exception);}* _temp6312;})); _temp6309;}); _temp6308;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6313= yyls; struct Cyc_Yyltype* _temp6315= _temp6313.curr
+( yylsp_offset - 1); if( _temp6313.base == 0? 1:( _temp6315 < _temp6313.base? 1:
_temp6315 >= _temp6313.last_plus_one)){ _throw( Null_Exception);}* _temp6315;}).first_line,({
struct _tagged_ptr3 _temp6316= yyls; struct Cyc_Yyltype* _temp6318= _temp6316.curr
+ yylsp_offset; if( _temp6316.base == 0? 1:( _temp6318 < _temp6316.base? 1:
_temp6318 >= _temp6316.last_plus_one)){ _throw( Null_Exception);}* _temp6318;}).last_line));
_temp6307;}); _temp6306;}); break; case 278: _LL6305: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp6320=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp6320[ 0]=({ struct Cyc_Pattern_tok_struct
_temp6321; _temp6321.tag= Cyc_Pattern_tok; _temp6321.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp6322=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp6322[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp6323; _temp6323.tag= Cyc_Absyn_Reference_p;
_temp6323.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp6324=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6324->f1=( void*) Cyc_Absyn_Loc_n;
_temp6324->f2=({ struct _tagged_string* _temp6325=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp6325[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6326= yyvs; void** _temp6328= _temp6326.curr +
yyvsp_offset; if( _temp6326.base == 0? 1:( _temp6328 < _temp6326.base? 1:
_temp6328 >= _temp6326.last_plus_one)){ _throw( Null_Exception);}* _temp6328;}));
_temp6325;}); _temp6324;}),( void*) Cyc_Absyn_VoidType, 0); _temp6323;});
_temp6322;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6329= yyls;
struct Cyc_Yyltype* _temp6331= _temp6329.curr +( yylsp_offset - 1); if(
_temp6329.base == 0? 1:( _temp6331 < _temp6329.base? 1: _temp6331 >= _temp6329.last_plus_one)){
_throw( Null_Exception);}* _temp6331;}).first_line,({ struct _tagged_ptr3
_temp6332= yyls; struct Cyc_Yyltype* _temp6334= _temp6332.curr + yylsp_offset;
if( _temp6332.base == 0? 1:( _temp6334 < _temp6332.base? 1: _temp6334 >=
_temp6332.last_plus_one)){ _throw( Null_Exception);}* _temp6334;}).last_line));
_temp6321;}); _temp6320;}); break; case 279: _LL6319: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp6336=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6336[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6337; _temp6337.tag= Cyc_PatternList_tok;
_temp6337.f1= 0; _temp6337;}); _temp6336;}); break; case 280: _LL6335: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp6339=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp6339[ 0]=({ struct
Cyc_PatternList_tok_struct _temp6340; _temp6340.tag= Cyc_PatternList_tok;
_temp6340.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6341= yyvs; void**
_temp6343= _temp6341.curr + yyvsp_offset; if( _temp6341.base == 0? 1:( _temp6343
< _temp6341.base? 1: _temp6343 >= _temp6341.last_plus_one)){ _throw(
Null_Exception);}* _temp6343;}))); _temp6340;}); _temp6339;}); break; case 281:
_LL6338: yyval=( void*)({ struct Cyc_PatternList_tok_struct* _temp6345=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6345[ 0]=({ struct Cyc_PatternList_tok_struct _temp6346; _temp6346.tag= Cyc_PatternList_tok;
_temp6346.f1=({ struct Cyc_List_List* _temp6347=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6347->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6348= yyvs; void** _temp6350= _temp6348.curr +
yyvsp_offset; if( _temp6348.base == 0? 1:( _temp6350 < _temp6348.base? 1:
_temp6350 >= _temp6348.last_plus_one)){ _throw( Null_Exception);}* _temp6350;}));
_temp6347->tl= 0; _temp6347;}); _temp6346;}); _temp6345;}); break; case 282:
_LL6344: yyval=( void*)({ struct Cyc_PatternList_tok_struct* _temp6352=( struct
Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp6352[ 0]=({ struct Cyc_PatternList_tok_struct _temp6353; _temp6353.tag= Cyc_PatternList_tok;
_temp6353.f1=({ struct Cyc_List_List* _temp6354=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6354->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp6358= yyvs; void** _temp6360= _temp6358.curr +
yyvsp_offset; if( _temp6358.base == 0? 1:( _temp6360 < _temp6358.base? 1:
_temp6360 >= _temp6358.last_plus_one)){ _throw( Null_Exception);}* _temp6360;}));
_temp6354->tl= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp6355= yyvs;
void** _temp6357= _temp6355.curr +( yyvsp_offset - 2); if( _temp6355.base == 0?
1:( _temp6357 < _temp6355.base? 1: _temp6357 >= _temp6355.last_plus_one)){
_throw( Null_Exception);}* _temp6357;})); _temp6354;}); _temp6353;}); _temp6352;});
break; case 283: _LL6351: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct*
_temp6362=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6362[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6363; _temp6363.tag=
Cyc_FieldPattern_tok; _temp6363.f1=({ struct _tuple13* _temp6364=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp6364->f1= 0; _temp6364->f2=
Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6365= yyvs; void** _temp6367=
_temp6365.curr + yyvsp_offset; if( _temp6365.base == 0? 1:( _temp6367 <
_temp6365.base? 1: _temp6367 >= _temp6365.last_plus_one)){ _throw(
Null_Exception);}* _temp6367;})); _temp6364;}); _temp6363;}); _temp6362;});
break; case 284: _LL6361: yyval=( void*)({ struct Cyc_FieldPattern_tok_struct*
_temp6369=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp6369[ 0]=({ struct Cyc_FieldPattern_tok_struct _temp6370; _temp6370.tag=
Cyc_FieldPattern_tok; _temp6370.f1=({ struct _tuple13* _temp6371=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp6371->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp6375= yyvs; void** _temp6377= _temp6375.curr +(
yyvsp_offset - 1); if( _temp6375.base == 0? 1:( _temp6377 < _temp6375.base? 1:
_temp6377 >= _temp6375.last_plus_one)){ _throw( Null_Exception);}* _temp6377;}));
_temp6371->f2= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp6372= yyvs;
void** _temp6374= _temp6372.curr + yyvsp_offset; if( _temp6372.base == 0? 1:(
_temp6374 < _temp6372.base? 1: _temp6374 >= _temp6372.last_plus_one)){ _throw(
Null_Exception);}* _temp6374;})); _temp6371;}); _temp6370;}); _temp6369;});
break; case 285: _LL6368: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct*
_temp6379=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp6379[ 0]=({ struct Cyc_FieldPatternList_tok_struct
_temp6380; _temp6380.tag= Cyc_FieldPatternList_tok; _temp6380.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6381= yyvs; void** _temp6383= _temp6381.curr +
yyvsp_offset; if( _temp6381.base == 0? 1:( _temp6383 < _temp6381.base? 1:
_temp6383 >= _temp6381.last_plus_one)){ _throw( Null_Exception);}* _temp6383;})));
_temp6380;}); _temp6379;}); break; case 286: _LL6378: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp6385=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp6385[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp6386; _temp6386.tag= Cyc_FieldPatternList_tok;
_temp6386.f1=({ struct Cyc_List_List* _temp6387=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6387->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp6388= yyvs; void** _temp6390= _temp6388.curr +
yyvsp_offset; if( _temp6388.base == 0? 1:( _temp6390 < _temp6388.base? 1:
_temp6390 >= _temp6388.last_plus_one)){ _throw( Null_Exception);}* _temp6390;}));
_temp6387->tl= 0; _temp6387;}); _temp6386;}); _temp6385;}); break; case 287:
_LL6384: yyval=( void*)({ struct Cyc_FieldPatternList_tok_struct* _temp6392=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));
_temp6392[ 0]=({ struct Cyc_FieldPatternList_tok_struct _temp6393; _temp6393.tag=
Cyc_FieldPatternList_tok; _temp6393.f1=({ struct Cyc_List_List* _temp6394=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp6394->hd=(
void*) Cyc_yyget_FieldPattern_tok(({ struct _tagged_ptr2 _temp6398= yyvs; void**
_temp6400= _temp6398.curr + yyvsp_offset; if( _temp6398.base == 0? 1:( _temp6400
< _temp6398.base? 1: _temp6400 >= _temp6398.last_plus_one)){ _throw(
Null_Exception);}* _temp6400;})); _temp6394->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp6395= yyvs; void** _temp6397= _temp6395.curr +(
yyvsp_offset - 2); if( _temp6395.base == 0? 1:( _temp6397 < _temp6395.base? 1:
_temp6397 >= _temp6395.last_plus_one)){ _throw( Null_Exception);}* _temp6397;}));
_temp6394;}); _temp6393;}); _temp6392;}); break; case 288: _LL6391: yyval=({
struct _tagged_ptr2 _temp6402= yyvs; void** _temp6404= _temp6402.curr +
yyvsp_offset; if( _temp6402.base == 0? 1:( _temp6404 < _temp6402.base? 1:
_temp6404 >= _temp6402.last_plus_one)){ _throw( Null_Exception);}* _temp6404;});
break; case 289: _LL6401: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6406=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6406[ 0]=({ struct Cyc_Exp_tok_struct _temp6407; _temp6407.tag= Cyc_Exp_tok;
_temp6407.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6408= yyvs; void** _temp6410= _temp6408.curr +( yyvsp_offset - 2); if(
_temp6408.base == 0? 1:( _temp6410 < _temp6408.base? 1: _temp6410 >= _temp6408.last_plus_one)){
_throw( Null_Exception);}* _temp6410;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6411= yyvs; void** _temp6413= _temp6411.curr + yyvsp_offset;
if( _temp6411.base == 0? 1:( _temp6413 < _temp6411.base? 1: _temp6413 >=
_temp6411.last_plus_one)){ _throw( Null_Exception);}* _temp6413;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6414= yyls; struct Cyc_Yyltype* _temp6416= _temp6414.curr
+( yylsp_offset - 2); if( _temp6414.base == 0? 1:( _temp6416 < _temp6414.base? 1:
_temp6416 >= _temp6414.last_plus_one)){ _throw( Null_Exception);}* _temp6416;}).first_line,({
struct _tagged_ptr3 _temp6417= yyls; struct Cyc_Yyltype* _temp6419= _temp6417.curr
+ yylsp_offset; if( _temp6417.base == 0? 1:( _temp6419 < _temp6417.base? 1:
_temp6419 >= _temp6417.last_plus_one)){ _throw( Null_Exception);}* _temp6419;}).last_line));
_temp6407;}); _temp6406;}); break; case 290: _LL6405: yyval=({ struct
_tagged_ptr2 _temp6421= yyvs; void** _temp6423= _temp6421.curr + yyvsp_offset;
if( _temp6421.base == 0? 1:( _temp6423 < _temp6421.base? 1: _temp6423 >=
_temp6421.last_plus_one)){ _throw( Null_Exception);}* _temp6423;}); break; case
291: _LL6420: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6425=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6425[ 0]=({ struct Cyc_Exp_tok_struct
_temp6426; _temp6426.tag= Cyc_Exp_tok; _temp6426.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6427= yyvs; void** _temp6429= _temp6427.curr +(
yyvsp_offset - 2); if( _temp6427.base == 0? 1:( _temp6429 < _temp6427.base? 1:
_temp6429 >= _temp6427.last_plus_one)){ _throw( Null_Exception);}* _temp6429;})),
Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2 _temp6430= yyvs; void** _temp6432=
_temp6430.curr +( yyvsp_offset - 1); if( _temp6430.base == 0? 1:( _temp6432 <
_temp6430.base? 1: _temp6432 >= _temp6430.last_plus_one)){ _throw(
Null_Exception);}* _temp6432;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6433= yyvs; void** _temp6435= _temp6433.curr + yyvsp_offset; if( _temp6433.base
== 0? 1:( _temp6435 < _temp6433.base? 1: _temp6435 >= _temp6433.last_plus_one)){
_throw( Null_Exception);}* _temp6435;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6436= yyls; struct Cyc_Yyltype* _temp6438= _temp6436.curr +(
yylsp_offset - 2); if( _temp6436.base == 0? 1:( _temp6438 < _temp6436.base? 1:
_temp6438 >= _temp6436.last_plus_one)){ _throw( Null_Exception);}* _temp6438;}).first_line,({
struct _tagged_ptr3 _temp6439= yyls; struct Cyc_Yyltype* _temp6441= _temp6439.curr
+ yylsp_offset; if( _temp6439.base == 0? 1:( _temp6441 < _temp6439.base? 1:
_temp6441 >= _temp6439.last_plus_one)){ _throw( Null_Exception);}* _temp6441;}).last_line));
_temp6426;}); _temp6425;}); break; case 292: _LL6424: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6443=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6443[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6444; _temp6444.tag= Cyc_Primopopt_tok; _temp6444.f1=
0; _temp6444;}); _temp6443;}); break; case 293: _LL6442: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6446=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6446[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6447; _temp6447.tag= Cyc_Primopopt_tok; _temp6447.f1=({
struct Cyc_Core_Opt* _temp6448=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6448->v=( void*)(( void*) Cyc_Absyn_Times); _temp6448;});
_temp6447;}); _temp6446;}); break; case 294: _LL6445: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6450=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6450[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6451; _temp6451.tag= Cyc_Primopopt_tok; _temp6451.f1=({
struct Cyc_Core_Opt* _temp6452=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6452->v=( void*)(( void*) Cyc_Absyn_Div); _temp6452;});
_temp6451;}); _temp6450;}); break; case 295: _LL6449: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6454=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6454[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6455; _temp6455.tag= Cyc_Primopopt_tok; _temp6455.f1=({
struct Cyc_Core_Opt* _temp6456=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6456->v=( void*)(( void*) Cyc_Absyn_Mod); _temp6456;});
_temp6455;}); _temp6454;}); break; case 296: _LL6453: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6458=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6458[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6459; _temp6459.tag= Cyc_Primopopt_tok; _temp6459.f1=({
struct Cyc_Core_Opt* _temp6460=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6460->v=( void*)(( void*) Cyc_Absyn_Plus); _temp6460;});
_temp6459;}); _temp6458;}); break; case 297: _LL6457: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6462=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6462[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6463; _temp6463.tag= Cyc_Primopopt_tok; _temp6463.f1=({
struct Cyc_Core_Opt* _temp6464=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6464->v=( void*)(( void*) Cyc_Absyn_Minus); _temp6464;});
_temp6463;}); _temp6462;}); break; case 298: _LL6461: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6466=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6466[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6467; _temp6467.tag= Cyc_Primopopt_tok; _temp6467.f1=({
struct Cyc_Core_Opt* _temp6468=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6468->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp6468;});
_temp6467;}); _temp6466;}); break; case 299: _LL6465: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6470=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6470[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6471; _temp6471.tag= Cyc_Primopopt_tok; _temp6471.f1=({
struct Cyc_Core_Opt* _temp6472=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6472->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp6472;});
_temp6471;}); _temp6470;}); break; case 300: _LL6469: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6474=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6474[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6475; _temp6475.tag= Cyc_Primopopt_tok; _temp6475.f1=({
struct Cyc_Core_Opt* _temp6476=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6476->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp6476;});
_temp6475;}); _temp6474;}); break; case 301: _LL6473: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6478=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6478[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6479; _temp6479.tag= Cyc_Primopopt_tok; _temp6479.f1=({
struct Cyc_Core_Opt* _temp6480=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6480->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp6480;});
_temp6479;}); _temp6478;}); break; case 302: _LL6477: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp6482=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp6482[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp6483; _temp6483.tag= Cyc_Primopopt_tok; _temp6483.f1=({
struct Cyc_Core_Opt* _temp6484=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp6484->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp6484;});
_temp6483;}); _temp6482;}); break; case 303: _LL6481: yyval=({ struct
_tagged_ptr2 _temp6486= yyvs; void** _temp6488= _temp6486.curr + yyvsp_offset;
if( _temp6486.base == 0? 1:( _temp6488 < _temp6486.base? 1: _temp6488 >=
_temp6486.last_plus_one)){ _throw( Null_Exception);}* _temp6488;}); break; case
304: _LL6485: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6490=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6490[ 0]=({ struct Cyc_Exp_tok_struct
_temp6491; _temp6491.tag= Cyc_Exp_tok; _temp6491.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6492= yyvs; void** _temp6494=
_temp6492.curr +( yyvsp_offset - 4); if( _temp6492.base == 0? 1:( _temp6494 <
_temp6492.base? 1: _temp6494 >= _temp6492.last_plus_one)){ _throw(
Null_Exception);}* _temp6494;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6495= yyvs; void** _temp6497= _temp6495.curr +( yyvsp_offset - 2); if(
_temp6495.base == 0? 1:( _temp6497 < _temp6495.base? 1: _temp6497 >= _temp6495.last_plus_one)){
_throw( Null_Exception);}* _temp6497;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6498= yyvs; void** _temp6500= _temp6498.curr + yyvsp_offset;
if( _temp6498.base == 0? 1:( _temp6500 < _temp6498.base? 1: _temp6500 >=
_temp6498.last_plus_one)){ _throw( Null_Exception);}* _temp6500;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6501= yyls; struct Cyc_Yyltype* _temp6503= _temp6501.curr
+( yylsp_offset - 4); if( _temp6501.base == 0? 1:( _temp6503 < _temp6501.base? 1:
_temp6503 >= _temp6501.last_plus_one)){ _throw( Null_Exception);}* _temp6503;}).first_line,({
struct _tagged_ptr3 _temp6504= yyls; struct Cyc_Yyltype* _temp6506= _temp6504.curr
+ yylsp_offset; if( _temp6504.base == 0? 1:( _temp6506 < _temp6504.base? 1:
_temp6506 >= _temp6504.last_plus_one)){ _throw( Null_Exception);}* _temp6506;}).last_line));
_temp6491;}); _temp6490;}); break; case 305: _LL6489: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6508=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6508[ 0]=({ struct Cyc_Exp_tok_struct
_temp6509; _temp6509.tag= Cyc_Exp_tok; _temp6509.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6510= yyvs; void** _temp6512= _temp6510.curr +
yyvsp_offset; if( _temp6510.base == 0? 1:( _temp6512 < _temp6510.base? 1:
_temp6512 >= _temp6510.last_plus_one)){ _throw( Null_Exception);}* _temp6512;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6513= yyls; struct Cyc_Yyltype*
_temp6515= _temp6513.curr +( yylsp_offset - 1); if( _temp6513.base == 0? 1:(
_temp6515 < _temp6513.base? 1: _temp6515 >= _temp6513.last_plus_one)){ _throw(
Null_Exception);}* _temp6515;}).first_line,({ struct _tagged_ptr3 _temp6516=
yyls; struct Cyc_Yyltype* _temp6518= _temp6516.curr + yylsp_offset; if(
_temp6516.base == 0? 1:( _temp6518 < _temp6516.base? 1: _temp6518 >= _temp6516.last_plus_one)){
_throw( Null_Exception);}* _temp6518;}).last_line)); _temp6509;}); _temp6508;});
break; case 306: _LL6507: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6520=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6520[ 0]=({ struct Cyc_Exp_tok_struct _temp6521; _temp6521.tag= Cyc_Exp_tok;
_temp6521.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6522= yyvs; void** _temp6524= _temp6522.curr + yyvsp_offset; if( _temp6522.base
== 0? 1:( _temp6524 < _temp6522.base? 1: _temp6524 >= _temp6522.last_plus_one)){
_throw( Null_Exception);}* _temp6524;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6525= yyls; struct Cyc_Yyltype* _temp6527= _temp6525.curr +(
yylsp_offset - 1); if( _temp6525.base == 0? 1:( _temp6527 < _temp6525.base? 1:
_temp6527 >= _temp6525.last_plus_one)){ _throw( Null_Exception);}* _temp6527;}).first_line,({
struct _tagged_ptr3 _temp6528= yyls; struct Cyc_Yyltype* _temp6530= _temp6528.curr
+( yylsp_offset + 1); if( _temp6528.base == 0? 1:( _temp6530 < _temp6528.base? 1:
_temp6530 >= _temp6528.last_plus_one)){ _throw( Null_Exception);}* _temp6530;}).last_line));
_temp6521;}); _temp6520;}); break; case 307: _LL6519: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6532=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6532[ 0]=({ struct Cyc_Exp_tok_struct
_temp6533; _temp6533.tag= Cyc_Exp_tok; _temp6533.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6534= yyvs; void** _temp6536= _temp6534.curr +
yyvsp_offset; if( _temp6534.base == 0? 1:( _temp6536 < _temp6534.base? 1:
_temp6536 >= _temp6534.last_plus_one)){ _throw( Null_Exception);}* _temp6536;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6537= yyls; struct Cyc_Yyltype*
_temp6539= _temp6537.curr +( yylsp_offset - 1); if( _temp6537.base == 0? 1:(
_temp6539 < _temp6537.base? 1: _temp6539 >= _temp6537.last_plus_one)){ _throw(
Null_Exception);}* _temp6539;}).first_line,({ struct _tagged_ptr3 _temp6540=
yyls; struct Cyc_Yyltype* _temp6542= _temp6540.curr +( yylsp_offset + 1); if(
_temp6540.base == 0? 1:( _temp6542 < _temp6540.base? 1: _temp6542 >= _temp6540.last_plus_one)){
_throw( Null_Exception);}* _temp6542;}).last_line)); _temp6533;}); _temp6532;});
break; case 308: _LL6531: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6544=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6544[ 0]=({ struct Cyc_Exp_tok_struct _temp6545; _temp6545.tag= Cyc_Exp_tok;
_temp6545.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6546= yyvs; void** _temp6548= _temp6546.curr +(
yyvsp_offset - 2); if( _temp6546.base == 0? 1:( _temp6548 < _temp6546.base? 1:
_temp6548 >= _temp6546.last_plus_one)){ _throw( Null_Exception);}* _temp6548;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6549= yyvs; void** _temp6551=
_temp6549.curr + yyvsp_offset; if( _temp6549.base == 0? 1:( _temp6551 <
_temp6549.base? 1: _temp6551 >= _temp6549.last_plus_one)){ _throw(
Null_Exception);}* _temp6551;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6552= yyls; struct Cyc_Yyltype* _temp6554= _temp6552.curr +(
yylsp_offset - 4); if( _temp6552.base == 0? 1:( _temp6554 < _temp6552.base? 1:
_temp6554 >= _temp6552.last_plus_one)){ _throw( Null_Exception);}* _temp6554;}).first_line,({
struct _tagged_ptr3 _temp6555= yyls; struct Cyc_Yyltype* _temp6557= _temp6555.curr
+ yylsp_offset; if( _temp6555.base == 0? 1:( _temp6557 < _temp6555.base? 1:
_temp6557 >= _temp6555.last_plus_one)){ _throw( Null_Exception);}* _temp6557;}).last_line));
_temp6545;}); _temp6544;}); break; case 309: _LL6543: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6559=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6559[ 0]=({ struct Cyc_Exp_tok_struct
_temp6560; _temp6560.tag= Cyc_Exp_tok; _temp6560.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6561= yyvs; void**
_temp6563= _temp6561.curr +( yyvsp_offset - 2); if( _temp6561.base == 0? 1:(
_temp6563 < _temp6561.base? 1: _temp6563 >= _temp6561.last_plus_one)){ _throw(
Null_Exception);}* _temp6563;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6564= yyvs; void** _temp6566= _temp6564.curr + yyvsp_offset; if( _temp6564.base
== 0? 1:( _temp6566 < _temp6564.base? 1: _temp6566 >= _temp6564.last_plus_one)){
_throw( Null_Exception);}* _temp6566;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6567= yyls; struct Cyc_Yyltype* _temp6569= _temp6567.curr +(
yylsp_offset - 4); if( _temp6567.base == 0? 1:( _temp6569 < _temp6567.base? 1:
_temp6569 >= _temp6567.last_plus_one)){ _throw( Null_Exception);}* _temp6569;}).first_line,({
struct _tagged_ptr3 _temp6570= yyls; struct Cyc_Yyltype* _temp6572= _temp6570.curr
+ yylsp_offset; if( _temp6570.base == 0? 1:( _temp6572 < _temp6570.base? 1:
_temp6572 >= _temp6570.last_plus_one)){ _throw( Null_Exception);}* _temp6572;}).last_line));
_temp6560;}); _temp6559;}); break; case 310: _LL6558: yyval=({ struct
_tagged_ptr2 _temp6574= yyvs; void** _temp6576= _temp6574.curr + yyvsp_offset;
if( _temp6574.base == 0? 1:( _temp6576 < _temp6574.base? 1: _temp6576 >=
_temp6574.last_plus_one)){ _throw( Null_Exception);}* _temp6576;}); break; case
311: _LL6573: yyval=({ struct _tagged_ptr2 _temp6578= yyvs; void** _temp6580=
_temp6578.curr + yyvsp_offset; if( _temp6578.base == 0? 1:( _temp6580 <
_temp6578.base? 1: _temp6580 >= _temp6578.last_plus_one)){ _throw(
Null_Exception);}* _temp6580;}); break; case 312: _LL6577: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp6582=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6582[ 0]=({ struct Cyc_Exp_tok_struct
_temp6583; _temp6583.tag= Cyc_Exp_tok; _temp6583.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6584= yyvs; void** _temp6586= _temp6584.curr +(
yyvsp_offset - 2); if( _temp6584.base == 0? 1:( _temp6586 < _temp6584.base? 1:
_temp6586 >= _temp6584.last_plus_one)){ _throw( Null_Exception);}* _temp6586;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6587= yyvs; void** _temp6589=
_temp6587.curr + yyvsp_offset; if( _temp6587.base == 0? 1:( _temp6589 <
_temp6587.base? 1: _temp6589 >= _temp6587.last_plus_one)){ _throw(
Null_Exception);}* _temp6589;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6590= yyls; struct Cyc_Yyltype* _temp6592= _temp6590.curr +(
yylsp_offset - 2); if( _temp6590.base == 0? 1:( _temp6592 < _temp6590.base? 1:
_temp6592 >= _temp6590.last_plus_one)){ _throw( Null_Exception);}* _temp6592;}).first_line,({
struct _tagged_ptr3 _temp6593= yyls; struct Cyc_Yyltype* _temp6595= _temp6593.curr
+ yylsp_offset; if( _temp6593.base == 0? 1:( _temp6595 < _temp6593.base? 1:
_temp6595 >= _temp6593.last_plus_one)){ _throw( Null_Exception);}* _temp6595;}).last_line));
_temp6583;}); _temp6582;}); break; case 313: _LL6581: yyval=({ struct
_tagged_ptr2 _temp6597= yyvs; void** _temp6599= _temp6597.curr + yyvsp_offset;
if( _temp6597.base == 0? 1:( _temp6599 < _temp6597.base? 1: _temp6599 >=
_temp6597.last_plus_one)){ _throw( Null_Exception);}* _temp6599;}); break; case
314: _LL6596: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6601=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6601[ 0]=({ struct Cyc_Exp_tok_struct
_temp6602; _temp6602.tag= Cyc_Exp_tok; _temp6602.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6603= yyvs; void** _temp6605= _temp6603.curr +(
yyvsp_offset - 2); if( _temp6603.base == 0? 1:( _temp6605 < _temp6603.base? 1:
_temp6605 >= _temp6603.last_plus_one)){ _throw( Null_Exception);}* _temp6605;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6606= yyvs; void** _temp6608=
_temp6606.curr + yyvsp_offset; if( _temp6606.base == 0? 1:( _temp6608 <
_temp6606.base? 1: _temp6608 >= _temp6606.last_plus_one)){ _throw(
Null_Exception);}* _temp6608;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6609= yyls; struct Cyc_Yyltype* _temp6611= _temp6609.curr +(
yylsp_offset - 2); if( _temp6609.base == 0? 1:( _temp6611 < _temp6609.base? 1:
_temp6611 >= _temp6609.last_plus_one)){ _throw( Null_Exception);}* _temp6611;}).first_line,({
struct _tagged_ptr3 _temp6612= yyls; struct Cyc_Yyltype* _temp6614= _temp6612.curr
+ yylsp_offset; if( _temp6612.base == 0? 1:( _temp6614 < _temp6612.base? 1:
_temp6614 >= _temp6612.last_plus_one)){ _throw( Null_Exception);}* _temp6614;}).last_line));
_temp6602;}); _temp6601;}); break; case 315: _LL6600: yyval=({ struct
_tagged_ptr2 _temp6616= yyvs; void** _temp6618= _temp6616.curr + yyvsp_offset;
if( _temp6616.base == 0? 1:( _temp6618 < _temp6616.base? 1: _temp6618 >=
_temp6616.last_plus_one)){ _throw( Null_Exception);}* _temp6618;}); break; case
316: _LL6615: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6620=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6620[ 0]=({ struct Cyc_Exp_tok_struct
_temp6621; _temp6621.tag= Cyc_Exp_tok; _temp6621.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6622= yyvs; void**
_temp6624= _temp6622.curr +( yyvsp_offset - 2); if( _temp6622.base == 0? 1:(
_temp6624 < _temp6622.base? 1: _temp6624 >= _temp6622.last_plus_one)){ _throw(
Null_Exception);}* _temp6624;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6625= yyvs; void** _temp6627= _temp6625.curr + yyvsp_offset; if( _temp6625.base
== 0? 1:( _temp6627 < _temp6625.base? 1: _temp6627 >= _temp6625.last_plus_one)){
_throw( Null_Exception);}* _temp6627;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6628= yyls; struct Cyc_Yyltype* _temp6630= _temp6628.curr +(
yylsp_offset - 2); if( _temp6628.base == 0? 1:( _temp6630 < _temp6628.base? 1:
_temp6630 >= _temp6628.last_plus_one)){ _throw( Null_Exception);}* _temp6630;}).first_line,({
struct _tagged_ptr3 _temp6631= yyls; struct Cyc_Yyltype* _temp6633= _temp6631.curr
+ yylsp_offset; if( _temp6631.base == 0? 1:( _temp6633 < _temp6631.base? 1:
_temp6633 >= _temp6631.last_plus_one)){ _throw( Null_Exception);}* _temp6633;}).last_line));
_temp6621;}); _temp6620;}); break; case 317: _LL6619: yyval=({ struct
_tagged_ptr2 _temp6635= yyvs; void** _temp6637= _temp6635.curr + yyvsp_offset;
if( _temp6635.base == 0? 1:( _temp6637 < _temp6635.base? 1: _temp6637 >=
_temp6635.last_plus_one)){ _throw( Null_Exception);}* _temp6637;}); break; case
318: _LL6634: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6639=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6639[ 0]=({ struct Cyc_Exp_tok_struct
_temp6640; _temp6640.tag= Cyc_Exp_tok; _temp6640.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6641= yyvs; void**
_temp6643= _temp6641.curr +( yyvsp_offset - 2); if( _temp6641.base == 0? 1:(
_temp6643 < _temp6641.base? 1: _temp6643 >= _temp6641.last_plus_one)){ _throw(
Null_Exception);}* _temp6643;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6644= yyvs; void** _temp6646= _temp6644.curr + yyvsp_offset; if( _temp6644.base
== 0? 1:( _temp6646 < _temp6644.base? 1: _temp6646 >= _temp6644.last_plus_one)){
_throw( Null_Exception);}* _temp6646;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6647= yyls; struct Cyc_Yyltype* _temp6649= _temp6647.curr +(
yylsp_offset - 2); if( _temp6647.base == 0? 1:( _temp6649 < _temp6647.base? 1:
_temp6649 >= _temp6647.last_plus_one)){ _throw( Null_Exception);}* _temp6649;}).first_line,({
struct _tagged_ptr3 _temp6650= yyls; struct Cyc_Yyltype* _temp6652= _temp6650.curr
+ yylsp_offset; if( _temp6650.base == 0? 1:( _temp6652 < _temp6650.base? 1:
_temp6652 >= _temp6650.last_plus_one)){ _throw( Null_Exception);}* _temp6652;}).last_line));
_temp6640;}); _temp6639;}); break; case 319: _LL6638: yyval=({ struct
_tagged_ptr2 _temp6654= yyvs; void** _temp6656= _temp6654.curr + yyvsp_offset;
if( _temp6654.base == 0? 1:( _temp6656 < _temp6654.base? 1: _temp6656 >=
_temp6654.last_plus_one)){ _throw( Null_Exception);}* _temp6656;}); break; case
320: _LL6653: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6658=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6658[ 0]=({ struct Cyc_Exp_tok_struct
_temp6659; _temp6659.tag= Cyc_Exp_tok; _temp6659.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6660= yyvs; void**
_temp6662= _temp6660.curr +( yyvsp_offset - 2); if( _temp6660.base == 0? 1:(
_temp6662 < _temp6660.base? 1: _temp6662 >= _temp6660.last_plus_one)){ _throw(
Null_Exception);}* _temp6662;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6663= yyvs; void** _temp6665= _temp6663.curr + yyvsp_offset; if( _temp6663.base
== 0? 1:( _temp6665 < _temp6663.base? 1: _temp6665 >= _temp6663.last_plus_one)){
_throw( Null_Exception);}* _temp6665;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6666= yyls; struct Cyc_Yyltype* _temp6668= _temp6666.curr +(
yylsp_offset - 2); if( _temp6666.base == 0? 1:( _temp6668 < _temp6666.base? 1:
_temp6668 >= _temp6666.last_plus_one)){ _throw( Null_Exception);}* _temp6668;}).first_line,({
struct _tagged_ptr3 _temp6669= yyls; struct Cyc_Yyltype* _temp6671= _temp6669.curr
+ yylsp_offset; if( _temp6669.base == 0? 1:( _temp6671 < _temp6669.base? 1:
_temp6671 >= _temp6669.last_plus_one)){ _throw( Null_Exception);}* _temp6671;}).last_line));
_temp6659;}); _temp6658;}); break; case 321: _LL6657: yyval=({ struct
_tagged_ptr2 _temp6673= yyvs; void** _temp6675= _temp6673.curr + yyvsp_offset;
if( _temp6673.base == 0? 1:( _temp6675 < _temp6673.base? 1: _temp6675 >=
_temp6673.last_plus_one)){ _throw( Null_Exception);}* _temp6675;}); break; case
322: _LL6672: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6677=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6677[ 0]=({ struct Cyc_Exp_tok_struct
_temp6678; _temp6678.tag= Cyc_Exp_tok; _temp6678.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6679= yyvs; void** _temp6681= _temp6679.curr +(
yyvsp_offset - 2); if( _temp6679.base == 0? 1:( _temp6681 < _temp6679.base? 1:
_temp6681 >= _temp6679.last_plus_one)){ _throw( Null_Exception);}* _temp6681;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6682= yyvs; void** _temp6684=
_temp6682.curr + yyvsp_offset; if( _temp6682.base == 0? 1:( _temp6684 <
_temp6682.base? 1: _temp6684 >= _temp6682.last_plus_one)){ _throw(
Null_Exception);}* _temp6684;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6685= yyls; struct Cyc_Yyltype* _temp6687= _temp6685.curr +(
yylsp_offset - 2); if( _temp6685.base == 0? 1:( _temp6687 < _temp6685.base? 1:
_temp6687 >= _temp6685.last_plus_one)){ _throw( Null_Exception);}* _temp6687;}).first_line,({
struct _tagged_ptr3 _temp6688= yyls; struct Cyc_Yyltype* _temp6690= _temp6688.curr
+ yylsp_offset; if( _temp6688.base == 0? 1:( _temp6690 < _temp6688.base? 1:
_temp6690 >= _temp6688.last_plus_one)){ _throw( Null_Exception);}* _temp6690;}).last_line));
_temp6678;}); _temp6677;}); break; case 323: _LL6676: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6692=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6692[ 0]=({ struct Cyc_Exp_tok_struct
_temp6693; _temp6693.tag= Cyc_Exp_tok; _temp6693.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6694= yyvs; void** _temp6696= _temp6694.curr +(
yyvsp_offset - 2); if( _temp6694.base == 0? 1:( _temp6696 < _temp6694.base? 1:
_temp6696 >= _temp6694.last_plus_one)){ _throw( Null_Exception);}* _temp6696;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6697= yyvs; void** _temp6699=
_temp6697.curr + yyvsp_offset; if( _temp6697.base == 0? 1:( _temp6699 <
_temp6697.base? 1: _temp6699 >= _temp6697.last_plus_one)){ _throw(
Null_Exception);}* _temp6699;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6700= yyls; struct Cyc_Yyltype* _temp6702= _temp6700.curr +(
yylsp_offset - 2); if( _temp6700.base == 0? 1:( _temp6702 < _temp6700.base? 1:
_temp6702 >= _temp6700.last_plus_one)){ _throw( Null_Exception);}* _temp6702;}).first_line,({
struct _tagged_ptr3 _temp6703= yyls; struct Cyc_Yyltype* _temp6705= _temp6703.curr
+ yylsp_offset; if( _temp6703.base == 0? 1:( _temp6705 < _temp6703.base? 1:
_temp6705 >= _temp6703.last_plus_one)){ _throw( Null_Exception);}* _temp6705;}).last_line));
_temp6693;}); _temp6692;}); break; case 324: _LL6691: yyval=({ struct
_tagged_ptr2 _temp6707= yyvs; void** _temp6709= _temp6707.curr + yyvsp_offset;
if( _temp6707.base == 0? 1:( _temp6709 < _temp6707.base? 1: _temp6709 >=
_temp6707.last_plus_one)){ _throw( Null_Exception);}* _temp6709;}); break; case
325: _LL6706: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6711=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6711[ 0]=({ struct Cyc_Exp_tok_struct
_temp6712; _temp6712.tag= Cyc_Exp_tok; _temp6712.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6713= yyvs; void** _temp6715= _temp6713.curr +(
yyvsp_offset - 2); if( _temp6713.base == 0? 1:( _temp6715 < _temp6713.base? 1:
_temp6715 >= _temp6713.last_plus_one)){ _throw( Null_Exception);}* _temp6715;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6716= yyvs; void** _temp6718=
_temp6716.curr + yyvsp_offset; if( _temp6716.base == 0? 1:( _temp6718 <
_temp6716.base? 1: _temp6718 >= _temp6716.last_plus_one)){ _throw(
Null_Exception);}* _temp6718;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6719= yyls; struct Cyc_Yyltype* _temp6721= _temp6719.curr +(
yylsp_offset - 2); if( _temp6719.base == 0? 1:( _temp6721 < _temp6719.base? 1:
_temp6721 >= _temp6719.last_plus_one)){ _throw( Null_Exception);}* _temp6721;}).first_line,({
struct _tagged_ptr3 _temp6722= yyls; struct Cyc_Yyltype* _temp6724= _temp6722.curr
+ yylsp_offset; if( _temp6722.base == 0? 1:( _temp6724 < _temp6722.base? 1:
_temp6724 >= _temp6722.last_plus_one)){ _throw( Null_Exception);}* _temp6724;}).last_line));
_temp6712;}); _temp6711;}); break; case 326: _LL6710: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6726=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6726[ 0]=({ struct Cyc_Exp_tok_struct
_temp6727; _temp6727.tag= Cyc_Exp_tok; _temp6727.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6728= yyvs; void** _temp6730= _temp6728.curr +(
yyvsp_offset - 2); if( _temp6728.base == 0? 1:( _temp6730 < _temp6728.base? 1:
_temp6730 >= _temp6728.last_plus_one)){ _throw( Null_Exception);}* _temp6730;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6731= yyvs; void** _temp6733=
_temp6731.curr + yyvsp_offset; if( _temp6731.base == 0? 1:( _temp6733 <
_temp6731.base? 1: _temp6733 >= _temp6731.last_plus_one)){ _throw(
Null_Exception);}* _temp6733;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6734= yyls; struct Cyc_Yyltype* _temp6736= _temp6734.curr +(
yylsp_offset - 2); if( _temp6734.base == 0? 1:( _temp6736 < _temp6734.base? 1:
_temp6736 >= _temp6734.last_plus_one)){ _throw( Null_Exception);}* _temp6736;}).first_line,({
struct _tagged_ptr3 _temp6737= yyls; struct Cyc_Yyltype* _temp6739= _temp6737.curr
+ yylsp_offset; if( _temp6737.base == 0? 1:( _temp6739 < _temp6737.base? 1:
_temp6739 >= _temp6737.last_plus_one)){ _throw( Null_Exception);}* _temp6739;}).last_line));
_temp6727;}); _temp6726;}); break; case 327: _LL6725: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6741=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6741[ 0]=({ struct Cyc_Exp_tok_struct
_temp6742; _temp6742.tag= Cyc_Exp_tok; _temp6742.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6743= yyvs; void** _temp6745= _temp6743.curr +(
yyvsp_offset - 2); if( _temp6743.base == 0? 1:( _temp6745 < _temp6743.base? 1:
_temp6745 >= _temp6743.last_plus_one)){ _throw( Null_Exception);}* _temp6745;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6746= yyvs; void** _temp6748=
_temp6746.curr + yyvsp_offset; if( _temp6746.base == 0? 1:( _temp6748 <
_temp6746.base? 1: _temp6748 >= _temp6746.last_plus_one)){ _throw(
Null_Exception);}* _temp6748;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6749= yyls; struct Cyc_Yyltype* _temp6751= _temp6749.curr +(
yylsp_offset - 2); if( _temp6749.base == 0? 1:( _temp6751 < _temp6749.base? 1:
_temp6751 >= _temp6749.last_plus_one)){ _throw( Null_Exception);}* _temp6751;}).first_line,({
struct _tagged_ptr3 _temp6752= yyls; struct Cyc_Yyltype* _temp6754= _temp6752.curr
+ yylsp_offset; if( _temp6752.base == 0? 1:( _temp6754 < _temp6752.base? 1:
_temp6754 >= _temp6752.last_plus_one)){ _throw( Null_Exception);}* _temp6754;}).last_line));
_temp6742;}); _temp6741;}); break; case 328: _LL6740: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6756=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6756[ 0]=({ struct Cyc_Exp_tok_struct
_temp6757; _temp6757.tag= Cyc_Exp_tok; _temp6757.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6758= yyvs; void** _temp6760= _temp6758.curr +(
yyvsp_offset - 2); if( _temp6758.base == 0? 1:( _temp6760 < _temp6758.base? 1:
_temp6760 >= _temp6758.last_plus_one)){ _throw( Null_Exception);}* _temp6760;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6761= yyvs; void** _temp6763=
_temp6761.curr + yyvsp_offset; if( _temp6761.base == 0? 1:( _temp6763 <
_temp6761.base? 1: _temp6763 >= _temp6761.last_plus_one)){ _throw(
Null_Exception);}* _temp6763;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6764= yyls; struct Cyc_Yyltype* _temp6766= _temp6764.curr +(
yylsp_offset - 2); if( _temp6764.base == 0? 1:( _temp6766 < _temp6764.base? 1:
_temp6766 >= _temp6764.last_plus_one)){ _throw( Null_Exception);}* _temp6766;}).first_line,({
struct _tagged_ptr3 _temp6767= yyls; struct Cyc_Yyltype* _temp6769= _temp6767.curr
+ yylsp_offset; if( _temp6767.base == 0? 1:( _temp6769 < _temp6767.base? 1:
_temp6769 >= _temp6767.last_plus_one)){ _throw( Null_Exception);}* _temp6769;}).last_line));
_temp6757;}); _temp6756;}); break; case 329: _LL6755: yyval=({ struct
_tagged_ptr2 _temp6771= yyvs; void** _temp6773= _temp6771.curr + yyvsp_offset;
if( _temp6771.base == 0? 1:( _temp6773 < _temp6771.base? 1: _temp6773 >=
_temp6771.last_plus_one)){ _throw( Null_Exception);}* _temp6773;}); break; case
330: _LL6770: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6775=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6775[ 0]=({ struct Cyc_Exp_tok_struct
_temp6776; _temp6776.tag= Cyc_Exp_tok; _temp6776.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6777= yyvs;
void** _temp6779= _temp6777.curr +( yyvsp_offset - 2); if( _temp6777.base == 0?
1:( _temp6779 < _temp6777.base? 1: _temp6779 >= _temp6777.last_plus_one)){
_throw( Null_Exception);}* _temp6779;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6780= yyvs; void** _temp6782= _temp6780.curr + yyvsp_offset;
if( _temp6780.base == 0? 1:( _temp6782 < _temp6780.base? 1: _temp6782 >=
_temp6780.last_plus_one)){ _throw( Null_Exception);}* _temp6782;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6783= yyls; struct Cyc_Yyltype* _temp6785= _temp6783.curr
+( yylsp_offset - 2); if( _temp6783.base == 0? 1:( _temp6785 < _temp6783.base? 1:
_temp6785 >= _temp6783.last_plus_one)){ _throw( Null_Exception);}* _temp6785;}).first_line,({
struct _tagged_ptr3 _temp6786= yyls; struct Cyc_Yyltype* _temp6788= _temp6786.curr
+ yylsp_offset; if( _temp6786.base == 0? 1:( _temp6788 < _temp6786.base? 1:
_temp6788 >= _temp6786.last_plus_one)){ _throw( Null_Exception);}* _temp6788;}).last_line));
_temp6776;}); _temp6775;}); break; case 331: _LL6774: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6790=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6790[ 0]=({ struct Cyc_Exp_tok_struct
_temp6791; _temp6791.tag= Cyc_Exp_tok; _temp6791.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6792= yyvs;
void** _temp6794= _temp6792.curr +( yyvsp_offset - 2); if( _temp6792.base == 0?
1:( _temp6794 < _temp6792.base? 1: _temp6794 >= _temp6792.last_plus_one)){
_throw( Null_Exception);}* _temp6794;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6795= yyvs; void** _temp6797= _temp6795.curr + yyvsp_offset;
if( _temp6795.base == 0? 1:( _temp6797 < _temp6795.base? 1: _temp6797 >=
_temp6795.last_plus_one)){ _throw( Null_Exception);}* _temp6797;})), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6798= yyls; struct Cyc_Yyltype* _temp6800= _temp6798.curr
+( yylsp_offset - 2); if( _temp6798.base == 0? 1:( _temp6800 < _temp6798.base? 1:
_temp6800 >= _temp6798.last_plus_one)){ _throw( Null_Exception);}* _temp6800;}).first_line,({
struct _tagged_ptr3 _temp6801= yyls; struct Cyc_Yyltype* _temp6803= _temp6801.curr
+ yylsp_offset; if( _temp6801.base == 0? 1:( _temp6803 < _temp6801.base? 1:
_temp6803 >= _temp6801.last_plus_one)){ _throw( Null_Exception);}* _temp6803;}).last_line));
_temp6791;}); _temp6790;}); break; case 332: _LL6789: yyval=({ struct
_tagged_ptr2 _temp6805= yyvs; void** _temp6807= _temp6805.curr + yyvsp_offset;
if( _temp6805.base == 0? 1:( _temp6807 < _temp6805.base? 1: _temp6807 >=
_temp6805.last_plus_one)){ _throw( Null_Exception);}* _temp6807;}); break; case
333: _LL6804: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6809=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6809[ 0]=({ struct Cyc_Exp_tok_struct
_temp6810; _temp6810.tag= Cyc_Exp_tok; _temp6810.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6811= yyvs; void**
_temp6813= _temp6811.curr +( yyvsp_offset - 2); if( _temp6811.base == 0? 1:(
_temp6813 < _temp6811.base? 1: _temp6813 >= _temp6811.last_plus_one)){ _throw(
Null_Exception);}* _temp6813;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6814= yyvs; void** _temp6816= _temp6814.curr + yyvsp_offset; if( _temp6814.base
== 0? 1:( _temp6816 < _temp6814.base? 1: _temp6816 >= _temp6814.last_plus_one)){
_throw( Null_Exception);}* _temp6816;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6817= yyls; struct Cyc_Yyltype* _temp6819= _temp6817.curr +(
yylsp_offset - 2); if( _temp6817.base == 0? 1:( _temp6819 < _temp6817.base? 1:
_temp6819 >= _temp6817.last_plus_one)){ _throw( Null_Exception);}* _temp6819;}).first_line,({
struct _tagged_ptr3 _temp6820= yyls; struct Cyc_Yyltype* _temp6822= _temp6820.curr
+ yylsp_offset; if( _temp6820.base == 0? 1:( _temp6822 < _temp6820.base? 1:
_temp6822 >= _temp6820.last_plus_one)){ _throw( Null_Exception);}* _temp6822;}).last_line));
_temp6810;}); _temp6809;}); break; case 334: _LL6808: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6824=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6824[ 0]=({ struct Cyc_Exp_tok_struct
_temp6825; _temp6825.tag= Cyc_Exp_tok; _temp6825.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6826= yyvs; void**
_temp6828= _temp6826.curr +( yyvsp_offset - 2); if( _temp6826.base == 0? 1:(
_temp6828 < _temp6826.base? 1: _temp6828 >= _temp6826.last_plus_one)){ _throw(
Null_Exception);}* _temp6828;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6829= yyvs; void** _temp6831= _temp6829.curr + yyvsp_offset; if( _temp6829.base
== 0? 1:( _temp6831 < _temp6829.base? 1: _temp6831 >= _temp6829.last_plus_one)){
_throw( Null_Exception);}* _temp6831;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6832= yyls; struct Cyc_Yyltype* _temp6834= _temp6832.curr +(
yylsp_offset - 2); if( _temp6832.base == 0? 1:( _temp6834 < _temp6832.base? 1:
_temp6834 >= _temp6832.last_plus_one)){ _throw( Null_Exception);}* _temp6834;}).first_line,({
struct _tagged_ptr3 _temp6835= yyls; struct Cyc_Yyltype* _temp6837= _temp6835.curr
+ yylsp_offset; if( _temp6835.base == 0? 1:( _temp6837 < _temp6835.base? 1:
_temp6837 >= _temp6835.last_plus_one)){ _throw( Null_Exception);}* _temp6837;}).last_line));
_temp6825;}); _temp6824;}); break; case 335: _LL6823: yyval=({ struct
_tagged_ptr2 _temp6839= yyvs; void** _temp6841= _temp6839.curr + yyvsp_offset;
if( _temp6839.base == 0? 1:( _temp6841 < _temp6839.base? 1: _temp6841 >=
_temp6839.last_plus_one)){ _throw( Null_Exception);}* _temp6841;}); break; case
336: _LL6838: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6843=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6843[ 0]=({ struct Cyc_Exp_tok_struct
_temp6844; _temp6844.tag= Cyc_Exp_tok; _temp6844.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6845= yyvs; void**
_temp6847= _temp6845.curr +( yyvsp_offset - 2); if( _temp6845.base == 0? 1:(
_temp6847 < _temp6845.base? 1: _temp6847 >= _temp6845.last_plus_one)){ _throw(
Null_Exception);}* _temp6847;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6848= yyvs; void** _temp6850= _temp6848.curr + yyvsp_offset; if( _temp6848.base
== 0? 1:( _temp6850 < _temp6848.base? 1: _temp6850 >= _temp6848.last_plus_one)){
_throw( Null_Exception);}* _temp6850;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6851= yyls; struct Cyc_Yyltype* _temp6853= _temp6851.curr +(
yylsp_offset - 2); if( _temp6851.base == 0? 1:( _temp6853 < _temp6851.base? 1:
_temp6853 >= _temp6851.last_plus_one)){ _throw( Null_Exception);}* _temp6853;}).first_line,({
struct _tagged_ptr3 _temp6854= yyls; struct Cyc_Yyltype* _temp6856= _temp6854.curr
+ yylsp_offset; if( _temp6854.base == 0? 1:( _temp6856 < _temp6854.base? 1:
_temp6856 >= _temp6854.last_plus_one)){ _throw( Null_Exception);}* _temp6856;}).last_line));
_temp6844;}); _temp6843;}); break; case 337: _LL6842: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6858=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6858[ 0]=({ struct Cyc_Exp_tok_struct
_temp6859; _temp6859.tag= Cyc_Exp_tok; _temp6859.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6860= yyvs; void**
_temp6862= _temp6860.curr +( yyvsp_offset - 2); if( _temp6860.base == 0? 1:(
_temp6862 < _temp6860.base? 1: _temp6862 >= _temp6860.last_plus_one)){ _throw(
Null_Exception);}* _temp6862;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6863= yyvs; void** _temp6865= _temp6863.curr + yyvsp_offset; if( _temp6863.base
== 0? 1:( _temp6865 < _temp6863.base? 1: _temp6865 >= _temp6863.last_plus_one)){
_throw( Null_Exception);}* _temp6865;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6866= yyls; struct Cyc_Yyltype* _temp6868= _temp6866.curr +(
yylsp_offset - 2); if( _temp6866.base == 0? 1:( _temp6868 < _temp6866.base? 1:
_temp6868 >= _temp6866.last_plus_one)){ _throw( Null_Exception);}* _temp6868;}).first_line,({
struct _tagged_ptr3 _temp6869= yyls; struct Cyc_Yyltype* _temp6871= _temp6869.curr
+ yylsp_offset; if( _temp6869.base == 0? 1:( _temp6871 < _temp6869.base? 1:
_temp6871 >= _temp6869.last_plus_one)){ _throw( Null_Exception);}* _temp6871;}).last_line));
_temp6859;}); _temp6858;}); break; case 338: _LL6857: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6873=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6873[ 0]=({ struct Cyc_Exp_tok_struct
_temp6874; _temp6874.tag= Cyc_Exp_tok; _temp6874.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6875= yyvs; void**
_temp6877= _temp6875.curr +( yyvsp_offset - 2); if( _temp6875.base == 0? 1:(
_temp6877 < _temp6875.base? 1: _temp6877 >= _temp6875.last_plus_one)){ _throw(
Null_Exception);}* _temp6877;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6878= yyvs; void** _temp6880= _temp6878.curr + yyvsp_offset; if( _temp6878.base
== 0? 1:( _temp6880 < _temp6878.base? 1: _temp6880 >= _temp6878.last_plus_one)){
_throw( Null_Exception);}* _temp6880;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6881= yyls; struct Cyc_Yyltype* _temp6883= _temp6881.curr +(
yylsp_offset - 2); if( _temp6881.base == 0? 1:( _temp6883 < _temp6881.base? 1:
_temp6883 >= _temp6881.last_plus_one)){ _throw( Null_Exception);}* _temp6883;}).first_line,({
struct _tagged_ptr3 _temp6884= yyls; struct Cyc_Yyltype* _temp6886= _temp6884.curr
+ yylsp_offset; if( _temp6884.base == 0? 1:( _temp6886 < _temp6884.base? 1:
_temp6886 >= _temp6884.last_plus_one)){ _throw( Null_Exception);}* _temp6886;}).last_line));
_temp6874;}); _temp6873;}); break; case 339: _LL6872: yyval=({ struct
_tagged_ptr2 _temp6888= yyvs; void** _temp6890= _temp6888.curr + yyvsp_offset;
if( _temp6888.base == 0? 1:( _temp6890 < _temp6888.base? 1: _temp6890 >=
_temp6888.last_plus_one)){ _throw( Null_Exception);}* _temp6890;}); break; case
340: _LL6887: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6892=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6892[ 0]=({ struct Cyc_Exp_tok_struct
_temp6893; _temp6893.tag= Cyc_Exp_tok; _temp6893.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp6894= yyvs; void** _temp6896= _temp6894.curr +(
yyvsp_offset - 2); if( _temp6894.base == 0? 1:( _temp6896 < _temp6894.base? 1:
_temp6896 >= _temp6894.last_plus_one)){ _throw( Null_Exception);}* _temp6896;}))).f3,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6897= yyvs; void** _temp6899=
_temp6897.curr + yyvsp_offset; if( _temp6897.base == 0? 1:( _temp6899 <
_temp6897.base? 1: _temp6899 >= _temp6897.last_plus_one)){ _throw(
Null_Exception);}* _temp6899;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6900= yyls; struct Cyc_Yyltype* _temp6902= _temp6900.curr +(
yylsp_offset - 3); if( _temp6900.base == 0? 1:( _temp6902 < _temp6900.base? 1:
_temp6902 >= _temp6900.last_plus_one)){ _throw( Null_Exception);}* _temp6902;}).first_line,({
struct _tagged_ptr3 _temp6903= yyls; struct Cyc_Yyltype* _temp6905= _temp6903.curr
+ yylsp_offset; if( _temp6903.base == 0? 1:( _temp6905 < _temp6903.base? 1:
_temp6905 >= _temp6903.last_plus_one)){ _throw( Null_Exception);}* _temp6905;}).last_line));
_temp6893;}); _temp6892;}); break; case 341: _LL6891: yyval=({ struct
_tagged_ptr2 _temp6907= yyvs; void** _temp6909= _temp6907.curr + yyvsp_offset;
if( _temp6907.base == 0? 1:( _temp6909 < _temp6907.base? 1: _temp6909 >=
_temp6907.last_plus_one)){ _throw( Null_Exception);}* _temp6909;}); break; case
342: _LL6906: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6911=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6911[ 0]=({ struct Cyc_Exp_tok_struct
_temp6912; _temp6912.tag= Cyc_Exp_tok; _temp6912.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6913= yyvs; void** _temp6915= _temp6913.curr +
yyvsp_offset; if( _temp6913.base == 0? 1:( _temp6915 < _temp6913.base? 1:
_temp6915 >= _temp6913.last_plus_one)){ _throw( Null_Exception);}* _temp6915;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6916= yyls; struct Cyc_Yyltype*
_temp6918= _temp6916.curr +( yylsp_offset - 1); if( _temp6916.base == 0? 1:(
_temp6918 < _temp6916.base? 1: _temp6918 >= _temp6916.last_plus_one)){ _throw(
Null_Exception);}* _temp6918;}).first_line,({ struct _tagged_ptr3 _temp6919=
yyls; struct Cyc_Yyltype* _temp6921= _temp6919.curr + yylsp_offset; if(
_temp6919.base == 0? 1:( _temp6921 < _temp6919.base? 1: _temp6921 >= _temp6919.last_plus_one)){
_throw( Null_Exception);}* _temp6921;}).last_line)); _temp6912;}); _temp6911;});
break; case 343: _LL6910: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6923=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6923[ 0]=({ struct Cyc_Exp_tok_struct _temp6924; _temp6924.tag= Cyc_Exp_tok;
_temp6924.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6925= yyvs; void** _temp6927= _temp6925.curr + yyvsp_offset; if( _temp6925.base
== 0? 1:( _temp6927 < _temp6925.base? 1: _temp6927 >= _temp6925.last_plus_one)){
_throw( Null_Exception);}* _temp6927;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6928= yyls; struct Cyc_Yyltype* _temp6930= _temp6928.curr +(
yylsp_offset - 1); if( _temp6928.base == 0? 1:( _temp6930 < _temp6928.base? 1:
_temp6930 >= _temp6928.last_plus_one)){ _throw( Null_Exception);}* _temp6930;}).first_line,({
struct _tagged_ptr3 _temp6931= yyls; struct Cyc_Yyltype* _temp6933= _temp6931.curr
+ yylsp_offset; if( _temp6931.base == 0? 1:( _temp6933 < _temp6931.base? 1:
_temp6933 >= _temp6931.last_plus_one)){ _throw( Null_Exception);}* _temp6933;}).last_line));
_temp6924;}); _temp6923;}); break; case 344: _LL6922: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6935=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6935[ 0]=({ struct Cyc_Exp_tok_struct
_temp6936; _temp6936.tag= Cyc_Exp_tok; _temp6936.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6937= yyvs; void** _temp6939= _temp6937.curr +
yyvsp_offset; if( _temp6937.base == 0? 1:( _temp6939 < _temp6937.base? 1:
_temp6939 >= _temp6937.last_plus_one)){ _throw( Null_Exception);}* _temp6939;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6940= yyls; struct Cyc_Yyltype*
_temp6942= _temp6940.curr +( yylsp_offset - 1); if( _temp6940.base == 0? 1:(
_temp6942 < _temp6940.base? 1: _temp6942 >= _temp6940.last_plus_one)){ _throw(
Null_Exception);}* _temp6942;}).first_line,({ struct _tagged_ptr3 _temp6943=
yyls; struct Cyc_Yyltype* _temp6945= _temp6943.curr + yylsp_offset; if(
_temp6943.base == 0? 1:( _temp6945 < _temp6943.base? 1: _temp6945 >= _temp6943.last_plus_one)){
_throw( Null_Exception);}* _temp6945;}).last_line)); _temp6936;}); _temp6935;});
break; case 345: _LL6934: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6947=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6947[ 0]=({ struct Cyc_Exp_tok_struct _temp6948; _temp6948.tag= Cyc_Exp_tok;
_temp6948.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6949= yyvs; void** _temp6951= _temp6949.curr + yyvsp_offset; if( _temp6949.base
== 0? 1:( _temp6951 < _temp6949.base? 1: _temp6951 >= _temp6949.last_plus_one)){
_throw( Null_Exception);}* _temp6951;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6952= yyls; struct Cyc_Yyltype* _temp6954= _temp6952.curr +(
yylsp_offset - 1); if( _temp6952.base == 0? 1:( _temp6954 < _temp6952.base? 1:
_temp6954 >= _temp6952.last_plus_one)){ _throw( Null_Exception);}* _temp6954;}).first_line,({
struct _tagged_ptr3 _temp6955= yyls; struct Cyc_Yyltype* _temp6957= _temp6955.curr
+ yylsp_offset; if( _temp6955.base == 0? 1:( _temp6957 < _temp6955.base? 1:
_temp6957 >= _temp6955.last_plus_one)){ _throw( Null_Exception);}* _temp6957;}).last_line));
_temp6948;}); _temp6947;}); break; case 346: _LL6946: yyval=({ struct
_tagged_ptr2 _temp6959= yyvs; void** _temp6961= _temp6959.curr + yyvsp_offset;
if( _temp6959.base == 0? 1:( _temp6961 < _temp6959.base? 1: _temp6961 >=
_temp6959.last_plus_one)){ _throw( Null_Exception);}* _temp6961;}); break; case
347: _LL6958: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp6963=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6963[ 0]=({ struct Cyc_Exp_tok_struct
_temp6964; _temp6964.tag= Cyc_Exp_tok; _temp6964.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(({
struct _tagged_ptr2 _temp6965= yyvs; void** _temp6967= _temp6965.curr +(
yyvsp_offset - 1); if( _temp6965.base == 0? 1:( _temp6967 < _temp6965.base? 1:
_temp6967 >= _temp6965.last_plus_one)){ _throw( Null_Exception);}* _temp6967;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6968= yyvs; void** _temp6970=
_temp6968.curr + yyvsp_offset; if( _temp6968.base == 0? 1:( _temp6970 <
_temp6968.base? 1: _temp6970 >= _temp6968.last_plus_one)){ _throw(
Null_Exception);}* _temp6970;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6971= yyls; struct Cyc_Yyltype* _temp6973= _temp6971.curr +(
yylsp_offset - 1); if( _temp6971.base == 0? 1:( _temp6973 < _temp6971.base? 1:
_temp6973 >= _temp6971.last_plus_one)){ _throw( Null_Exception);}* _temp6973;}).first_line,({
struct _tagged_ptr3 _temp6974= yyls; struct Cyc_Yyltype* _temp6976= _temp6974.curr
+ yylsp_offset; if( _temp6974.base == 0? 1:( _temp6976 < _temp6974.base? 1:
_temp6976 >= _temp6974.last_plus_one)){ _throw( Null_Exception);}* _temp6976;}).last_line));
_temp6964;}); _temp6963;}); break; case 348: _LL6962: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6978=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6978[ 0]=({ struct Cyc_Exp_tok_struct
_temp6979; _temp6979.tag= Cyc_Exp_tok; _temp6979.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6980= yyvs; void** _temp6982=
_temp6980.curr +( yyvsp_offset - 1); if( _temp6980.base == 0? 1:( _temp6982 <
_temp6980.base? 1: _temp6982 >= _temp6980.last_plus_one)){ _throw(
Null_Exception);}* _temp6982;}))).f3, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6983= yyls; struct Cyc_Yyltype* _temp6985= _temp6983.curr +(
yylsp_offset - 3); if( _temp6983.base == 0? 1:( _temp6985 < _temp6983.base? 1:
_temp6985 >= _temp6983.last_plus_one)){ _throw( Null_Exception);}* _temp6985;}).first_line,({
struct _tagged_ptr3 _temp6986= yyls; struct Cyc_Yyltype* _temp6988= _temp6986.curr
+ yylsp_offset; if( _temp6986.base == 0? 1:( _temp6988 < _temp6986.base? 1:
_temp6988 >= _temp6986.last_plus_one)){ _throw( Null_Exception);}* _temp6988;}).last_line));
_temp6979;}); _temp6978;}); break; case 349: _LL6977: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp6990=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp6990[ 0]=({ struct Cyc_Exp_tok_struct
_temp6991; _temp6991.tag= Cyc_Exp_tok; _temp6991.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6992= yyvs; void** _temp6994=
_temp6992.curr + yyvsp_offset; if( _temp6992.base == 0? 1:( _temp6994 <
_temp6992.base? 1: _temp6994 >= _temp6992.last_plus_one)){ _throw(
Null_Exception);}* _temp6994;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6995= yyls; struct Cyc_Yyltype* _temp6997= _temp6995.curr +(
yylsp_offset - 1); if( _temp6995.base == 0? 1:( _temp6997 < _temp6995.base? 1:
_temp6997 >= _temp6995.last_plus_one)){ _throw( Null_Exception);}* _temp6997;}).first_line,({
struct _tagged_ptr3 _temp6998= yyls; struct Cyc_Yyltype* _temp7000= _temp6998.curr
+ yylsp_offset; if( _temp6998.base == 0? 1:( _temp7000 < _temp6998.base? 1:
_temp7000 >= _temp6998.last_plus_one)){ _throw( Null_Exception);}* _temp7000;}).last_line));
_temp6991;}); _temp6990;}); break; case 350: _LL6989: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7002=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7002[ 0]=({ struct Cyc_Exp_tok_struct
_temp7003; _temp7003.tag= Cyc_Exp_tok; _temp7003.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(({
struct _tagged_ptr2 _temp7004= yyvs; void** _temp7006= _temp7004.curr +(
yyvsp_offset - 3); if( _temp7004.base == 0? 1:( _temp7006 < _temp7004.base? 1:
_temp7006 >= _temp7004.last_plus_one)){ _throw( Null_Exception);}* _temp7006;})),
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7007= yyvs; void** _temp7009=
_temp7007.curr +( yyvsp_offset - 1); if( _temp7007.base == 0? 1:( _temp7009 <
_temp7007.base? 1: _temp7009 >= _temp7007.last_plus_one)){ _throw(
Null_Exception);}* _temp7009;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7010= yyls; struct Cyc_Yyltype* _temp7012= _temp7010.curr +(
yylsp_offset - 3); if( _temp7010.base == 0? 1:( _temp7012 < _temp7010.base? 1:
_temp7012 >= _temp7010.last_plus_one)){ _throw( Null_Exception);}* _temp7012;}).first_line,({
struct _tagged_ptr3 _temp7013= yyls; struct Cyc_Yyltype* _temp7015= _temp7013.curr
+ yylsp_offset; if( _temp7013.base == 0? 1:( _temp7015 < _temp7013.base? 1:
_temp7015 >= _temp7013.last_plus_one)){ _throw( Null_Exception);}* _temp7015;}).last_line));
_temp7003;}); _temp7002;}); break; case 351: _LL7001: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7017=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7017[ 0]=({ struct Cyc_Exp_tok_struct
_temp7018; _temp7018.tag= Cyc_Exp_tok; _temp7018.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp7019=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp7019[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp7020; _temp7020.tag= Cyc_Absyn_Malloc_e;
_temp7020.f1= 0; _temp7020.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp7021= yyvs; void** _temp7023= _temp7021.curr +(
yyvsp_offset - 2); if( _temp7021.base == 0? 1:( _temp7023 < _temp7021.base? 1:
_temp7023 >= _temp7021.last_plus_one)){ _throw( Null_Exception);}* _temp7023;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7024= yyls; struct Cyc_Yyltype*
_temp7026= _temp7024.curr +( yylsp_offset - 2); if( _temp7024.base == 0? 1:(
_temp7026 < _temp7024.base? 1: _temp7026 >= _temp7024.last_plus_one)){ _throw(
Null_Exception);}* _temp7026;}).first_line,({ struct _tagged_ptr3 _temp7027=
yyls; struct Cyc_Yyltype* _temp7029= _temp7027.curr +( yylsp_offset - 2); if(
_temp7027.base == 0? 1:( _temp7029 < _temp7027.base? 1: _temp7029 >= _temp7027.last_plus_one)){
_throw( Null_Exception);}* _temp7029;}).last_line)); _temp7020;}); _temp7019;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7030= yyls; struct Cyc_Yyltype*
_temp7032= _temp7030.curr +( yylsp_offset - 6); if( _temp7030.base == 0? 1:(
_temp7032 < _temp7030.base? 1: _temp7032 >= _temp7030.last_plus_one)){ _throw(
Null_Exception);}* _temp7032;}).first_line,({ struct _tagged_ptr3 _temp7033=
yyls; struct Cyc_Yyltype* _temp7035= _temp7033.curr + yylsp_offset; if(
_temp7033.base == 0? 1:( _temp7035 < _temp7033.base? 1: _temp7035 >= _temp7033.last_plus_one)){
_throw( Null_Exception);}* _temp7035;}).last_line)); _temp7018;}); _temp7017;});
break; case 352: _LL7016: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7037=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7037[ 0]=({ struct Cyc_Exp_tok_struct _temp7038; _temp7038.tag= Cyc_Exp_tok;
_temp7038.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp7039=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp7039[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp7040; _temp7040.tag= Cyc_Absyn_Malloc_e;
_temp7040.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp7041= yyvs; void** _temp7043= _temp7041.curr +( yyvsp_offset - 6); if(
_temp7041.base == 0? 1:( _temp7043 < _temp7041.base? 1: _temp7043 >= _temp7041.last_plus_one)){
_throw( Null_Exception);}* _temp7043;})); _temp7040.f2=( void*) Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp7044= yyvs; void**
_temp7046= _temp7044.curr +( yyvsp_offset - 2); if( _temp7044.base == 0? 1:(
_temp7046 < _temp7044.base? 1: _temp7046 >= _temp7044.last_plus_one)){ _throw(
Null_Exception);}* _temp7046;}))).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7047= yyls; struct Cyc_Yyltype* _temp7049= _temp7047.curr +(
yylsp_offset - 2); if( _temp7047.base == 0? 1:( _temp7049 < _temp7047.base? 1:
_temp7049 >= _temp7047.last_plus_one)){ _throw( Null_Exception);}* _temp7049;}).first_line,({
struct _tagged_ptr3 _temp7050= yyls; struct Cyc_Yyltype* _temp7052= _temp7050.curr
+( yylsp_offset - 2); if( _temp7050.base == 0? 1:( _temp7052 < _temp7050.base? 1:
_temp7052 >= _temp7050.last_plus_one)){ _throw( Null_Exception);}* _temp7052;}).last_line));
_temp7040;}); _temp7039;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7053= yyls; struct Cyc_Yyltype* _temp7055= _temp7053.curr +( yylsp_offset -
8); if( _temp7053.base == 0? 1:( _temp7055 < _temp7053.base? 1: _temp7055 >=
_temp7053.last_plus_one)){ _throw( Null_Exception);}* _temp7055;}).first_line,({
struct _tagged_ptr3 _temp7056= yyls; struct Cyc_Yyltype* _temp7058= _temp7056.curr
+ yylsp_offset; if( _temp7056.base == 0? 1:( _temp7058 < _temp7056.base? 1:
_temp7058 >= _temp7056.last_plus_one)){ _throw( Null_Exception);}* _temp7058;}).last_line));
_temp7038;}); _temp7037;}); break; case 353: _LL7036: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp7060=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp7060[ 0]=({ struct Cyc_Primop_tok_struct
_temp7061; _temp7061.tag= Cyc_Primop_tok; _temp7061.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp7061;}); _temp7060;}); break; case 354: _LL7059: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp7063=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp7063[ 0]=({ struct Cyc_Primop_tok_struct
_temp7064; _temp7064.tag= Cyc_Primop_tok; _temp7064.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp7064;}); _temp7063;}); break; case 355: _LL7062: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp7066=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp7066[ 0]=({ struct Cyc_Primop_tok_struct
_temp7067; _temp7067.tag= Cyc_Primop_tok; _temp7067.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp7067;}); _temp7066;}); break; case 356: _LL7065: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp7069=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp7069[ 0]=({ struct Cyc_Primop_tok_struct
_temp7070; _temp7070.tag= Cyc_Primop_tok; _temp7070.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp7070;}); _temp7069;}); break; case 357: _LL7068: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp7072=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp7072[ 0]=({ struct Cyc_Primop_tok_struct
_temp7073; _temp7073.tag= Cyc_Primop_tok; _temp7073.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp7073;}); _temp7072;}); break; case 358: _LL7071: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp7075=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp7075[ 0]=({ struct Cyc_Primop_tok_struct
_temp7076; _temp7076.tag= Cyc_Primop_tok; _temp7076.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp7076;}); _temp7075;}); break; case 359: _LL7074: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp7078=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp7078[ 0]=({ struct Cyc_Primop_tok_struct
_temp7079; _temp7079.tag= Cyc_Primop_tok; _temp7079.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp7079;}); _temp7078;}); break; case 360: _LL7077: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp7081=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp7081[ 0]=({ struct Cyc_Primop_tok_struct
_temp7082; _temp7082.tag= Cyc_Primop_tok; _temp7082.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp7082;}); _temp7081;}); break; case 361: _LL7080: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp7084=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp7084[ 0]=({ struct Cyc_Primop_tok_struct
_temp7085; _temp7085.tag= Cyc_Primop_tok; _temp7085.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp7085;}); _temp7084;}); break; case 362: _LL7083: yyval=({ struct
_tagged_ptr2 _temp7087= yyvs; void** _temp7089= _temp7087.curr + yyvsp_offset;
if( _temp7087.base == 0? 1:( _temp7089 < _temp7087.base? 1: _temp7089 >=
_temp7087.last_plus_one)){ _throw( Null_Exception);}* _temp7089;}); break; case
363: _LL7086: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7091=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7091[ 0]=({ struct Cyc_Exp_tok_struct
_temp7092; _temp7092.tag= Cyc_Exp_tok; _temp7092.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7093= yyvs; void** _temp7095=
_temp7093.curr +( yyvsp_offset - 3); if( _temp7093.base == 0? 1:( _temp7095 <
_temp7093.base? 1: _temp7095 >= _temp7093.last_plus_one)){ _throw(
Null_Exception);}* _temp7095;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp7096= yyvs; void** _temp7098= _temp7096.curr +( yyvsp_offset - 1); if(
_temp7096.base == 0? 1:( _temp7098 < _temp7096.base? 1: _temp7098 >= _temp7096.last_plus_one)){
_throw( Null_Exception);}* _temp7098;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7099= yyls; struct Cyc_Yyltype* _temp7101= _temp7099.curr +(
yylsp_offset - 3); if( _temp7099.base == 0? 1:( _temp7101 < _temp7099.base? 1:
_temp7101 >= _temp7099.last_plus_one)){ _throw( Null_Exception);}* _temp7101;}).first_line,({
struct _tagged_ptr3 _temp7102= yyls; struct Cyc_Yyltype* _temp7104= _temp7102.curr
+ yylsp_offset; if( _temp7102.base == 0? 1:( _temp7104 < _temp7102.base? 1:
_temp7104 >= _temp7102.last_plus_one)){ _throw( Null_Exception);}* _temp7104;}).last_line));
_temp7092;}); _temp7091;}); break; case 364: _LL7090: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7106=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7106[ 0]=({ struct Cyc_Exp_tok_struct
_temp7107; _temp7107.tag= Cyc_Exp_tok; _temp7107.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7108= yyvs; void** _temp7110=
_temp7108.curr +( yyvsp_offset - 2); if( _temp7108.base == 0? 1:( _temp7110 <
_temp7108.base? 1: _temp7110 >= _temp7108.last_plus_one)){ _throw(
Null_Exception);}* _temp7110;})), 0, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7111= yyls; struct Cyc_Yyltype* _temp7113= _temp7111.curr +(
yylsp_offset - 2); if( _temp7111.base == 0? 1:( _temp7113 < _temp7111.base? 1:
_temp7113 >= _temp7111.last_plus_one)){ _throw( Null_Exception);}* _temp7113;}).first_line,({
struct _tagged_ptr3 _temp7114= yyls; struct Cyc_Yyltype* _temp7116= _temp7114.curr
+ yylsp_offset; if( _temp7114.base == 0? 1:( _temp7116 < _temp7114.base? 1:
_temp7116 >= _temp7114.last_plus_one)){ _throw( Null_Exception);}* _temp7116;}).last_line));
_temp7107;}); _temp7106;}); break; case 365: _LL7105: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7118=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7118[ 0]=({ struct Cyc_Exp_tok_struct
_temp7119; _temp7119.tag= Cyc_Exp_tok; _temp7119.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7120= yyvs; void** _temp7122=
_temp7120.curr +( yyvsp_offset - 3); if( _temp7120.base == 0? 1:( _temp7122 <
_temp7120.base? 1: _temp7122 >= _temp7120.last_plus_one)){ _throw(
Null_Exception);}* _temp7122;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp7123= yyvs; void** _temp7125= _temp7123.curr +( yyvsp_offset - 1); if(
_temp7123.base == 0? 1:( _temp7125 < _temp7123.base? 1: _temp7125 >= _temp7123.last_plus_one)){
_throw( Null_Exception);}* _temp7125;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7126= yyls; struct Cyc_Yyltype* _temp7128= _temp7126.curr +(
yylsp_offset - 3); if( _temp7126.base == 0? 1:( _temp7128 < _temp7126.base? 1:
_temp7128 >= _temp7126.last_plus_one)){ _throw( Null_Exception);}* _temp7128;}).first_line,({
struct _tagged_ptr3 _temp7129= yyls; struct Cyc_Yyltype* _temp7131= _temp7129.curr
+ yylsp_offset; if( _temp7129.base == 0? 1:( _temp7131 < _temp7129.base? 1:
_temp7131 >= _temp7129.last_plus_one)){ _throw( Null_Exception);}* _temp7131;}).last_line));
_temp7119;}); _temp7118;}); break; case 366: _LL7117: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7133=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7133[ 0]=({ struct Cyc_Exp_tok_struct
_temp7134; _temp7134.tag= Cyc_Exp_tok; _temp7134.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7135= yyvs; void** _temp7137=
_temp7135.curr +( yyvsp_offset - 2); if( _temp7135.base == 0? 1:( _temp7137 <
_temp7135.base? 1: _temp7137 >= _temp7135.last_plus_one)){ _throw(
Null_Exception);}* _temp7137;})),({ struct _tagged_string* _temp7138=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7138[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7139= yyvs; void** _temp7141=
_temp7139.curr + yyvsp_offset; if( _temp7139.base == 0? 1:( _temp7141 <
_temp7139.base? 1: _temp7141 >= _temp7139.last_plus_one)){ _throw(
Null_Exception);}* _temp7141;})); _temp7138;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7142= yyls; struct Cyc_Yyltype* _temp7144= _temp7142.curr
+( yylsp_offset - 2); if( _temp7142.base == 0? 1:( _temp7144 < _temp7142.base? 1:
_temp7144 >= _temp7142.last_plus_one)){ _throw( Null_Exception);}* _temp7144;}).first_line,({
struct _tagged_ptr3 _temp7145= yyls; struct Cyc_Yyltype* _temp7147= _temp7145.curr
+ yylsp_offset; if( _temp7145.base == 0? 1:( _temp7147 < _temp7145.base? 1:
_temp7147 >= _temp7145.last_plus_one)){ _throw( Null_Exception);}* _temp7147;}).last_line));
_temp7134;}); _temp7133;}); break; case 367: _LL7132: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp7182= yyvs; void** _temp7184= _temp7182.curr +
yyvsp_offset; if( _temp7182.base == 0? 1:( _temp7184 < _temp7182.base? 1:
_temp7184 >= _temp7182.last_plus_one)){ _throw( Null_Exception);}* _temp7184;}));{
void* _temp7149=(* q).f1; struct Cyc_List_List* _temp7159; struct Cyc_List_List*
_temp7161; _LL7151: if( _temp7149 ==( void*) Cyc_Absyn_Loc_n){ goto _LL7152;}
else{ goto _LL7153;} _LL7153: if(( unsigned int) _temp7149 > 1u?*(( int*)
_temp7149) == Cyc_Absyn_Rel_n: 0){ _LL7160: _temp7159=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp7149)->f1; if( _temp7159 == 0){ goto
_LL7154;} else{ goto _LL7155;}} else{ goto _LL7155;} _LL7155: if(( unsigned int)
_temp7149 > 1u?*(( int*) _temp7149) == Cyc_Absyn_Abs_n: 0){ _LL7162: _temp7161=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp7149)->f1; if(
_temp7161 == 0){ goto _LL7156;} else{ goto _LL7157;}} else{ goto _LL7157;}
_LL7157: goto _LL7158; _LL7152: goto _LL7150; _LL7154: goto _LL7150; _LL7156:
goto _LL7150; _LL7158: Cyc_Parse_err(( struct _tagged_string)({ char* _temp7163=(
char*)"struct field name is qualified"; struct _tagged_string _temp7164;
_temp7164.curr= _temp7163; _temp7164.base= _temp7163; _temp7164.last_plus_one=
_temp7163 + 31; _temp7164;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7165= yyls; struct Cyc_Yyltype* _temp7167= _temp7165.curr + yylsp_offset;
if( _temp7165.base == 0? 1:( _temp7167 < _temp7165.base? 1: _temp7167 >=
_temp7165.last_plus_one)){ _throw( Null_Exception);}* _temp7167;}).first_line,({
struct _tagged_ptr3 _temp7168= yyls; struct Cyc_Yyltype* _temp7170= _temp7168.curr
+ yylsp_offset; if( _temp7168.base == 0? 1:( _temp7170 < _temp7168.base? 1:
_temp7170 >= _temp7168.last_plus_one)){ _throw( Null_Exception);}* _temp7170;}).last_line));
goto _LL7150; _LL7150:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7171=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7171[ 0]=({ struct Cyc_Exp_tok_struct _temp7172; _temp7172.tag= Cyc_Exp_tok;
_temp7172.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7173= yyvs; void** _temp7175= _temp7173.curr +( yyvsp_offset -
2); if( _temp7173.base == 0? 1:( _temp7175 < _temp7173.base? 1: _temp7175 >=
_temp7173.last_plus_one)){ _throw( Null_Exception);}* _temp7175;})),(* q).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7176= yyls; struct Cyc_Yyltype*
_temp7178= _temp7176.curr +( yylsp_offset - 2); if( _temp7176.base == 0? 1:(
_temp7178 < _temp7176.base? 1: _temp7178 >= _temp7176.last_plus_one)){ _throw(
Null_Exception);}* _temp7178;}).first_line,({ struct _tagged_ptr3 _temp7179=
yyls; struct Cyc_Yyltype* _temp7181= _temp7179.curr + yylsp_offset; if(
_temp7179.base == 0? 1:( _temp7181 < _temp7179.base? 1: _temp7181 >= _temp7179.last_plus_one)){
_throw( Null_Exception);}* _temp7181;}).last_line)); _temp7172;}); _temp7171;});
break;} case 368: _LL7148: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7186=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7186[ 0]=({ struct Cyc_Exp_tok_struct _temp7187; _temp7187.tag= Cyc_Exp_tok;
_temp7187.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7188= yyvs; void** _temp7190= _temp7188.curr +( yyvsp_offset -
2); if( _temp7188.base == 0? 1:( _temp7190 < _temp7188.base? 1: _temp7190 >=
_temp7188.last_plus_one)){ _throw( Null_Exception);}* _temp7190;})),({ struct
_tagged_string* _temp7191=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp7191[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7192= yyvs; void** _temp7194= _temp7192.curr + yyvsp_offset; if( _temp7192.base
== 0? 1:( _temp7194 < _temp7192.base? 1: _temp7194 >= _temp7192.last_plus_one)){
_throw( Null_Exception);}* _temp7194;})); _temp7191;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7195= yyls; struct Cyc_Yyltype* _temp7197= _temp7195.curr
+( yylsp_offset - 2); if( _temp7195.base == 0? 1:( _temp7197 < _temp7195.base? 1:
_temp7197 >= _temp7195.last_plus_one)){ _throw( Null_Exception);}* _temp7197;}).first_line,({
struct _tagged_ptr3 _temp7198= yyls; struct Cyc_Yyltype* _temp7200= _temp7198.curr
+ yylsp_offset; if( _temp7198.base == 0? 1:( _temp7200 < _temp7198.base? 1:
_temp7200 >= _temp7198.last_plus_one)){ _throw( Null_Exception);}* _temp7200;}).last_line));
_temp7187;}); _temp7186;}); break; case 369: _LL7185: { struct _tuple1* q= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp7235= yyvs; void** _temp7237= _temp7235.curr +
yyvsp_offset; if( _temp7235.base == 0? 1:( _temp7237 < _temp7235.base? 1:
_temp7237 >= _temp7235.last_plus_one)){ _throw( Null_Exception);}* _temp7237;}));{
void* _temp7202=(* q).f1; struct Cyc_List_List* _temp7212; struct Cyc_List_List*
_temp7214; _LL7204: if( _temp7202 ==( void*) Cyc_Absyn_Loc_n){ goto _LL7205;}
else{ goto _LL7206;} _LL7206: if(( unsigned int) _temp7202 > 1u?*(( int*)
_temp7202) == Cyc_Absyn_Rel_n: 0){ _LL7213: _temp7212=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp7202)->f1; if( _temp7212 == 0){ goto
_LL7207;} else{ goto _LL7208;}} else{ goto _LL7208;} _LL7208: if(( unsigned int)
_temp7202 > 1u?*(( int*) _temp7202) == Cyc_Absyn_Abs_n: 0){ _LL7215: _temp7214=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp7202)->f1; if(
_temp7214 == 0){ goto _LL7209;} else{ goto _LL7210;}} else{ goto _LL7210;}
_LL7210: goto _LL7211; _LL7205: goto _LL7203; _LL7207: goto _LL7203; _LL7209:
goto _LL7203; _LL7211: Cyc_Parse_err(( struct _tagged_string)({ char* _temp7216=(
char*)"struct field is qualified with module name"; struct _tagged_string
_temp7217; _temp7217.curr= _temp7216; _temp7217.base= _temp7216; _temp7217.last_plus_one=
_temp7216 + 43; _temp7217;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7218= yyls; struct Cyc_Yyltype* _temp7220= _temp7218.curr + yylsp_offset;
if( _temp7218.base == 0? 1:( _temp7220 < _temp7218.base? 1: _temp7220 >=
_temp7218.last_plus_one)){ _throw( Null_Exception);}* _temp7220;}).first_line,({
struct _tagged_ptr3 _temp7221= yyls; struct Cyc_Yyltype* _temp7223= _temp7221.curr
+ yylsp_offset; if( _temp7221.base == 0? 1:( _temp7223 < _temp7221.base? 1:
_temp7223 >= _temp7221.last_plus_one)){ _throw( Null_Exception);}* _temp7223;}).last_line));
goto _LL7203; _LL7203:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7224=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7224[ 0]=({ struct Cyc_Exp_tok_struct _temp7225; _temp7225.tag= Cyc_Exp_tok;
_temp7225.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp7226= yyvs; void** _temp7228= _temp7226.curr +( yyvsp_offset -
2); if( _temp7226.base == 0? 1:( _temp7228 < _temp7226.base? 1: _temp7228 >=
_temp7226.last_plus_one)){ _throw( Null_Exception);}* _temp7228;})),(* q).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7229= yyls; struct Cyc_Yyltype*
_temp7231= _temp7229.curr +( yylsp_offset - 2); if( _temp7229.base == 0? 1:(
_temp7231 < _temp7229.base? 1: _temp7231 >= _temp7229.last_plus_one)){ _throw(
Null_Exception);}* _temp7231;}).first_line,({ struct _tagged_ptr3 _temp7232=
yyls; struct Cyc_Yyltype* _temp7234= _temp7232.curr + yylsp_offset; if(
_temp7232.base == 0? 1:( _temp7234 < _temp7232.base? 1: _temp7234 >= _temp7232.last_plus_one)){
_throw( Null_Exception);}* _temp7234;}).last_line)); _temp7225;}); _temp7224;});
break;} case 370: _LL7201: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7239=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7239[ 0]=({ struct Cyc_Exp_tok_struct _temp7240; _temp7240.tag= Cyc_Exp_tok;
_temp7240.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp7241= yyvs; void** _temp7243= _temp7241.curr +( yyvsp_offset - 1); if(
_temp7241.base == 0? 1:( _temp7243 < _temp7241.base? 1: _temp7243 >= _temp7241.last_plus_one)){
_throw( Null_Exception);}* _temp7243;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7244= yyls; struct Cyc_Yyltype* _temp7246= _temp7244.curr +(
yylsp_offset - 1); if( _temp7244.base == 0? 1:( _temp7246 < _temp7244.base? 1:
_temp7246 >= _temp7244.last_plus_one)){ _throw( Null_Exception);}* _temp7246;}).first_line,({
struct _tagged_ptr3 _temp7247= yyls; struct Cyc_Yyltype* _temp7249= _temp7247.curr
+ yylsp_offset; if( _temp7247.base == 0? 1:( _temp7249 < _temp7247.base? 1:
_temp7249 >= _temp7247.last_plus_one)){ _throw( Null_Exception);}* _temp7249;}).last_line));
_temp7240;}); _temp7239;}); break; case 371: _LL7238: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7251=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7251[ 0]=({ struct Cyc_Exp_tok_struct
_temp7252; _temp7252.tag= Cyc_Exp_tok; _temp7252.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7253= yyvs; void** _temp7255= _temp7253.curr +(
yyvsp_offset - 1); if( _temp7253.base == 0? 1:( _temp7255 < _temp7253.base? 1:
_temp7255 >= _temp7253.last_plus_one)){ _throw( Null_Exception);}* _temp7255;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7256= yyls; struct Cyc_Yyltype*
_temp7258= _temp7256.curr +( yylsp_offset - 1); if( _temp7256.base == 0? 1:(
_temp7258 < _temp7256.base? 1: _temp7258 >= _temp7256.last_plus_one)){ _throw(
Null_Exception);}* _temp7258;}).first_line,({ struct _tagged_ptr3 _temp7259=
yyls; struct Cyc_Yyltype* _temp7261= _temp7259.curr + yylsp_offset; if(
_temp7259.base == 0? 1:( _temp7261 < _temp7259.base? 1: _temp7261 >= _temp7259.last_plus_one)){
_throw( Null_Exception);}* _temp7261;}).last_line)); _temp7252;}); _temp7251;});
break; case 372: _LL7250: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7263=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7263[ 0]=({ struct Cyc_Exp_tok_struct _temp7264; _temp7264.tag= Cyc_Exp_tok;
_temp7264.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_CompoundLit_e_struct*
_temp7265=( struct Cyc_Absyn_CompoundLit_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_CompoundLit_e_struct)); _temp7265[ 0]=({ struct Cyc_Absyn_CompoundLit_e_struct
_temp7266; _temp7266.tag= Cyc_Absyn_CompoundLit_e; _temp7266.f1= Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp7267= yyvs; void** _temp7269= _temp7267.curr +(
yyvsp_offset - 4); if( _temp7267.base == 0? 1:( _temp7269 < _temp7267.base? 1:
_temp7269 >= _temp7267.last_plus_one)){ _throw( Null_Exception);}* _temp7269;}));
_temp7266.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2 _temp7270= yyvs; void**
_temp7272= _temp7270.curr +( yyvsp_offset - 1); if( _temp7270.base == 0? 1:(
_temp7272 < _temp7270.base? 1: _temp7272 >= _temp7270.last_plus_one)){ _throw(
Null_Exception);}* _temp7272;}))); _temp7266;}); _temp7265;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7273= yyls; struct Cyc_Yyltype* _temp7275= _temp7273.curr
+( yylsp_offset - 5); if( _temp7273.base == 0? 1:( _temp7275 < _temp7273.base? 1:
_temp7275 >= _temp7273.last_plus_one)){ _throw( Null_Exception);}* _temp7275;}).first_line,({
struct _tagged_ptr3 _temp7276= yyls; struct Cyc_Yyltype* _temp7278= _temp7276.curr
+ yylsp_offset; if( _temp7276.base == 0? 1:( _temp7278 < _temp7276.base? 1:
_temp7278 >= _temp7276.last_plus_one)){ _throw( Null_Exception);}* _temp7278;}).last_line));
_temp7264;}); _temp7263;}); break; case 373: _LL7262: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7280=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7280[ 0]=({ struct Cyc_Exp_tok_struct
_temp7281; _temp7281.tag= Cyc_Exp_tok; _temp7281.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp7282=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp7282[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp7283; _temp7283.tag= Cyc_Absyn_CompoundLit_e;
_temp7283.f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp7284= yyvs;
void** _temp7286= _temp7284.curr +( yyvsp_offset - 5); if( _temp7284.base == 0?
1:( _temp7286 < _temp7284.base? 1: _temp7286 >= _temp7284.last_plus_one)){
_throw( Null_Exception);}* _temp7286;})); _temp7283.f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7287= yyvs; void** _temp7289= _temp7287.curr +(
yyvsp_offset - 2); if( _temp7287.base == 0? 1:( _temp7289 < _temp7287.base? 1:
_temp7289 >= _temp7287.last_plus_one)){ _throw( Null_Exception);}* _temp7289;})));
_temp7283;}); _temp7282;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7290= yyls; struct Cyc_Yyltype* _temp7292= _temp7290.curr +( yylsp_offset -
6); if( _temp7290.base == 0? 1:( _temp7292 < _temp7290.base? 1: _temp7292 >=
_temp7290.last_plus_one)){ _throw( Null_Exception);}* _temp7292;}).first_line,({
struct _tagged_ptr3 _temp7293= yyls; struct Cyc_Yyltype* _temp7295= _temp7293.curr
+ yylsp_offset; if( _temp7293.base == 0? 1:( _temp7295 < _temp7293.base? 1:
_temp7295 >= _temp7293.last_plus_one)){ _throw( Null_Exception);}* _temp7295;}).last_line));
_temp7281;}); _temp7280;}); break; case 374: _LL7279: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7297=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7297[ 0]=({ struct Cyc_Exp_tok_struct
_temp7298; _temp7298.tag= Cyc_Exp_tok; _temp7298.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp7299=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp7299[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp7300; _temp7300.tag= Cyc_Absyn_Fill_e; _temp7300.f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7301= yyvs; void** _temp7303= _temp7301.curr +(
yyvsp_offset - 1); if( _temp7301.base == 0? 1:( _temp7303 < _temp7301.base? 1:
_temp7303 >= _temp7301.last_plus_one)){ _throw( Null_Exception);}* _temp7303;}));
_temp7300;}); _temp7299;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp7304= yyls; struct Cyc_Yyltype* _temp7306= _temp7304.curr +( yylsp_offset -
3); if( _temp7304.base == 0? 1:( _temp7306 < _temp7304.base? 1: _temp7306 >=
_temp7304.last_plus_one)){ _throw( Null_Exception);}* _temp7306;}).first_line,({
struct _tagged_ptr3 _temp7307= yyls; struct Cyc_Yyltype* _temp7309= _temp7307.curr
+ yylsp_offset; if( _temp7307.base == 0? 1:( _temp7309 < _temp7307.base? 1:
_temp7309 >= _temp7307.last_plus_one)){ _throw( Null_Exception);}* _temp7309;}).last_line));
_temp7298;}); _temp7297;}); break; case 375: _LL7296: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7311=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7311[ 0]=({ struct Cyc_Exp_tok_struct
_temp7312; _temp7312.tag= Cyc_Exp_tok; _temp7312.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Codegen_e_struct* _temp7313=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp7313[ 0]=({ struct
Cyc_Absyn_Codegen_e_struct _temp7314; _temp7314.tag= Cyc_Absyn_Codegen_e;
_temp7314.f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp7315= yyvs; void**
_temp7317= _temp7315.curr +( yyvsp_offset - 1); if( _temp7315.base == 0? 1:(
_temp7317 < _temp7315.base? 1: _temp7317 >= _temp7315.last_plus_one)){ _throw(
Null_Exception);}* _temp7317;})); _temp7314;}); _temp7313;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7318= yyls; struct Cyc_Yyltype* _temp7320= _temp7318.curr
+( yylsp_offset - 3); if( _temp7318.base == 0? 1:( _temp7320 < _temp7318.base? 1:
_temp7320 >= _temp7318.last_plus_one)){ _throw( Null_Exception);}* _temp7320;}).first_line,({
struct _tagged_ptr3 _temp7321= yyls; struct Cyc_Yyltype* _temp7323= _temp7321.curr
+ yylsp_offset; if( _temp7321.base == 0? 1:( _temp7323 < _temp7321.base? 1:
_temp7323 >= _temp7321.last_plus_one)){ _throw( Null_Exception);}* _temp7323;}).last_line));
_temp7312;}); _temp7311;}); break; case 376: _LL7310: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7325=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7325[ 0]=({ struct Cyc_Exp_tok_struct
_temp7326; _temp7326.tag= Cyc_Exp_tok; _temp7326.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp7327=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp7327[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp7328; _temp7328.tag= Cyc_Absyn_UnknownId_e;
_temp7328.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7329= yyvs; void**
_temp7331= _temp7329.curr + yyvsp_offset; if( _temp7329.base == 0? 1:( _temp7331
< _temp7329.base? 1: _temp7331 >= _temp7329.last_plus_one)){ _throw(
Null_Exception);}* _temp7331;})); _temp7328;}); _temp7327;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7332= yyls; struct Cyc_Yyltype* _temp7334= _temp7332.curr
+ yylsp_offset; if( _temp7332.base == 0? 1:( _temp7334 < _temp7332.base? 1:
_temp7334 >= _temp7332.last_plus_one)){ _throw( Null_Exception);}* _temp7334;}).first_line,({
struct _tagged_ptr3 _temp7335= yyls; struct Cyc_Yyltype* _temp7337= _temp7335.curr
+ yylsp_offset; if( _temp7335.base == 0? 1:( _temp7337 < _temp7335.base? 1:
_temp7337 >= _temp7335.last_plus_one)){ _throw( Null_Exception);}* _temp7337;}).last_line));
_temp7326;}); _temp7325;}); break; case 377: _LL7324: yyval=({ struct
_tagged_ptr2 _temp7339= yyvs; void** _temp7341= _temp7339.curr + yyvsp_offset;
if( _temp7339.base == 0? 1:( _temp7341 < _temp7339.base? 1: _temp7341 >=
_temp7339.last_plus_one)){ _throw( Null_Exception);}* _temp7341;}); break; case
378: _LL7338: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7343=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp7343[ 0]=({ struct Cyc_Exp_tok_struct
_temp7344; _temp7344.tag= Cyc_Exp_tok; _temp7344.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp7345= yyvs; void** _temp7347= _temp7345.curr +
yyvsp_offset; if( _temp7345.base == 0? 1:( _temp7347 < _temp7345.base? 1:
_temp7347 >= _temp7345.last_plus_one)){ _throw( Null_Exception);}* _temp7347;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7348= yyls; struct Cyc_Yyltype*
_temp7350= _temp7348.curr + yylsp_offset; if( _temp7348.base == 0? 1:( _temp7350
< _temp7348.base? 1: _temp7350 >= _temp7348.last_plus_one)){ _throw(
Null_Exception);}* _temp7350;}).first_line,({ struct _tagged_ptr3 _temp7351=
yyls; struct Cyc_Yyltype* _temp7353= _temp7351.curr + yylsp_offset; if(
_temp7351.base == 0? 1:( _temp7353 < _temp7351.base? 1: _temp7353 >= _temp7351.last_plus_one)){
_throw( Null_Exception);}* _temp7353;}).last_line)); _temp7344;}); _temp7343;});
break; case 379: _LL7342: yyval=({ struct _tagged_ptr2 _temp7355= yyvs; void**
_temp7357= _temp7355.curr +( yyvsp_offset - 1); if( _temp7355.base == 0? 1:(
_temp7357 < _temp7355.base? 1: _temp7357 >= _temp7355.last_plus_one)){ _throw(
Null_Exception);}* _temp7357;}); break; case 380: _LL7354: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp7359=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp7359[ 0]=({ struct Cyc_Exp_tok_struct
_temp7360; _temp7360.tag= Cyc_Exp_tok; _temp7360.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp7361=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7361[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7362; _temp7362.tag=
Cyc_Absyn_UnknownId_e; _temp7362.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp7363= yyvs; void** _temp7365= _temp7363.curr +( yyvsp_offset - 1); if(
_temp7363.base == 0? 1:( _temp7365 < _temp7363.base? 1: _temp7365 >= _temp7363.last_plus_one)){
_throw( Null_Exception);}* _temp7365;})); _temp7362;}); _temp7361;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7366= yyls; struct Cyc_Yyltype* _temp7368= _temp7366.curr
+( yylsp_offset - 1); if( _temp7366.base == 0? 1:( _temp7368 < _temp7366.base? 1:
_temp7368 >= _temp7366.last_plus_one)){ _throw( Null_Exception);}* _temp7368;}).first_line,({
struct _tagged_ptr3 _temp7369= yyls; struct Cyc_Yyltype* _temp7371= _temp7369.curr
+( yylsp_offset - 1); if( _temp7369.base == 0? 1:( _temp7371 < _temp7369.base? 1:
_temp7371 >= _temp7369.last_plus_one)){ _throw( Null_Exception);}* _temp7371;}).last_line)),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7372= yyls; struct Cyc_Yyltype*
_temp7374= _temp7372.curr +( yylsp_offset - 1); if( _temp7372.base == 0? 1:(
_temp7374 < _temp7372.base? 1: _temp7374 >= _temp7372.last_plus_one)){ _throw(
Null_Exception);}* _temp7374;}).first_line,({ struct _tagged_ptr3 _temp7375=
yyls; struct Cyc_Yyltype* _temp7377= _temp7375.curr + yylsp_offset; if(
_temp7375.base == 0? 1:( _temp7377 < _temp7375.base? 1: _temp7377 >= _temp7375.last_plus_one)){
_throw( Null_Exception);}* _temp7377;}).last_line)); _temp7360;}); _temp7359;});
break; case 381: _LL7358: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7379=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7379[ 0]=({ struct Cyc_Exp_tok_struct _temp7380; _temp7380.tag= Cyc_Exp_tok;
_temp7380.f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp7381=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp7381[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp7382; _temp7382.tag=
Cyc_Absyn_UnknownId_e; _temp7382.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp7383= yyvs; void** _temp7385= _temp7383.curr +( yyvsp_offset - 4); if(
_temp7383.base == 0? 1:( _temp7385 < _temp7383.base? 1: _temp7385 >= _temp7383.last_plus_one)){
_throw( Null_Exception);}* _temp7385;})); _temp7382;}); _temp7381;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7386= yyls; struct Cyc_Yyltype* _temp7388= _temp7386.curr
+( yylsp_offset - 4); if( _temp7386.base == 0? 1:( _temp7388 < _temp7386.base? 1:
_temp7388 >= _temp7386.last_plus_one)){ _throw( Null_Exception);}* _temp7388;}).first_line,({
struct _tagged_ptr3 _temp7389= yyls; struct Cyc_Yyltype* _temp7391= _temp7389.curr
+( yylsp_offset - 4); if( _temp7389.base == 0? 1:( _temp7391 < _temp7389.base? 1:
_temp7391 >= _temp7389.last_plus_one)){ _throw( Null_Exception);}* _temp7391;}).last_line)),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp7392= yyvs; void** _temp7394= _temp7392.curr +(
yyvsp_offset - 1); if( _temp7392.base == 0? 1:( _temp7394 < _temp7392.base? 1:
_temp7394 >= _temp7392.last_plus_one)){ _throw( Null_Exception);}* _temp7394;}))),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7395= yyls; struct Cyc_Yyltype*
_temp7397= _temp7395.curr +( yylsp_offset - 4); if( _temp7395.base == 0? 1:(
_temp7397 < _temp7395.base? 1: _temp7397 >= _temp7395.last_plus_one)){ _throw(
Null_Exception);}* _temp7397;}).first_line,({ struct _tagged_ptr3 _temp7398=
yyls; struct Cyc_Yyltype* _temp7400= _temp7398.curr + yylsp_offset; if(
_temp7398.base == 0? 1:( _temp7400 < _temp7398.base? 1: _temp7400 >= _temp7398.last_plus_one)){
_throw( Null_Exception);}* _temp7400;}).last_line)); _temp7380;}); _temp7379;});
break; case 382: _LL7378: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7402=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7402[ 0]=({ struct Cyc_Exp_tok_struct _temp7403; _temp7403.tag= Cyc_Exp_tok;
_temp7403.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp7404= yyvs; void** _temp7406= _temp7404.curr +( yyvsp_offset - 1); if(
_temp7404.base == 0? 1:( _temp7406 < _temp7404.base? 1: _temp7406 >= _temp7404.last_plus_one)){
_throw( Null_Exception);}* _temp7406;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7407= yyls; struct Cyc_Yyltype* _temp7409= _temp7407.curr +(
yylsp_offset - 3); if( _temp7407.base == 0? 1:( _temp7409 < _temp7407.base? 1:
_temp7409 >= _temp7407.last_plus_one)){ _throw( Null_Exception);}* _temp7409;}).first_line,({
struct _tagged_ptr3 _temp7410= yyls; struct Cyc_Yyltype* _temp7412= _temp7410.curr
+ yylsp_offset; if( _temp7410.base == 0? 1:( _temp7412 < _temp7410.base? 1:
_temp7412 >= _temp7410.last_plus_one)){ _throw( Null_Exception);}* _temp7412;}).last_line));
_temp7403;}); _temp7402;}); break; case 383: _LL7401: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7414=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7414[ 0]=({ struct Cyc_Exp_tok_struct
_temp7415; _temp7415.tag= Cyc_Exp_tok; _temp7415.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp7416=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp7416[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp7417; _temp7417.tag= Cyc_Absyn_Struct_e;
_temp7417.f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp7418= yyvs; void**
_temp7420= _temp7418.curr +( yyvsp_offset - 3); if( _temp7418.base == 0? 1:(
_temp7420 < _temp7418.base? 1: _temp7420 >= _temp7418.last_plus_one)){ _throw(
Null_Exception);}* _temp7420;})); _temp7417.f2= 0; _temp7417.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp7421= yyvs; void** _temp7423= _temp7421.curr +(
yyvsp_offset - 1); if( _temp7421.base == 0? 1:( _temp7423 < _temp7421.base? 1:
_temp7423 >= _temp7421.last_plus_one)){ _throw( Null_Exception);}* _temp7423;})));
_temp7417.f4= 0; _temp7417;}); _temp7416;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7424= yyls; struct Cyc_Yyltype* _temp7426= _temp7424.curr
+( yylsp_offset - 3); if( _temp7424.base == 0? 1:( _temp7426 < _temp7424.base? 1:
_temp7426 >= _temp7424.last_plus_one)){ _throw( Null_Exception);}* _temp7426;}).first_line,({
struct _tagged_ptr3 _temp7427= yyls; struct Cyc_Yyltype* _temp7429= _temp7427.curr
+ yylsp_offset; if( _temp7427.base == 0? 1:( _temp7429 < _temp7427.base? 1:
_temp7429 >= _temp7427.last_plus_one)){ _throw( Null_Exception);}* _temp7429;}).last_line));
_temp7415;}); _temp7414;}); break; case 384: _LL7413: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7431=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7431[ 0]=({ struct Cyc_Exp_tok_struct
_temp7432; _temp7432.tag= Cyc_Exp_tok; _temp7432.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp7433= yyvs; void** _temp7435= _temp7433.curr +(
yyvsp_offset - 2); if( _temp7433.base == 0? 1:( _temp7435 < _temp7433.base? 1:
_temp7435 >= _temp7433.last_plus_one)){ _throw( Null_Exception);}* _temp7435;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7436= yyls; struct Cyc_Yyltype*
_temp7438= _temp7436.curr +( yylsp_offset - 4); if( _temp7436.base == 0? 1:(
_temp7438 < _temp7436.base? 1: _temp7438 >= _temp7436.last_plus_one)){ _throw(
Null_Exception);}* _temp7438;}).first_line,({ struct _tagged_ptr3 _temp7439=
yyls; struct Cyc_Yyltype* _temp7441= _temp7439.curr + yylsp_offset; if(
_temp7439.base == 0? 1:( _temp7441 < _temp7439.base? 1: _temp7441 >= _temp7439.last_plus_one)){
_throw( Null_Exception);}* _temp7441;}).last_line)); _temp7432;}); _temp7431;});
break; case 385: _LL7430: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp7443=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7443[ 0]=({ struct Cyc_ExpList_tok_struct _temp7444; _temp7444.tag= Cyc_ExpList_tok;
_temp7444.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7445= yyvs; void** _temp7447=
_temp7445.curr + yyvsp_offset; if( _temp7445.base == 0? 1:( _temp7447 <
_temp7445.base? 1: _temp7447 >= _temp7445.last_plus_one)){ _throw(
Null_Exception);}* _temp7447;}))); _temp7444;}); _temp7443;}); break; case 386:
_LL7442: yyval=( void*)({ struct Cyc_ExpList_tok_struct* _temp7449=( struct Cyc_ExpList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ExpList_tok_struct)); _temp7449[ 0]=({ struct Cyc_ExpList_tok_struct
_temp7450; _temp7450.tag= Cyc_ExpList_tok; _temp7450.f1=({ struct Cyc_List_List*
_temp7451=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp7451->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp7452= yyvs;
void** _temp7454= _temp7452.curr + yyvsp_offset; if( _temp7452.base == 0? 1:(
_temp7454 < _temp7452.base? 1: _temp7454 >= _temp7452.last_plus_one)){ _throw(
Null_Exception);}* _temp7454;})); _temp7451->tl= 0; _temp7451;}); _temp7450;});
_temp7449;}); break; case 387: _LL7448: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp7456=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp7456[ 0]=({ struct Cyc_ExpList_tok_struct _temp7457; _temp7457.tag= Cyc_ExpList_tok;
_temp7457.f1=({ struct Cyc_List_List* _temp7458=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp7458->hd=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp7462= yyvs; void** _temp7464= _temp7462.curr +
yyvsp_offset; if( _temp7462.base == 0? 1:( _temp7464 < _temp7462.base? 1:
_temp7464 >= _temp7462.last_plus_one)){ _throw( Null_Exception);}* _temp7464;}));
_temp7458->tl= Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp7459= yyvs;
void** _temp7461= _temp7459.curr +( yyvsp_offset - 2); if( _temp7459.base == 0?
1:( _temp7461 < _temp7459.base? 1: _temp7461 >= _temp7459.last_plus_one)){
_throw( Null_Exception);}* _temp7461;})); _temp7458;}); _temp7457;}); _temp7456;});
break; case 388: _LL7455: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7466=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7466[ 0]=({ struct Cyc_Exp_tok_struct _temp7467; _temp7467.tag= Cyc_Exp_tok;
_temp7467.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(({ struct _tagged_ptr2
_temp7468= yyvs; void** _temp7470= _temp7468.curr + yyvsp_offset; if( _temp7468.base
== 0? 1:( _temp7470 < _temp7468.base? 1: _temp7470 >= _temp7468.last_plus_one)){
_throw( Null_Exception);}* _temp7470;}))).f1,(* Cyc_yyget_Int_tok(({ struct
_tagged_ptr2 _temp7471= yyvs; void** _temp7473= _temp7471.curr + yyvsp_offset;
if( _temp7471.base == 0? 1:( _temp7473 < _temp7471.base? 1: _temp7473 >=
_temp7471.last_plus_one)){ _throw( Null_Exception);}* _temp7473;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7474= yyls; struct Cyc_Yyltype* _temp7476= _temp7474.curr
+ yylsp_offset; if( _temp7474.base == 0? 1:( _temp7476 < _temp7474.base? 1:
_temp7476 >= _temp7474.last_plus_one)){ _throw( Null_Exception);}* _temp7476;}).first_line,({
struct _tagged_ptr3 _temp7477= yyls; struct Cyc_Yyltype* _temp7479= _temp7477.curr
+ yylsp_offset; if( _temp7477.base == 0? 1:( _temp7479 < _temp7477.base? 1:
_temp7479 >= _temp7477.last_plus_one)){ _throw( Null_Exception);}* _temp7479;}).last_line));
_temp7467;}); _temp7466;}); break; case 389: _LL7465: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7481=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7481[ 0]=({ struct Cyc_Exp_tok_struct
_temp7482; _temp7482.tag= Cyc_Exp_tok; _temp7482.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(({
struct _tagged_ptr2 _temp7483= yyvs; void** _temp7485= _temp7483.curr +
yyvsp_offset; if( _temp7483.base == 0? 1:( _temp7485 < _temp7483.base? 1:
_temp7485 >= _temp7483.last_plus_one)){ _throw( Null_Exception);}* _temp7485;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp7486= yyls; struct Cyc_Yyltype*
_temp7488= _temp7486.curr + yylsp_offset; if( _temp7486.base == 0? 1:( _temp7488
< _temp7486.base? 1: _temp7488 >= _temp7486.last_plus_one)){ _throw(
Null_Exception);}* _temp7488;}).first_line,({ struct _tagged_ptr3 _temp7489=
yyls; struct Cyc_Yyltype* _temp7491= _temp7489.curr + yylsp_offset; if(
_temp7489.base == 0? 1:( _temp7491 < _temp7489.base? 1: _temp7491 >= _temp7489.last_plus_one)){
_throw( Null_Exception);}* _temp7491;}).last_line)); _temp7482;}); _temp7481;});
break; case 390: _LL7480: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp7493=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp7493[ 0]=({ struct Cyc_Exp_tok_struct _temp7494; _temp7494.tag= Cyc_Exp_tok;
_temp7494.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp7495= yyvs; void** _temp7497= _temp7495.curr + yyvsp_offset; if( _temp7495.base
== 0? 1:( _temp7497 < _temp7495.base? 1: _temp7497 >= _temp7495.last_plus_one)){
_throw( Null_Exception);}* _temp7497;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp7498= yyls; struct Cyc_Yyltype* _temp7500= _temp7498.curr +
yylsp_offset; if( _temp7498.base == 0? 1:( _temp7500 < _temp7498.base? 1:
_temp7500 >= _temp7498.last_plus_one)){ _throw( Null_Exception);}* _temp7500;}).first_line,({
struct _tagged_ptr3 _temp7501= yyls; struct Cyc_Yyltype* _temp7503= _temp7501.curr
+ yylsp_offset; if( _temp7501.base == 0? 1:( _temp7503 < _temp7501.base? 1:
_temp7503 >= _temp7501.last_plus_one)){ _throw( Null_Exception);}* _temp7503;}).last_line));
_temp7494;}); _temp7493;}); break; case 391: _LL7492: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp7505=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp7505[ 0]=({ struct Cyc_Exp_tok_struct
_temp7506; _temp7506.tag= Cyc_Exp_tok; _temp7506.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp7507= yyls; struct Cyc_Yyltype* _temp7509= _temp7507.curr
+ yylsp_offset; if( _temp7507.base == 0? 1:( _temp7509 < _temp7507.base? 1:
_temp7509 >= _temp7507.last_plus_one)){ _throw( Null_Exception);}* _temp7509;}).first_line,({
struct _tagged_ptr3 _temp7510= yyls; struct Cyc_Yyltype* _temp7512= _temp7510.curr
+ yylsp_offset; if( _temp7510.base == 0? 1:( _temp7512 < _temp7510.base? 1:
_temp7512 >= _temp7510.last_plus_one)){ _throw( Null_Exception);}* _temp7512;}).last_line));
_temp7506;}); _temp7505;}); break; case 392: _LL7504: yyval=( void*)({ struct
Cyc_QualId_tok_struct* _temp7514=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp7514[ 0]=({ struct Cyc_QualId_tok_struct
_temp7515; _temp7515.tag= Cyc_QualId_tok; _temp7515.f1=({ struct _tuple1*
_temp7516=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp7516->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp7521=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp7521[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp7522; _temp7522.tag= Cyc_Absyn_Rel_n; _temp7522.f1= 0; _temp7522;});
_temp7521;}); _temp7516->f2=({ struct _tagged_string* _temp7517=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp7517[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp7518= yyvs; void** _temp7520=
_temp7518.curr + yyvsp_offset; if( _temp7518.base == 0? 1:( _temp7520 <
_temp7518.base? 1: _temp7520 >= _temp7518.last_plus_one)){ _throw(
Null_Exception);}* _temp7520;})); _temp7517;}); _temp7516;}); _temp7515;});
_temp7514;}); break; case 393: _LL7513: yyval=({ struct _tagged_ptr2 _temp7524=
yyvs; void** _temp7526= _temp7524.curr + yyvsp_offset; if( _temp7524.base == 0?
1:( _temp7526 < _temp7524.base? 1: _temp7526 >= _temp7524.last_plus_one)){
_throw( Null_Exception);}* _temp7526;}); break; default: _LL7523:( void) _throw((
void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen; yyssp_offset -= yylen;
yylsp_offset -= yylen;({ struct _tagged_ptr2 _temp7528= yyvs; void** _temp7530=
_temp7528.curr +( ++ yyvsp_offset); if( _temp7528.base == 0? 1:( _temp7530 <
_temp7528.base? 1: _temp7530 >= _temp7528.last_plus_one)){ _throw(
Null_Exception);}* _temp7530= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp7531= yyls; struct Cyc_Yyltype* _temp7533= _temp7531.curr
+ yylsp_offset; if( _temp7531.base == 0? 1:( _temp7533 < _temp7531.base? 1:
_temp7533 >= _temp7531.last_plus_one)){ _throw( Null_Exception);}(* _temp7533).first_line=
Cyc_yylloc.first_line;});({ struct _tagged_ptr3 _temp7534= yyls; struct Cyc_Yyltype*
_temp7536= _temp7534.curr + yylsp_offset; if( _temp7534.base == 0? 1:( _temp7536
< _temp7534.base? 1: _temp7536 >= _temp7534.last_plus_one)){ _throw(
Null_Exception);}(* _temp7536).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp7537= yyls; struct Cyc_Yyltype* _temp7539= _temp7537.curr
+ yylsp_offset; if( _temp7537.base == 0? 1:( _temp7539 < _temp7537.base? 1:
_temp7539 >= _temp7537.last_plus_one)){ _throw( Null_Exception);}(* _temp7539).last_line=({
struct _tagged_ptr3 _temp7540= yyls; struct Cyc_Yyltype* _temp7542= _temp7540.curr
+( yylsp_offset - 1); if( _temp7540.base == 0? 1:( _temp7542 < _temp7540.base? 1:
_temp7542 >= _temp7540.last_plus_one)){ _throw( Null_Exception);}* _temp7542;}).last_line;});({
struct _tagged_ptr3 _temp7543= yyls; struct Cyc_Yyltype* _temp7545= _temp7543.curr
+ yylsp_offset; if( _temp7543.base == 0? 1:( _temp7545 < _temp7543.base? 1:
_temp7545 >= _temp7543.last_plus_one)){ _throw( Null_Exception);}(* _temp7545).last_column=({
struct _tagged_ptr3 _temp7546= yyls; struct Cyc_Yyltype* _temp7548= _temp7546.curr
+( yylsp_offset - 1); if( _temp7546.base == 0? 1:( _temp7548 < _temp7546.base? 1:
_temp7548 >= _temp7546.last_plus_one)){ _throw( Null_Exception);}* _temp7548;}).last_column;});({
struct _tagged_ptr3 _temp7549= yyls; struct Cyc_Yyltype* _temp7551= _temp7549.curr
+ yylsp_offset; if( _temp7549.base == 0? 1:( _temp7551 < _temp7549.base? 1:
_temp7551 >= _temp7549.last_plus_one)){ _throw( Null_Exception);}(* _temp7551).text=(
struct _tagged_string)({ char* _temp7552=( char*)""; struct _tagged_string
_temp7553; _temp7553.curr= _temp7552; _temp7553.base= _temp7552; _temp7553.last_plus_one=
_temp7552 + 1; _temp7553;});});} else{({ struct _tagged_ptr3 _temp7554= yyls;
struct Cyc_Yyltype* _temp7556= _temp7554.curr + yylsp_offset; if( _temp7554.base
== 0? 1:( _temp7556 < _temp7554.base? 1: _temp7556 >= _temp7554.last_plus_one)){
_throw( Null_Exception);}(* _temp7556).last_line=({ struct _tagged_ptr3
_temp7557= yyls; struct Cyc_Yyltype* _temp7559= _temp7557.curr +(( yylsp_offset
+ yylen) - 1); if( _temp7557.base == 0? 1:( _temp7559 < _temp7557.base? 1:
_temp7559 >= _temp7557.last_plus_one)){ _throw( Null_Exception);}* _temp7559;}).last_line;});({
struct _tagged_ptr3 _temp7560= yyls; struct Cyc_Yyltype* _temp7562= _temp7560.curr
+ yylsp_offset; if( _temp7560.base == 0? 1:( _temp7562 < _temp7560.base? 1:
_temp7562 >= _temp7560.last_plus_one)){ _throw( Null_Exception);}(* _temp7562).last_column=({
struct _tagged_ptr3 _temp7563= yyls; struct Cyc_Yyltype* _temp7565= _temp7563.curr
+(( yylsp_offset + yylen) - 1); if( _temp7563.base == 0? 1:( _temp7565 <
_temp7563.base? 1: _temp7565 >= _temp7563.last_plus_one)){ _throw(
Null_Exception);}* _temp7565;}).last_column;});} yyn=( int)({ short* _temp7566=(
short*) Cyc_yyr1; unsigned int _temp7567= yyn; if( _temp7567 >= 394u){ _throw(
Null_Exception);} _temp7566[ _temp7567];}); yystate=({ short* _temp7568=( short*)
Cyc_yypgoto; unsigned int _temp7569= yyn - 124; if( _temp7569 >= 108u){ _throw(
Null_Exception);} _temp7568[ _temp7569];}) +({ struct _tagged_ptr1 _temp7570=
yyss; short* _temp7572= _temp7570.curr + yyssp_offset; if( _temp7570.base == 0?
1:( _temp7572 < _temp7570.base? 1: _temp7572 >= _temp7570.last_plus_one)){
_throw( Null_Exception);}* _temp7572;}); if(( yystate >= 0? yystate <= 4222: 0)?({
short* _temp7573=( short*) Cyc_yycheck; unsigned int _temp7574= yystate; if(
_temp7574 >= 4223u){ _throw( Null_Exception);} _temp7573[ _temp7574];}) ==({
struct _tagged_ptr1 _temp7575= yyss; short* _temp7577= _temp7575.curr +
yyssp_offset; if( _temp7575.base == 0? 1:( _temp7577 < _temp7575.base? 1:
_temp7577 >= _temp7575.last_plus_one)){ _throw( Null_Exception);}* _temp7577;}):
0){ yystate=( int)({ short* _temp7578=( short*) Cyc_yytable; unsigned int
_temp7579= yystate; if( _temp7579 >= 4223u){ _throw( Null_Exception);} _temp7578[
_temp7579];});} else{ yystate=( int)({ short* _temp7580=( short*) Cyc_yydefgoto;
unsigned int _temp7581= yyn - 124; if( _temp7581 >= 108u){ _throw(
Null_Exception);} _temp7580[ _temp7581];});} goto yynewstate; yyerrlab: if(
yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short* _temp7582=( short*) Cyc_yypact;
unsigned int _temp7583= yystate; if( _temp7583 >= 751u){ _throw( Null_Exception);}
_temp7582[ _temp7583];}); if( yyn > - 32768? yyn < 4222: 0){ int sze= 0; struct
_tagged_string msg; int x; int count; count= 0; for( x= yyn < 0? - yyn: 0; x <
232u / sizeof( char*); x ++){ if(({ short* _temp7584=( short*) Cyc_yycheck;
unsigned int _temp7585= x + yyn; if( _temp7585 >= 4223u){ _throw( Null_Exception);}
_temp7584[ _temp7585];}) == x){ sze += Cyc_String_strlen(({ struct
_tagged_string* _temp7586=( struct _tagged_string*) Cyc_yytname; unsigned int
_temp7587= x; if( _temp7587 >= 232u){ _throw( Null_Exception);} _temp7586[
_temp7587];})) + 15, count ++;}} msg=({ unsigned int _temp7588=( unsigned int)(
sze + 15); char* _temp7589=( char*) GC_malloc_atomic( sizeof( char) * _temp7588);
struct _tagged_string _temp7592={ _temp7589, _temp7589, _temp7589 + _temp7588};{
unsigned int _temp7590= _temp7588; unsigned int i; for( i= 0; i < _temp7590; i
++){ _temp7589[ i]='\000';}}; _temp7592;}); Cyc_String_strcpy( msg,( struct
_tagged_string)({ char* _temp7593=( char*)"parse error"; struct _tagged_string
_temp7594; _temp7594.curr= _temp7593; _temp7594.base= _temp7593; _temp7594.last_plus_one=
_temp7593 + 12; _temp7594;})); if( count < 5){ count= 0; for( x= yyn < 0? - yyn:
0; x < 232u / sizeof( char*); x ++){ if(({ short* _temp7595=( short*) Cyc_yycheck;
unsigned int _temp7596= x + yyn; if( _temp7596 >= 4223u){ _throw( Null_Exception);}
_temp7595[ _temp7596];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp7597=( char*)", expecting `"; struct _tagged_string
_temp7598; _temp7598.curr= _temp7597; _temp7598.base= _temp7597; _temp7598.last_plus_one=
_temp7597 + 14; _temp7598;}):( struct _tagged_string)({ char* _temp7599=( char*)" or `";
struct _tagged_string _temp7600; _temp7600.curr= _temp7599; _temp7600.base=
_temp7599; _temp7600.last_plus_one= _temp7599 + 6; _temp7600;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp7601=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp7602= x; if( _temp7602 >= 232u){ _throw( Null_Exception);}
_temp7601[ _temp7602];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp7603=( char*)"'"; struct _tagged_string _temp7604; _temp7604.curr=
_temp7603; _temp7604.base= _temp7603; _temp7604.last_plus_one= _temp7603 + 2;
_temp7604;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp7605=( char*)"parse error"; struct _tagged_string
_temp7606; _temp7606.curr= _temp7605; _temp7606.base= _temp7605; _temp7606.last_plus_one=
_temp7605 + 12; _temp7606;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp7607= yyss; short*
_temp7609= _temp7607.curr +( -- yyssp_offset); if( _temp7607.base == 0? 1:(
_temp7609 < _temp7607.base? 1: _temp7609 >= _temp7607.last_plus_one)){ _throw(
Null_Exception);}* _temp7609;}); yylsp_offset --; yyerrhandle: yyn=( int)({
short* _temp7610=( short*) Cyc_yypact; unsigned int _temp7611= yystate; if(
_temp7611 >= 751u){ _throw( Null_Exception);} _temp7610[ _temp7611];}); if( yyn
== - 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4222)? 1:({
short* _temp7612=( short*) Cyc_yycheck; unsigned int _temp7613= yyn; if(
_temp7613 >= 4223u){ _throw( Null_Exception);} _temp7612[ _temp7613];}) != 1){
goto yyerrdefault;} yyn=( int)({ short* _temp7614=( short*) Cyc_yytable;
unsigned int _temp7615= yyn; if( _temp7615 >= 4223u){ _throw( Null_Exception);}
_temp7614[ _temp7615];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 750){
return 0;}({ struct _tagged_ptr2 _temp7616= yyvs; void** _temp7618= _temp7616.curr
+( ++ yyvsp_offset); if( _temp7616.base == 0? 1:( _temp7618 < _temp7616.base? 1:
_temp7618 >= _temp7616.last_plus_one)){ _throw( Null_Exception);}* _temp7618=
Cyc_yylval;});({ struct _tagged_ptr3 _temp7619= yyls; struct Cyc_Yyltype*
_temp7621= _temp7619.curr +( ++ yylsp_offset); if( _temp7619.base == 0? 1:(
_temp7621 < _temp7619.base? 1: _temp7621 >= _temp7619.last_plus_one)){ _throw(
Null_Exception);}* _temp7621= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, void* v){ void* _temp7640= v; struct _tuple15* _temp7660; struct _tuple15
_temp7662; int _temp7663; void* _temp7665; char _temp7667; short _temp7669;
struct _tagged_string _temp7671; struct Cyc_Core_Opt* _temp7673; struct Cyc_Core_Opt*
_temp7675; struct Cyc_Core_Opt _temp7677; struct _tagged_string* _temp7678;
struct _tuple1* _temp7680; struct _tuple1 _temp7682; struct _tagged_string*
_temp7683; void* _temp7685; _LL7642: if( _temp7640 == Cyc_Okay_tok){ goto
_LL7643;} else{ goto _LL7644;} _LL7644: if(*(( void**) _temp7640) == Cyc_Int_tok){
_LL7661: _temp7660=( struct _tuple15*)(( struct Cyc_Int_tok_struct*) _temp7640)->f1;
_temp7662=* _temp7660; _LL7666: _temp7665= _temp7662.f1; goto _LL7664; _LL7664:
_temp7663= _temp7662.f2; goto _LL7645;} else{ goto _LL7646;} _LL7646: if(*((
void**) _temp7640) == Cyc_Char_tok){ _LL7668: _temp7667=( char)(( struct Cyc_Char_tok_struct*)
_temp7640)->f1; goto _LL7647;} else{ goto _LL7648;} _LL7648: if(*(( void**)
_temp7640) == Cyc_Short_tok){ _LL7670: _temp7669=( short)(( struct Cyc_Short_tok_struct*)
_temp7640)->f1; goto _LL7649;} else{ goto _LL7650;} _LL7650: if(*(( void**)
_temp7640) == Cyc_String_tok){ _LL7672: _temp7671=( struct _tagged_string)((
struct Cyc_String_tok_struct*) _temp7640)->f1; goto _LL7651;} else{ goto _LL7652;}
_LL7652: if(*(( void**) _temp7640) == Cyc_Stringopt_tok){ _LL7674: _temp7673=(
struct Cyc_Core_Opt*)(( struct Cyc_Stringopt_tok_struct*) _temp7640)->f1; if(
_temp7673 == 0){ goto _LL7653;} else{ goto _LL7654;}} else{ goto _LL7654;}
_LL7654: if(*(( void**) _temp7640) == Cyc_Stringopt_tok){ _LL7676: _temp7675=(
struct Cyc_Core_Opt*)(( struct Cyc_Stringopt_tok_struct*) _temp7640)->f1; if(
_temp7675 == 0){ goto _LL7656;} else{ _temp7677=* _temp7675; _LL7679: _temp7678=(
struct _tagged_string*) _temp7677.v; goto _LL7655;}} else{ goto _LL7656;}
_LL7656: if(*(( void**) _temp7640) == Cyc_QualId_tok){ _LL7681: _temp7680=(
struct _tuple1*)(( struct Cyc_QualId_tok_struct*) _temp7640)->f1; _temp7682=*
_temp7680; _LL7686: _temp7685= _temp7682.f1; goto _LL7684; _LL7684: _temp7683=
_temp7682.f2; goto _LL7657;} else{ goto _LL7658;} _LL7658: goto _LL7659; _LL7643:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL7641; _LL7645: fprintf( Cyc_Stdio_stderr,"%d",
_temp7663); goto _LL7641; _LL7647: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp7667); goto _LL7641; _LL7649: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp7669); goto _LL7641; _LL7651:({ struct _tagged_string _temp7687= _temp7671;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp7687.last_plus_one - _temp7687.curr,
_temp7687.curr);}); goto _LL7641; _LL7653: fprintf( Cyc_Stdio_stderr,"null");
goto _LL7641; _LL7655:({ struct _tagged_string _temp7688=* _temp7678; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp7688.last_plus_one - _temp7688.curr, _temp7688.curr);});
goto _LL7641; _LL7657: { struct Cyc_List_List* _temp7689= 0; goto _LL7690;
_LL7690:{ void* _temp7691= _temp7685; struct Cyc_List_List* _temp7699; struct
Cyc_List_List* _temp7701; _LL7693: if(( unsigned int) _temp7691 > 1u?*(( int*)
_temp7691) == Cyc_Absyn_Rel_n: 0){ _LL7700: _temp7699=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp7691)->f1; goto _LL7694;} else{ goto
_LL7695;} _LL7695: if(( unsigned int) _temp7691 > 1u?*(( int*) _temp7691) == Cyc_Absyn_Abs_n:
0){ _LL7702: _temp7701=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp7691)->f1; goto _LL7696;} else{ goto _LL7697;} _LL7697: if( _temp7691 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL7698;} else{ goto _LL7692;} _LL7694: _temp7689=
_temp7699; goto _LL7692; _LL7696: _temp7689= _temp7701; goto _LL7692; _LL7698:
goto _LL7692; _LL7692:;} for( 0; _temp7689 != 0; _temp7689=({ struct Cyc_List_List*
_temp7703= _temp7689; if( _temp7703 == 0){ _throw( Null_Exception);} _temp7703->tl;})){({
struct _tagged_string _temp7705=*(( struct _tagged_string*)({ struct Cyc_List_List*
_temp7704= _temp7689; if( _temp7704 == 0){ _throw( Null_Exception);} _temp7704->hd;}));
fprintf( Cyc_Stdio_stderr,"%.*s::", _temp7705.last_plus_one - _temp7705.curr,
_temp7705.curr);});}({ struct _tagged_string _temp7706=* _temp7683; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp7706.last_plus_one - _temp7706.curr, _temp7706.curr);}); goto _LL7641;}
_LL7659: fprintf( Cyc_Stdio_stderr,"?"); goto _LL7641; _LL7641:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp7707=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp7707->v=( void*) Cyc_Lexing_from_file(
f); _temp7707;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}