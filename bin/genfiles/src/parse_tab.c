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
sizeof( struct Cyc_Core_Opt)); _temp47->v=( void*)(*(( struct _tuple1*)(( struct
Cyc_Core_Opt*) _check_null( _temp43))->v)).f2; _temp47;});{ void* _temp48=(*((
struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp43))->v)).f1; struct
Cyc_List_List* _temp58; struct Cyc_List_List* _temp60; _LL50: if(( unsigned int)
_temp48 > 1u?*(( int*) _temp48) == Cyc_Absyn_Rel_n: 0){ _LL59: _temp58=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp48)->f1; if( _temp58 == 0){
goto _LL51;} else{ goto _LL52;}} else{ goto _LL52;} _LL52: if(( unsigned int)
_temp48 > 1u?*(( int*) _temp48) == Cyc_Absyn_Abs_n: 0){ _LL61: _temp60=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp48)->f1; if( _temp60 == 0){
goto _LL53;} else{ goto _LL54;}} else{ goto _LL54;} _LL54: if( _temp48 ==( void*)
Cyc_Absyn_Loc_n){ goto _LL55;} else{ goto _LL56;} _LL56: goto _LL57; _LL51: goto
_LL49; _LL53: goto _LL49; _LL55: goto _LL49; _LL57: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp62=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp63; _temp63.curr= _temp62; _temp63.base= _temp62;
_temp63.last_plus_one= _temp62 + 49; _temp63;}), loc); goto _LL49; _LL49:;}} if(
_temp37 != 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp64=( char*)"parameter should have no type parameters";
struct _tagged_string _temp65; _temp65.curr= _temp64; _temp65.base= _temp64;
_temp65.last_plus_one= _temp64 + 41; _temp65;}), loc);} return({ struct _tuple2*
_temp66=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp66->f1=
_temp45; _temp66->f2= _temp41; _temp66->f3= _temp39; _temp66;});}} static void*
Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){ return( void*)({
struct Cyc_Parse_Type_spec_struct* _temp67=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp67[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp68; _temp68.tag= Cyc_Parse_Type_spec; _temp68.f1=(
void*) t; _temp68.f2= loc; _temp68;}); _temp67;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp69= t; struct Cyc_Absyn_Exp* _temp75; struct Cyc_Absyn_Tqual
_temp77; void* _temp79; _LL71: if(( unsigned int) _temp69 > 4u?*(( int*) _temp69)
== Cyc_Absyn_ArrayType: 0){ _LL80: _temp79=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp69)->f1; goto _LL78; _LL78: _temp77=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp69)->f2; goto _LL76; _LL76: _temp75=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp69)->f3; goto _LL72;} else{ goto _LL73;} _LL73: goto _LL74; _LL72: return
Cyc_Absyn_starb_typ( _temp79,( void*) Cyc_Absyn_HeapRgn, _temp77, _temp75 == 0?(
void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp81=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp81[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp82; _temp82.tag= Cyc_Absyn_Upper_b;
_temp82.f1=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Exp*) _check_null(
_temp75)); _temp82;}); _temp81;})); _LL74: return t; _LL70:;} static void Cyc_Parse_arg_array2ptr(
struct _tuple2* x){(* x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple10{
struct Cyc_Absyn_Tqual f1; void* f2; } ; static struct _tuple10* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple10*
_temp83=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp83->f1=(*
t).f2; _temp83->f2=(* t).f3; _temp83;});} static void Cyc_Parse_only_vardecl(
struct Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_string
decl_kind;{ void* _temp84=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp108; int
_temp110; struct Cyc_Absyn_Exp* _temp112; struct Cyc_Core_Opt* _temp114; struct
Cyc_Core_Opt* _temp116; struct Cyc_Absyn_Pat* _temp118; struct Cyc_Absyn_Fndecl*
_temp120; struct Cyc_Absyn_Structdecl* _temp122; struct Cyc_Absyn_Uniondecl*
_temp124; struct Cyc_Absyn_Tuniondecl* _temp126; struct Cyc_Absyn_Typedefdecl*
_temp128; struct Cyc_Absyn_Enumdecl* _temp130; struct Cyc_List_List* _temp132;
struct _tagged_string* _temp134; struct Cyc_List_List* _temp136; struct _tuple1*
_temp138; struct Cyc_List_List* _temp140; _LL86: if(*(( int*) _temp84) == Cyc_Absyn_Var_d){
_LL109: _temp108=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp84)->f1; goto _LL87;} else{ goto _LL88;} _LL88: if(*(( int*) _temp84) ==
Cyc_Absyn_Let_d){ _LL119: _temp118=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp84)->f1; goto _LL117; _LL117: _temp116=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp84)->f2; goto _LL115; _LL115: _temp114=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp84)->f3; goto _LL113; _LL113: _temp112=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Let_d_struct*) _temp84)->f4; goto _LL111; _LL111: _temp110=( int)((
struct Cyc_Absyn_Let_d_struct*) _temp84)->f5; goto _LL89;} else{ goto _LL90;}
_LL90: if(*(( int*) _temp84) == Cyc_Absyn_Fn_d){ _LL121: _temp120=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp84)->f1; goto _LL91;} else{ goto _LL92;}
_LL92: if(*(( int*) _temp84) == Cyc_Absyn_Struct_d){ _LL123: _temp122=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp84)->f1; goto
_LL93;} else{ goto _LL94;} _LL94: if(*(( int*) _temp84) == Cyc_Absyn_Union_d){
_LL125: _temp124=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp84)->f1; goto _LL95;} else{ goto _LL96;} _LL96: if(*(( int*) _temp84) ==
Cyc_Absyn_Tunion_d){ _LL127: _temp126=( struct Cyc_Absyn_Tuniondecl*)(( struct
Cyc_Absyn_Tunion_d_struct*) _temp84)->f1; goto _LL97;} else{ goto _LL98;} _LL98:
if(*(( int*) _temp84) == Cyc_Absyn_Typedef_d){ _LL129: _temp128=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp84)->f1; goto _LL99;} else{ goto _LL100;}
_LL100: if(*(( int*) _temp84) == Cyc_Absyn_Enum_d){ _LL131: _temp130=( struct
Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp84)->f1; goto
_LL101;} else{ goto _LL102;} _LL102: if(*(( int*) _temp84) == Cyc_Absyn_Namespace_d){
_LL135: _temp134=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp84)->f1; goto _LL133; _LL133: _temp132=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp84)->f2; goto _LL103;} else{ goto _LL104;}
_LL104: if(*(( int*) _temp84) == Cyc_Absyn_Using_d){ _LL139: _temp138=( struct
_tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp84)->f1; goto _LL137; _LL137:
_temp136=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp84)->f2;
goto _LL105;} else{ goto _LL106;} _LL106: if(*(( int*) _temp84) == Cyc_Absyn_ExternC_d){
_LL141: _temp140=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp84)->f1; goto _LL107;} else{ goto _LL85;} _LL87: if( _temp108->initializer
!= 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg))
Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp142=( char*)"initializers are not allowed in parameter declarations";
struct _tagged_string _temp143; _temp143.curr= _temp142; _temp143.base= _temp142;
_temp143.last_plus_one= _temp142 + 55; _temp143;}), x->loc);}{ void* _temp144=(*
_temp108->name).f1; struct Cyc_List_List* _temp154; struct Cyc_List_List*
_temp156; _LL146: if( _temp144 ==( void*) Cyc_Absyn_Loc_n){ goto _LL147;} else{
goto _LL148;} _LL148: if(( unsigned int) _temp144 > 1u?*(( int*) _temp144) ==
Cyc_Absyn_Rel_n: 0){ _LL155: _temp154=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp144)->f1; if( _temp154 == 0){ goto _LL149;} else{ goto _LL150;}} else{ goto
_LL150;} _LL150: if(( unsigned int) _temp144 > 1u?*(( int*) _temp144) == Cyc_Absyn_Abs_n:
0){ _LL157: _temp156=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp144)->f1; if( _temp156 == 0){ goto _LL151;} else{ goto _LL152;}} else{ goto
_LL152;} _LL152: goto _LL153; _LL147: goto _LL145; _LL149: goto _LL145; _LL151:
goto _LL145; _LL153: Cyc_Parse_err(( struct _tagged_string)({ char* _temp158=(
char*)"module names not allowed on parameter declarations"; struct
_tagged_string _temp159; _temp159.curr= _temp158; _temp159.base= _temp158;
_temp159.last_plus_one= _temp158 + 51; _temp159;}), x->loc); goto _LL145; _LL145:;}{
int found= 0; for( 0; params != 0; params=(( struct Cyc_List_List*) _check_null(
params))->tl){ if( Cyc_String_zstrptrcmp((* _temp108->name).f2,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( params))->hd) == 0){
found= 1; break;}} if( ! found){(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp160=*(*
_temp108->name).f2; xprintf("%.*s is not listed as a parameter", _temp160.last_plus_one
- _temp160.curr, _temp160.curr);}), x->loc);} return;} _LL89: decl_kind=( struct
_tagged_string)({ char* _temp161=( char*)"let declaration"; struct
_tagged_string _temp162; _temp162.curr= _temp161; _temp162.base= _temp161;
_temp162.last_plus_one= _temp161 + 16; _temp162;}); goto _LL85; _LL91: decl_kind=(
struct _tagged_string)({ char* _temp163=( char*)"function declaration"; struct
_tagged_string _temp164; _temp164.curr= _temp163; _temp164.base= _temp163;
_temp164.last_plus_one= _temp163 + 21; _temp164;}); goto _LL85; _LL93: decl_kind=(
struct _tagged_string)({ char* _temp165=( char*)"struct declaration"; struct
_tagged_string _temp166; _temp166.curr= _temp165; _temp166.base= _temp165;
_temp166.last_plus_one= _temp165 + 19; _temp166;}); goto _LL85; _LL95: decl_kind=(
struct _tagged_string)({ char* _temp167=( char*)"union declaration"; struct
_tagged_string _temp168; _temp168.curr= _temp167; _temp168.base= _temp167;
_temp168.last_plus_one= _temp167 + 18; _temp168;}); goto _LL85; _LL97: decl_kind=(
struct _tagged_string)({ char* _temp169=( char*)"tunion declaration"; struct
_tagged_string _temp170; _temp170.curr= _temp169; _temp170.base= _temp169;
_temp170.last_plus_one= _temp169 + 19; _temp170;}); goto _LL85; _LL99: decl_kind=(
struct _tagged_string)({ char* _temp171=( char*)"typedef"; struct _tagged_string
_temp172; _temp172.curr= _temp171; _temp172.base= _temp171; _temp172.last_plus_one=
_temp171 + 8; _temp172;}); goto _LL85; _LL101: decl_kind=( struct _tagged_string)({
char* _temp173=( char*)"enum declaration"; struct _tagged_string _temp174;
_temp174.curr= _temp173; _temp174.base= _temp173; _temp174.last_plus_one=
_temp173 + 17; _temp174;}); goto _LL85; _LL103: decl_kind=( struct
_tagged_string)({ char* _temp175=( char*)"namespace declaration"; struct
_tagged_string _temp176; _temp176.curr= _temp175; _temp176.base= _temp175;
_temp176.last_plus_one= _temp175 + 22; _temp176;}); goto _LL85; _LL105:
decl_kind=( struct _tagged_string)({ char* _temp177=( char*)"using declaration";
struct _tagged_string _temp178; _temp178.curr= _temp177; _temp178.base= _temp177;
_temp178.last_plus_one= _temp177 + 18; _temp178;}); goto _LL85; _LL107:
decl_kind=( struct _tagged_string)({ char* _temp179=( char*)"extern C declaration";
struct _tagged_string _temp180; _temp180.curr= _temp179; _temp180.base= _temp179;
_temp180.last_plus_one= _temp179 + 21; _temp180;}); goto _LL85; _LL85:;}(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp181= decl_kind; xprintf("%.*s appears in parameter type",
_temp181.last_plus_one - _temp181.curr, _temp181.curr);}), x->loc); return;}
struct _tuple11{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ;
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple11* env, struct
_tagged_string* x){ struct _tuple11 _temp184; struct Cyc_Position_Segment*
_temp185; struct Cyc_List_List* _temp187; struct _tuple11* _temp182= env;
_temp184=* _temp182; _LL188: _temp187= _temp184.f1; goto _LL186; _LL186:
_temp185= _temp184.f2; goto _LL183; _LL183: if( _temp187 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp189=* x; xprintf("missing type for parameter %.*s",
_temp189.last_plus_one - _temp189.curr, _temp189.curr);}), _temp185);}{ void*
_temp190=( void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null(
_temp187))->hd)->r; struct Cyc_Absyn_Vardecl* _temp196; _LL192: if(*(( int*)
_temp190) == Cyc_Absyn_Var_d){ _LL197: _temp196=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp190)->f1; goto _LL193;} else{ goto _LL194;}
_LL194: goto _LL195; _LL193:{ void* _temp198=(* _temp196->name).f1; struct Cyc_List_List*
_temp208; struct Cyc_List_List* _temp210; _LL200: if( _temp198 ==( void*) Cyc_Absyn_Loc_n){
goto _LL201;} else{ goto _LL202;} _LL202: if(( unsigned int) _temp198 > 1u?*((
int*) _temp198) == Cyc_Absyn_Rel_n: 0){ _LL209: _temp208=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp198)->f1; if( _temp208 == 0){ goto _LL203;}
else{ goto _LL204;}} else{ goto _LL204;} _LL204: if(( unsigned int) _temp198 > 1u?*((
int*) _temp198) == Cyc_Absyn_Abs_n: 0){ _LL211: _temp210=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp198)->f1; if( _temp210 == 0){ goto _LL205;}
else{ goto _LL206;}} else{ goto _LL206;} _LL206: goto _LL207; _LL201: goto
_LL199; _LL203: goto _LL199; _LL205: goto _LL199; _LL207: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp212=( char*)"module name not allowed on parameter";
struct _tagged_string _temp213; _temp213.curr= _temp212; _temp213.base= _temp212;
_temp213.last_plus_one= _temp212 + 37; _temp213;}), _temp185); goto _LL199;
_LL199:;} if( Cyc_String_zstrptrcmp((* _temp196->name).f2, x) == 0){ return({
struct _tuple2* _temp214=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp214[ 0]=({ struct _tuple2 _temp215; _temp215.f1=({ struct Cyc_Core_Opt*
_temp216=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp216[ 0]=({ struct Cyc_Core_Opt _temp217; _temp217.v=( void*)(* _temp196->name).f2;
_temp217;}); _temp216;}); _temp215.f2= _temp196->tq; _temp215.f3=( void*)
_temp196->type; _temp215;}); _temp214;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple11* _temp218=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp218->f1=(( struct Cyc_List_List*) _check_null( _temp187))->tl; _temp218->f2=
_temp185; _temp218;}), x);} _LL195: return(( struct _tuple2*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp219=( char*)"non-variable declaration"; struct
_tagged_string _temp220; _temp220.curr= _temp219; _temp220.base= _temp219;
_temp220.last_plus_one= _temp219 + 25; _temp220;}),(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp187))->hd)->loc); _LL191:;}} static int
Cyc_Parse_is_typeparam( void* tm){ void* _temp221= tm; int _temp227; struct Cyc_Position_Segment*
_temp229; struct Cyc_List_List* _temp231; _LL223: if(( unsigned int) _temp221 >
1u?*(( int*) _temp221) == Cyc_Absyn_TypeParams_mod: 0){ _LL232: _temp231=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp221)->f1;
goto _LL230; _LL230: _temp229=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp221)->f2; goto _LL228; _LL228: _temp227=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp221)->f3; goto _LL224;} else{ goto _LL225;} _LL225: goto _LL226; _LL224:
return 1; _LL226: return 0; _LL222:;} static void* Cyc_Parse_id2type( struct
_tagged_string s, struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp233=( char*)"`H"; struct _tagged_string
_temp234; _temp234.curr= _temp233; _temp234.base= _temp233; _temp234.last_plus_one=
_temp233 + 3; _temp234;})) == 0){ return( void*) Cyc_Absyn_HeapRgn;} else{
return( void*)({ struct Cyc_Absyn_VarType_struct* _temp235=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp235[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp236; _temp236.tag= Cyc_Absyn_VarType; _temp236.f1=({ struct Cyc_Absyn_Tvar*
_temp237=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp237->name=({ struct _tagged_string* _temp238=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp238[ 0]= s; _temp238;});
_temp237->kind= k; _temp237;}); _temp236;}); _temp235;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp239=
t; struct Cyc_Absyn_Tvar* _temp245; _LL241: if(( unsigned int) _temp239 > 4u?*((
int*) _temp239) == Cyc_Absyn_VarType: 0){ _LL246: _temp245=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp239)->f1; goto _LL242;} else{ goto _LL243;}
_LL243: goto _LL244; _LL242: return _temp245; _LL244: return(( struct Cyc_Absyn_Tvar*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp247=( char*)"expecting a list of type variables, not types";
struct _tagged_string _temp248; _temp248.curr= _temp247; _temp248.base= _temp247;
_temp248.last_plus_one= _temp247 + 46; _temp248;}), loc); _LL240:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp249=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp249[ 0]=({ struct Cyc_Absyn_VarType_struct _temp250; _temp250.tag= Cyc_Absyn_VarType;
_temp250.f1= pr; _temp250;}); _temp249;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp251=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; void* _temp257; _LL253:
if(( unsigned int) _temp251 > 1u?*(( int*) _temp251) == Cyc_Absyn_Function_mod:
0){ _LL258: _temp257=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp251)->f1;
goto _LL254;} else{ goto _LL255;} _LL255: goto _LL256; _LL254: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
== 0: 0)){ void* _temp259= _temp257; struct Cyc_Core_Opt* _temp265; int _temp267;
struct Cyc_List_List* _temp269; struct Cyc_Position_Segment* _temp271; struct
Cyc_List_List* _temp273; _LL261: if(*(( int*) _temp259) == Cyc_Absyn_WithTypes){
_LL270: _temp269=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp259)->f1; goto _LL268; _LL268: _temp267=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp259)->f2; goto _LL266; _LL266: _temp265=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp259)->f3; goto _LL262;} else{ goto _LL263;}
_LL263: if(*(( int*) _temp259) == Cyc_Absyn_NoTypes){ _LL274: _temp273=( struct
Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp259)->f1; goto _LL272;
_LL272: _temp271=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp259)->f2; goto _LL264;} else{ goto _LL260;} _LL262: Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp275=( char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp276; _temp276.curr= _temp275; _temp276.base= _temp275;
_temp276.last_plus_one= _temp275 + 92; _temp276;}), loc); return tms; _LL264:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp273, tds);{ struct _tuple11* _temp278=({ struct _tuple11* _temp277=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp277->f1= tds; _temp277->f2=
loc; _temp277;}); goto _LL279; _LL279: return({ struct Cyc_List_List* _temp280=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp280->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp281=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp281[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp282; _temp282.tag= Cyc_Absyn_Function_mod;
_temp282.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp283=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp283[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp284; _temp284.tag= Cyc_Absyn_WithTypes;
_temp284.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple11*,
struct _tagged_string*), struct _tuple11* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp278, _temp273); _temp284.f2= 0; _temp284.f3= 0;
_temp284;}); _temp283;})); _temp282;}); _temp281;})); _temp280->tl= 0; _temp280;});}
_LL260:;} else{ return({ struct Cyc_List_List* _temp285=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp285->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( tms))->hd); _temp285->tl= Cyc_Parse_oldstyle2newstyle(((
struct Cyc_List_List*) _check_null( tms))->tl, tds, loc); _temp285;});} _LL256:
return({ struct Cyc_List_List* _temp286=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp286->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd); _temp286->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*)
_check_null( tms))->tl, tds, loc); _temp286;}); _LL252:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp287=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp287->id= d->id; _temp287->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp287;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ void* _temp288=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL290: if( _temp288 ==(
void*) Cyc_Parse_Extern_sc){ goto _LL291;} else{ goto _LL292;} _LL292: if(
_temp288 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL293;} else{ goto _LL294;}
_LL294: if( _temp288 ==( void*) Cyc_Parse_Static_sc){ goto _LL295;} else{ goto
_LL296;} _LL296: goto _LL297; _LL291: sc=( void*) Cyc_Absyn_Extern; goto _LL289;
_LL293: sc=( void*) Cyc_Absyn_ExternC; goto _LL289; _LL295: sc=( void*) Cyc_Absyn_Static;
goto _LL289; _LL297: Cyc_Parse_err(( struct _tagged_string)({ char* _temp298=(
char*)"bad storage class on function"; struct _tagged_string _temp299; _temp299.curr=
_temp298; _temp299.base= _temp298; _temp299.last_plus_one= _temp298 + 30;
_temp299;}), loc); goto _LL289; _LL289:;}}{ struct Cyc_Core_Opt* _temp302; void*
_temp304; struct _tuple5 _temp300= Cyc_Parse_collapse_type_specifiers( tss, loc);
_LL305: _temp304= _temp300.f1; goto _LL303; _LL303: _temp302= _temp300.f2; goto
_LL301; _LL301: { struct Cyc_List_List* _temp308; struct Cyc_List_List* _temp310;
void* _temp312; struct Cyc_Absyn_Tqual _temp314; struct _tuple6 _temp306= Cyc_Parse_apply_tms(
tq, _temp304, atts, d->tms); _LL315: _temp314= _temp306.f1; goto _LL313; _LL313:
_temp312= _temp306.f2; goto _LL311; _LL311: _temp310= _temp306.f3; goto _LL309;
_LL309: _temp308= _temp306.f4; goto _LL307; _LL307: if( _temp302 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp316=( char*)"nested type declaration within function prototype";
struct _tagged_string _temp317; _temp317.curr= _temp316; _temp317.base= _temp316;
_temp317.last_plus_one= _temp316 + 50; _temp317;}), loc);} if( _temp310 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp318=( char*)"bad type params, ignoring";
struct _tagged_string _temp319; _temp319.curr= _temp318; _temp319.base= _temp318;
_temp319.last_plus_one= _temp318 + 26; _temp319;}), loc);}{ void* _temp320=
_temp312; struct Cyc_Absyn_FnInfo _temp326; struct Cyc_List_List* _temp328; int
_temp330; struct Cyc_List_List* _temp332; void* _temp334; struct Cyc_Core_Opt*
_temp336; struct Cyc_List_List* _temp338; _LL322: if(( unsigned int) _temp320 >
4u?*(( int*) _temp320) == Cyc_Absyn_FnType: 0){ _LL327: _temp326=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp320)->f1; _LL339: _temp338=( struct Cyc_List_List*)
_temp326.tvars; goto _LL337; _LL337: _temp336=( struct Cyc_Core_Opt*) _temp326.effect;
goto _LL335; _LL335: _temp334=( void*) _temp326.ret_typ; goto _LL333; _LL333:
_temp332=( struct Cyc_List_List*) _temp326.args; goto _LL331; _LL331: _temp330=(
int) _temp326.varargs; goto _LL329; _LL329: _temp328=( struct Cyc_List_List*)
_temp326.attributes; goto _LL323;} else{ goto _LL324;} _LL324: goto _LL325;
_LL323: { struct Cyc_List_List* _temp340=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc,
_temp332); goto _LL341; _LL341: return({ struct Cyc_Absyn_Fndecl* _temp342=(
struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl)); _temp342->sc=(
void*) sc; _temp342->name= d->id; _temp342->tvs= _temp338; _temp342->is_inline=
is_inline; _temp342->effect= _temp336; _temp342->ret_type=( void*) _temp334;
_temp342->args= _temp340; _temp342->varargs= _temp330; _temp342->body= body;
_temp342->cached_typ= 0; _temp342->param_vardecls= 0; _temp342->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp328, _temp308); _temp342;});} _LL325: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp343=( char*)"declarator is not a function prototype";
struct _tagged_string _temp344; _temp344.curr= _temp343; _temp344.base= _temp343;
_temp344.last_plus_one= _temp343 + 39; _temp344;}), loc); _LL321:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp345=( char*)"missing argument variable in function prototype"; struct
_tagged_string _temp346; _temp346.curr= _temp345; _temp346.base= _temp345;
_temp346.last_plus_one= _temp345 + 48; _temp346;}), loc); return({ struct
_tuple4* _temp347=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp347->f1=({ struct _tagged_string* _temp348=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp348[ 0]=( struct
_tagged_string)({ char* _temp349=( char*)"?"; struct _tagged_string _temp350;
_temp350.curr= _temp349; _temp350.base= _temp349; _temp350.last_plus_one=
_temp349 + 2; _temp350;}); _temp348;}); _temp347->f2=(* t).f2; _temp347->f3=(* t).f3;
_temp347;});} else{ return({ struct _tuple4* _temp351=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp351->f1=( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null((* t).f1))->v; _temp351->f2=(* t).f2; _temp351->f3=(*
t).f3; _temp351;});}} static char _temp354[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_string Cyc_Parse_msg1=( struct _tagged_string){ _temp354,
_temp354, _temp354 + 52u}; static char _temp357[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2=( struct _tagged_string){ _temp357,
_temp357, _temp357 + 63u}; static char _temp360[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3=( struct _tagged_string){ _temp360,
_temp360, _temp360 + 50u}; static char _temp363[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4=( struct _tagged_string){ _temp363,
_temp363, _temp363 + 60u}; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
declopt= 0; int seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*)
Cyc_Absyn_VoidType; void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed;
struct Cyc_Position_Segment* last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*)
_check_null( ts))->tl){ void* _temp364=( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd; struct Cyc_Position_Segment* _temp378; void* _temp380;
struct Cyc_Position_Segment* _temp382; struct Cyc_Position_Segment* _temp384;
struct Cyc_Position_Segment* _temp386; struct Cyc_Position_Segment* _temp388;
struct Cyc_Absyn_Decl* _temp390; _LL366: if(*(( int*) _temp364) == Cyc_Parse_Type_spec){
_LL381: _temp380=( void*)(( struct Cyc_Parse_Type_spec_struct*) _temp364)->f1;
goto _LL379; _LL379: _temp378=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Type_spec_struct*)
_temp364)->f2; goto _LL367;} else{ goto _LL368;} _LL368: if(*(( int*) _temp364)
== Cyc_Parse_Signed_spec){ _LL383: _temp382=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Signed_spec_struct*) _temp364)->f1; goto _LL369;} else{ goto
_LL370;} _LL370: if(*(( int*) _temp364) == Cyc_Parse_Unsigned_spec){ _LL385:
_temp384=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Unsigned_spec_struct*)
_temp364)->f1; goto _LL371;} else{ goto _LL372;} _LL372: if(*(( int*) _temp364)
== Cyc_Parse_Short_spec){ _LL387: _temp386=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Short_spec_struct*) _temp364)->f1; goto _LL373;} else{ goto
_LL374;} _LL374: if(*(( int*) _temp364) == Cyc_Parse_Long_spec){ _LL389:
_temp388=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Long_spec_struct*)
_temp364)->f1; goto _LL375;} else{ goto _LL376;} _LL376: if(*(( int*) _temp364)
== Cyc_Parse_Decl_spec){ _LL391: _temp390=( struct Cyc_Absyn_Decl*)(( struct Cyc_Parse_Decl_spec_struct*)
_temp364)->f1; goto _LL377;} else{ goto _LL365;} _LL367: if( seen_type){ Cyc_Parse_err(
Cyc_Parse_msg1, _temp378);} last_loc= _temp378; seen_type= 1; t= _temp380; goto
_LL365; _LL369: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp382);} if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp392=( char*)"signed qualifier must come before type";
struct _tagged_string _temp393; _temp393.curr= _temp392; _temp393.base= _temp392;
_temp393.last_plus_one= _temp392 + 39; _temp393;}), _temp382);} last_loc=
_temp382; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed; goto _LL365; _LL371: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp384);} if( seen_type){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp394=( char*)"signed qualifier must come before type";
struct _tagged_string _temp395; _temp395.curr= _temp394; _temp395.base= _temp394;
_temp395.last_plus_one= _temp394 + 39; _temp395;}), _temp384);} last_loc=
_temp384; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL365; _LL373:
if( seen_size){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp396=( char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp397; _temp397.curr= _temp396; _temp397.base= _temp396;
_temp397.last_plus_one= _temp396 + 59; _temp397;}), _temp386);} if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp398=( char*)"short modifier must come before base type";
struct _tagged_string _temp399; _temp399.curr= _temp398; _temp399.base= _temp398;
_temp399.last_plus_one= _temp398 + 42; _temp399;}), _temp386);} last_loc=
_temp386; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL365; _LL375: if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp400=( char*)"long modifier must come before base type";
struct _tagged_string _temp401; _temp401.curr= _temp400; _temp401.base= _temp400;
_temp401.last_plus_one= _temp400 + 41; _temp401;}), _temp388);} if( seen_size){
void* _temp402= sz; _LL404: if( _temp402 ==( void*) Cyc_Absyn_B4){ goto _LL405;}
else{ goto _LL406;} _LL406: goto _LL407; _LL405: sz=( void*) Cyc_Absyn_B8; goto
_LL403; _LL407: Cyc_Parse_err(( struct _tagged_string)({ char* _temp408=( char*)"extra long in type specifier";
struct _tagged_string _temp409; _temp409.curr= _temp408; _temp409.base= _temp408;
_temp409.last_plus_one= _temp408 + 29; _temp409;}), _temp388); goto _LL403;
_LL403:;} last_loc= _temp388; seen_size= 1; goto _LL365; _LL377: last_loc=
_temp390->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp410=(
void*) _temp390->r; struct Cyc_Absyn_Structdecl* _temp422; struct Cyc_Absyn_Tuniondecl*
_temp424; struct Cyc_Absyn_Uniondecl* _temp426; struct Cyc_Absyn_Enumdecl*
_temp428; _LL412: if(*(( int*) _temp410) == Cyc_Absyn_Struct_d){ _LL423:
_temp422=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp410)->f1; goto _LL413;} else{ goto _LL414;} _LL414: if(*(( int*) _temp410)
== Cyc_Absyn_Tunion_d){ _LL425: _temp424=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp410)->f1; goto _LL415;} else{ goto
_LL416;} _LL416: if(*(( int*) _temp410) == Cyc_Absyn_Union_d){ _LL427: _temp426=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp410)->f1;
goto _LL417;} else{ goto _LL418;} _LL418: if(*(( int*) _temp410) == Cyc_Absyn_Enum_d){
_LL429: _temp428=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp410)->f1; goto _LL419;} else{ goto _LL420;} _LL420: goto _LL421; _LL413: {
struct Cyc_List_List* _temp430=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ,
_temp422->tvs); goto _LL431; _LL431: t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp432=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp432[ 0]=({ struct Cyc_Absyn_StructType_struct _temp433; _temp433.tag= Cyc_Absyn_StructType;
_temp433.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp422->name))->v); _temp433.f2= _temp430; _temp433.f3= 0;
_temp433;}); _temp432;}); if( _temp422->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp434=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp434->v=( void*) _temp390; _temp434;});} goto _LL411;} _LL415: { struct Cyc_List_List*
_temp435=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp424->tvs);
goto _LL436; _LL436: t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp437=(
struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp437[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp438; _temp438.tag= Cyc_Absyn_TunionType;
_temp438.f1=({ struct Cyc_Absyn_TunionInfo _temp439; _temp439.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp440=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp440[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp441; _temp441.tag= Cyc_Absyn_KnownTunion;
_temp441.f1= _temp424; _temp441;}); _temp440;})); _temp439.targs= _temp435;
_temp439.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp439;}); _temp438;});
_temp437;}); if( _temp424->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp442=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp442->v=( void*) _temp390; _temp442;});} goto _LL411;} _LL417: { struct Cyc_List_List*
_temp443=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp426->tvs);
goto _LL444; _LL444: t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp445=(
struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp445[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp446; _temp446.tag= Cyc_Absyn_UnionType;
_temp446.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp426->name))->v); _temp446.f2= _temp443; _temp446.f3= 0;
_temp446;}); _temp445;}); if( _temp426->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp447=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp447->v=( void*) _temp390; _temp447;});} goto _LL411;} _LL419: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp448=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp448[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp449; _temp449.tag= Cyc_Absyn_EnumType; _temp449.f1=
_temp428->name; _temp449.f2= 0; _temp449;}); _temp448;}); declopt=({ struct Cyc_Core_Opt*
_temp450=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp450->v=( void*) _temp390; _temp450;}); goto _LL411; _LL421:(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp451=( char*)"bad declaration within type specifier";
struct _tagged_string _temp452; _temp452.curr= _temp451; _temp452.base= _temp451;
_temp452.last_plus_one= _temp451 + 38; _temp452;}), _temp390->loc); goto _LL411;
_LL411:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp390->loc);} goto _LL365;
_LL365:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp453=( char*)"missing type withing specifier";
struct _tagged_string _temp454; _temp454.curr= _temp453; _temp454.base= _temp453;
_temp454.last_plus_one= _temp453 + 31; _temp454;}), last_loc);} t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp455=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp455[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp456; _temp456.tag= Cyc_Absyn_IntType; _temp456.f1=( void*) sgn; _temp456.f2=(
void*) sz; _temp456;}); _temp455;});} else{ if( seen_sign){ void* _temp457= t;
void* _temp463; void* _temp465; _LL459: if(( unsigned int) _temp457 > 4u?*(( int*)
_temp457) == Cyc_Absyn_IntType: 0){ _LL466: _temp465=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp457)->f1; goto _LL464; _LL464: _temp463=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp457)->f2; goto _LL460;} else{ goto _LL461;} _LL461: goto _LL462; _LL460: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp467=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp467[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp468; _temp468.tag= Cyc_Absyn_IntType; _temp468.f1=( void*) sgn; _temp468.f2=(
void*) _temp463; _temp468;}); _temp467;}); goto _LL458; _LL462: Cyc_Parse_err((
struct _tagged_string)({ char* _temp469=( char*)"sign specification on non-integral type";
struct _tagged_string _temp470; _temp470.curr= _temp469; _temp470.base= _temp469;
_temp470.last_plus_one= _temp469 + 40; _temp470;}), last_loc); goto _LL458;
_LL458:;} if( seen_size){ void* _temp471= t; void* _temp477; void* _temp479;
_LL473: if(( unsigned int) _temp471 > 4u?*(( int*) _temp471) == Cyc_Absyn_IntType:
0){ _LL480: _temp479=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp471)->f1;
goto _LL478; _LL478: _temp477=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp471)->f2; goto _LL474;} else{ goto _LL475;} _LL475: goto _LL476; _LL474: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp481=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp481[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp482; _temp482.tag= Cyc_Absyn_IntType; _temp482.f1=( void*) _temp479;
_temp482.f2=( void*) sz; _temp482;}); _temp481;}); goto _LL472; _LL476: Cyc_Parse_err((
struct _tagged_string)({ char* _temp483=( char*)"size qualifier on non-integral type";
struct _tagged_string _temp484; _temp484.curr= _temp483; _temp484.base= _temp483;
_temp484.last_plus_one= _temp483 + 36; _temp484;}), last_loc); goto _LL472;
_LL472:;}} return({ struct _tuple5 _temp485; _temp485.f1= t; _temp485.f2=
declopt; _temp485;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp486=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
goto _LL487; _LL487: { struct _tuple1* _temp488= _temp486->id; goto _LL489;
_LL489: { struct Cyc_List_List* _temp492; struct Cyc_List_List* _temp494; void*
_temp496; struct Cyc_Absyn_Tqual _temp498; struct _tuple6 _temp490= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp486->tms); _LL499: _temp498= _temp490.f1; goto _LL497;
_LL497: _temp496= _temp490.f2; goto _LL495; _LL495: _temp494= _temp490.f3; goto
_LL493; _LL493: _temp492= _temp490.f4; goto _LL491; _LL491: return({ struct Cyc_List_List*
_temp500=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp500->hd=( void*)({ struct _tuple7* _temp501=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp501->f1= _temp488; _temp501->f2= _temp498;
_temp501->f3= _temp496; _temp501->f4= _temp494; _temp501->f5= _temp492; _temp501;});
_temp500->tl= Cyc_Parse_apply_tmss( _temp498, t,(( struct Cyc_List_List*)
_check_null( ds))->tl, shared_atts); _temp500;});}}}} static int Cyc_Parse_fn_type_att(
void* a){ void* _temp502= a; int _temp516; _LL504: if(( unsigned int) _temp502 >
15u?*(( int*) _temp502) == Cyc_Absyn_Regparm_att: 0){ _LL517: _temp516=( int)((
struct Cyc_Absyn_Regparm_att_struct*) _temp502)->f1; goto _LL505;} else{ goto
_LL506;} _LL506: if( _temp502 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL507;}
else{ goto _LL508;} _LL508: if( _temp502 ==( void*) Cyc_Absyn_Cdecl_att){ goto
_LL509;} else{ goto _LL510;} _LL510: if( _temp502 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL511;} else{ goto _LL512;} _LL512: if( _temp502 ==( void*) Cyc_Absyn_Const_att){
goto _LL513;} else{ goto _LL514;} _LL514: goto _LL515; _LL505: goto _LL507;
_LL507: goto _LL509; _LL509: goto _LL511; _LL511: goto _LL513; _LL513: return 1;
_LL515: return 0; _LL503:;} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp518; _temp518.f1= tq; _temp518.f2=
t; _temp518.f3= 0; _temp518.f4= atts; _temp518;});}{ void* _temp519=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp533;
void* _temp535; int _temp537; struct Cyc_Position_Segment* _temp539; struct Cyc_List_List*
_temp541; struct Cyc_Absyn_Tqual _temp543; void* _temp545; void* _temp547;
struct Cyc_List_List* _temp549; struct Cyc_Position_Segment* _temp551; _LL521:
if( _temp519 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL522;} else{ goto _LL523;}
_LL523: if(( unsigned int) _temp519 > 1u?*(( int*) _temp519) == Cyc_Absyn_ConstArray_mod:
0){ _LL534: _temp533=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp519)->f1; goto _LL524;} else{ goto _LL525;} _LL525: if(( unsigned int)
_temp519 > 1u?*(( int*) _temp519) == Cyc_Absyn_Function_mod: 0){ _LL536:
_temp535=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp519)->f1; goto
_LL526;} else{ goto _LL527;} _LL527: if(( unsigned int) _temp519 > 1u?*(( int*)
_temp519) == Cyc_Absyn_TypeParams_mod: 0){ _LL542: _temp541=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp519)->f1; goto _LL540; _LL540:
_temp539=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp519)->f2; goto _LL538; _LL538: _temp537=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp519)->f3; goto _LL528;} else{ goto _LL529;} _LL529: if(( unsigned int)
_temp519 > 1u?*(( int*) _temp519) == Cyc_Absyn_Pointer_mod: 0){ _LL548: _temp547=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp519)->f1; goto _LL546;
_LL546: _temp545=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp519)->f2;
goto _LL544; _LL544: _temp543=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp519)->f3; goto _LL530;} else{ goto _LL531;} _LL531: if(( unsigned int)
_temp519 > 1u?*(( int*) _temp519) == Cyc_Absyn_Attributes_mod: 0){ _LL552:
_temp551=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp519)->f1; goto _LL550; _LL550: _temp549=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp519)->f2; goto _LL532;} else{ goto _LL520;}
_LL522: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp553=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp553[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp554; _temp554.tag= Cyc_Absyn_ArrayType; _temp554.f1=(
void*) t; _temp554.f2= tq; _temp554.f3= 0; _temp554;}); _temp553;}), atts,((
struct Cyc_List_List*) _check_null( tms))->tl); _LL524: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp555=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp555[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp556; _temp556.tag= Cyc_Absyn_ArrayType;
_temp556.f1=( void*) t; _temp556.f2= tq; _temp556.f3=( struct Cyc_Absyn_Exp*)
_temp533; _temp556;}); _temp555;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL526: { void* _temp557= _temp535; struct Cyc_Core_Opt* _temp563;
int _temp565; struct Cyc_List_List* _temp567; struct Cyc_Position_Segment*
_temp569; struct Cyc_List_List* _temp571; _LL559: if(*(( int*) _temp557) == Cyc_Absyn_WithTypes){
_LL568: _temp567=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp557)->f1; goto _LL566; _LL566: _temp565=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp557)->f2; goto _LL564; _LL564: _temp563=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp557)->f3; goto _LL560;} else{ goto _LL561;}
_LL561: if(*(( int*) _temp557) == Cyc_Absyn_NoTypes){ _LL572: _temp571=( struct
Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp557)->f1; goto _LL570;
_LL570: _temp569=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp557)->f2; goto _LL562;} else{ goto _LL558;} _LL560: { struct Cyc_List_List*
typvars= 0; struct Cyc_List_List* fn_atts= 0; struct Cyc_List_List* new_atts= 0;{
struct Cyc_List_List* as= atts; for( 0; as != 0; as=(( struct Cyc_List_List*)
_check_null( as))->tl){ if( Cyc_Parse_fn_type_att(( void*)(( struct Cyc_List_List*)
_check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List* _temp573=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp573->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( as))->hd); _temp573->tl= fn_atts; _temp573;});}
else{ new_atts=({ struct Cyc_List_List* _temp574=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp574->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( as))->hd); _temp574->tl= new_atts; _temp574;});}}}
if((( struct Cyc_List_List*) _check_null( tms))->tl != 0){ void* _temp575=( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd;
int _temp581; struct Cyc_Position_Segment* _temp583; struct Cyc_List_List*
_temp585; _LL577: if(( unsigned int) _temp575 > 1u?*(( int*) _temp575) == Cyc_Absyn_TypeParams_mod:
0){ _LL586: _temp585=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp575)->f1; goto _LL584; _LL584: _temp583=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp575)->f2; goto _LL582; _LL582:
_temp581=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp575)->f3; goto
_LL578;} else{ goto _LL579;} _LL579: goto _LL580; _LL578: typvars= _temp585; tms=((
struct Cyc_List_List*) _check_null( tms))->tl; goto _LL576; _LL580: goto _LL576;
_LL576:;} if(((( ! _temp565? _temp567 != 0: 0)?(( struct Cyc_List_List*)
_check_null( _temp567))->tl == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp567))->hd)).f1 == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp567))->hd)).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp567= 0;
_temp565= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp567);
return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp563, t, _temp567, _temp565, fn_atts), new_atts,(( struct Cyc_List_List*)
_check_null( tms))->tl);} _LL562:( void) _throw((( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp587=( char*)"function declaration without parameter types";
struct _tagged_string _temp588; _temp588.curr= _temp587; _temp588.base= _temp587;
_temp588.last_plus_one= _temp587 + 45; _temp588;}), _temp569)); _LL558:;} _LL528:
if((( struct Cyc_List_List*) _check_null( tms))->tl == 0){ return({ struct
_tuple6 _temp589; _temp589.f1= tq; _temp589.f2= t; _temp589.f3= _temp541;
_temp589.f4= atts; _temp589;});}( void) _throw((( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp590=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp591; _temp591.curr= _temp590; _temp591.base= _temp590;
_temp591.last_plus_one= _temp590 + 68; _temp591;}), _temp539)); _LL530: { void*
_temp592= _temp547; struct Cyc_Absyn_Exp* _temp600; struct Cyc_Absyn_Exp*
_temp602; _LL594: if(( unsigned int) _temp592 > 1u?*(( int*) _temp592) == Cyc_Absyn_NonNullable_ps:
0){ _LL601: _temp600=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp592)->f1; goto _LL595;} else{ goto _LL596;} _LL596: if(( unsigned int)
_temp592 > 1u?*(( int*) _temp592) == Cyc_Absyn_Nullable_ps: 0){ _LL603: _temp602=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp592)->f1;
goto _LL597;} else{ goto _LL598;} _LL598: if( _temp592 ==( void*) Cyc_Absyn_TaggedArray_ps){
goto _LL599;} else{ goto _LL593;} _LL595: return Cyc_Parse_apply_tms( _temp543,
Cyc_Absyn_atb_typ( t, _temp545, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp604=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp604[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp605; _temp605.tag= Cyc_Absyn_Upper_b;
_temp605.f1= _temp600; _temp605;}); _temp604;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL597: return Cyc_Parse_apply_tms( _temp543, Cyc_Absyn_starb_typ(
t, _temp545, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp606=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp606[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp607; _temp607.tag= Cyc_Absyn_Upper_b;
_temp607.f1= _temp602; _temp607;}); _temp606;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL599: return Cyc_Parse_apply_tms( _temp543, Cyc_Absyn_tagged_typ(
t, _temp545, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL593:;}
_LL532: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp549),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL520:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp610; void* _temp612; struct _tuple5 _temp608= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL613: _temp612= _temp608.f1; goto _LL611; _LL611: _temp610=
_temp608.f2; goto _LL609; _LL609: if( _temp610 != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp614=( char*)"ignoring nested type declaration(s) in specifier list";
struct _tagged_string _temp615; _temp615.curr= _temp614; _temp615.base= _temp614;
_temp615.last_plus_one= _temp614 + 54; _temp615;}), loc);} return _temp612;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp616=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp616[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp617; _temp617.tag= Cyc_Absyn_Decl_s;
_temp617.f1= d; _temp617.f2= s; _temp617;}); _temp616;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp618=( char*)"inline is only allowed on function definitions"; struct
_tagged_string _temp619; _temp619.curr= _temp618; _temp619.base= _temp618;
_temp619.last_plus_one= _temp618 + 47; _temp619;}), loc);} if( tss == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp620=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp621; _temp621.curr= _temp620; _temp621.base= _temp620;
_temp621.last_plus_one= _temp620 + 39; _temp621;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp622=( void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v;
_LL624: if( _temp622 ==( void*) Cyc_Parse_Typedef_sc){ goto _LL625;} else{ goto
_LL626;} _LL626: if( _temp622 ==( void*) Cyc_Parse_Extern_sc){ goto _LL627;}
else{ goto _LL628;} _LL628: if( _temp622 ==( void*) Cyc_Parse_ExternC_sc){ goto
_LL629;} else{ goto _LL630;} _LL630: if( _temp622 ==( void*) Cyc_Parse_Static_sc){
goto _LL631;} else{ goto _LL632;} _LL632: if( _temp622 ==( void*) Cyc_Parse_Auto_sc){
goto _LL633;} else{ goto _LL634;} _LL634: if( _temp622 ==( void*) Cyc_Parse_Register_sc){
goto _LL635;} else{ goto _LL636;} _LL636: if( _temp622 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL637;} else{ goto _LL623;} _LL625: istypedef= 1; goto _LL623; _LL627: s=(
void*) Cyc_Absyn_Extern; goto _LL623; _LL629: s=( void*) Cyc_Absyn_ExternC; goto
_LL623; _LL631: s=( void*) Cyc_Absyn_Static; goto _LL623; _LL633: s=( void*) Cyc_Absyn_Public;
goto _LL623; _LL635: s=( void*) Cyc_Absyn_Public; goto _LL623; _LL637: s=( void*)
Cyc_Absyn_Abstract; goto _LL623; _LL623:;}{ struct Cyc_List_List* _temp640;
struct Cyc_List_List* _temp642; struct _tuple0 _temp638=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL643: _temp642= _temp638.f1;
goto _LL641; _LL641: _temp640= _temp638.f2; goto _LL639; _LL639: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp640; for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp644= Cyc_Parse_collapse_type_specifiers( tss, loc); goto
_LL645; _LL645: if( _temp642 == 0){ struct Cyc_Core_Opt* _temp648; void*
_temp650; struct _tuple5 _temp646= _temp644; _LL651: _temp650= _temp646.f1; goto
_LL649; _LL649: _temp648= _temp646.f2; goto _LL647; _LL647: if( _temp648 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp648))->v;{ void* _temp652=( void*) d->r; struct Cyc_Absyn_Enumdecl*
_temp664; struct Cyc_Absyn_Structdecl* _temp666; struct Cyc_Absyn_Uniondecl*
_temp668; struct Cyc_Absyn_Tuniondecl* _temp670; _LL654: if(*(( int*) _temp652)
== Cyc_Absyn_Enum_d){ _LL665: _temp664=( struct Cyc_Absyn_Enumdecl*)(( struct
Cyc_Absyn_Enum_d_struct*) _temp652)->f1; goto _LL655;} else{ goto _LL656;}
_LL656: if(*(( int*) _temp652) == Cyc_Absyn_Struct_d){ _LL667: _temp666=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp652)->f1; goto
_LL657;} else{ goto _LL658;} _LL658: if(*(( int*) _temp652) == Cyc_Absyn_Union_d){
_LL669: _temp668=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp652)->f1; goto _LL659;} else{ goto _LL660;} _LL660: if(*(( int*) _temp652)
== Cyc_Absyn_Tunion_d){ _LL671: _temp670=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp652)->f1; goto _LL661;} else{ goto
_LL662;} _LL662: goto _LL663; _LL655:( void*)( _temp664->sc=( void*) s); if(
atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp672=( char*)"bad attributes on enum";
struct _tagged_string _temp673; _temp673.curr= _temp672; _temp673.base= _temp672;
_temp673.last_plus_one= _temp672 + 23; _temp673;}), loc);} goto _LL653; _LL657:(
void*)( _temp666->sc=( void*) s); _temp666->attributes= atts; goto _LL653;
_LL659:( void*)( _temp668->sc=( void*) s); _temp668->attributes= atts; goto
_LL653; _LL661:( void*)( _temp670->sc=( void*) s); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp674=( char*)"bad attributes on tunion";
struct _tagged_string _temp675; _temp675.curr= _temp674; _temp675.base= _temp674;
_temp675.last_plus_one= _temp674 + 25; _temp675;}), loc);} goto _LL653; _LL663:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp676=( char*)"bad declaration";
struct _tagged_string _temp677; _temp677.curr= _temp676; _temp677.base= _temp676;
_temp677.last_plus_one= _temp676 + 16; _temp677;}), loc); return 0; _LL653:;}
return({ struct Cyc_List_List* _temp678=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp678->hd=( void*) d; _temp678->tl= 0;
_temp678;});} else{ void* _temp679= _temp650; struct Cyc_Absyn_Structdecl**
_temp693; struct Cyc_List_List* _temp695; struct _tuple1* _temp697; struct Cyc_Absyn_TunionInfo
_temp699; void* _temp701; struct Cyc_List_List* _temp703; void* _temp705; struct
Cyc_Absyn_Tuniondecl* _temp707; struct Cyc_Absyn_TunionInfo _temp709; void*
_temp711; struct Cyc_List_List* _temp713; void* _temp715; struct Cyc_Absyn_UnknownTunionInfo
_temp717; int _temp719; struct _tuple1* _temp721; struct Cyc_Absyn_Uniondecl**
_temp723; struct Cyc_List_List* _temp725; struct _tuple1* _temp727; struct Cyc_Absyn_Enumdecl*
_temp729; struct _tuple1* _temp731; _LL681: if(( unsigned int) _temp679 > 4u?*((
int*) _temp679) == Cyc_Absyn_StructType: 0){ _LL698: _temp697=( struct _tuple1*)((
struct Cyc_Absyn_StructType_struct*) _temp679)->f1; goto _LL696; _LL696:
_temp695=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp679)->f2; goto _LL694; _LL694: _temp693=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp679)->f3; goto _LL682;} else{ goto
_LL683;} _LL683: if(( unsigned int) _temp679 > 4u?*(( int*) _temp679) == Cyc_Absyn_TunionType:
0){ _LL700: _temp699=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp679)->f1; _LL706: _temp705=( void*) _temp699.tunion_info; if(*(( int*)
_temp705) == Cyc_Absyn_KnownTunion){ _LL708: _temp707=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp705)->f1; goto _LL704;} else{ goto
_LL685;} _LL704: _temp703=( struct Cyc_List_List*) _temp699.targs; goto _LL702;
_LL702: _temp701=( void*) _temp699.rgn; goto _LL684;} else{ goto _LL685;} _LL685:
if(( unsigned int) _temp679 > 4u?*(( int*) _temp679) == Cyc_Absyn_TunionType: 0){
_LL710: _temp709=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp679)->f1; _LL716: _temp715=( void*) _temp709.tunion_info; if(*(( int*)
_temp715) == Cyc_Absyn_UnknownTunion){ _LL718: _temp717=( struct Cyc_Absyn_UnknownTunionInfo)((
struct Cyc_Absyn_UnknownTunion_struct*) _temp715)->f1; _LL722: _temp721=( struct
_tuple1*) _temp717.name; goto _LL720; _LL720: _temp719=( int) _temp717.is_xtunion;
goto _LL714;} else{ goto _LL687;} _LL714: _temp713=( struct Cyc_List_List*)
_temp709.targs; goto _LL712; _LL712: _temp711=( void*) _temp709.rgn; goto _LL686;}
else{ goto _LL687;} _LL687: if(( unsigned int) _temp679 > 4u?*(( int*) _temp679)
== Cyc_Absyn_UnionType: 0){ _LL728: _temp727=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp679)->f1; goto _LL726; _LL726: _temp725=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp679)->f2; goto _LL724; _LL724: _temp723=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp679)->f3;
goto _LL688;} else{ goto _LL689;} _LL689: if(( unsigned int) _temp679 > 4u?*((
int*) _temp679) == Cyc_Absyn_EnumType: 0){ _LL732: _temp731=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp679)->f1; goto _LL730; _LL730: _temp729=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp679)->f2;
goto _LL690;} else{ goto _LL691;} _LL691: goto _LL692; _LL682: { struct Cyc_List_List*
_temp733=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp695); goto _LL734; _LL734: { struct Cyc_Absyn_Structdecl*
_temp737=({ struct Cyc_Absyn_Structdecl* _temp735=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp735->sc=( void*) s;
_temp735->name=({ struct Cyc_Core_Opt* _temp736=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp736->v=( void*)(( struct _tuple1*)((
struct _tuple1*) _check_null( _temp697))); _temp736;}); _temp735->tvs= _temp733;
_temp735->fields= 0; _temp735->attributes= 0; _temp735;}); goto _LL738; _LL738:
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp739=( char*)"bad attributes on struct";
struct _tagged_string _temp740; _temp740.curr= _temp739; _temp740.base= _temp739;
_temp740.last_plus_one= _temp739 + 25; _temp740;}), loc);} return({ struct Cyc_List_List*
_temp741=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp741->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp742=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp742[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp743; _temp743.tag= Cyc_Absyn_Struct_d;
_temp743.f1= _temp737; _temp743;}); _temp742;}), loc); _temp741->tl= 0; _temp741;});}}
_LL684: if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp744=(
char*)"bad attributes on tunion"; struct _tagged_string _temp745; _temp745.curr=
_temp744; _temp745.base= _temp744; _temp745.last_plus_one= _temp744 + 25;
_temp745;}), loc);} return({ struct Cyc_List_List* _temp746=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp746->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp747=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp747[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp748; _temp748.tag= Cyc_Absyn_Tunion_d; _temp748.f1=
_temp707; _temp748;}); _temp747;}), loc); _temp746->tl= 0; _temp746;}); _LL686: {
struct Cyc_List_List* _temp749=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp713);
goto _LL750; _LL750: { struct Cyc_Absyn_Decl* _temp751= Cyc_Absyn_tunion_decl( s,
_temp721, _temp749, 0, _temp719, loc); goto _LL752; _LL752: if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp753=( char*)"bad attributes on tunion";
struct _tagged_string _temp754; _temp754.curr= _temp753; _temp754.base= _temp753;
_temp754.last_plus_one= _temp753 + 25; _temp754;}), loc);} return({ struct Cyc_List_List*
_temp755=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp755->hd=( void*) _temp751; _temp755->tl= 0; _temp755;});}} _LL688: { struct
Cyc_List_List* _temp756=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp725); goto
_LL757; _LL757: { struct Cyc_Absyn_Uniondecl* _temp760=({ struct Cyc_Absyn_Uniondecl*
_temp758=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp758->sc=( void*) s; _temp758->name=({ struct Cyc_Core_Opt* _temp759=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp759->v=(
void*)(( struct _tuple1*)(( struct _tuple1*) _check_null( _temp727))); _temp759;});
_temp758->tvs= _temp756; _temp758->fields= 0; _temp758->attributes= 0; _temp758;});
goto _LL761; _LL761: if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp762=( char*)"bad attributes on union"; struct _tagged_string _temp763;
_temp763.curr= _temp762; _temp763.base= _temp762; _temp763.last_plus_one=
_temp762 + 24; _temp763;}), loc);} return({ struct Cyc_List_List* _temp764=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp764->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp765=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp765->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp766=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp766[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp767; _temp767.tag= Cyc_Absyn_Union_d;
_temp767.f1= _temp760; _temp767;}); _temp766;})); _temp765->loc= loc; _temp765;});
_temp764->tl= 0; _temp764;});}} _LL690: { struct Cyc_Absyn_Enumdecl* _temp769=({
struct Cyc_Absyn_Enumdecl* _temp768=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp768->sc=( void*) s; _temp768->name=
_temp731; _temp768->fields= 0; _temp768;}); goto _LL770; _LL770: if( atts != 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp771=( char*)"bad attributes on enum";
struct _tagged_string _temp772; _temp772.curr= _temp771; _temp772.base= _temp771;
_temp772.last_plus_one= _temp771 + 23; _temp772;}), loc);} return({ struct Cyc_List_List*
_temp773=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp773->hd=( void*)({ struct Cyc_Absyn_Decl* _temp774=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp774->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp775=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp775[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp776; _temp776.tag= Cyc_Absyn_Enum_d; _temp776.f1= _temp769; _temp776;});
_temp775;})); _temp774->loc= loc; _temp774;}); _temp773->tl= 0; _temp773;});}
_LL692: Cyc_Parse_err(( struct _tagged_string)({ char* _temp777=( char*)"missing declarator";
struct _tagged_string _temp778; _temp778.curr= _temp777; _temp778.base= _temp777;
_temp778.last_plus_one= _temp777 + 19; _temp778;}), loc); return 0; _LL680:;}}
else{ void* t= _temp644.f1; struct Cyc_List_List* _temp779= Cyc_Parse_apply_tmss(
tq, t, _temp642, atts); goto _LL780; _LL780: if( istypedef){ if( ! exps_empty){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp781=( char*)"initializer in typedef declaration";
struct _tagged_string _temp782; _temp782.curr= _temp781; _temp782.base= _temp781;
_temp782.last_plus_one= _temp781 + 35; _temp782;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, _temp779); if( _temp644.f2 !=
0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp644.f2))->v;{ void* _temp783=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp795; struct Cyc_Absyn_Tuniondecl* _temp797; struct Cyc_Absyn_Uniondecl*
_temp799; struct Cyc_Absyn_Enumdecl* _temp801; _LL785: if(*(( int*) _temp783) ==
Cyc_Absyn_Struct_d){ _LL796: _temp795=( struct Cyc_Absyn_Structdecl*)(( struct
Cyc_Absyn_Struct_d_struct*) _temp783)->f1; goto _LL786;} else{ goto _LL787;}
_LL787: if(*(( int*) _temp783) == Cyc_Absyn_Tunion_d){ _LL798: _temp797=( struct
Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp783)->f1; goto
_LL788;} else{ goto _LL789;} _LL789: if(*(( int*) _temp783) == Cyc_Absyn_Union_d){
_LL800: _temp799=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp783)->f1; goto _LL790;} else{ goto _LL791;} _LL791: if(*(( int*) _temp783)
== Cyc_Absyn_Enum_d){ _LL802: _temp801=( struct Cyc_Absyn_Enumdecl*)(( struct
Cyc_Absyn_Enum_d_struct*) _temp783)->f1; goto _LL792;} else{ goto _LL793;}
_LL793: goto _LL794; _LL786:( void*)( _temp795->sc=( void*) s); _temp795->attributes=
atts; atts= 0; goto _LL784; _LL788:( void*)( _temp797->sc=( void*) s); goto
_LL784; _LL790:( void*)( _temp799->sc=( void*) s); goto _LL784; _LL792:( void*)(
_temp801->sc=( void*) s); goto _LL784; _LL794: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp803=( char*)"declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp804; _temp804.curr= _temp803; _temp804.base= _temp803;
_temp804.last_plus_one= _temp803 + 70; _temp804;}), loc); goto _LL784; _LL784:;}
decls=({ struct Cyc_List_List* _temp805=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp805->hd=( void*) d; _temp805->tl= decls;
_temp805;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp806=
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);
xprintf("bad attribute %.*s in typedef", _temp806.last_plus_one - _temp806.curr,
_temp806.curr);}), loc);} return decls;}} else{ if( _temp644.f2 != 0){ Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp807=( char*)"nested type declaration within declarator";
struct _tagged_string _temp808; _temp808.curr= _temp807; _temp808.base= _temp807;
_temp808.last_plus_one= _temp807 + 42; _temp808;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* _temp809= _temp779; goto _LL810; _LL810: for( 0;
_temp809 != 0; _temp809=(( struct Cyc_List_List*) _check_null( _temp809))->tl,
_temp640=(( struct Cyc_List_List*) _check_null( _temp640))->tl){ struct _tuple7
_temp813; struct Cyc_List_List* _temp814; struct Cyc_List_List* _temp816; void*
_temp818; struct Cyc_Absyn_Tqual _temp820; struct _tuple1* _temp822; struct
_tuple7* _temp811=( struct _tuple7*)(( struct Cyc_List_List*) _check_null(
_temp809))->hd; _temp813=* _temp811; _LL823: _temp822= _temp813.f1; goto _LL821;
_LL821: _temp820= _temp813.f2; goto _LL819; _LL819: _temp818= _temp813.f3; goto
_LL817; _LL817: _temp816= _temp813.f4; goto _LL815; _LL815: _temp814= _temp813.f5;
goto _LL812; _LL812: if( _temp816 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp824=( char*)"bad type params, ignoring"; struct _tagged_string
_temp825; _temp825.curr= _temp824; _temp825.base= _temp824; _temp825.last_plus_one=
_temp824 + 26; _temp825;}), loc);} if( _temp640 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp826=( char*)"unexpected null in parse!"; struct
_tagged_string _temp827; _temp827.curr= _temp826; _temp827.base= _temp826;
_temp827.last_plus_one= _temp826 + 26; _temp827;}), loc);}{ struct Cyc_Absyn_Exp*
_temp828=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp640))->hd;
goto _LL829; _LL829: { struct Cyc_Absyn_Vardecl* _temp830= Cyc_Absyn_new_vardecl(
_temp822, _temp818, _temp828); goto _LL831; _LL831: _temp830->tq= _temp820;(
void*)( _temp830->sc=( void*) s); _temp830->attributes= _temp814;{ struct Cyc_Absyn_Decl*
_temp835=({ struct Cyc_Absyn_Decl* _temp832=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp832->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp833=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp833[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp834; _temp834.tag= Cyc_Absyn_Var_d;
_temp834.f1= _temp830; _temp834;}); _temp833;})); _temp832->loc= loc; _temp832;});
goto _LL836; _LL836: decls=({ struct Cyc_List_List* _temp837=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp837->hd=( void*) _temp835;
_temp837->tl= decls; _temp837;});}}}}} return(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen(
s) != 1){ Cyc_Parse_err(({ struct _tagged_string _temp838= s; xprintf("bad kind: %.*s",
_temp838.last_plus_one - _temp838.curr, _temp838.curr);}), loc); return( void*)
Cyc_Absyn_BoxKind;} else{ switch(*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( char), 0)){
case 'A': _LL839: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL840: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL841: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL842: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL843: return(
void*) Cyc_Absyn_EffKind; default: _LL844: Cyc_Parse_err(({ struct
_tagged_string _temp846= s; xprintf("bad kind: %.*s", _temp846.last_plus_one -
_temp846.curr, _temp846.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp847=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp847->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp848=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp848[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp849; _temp849.tag= Cyc_Absyn_Attributes_mod;
_temp849.f1= loc; _temp849.f2= atts; _temp849;}); _temp848;})); _temp847->tl=
tms; _temp847;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(({ struct
_tagged_string _temp850= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null((* t).f5))->hd); xprintf("bad attribute %.*s within typedef",
_temp850.last_plus_one - _temp850.curr, _temp850.curr);}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp851=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp851[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp852; _temp852.tag= Cyc_Absyn_Typedef_d; _temp852.f1=({
struct Cyc_Absyn_Typedefdecl* _temp853=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp853->name= x; _temp853->tvs=(*
t).f4; _temp853->defn=( void*)(* t).f3; _temp853;}); _temp852;}); _temp851;}),
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
} ; static char _temp857[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp857,
_temp857, _temp857 + 8u}}; struct _tuple15* Cyc_yyget_Int_tok( void* yy1){ void*
_temp858= yy1; struct _tuple15* _temp864; _LL860: if(*(( void**) _temp858) ==
Cyc_Int_tok){ _LL865: _temp864=( struct _tuple15*)(( struct Cyc_Int_tok_struct*)
_temp858)->f1; goto _LL861;} else{ goto _LL862;} _LL862: goto _LL863; _LL861:
return _temp864; _LL863:( void) _throw(( void*)& Cyc_yyfail_Int_tok); _LL859:;}
static char _temp869[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp869,
_temp869, _temp869 + 11u}}; struct _tagged_string Cyc_yyget_String_tok( void*
yy1){ void* _temp870= yy1; struct _tagged_string _temp876; _LL872: if(*(( void**)
_temp870) == Cyc_String_tok){ _LL877: _temp876=( struct _tagged_string)(( struct
Cyc_String_tok_struct*) _temp870)->f1; goto _LL873;} else{ goto _LL874;} _LL874:
goto _LL875; _LL873: return _temp876; _LL875:( void) _throw(( void*)& Cyc_yyfail_String_tok);
_LL871:;} static char _temp881[ 9u]="Char_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp881,
_temp881, _temp881 + 9u}}; char Cyc_yyget_Char_tok( void* yy1){ void* _temp882=
yy1; char _temp888; _LL884: if(*(( void**) _temp882) == Cyc_Char_tok){ _LL889:
_temp888=( char)(( struct Cyc_Char_tok_struct*) _temp882)->f1; goto _LL885;}
else{ goto _LL886;} _LL886: goto _LL887; _LL885: return _temp888; _LL887:( void)
_throw(( void*)& Cyc_yyfail_Char_tok); _LL883:;} static char _temp893[ 17u]="Pointer_Sort_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp893, _temp893, _temp893 + 17u}}; void* Cyc_yyget_Pointer_Sort_tok(
void* yy1){ void* _temp894= yy1; void* _temp900; _LL896: if(*(( void**) _temp894)
== Cyc_Pointer_Sort_tok){ _LL901: _temp900=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp894)->f1; goto _LL897;} else{ goto _LL898;} _LL898: goto _LL899; _LL897:
return _temp900; _LL899:( void) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL895:;} static char _temp905[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp905,
_temp905, _temp905 + 8u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
void* _temp906= yy1; struct Cyc_Absyn_Exp* _temp912; _LL908: if(*(( void**)
_temp906) == Cyc_Exp_tok){ _LL913: _temp912=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Exp_tok_struct*) _temp906)->f1; goto _LL909;} else{ goto _LL910;} _LL910:
goto _LL911; _LL909: return _temp912; _LL911:( void) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL907:;} static char _temp917[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp917,
_temp917, _temp917 + 12u}}; struct Cyc_List_List* Cyc_yyget_ExpList_tok( void*
yy1){ void* _temp918= yy1; struct Cyc_List_List* _temp924; _LL920: if(*(( void**)
_temp918) == Cyc_ExpList_tok){ _LL925: _temp924=( struct Cyc_List_List*)((
struct Cyc_ExpList_tok_struct*) _temp918)->f1; goto _LL921;} else{ goto _LL922;}
_LL922: goto _LL923; _LL921: return _temp924; _LL923:( void) _throw(( void*)&
Cyc_yyfail_ExpList_tok); _LL919:;} static char _temp929[ 20u]="InitializerList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp929, _temp929, _temp929 + 20u}}; struct Cyc_List_List*
Cyc_yyget_InitializerList_tok( void* yy1){ void* _temp930= yy1; struct Cyc_List_List*
_temp936; _LL932: if(*(( void**) _temp930) == Cyc_InitializerList_tok){ _LL937:
_temp936=( struct Cyc_List_List*)(( struct Cyc_InitializerList_tok_struct*)
_temp930)->f1; goto _LL933;} else{ goto _LL934;} _LL934: goto _LL935; _LL933:
return _temp936; _LL935:( void) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL931:;} static char _temp941[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp941,
_temp941, _temp941 + 11u}}; void* Cyc_yyget_Primop_tok( void* yy1){ void*
_temp942= yy1; void* _temp948; _LL944: if(*(( void**) _temp942) == Cyc_Primop_tok){
_LL949: _temp948=( void*)(( struct Cyc_Primop_tok_struct*) _temp942)->f1; goto
_LL945;} else{ goto _LL946;} _LL946: goto _LL947; _LL945: return _temp948;
_LL947:( void) _throw(( void*)& Cyc_yyfail_Primop_tok); _LL943:;} static char
_temp953[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp953, _temp953, _temp953 + 14u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ void* _temp954= yy1;
struct Cyc_Core_Opt* _temp960; _LL956: if(*(( void**) _temp954) == Cyc_Primopopt_tok){
_LL961: _temp960=( struct Cyc_Core_Opt*)(( struct Cyc_Primopopt_tok_struct*)
_temp954)->f1; goto _LL957;} else{ goto _LL958;} _LL958: goto _LL959; _LL957:
return _temp960; _LL959:( void) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL955:;} static char _temp965[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp965,
_temp965, _temp965 + 11u}}; struct _tuple1* Cyc_yyget_QualId_tok( void* yy1){
void* _temp966= yy1; struct _tuple1* _temp972; _LL968: if(*(( void**) _temp966)
== Cyc_QualId_tok){ _LL973: _temp972=( struct _tuple1*)(( struct Cyc_QualId_tok_struct*)
_temp966)->f1; goto _LL969;} else{ goto _LL970;} _LL970: goto _LL971; _LL969:
return _temp972; _LL971:( void) _throw(( void*)& Cyc_yyfail_QualId_tok); _LL967:;}
static char _temp977[ 9u]="Stmt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp977, _temp977, _temp977 + 9u}};
struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( void* yy1){ void* _temp978= yy1;
struct Cyc_Absyn_Stmt* _temp984; _LL980: if(*(( void**) _temp978) == Cyc_Stmt_tok){
_LL985: _temp984=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Stmt_tok_struct*)
_temp978)->f1; goto _LL981;} else{ goto _LL982;} _LL982: goto _LL983; _LL981:
return _temp984; _LL983:( void) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL979:;}
static char _temp989[ 14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_BlockItem_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp989,
_temp989, _temp989 + 14u}}; void* Cyc_yyget_BlockItem_tok( void* yy1){ void*
_temp990= yy1; void* _temp996; _LL992: if(*(( void**) _temp990) == Cyc_BlockItem_tok){
_LL997: _temp996=( void*)(( struct Cyc_BlockItem_tok_struct*) _temp990)->f1;
goto _LL993;} else{ goto _LL994;} _LL994: goto _LL995; _LL993: return _temp996;
_LL995:( void) _throw(( void*)& Cyc_yyfail_BlockItem_tok); _LL991:;} static char
_temp1001[ 21u]="SwitchClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1001, _temp1001, _temp1001 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
void* yy1){ void* _temp1002= yy1; struct Cyc_List_List* _temp1008; _LL1004: if(*((
void**) _temp1002) == Cyc_SwitchClauseList_tok){ _LL1009: _temp1008=( struct Cyc_List_List*)((
struct Cyc_SwitchClauseList_tok_struct*) _temp1002)->f1; goto _LL1005;} else{
goto _LL1006;} _LL1006: goto _LL1007; _LL1005: return _temp1008; _LL1007:( void)
_throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL1003:;} static char
_temp1013[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1013, _temp1013, _temp1013 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp1014= yy1;
struct Cyc_Absyn_Pat* _temp1020; _LL1016: if(*(( void**) _temp1014) == Cyc_Pattern_tok){
_LL1021: _temp1020=( struct Cyc_Absyn_Pat*)(( struct Cyc_Pattern_tok_struct*)
_temp1014)->f1; goto _LL1017;} else{ goto _LL1018;} _LL1018: goto _LL1019;
_LL1017: return _temp1020; _LL1019:( void) _throw(( void*)& Cyc_yyfail_Pattern_tok);
_LL1015:;} static char _temp1025[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1025, _temp1025, _temp1025 + 16u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok(
void* yy1){ void* _temp1026= yy1; struct Cyc_List_List* _temp1032; _LL1028: if(*((
void**) _temp1026) == Cyc_PatternList_tok){ _LL1033: _temp1032=( struct Cyc_List_List*)((
struct Cyc_PatternList_tok_struct*) _temp1026)->f1; goto _LL1029;} else{ goto
_LL1030;} _LL1030: goto _LL1031; _LL1029: return _temp1032; _LL1031:( void)
_throw(( void*)& Cyc_yyfail_PatternList_tok); _LL1027:;} static char _temp1037[
17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1037, _temp1037, _temp1037 + 17u}};
struct _tuple13* Cyc_yyget_FieldPattern_tok( void* yy1){ void* _temp1038= yy1;
struct _tuple13* _temp1044; _LL1040: if(*(( void**) _temp1038) == Cyc_FieldPattern_tok){
_LL1045: _temp1044=( struct _tuple13*)(( struct Cyc_FieldPattern_tok_struct*)
_temp1038)->f1; goto _LL1041;} else{ goto _LL1042;} _LL1042: goto _LL1043;
_LL1041: return _temp1044; _LL1043:( void) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL1039:;} static char _temp1049[ 21u]="FieldPatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1049, _temp1049, _temp1049 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp1050= yy1; struct Cyc_List_List* _temp1056; _LL1052: if(*((
void**) _temp1050) == Cyc_FieldPatternList_tok){ _LL1057: _temp1056=( struct Cyc_List_List*)((
struct Cyc_FieldPatternList_tok_struct*) _temp1050)->f1; goto _LL1053;} else{
goto _LL1054;} _LL1054: goto _LL1055; _LL1053: return _temp1056; _LL1055:( void)
_throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL1051:;} static char
_temp1061[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FnDecl_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1061, _temp1061, _temp1061 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp1062= yy1;
struct Cyc_Absyn_Fndecl* _temp1068; _LL1064: if(*(( void**) _temp1062) == Cyc_FnDecl_tok){
_LL1069: _temp1068=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_FnDecl_tok_struct*)
_temp1062)->f1; goto _LL1065;} else{ goto _LL1066;} _LL1066: goto _LL1067;
_LL1065: return _temp1068; _LL1067:( void) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL1063:;} static char _temp1073[ 13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1073,
_temp1073, _temp1073 + 13u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok( void*
yy1){ void* _temp1074= yy1; struct Cyc_List_List* _temp1080; _LL1076: if(*((
void**) _temp1074) == Cyc_DeclList_tok){ _LL1081: _temp1080=( struct Cyc_List_List*)((
struct Cyc_DeclList_tok_struct*) _temp1074)->f1; goto _LL1077;} else{ goto
_LL1078;} _LL1078: goto _LL1079; _LL1077: return _temp1080; _LL1079:( void)
_throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1075:;} static char _temp1085[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1085, _temp1085, _temp1085 + 13u}}; struct Cyc_Parse_Declaration_spec*
Cyc_yyget_DeclSpec_tok( void* yy1){ void* _temp1086= yy1; struct Cyc_Parse_Declaration_spec*
_temp1092; _LL1088: if(*(( void**) _temp1086) == Cyc_DeclSpec_tok){ _LL1093:
_temp1092=( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_DeclSpec_tok_struct*)
_temp1086)->f1; goto _LL1089;} else{ goto _LL1090;} _LL1090: goto _LL1091;
_LL1089: return _temp1092; _LL1091:( void) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1087:;} static char _temp1097[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1097,
_temp1097, _temp1097 + 13u}}; struct _tuple14* Cyc_yyget_InitDecl_tok( void* yy1){
void* _temp1098= yy1; struct _tuple14* _temp1104; _LL1100: if(*(( void**)
_temp1098) == Cyc_InitDecl_tok){ _LL1105: _temp1104=( struct _tuple14*)(( struct
Cyc_InitDecl_tok_struct*) _temp1098)->f1; goto _LL1101;} else{ goto _LL1102;}
_LL1102: goto _LL1103; _LL1101: return _temp1104; _LL1103:( void) _throw(( void*)&
Cyc_yyfail_InitDecl_tok); _LL1099:;} static char _temp1109[ 17u]="InitDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1109, _temp1109, _temp1109 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( void* yy1){ void* _temp1110= yy1; struct Cyc_List_List*
_temp1116; _LL1112: if(*(( void**) _temp1110) == Cyc_InitDeclList_tok){ _LL1117:
_temp1116=( struct Cyc_List_List*)(( struct Cyc_InitDeclList_tok_struct*)
_temp1110)->f1; goto _LL1113;} else{ goto _LL1114;} _LL1114: goto _LL1115;
_LL1113: return _temp1116; _LL1115:( void) _throw(( void*)& Cyc_yyfail_InitDeclList_tok);
_LL1111:;} static char _temp1121[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1121, _temp1121, _temp1121 + 17u}}; void* Cyc_yyget_StorageClass_tok( void*
yy1){ void* _temp1122= yy1; void* _temp1128; _LL1124: if(*(( void**) _temp1122)
== Cyc_StorageClass_tok){ _LL1129: _temp1128=( void*)(( struct Cyc_StorageClass_tok_struct*)
_temp1122)->f1; goto _LL1125;} else{ goto _LL1126;} _LL1126: goto _LL1127;
_LL1125: return _temp1128; _LL1127:( void) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL1123:;} static char _temp1133[ 18u]="TypeSpecifier_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1133, _temp1133, _temp1133 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* _temp1134= yy1; void* _temp1140; _LL1136: if(*(( void**) _temp1134)
== Cyc_TypeSpecifier_tok){ _LL1141: _temp1140=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp1134)->f1; goto _LL1137;} else{ goto _LL1138;} _LL1138: goto _LL1139;
_LL1137: return _temp1140; _LL1139:( void) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1135:;} static char _temp1145[ 18u]="StructOrUnion_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1145, _temp1145, _temp1145 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void*
yy1){ void* _temp1146= yy1; void* _temp1152; _LL1148: if(*(( void**) _temp1146)
== Cyc_StructOrUnion_tok){ _LL1153: _temp1152=( void*)(( struct Cyc_StructOrUnion_tok_struct*)
_temp1146)->f1; goto _LL1149;} else{ goto _LL1150;} _LL1150: goto _LL1151;
_LL1149: return _temp1152; _LL1151:( void) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL1147:;} static char _temp1157[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1157,
_temp1157, _temp1157 + 13u}}; struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(
void* yy1){ void* _temp1158= yy1; struct Cyc_Absyn_Tqual _temp1164; _LL1160: if(*((
void**) _temp1158) == Cyc_TypeQual_tok){ _LL1165: _temp1164=( struct Cyc_Absyn_Tqual)((
struct Cyc_TypeQual_tok_struct*) _temp1158)->f1; goto _LL1161;} else{ goto
_LL1162;} _LL1162: goto _LL1163; _LL1161: return _temp1164; _LL1163:( void)
_throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL1159:;} static char _temp1169[ 24u]="StructFieldDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1169, _temp1169, _temp1169 + 24u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclList_tok( void* yy1){ void* _temp1170= yy1; struct Cyc_List_List*
_temp1176; _LL1172: if(*(( void**) _temp1170) == Cyc_StructFieldDeclList_tok){
_LL1177: _temp1176=( struct Cyc_List_List*)(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp1170)->f1; goto _LL1173;} else{ goto _LL1174;} _LL1174: goto _LL1175;
_LL1173: return _temp1176; _LL1175:( void) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL1171:;} static char _temp1181[ 28u]="StructFieldDeclListList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1181, _temp1181, _temp1181 + 28u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ void* _temp1182= yy1; struct
Cyc_List_List* _temp1188; _LL1184: if(*(( void**) _temp1182) == Cyc_StructFieldDeclListList_tok){
_LL1189: _temp1188=( struct Cyc_List_List*)(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp1182)->f1; goto _LL1185;} else{ goto _LL1186;} _LL1186: goto _LL1187;
_LL1185: return _temp1188; _LL1187:( void) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL1183:;} static char _temp1193[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1193, _temp1193, _temp1193 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ void* _temp1194= yy1; struct Cyc_List_List* _temp1200; _LL1196: if(*((
void**) _temp1194) == Cyc_TypeModifierList_tok){ _LL1201: _temp1200=( struct Cyc_List_List*)((
struct Cyc_TypeModifierList_tok_struct*) _temp1194)->f1; goto _LL1197;} else{
goto _LL1198;} _LL1198: goto _LL1199; _LL1197: return _temp1200; _LL1199:( void)
_throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL1195:;} static char
_temp1205[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgn_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1205, _temp1205, _temp1205 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp1206= yy1; void* _temp1212;
_LL1208: if(*(( void**) _temp1206) == Cyc_Rgn_tok){ _LL1213: _temp1212=( void*)((
struct Cyc_Rgn_tok_struct*) _temp1206)->f1; goto _LL1209;} else{ goto _LL1210;}
_LL1210: goto _LL1211; _LL1209: return _temp1212; _LL1211:( void) _throw(( void*)&
Cyc_yyfail_Rgn_tok); _LL1207:;} static char _temp1217[ 15u]="Declarator_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1217, _temp1217, _temp1217 + 15u}}; struct Cyc_Parse_Declarator*
Cyc_yyget_Declarator_tok( void* yy1){ void* _temp1218= yy1; struct Cyc_Parse_Declarator*
_temp1224; _LL1220: if(*(( void**) _temp1218) == Cyc_Declarator_tok){ _LL1225:
_temp1224=( struct Cyc_Parse_Declarator*)(( struct Cyc_Declarator_tok_struct*)
_temp1218)->f1; goto _LL1221;} else{ goto _LL1222;} _LL1222: goto _LL1223;
_LL1221: return _temp1224; _LL1223:( void) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL1219:;} static char _temp1229[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1229, _temp1229, _temp1229 + 21u}}; struct _tuple12* Cyc_yyget_DeclaratorExpopt_tok(
void* yy1){ void* _temp1230= yy1; struct _tuple12* _temp1236; _LL1232: if(*((
void**) _temp1230) == Cyc_DeclaratorExpopt_tok){ _LL1237: _temp1236=( struct
_tuple12*)(( struct Cyc_DeclaratorExpopt_tok_struct*) _temp1230)->f1; goto
_LL1233;} else{ goto _LL1234;} _LL1234: goto _LL1235; _LL1233: return _temp1236;
_LL1235:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok); _LL1231:;}
static char _temp1241[ 25u]="DeclaratorExpoptList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1241, _temp1241, _temp1241 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1242= yy1; struct Cyc_List_List* _temp1248; _LL1244: if(*((
void**) _temp1242) == Cyc_DeclaratorExpoptList_tok){ _LL1249: _temp1248=( struct
Cyc_List_List*)(( struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1242)->f1;
goto _LL1245;} else{ goto _LL1246;} _LL1246: goto _LL1247; _LL1245: return
_temp1248; _LL1247:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok);
_LL1243:;} static char _temp1253[ 23u]="AbstractDeclarator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1253, _temp1253, _temp1253 + 23u}}; struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_AbstractDeclarator_tok( void* yy1){ void* _temp1254= yy1; struct Cyc_Parse_Abstractdeclarator*
_temp1260; _LL1256: if(*(( void**) _temp1254) == Cyc_AbstractDeclarator_tok){
_LL1261: _temp1260=( struct Cyc_Parse_Abstractdeclarator*)(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp1254)->f1; goto _LL1257;} else{ goto _LL1258;} _LL1258: goto _LL1259;
_LL1257: return _temp1260; _LL1259:( void) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL1255:;} static char _temp1265[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1265,
_temp1265, _temp1265 + 9u}}; int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1266=
yy1; int _temp1272; _LL1268: if(*(( void**) _temp1266) == Cyc_Bool_tok){ _LL1273:
_temp1272=( int)(( struct Cyc_Bool_tok_struct*) _temp1266)->f1; goto _LL1269;}
else{ goto _LL1270;} _LL1270: goto _LL1271; _LL1269: return _temp1272; _LL1271:(
void) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL1267:;} static char _temp1277[
10u]="Scope_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1277, _temp1277, _temp1277 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1278= yy1; void* _temp1284;
_LL1280: if(*(( void**) _temp1278) == Cyc_Scope_tok){ _LL1285: _temp1284=( void*)((
struct Cyc_Scope_tok_struct*) _temp1278)->f1; goto _LL1281;} else{ goto _LL1282;}
_LL1282: goto _LL1283; _LL1281: return _temp1284; _LL1283:( void) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1279:;} static char _temp1289[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1289, _temp1289, _temp1289 + 16u}}; struct Cyc_Absyn_Tunionfield*
Cyc_yyget_TunionField_tok( void* yy1){ void* _temp1290= yy1; struct Cyc_Absyn_Tunionfield*
_temp1296; _LL1292: if(*(( void**) _temp1290) == Cyc_TunionField_tok){ _LL1297:
_temp1296=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_TunionField_tok_struct*)
_temp1290)->f1; goto _LL1293;} else{ goto _LL1294;} _LL1294: goto _LL1295;
_LL1293: return _temp1296; _LL1295:( void) _throw(( void*)& Cyc_yyfail_TunionField_tok);
_LL1291:;} static char _temp1301[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1301, _temp1301, _temp1301 + 20u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ void* _temp1302= yy1; struct Cyc_List_List* _temp1308; _LL1304: if(*((
void**) _temp1302) == Cyc_TunionFieldList_tok){ _LL1309: _temp1308=( struct Cyc_List_List*)((
struct Cyc_TunionFieldList_tok_struct*) _temp1302)->f1; goto _LL1305;} else{
goto _LL1306;} _LL1306: goto _LL1307; _LL1305: return _temp1308; _LL1307:( void)
_throw(( void*)& Cyc_yyfail_TunionFieldList_tok); _LL1303:;} static char
_temp1313[ 17u]="QualSpecList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1313, _temp1313, _temp1313 + 17u}};
struct _tuple17* Cyc_yyget_QualSpecList_tok( void* yy1){ void* _temp1314= yy1;
struct _tuple17* _temp1320; _LL1316: if(*(( void**) _temp1314) == Cyc_QualSpecList_tok){
_LL1321: _temp1320=( struct _tuple17*)(( struct Cyc_QualSpecList_tok_struct*)
_temp1314)->f1; goto _LL1317;} else{ goto _LL1318;} _LL1318: goto _LL1319;
_LL1317: return _temp1320; _LL1319:( void) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1315:;} static char _temp1325[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1325,
_temp1325, _temp1325 + 11u}}; struct Cyc_List_List* Cyc_yyget_IdList_tok( void*
yy1){ void* _temp1326= yy1; struct Cyc_List_List* _temp1332; _LL1328: if(*((
void**) _temp1326) == Cyc_IdList_tok){ _LL1333: _temp1332=( struct Cyc_List_List*)((
struct Cyc_IdList_tok_struct*) _temp1326)->f1; goto _LL1329;} else{ goto _LL1330;}
_LL1330: goto _LL1331; _LL1329: return _temp1332; _LL1331:( void) _throw(( void*)&
Cyc_yyfail_IdList_tok); _LL1327:;} static char _temp1337[ 14u]="ParamDecl_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1337, _temp1337, _temp1337 + 14u}}; struct _tuple2*
Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1338= yy1; struct _tuple2*
_temp1344; _LL1340: if(*(( void**) _temp1338) == Cyc_ParamDecl_tok){ _LL1345:
_temp1344=( struct _tuple2*)(( struct Cyc_ParamDecl_tok_struct*) _temp1338)->f1;
goto _LL1341;} else{ goto _LL1342;} _LL1342: goto _LL1343; _LL1341: return
_temp1344; _LL1343:( void) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1339:;}
static char _temp1349[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1349, _temp1349, _temp1349 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ void* _temp1350= yy1; struct Cyc_List_List* _temp1356; _LL1352: if(*((
void**) _temp1350) == Cyc_ParamDeclList_tok){ _LL1357: _temp1356=( struct Cyc_List_List*)((
struct Cyc_ParamDeclList_tok_struct*) _temp1350)->f1; goto _LL1353;} else{ goto
_LL1354;} _LL1354: goto _LL1355; _LL1353: return _temp1356; _LL1355:( void)
_throw(( void*)& Cyc_yyfail_ParamDeclList_tok); _LL1351:;} static char _temp1361[
22u]="ParamDeclListBool_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1361, _temp1361, _temp1361 + 22u}};
struct _tuple16* Cyc_yyget_ParamDeclListBool_tok( void* yy1){ void* _temp1362=
yy1; struct _tuple16* _temp1368; _LL1364: if(*(( void**) _temp1362) == Cyc_ParamDeclListBool_tok){
_LL1369: _temp1368=( struct _tuple16*)(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp1362)->f1; goto _LL1365;} else{ goto _LL1366;} _LL1366: goto _LL1367;
_LL1365: return _temp1368; _LL1367:( void) _throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL1363:;} static char _temp1373[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1373,
_temp1373, _temp1373 + 13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void*
yy1){ void* _temp1374= yy1; struct Cyc_List_List* _temp1380; _LL1376: if(*((
void**) _temp1374) == Cyc_TypeList_tok){ _LL1381: _temp1380=( struct Cyc_List_List*)((
struct Cyc_TypeList_tok_struct*) _temp1374)->f1; goto _LL1377;} else{ goto
_LL1378;} _LL1378: goto _LL1379; _LL1377: return _temp1380; _LL1379:( void)
_throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1375:;} static char _temp1385[ 19u]="DesignatorList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1385, _temp1385, _temp1385 + 19u}}; struct Cyc_List_List*
Cyc_yyget_DesignatorList_tok( void* yy1){ void* _temp1386= yy1; struct Cyc_List_List*
_temp1392; _LL1388: if(*(( void**) _temp1386) == Cyc_DesignatorList_tok){
_LL1393: _temp1392=( struct Cyc_List_List*)(( struct Cyc_DesignatorList_tok_struct*)
_temp1386)->f1; goto _LL1389;} else{ goto _LL1390;} _LL1390: goto _LL1391;
_LL1389: return _temp1392; _LL1391:( void) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1387:;} static char _temp1397[ 15u]="Designator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1397,
_temp1397, _temp1397 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1398= yy1; void* _temp1404; _LL1400: if(*(( void**) _temp1398) == Cyc_Designator_tok){
_LL1405: _temp1404=( void*)(( struct Cyc_Designator_tok_struct*) _temp1398)->f1;
goto _LL1401;} else{ goto _LL1402;} _LL1402: goto _LL1403; _LL1401: return
_temp1404; _LL1403:( void) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1399:;}
static char _temp1409[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1409, _temp1409, _temp1409 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1410= yy1; void* _temp1416;
_LL1412: if(*(( void**) _temp1410) == Cyc_Kind_tok){ _LL1417: _temp1416=( void*)((
struct Cyc_Kind_tok_struct*) _temp1410)->f1; goto _LL1413;} else{ goto _LL1414;}
_LL1414: goto _LL1415; _LL1413: return _temp1416; _LL1415:( void) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1411:;} static char _temp1421[ 9u]="Type_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,( struct
_tagged_string){ _temp1421, _temp1421, _temp1421 + 9u}}; void* Cyc_yyget_Type_tok(
void* yy1){ void* _temp1422= yy1; void* _temp1428; _LL1424: if(*(( void**)
_temp1422) == Cyc_Type_tok){ _LL1429: _temp1428=( void*)(( struct Cyc_Type_tok_struct*)
_temp1422)->f1; goto _LL1425;} else{ goto _LL1426;} _LL1426: goto _LL1427;
_LL1425: return _temp1428; _LL1427:( void) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1423:;} static char _temp1433[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1433, _temp1433, _temp1433 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ void* _temp1434= yy1; struct Cyc_List_List* _temp1440; _LL1436: if(*((
void**) _temp1434) == Cyc_AttributeList_tok){ _LL1441: _temp1440=( struct Cyc_List_List*)((
struct Cyc_AttributeList_tok_struct*) _temp1434)->f1; goto _LL1437;} else{ goto
_LL1438;} _LL1438: goto _LL1439; _LL1437: return _temp1440; _LL1439:( void)
_throw(( void*)& Cyc_yyfail_AttributeList_tok); _LL1435:;} static char _temp1445[
14u]="Attribute_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1445, _temp1445, _temp1445 + 14u}};
void* Cyc_yyget_Attribute_tok( void* yy1){ void* _temp1446= yy1; void* _temp1452;
_LL1448: if(*(( void**) _temp1446) == Cyc_Attribute_tok){ _LL1453: _temp1452=(
void*)(( struct Cyc_Attribute_tok_struct*) _temp1446)->f1; goto _LL1449;} else{
goto _LL1450;} _LL1450: goto _LL1451; _LL1449: return _temp1452; _LL1451:( void)
_throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1447:;} static char _temp1457[ 14u]="Enumfield_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1457, _temp1457, _temp1457 + 14u}}; struct Cyc_Absyn_Enumfield*
Cyc_yyget_Enumfield_tok( void* yy1){ void* _temp1458= yy1; struct Cyc_Absyn_Enumfield*
_temp1464; _LL1460: if(*(( void**) _temp1458) == Cyc_Enumfield_tok){ _LL1465:
_temp1464=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Enumfield_tok_struct*)
_temp1458)->f1; goto _LL1461;} else{ goto _LL1462;} _LL1462: goto _LL1463;
_LL1461: return _temp1464; _LL1463:( void) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1459:;} static char _temp1469[ 18u]="EnumfieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1469, _temp1469, _temp1469 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1470= yy1; struct Cyc_List_List* _temp1476; _LL1472: if(*((
void**) _temp1470) == Cyc_EnumfieldList_tok){ _LL1477: _temp1476=( struct Cyc_List_List*)((
struct Cyc_EnumfieldList_tok_struct*) _temp1470)->f1; goto _LL1473;} else{ goto
_LL1474;} _LL1474: goto _LL1475; _LL1473: return _temp1476; _LL1475:( void)
_throw(( void*)& Cyc_yyfail_EnumfieldList_tok); _LL1471:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct
Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1478; _temp1478.timestamp=
0; _temp1478.first_line= 0; _temp1478.first_column= 0; _temp1478.last_line= 0;
_temp1478.last_column= 0; _temp1478.text=( struct _tagged_string)({ char*
_temp1479=( char*)""; struct _tagged_string _temp1480; _temp1480.curr= _temp1479;
_temp1480.base= _temp1479; _temp1480.last_plus_one= _temp1479 + 1; _temp1480;});
_temp1478;});} static char _temp1483[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc=(
struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column= 0,.last_line= 0,.last_column=
0,.text=( struct _tagged_string){ _temp1483, _temp1483, _temp1483 + 1u}}; static
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
(short)93, (short)94, (short)95, (short)96}; static char _temp1486[ 2u]="$";
static char _temp1489[ 6u]="error"; static char _temp1492[ 12u]="$undefined.";
static char _temp1495[ 5u]="AUTO"; static char _temp1498[ 9u]="REGISTER"; static
char _temp1501[ 7u]="STATIC"; static char _temp1504[ 7u]="EXTERN"; static char
_temp1507[ 8u]="TYPEDEF"; static char _temp1510[ 5u]="VOID"; static char
_temp1513[ 5u]="CHAR"; static char _temp1516[ 6u]="SHORT"; static char _temp1519[
4u]="INT"; static char _temp1522[ 5u]="LONG"; static char _temp1525[ 6u]="FLOAT";
static char _temp1528[ 7u]="DOUBLE"; static char _temp1531[ 7u]="SIGNED"; static
char _temp1534[ 9u]="UNSIGNED"; static char _temp1537[ 6u]="CONST"; static char
_temp1540[ 9u]="VOLATILE"; static char _temp1543[ 9u]="RESTRICT"; static char
_temp1546[ 7u]="STRUCT"; static char _temp1549[ 6u]="UNION"; static char
_temp1552[ 5u]="CASE"; static char _temp1555[ 8u]="DEFAULT"; static char
_temp1558[ 7u]="INLINE"; static char _temp1561[ 3u]="IF"; static char _temp1564[
5u]="ELSE"; static char _temp1567[ 7u]="SWITCH"; static char _temp1570[ 6u]="WHILE";
static char _temp1573[ 3u]="DO"; static char _temp1576[ 4u]="FOR"; static char
_temp1579[ 5u]="GOTO"; static char _temp1582[ 9u]="CONTINUE"; static char
_temp1585[ 6u]="BREAK"; static char _temp1588[ 7u]="RETURN"; static char
_temp1591[ 7u]="SIZEOF"; static char _temp1594[ 5u]="ENUM"; static char
_temp1597[ 8u]="NULL_kw"; static char _temp1600[ 4u]="LET"; static char
_temp1603[ 6u]="THROW"; static char _temp1606[ 4u]="TRY"; static char _temp1609[
6u]="CATCH"; static char _temp1612[ 4u]="NEW"; static char _temp1615[ 9u]="ABSTRACT";
static char _temp1618[ 9u]="FALLTHRU"; static char _temp1621[ 6u]="USING";
static char _temp1624[ 10u]="NAMESPACE"; static char _temp1627[ 7u]="TUNION";
static char _temp1630[ 8u]="XTUNION"; static char _temp1633[ 5u]="FILL"; static
char _temp1636[ 8u]="CODEGEN"; static char _temp1639[ 4u]="CUT"; static char
_temp1642[ 7u]="SPLICE"; static char _temp1645[ 7u]="PRINTF"; static char
_temp1648[ 8u]="FPRINTF"; static char _temp1651[ 8u]="XPRINTF"; static char
_temp1654[ 6u]="SCANF"; static char _temp1657[ 7u]="FSCANF"; static char
_temp1660[ 7u]="SSCANF"; static char _temp1663[ 7u]="MALLOC"; static char
_temp1666[ 9u]="REGION_T"; static char _temp1669[ 7u]="REGION"; static char
_temp1672[ 5u]="RNEW"; static char _temp1675[ 8u]="RMALLOC"; static char
_temp1678[ 7u]="PTR_OP"; static char _temp1681[ 7u]="INC_OP"; static char
_temp1684[ 7u]="DEC_OP"; static char _temp1687[ 8u]="LEFT_OP"; static char
_temp1690[ 9u]="RIGHT_OP"; static char _temp1693[ 6u]="LE_OP"; static char
_temp1696[ 6u]="GE_OP"; static char _temp1699[ 6u]="EQ_OP"; static char
_temp1702[ 6u]="NE_OP"; static char _temp1705[ 7u]="AND_OP"; static char
_temp1708[ 6u]="OR_OP"; static char _temp1711[ 11u]="MUL_ASSIGN"; static char
_temp1714[ 11u]="DIV_ASSIGN"; static char _temp1717[ 11u]="MOD_ASSIGN"; static
char _temp1720[ 11u]="ADD_ASSIGN"; static char _temp1723[ 11u]="SUB_ASSIGN";
static char _temp1726[ 12u]="LEFT_ASSIGN"; static char _temp1729[ 13u]="RIGHT_ASSIGN";
static char _temp1732[ 11u]="AND_ASSIGN"; static char _temp1735[ 11u]="XOR_ASSIGN";
static char _temp1738[ 10u]="OR_ASSIGN"; static char _temp1741[ 9u]="ELLIPSIS";
static char _temp1744[ 11u]="LEFT_RIGHT"; static char _temp1747[ 12u]="COLON_COLON";
static char _temp1750[ 11u]="IDENTIFIER"; static char _temp1753[ 17u]="INTEGER_CONSTANT";
static char _temp1756[ 7u]="STRING"; static char _temp1759[ 19u]="CHARACTER_CONSTANT";
static char _temp1762[ 18u]="FLOATING_CONSTANT"; static char _temp1765[ 9u]="TYPE_VAR";
static char _temp1768[ 16u]="QUAL_IDENTIFIER"; static char _temp1771[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1774[ 10u]="ATTRIBUTE"; static char _temp1777[ 4u]="';'";
static char _temp1780[ 4u]="'{'"; static char _temp1783[ 4u]="'}'"; static char
_temp1786[ 4u]="'='"; static char _temp1789[ 4u]="'('"; static char _temp1792[ 4u]="')'";
static char _temp1795[ 4u]="','"; static char _temp1798[ 4u]="'_'"; static char
_temp1801[ 4u]="'$'"; static char _temp1804[ 4u]="'<'"; static char _temp1807[ 4u]="'>'";
static char _temp1810[ 4u]="':'"; static char _temp1813[ 4u]="'.'"; static char
_temp1816[ 4u]="'['"; static char _temp1819[ 4u]="']'"; static char _temp1822[ 4u]="'*'";
static char _temp1825[ 4u]="'@'"; static char _temp1828[ 4u]="'?'"; static char
_temp1831[ 4u]="'+'"; static char _temp1834[ 4u]="'-'"; static char _temp1837[ 4u]="'&'";
static char _temp1840[ 4u]="'|'"; static char _temp1843[ 4u]="'^'"; static char
_temp1846[ 4u]="'/'"; static char _temp1849[ 4u]="'%'"; static char _temp1852[ 4u]="'~'";
static char _temp1855[ 4u]="'!'"; static char _temp1858[ 5u]="prog"; static char
_temp1861[ 17u]="translation_unit"; static char _temp1864[ 21u]="external_declaration";
static char _temp1867[ 20u]="function_definition"; static char _temp1870[ 21u]="function_definition2";
static char _temp1873[ 13u]="using_action"; static char _temp1876[ 15u]="unusing_action";
static char _temp1879[ 17u]="namespace_action"; static char _temp1882[ 19u]="unnamespace_action";
static char _temp1885[ 12u]="declaration"; static char _temp1888[ 17u]="declaration_list";
static char _temp1891[ 23u]="declaration_specifiers"; static char _temp1894[ 24u]="storage_class_specifier";
static char _temp1897[ 15u]="attributes_opt"; static char _temp1900[ 11u]="attributes";
static char _temp1903[ 15u]="attribute_list"; static char _temp1906[ 10u]="attribute";
static char _temp1909[ 15u]="type_specifier"; static char _temp1912[ 5u]="kind";
static char _temp1915[ 15u]="type_qualifier"; static char _temp1918[ 15u]="enum_specifier";
static char _temp1921[ 11u]="enum_field"; static char _temp1924[ 22u]="enum_declaration_list";
static char _temp1927[ 26u]="struct_or_union_specifier"; static char _temp1930[
16u]="type_params_opt"; static char _temp1933[ 16u]="struct_or_union"; static
char _temp1936[ 24u]="struct_declaration_list"; static char _temp1939[ 25u]="struct_declaration_list0";
static char _temp1942[ 21u]="init_declarator_list"; static char _temp1945[ 22u]="init_declarator_list0";
static char _temp1948[ 16u]="init_declarator"; static char _temp1951[ 19u]="struct_declaration";
static char _temp1954[ 25u]="specifier_qualifier_list"; static char _temp1957[
23u]="struct_declarator_list"; static char _temp1960[ 24u]="struct_declarator_list0";
static char _temp1963[ 18u]="struct_declarator"; static char _temp1966[ 17u]="tunion_specifier";
static char _temp1969[ 18u]="tunion_or_xtunion"; static char _temp1972[ 17u]="tunionfield_list";
static char _temp1975[ 18u]="tunionfield_scope"; static char _temp1978[ 12u]="tunionfield";
static char _temp1981[ 11u]="declarator"; static char _temp1984[ 18u]="direct_declarator";
static char _temp1987[ 8u]="pointer"; static char _temp1990[ 13u]="pointer_char";
static char _temp1993[ 8u]="rgn_opt"; static char _temp1996[ 4u]="rgn"; static
char _temp1999[ 20u]="type_qualifier_list"; static char _temp2002[ 20u]="parameter_type_list";
static char _temp2005[ 11u]="effect_set"; static char _temp2008[ 14u]="atomic_effect";
static char _temp2011[ 11u]="region_set"; static char _temp2014[ 15u]="parameter_list";
static char _temp2017[ 22u]="parameter_declaration"; static char _temp2020[ 16u]="identifier_list";
static char _temp2023[ 17u]="identifier_list0"; static char _temp2026[ 12u]="initializer";
static char _temp2029[ 18u]="array_initializer"; static char _temp2032[ 17u]="initializer_list";
static char _temp2035[ 12u]="designation"; static char _temp2038[ 16u]="designator_list";
static char _temp2041[ 11u]="designator"; static char _temp2044[ 10u]="type_name";
static char _temp2047[ 14u]="any_type_name"; static char _temp2050[ 15u]="type_name_list";
static char _temp2053[ 20u]="abstract_declarator"; static char _temp2056[ 27u]="direct_abstract_declarator";
static char _temp2059[ 10u]="statement"; static char _temp2062[ 18u]="labeled_statement";
static char _temp2065[ 21u]="expression_statement"; static char _temp2068[ 19u]="compound_statement";
static char _temp2071[ 16u]="block_item_list"; static char _temp2074[ 11u]="block_item";
static char _temp2077[ 20u]="selection_statement"; static char _temp2080[ 15u]="switch_clauses";
static char _temp2083[ 20u]="iteration_statement"; static char _temp2086[ 15u]="jump_statement";
static char _temp2089[ 8u]="pattern"; static char _temp2092[ 19u]="tuple_pattern_list";
static char _temp2095[ 20u]="tuple_pattern_list0"; static char _temp2098[ 14u]="field_pattern";
static char _temp2101[ 19u]="field_pattern_list"; static char _temp2104[ 20u]="field_pattern_list0";
static char _temp2107[ 11u]="expression"; static char _temp2110[ 22u]="assignment_expression";
static char _temp2113[ 20u]="assignment_operator"; static char _temp2116[ 23u]="conditional_expression";
static char _temp2119[ 20u]="constant_expression"; static char _temp2122[ 22u]="logical_or_expression";
static char _temp2125[ 23u]="logical_and_expression"; static char _temp2128[ 24u]="inclusive_or_expression";
static char _temp2131[ 24u]="exclusive_or_expression"; static char _temp2134[ 15u]="and_expression";
static char _temp2137[ 20u]="equality_expression"; static char _temp2140[ 22u]="relational_expression";
static char _temp2143[ 17u]="shift_expression"; static char _temp2146[ 20u]="additive_expression";
static char _temp2149[ 26u]="multiplicative_expression"; static char _temp2152[
16u]="cast_expression"; static char _temp2155[ 17u]="unary_expression"; static
char _temp2158[ 14u]="format_primop"; static char _temp2161[ 15u]="unary_operator";
static char _temp2164[ 19u]="postfix_expression"; static char _temp2167[ 19u]="primary_expression";
static char _temp2170[ 25u]="argument_expression_list"; static char _temp2173[
26u]="argument_expression_list0"; static char _temp2176[ 9u]="constant"; static
char _temp2179[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
232u]={( struct _tagged_string){ _temp1486, _temp1486, _temp1486 + 2u},( struct
_tagged_string){ _temp1489, _temp1489, _temp1489 + 6u},( struct _tagged_string){
_temp1492, _temp1492, _temp1492 + 12u},( struct _tagged_string){ _temp1495,
_temp1495, _temp1495 + 5u},( struct _tagged_string){ _temp1498, _temp1498,
_temp1498 + 9u},( struct _tagged_string){ _temp1501, _temp1501, _temp1501 + 7u},(
struct _tagged_string){ _temp1504, _temp1504, _temp1504 + 7u},( struct
_tagged_string){ _temp1507, _temp1507, _temp1507 + 8u},( struct _tagged_string){
_temp1510, _temp1510, _temp1510 + 5u},( struct _tagged_string){ _temp1513,
_temp1513, _temp1513 + 5u},( struct _tagged_string){ _temp1516, _temp1516,
_temp1516 + 6u},( struct _tagged_string){ _temp1519, _temp1519, _temp1519 + 4u},(
struct _tagged_string){ _temp1522, _temp1522, _temp1522 + 5u},( struct
_tagged_string){ _temp1525, _temp1525, _temp1525 + 6u},( struct _tagged_string){
_temp1528, _temp1528, _temp1528 + 7u},( struct _tagged_string){ _temp1531,
_temp1531, _temp1531 + 7u},( struct _tagged_string){ _temp1534, _temp1534,
_temp1534 + 9u},( struct _tagged_string){ _temp1537, _temp1537, _temp1537 + 6u},(
struct _tagged_string){ _temp1540, _temp1540, _temp1540 + 9u},( struct
_tagged_string){ _temp1543, _temp1543, _temp1543 + 9u},( struct _tagged_string){
_temp1546, _temp1546, _temp1546 + 7u},( struct _tagged_string){ _temp1549,
_temp1549, _temp1549 + 6u},( struct _tagged_string){ _temp1552, _temp1552,
_temp1552 + 5u},( struct _tagged_string){ _temp1555, _temp1555, _temp1555 + 8u},(
struct _tagged_string){ _temp1558, _temp1558, _temp1558 + 7u},( struct
_tagged_string){ _temp1561, _temp1561, _temp1561 + 3u},( struct _tagged_string){
_temp1564, _temp1564, _temp1564 + 5u},( struct _tagged_string){ _temp1567,
_temp1567, _temp1567 + 7u},( struct _tagged_string){ _temp1570, _temp1570,
_temp1570 + 6u},( struct _tagged_string){ _temp1573, _temp1573, _temp1573 + 3u},(
struct _tagged_string){ _temp1576, _temp1576, _temp1576 + 4u},( struct
_tagged_string){ _temp1579, _temp1579, _temp1579 + 5u},( struct _tagged_string){
_temp1582, _temp1582, _temp1582 + 9u},( struct _tagged_string){ _temp1585,
_temp1585, _temp1585 + 6u},( struct _tagged_string){ _temp1588, _temp1588,
_temp1588 + 7u},( struct _tagged_string){ _temp1591, _temp1591, _temp1591 + 7u},(
struct _tagged_string){ _temp1594, _temp1594, _temp1594 + 5u},( struct
_tagged_string){ _temp1597, _temp1597, _temp1597 + 8u},( struct _tagged_string){
_temp1600, _temp1600, _temp1600 + 4u},( struct _tagged_string){ _temp1603,
_temp1603, _temp1603 + 6u},( struct _tagged_string){ _temp1606, _temp1606,
_temp1606 + 4u},( struct _tagged_string){ _temp1609, _temp1609, _temp1609 + 6u},(
struct _tagged_string){ _temp1612, _temp1612, _temp1612 + 4u},( struct
_tagged_string){ _temp1615, _temp1615, _temp1615 + 9u},( struct _tagged_string){
_temp1618, _temp1618, _temp1618 + 9u},( struct _tagged_string){ _temp1621,
_temp1621, _temp1621 + 6u},( struct _tagged_string){ _temp1624, _temp1624,
_temp1624 + 10u},( struct _tagged_string){ _temp1627, _temp1627, _temp1627 + 7u},(
struct _tagged_string){ _temp1630, _temp1630, _temp1630 + 8u},( struct
_tagged_string){ _temp1633, _temp1633, _temp1633 + 5u},( struct _tagged_string){
_temp1636, _temp1636, _temp1636 + 8u},( struct _tagged_string){ _temp1639,
_temp1639, _temp1639 + 4u},( struct _tagged_string){ _temp1642, _temp1642,
_temp1642 + 7u},( struct _tagged_string){ _temp1645, _temp1645, _temp1645 + 7u},(
struct _tagged_string){ _temp1648, _temp1648, _temp1648 + 8u},( struct
_tagged_string){ _temp1651, _temp1651, _temp1651 + 8u},( struct _tagged_string){
_temp1654, _temp1654, _temp1654 + 6u},( struct _tagged_string){ _temp1657,
_temp1657, _temp1657 + 7u},( struct _tagged_string){ _temp1660, _temp1660,
_temp1660 + 7u},( struct _tagged_string){ _temp1663, _temp1663, _temp1663 + 7u},(
struct _tagged_string){ _temp1666, _temp1666, _temp1666 + 9u},( struct
_tagged_string){ _temp1669, _temp1669, _temp1669 + 7u},( struct _tagged_string){
_temp1672, _temp1672, _temp1672 + 5u},( struct _tagged_string){ _temp1675,
_temp1675, _temp1675 + 8u},( struct _tagged_string){ _temp1678, _temp1678,
_temp1678 + 7u},( struct _tagged_string){ _temp1681, _temp1681, _temp1681 + 7u},(
struct _tagged_string){ _temp1684, _temp1684, _temp1684 + 7u},( struct
_tagged_string){ _temp1687, _temp1687, _temp1687 + 8u},( struct _tagged_string){
_temp1690, _temp1690, _temp1690 + 9u},( struct _tagged_string){ _temp1693,
_temp1693, _temp1693 + 6u},( struct _tagged_string){ _temp1696, _temp1696,
_temp1696 + 6u},( struct _tagged_string){ _temp1699, _temp1699, _temp1699 + 6u},(
struct _tagged_string){ _temp1702, _temp1702, _temp1702 + 6u},( struct
_tagged_string){ _temp1705, _temp1705, _temp1705 + 7u},( struct _tagged_string){
_temp1708, _temp1708, _temp1708 + 6u},( struct _tagged_string){ _temp1711,
_temp1711, _temp1711 + 11u},( struct _tagged_string){ _temp1714, _temp1714,
_temp1714 + 11u},( struct _tagged_string){ _temp1717, _temp1717, _temp1717 + 11u},(
struct _tagged_string){ _temp1720, _temp1720, _temp1720 + 11u},( struct
_tagged_string){ _temp1723, _temp1723, _temp1723 + 11u},( struct _tagged_string){
_temp1726, _temp1726, _temp1726 + 12u},( struct _tagged_string){ _temp1729,
_temp1729, _temp1729 + 13u},( struct _tagged_string){ _temp1732, _temp1732,
_temp1732 + 11u},( struct _tagged_string){ _temp1735, _temp1735, _temp1735 + 11u},(
struct _tagged_string){ _temp1738, _temp1738, _temp1738 + 10u},( struct
_tagged_string){ _temp1741, _temp1741, _temp1741 + 9u},( struct _tagged_string){
_temp1744, _temp1744, _temp1744 + 11u},( struct _tagged_string){ _temp1747,
_temp1747, _temp1747 + 12u},( struct _tagged_string){ _temp1750, _temp1750,
_temp1750 + 11u},( struct _tagged_string){ _temp1753, _temp1753, _temp1753 + 17u},(
struct _tagged_string){ _temp1756, _temp1756, _temp1756 + 7u},( struct
_tagged_string){ _temp1759, _temp1759, _temp1759 + 19u},( struct _tagged_string){
_temp1762, _temp1762, _temp1762 + 18u},( struct _tagged_string){ _temp1765,
_temp1765, _temp1765 + 9u},( struct _tagged_string){ _temp1768, _temp1768,
_temp1768 + 16u},( struct _tagged_string){ _temp1771, _temp1771, _temp1771 + 18u},(
struct _tagged_string){ _temp1774, _temp1774, _temp1774 + 10u},( struct
_tagged_string){ _temp1777, _temp1777, _temp1777 + 4u},( struct _tagged_string){
_temp1780, _temp1780, _temp1780 + 4u},( struct _tagged_string){ _temp1783,
_temp1783, _temp1783 + 4u},( struct _tagged_string){ _temp1786, _temp1786,
_temp1786 + 4u},( struct _tagged_string){ _temp1789, _temp1789, _temp1789 + 4u},(
struct _tagged_string){ _temp1792, _temp1792, _temp1792 + 4u},( struct
_tagged_string){ _temp1795, _temp1795, _temp1795 + 4u},( struct _tagged_string){
_temp1798, _temp1798, _temp1798 + 4u},( struct _tagged_string){ _temp1801,
_temp1801, _temp1801 + 4u},( struct _tagged_string){ _temp1804, _temp1804,
_temp1804 + 4u},( struct _tagged_string){ _temp1807, _temp1807, _temp1807 + 4u},(
struct _tagged_string){ _temp1810, _temp1810, _temp1810 + 4u},( struct
_tagged_string){ _temp1813, _temp1813, _temp1813 + 4u},( struct _tagged_string){
_temp1816, _temp1816, _temp1816 + 4u},( struct _tagged_string){ _temp1819,
_temp1819, _temp1819 + 4u},( struct _tagged_string){ _temp1822, _temp1822,
_temp1822 + 4u},( struct _tagged_string){ _temp1825, _temp1825, _temp1825 + 4u},(
struct _tagged_string){ _temp1828, _temp1828, _temp1828 + 4u},( struct
_tagged_string){ _temp1831, _temp1831, _temp1831 + 4u},( struct _tagged_string){
_temp1834, _temp1834, _temp1834 + 4u},( struct _tagged_string){ _temp1837,
_temp1837, _temp1837 + 4u},( struct _tagged_string){ _temp1840, _temp1840,
_temp1840 + 4u},( struct _tagged_string){ _temp1843, _temp1843, _temp1843 + 4u},(
struct _tagged_string){ _temp1846, _temp1846, _temp1846 + 4u},( struct
_tagged_string){ _temp1849, _temp1849, _temp1849 + 4u},( struct _tagged_string){
_temp1852, _temp1852, _temp1852 + 4u},( struct _tagged_string){ _temp1855,
_temp1855, _temp1855 + 4u},( struct _tagged_string){ _temp1858, _temp1858,
_temp1858 + 5u},( struct _tagged_string){ _temp1861, _temp1861, _temp1861 + 17u},(
struct _tagged_string){ _temp1864, _temp1864, _temp1864 + 21u},( struct
_tagged_string){ _temp1867, _temp1867, _temp1867 + 20u},( struct _tagged_string){
_temp1870, _temp1870, _temp1870 + 21u},( struct _tagged_string){ _temp1873,
_temp1873, _temp1873 + 13u},( struct _tagged_string){ _temp1876, _temp1876,
_temp1876 + 15u},( struct _tagged_string){ _temp1879, _temp1879, _temp1879 + 17u},(
struct _tagged_string){ _temp1882, _temp1882, _temp1882 + 19u},( struct
_tagged_string){ _temp1885, _temp1885, _temp1885 + 12u},( struct _tagged_string){
_temp1888, _temp1888, _temp1888 + 17u},( struct _tagged_string){ _temp1891,
_temp1891, _temp1891 + 23u},( struct _tagged_string){ _temp1894, _temp1894,
_temp1894 + 24u},( struct _tagged_string){ _temp1897, _temp1897, _temp1897 + 15u},(
struct _tagged_string){ _temp1900, _temp1900, _temp1900 + 11u},( struct
_tagged_string){ _temp1903, _temp1903, _temp1903 + 15u},( struct _tagged_string){
_temp1906, _temp1906, _temp1906 + 10u},( struct _tagged_string){ _temp1909,
_temp1909, _temp1909 + 15u},( struct _tagged_string){ _temp1912, _temp1912,
_temp1912 + 5u},( struct _tagged_string){ _temp1915, _temp1915, _temp1915 + 15u},(
struct _tagged_string){ _temp1918, _temp1918, _temp1918 + 15u},( struct
_tagged_string){ _temp1921, _temp1921, _temp1921 + 11u},( struct _tagged_string){
_temp1924, _temp1924, _temp1924 + 22u},( struct _tagged_string){ _temp1927,
_temp1927, _temp1927 + 26u},( struct _tagged_string){ _temp1930, _temp1930,
_temp1930 + 16u},( struct _tagged_string){ _temp1933, _temp1933, _temp1933 + 16u},(
struct _tagged_string){ _temp1936, _temp1936, _temp1936 + 24u},( struct
_tagged_string){ _temp1939, _temp1939, _temp1939 + 25u},( struct _tagged_string){
_temp1942, _temp1942, _temp1942 + 21u},( struct _tagged_string){ _temp1945,
_temp1945, _temp1945 + 22u},( struct _tagged_string){ _temp1948, _temp1948,
_temp1948 + 16u},( struct _tagged_string){ _temp1951, _temp1951, _temp1951 + 19u},(
struct _tagged_string){ _temp1954, _temp1954, _temp1954 + 25u},( struct
_tagged_string){ _temp1957, _temp1957, _temp1957 + 23u},( struct _tagged_string){
_temp1960, _temp1960, _temp1960 + 24u},( struct _tagged_string){ _temp1963,
_temp1963, _temp1963 + 18u},( struct _tagged_string){ _temp1966, _temp1966,
_temp1966 + 17u},( struct _tagged_string){ _temp1969, _temp1969, _temp1969 + 18u},(
struct _tagged_string){ _temp1972, _temp1972, _temp1972 + 17u},( struct
_tagged_string){ _temp1975, _temp1975, _temp1975 + 18u},( struct _tagged_string){
_temp1978, _temp1978, _temp1978 + 12u},( struct _tagged_string){ _temp1981,
_temp1981, _temp1981 + 11u},( struct _tagged_string){ _temp1984, _temp1984,
_temp1984 + 18u},( struct _tagged_string){ _temp1987, _temp1987, _temp1987 + 8u},(
struct _tagged_string){ _temp1990, _temp1990, _temp1990 + 13u},( struct
_tagged_string){ _temp1993, _temp1993, _temp1993 + 8u},( struct _tagged_string){
_temp1996, _temp1996, _temp1996 + 4u},( struct _tagged_string){ _temp1999,
_temp1999, _temp1999 + 20u},( struct _tagged_string){ _temp2002, _temp2002,
_temp2002 + 20u},( struct _tagged_string){ _temp2005, _temp2005, _temp2005 + 11u},(
struct _tagged_string){ _temp2008, _temp2008, _temp2008 + 14u},( struct
_tagged_string){ _temp2011, _temp2011, _temp2011 + 11u},( struct _tagged_string){
_temp2014, _temp2014, _temp2014 + 15u},( struct _tagged_string){ _temp2017,
_temp2017, _temp2017 + 22u},( struct _tagged_string){ _temp2020, _temp2020,
_temp2020 + 16u},( struct _tagged_string){ _temp2023, _temp2023, _temp2023 + 17u},(
struct _tagged_string){ _temp2026, _temp2026, _temp2026 + 12u},( struct
_tagged_string){ _temp2029, _temp2029, _temp2029 + 18u},( struct _tagged_string){
_temp2032, _temp2032, _temp2032 + 17u},( struct _tagged_string){ _temp2035,
_temp2035, _temp2035 + 12u},( struct _tagged_string){ _temp2038, _temp2038,
_temp2038 + 16u},( struct _tagged_string){ _temp2041, _temp2041, _temp2041 + 11u},(
struct _tagged_string){ _temp2044, _temp2044, _temp2044 + 10u},( struct
_tagged_string){ _temp2047, _temp2047, _temp2047 + 14u},( struct _tagged_string){
_temp2050, _temp2050, _temp2050 + 15u},( struct _tagged_string){ _temp2053,
_temp2053, _temp2053 + 20u},( struct _tagged_string){ _temp2056, _temp2056,
_temp2056 + 27u},( struct _tagged_string){ _temp2059, _temp2059, _temp2059 + 10u},(
struct _tagged_string){ _temp2062, _temp2062, _temp2062 + 18u},( struct
_tagged_string){ _temp2065, _temp2065, _temp2065 + 21u},( struct _tagged_string){
_temp2068, _temp2068, _temp2068 + 19u},( struct _tagged_string){ _temp2071,
_temp2071, _temp2071 + 16u},( struct _tagged_string){ _temp2074, _temp2074,
_temp2074 + 11u},( struct _tagged_string){ _temp2077, _temp2077, _temp2077 + 20u},(
struct _tagged_string){ _temp2080, _temp2080, _temp2080 + 15u},( struct
_tagged_string){ _temp2083, _temp2083, _temp2083 + 20u},( struct _tagged_string){
_temp2086, _temp2086, _temp2086 + 15u},( struct _tagged_string){ _temp2089,
_temp2089, _temp2089 + 8u},( struct _tagged_string){ _temp2092, _temp2092,
_temp2092 + 19u},( struct _tagged_string){ _temp2095, _temp2095, _temp2095 + 20u},(
struct _tagged_string){ _temp2098, _temp2098, _temp2098 + 14u},( struct
_tagged_string){ _temp2101, _temp2101, _temp2101 + 19u},( struct _tagged_string){
_temp2104, _temp2104, _temp2104 + 20u},( struct _tagged_string){ _temp2107,
_temp2107, _temp2107 + 11u},( struct _tagged_string){ _temp2110, _temp2110,
_temp2110 + 22u},( struct _tagged_string){ _temp2113, _temp2113, _temp2113 + 20u},(
struct _tagged_string){ _temp2116, _temp2116, _temp2116 + 23u},( struct
_tagged_string){ _temp2119, _temp2119, _temp2119 + 20u},( struct _tagged_string){
_temp2122, _temp2122, _temp2122 + 22u},( struct _tagged_string){ _temp2125,
_temp2125, _temp2125 + 23u},( struct _tagged_string){ _temp2128, _temp2128,
_temp2128 + 24u},( struct _tagged_string){ _temp2131, _temp2131, _temp2131 + 24u},(
struct _tagged_string){ _temp2134, _temp2134, _temp2134 + 15u},( struct
_tagged_string){ _temp2137, _temp2137, _temp2137 + 20u},( struct _tagged_string){
_temp2140, _temp2140, _temp2140 + 22u},( struct _tagged_string){ _temp2143,
_temp2143, _temp2143 + 17u},( struct _tagged_string){ _temp2146, _temp2146,
_temp2146 + 20u},( struct _tagged_string){ _temp2149, _temp2149, _temp2149 + 26u},(
struct _tagged_string){ _temp2152, _temp2152, _temp2152 + 16u},( struct
_tagged_string){ _temp2155, _temp2155, _temp2155 + 17u},( struct _tagged_string){
_temp2158, _temp2158, _temp2158 + 14u},( struct _tagged_string){ _temp2161,
_temp2161, _temp2161 + 15u},( struct _tagged_string){ _temp2164, _temp2164,
_temp2164 + 19u},( struct _tagged_string){ _temp2167, _temp2167, _temp2167 + 19u},(
struct _tagged_string){ _temp2170, _temp2170, _temp2170 + 25u},( struct
_tagged_string){ _temp2173, _temp2173, _temp2173 + 26u},( struct _tagged_string){
_temp2176, _temp2176, _temp2176 + 9u},( struct _tagged_string){ _temp2179,
_temp2179, _temp2179 + 20u}}; static short Cyc_yyr1[ 394u]={ (short)0, (short)124,
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
_temp4193=( short*)({ unsigned int _temp4189= 200u; short* _temp4190=( short*)
GC_malloc_atomic( sizeof( short) * _temp4189);{ unsigned int _temp4191=
_temp4189; unsigned int i; for( i= 0; i < _temp4191; i ++){ _temp4190[ i]=
(short)0;}}; _temp4190;}); struct _tagged_ptr1 _temp4194; _temp4194.curr=
_temp4193; _temp4194.base= _temp4193; _temp4194.last_plus_one= _temp4193 + 200;
_temp4194;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=( struct _tagged_ptr2)({
void** _temp4187=( void**)({ unsigned int _temp4183= 200u; void** _temp4184=(
void**) GC_malloc( sizeof( void*) * _temp4183);{ unsigned int _temp4185=
_temp4183; unsigned int i; for( i= 0; i < _temp4185; i ++){ _temp4184[ i]= Cyc_yylval;}};
_temp4184;}); struct _tagged_ptr2 _temp4188; _temp4188.curr= _temp4187;
_temp4188.base= _temp4187; _temp4188.last_plus_one= _temp4187 + 200; _temp4188;});
int yylsp_offset; struct _tagged_ptr3 yyls=( struct _tagged_ptr3)({ struct Cyc_Yyltype*
_temp4181=( struct Cyc_Yyltype*)({ unsigned int _temp4177= 200u; struct Cyc_Yyltype*
_temp4178=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp4177);{ unsigned int _temp4179= _temp4177; unsigned int i; for( i= 0; i <
_temp4179; i ++){ _temp4178[ i]= Cyc_yynewloc();}}; _temp4178;}); struct
_tagged_ptr3 _temp4182; _temp4182.curr= _temp4181; _temp4182.base= _temp4181;
_temp4182.last_plus_one= _temp4181 + 200; _temp4182;}); int yystacksize= 200;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:*((
short*(*)( struct _tagged_ptr1, unsigned int, unsigned int))
_check_unknown_subscript)( yyss, sizeof( short),( ++ yyssp_offset))=( short)
yystate; if( yyssp_offset >= yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror((
struct _tagged_string)({ char* _temp2180=( char*)"parser stack overflow"; struct
_tagged_string _temp2181; _temp2181.curr= _temp2180; _temp2181.base= _temp2180;
_temp2181.last_plus_one= _temp2180 + 22; _temp2181;}));( void) _throw(( void*)
Cyc_Yystack_overflow);} yystacksize *= 2; if( yystacksize > 10000){ yystacksize=
10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp2192=( unsigned int)
yystacksize; short* _temp2193=( short*) GC_malloc_atomic( sizeof( short) *
_temp2192); struct _tagged_ptr1 _temp2196={ _temp2193, _temp2193, _temp2193 +
_temp2192};{ unsigned int _temp2194= _temp2192; unsigned int i; for( i= 0; i <
_temp2194; i ++){ _temp2193[ i]= (short)0;}}; _temp2196;}); struct _tagged_ptr2
yyvs1=({ unsigned int _temp2187=( unsigned int) yystacksize; void** _temp2188=(
void**) GC_malloc( sizeof( void*) * _temp2187); struct _tagged_ptr2 _temp2191={
_temp2188, _temp2188, _temp2188 + _temp2187};{ unsigned int _temp2189= _temp2187;
unsigned int i; for( i= 0; i < _temp2189; i ++){ _temp2188[ i]= Cyc_yylval;}};
_temp2191;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2182=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2183=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2182); struct _tagged_ptr3 _temp2186={
_temp2183, _temp2183, _temp2183 + _temp2182};{ unsigned int _temp2184= _temp2182;
unsigned int i; for( i= 0; i < _temp2184; i ++){ _temp2183[ i]= Cyc_yynewloc();}};
_temp2186;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){*(( short*(*)( struct
_tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( yyss1,
sizeof( short), i)=*(( short*(*)( struct _tagged_ptr1, unsigned int,
unsigned int)) _check_unknown_subscript)( yyss, sizeof( short), i);*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs1, sizeof( void*), i)=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), i);*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls1, sizeof( struct Cyc_Yyltype), i)=*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), i);}} yyss= yyss1; yyvs= yyvs1; yyls= yyls1;}}
goto yybackup; yybackup: yyn=( int)(( short*) Cyc_yypact)[
_check_known_subscript_notnull( 751u, yystate)]; if( yyn == - 32768){ goto
yydefault;} if( Cyc_yychar == - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <=
0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <=
351: 0)?( int)(( short*) Cyc_yytranslate)[ _check_known_subscript_notnull( 352u,
Cyc_yychar)]: 232;} yyn += yychar1; if(( yyn < 0? 1: yyn > 4222)? 1:(( short*)
Cyc_yycheck)[ _check_known_subscript_notnull( 4223u, yyn)] != yychar1){ goto
yydefault;} yyn=( int)(( short*) Cyc_yytable)[ _check_known_subscript_notnull(
4223u, yyn)]; if( yyn < 0){ if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn;
goto yyreduce;} else{ if( yyn == 0){ goto yyerrlab;}} if( yyn == 750){ return 0;}
if( Cyc_yychar != 0){ Cyc_yychar= - 2;}*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),(
++ yyvsp_offset))= Cyc_yylval;*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),(
++ yylsp_offset))= Cyc_yylloc; if( yyerrstatus != 0){ yyerrstatus --;} yystate=
yyn; goto yynewstate; yydefault: yyn=( int)(( short*) Cyc_yydefact)[
_check_known_subscript_notnull( 751u, yystate)]; if( yyn == 0){ goto yyerrlab;}
yyreduce: yylen=( int)(( short*) Cyc_yyr2)[ _check_known_subscript_notnull( 394u,
yyn)]; if( yylen > 0){ yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),( yyvsp_offset +
1) - yylen);} switch( yyn){ case 1: _LL2197: yyval=*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); break; case 2:
_LL2198: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2200=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2200[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2201; _temp2201.tag= Cyc_DeclList_tok; _temp2201.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp2201;});
_temp2200;}); break; case 3: _LL2199: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2203=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2203[ 0]=({ struct Cyc_DeclList_tok_struct _temp2204; _temp2204.tag= Cyc_DeclList_tok;
_temp2204.f1=({ struct Cyc_List_List* _temp2205=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2205->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2206=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2206->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2207=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2207[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2208; _temp2208.tag= Cyc_Absyn_Using_d;
_temp2208.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2208.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2208;}); _temp2207;})); _temp2206->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2206;}); _temp2205->tl= 0; _temp2205;}); _temp2204;}); _temp2203;}); Cyc_Lex_leave_using();
break; case 4: _LL2202: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2210=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2210[ 0]=({ struct Cyc_DeclList_tok_struct _temp2211; _temp2211.tag= Cyc_DeclList_tok;
_temp2211.f1=({ struct Cyc_List_List* _temp2212=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2212->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2213=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2213->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2214=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2214[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2215; _temp2215.tag= Cyc_Absyn_Using_d;
_temp2215.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2215.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2215;}); _temp2214;})); _temp2213->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2213;}); _temp2212->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2212;});
_temp2211;}); _temp2210;}); break; case 5: _LL2209: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2217=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2217[ 0]=({ struct Cyc_DeclList_tok_struct _temp2218; _temp2218.tag= Cyc_DeclList_tok;
_temp2218.f1=({ struct Cyc_List_List* _temp2219=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2219->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2220=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2220->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2221=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2221[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2222; _temp2222.tag=
Cyc_Absyn_Namespace_d; _temp2222.f1=({ struct _tagged_string* _temp2223=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2223[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2223;}); _temp2222.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2222;}); _temp2221;})); _temp2220->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2220;}); _temp2219->tl= 0; _temp2219;}); _temp2218;}); _temp2217;}); Cyc_Lex_leave_namespace();
break; case 6: _LL2216: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2225=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2225[ 0]=({ struct Cyc_DeclList_tok_struct _temp2226; _temp2226.tag= Cyc_DeclList_tok;
_temp2226.f1=({ struct Cyc_List_List* _temp2227=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2227->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2228=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2228->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2229=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2229[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2230; _temp2230.tag=
Cyc_Absyn_Namespace_d; _temp2230.f1=({ struct _tagged_string* _temp2231=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2231[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4));
_temp2231;}); _temp2230.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2230;}); _temp2229;})); _temp2228->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2228;}); _temp2227->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2227;});
_temp2226;}); _temp2225;}); break; case 7: _LL2224: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),( struct
_tagged_string)({ char* _temp2233=( char*)"C"; struct _tagged_string _temp2234;
_temp2234.curr= _temp2233; _temp2234.base= _temp2233; _temp2234.last_plus_one=
_temp2233 + 2; _temp2234;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2235=( char*)"only extern \"C\" { ... } is allowed"; struct
_tagged_string _temp2236; _temp2236.curr= _temp2235; _temp2236.base= _temp2235;
_temp2236.last_plus_one= _temp2235 + 35; _temp2236;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).last_line));}
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2237=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2237[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2238; _temp2238.tag= Cyc_DeclList_tok; _temp2238.f1=({ struct Cyc_List_List*
_temp2239=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2239->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2240=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2240->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp2241=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp2241[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp2242; _temp2242.tag= Cyc_Absyn_ExternC_d;
_temp2242.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2242;}); _temp2241;})); _temp2240->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp2240;}); _temp2239->tl= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2239;}); _temp2238;}); _temp2237;}); break;
case 8: _LL2232: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2244=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2244[ 0]=({ struct Cyc_DeclList_tok_struct _temp2245; _temp2245.tag= Cyc_DeclList_tok;
_temp2245.f1= 0; _temp2245;}); _temp2244;}); break; case 9: _LL2243: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp2247=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2247[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2248; _temp2248.tag= Cyc_DeclList_tok; _temp2248.f1=({ struct Cyc_List_List*
_temp2249=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2249->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2250=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2250[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2251; _temp2251.tag= Cyc_Absyn_Fn_d;
_temp2251.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2251;}); _temp2250;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2249->tl= 0; _temp2249;}); _temp2248;}); _temp2247;}); break; case 10:
_LL2246: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 11:
_LL2252: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2254=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2254[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2255; _temp2255.tag= Cyc_FnDecl_tok; _temp2255.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2255;}); _temp2254;}); break; case 12: _LL2253:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2257=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2257[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2258; _temp2258.tag= Cyc_FnDecl_tok; _temp2258.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2259=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2259->v=( void*) Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2259;}), Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2258;}); _temp2257;}); break; case 13: _LL2256: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2261=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2261[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2262; _temp2262.tag= Cyc_FnDecl_tok;
_temp2262.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2262;}); _temp2261;}); break; case 14: _LL2260: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2264=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2264[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2265; _temp2265.tag= Cyc_FnDecl_tok;
_temp2265.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2266=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2266->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2266;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2265;});
_temp2264;}); break; case 15: _LL2263: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2268=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2268[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2269; _temp2269.tag= Cyc_FnDecl_tok;
_temp2269.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2270=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2270->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2270;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2269;});
_temp2268;}); break; case 16: _LL2267: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2272=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2272[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2273; _temp2273.tag= Cyc_FnDecl_tok;
_temp2273.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2274=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2274->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2274;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2273;});
_temp2272;}); break; case 17: _LL2271: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 18: _LL2275: Cyc_Lex_leave_using();
break; case 19: _LL2276: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2278=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2278[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2278;})); yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 20: _LL2277: Cyc_Lex_leave_namespace(); break; case
21: _LL2279: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2281=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2281[ 0]=({ struct Cyc_DeclList_tok_struct _temp2282; _temp2282.tag= Cyc_DeclList_tok;
_temp2282.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp2282;}); _temp2281;}); break; case 22: _LL2280: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2284=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2284[ 0]=({ struct Cyc_DeclList_tok_struct _temp2285; _temp2285.tag= Cyc_DeclList_tok;
_temp2285.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_InitDeclList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2285;}); _temp2284;}); break; case 23: _LL2283: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2287=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2287[ 0]=({ struct Cyc_DeclList_tok_struct _temp2288; _temp2288.tag= Cyc_DeclList_tok;
_temp2288.f1=({ struct Cyc_List_List* _temp2289=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2289->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
0, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2289->tl= 0; _temp2289;}); _temp2288;});
_temp2287;}); break; case 24: _LL2286: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 25: _LL2290: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2292=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2292[ 0]=({ struct Cyc_DeclList_tok_struct _temp2293; _temp2293.tag= Cyc_DeclList_tok;
_temp2293.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp2293;}); _temp2292;}); break; case 26:
_LL2291: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2295=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2295[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2296; _temp2296.tag= Cyc_DeclSpec_tok; _temp2296.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2297=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2297->sc=({ struct Cyc_Core_Opt* _temp2298=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2298->v=( void*) Cyc_yyget_StorageClass_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2298;});
_temp2297->tq= Cyc_Absyn_empty_tqual(); _temp2297->type_specs= 0; _temp2297->is_inline=
0; _temp2297->attributes= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2297;}); _temp2296;}); _temp2295;}); break;
case 27: _LL2294: if(( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->sc != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp2300=( char*)"Only one storage class is allowed in a declaration"; struct
_tagged_string _temp2301; _temp2301.curr= _temp2300; _temp2301.base= _temp2300;
_temp2301.last_plus_one= _temp2300 + 51; _temp2301;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2302=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2302[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2303; _temp2303.tag= Cyc_DeclSpec_tok; _temp2303.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2304=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2304->sc=({ struct Cyc_Core_Opt* _temp2305=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2305->v=( void*) Cyc_yyget_StorageClass_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2305;});
_temp2304->tq=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tq; _temp2304->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->type_specs; _temp2304->is_inline=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->is_inline;
_temp2304->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2304;}); _temp2303;}); _temp2302;}); break; case 28: _LL2299: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2307=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2307[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2308; _temp2308.tag= Cyc_DeclSpec_tok; _temp2308.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2309=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2309->sc= 0; _temp2309->tq= Cyc_Absyn_empty_tqual(); _temp2309->type_specs=({
struct Cyc_List_List* _temp2310=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2310->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2310->tl=
0; _temp2310;}); _temp2309->is_inline= 0; _temp2309->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2309;});
_temp2308;}); _temp2307;}); break; case 29: _LL2306: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2312=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2312[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2313; _temp2313.tag= Cyc_DeclSpec_tok;
_temp2313.f1=({ struct Cyc_Parse_Declaration_spec* _temp2314=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2314->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2314->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2314->type_specs=({ struct Cyc_List_List* _temp2315=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2315->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2315->tl=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2315;}); _temp2314->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->is_inline; _temp2314->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2314;}); _temp2313;}); _temp2312;}); break; case 30: _LL2311: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2317=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2317[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2318; _temp2318.tag= Cyc_DeclSpec_tok; _temp2318.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2319=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2319->sc= 0; _temp2319->tq= Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2319->type_specs= 0; _temp2319->is_inline=
0; _temp2319->attributes= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2319;}); _temp2318;}); _temp2317;}); break;
case 31: _LL2316: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2321=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2321[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2322; _temp2322.tag= Cyc_DeclSpec_tok;
_temp2322.f1=({ struct Cyc_Parse_Declaration_spec* _temp2323=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2323->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2323->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)),( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tq); _temp2323->type_specs=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2323->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->is_inline; _temp2323->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2323;}); _temp2322;}); _temp2321;}); break; case 32: _LL2320: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2325=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2325[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2326; _temp2326.tag= Cyc_DeclSpec_tok; _temp2326.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2327=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2327->sc= 0; _temp2327->tq= Cyc_Absyn_empty_tqual(); _temp2327->type_specs=
0; _temp2327->is_inline= 1; _temp2327->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2327;});
_temp2326;}); _temp2325;}); break; case 33: _LL2324: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2329=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2329[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2330; _temp2330.tag= Cyc_DeclSpec_tok;
_temp2330.f1=({ struct Cyc_Parse_Declaration_spec* _temp2331=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2331->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2331->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2331->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->type_specs; _temp2331->is_inline= 1; _temp2331->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2331;}); _temp2330;}); _temp2329;}); break; case 34: _LL2328: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp2333=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2333[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2334; _temp2334.tag= Cyc_StorageClass_tok;
_temp2334.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp2334;}); _temp2333;});
break; case 35: _LL2332: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2336=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2336[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2337; _temp2337.tag=
Cyc_StorageClass_tok; _temp2337.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp2337;}); _temp2336;}); break; case 36: _LL2335: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2339=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2339[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2340; _temp2340.tag=
Cyc_StorageClass_tok; _temp2340.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2340;}); _temp2339;}); break; case 37: _LL2338: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2342=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2342[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2343; _temp2343.tag=
Cyc_StorageClass_tok; _temp2343.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp2343;}); _temp2342;}); break; case 38: _LL2341: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),( struct
_tagged_string)({ char* _temp2345=( char*)"C"; struct _tagged_string _temp2346;
_temp2346.curr= _temp2345; _temp2346.base= _temp2345; _temp2346.last_plus_one=
_temp2345 + 2; _temp2346;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2347=( char*)"only extern or extern \"C\" is allowed"; struct
_tagged_string _temp2348; _temp2348.curr= _temp2347; _temp2348.base= _temp2347;
_temp2348.last_plus_one= _temp2347 + 37; _temp2348;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp2349=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2349[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2350; _temp2350.tag= Cyc_StorageClass_tok;
_temp2350.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp2350;}); _temp2349;});
break; case 39: _LL2344: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2352=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2352[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2353; _temp2353.tag=
Cyc_StorageClass_tok; _temp2353.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp2353;}); _temp2352;}); break; case 40: _LL2351: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2355=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2355[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2356; _temp2356.tag=
Cyc_StorageClass_tok; _temp2356.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2356;}); _temp2355;}); break; case 41: _LL2354: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2358=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2358[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2359; _temp2359.tag=
Cyc_AttributeList_tok; _temp2359.f1= 0; _temp2359;}); _temp2358;}); break; case
42: _LL2357: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 43: _LL2360: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2362=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2362[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2363; _temp2363.tag=
Cyc_AttributeList_tok; _temp2363.f1= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2363;}); _temp2362;}); break;
case 44: _LL2361: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2365=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2365[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2366; _temp2366.tag=
Cyc_AttributeList_tok; _temp2366.f1=({ struct Cyc_List_List* _temp2367=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2367->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2367->tl= 0; _temp2367;}); _temp2366;}); _temp2365;}); break; case 45:
_LL2364: yyval=( void*)({ struct Cyc_AttributeList_tok_struct* _temp2369=(
struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2369[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2370; _temp2370.tag=
Cyc_AttributeList_tok; _temp2370.f1=({ struct Cyc_List_List* _temp2371=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2371->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2371->tl= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2371;}); _temp2370;}); _temp2369;}); break; case 46:
_LL2368: { struct _tagged_string _temp2373= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); goto _LL2374; _LL2374: { void* a; if( Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2375=( char*)"stdcall"; struct
_tagged_string _temp2376; _temp2376.curr= _temp2375; _temp2376.base= _temp2375;
_temp2376.last_plus_one= _temp2375 + 8; _temp2376;})) == 0? 1: Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2377=( char*)"__stdcall__";
struct _tagged_string _temp2378; _temp2378.curr= _temp2377; _temp2378.base=
_temp2377; _temp2378.last_plus_one= _temp2377 + 12; _temp2378;})) == 0){ a=(
void*) Cyc_Absyn_Stdcall_att;} else{ if( Cyc_String_zstrcmp( _temp2373,( struct
_tagged_string)({ char* _temp2379=( char*)"cdecl"; struct _tagged_string
_temp2380; _temp2380.curr= _temp2379; _temp2380.base= _temp2379; _temp2380.last_plus_one=
_temp2379 + 6; _temp2380;})) == 0? 1: Cyc_String_zstrcmp( _temp2373,( struct
_tagged_string)({ char* _temp2381=( char*)"__cdecl__"; struct _tagged_string
_temp2382; _temp2382.curr= _temp2381; _temp2382.base= _temp2381; _temp2382.last_plus_one=
_temp2381 + 10; _temp2382;})) == 0){ a=( void*) Cyc_Absyn_Cdecl_att;} else{ if(
Cyc_String_zstrcmp( _temp2373,( struct _tagged_string)({ char* _temp2383=( char*)"noreturn";
struct _tagged_string _temp2384; _temp2384.curr= _temp2383; _temp2384.base=
_temp2383; _temp2384.last_plus_one= _temp2383 + 9; _temp2384;})) == 0? 1: Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2385=( char*)"__noreturn__";
struct _tagged_string _temp2386; _temp2386.curr= _temp2385; _temp2386.base=
_temp2385; _temp2386.last_plus_one= _temp2385 + 13; _temp2386;})) == 0){ a=(
void*) Cyc_Absyn_Noreturn_att;} else{ if( Cyc_String_zstrcmp( _temp2373,( struct
_tagged_string)({ char* _temp2387=( char*)"noreturn"; struct _tagged_string
_temp2388; _temp2388.curr= _temp2387; _temp2388.base= _temp2387; _temp2388.last_plus_one=
_temp2387 + 9; _temp2388;})) == 0? 1: Cyc_String_zstrcmp( _temp2373,( struct
_tagged_string)({ char* _temp2389=( char*)"__noreturn__"; struct _tagged_string
_temp2390; _temp2390.curr= _temp2389; _temp2390.base= _temp2389; _temp2390.last_plus_one=
_temp2389 + 13; _temp2390;})) == 0){ a=( void*) Cyc_Absyn_Noreturn_att;} else{
if( Cyc_String_zstrcmp( _temp2373,( struct _tagged_string)({ char* _temp2391=(
char*)"__const__"; struct _tagged_string _temp2392; _temp2392.curr= _temp2391;
_temp2392.base= _temp2391; _temp2392.last_plus_one= _temp2391 + 10; _temp2392;}))
== 0){ a=( void*) Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( _temp2373,(
struct _tagged_string)({ char* _temp2393=( char*)"aligned"; struct
_tagged_string _temp2394; _temp2394.curr= _temp2393; _temp2394.base= _temp2393;
_temp2394.last_plus_one= _temp2393 + 8; _temp2394;})) == 0? 1: Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2395=( char*)"__aligned__";
struct _tagged_string _temp2396; _temp2396.curr= _temp2395; _temp2396.base=
_temp2395; _temp2396.last_plus_one= _temp2395 + 12; _temp2396;})) == 0){ a=(
void*)({ struct Cyc_Absyn_Aligned_att_struct* _temp2397=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2397[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2398; _temp2398.tag= Cyc_Absyn_Aligned_att;
_temp2398.f1= - 1; _temp2398;}); _temp2397;});} else{ if( Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2399=( char*)"packed"; struct
_tagged_string _temp2400; _temp2400.curr= _temp2399; _temp2400.base= _temp2399;
_temp2400.last_plus_one= _temp2399 + 7; _temp2400;})) == 0? 1: Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2401=( char*)"__packed__";
struct _tagged_string _temp2402; _temp2402.curr= _temp2401; _temp2402.base=
_temp2401; _temp2402.last_plus_one= _temp2401 + 11; _temp2402;})) == 0){ a=(
void*) Cyc_Absyn_Packed_att;} else{ if( Cyc_String_zstrcmp( _temp2373,( struct
_tagged_string)({ char* _temp2403=( char*)"shared"; struct _tagged_string
_temp2404; _temp2404.curr= _temp2403; _temp2404.base= _temp2403; _temp2404.last_plus_one=
_temp2403 + 7; _temp2404;})) == 0? 1: Cyc_String_zstrcmp( _temp2373,( struct
_tagged_string)({ char* _temp2405=( char*)"__shared__"; struct _tagged_string
_temp2406; _temp2406.curr= _temp2405; _temp2406.base= _temp2405; _temp2406.last_plus_one=
_temp2405 + 11; _temp2406;})) == 0){ a=( void*) Cyc_Absyn_Shared_att;} else{ if(
Cyc_String_zstrcmp( _temp2373,( struct _tagged_string)({ char* _temp2407=( char*)"unused";
struct _tagged_string _temp2408; _temp2408.curr= _temp2407; _temp2408.base=
_temp2407; _temp2408.last_plus_one= _temp2407 + 7; _temp2408;})) == 0? 1: Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2409=( char*)"__unused__";
struct _tagged_string _temp2410; _temp2410.curr= _temp2409; _temp2410.base=
_temp2409; _temp2410.last_plus_one= _temp2409 + 11; _temp2410;})) == 0){ a=(
void*) Cyc_Absyn_Unused_att;} else{ if( Cyc_String_zstrcmp( _temp2373,( struct
_tagged_string)({ char* _temp2411=( char*)"weak"; struct _tagged_string
_temp2412; _temp2412.curr= _temp2411; _temp2412.base= _temp2411; _temp2412.last_plus_one=
_temp2411 + 5; _temp2412;})) == 0? 1: Cyc_String_zstrcmp( _temp2373,( struct
_tagged_string)({ char* _temp2413=( char*)"__weak__"; struct _tagged_string
_temp2414; _temp2414.curr= _temp2413; _temp2414.base= _temp2413; _temp2414.last_plus_one=
_temp2413 + 9; _temp2414;})) == 0){ a=( void*) Cyc_Absyn_Weak_att;} else{ if(
Cyc_String_zstrcmp( _temp2373,( struct _tagged_string)({ char* _temp2415=( char*)"dllimport";
struct _tagged_string _temp2416; _temp2416.curr= _temp2415; _temp2416.base=
_temp2415; _temp2416.last_plus_one= _temp2415 + 10; _temp2416;})) == 0? 1: Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2417=( char*)"__dllimport__";
struct _tagged_string _temp2418; _temp2418.curr= _temp2417; _temp2418.base=
_temp2417; _temp2418.last_plus_one= _temp2417 + 14; _temp2418;})) == 0){ a=(
void*) Cyc_Absyn_Dllimport_att;} else{ if( Cyc_String_zstrcmp( _temp2373,(
struct _tagged_string)({ char* _temp2419=( char*)"dllexport"; struct
_tagged_string _temp2420; _temp2420.curr= _temp2419; _temp2420.base= _temp2419;
_temp2420.last_plus_one= _temp2419 + 10; _temp2420;})) == 0? 1: Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2421=( char*)"__dllexport__";
struct _tagged_string _temp2422; _temp2422.curr= _temp2421; _temp2422.base=
_temp2421; _temp2422.last_plus_one= _temp2421 + 14; _temp2422;})) == 0){ a=(
void*) Cyc_Absyn_Dllexport_att;} else{ if( Cyc_String_zstrcmp( _temp2373,(
struct _tagged_string)({ char* _temp2423=( char*)"no_instrument_function";
struct _tagged_string _temp2424; _temp2424.curr= _temp2423; _temp2424.base=
_temp2423; _temp2424.last_plus_one= _temp2423 + 23; _temp2424;})) == 0? 1: Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2425=( char*)"__no_instrument_function__";
struct _tagged_string _temp2426; _temp2426.curr= _temp2425; _temp2426.base=
_temp2425; _temp2426.last_plus_one= _temp2425 + 27; _temp2426;})) == 0){ a=(
void*) Cyc_Absyn_No_instrument_function_att;} else{ if( Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2427=( char*)"constructor";
struct _tagged_string _temp2428; _temp2428.curr= _temp2427; _temp2428.base=
_temp2427; _temp2428.last_plus_one= _temp2427 + 12; _temp2428;})) == 0? 1: Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2429=( char*)"__constructor__";
struct _tagged_string _temp2430; _temp2430.curr= _temp2429; _temp2430.base=
_temp2429; _temp2430.last_plus_one= _temp2429 + 16; _temp2430;})) == 0){ a=(
void*) Cyc_Absyn_Constructor_att;} else{ if( Cyc_String_zstrcmp( _temp2373,(
struct _tagged_string)({ char* _temp2431=( char*)"destructor"; struct
_tagged_string _temp2432; _temp2432.curr= _temp2431; _temp2432.base= _temp2431;
_temp2432.last_plus_one= _temp2431 + 11; _temp2432;})) == 0? 1: Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2433=( char*)"__destructor__";
struct _tagged_string _temp2434; _temp2434.curr= _temp2433; _temp2434.base=
_temp2433; _temp2434.last_plus_one= _temp2433 + 15; _temp2434;})) == 0){ a=(
void*) Cyc_Absyn_Destructor_att;} else{ if( Cyc_String_zstrcmp( _temp2373,(
struct _tagged_string)({ char* _temp2435=( char*)"no_check_memory_usage"; struct
_tagged_string _temp2436; _temp2436.curr= _temp2435; _temp2436.base= _temp2435;
_temp2436.last_plus_one= _temp2435 + 22; _temp2436;})) == 0? 1: Cyc_String_zstrcmp(
_temp2373,( struct _tagged_string)({ char* _temp2437=( char*)"__no_check_memory_usage__";
struct _tagged_string _temp2438; _temp2438.curr= _temp2437; _temp2438.base=
_temp2437; _temp2438.last_plus_one= _temp2437 + 26; _temp2438;})) == 0){ a=(
void*) Cyc_Absyn_No_check_memory_usage_att;} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2439=( char*)"unrecognized attribute"; struct
_tagged_string _temp2440; _temp2440.curr= _temp2439; _temp2440.base= _temp2439;
_temp2440.last_plus_one= _temp2439 + 23; _temp2440;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2441=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2441[ 0]=({ struct Cyc_Attribute_tok_struct _temp2442; _temp2442.tag= Cyc_Attribute_tok;
_temp2442.f1=( void*) a; _temp2442;}); _temp2441;}); break;}} case 47: _LL2372:
yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2444=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2444[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2445; _temp2445.tag= Cyc_Attribute_tok; _temp2445.f1=(
void*)(( void*) Cyc_Absyn_Const_att); _temp2445;}); _temp2444;}); break; case 48:
_LL2443: { struct _tagged_string _temp2447= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); goto _LL2448; _LL2448: { struct
_tuple15 _temp2451; int _temp2452; void* _temp2454; struct _tuple15* _temp2449=
Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2451=*
_temp2449; _LL2455: _temp2454= _temp2451.f1; goto _LL2453; _LL2453: _temp2452=
_temp2451.f2; goto _LL2450; _LL2450: { void* a; if( Cyc_String_zstrcmp(
_temp2447,( struct _tagged_string)({ char* _temp2456=( char*)"regparm"; struct
_tagged_string _temp2457; _temp2457.curr= _temp2456; _temp2457.base= _temp2456;
_temp2457.last_plus_one= _temp2456 + 8; _temp2457;})) == 0? 1: Cyc_String_zstrcmp(
_temp2447,( struct _tagged_string)({ char* _temp2458=( char*)"__regparm__";
struct _tagged_string _temp2459; _temp2459.curr= _temp2458; _temp2459.base=
_temp2458; _temp2459.last_plus_one= _temp2458 + 12; _temp2459;})) == 0){ if(
_temp2452 <= 0? 1: _temp2452 > 3){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp2460=( char*)"regparm requires value between 1 and 3"; struct
_tagged_string _temp2461; _temp2461.curr= _temp2460; _temp2461.base= _temp2460;
_temp2461.last_plus_one= _temp2460 + 39; _temp2461;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2462=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2462[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2463; _temp2463.tag= Cyc_Absyn_Regparm_att;
_temp2463.f1= _temp2452; _temp2463;}); _temp2462;});} else{ if( Cyc_String_zstrcmp(
_temp2447,( struct _tagged_string)({ char* _temp2464=( char*)"aligned"; struct
_tagged_string _temp2465; _temp2465.curr= _temp2464; _temp2465.base= _temp2464;
_temp2465.last_plus_one= _temp2464 + 8; _temp2465;})) == 0? 1: Cyc_String_zstrcmp(
_temp2447,( struct _tagged_string)({ char* _temp2466=( char*)"__aligned__";
struct _tagged_string _temp2467; _temp2467.curr= _temp2466; _temp2467.base=
_temp2466; _temp2467.last_plus_one= _temp2466 + 12; _temp2467;})) == 0){ if(
_temp2452 < 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2468=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2469; _temp2469.curr= _temp2468; _temp2469.base=
_temp2468; _temp2469.last_plus_one= _temp2468 + 39; _temp2469;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}{
unsigned int j=( unsigned int) _temp2452; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2470=(
char*)"aligned requires positive power of two"; struct _tagged_string _temp2471;
_temp2471.curr= _temp2470; _temp2471.base= _temp2470; _temp2471.last_plus_one=
_temp2470 + 39; _temp2471;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));} a=( void*)({
struct Cyc_Absyn_Aligned_att_struct* _temp2472=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2472[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2473; _temp2473.tag= Cyc_Absyn_Aligned_att;
_temp2473.f1= _temp2452; _temp2473;}); _temp2472;});}} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2474=( char*)"unrecognized attribute";
struct _tagged_string _temp2475; _temp2475.curr= _temp2474; _temp2475.base=
_temp2474; _temp2475.last_plus_one= _temp2474 + 23; _temp2475;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2476=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2476[ 0]=({ struct Cyc_Attribute_tok_struct _temp2477; _temp2477.tag= Cyc_Attribute_tok;
_temp2477.f1=( void*) a; _temp2477;}); _temp2476;}); break;}}} case 49: _LL2446: {
struct _tagged_string _temp2479= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); goto _LL2480; _LL2480: { struct
_tagged_string _temp2481= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); goto _LL2482; _LL2482: { void* a; if( Cyc_String_zstrcmp(
_temp2479,( struct _tagged_string)({ char* _temp2483=( char*)"section"; struct
_tagged_string _temp2484; _temp2484.curr= _temp2483; _temp2484.base= _temp2483;
_temp2484.last_plus_one= _temp2483 + 8; _temp2484;})) == 0? 1: Cyc_String_zstrcmp(
_temp2479,( struct _tagged_string)({ char* _temp2485=( char*)"__section__";
struct _tagged_string _temp2486; _temp2486.curr= _temp2485; _temp2486.base=
_temp2485; _temp2486.last_plus_one= _temp2485 + 12; _temp2486;}))){ a=( void*)({
struct Cyc_Absyn_Section_att_struct* _temp2487=( struct Cyc_Absyn_Section_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct)); _temp2487[ 0]=({
struct Cyc_Absyn_Section_att_struct _temp2488; _temp2488.tag= Cyc_Absyn_Section_att;
_temp2488.f1= _temp2481; _temp2488;}); _temp2487;});} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2489=( char*)"unrecognized attribute";
struct _tagged_string _temp2490; _temp2490.curr= _temp2489; _temp2490.base=
_temp2489; _temp2490.last_plus_one= _temp2489 + 23; _temp2490;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2491=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2491[ 0]=({ struct Cyc_Attribute_tok_struct _temp2492; _temp2492.tag= Cyc_Attribute_tok;
_temp2492.f1=( void*) a; _temp2492;}); _temp2491;}); break;}}} case 50: _LL2478:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2494=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2494[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2495; _temp2495.tag= Cyc_TypeSpecifier_tok;
_temp2495.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2495;}); _temp2494;}); break; case 51: _LL2493: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2497=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2497[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2498; _temp2498.tag=
Cyc_TypeSpecifier_tok; _temp2498.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2498;});
_temp2497;}); break; case 52: _LL2496: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2500=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2500[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2501; _temp2501.tag=
Cyc_TypeSpecifier_tok; _temp2501.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2501;}); _temp2500;}); break; case 53: _LL2499:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2503=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2503[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2504; _temp2504.tag= Cyc_TypeSpecifier_tok;
_temp2504.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct* _temp2505=(
struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2505[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2506; _temp2506.tag=
Cyc_Parse_Short_spec; _temp2506.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2506;});
_temp2505;})); _temp2504;}); _temp2503;}); break; case 54: _LL2502: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2508=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2508[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2509; _temp2509.tag= Cyc_TypeSpecifier_tok;
_temp2509.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2509;}); _temp2508;}); break; case 55: _LL2507: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2511=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2511[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2512; _temp2512.tag=
Cyc_TypeSpecifier_tok; _temp2512.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2513=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2513[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2514; _temp2514.tag= Cyc_Parse_Long_spec;
_temp2514.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2514;});
_temp2513;})); _temp2512;}); _temp2511;}); break; case 56: _LL2510: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2516=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2516[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2517; _temp2517.tag= Cyc_TypeSpecifier_tok;
_temp2517.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2517;}); _temp2516;}); break; case 57: _LL2515: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2519=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2519[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2520; _temp2520.tag=
Cyc_TypeSpecifier_tok; _temp2520.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2520;}); _temp2519;}); break; case 58: _LL2518:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2522=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2522[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2523; _temp2523.tag= Cyc_TypeSpecifier_tok;
_temp2523.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct* _temp2524=(
struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2524[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp2525; _temp2525.tag=
Cyc_Parse_Signed_spec; _temp2525.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2525;});
_temp2524;})); _temp2523;}); _temp2522;}); break; case 59: _LL2521: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2527=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2527[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2528; _temp2528.tag= Cyc_TypeSpecifier_tok;
_temp2528.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp2529=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp2529[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp2530; _temp2530.tag= Cyc_Parse_Unsigned_spec; _temp2530.f1= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2530;}); _temp2529;})); _temp2528;}); _temp2527;}); break; case 60: _LL2526:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 61:
_LL2531: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 62:
_LL2532: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 63:
_LL2533: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2535=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2535[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2536; _temp2536.tag=
Cyc_TypeSpecifier_tok; _temp2536.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2536;}); _temp2535;}); break; case 64: _LL2534: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2538=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2538[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2539; _temp2539.tag=
Cyc_TypeSpecifier_tok; _temp2539.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2539;}); _temp2538;}); break; case 65: _LL2537: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2541=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2541[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2542; _temp2542.tag=
Cyc_TypeSpecifier_tok; _temp2542.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp2543=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp2543[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp2544; _temp2544.tag= Cyc_Absyn_TypedefType;
_temp2544.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2544.f2= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2544.f3= 0; _temp2544;}); _temp2543;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2542;}); _temp2541;}); break; case 66: _LL2540: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2546=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2546[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2547; _temp2547.tag=
Cyc_TypeSpecifier_tok; _temp2547.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2548=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2548[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2549; _temp2549.tag= Cyc_Absyn_TupleType;
_temp2549.f1=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); _temp2549;});
_temp2548;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2547;});
_temp2546;}); break; case 67: _LL2545: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2551=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2551[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2552; _temp2552.tag=
Cyc_TypeSpecifier_tok; _temp2552.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2553=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2553[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2554; _temp2554.tag= Cyc_Absyn_RgnHandleType;
_temp2554.f1=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2554;}); _temp2553;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2552;}); _temp2551;}); break; case 68: _LL2550: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp2556=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp2556[ 0]=({ struct Cyc_Kind_tok_struct _temp2557; _temp2557.tag= Cyc_Kind_tok;
_temp2557.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2557;}); _temp2556;}); break; case 69: _LL2555: { struct _tagged_string*
_temp2561; void* _temp2563; struct _tuple1 _temp2559=* Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _LL2564:
_temp2563= _temp2559.f1; goto _LL2562; _LL2562: _temp2561= _temp2559.f2; goto
_LL2560; _LL2560: if( _temp2563 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2565=( char*)"bad kind in type specifier";
struct _tagged_string _temp2566; _temp2566.curr= _temp2565; _temp2566.base=
_temp2565; _temp2566.last_plus_one= _temp2565 + 27; _temp2566;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_Kind_tok_struct* _temp2567=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp2567[ 0]=({ struct Cyc_Kind_tok_struct
_temp2568; _temp2568.tag= Cyc_Kind_tok; _temp2568.f1=( void*) Cyc_Parse_id_to_kind(*
_temp2561, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2568;});
_temp2567;}); break;} case 70: _LL2558: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2570=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2570[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2571; _temp2571.tag= Cyc_TypeQual_tok;
_temp2571.f1=({ struct Cyc_Absyn_Tqual _temp2572; _temp2572.q_const= 1;
_temp2572.q_volatile= 0; _temp2572.q_restrict= 0; _temp2572;}); _temp2571;});
_temp2570;}); break; case 71: _LL2569: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2574=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2574[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2575; _temp2575.tag= Cyc_TypeQual_tok;
_temp2575.f1=({ struct Cyc_Absyn_Tqual _temp2576; _temp2576.q_const= 0;
_temp2576.q_volatile= 1; _temp2576.q_restrict= 0; _temp2576;}); _temp2575;});
_temp2574;}); break; case 72: _LL2573: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2578=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2578[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2579; _temp2579.tag= Cyc_TypeQual_tok;
_temp2579.f1=({ struct Cyc_Absyn_Tqual _temp2580; _temp2580.q_const= 0;
_temp2580.q_volatile= 0; _temp2580.q_restrict= 1; _temp2580;}); _temp2579;});
_temp2578;}); break; case 73: _LL2577: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2582=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2582[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2583; _temp2583.tag=
Cyc_TypeSpecifier_tok; _temp2583.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2584=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2584[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2585; _temp2585.tag= Cyc_Parse_Decl_spec;
_temp2585.f1=({ struct Cyc_Absyn_Decl* _temp2586=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2586->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp2587=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp2587[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp2588; _temp2588.tag= Cyc_Absyn_Enum_d; _temp2588.f1=({ struct Cyc_Absyn_Enumdecl*
_temp2589=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp2589->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2589->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2589->fields=({
struct Cyc_Core_Opt* _temp2590=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2590->v=( void*) Cyc_yyget_EnumfieldList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2590;}); _temp2589;}); _temp2588;});
_temp2587;})); _temp2586->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2586;});
_temp2585;}); _temp2584;})); _temp2583;}); _temp2582;}); break; case 74: _LL2581:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2592=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2592[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2593; _temp2593.tag= Cyc_TypeSpecifier_tok;
_temp2593.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp2594=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2594[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2595; _temp2595.tag= Cyc_Absyn_EnumType;
_temp2595.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2595.f2= 0; _temp2595;}); _temp2594;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2593;}); _temp2592;}); break; case 75: _LL2591: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2597=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2597[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2598; _temp2598.tag= Cyc_Enumfield_tok;
_temp2598.f1=({ struct Cyc_Absyn_Enumfield* _temp2599=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2599->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2599->tag=
0; _temp2599->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2599;});
_temp2598;}); _temp2597;}); break; case 76: _LL2596: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2601=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2601[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2602; _temp2602.tag= Cyc_Enumfield_tok;
_temp2602.f1=({ struct Cyc_Absyn_Enumfield* _temp2603=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2603->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2603->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2603->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2603;});
_temp2602;}); _temp2601;}); break; case 77: _LL2600: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp2605=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2605[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2606; _temp2606.tag=
Cyc_EnumfieldList_tok; _temp2606.f1=({ struct Cyc_List_List* _temp2607=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2607->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2607->tl= 0; _temp2607;}); _temp2606;}); _temp2605;}); break; case 78:
_LL2604: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct* _temp2609=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2609[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2610; _temp2610.tag=
Cyc_EnumfieldList_tok; _temp2610.f1=({ struct Cyc_List_List* _temp2611=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2611->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2611->tl= Cyc_yyget_EnumfieldList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2611;}); _temp2610;}); _temp2609;}); break; case 79:
_LL2608: { void* t;{ void* _temp2613= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _LL2615: if( _temp2613 ==( void*) Cyc_Parse_Struct_su){
goto _LL2616;} else{ goto _LL2617;} _LL2617: if( _temp2613 ==( void*) Cyc_Parse_Union_su){
goto _LL2618;} else{ goto _LL2614;} _LL2616: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2619=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2619[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2620; _temp2620.tag= Cyc_Absyn_AnonStructType; _temp2620.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2620;});
_temp2619;}); goto _LL2614; _LL2618: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp2621=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp2621[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp2622; _temp2622.tag= Cyc_Absyn_AnonUnionType; _temp2622.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2622;});
_temp2621;}); goto _LL2614; _LL2614:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2623=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2623[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2624; _temp2624.tag=
Cyc_TypeSpecifier_tok; _temp2624.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp2625=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp2625[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp2626; _temp2626.tag= Cyc_Parse_Type_spec;
_temp2626.f1=( void*) t; _temp2626.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2626;}); _temp2625;})); _temp2624;}); _temp2623;}); break;} case 80:
_LL2612: { struct Cyc_List_List* _temp2628=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2629; _LL2629: { struct Cyc_Absyn_Decl* d;{ void* _temp2630= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2632:
if( _temp2630 ==( void*) Cyc_Parse_Struct_su){ goto _LL2633;} else{ goto _LL2634;}
_LL2634: if( _temp2630 ==( void*) Cyc_Parse_Union_su){ goto _LL2635;} else{ goto
_LL2631;} _LL2633: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2636=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2636->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2636;}), _temp2628,({ struct Cyc_Core_Opt* _temp2637=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2637->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2637;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2631;
_LL2635: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2638=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2638->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2638;}), _temp2628,({ struct Cyc_Core_Opt* _temp2639=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2639->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2639;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2631;
_LL2631:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2640=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2640[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2641; _temp2641.tag=
Cyc_TypeSpecifier_tok; _temp2641.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2642=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2642[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2643; _temp2643.tag= Cyc_Parse_Decl_spec;
_temp2643.f1= d; _temp2643;}); _temp2642;})); _temp2641;}); _temp2640;}); break;}}
case 81: _LL2627: { struct Cyc_List_List* _temp2645=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2646; _LL2646: { struct Cyc_Absyn_Decl* d;{ void* _temp2647= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2649:
if( _temp2647 ==( void*) Cyc_Parse_Struct_su){ goto _LL2650;} else{ goto _LL2651;}
_LL2651: if( _temp2647 ==( void*) Cyc_Parse_Union_su){ goto _LL2652;} else{ goto
_LL2648;} _LL2650: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2653=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2653->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2653;}), _temp2645,({ struct Cyc_Core_Opt* _temp2654=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2654->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2654;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2648;
_LL2652: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2655=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2655->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2655;}), _temp2645,({ struct Cyc_Core_Opt* _temp2656=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2656->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2656;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2648;
_LL2648:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2657=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2657[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2658; _temp2658.tag=
Cyc_TypeSpecifier_tok; _temp2658.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2659=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2659[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2660; _temp2660.tag= Cyc_Parse_Decl_spec;
_temp2660.f1= d; _temp2660;}); _temp2659;})); _temp2658;}); _temp2657;}); break;}}
case 82: _LL2644:{ void* _temp2662= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _LL2664: if( _temp2662 ==( void*) Cyc_Parse_Struct_su){
goto _LL2665;} else{ goto _LL2666;} _LL2666: if( _temp2662 ==( void*) Cyc_Parse_Union_su){
goto _LL2667;} else{ goto _LL2663;} _LL2665: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2668=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2668[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2669; _temp2669.tag=
Cyc_TypeSpecifier_tok; _temp2669.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2670=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2670[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2671; _temp2671.tag= Cyc_Absyn_StructType;
_temp2671.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2671.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2671.f3= 0;
_temp2671;}); _temp2670;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2669;});
_temp2668;}); goto _LL2663; _LL2667: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2672=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2672[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2673; _temp2673.tag=
Cyc_TypeSpecifier_tok; _temp2673.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2674=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2674[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2675; _temp2675.tag= Cyc_Absyn_UnionType;
_temp2675.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2675.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2675.f3= 0;
_temp2675;}); _temp2674;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2673;});
_temp2672;}); goto _LL2663; _LL2663:;} break; case 83: _LL2661:{ void* _temp2677=
Cyc_yyget_StructOrUnion_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_LL2679: if( _temp2677 ==( void*) Cyc_Parse_Struct_su){ goto _LL2680;} else{
goto _LL2681;} _LL2681: if( _temp2677 ==( void*) Cyc_Parse_Union_su){ goto
_LL2682;} else{ goto _LL2678;} _LL2680: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2683=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2683[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2684; _temp2684.tag=
Cyc_TypeSpecifier_tok; _temp2684.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2685=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2685[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2686; _temp2686.tag= Cyc_Absyn_StructType;
_temp2686.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2686.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2686.f3= 0;
_temp2686;}); _temp2685;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2684;});
_temp2683;}); goto _LL2678; _LL2682: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2687=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2687[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2688; _temp2688.tag=
Cyc_TypeSpecifier_tok; _temp2688.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2689=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2689[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2690; _temp2690.tag= Cyc_Absyn_UnionType;
_temp2690.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2690.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2690.f3= 0;
_temp2690;}); _temp2689;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2688;});
_temp2687;}); goto _LL2678; _LL2678:;} break; case 84: _LL2676: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2692=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2692[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2693; _temp2693.tag= Cyc_TypeList_tok; _temp2693.f1= 0; _temp2693;});
_temp2692;}); break; case 85: _LL2691: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2695=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2695[ 0]=({ struct Cyc_TypeList_tok_struct _temp2696; _temp2696.tag= Cyc_TypeList_tok;
_temp2696.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2696;}); _temp2695;}); break; case 86: _LL2694:(( struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2698=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2698[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2699; _temp2699.tag= Cyc_TypeList_tok; _temp2699.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2699;}); _temp2698;}); break;
case 87: _LL2697: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2701=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2701[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2702; _temp2702.tag=
Cyc_StructOrUnion_tok; _temp2702.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp2702;}); _temp2701;}); break; case 88: _LL2700: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2704=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2704[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2705; _temp2705.tag=
Cyc_StructOrUnion_tok; _temp2705.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp2705;}); _temp2704;}); break; case 89: _LL2703: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2707=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2707[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2708; _temp2708.tag= Cyc_StructFieldDeclList_tok; _temp2708.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))); _temp2708;});
_temp2707;}); break; case 90: _LL2706: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp2710=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp2710[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp2711; _temp2711.tag= Cyc_StructFieldDeclListList_tok; _temp2711.f1=({
struct Cyc_List_List* _temp2712=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2712->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2712->tl=
0; _temp2712;}); _temp2711;}); _temp2710;}); break; case 91: _LL2709: yyval=(
void*)({ struct Cyc_StructFieldDeclListList_tok_struct* _temp2714=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp2714[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp2715; _temp2715.tag= Cyc_StructFieldDeclListList_tok;
_temp2715.f1=({ struct Cyc_List_List* _temp2716=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2716->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2716->tl=
Cyc_yyget_StructFieldDeclListList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2716;}); _temp2715;}); _temp2714;}); break; case 92:
_LL2713: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2718=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2718[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2719; _temp2719.tag=
Cyc_InitDeclList_tok; _temp2719.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp2719;}); _temp2718;}); break; case 93:
_LL2717: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2721=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2721[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2722; _temp2722.tag=
Cyc_InitDeclList_tok; _temp2722.f1=({ struct Cyc_List_List* _temp2723=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2723->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2723->tl= 0; _temp2723;}); _temp2722;}); _temp2721;}); break; case 94:
_LL2720: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2725=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2725[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2726; _temp2726.tag=
Cyc_InitDeclList_tok; _temp2726.f1=({ struct Cyc_List_List* _temp2727=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2727->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2727->tl= Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2727;}); _temp2726;}); _temp2725;}); break; case 95:
_LL2724: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2729=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2729[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2730; _temp2730.tag= Cyc_InitDecl_tok; _temp2730.f1=({ struct _tuple14*
_temp2731=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2731->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2731->f2= 0; _temp2731;}); _temp2730;}); _temp2729;}); break; case 96:
_LL2728: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2733=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2733[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2734; _temp2734.tag= Cyc_InitDecl_tok; _temp2734.f1=({ struct _tuple14*
_temp2735=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp2735->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2735->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2735;}); _temp2734;}); _temp2733;}); break;
case 97: _LL2732: { struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f1; struct
Cyc_List_List* _temp2737=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f3; goto _LL2738; _LL2738: { void* _temp2739=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
goto _LL2740; _LL2740: { struct Cyc_List_List* _temp2743; struct Cyc_List_List*
_temp2745; struct _tuple0 _temp2741=(( struct _tuple0(*)( struct Cyc_List_List*
x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _LL2746: _temp2745= _temp2741.f1; goto
_LL2744; _LL2744: _temp2743= _temp2741.f2; goto _LL2742; _LL2742: { struct Cyc_List_List*
_temp2747=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, _temp2739, _temp2745, _temp2737),
_temp2743); goto _LL2748; _LL2748: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2749=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2749[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2750; _temp2750.tag= Cyc_StructFieldDeclList_tok; _temp2750.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple8*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
_temp2747); _temp2750;}); _temp2749;}); break;}}}} case 98: _LL2736: yyval=(
void*)({ struct Cyc_QualSpecList_tok_struct* _temp2752=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp2752[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp2753; _temp2753.tag= Cyc_QualSpecList_tok;
_temp2753.f1=({ struct _tuple17* _temp2754=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp2754->f1= Cyc_Absyn_empty_tqual(); _temp2754->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2755=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2755->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2755->tl=
0; _temp2755;}); _temp2754->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp2754;}); _temp2753;}); _temp2752;});
break; case 99: _LL2751: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2757=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2757[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2758; _temp2758.tag=
Cyc_QualSpecList_tok; _temp2758.f1=({ struct _tuple17* _temp2759=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2759->f1=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; _temp2759->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2760=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2760->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2760->tl=(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2;
_temp2760;}); _temp2759->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2759;}); _temp2758;}); _temp2757;});
break; case 100: _LL2756: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2762=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2762[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2763; _temp2763.tag=
Cyc_QualSpecList_tok; _temp2763.f1=({ struct _tuple17* _temp2764=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2764->f1= Cyc_yyget_TypeQual_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2764->f2=
0; _temp2764->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2764;}); _temp2763;}); _temp2762;}); break; case 101:
_LL2761: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp2766=( struct
Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2766[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2767; _temp2767.tag=
Cyc_QualSpecList_tok; _temp2767.f1=({ struct _tuple17* _temp2768=( struct
_tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp2768->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1);
_temp2768->f2=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp2768->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2768;});
_temp2767;}); _temp2766;}); break; case 102: _LL2765: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp2770=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2770[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2771; _temp2771.tag= Cyc_DeclaratorExpoptList_tok;
_temp2771.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))); _temp2771;}); _temp2770;}); break; case 103: _LL2769: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2773=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2773[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2774; _temp2774.tag= Cyc_DeclaratorExpoptList_tok;
_temp2774.f1=({ struct Cyc_List_List* _temp2775=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2775->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2775->tl=
0; _temp2775;}); _temp2774;}); _temp2773;}); break; case 104: _LL2772: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2777=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2777[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2778; _temp2778.tag= Cyc_DeclaratorExpoptList_tok;
_temp2778.f1=({ struct Cyc_List_List* _temp2779=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2779->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2779->tl=
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2779;}); _temp2778;}); _temp2777;}); break; case 105:
_LL2776: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2781=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp2781[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp2782; _temp2782.tag=
Cyc_DeclaratorExpopt_tok; _temp2782.f1=({ struct _tuple12* _temp2783=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp2783->f1= Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2783->f2=
0; _temp2783;}); _temp2782;}); _temp2781;}); break; case 106: _LL2780: yyval=(
void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2785=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2785[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2786; _temp2786.tag= Cyc_DeclaratorExpopt_tok;
_temp2786.f1=({ struct _tuple12* _temp2787=( struct _tuple12*) GC_malloc(
sizeof( struct _tuple12)); _temp2787->f1=({ struct Cyc_Parse_Declarator*
_temp2789=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp2789->id=({ struct _tuple1* _temp2790=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2790->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp2794=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp2794[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp2795; _temp2795.tag= Cyc_Absyn_Rel_n;
_temp2795.f1= 0; _temp2795;}); _temp2794;}); _temp2790->f2=({ struct
_tagged_string* _temp2791=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp2791[ 0]=( struct _tagged_string)({ char* _temp2792=(
char*)""; struct _tagged_string _temp2793; _temp2793.curr= _temp2792; _temp2793.base=
_temp2792; _temp2793.last_plus_one= _temp2792 + 1; _temp2793;}); _temp2791;});
_temp2790;}); _temp2789->tms= 0; _temp2789;}); _temp2787->f2=({ struct Cyc_Core_Opt*
_temp2788=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2788->v=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2788;}); _temp2787;}); _temp2786;}); _temp2785;}); break;
case 107: _LL2784: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp2797=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp2797[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp2798; _temp2798.tag= Cyc_DeclaratorExpopt_tok; _temp2798.f1=({ struct
_tuple12* _temp2799=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp2799->f1= Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2799->f2=({ struct Cyc_Core_Opt* _temp2800=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2800->v=( void*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2800;});
_temp2799;}); _temp2798;}); _temp2797;}); break; case 108: _LL2796: { struct Cyc_List_List*
_temp2802=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL2803; _LL2803: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2804=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2804[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2805; _temp2805.tag=
Cyc_TypeSpecifier_tok; _temp2805.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2806=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2806[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2807; _temp2807.tag= Cyc_Parse_Decl_spec;
_temp2807.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), _temp2802,({
struct Cyc_Core_Opt* _temp2808=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2808->v=( void*) Cyc_yyget_TunionFieldList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2808;}), Cyc_yyget_Bool_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2807;}); _temp2806;})); _temp2805;}); _temp2804;}); break;} case 109:
_LL2801: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2810=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2810[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2811; _temp2811.tag=
Cyc_TypeSpecifier_tok; _temp2811.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2812=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2812[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2813; _temp2813.tag= Cyc_Absyn_TunionType;
_temp2813.f1=({ struct Cyc_Absyn_TunionInfo _temp2814; _temp2814.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2815=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2815[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2816; _temp2816.tag= Cyc_Absyn_UnknownTunion;
_temp2816.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2817; _temp2817.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2817.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2817;}); _temp2816;}); _temp2815;})); _temp2814.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2814.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2814;}); _temp2813;});
_temp2812;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2811;});
_temp2810;}); break; case 110: _LL2809: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2819=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2819[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2820; _temp2820.tag=
Cyc_TypeSpecifier_tok; _temp2820.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2821=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2821[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2822; _temp2822.tag= Cyc_Absyn_TunionType;
_temp2822.f1=({ struct Cyc_Absyn_TunionInfo _temp2823; _temp2823.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2824=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2824[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2825; _temp2825.tag= Cyc_Absyn_UnknownTunion;
_temp2825.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2826; _temp2826.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2826.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp2826;}); _temp2825;}); _temp2824;})); _temp2823.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2823.rgn=( void*) Cyc_yyget_Rgn_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2823;}); _temp2822;}); _temp2821;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2820;}); _temp2819;}); break; case 111: _LL2818: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2828=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2828[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2829; _temp2829.tag= Cyc_TypeSpecifier_tok;
_temp2829.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2830=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2830[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2831; _temp2831.tag= Cyc_Absyn_TunionFieldType; _temp2831.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2832; _temp2832.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2833=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2833[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2834; _temp2834.tag= Cyc_Absyn_UnknownTunionfield; _temp2834.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2835; _temp2835.tunion_name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2835.field_name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2835.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2835;}); _temp2834;}); _temp2833;})); _temp2832.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2832;}); _temp2831;}); _temp2830;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2829;}); _temp2828;}); break; case 112: _LL2827: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2837=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp2837[ 0]=({ struct Cyc_Bool_tok_struct
_temp2838; _temp2838.tag= Cyc_Bool_tok; _temp2838.f1= 0; _temp2838;}); _temp2837;});
break; case 113: _LL2836: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2840=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2840[ 0]=({ struct Cyc_Bool_tok_struct _temp2841; _temp2841.tag= Cyc_Bool_tok;
_temp2841.f1= 1; _temp2841;}); _temp2840;}); break; case 114: _LL2839: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2843=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2843[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2844; _temp2844.tag= Cyc_TunionFieldList_tok;
_temp2844.f1=({ struct Cyc_List_List* _temp2845=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2845->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2845->tl=
0; _temp2845;}); _temp2844;}); _temp2843;}); break; case 115: _LL2842: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2847=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2847[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2848; _temp2848.tag= Cyc_TunionFieldList_tok;
_temp2848.f1=({ struct Cyc_List_List* _temp2849=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2849->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2849->tl=
0; _temp2849;}); _temp2848;}); _temp2847;}); break; case 116: _LL2846: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2851=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2851[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2852; _temp2852.tag= Cyc_TunionFieldList_tok;
_temp2852.f1=({ struct Cyc_List_List* _temp2853=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2853->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2853->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2853;}); _temp2852;}); _temp2851;}); break; case 117: _LL2850: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2855=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2855[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2856; _temp2856.tag= Cyc_TunionFieldList_tok;
_temp2856.f1=({ struct Cyc_List_List* _temp2857=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2857->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2857->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2857;}); _temp2856;}); _temp2855;}); break; case 118: _LL2854: yyval=( void*)({
struct Cyc_Scope_tok_struct* _temp2859=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2859[ 0]=({ struct Cyc_Scope_tok_struct
_temp2860; _temp2860.tag= Cyc_Scope_tok; _temp2860.f1=( void*)(( void*) Cyc_Absyn_Public);
_temp2860;}); _temp2859;}); break; case 119: _LL2858: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2862=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2862[ 0]=({ struct Cyc_Scope_tok_struct
_temp2863; _temp2863.tag= Cyc_Scope_tok; _temp2863.f1=( void*)(( void*) Cyc_Absyn_Extern);
_temp2863;}); _temp2862;}); break; case 120: _LL2861: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2865=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2865[ 0]=({ struct Cyc_Scope_tok_struct
_temp2866; _temp2866.tag= Cyc_Scope_tok; _temp2866.f1=( void*)(( void*) Cyc_Absyn_Static);
_temp2866;}); _temp2865;}); break; case 121: _LL2864: yyval=( void*)({ struct
Cyc_TunionField_tok_struct* _temp2868=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2868[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2869; _temp2869.tag= Cyc_TunionField_tok;
_temp2869.f1=({ struct Cyc_Absyn_Tunionfield* _temp2870=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2870->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2870->tvs=
0; _temp2870->typs= 0; _temp2870->loc= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2870->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2870;}); _temp2869;}); _temp2868;}); break; case 122:
_LL2867: { struct Cyc_List_List* _temp2872=(( struct Cyc_List_List*(*)( struct
_tuple10*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2873; _LL2873: { struct Cyc_List_List* _temp2874=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2875; _LL2875: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2876=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2876[ 0]=({ struct Cyc_TunionField_tok_struct _temp2877; _temp2877.tag= Cyc_TunionField_tok;
_temp2877.f1=({ struct Cyc_Absyn_Tunionfield* _temp2878=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2878->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp2878->tvs=
_temp2874; _temp2878->typs= _temp2872; _temp2878->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2878->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp2878;}); _temp2877;}); _temp2876;}); break;}} case 123:
_LL2871: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 124:
_LL2879: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2881=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2881[ 0]=({ struct Cyc_Declarator_tok_struct _temp2882; _temp2882.tag= Cyc_Declarator_tok;
_temp2882.f1=({ struct Cyc_Parse_Declarator* _temp2883=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2883->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->id; _temp2883->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms);
_temp2883;}); _temp2882;}); _temp2881;}); break; case 125: _LL2880: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2885=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2885[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2886; _temp2886.tag= Cyc_Declarator_tok;
_temp2886.f1=({ struct Cyc_Parse_Declarator* _temp2887=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2887->id= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2887->tms=
0; _temp2887;}); _temp2886;}); _temp2885;}); break; case 126: _LL2884: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1); break; case
127: _LL2888: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2890=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2890[ 0]=({ struct Cyc_Declarator_tok_struct _temp2891; _temp2891.tag= Cyc_Declarator_tok;
_temp2891.f1=({ struct Cyc_Parse_Declarator* _temp2892=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2892->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2892->tms=({ struct Cyc_List_List* _temp2893=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2893->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2893->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)))->tms; _temp2893;}); _temp2892;}); _temp2891;}); _temp2890;});
break; case 128: _LL2889: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2895=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2895[ 0]=({ struct Cyc_Declarator_tok_struct _temp2896; _temp2896.tag= Cyc_Declarator_tok;
_temp2896.f1=({ struct Cyc_Parse_Declarator* _temp2897=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2897->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2897->tms=({ struct Cyc_List_List* _temp2898=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2898->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2899=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2899[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2900; _temp2900.tag= Cyc_Absyn_ConstArray_mod;
_temp2900.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2900;}); _temp2899;})); _temp2898->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp2898;}); _temp2897;});
_temp2896;}); _temp2895;}); break; case 129: _LL2894: { struct _tuple16
_temp2904; struct Cyc_Core_Opt* _temp2905; int _temp2907; struct Cyc_List_List*
_temp2909; struct _tuple16* _temp2902= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2904=* _temp2902; _LL2910:
_temp2909= _temp2904.f1; goto _LL2908; _LL2908: _temp2907= _temp2904.f2; goto
_LL2906; _LL2906: _temp2905= _temp2904.f3; goto _LL2903; _LL2903: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2911=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2911[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2912; _temp2912.tag= Cyc_Declarator_tok;
_temp2912.f1=({ struct Cyc_Parse_Declarator* _temp2913=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2913->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2913->tms=({ struct Cyc_List_List* _temp2914=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2914->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2915=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2915[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2916; _temp2916.tag= Cyc_Absyn_Function_mod;
_temp2916.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2917=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2917[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2918; _temp2918.tag= Cyc_Absyn_WithTypes;
_temp2918.f1= _temp2909; _temp2918.f2= _temp2907; _temp2918.f3= _temp2905;
_temp2918;}); _temp2917;})); _temp2916;}); _temp2915;})); _temp2914->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2914;}); _temp2913;}); _temp2912;}); _temp2911;}); break;} case 130:
_LL2901: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2920=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2920[ 0]=({ struct Cyc_Declarator_tok_struct _temp2921; _temp2921.tag= Cyc_Declarator_tok;
_temp2921.f1=({ struct Cyc_Parse_Declarator* _temp2922=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2922->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2922->tms=({ struct Cyc_List_List* _temp2923=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2923->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2924=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2924[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2925; _temp2925.tag= Cyc_Absyn_Function_mod;
_temp2925.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2926=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2926[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2927; _temp2927.tag= Cyc_Absyn_WithTypes;
_temp2927.f1= 0; _temp2927.f2= 0; _temp2927.f3= 0; _temp2927;}); _temp2926;}));
_temp2925;}); _temp2924;})); _temp2923->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)))->tms; _temp2923;}); _temp2922;});
_temp2921;}); _temp2920;}); break; case 131: _LL2919: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2929=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2929[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2930; _temp2930.tag= Cyc_Declarator_tok;
_temp2930.f1=({ struct Cyc_Parse_Declarator* _temp2931=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2931->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->id;
_temp2931->tms=({ struct Cyc_List_List* _temp2932=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2932->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2933=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2933[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2934; _temp2934.tag= Cyc_Absyn_Function_mod;
_temp2934.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2935=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2935[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2936; _temp2936.tag= Cyc_Absyn_WithTypes;
_temp2936.f1= 0; _temp2936.f2= 0; _temp2936.f3=({ struct Cyc_Core_Opt* _temp2937=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2937->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2938=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2938[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2939; _temp2939.tag= Cyc_Absyn_JoinEff; _temp2939.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2939;}); _temp2938;})); _temp2937;}); _temp2936;}); _temp2935;}));
_temp2934;}); _temp2933;})); _temp2932->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)))->tms; _temp2932;}); _temp2931;});
_temp2930;}); _temp2929;}); break; case 132: _LL2928: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2941=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2941[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2942; _temp2942.tag= Cyc_Declarator_tok;
_temp2942.f1=({ struct Cyc_Parse_Declarator* _temp2943=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2943->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2943->tms=({ struct Cyc_List_List* _temp2944=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2944->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2945=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2945[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2946; _temp2946.tag= Cyc_Absyn_Function_mod;
_temp2946.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2947=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2947[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2948; _temp2948.tag= Cyc_Absyn_NoTypes;
_temp2948.f1= Cyc_yyget_IdList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2948.f2= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2948;});
_temp2947;})); _temp2946;}); _temp2945;})); _temp2944->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2944;}); _temp2943;}); _temp2942;}); _temp2941;}); break; case 133: _LL2940: {
struct Cyc_List_List* _temp2950=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2951; _LL2951: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2952=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2952[ 0]=({ struct Cyc_Declarator_tok_struct _temp2953; _temp2953.tag= Cyc_Declarator_tok;
_temp2953.f1=({ struct Cyc_Parse_Declarator* _temp2954=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2954->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2954->tms=({ struct Cyc_List_List* _temp2955=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2955->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2956=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2956[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2957; _temp2957.tag= Cyc_Absyn_TypeParams_mod;
_temp2957.f1= _temp2950; _temp2957.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2957.f3= 0; _temp2957;}); _temp2956;})); _temp2955->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2955;}); _temp2954;}); _temp2953;}); _temp2952;}); break;} case 134:
_LL2949:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2959=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2960; _LL2960: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2961=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2961[ 0]=({ struct Cyc_Declarator_tok_struct _temp2962; _temp2962.tag= Cyc_Declarator_tok;
_temp2962.f1=({ struct Cyc_Parse_Declarator* _temp2963=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2963->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2963->tms=({ struct Cyc_List_List* _temp2964=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2964->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2965=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2965[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2966; _temp2966.tag= Cyc_Absyn_TypeParams_mod;
_temp2966.f1= _temp2959; _temp2966.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2966.f3= 0; _temp2966;}); _temp2965;})); _temp2964->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2964;}); _temp2963;}); _temp2962;}); _temp2961;}); break;} case 135:
_LL2958: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2968=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2968[ 0]=({ struct Cyc_Declarator_tok_struct _temp2969; _temp2969.tag= Cyc_Declarator_tok;
_temp2969.f1=({ struct Cyc_Parse_Declarator* _temp2970=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2970->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))->id;
_temp2970->tms=({ struct Cyc_List_List* _temp2971=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2971->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2972=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2972[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2973; _temp2973.tag= Cyc_Absyn_Attributes_mod;
_temp2973.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2973.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2973;});
_temp2972;})); _temp2971->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)))->tms; _temp2971;}); _temp2970;}); _temp2969;});
_temp2968;}); break; case 136: _LL2967: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2975=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2975[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2976; _temp2976.tag= Cyc_TypeModifierList_tok; _temp2976.f1=({ struct Cyc_List_List*
_temp2977=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2977->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2978=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2978[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2979; _temp2979.tag=
Cyc_Absyn_Pointer_mod; _temp2979.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2979.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2979.f3=
Cyc_Absyn_empty_tqual(); _temp2979;}); _temp2978;})); _temp2977->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), 0); _temp2977;}); _temp2976;}); _temp2975;});
break; case 137: _LL2974: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2981=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2981[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2982; _temp2982.tag= Cyc_TypeModifierList_tok; _temp2982.f1=({ struct Cyc_List_List*
_temp2983=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2983->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2984=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2984[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2985; _temp2985.tag=
Cyc_Absyn_Pointer_mod; _temp2985.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2985.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2985.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2985;}); _temp2984;})); _temp2983->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0); _temp2983;}); _temp2982;}); _temp2981;}); break; case 138: _LL2980: yyval=(
void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2987=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2987[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2988; _temp2988.tag= Cyc_TypeModifierList_tok;
_temp2988.f1=({ struct Cyc_List_List* _temp2989=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2989->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2990=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2990[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2991; _temp2991.tag= Cyc_Absyn_Pointer_mod;
_temp2991.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp2991.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2991.f3=
Cyc_Absyn_empty_tqual(); _temp2991;}); _temp2990;})); _temp2989->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_TypeModifierList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp2989;}); _temp2988;}); _temp2987;});
break; case 139: _LL2986: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2993=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2993[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2994; _temp2994.tag= Cyc_TypeModifierList_tok; _temp2994.f1=({ struct Cyc_List_List*
_temp2995=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2995->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2996=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2996[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2997; _temp2997.tag=
Cyc_Absyn_Pointer_mod; _temp2997.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)); _temp2997.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2997.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2997;}); _temp2996;})); _temp2995->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp2995;}); _temp2994;}); _temp2993;}); break; case 140: _LL2992: yyval=( void*)({
struct Cyc_Pointer_Sort_tok_struct* _temp2999=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2999[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3000; _temp3000.tag= Cyc_Pointer_Sort_tok;
_temp3000.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp3001=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3001[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3002; _temp3002.tag=
Cyc_Absyn_Nullable_ps; _temp3002.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3002;}); _temp3001;})); _temp3000;}); _temp2999;}); break; case 141:
_LL2998: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3004=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3004[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3005; _temp3005.tag=
Cyc_Pointer_Sort_tok; _temp3005.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3006=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3006[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3007; _temp3007.tag= Cyc_Absyn_NonNullable_ps; _temp3007.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3007;}); _temp3006;})); _temp3005;}); _temp3004;});
break; case 142: _LL3003: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3009=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3009[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3010; _temp3010.tag=
Cyc_Pointer_Sort_tok; _temp3010.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3011=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3011[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp3012; _temp3012.tag=
Cyc_Absyn_Nullable_ps; _temp3012.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3012;}); _temp3011;})); _temp3010;});
_temp3009;}); break; case 143: _LL3008: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp3014=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3014[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3015; _temp3015.tag=
Cyc_Pointer_Sort_tok; _temp3015.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3016=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3016[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3017; _temp3017.tag= Cyc_Absyn_NonNullable_ps; _temp3017.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3017;});
_temp3016;})); _temp3015;}); _temp3014;}); break; case 144: _LL3013: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3019=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3019[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3020; _temp3020.tag= Cyc_Pointer_Sort_tok;
_temp3020.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp3020;});
_temp3019;}); break; case 145: _LL3018: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp3022=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3022[ 0]=({ struct Cyc_Rgn_tok_struct _temp3023; _temp3023.tag= Cyc_Rgn_tok;
_temp3023.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp3023;}); _temp3022;});
break; case 146: _LL3021: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 147: _LL3024: yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp3026=(
struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3026[ 0]=({ struct Cyc_Rgn_tok_struct _temp3027; _temp3027.tag= Cyc_Rgn_tok;
_temp3027.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3027;}); _temp3026;});
break; case 148: _LL3025: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3029=( char*)"expecting region kind\n";
struct _tagged_string _temp3030; _temp3030.curr= _temp3029; _temp3030.base=
_temp3029; _temp3030.last_plus_one= _temp3029 + 23; _temp3030;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp3031=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp3031[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3032; _temp3032.tag= Cyc_Rgn_tok; _temp3032.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp3032;}); _temp3031;}); break; case 149: _LL3028: yyval=( void*)({ struct
Cyc_Rgn_tok_struct* _temp3034=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp3034[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3035; _temp3035.tag= Cyc_Rgn_tok; _temp3035.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp3035;}); _temp3034;}); break; case 150: _LL3033:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 151:
_LL3036: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp3038=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3038[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3039; _temp3039.tag= Cyc_TypeQual_tok; _temp3039.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3039;}); _temp3038;}); break; case 152: _LL3037: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3041=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3041[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3042; _temp3042.tag= Cyc_ParamDeclListBool_tok;
_temp3042.f1=({ struct _tuple16* _temp3043=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp3043->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp3043->f2= 0; _temp3043->f3= 0; _temp3043;});
_temp3042;}); _temp3041;}); break; case 153: _LL3040: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3045=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3045[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3046; _temp3046.tag= Cyc_ParamDeclListBool_tok;
_temp3046.f1=({ struct _tuple16* _temp3047=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp3047->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp3047->f2= 1; _temp3047->f3= 0;
_temp3047;}); _temp3046;}); _temp3045;}); break; case 154: _LL3044: yyval=( void*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp3049=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3049[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3050; _temp3050.tag= Cyc_ParamDeclListBool_tok;
_temp3050.f1=({ struct _tuple16* _temp3051=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp3051->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp3051->f2= 0; _temp3051->f3=( struct
Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp3052=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3052->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp3053=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3053[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3054; _temp3054.tag= Cyc_Absyn_JoinEff; _temp3054.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3054;}); _temp3053;})); _temp3052;}); _temp3051;}); _temp3050;}); _temp3049;});
break; case 155: _LL3048: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 156: _LL3055: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3057=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3057[ 0]=({ struct Cyc_TypeList_tok_struct _temp3058; _temp3058.tag= Cyc_TypeList_tok;
_temp3058.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp3058;}); _temp3057;}); break; case 157:
_LL3056: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3060=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3060[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3061; _temp3061.tag= Cyc_TypeList_tok; _temp3061.f1= 0; _temp3061;});
_temp3060;}); break; case 158: _LL3059: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 159: _LL3062: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3064=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3064[ 0]=({ struct Cyc_TypeList_tok_struct _temp3065; _temp3065.tag= Cyc_TypeList_tok;
_temp3065.f1=({ struct Cyc_List_List* _temp3066=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3066->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp3066->tl= 0; _temp3066;}); _temp3065;}); _temp3064;}); break; case 160:
_LL3063: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp3068=( char*)"expecing effect kind (E)"; struct _tagged_string _temp3069;
_temp3069.curr= _temp3068; _temp3069.base= _temp3068; _temp3069.last_plus_one=
_temp3068 + 25; _temp3069;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp3070=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3070[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3071; _temp3071.tag= Cyc_TypeList_tok; _temp3071.f1=({ struct Cyc_List_List*
_temp3072=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3072->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp3072->tl= 0; _temp3072;});
_temp3071;}); _temp3070;}); break; case 161: _LL3067: yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp3074=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3074[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3075; _temp3075.tag= Cyc_TypeList_tok; _temp3075.f1=({ struct Cyc_List_List*
_temp3076=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3076->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3077=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3077[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3078; _temp3078.tag= Cyc_Absyn_AccessEff;
_temp3078.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3078;}); _temp3077;}));
_temp3076->tl= 0; _temp3076;}); _temp3075;}); _temp3074;}); break; case 162:
_LL3073: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3080=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3080[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3081; _temp3081.tag= Cyc_TypeList_tok; _temp3081.f1=({ struct Cyc_List_List*
_temp3082=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3082->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3083=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3083[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3084; _temp3084.tag= Cyc_Absyn_AccessEff;
_temp3084.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3084;}); _temp3083;}));
_temp3082->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3082;}); _temp3081;}); _temp3080;}); break; case 163:
_LL3079: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp3086=( char*)"expecting region kind (R)"; struct _tagged_string _temp3087;
_temp3087.curr= _temp3086; _temp3087.base= _temp3086; _temp3087.last_plus_one=
_temp3086 + 26; _temp3087;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp3088=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3088[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3089; _temp3089.tag= Cyc_TypeList_tok; _temp3089.f1=({ struct Cyc_List_List*
_temp3090=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3090->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3091=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3091[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3092; _temp3092.tag= Cyc_Absyn_AccessEff;
_temp3092.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3092;}); _temp3091;}));
_temp3090->tl= 0; _temp3090;}); _temp3089;}); _temp3088;}); break; case 164:
_LL3085: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp3094=( char*)"expecting region kind (R)"; struct _tagged_string _temp3095;
_temp3095.curr= _temp3094; _temp3095.base= _temp3094; _temp3095.last_plus_one=
_temp3094 + 26; _temp3095;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));} yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp3096=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3096[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3097; _temp3097.tag= Cyc_TypeList_tok; _temp3097.f1=({ struct Cyc_List_List*
_temp3098=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3098->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3099=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3099[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3100; _temp3100.tag= Cyc_Absyn_AccessEff;
_temp3100.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3100;}); _temp3099;}));
_temp3098->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3098;}); _temp3097;}); _temp3096;}); break; case 165:
_LL3093: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3102=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3102[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3103; _temp3103.tag=
Cyc_ParamDeclList_tok; _temp3103.f1=({ struct Cyc_List_List* _temp3104=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3104->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3104->tl= 0; _temp3104;}); _temp3103;}); _temp3102;}); break; case 166:
_LL3101: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3106=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3106[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3107; _temp3107.tag=
Cyc_ParamDeclList_tok; _temp3107.f1=({ struct Cyc_List_List* _temp3108=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3108->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3108->tl= Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3108;}); _temp3107;}); _temp3106;}); break; case 167:
_LL3105: { void* _temp3110= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3111; _LL3111: { struct Cyc_List_List* _temp3112=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3; goto
_LL3113; _LL3113: { struct Cyc_Absyn_Tqual _temp3114=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f1; goto
_LL3115; _LL3115: { struct Cyc_List_List* _temp3116=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms; goto
_LL3117; _LL3117: { struct _tuple6 _temp3118= Cyc_Parse_apply_tms( _temp3114,
_temp3110, _temp3112, _temp3116); goto _LL3119; _LL3119: if( _temp3118.f3 != 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp3120=( char*)"parameter with bad type params";
struct _tagged_string _temp3121; _temp3121.curr= _temp3120; _temp3121.base=
_temp3120; _temp3121.last_plus_one= _temp3120 + 31; _temp3121;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct _tuple1* _temp3122=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->id; goto _LL3123; _LL3123:{ void* _temp3124=(*
_temp3122).f1; struct Cyc_List_List* _temp3134; struct Cyc_List_List* _temp3136;
_LL3126: if( _temp3124 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3127;} else{ goto
_LL3128;} _LL3128: if(( unsigned int) _temp3124 > 1u?*(( int*) _temp3124) == Cyc_Absyn_Rel_n:
0){ _LL3135: _temp3134=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp3124)->f1; if( _temp3134 == 0){ goto _LL3129;} else{ goto _LL3130;}} else{
goto _LL3130;} _LL3130: if(( unsigned int) _temp3124 > 1u?*(( int*) _temp3124)
== Cyc_Absyn_Abs_n: 0){ _LL3137: _temp3136=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp3124)->f1; if( _temp3136 == 0){ goto _LL3131;} else{ goto _LL3132;}} else{
goto _LL3132;} _LL3132: goto _LL3133; _LL3127: goto _LL3125; _LL3129: goto
_LL3125; _LL3131: goto _LL3125; _LL3133: Cyc_Parse_err(( struct _tagged_string)({
char* _temp3138=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp3139; _temp3139.curr= _temp3138; _temp3139.base=
_temp3138; _temp3139.last_plus_one= _temp3138 + 49; _temp3139;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3125; _LL3125:;}{ struct Cyc_Core_Opt* _temp3141=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp3140=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3140->v=( void*)(* _temp3122).f2; _temp3140;}); goto
_LL3142; _LL3142: if( _temp3118.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp3143=( char*)"extra attributes on parameter, ignoring"; struct
_tagged_string _temp3144; _temp3144.curr= _temp3143; _temp3144.base= _temp3143;
_temp3144.last_plus_one= _temp3143 + 40; _temp3144;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3118.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp3145=( char*)"extra type variables on parameter, ignoring"; struct
_tagged_string _temp3146; _temp3146.curr= _temp3145; _temp3146.base= _temp3145;
_temp3146.last_plus_one= _temp3145 + 44; _temp3146;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3147=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3147[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3148; _temp3148.tag= Cyc_ParamDecl_tok; _temp3148.f1=({
struct _tuple2* _temp3149=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3149->f1= _temp3141; _temp3149->f2= _temp3118.f1; _temp3149->f3= _temp3118.f2;
_temp3149;}); _temp3148;}); _temp3147;}); break;}}}}}}} case 168: _LL3109: {
void* _temp3151= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3152; _LL3152: { struct Cyc_List_List* _temp3153=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3154; _LL3154: { struct Cyc_Absyn_Tqual _temp3155=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; goto
_LL3156; _LL3156: if( _temp3153 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp3157=( char*)"bad attributes on parameter, ignoring"; struct
_tagged_string _temp3158; _temp3158.curr= _temp3157; _temp3158.base= _temp3157;
_temp3158.last_plus_one= _temp3157 + 38; _temp3158;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3159=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3159[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3160; _temp3160.tag= Cyc_ParamDecl_tok; _temp3160.f1=({
struct _tuple2* _temp3161=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3161->f1= 0; _temp3161->f2= _temp3155; _temp3161->f3= _temp3151; _temp3161;});
_temp3160;}); _temp3159;}); break;}}} case 169: _LL3150: { void* _temp3163= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3164; _LL3164: { struct Cyc_List_List*
_temp3165=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3166; _LL3166: { struct Cyc_Absyn_Tqual
_temp3167=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3168; _LL3168: { struct Cyc_List_List* _temp3169=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3170; _LL3170: { struct _tuple6 _temp3171= Cyc_Parse_apply_tms(
_temp3167, _temp3163, _temp3165, _temp3169); goto _LL3172; _LL3172: if(
_temp3171.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3173=(
char*)"bad type parameters on formal argument, ignoring"; struct _tagged_string
_temp3174; _temp3174.curr= _temp3173; _temp3174.base= _temp3173; _temp3174.last_plus_one=
_temp3173 + 49; _temp3174;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} if( _temp3171.f4
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3175=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp3176; _temp3176.curr= _temp3175; _temp3176.base=
_temp3175; _temp3176.last_plus_one= _temp3175 + 38; _temp3176;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3177=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3177[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3178; _temp3178.tag= Cyc_ParamDecl_tok; _temp3178.f1=({
struct _tuple2* _temp3179=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3179->f1= 0; _temp3179->f2= _temp3171.f1; _temp3179->f3= _temp3171.f2;
_temp3179;}); _temp3178;}); _temp3177;}); break;}}}}} case 170: _LL3162: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp3181=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp3181[ 0]=({ struct Cyc_IdList_tok_struct
_temp3182; _temp3182.tag= Cyc_IdList_tok; _temp3182.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp3182;}); _temp3181;}); break; case
171: _LL3180: yyval=( void*)({ struct Cyc_IdList_tok_struct* _temp3184=( struct
Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp3184[ 0]=({ struct Cyc_IdList_tok_struct _temp3185; _temp3185.tag= Cyc_IdList_tok;
_temp3185.f1=({ struct Cyc_List_List* _temp3186=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3186->hd=( void*)({ struct
_tagged_string* _temp3187=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp3187[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3187;}); _temp3186->tl= 0; _temp3186;});
_temp3185;}); _temp3184;}); break; case 172: _LL3183: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp3189=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp3189[ 0]=({ struct Cyc_IdList_tok_struct
_temp3190; _temp3190.tag= Cyc_IdList_tok; _temp3190.f1=({ struct Cyc_List_List*
_temp3191=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3191->hd=( void*)({ struct _tagged_string* _temp3192=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3192[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3192;}); _temp3191->tl= Cyc_yyget_IdList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3191;}); _temp3190;}); _temp3189;});
break; case 173: _LL3188: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 174: _LL3193: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 175: _LL3194: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3196=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3196[ 0]=({ struct Cyc_Exp_tok_struct _temp3197; _temp3197.tag= Cyc_Exp_tok;
_temp3197.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3198=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3198[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3199; _temp3199.tag= Cyc_Absyn_UnresolvedMem_e; _temp3199.f1= 0; _temp3199.f2=
0; _temp3199;}); _temp3198;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3197;});
_temp3196;}); break; case 176: _LL3195: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3201=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3201[ 0]=({ struct Cyc_Exp_tok_struct _temp3202; _temp3202.tag= Cyc_Exp_tok;
_temp3202.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3203=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3203[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3204; _temp3204.tag= Cyc_Absyn_UnresolvedMem_e; _temp3204.f1= 0; _temp3204.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp3204;});
_temp3203;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3202;});
_temp3201;}); break; case 177: _LL3200: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3206=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3206[ 0]=({ struct Cyc_Exp_tok_struct _temp3207; _temp3207.tag= Cyc_Exp_tok;
_temp3207.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3208=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3208[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3209; _temp3209.tag= Cyc_Absyn_UnresolvedMem_e; _temp3209.f1= 0; _temp3209.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp3209;});
_temp3208;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3207;});
_temp3206;}); break; case 178: _LL3205: { struct Cyc_Absyn_Vardecl* _temp3213=
Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3211=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp3211->f1=( void*) Cyc_Absyn_Loc_n; _temp3211->f2=({
struct _tagged_string* _temp3212=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3212[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3212;}); _temp3211;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).last_line)));
goto _LL3214; _LL3214: _temp3213->tq=({ struct Cyc_Absyn_Tqual _temp3215;
_temp3215.q_const= 1; _temp3215.q_volatile= 0; _temp3215.q_restrict= 1;
_temp3215;}); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3216=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3216[ 0]=({ struct Cyc_Exp_tok_struct
_temp3217; _temp3217.tag= Cyc_Exp_tok; _temp3217.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp3218=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp3218[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp3219; _temp3219.tag= Cyc_Absyn_Comprehension_e;
_temp3219.f1= _temp3213; _temp3219.f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3219.f3= Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3219;}); _temp3218;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3217;}); _temp3216;}); break;} case 179: _LL3210: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3221=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3221[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3222; _temp3222.tag= Cyc_InitializerList_tok;
_temp3222.f1=({ struct Cyc_List_List* _temp3223=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3223->hd=( void*)({ struct
_tuple18* _temp3224=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp3224->f1= 0; _temp3224->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3224;}); _temp3223->tl= 0; _temp3223;});
_temp3222;}); _temp3221;}); break; case 180: _LL3220: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3226=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3226[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3227; _temp3227.tag= Cyc_InitializerList_tok;
_temp3227.f1=({ struct Cyc_List_List* _temp3228=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3228->hd=( void*)({ struct
_tuple18* _temp3229=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp3229->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3229->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3229;}); _temp3228->tl= 0; _temp3228;});
_temp3227;}); _temp3226;}); break; case 181: _LL3225: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3231=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3231[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3232; _temp3232.tag= Cyc_InitializerList_tok;
_temp3232.f1=({ struct Cyc_List_List* _temp3233=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3233->hd=( void*)({ struct
_tuple18* _temp3234=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp3234->f1= 0; _temp3234->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3234;}); _temp3233->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3233;});
_temp3232;}); _temp3231;}); break; case 182: _LL3230: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3236=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3236[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3237; _temp3237.tag= Cyc_InitializerList_tok;
_temp3237.f1=({ struct Cyc_List_List* _temp3238=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3238->hd=( void*)({ struct
_tuple18* _temp3239=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp3239->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3239->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3239;}); _temp3238->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3238;});
_temp3237;}); _temp3236;}); break; case 183: _LL3235: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3241=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3241[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3242; _temp3242.tag= Cyc_DesignatorList_tok;
_temp3242.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp3242;}); _temp3241;}); break; case 184: _LL3240: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3244=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3244[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3245; _temp3245.tag= Cyc_DesignatorList_tok;
_temp3245.f1=({ struct Cyc_List_List* _temp3246=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3246->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3246->tl=
0; _temp3246;}); _temp3245;}); _temp3244;}); break; case 185: _LL3243: yyval=(
void*)({ struct Cyc_DesignatorList_tok_struct* _temp3248=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3248[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3249; _temp3249.tag= Cyc_DesignatorList_tok;
_temp3249.f1=({ struct Cyc_List_List* _temp3250=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3250->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3250->tl=
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3250;}); _temp3249;}); _temp3248;}); break; case 186: _LL3247: yyval=( void*)({
struct Cyc_Designator_tok_struct* _temp3252=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp3252[ 0]=({ struct
Cyc_Designator_tok_struct _temp3253; _temp3253.tag= Cyc_Designator_tok;
_temp3253.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp3254=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp3254[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp3255; _temp3255.tag=
Cyc_Absyn_ArrayElement; _temp3255.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3255;}); _temp3254;})); _temp3253;});
_temp3252;}); break; case 187: _LL3251: yyval=( void*)({ struct Cyc_Designator_tok_struct*
_temp3257=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp3257[ 0]=({ struct Cyc_Designator_tok_struct _temp3258; _temp3258.tag= Cyc_Designator_tok;
_temp3258.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3259=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3259[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3260; _temp3260.tag= Cyc_Absyn_FieldName;
_temp3260.f1=({ struct _tagged_string* _temp3261=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3261[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3261;});
_temp3260;}); _temp3259;})); _temp3258;}); _temp3257;}); break; case 188:
_LL3256: { void* _temp3263= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3264; _LL3264: { struct Cyc_List_List* _temp3265=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3266; _LL3266: if( _temp3265 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp3267=( char*)"ignoring attributes in type"; struct _tagged_string
_temp3268; _temp3268.curr= _temp3267; _temp3268.base= _temp3267; _temp3268.last_plus_one=
_temp3267 + 28; _temp3268;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{ struct Cyc_Absyn_Tqual
_temp3269=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f1; goto _LL3270; _LL3270: yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp3271=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp3271[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp3272; _temp3272.tag= Cyc_ParamDecl_tok;
_temp3272.f1=({ struct _tuple2* _temp3273=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp3273[ 0]=({ struct _tuple2 _temp3274; _temp3274.f1= 0;
_temp3274.f2= _temp3269; _temp3274.f3= _temp3263; _temp3274;}); _temp3273;});
_temp3272;}); _temp3271;}); break;}}} case 189: _LL3262: { void* _temp3276= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3277; _LL3277: { struct Cyc_List_List*
_temp3278=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3279; _LL3279: { struct Cyc_Absyn_Tqual
_temp3280=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3281; _LL3281: { struct Cyc_List_List* _temp3282=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3283; _LL3283: { struct _tuple6 _temp3284= Cyc_Parse_apply_tms(
_temp3280, _temp3276, _temp3278, _temp3282); goto _LL3285; _LL3285: if(
_temp3284.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3286=(
char*)"bad type params, ignoring"; struct _tagged_string _temp3287; _temp3287.curr=
_temp3286; _temp3287.base= _temp3286; _temp3287.last_plus_one= _temp3286 + 26;
_temp3287;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} if( _temp3284.f4
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3288=( char*)"bad specifiers, ignoring";
struct _tagged_string _temp3289; _temp3289.curr= _temp3288; _temp3289.base=
_temp3288; _temp3289.last_plus_one= _temp3288 + 25; _temp3289;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3290=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3290[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3291; _temp3291.tag= Cyc_ParamDecl_tok; _temp3291.f1=({
struct _tuple2* _temp3292=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3292->f1= 0; _temp3292->f2= _temp3284.f1; _temp3292->f3= _temp3284.f2;
_temp3292;}); _temp3291;}); _temp3290;}); break;}}}}} case 190: _LL3275: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp3294=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp3294[ 0]=({ struct Cyc_Type_tok_struct
_temp3295; _temp3295.tag= Cyc_Type_tok; _temp3295.f1=( void*)(* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; _temp3295;});
_temp3294;}); break; case 191: _LL3293: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp3297=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3297[ 0]=({ struct Cyc_Type_tok_struct _temp3298; _temp3298.tag= Cyc_Type_tok;
_temp3298.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3299=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3299[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3300; _temp3300.tag= Cyc_Absyn_JoinEff;
_temp3300.f1= 0; _temp3300;}); _temp3299;})); _temp3298;}); _temp3297;}); break;
case 192: _LL3296: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3302=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3302[ 0]=({ struct Cyc_Type_tok_struct _temp3303; _temp3303.tag= Cyc_Type_tok;
_temp3303.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3304=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3304[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3305; _temp3305.tag= Cyc_Absyn_JoinEff;
_temp3305.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3305;}); _temp3304;})); _temp3303;}); _temp3302;});
break; case 193: _LL3301: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3307=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3307[ 0]=({ struct Cyc_Type_tok_struct _temp3308; _temp3308.tag= Cyc_Type_tok;
_temp3308.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3309=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3309[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3310; _temp3310.tag= Cyc_Absyn_JoinEff;
_temp3310.f1=({ struct Cyc_List_List* _temp3311=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3311->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3311->tl=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3311;}); _temp3310;}); _temp3309;})); _temp3308;}); _temp3307;}); break;
case 194: _LL3306: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3313=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3313[ 0]=({ struct Cyc_TypeList_tok_struct _temp3314; _temp3314.tag= Cyc_TypeList_tok;
_temp3314.f1=({ struct Cyc_List_List* _temp3315=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3315->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3315->tl=
0; _temp3315;}); _temp3314;}); _temp3313;}); break; case 195: _LL3312: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp3317=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3317[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3318; _temp3318.tag= Cyc_TypeList_tok; _temp3318.f1=({ struct Cyc_List_List*
_temp3319=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3319->hd=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3319->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3319;}); _temp3318;}); _temp3317;});
break; case 196: _LL3316: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3321=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3321[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3322; _temp3322.tag= Cyc_AbstractDeclarator_tok; _temp3322.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3323=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3323->tms= Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3323;}); _temp3322;}); _temp3321;}); break;
case 197: _LL3320: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 198: _LL3324: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3326=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3326[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3327; _temp3327.tag= Cyc_AbstractDeclarator_tok; _temp3327.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3328=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3328->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tms); _temp3328;}); _temp3327;});
_temp3326;}); break; case 199: _LL3325: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 200: _LL3329: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3331=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3331[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3332; _temp3332.tag= Cyc_AbstractDeclarator_tok; _temp3332.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3333=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3333->tms=({ struct Cyc_List_List* _temp3334=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3334->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3334->tl= 0; _temp3334;}); _temp3333;}); _temp3332;}); _temp3331;}); break;
case 201: _LL3330: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3336=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3336[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3337; _temp3337.tag= Cyc_AbstractDeclarator_tok; _temp3337.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3338=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3338->tms=({ struct Cyc_List_List* _temp3339=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3339->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3339->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)))->tms; _temp3339;}); _temp3338;}); _temp3337;});
_temp3336;}); break; case 202: _LL3335: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3341=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3341[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3342; _temp3342.tag= Cyc_AbstractDeclarator_tok; _temp3342.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3343=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3343->tms=({ struct Cyc_List_List* _temp3344=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3344->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3345=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3345[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3346; _temp3346.tag= Cyc_Absyn_ConstArray_mod;
_temp3346.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3346;}); _temp3345;})); _temp3344->tl= 0; _temp3344;}); _temp3343;});
_temp3342;}); _temp3341;}); break; case 203: _LL3340: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3348=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3348[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3349; _temp3349.tag= Cyc_AbstractDeclarator_tok;
_temp3349.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3350=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3350->tms=({
struct Cyc_List_List* _temp3351=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3351->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3352=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3352[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp3353; _temp3353.tag= Cyc_Absyn_ConstArray_mod; _temp3353.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3353;});
_temp3352;})); _temp3351->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp3351;}); _temp3350;});
_temp3349;}); _temp3348;}); break; case 204: _LL3347: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3355=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3355[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3356; _temp3356.tag= Cyc_AbstractDeclarator_tok;
_temp3356.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3357=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3357->tms=({
struct Cyc_List_List* _temp3358=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3358->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3359=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3359[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3360; _temp3360.tag=
Cyc_Absyn_Function_mod; _temp3360.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3361=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3361[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3362; _temp3362.tag= Cyc_Absyn_WithTypes;
_temp3362.f1= 0; _temp3362.f2= 0; _temp3362.f3= 0; _temp3362;}); _temp3361;}));
_temp3360;}); _temp3359;})); _temp3358->tl= 0; _temp3358;}); _temp3357;});
_temp3356;}); _temp3355;}); break; case 205: _LL3354: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3364=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3364[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3365; _temp3365.tag= Cyc_AbstractDeclarator_tok;
_temp3365.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3366=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3366->tms=({
struct Cyc_List_List* _temp3367=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3367->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3368=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3368[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3369; _temp3369.tag=
Cyc_Absyn_Function_mod; _temp3369.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3370=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3370[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3371; _temp3371.tag= Cyc_Absyn_WithTypes;
_temp3371.f1= 0; _temp3371.f2= 0; _temp3371.f3=({ struct Cyc_Core_Opt* _temp3372=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3372->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3373=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3373[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3374; _temp3374.tag= Cyc_Absyn_JoinEff; _temp3374.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3374;}); _temp3373;})); _temp3372;}); _temp3371;}); _temp3370;}));
_temp3369;}); _temp3368;})); _temp3367->tl= 0; _temp3367;}); _temp3366;});
_temp3365;}); _temp3364;}); break; case 206: _LL3363: { struct _tuple16
_temp3378; struct Cyc_Core_Opt* _temp3379; int _temp3381; struct Cyc_List_List*
_temp3383; struct _tuple16* _temp3376= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3378=* _temp3376; _LL3384:
_temp3383= _temp3378.f1; goto _LL3382; _LL3382: _temp3381= _temp3378.f2; goto
_LL3380; _LL3380: _temp3379= _temp3378.f3; goto _LL3377; _LL3377: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3385=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3385[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3386; _temp3386.tag= Cyc_AbstractDeclarator_tok;
_temp3386.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3387=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3387->tms=({
struct Cyc_List_List* _temp3388=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3388->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3389=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3389[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3390; _temp3390.tag=
Cyc_Absyn_Function_mod; _temp3390.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3391=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3391[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3392; _temp3392.tag= Cyc_Absyn_WithTypes;
_temp3392.f1= _temp3383; _temp3392.f2= _temp3381; _temp3392.f3= _temp3379;
_temp3392;}); _temp3391;})); _temp3390;}); _temp3389;})); _temp3388->tl= 0;
_temp3388;}); _temp3387;}); _temp3386;}); _temp3385;}); break;} case 207:
_LL3375: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3394=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3394[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3395; _temp3395.tag=
Cyc_AbstractDeclarator_tok; _temp3395.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3396=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3396->tms=({ struct Cyc_List_List* _temp3397=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3397->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3398=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3398[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3399; _temp3399.tag= Cyc_Absyn_Function_mod;
_temp3399.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3400=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3400[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3401; _temp3401.tag= Cyc_Absyn_WithTypes;
_temp3401.f1= 0; _temp3401.f2= 0; _temp3401.f3= 0; _temp3401;}); _temp3400;}));
_temp3399;}); _temp3398;})); _temp3397->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->tms;
_temp3397;}); _temp3396;}); _temp3395;}); _temp3394;}); break; case 208: _LL3393: {
struct Cyc_Core_Opt* _temp3406=({ struct Cyc_Core_Opt* _temp3403=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3403->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp3404=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3404[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3405; _temp3405.tag= Cyc_Absyn_JoinEff; _temp3405.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3405;}); _temp3404;})); _temp3403;}); goto _LL3407; _LL3407: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3408=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3408[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3409; _temp3409.tag= Cyc_AbstractDeclarator_tok;
_temp3409.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3410=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3410->tms=({
struct Cyc_List_List* _temp3411=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3411->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3412=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3412[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3413; _temp3413.tag=
Cyc_Absyn_Function_mod; _temp3413.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3414=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3414[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3415; _temp3415.tag= Cyc_Absyn_WithTypes;
_temp3415.f1= 0; _temp3415.f2= 0; _temp3415.f3= _temp3406; _temp3415;});
_temp3414;})); _temp3413;}); _temp3412;})); _temp3411->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->tms;
_temp3411;}); _temp3410;}); _temp3409;}); _temp3408;}); break;} case 209:
_LL3402: { struct _tuple16 _temp3419; struct Cyc_Core_Opt* _temp3420; int
_temp3422; struct Cyc_List_List* _temp3424; struct _tuple16* _temp3417= Cyc_yyget_ParamDeclListBool_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3419=*
_temp3417; _LL3425: _temp3424= _temp3419.f1; goto _LL3423; _LL3423: _temp3422=
_temp3419.f2; goto _LL3421; _LL3421: _temp3420= _temp3419.f3; goto _LL3418;
_LL3418: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3426=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3426[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3427; _temp3427.tag=
Cyc_AbstractDeclarator_tok; _temp3427.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3428=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3428->tms=({ struct Cyc_List_List* _temp3429=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3429->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3430=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3430[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3431; _temp3431.tag= Cyc_Absyn_Function_mod;
_temp3431.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3432=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3432[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3433; _temp3433.tag= Cyc_Absyn_WithTypes;
_temp3433.f1= _temp3424; _temp3433.f2= _temp3422; _temp3433.f3= _temp3420;
_temp3433;}); _temp3432;})); _temp3431;}); _temp3430;})); _temp3429->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3429;}); _temp3428;}); _temp3427;}); _temp3426;}); break;} case 210:
_LL3416: { struct Cyc_List_List* _temp3435=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL3436; _LL3436: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3437=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3437[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3438; _temp3438.tag= Cyc_AbstractDeclarator_tok; _temp3438.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3439=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3439->tms=({ struct Cyc_List_List* _temp3440=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3440->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3441=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3441[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3442; _temp3442.tag= Cyc_Absyn_TypeParams_mod;
_temp3442.f1= _temp3435; _temp3442.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3442.f3= 0; _temp3442;}); _temp3441;})); _temp3440->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3440;}); _temp3439;}); _temp3438;}); _temp3437;}); break;} case 211:
_LL3434:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp3444=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL3445; _LL3445: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3446=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3446[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3447; _temp3447.tag= Cyc_AbstractDeclarator_tok; _temp3447.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3448=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3448->tms=({ struct Cyc_List_List* _temp3449=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3449->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3450=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3450[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3451; _temp3451.tag= Cyc_Absyn_TypeParams_mod;
_temp3451.f1= _temp3444; _temp3451.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3451.f3= 0; _temp3451;}); _temp3450;})); _temp3449->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3449;}); _temp3448;}); _temp3447;}); _temp3446;}); break;} case 212:
_LL3443: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3453=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3453[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3454; _temp3454.tag=
Cyc_AbstractDeclarator_tok; _temp3454.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3455=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3455->tms=({ struct Cyc_List_List* _temp3456=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3456->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3457=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3457[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3458; _temp3458.tag= Cyc_Absyn_Attributes_mod;
_temp3458.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3458.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3458;});
_temp3457;})); _temp3456->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)))->tms; _temp3456;}); _temp3455;});
_temp3454;}); _temp3453;}); break; case 213: _LL3452: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 214: _LL3459: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 215: _LL3460: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 216: _LL3461: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 217: _LL3462: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 218: _LL3463: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 219: _LL3464: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),(
struct _tagged_string)({ char* _temp3466=( char*)"`H"; struct _tagged_string
_temp3467; _temp3467.curr= _temp3466; _temp3467.base= _temp3466; _temp3467.last_plus_one=
_temp3466 + 3; _temp3467;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp3468=( char*)"bad occurrence of heap region `H"; struct
_tagged_string _temp3469; _temp3469.curr= _temp3468; _temp3469.base= _temp3468;
_temp3469.last_plus_one= _temp3468 + 33; _temp3469;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp3480=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3480->name=({ struct
_tagged_string* _temp3481=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3481[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3481;}); _temp3480->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp3480;}); void*
t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp3478=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3478[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3479; _temp3479.tag= Cyc_Absyn_VarType; _temp3479.f1=
tv; _temp3479;}); _temp3478;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3470=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3470[ 0]=({ struct Cyc_Stmt_tok_struct _temp3471; _temp3471.tag= Cyc_Stmt_tok;
_temp3471.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3472=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3472[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3473; _temp3473.tag= Cyc_Absyn_Region_s;
_temp3473.f1= tv; _temp3473.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3474=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3474->f1=(
void*) Cyc_Absyn_Loc_n; _temp3474->f2=({ struct _tagged_string* _temp3475=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3475[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3475;}); _temp3474;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3476=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3476[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3477; _temp3477.tag= Cyc_Absyn_RgnHandleType; _temp3477.f1=( void*) t;
_temp3477;}); _temp3476;}), 0); _temp3473.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3473;}); _temp3472;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3471;}); _temp3470;}); break;} case 220: _LL3465: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
struct _tagged_string)({ char* _temp3483=( char*)"H"; struct _tagged_string
_temp3484; _temp3484.curr= _temp3483; _temp3484.base= _temp3483; _temp3484.last_plus_one=
_temp3483 + 2; _temp3484;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp3485=( char*)"bad occurrence of heap region `H"; struct
_tagged_string _temp3486; _temp3486.curr= _temp3485; _temp3486.base= _temp3485;
_temp3486.last_plus_one= _temp3485 + 33; _temp3486;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp3497=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3497->name=({ struct
_tagged_string* _temp3498=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3498[ 0]=({ struct _tagged_string _temp3499= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); xprintf("`%.*s",
_temp3499.last_plus_one - _temp3499.curr, _temp3499.curr);}); _temp3498;});
_temp3497->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp3497;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3495=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3495[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3496; _temp3496.tag= Cyc_Absyn_VarType;
_temp3496.f1= tv; _temp3496;}); _temp3495;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3487=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3487[ 0]=({ struct Cyc_Stmt_tok_struct _temp3488; _temp3488.tag= Cyc_Stmt_tok;
_temp3488.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3489=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3489[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3490; _temp3490.tag= Cyc_Absyn_Region_s;
_temp3490.f1= tv; _temp3490.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3491=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3491->f1=(
void*) Cyc_Absyn_Loc_n; _temp3491->f2=({ struct _tagged_string* _temp3492=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3492[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3492;}); _temp3491;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3493=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3493[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3494; _temp3494.tag= Cyc_Absyn_RgnHandleType; _temp3494.f1=( void*) t;
_temp3494;}); _temp3493;}), 0); _temp3490.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3490;}); _temp3489;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3488;}); _temp3487;}); break;} case 221: _LL3482: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3501=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3501[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3502; _temp3502.tag= Cyc_Stmt_tok; _temp3502.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Cut_s_struct* _temp3503=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp3503[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp3504; _temp3504.tag= Cyc_Absyn_Cut_s; _temp3504.f1= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3504;});
_temp3503;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3502;});
_temp3501;}); break; case 222: _LL3500: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3506=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3506[ 0]=({ struct Cyc_Stmt_tok_struct _temp3507; _temp3507.tag= Cyc_Stmt_tok;
_temp3507.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp3508=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp3508[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp3509; _temp3509.tag= Cyc_Absyn_Splice_s;
_temp3509.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3509;}); _temp3508;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3507;}); _temp3506;}); break; case 223: _LL3505: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3511=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3511[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3512; _temp3512.tag= Cyc_Stmt_tok; _temp3512.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Label_s_struct* _temp3513=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp3513[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp3514; _temp3514.tag= Cyc_Absyn_Label_s; _temp3514.f1=({
struct _tagged_string* _temp3515=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3515[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3515;}); _temp3514.f2= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3514;});
_temp3513;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3512;});
_temp3511;}); break; case 224: _LL3510: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3517=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3517[ 0]=({ struct Cyc_Stmt_tok_struct _temp3518; _temp3518.tag= Cyc_Stmt_tok;
_temp3518.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3518;});
_temp3517;}); break; case 225: _LL3516: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3520=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3520[ 0]=({ struct Cyc_Stmt_tok_struct _temp3521; _temp3521.tag= Cyc_Stmt_tok;
_temp3521.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3521;});
_temp3520;}); break; case 226: _LL3519: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3523=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3523[ 0]=({ struct Cyc_Stmt_tok_struct _temp3524; _temp3524.tag= Cyc_Stmt_tok;
_temp3524.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3524;});
_temp3523;}); break; case 227: _LL3522: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 228: _LL3525:{ void* _temp3527= Cyc_yyget_BlockItem_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); struct Cyc_List_List*
_temp3535; struct Cyc_Absyn_Fndecl* _temp3537; struct Cyc_Absyn_Stmt* _temp3539;
_LL3529: if(*(( int*) _temp3527) == Cyc_Parse_TopDecls_bl){ _LL3536: _temp3535=(
struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*) _temp3527)->f1;
goto _LL3530;} else{ goto _LL3531;} _LL3531: if(*(( int*) _temp3527) == Cyc_Parse_FnDecl_bl){
_LL3538: _temp3537=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp3527)->f1; goto _LL3532;} else{ goto _LL3533;} _LL3533: if(*(( int*)
_temp3527) == Cyc_Parse_Stmt_bl){ _LL3540: _temp3539=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp3527)->f1; goto _LL3534;} else{ goto
_LL3528;} _LL3530: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3541=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3541[ 0]=({ struct Cyc_Stmt_tok_struct _temp3542; _temp3542.tag= Cyc_Stmt_tok;
_temp3542.f1= Cyc_Parse_flatten_declarations( _temp3535, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line))); _temp3542;}); _temp3541;}); goto _LL3528; _LL3532:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3543=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3543[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3544; _temp3544.tag= Cyc_Stmt_tok; _temp3544.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp3545=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3545[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3546; _temp3546.tag= Cyc_Absyn_Fn_d;
_temp3546.f1= _temp3537; _temp3546;}); _temp3545;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line))); _temp3544;});
_temp3543;}); goto _LL3528; _LL3534: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3547=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3547[ 0]=({ struct Cyc_Stmt_tok_struct _temp3548; _temp3548.tag= Cyc_Stmt_tok;
_temp3548.f1= _temp3539; _temp3548;}); _temp3547;}); goto _LL3528; _LL3528:;}
break; case 229: _LL3526:{ void* _temp3550= Cyc_yyget_BlockItem_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); struct Cyc_List_List* _temp3558;
struct Cyc_Absyn_Fndecl* _temp3560; struct Cyc_Absyn_Stmt* _temp3562; _LL3552:
if(*(( int*) _temp3550) == Cyc_Parse_TopDecls_bl){ _LL3559: _temp3558=( struct
Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*) _temp3550)->f1; goto
_LL3553;} else{ goto _LL3554;} _LL3554: if(*(( int*) _temp3550) == Cyc_Parse_FnDecl_bl){
_LL3561: _temp3560=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp3550)->f1; goto _LL3555;} else{ goto _LL3556;} _LL3556: if(*(( int*)
_temp3550) == Cyc_Parse_Stmt_bl){ _LL3563: _temp3562=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp3550)->f1; goto _LL3557;} else{ goto
_LL3551;} _LL3553: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3564=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3564[ 0]=({ struct Cyc_Stmt_tok_struct _temp3565; _temp3565.tag= Cyc_Stmt_tok;
_temp3565.f1= Cyc_Parse_flatten_declarations( _temp3558, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp3565;});
_temp3564;}); goto _LL3551; _LL3555: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3566=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3566[ 0]=({ struct Cyc_Stmt_tok_struct _temp3567; _temp3567.tag= Cyc_Stmt_tok;
_temp3567.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp3568=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3568[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3569; _temp3569.tag= Cyc_Absyn_Fn_d;
_temp3569.f1= _temp3560; _temp3569;}); _temp3568;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3567;}); _temp3566;}); goto _LL3551; _LL3557: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3570=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3570[ 0]=({ struct Cyc_Stmt_tok_struct _temp3571; _temp3571.tag= Cyc_Stmt_tok;
_temp3571.f1= Cyc_Absyn_seq_stmt( _temp3562, Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3571;}); _temp3570;}); goto _LL3551; _LL3551:;} break; case 230: _LL3549:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3573=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp3573[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp3574; _temp3574.tag= Cyc_BlockItem_tok; _temp3574.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp3575=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp3575[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp3576; _temp3576.tag= Cyc_Parse_TopDecls_bl;
_temp3576.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3576;}); _temp3575;})); _temp3574;}); _temp3573;}); break;
case 231: _LL3572: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3578=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3578[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3579; _temp3579.tag= Cyc_BlockItem_tok;
_temp3579.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp3580=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp3580[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp3581; _temp3581.tag= Cyc_Parse_Stmt_bl;
_temp3581.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3581;}); _temp3580;})); _temp3579;}); _temp3578;}); break;
case 232: _LL3577: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3583=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3583[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3584; _temp3584.tag= Cyc_BlockItem_tok;
_temp3584.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp3585=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp3585[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp3586; _temp3586.tag= Cyc_Parse_FnDecl_bl;
_temp3586.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3586;}); _temp3585;})); _temp3584;}); _temp3583;}); break;
case 233: _LL3582: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3588=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3588[ 0]=({ struct Cyc_Stmt_tok_struct _temp3589; _temp3589.tag= Cyc_Stmt_tok;
_temp3589.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3589;}); _temp3588;}); break; case 234: _LL3587: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3591=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3591[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3592; _temp3592.tag= Cyc_Stmt_tok; _temp3592.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3592;}); _temp3591;}); break; case 235: _LL3590: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3594=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3594[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3595; _temp3595.tag= Cyc_Stmt_tok; _temp3595.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3595;}); _temp3594;}); break; case 236: _LL3593: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3597=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3597[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3598; _temp3598.tag= Cyc_Stmt_tok; _temp3598.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),
Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3598;}); _temp3597;}); break; case 237: _LL3596:
yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3600=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3600[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3601; _temp3601.tag= Cyc_SwitchClauseList_tok;
_temp3601.f1= 0; _temp3601;}); _temp3600;}); break; case 238: _LL3599: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3603=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3603[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3604; _temp3604.tag= Cyc_SwitchClauseList_tok;
_temp3604.f1=({ struct Cyc_List_List* _temp3605=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3605->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3606=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3606->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp3606->pat_vars= 0; _temp3606->where_clause= 0; _temp3606->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3606->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3606;}); _temp3605->tl= 0; _temp3605;});
_temp3604;}); _temp3603;}); break; case 239: _LL3602: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp3608=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3608[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3609; _temp3609.tag= Cyc_SwitchClauseList_tok;
_temp3609.f1=({ struct Cyc_List_List* _temp3610=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3610->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3611=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3611->pattern= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3611->pat_vars= 0; _temp3611->where_clause= 0;
_temp3611->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3611->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3611;});
_temp3610->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3610;}); _temp3609;}); _temp3608;}); break; case 240:
_LL3607: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3613=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp3613[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp3614; _temp3614.tag=
Cyc_SwitchClauseList_tok; _temp3614.f1=({ struct Cyc_List_List* _temp3615=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3615->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp3616=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3616->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3616->pat_vars=
0; _temp3616->where_clause= 0; _temp3616->body= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3616->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp3616;}); _temp3615->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3615;}); _temp3614;}); _temp3613;});
break; case 241: _LL3612: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3618=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3618[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3619; _temp3619.tag= Cyc_SwitchClauseList_tok; _temp3619.f1=({ struct Cyc_List_List*
_temp3620=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3620->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3621=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3621->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3621->pat_vars=
0; _temp3621->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3621->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3621->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3621;}); _temp3620->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3620;}); _temp3619;}); _temp3618;});
break; case 242: _LL3617: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3623=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3623[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3624; _temp3624.tag= Cyc_SwitchClauseList_tok; _temp3624.f1=({ struct Cyc_List_List*
_temp3625=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3625->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3626=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3626->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3626->pat_vars=
0; _temp3626->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3626->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3626->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3626;}); _temp3625->tl= Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3625;});
_temp3624;}); _temp3623;}); break; case 243: _LL3622: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3628=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3628[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3629; _temp3629.tag= Cyc_Stmt_tok; _temp3629.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3629;}); _temp3628;}); break; case 244: _LL3627: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3631=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3631[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3632; _temp3632.tag= Cyc_Stmt_tok; _temp3632.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3632;}); _temp3631;}); break; case 245: _LL3630: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3634=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3634[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3635; _temp3635.tag= Cyc_Stmt_tok; _temp3635.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3635;}); _temp3634;}); break; case 246: _LL3633: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3637=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3637[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3638; _temp3638.tag= Cyc_Stmt_tok; _temp3638.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3638;});
_temp3637;}); break; case 247: _LL3636: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3640=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3640[ 0]=({ struct Cyc_Stmt_tok_struct _temp3641; _temp3641.tag= Cyc_Stmt_tok;
_temp3641.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3641;}); _temp3640;}); break; case 248: _LL3639:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3643=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3643[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3644; _temp3644.tag= Cyc_Stmt_tok; _temp3644.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3644;}); _temp3643;}); break; case 249: _LL3642: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3646=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3646[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3647; _temp3647.tag= Cyc_Stmt_tok; _temp3647.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3647;});
_temp3646;}); break; case 250: _LL3645: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3649=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3649[ 0]=({ struct Cyc_Stmt_tok_struct _temp3650; _temp3650.tag= Cyc_Stmt_tok;
_temp3650.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 5)), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3650;}); _temp3649;}); break; case 251: _LL3648: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3652=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3652[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3653; _temp3653.tag= Cyc_Stmt_tok; _temp3653.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 7)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3653;}); _temp3652;}); break; case 252: _LL3651:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3655=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3655[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3656; _temp3656.tag= Cyc_Stmt_tok; _temp3656.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 6)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 8)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3656;}); _temp3655;}); break; case 253: _LL3654: { struct Cyc_List_List*
_temp3658= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); goto _LL3659; _LL3659: { struct Cyc_Absyn_Stmt* _temp3660=
Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); goto _LL3661; _LL3661: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3662=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3662[ 0]=({ struct Cyc_Stmt_tok_struct _temp3663; _temp3663.tag= Cyc_Stmt_tok;
_temp3663.f1= Cyc_Parse_flatten_declarations( _temp3658, _temp3660); _temp3663;});
_temp3662;}); break;}} case 254: _LL3657: { struct Cyc_List_List* _temp3665= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); goto
_LL3666; _LL3666: { struct Cyc_Absyn_Stmt* _temp3667= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL3668;
_LL3668: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3669=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3669[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3670; _temp3670.tag= Cyc_Stmt_tok; _temp3670.f1= Cyc_Parse_flatten_declarations(
_temp3665, _temp3667); _temp3670;}); _temp3669;}); break;}} case 255: _LL3664: {
struct Cyc_List_List* _temp3672= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); goto _LL3673; _LL3673: { struct Cyc_Absyn_Stmt*
_temp3674= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3675; _LL3675: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3676=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3676[ 0]=({ struct Cyc_Stmt_tok_struct _temp3677; _temp3677.tag= Cyc_Stmt_tok;
_temp3677.f1= Cyc_Parse_flatten_declarations( _temp3672, _temp3674); _temp3677;});
_temp3676;}); break;}} case 256: _LL3671: { struct Cyc_List_List* _temp3679= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); goto
_LL3680; _LL3680: { struct Cyc_Absyn_Stmt* _temp3681= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3682; _LL3682: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3683=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3683[ 0]=({ struct Cyc_Stmt_tok_struct _temp3684; _temp3684.tag= Cyc_Stmt_tok;
_temp3684.f1= Cyc_Parse_flatten_declarations( _temp3679, _temp3681); _temp3684;});
_temp3683;}); break;}} case 257: _LL3678: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3686=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3686[ 0]=({ struct Cyc_Stmt_tok_struct _temp3687; _temp3687.tag= Cyc_Stmt_tok;
_temp3687.f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp3688=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3688[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3688;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3687;});
_temp3686;}); break; case 258: _LL3685: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3690=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3690[ 0]=({ struct Cyc_Stmt_tok_struct _temp3691; _temp3691.tag= Cyc_Stmt_tok;
_temp3691.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3691;}); _temp3690;}); break; case 259: _LL3689: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3693=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3693[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3694; _temp3694.tag= Cyc_Stmt_tok; _temp3694.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3694;}); _temp3693;}); break; case 260: _LL3692: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3696=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3696[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3697; _temp3697.tag= Cyc_Stmt_tok; _temp3697.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3697;}); _temp3696;}); break; case 261:
_LL3695: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3699=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3699[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3700; _temp3700.tag= Cyc_Stmt_tok; _temp3700.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3700;});
_temp3699;}); break; case 262: _LL3698: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3702=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3702[ 0]=({ struct Cyc_Stmt_tok_struct _temp3703; _temp3703.tag= Cyc_Stmt_tok;
_temp3703.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3703;}); _temp3702;}); break; case 263: _LL3701: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3705=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3705[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3706; _temp3706.tag= Cyc_Stmt_tok; _temp3706.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).last_line)); _temp3706;}); _temp3705;}); break; case 264:
_LL3704: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3708=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3708[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3709; _temp3709.tag= Cyc_Stmt_tok; _temp3709.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).last_line)); _temp3709;}); _temp3708;}); break; case 265:
_LL3707: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3711=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp3711[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3712; _temp3712.tag= Cyc_Pattern_tok; _temp3712.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3712;});
_temp3711;}); break; case 266: _LL3710: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 267: _LL3713: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3715=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3715[ 0]=({ struct Cyc_Pattern_tok_struct _temp3716; _temp3716.tag= Cyc_Pattern_tok;
_temp3716.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp3717=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp3717[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp3718; _temp3718.tag= Cyc_Absyn_Int_p;
_temp3718.f1=( void*)(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f1; _temp3718.f2=(* Cyc_yyget_Int_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))).f2; _temp3718;}); _temp3717;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3716;}); _temp3715;}); break; case 268: _LL3714: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3720=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3720[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3721; _temp3721.tag= Cyc_Pattern_tok; _temp3721.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp3722=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp3722[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp3723; _temp3723.tag= Cyc_Absyn_Int_p; _temp3723.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp3723.f2= -(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp3723;}); _temp3722;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3721;}); _temp3720;}); break; case 269: _LL3719: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3725=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3725[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3726; _temp3726.tag= Cyc_Pattern_tok; _temp3726.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp3727=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp3727[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp3728; _temp3728.tag= Cyc_Absyn_Float_p; _temp3728.f1=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3728;}); _temp3727;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3726;});
_temp3725;}); break; case 270: _LL3724: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3730=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3730[ 0]=({ struct Cyc_Pattern_tok_struct _temp3731; _temp3731.tag= Cyc_Pattern_tok;
_temp3731.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp3732=( struct Cyc_Absyn_Char_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct));
_temp3732[ 0]=({ struct Cyc_Absyn_Char_p_struct _temp3733; _temp3733.tag= Cyc_Absyn_Char_p;
_temp3733.f1= Cyc_yyget_Char_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3733;}); _temp3732;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3731;}); _temp3730;}); break; case 271: _LL3729: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3735=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3735[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3736; _temp3736.tag= Cyc_Pattern_tok; _temp3736.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3736;});
_temp3735;}); break; case 272: _LL3734: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3738=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3738[ 0]=({ struct Cyc_Pattern_tok_struct _temp3739; _temp3739.tag= Cyc_Pattern_tok;
_temp3739.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp3740=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp3740[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp3741; _temp3741.tag=
Cyc_Absyn_UnknownId_p; _temp3741.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3741;}); _temp3740;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3739;}); _temp3738;}); break; case 273: _LL3737: { struct Cyc_List_List*
_temp3743=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3744; _LL3744: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3745=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3745[ 0]=({ struct Cyc_Pattern_tok_struct _temp3746; _temp3746.tag= Cyc_Pattern_tok;
_temp3746.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp3747=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp3747[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp3748; _temp3748.tag= Cyc_Absyn_UnknownCall_p; _temp3748.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3748.f2=
_temp3743; _temp3748.f3= Cyc_yyget_PatternList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3748;}); _temp3747;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3746;}); _temp3745;}); break;} case 274: _LL3742: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3750=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3750[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3751; _temp3751.tag= Cyc_Pattern_tok; _temp3751.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp3752=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp3752[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp3753; _temp3753.tag= Cyc_Absyn_Tuple_p; _temp3753.f1=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3753;}); _temp3752;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3751;});
_temp3750;}); break; case 275: _LL3749: { struct Cyc_List_List* _temp3755=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); goto
_LL3756; _LL3756: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3757=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3757[ 0]=({ struct Cyc_Pattern_tok_struct _temp3758; _temp3758.tag= Cyc_Pattern_tok;
_temp3758.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3759=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3759[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3760; _temp3760.tag= Cyc_Absyn_UnknownFields_p; _temp3760.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3760.f2=
_temp3755; _temp3760.f3= 0; _temp3760;}); _temp3759;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3758;}); _temp3757;}); break;} case 276: _LL3754: { struct Cyc_List_List*
_temp3762=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3763; _LL3763: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3764=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3764[ 0]=({ struct Cyc_Pattern_tok_struct _temp3765; _temp3765.tag= Cyc_Pattern_tok;
_temp3765.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3766=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3766[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3767; _temp3767.tag= Cyc_Absyn_UnknownFields_p; _temp3767.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3767.f2=
_temp3762; _temp3767.f3= Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3767;}); _temp3766;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3765;}); _temp3764;}); break;} case 277: _LL3761: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3769=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3769[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3770; _temp3770.tag= Cyc_Pattern_tok; _temp3770.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp3771=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp3771[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp3772; _temp3772.tag= Cyc_Absyn_Pointer_p;
_temp3772.f1= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3772;}); _temp3771;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3770;}); _temp3769;}); break; case 278: _LL3768: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3774=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3774[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3775; _temp3775.tag= Cyc_Pattern_tok; _temp3775.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp3776=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp3776[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp3777; _temp3777.tag= Cyc_Absyn_Reference_p;
_temp3777.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3778=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3778->f1=( void*) Cyc_Absyn_Loc_n;
_temp3778->f2=({ struct _tagged_string* _temp3779=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3779[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3779;});
_temp3778;}),( void*) Cyc_Absyn_VoidType, 0); _temp3777;}); _temp3776;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3775;}); _temp3774;}); break; case 279: _LL3773: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3781=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3781[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3782; _temp3782.tag= Cyc_PatternList_tok;
_temp3782.f1= 0; _temp3782;}); _temp3781;}); break; case 280: _LL3780: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3784=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3784[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3785; _temp3785.tag= Cyc_PatternList_tok;
_temp3785.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3785;}); _temp3784;}); break; case 281: _LL3783: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3787=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3787[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3788; _temp3788.tag= Cyc_PatternList_tok;
_temp3788.f1=({ struct Cyc_List_List* _temp3789=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3789->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3789->tl=
0; _temp3789;}); _temp3788;}); _temp3787;}); break; case 282: _LL3786: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3791=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3791[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3792; _temp3792.tag= Cyc_PatternList_tok;
_temp3792.f1=({ struct Cyc_List_List* _temp3793=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3793->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3793->tl=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3793;}); _temp3792;}); _temp3791;}); break; case 283: _LL3790: yyval=( void*)({
struct Cyc_FieldPattern_tok_struct* _temp3795=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3795[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3796; _temp3796.tag= Cyc_FieldPattern_tok;
_temp3796.f1=({ struct _tuple13* _temp3797=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3797->f1= 0; _temp3797->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3797;});
_temp3796;}); _temp3795;}); break; case 284: _LL3794: yyval=( void*)({ struct
Cyc_FieldPattern_tok_struct* _temp3799=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3799[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3800; _temp3800.tag= Cyc_FieldPattern_tok;
_temp3800.f1=({ struct _tuple13* _temp3801=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp3801->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3801->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3801;});
_temp3800;}); _temp3799;}); break; case 285: _LL3798: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3803=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3803[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3804; _temp3804.tag= Cyc_FieldPatternList_tok;
_temp3804.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3804;}); _temp3803;}); break; case 286: _LL3802: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3806=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3806[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3807; _temp3807.tag= Cyc_FieldPatternList_tok;
_temp3807.f1=({ struct Cyc_List_List* _temp3808=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3808->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3808->tl=
0; _temp3808;}); _temp3807;}); _temp3806;}); break; case 287: _LL3805: yyval=(
void*)({ struct Cyc_FieldPatternList_tok_struct* _temp3810=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3810[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3811; _temp3811.tag= Cyc_FieldPatternList_tok;
_temp3811.f1=({ struct Cyc_List_List* _temp3812=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3812->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3812->tl=
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3812;}); _temp3811;}); _temp3810;}); break; case 288: _LL3809: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 289:
_LL3813: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3815=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3815[ 0]=({ struct Cyc_Exp_tok_struct
_temp3816; _temp3816.tag= Cyc_Exp_tok; _temp3816.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3816;}); _temp3815;}); break; case 290: _LL3814: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 291: _LL3817: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3819=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3819[ 0]=({ struct Cyc_Exp_tok_struct
_temp3820; _temp3820.tag= Cyc_Exp_tok; _temp3820.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Primopopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3820;}); _temp3819;}); break; case 292: _LL3818: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3822=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3822[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3823; _temp3823.tag= Cyc_Primopopt_tok; _temp3823.f1=
0; _temp3823;}); _temp3822;}); break; case 293: _LL3821: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3825=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3825[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3826; _temp3826.tag= Cyc_Primopopt_tok; _temp3826.f1=({
struct Cyc_Core_Opt* _temp3827=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3827->v=( void*)(( void*) Cyc_Absyn_Times); _temp3827;});
_temp3826;}); _temp3825;}); break; case 294: _LL3824: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3829=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3829[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3830; _temp3830.tag= Cyc_Primopopt_tok; _temp3830.f1=({
struct Cyc_Core_Opt* _temp3831=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3831->v=( void*)(( void*) Cyc_Absyn_Div); _temp3831;});
_temp3830;}); _temp3829;}); break; case 295: _LL3828: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3833=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3833[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3834; _temp3834.tag= Cyc_Primopopt_tok; _temp3834.f1=({
struct Cyc_Core_Opt* _temp3835=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3835->v=( void*)(( void*) Cyc_Absyn_Mod); _temp3835;});
_temp3834;}); _temp3833;}); break; case 296: _LL3832: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3837=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3837[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3838; _temp3838.tag= Cyc_Primopopt_tok; _temp3838.f1=({
struct Cyc_Core_Opt* _temp3839=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3839->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3839;});
_temp3838;}); _temp3837;}); break; case 297: _LL3836: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3841=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3841[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3842; _temp3842.tag= Cyc_Primopopt_tok; _temp3842.f1=({
struct Cyc_Core_Opt* _temp3843=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3843->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3843;});
_temp3842;}); _temp3841;}); break; case 298: _LL3840: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3845=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3845[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3846; _temp3846.tag= Cyc_Primopopt_tok; _temp3846.f1=({
struct Cyc_Core_Opt* _temp3847=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3847->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3847;});
_temp3846;}); _temp3845;}); break; case 299: _LL3844: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3849=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3849[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3850; _temp3850.tag= Cyc_Primopopt_tok; _temp3850.f1=({
struct Cyc_Core_Opt* _temp3851=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3851->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3851;});
_temp3850;}); _temp3849;}); break; case 300: _LL3848: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3853=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3853[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3854; _temp3854.tag= Cyc_Primopopt_tok; _temp3854.f1=({
struct Cyc_Core_Opt* _temp3855=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3855->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3855;});
_temp3854;}); _temp3853;}); break; case 301: _LL3852: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3857=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3857[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3858; _temp3858.tag= Cyc_Primopopt_tok; _temp3858.f1=({
struct Cyc_Core_Opt* _temp3859=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3859->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3859;});
_temp3858;}); _temp3857;}); break; case 302: _LL3856: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3861=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3861[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3862; _temp3862.tag= Cyc_Primopopt_tok; _temp3862.f1=({
struct Cyc_Core_Opt* _temp3863=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3863->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3863;});
_temp3862;}); _temp3861;}); break; case 303: _LL3860: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 304: _LL3864: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3866=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3866[ 0]=({ struct Cyc_Exp_tok_struct
_temp3867; _temp3867.tag= Cyc_Exp_tok; _temp3867.f1= Cyc_Absyn_conditional_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3867;}); _temp3866;}); break; case 305: _LL3865: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3869=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3869[ 0]=({ struct Cyc_Exp_tok_struct
_temp3870; _temp3870.tag= Cyc_Exp_tok; _temp3870.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3870;}); _temp3869;}); break; case 306: _LL3868: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3872=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3872[ 0]=({ struct Cyc_Exp_tok_struct
_temp3873; _temp3873.tag= Cyc_Exp_tok; _temp3873.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3873;}); _temp3872;}); break; case 307: _LL3871: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3875=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3875[ 0]=({ struct Cyc_Exp_tok_struct
_temp3876; _temp3876.tag= Cyc_Exp_tok; _temp3876.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3876;}); _temp3875;}); break; case 308: _LL3874: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3878=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3878[ 0]=({ struct Cyc_Exp_tok_struct
_temp3879; _temp3879.tag= Cyc_Exp_tok; _temp3879.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3879;});
_temp3878;}); break; case 309: _LL3877: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3881=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3881[ 0]=({ struct Cyc_Exp_tok_struct _temp3882; _temp3882.tag= Cyc_Exp_tok;
_temp3882.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3882;}); _temp3881;}); break; case 310: _LL3880: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 311: _LL3883: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 312: _LL3884: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3886=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3886[ 0]=({ struct Cyc_Exp_tok_struct
_temp3887; _temp3887.tag= Cyc_Exp_tok; _temp3887.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3887;}); _temp3886;}); break; case 313: _LL3885: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 314: _LL3888: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3890=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3890[ 0]=({ struct Cyc_Exp_tok_struct
_temp3891; _temp3891.tag= Cyc_Exp_tok; _temp3891.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3891;}); _temp3890;}); break; case 315: _LL3889: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 316: _LL3892: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3894=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3894[ 0]=({ struct Cyc_Exp_tok_struct
_temp3895; _temp3895.tag= Cyc_Exp_tok; _temp3895.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3895;});
_temp3894;}); break; case 317: _LL3893: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 318: _LL3896: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3898=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3898[ 0]=({ struct Cyc_Exp_tok_struct _temp3899; _temp3899.tag= Cyc_Exp_tok;
_temp3899.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3899;}); _temp3898;}); break; case 319: _LL3897: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 320: _LL3900: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3902=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3902[ 0]=({ struct Cyc_Exp_tok_struct
_temp3903; _temp3903.tag= Cyc_Exp_tok; _temp3903.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3903;});
_temp3902;}); break; case 321: _LL3901: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 322: _LL3904: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3906=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3906[ 0]=({ struct Cyc_Exp_tok_struct _temp3907; _temp3907.tag= Cyc_Exp_tok;
_temp3907.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3907;});
_temp3906;}); break; case 323: _LL3905: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3909=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3909[ 0]=({ struct Cyc_Exp_tok_struct _temp3910; _temp3910.tag= Cyc_Exp_tok;
_temp3910.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3910;});
_temp3909;}); break; case 324: _LL3908: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 325: _LL3911: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3913=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3913[ 0]=({ struct Cyc_Exp_tok_struct _temp3914; _temp3914.tag= Cyc_Exp_tok;
_temp3914.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3914;});
_temp3913;}); break; case 326: _LL3912: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3916=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3916[ 0]=({ struct Cyc_Exp_tok_struct _temp3917; _temp3917.tag= Cyc_Exp_tok;
_temp3917.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3917;});
_temp3916;}); break; case 327: _LL3915: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3919=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3919[ 0]=({ struct Cyc_Exp_tok_struct _temp3920; _temp3920.tag= Cyc_Exp_tok;
_temp3920.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3920;});
_temp3919;}); break; case 328: _LL3918: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3922=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3922[ 0]=({ struct Cyc_Exp_tok_struct _temp3923; _temp3923.tag= Cyc_Exp_tok;
_temp3923.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3923;});
_temp3922;}); break; case 329: _LL3921: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 330: _LL3924: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3926=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3926[ 0]=({ struct Cyc_Exp_tok_struct _temp3927; _temp3927.tag= Cyc_Exp_tok;
_temp3927.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3927;}); _temp3926;}); break; case 331: _LL3925: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3929=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3929[ 0]=({ struct Cyc_Exp_tok_struct
_temp3930; _temp3930.tag= Cyc_Exp_tok; _temp3930.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3930;});
_temp3929;}); break; case 332: _LL3928: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 333: _LL3931: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3933=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3933[ 0]=({ struct Cyc_Exp_tok_struct _temp3934; _temp3934.tag= Cyc_Exp_tok;
_temp3934.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3934;}); _temp3933;}); break; case 334: _LL3932: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3936=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3936[ 0]=({ struct Cyc_Exp_tok_struct
_temp3937; _temp3937.tag= Cyc_Exp_tok; _temp3937.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3937;});
_temp3936;}); break; case 335: _LL3935: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 336: _LL3938: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3940=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3940[ 0]=({ struct Cyc_Exp_tok_struct _temp3941; _temp3941.tag= Cyc_Exp_tok;
_temp3941.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3941;}); _temp3940;}); break; case 337: _LL3939: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3943=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3943[ 0]=({ struct Cyc_Exp_tok_struct
_temp3944; _temp3944.tag= Cyc_Exp_tok; _temp3944.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3944;});
_temp3943;}); break; case 338: _LL3942: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3946=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3946[ 0]=({ struct Cyc_Exp_tok_struct _temp3947; _temp3947.tag= Cyc_Exp_tok;
_temp3947.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3947;}); _temp3946;}); break; case 339: _LL3945: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 340: _LL3948: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3950=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3950[ 0]=({ struct Cyc_Exp_tok_struct
_temp3951; _temp3951.tag= Cyc_Exp_tok; _temp3951.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f3, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3951;}); _temp3950;}); break; case 341: _LL3949: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 342: _LL3952: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3954=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3954[ 0]=({ struct Cyc_Exp_tok_struct
_temp3955; _temp3955.tag= Cyc_Exp_tok; _temp3955.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3955;}); _temp3954;}); break; case 343: _LL3953: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3957=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3957[ 0]=({ struct Cyc_Exp_tok_struct
_temp3958; _temp3958.tag= Cyc_Exp_tok; _temp3958.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3958;}); _temp3957;}); break; case 344: _LL3956: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3960=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3960[ 0]=({ struct Cyc_Exp_tok_struct
_temp3961; _temp3961.tag= Cyc_Exp_tok; _temp3961.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3961;}); _temp3960;}); break; case 345: _LL3959: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3963=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3963[ 0]=({ struct Cyc_Exp_tok_struct
_temp3964; _temp3964.tag= Cyc_Exp_tok; _temp3964.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3964;}); _temp3963;}); break; case 346: _LL3962: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 347: _LL3965: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3967=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3967[ 0]=({ struct Cyc_Exp_tok_struct
_temp3968; _temp3968.tag= Cyc_Exp_tok; _temp3968.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3968;}); _temp3967;}); break; case 348: _LL3966: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3970=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3970[ 0]=({ struct Cyc_Exp_tok_struct
_temp3971; _temp3971.tag= Cyc_Exp_tok; _temp3971.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3971;}); _temp3970;}); break; case 349: _LL3969:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3973=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3973[ 0]=({ struct Cyc_Exp_tok_struct
_temp3974; _temp3974.tag= Cyc_Exp_tok; _temp3974.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3974;}); _temp3973;}); break; case 350: _LL3972: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3976=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3976[ 0]=({ struct Cyc_Exp_tok_struct
_temp3977; _temp3977.tag= Cyc_Exp_tok; _temp3977.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3977;}); _temp3976;}); break; case 351: _LL3975: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3979=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3979[ 0]=({ struct Cyc_Exp_tok_struct
_temp3980; _temp3980.tag= Cyc_Exp_tok; _temp3980.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3981=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3981[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3982; _temp3982.tag= Cyc_Absyn_Malloc_e;
_temp3982.f1= 0; _temp3982.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp3982;}); _temp3981;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3980;});
_temp3979;}); break; case 352: _LL3978: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3984=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3984[ 0]=({ struct Cyc_Exp_tok_struct _temp3985; _temp3985.tag= Cyc_Exp_tok;
_temp3985.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3986=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3986[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3987; _temp3987.tag= Cyc_Absyn_Malloc_e;
_temp3987.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 6)); _temp3987.f2=( void*) Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).last_line)); _temp3987;}); _temp3986;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 8)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3985;}); _temp3984;}); break; case 353: _LL3983: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3989=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3989[ 0]=({ struct Cyc_Primop_tok_struct
_temp3990; _temp3990.tag= Cyc_Primop_tok; _temp3990.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp3990;}); _temp3989;}); break; case 354: _LL3988: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3992=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3992[ 0]=({ struct Cyc_Primop_tok_struct
_temp3993; _temp3993.tag= Cyc_Primop_tok; _temp3993.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp3993;}); _temp3992;}); break; case 355: _LL3991: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3995=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3995[ 0]=({ struct Cyc_Primop_tok_struct
_temp3996; _temp3996.tag= Cyc_Primop_tok; _temp3996.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp3996;}); _temp3995;}); break; case 356: _LL3994: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3998=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3998[ 0]=({ struct Cyc_Primop_tok_struct
_temp3999; _temp3999.tag= Cyc_Primop_tok; _temp3999.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp3999;}); _temp3998;}); break; case 357: _LL3997: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4001=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4001[ 0]=({ struct Cyc_Primop_tok_struct
_temp4002; _temp4002.tag= Cyc_Primop_tok; _temp4002.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp4002;}); _temp4001;}); break; case 358: _LL4000: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4004=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4004[ 0]=({ struct Cyc_Primop_tok_struct
_temp4005; _temp4005.tag= Cyc_Primop_tok; _temp4005.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp4005;}); _temp4004;}); break; case 359: _LL4003: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4007=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4007[ 0]=({ struct Cyc_Primop_tok_struct
_temp4008; _temp4008.tag= Cyc_Primop_tok; _temp4008.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp4008;}); _temp4007;}); break; case 360: _LL4006: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4010=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4010[ 0]=({ struct Cyc_Primop_tok_struct
_temp4011; _temp4011.tag= Cyc_Primop_tok; _temp4011.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp4011;}); _temp4010;}); break; case 361: _LL4009: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp4013=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp4013[ 0]=({ struct Cyc_Primop_tok_struct
_temp4014; _temp4014.tag= Cyc_Primop_tok; _temp4014.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp4014;}); _temp4013;}); break; case 362: _LL4012: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 363: _LL4015: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4017=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4017[ 0]=({ struct Cyc_Exp_tok_struct
_temp4018; _temp4018.tag= Cyc_Exp_tok; _temp4018.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4018;}); _temp4017;}); break; case 364: _LL4016: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4020=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4020[ 0]=({ struct Cyc_Exp_tok_struct
_temp4021; _temp4021.tag= Cyc_Exp_tok; _temp4021.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4021;}); _temp4020;}); break; case 365: _LL4019: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4023=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4023[ 0]=({ struct Cyc_Exp_tok_struct
_temp4024; _temp4024.tag= Cyc_Exp_tok; _temp4024.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4024;}); _temp4023;}); break; case 366: _LL4022: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4026=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4026[ 0]=({ struct Cyc_Exp_tok_struct
_temp4027; _temp4027.tag= Cyc_Exp_tok; _temp4027.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4028=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4028[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4028;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4027;}); _temp4026;}); break; case 367: _LL4025: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4030=(* q).f1; struct Cyc_List_List* _temp4040; struct Cyc_List_List*
_temp4042; _LL4032: if( _temp4030 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4033;}
else{ goto _LL4034;} _LL4034: if(( unsigned int) _temp4030 > 1u?*(( int*)
_temp4030) == Cyc_Absyn_Rel_n: 0){ _LL4041: _temp4040=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp4030)->f1; if( _temp4040 == 0){ goto
_LL4035;} else{ goto _LL4036;}} else{ goto _LL4036;} _LL4036: if(( unsigned int)
_temp4030 > 1u?*(( int*) _temp4030) == Cyc_Absyn_Abs_n: 0){ _LL4043: _temp4042=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp4030)->f1; if(
_temp4042 == 0){ goto _LL4037;} else{ goto _LL4038;}} else{ goto _LL4038;}
_LL4038: goto _LL4039; _LL4033: goto _LL4031; _LL4035: goto _LL4031; _LL4037:
goto _LL4031; _LL4039: Cyc_Parse_err(( struct _tagged_string)({ char* _temp4044=(
char*)"struct field name is qualified"; struct _tagged_string _temp4045;
_temp4045.curr= _temp4044; _temp4045.base= _temp4044; _temp4045.last_plus_one=
_temp4044 + 31; _temp4045;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL4031;
_LL4031:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4046=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4046[ 0]=({ struct Cyc_Exp_tok_struct
_temp4047; _temp4047.tag= Cyc_Exp_tok; _temp4047.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(* q).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4047;}); _temp4046;}); break;} case 368:
_LL4029: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4049=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4049[ 0]=({ struct Cyc_Exp_tok_struct
_temp4050; _temp4050.tag= Cyc_Exp_tok; _temp4050.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4051=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4051[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4051;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4050;}); _temp4049;}); break; case 369: _LL4048: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4053=(* q).f1; struct Cyc_List_List* _temp4063; struct Cyc_List_List*
_temp4065; _LL4055: if( _temp4053 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4056;}
else{ goto _LL4057;} _LL4057: if(( unsigned int) _temp4053 > 1u?*(( int*)
_temp4053) == Cyc_Absyn_Rel_n: 0){ _LL4064: _temp4063=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp4053)->f1; if( _temp4063 == 0){ goto
_LL4058;} else{ goto _LL4059;}} else{ goto _LL4059;} _LL4059: if(( unsigned int)
_temp4053 > 1u?*(( int*) _temp4053) == Cyc_Absyn_Abs_n: 0){ _LL4066: _temp4065=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp4053)->f1; if(
_temp4065 == 0){ goto _LL4060;} else{ goto _LL4061;}} else{ goto _LL4061;}
_LL4061: goto _LL4062; _LL4056: goto _LL4054; _LL4058: goto _LL4054; _LL4060:
goto _LL4054; _LL4062: Cyc_Parse_err(( struct _tagged_string)({ char* _temp4067=(
char*)"struct field is qualified with module name"; struct _tagged_string
_temp4068; _temp4068.curr= _temp4067; _temp4068.base= _temp4067; _temp4068.last_plus_one=
_temp4067 + 43; _temp4068;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL4054;
_LL4054:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4069=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4069[ 0]=({ struct Cyc_Exp_tok_struct
_temp4070; _temp4070.tag= Cyc_Exp_tok; _temp4070.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(* q).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4070;}); _temp4069;}); break;} case 370:
_LL4052: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4072=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4072[ 0]=({ struct Cyc_Exp_tok_struct
_temp4073; _temp4073.tag= Cyc_Exp_tok; _temp4073.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4073;}); _temp4072;}); break; case 371: _LL4071: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4075=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4075[ 0]=({ struct Cyc_Exp_tok_struct
_temp4076; _temp4076.tag= Cyc_Exp_tok; _temp4076.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4076;}); _temp4075;}); break; case 372: _LL4074: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4078=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4078[ 0]=({ struct Cyc_Exp_tok_struct
_temp4079; _temp4079.tag= Cyc_Exp_tok; _temp4079.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4080=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4080[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4081; _temp4081.tag= Cyc_Absyn_CompoundLit_e;
_temp4081.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp4081.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp4081;}); _temp4080;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4079;}); _temp4078;}); break; case 373: _LL4077: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4083=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4083[ 0]=({ struct Cyc_Exp_tok_struct
_temp4084; _temp4084.tag= Cyc_Exp_tok; _temp4084.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4085=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4085[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4086; _temp4086.tag= Cyc_Absyn_CompoundLit_e;
_temp4086.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp4086.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp4086;}); _temp4085;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4084;}); _temp4083;}); break; case 374: _LL4082: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4088=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4088[ 0]=({ struct Cyc_Exp_tok_struct
_temp4089; _temp4089.tag= Cyc_Exp_tok; _temp4089.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp4090=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp4090[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp4091; _temp4091.tag= Cyc_Absyn_Fill_e; _temp4091.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp4091;});
_temp4090;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4089;});
_temp4088;}); break; case 375: _LL4087: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4093=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4093[ 0]=({ struct Cyc_Exp_tok_struct _temp4094; _temp4094.tag= Cyc_Exp_tok;
_temp4094.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Codegen_e_struct*
_temp4095=( struct Cyc_Absyn_Codegen_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct));
_temp4095[ 0]=({ struct Cyc_Absyn_Codegen_e_struct _temp4096; _temp4096.tag= Cyc_Absyn_Codegen_e;
_temp4096.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp4096;}); _temp4095;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4094;}); _temp4093;}); break; case 376: _LL4092: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4098=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4098[ 0]=({ struct Cyc_Exp_tok_struct
_temp4099; _temp4099.tag= Cyc_Exp_tok; _temp4099.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp4100=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp4100[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp4101; _temp4101.tag= Cyc_Absyn_UnknownId_e;
_temp4101.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4101;}); _temp4100;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4099;}); _temp4098;}); break; case 377: _LL4097: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 378: _LL4102: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4104=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4104[ 0]=({ struct Cyc_Exp_tok_struct
_temp4105; _temp4105.tag= Cyc_Exp_tok; _temp4105.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4105;}); _temp4104;}); break; case 379: _LL4103: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1); break; case 380: _LL4106: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4108=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4108[ 0]=({ struct Cyc_Exp_tok_struct
_temp4109; _temp4109.tag= Cyc_Exp_tok; _temp4109.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4110=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4110[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4111; _temp4111.tag=
Cyc_Absyn_UnknownId_e; _temp4111.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp4111;}); _temp4110;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4109;}); _temp4108;}); break; case 381: _LL4107:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4113=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4113[ 0]=({ struct Cyc_Exp_tok_struct
_temp4114; _temp4114.tag= Cyc_Exp_tok; _temp4114.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4115=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4115[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4116; _temp4116.tag=
Cyc_Absyn_UnknownId_e; _temp4116.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); _temp4116;}); _temp4115;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).last_line)),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4114;}); _temp4113;}); break; case 382: _LL4112: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4118=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4118[ 0]=({ struct Cyc_Exp_tok_struct
_temp4119; _temp4119.tag= Cyc_Exp_tok; _temp4119.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4119;}); _temp4118;}); break; case 383: _LL4117: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4121=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4121[ 0]=({ struct Cyc_Exp_tok_struct
_temp4122; _temp4122.tag= Cyc_Exp_tok; _temp4122.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp4123=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp4123[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp4124; _temp4124.tag= Cyc_Absyn_Struct_e;
_temp4124.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp4124.f2= 0; _temp4124.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp4124.f4=
0; _temp4124;}); _temp4123;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4122;});
_temp4121;}); break; case 384: _LL4120: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4126=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4126[ 0]=({ struct Cyc_Exp_tok_struct _temp4127; _temp4127.tag= Cyc_Exp_tok;
_temp4127.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4127;});
_temp4126;}); break; case 385: _LL4125: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4129=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4129[ 0]=({ struct Cyc_ExpList_tok_struct _temp4130; _temp4130.tag= Cyc_ExpList_tok;
_temp4130.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp4130;}); _temp4129;}); break; case 386: _LL4128: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp4132=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp4132[ 0]=({ struct Cyc_ExpList_tok_struct
_temp4133; _temp4133.tag= Cyc_ExpList_tok; _temp4133.f1=({ struct Cyc_List_List*
_temp4134=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4134->hd=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4134->tl= 0; _temp4134;}); _temp4133;}); _temp4132;});
break; case 387: _LL4131: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4136=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4136[ 0]=({ struct Cyc_ExpList_tok_struct _temp4137; _temp4137.tag= Cyc_ExpList_tok;
_temp4137.f1=({ struct Cyc_List_List* _temp4138=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4138->hd=( void*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp4138->tl=
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp4138;}); _temp4137;}); _temp4136;}); break; case 388: _LL4135: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4140=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4140[ 0]=({ struct Cyc_Exp_tok_struct
_temp4141; _temp4141.tag= Cyc_Exp_tok; _temp4141.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1,(* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4141;}); _temp4140;}); break; case 389: _LL4139: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4143=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4143[ 0]=({ struct Cyc_Exp_tok_struct
_temp4144; _temp4144.tag= Cyc_Exp_tok; _temp4144.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4144;}); _temp4143;}); break; case 390: _LL4142: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4146=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4146[ 0]=({ struct Cyc_Exp_tok_struct
_temp4147; _temp4147.tag= Cyc_Exp_tok; _temp4147.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4147;}); _temp4146;}); break; case 391: _LL4145: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4149=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4149[ 0]=({ struct Cyc_Exp_tok_struct
_temp4150; _temp4150.tag= Cyc_Exp_tok; _temp4150.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4150;}); _temp4149;}); break; case 392: _LL4148: yyval=( void*)({ struct
Cyc_QualId_tok_struct* _temp4152=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp4152[ 0]=({ struct Cyc_QualId_tok_struct
_temp4153; _temp4153.tag= Cyc_QualId_tok; _temp4153.f1=({ struct _tuple1*
_temp4154=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4154->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp4156=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp4156[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp4157; _temp4157.tag= Cyc_Absyn_Rel_n; _temp4157.f1= 0; _temp4157;});
_temp4156;}); _temp4154->f2=({ struct _tagged_string* _temp4155=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4155[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp4155;}); _temp4154;}); _temp4153;}); _temp4152;}); break; case 393: _LL4151:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; default:
_LL4158:( void) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen;
yyssp_offset -= yylen; yylsp_offset -= yylen;*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),(
++ yyvsp_offset))= yyval; yylsp_offset ++; if( yylen == 0){(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line= Cyc_yylloc.first_line;(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_column=
Cyc_yylloc.first_column;(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line=(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line;(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_column=(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_column;(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).text=( struct _tagged_string)({ char* _temp4160=( char*)"";
struct _tagged_string _temp4161; _temp4161.curr= _temp4160; _temp4161.base=
_temp4160; _temp4161.last_plus_one= _temp4160 + 1; _temp4161;});} else{(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line=(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),( yylsp_offset +
yylen) - 1)).last_line;(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_column=(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),(
yylsp_offset + yylen) - 1)).last_column;} yyn=( int)(( short*) Cyc_yyr1)[
_check_known_subscript_notnull( 394u, yyn)]; yystate=(( short*) Cyc_yypgoto)[
_check_known_subscript_notnull( 108u, yyn - 124)] +*(( short*(*)( struct
_tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( yyss,
sizeof( short), yyssp_offset); if(( yystate >= 0? yystate <= 4222: 0)?(( short*)
Cyc_yycheck)[ _check_known_subscript_notnull( 4223u, yystate)] ==*(( short*(*)(
struct _tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)(
yyss, sizeof( short), yyssp_offset): 0){ yystate=( int)(( short*) Cyc_yytable)[
_check_known_subscript_notnull( 4223u, yystate)];} else{ yystate=( int)(( short*)
Cyc_yydefgoto)[ _check_known_subscript_notnull( 108u, yyn - 124)];} goto
yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)(( short*)
Cyc_yypact)[ _check_known_subscript_notnull( 751u, yystate)]; if( yyn > - 32768?
yyn < 4222: 0){ int sze= 0; struct _tagged_string msg; int x; int count; count=
0; for( x= yyn < 0? - yyn: 0; x < 232u / sizeof( char*); x ++){ if((( short*)
Cyc_yycheck)[ _check_known_subscript_notnull( 4223u, x + yyn)] == x){ sze += Cyc_String_strlen(((
struct _tagged_string*) Cyc_yytname)[ _check_known_subscript_notnull( 232u, x)])
+ 15, count ++;}} msg=({ unsigned int _temp4162=( unsigned int)( sze + 15); char*
_temp4163=( char*) GC_malloc_atomic( sizeof( char) * _temp4162); struct
_tagged_string _temp4166={ _temp4163, _temp4163, _temp4163 + _temp4162};{
unsigned int _temp4164= _temp4162; unsigned int i; for( i= 0; i < _temp4164; i
++){ _temp4163[ i]='\000';}}; _temp4166;}); Cyc_String_strcpy( msg,( struct
_tagged_string)({ char* _temp4167=( char*)"parse error"; struct _tagged_string
_temp4168; _temp4168.curr= _temp4167; _temp4168.base= _temp4167; _temp4168.last_plus_one=
_temp4167 + 12; _temp4168;})); if( count < 5){ count= 0; for( x= yyn < 0? - yyn:
0; x < 232u / sizeof( char*); x ++){ if((( short*) Cyc_yycheck)[
_check_known_subscript_notnull( 4223u, x + yyn)] == x){ Cyc_String_strcat( msg,
count == 0?( struct _tagged_string)({ char* _temp4169=( char*)", expecting `";
struct _tagged_string _temp4170; _temp4170.curr= _temp4169; _temp4170.base=
_temp4169; _temp4170.last_plus_one= _temp4169 + 14; _temp4170;}):( struct
_tagged_string)({ char* _temp4171=( char*)" or `"; struct _tagged_string
_temp4172; _temp4172.curr= _temp4171; _temp4172.base= _temp4171; _temp4172.last_plus_one=
_temp4171 + 6; _temp4172;})); Cyc_String_strcat( msg,(( struct _tagged_string*)
Cyc_yytname)[ _check_known_subscript_notnull( 232u, x)]); Cyc_String_strcat( msg,(
struct _tagged_string)({ char* _temp4173=( char*)"'"; struct _tagged_string
_temp4174; _temp4174.curr= _temp4173; _temp4174.base= _temp4173; _temp4174.last_plus_one=
_temp4173 + 2; _temp4174;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror((
struct _tagged_string)({ char* _temp4175=( char*)"parse error"; struct
_tagged_string _temp4176; _temp4176.curr= _temp4175; _temp4176.base= _temp4175;
_temp4176.last_plus_one= _temp4175 + 12; _temp4176;}));}} goto yyerrlab1;
yyerrlab1: if( yyerrstatus == 3){ if( Cyc_yychar == 0){ return 1;} Cyc_yychar= -
2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset
== 0){ return 1;} yyvsp_offset --; yystate=( int)*(( short*(*)( struct
_tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( yyss,
sizeof( short),( -- yyssp_offset)); yylsp_offset --; yyerrhandle: yyn=( int)((
short*) Cyc_yypact)[ _check_known_subscript_notnull( 751u, yystate)]; if( yyn ==
- 32768){ goto yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4222)? 1:(( short*)
Cyc_yycheck)[ _check_known_subscript_notnull( 4223u, yyn)] != 1){ goto
yyerrdefault;} yyn=( int)(( short*) Cyc_yytable)[ _check_known_subscript_notnull(
4223u, yyn)]; if( yyn < 0){ if( yyn == - 32768){ goto yyerrpop;} yyn= - yyn;
goto yyreduce;} else{ if( yyn == 0){ goto yyerrpop;}} if( yyn == 750){ return 0;}*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*),( ++ yyvsp_offset))= Cyc_yylval;*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),( ++ yylsp_offset))=
Cyc_yylloc; goto yynewstate;} void Cyc_yyprint( int i, void* v){ void* _temp4195=
v; struct _tuple15* _temp4215; struct _tuple15 _temp4217; int _temp4218; void*
_temp4220; char _temp4222; short _temp4224; struct _tagged_string _temp4226;
struct Cyc_Core_Opt* _temp4228; struct Cyc_Core_Opt* _temp4230; struct Cyc_Core_Opt
_temp4232; struct _tagged_string* _temp4233; struct _tuple1* _temp4235; struct
_tuple1 _temp4237; struct _tagged_string* _temp4238; void* _temp4240; _LL4197:
if( _temp4195 == Cyc_Okay_tok){ goto _LL4198;} else{ goto _LL4199;} _LL4199: if(*((
void**) _temp4195) == Cyc_Int_tok){ _LL4216: _temp4215=( struct _tuple15*)((
struct Cyc_Int_tok_struct*) _temp4195)->f1; _temp4217=* _temp4215; _LL4221:
_temp4220= _temp4217.f1; goto _LL4219; _LL4219: _temp4218= _temp4217.f2; goto
_LL4200;} else{ goto _LL4201;} _LL4201: if(*(( void**) _temp4195) == Cyc_Char_tok){
_LL4223: _temp4222=( char)(( struct Cyc_Char_tok_struct*) _temp4195)->f1; goto
_LL4202;} else{ goto _LL4203;} _LL4203: if(*(( void**) _temp4195) == Cyc_Short_tok){
_LL4225: _temp4224=( short)(( struct Cyc_Short_tok_struct*) _temp4195)->f1; goto
_LL4204;} else{ goto _LL4205;} _LL4205: if(*(( void**) _temp4195) == Cyc_String_tok){
_LL4227: _temp4226=( struct _tagged_string)(( struct Cyc_String_tok_struct*)
_temp4195)->f1; goto _LL4206;} else{ goto _LL4207;} _LL4207: if(*(( void**)
_temp4195) == Cyc_Stringopt_tok){ _LL4229: _temp4228=( struct Cyc_Core_Opt*)((
struct Cyc_Stringopt_tok_struct*) _temp4195)->f1; if( _temp4228 == 0){ goto
_LL4208;} else{ goto _LL4209;}} else{ goto _LL4209;} _LL4209: if(*(( void**)
_temp4195) == Cyc_Stringopt_tok){ _LL4231: _temp4230=( struct Cyc_Core_Opt*)((
struct Cyc_Stringopt_tok_struct*) _temp4195)->f1; if( _temp4230 == 0){ goto
_LL4211;} else{ _temp4232=* _temp4230; _LL4234: _temp4233=( struct
_tagged_string*) _temp4232.v; goto _LL4210;}} else{ goto _LL4211;} _LL4211: if(*((
void**) _temp4195) == Cyc_QualId_tok){ _LL4236: _temp4235=( struct _tuple1*)((
struct Cyc_QualId_tok_struct*) _temp4195)->f1; _temp4237=* _temp4235; _LL4241:
_temp4240= _temp4237.f1; goto _LL4239; _LL4239: _temp4238= _temp4237.f2; goto
_LL4212;} else{ goto _LL4213;} _LL4213: goto _LL4214; _LL4198: fprintf( Cyc_Stdio_stderr,"ok");
goto _LL4196; _LL4200: fprintf( Cyc_Stdio_stderr,"%d", _temp4218); goto _LL4196;
_LL4202: fprintf( Cyc_Stdio_stderr,"%c",( int) _temp4222); goto _LL4196; _LL4204:
fprintf( Cyc_Stdio_stderr,"%ds",( int) _temp4224); goto _LL4196; _LL4206:({
struct _tagged_string _temp4242= _temp4226; fprintf( Cyc_Stdio_stderr,"\"%.*s\"",
_temp4242.last_plus_one - _temp4242.curr, _temp4242.curr);}); goto _LL4196;
_LL4208: fprintf( Cyc_Stdio_stderr,"null"); goto _LL4196; _LL4210:({ struct
_tagged_string _temp4243=* _temp4233; fprintf( Cyc_Stdio_stderr,"\"%.*s\"",
_temp4243.last_plus_one - _temp4243.curr, _temp4243.curr);}); goto _LL4196;
_LL4212: { struct Cyc_List_List* _temp4244= 0; goto _LL4245; _LL4245:{ void*
_temp4246= _temp4240; struct Cyc_List_List* _temp4254; struct Cyc_List_List*
_temp4256; _LL4248: if(( unsigned int) _temp4246 > 1u?*(( int*) _temp4246) ==
Cyc_Absyn_Rel_n: 0){ _LL4255: _temp4254=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp4246)->f1; goto _LL4249;} else{ goto _LL4250;} _LL4250: if(( unsigned int)
_temp4246 > 1u?*(( int*) _temp4246) == Cyc_Absyn_Abs_n: 0){ _LL4257: _temp4256=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp4246)->f1; goto
_LL4251;} else{ goto _LL4252;} _LL4252: if( _temp4246 ==( void*) Cyc_Absyn_Loc_n){
goto _LL4253;} else{ goto _LL4247;} _LL4249: _temp4244= _temp4254; goto _LL4247;
_LL4251: _temp4244= _temp4256; goto _LL4247; _LL4253: goto _LL4247; _LL4247:;}
for( 0; _temp4244 != 0; _temp4244=(( struct Cyc_List_List*) _check_null(
_temp4244))->tl){({ struct _tagged_string _temp4258=*(( struct _tagged_string*)((
struct Cyc_List_List*) _check_null( _temp4244))->hd); fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp4258.last_plus_one - _temp4258.curr, _temp4258.curr);});}({ struct
_tagged_string _temp4259=* _temp4238; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp4259.last_plus_one - _temp4259.curr, _temp4259.curr);}); goto _LL4196;}
_LL4214: fprintf( Cyc_Stdio_stderr,"?"); goto _LL4196; _LL4196:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp4260=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4260->v=( void*) Cyc_Lexing_from_file(
f); _temp4260;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}