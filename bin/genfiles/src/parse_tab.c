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
extern struct _tagged_string Cyc_String_substring( struct _tagged_string, int
ofs, unsigned int n); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent[ 11u]; struct Cyc_Lineno_Pos{ struct _tagged_string
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
Cyc_List_List* Cyc_Parse_parse_result= 0; struct Cyc_Absyn_Aligned_att_struct
Cyc_Parse_att_aligned={ 1u, - 1}; struct _tuple8{ struct _tagged_string f1; void*
f2; } ; static char _temp3[ 8u]="stdcall"; static char _temp6[ 6u]="cdecl";
static char _temp9[ 9u]="noreturn"; static char _temp12[ 6u]="const"; static
char _temp15[ 8u]="aligned"; static char _temp18[ 7u]="packed"; static char
_temp21[ 7u]="shared"; static char _temp24[ 7u]="unused"; static char _temp27[ 5u]="weak";
static char _temp30[ 10u]="dllimport"; static char _temp33[ 10u]="dllexport";
static char _temp36[ 23u]="no_instrument_function"; static char _temp39[ 12u]="constructor";
static char _temp42[ 11u]="destructor"; static char _temp45[ 22u]="no_check_memory_usage";
struct _tuple8 Cyc_Parse_att_map[ 15u]={( struct _tuple8){.f1=( struct
_tagged_string){ _temp3, _temp3, _temp3 + 8u},.f2=( void*) 0u},( struct _tuple8){.f1=(
struct _tagged_string){ _temp6, _temp6, _temp6 + 6u},.f2=( void*) 1u},( struct
_tuple8){.f1=( struct _tagged_string){ _temp9, _temp9, _temp9 + 9u},.f2=( void*)
2u},( struct _tuple8){.f1=( struct _tagged_string){ _temp12, _temp12, _temp12 +
6u},.f2=( void*) 3u},( struct _tuple8){.f1=( struct _tagged_string){ _temp15,
_temp15, _temp15 + 8u},.f2=( void*)& Cyc_Parse_att_aligned},( struct _tuple8){.f1=(
struct _tagged_string){ _temp18, _temp18, _temp18 + 7u},.f2=( void*) 4u},(
struct _tuple8){.f1=( struct _tagged_string){ _temp21, _temp21, _temp21 + 7u},.f2=(
void*) 6u},( struct _tuple8){.f1=( struct _tagged_string){ _temp24, _temp24,
_temp24 + 7u},.f2=( void*) 7u},( struct _tuple8){.f1=( struct _tagged_string){
_temp27, _temp27, _temp27 + 5u},.f2=( void*) 8u},( struct _tuple8){.f1=( struct
_tagged_string){ _temp30, _temp30, _temp30 + 10u},.f2=( void*) 9u},( struct
_tuple8){.f1=( struct _tagged_string){ _temp33, _temp33, _temp33 + 10u},.f2=(
void*) 10u},( struct _tuple8){.f1=( struct _tagged_string){ _temp36, _temp36,
_temp36 + 23u},.f2=( void*) 11u},( struct _tuple8){.f1=( struct _tagged_string){
_temp39, _temp39, _temp39 + 12u},.f2=( void*) 12u},( struct _tuple8){.f1=(
struct _tagged_string){ _temp42, _temp42, _temp42 + 11u},.f2=( void*) 13u},(
struct _tuple8){.f1=( struct _tagged_string){ _temp45, _temp45, _temp45 + 22u},.f2=(
void*) 14u}}; static void Cyc_Parse_err( struct _tagged_string msg, struct Cyc_Position_Segment*
sg){ Cyc_Position_post_error( Cyc_Position_mk_err_parse( sg, msg));} static void*
Cyc_Parse_abort( struct _tagged_string msg, struct Cyc_Position_Segment* sg){
Cyc_Parse_err( msg, sg);( void) _throw(( void*) Cyc_Position_Exit);} static void
Cyc_Parse_warn( struct _tagged_string msg, struct Cyc_Position_Segment* sg){({
struct _tagged_string _temp46= Cyc_Position_string_of_segment( sg); struct
_tagged_string _temp47= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: %.*s\n",
_temp46.last_plus_one - _temp46.curr, _temp46.curr, _temp47.last_plus_one -
_temp47.curr, _temp47.curr);}); return;} static void* Cyc_Parse_unimp( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){ return(( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct
_tagged_string _temp48= msg; xprintf("%.*s unimplemented", _temp48.last_plus_one
- _temp48.curr, _temp48.curr);}), sg);} static void Cyc_Parse_unimp2( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){({ struct _tagged_string
_temp49= Cyc_Position_string_of_segment( sg); struct _tagged_string _temp50= msg;
fprintf( Cyc_Stdio_stderr,"%.*s: Warning: Cyclone does not yet support %.*s\n",
_temp49.last_plus_one - _temp49.curr, _temp49.curr, _temp50.last_plus_one -
_temp50.curr, _temp50.curr);}); return;} struct _tuple9{ struct _tuple7* f1;
struct Cyc_Core_Opt* f2; } ; static struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field(
struct Cyc_Position_Segment* loc, struct _tuple9* field_info){ struct Cyc_Core_Opt*
_temp53; struct _tuple7* _temp55; struct _tuple9 _temp51=* field_info; _LL56:
_temp55= _temp51.f1; goto _LL54; _LL54: _temp53= _temp51.f2; goto _LL52; _LL52:
if((* _temp55).f4 != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp57=(
char*)"bad type params in struct field"; struct _tagged_string _temp58; _temp58.curr=
_temp57; _temp58.base= _temp57; _temp58.last_plus_one= _temp57 + 32; _temp58;}),
loc);}{ struct _tuple1* _temp59=(* _temp55).f1; goto _LL60; _LL60:{ void*
_temp61=(* _temp59).f1; struct Cyc_List_List* _temp71; struct Cyc_List_List*
_temp73; _LL63: if(( unsigned int) _temp61 > 1u?*(( int*) _temp61) == Cyc_Absyn_Rel_n:
0){ _LL72: _temp71=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp61)->f1; if( _temp71 == 0){ goto _LL64;} else{ goto _LL65;}} else{ goto
_LL65;} _LL65: if(( unsigned int) _temp61 > 1u?*(( int*) _temp61) == Cyc_Absyn_Abs_n:
0){ _LL74: _temp73=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp61)->f1; if( _temp73 == 0){ goto _LL66;} else{ goto _LL67;}} else{ goto
_LL67;} _LL67: if( _temp61 ==( void*) Cyc_Absyn_Loc_n){ goto _LL68;} else{ goto
_LL69;} _LL69: goto _LL70; _LL64: goto _LL62; _LL66: goto _LL62; _LL68: goto
_LL62; _LL70: Cyc_Parse_err(( struct _tagged_string)({ char* _temp75=( char*)"struct field cannot be qualified with a module name";
struct _tagged_string _temp76; _temp76.curr= _temp75; _temp76.base= _temp75;
_temp76.last_plus_one= _temp75 + 52; _temp76;}), loc); goto _LL62; _LL62:;}{
struct Cyc_List_List* _temp77=(* _temp55).f5; goto _LL78; _LL78: return({ struct
Cyc_Absyn_Structfield* _temp79=( struct Cyc_Absyn_Structfield*) GC_malloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp79->name=(* _temp59).f2; _temp79->tq=(*
_temp55).f2; _temp79->type=( void*)(* _temp55).f3; _temp79->width= _temp53;
_temp79->attributes= _temp77; _temp79;});}}} struct _tuple10{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; struct Cyc_List_List* f4; } ; static
struct _tuple2* Cyc_Parse_make_param( struct Cyc_Position_Segment* loc, struct
_tuple10* field){ struct _tuple10 _temp82; struct Cyc_List_List* _temp83; void*
_temp85; struct Cyc_Absyn_Tqual _temp87; struct Cyc_Core_Opt* _temp89; struct
_tuple10* _temp80= field; _temp82=* _temp80; _LL90: _temp89= _temp82.f1; goto
_LL88; _LL88: _temp87= _temp82.f2; goto _LL86; _LL86: _temp85= _temp82.f3; goto
_LL84; _LL84: _temp83= _temp82.f4; goto _LL81; _LL81: { struct Cyc_Core_Opt*
_temp91= 0; goto _LL92; _LL92: if( _temp89 != 0){ _temp91=({ struct Cyc_Core_Opt*
_temp93=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp93->v=( void*)(*(( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null(
_temp89))->v)).f2; _temp93;});{ void* _temp94=(*(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp89))->v)).f1; struct Cyc_List_List* _temp104; struct Cyc_List_List*
_temp106; _LL96: if(( unsigned int) _temp94 > 1u?*(( int*) _temp94) == Cyc_Absyn_Rel_n:
0){ _LL105: _temp104=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp94)->f1; if( _temp104 == 0){ goto _LL97;} else{ goto _LL98;}} else{ goto
_LL98;} _LL98: if(( unsigned int) _temp94 > 1u?*(( int*) _temp94) == Cyc_Absyn_Abs_n:
0){ _LL107: _temp106=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp94)->f1; if( _temp106 == 0){ goto _LL99;} else{ goto _LL100;}} else{ goto
_LL100;} _LL100: if( _temp94 ==( void*) Cyc_Absyn_Loc_n){ goto _LL101;} else{
goto _LL102;} _LL102: goto _LL103; _LL97: goto _LL95; _LL99: goto _LL95; _LL101:
goto _LL95; _LL103: Cyc_Parse_err(( struct _tagged_string)({ char* _temp108=(
char*)"parameter cannot be qualified with a module name"; struct _tagged_string
_temp109; _temp109.curr= _temp108; _temp109.base= _temp108; _temp109.last_plus_one=
_temp108 + 49; _temp109;}), loc); goto _LL95; _LL95:;}} if( _temp83 != 0){((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp110=( char*)"parameter should have no type parameters";
struct _tagged_string _temp111; _temp111.curr= _temp110; _temp111.base= _temp110;
_temp111.last_plus_one= _temp110 + 41; _temp111;}), loc);} return({ struct
_tuple2* _temp112=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp112->f1= _temp91; _temp112->f2= _temp87; _temp112->f3= _temp85; _temp112;});}}
static void* Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){
return( void*)({ struct Cyc_Parse_Type_spec_struct* _temp113=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp113[ 0]=({ struct
Cyc_Parse_Type_spec_struct _temp114; _temp114.tag= Cyc_Parse_Type_spec; _temp114.f1=(
void*) t; _temp114.f2= loc; _temp114;}); _temp113;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp115= t; struct Cyc_Absyn_Exp* _temp121; struct Cyc_Absyn_Tqual
_temp123; void* _temp125; _LL117: if(( unsigned int) _temp115 > 4u?*(( int*)
_temp115) == Cyc_Absyn_ArrayType: 0){ _LL126: _temp125=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp115)->f1; goto _LL124; _LL124: _temp123=( struct Cyc_Absyn_Tqual)(( struct
Cyc_Absyn_ArrayType_struct*) _temp115)->f2; goto _LL122; _LL122: _temp121=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*) _temp115)->f3; goto
_LL118;} else{ goto _LL119;} _LL119: goto _LL120; _LL118: return Cyc_Absyn_starb_typ(
_temp125,( void*) Cyc_Absyn_HeapRgn, _temp123, _temp121 == 0?( void*) Cyc_Absyn_Unknown_b:(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp127=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp127[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp128; _temp128.tag= Cyc_Absyn_Upper_b; _temp128.f1=( struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*) _check_null( _temp121)); _temp128;}); _temp127;}));
_LL120: return t; _LL116:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2*
x){(* x).f3= Cyc_Parse_array2ptr((* x).f3);} struct _tuple11{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct _tuple11* Cyc_Parse_get_tqual_typ( struct Cyc_Position_Segment*
loc, struct _tuple2* t){ return({ struct _tuple11* _temp129=( struct _tuple11*)
GC_malloc( sizeof( struct _tuple11)); _temp129->f1=(* t).f2; _temp129->f2=(* t).f3;
_temp129;});} static void Cyc_Parse_only_vardecl( struct Cyc_List_List* params,
struct Cyc_Absyn_Decl* x){ struct _tagged_string decl_kind;{ void* _temp130=(
void*) x->r; struct Cyc_Absyn_Vardecl* _temp154; int _temp156; struct Cyc_Absyn_Exp*
_temp158; struct Cyc_Core_Opt* _temp160; struct Cyc_Core_Opt* _temp162; struct
Cyc_Absyn_Pat* _temp164; struct Cyc_Absyn_Fndecl* _temp166; struct Cyc_Absyn_Structdecl*
_temp168; struct Cyc_Absyn_Uniondecl* _temp170; struct Cyc_Absyn_Tuniondecl*
_temp172; struct Cyc_Absyn_Typedefdecl* _temp174; struct Cyc_Absyn_Enumdecl*
_temp176; struct Cyc_List_List* _temp178; struct _tagged_string* _temp180;
struct Cyc_List_List* _temp182; struct _tuple1* _temp184; struct Cyc_List_List*
_temp186; _LL132: if(*(( int*) _temp130) == Cyc_Absyn_Var_d){ _LL155: _temp154=(
struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*) _temp130)->f1; goto
_LL133;} else{ goto _LL134;} _LL134: if(*(( int*) _temp130) == Cyc_Absyn_Let_d){
_LL165: _temp164=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp130)->f1; goto _LL163; _LL163: _temp162=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_Let_d_struct*) _temp130)->f2; goto _LL161; _LL161: _temp160=( struct
Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*) _temp130)->f3; goto _LL159;
_LL159: _temp158=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp130)->f4; goto _LL157; _LL157: _temp156=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp130)->f5; goto _LL135;} else{ goto _LL136;} _LL136: if(*(( int*) _temp130)
== Cyc_Absyn_Fn_d){ _LL167: _temp166=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*)
_temp130)->f1; goto _LL137;} else{ goto _LL138;} _LL138: if(*(( int*) _temp130)
== Cyc_Absyn_Struct_d){ _LL169: _temp168=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp130)->f1; goto _LL139;} else{ goto
_LL140;} _LL140: if(*(( int*) _temp130) == Cyc_Absyn_Union_d){ _LL171: _temp170=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp130)->f1;
goto _LL141;} else{ goto _LL142;} _LL142: if(*(( int*) _temp130) == Cyc_Absyn_Tunion_d){
_LL173: _temp172=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp130)->f1; goto _LL143;} else{ goto _LL144;} _LL144: if(*(( int*) _temp130)
== Cyc_Absyn_Typedef_d){ _LL175: _temp174=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp130)->f1; goto _LL145;} else{ goto
_LL146;} _LL146: if(*(( int*) _temp130) == Cyc_Absyn_Enum_d){ _LL177: _temp176=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp130)->f1;
goto _LL147;} else{ goto _LL148;} _LL148: if(*(( int*) _temp130) == Cyc_Absyn_Namespace_d){
_LL181: _temp180=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp130)->f1; goto _LL179; _LL179: _temp178=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp130)->f2; goto _LL149;} else{ goto _LL150;}
_LL150: if(*(( int*) _temp130) == Cyc_Absyn_Using_d){ _LL185: _temp184=( struct
_tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp130)->f1; goto _LL183; _LL183:
_temp182=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp130)->f2;
goto _LL151;} else{ goto _LL152;} _LL152: if(*(( int*) _temp130) == Cyc_Absyn_ExternC_d){
_LL187: _temp186=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp130)->f1; goto _LL153;} else{ goto _LL131;} _LL133: if( _temp154->initializer
!= 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg))
Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp188=( char*)"initializers are not allowed in parameter declarations";
struct _tagged_string _temp189; _temp189.curr= _temp188; _temp189.base= _temp188;
_temp189.last_plus_one= _temp188 + 55; _temp189;}), x->loc);}{ void* _temp190=(*
_temp154->name).f1; struct Cyc_List_List* _temp200; struct Cyc_List_List*
_temp202; _LL192: if( _temp190 ==( void*) Cyc_Absyn_Loc_n){ goto _LL193;} else{
goto _LL194;} _LL194: if(( unsigned int) _temp190 > 1u?*(( int*) _temp190) ==
Cyc_Absyn_Rel_n: 0){ _LL201: _temp200=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp190)->f1; if( _temp200 == 0){ goto _LL195;} else{ goto _LL196;}} else{ goto
_LL196;} _LL196: if(( unsigned int) _temp190 > 1u?*(( int*) _temp190) == Cyc_Absyn_Abs_n:
0){ _LL203: _temp202=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp190)->f1; if( _temp202 == 0){ goto _LL197;} else{ goto _LL198;}} else{ goto
_LL198;} _LL198: goto _LL199; _LL193: goto _LL191; _LL195: goto _LL191; _LL197:
goto _LL191; _LL199: Cyc_Parse_err(( struct _tagged_string)({ char* _temp204=(
char*)"module names not allowed on parameter declarations"; struct
_tagged_string _temp205; _temp205.curr= _temp204; _temp205.base= _temp204;
_temp205.last_plus_one= _temp204 + 51; _temp205;}), x->loc); goto _LL191; _LL191:;}{
int found= 0; for( 0; params != 0; params=(( struct Cyc_List_List*) _check_null(
params))->tl){ if( Cyc_String_zstrptrcmp((* _temp154->name).f2,( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( params))->hd) == 0){
found= 1; break;}} if( ! found){(( void(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp206=*(*
_temp154->name).f2; xprintf("%.*s is not listed as a parameter", _temp206.last_plus_one
- _temp206.curr, _temp206.curr);}), x->loc);} return;} _LL135: decl_kind=(
struct _tagged_string)({ char* _temp207=( char*)"let declaration"; struct
_tagged_string _temp208; _temp208.curr= _temp207; _temp208.base= _temp207;
_temp208.last_plus_one= _temp207 + 16; _temp208;}); goto _LL131; _LL137:
decl_kind=( struct _tagged_string)({ char* _temp209=( char*)"function declaration";
struct _tagged_string _temp210; _temp210.curr= _temp209; _temp210.base= _temp209;
_temp210.last_plus_one= _temp209 + 21; _temp210;}); goto _LL131; _LL139:
decl_kind=( struct _tagged_string)({ char* _temp211=( char*)"struct declaration";
struct _tagged_string _temp212; _temp212.curr= _temp211; _temp212.base= _temp211;
_temp212.last_plus_one= _temp211 + 19; _temp212;}); goto _LL131; _LL141:
decl_kind=( struct _tagged_string)({ char* _temp213=( char*)"union declaration";
struct _tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 18; _temp214;}); goto _LL131; _LL143:
decl_kind=( struct _tagged_string)({ char* _temp215=( char*)"tunion declaration";
struct _tagged_string _temp216; _temp216.curr= _temp215; _temp216.base= _temp215;
_temp216.last_plus_one= _temp215 + 19; _temp216;}); goto _LL131; _LL145:
decl_kind=( struct _tagged_string)({ char* _temp217=( char*)"typedef"; struct
_tagged_string _temp218; _temp218.curr= _temp217; _temp218.base= _temp217;
_temp218.last_plus_one= _temp217 + 8; _temp218;}); goto _LL131; _LL147:
decl_kind=( struct _tagged_string)({ char* _temp219=( char*)"enum declaration";
struct _tagged_string _temp220; _temp220.curr= _temp219; _temp220.base= _temp219;
_temp220.last_plus_one= _temp219 + 17; _temp220;}); goto _LL131; _LL149:
decl_kind=( struct _tagged_string)({ char* _temp221=( char*)"namespace declaration";
struct _tagged_string _temp222; _temp222.curr= _temp221; _temp222.base= _temp221;
_temp222.last_plus_one= _temp221 + 22; _temp222;}); goto _LL131; _LL151:
decl_kind=( struct _tagged_string)({ char* _temp223=( char*)"using declaration";
struct _tagged_string _temp224; _temp224.curr= _temp223; _temp224.base= _temp223;
_temp224.last_plus_one= _temp223 + 18; _temp224;}); goto _LL131; _LL153:
decl_kind=( struct _tagged_string)({ char* _temp225=( char*)"extern C declaration";
struct _tagged_string _temp226; _temp226.curr= _temp225; _temp226.base= _temp225;
_temp226.last_plus_one= _temp225 + 21; _temp226;}); goto _LL131; _LL131:;}((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp227= decl_kind; xprintf("%.*s appears in parameter type",
_temp227.last_plus_one - _temp227.curr, _temp227.curr);}), x->loc); return;}
struct _tuple12{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ;
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple12* env, struct
_tagged_string* x){ struct _tuple12 _temp230; struct Cyc_Position_Segment*
_temp231; struct Cyc_List_List* _temp233; struct _tuple12* _temp228= env;
_temp230=* _temp228; _LL234: _temp233= _temp230.f1; goto _LL232; _LL232:
_temp231= _temp230.f2; goto _LL229; _LL229: if( _temp233 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp235=* x; xprintf("missing type for parameter %.*s",
_temp235.last_plus_one - _temp235.curr, _temp235.curr);}), _temp231);}{ void*
_temp236=( void*)(( struct Cyc_Absyn_Decl*)(( struct Cyc_List_List*) _check_null(
_temp233))->hd)->r; struct Cyc_Absyn_Vardecl* _temp242; _LL238: if(*(( int*)
_temp236) == Cyc_Absyn_Var_d){ _LL243: _temp242=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp236)->f1; goto _LL239;} else{ goto _LL240;}
_LL240: goto _LL241; _LL239:{ void* _temp244=(* _temp242->name).f1; struct Cyc_List_List*
_temp254; struct Cyc_List_List* _temp256; _LL246: if( _temp244 ==( void*) Cyc_Absyn_Loc_n){
goto _LL247;} else{ goto _LL248;} _LL248: if(( unsigned int) _temp244 > 1u?*((
int*) _temp244) == Cyc_Absyn_Rel_n: 0){ _LL255: _temp254=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp244)->f1; if( _temp254 == 0){ goto _LL249;}
else{ goto _LL250;}} else{ goto _LL250;} _LL250: if(( unsigned int) _temp244 > 1u?*((
int*) _temp244) == Cyc_Absyn_Abs_n: 0){ _LL257: _temp256=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp244)->f1; if( _temp256 == 0){ goto _LL251;}
else{ goto _LL252;}} else{ goto _LL252;} _LL252: goto _LL253; _LL247: goto
_LL245; _LL249: goto _LL245; _LL251: goto _LL245; _LL253: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp258=( char*)"module name not allowed on parameter";
struct _tagged_string _temp259; _temp259.curr= _temp258; _temp259.base= _temp258;
_temp259.last_plus_one= _temp258 + 37; _temp259;}), _temp231); goto _LL245;
_LL245:;} if( Cyc_String_zstrptrcmp((* _temp242->name).f2, x) == 0){ return({
struct _tuple2* _temp260=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp260[ 0]=({ struct _tuple2 _temp261; _temp261.f1=({ struct Cyc_Core_Opt*
_temp262=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp262[ 0]=({ struct Cyc_Core_Opt _temp263; _temp263.v=( void*)(* _temp242->name).f2;
_temp263;}); _temp262;}); _temp261.f2= _temp242->tq; _temp261.f3=( void*)
_temp242->type; _temp261;}); _temp260;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple12* _temp264=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12));
_temp264->f1=(( struct Cyc_List_List*) _check_null( _temp233))->tl; _temp264->f2=
_temp231; _temp264;}), x);} _LL241: return(( struct _tuple2*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp265=( char*)"non-variable declaration"; struct
_tagged_string _temp266; _temp266.curr= _temp265; _temp266.base= _temp265;
_temp266.last_plus_one= _temp265 + 25; _temp266;}),(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( _temp233))->hd)->loc); _LL237:;}} static int
Cyc_Parse_is_typeparam( void* tm){ void* _temp267= tm; int _temp273; struct Cyc_Position_Segment*
_temp275; struct Cyc_List_List* _temp277; _LL269: if(( unsigned int) _temp267 >
1u?*(( int*) _temp267) == Cyc_Absyn_TypeParams_mod: 0){ _LL278: _temp277=(
struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp267)->f1;
goto _LL276; _LL276: _temp275=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp267)->f2; goto _LL274; _LL274: _temp273=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp267)->f3; goto _LL270;} else{ goto _LL271;} _LL271: goto _LL272; _LL270:
return 1; _LL272: return 0; _LL268:;} static void* Cyc_Parse_id2type( struct
_tagged_string s, struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp279=( char*)"`H"; struct _tagged_string
_temp280; _temp280.curr= _temp279; _temp280.base= _temp279; _temp280.last_plus_one=
_temp279 + 3; _temp280;})) == 0){ return( void*) Cyc_Absyn_HeapRgn;} else{
return( void*)({ struct Cyc_Absyn_VarType_struct* _temp281=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp281[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp282; _temp282.tag= Cyc_Absyn_VarType; _temp282.f1=({ struct Cyc_Absyn_Tvar*
_temp283=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp283->name=({ struct _tagged_string* _temp284=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp284[ 0]= s; _temp284;});
_temp283->kind= k; _temp283;}); _temp282;}); _temp281;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp285=
t; struct Cyc_Absyn_Tvar* _temp291; _LL287: if(( unsigned int) _temp285 > 4u?*((
int*) _temp285) == Cyc_Absyn_VarType: 0){ _LL292: _temp291=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp285)->f1; goto _LL288;} else{ goto _LL289;}
_LL289: goto _LL290; _LL288: return _temp291; _LL290: return(( struct Cyc_Absyn_Tvar*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp293=( char*)"expecting a list of type variables, not types";
struct _tagged_string _temp294; _temp294.curr= _temp293; _temp294.base= _temp293;
_temp294.last_plus_one= _temp293 + 46; _temp294;}), loc); _LL286:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return( void*)({ struct Cyc_Absyn_VarType_struct*
_temp295=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp295[ 0]=({ struct Cyc_Absyn_VarType_struct _temp296; _temp296.tag= Cyc_Absyn_VarType;
_temp296.f1= pr; _temp296;}); _temp295;});} static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List* tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment*
loc){ if( tds == 0){ return tms;} if( tms == 0){ return 0;}{ void* _temp297=(
void*)(( struct Cyc_List_List*) _check_null( tms))->hd; void* _temp303; _LL299:
if(( unsigned int) _temp297 > 1u?*(( int*) _temp297) == Cyc_Absyn_Function_mod:
0){ _LL304: _temp303=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp297)->f1;
goto _LL300;} else{ goto _LL301;} _LL301: goto _LL302; _LL300: if((( struct Cyc_List_List*)
_check_null( tms))->tl == 0? 1:( Cyc_Parse_is_typeparam(( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd)?(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->tl
== 0: 0)){ void* _temp305= _temp303; struct Cyc_Core_Opt* _temp311; int _temp313;
struct Cyc_List_List* _temp315; struct Cyc_Position_Segment* _temp317; struct
Cyc_List_List* _temp319; _LL307: if(*(( int*) _temp305) == Cyc_Absyn_WithTypes){
_LL316: _temp315=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp305)->f1; goto _LL314; _LL314: _temp313=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp305)->f2; goto _LL312; _LL312: _temp311=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp305)->f3; goto _LL308;} else{ goto _LL309;}
_LL309: if(*(( int*) _temp305) == Cyc_Absyn_NoTypes){ _LL320: _temp319=( struct
Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp305)->f1; goto _LL318;
_LL318: _temp317=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp305)->f2; goto _LL310;} else{ goto _LL306;} _LL308: Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp321=( char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp322; _temp322.curr= _temp321; _temp322.base= _temp321;
_temp322.last_plus_one= _temp321 + 92; _temp322;}), loc); return tms; _LL310:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp319, tds);{ struct _tuple12* _temp324=({ struct _tuple12* _temp323=( struct
_tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp323->f1= tds; _temp323->f2=
loc; _temp323;}); goto _LL325; _LL325: return({ struct Cyc_List_List* _temp326=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp326->hd=(
void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp327=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp327[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp328; _temp328.tag= Cyc_Absyn_Function_mod;
_temp328.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp329=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp329[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp330; _temp330.tag= Cyc_Absyn_WithTypes;
_temp330.f1=(( struct Cyc_List_List*(*)( struct _tuple2*(* f)( struct _tuple12*,
struct _tagged_string*), struct _tuple12* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_param_type, _temp324, _temp319); _temp330.f2= 0; _temp330.f3= 0;
_temp330;}); _temp329;})); _temp328;}); _temp327;})); _temp326->tl= 0; _temp326;});}
_LL306:;} else{ return({ struct Cyc_List_List* _temp331=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp331->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( tms))->hd); _temp331->tl= Cyc_Parse_oldstyle2newstyle(((
struct Cyc_List_List*) _check_null( tms))->tl, tds, loc); _temp331;});} _LL302:
return({ struct Cyc_List_List* _temp332=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp332->hd=( void*)(( void*)(( struct Cyc_List_List*)
_check_null( tms))->hd); _temp332->tl= Cyc_Parse_oldstyle2newstyle((( struct Cyc_List_List*)
_check_null( tms))->tl, tds, loc); _temp332;}); _LL298:;}} static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function( struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator*
d, struct Cyc_List_List* tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment*
loc){ if( tds != 0){ d=({ struct Cyc_Parse_Declarator* _temp333=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp333->id= d->id; _temp333->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp333;});}{ void* sc=( void*)
Cyc_Absyn_Public; struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null( dso))->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*) _check_null(
dso))->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_Core_Opt*)
_check_null( dso))->v)->is_inline; atts=(( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->attributes; if((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc != 0){ void* _temp334=( void*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Parse_Declaration_spec*)((
struct Cyc_Core_Opt*) _check_null( dso))->v)->sc))->v; _LL336: if( _temp334 ==(
void*) Cyc_Parse_Extern_sc){ goto _LL337;} else{ goto _LL338;} _LL338: if(
_temp334 ==( void*) Cyc_Parse_ExternC_sc){ goto _LL339;} else{ goto _LL340;}
_LL340: if( _temp334 ==( void*) Cyc_Parse_Static_sc){ goto _LL341;} else{ goto
_LL342;} _LL342: goto _LL343; _LL337: sc=( void*) Cyc_Absyn_Extern; goto _LL335;
_LL339: sc=( void*) Cyc_Absyn_ExternC; goto _LL335; _LL341: sc=( void*) Cyc_Absyn_Static;
goto _LL335; _LL343: Cyc_Parse_err(( struct _tagged_string)({ char* _temp344=(
char*)"bad storage class on function"; struct _tagged_string _temp345; _temp345.curr=
_temp344; _temp345.base= _temp344; _temp345.last_plus_one= _temp344 + 30;
_temp345;}), loc); goto _LL335; _LL335:;}}{ struct Cyc_Core_Opt* _temp348; void*
_temp350; struct _tuple5 _temp346= Cyc_Parse_collapse_type_specifiers( tss, loc);
_LL351: _temp350= _temp346.f1; goto _LL349; _LL349: _temp348= _temp346.f2; goto
_LL347; _LL347: { struct Cyc_List_List* _temp354; struct Cyc_List_List* _temp356;
void* _temp358; struct Cyc_Absyn_Tqual _temp360; struct _tuple6 _temp352= Cyc_Parse_apply_tms(
tq, _temp350, atts, d->tms); _LL361: _temp360= _temp352.f1; goto _LL359; _LL359:
_temp358= _temp352.f2; goto _LL357; _LL357: _temp356= _temp352.f3; goto _LL355;
_LL355: _temp354= _temp352.f4; goto _LL353; _LL353: if( _temp348 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp362=( char*)"nested type declaration within function prototype";
struct _tagged_string _temp363; _temp363.curr= _temp362; _temp363.base= _temp362;
_temp363.last_plus_one= _temp362 + 50; _temp363;}), loc);} if( _temp356 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp364=( char*)"bad type params, ignoring";
struct _tagged_string _temp365; _temp365.curr= _temp364; _temp365.base= _temp364;
_temp365.last_plus_one= _temp364 + 26; _temp365;}), loc);}{ void* _temp366=
_temp358; struct Cyc_Absyn_FnInfo _temp372; struct Cyc_List_List* _temp374; int
_temp376; struct Cyc_List_List* _temp378; void* _temp380; struct Cyc_Core_Opt*
_temp382; struct Cyc_List_List* _temp384; _LL368: if(( unsigned int) _temp366 >
4u?*(( int*) _temp366) == Cyc_Absyn_FnType: 0){ _LL373: _temp372=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp366)->f1; _LL385: _temp384=( struct Cyc_List_List*)
_temp372.tvars; goto _LL383; _LL383: _temp382=( struct Cyc_Core_Opt*) _temp372.effect;
goto _LL381; _LL381: _temp380=( void*) _temp372.ret_typ; goto _LL379; _LL379:
_temp378=( struct Cyc_List_List*) _temp372.args; goto _LL377; _LL377: _temp376=(
int) _temp372.varargs; goto _LL375; _LL375: _temp374=( struct Cyc_List_List*)
_temp372.attributes; goto _LL369;} else{ goto _LL370;} _LL370: goto _LL371;
_LL369: { struct Cyc_List_List* _temp386=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc,
_temp378); goto _LL387; _LL387: return({ struct Cyc_Absyn_Fndecl* _temp388=(
struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl)); _temp388->sc=(
void*) sc; _temp388->name= d->id; _temp388->tvs= _temp384; _temp388->is_inline=
is_inline; _temp388->effect= _temp382; _temp388->ret_type=( void*) _temp380;
_temp388->args= _temp386; _temp388->varargs= _temp376; _temp388->body= body;
_temp388->cached_typ= 0; _temp388->param_vardecls= 0; _temp388->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp374, _temp354); _temp388;});} _LL371: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp389=( char*)"declarator is not a function prototype";
struct _tagged_string _temp390; _temp390.curr= _temp389; _temp390.base= _temp389;
_temp390.last_plus_one= _temp389 + 39; _temp390;}), loc); _LL367:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp391=( char*)"missing argument variable in function prototype"; struct
_tagged_string _temp392; _temp392.curr= _temp391; _temp392.base= _temp391;
_temp392.last_plus_one= _temp391 + 48; _temp392;}), loc); return({ struct
_tuple4* _temp393=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp393->f1=({ struct _tagged_string* _temp394=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp394[ 0]=( struct
_tagged_string)({ char* _temp395=( char*)"?"; struct _tagged_string _temp396;
_temp396.curr= _temp395; _temp396.base= _temp395; _temp396.last_plus_one=
_temp395 + 2; _temp396;}); _temp394;}); _temp393->f2=(* t).f2; _temp393->f3=(* t).f3;
_temp393;});} else{ return({ struct _tuple4* _temp397=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp397->f1=( struct _tagged_string*)((
struct Cyc_Core_Opt*) _check_null((* t).f1))->v; _temp397->f2=(* t).f2; _temp397->f3=(*
t).f3; _temp397;});}} static char _temp400[ 52u]="at most one type may appear within a type specifier";
static struct _tagged_string Cyc_Parse_msg1=( struct _tagged_string){ _temp400,
_temp400, _temp400 + 52u}; static char _temp403[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2=( struct _tagged_string){ _temp403,
_temp403, _temp403 + 63u}; static char _temp406[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3=( struct _tagged_string){ _temp406,
_temp406, _temp406 + 50u}; static char _temp409[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4=( struct _tagged_string){ _temp409,
_temp409, _temp409 + 60u}; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
declopt= 0; int seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t=( void*)
Cyc_Absyn_VoidType; void* sz=( void*) Cyc_Absyn_B4; void* sgn=( void*) Cyc_Absyn_Signed;
struct Cyc_Position_Segment* last_loc= loc; for( 0; ts != 0; ts=(( struct Cyc_List_List*)
_check_null( ts))->tl){ void* _temp410=( void*)(( struct Cyc_List_List*)
_check_null( ts))->hd; struct Cyc_Position_Segment* _temp424; void* _temp426;
struct Cyc_Position_Segment* _temp428; struct Cyc_Position_Segment* _temp430;
struct Cyc_Position_Segment* _temp432; struct Cyc_Position_Segment* _temp434;
struct Cyc_Absyn_Decl* _temp436; _LL412: if(*(( int*) _temp410) == Cyc_Parse_Type_spec){
_LL427: _temp426=( void*)(( struct Cyc_Parse_Type_spec_struct*) _temp410)->f1;
goto _LL425; _LL425: _temp424=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Type_spec_struct*)
_temp410)->f2; goto _LL413;} else{ goto _LL414;} _LL414: if(*(( int*) _temp410)
== Cyc_Parse_Signed_spec){ _LL429: _temp428=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Signed_spec_struct*) _temp410)->f1; goto _LL415;} else{ goto
_LL416;} _LL416: if(*(( int*) _temp410) == Cyc_Parse_Unsigned_spec){ _LL431:
_temp430=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Unsigned_spec_struct*)
_temp410)->f1; goto _LL417;} else{ goto _LL418;} _LL418: if(*(( int*) _temp410)
== Cyc_Parse_Short_spec){ _LL433: _temp432=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Short_spec_struct*) _temp410)->f1; goto _LL419;} else{ goto
_LL420;} _LL420: if(*(( int*) _temp410) == Cyc_Parse_Long_spec){ _LL435:
_temp434=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Long_spec_struct*)
_temp410)->f1; goto _LL421;} else{ goto _LL422;} _LL422: if(*(( int*) _temp410)
== Cyc_Parse_Decl_spec){ _LL437: _temp436=( struct Cyc_Absyn_Decl*)(( struct Cyc_Parse_Decl_spec_struct*)
_temp410)->f1; goto _LL423;} else{ goto _LL411;} _LL413: if( seen_type){ Cyc_Parse_err(
Cyc_Parse_msg1, _temp424);} last_loc= _temp424; seen_type= 1; t= _temp426; goto
_LL411; _LL415: if( seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp428);} if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp438=( char*)"signed qualifier must come before type";
struct _tagged_string _temp439; _temp439.curr= _temp438; _temp439.base= _temp438;
_temp439.last_plus_one= _temp438 + 39; _temp439;}), _temp428);} last_loc=
_temp428; seen_sign= 1; sgn=( void*) Cyc_Absyn_Signed; goto _LL411; _LL417: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp430);} if( seen_type){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp440=( char*)"signed qualifier must come before type";
struct _tagged_string _temp441; _temp441.curr= _temp440; _temp441.base= _temp440;
_temp441.last_plus_one= _temp440 + 39; _temp441;}), _temp430);} last_loc=
_temp430; seen_sign= 1; sgn=( void*) Cyc_Absyn_Unsigned; goto _LL411; _LL419:
if( seen_size){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp442=( char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp443; _temp443.curr= _temp442; _temp443.base= _temp442;
_temp443.last_plus_one= _temp442 + 59; _temp443;}), _temp432);} if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp444=( char*)"short modifier must come before base type";
struct _tagged_string _temp445; _temp445.curr= _temp444; _temp445.base= _temp444;
_temp445.last_plus_one= _temp444 + 42; _temp445;}), _temp432);} last_loc=
_temp432; seen_size= 1; sz=( void*) Cyc_Absyn_B2; goto _LL411; _LL421: if(
seen_type){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp446=( char*)"long modifier must come before base type";
struct _tagged_string _temp447; _temp447.curr= _temp446; _temp447.base= _temp446;
_temp447.last_plus_one= _temp446 + 41; _temp447;}), _temp434);} if( seen_size){
void* _temp448= sz; _LL450: if( _temp448 ==( void*) Cyc_Absyn_B4){ goto _LL451;}
else{ goto _LL452;} _LL452: goto _LL453; _LL451: sz=( void*) Cyc_Absyn_B8; goto
_LL449; _LL453: Cyc_Parse_err(( struct _tagged_string)({ char* _temp454=( char*)"extra long in type specifier";
struct _tagged_string _temp455; _temp455.curr= _temp454; _temp455.base= _temp454;
_temp455.last_plus_one= _temp454 + 29; _temp455;}), _temp434); goto _LL449;
_LL449:;} last_loc= _temp434; seen_size= 1; goto _LL411; _LL423: last_loc=
_temp436->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp456=(
void*) _temp436->r; struct Cyc_Absyn_Structdecl* _temp468; struct Cyc_Absyn_Tuniondecl*
_temp470; struct Cyc_Absyn_Uniondecl* _temp472; struct Cyc_Absyn_Enumdecl*
_temp474; _LL458: if(*(( int*) _temp456) == Cyc_Absyn_Struct_d){ _LL469:
_temp468=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp456)->f1; goto _LL459;} else{ goto _LL460;} _LL460: if(*(( int*) _temp456)
== Cyc_Absyn_Tunion_d){ _LL471: _temp470=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp456)->f1; goto _LL461;} else{ goto
_LL462;} _LL462: if(*(( int*) _temp456) == Cyc_Absyn_Union_d){ _LL473: _temp472=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp456)->f1;
goto _LL463;} else{ goto _LL464;} _LL464: if(*(( int*) _temp456) == Cyc_Absyn_Enum_d){
_LL475: _temp474=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp456)->f1; goto _LL465;} else{ goto _LL466;} _LL466: goto _LL467; _LL459: {
struct Cyc_List_List* _temp476=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ,
_temp468->tvs); goto _LL477; _LL477: t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp478=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp478[ 0]=({ struct Cyc_Absyn_StructType_struct _temp479; _temp479.tag= Cyc_Absyn_StructType;
_temp479.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp468->name))->v); _temp479.f2= _temp476; _temp479.f3= 0;
_temp479;}); _temp478;}); if( _temp468->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp480=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp480->v=( void*) _temp436; _temp480;});} goto _LL457;} _LL461: { struct Cyc_List_List*
_temp481=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp470->tvs);
goto _LL482; _LL482: t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp483=(
struct Cyc_Absyn_TunionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp483[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp484; _temp484.tag= Cyc_Absyn_TunionType;
_temp484.f1=({ struct Cyc_Absyn_TunionInfo _temp485; _temp485.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp486=( struct Cyc_Absyn_KnownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp486[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp487; _temp487.tag= Cyc_Absyn_KnownTunion;
_temp487.f1= _temp470; _temp487;}); _temp486;})); _temp485.targs= _temp481;
_temp485.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp485;}); _temp484;});
_temp483;}); if( _temp470->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp488=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp488->v=( void*) _temp436; _temp488;});} goto _LL457;} _LL463: { struct Cyc_List_List*
_temp489=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp472->tvs);
goto _LL490; _LL490: t=( void*)({ struct Cyc_Absyn_UnionType_struct* _temp491=(
struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp491[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp492; _temp492.tag= Cyc_Absyn_UnionType;
_temp492.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp472->name))->v); _temp492.f2= _temp489; _temp492.f3= 0;
_temp492;}); _temp491;}); if( _temp472->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp493=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp493->v=( void*) _temp436; _temp493;});} goto _LL457;} _LL465: t=( void*)({
struct Cyc_Absyn_EnumType_struct* _temp494=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp494[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp495; _temp495.tag= Cyc_Absyn_EnumType; _temp495.f1=
_temp474->name; _temp495.f2= 0; _temp495;}); _temp494;}); declopt=({ struct Cyc_Core_Opt*
_temp496=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp496->v=( void*) _temp436; _temp496;}); goto _LL457; _LL467:(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp497=( char*)"bad declaration within type specifier";
struct _tagged_string _temp498; _temp498.curr= _temp497; _temp498.base= _temp497;
_temp498.last_plus_one= _temp497 + 38; _temp498;}), _temp436->loc); goto _LL457;
_LL457:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp436->loc);} goto _LL411;
_LL411:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp499=( char*)"missing type withing specifier";
struct _tagged_string _temp500; _temp500.curr= _temp499; _temp500.base= _temp499;
_temp500.last_plus_one= _temp499 + 31; _temp500;}), last_loc);} t=( void*)({
struct Cyc_Absyn_IntType_struct* _temp501=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp501[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp502; _temp502.tag= Cyc_Absyn_IntType; _temp502.f1=( void*) sgn; _temp502.f2=(
void*) sz; _temp502;}); _temp501;});} else{ if( seen_sign){ void* _temp503= t;
void* _temp509; void* _temp511; _LL505: if(( unsigned int) _temp503 > 4u?*(( int*)
_temp503) == Cyc_Absyn_IntType: 0){ _LL512: _temp511=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp503)->f1; goto _LL510; _LL510: _temp509=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp503)->f2; goto _LL506;} else{ goto _LL507;} _LL507: goto _LL508; _LL506: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp513=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp513[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp514; _temp514.tag= Cyc_Absyn_IntType; _temp514.f1=( void*) sgn; _temp514.f2=(
void*) _temp509; _temp514;}); _temp513;}); goto _LL504; _LL508: Cyc_Parse_err((
struct _tagged_string)({ char* _temp515=( char*)"sign specification on non-integral type";
struct _tagged_string _temp516; _temp516.curr= _temp515; _temp516.base= _temp515;
_temp516.last_plus_one= _temp515 + 40; _temp516;}), last_loc); goto _LL504;
_LL504:;} if( seen_size){ void* _temp517= t; void* _temp523; void* _temp525;
_LL519: if(( unsigned int) _temp517 > 4u?*(( int*) _temp517) == Cyc_Absyn_IntType:
0){ _LL526: _temp525=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp517)->f1;
goto _LL524; _LL524: _temp523=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp517)->f2; goto _LL520;} else{ goto _LL521;} _LL521: goto _LL522; _LL520: t=(
void*)({ struct Cyc_Absyn_IntType_struct* _temp527=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp527[ 0]=({ struct Cyc_Absyn_IntType_struct
_temp528; _temp528.tag= Cyc_Absyn_IntType; _temp528.f1=( void*) _temp525;
_temp528.f2=( void*) sz; _temp528;}); _temp527;}); goto _LL518; _LL522: Cyc_Parse_err((
struct _tagged_string)({ char* _temp529=( char*)"size qualifier on non-integral type";
struct _tagged_string _temp530; _temp530.curr= _temp529; _temp530.base= _temp529;
_temp530.last_plus_one= _temp529 + 36; _temp530;}), last_loc); goto _LL518;
_LL518:;}} return({ struct _tuple5 _temp531; _temp531.f1= t; _temp531.f2=
declopt; _temp531;});} static struct Cyc_List_List* Cyc_Parse_apply_tmss( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* ds, struct Cyc_List_List*
shared_atts){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator* _temp532=(
struct Cyc_Parse_Declarator*)(( struct Cyc_List_List*) _check_null( ds))->hd;
goto _LL533; _LL533: { struct _tuple1* _temp534= _temp532->id; goto _LL535;
_LL535: { struct Cyc_List_List* _temp538; struct Cyc_List_List* _temp540; void*
_temp542; struct Cyc_Absyn_Tqual _temp544; struct _tuple6 _temp536= Cyc_Parse_apply_tms(
tq, t, shared_atts, _temp532->tms); _LL545: _temp544= _temp536.f1; goto _LL543;
_LL543: _temp542= _temp536.f2; goto _LL541; _LL541: _temp540= _temp536.f3; goto
_LL539; _LL539: _temp538= _temp536.f4; goto _LL537; _LL537: return({ struct Cyc_List_List*
_temp546=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp546->hd=( void*)({ struct _tuple7* _temp547=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp547->f1= _temp534; _temp547->f2= _temp544;
_temp547->f3= _temp542; _temp547->f4= _temp540; _temp547->f5= _temp538; _temp547;});
_temp546->tl= Cyc_Parse_apply_tmss( _temp544, t,(( struct Cyc_List_List*)
_check_null( ds))->tl, shared_atts); _temp546;});}}}} static int Cyc_Parse_fn_type_att(
void* a){ void* _temp548= a; int _temp562; _LL550: if(( unsigned int) _temp548 >
15u?*(( int*) _temp548) == Cyc_Absyn_Regparm_att: 0){ _LL563: _temp562=( int)((
struct Cyc_Absyn_Regparm_att_struct*) _temp548)->f1; goto _LL551;} else{ goto
_LL552;} _LL552: if( _temp548 ==( void*) Cyc_Absyn_Stdcall_att){ goto _LL553;}
else{ goto _LL554;} _LL554: if( _temp548 ==( void*) Cyc_Absyn_Cdecl_att){ goto
_LL555;} else{ goto _LL556;} _LL556: if( _temp548 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL557;} else{ goto _LL558;} _LL558: if( _temp548 ==( void*) Cyc_Absyn_Const_att){
goto _LL559;} else{ goto _LL560;} _LL560: goto _LL561; _LL551: goto _LL553;
_LL553: goto _LL555; _LL555: goto _LL557; _LL557: goto _LL559; _LL559: return 1;
_LL561: return 0; _LL549:;} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp564; _temp564.f1= tq; _temp564.f2=
t; _temp564.f3= 0; _temp564.f4= atts; _temp564;});}{ void* _temp565=( void*)((
struct Cyc_List_List*) _check_null( tms))->hd; struct Cyc_Absyn_Exp* _temp579;
void* _temp581; int _temp583; struct Cyc_Position_Segment* _temp585; struct Cyc_List_List*
_temp587; struct Cyc_Absyn_Tqual _temp589; void* _temp591; void* _temp593;
struct Cyc_List_List* _temp595; struct Cyc_Position_Segment* _temp597; _LL567:
if( _temp565 ==( void*) Cyc_Absyn_Carray_mod){ goto _LL568;} else{ goto _LL569;}
_LL569: if(( unsigned int) _temp565 > 1u?*(( int*) _temp565) == Cyc_Absyn_ConstArray_mod:
0){ _LL580: _temp579=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp565)->f1; goto _LL570;} else{ goto _LL571;} _LL571: if(( unsigned int)
_temp565 > 1u?*(( int*) _temp565) == Cyc_Absyn_Function_mod: 0){ _LL582:
_temp581=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp565)->f1; goto
_LL572;} else{ goto _LL573;} _LL573: if(( unsigned int) _temp565 > 1u?*(( int*)
_temp565) == Cyc_Absyn_TypeParams_mod: 0){ _LL588: _temp587=( struct Cyc_List_List*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp565)->f1; goto _LL586; _LL586:
_temp585=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp565)->f2; goto _LL584; _LL584: _temp583=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp565)->f3; goto _LL574;} else{ goto _LL575;} _LL575: if(( unsigned int)
_temp565 > 1u?*(( int*) _temp565) == Cyc_Absyn_Pointer_mod: 0){ _LL594: _temp593=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp565)->f1; goto _LL592;
_LL592: _temp591=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp565)->f2;
goto _LL590; _LL590: _temp589=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp565)->f3; goto _LL576;} else{ goto _LL577;} _LL577: if(( unsigned int)
_temp565 > 1u?*(( int*) _temp565) == Cyc_Absyn_Attributes_mod: 0){ _LL598:
_temp597=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp565)->f1; goto _LL596; _LL596: _temp595=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp565)->f2; goto _LL578;} else{ goto _LL566;}
_LL568: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),( void*)({ struct
Cyc_Absyn_ArrayType_struct* _temp599=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp599[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp600; _temp600.tag= Cyc_Absyn_ArrayType; _temp600.f1=(
void*) t; _temp600.f2= tq; _temp600.f3= 0; _temp600;}); _temp599;}), atts,((
struct Cyc_List_List*) _check_null( tms))->tl); _LL570: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp601=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp601[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp602; _temp602.tag= Cyc_Absyn_ArrayType;
_temp602.f1=( void*) t; _temp602.f2= tq; _temp602.f3=( struct Cyc_Absyn_Exp*)
_temp579; _temp602;}); _temp601;}), atts,(( struct Cyc_List_List*) _check_null(
tms))->tl); _LL572: { void* _temp603= _temp581; struct Cyc_Core_Opt* _temp609;
int _temp611; struct Cyc_List_List* _temp613; struct Cyc_Position_Segment*
_temp615; struct Cyc_List_List* _temp617; _LL605: if(*(( int*) _temp603) == Cyc_Absyn_WithTypes){
_LL614: _temp613=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp603)->f1; goto _LL612; _LL612: _temp611=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp603)->f2; goto _LL610; _LL610: _temp609=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp603)->f3; goto _LL606;} else{ goto _LL607;}
_LL607: if(*(( int*) _temp603) == Cyc_Absyn_NoTypes){ _LL618: _temp617=( struct
Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*) _temp603)->f1; goto _LL616;
_LL616: _temp615=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp603)->f2; goto _LL608;} else{ goto _LL604;} _LL606: { struct Cyc_List_List*
typvars= 0; struct Cyc_List_List* fn_atts= 0; struct Cyc_List_List* new_atts= 0;{
struct Cyc_List_List* as= atts; for( 0; as != 0; as=(( struct Cyc_List_List*)
_check_null( as))->tl){ if( Cyc_Parse_fn_type_att(( void*)(( struct Cyc_List_List*)
_check_null( as))->hd)){ fn_atts=({ struct Cyc_List_List* _temp619=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp619->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( as))->hd); _temp619->tl= fn_atts; _temp619;});}
else{ new_atts=({ struct Cyc_List_List* _temp620=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp620->hd=( void*)(( void*)((
struct Cyc_List_List*) _check_null( as))->hd); _temp620->tl= new_atts; _temp620;});}}}
if((( struct Cyc_List_List*) _check_null( tms))->tl != 0){ void* _temp621=( void*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( tms))->tl))->hd;
int _temp627; struct Cyc_Position_Segment* _temp629; struct Cyc_List_List*
_temp631; _LL623: if(( unsigned int) _temp621 > 1u?*(( int*) _temp621) == Cyc_Absyn_TypeParams_mod:
0){ _LL632: _temp631=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp621)->f1; goto _LL630; _LL630: _temp629=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp621)->f2; goto _LL628; _LL628:
_temp627=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp621)->f3; goto
_LL624;} else{ goto _LL625;} _LL625: goto _LL626; _LL624: typvars= _temp631; tms=((
struct Cyc_List_List*) _check_null( tms))->tl; goto _LL622; _LL626: goto _LL622;
_LL622:;} if(((( ! _temp611? _temp613 != 0: 0)?(( struct Cyc_List_List*)
_check_null( _temp613))->tl == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp613))->hd)).f1 == 0: 0)?(*(( struct _tuple2*)(( struct Cyc_List_List*)
_check_null( _temp613))->hd)).f3 ==( void*) Cyc_Absyn_VoidType: 0){ _temp613= 0;
_temp611= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp613);
return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp609, t, _temp613, _temp611, fn_atts), new_atts,(( struct Cyc_List_List*)
_check_null( tms))->tl);} _LL608:( void) _throw((( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp633=( char*)"function declaration without parameter types";
struct _tagged_string _temp634; _temp634.curr= _temp633; _temp634.base= _temp633;
_temp634.last_plus_one= _temp633 + 45; _temp634;}), _temp615)); _LL604:;} _LL574:
if((( struct Cyc_List_List*) _check_null( tms))->tl == 0){ return({ struct
_tuple6 _temp635; _temp635.f1= tq; _temp635.f2= t; _temp635.f3= _temp587;
_temp635.f4= atts; _temp635;});}( void) _throw((( void*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp636=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp637; _temp637.curr= _temp636; _temp637.base= _temp636;
_temp637.last_plus_one= _temp636 + 68; _temp637;}), _temp585)); _LL576: { void*
_temp638= _temp593; struct Cyc_Absyn_Exp* _temp646; struct Cyc_Absyn_Exp*
_temp648; _LL640: if(( unsigned int) _temp638 > 1u?*(( int*) _temp638) == Cyc_Absyn_NonNullable_ps:
0){ _LL647: _temp646=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*)
_temp638)->f1; goto _LL641;} else{ goto _LL642;} _LL642: if(( unsigned int)
_temp638 > 1u?*(( int*) _temp638) == Cyc_Absyn_Nullable_ps: 0){ _LL649: _temp648=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp638)->f1;
goto _LL643;} else{ goto _LL644;} _LL644: if( _temp638 ==( void*) Cyc_Absyn_TaggedArray_ps){
goto _LL645;} else{ goto _LL639;} _LL641: return Cyc_Parse_apply_tms( _temp589,
Cyc_Absyn_atb_typ( t, _temp591, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp650=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp650[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp651; _temp651.tag= Cyc_Absyn_Upper_b;
_temp651.f1= _temp646; _temp651;}); _temp650;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL643: return Cyc_Parse_apply_tms( _temp589, Cyc_Absyn_starb_typ(
t, _temp591, tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp652=( struct
Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp652[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp653; _temp653.tag= Cyc_Absyn_Upper_b;
_temp653.f1= _temp648; _temp653;}); _temp652;})), atts,(( struct Cyc_List_List*)
_check_null( tms))->tl); _LL645: return Cyc_Parse_apply_tms( _temp589, Cyc_Absyn_tagged_typ(
t, _temp591, tq), atts,(( struct Cyc_List_List*) _check_null( tms))->tl); _LL639:;}
_LL578: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp595),((
struct Cyc_List_List*) _check_null( tms))->tl); _LL566:;}} void* Cyc_Parse_speclist2typ(
struct Cyc_List_List* tss, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
_temp656; void* _temp658; struct _tuple5 _temp654= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL659: _temp658= _temp654.f1; goto _LL657; _LL657: _temp656=
_temp654.f2; goto _LL655; _LL655: if( _temp656 != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp660=( char*)"ignoring nested type declaration(s) in specifier list";
struct _tagged_string _temp661; _temp661.curr= _temp660; _temp661.base= _temp660;
_temp661.last_plus_one= _temp660 + 54; _temp661;}), loc);} return _temp658;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp662=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp662[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp663; _temp663.tag= Cyc_Absyn_Decl_s;
_temp663.f1= d; _temp663.f2= s; _temp663;}); _temp662;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual tq= ds->tq;
int istypedef= 0; void* s=( void*) Cyc_Absyn_Public; struct Cyc_List_List* atts=
ds->attributes; if( ds->is_inline){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp664=( char*)"inline is only allowed on function definitions"; struct
_tagged_string _temp665; _temp665.curr= _temp664; _temp665.base= _temp664;
_temp665.last_plus_one= _temp664 + 47; _temp665;}), loc);} if( tss == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp666=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp667; _temp667.curr= _temp666; _temp667.base= _temp666;
_temp667.last_plus_one= _temp666 + 39; _temp667;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp668=( void*)(( struct Cyc_Core_Opt*) _check_null( ds->sc))->v;
_LL670: if( _temp668 ==( void*) Cyc_Parse_Typedef_sc){ goto _LL671;} else{ goto
_LL672;} _LL672: if( _temp668 ==( void*) Cyc_Parse_Extern_sc){ goto _LL673;}
else{ goto _LL674;} _LL674: if( _temp668 ==( void*) Cyc_Parse_ExternC_sc){ goto
_LL675;} else{ goto _LL676;} _LL676: if( _temp668 ==( void*) Cyc_Parse_Static_sc){
goto _LL677;} else{ goto _LL678;} _LL678: if( _temp668 ==( void*) Cyc_Parse_Auto_sc){
goto _LL679;} else{ goto _LL680;} _LL680: if( _temp668 ==( void*) Cyc_Parse_Register_sc){
goto _LL681;} else{ goto _LL682;} _LL682: if( _temp668 ==( void*) Cyc_Parse_Abstract_sc){
goto _LL683;} else{ goto _LL669;} _LL671: istypedef= 1; goto _LL669; _LL673: s=(
void*) Cyc_Absyn_Extern; goto _LL669; _LL675: s=( void*) Cyc_Absyn_ExternC; goto
_LL669; _LL677: s=( void*) Cyc_Absyn_Static; goto _LL669; _LL679: s=( void*) Cyc_Absyn_Public;
goto _LL669; _LL681: s=( void*) Cyc_Absyn_Public; goto _LL669; _LL683: s=( void*)
Cyc_Absyn_Abstract; goto _LL669; _LL669:;}{ struct Cyc_List_List* _temp686;
struct Cyc_List_List* _temp688; struct _tuple0 _temp684=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( ids); _LL689: _temp688= _temp684.f1;
goto _LL687; _LL687: _temp686= _temp684.f2; goto _LL685; _LL685: { int
exps_empty= 1;{ struct Cyc_List_List* es= _temp686; for( 0; es != 0; es=((
struct Cyc_List_List*) _check_null( es))->tl){ if(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( es))->hd != 0){ exps_empty= 0; break;}}}{
struct _tuple5 _temp690= Cyc_Parse_collapse_type_specifiers( tss, loc); goto
_LL691; _LL691: if( _temp688 == 0){ struct Cyc_Core_Opt* _temp694; void*
_temp696; struct _tuple5 _temp692= _temp690; _LL697: _temp696= _temp692.f1; goto
_LL695; _LL695: _temp694= _temp692.f2; goto _LL693; _LL693: if( _temp694 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp694))->v;{ void* _temp698=( void*) d->r; struct Cyc_Absyn_Enumdecl*
_temp710; struct Cyc_Absyn_Structdecl* _temp712; struct Cyc_Absyn_Uniondecl*
_temp714; struct Cyc_Absyn_Tuniondecl* _temp716; _LL700: if(*(( int*) _temp698)
== Cyc_Absyn_Enum_d){ _LL711: _temp710=( struct Cyc_Absyn_Enumdecl*)(( struct
Cyc_Absyn_Enum_d_struct*) _temp698)->f1; goto _LL701;} else{ goto _LL702;}
_LL702: if(*(( int*) _temp698) == Cyc_Absyn_Struct_d){ _LL713: _temp712=( struct
Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp698)->f1; goto
_LL703;} else{ goto _LL704;} _LL704: if(*(( int*) _temp698) == Cyc_Absyn_Union_d){
_LL715: _temp714=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp698)->f1; goto _LL705;} else{ goto _LL706;} _LL706: if(*(( int*) _temp698)
== Cyc_Absyn_Tunion_d){ _LL717: _temp716=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_Tunion_d_struct*) _temp698)->f1; goto _LL707;} else{ goto
_LL708;} _LL708: goto _LL709; _LL701:( void*)( _temp710->sc=( void*) s); if(
atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp718=( char*)"bad attributes on enum";
struct _tagged_string _temp719; _temp719.curr= _temp718; _temp719.base= _temp718;
_temp719.last_plus_one= _temp718 + 23; _temp719;}), loc);} goto _LL699; _LL703:(
void*)( _temp712->sc=( void*) s); _temp712->attributes= atts; goto _LL699;
_LL705:( void*)( _temp714->sc=( void*) s); _temp714->attributes= atts; goto
_LL699; _LL707:( void*)( _temp716->sc=( void*) s); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp720=( char*)"bad attributes on tunion";
struct _tagged_string _temp721; _temp721.curr= _temp720; _temp721.base= _temp720;
_temp721.last_plus_one= _temp720 + 25; _temp721;}), loc);} goto _LL699; _LL709:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp722=( char*)"bad declaration";
struct _tagged_string _temp723; _temp723.curr= _temp722; _temp723.base= _temp722;
_temp723.last_plus_one= _temp722 + 16; _temp723;}), loc); return 0; _LL699:;}
return({ struct Cyc_List_List* _temp724=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp724->hd=( void*) d; _temp724->tl= 0;
_temp724;});} else{ void* _temp725= _temp696; struct Cyc_Absyn_Structdecl**
_temp739; struct Cyc_List_List* _temp741; struct _tuple1* _temp743; struct Cyc_Absyn_TunionInfo
_temp745; void* _temp747; struct Cyc_List_List* _temp749; void* _temp751; struct
Cyc_Absyn_Tuniondecl* _temp753; struct Cyc_Absyn_TunionInfo _temp755; void*
_temp757; struct Cyc_List_List* _temp759; void* _temp761; struct Cyc_Absyn_UnknownTunionInfo
_temp763; int _temp765; struct _tuple1* _temp767; struct Cyc_Absyn_Uniondecl**
_temp769; struct Cyc_List_List* _temp771; struct _tuple1* _temp773; struct Cyc_Absyn_Enumdecl*
_temp775; struct _tuple1* _temp777; _LL727: if(( unsigned int) _temp725 > 4u?*((
int*) _temp725) == Cyc_Absyn_StructType: 0){ _LL744: _temp743=( struct _tuple1*)((
struct Cyc_Absyn_StructType_struct*) _temp725)->f1; goto _LL742; _LL742:
_temp741=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp725)->f2; goto _LL740; _LL740: _temp739=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp725)->f3; goto _LL728;} else{ goto
_LL729;} _LL729: if(( unsigned int) _temp725 > 4u?*(( int*) _temp725) == Cyc_Absyn_TunionType:
0){ _LL746: _temp745=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp725)->f1; _LL752: _temp751=( void*) _temp745.tunion_info; if(*(( int*)
_temp751) == Cyc_Absyn_KnownTunion){ _LL754: _temp753=( struct Cyc_Absyn_Tuniondecl*)((
struct Cyc_Absyn_KnownTunion_struct*) _temp751)->f1; goto _LL750;} else{ goto
_LL731;} _LL750: _temp749=( struct Cyc_List_List*) _temp745.targs; goto _LL748;
_LL748: _temp747=( void*) _temp745.rgn; goto _LL730;} else{ goto _LL731;} _LL731:
if(( unsigned int) _temp725 > 4u?*(( int*) _temp725) == Cyc_Absyn_TunionType: 0){
_LL756: _temp755=( struct Cyc_Absyn_TunionInfo)(( struct Cyc_Absyn_TunionType_struct*)
_temp725)->f1; _LL762: _temp761=( void*) _temp755.tunion_info; if(*(( int*)
_temp761) == Cyc_Absyn_UnknownTunion){ _LL764: _temp763=( struct Cyc_Absyn_UnknownTunionInfo)((
struct Cyc_Absyn_UnknownTunion_struct*) _temp761)->f1; _LL768: _temp767=( struct
_tuple1*) _temp763.name; goto _LL766; _LL766: _temp765=( int) _temp763.is_xtunion;
goto _LL760;} else{ goto _LL733;} _LL760: _temp759=( struct Cyc_List_List*)
_temp755.targs; goto _LL758; _LL758: _temp757=( void*) _temp755.rgn; goto _LL732;}
else{ goto _LL733;} _LL733: if(( unsigned int) _temp725 > 4u?*(( int*) _temp725)
== Cyc_Absyn_UnionType: 0){ _LL774: _temp773=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*)
_temp725)->f1; goto _LL772; _LL772: _temp771=( struct Cyc_List_List*)(( struct
Cyc_Absyn_UnionType_struct*) _temp725)->f2; goto _LL770; _LL770: _temp769=(
struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*) _temp725)->f3;
goto _LL734;} else{ goto _LL735;} _LL735: if(( unsigned int) _temp725 > 4u?*((
int*) _temp725) == Cyc_Absyn_EnumType: 0){ _LL778: _temp777=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp725)->f1; goto _LL776; _LL776: _temp775=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_EnumType_struct*) _temp725)->f2;
goto _LL736;} else{ goto _LL737;} _LL737: goto _LL738; _LL728: { struct Cyc_List_List*
_temp779=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, loc, _temp741); goto _LL780; _LL780: { struct Cyc_Absyn_Structdecl*
_temp783=({ struct Cyc_Absyn_Structdecl* _temp781=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp781->sc=( void*) s;
_temp781->name=({ struct Cyc_Core_Opt* _temp782=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp782->v=( void*)(( struct _tuple1*)((
struct _tuple1*) _check_null( _temp743))); _temp782;}); _temp781->tvs= _temp779;
_temp781->fields= 0; _temp781->attributes= 0; _temp781;}); goto _LL784; _LL784:
if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp785=( char*)"bad attributes on struct";
struct _tagged_string _temp786; _temp786.curr= _temp785; _temp786.base= _temp785;
_temp786.last_plus_one= _temp785 + 25; _temp786;}), loc);} return({ struct Cyc_List_List*
_temp787=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp787->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp788=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp788[ 0]=({ struct Cyc_Absyn_Struct_d_struct _temp789; _temp789.tag= Cyc_Absyn_Struct_d;
_temp789.f1= _temp783; _temp789;}); _temp788;}), loc); _temp787->tl= 0; _temp787;});}}
_LL730: if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp790=(
char*)"bad attributes on tunion"; struct _tagged_string _temp791; _temp791.curr=
_temp790; _temp791.base= _temp790; _temp791.last_plus_one= _temp790 + 25;
_temp791;}), loc);} return({ struct Cyc_List_List* _temp792=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp792->hd=( void*) Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Tunion_d_struct* _temp793=( struct Cyc_Absyn_Tunion_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct)); _temp793[ 0]=({ struct
Cyc_Absyn_Tunion_d_struct _temp794; _temp794.tag= Cyc_Absyn_Tunion_d; _temp794.f1=
_temp753; _temp794;}); _temp793;}), loc); _temp792->tl= 0; _temp792;}); _LL732: {
struct Cyc_List_List* _temp795=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp759);
goto _LL796; _LL796: { struct Cyc_Absyn_Decl* _temp797= Cyc_Absyn_tunion_decl( s,
_temp767, _temp795, 0, _temp765, loc); goto _LL798; _LL798: if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp799=( char*)"bad attributes on tunion";
struct _tagged_string _temp800; _temp800.curr= _temp799; _temp800.base= _temp799;
_temp800.last_plus_one= _temp799 + 25; _temp800;}), loc);} return({ struct Cyc_List_List*
_temp801=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp801->hd=( void*) _temp797; _temp801->tl= 0; _temp801;});}} _LL734: { struct
Cyc_List_List* _temp802=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp771); goto
_LL803; _LL803: { struct Cyc_Absyn_Uniondecl* _temp806=({ struct Cyc_Absyn_Uniondecl*
_temp804=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp804->sc=( void*) s; _temp804->name=({ struct Cyc_Core_Opt* _temp805=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp805->v=(
void*)(( struct _tuple1*)(( struct _tuple1*) _check_null( _temp773))); _temp805;});
_temp804->tvs= _temp802; _temp804->fields= 0; _temp804->attributes= 0; _temp804;});
goto _LL807; _LL807: if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp808=( char*)"bad attributes on union"; struct _tagged_string _temp809;
_temp809.curr= _temp808; _temp809.base= _temp808; _temp809.last_plus_one=
_temp808 + 24; _temp809;}), loc);} return({ struct Cyc_List_List* _temp810=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp810->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp811=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp811->r=( void*)(( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp812=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp812[ 0]=({ struct Cyc_Absyn_Union_d_struct _temp813; _temp813.tag= Cyc_Absyn_Union_d;
_temp813.f1= _temp806; _temp813;}); _temp812;})); _temp811->loc= loc; _temp811;});
_temp810->tl= 0; _temp810;});}} _LL736: { struct Cyc_Absyn_Enumdecl* _temp815=({
struct Cyc_Absyn_Enumdecl* _temp814=( struct Cyc_Absyn_Enumdecl*) GC_malloc(
sizeof( struct Cyc_Absyn_Enumdecl)); _temp814->sc=( void*) s; _temp814->name=
_temp777; _temp814->fields= 0; _temp814;}); goto _LL816; _LL816: if( atts != 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp817=( char*)"bad attributes on enum";
struct _tagged_string _temp818; _temp818.curr= _temp817; _temp818.base= _temp817;
_temp818.last_plus_one= _temp817 + 23; _temp818;}), loc);} return({ struct Cyc_List_List*
_temp819=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp819->hd=( void*)({ struct Cyc_Absyn_Decl* _temp820=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp820->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp821=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp821[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp822; _temp822.tag= Cyc_Absyn_Enum_d; _temp822.f1= _temp815; _temp822;});
_temp821;})); _temp820->loc= loc; _temp820;}); _temp819->tl= 0; _temp819;});}
_LL738: Cyc_Parse_err(( struct _tagged_string)({ char* _temp823=( char*)"missing declarator";
struct _tagged_string _temp824; _temp824.curr= _temp823; _temp824.base= _temp823;
_temp824.last_plus_one= _temp823 + 19; _temp824;}), loc); return 0; _LL726:;}}
else{ void* t= _temp690.f1; struct Cyc_List_List* _temp825= Cyc_Parse_apply_tmss(
tq, t, _temp688, atts); goto _LL826; _LL826: if( istypedef){ if( ! exps_empty){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp827=( char*)"initializer in typedef declaration";
struct _tagged_string _temp828; _temp828.curr= _temp827; _temp828.base= _temp827;
_temp828.last_plus_one= _temp827 + 35; _temp828;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, _temp825); if( _temp690.f2 !=
0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( _temp690.f2))->v;{ void* _temp829=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp841; struct Cyc_Absyn_Tuniondecl* _temp843; struct Cyc_Absyn_Uniondecl*
_temp845; struct Cyc_Absyn_Enumdecl* _temp847; _LL831: if(*(( int*) _temp829) ==
Cyc_Absyn_Struct_d){ _LL842: _temp841=( struct Cyc_Absyn_Structdecl*)(( struct
Cyc_Absyn_Struct_d_struct*) _temp829)->f1; goto _LL832;} else{ goto _LL833;}
_LL833: if(*(( int*) _temp829) == Cyc_Absyn_Tunion_d){ _LL844: _temp843=( struct
Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*) _temp829)->f1; goto
_LL834;} else{ goto _LL835;} _LL835: if(*(( int*) _temp829) == Cyc_Absyn_Union_d){
_LL846: _temp845=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp829)->f1; goto _LL836;} else{ goto _LL837;} _LL837: if(*(( int*) _temp829)
== Cyc_Absyn_Enum_d){ _LL848: _temp847=( struct Cyc_Absyn_Enumdecl*)(( struct
Cyc_Absyn_Enum_d_struct*) _temp829)->f1; goto _LL838;} else{ goto _LL839;}
_LL839: goto _LL840; _LL832:( void*)( _temp841->sc=( void*) s); _temp841->attributes=
atts; atts= 0; goto _LL830; _LL834:( void*)( _temp843->sc=( void*) s); goto
_LL830; _LL836:( void*)( _temp845->sc=( void*) s); goto _LL830; _LL838:( void*)(
_temp847->sc=( void*) s); goto _LL830; _LL840: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp849=( char*)"declaration within typedef is not a struct, union, tunion, or xtunion";
struct _tagged_string _temp850; _temp850.curr= _temp849; _temp850.base= _temp849;
_temp850.last_plus_one= _temp849 + 70; _temp850;}), loc); goto _LL830; _LL830:;}
decls=({ struct Cyc_List_List* _temp851=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp851->hd=( void*) d; _temp851->tl= decls;
_temp851;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp852=
Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*) _check_null( atts))->hd);
xprintf("bad attribute %.*s in typedef", _temp852.last_plus_one - _temp852.curr,
_temp852.curr);}), loc);} return decls;}} else{ if( _temp690.f2 != 0){ Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp853=( char*)"nested type declaration within declarator";
struct _tagged_string _temp854; _temp854.curr= _temp853; _temp854.base= _temp853;
_temp854.last_plus_one= _temp853 + 42; _temp854;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* _temp855= _temp825; goto _LL856; _LL856: for( 0;
_temp855 != 0;( _temp855=(( struct Cyc_List_List*) _check_null( _temp855))->tl,
_temp686=(( struct Cyc_List_List*) _check_null( _temp686))->tl)){ struct _tuple7
_temp859; struct Cyc_List_List* _temp860; struct Cyc_List_List* _temp862; void*
_temp864; struct Cyc_Absyn_Tqual _temp866; struct _tuple1* _temp868; struct
_tuple7* _temp857=( struct _tuple7*)(( struct Cyc_List_List*) _check_null(
_temp855))->hd; _temp859=* _temp857; _LL869: _temp868= _temp859.f1; goto _LL867;
_LL867: _temp866= _temp859.f2; goto _LL865; _LL865: _temp864= _temp859.f3; goto
_LL863; _LL863: _temp862= _temp859.f4; goto _LL861; _LL861: _temp860= _temp859.f5;
goto _LL858; _LL858: if( _temp862 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp870=( char*)"bad type params, ignoring"; struct _tagged_string
_temp871; _temp871.curr= _temp870; _temp871.base= _temp870; _temp871.last_plus_one=
_temp870 + 26; _temp871;}), loc);} if( _temp686 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp872=( char*)"unexpected null in parse!"; struct
_tagged_string _temp873; _temp873.curr= _temp872; _temp873.base= _temp872;
_temp873.last_plus_one= _temp872 + 26; _temp873;}), loc);}{ struct Cyc_Absyn_Exp*
_temp874=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp686))->hd;
goto _LL875; _LL875: { struct Cyc_Absyn_Vardecl* _temp876= Cyc_Absyn_new_vardecl(
_temp868, _temp864, _temp874); goto _LL877; _LL877: _temp876->tq= _temp866;(
void*)( _temp876->sc=( void*) s); _temp876->attributes= _temp860;{ struct Cyc_Absyn_Decl*
_temp881=({ struct Cyc_Absyn_Decl* _temp878=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp878->r=( void*)(( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp879=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp879[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp880; _temp880.tag= Cyc_Absyn_Var_d;
_temp880.f1= _temp876; _temp880;}); _temp879;})); _temp878->loc= loc; _temp878;});
goto _LL882; _LL882: decls=({ struct Cyc_List_List* _temp883=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp883->hd=( void*) _temp881;
_temp883->tl= decls; _temp883;});}}}}} return(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen(
s) != 1){ Cyc_Parse_err(({ struct _tagged_string _temp884= s; xprintf("bad kind: %.*s",
_temp884.last_plus_one - _temp884.curr, _temp884.curr);}), loc); return( void*)
Cyc_Absyn_BoxKind;} else{ switch(*(( char*(*)( struct _tagged_string,
unsigned int, unsigned int)) _check_unknown_subscript)( s, sizeof( char), 0)){
case 'A': _LL885: return( void*) Cyc_Absyn_AnyKind; case 'M': _LL886: return(
void*) Cyc_Absyn_MemKind; case 'B': _LL887: return( void*) Cyc_Absyn_BoxKind;
case 'R': _LL888: return( void*) Cyc_Absyn_RgnKind; case 'E': _LL889: return(
void*) Cyc_Absyn_EffKind; default: _LL890: Cyc_Parse_err(({ struct
_tagged_string _temp892= s; xprintf("bad kind: %.*s", _temp892.last_plus_one -
_temp892.curr, _temp892.curr);}), loc); return( void*) Cyc_Absyn_BoxKind;}}}
static struct Cyc_List_List* Cyc_Parse_attopt_to_tms( struct Cyc_Position_Segment*
loc, struct Cyc_List_List* atts, struct Cyc_List_List* tms){ if( atts == 0){
return tms;} else{ return({ struct Cyc_List_List* _temp893=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp893->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp894=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp894[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp895; _temp895.tag= Cyc_Absyn_Attributes_mod;
_temp895.f1= loc; _temp895.f2= atts; _temp895;}); _temp894;})); _temp893->tl=
tms; _temp893;});}} static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1;
Cyc_Lex_register_typedef( x); if((* t).f5 != 0){ Cyc_Parse_err(({ struct
_tagged_string _temp896= Cyc_Absyn_attribute2string(( void*)(( struct Cyc_List_List*)
_check_null((* t).f5))->hd); xprintf("bad attribute %.*s within typedef",
_temp896.last_plus_one - _temp896.curr, _temp896.curr);}), loc);} return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Typedef_d_struct* _temp897=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp897[ 0]=({ struct
Cyc_Absyn_Typedef_d_struct _temp898; _temp898.tag= Cyc_Absyn_Typedef_d; _temp898.f1=({
struct Cyc_Absyn_Typedefdecl* _temp899=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp899->name= x; _temp899->tvs=(*
t).f4; _temp899->defn=( void*)(* t).f3; _temp899;}); _temp898;}); _temp897;}),
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
struct _tuple13{ struct Cyc_Parse_Declarator* f1; struct Cyc_Core_Opt* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ char* tag; struct _tuple13* f1; } ; char
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
_tuple14{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
char* tag; struct _tuple14* f1; } ; char Cyc_FnDecl_tok[ 15u]="\000\000\000\000FnDecl_tok";
struct Cyc_FnDecl_tok_struct{ char* tag; struct Cyc_Absyn_Fndecl* f1; } ; char
Cyc_IdList_tok[ 15u]="\000\000\000\000IdList_tok"; struct Cyc_IdList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_InitDeclList_tok[ 21u]="\000\000\000\000InitDeclList_tok";
struct Cyc_InitDeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_InitDecl_tok[ 17u]="\000\000\000\000InitDecl_tok"; struct _tuple15{
struct Cyc_Parse_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_InitDecl_tok_struct{
char* tag; struct _tuple15* f1; } ; char Cyc_InitializerList_tok[ 24u]="\000\000\000\000InitializerList_tok";
struct Cyc_InitializerList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_Int_tok[ 12u]="\000\000\000\000Int_tok"; struct _tuple16{ void* f1; int
f2; } ; struct Cyc_Int_tok_struct{ char* tag; struct _tuple16* f1; } ; char Cyc_Kind_tok[
13u]="\000\000\000\000Kind_tok"; struct Cyc_Kind_tok_struct{ char* tag; void* f1;
} ; char Cyc_Okay_tok[ 13u]="\000\000\000\000Okay_tok"; char Cyc_ParamDeclListBool_tok[
26u]="\000\000\000\000ParamDeclListBool_tok"; struct _tuple17{ struct Cyc_List_List*
f1; int f2; struct Cyc_Core_Opt* f3; } ; struct Cyc_ParamDeclListBool_tok_struct{
char* tag; struct _tuple17* f1; } ; char Cyc_ParamDeclList_tok[ 22u]="\000\000\000\000ParamDeclList_tok";
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
21u]="\000\000\000\000QualSpecList_tok"; struct _tuple18{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
char* tag; struct _tuple18* f1; } ; char Cyc_Rgn_tok[ 12u]="\000\000\000\000Rgn_tok";
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
} ; static char _temp903[ 8u]="Int_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp903,
_temp903, _temp903 + 8u}}; struct _tuple16* Cyc_yyget_Int_tok( void* yy1){ void*
_temp904= yy1; struct _tuple16* _temp910; _LL906: if(*(( void**) _temp904) ==
Cyc_Int_tok){ _LL911: _temp910=( struct _tuple16*)(( struct Cyc_Int_tok_struct*)
_temp904)->f1; goto _LL907;} else{ goto _LL908;} _LL908: goto _LL909; _LL907:
return _temp910; _LL909:( void) _throw(( void*)& Cyc_yyfail_Int_tok); _LL905:;}
static char _temp915[ 11u]="String_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_String_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp915,
_temp915, _temp915 + 11u}}; struct _tagged_string Cyc_yyget_String_tok( void*
yy1){ void* _temp916= yy1; struct _tagged_string _temp922; _LL918: if(*(( void**)
_temp916) == Cyc_String_tok){ _LL923: _temp922=( struct _tagged_string)(( struct
Cyc_String_tok_struct*) _temp916)->f1; goto _LL919;} else{ goto _LL920;} _LL920:
goto _LL921; _LL919: return _temp922; _LL921:( void) _throw(( void*)& Cyc_yyfail_String_tok);
_LL917:;} static char _temp927[ 9u]="Char_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp927,
_temp927, _temp927 + 9u}}; char Cyc_yyget_Char_tok( void* yy1){ void* _temp928=
yy1; char _temp934; _LL930: if(*(( void**) _temp928) == Cyc_Char_tok){ _LL935:
_temp934=( char)(( struct Cyc_Char_tok_struct*) _temp928)->f1; goto _LL931;}
else{ goto _LL932;} _LL932: goto _LL933; _LL931: return _temp934; _LL933:( void)
_throw(( void*)& Cyc_yyfail_Char_tok); _LL929:;} static char _temp939[ 17u]="Pointer_Sort_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Pointer_Sort_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp939, _temp939, _temp939 + 17u}}; void* Cyc_yyget_Pointer_Sort_tok(
void* yy1){ void* _temp940= yy1; void* _temp946; _LL942: if(*(( void**) _temp940)
== Cyc_Pointer_Sort_tok){ _LL947: _temp946=( void*)(( struct Cyc_Pointer_Sort_tok_struct*)
_temp940)->f1; goto _LL943;} else{ goto _LL944;} _LL944: goto _LL945; _LL943:
return _temp946; _LL945:( void) _throw(( void*)& Cyc_yyfail_Pointer_Sort_tok);
_LL941:;} static char _temp951[ 8u]="Exp_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Exp_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp951,
_temp951, _temp951 + 8u}}; struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( void* yy1){
void* _temp952= yy1; struct Cyc_Absyn_Exp* _temp958; _LL954: if(*(( void**)
_temp952) == Cyc_Exp_tok){ _LL959: _temp958=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Exp_tok_struct*) _temp952)->f1; goto _LL955;} else{ goto _LL956;} _LL956:
goto _LL957; _LL955: return _temp958; _LL957:( void) _throw(( void*)& Cyc_yyfail_Exp_tok);
_LL953:;} static char _temp963[ 12u]="ExpList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ExpList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp963,
_temp963, _temp963 + 12u}}; struct Cyc_List_List* Cyc_yyget_ExpList_tok( void*
yy1){ void* _temp964= yy1; struct Cyc_List_List* _temp970; _LL966: if(*(( void**)
_temp964) == Cyc_ExpList_tok){ _LL971: _temp970=( struct Cyc_List_List*)((
struct Cyc_ExpList_tok_struct*) _temp964)->f1; goto _LL967;} else{ goto _LL968;}
_LL968: goto _LL969; _LL967: return _temp970; _LL969:( void) _throw(( void*)&
Cyc_yyfail_ExpList_tok); _LL965:;} static char _temp975[ 20u]="InitializerList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitializerList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp975, _temp975, _temp975 + 20u}}; struct Cyc_List_List*
Cyc_yyget_InitializerList_tok( void* yy1){ void* _temp976= yy1; struct Cyc_List_List*
_temp982; _LL978: if(*(( void**) _temp976) == Cyc_InitializerList_tok){ _LL983:
_temp982=( struct Cyc_List_List*)(( struct Cyc_InitializerList_tok_struct*)
_temp976)->f1; goto _LL979;} else{ goto _LL980;} _LL980: goto _LL981; _LL979:
return _temp982; _LL981:( void) _throw(( void*)& Cyc_yyfail_InitializerList_tok);
_LL977:;} static char _temp987[ 11u]="Primop_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Primop_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp987,
_temp987, _temp987 + 11u}}; void* Cyc_yyget_Primop_tok( void* yy1){ void*
_temp988= yy1; void* _temp994; _LL990: if(*(( void**) _temp988) == Cyc_Primop_tok){
_LL995: _temp994=( void*)(( struct Cyc_Primop_tok_struct*) _temp988)->f1; goto
_LL991;} else{ goto _LL992;} _LL992: goto _LL993; _LL991: return _temp994;
_LL993:( void) _throw(( void*)& Cyc_yyfail_Primop_tok); _LL989:;} static char
_temp999[ 14u]="Primopopt_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Primopopt_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp999, _temp999, _temp999 + 14u}};
struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( void* yy1){ void* _temp1000= yy1;
struct Cyc_Core_Opt* _temp1006; _LL1002: if(*(( void**) _temp1000) == Cyc_Primopopt_tok){
_LL1007: _temp1006=( struct Cyc_Core_Opt*)(( struct Cyc_Primopopt_tok_struct*)
_temp1000)->f1; goto _LL1003;} else{ goto _LL1004;} _LL1004: goto _LL1005;
_LL1003: return _temp1006; _LL1005:( void) _throw(( void*)& Cyc_yyfail_Primopopt_tok);
_LL1001:;} static char _temp1011[ 11u]="QualId_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1011,
_temp1011, _temp1011 + 11u}}; struct _tuple1* Cyc_yyget_QualId_tok( void* yy1){
void* _temp1012= yy1; struct _tuple1* _temp1018; _LL1014: if(*(( void**)
_temp1012) == Cyc_QualId_tok){ _LL1019: _temp1018=( struct _tuple1*)(( struct
Cyc_QualId_tok_struct*) _temp1012)->f1; goto _LL1015;} else{ goto _LL1016;}
_LL1016: goto _LL1017; _LL1015: return _temp1018; _LL1017:( void) _throw(( void*)&
Cyc_yyfail_QualId_tok); _LL1013:;} static char _temp1023[ 9u]="Stmt_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Stmt_tok={ Cyc_Core_Failure,( struct
_tagged_string){ _temp1023, _temp1023, _temp1023 + 9u}}; struct Cyc_Absyn_Stmt*
Cyc_yyget_Stmt_tok( void* yy1){ void* _temp1024= yy1; struct Cyc_Absyn_Stmt*
_temp1030; _LL1026: if(*(( void**) _temp1024) == Cyc_Stmt_tok){ _LL1031:
_temp1030=( struct Cyc_Absyn_Stmt*)(( struct Cyc_Stmt_tok_struct*) _temp1024)->f1;
goto _LL1027;} else{ goto _LL1028;} _LL1028: goto _LL1029; _LL1027: return
_temp1030; _LL1029:( void) _throw(( void*)& Cyc_yyfail_Stmt_tok); _LL1025:;}
static char _temp1035[ 14u]="BlockItem_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_BlockItem_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1035,
_temp1035, _temp1035 + 14u}}; void* Cyc_yyget_BlockItem_tok( void* yy1){ void*
_temp1036= yy1; void* _temp1042; _LL1038: if(*(( void**) _temp1036) == Cyc_BlockItem_tok){
_LL1043: _temp1042=( void*)(( struct Cyc_BlockItem_tok_struct*) _temp1036)->f1;
goto _LL1039;} else{ goto _LL1040;} _LL1040: goto _LL1041; _LL1039: return
_temp1042; _LL1041:( void) _throw(( void*)& Cyc_yyfail_BlockItem_tok); _LL1037:;}
static char _temp1047[ 21u]="SwitchClauseList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_SwitchClauseList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1047, _temp1047, _temp1047 + 21u}}; struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
void* yy1){ void* _temp1048= yy1; struct Cyc_List_List* _temp1054; _LL1050: if(*((
void**) _temp1048) == Cyc_SwitchClauseList_tok){ _LL1055: _temp1054=( struct Cyc_List_List*)((
struct Cyc_SwitchClauseList_tok_struct*) _temp1048)->f1; goto _LL1051;} else{
goto _LL1052;} _LL1052: goto _LL1053; _LL1051: return _temp1054; _LL1053:( void)
_throw(( void*)& Cyc_yyfail_SwitchClauseList_tok); _LL1049:;} static char
_temp1059[ 12u]="Pattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Pattern_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1059, _temp1059, _temp1059 + 12u}};
struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( void* yy1){ void* _temp1060= yy1;
struct Cyc_Absyn_Pat* _temp1066; _LL1062: if(*(( void**) _temp1060) == Cyc_Pattern_tok){
_LL1067: _temp1066=( struct Cyc_Absyn_Pat*)(( struct Cyc_Pattern_tok_struct*)
_temp1060)->f1; goto _LL1063;} else{ goto _LL1064;} _LL1064: goto _LL1065;
_LL1063: return _temp1066; _LL1065:( void) _throw(( void*)& Cyc_yyfail_Pattern_tok);
_LL1061:;} static char _temp1071[ 16u]="PatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_PatternList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1071, _temp1071, _temp1071 + 16u}}; struct Cyc_List_List* Cyc_yyget_PatternList_tok(
void* yy1){ void* _temp1072= yy1; struct Cyc_List_List* _temp1078; _LL1074: if(*((
void**) _temp1072) == Cyc_PatternList_tok){ _LL1079: _temp1078=( struct Cyc_List_List*)((
struct Cyc_PatternList_tok_struct*) _temp1072)->f1; goto _LL1075;} else{ goto
_LL1076;} _LL1076: goto _LL1077; _LL1075: return _temp1078; _LL1077:( void)
_throw(( void*)& Cyc_yyfail_PatternList_tok); _LL1073:;} static char _temp1083[
17u]="FieldPattern_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FieldPattern_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1083, _temp1083, _temp1083 + 17u}};
struct _tuple14* Cyc_yyget_FieldPattern_tok( void* yy1){ void* _temp1084= yy1;
struct _tuple14* _temp1090; _LL1086: if(*(( void**) _temp1084) == Cyc_FieldPattern_tok){
_LL1091: _temp1090=( struct _tuple14*)(( struct Cyc_FieldPattern_tok_struct*)
_temp1084)->f1; goto _LL1087;} else{ goto _LL1088;} _LL1088: goto _LL1089;
_LL1087: return _temp1090; _LL1089:( void) _throw(( void*)& Cyc_yyfail_FieldPattern_tok);
_LL1085:;} static char _temp1095[ 21u]="FieldPatternList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_FieldPatternList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1095, _temp1095, _temp1095 + 21u}}; struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
void* yy1){ void* _temp1096= yy1; struct Cyc_List_List* _temp1102; _LL1098: if(*((
void**) _temp1096) == Cyc_FieldPatternList_tok){ _LL1103: _temp1102=( struct Cyc_List_List*)((
struct Cyc_FieldPatternList_tok_struct*) _temp1096)->f1; goto _LL1099;} else{
goto _LL1100;} _LL1100: goto _LL1101; _LL1099: return _temp1102; _LL1101:( void)
_throw(( void*)& Cyc_yyfail_FieldPatternList_tok); _LL1097:;} static char
_temp1107[ 11u]="FnDecl_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_FnDecl_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1107, _temp1107, _temp1107 + 11u}};
struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( void* yy1){ void* _temp1108= yy1;
struct Cyc_Absyn_Fndecl* _temp1114; _LL1110: if(*(( void**) _temp1108) == Cyc_FnDecl_tok){
_LL1115: _temp1114=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_FnDecl_tok_struct*)
_temp1108)->f1; goto _LL1111;} else{ goto _LL1112;} _LL1112: goto _LL1113;
_LL1111: return _temp1114; _LL1113:( void) _throw(( void*)& Cyc_yyfail_FnDecl_tok);
_LL1109:;} static char _temp1119[ 13u]="DeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1119,
_temp1119, _temp1119 + 13u}}; struct Cyc_List_List* Cyc_yyget_DeclList_tok( void*
yy1){ void* _temp1120= yy1; struct Cyc_List_List* _temp1126; _LL1122: if(*((
void**) _temp1120) == Cyc_DeclList_tok){ _LL1127: _temp1126=( struct Cyc_List_List*)((
struct Cyc_DeclList_tok_struct*) _temp1120)->f1; goto _LL1123;} else{ goto
_LL1124;} _LL1124: goto _LL1125; _LL1123: return _temp1126; _LL1125:( void)
_throw(( void*)& Cyc_yyfail_DeclList_tok); _LL1121:;} static char _temp1131[ 13u]="DeclSpec_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DeclSpec_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1131, _temp1131, _temp1131 + 13u}}; struct Cyc_Parse_Declaration_spec*
Cyc_yyget_DeclSpec_tok( void* yy1){ void* _temp1132= yy1; struct Cyc_Parse_Declaration_spec*
_temp1138; _LL1134: if(*(( void**) _temp1132) == Cyc_DeclSpec_tok){ _LL1139:
_temp1138=( struct Cyc_Parse_Declaration_spec*)(( struct Cyc_DeclSpec_tok_struct*)
_temp1132)->f1; goto _LL1135;} else{ goto _LL1136;} _LL1136: goto _LL1137;
_LL1135: return _temp1138; _LL1137:( void) _throw(( void*)& Cyc_yyfail_DeclSpec_tok);
_LL1133:;} static char _temp1143[ 13u]="InitDecl_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_InitDecl_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1143,
_temp1143, _temp1143 + 13u}}; struct _tuple15* Cyc_yyget_InitDecl_tok( void* yy1){
void* _temp1144= yy1; struct _tuple15* _temp1150; _LL1146: if(*(( void**)
_temp1144) == Cyc_InitDecl_tok){ _LL1151: _temp1150=( struct _tuple15*)(( struct
Cyc_InitDecl_tok_struct*) _temp1144)->f1; goto _LL1147;} else{ goto _LL1148;}
_LL1148: goto _LL1149; _LL1147: return _temp1150; _LL1149:( void) _throw(( void*)&
Cyc_yyfail_InitDecl_tok); _LL1145:;} static char _temp1155[ 17u]="InitDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_InitDeclList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1155, _temp1155, _temp1155 + 17u}}; struct Cyc_List_List*
Cyc_yyget_InitDeclList_tok( void* yy1){ void* _temp1156= yy1; struct Cyc_List_List*
_temp1162; _LL1158: if(*(( void**) _temp1156) == Cyc_InitDeclList_tok){ _LL1163:
_temp1162=( struct Cyc_List_List*)(( struct Cyc_InitDeclList_tok_struct*)
_temp1156)->f1; goto _LL1159;} else{ goto _LL1160;} _LL1160: goto _LL1161;
_LL1159: return _temp1162; _LL1161:( void) _throw(( void*)& Cyc_yyfail_InitDeclList_tok);
_LL1157:;} static char _temp1167[ 17u]="StorageClass_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StorageClass_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1167, _temp1167, _temp1167 + 17u}}; void* Cyc_yyget_StorageClass_tok( void*
yy1){ void* _temp1168= yy1; void* _temp1174; _LL1170: if(*(( void**) _temp1168)
== Cyc_StorageClass_tok){ _LL1175: _temp1174=( void*)(( struct Cyc_StorageClass_tok_struct*)
_temp1168)->f1; goto _LL1171;} else{ goto _LL1172;} _LL1172: goto _LL1173;
_LL1171: return _temp1174; _LL1173:( void) _throw(( void*)& Cyc_yyfail_StorageClass_tok);
_LL1169:;} static char _temp1179[ 18u]="TypeSpecifier_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeSpecifier_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1179, _temp1179, _temp1179 + 18u}}; void* Cyc_yyget_TypeSpecifier_tok( void*
yy1){ void* _temp1180= yy1; void* _temp1186; _LL1182: if(*(( void**) _temp1180)
== Cyc_TypeSpecifier_tok){ _LL1187: _temp1186=( void*)(( struct Cyc_TypeSpecifier_tok_struct*)
_temp1180)->f1; goto _LL1183;} else{ goto _LL1184;} _LL1184: goto _LL1185;
_LL1183: return _temp1186; _LL1185:( void) _throw(( void*)& Cyc_yyfail_TypeSpecifier_tok);
_LL1181:;} static char _temp1191[ 18u]="StructOrUnion_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_StructOrUnion_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1191, _temp1191, _temp1191 + 18u}}; void* Cyc_yyget_StructOrUnion_tok( void*
yy1){ void* _temp1192= yy1; void* _temp1198; _LL1194: if(*(( void**) _temp1192)
== Cyc_StructOrUnion_tok){ _LL1199: _temp1198=( void*)(( struct Cyc_StructOrUnion_tok_struct*)
_temp1192)->f1; goto _LL1195;} else{ goto _LL1196;} _LL1196: goto _LL1197;
_LL1195: return _temp1198; _LL1197:( void) _throw(( void*)& Cyc_yyfail_StructOrUnion_tok);
_LL1193:;} static char _temp1203[ 13u]="TypeQual_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeQual_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1203,
_temp1203, _temp1203 + 13u}}; struct Cyc_Absyn_Tqual Cyc_yyget_TypeQual_tok(
void* yy1){ void* _temp1204= yy1; struct Cyc_Absyn_Tqual _temp1210; _LL1206: if(*((
void**) _temp1204) == Cyc_TypeQual_tok){ _LL1211: _temp1210=( struct Cyc_Absyn_Tqual)((
struct Cyc_TypeQual_tok_struct*) _temp1204)->f1; goto _LL1207;} else{ goto
_LL1208;} _LL1208: goto _LL1209; _LL1207: return _temp1210; _LL1209:( void)
_throw(( void*)& Cyc_yyfail_TypeQual_tok); _LL1205:;} static char _temp1215[ 24u]="StructFieldDeclList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1215, _temp1215, _temp1215 + 24u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclList_tok( void* yy1){ void* _temp1216= yy1; struct Cyc_List_List*
_temp1222; _LL1218: if(*(( void**) _temp1216) == Cyc_StructFieldDeclList_tok){
_LL1223: _temp1222=( struct Cyc_List_List*)(( struct Cyc_StructFieldDeclList_tok_struct*)
_temp1216)->f1; goto _LL1219;} else{ goto _LL1220;} _LL1220: goto _LL1221;
_LL1219: return _temp1222; _LL1221:( void) _throw(( void*)& Cyc_yyfail_StructFieldDeclList_tok);
_LL1217:;} static char _temp1227[ 28u]="StructFieldDeclListList_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_StructFieldDeclListList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1227, _temp1227, _temp1227 + 28u}}; struct Cyc_List_List*
Cyc_yyget_StructFieldDeclListList_tok( void* yy1){ void* _temp1228= yy1; struct
Cyc_List_List* _temp1234; _LL1230: if(*(( void**) _temp1228) == Cyc_StructFieldDeclListList_tok){
_LL1235: _temp1234=( struct Cyc_List_List*)(( struct Cyc_StructFieldDeclListList_tok_struct*)
_temp1228)->f1; goto _LL1231;} else{ goto _LL1232;} _LL1232: goto _LL1233;
_LL1231: return _temp1234; _LL1233:( void) _throw(( void*)& Cyc_yyfail_StructFieldDeclListList_tok);
_LL1229:;} static char _temp1239[ 21u]="TypeModifierList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeModifierList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1239, _temp1239, _temp1239 + 21u}}; struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
void* yy1){ void* _temp1240= yy1; struct Cyc_List_List* _temp1246; _LL1242: if(*((
void**) _temp1240) == Cyc_TypeModifierList_tok){ _LL1247: _temp1246=( struct Cyc_List_List*)((
struct Cyc_TypeModifierList_tok_struct*) _temp1240)->f1; goto _LL1243;} else{
goto _LL1244;} _LL1244: goto _LL1245; _LL1243: return _temp1246; _LL1245:( void)
_throw(( void*)& Cyc_yyfail_TypeModifierList_tok); _LL1241:;} static char
_temp1251[ 8u]="Rgn_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Rgn_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1251, _temp1251, _temp1251 + 8u}};
void* Cyc_yyget_Rgn_tok( void* yy1){ void* _temp1252= yy1; void* _temp1258;
_LL1254: if(*(( void**) _temp1252) == Cyc_Rgn_tok){ _LL1259: _temp1258=( void*)((
struct Cyc_Rgn_tok_struct*) _temp1252)->f1; goto _LL1255;} else{ goto _LL1256;}
_LL1256: goto _LL1257; _LL1255: return _temp1258; _LL1257:( void) _throw(( void*)&
Cyc_yyfail_Rgn_tok); _LL1253:;} static char _temp1263[ 15u]="Declarator_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Declarator_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1263, _temp1263, _temp1263 + 15u}}; struct Cyc_Parse_Declarator*
Cyc_yyget_Declarator_tok( void* yy1){ void* _temp1264= yy1; struct Cyc_Parse_Declarator*
_temp1270; _LL1266: if(*(( void**) _temp1264) == Cyc_Declarator_tok){ _LL1271:
_temp1270=( struct Cyc_Parse_Declarator*)(( struct Cyc_Declarator_tok_struct*)
_temp1264)->f1; goto _LL1267;} else{ goto _LL1268;} _LL1268: goto _LL1269;
_LL1267: return _temp1270; _LL1269:( void) _throw(( void*)& Cyc_yyfail_Declarator_tok);
_LL1265:;} static char _temp1275[ 21u]="DeclaratorExpopt_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpopt_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1275, _temp1275, _temp1275 + 21u}}; struct _tuple13* Cyc_yyget_DeclaratorExpopt_tok(
void* yy1){ void* _temp1276= yy1; struct _tuple13* _temp1282; _LL1278: if(*((
void**) _temp1276) == Cyc_DeclaratorExpopt_tok){ _LL1283: _temp1282=( struct
_tuple13*)(( struct Cyc_DeclaratorExpopt_tok_struct*) _temp1276)->f1; goto
_LL1279;} else{ goto _LL1280;} _LL1280: goto _LL1281; _LL1279: return _temp1282;
_LL1281:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpopt_tok); _LL1277:;}
static char _temp1287[ 25u]="DeclaratorExpoptList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_DeclaratorExpoptList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1287, _temp1287, _temp1287 + 25u}}; struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok(
void* yy1){ void* _temp1288= yy1; struct Cyc_List_List* _temp1294; _LL1290: if(*((
void**) _temp1288) == Cyc_DeclaratorExpoptList_tok){ _LL1295: _temp1294=( struct
Cyc_List_List*)(( struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1288)->f1;
goto _LL1291;} else{ goto _LL1292;} _LL1292: goto _LL1293; _LL1291: return
_temp1294; _LL1293:( void) _throw(( void*)& Cyc_yyfail_DeclaratorExpoptList_tok);
_LL1289:;} static char _temp1299[ 23u]="AbstractDeclarator_tok"; static struct
Cyc_Core_Failure_struct Cyc_yyfail_AbstractDeclarator_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1299, _temp1299, _temp1299 + 23u}}; struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_AbstractDeclarator_tok( void* yy1){ void* _temp1300= yy1; struct Cyc_Parse_Abstractdeclarator*
_temp1306; _LL1302: if(*(( void**) _temp1300) == Cyc_AbstractDeclarator_tok){
_LL1307: _temp1306=( struct Cyc_Parse_Abstractdeclarator*)(( struct Cyc_AbstractDeclarator_tok_struct*)
_temp1300)->f1; goto _LL1303;} else{ goto _LL1304;} _LL1304: goto _LL1305;
_LL1303: return _temp1306; _LL1305:( void) _throw(( void*)& Cyc_yyfail_AbstractDeclarator_tok);
_LL1301:;} static char _temp1311[ 9u]="Bool_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Bool_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1311,
_temp1311, _temp1311 + 9u}}; int Cyc_yyget_Bool_tok( void* yy1){ void* _temp1312=
yy1; int _temp1318; _LL1314: if(*(( void**) _temp1312) == Cyc_Bool_tok){ _LL1319:
_temp1318=( int)(( struct Cyc_Bool_tok_struct*) _temp1312)->f1; goto _LL1315;}
else{ goto _LL1316;} _LL1316: goto _LL1317; _LL1315: return _temp1318; _LL1317:(
void) _throw(( void*)& Cyc_yyfail_Bool_tok); _LL1313:;} static char _temp1323[
10u]="Scope_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Scope_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1323, _temp1323, _temp1323 + 10u}};
void* Cyc_yyget_Scope_tok( void* yy1){ void* _temp1324= yy1; void* _temp1330;
_LL1326: if(*(( void**) _temp1324) == Cyc_Scope_tok){ _LL1331: _temp1330=( void*)((
struct Cyc_Scope_tok_struct*) _temp1324)->f1; goto _LL1327;} else{ goto _LL1328;}
_LL1328: goto _LL1329; _LL1327: return _temp1330; _LL1329:( void) _throw(( void*)&
Cyc_yyfail_Scope_tok); _LL1325:;} static char _temp1335[ 16u]="TunionField_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_TunionField_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1335, _temp1335, _temp1335 + 16u}}; struct Cyc_Absyn_Tunionfield*
Cyc_yyget_TunionField_tok( void* yy1){ void* _temp1336= yy1; struct Cyc_Absyn_Tunionfield*
_temp1342; _LL1338: if(*(( void**) _temp1336) == Cyc_TunionField_tok){ _LL1343:
_temp1342=( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_TunionField_tok_struct*)
_temp1336)->f1; goto _LL1339;} else{ goto _LL1340;} _LL1340: goto _LL1341;
_LL1339: return _temp1342; _LL1341:( void) _throw(( void*)& Cyc_yyfail_TunionField_tok);
_LL1337:;} static char _temp1347[ 20u]="TunionFieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TunionFieldList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1347, _temp1347, _temp1347 + 20u}}; struct Cyc_List_List* Cyc_yyget_TunionFieldList_tok(
void* yy1){ void* _temp1348= yy1; struct Cyc_List_List* _temp1354; _LL1350: if(*((
void**) _temp1348) == Cyc_TunionFieldList_tok){ _LL1355: _temp1354=( struct Cyc_List_List*)((
struct Cyc_TunionFieldList_tok_struct*) _temp1348)->f1; goto _LL1351;} else{
goto _LL1352;} _LL1352: goto _LL1353; _LL1351: return _temp1354; _LL1353:( void)
_throw(( void*)& Cyc_yyfail_TunionFieldList_tok); _LL1349:;} static char
_temp1359[ 17u]="QualSpecList_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_QualSpecList_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1359, _temp1359, _temp1359 + 17u}};
struct _tuple18* Cyc_yyget_QualSpecList_tok( void* yy1){ void* _temp1360= yy1;
struct _tuple18* _temp1366; _LL1362: if(*(( void**) _temp1360) == Cyc_QualSpecList_tok){
_LL1367: _temp1366=( struct _tuple18*)(( struct Cyc_QualSpecList_tok_struct*)
_temp1360)->f1; goto _LL1363;} else{ goto _LL1364;} _LL1364: goto _LL1365;
_LL1363: return _temp1366; _LL1365:( void) _throw(( void*)& Cyc_yyfail_QualSpecList_tok);
_LL1361:;} static char _temp1371[ 11u]="IdList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_IdList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1371,
_temp1371, _temp1371 + 11u}}; struct Cyc_List_List* Cyc_yyget_IdList_tok( void*
yy1){ void* _temp1372= yy1; struct Cyc_List_List* _temp1378; _LL1374: if(*((
void**) _temp1372) == Cyc_IdList_tok){ _LL1379: _temp1378=( struct Cyc_List_List*)((
struct Cyc_IdList_tok_struct*) _temp1372)->f1; goto _LL1375;} else{ goto _LL1376;}
_LL1376: goto _LL1377; _LL1375: return _temp1378; _LL1377:( void) _throw(( void*)&
Cyc_yyfail_IdList_tok); _LL1373:;} static char _temp1383[ 14u]="ParamDecl_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDecl_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1383, _temp1383, _temp1383 + 14u}}; struct _tuple2*
Cyc_yyget_ParamDecl_tok( void* yy1){ void* _temp1384= yy1; struct _tuple2*
_temp1390; _LL1386: if(*(( void**) _temp1384) == Cyc_ParamDecl_tok){ _LL1391:
_temp1390=( struct _tuple2*)(( struct Cyc_ParamDecl_tok_struct*) _temp1384)->f1;
goto _LL1387;} else{ goto _LL1388;} _LL1388: goto _LL1389; _LL1387: return
_temp1390; _LL1389:( void) _throw(( void*)& Cyc_yyfail_ParamDecl_tok); _LL1385:;}
static char _temp1395[ 18u]="ParamDeclList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_ParamDeclList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1395, _temp1395, _temp1395 + 18u}}; struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
void* yy1){ void* _temp1396= yy1; struct Cyc_List_List* _temp1402; _LL1398: if(*((
void**) _temp1396) == Cyc_ParamDeclList_tok){ _LL1403: _temp1402=( struct Cyc_List_List*)((
struct Cyc_ParamDeclList_tok_struct*) _temp1396)->f1; goto _LL1399;} else{ goto
_LL1400;} _LL1400: goto _LL1401; _LL1399: return _temp1402; _LL1401:( void)
_throw(( void*)& Cyc_yyfail_ParamDeclList_tok); _LL1397:;} static char _temp1407[
22u]="ParamDeclListBool_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_ParamDeclListBool_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1407, _temp1407, _temp1407 + 22u}};
struct _tuple17* Cyc_yyget_ParamDeclListBool_tok( void* yy1){ void* _temp1408=
yy1; struct _tuple17* _temp1414; _LL1410: if(*(( void**) _temp1408) == Cyc_ParamDeclListBool_tok){
_LL1415: _temp1414=( struct _tuple17*)(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp1408)->f1; goto _LL1411;} else{ goto _LL1412;} _LL1412: goto _LL1413;
_LL1411: return _temp1414; _LL1413:( void) _throw(( void*)& Cyc_yyfail_ParamDeclListBool_tok);
_LL1409:;} static char _temp1419[ 13u]="TypeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_TypeList_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1419,
_temp1419, _temp1419 + 13u}}; struct Cyc_List_List* Cyc_yyget_TypeList_tok( void*
yy1){ void* _temp1420= yy1; struct Cyc_List_List* _temp1426; _LL1422: if(*((
void**) _temp1420) == Cyc_TypeList_tok){ _LL1427: _temp1426=( struct Cyc_List_List*)((
struct Cyc_TypeList_tok_struct*) _temp1420)->f1; goto _LL1423;} else{ goto
_LL1424;} _LL1424: goto _LL1425; _LL1423: return _temp1426; _LL1425:( void)
_throw(( void*)& Cyc_yyfail_TypeList_tok); _LL1421:;} static char _temp1431[ 19u]="DesignatorList_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_DesignatorList_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1431, _temp1431, _temp1431 + 19u}}; struct Cyc_List_List*
Cyc_yyget_DesignatorList_tok( void* yy1){ void* _temp1432= yy1; struct Cyc_List_List*
_temp1438; _LL1434: if(*(( void**) _temp1432) == Cyc_DesignatorList_tok){
_LL1439: _temp1438=( struct Cyc_List_List*)(( struct Cyc_DesignatorList_tok_struct*)
_temp1432)->f1; goto _LL1435;} else{ goto _LL1436;} _LL1436: goto _LL1437;
_LL1435: return _temp1438; _LL1437:( void) _throw(( void*)& Cyc_yyfail_DesignatorList_tok);
_LL1433:;} static char _temp1443[ 15u]="Designator_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_Designator_tok={ Cyc_Core_Failure,( struct _tagged_string){ _temp1443,
_temp1443, _temp1443 + 15u}}; void* Cyc_yyget_Designator_tok( void* yy1){ void*
_temp1444= yy1; void* _temp1450; _LL1446: if(*(( void**) _temp1444) == Cyc_Designator_tok){
_LL1451: _temp1450=( void*)(( struct Cyc_Designator_tok_struct*) _temp1444)->f1;
goto _LL1447;} else{ goto _LL1448;} _LL1448: goto _LL1449; _LL1447: return
_temp1450; _LL1449:( void) _throw(( void*)& Cyc_yyfail_Designator_tok); _LL1445:;}
static char _temp1455[ 9u]="Kind_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Kind_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1455, _temp1455, _temp1455 + 9u}};
void* Cyc_yyget_Kind_tok( void* yy1){ void* _temp1456= yy1; void* _temp1462;
_LL1458: if(*(( void**) _temp1456) == Cyc_Kind_tok){ _LL1463: _temp1462=( void*)((
struct Cyc_Kind_tok_struct*) _temp1456)->f1; goto _LL1459;} else{ goto _LL1460;}
_LL1460: goto _LL1461; _LL1459: return _temp1462; _LL1461:( void) _throw(( void*)&
Cyc_yyfail_Kind_tok); _LL1457:;} static char _temp1467[ 9u]="Type_tok"; static
struct Cyc_Core_Failure_struct Cyc_yyfail_Type_tok={ Cyc_Core_Failure,( struct
_tagged_string){ _temp1467, _temp1467, _temp1467 + 9u}}; void* Cyc_yyget_Type_tok(
void* yy1){ void* _temp1468= yy1; void* _temp1474; _LL1470: if(*(( void**)
_temp1468) == Cyc_Type_tok){ _LL1475: _temp1474=( void*)(( struct Cyc_Type_tok_struct*)
_temp1468)->f1; goto _LL1471;} else{ goto _LL1472;} _LL1472: goto _LL1473;
_LL1471: return _temp1474; _LL1473:( void) _throw(( void*)& Cyc_yyfail_Type_tok);
_LL1469:;} static char _temp1479[ 18u]="AttributeList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_AttributeList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1479, _temp1479, _temp1479 + 18u}}; struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
void* yy1){ void* _temp1480= yy1; struct Cyc_List_List* _temp1486; _LL1482: if(*((
void**) _temp1480) == Cyc_AttributeList_tok){ _LL1487: _temp1486=( struct Cyc_List_List*)((
struct Cyc_AttributeList_tok_struct*) _temp1480)->f1; goto _LL1483;} else{ goto
_LL1484;} _LL1484: goto _LL1485; _LL1483: return _temp1486; _LL1485:( void)
_throw(( void*)& Cyc_yyfail_AttributeList_tok); _LL1481:;} static char _temp1491[
14u]="Attribute_tok"; static struct Cyc_Core_Failure_struct Cyc_yyfail_Attribute_tok={
Cyc_Core_Failure,( struct _tagged_string){ _temp1491, _temp1491, _temp1491 + 14u}};
void* Cyc_yyget_Attribute_tok( void* yy1){ void* _temp1492= yy1; void* _temp1498;
_LL1494: if(*(( void**) _temp1492) == Cyc_Attribute_tok){ _LL1499: _temp1498=(
void*)(( struct Cyc_Attribute_tok_struct*) _temp1492)->f1; goto _LL1495;} else{
goto _LL1496;} _LL1496: goto _LL1497; _LL1495: return _temp1498; _LL1497:( void)
_throw(( void*)& Cyc_yyfail_Attribute_tok); _LL1493:;} static char _temp1503[ 14u]="Enumfield_tok";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Enumfield_tok={ Cyc_Core_Failure,(
struct _tagged_string){ _temp1503, _temp1503, _temp1503 + 14u}}; struct Cyc_Absyn_Enumfield*
Cyc_yyget_Enumfield_tok( void* yy1){ void* _temp1504= yy1; struct Cyc_Absyn_Enumfield*
_temp1510; _LL1506: if(*(( void**) _temp1504) == Cyc_Enumfield_tok){ _LL1511:
_temp1510=( struct Cyc_Absyn_Enumfield*)(( struct Cyc_Enumfield_tok_struct*)
_temp1504)->f1; goto _LL1507;} else{ goto _LL1508;} _LL1508: goto _LL1509;
_LL1507: return _temp1510; _LL1509:( void) _throw(( void*)& Cyc_yyfail_Enumfield_tok);
_LL1505:;} static char _temp1515[ 18u]="EnumfieldList_tok"; static struct Cyc_Core_Failure_struct
Cyc_yyfail_EnumfieldList_tok={ Cyc_Core_Failure,( struct _tagged_string){
_temp1515, _temp1515, _temp1515 + 18u}}; struct Cyc_List_List* Cyc_yyget_EnumfieldList_tok(
void* yy1){ void* _temp1516= yy1; struct Cyc_List_List* _temp1522; _LL1518: if(*((
void**) _temp1516) == Cyc_EnumfieldList_tok){ _LL1523: _temp1522=( struct Cyc_List_List*)((
struct Cyc_EnumfieldList_tok_struct*) _temp1516)->f1; goto _LL1519;} else{ goto
_LL1520;} _LL1520: goto _LL1521; _LL1519: return _temp1522; _LL1521:( void)
_throw(( void*)& Cyc_yyfail_EnumfieldList_tok); _LL1517:;} struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
struct _tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct
Cyc_Yyltype Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1524; _temp1524.timestamp=
0; _temp1524.first_line= 0; _temp1524.first_column= 0; _temp1524.last_line= 0;
_temp1524.last_column= 0; _temp1524.text=( struct _tagged_string)({ char*
_temp1525=( char*)""; struct _tagged_string _temp1526; _temp1526.curr= _temp1525;
_temp1526.base= _temp1525; _temp1526.last_plus_one= _temp1525 + 1; _temp1526;});
_temp1524;});} static char _temp1529[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc=(
struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column= 0,.last_line= 0,.last_column=
0,.text=( struct _tagged_string){ _temp1529, _temp1529, _temp1529 + 1u}}; static
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
(short)93, (short)94, (short)95, (short)96}; static char _temp1532[ 2u]="$";
static char _temp1535[ 6u]="error"; static char _temp1538[ 12u]="$undefined.";
static char _temp1541[ 5u]="AUTO"; static char _temp1544[ 9u]="REGISTER"; static
char _temp1547[ 7u]="STATIC"; static char _temp1550[ 7u]="EXTERN"; static char
_temp1553[ 8u]="TYPEDEF"; static char _temp1556[ 5u]="VOID"; static char
_temp1559[ 5u]="CHAR"; static char _temp1562[ 6u]="SHORT"; static char _temp1565[
4u]="INT"; static char _temp1568[ 5u]="LONG"; static char _temp1571[ 6u]="FLOAT";
static char _temp1574[ 7u]="DOUBLE"; static char _temp1577[ 7u]="SIGNED"; static
char _temp1580[ 9u]="UNSIGNED"; static char _temp1583[ 6u]="CONST"; static char
_temp1586[ 9u]="VOLATILE"; static char _temp1589[ 9u]="RESTRICT"; static char
_temp1592[ 7u]="STRUCT"; static char _temp1595[ 6u]="UNION"; static char
_temp1598[ 5u]="CASE"; static char _temp1601[ 8u]="DEFAULT"; static char
_temp1604[ 7u]="INLINE"; static char _temp1607[ 3u]="IF"; static char _temp1610[
5u]="ELSE"; static char _temp1613[ 7u]="SWITCH"; static char _temp1616[ 6u]="WHILE";
static char _temp1619[ 3u]="DO"; static char _temp1622[ 4u]="FOR"; static char
_temp1625[ 5u]="GOTO"; static char _temp1628[ 9u]="CONTINUE"; static char
_temp1631[ 6u]="BREAK"; static char _temp1634[ 7u]="RETURN"; static char
_temp1637[ 7u]="SIZEOF"; static char _temp1640[ 5u]="ENUM"; static char
_temp1643[ 8u]="NULL_kw"; static char _temp1646[ 4u]="LET"; static char
_temp1649[ 6u]="THROW"; static char _temp1652[ 4u]="TRY"; static char _temp1655[
6u]="CATCH"; static char _temp1658[ 4u]="NEW"; static char _temp1661[ 9u]="ABSTRACT";
static char _temp1664[ 9u]="FALLTHRU"; static char _temp1667[ 6u]="USING";
static char _temp1670[ 10u]="NAMESPACE"; static char _temp1673[ 7u]="TUNION";
static char _temp1676[ 8u]="XTUNION"; static char _temp1679[ 5u]="FILL"; static
char _temp1682[ 8u]="CODEGEN"; static char _temp1685[ 4u]="CUT"; static char
_temp1688[ 7u]="SPLICE"; static char _temp1691[ 7u]="PRINTF"; static char
_temp1694[ 8u]="FPRINTF"; static char _temp1697[ 8u]="XPRINTF"; static char
_temp1700[ 6u]="SCANF"; static char _temp1703[ 7u]="FSCANF"; static char
_temp1706[ 7u]="SSCANF"; static char _temp1709[ 7u]="MALLOC"; static char
_temp1712[ 9u]="REGION_T"; static char _temp1715[ 7u]="REGION"; static char
_temp1718[ 5u]="RNEW"; static char _temp1721[ 8u]="RMALLOC"; static char
_temp1724[ 7u]="PTR_OP"; static char _temp1727[ 7u]="INC_OP"; static char
_temp1730[ 7u]="DEC_OP"; static char _temp1733[ 8u]="LEFT_OP"; static char
_temp1736[ 9u]="RIGHT_OP"; static char _temp1739[ 6u]="LE_OP"; static char
_temp1742[ 6u]="GE_OP"; static char _temp1745[ 6u]="EQ_OP"; static char
_temp1748[ 6u]="NE_OP"; static char _temp1751[ 7u]="AND_OP"; static char
_temp1754[ 6u]="OR_OP"; static char _temp1757[ 11u]="MUL_ASSIGN"; static char
_temp1760[ 11u]="DIV_ASSIGN"; static char _temp1763[ 11u]="MOD_ASSIGN"; static
char _temp1766[ 11u]="ADD_ASSIGN"; static char _temp1769[ 11u]="SUB_ASSIGN";
static char _temp1772[ 12u]="LEFT_ASSIGN"; static char _temp1775[ 13u]="RIGHT_ASSIGN";
static char _temp1778[ 11u]="AND_ASSIGN"; static char _temp1781[ 11u]="XOR_ASSIGN";
static char _temp1784[ 10u]="OR_ASSIGN"; static char _temp1787[ 9u]="ELLIPSIS";
static char _temp1790[ 11u]="LEFT_RIGHT"; static char _temp1793[ 12u]="COLON_COLON";
static char _temp1796[ 11u]="IDENTIFIER"; static char _temp1799[ 17u]="INTEGER_CONSTANT";
static char _temp1802[ 7u]="STRING"; static char _temp1805[ 19u]="CHARACTER_CONSTANT";
static char _temp1808[ 18u]="FLOATING_CONSTANT"; static char _temp1811[ 9u]="TYPE_VAR";
static char _temp1814[ 16u]="QUAL_IDENTIFIER"; static char _temp1817[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1820[ 10u]="ATTRIBUTE"; static char _temp1823[ 4u]="';'";
static char _temp1826[ 4u]="'{'"; static char _temp1829[ 4u]="'}'"; static char
_temp1832[ 4u]="'='"; static char _temp1835[ 4u]="'('"; static char _temp1838[ 4u]="')'";
static char _temp1841[ 4u]="','"; static char _temp1844[ 4u]="'_'"; static char
_temp1847[ 4u]="'$'"; static char _temp1850[ 4u]="'<'"; static char _temp1853[ 4u]="'>'";
static char _temp1856[ 4u]="':'"; static char _temp1859[ 4u]="'.'"; static char
_temp1862[ 4u]="'['"; static char _temp1865[ 4u]="']'"; static char _temp1868[ 4u]="'*'";
static char _temp1871[ 4u]="'@'"; static char _temp1874[ 4u]="'?'"; static char
_temp1877[ 4u]="'+'"; static char _temp1880[ 4u]="'-'"; static char _temp1883[ 4u]="'&'";
static char _temp1886[ 4u]="'|'"; static char _temp1889[ 4u]="'^'"; static char
_temp1892[ 4u]="'/'"; static char _temp1895[ 4u]="'%'"; static char _temp1898[ 4u]="'~'";
static char _temp1901[ 4u]="'!'"; static char _temp1904[ 5u]="prog"; static char
_temp1907[ 17u]="translation_unit"; static char _temp1910[ 21u]="external_declaration";
static char _temp1913[ 20u]="function_definition"; static char _temp1916[ 21u]="function_definition2";
static char _temp1919[ 13u]="using_action"; static char _temp1922[ 15u]="unusing_action";
static char _temp1925[ 17u]="namespace_action"; static char _temp1928[ 19u]="unnamespace_action";
static char _temp1931[ 12u]="declaration"; static char _temp1934[ 17u]="declaration_list";
static char _temp1937[ 23u]="declaration_specifiers"; static char _temp1940[ 24u]="storage_class_specifier";
static char _temp1943[ 15u]="attributes_opt"; static char _temp1946[ 11u]="attributes";
static char _temp1949[ 15u]="attribute_list"; static char _temp1952[ 10u]="attribute";
static char _temp1955[ 15u]="type_specifier"; static char _temp1958[ 5u]="kind";
static char _temp1961[ 15u]="type_qualifier"; static char _temp1964[ 15u]="enum_specifier";
static char _temp1967[ 11u]="enum_field"; static char _temp1970[ 22u]="enum_declaration_list";
static char _temp1973[ 26u]="struct_or_union_specifier"; static char _temp1976[
16u]="type_params_opt"; static char _temp1979[ 16u]="struct_or_union"; static
char _temp1982[ 24u]="struct_declaration_list"; static char _temp1985[ 25u]="struct_declaration_list0";
static char _temp1988[ 21u]="init_declarator_list"; static char _temp1991[ 22u]="init_declarator_list0";
static char _temp1994[ 16u]="init_declarator"; static char _temp1997[ 19u]="struct_declaration";
static char _temp2000[ 25u]="specifier_qualifier_list"; static char _temp2003[
23u]="struct_declarator_list"; static char _temp2006[ 24u]="struct_declarator_list0";
static char _temp2009[ 18u]="struct_declarator"; static char _temp2012[ 17u]="tunion_specifier";
static char _temp2015[ 18u]="tunion_or_xtunion"; static char _temp2018[ 17u]="tunionfield_list";
static char _temp2021[ 18u]="tunionfield_scope"; static char _temp2024[ 12u]="tunionfield";
static char _temp2027[ 11u]="declarator"; static char _temp2030[ 18u]="direct_declarator";
static char _temp2033[ 8u]="pointer"; static char _temp2036[ 13u]="pointer_char";
static char _temp2039[ 8u]="rgn_opt"; static char _temp2042[ 4u]="rgn"; static
char _temp2045[ 20u]="type_qualifier_list"; static char _temp2048[ 20u]="parameter_type_list";
static char _temp2051[ 11u]="effect_set"; static char _temp2054[ 14u]="atomic_effect";
static char _temp2057[ 11u]="region_set"; static char _temp2060[ 15u]="parameter_list";
static char _temp2063[ 22u]="parameter_declaration"; static char _temp2066[ 16u]="identifier_list";
static char _temp2069[ 17u]="identifier_list0"; static char _temp2072[ 12u]="initializer";
static char _temp2075[ 18u]="array_initializer"; static char _temp2078[ 17u]="initializer_list";
static char _temp2081[ 12u]="designation"; static char _temp2084[ 16u]="designator_list";
static char _temp2087[ 11u]="designator"; static char _temp2090[ 10u]="type_name";
static char _temp2093[ 14u]="any_type_name"; static char _temp2096[ 15u]="type_name_list";
static char _temp2099[ 20u]="abstract_declarator"; static char _temp2102[ 27u]="direct_abstract_declarator";
static char _temp2105[ 10u]="statement"; static char _temp2108[ 18u]="labeled_statement";
static char _temp2111[ 21u]="expression_statement"; static char _temp2114[ 19u]="compound_statement";
static char _temp2117[ 16u]="block_item_list"; static char _temp2120[ 11u]="block_item";
static char _temp2123[ 20u]="selection_statement"; static char _temp2126[ 15u]="switch_clauses";
static char _temp2129[ 20u]="iteration_statement"; static char _temp2132[ 15u]="jump_statement";
static char _temp2135[ 8u]="pattern"; static char _temp2138[ 19u]="tuple_pattern_list";
static char _temp2141[ 20u]="tuple_pattern_list0"; static char _temp2144[ 14u]="field_pattern";
static char _temp2147[ 19u]="field_pattern_list"; static char _temp2150[ 20u]="field_pattern_list0";
static char _temp2153[ 11u]="expression"; static char _temp2156[ 22u]="assignment_expression";
static char _temp2159[ 20u]="assignment_operator"; static char _temp2162[ 23u]="conditional_expression";
static char _temp2165[ 20u]="constant_expression"; static char _temp2168[ 22u]="logical_or_expression";
static char _temp2171[ 23u]="logical_and_expression"; static char _temp2174[ 24u]="inclusive_or_expression";
static char _temp2177[ 24u]="exclusive_or_expression"; static char _temp2180[ 15u]="and_expression";
static char _temp2183[ 20u]="equality_expression"; static char _temp2186[ 22u]="relational_expression";
static char _temp2189[ 17u]="shift_expression"; static char _temp2192[ 20u]="additive_expression";
static char _temp2195[ 26u]="multiplicative_expression"; static char _temp2198[
16u]="cast_expression"; static char _temp2201[ 17u]="unary_expression"; static
char _temp2204[ 14u]="format_primop"; static char _temp2207[ 15u]="unary_operator";
static char _temp2210[ 19u]="postfix_expression"; static char _temp2213[ 19u]="primary_expression";
static char _temp2216[ 25u]="argument_expression_list"; static char _temp2219[
26u]="argument_expression_list0"; static char _temp2222[ 9u]="constant"; static
char _temp2225[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
232u]={( struct _tagged_string){ _temp1532, _temp1532, _temp1532 + 2u},( struct
_tagged_string){ _temp1535, _temp1535, _temp1535 + 6u},( struct _tagged_string){
_temp1538, _temp1538, _temp1538 + 12u},( struct _tagged_string){ _temp1541,
_temp1541, _temp1541 + 5u},( struct _tagged_string){ _temp1544, _temp1544,
_temp1544 + 9u},( struct _tagged_string){ _temp1547, _temp1547, _temp1547 + 7u},(
struct _tagged_string){ _temp1550, _temp1550, _temp1550 + 7u},( struct
_tagged_string){ _temp1553, _temp1553, _temp1553 + 8u},( struct _tagged_string){
_temp1556, _temp1556, _temp1556 + 5u},( struct _tagged_string){ _temp1559,
_temp1559, _temp1559 + 5u},( struct _tagged_string){ _temp1562, _temp1562,
_temp1562 + 6u},( struct _tagged_string){ _temp1565, _temp1565, _temp1565 + 4u},(
struct _tagged_string){ _temp1568, _temp1568, _temp1568 + 5u},( struct
_tagged_string){ _temp1571, _temp1571, _temp1571 + 6u},( struct _tagged_string){
_temp1574, _temp1574, _temp1574 + 7u},( struct _tagged_string){ _temp1577,
_temp1577, _temp1577 + 7u},( struct _tagged_string){ _temp1580, _temp1580,
_temp1580 + 9u},( struct _tagged_string){ _temp1583, _temp1583, _temp1583 + 6u},(
struct _tagged_string){ _temp1586, _temp1586, _temp1586 + 9u},( struct
_tagged_string){ _temp1589, _temp1589, _temp1589 + 9u},( struct _tagged_string){
_temp1592, _temp1592, _temp1592 + 7u},( struct _tagged_string){ _temp1595,
_temp1595, _temp1595 + 6u},( struct _tagged_string){ _temp1598, _temp1598,
_temp1598 + 5u},( struct _tagged_string){ _temp1601, _temp1601, _temp1601 + 8u},(
struct _tagged_string){ _temp1604, _temp1604, _temp1604 + 7u},( struct
_tagged_string){ _temp1607, _temp1607, _temp1607 + 3u},( struct _tagged_string){
_temp1610, _temp1610, _temp1610 + 5u},( struct _tagged_string){ _temp1613,
_temp1613, _temp1613 + 7u},( struct _tagged_string){ _temp1616, _temp1616,
_temp1616 + 6u},( struct _tagged_string){ _temp1619, _temp1619, _temp1619 + 3u},(
struct _tagged_string){ _temp1622, _temp1622, _temp1622 + 4u},( struct
_tagged_string){ _temp1625, _temp1625, _temp1625 + 5u},( struct _tagged_string){
_temp1628, _temp1628, _temp1628 + 9u},( struct _tagged_string){ _temp1631,
_temp1631, _temp1631 + 6u},( struct _tagged_string){ _temp1634, _temp1634,
_temp1634 + 7u},( struct _tagged_string){ _temp1637, _temp1637, _temp1637 + 7u},(
struct _tagged_string){ _temp1640, _temp1640, _temp1640 + 5u},( struct
_tagged_string){ _temp1643, _temp1643, _temp1643 + 8u},( struct _tagged_string){
_temp1646, _temp1646, _temp1646 + 4u},( struct _tagged_string){ _temp1649,
_temp1649, _temp1649 + 6u},( struct _tagged_string){ _temp1652, _temp1652,
_temp1652 + 4u},( struct _tagged_string){ _temp1655, _temp1655, _temp1655 + 6u},(
struct _tagged_string){ _temp1658, _temp1658, _temp1658 + 4u},( struct
_tagged_string){ _temp1661, _temp1661, _temp1661 + 9u},( struct _tagged_string){
_temp1664, _temp1664, _temp1664 + 9u},( struct _tagged_string){ _temp1667,
_temp1667, _temp1667 + 6u},( struct _tagged_string){ _temp1670, _temp1670,
_temp1670 + 10u},( struct _tagged_string){ _temp1673, _temp1673, _temp1673 + 7u},(
struct _tagged_string){ _temp1676, _temp1676, _temp1676 + 8u},( struct
_tagged_string){ _temp1679, _temp1679, _temp1679 + 5u},( struct _tagged_string){
_temp1682, _temp1682, _temp1682 + 8u},( struct _tagged_string){ _temp1685,
_temp1685, _temp1685 + 4u},( struct _tagged_string){ _temp1688, _temp1688,
_temp1688 + 7u},( struct _tagged_string){ _temp1691, _temp1691, _temp1691 + 7u},(
struct _tagged_string){ _temp1694, _temp1694, _temp1694 + 8u},( struct
_tagged_string){ _temp1697, _temp1697, _temp1697 + 8u},( struct _tagged_string){
_temp1700, _temp1700, _temp1700 + 6u},( struct _tagged_string){ _temp1703,
_temp1703, _temp1703 + 7u},( struct _tagged_string){ _temp1706, _temp1706,
_temp1706 + 7u},( struct _tagged_string){ _temp1709, _temp1709, _temp1709 + 7u},(
struct _tagged_string){ _temp1712, _temp1712, _temp1712 + 9u},( struct
_tagged_string){ _temp1715, _temp1715, _temp1715 + 7u},( struct _tagged_string){
_temp1718, _temp1718, _temp1718 + 5u},( struct _tagged_string){ _temp1721,
_temp1721, _temp1721 + 8u},( struct _tagged_string){ _temp1724, _temp1724,
_temp1724 + 7u},( struct _tagged_string){ _temp1727, _temp1727, _temp1727 + 7u},(
struct _tagged_string){ _temp1730, _temp1730, _temp1730 + 7u},( struct
_tagged_string){ _temp1733, _temp1733, _temp1733 + 8u},( struct _tagged_string){
_temp1736, _temp1736, _temp1736 + 9u},( struct _tagged_string){ _temp1739,
_temp1739, _temp1739 + 6u},( struct _tagged_string){ _temp1742, _temp1742,
_temp1742 + 6u},( struct _tagged_string){ _temp1745, _temp1745, _temp1745 + 6u},(
struct _tagged_string){ _temp1748, _temp1748, _temp1748 + 6u},( struct
_tagged_string){ _temp1751, _temp1751, _temp1751 + 7u},( struct _tagged_string){
_temp1754, _temp1754, _temp1754 + 6u},( struct _tagged_string){ _temp1757,
_temp1757, _temp1757 + 11u},( struct _tagged_string){ _temp1760, _temp1760,
_temp1760 + 11u},( struct _tagged_string){ _temp1763, _temp1763, _temp1763 + 11u},(
struct _tagged_string){ _temp1766, _temp1766, _temp1766 + 11u},( struct
_tagged_string){ _temp1769, _temp1769, _temp1769 + 11u},( struct _tagged_string){
_temp1772, _temp1772, _temp1772 + 12u},( struct _tagged_string){ _temp1775,
_temp1775, _temp1775 + 13u},( struct _tagged_string){ _temp1778, _temp1778,
_temp1778 + 11u},( struct _tagged_string){ _temp1781, _temp1781, _temp1781 + 11u},(
struct _tagged_string){ _temp1784, _temp1784, _temp1784 + 10u},( struct
_tagged_string){ _temp1787, _temp1787, _temp1787 + 9u},( struct _tagged_string){
_temp1790, _temp1790, _temp1790 + 11u},( struct _tagged_string){ _temp1793,
_temp1793, _temp1793 + 12u},( struct _tagged_string){ _temp1796, _temp1796,
_temp1796 + 11u},( struct _tagged_string){ _temp1799, _temp1799, _temp1799 + 17u},(
struct _tagged_string){ _temp1802, _temp1802, _temp1802 + 7u},( struct
_tagged_string){ _temp1805, _temp1805, _temp1805 + 19u},( struct _tagged_string){
_temp1808, _temp1808, _temp1808 + 18u},( struct _tagged_string){ _temp1811,
_temp1811, _temp1811 + 9u},( struct _tagged_string){ _temp1814, _temp1814,
_temp1814 + 16u},( struct _tagged_string){ _temp1817, _temp1817, _temp1817 + 18u},(
struct _tagged_string){ _temp1820, _temp1820, _temp1820 + 10u},( struct
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
_temp1868 + 4u},( struct _tagged_string){ _temp1871, _temp1871, _temp1871 + 4u},(
struct _tagged_string){ _temp1874, _temp1874, _temp1874 + 4u},( struct
_tagged_string){ _temp1877, _temp1877, _temp1877 + 4u},( struct _tagged_string){
_temp1880, _temp1880, _temp1880 + 4u},( struct _tagged_string){ _temp1883,
_temp1883, _temp1883 + 4u},( struct _tagged_string){ _temp1886, _temp1886,
_temp1886 + 4u},( struct _tagged_string){ _temp1889, _temp1889, _temp1889 + 4u},(
struct _tagged_string){ _temp1892, _temp1892, _temp1892 + 4u},( struct
_tagged_string){ _temp1895, _temp1895, _temp1895 + 4u},( struct _tagged_string){
_temp1898, _temp1898, _temp1898 + 4u},( struct _tagged_string){ _temp1901,
_temp1901, _temp1901 + 4u},( struct _tagged_string){ _temp1904, _temp1904,
_temp1904 + 5u},( struct _tagged_string){ _temp1907, _temp1907, _temp1907 + 17u},(
struct _tagged_string){ _temp1910, _temp1910, _temp1910 + 21u},( struct
_tagged_string){ _temp1913, _temp1913, _temp1913 + 20u},( struct _tagged_string){
_temp1916, _temp1916, _temp1916 + 21u},( struct _tagged_string){ _temp1919,
_temp1919, _temp1919 + 13u},( struct _tagged_string){ _temp1922, _temp1922,
_temp1922 + 15u},( struct _tagged_string){ _temp1925, _temp1925, _temp1925 + 17u},(
struct _tagged_string){ _temp1928, _temp1928, _temp1928 + 19u},( struct
_tagged_string){ _temp1931, _temp1931, _temp1931 + 12u},( struct _tagged_string){
_temp1934, _temp1934, _temp1934 + 17u},( struct _tagged_string){ _temp1937,
_temp1937, _temp1937 + 23u},( struct _tagged_string){ _temp1940, _temp1940,
_temp1940 + 24u},( struct _tagged_string){ _temp1943, _temp1943, _temp1943 + 15u},(
struct _tagged_string){ _temp1946, _temp1946, _temp1946 + 11u},( struct
_tagged_string){ _temp1949, _temp1949, _temp1949 + 15u},( struct _tagged_string){
_temp1952, _temp1952, _temp1952 + 10u},( struct _tagged_string){ _temp1955,
_temp1955, _temp1955 + 15u},( struct _tagged_string){ _temp1958, _temp1958,
_temp1958 + 5u},( struct _tagged_string){ _temp1961, _temp1961, _temp1961 + 15u},(
struct _tagged_string){ _temp1964, _temp1964, _temp1964 + 15u},( struct
_tagged_string){ _temp1967, _temp1967, _temp1967 + 11u},( struct _tagged_string){
_temp1970, _temp1970, _temp1970 + 22u},( struct _tagged_string){ _temp1973,
_temp1973, _temp1973 + 26u},( struct _tagged_string){ _temp1976, _temp1976,
_temp1976 + 16u},( struct _tagged_string){ _temp1979, _temp1979, _temp1979 + 16u},(
struct _tagged_string){ _temp1982, _temp1982, _temp1982 + 24u},( struct
_tagged_string){ _temp1985, _temp1985, _temp1985 + 25u},( struct _tagged_string){
_temp1988, _temp1988, _temp1988 + 21u},( struct _tagged_string){ _temp1991,
_temp1991, _temp1991 + 22u},( struct _tagged_string){ _temp1994, _temp1994,
_temp1994 + 16u},( struct _tagged_string){ _temp1997, _temp1997, _temp1997 + 19u},(
struct _tagged_string){ _temp2000, _temp2000, _temp2000 + 25u},( struct
_tagged_string){ _temp2003, _temp2003, _temp2003 + 23u},( struct _tagged_string){
_temp2006, _temp2006, _temp2006 + 24u},( struct _tagged_string){ _temp2009,
_temp2009, _temp2009 + 18u},( struct _tagged_string){ _temp2012, _temp2012,
_temp2012 + 17u},( struct _tagged_string){ _temp2015, _temp2015, _temp2015 + 18u},(
struct _tagged_string){ _temp2018, _temp2018, _temp2018 + 17u},( struct
_tagged_string){ _temp2021, _temp2021, _temp2021 + 18u},( struct _tagged_string){
_temp2024, _temp2024, _temp2024 + 12u},( struct _tagged_string){ _temp2027,
_temp2027, _temp2027 + 11u},( struct _tagged_string){ _temp2030, _temp2030,
_temp2030 + 18u},( struct _tagged_string){ _temp2033, _temp2033, _temp2033 + 8u},(
struct _tagged_string){ _temp2036, _temp2036, _temp2036 + 13u},( struct
_tagged_string){ _temp2039, _temp2039, _temp2039 + 8u},( struct _tagged_string){
_temp2042, _temp2042, _temp2042 + 4u},( struct _tagged_string){ _temp2045,
_temp2045, _temp2045 + 20u},( struct _tagged_string){ _temp2048, _temp2048,
_temp2048 + 20u},( struct _tagged_string){ _temp2051, _temp2051, _temp2051 + 11u},(
struct _tagged_string){ _temp2054, _temp2054, _temp2054 + 14u},( struct
_tagged_string){ _temp2057, _temp2057, _temp2057 + 11u},( struct _tagged_string){
_temp2060, _temp2060, _temp2060 + 15u},( struct _tagged_string){ _temp2063,
_temp2063, _temp2063 + 22u},( struct _tagged_string){ _temp2066, _temp2066,
_temp2066 + 16u},( struct _tagged_string){ _temp2069, _temp2069, _temp2069 + 17u},(
struct _tagged_string){ _temp2072, _temp2072, _temp2072 + 12u},( struct
_tagged_string){ _temp2075, _temp2075, _temp2075 + 18u},( struct _tagged_string){
_temp2078, _temp2078, _temp2078 + 17u},( struct _tagged_string){ _temp2081,
_temp2081, _temp2081 + 12u},( struct _tagged_string){ _temp2084, _temp2084,
_temp2084 + 16u},( struct _tagged_string){ _temp2087, _temp2087, _temp2087 + 11u},(
struct _tagged_string){ _temp2090, _temp2090, _temp2090 + 10u},( struct
_tagged_string){ _temp2093, _temp2093, _temp2093 + 14u},( struct _tagged_string){
_temp2096, _temp2096, _temp2096 + 15u},( struct _tagged_string){ _temp2099,
_temp2099, _temp2099 + 20u},( struct _tagged_string){ _temp2102, _temp2102,
_temp2102 + 27u},( struct _tagged_string){ _temp2105, _temp2105, _temp2105 + 10u},(
struct _tagged_string){ _temp2108, _temp2108, _temp2108 + 18u},( struct
_tagged_string){ _temp2111, _temp2111, _temp2111 + 21u},( struct _tagged_string){
_temp2114, _temp2114, _temp2114 + 19u},( struct _tagged_string){ _temp2117,
_temp2117, _temp2117 + 16u},( struct _tagged_string){ _temp2120, _temp2120,
_temp2120 + 11u},( struct _tagged_string){ _temp2123, _temp2123, _temp2123 + 20u},(
struct _tagged_string){ _temp2126, _temp2126, _temp2126 + 15u},( struct
_tagged_string){ _temp2129, _temp2129, _temp2129 + 20u},( struct _tagged_string){
_temp2132, _temp2132, _temp2132 + 15u},( struct _tagged_string){ _temp2135,
_temp2135, _temp2135 + 8u},( struct _tagged_string){ _temp2138, _temp2138,
_temp2138 + 19u},( struct _tagged_string){ _temp2141, _temp2141, _temp2141 + 20u},(
struct _tagged_string){ _temp2144, _temp2144, _temp2144 + 14u},( struct
_tagged_string){ _temp2147, _temp2147, _temp2147 + 19u},( struct _tagged_string){
_temp2150, _temp2150, _temp2150 + 20u},( struct _tagged_string){ _temp2153,
_temp2153, _temp2153 + 11u},( struct _tagged_string){ _temp2156, _temp2156,
_temp2156 + 22u},( struct _tagged_string){ _temp2159, _temp2159, _temp2159 + 20u},(
struct _tagged_string){ _temp2162, _temp2162, _temp2162 + 23u},( struct
_tagged_string){ _temp2165, _temp2165, _temp2165 + 20u},( struct _tagged_string){
_temp2168, _temp2168, _temp2168 + 22u},( struct _tagged_string){ _temp2171,
_temp2171, _temp2171 + 23u},( struct _tagged_string){ _temp2174, _temp2174,
_temp2174 + 24u},( struct _tagged_string){ _temp2177, _temp2177, _temp2177 + 24u},(
struct _tagged_string){ _temp2180, _temp2180, _temp2180 + 15u},( struct
_tagged_string){ _temp2183, _temp2183, _temp2183 + 20u},( struct _tagged_string){
_temp2186, _temp2186, _temp2186 + 22u},( struct _tagged_string){ _temp2189,
_temp2189, _temp2189 + 17u},( struct _tagged_string){ _temp2192, _temp2192,
_temp2192 + 20u},( struct _tagged_string){ _temp2195, _temp2195, _temp2195 + 26u},(
struct _tagged_string){ _temp2198, _temp2198, _temp2198 + 16u},( struct
_tagged_string){ _temp2201, _temp2201, _temp2201 + 17u},( struct _tagged_string){
_temp2204, _temp2204, _temp2204 + 14u},( struct _tagged_string){ _temp2207,
_temp2207, _temp2207 + 15u},( struct _tagged_string){ _temp2210, _temp2210,
_temp2210 + 19u},( struct _tagged_string){ _temp2213, _temp2213, _temp2213 + 19u},(
struct _tagged_string){ _temp2216, _temp2216, _temp2216 + 25u},( struct
_tagged_string){ _temp2219, _temp2219, _temp2219 + 26u},( struct _tagged_string){
_temp2222, _temp2222, _temp2222 + 9u},( struct _tagged_string){ _temp2225,
_temp2225, _temp2225 + 20u}}; static short Cyc_yyr1[ 394u]={ (short)0, (short)124,
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
last_plus_one; } ; struct _tuple19{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; int Cyc_yyparse(){ int yystate; int yyn= 0; int yyerrstatus; int yychar1=
0; int yyssp_offset; struct _tagged_ptr1 yyss=( struct _tagged_ptr1)({ short*
_temp4174=( short*)({ unsigned int _temp4171= 200u; short* _temp4172=( short*)
GC_malloc_atomic( sizeof( short) * _temp4171);{ unsigned int _temp4173=
_temp4171; unsigned int i; for( i= 0; i < _temp4173; i ++){ _temp4172[ i]=
(short)0;}}; _temp4172;}); struct _tagged_ptr1 _temp4175; _temp4175.curr=
_temp4174; _temp4175.base= _temp4174; _temp4175.last_plus_one= _temp4174 + 200;
_temp4175;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=( struct _tagged_ptr2)({
void** _temp4169=( void**)({ unsigned int _temp4166= 200u; void** _temp4167=(
void**) GC_malloc( sizeof( void*) * _temp4166);{ unsigned int _temp4168=
_temp4166; unsigned int i; for( i= 0; i < _temp4168; i ++){ _temp4167[ i]= Cyc_yylval;}};
_temp4167;}); struct _tagged_ptr2 _temp4170; _temp4170.curr= _temp4169;
_temp4170.base= _temp4169; _temp4170.last_plus_one= _temp4169 + 200; _temp4170;});
int yylsp_offset; struct _tagged_ptr3 yyls=( struct _tagged_ptr3)({ struct Cyc_Yyltype*
_temp4164=( struct Cyc_Yyltype*)({ unsigned int _temp4161= 200u; struct Cyc_Yyltype*
_temp4162=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp4161);{ unsigned int _temp4163= _temp4161; unsigned int i; for( i= 0; i <
_temp4163; i ++){ _temp4162[ i]= Cyc_yynewloc();}}; _temp4162;}); struct
_tagged_ptr3 _temp4165; _temp4165.curr= _temp4164; _temp4165.base= _temp4164;
_temp4165.last_plus_one= _temp4164 + 200; _temp4165;}); int yystacksize= 200;
void* yyval= Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0;
Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:*((
short*(*)( struct _tagged_ptr1, unsigned int, unsigned int))
_check_unknown_subscript)( yyss, sizeof( short), ++ yyssp_offset)=( short)
yystate; if( yyssp_offset >= yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror((
struct _tagged_string)({ char* _temp2226=( char*)"parser stack overflow"; struct
_tagged_string _temp2227; _temp2227.curr= _temp2226; _temp2227.base= _temp2226;
_temp2227.last_plus_one= _temp2226 + 22; _temp2227;}));( void) _throw(( void*)
Cyc_Yystack_overflow);} yystacksize *= 2; if( yystacksize > 10000){ yystacksize=
10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp2236=( unsigned int)
yystacksize; short* _temp2237=( short*) GC_malloc_atomic( sizeof( short) *
_temp2236); struct _tagged_ptr1 _temp2239={ _temp2237, _temp2237, _temp2237 +
_temp2236};{ unsigned int _temp2238= _temp2236; unsigned int i; for( i= 0; i <
_temp2238; i ++){ _temp2237[ i]= (short)0;}}; _temp2239;}); struct _tagged_ptr2
yyvs1=({ unsigned int _temp2232=( unsigned int) yystacksize; void** _temp2233=(
void**) GC_malloc( sizeof( void*) * _temp2232); struct _tagged_ptr2 _temp2235={
_temp2233, _temp2233, _temp2233 + _temp2232};{ unsigned int _temp2234= _temp2232;
unsigned int i; for( i= 0; i < _temp2234; i ++){ _temp2233[ i]= Cyc_yylval;}};
_temp2235;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp2228=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp2229=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp2228); struct _tagged_ptr3 _temp2231={
_temp2229, _temp2229, _temp2229 + _temp2228};{ unsigned int _temp2230= _temp2228;
unsigned int i; for( i= 0; i < _temp2230; i ++){ _temp2229[ i]= Cyc_yynewloc();}};
_temp2231;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){*(( short*(*)( struct
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
goto yybackup; yybackup: yyn=( int) Cyc_yypact[ _check_known_subscript_notnull(
751u, yystate)]; if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar == - 2){
Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar= 0;}
else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 351: 0)?( int) Cyc_yytranslate[
_check_known_subscript_notnull( 352u, Cyc_yychar)]: 232;} yyn += yychar1; if((
yyn < 0? 1: yyn > 4222)? 1: Cyc_yycheck[ _check_known_subscript_notnull( 4223u,
yyn)] != yychar1){ goto yydefault;} yyn=( int) Cyc_yytable[
_check_known_subscript_notnull( 4223u, yyn)]; if( yyn < 0){ if( yyn == - 32768){
goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrlab;}}
if( yyn == 750){ return 0;} if( Cyc_yychar != 0){ Cyc_yychar= - 2;}*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), ++ yyvsp_offset)= Cyc_yylval;*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), ++ yylsp_offset)= Cyc_yylloc; if( yyerrstatus
!= 0){ yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int) Cyc_yydefact[
_check_known_subscript_notnull( 751u, yystate)]; if( yyn == 0){ goto yyerrlab;}
yyreduce: yylen=( int) Cyc_yyr2[ _check_known_subscript_notnull( 394u, yyn)];
if( yylen > 0){ yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),( yyvsp_offset +
1) - yylen);} switch( yyn){ case 1: _LL2240: yyval=*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); break; case 2:
_LL2241: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2243=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2243[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2244; _temp2244.tag= Cyc_DeclList_tok; _temp2244.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp2244;});
_temp2243;}); break; case 3: _LL2242: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2246=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2246[ 0]=({ struct Cyc_DeclList_tok_struct _temp2247; _temp2247.tag= Cyc_DeclList_tok;
_temp2247.f1=({ struct Cyc_List_List* _temp2248=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2248->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2249=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2249->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2250=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2250[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2251; _temp2251.tag= Cyc_Absyn_Using_d;
_temp2251.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2251.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2251;}); _temp2250;})); _temp2249->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2249;}); _temp2248->tl= 0; _temp2248;}); _temp2247;}); _temp2246;}); Cyc_Lex_leave_using();
break; case 4: _LL2245: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2253=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2253[ 0]=({ struct Cyc_DeclList_tok_struct _temp2254; _temp2254.tag= Cyc_DeclList_tok;
_temp2254.f1=({ struct Cyc_List_List* _temp2255=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2255->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2256=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2256->r=( void*)(( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2257=(
struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2257[ 0]=({ struct Cyc_Absyn_Using_d_struct _temp2258; _temp2258.tag= Cyc_Absyn_Using_d;
_temp2258.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2258.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2258;}); _temp2257;})); _temp2256->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2256;}); _temp2255->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2255;});
_temp2254;}); _temp2253;}); break; case 5: _LL2252: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2260=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2260[ 0]=({ struct Cyc_DeclList_tok_struct _temp2261; _temp2261.tag= Cyc_DeclList_tok;
_temp2261.f1=({ struct Cyc_List_List* _temp2262=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2262->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2263=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2263->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2264=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2264[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2265; _temp2265.tag=
Cyc_Absyn_Namespace_d; _temp2265.f1=({ struct _tagged_string* _temp2266=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2266[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2266;}); _temp2265.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2265;}); _temp2264;})); _temp2263->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2263;}); _temp2262->tl= 0; _temp2262;}); _temp2261;}); _temp2260;}); Cyc_Lex_leave_namespace();
break; case 6: _LL2259: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2268=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2268[ 0]=({ struct Cyc_DeclList_tok_struct _temp2269; _temp2269.tag= Cyc_DeclList_tok;
_temp2269.f1=({ struct Cyc_List_List* _temp2270=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2270->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2271=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2271->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2272=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2272[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp2273; _temp2273.tag=
Cyc_Absyn_Namespace_d; _temp2273.f1=({ struct _tagged_string* _temp2274=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp2274[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4));
_temp2274;}); _temp2273.f2= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp2273;}); _temp2272;})); _temp2271->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line); _temp2271;}); _temp2270->tl= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2270;});
_temp2269;}); _temp2268;}); break; case 7: _LL2267: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),( struct
_tagged_string)({ char* _temp2276=( char*)"C"; struct _tagged_string _temp2277;
_temp2277.curr= _temp2276; _temp2277.base= _temp2276; _temp2277.last_plus_one=
_temp2276 + 2; _temp2277;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2278=( char*)"only extern \"C\" { ... } is allowed"; struct
_tagged_string _temp2279; _temp2279.curr= _temp2278; _temp2279.base= _temp2278;
_temp2279.last_plus_one= _temp2278 + 35; _temp2279;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).last_line));}
yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2280=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2280[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2281; _temp2281.tag= Cyc_DeclList_tok; _temp2281.f1=({ struct Cyc_List_List*
_temp2282=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2282->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2283=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2283->r=( void*)(( void*)({
struct Cyc_Absyn_ExternC_d_struct* _temp2284=( struct Cyc_Absyn_ExternC_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct)); _temp2284[ 0]=({ struct
Cyc_Absyn_ExternC_d_struct _temp2285; _temp2285.tag= Cyc_Absyn_ExternC_d;
_temp2285.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2285;}); _temp2284;})); _temp2283->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp2283;}); _temp2282->tl= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2282;}); _temp2281;}); _temp2280;}); break;
case 8: _LL2275: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2287=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2287[ 0]=({ struct Cyc_DeclList_tok_struct _temp2288; _temp2288.tag= Cyc_DeclList_tok;
_temp2288.f1= 0; _temp2288;}); _temp2287;}); break; case 9: _LL2286: yyval=(
void*)({ struct Cyc_DeclList_tok_struct* _temp2290=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2290[ 0]=({ struct Cyc_DeclList_tok_struct
_temp2291; _temp2291.tag= Cyc_DeclList_tok; _temp2291.f1=({ struct Cyc_List_List*
_temp2292=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2292->hd=( void*) Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp2293=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2293[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp2294; _temp2294.tag= Cyc_Absyn_Fn_d;
_temp2294.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2294;}); _temp2293;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2292->tl= 0; _temp2292;}); _temp2291;}); _temp2290;}); break; case 10:
_LL2289: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 11:
_LL2295: yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2297=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2297[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2298; _temp2298.tag= Cyc_FnDecl_tok; _temp2298.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2298;}); _temp2297;}); break; case 12: _LL2296:
yyval=( void*)({ struct Cyc_FnDecl_tok_struct* _temp2300=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2300[ 0]=({ struct Cyc_FnDecl_tok_struct
_temp2301; _temp2301.tag= Cyc_FnDecl_tok; _temp2301.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2302=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2302->v=( void*) Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2302;}), Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2301;}); _temp2300;}); break; case 13: _LL2299: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2304=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2304[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2305; _temp2305.tag= Cyc_FnDecl_tok;
_temp2305.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(*(( void**(*)(
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
_temp2305;}); _temp2304;}); break; case 14: _LL2303: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2307=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2307[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2308; _temp2308.tag= Cyc_FnDecl_tok;
_temp2308.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2309=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2309->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2309;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2308;});
_temp2307;}); break; case 15: _LL2306: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2311=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2311[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2312; _temp2312.tag= Cyc_FnDecl_tok;
_temp2312.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2313=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2313->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2313;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), 0, Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2312;});
_temp2311;}); break; case 16: _LL2310: yyval=( void*)({ struct Cyc_FnDecl_tok_struct*
_temp2315=( struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2315[ 0]=({ struct Cyc_FnDecl_tok_struct _temp2316; _temp2316.tag= Cyc_FnDecl_tok;
_temp2316.f1= Cyc_Parse_make_function(({ struct Cyc_Core_Opt* _temp2317=( struct
Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2317->v=( void*)
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3));
_temp2317;}), Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2316;});
_temp2315;}); break; case 17: _LL2314: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 18: _LL2318: Cyc_Lex_leave_using();
break; case 19: _LL2319: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2321=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2321[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2321;})); yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 20: _LL2320: Cyc_Lex_leave_namespace(); break; case
21: _LL2322: yyval=( void*)({ struct Cyc_DeclList_tok_struct* _temp2324=( struct
Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2324[ 0]=({ struct Cyc_DeclList_tok_struct _temp2325; _temp2325.tag= Cyc_DeclList_tok;
_temp2325.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp2325;}); _temp2324;}); break; case 22: _LL2323: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2327=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2327[ 0]=({ struct Cyc_DeclList_tok_struct _temp2328; _temp2328.tag= Cyc_DeclList_tok;
_temp2328.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_InitDeclList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2328;}); _temp2327;}); break; case 23: _LL2326: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2330=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2330[ 0]=({ struct Cyc_DeclList_tok_struct _temp2331; _temp2331.tag= Cyc_DeclList_tok;
_temp2331.f1=({ struct Cyc_List_List* _temp2332=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2332->hd=( void*) Cyc_Absyn_let_decl(
Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
0, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2332->tl= 0; _temp2332;}); _temp2331;});
_temp2330;}); break; case 24: _LL2329: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 25: _LL2333: yyval=( void*)({ struct Cyc_DeclList_tok_struct*
_temp2335=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2335[ 0]=({ struct Cyc_DeclList_tok_struct _temp2336; _temp2336.tag= Cyc_DeclList_tok;
_temp2336.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp2336;}); _temp2335;}); break; case 26:
_LL2334: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2338=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2338[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2339; _temp2339.tag= Cyc_DeclSpec_tok; _temp2339.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2340=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2340->sc=({ struct Cyc_Core_Opt* _temp2341=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2341->v=( void*) Cyc_yyget_StorageClass_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2341;});
_temp2340->tq= Cyc_Absyn_empty_tqual(); _temp2340->type_specs= 0; _temp2340->is_inline=
0; _temp2340->attributes= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2340;}); _temp2339;}); _temp2338;}); break;
case 27: _LL2337: if(( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->sc != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp2343=( char*)"Only one storage class is allowed in a declaration"; struct
_tagged_string _temp2344; _temp2344.curr= _temp2343; _temp2344.base= _temp2343;
_temp2344.last_plus_one= _temp2343 + 51; _temp2344;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2345=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2345[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2346; _temp2346.tag= Cyc_DeclSpec_tok; _temp2346.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2347=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2347->sc=({ struct Cyc_Core_Opt* _temp2348=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2348->v=( void*) Cyc_yyget_StorageClass_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2348;});
_temp2347->tq=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tq; _temp2347->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->type_specs; _temp2347->is_inline=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->is_inline;
_temp2347->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2347;}); _temp2346;}); _temp2345;}); break; case 28: _LL2342: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2350=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2350[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2351; _temp2351.tag= Cyc_DeclSpec_tok; _temp2351.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2352=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2352->sc= 0; _temp2352->tq= Cyc_Absyn_empty_tqual(); _temp2352->type_specs=({
struct Cyc_List_List* _temp2353=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2353->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2353->tl=
0; _temp2353;}); _temp2352->is_inline= 0; _temp2352->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2352;});
_temp2351;}); _temp2350;}); break; case 29: _LL2349: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2355=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2355[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2356; _temp2356.tag= Cyc_DeclSpec_tok;
_temp2356.f1=({ struct Cyc_Parse_Declaration_spec* _temp2357=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2357->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2357->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2357->type_specs=({ struct Cyc_List_List* _temp2358=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2358->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2358->tl=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2358;}); _temp2357->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->is_inline; _temp2357->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2357;}); _temp2356;}); _temp2355;}); break; case 30: _LL2354: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2360=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2360[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2361; _temp2361.tag= Cyc_DeclSpec_tok; _temp2361.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2362=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2362->sc= 0; _temp2362->tq= Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2362->type_specs= 0; _temp2362->is_inline=
0; _temp2362->attributes= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2362;}); _temp2361;}); _temp2360;}); break;
case 31: _LL2359: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct* _temp2364=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2364[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2365; _temp2365.tag= Cyc_DeclSpec_tok;
_temp2365.f1=({ struct Cyc_Parse_Declaration_spec* _temp2366=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2366->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2366->tq=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)),( Cyc_yyget_DeclSpec_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tq); _temp2366->type_specs=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->type_specs;
_temp2366->is_inline=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->is_inline; _temp2366->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2366;}); _temp2365;}); _temp2364;}); break; case 32: _LL2363: yyval=( void*)({
struct Cyc_DeclSpec_tok_struct* _temp2368=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct)); _temp2368[ 0]=({ struct Cyc_DeclSpec_tok_struct
_temp2369; _temp2369.tag= Cyc_DeclSpec_tok; _temp2369.f1=({ struct Cyc_Parse_Declaration_spec*
_temp2370=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2370->sc= 0; _temp2370->tq= Cyc_Absyn_empty_tqual(); _temp2370->type_specs=
0; _temp2370->is_inline= 1; _temp2370->attributes= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2370;});
_temp2369;}); _temp2368;}); break; case 33: _LL2367: yyval=( void*)({ struct Cyc_DeclSpec_tok_struct*
_temp2372=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2372[ 0]=({ struct Cyc_DeclSpec_tok_struct _temp2373; _temp2373.tag= Cyc_DeclSpec_tok;
_temp2373.f1=({ struct Cyc_Parse_Declaration_spec* _temp2374=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2374->sc=( Cyc_yyget_DeclSpec_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->sc; _temp2374->tq=(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tq;
_temp2374->type_specs=( Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->type_specs; _temp2374->is_inline= 1; _temp2374->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_DeclSpec_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->attributes);
_temp2374;}); _temp2373;}); _temp2372;}); break; case 34: _LL2371: yyval=( void*)({
struct Cyc_StorageClass_tok_struct* _temp2376=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2376[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2377; _temp2377.tag= Cyc_StorageClass_tok;
_temp2377.f1=( void*)(( void*) Cyc_Parse_Auto_sc); _temp2377;}); _temp2376;});
break; case 35: _LL2375: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2379=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2379[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2380; _temp2380.tag=
Cyc_StorageClass_tok; _temp2380.f1=( void*)(( void*) Cyc_Parse_Register_sc);
_temp2380;}); _temp2379;}); break; case 36: _LL2378: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2382=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2382[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2383; _temp2383.tag=
Cyc_StorageClass_tok; _temp2383.f1=( void*)(( void*) Cyc_Parse_Static_sc);
_temp2383;}); _temp2382;}); break; case 37: _LL2381: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2385=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2385[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2386; _temp2386.tag=
Cyc_StorageClass_tok; _temp2386.f1=( void*)(( void*) Cyc_Parse_Extern_sc);
_temp2386;}); _temp2385;}); break; case 38: _LL2384: if( Cyc_String_strcmp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),( struct
_tagged_string)({ char* _temp2388=( char*)"C"; struct _tagged_string _temp2389;
_temp2389.curr= _temp2388; _temp2389.base= _temp2388; _temp2389.last_plus_one=
_temp2388 + 2; _temp2389;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp2390=( char*)"only extern or extern \"C\" is allowed"; struct
_tagged_string _temp2391; _temp2391.curr= _temp2390; _temp2391.base= _temp2390;
_temp2391.last_plus_one= _temp2390 + 37; _temp2391;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_StorageClass_tok_struct* _temp2392=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2392[ 0]=({ struct
Cyc_StorageClass_tok_struct _temp2393; _temp2393.tag= Cyc_StorageClass_tok;
_temp2393.f1=( void*)(( void*) Cyc_Parse_ExternC_sc); _temp2393;}); _temp2392;});
break; case 39: _LL2387: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2395=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2395[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2396; _temp2396.tag=
Cyc_StorageClass_tok; _temp2396.f1=( void*)(( void*) Cyc_Parse_Typedef_sc);
_temp2396;}); _temp2395;}); break; case 40: _LL2394: yyval=( void*)({ struct Cyc_StorageClass_tok_struct*
_temp2398=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2398[ 0]=({ struct Cyc_StorageClass_tok_struct _temp2399; _temp2399.tag=
Cyc_StorageClass_tok; _temp2399.f1=( void*)(( void*) Cyc_Parse_Abstract_sc);
_temp2399;}); _temp2398;}); break; case 41: _LL2397: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2401=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2401[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2402; _temp2402.tag=
Cyc_AttributeList_tok; _temp2402.f1= 0; _temp2402;}); _temp2401;}); break; case
42: _LL2400: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 43: _LL2403: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2405=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2405[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2406; _temp2406.tag=
Cyc_AttributeList_tok; _temp2406.f1= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2406;}); _temp2405;}); break;
case 44: _LL2404: yyval=( void*)({ struct Cyc_AttributeList_tok_struct*
_temp2408=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2408[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2409; _temp2409.tag=
Cyc_AttributeList_tok; _temp2409.f1=({ struct Cyc_List_List* _temp2410=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2410->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2410->tl= 0; _temp2410;}); _temp2409;}); _temp2408;}); break; case 45:
_LL2407: yyval=( void*)({ struct Cyc_AttributeList_tok_struct* _temp2412=(
struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2412[ 0]=({ struct Cyc_AttributeList_tok_struct _temp2413; _temp2413.tag=
Cyc_AttributeList_tok; _temp2413.f1=({ struct Cyc_List_List* _temp2414=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2414->hd=( void*)
Cyc_yyget_Attribute_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2414->tl= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2414;}); _temp2413;}); _temp2412;}); break; case 46:
_LL2411: { struct _tagged_string _temp2416= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); goto _LL2417; _LL2417: if((((({ struct
_tagged_string _temp2418= _temp2416;( unsigned int)( _temp2418.last_plus_one -
_temp2418.curr);}) > 4?*(( char*(*)( struct _tagged_string, unsigned int,
unsigned int)) _check_unknown_subscript)( _temp2416, sizeof( char), 0) =='_': 0)?*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2416, sizeof( char), 1) =='_': 0)?*(( char*(*)(
struct _tagged_string, unsigned int, unsigned int)) _check_unknown_subscript)(
_temp2416, sizeof( char),( int)(({ struct _tagged_string _temp2419= _temp2416;(
unsigned int)( _temp2419.last_plus_one - _temp2419.curr);}) - 2)) =='_': 0)?*((
char*(*)( struct _tagged_string, unsigned int, unsigned int))
_check_unknown_subscript)( _temp2416, sizeof( char),( int)(({ struct
_tagged_string _temp2420= _temp2416;( unsigned int)( _temp2420.last_plus_one -
_temp2420.curr);}) - 3)) =='_': 0){ _temp2416= Cyc_String_substring( _temp2416,
2,({ struct _tagged_string _temp2421= _temp2416;( unsigned int)( _temp2421.last_plus_one
- _temp2421.curr);}) - 5);}{ int i= 0; for( 0; i < 15u; ++ i){ if( Cyc_String_strcmp(
_temp2416,( Cyc_Parse_att_map[ _check_known_subscript_notnull( 15u, i)]).f1) ==
0){ yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2422=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2422[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2423; _temp2423.tag= Cyc_Attribute_tok; _temp2423.f1=(
void*)( Cyc_Parse_att_map[ _check_known_subscript_notnull( 15u, i)]).f2;
_temp2423;}); _temp2422;}); break;}} if( i == 15u){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2424=( char*)"unrecognized attribute"; struct
_tagged_string _temp2425; _temp2425.curr= _temp2424; _temp2425.base= _temp2424;
_temp2425.last_plus_one= _temp2424 + 23; _temp2425;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2426=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2426[ 0]=({ struct
Cyc_Attribute_tok_struct _temp2427; _temp2427.tag= Cyc_Attribute_tok; _temp2427.f1=(
void*)(( void*) Cyc_Absyn_Cdecl_att); _temp2427;}); _temp2426;});} break;}} case
47: _LL2415: yyval=( void*)({ struct Cyc_Attribute_tok_struct* _temp2429=(
struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2429[ 0]=({ struct Cyc_Attribute_tok_struct _temp2430; _temp2430.tag= Cyc_Attribute_tok;
_temp2430.f1=( void*)(( void*) Cyc_Absyn_Const_att); _temp2430;}); _temp2429;});
break; case 48: _LL2428: { struct _tagged_string _temp2432= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); goto
_LL2433; _LL2433: { struct _tuple16 _temp2436; int _temp2437; void* _temp2439;
struct _tuple16* _temp2434= Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2436=* _temp2434; _LL2440: _temp2439= _temp2436.f1;
goto _LL2438; _LL2438: _temp2437= _temp2436.f2; goto _LL2435; _LL2435: { void* a;
if( Cyc_String_zstrcmp( _temp2432,( struct _tagged_string)({ char* _temp2441=(
char*)"regparm"; struct _tagged_string _temp2442; _temp2442.curr= _temp2441;
_temp2442.base= _temp2441; _temp2442.last_plus_one= _temp2441 + 8; _temp2442;}))
== 0? 1: Cyc_String_zstrcmp( _temp2432,( struct _tagged_string)({ char*
_temp2443=( char*)"__regparm__"; struct _tagged_string _temp2444; _temp2444.curr=
_temp2443; _temp2444.base= _temp2443; _temp2444.last_plus_one= _temp2443 + 12;
_temp2444;})) == 0){ if( _temp2437 <= 0? 1: _temp2437 > 3){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2445=( char*)"regparm requires value between 1 and 3";
struct _tagged_string _temp2446; _temp2446.curr= _temp2445; _temp2446.base=
_temp2445; _temp2446.last_plus_one= _temp2445 + 39; _temp2446;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}
a=( void*)({ struct Cyc_Absyn_Regparm_att_struct* _temp2447=( struct Cyc_Absyn_Regparm_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Regparm_att_struct)); _temp2447[ 0]=({
struct Cyc_Absyn_Regparm_att_struct _temp2448; _temp2448.tag= Cyc_Absyn_Regparm_att;
_temp2448.f1= _temp2437; _temp2448;}); _temp2447;});} else{ if( Cyc_String_zstrcmp(
_temp2432,( struct _tagged_string)({ char* _temp2449=( char*)"aligned"; struct
_tagged_string _temp2450; _temp2450.curr= _temp2449; _temp2450.base= _temp2449;
_temp2450.last_plus_one= _temp2449 + 8; _temp2450;})) == 0? 1: Cyc_String_zstrcmp(
_temp2432,( struct _tagged_string)({ char* _temp2451=( char*)"__aligned__";
struct _tagged_string _temp2452; _temp2452.curr= _temp2451; _temp2452.base=
_temp2451; _temp2452.last_plus_one= _temp2451 + 12; _temp2452;})) == 0){ if(
_temp2437 < 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2453=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2454; _temp2454.curr= _temp2453; _temp2454.base=
_temp2453; _temp2454.last_plus_one= _temp2453 + 39; _temp2454;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));}{
unsigned int j=( unsigned int) _temp2437; for( 0;( j & 1) == 0; j= j >> 1){;} j=
j >> 1; if( j != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2455=(
char*)"aligned requires positive power of two"; struct _tagged_string _temp2456;
_temp2456.curr= _temp2455; _temp2456.base= _temp2455; _temp2456.last_plus_one=
_temp2455 + 39; _temp2456;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));} a=( void*)({
struct Cyc_Absyn_Aligned_att_struct* _temp2457=( struct Cyc_Absyn_Aligned_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Aligned_att_struct)); _temp2457[ 0]=({
struct Cyc_Absyn_Aligned_att_struct _temp2458; _temp2458.tag= Cyc_Absyn_Aligned_att;
_temp2458.f1= _temp2437; _temp2458;}); _temp2457;});}} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2459=( char*)"unrecognized attribute";
struct _tagged_string _temp2460; _temp2460.curr= _temp2459; _temp2460.base=
_temp2459; _temp2460.last_plus_one= _temp2459 + 23; _temp2460;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;}} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2461=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2461[ 0]=({ struct Cyc_Attribute_tok_struct _temp2462; _temp2462.tag= Cyc_Attribute_tok;
_temp2462.f1=( void*) a; _temp2462;}); _temp2461;}); break;}}} case 49: _LL2431: {
struct _tagged_string _temp2464= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); goto _LL2465; _LL2465: { struct
_tagged_string _temp2466= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); goto _LL2467; _LL2467: { void* a; if( Cyc_String_zstrcmp(
_temp2464,( struct _tagged_string)({ char* _temp2468=( char*)"section"; struct
_tagged_string _temp2469; _temp2469.curr= _temp2468; _temp2469.base= _temp2468;
_temp2469.last_plus_one= _temp2468 + 8; _temp2469;})) == 0? 1: Cyc_String_zstrcmp(
_temp2464,( struct _tagged_string)({ char* _temp2470=( char*)"__section__";
struct _tagged_string _temp2471; _temp2471.curr= _temp2470; _temp2471.base=
_temp2470; _temp2471.last_plus_one= _temp2470 + 12; _temp2471;}))){ a=( void*)({
struct Cyc_Absyn_Section_att_struct* _temp2472=( struct Cyc_Absyn_Section_att_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct)); _temp2472[ 0]=({
struct Cyc_Absyn_Section_att_struct _temp2473; _temp2473.tag= Cyc_Absyn_Section_att;
_temp2473.f1= _temp2466; _temp2473;}); _temp2472;});} else{ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2474=( char*)"unrecognized attribute";
struct _tagged_string _temp2475; _temp2475.curr= _temp2474; _temp2475.base=
_temp2474; _temp2475.last_plus_one= _temp2474 + 23; _temp2475;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));
a=( void*) Cyc_Absyn_Cdecl_att;} yyval=( void*)({ struct Cyc_Attribute_tok_struct*
_temp2476=( struct Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));
_temp2476[ 0]=({ struct Cyc_Attribute_tok_struct _temp2477; _temp2477.tag= Cyc_Attribute_tok;
_temp2477.f1=( void*) a; _temp2477;}); _temp2476;}); break;}}} case 50: _LL2463:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2479=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2479[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2480; _temp2480.tag= Cyc_TypeSpecifier_tok;
_temp2480.f1=( void*) Cyc_Parse_type_spec(( void*) Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2480;}); _temp2479;}); break; case 51: _LL2478: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2482=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2482[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2483; _temp2483.tag=
Cyc_TypeSpecifier_tok; _temp2483.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_new_evar((
void*) Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2483;});
_temp2482;}); break; case 52: _LL2481: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2485=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2485[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2486; _temp2486.tag=
Cyc_TypeSpecifier_tok; _temp2486.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_uchar_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2486;}); _temp2485;}); break; case 53: _LL2484:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2488=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2488[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2489; _temp2489.tag= Cyc_TypeSpecifier_tok;
_temp2489.f1=( void*)(( void*)({ struct Cyc_Parse_Short_spec_struct* _temp2490=(
struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2490[ 0]=({ struct Cyc_Parse_Short_spec_struct _temp2491; _temp2491.tag=
Cyc_Parse_Short_spec; _temp2491.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2491;});
_temp2490;})); _temp2489;}); _temp2488;}); break; case 54: _LL2487: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2493=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2493[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2494; _temp2494.tag= Cyc_TypeSpecifier_tok;
_temp2494.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2494;}); _temp2493;}); break; case 55: _LL2492: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2496=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2496[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2497; _temp2497.tag=
Cyc_TypeSpecifier_tok; _temp2497.f1=( void*)(( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2498=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2498[ 0]=({ struct Cyc_Parse_Long_spec_struct _temp2499; _temp2499.tag= Cyc_Parse_Long_spec;
_temp2499.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2499;});
_temp2498;})); _temp2497;}); _temp2496;}); break; case 56: _LL2495: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2501=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2501[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2502; _temp2502.tag= Cyc_TypeSpecifier_tok;
_temp2502.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2502;}); _temp2501;}); break; case 57: _LL2500: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2504=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2504[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2505; _temp2505.tag=
Cyc_TypeSpecifier_tok; _temp2505.f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_double_t,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2505;}); _temp2504;}); break; case 58: _LL2503:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2507=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2507[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2508; _temp2508.tag= Cyc_TypeSpecifier_tok;
_temp2508.f1=( void*)(( void*)({ struct Cyc_Parse_Signed_spec_struct* _temp2509=(
struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2509[ 0]=({ struct Cyc_Parse_Signed_spec_struct _temp2510; _temp2510.tag=
Cyc_Parse_Signed_spec; _temp2510.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2510;});
_temp2509;})); _temp2508;}); _temp2507;}); break; case 59: _LL2506: yyval=( void*)({
struct Cyc_TypeSpecifier_tok_struct* _temp2512=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2512[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2513; _temp2513.tag= Cyc_TypeSpecifier_tok;
_temp2513.f1=( void*)(( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp2514=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp2514[ 0]=({ struct Cyc_Parse_Unsigned_spec_struct
_temp2515; _temp2515.tag= Cyc_Parse_Unsigned_spec; _temp2515.f1= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2515;}); _temp2514;})); _temp2513;}); _temp2512;}); break; case 60: _LL2511:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 61:
_LL2516: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 62:
_LL2517: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 63:
_LL2518: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2520=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2520[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2521; _temp2521.tag=
Cyc_TypeSpecifier_tok; _temp2521.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2521;}); _temp2520;}); break; case 64: _LL2519: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2523=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2523[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2524; _temp2524.tag=
Cyc_TypeSpecifier_tok; _temp2524.f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2524;}); _temp2523;}); break; case 65: _LL2522: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2526=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2526[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2527; _temp2527.tag=
Cyc_TypeSpecifier_tok; _temp2527.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TypedefType_struct* _temp2528=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp2528[ 0]=({
struct Cyc_Absyn_TypedefType_struct _temp2529; _temp2529.tag= Cyc_Absyn_TypedefType;
_temp2529.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2529.f2= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2529.f3= 0; _temp2529;}); _temp2528;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2527;}); _temp2526;}); break; case 66: _LL2525: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2531=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2531[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2532; _temp2532.tag=
Cyc_TypeSpecifier_tok; _temp2532.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TupleType_struct* _temp2533=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2533[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp2534; _temp2534.tag= Cyc_Absyn_TupleType;
_temp2534.f1=(( struct Cyc_List_List*(*)( struct _tuple11*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); _temp2534;});
_temp2533;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2532;});
_temp2531;}); break; case 67: _LL2530: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2536=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2536[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2537; _temp2537.tag=
Cyc_TypeSpecifier_tok; _temp2537.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_RgnHandleType_struct* _temp2538=( struct Cyc_Absyn_RgnHandleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp2538[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp2539; _temp2539.tag= Cyc_Absyn_RgnHandleType;
_temp2539.f1=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2539;}); _temp2538;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2537;}); _temp2536;}); break; case 68: _LL2535: yyval=( void*)({ struct Cyc_Kind_tok_struct*
_temp2541=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp2541[ 0]=({ struct Cyc_Kind_tok_struct _temp2542; _temp2542.tag= Cyc_Kind_tok;
_temp2542.f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2542;}); _temp2541;}); break; case 69: _LL2540: { struct _tagged_string*
_temp2546; void* _temp2548; struct _tuple1 _temp2544=* Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _LL2549:
_temp2548= _temp2544.f1; goto _LL2547; _LL2547: _temp2546= _temp2544.f2; goto
_LL2545; _LL2545: if( _temp2548 !=( void*) Cyc_Absyn_Loc_n){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2550=( char*)"bad kind in type specifier";
struct _tagged_string _temp2551; _temp2551.curr= _temp2550; _temp2551.base=
_temp2550; _temp2551.last_plus_one= _temp2550 + 27; _temp2551;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_Kind_tok_struct* _temp2552=( struct Cyc_Kind_tok_struct*)
GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp2552[ 0]=({ struct Cyc_Kind_tok_struct
_temp2553; _temp2553.tag= Cyc_Kind_tok; _temp2553.f1=( void*) Cyc_Parse_id_to_kind(*
_temp2546, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2553;});
_temp2552;}); break;} case 70: _LL2543: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2555=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2555[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2556; _temp2556.tag= Cyc_TypeQual_tok;
_temp2556.f1=({ struct Cyc_Absyn_Tqual _temp2557; _temp2557.q_const= 1;
_temp2557.q_volatile= 0; _temp2557.q_restrict= 0; _temp2557;}); _temp2556;});
_temp2555;}); break; case 71: _LL2554: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2559=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2559[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2560; _temp2560.tag= Cyc_TypeQual_tok;
_temp2560.f1=({ struct Cyc_Absyn_Tqual _temp2561; _temp2561.q_const= 0;
_temp2561.q_volatile= 1; _temp2561.q_restrict= 0; _temp2561;}); _temp2560;});
_temp2559;}); break; case 72: _LL2558: yyval=( void*)({ struct Cyc_TypeQual_tok_struct*
_temp2563=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2563[ 0]=({ struct Cyc_TypeQual_tok_struct _temp2564; _temp2564.tag= Cyc_TypeQual_tok;
_temp2564.f1=({ struct Cyc_Absyn_Tqual _temp2565; _temp2565.q_const= 0;
_temp2565.q_volatile= 0; _temp2565.q_restrict= 1; _temp2565;}); _temp2564;});
_temp2563;}); break; case 73: _LL2562: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2567=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2567[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2568; _temp2568.tag=
Cyc_TypeSpecifier_tok; _temp2568.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2569=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2569[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2570; _temp2570.tag= Cyc_Parse_Decl_spec;
_temp2570.f1=({ struct Cyc_Absyn_Decl* _temp2571=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2571->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_d_struct* _temp2572=( struct Cyc_Absyn_Enum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Enum_d_struct)); _temp2572[ 0]=({ struct Cyc_Absyn_Enum_d_struct
_temp2573; _temp2573.tag= Cyc_Absyn_Enum_d; _temp2573.f1=({ struct Cyc_Absyn_Enumdecl*
_temp2574=( struct Cyc_Absyn_Enumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl));
_temp2574->sc=( void*)(( void*) Cyc_Absyn_Public); _temp2574->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2574->fields=({
struct Cyc_Core_Opt* _temp2575=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2575->v=( void*) Cyc_yyget_EnumfieldList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2575;}); _temp2574;}); _temp2573;});
_temp2572;})); _temp2571->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2571;});
_temp2570;}); _temp2569;})); _temp2568;}); _temp2567;}); break; case 74: _LL2566:
yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2577=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2577[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2578; _temp2578.tag= Cyc_TypeSpecifier_tok;
_temp2578.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_EnumType_struct*
_temp2579=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2579[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2580; _temp2580.tag= Cyc_Absyn_EnumType;
_temp2580.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2580.f2= 0; _temp2580;}); _temp2579;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2578;}); _temp2577;}); break; case 75: _LL2576: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2582=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2582[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2583; _temp2583.tag= Cyc_Enumfield_tok;
_temp2583.f1=({ struct Cyc_Absyn_Enumfield* _temp2584=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2584->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2584->tag=
0; _temp2584->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2584;});
_temp2583;}); _temp2582;}); break; case 76: _LL2581: yyval=( void*)({ struct Cyc_Enumfield_tok_struct*
_temp2586=( struct Cyc_Enumfield_tok_struct*) GC_malloc( sizeof( struct Cyc_Enumfield_tok_struct));
_temp2586[ 0]=({ struct Cyc_Enumfield_tok_struct _temp2587; _temp2587.tag= Cyc_Enumfield_tok;
_temp2587.f1=({ struct Cyc_Absyn_Enumfield* _temp2588=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp2588->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2588->tag=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2588->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2588;});
_temp2587;}); _temp2586;}); break; case 77: _LL2585: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct*
_temp2590=( struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2590[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2591; _temp2591.tag=
Cyc_EnumfieldList_tok; _temp2591.f1=({ struct Cyc_List_List* _temp2592=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2592->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2592->tl= 0; _temp2592;}); _temp2591;}); _temp2590;}); break; case 78:
_LL2589: yyval=( void*)({ struct Cyc_EnumfieldList_tok_struct* _temp2594=(
struct Cyc_EnumfieldList_tok_struct*) GC_malloc( sizeof( struct Cyc_EnumfieldList_tok_struct));
_temp2594[ 0]=({ struct Cyc_EnumfieldList_tok_struct _temp2595; _temp2595.tag=
Cyc_EnumfieldList_tok; _temp2595.f1=({ struct Cyc_List_List* _temp2596=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2596->hd=( void*)
Cyc_yyget_Enumfield_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2596->tl= Cyc_yyget_EnumfieldList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2596;}); _temp2595;}); _temp2594;}); break; case 79:
_LL2593: { void* t;{ void* _temp2598= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _LL2600: if( _temp2598 ==( void*) Cyc_Parse_Struct_su){
goto _LL2601;} else{ goto _LL2602;} _LL2602: if( _temp2598 ==( void*) Cyc_Parse_Union_su){
goto _LL2603;} else{ goto _LL2599;} _LL2601: t=( void*)({ struct Cyc_Absyn_AnonStructType_struct*
_temp2604=( struct Cyc_Absyn_AnonStructType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonStructType_struct)); _temp2604[ 0]=({ struct Cyc_Absyn_AnonStructType_struct
_temp2605; _temp2605.tag= Cyc_Absyn_AnonStructType; _temp2605.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2605;});
_temp2604;}); goto _LL2599; _LL2603: t=( void*)({ struct Cyc_Absyn_AnonUnionType_struct*
_temp2606=( struct Cyc_Absyn_AnonUnionType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_AnonUnionType_struct)); _temp2606[ 0]=({ struct Cyc_Absyn_AnonUnionType_struct
_temp2607; _temp2607.tag= Cyc_Absyn_AnonUnionType; _temp2607.f1= Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2607;});
_temp2606;}); goto _LL2599; _LL2599:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2608=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2608[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2609; _temp2609.tag=
Cyc_TypeSpecifier_tok; _temp2609.f1=( void*)(( void*)({ struct Cyc_Parse_Type_spec_struct*
_temp2610=( struct Cyc_Parse_Type_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct));
_temp2610[ 0]=({ struct Cyc_Parse_Type_spec_struct _temp2611; _temp2611.tag= Cyc_Parse_Type_spec;
_temp2611.f1=( void*) t; _temp2611.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2611;}); _temp2610;})); _temp2609;}); _temp2608;}); break;} case 80:
_LL2597: { struct Cyc_List_List* _temp2613=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2614; _LL2614: { struct Cyc_Absyn_Decl* d;{ void* _temp2615= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2617:
if( _temp2615 ==( void*) Cyc_Parse_Struct_su){ goto _LL2618;} else{ goto _LL2619;}
_LL2619: if( _temp2615 ==( void*) Cyc_Parse_Union_su){ goto _LL2620;} else{ goto
_LL2616;} _LL2618: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2621=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2621->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2621;}), _temp2613,({ struct Cyc_Core_Opt* _temp2622=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2622->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2622;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2616;
_LL2620: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2623=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2623->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2623;}), _temp2613,({ struct Cyc_Core_Opt* _temp2624=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2624->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2624;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2616;
_LL2616:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2625=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2625[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2626; _temp2626.tag=
Cyc_TypeSpecifier_tok; _temp2626.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2627=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2627[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2628; _temp2628.tag= Cyc_Parse_Decl_spec;
_temp2628.f1= d; _temp2628;}); _temp2627;})); _temp2626;}); _temp2625;}); break;}}
case 81: _LL2612: { struct Cyc_List_List* _temp2630=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2631; _LL2631: { struct Cyc_Absyn_Decl* d;{ void* _temp2632= Cyc_yyget_StructOrUnion_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _LL2634:
if( _temp2632 ==( void*) Cyc_Parse_Struct_su){ goto _LL2635;} else{ goto _LL2636;}
_LL2636: if( _temp2632 ==( void*) Cyc_Parse_Union_su){ goto _LL2637;} else{ goto
_LL2633;} _LL2635: d= Cyc_Absyn_struct_decl(( void*) Cyc_Absyn_Public,({ struct
Cyc_Core_Opt* _temp2638=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2638->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2638;}), _temp2630,({ struct Cyc_Core_Opt* _temp2639=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2639->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2639;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2633;
_LL2637: d= Cyc_Absyn_union_decl(( void*) Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2640=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2640->v=( void*) Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2640;}), _temp2630,({ struct Cyc_Core_Opt* _temp2641=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2641->v=(
void*) Cyc_yyget_StructFieldDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2641;}), 0, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL2633;
_LL2633:;} yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2642=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2642[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2643; _temp2643.tag=
Cyc_TypeSpecifier_tok; _temp2643.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2644=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2644[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2645; _temp2645.tag= Cyc_Parse_Decl_spec;
_temp2645.f1= d; _temp2645;}); _temp2644;})); _temp2643;}); _temp2642;}); break;}}
case 82: _LL2629:{ void* _temp2647= Cyc_yyget_StructOrUnion_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _LL2649: if( _temp2647 ==( void*) Cyc_Parse_Struct_su){
goto _LL2650;} else{ goto _LL2651;} _LL2651: if( _temp2647 ==( void*) Cyc_Parse_Union_su){
goto _LL2652;} else{ goto _LL2648;} _LL2650: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2653=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2653[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2654; _temp2654.tag=
Cyc_TypeSpecifier_tok; _temp2654.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_StructType_struct* _temp2655=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp2655[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp2656; _temp2656.tag= Cyc_Absyn_StructType;
_temp2656.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2656.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2656.f3= 0;
_temp2656;}); _temp2655;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2654;});
_temp2653;}); goto _LL2648; _LL2652: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2657=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2657[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2658; _temp2658.tag=
Cyc_TypeSpecifier_tok; _temp2658.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_UnionType_struct* _temp2659=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp2659[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp2660; _temp2660.tag= Cyc_Absyn_UnionType;
_temp2660.f1=( struct _tuple1*) Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2660.f2= Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2660.f3= 0;
_temp2660;}); _temp2659;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2658;});
_temp2657;}); goto _LL2648; _LL2648:;} break; case 83: _LL2646:{ void* _temp2662=
Cyc_yyget_StructOrUnion_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_LL2664: if( _temp2662 ==( void*) Cyc_Parse_Struct_su){ goto _LL2665;} else{
goto _LL2666;} _LL2666: if( _temp2662 ==( void*) Cyc_Parse_Union_su){ goto
_LL2667;} else{ goto _LL2663;} _LL2665: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
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
_temp2672;}); goto _LL2663; _LL2663:;} break; case 84: _LL2661: yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp2677=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2677[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2678; _temp2678.tag= Cyc_TypeList_tok; _temp2678.f1= 0; _temp2678;});
_temp2677;}); break; case 85: _LL2676: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp2680=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp2680[ 0]=({ struct Cyc_TypeList_tok_struct _temp2681; _temp2681.tag= Cyc_TypeList_tok;
_temp2681.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp2681;}); _temp2680;}); break; case 86: _LL2679:(( struct Cyc_Lexing_lexbuf*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos -= 1;
yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp2683=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp2683[ 0]=({ struct Cyc_TypeList_tok_struct
_temp2684; _temp2684.tag= Cyc_TypeList_tok; _temp2684.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1))); _temp2684;}); _temp2683;}); break;
case 87: _LL2682: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2686=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2686[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2687; _temp2687.tag=
Cyc_StructOrUnion_tok; _temp2687.f1=( void*)(( void*) Cyc_Parse_Struct_su);
_temp2687;}); _temp2686;}); break; case 88: _LL2685: yyval=( void*)({ struct Cyc_StructOrUnion_tok_struct*
_temp2689=( struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp2689[ 0]=({ struct Cyc_StructOrUnion_tok_struct _temp2690; _temp2690.tag=
Cyc_StructOrUnion_tok; _temp2690.f1=( void*)(( void*) Cyc_Parse_Union_su);
_temp2690;}); _temp2689;}); break; case 89: _LL2688: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2692=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2692[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2693; _temp2693.tag= Cyc_StructFieldDeclList_tok; _temp2693.f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))); _temp2693;});
_temp2692;}); break; case 90: _LL2691: yyval=( void*)({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp2695=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp2695[ 0]=({ struct Cyc_StructFieldDeclListList_tok_struct
_temp2696; _temp2696.tag= Cyc_StructFieldDeclListList_tok; _temp2696.f1=({
struct Cyc_List_List* _temp2697=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2697->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2697->tl=
0; _temp2697;}); _temp2696;}); _temp2695;}); break; case 91: _LL2694: yyval=(
void*)({ struct Cyc_StructFieldDeclListList_tok_struct* _temp2699=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp2699[ 0]=({
struct Cyc_StructFieldDeclListList_tok_struct _temp2700; _temp2700.tag= Cyc_StructFieldDeclListList_tok;
_temp2700.f1=({ struct Cyc_List_List* _temp2701=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2701->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2701->tl=
Cyc_yyget_StructFieldDeclListList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2701;}); _temp2700;}); _temp2699;}); break; case 92:
_LL2698: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2703=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2703[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2704; _temp2704.tag=
Cyc_InitDeclList_tok; _temp2704.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp2704;}); _temp2703;}); break; case 93:
_LL2702: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2706=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2706[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2707; _temp2707.tag=
Cyc_InitDeclList_tok; _temp2707.f1=({ struct Cyc_List_List* _temp2708=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2708->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2708->tl= 0; _temp2708;}); _temp2707;}); _temp2706;}); break; case 94:
_LL2705: yyval=( void*)({ struct Cyc_InitDeclList_tok_struct* _temp2710=( struct
Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp2710[ 0]=({ struct Cyc_InitDeclList_tok_struct _temp2711; _temp2711.tag=
Cyc_InitDeclList_tok; _temp2711.f1=({ struct Cyc_List_List* _temp2712=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2712->hd=( void*)
Cyc_yyget_InitDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2712->tl= Cyc_yyget_InitDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2712;}); _temp2711;}); _temp2710;}); break; case 95:
_LL2709: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2714=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2714[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2715; _temp2715.tag= Cyc_InitDecl_tok; _temp2715.f1=({ struct _tuple15*
_temp2716=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2716->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2716->f2= 0; _temp2716;}); _temp2715;}); _temp2714;}); break; case 96:
_LL2713: yyval=( void*)({ struct Cyc_InitDecl_tok_struct* _temp2718=( struct Cyc_InitDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct)); _temp2718[ 0]=({ struct Cyc_InitDecl_tok_struct
_temp2719; _temp2719.tag= Cyc_InitDecl_tok; _temp2719.f1=({ struct _tuple15*
_temp2720=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp2720->f1=
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp2720->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp2720;}); _temp2719;}); _temp2718;}); break;
case 97: _LL2717: { struct Cyc_Absyn_Tqual tq=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f1; struct
Cyc_List_List* _temp2722=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f3; goto _LL2723; _LL2723: { void* _temp2724=
Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
goto _LL2725; _LL2725: { struct Cyc_List_List* _temp2728; struct Cyc_List_List*
_temp2730; struct _tuple0 _temp2726=(( struct _tuple0(*)( struct Cyc_List_List*
x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _LL2731: _temp2730= _temp2726.f1; goto
_LL2729; _LL2729: _temp2728= _temp2726.f2; goto _LL2727; _LL2727: { struct Cyc_List_List*
_temp2732=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, _temp2724, _temp2730, _temp2722),
_temp2728); goto _LL2733; _LL2733: yyval=( void*)({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2734=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp2734[ 0]=({ struct Cyc_StructFieldDeclList_tok_struct
_temp2735; _temp2735.tag= Cyc_StructFieldDeclList_tok; _temp2735.f1=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*,
struct _tuple9*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
_temp2732); _temp2735;}); _temp2734;}); break;}}}} case 98: _LL2721: yyval=(
void*)({ struct Cyc_QualSpecList_tok_struct* _temp2737=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct)); _temp2737[ 0]=({ struct
Cyc_QualSpecList_tok_struct _temp2738; _temp2738.tag= Cyc_QualSpecList_tok;
_temp2738.f1=({ struct _tuple18* _temp2739=( struct _tuple18*) GC_malloc(
sizeof( struct _tuple18)); _temp2739->f1= Cyc_Absyn_empty_tqual(); _temp2739->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2740=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2740->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2740->tl=
0; _temp2740;}); _temp2739->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp2739;}); _temp2738;}); _temp2737;});
break; case 99: _LL2736: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2742=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2742[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2743; _temp2743.tag=
Cyc_QualSpecList_tok; _temp2743.f1=({ struct _tuple18* _temp2744=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2744->f1=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; _temp2744->f2=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp2745=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2745->hd=( void*) Cyc_yyget_TypeSpecifier_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2745->tl=(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2;
_temp2745;}); _temp2744->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2744;}); _temp2743;}); _temp2742;});
break; case 100: _LL2741: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct*
_temp2747=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2747[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2748; _temp2748.tag=
Cyc_QualSpecList_tok; _temp2748.f1=({ struct _tuple18* _temp2749=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2749->f1= Cyc_yyget_TypeQual_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2749->f2=
0; _temp2749->f3= Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2749;}); _temp2748;}); _temp2747;}); break; case 101:
_LL2746: yyval=( void*)({ struct Cyc_QualSpecList_tok_struct* _temp2751=( struct
Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp2751[ 0]=({ struct Cyc_QualSpecList_tok_struct _temp2752; _temp2752.tag=
Cyc_QualSpecList_tok; _temp2752.f1=({ struct _tuple18* _temp2753=( struct
_tuple18*) GC_malloc( sizeof( struct _tuple18)); _temp2753->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1);
_temp2753->f2=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp2753->f3=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3); _temp2753;});
_temp2752;}); _temp2751;}); break; case 102: _LL2750: yyval=( void*)({ struct
Cyc_DeclaratorExpoptList_tok_struct* _temp2755=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2755[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2756; _temp2756.tag= Cyc_DeclaratorExpoptList_tok;
_temp2756.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))); _temp2756;}); _temp2755;}); break; case 103: _LL2754: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2758=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2758[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2759; _temp2759.tag= Cyc_DeclaratorExpoptList_tok;
_temp2759.f1=({ struct Cyc_List_List* _temp2760=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2760->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2760->tl=
0; _temp2760;}); _temp2759;}); _temp2758;}); break; case 104: _LL2757: yyval=(
void*)({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp2762=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp2762[ 0]=({
struct Cyc_DeclaratorExpoptList_tok_struct _temp2763; _temp2763.tag= Cyc_DeclaratorExpoptList_tok;
_temp2763.f1=({ struct Cyc_List_List* _temp2764=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2764->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2764->tl=
Cyc_yyget_DeclaratorExpoptList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2764;}); _temp2763;}); _temp2762;}); break; case 105:
_LL2761: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2766=(
struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct));
_temp2766[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct _temp2767; _temp2767.tag=
Cyc_DeclaratorExpopt_tok; _temp2767.f1=({ struct _tuple13* _temp2768=( struct
_tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp2768->f1= Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2768->f2=
0; _temp2768;}); _temp2767;}); _temp2766;}); break; case 106: _LL2765: yyval=(
void*)({ struct Cyc_DeclaratorExpopt_tok_struct* _temp2770=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp2770[ 0]=({
struct Cyc_DeclaratorExpopt_tok_struct _temp2771; _temp2771.tag= Cyc_DeclaratorExpopt_tok;
_temp2771.f1=({ struct _tuple13* _temp2772=( struct _tuple13*) GC_malloc(
sizeof( struct _tuple13)); _temp2772->f1=({ struct Cyc_Parse_Declarator*
_temp2774=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp2774->id=({ struct _tuple1* _temp2775=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp2775->f1=( void*)({ struct Cyc_Absyn_Rel_n_struct*
_temp2779=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp2779[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp2780; _temp2780.tag= Cyc_Absyn_Rel_n;
_temp2780.f1= 0; _temp2780;}); _temp2779;}); _temp2775->f2=({ struct
_tagged_string* _temp2776=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp2776[ 0]=( struct _tagged_string)({ char* _temp2777=(
char*)""; struct _tagged_string _temp2778; _temp2778.curr= _temp2777; _temp2778.base=
_temp2777; _temp2778.last_plus_one= _temp2777 + 1; _temp2778;}); _temp2776;});
_temp2775;}); _temp2774->tms= 0; _temp2774;}); _temp2772->f2=({ struct Cyc_Core_Opt*
_temp2773=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2773->v=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp2773;}); _temp2772;}); _temp2771;}); _temp2770;}); break;
case 107: _LL2769: yyval=( void*)({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp2782=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp2782[ 0]=({ struct Cyc_DeclaratorExpopt_tok_struct
_temp2783; _temp2783.tag= Cyc_DeclaratorExpopt_tok; _temp2783.f1=({ struct
_tuple13* _temp2784=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13));
_temp2784->f1= Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2784->f2=({ struct Cyc_Core_Opt* _temp2785=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2785->v=( void*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2785;});
_temp2784;}); _temp2783;}); _temp2782;}); break; case 108: _LL2781: { struct Cyc_List_List*
_temp2787=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL2788; _LL2788: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2789=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2789[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2790; _temp2790.tag=
Cyc_TypeSpecifier_tok; _temp2790.f1=( void*)(( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp2791=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2791[ 0]=({ struct Cyc_Parse_Decl_spec_struct _temp2792; _temp2792.tag= Cyc_Parse_Decl_spec;
_temp2792.f1= Cyc_Absyn_tunion_decl(( void*) Cyc_Absyn_Public, Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), _temp2787,({
struct Cyc_Core_Opt* _temp2793=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2793->v=( void*) Cyc_yyget_TunionFieldList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2793;}), Cyc_yyget_Bool_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2792;}); _temp2791;})); _temp2790;}); _temp2789;}); break;} case 109:
_LL2786: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct* _temp2795=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2795[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2796; _temp2796.tag=
Cyc_TypeSpecifier_tok; _temp2796.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2797=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2797[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2798; _temp2798.tag= Cyc_Absyn_TunionType;
_temp2798.f1=({ struct Cyc_Absyn_TunionInfo _temp2799; _temp2799.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2800=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2800[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2801; _temp2801.tag= Cyc_Absyn_UnknownTunion;
_temp2801.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2802; _temp2802.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2802.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2802;}); _temp2801;}); _temp2800;})); _temp2799.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2799.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp2799;}); _temp2798;});
_temp2797;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp2796;});
_temp2795;}); break; case 110: _LL2794: yyval=( void*)({ struct Cyc_TypeSpecifier_tok_struct*
_temp2804=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2804[ 0]=({ struct Cyc_TypeSpecifier_tok_struct _temp2805; _temp2805.tag=
Cyc_TypeSpecifier_tok; _temp2805.f1=( void*) Cyc_Parse_type_spec(( void*)({
struct Cyc_Absyn_TunionType_struct* _temp2806=( struct Cyc_Absyn_TunionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp2806[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp2807; _temp2807.tag= Cyc_Absyn_TunionType;
_temp2807.f1=({ struct Cyc_Absyn_TunionInfo _temp2808; _temp2808.tunion_info=(
void*)(( void*)({ struct Cyc_Absyn_UnknownTunion_struct* _temp2809=( struct Cyc_Absyn_UnknownTunion_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp2809[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp2810; _temp2810.tag= Cyc_Absyn_UnknownTunion;
_temp2810.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp2811; _temp2811.name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2811.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp2811;}); _temp2810;}); _temp2809;})); _temp2808.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2808.rgn=( void*) Cyc_yyget_Rgn_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp2808;}); _temp2807;}); _temp2806;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2805;}); _temp2804;}); break; case 111: _LL2803: yyval=( void*)({ struct
Cyc_TypeSpecifier_tok_struct* _temp2813=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2813[ 0]=({
struct Cyc_TypeSpecifier_tok_struct _temp2814; _temp2814.tag= Cyc_TypeSpecifier_tok;
_temp2814.f1=( void*) Cyc_Parse_type_spec(( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp2815=( struct Cyc_Absyn_TunionFieldType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp2815[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp2816; _temp2816.tag= Cyc_Absyn_TunionFieldType; _temp2816.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp2817; _temp2817.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp2818=( struct Cyc_Absyn_UnknownTunionfield_struct*) GC_malloc( sizeof(
struct Cyc_Absyn_UnknownTunionfield_struct)); _temp2818[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp2819; _temp2819.tag= Cyc_Absyn_UnknownTunionfield; _temp2819.f1=({ struct
Cyc_Absyn_UnknownTunionFieldInfo _temp2820; _temp2820.tunion_name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2820.field_name=
Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2820.is_xtunion= Cyc_yyget_Bool_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp2820;}); _temp2819;}); _temp2818;})); _temp2817.targs=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2817;}); _temp2816;}); _temp2815;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2814;}); _temp2813;}); break; case 112: _LL2812: yyval=( void*)({ struct
Cyc_Bool_tok_struct* _temp2822=( struct Cyc_Bool_tok_struct*) GC_malloc( sizeof(
struct Cyc_Bool_tok_struct)); _temp2822[ 0]=({ struct Cyc_Bool_tok_struct
_temp2823; _temp2823.tag= Cyc_Bool_tok; _temp2823.f1= 0; _temp2823;}); _temp2822;});
break; case 113: _LL2821: yyval=( void*)({ struct Cyc_Bool_tok_struct* _temp2825=(
struct Cyc_Bool_tok_struct*) GC_malloc( sizeof( struct Cyc_Bool_tok_struct));
_temp2825[ 0]=({ struct Cyc_Bool_tok_struct _temp2826; _temp2826.tag= Cyc_Bool_tok;
_temp2826.f1= 1; _temp2826;}); _temp2825;}); break; case 114: _LL2824: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2828=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2828[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2829; _temp2829.tag= Cyc_TunionFieldList_tok;
_temp2829.f1=({ struct Cyc_List_List* _temp2830=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2830->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2830->tl=
0; _temp2830;}); _temp2829;}); _temp2828;}); break; case 115: _LL2827: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2832=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2832[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2833; _temp2833.tag= Cyc_TunionFieldList_tok;
_temp2833.f1=({ struct Cyc_List_List* _temp2834=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2834->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2834->tl=
0; _temp2834;}); _temp2833;}); _temp2832;}); break; case 116: _LL2831: yyval=(
void*)({ struct Cyc_TunionFieldList_tok_struct* _temp2836=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2836[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2837; _temp2837.tag= Cyc_TunionFieldList_tok;
_temp2837.f1=({ struct Cyc_List_List* _temp2838=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2838->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2838->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2838;}); _temp2837;}); _temp2836;}); break; case 117: _LL2835: yyval=( void*)({
struct Cyc_TunionFieldList_tok_struct* _temp2840=( struct Cyc_TunionFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionFieldList_tok_struct)); _temp2840[ 0]=({
struct Cyc_TunionFieldList_tok_struct _temp2841; _temp2841.tag= Cyc_TunionFieldList_tok;
_temp2841.f1=({ struct Cyc_List_List* _temp2842=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2842->hd=( void*) Cyc_yyget_TunionField_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2842->tl=
Cyc_yyget_TunionFieldList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2842;}); _temp2841;}); _temp2840;}); break; case 118: _LL2839: yyval=( void*)({
struct Cyc_Scope_tok_struct* _temp2844=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2844[ 0]=({ struct Cyc_Scope_tok_struct
_temp2845; _temp2845.tag= Cyc_Scope_tok; _temp2845.f1=( void*)(( void*) Cyc_Absyn_Public);
_temp2845;}); _temp2844;}); break; case 119: _LL2843: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2847=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2847[ 0]=({ struct Cyc_Scope_tok_struct
_temp2848; _temp2848.tag= Cyc_Scope_tok; _temp2848.f1=( void*)(( void*) Cyc_Absyn_Extern);
_temp2848;}); _temp2847;}); break; case 120: _LL2846: yyval=( void*)({ struct
Cyc_Scope_tok_struct* _temp2850=( struct Cyc_Scope_tok_struct*) GC_malloc(
sizeof( struct Cyc_Scope_tok_struct)); _temp2850[ 0]=({ struct Cyc_Scope_tok_struct
_temp2851; _temp2851.tag= Cyc_Scope_tok; _temp2851.f1=( void*)(( void*) Cyc_Absyn_Static);
_temp2851;}); _temp2850;}); break; case 121: _LL2849: yyval=( void*)({ struct
Cyc_TunionField_tok_struct* _temp2853=( struct Cyc_TunionField_tok_struct*)
GC_malloc( sizeof( struct Cyc_TunionField_tok_struct)); _temp2853[ 0]=({ struct
Cyc_TunionField_tok_struct _temp2854; _temp2854.tag= Cyc_TunionField_tok;
_temp2854.f1=({ struct Cyc_Absyn_Tunionfield* _temp2855=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2855->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2855->tvs=
0; _temp2855->typs= 0; _temp2855->loc= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2855->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2855;}); _temp2854;}); _temp2853;}); break; case 122:
_LL2852: { struct Cyc_List_List* _temp2857=(( struct Cyc_List_List*(*)( struct
_tuple11*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2858; _LL2858: { struct Cyc_List_List* _temp2859=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line),
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)));
goto _LL2860; _LL2860: yyval=( void*)({ struct Cyc_TunionField_tok_struct*
_temp2861=( struct Cyc_TunionField_tok_struct*) GC_malloc( sizeof( struct Cyc_TunionField_tok_struct));
_temp2861[ 0]=({ struct Cyc_TunionField_tok_struct _temp2862; _temp2862.tag= Cyc_TunionField_tok;
_temp2862.f1=({ struct Cyc_Absyn_Tunionfield* _temp2863=( struct Cyc_Absyn_Tunionfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Tunionfield)); _temp2863->name= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp2863->tvs=
_temp2859; _temp2863->typs= _temp2857; _temp2863->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2863->sc=( void*) Cyc_yyget_Scope_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp2863;}); _temp2862;}); _temp2861;}); break;}} case 123:
_LL2856: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 124:
_LL2864: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2866=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2866[ 0]=({ struct Cyc_Declarator_tok_struct _temp2867; _temp2867.tag= Cyc_Declarator_tok;
_temp2867.f1=({ struct Cyc_Parse_Declarator* _temp2868=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2868->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->id; _temp2868->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms);
_temp2868;}); _temp2867;}); _temp2866;}); break; case 125: _LL2865: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2870=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2870[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2871; _temp2871.tag= Cyc_Declarator_tok;
_temp2871.f1=({ struct Cyc_Parse_Declarator* _temp2872=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2872->id= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2872->tms=
0; _temp2872;}); _temp2871;}); _temp2870;}); break; case 126: _LL2869: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1); break; case
127: _LL2873: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2875=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2875[ 0]=({ struct Cyc_Declarator_tok_struct _temp2876; _temp2876.tag= Cyc_Declarator_tok;
_temp2876.f1=({ struct Cyc_Parse_Declarator* _temp2877=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2877->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2877->tms=({ struct Cyc_List_List* _temp2878=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2878->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp2878->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)))->tms; _temp2878;}); _temp2877;}); _temp2876;}); _temp2875;});
break; case 128: _LL2874: yyval=( void*)({ struct Cyc_Declarator_tok_struct*
_temp2880=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2880[ 0]=({ struct Cyc_Declarator_tok_struct _temp2881; _temp2881.tag= Cyc_Declarator_tok;
_temp2881.f1=({ struct Cyc_Parse_Declarator* _temp2882=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2882->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2882->tms=({ struct Cyc_List_List* _temp2883=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2883->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp2884=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp2884[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp2885; _temp2885.tag= Cyc_Absyn_ConstArray_mod;
_temp2885.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2885;}); _temp2884;})); _temp2883->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp2883;}); _temp2882;});
_temp2881;}); _temp2880;}); break; case 129: _LL2879: { struct _tuple17
_temp2889; struct Cyc_Core_Opt* _temp2890; int _temp2892; struct Cyc_List_List*
_temp2894; struct _tuple17* _temp2887= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2889=* _temp2887; _LL2895:
_temp2894= _temp2889.f1; goto _LL2893; _LL2893: _temp2892= _temp2889.f2; goto
_LL2891; _LL2891: _temp2890= _temp2889.f3; goto _LL2888; _LL2888: yyval=( void*)({
struct Cyc_Declarator_tok_struct* _temp2896=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2896[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2897; _temp2897.tag= Cyc_Declarator_tok;
_temp2897.f1=({ struct Cyc_Parse_Declarator* _temp2898=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2898->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2898->tms=({ struct Cyc_List_List* _temp2899=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2899->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2900=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2900[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2901; _temp2901.tag= Cyc_Absyn_Function_mod;
_temp2901.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2902=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2902[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2903; _temp2903.tag= Cyc_Absyn_WithTypes;
_temp2903.f1= _temp2894; _temp2903.f2= _temp2892; _temp2903.f3= _temp2890;
_temp2903;}); _temp2902;})); _temp2901;}); _temp2900;})); _temp2899->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2899;}); _temp2898;}); _temp2897;}); _temp2896;}); break;} case 130:
_LL2886: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2905=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2905[ 0]=({ struct Cyc_Declarator_tok_struct _temp2906; _temp2906.tag= Cyc_Declarator_tok;
_temp2906.f1=({ struct Cyc_Parse_Declarator* _temp2907=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2907->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->id;
_temp2907->tms=({ struct Cyc_List_List* _temp2908=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2908->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2909=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2909[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2910; _temp2910.tag= Cyc_Absyn_Function_mod;
_temp2910.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2911=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2911[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2912; _temp2912.tag= Cyc_Absyn_WithTypes;
_temp2912.f1= 0; _temp2912.f2= 0; _temp2912.f3= 0; _temp2912;}); _temp2911;}));
_temp2910;}); _temp2909;})); _temp2908->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)))->tms; _temp2908;}); _temp2907;});
_temp2906;}); _temp2905;}); break; case 131: _LL2904: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2914=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2914[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2915; _temp2915.tag= Cyc_Declarator_tok;
_temp2915.f1=({ struct Cyc_Parse_Declarator* _temp2916=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2916->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->id;
_temp2916->tms=({ struct Cyc_List_List* _temp2917=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2917->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2918=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2918[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2919; _temp2919.tag= Cyc_Absyn_Function_mod;
_temp2919.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp2920=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp2920[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp2921; _temp2921.tag= Cyc_Absyn_WithTypes;
_temp2921.f1= 0; _temp2921.f2= 0; _temp2921.f3=({ struct Cyc_Core_Opt* _temp2922=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2922->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp2923=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp2923[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp2924; _temp2924.tag= Cyc_Absyn_JoinEff; _temp2924.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2924;}); _temp2923;})); _temp2922;}); _temp2921;}); _temp2920;}));
_temp2919;}); _temp2918;})); _temp2917->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)))->tms; _temp2917;}); _temp2916;});
_temp2915;}); _temp2914;}); break; case 132: _LL2913: yyval=( void*)({ struct
Cyc_Declarator_tok_struct* _temp2926=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp2926[ 0]=({ struct
Cyc_Declarator_tok_struct _temp2927; _temp2927.tag= Cyc_Declarator_tok;
_temp2927.f1=({ struct Cyc_Parse_Declarator* _temp2928=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2928->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2928->tms=({ struct Cyc_List_List* _temp2929=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2929->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp2930=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp2930[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp2931; _temp2931.tag= Cyc_Absyn_Function_mod;
_temp2931.f1=( void*)(( void*)({ struct Cyc_Absyn_NoTypes_struct* _temp2932=(
struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp2932[ 0]=({ struct Cyc_Absyn_NoTypes_struct _temp2933; _temp2933.tag= Cyc_Absyn_NoTypes;
_temp2933.f1= Cyc_yyget_IdList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp2933.f2= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2933;});
_temp2932;})); _temp2931;}); _temp2930;})); _temp2929->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2929;}); _temp2928;}); _temp2927;}); _temp2926;}); break; case 133: _LL2925: {
struct Cyc_List_List* _temp2935=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2936; _LL2936: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2937=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2937[ 0]=({ struct Cyc_Declarator_tok_struct _temp2938; _temp2938.tag= Cyc_Declarator_tok;
_temp2938.f1=({ struct Cyc_Parse_Declarator* _temp2939=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2939->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2939->tms=({ struct Cyc_List_List* _temp2940=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2940->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2941=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2941[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2942; _temp2942.tag= Cyc_Absyn_TypeParams_mod;
_temp2942.f1= _temp2935; _temp2942.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2942.f3= 0; _temp2942;}); _temp2941;})); _temp2940->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2940;}); _temp2939;}); _temp2938;}); _temp2937;}); break;} case 134:
_LL2934:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp2944=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL2945; _LL2945: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2946=(
struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2946[ 0]=({ struct Cyc_Declarator_tok_struct _temp2947; _temp2947.tag= Cyc_Declarator_tok;
_temp2947.f1=({ struct Cyc_Parse_Declarator* _temp2948=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2948->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->id;
_temp2948->tms=({ struct Cyc_List_List* _temp2949=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2949->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp2950=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp2950[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp2951; _temp2951.tag= Cyc_Absyn_TypeParams_mod;
_temp2951.f1= _temp2944; _temp2951.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp2951.f3= 0; _temp2951;}); _temp2950;})); _temp2949->tl=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp2949;}); _temp2948;}); _temp2947;}); _temp2946;}); break;} case 135:
_LL2943: yyval=( void*)({ struct Cyc_Declarator_tok_struct* _temp2953=( struct
Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp2953[ 0]=({ struct Cyc_Declarator_tok_struct _temp2954; _temp2954.tag= Cyc_Declarator_tok;
_temp2954.f1=({ struct Cyc_Parse_Declarator* _temp2955=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp2955->id=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))->id;
_temp2955->tms=({ struct Cyc_List_List* _temp2956=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2956->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp2957=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp2957[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp2958; _temp2958.tag= Cyc_Absyn_Attributes_mod;
_temp2958.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp2958.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp2958;});
_temp2957;})); _temp2956->tl=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)))->tms; _temp2956;}); _temp2955;}); _temp2954;});
_temp2953;}); break; case 136: _LL2952: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2960=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2960[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2961; _temp2961.tag= Cyc_TypeModifierList_tok; _temp2961.f1=({ struct Cyc_List_List*
_temp2962=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2962->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2963=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2963[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2964; _temp2964.tag=
Cyc_Absyn_Pointer_mod; _temp2964.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2964.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp2964.f3=
Cyc_Absyn_empty_tqual(); _temp2964;}); _temp2963;})); _temp2962->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), 0); _temp2962;}); _temp2961;}); _temp2960;});
break; case 137: _LL2959: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2966=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2966[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2967; _temp2967.tag= Cyc_TypeModifierList_tok; _temp2967.f1=({ struct Cyc_List_List*
_temp2968=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2968->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2969=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2969[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2970; _temp2970.tag=
Cyc_Absyn_Pointer_mod; _temp2970.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2970.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2970.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp2970;}); _temp2969;})); _temp2968->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
0); _temp2968;}); _temp2967;}); _temp2966;}); break; case 138: _LL2965: yyval=(
void*)({ struct Cyc_TypeModifierList_tok_struct* _temp2972=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp2972[ 0]=({
struct Cyc_TypeModifierList_tok_struct _temp2973; _temp2973.tag= Cyc_TypeModifierList_tok;
_temp2973.f1=({ struct Cyc_List_List* _temp2974=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2974->hd=( void*)(( void*)({
struct Cyc_Absyn_Pointer_mod_struct* _temp2975=( struct Cyc_Absyn_Pointer_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct)); _temp2975[ 0]=({
struct Cyc_Absyn_Pointer_mod_struct _temp2976; _temp2976.tag= Cyc_Absyn_Pointer_mod;
_temp2976.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp2976.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp2976.f3=
Cyc_Absyn_empty_tqual(); _temp2976;}); _temp2975;})); _temp2974->tl= Cyc_Parse_attopt_to_tms(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line), Cyc_yyget_AttributeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_TypeModifierList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp2974;}); _temp2973;}); _temp2972;});
break; case 139: _LL2971: yyval=( void*)({ struct Cyc_TypeModifierList_tok_struct*
_temp2978=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp2978[ 0]=({ struct Cyc_TypeModifierList_tok_struct
_temp2979; _temp2979.tag= Cyc_TypeModifierList_tok; _temp2979.f1=({ struct Cyc_List_List*
_temp2980=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2980->hd=( void*)(( void*)({ struct Cyc_Absyn_Pointer_mod_struct* _temp2981=(
struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp2981[ 0]=({ struct Cyc_Absyn_Pointer_mod_struct _temp2982; _temp2982.tag=
Cyc_Absyn_Pointer_mod; _temp2982.f1=( void*) Cyc_yyget_Pointer_Sort_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)); _temp2982.f2=( void*) Cyc_yyget_Rgn_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp2982.f3=
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp2982;}); _temp2981;})); _temp2980->tl= Cyc_Parse_attopt_to_tms( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line),
Cyc_yyget_AttributeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp2980;}); _temp2979;}); _temp2978;}); break; case 140: _LL2977: yyval=( void*)({
struct Cyc_Pointer_Sort_tok_struct* _temp2984=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp2984[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp2985; _temp2985.tag= Cyc_Pointer_Sort_tok;
_temp2985.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp2986=(
struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2986[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2987; _temp2987.tag=
Cyc_Absyn_Nullable_ps; _temp2987.f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp2987;}); _temp2986;})); _temp2985;}); _temp2984;}); break; case 141:
_LL2983: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp2989=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2989[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2990; _temp2990.tag=
Cyc_Pointer_Sort_tok; _temp2990.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp2991=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp2991[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp2992; _temp2992.tag= Cyc_Absyn_NonNullable_ps; _temp2992.f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp2992;}); _temp2991;})); _temp2990;}); _temp2989;});
break; case 142: _LL2988: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp2994=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2994[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp2995; _temp2995.tag=
Cyc_Pointer_Sort_tok; _temp2995.f1=( void*)(( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp2996=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp2996[ 0]=({ struct Cyc_Absyn_Nullable_ps_struct _temp2997; _temp2997.tag=
Cyc_Absyn_Nullable_ps; _temp2997.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp2997;}); _temp2996;})); _temp2995;});
_temp2994;}); break; case 143: _LL2993: yyval=( void*)({ struct Cyc_Pointer_Sort_tok_struct*
_temp2999=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp2999[ 0]=({ struct Cyc_Pointer_Sort_tok_struct _temp3000; _temp3000.tag=
Cyc_Pointer_Sort_tok; _temp3000.f1=( void*)(( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3001=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3001[ 0]=({ struct Cyc_Absyn_NonNullable_ps_struct
_temp3002; _temp3002.tag= Cyc_Absyn_NonNullable_ps; _temp3002.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3002;});
_temp3001;})); _temp3000;}); _temp2999;}); break; case 144: _LL2998: yyval=(
void*)({ struct Cyc_Pointer_Sort_tok_struct* _temp3004=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3004[ 0]=({ struct
Cyc_Pointer_Sort_tok_struct _temp3005; _temp3005.tag= Cyc_Pointer_Sort_tok;
_temp3005.f1=( void*)(( void*) Cyc_Absyn_TaggedArray_ps); _temp3005;});
_temp3004;}); break; case 145: _LL3003: yyval=( void*)({ struct Cyc_Rgn_tok_struct*
_temp3007=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3007[ 0]=({ struct Cyc_Rgn_tok_struct _temp3008; _temp3008.tag= Cyc_Rgn_tok;
_temp3008.f1=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp3008;}); _temp3007;});
break; case 146: _LL3006: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 147: _LL3009: yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp3011=(
struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3011[ 0]=({ struct Cyc_Rgn_tok_struct _temp3012; _temp3012.tag= Cyc_Rgn_tok;
_temp3012.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3012;}); _temp3011;});
break; case 148: _LL3010: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)) !=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3014=( char*)"expecting region kind\n";
struct _tagged_string _temp3015; _temp3015.curr= _temp3014; _temp3015.base=
_temp3014; _temp3015.last_plus_one= _temp3014 + 23; _temp3015;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_Rgn_tok_struct* _temp3016=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp3016[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3017; _temp3017.tag= Cyc_Rgn_tok; _temp3017.f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind));
_temp3017;}); _temp3016;}); break; case 149: _LL3013: yyval=( void*)({ struct
Cyc_Rgn_tok_struct* _temp3019=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp3019[ 0]=({ struct Cyc_Rgn_tok_struct
_temp3020; _temp3020.tag= Cyc_Rgn_tok; _temp3020.f1=( void*) Cyc_Absyn_new_evar((
void*) Cyc_Absyn_RgnKind); _temp3020;}); _temp3019;}); break; case 150: _LL3018:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 151:
_LL3021: yyval=( void*)({ struct Cyc_TypeQual_tok_struct* _temp3023=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3023[ 0]=({ struct Cyc_TypeQual_tok_struct
_temp3024; _temp3024.tag= Cyc_TypeQual_tok; _temp3024.f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_yyget_TypeQual_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3024;}); _temp3023;}); break; case 152: _LL3022: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3026=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3026[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3027; _temp3027.tag= Cyc_ParamDeclListBool_tok;
_temp3027.f1=({ struct _tuple17* _temp3028=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3028->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp3028->f2= 0; _temp3028->f3= 0; _temp3028;});
_temp3027;}); _temp3026;}); break; case 153: _LL3025: yyval=( void*)({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3030=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3030[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3031; _temp3031.tag= Cyc_ParamDeclListBool_tok;
_temp3031.f1=({ struct _tuple17* _temp3032=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3032->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp3032->f2= 1; _temp3032->f3= 0;
_temp3032;}); _temp3031;}); _temp3030;}); break; case 154: _LL3029: yyval=( void*)({
struct Cyc_ParamDeclListBool_tok_struct* _temp3034=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3034[ 0]=({
struct Cyc_ParamDeclListBool_tok_struct _temp3035; _temp3035.tag= Cyc_ParamDeclListBool_tok;
_temp3035.f1=({ struct _tuple17* _temp3036=( struct _tuple17*) GC_malloc(
sizeof( struct _tuple17)); _temp3036->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp3036->f2= 0; _temp3036->f3=( struct
Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp3037=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3037->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp3038=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3038[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3039; _temp3039.tag= Cyc_Absyn_JoinEff; _temp3039.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3039;}); _temp3038;})); _temp3037;}); _temp3036;}); _temp3035;}); _temp3034;});
break; case 155: _LL3033: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 156: _LL3040: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3042=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3042[ 0]=({ struct Cyc_TypeList_tok_struct _temp3043; _temp3043.tag= Cyc_TypeList_tok;
_temp3043.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))); _temp3043;}); _temp3042;}); break; case 157:
_LL3041: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3045=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3045[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3046; _temp3046.tag= Cyc_TypeList_tok; _temp3046.f1= 0; _temp3046;});
_temp3045;}); break; case 158: _LL3044: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 159: _LL3047: yyval=( void*)({ struct Cyc_TypeList_tok_struct*
_temp3049=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3049[ 0]=({ struct Cyc_TypeList_tok_struct _temp3050; _temp3050.tag= Cyc_TypeList_tok;
_temp3050.f1=({ struct Cyc_List_List* _temp3051=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3051->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),((
struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind));
_temp3051->tl= 0; _temp3051;}); _temp3050;}); _temp3049;}); break; case 160:
_LL3048: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_EffKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp3053=( char*)"expecing effect kind (E)"; struct _tagged_string _temp3054;
_temp3054.curr= _temp3053; _temp3054.base= _temp3053; _temp3054.last_plus_one=
_temp3053 + 25; _temp3054;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp3055=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3055[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3056; _temp3056.tag= Cyc_TypeList_tok; _temp3056.f1=({ struct Cyc_List_List*
_temp3057=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3057->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_EffKind)); _temp3057->tl= 0; _temp3057;});
_temp3056;}); _temp3055;}); break; case 161: _LL3052: yyval=( void*)({ struct
Cyc_TypeList_tok_struct* _temp3059=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3059[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3060; _temp3060.tag= Cyc_TypeList_tok; _temp3060.f1=({ struct Cyc_List_List*
_temp3061=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3061->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3062=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3062[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3063; _temp3063.tag= Cyc_Absyn_AccessEff;
_temp3063.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3063;}); _temp3062;}));
_temp3061->tl= 0; _temp3061;}); _temp3060;}); _temp3059;}); break; case 162:
_LL3058: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3065=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3065[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3066; _temp3066.tag= Cyc_TypeList_tok; _temp3066.f1=({ struct Cyc_List_List*
_temp3067=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3067->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3068=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3068[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3069; _temp3069.tag= Cyc_Absyn_AccessEff;
_temp3069.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3069;}); _temp3068;}));
_temp3067->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3067;}); _temp3066;}); _temp3065;}); break; case 163:
_LL3064: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp3071=( char*)"expecting region kind (R)"; struct _tagged_string _temp3072;
_temp3072.curr= _temp3071; _temp3072.base= _temp3071; _temp3072.last_plus_one=
_temp3071 + 26; _temp3072;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp3073=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3073[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3074; _temp3074.tag= Cyc_TypeList_tok; _temp3074.f1=({ struct Cyc_List_List*
_temp3075=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3075->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3076=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3076[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3077; _temp3077.tag= Cyc_Absyn_AccessEff;
_temp3077.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3077;}); _temp3076;}));
_temp3075->tl= 0; _temp3075;}); _temp3074;}); _temp3073;}); break; case 164:
_LL3070: if( Cyc_yyget_Kind_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))
!=( void*) Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp3079=( char*)"expecting region kind (R)"; struct _tagged_string _temp3080;
_temp3080.curr= _temp3079; _temp3080.base= _temp3079; _temp3080.last_plus_one=
_temp3079 + 26; _temp3080;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));} yyval=( void*)({
struct Cyc_TypeList_tok_struct* _temp3081=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3081[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3082; _temp3082.tag= Cyc_TypeList_tok; _temp3082.f1=({ struct Cyc_List_List*
_temp3083=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3083->hd=( void*)(( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3084=(
struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3084[ 0]=({ struct Cyc_Absyn_AccessEff_struct _temp3085; _temp3085.tag= Cyc_Absyn_AccessEff;
_temp3085.f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 4)),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind)); _temp3085;}); _temp3084;}));
_temp3083->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3083;}); _temp3082;}); _temp3081;}); break; case 165:
_LL3078: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3087=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3087[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3088; _temp3088.tag=
Cyc_ParamDeclList_tok; _temp3088.f1=({ struct Cyc_List_List* _temp3089=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3089->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3089->tl= 0; _temp3089;}); _temp3088;}); _temp3087;}); break; case 166:
_LL3086: yyval=( void*)({ struct Cyc_ParamDeclList_tok_struct* _temp3091=(
struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3091[ 0]=({ struct Cyc_ParamDeclList_tok_struct _temp3092; _temp3092.tag=
Cyc_ParamDeclList_tok; _temp3092.f1=({ struct Cyc_List_List* _temp3093=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3093->hd=( void*)
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3093->tl= Cyc_yyget_ParamDeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3093;}); _temp3092;}); _temp3091;}); break; case 167:
_LL3090: { void* _temp3095= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3096; _LL3096: { struct Cyc_List_List* _temp3097=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3; goto
_LL3098; _LL3098: { struct Cyc_Absyn_Tqual _temp3099=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f1; goto
_LL3100; _LL3100: { struct Cyc_List_List* _temp3101=( Cyc_yyget_Declarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)))->tms; goto
_LL3102; _LL3102: { struct _tuple6 _temp3103= Cyc_Parse_apply_tms( _temp3099,
_temp3095, _temp3097, _temp3101); goto _LL3104; _LL3104: if( _temp3103.f3 != 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp3105=( char*)"parameter with bad type params";
struct _tagged_string _temp3106; _temp3106.curr= _temp3105; _temp3106.base=
_temp3105; _temp3106.last_plus_one= _temp3105 + 31; _temp3106;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct _tuple1* _temp3107=( Cyc_yyget_Declarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)))->id; goto _LL3108; _LL3108:{ void* _temp3109=(*
_temp3107).f1; struct Cyc_List_List* _temp3119; struct Cyc_List_List* _temp3121;
_LL3111: if( _temp3109 ==( void*) Cyc_Absyn_Loc_n){ goto _LL3112;} else{ goto
_LL3113;} _LL3113: if(( unsigned int) _temp3109 > 1u?*(( int*) _temp3109) == Cyc_Absyn_Rel_n:
0){ _LL3120: _temp3119=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp3109)->f1; if( _temp3119 == 0){ goto _LL3114;} else{ goto _LL3115;}} else{
goto _LL3115;} _LL3115: if(( unsigned int) _temp3109 > 1u?*(( int*) _temp3109)
== Cyc_Absyn_Abs_n: 0){ _LL3122: _temp3121=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp3109)->f1; if( _temp3121 == 0){ goto _LL3116;} else{ goto _LL3117;}} else{
goto _LL3117;} _LL3117: goto _LL3118; _LL3112: goto _LL3110; _LL3114: goto
_LL3110; _LL3116: goto _LL3110; _LL3118: Cyc_Parse_err(( struct _tagged_string)({
char* _temp3123=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp3124; _temp3124.curr= _temp3123; _temp3124.base=
_temp3123; _temp3124.last_plus_one= _temp3123 + 49; _temp3124;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
goto _LL3110; _LL3110:;}{ struct Cyc_Core_Opt* _temp3126=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp3125=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3125->v=( void*)(* _temp3107).f2; _temp3125;}); goto
_LL3127; _LL3127: if( _temp3103.f4 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp3128=( char*)"extra attributes on parameter, ignoring"; struct
_tagged_string _temp3129; _temp3129.curr= _temp3128; _temp3129.base= _temp3128;
_temp3129.last_plus_one= _temp3128 + 40; _temp3129;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
if( _temp3103.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp3130=( char*)"extra type variables on parameter, ignoring"; struct
_tagged_string _temp3131; _temp3131.curr= _temp3130; _temp3131.base= _temp3130;
_temp3131.last_plus_one= _temp3130 + 44; _temp3131;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3132=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3132[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3133; _temp3133.tag= Cyc_ParamDecl_tok; _temp3133.f1=({
struct _tuple2* _temp3134=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3134->f1= _temp3126; _temp3134->f2= _temp3103.f1; _temp3134->f3= _temp3103.f2;
_temp3134;}); _temp3133;}); _temp3132;}); break;}}}}}}} case 168: _LL3094: {
void* _temp3136= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3137; _LL3137: { struct Cyc_List_List* _temp3138=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3139; _LL3139: { struct Cyc_Absyn_Tqual _temp3140=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1; goto
_LL3141; _LL3141: if( _temp3138 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp3142=( char*)"bad attributes on parameter, ignoring"; struct
_tagged_string _temp3143; _temp3143.curr= _temp3142; _temp3143.base= _temp3142;
_temp3143.last_plus_one= _temp3142 + 38; _temp3143;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3144=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3144[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3145; _temp3145.tag= Cyc_ParamDecl_tok; _temp3145.f1=({
struct _tuple2* _temp3146=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3146->f1= 0; _temp3146->f2= _temp3140; _temp3146->f3= _temp3136; _temp3146;});
_temp3145;}); _temp3144;}); break;}}} case 169: _LL3135: { void* _temp3148= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3149; _LL3149: { struct Cyc_List_List*
_temp3150=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3151; _LL3151: { struct Cyc_Absyn_Tqual
_temp3152=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3153; _LL3153: { struct Cyc_List_List* _temp3154=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3155; _LL3155: { struct _tuple6 _temp3156= Cyc_Parse_apply_tms(
_temp3152, _temp3148, _temp3150, _temp3154); goto _LL3157; _LL3157: if(
_temp3156.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3158=(
char*)"bad type parameters on formal argument, ignoring"; struct _tagged_string
_temp3159; _temp3159.curr= _temp3158; _temp3159.base= _temp3158; _temp3159.last_plus_one=
_temp3158 + 49; _temp3159;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} if( _temp3156.f4
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3160=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp3161; _temp3161.curr= _temp3160; _temp3161.base=
_temp3160; _temp3161.last_plus_one= _temp3160 + 38; _temp3161;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3162=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3162[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3163; _temp3163.tag= Cyc_ParamDecl_tok; _temp3163.f1=({
struct _tuple2* _temp3164=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3164->f1= 0; _temp3164->f2= _temp3156.f1; _temp3164->f3= _temp3156.f2;
_temp3164;}); _temp3163;}); _temp3162;}); break;}}}}} case 170: _LL3147: yyval=(
void*)({ struct Cyc_IdList_tok_struct* _temp3166=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct)); _temp3166[ 0]=({ struct Cyc_IdList_tok_struct
_temp3167; _temp3167.tag= Cyc_IdList_tok; _temp3167.f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset))); _temp3167;}); _temp3166;}); break; case
171: _LL3165: yyval=( void*)({ struct Cyc_IdList_tok_struct* _temp3169=( struct
Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp3169[ 0]=({ struct Cyc_IdList_tok_struct _temp3170; _temp3170.tag= Cyc_IdList_tok;
_temp3170.f1=({ struct Cyc_List_List* _temp3171=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3171->hd=( void*)({ struct
_tagged_string* _temp3172=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp3172[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3172;}); _temp3171->tl= 0; _temp3171;});
_temp3170;}); _temp3169;}); break; case 172: _LL3168: yyval=( void*)({ struct
Cyc_IdList_tok_struct* _temp3174=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct)); _temp3174[ 0]=({ struct Cyc_IdList_tok_struct
_temp3175; _temp3175.tag= Cyc_IdList_tok; _temp3175.f1=({ struct Cyc_List_List*
_temp3176=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3176->hd=( void*)({ struct _tagged_string* _temp3177=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3177[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3177;}); _temp3176->tl= Cyc_yyget_IdList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3176;}); _temp3175;}); _temp3174;});
break; case 173: _LL3173: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 174: _LL3178: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 175: _LL3179: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3181=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3181[ 0]=({ struct Cyc_Exp_tok_struct _temp3182; _temp3182.tag= Cyc_Exp_tok;
_temp3182.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3183=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3183[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3184; _temp3184.tag= Cyc_Absyn_UnresolvedMem_e; _temp3184.f1= 0; _temp3184.f2=
0; _temp3184;}); _temp3183;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3182;});
_temp3181;}); break; case 176: _LL3180: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3186=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3186[ 0]=({ struct Cyc_Exp_tok_struct _temp3187; _temp3187.tag= Cyc_Exp_tok;
_temp3187.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3188=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3188[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3189; _temp3189.tag= Cyc_Absyn_UnresolvedMem_e; _temp3189.f1= 0; _temp3189.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp3189;});
_temp3188;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3187;});
_temp3186;}); break; case 177: _LL3185: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3191=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3191[ 0]=({ struct Cyc_Exp_tok_struct _temp3192; _temp3192.tag= Cyc_Exp_tok;
_temp3192.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp3193=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp3193[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp3194; _temp3194.tag= Cyc_Absyn_UnresolvedMem_e; _temp3194.f1= 0; _temp3194.f2=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); _temp3194;});
_temp3193;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3192;});
_temp3191;}); break; case 178: _LL3190: { struct Cyc_Absyn_Vardecl* _temp3198=
Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3196=( struct _tuple1*) GC_malloc(
sizeof( struct _tuple1)); _temp3196->f1=( void*) Cyc_Absyn_Loc_n; _temp3196->f2=({
struct _tagged_string* _temp3197=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3197[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3197;}); _temp3196;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).last_line)));
goto _LL3199; _LL3199: _temp3198->tq=({ struct Cyc_Absyn_Tqual _temp3200;
_temp3200.q_const= 1; _temp3200.q_volatile= 0; _temp3200.q_restrict= 1;
_temp3200;}); yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3201=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3201[ 0]=({ struct Cyc_Exp_tok_struct
_temp3202; _temp3202.tag= Cyc_Exp_tok; _temp3202.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Comprehension_e_struct* _temp3203=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp3203[ 0]=({
struct Cyc_Absyn_Comprehension_e_struct _temp3204; _temp3204.tag= Cyc_Absyn_Comprehension_e;
_temp3204.f1= _temp3198; _temp3204.f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3204.f3= Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3204;}); _temp3203;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 7)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3202;}); _temp3201;}); break;} case 179: _LL3195: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3206=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3206[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3207; _temp3207.tag= Cyc_InitializerList_tok;
_temp3207.f1=({ struct Cyc_List_List* _temp3208=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3208->hd=( void*)({ struct
_tuple19* _temp3209=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3209->f1= 0; _temp3209->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3209;}); _temp3208->tl= 0; _temp3208;});
_temp3207;}); _temp3206;}); break; case 180: _LL3205: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3211=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3211[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3212; _temp3212.tag= Cyc_InitializerList_tok;
_temp3212.f1=({ struct Cyc_List_List* _temp3213=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3213->hd=( void*)({ struct
_tuple19* _temp3214=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3214->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3214->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3214;}); _temp3213->tl= 0; _temp3213;});
_temp3212;}); _temp3211;}); break; case 181: _LL3210: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3216=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3216[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3217; _temp3217.tag= Cyc_InitializerList_tok;
_temp3217.f1=({ struct Cyc_List_List* _temp3218=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3218->hd=( void*)({ struct
_tuple19* _temp3219=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3219->f1= 0; _temp3219->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3219;}); _temp3218->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3218;});
_temp3217;}); _temp3216;}); break; case 182: _LL3215: yyval=( void*)({ struct
Cyc_InitializerList_tok_struct* _temp3221=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp3221[ 0]=({
struct Cyc_InitializerList_tok_struct _temp3222; _temp3222.tag= Cyc_InitializerList_tok;
_temp3222.f1=({ struct Cyc_List_List* _temp3223=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3223->hd=( void*)({ struct
_tuple19* _temp3224=( struct _tuple19*) GC_malloc( sizeof( struct _tuple19));
_temp3224->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3224->f2= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3224;}); _temp3223->tl= Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3223;});
_temp3222;}); _temp3221;}); break; case 183: _LL3220: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3226=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3226[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3227; _temp3227.tag= Cyc_DesignatorList_tok;
_temp3227.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)));
_temp3227;}); _temp3226;}); break; case 184: _LL3225: yyval=( void*)({ struct
Cyc_DesignatorList_tok_struct* _temp3229=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3229[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3230; _temp3230.tag= Cyc_DesignatorList_tok;
_temp3230.f1=({ struct Cyc_List_List* _temp3231=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3231->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3231->tl=
0; _temp3231;}); _temp3230;}); _temp3229;}); break; case 185: _LL3228: yyval=(
void*)({ struct Cyc_DesignatorList_tok_struct* _temp3233=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct)); _temp3233[ 0]=({
struct Cyc_DesignatorList_tok_struct _temp3234; _temp3234.tag= Cyc_DesignatorList_tok;
_temp3234.f1=({ struct Cyc_List_List* _temp3235=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3235->hd=( void*) Cyc_yyget_Designator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3235->tl=
Cyc_yyget_DesignatorList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3235;}); _temp3234;}); _temp3233;}); break; case 186: _LL3232: yyval=( void*)({
struct Cyc_Designator_tok_struct* _temp3237=( struct Cyc_Designator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Designator_tok_struct)); _temp3237[ 0]=({ struct
Cyc_Designator_tok_struct _temp3238; _temp3238.tag= Cyc_Designator_tok;
_temp3238.f1=( void*)(( void*)({ struct Cyc_Absyn_ArrayElement_struct* _temp3239=(
struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp3239[ 0]=({ struct Cyc_Absyn_ArrayElement_struct _temp3240; _temp3240.tag=
Cyc_Absyn_ArrayElement; _temp3240.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3240;}); _temp3239;})); _temp3238;});
_temp3237;}); break; case 187: _LL3236: yyval=( void*)({ struct Cyc_Designator_tok_struct*
_temp3242=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp3242[ 0]=({ struct Cyc_Designator_tok_struct _temp3243; _temp3243.tag= Cyc_Designator_tok;
_temp3243.f1=( void*)(( void*)({ struct Cyc_Absyn_FieldName_struct* _temp3244=(
struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp3244[ 0]=({ struct Cyc_Absyn_FieldName_struct _temp3245; _temp3245.tag= Cyc_Absyn_FieldName;
_temp3245.f1=({ struct _tagged_string* _temp3246=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3246[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3246;});
_temp3245;}); _temp3244;})); _temp3243;}); _temp3242;}); break; case 188:
_LL3241: { void* _temp3248= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3249; _LL3249: { struct Cyc_List_List* _temp3250=(* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; goto
_LL3251; _LL3251: if( _temp3250 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp3252=( char*)"ignoring attributes in type"; struct _tagged_string
_temp3253; _temp3253.curr= _temp3252; _temp3253.base= _temp3252; _temp3253.last_plus_one=
_temp3252 + 28; _temp3253;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{ struct Cyc_Absyn_Tqual
_temp3254=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f1; goto _LL3255; _LL3255: yyval=( void*)({ struct Cyc_ParamDecl_tok_struct*
_temp3256=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp3256[ 0]=({ struct Cyc_ParamDecl_tok_struct _temp3257; _temp3257.tag= Cyc_ParamDecl_tok;
_temp3257.f1=({ struct _tuple2* _temp3258=( struct _tuple2*) GC_malloc( sizeof(
struct _tuple2) * 1); _temp3258[ 0]=({ struct _tuple2 _temp3259; _temp3259.f1= 0;
_temp3259.f2= _temp3254; _temp3259.f3= _temp3248; _temp3259;}); _temp3258;});
_temp3257;}); _temp3256;}); break;}}} case 189: _LL3247: { void* _temp3261= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); goto _LL3262; _LL3262: { struct Cyc_List_List*
_temp3263=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f3; goto _LL3264; _LL3264: { struct Cyc_Absyn_Tqual
_temp3265=(* Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1))).f1; goto _LL3266; _LL3266: { struct Cyc_List_List* _temp3267=(
Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)))->tms; goto _LL3268; _LL3268: { struct _tuple6 _temp3269= Cyc_Parse_apply_tms(
_temp3265, _temp3261, _temp3263, _temp3267); goto _LL3270; _LL3270: if(
_temp3269.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3271=(
char*)"bad type params, ignoring"; struct _tagged_string _temp3272; _temp3272.curr=
_temp3271; _temp3272.base= _temp3271; _temp3272.last_plus_one= _temp3271 + 26;
_temp3272;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));} if( _temp3269.f4
!= 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3273=( char*)"bad specifiers, ignoring";
struct _tagged_string _temp3274; _temp3274.curr= _temp3273; _temp3274.base=
_temp3273; _temp3274.last_plus_one= _temp3273 + 25; _temp3274;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}
yyval=( void*)({ struct Cyc_ParamDecl_tok_struct* _temp3275=( struct Cyc_ParamDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct)); _temp3275[ 0]=({ struct
Cyc_ParamDecl_tok_struct _temp3276; _temp3276.tag= Cyc_ParamDecl_tok; _temp3276.f1=({
struct _tuple2* _temp3277=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3277->f1= 0; _temp3277->f2= _temp3269.f1; _temp3277->f3= _temp3269.f2;
_temp3277;}); _temp3276;}); _temp3275;}); break;}}}}} case 190: _LL3260: yyval=(
void*)({ struct Cyc_Type_tok_struct* _temp3279=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct)); _temp3279[ 0]=({ struct Cyc_Type_tok_struct
_temp3280; _temp3280.tag= Cyc_Type_tok; _temp3280.f1=( void*)(* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f3; _temp3280;});
_temp3279;}); break; case 191: _LL3278: yyval=( void*)({ struct Cyc_Type_tok_struct*
_temp3282=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3282[ 0]=({ struct Cyc_Type_tok_struct _temp3283; _temp3283.tag= Cyc_Type_tok;
_temp3283.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3284=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3284[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3285; _temp3285.tag= Cyc_Absyn_JoinEff;
_temp3285.f1= 0; _temp3285;}); _temp3284;})); _temp3283;}); _temp3282;}); break;
case 192: _LL3281: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3287=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3287[ 0]=({ struct Cyc_Type_tok_struct _temp3288; _temp3288.tag= Cyc_Type_tok;
_temp3288.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3289=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3289[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3290; _temp3290.tag= Cyc_Absyn_JoinEff;
_temp3290.f1= Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp3290;}); _temp3289;})); _temp3288;}); _temp3287;});
break; case 193: _LL3286: yyval=( void*)({ struct Cyc_Type_tok_struct* _temp3292=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp3292[ 0]=({ struct Cyc_Type_tok_struct _temp3293; _temp3293.tag= Cyc_Type_tok;
_temp3293.f1=( void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3294=(
struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3294[ 0]=({ struct Cyc_Absyn_JoinEff_struct _temp3295; _temp3295.tag= Cyc_Absyn_JoinEff;
_temp3295.f1=({ struct Cyc_List_List* _temp3296=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3296->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3296->tl=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3296;}); _temp3295;}); _temp3294;})); _temp3293;}); _temp3292;}); break;
case 194: _LL3291: yyval=( void*)({ struct Cyc_TypeList_tok_struct* _temp3298=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3298[ 0]=({ struct Cyc_TypeList_tok_struct _temp3299; _temp3299.tag= Cyc_TypeList_tok;
_temp3299.f1=({ struct Cyc_List_List* _temp3300=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3300->hd=( void*) Cyc_yyget_Type_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3300->tl=
0; _temp3300;}); _temp3299;}); _temp3298;}); break; case 195: _LL3297: yyval=(
void*)({ struct Cyc_TypeList_tok_struct* _temp3302=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3302[ 0]=({ struct Cyc_TypeList_tok_struct
_temp3303; _temp3303.tag= Cyc_TypeList_tok; _temp3303.f1=({ struct Cyc_List_List*
_temp3304=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3304->hd=( void*) Cyc_yyget_Type_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3304->tl= Cyc_yyget_TypeList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)); _temp3304;}); _temp3303;}); _temp3302;});
break; case 196: _LL3301: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3306=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3306[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3307; _temp3307.tag= Cyc_AbstractDeclarator_tok; _temp3307.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3308=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3308->tms= Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3308;}); _temp3307;}); _temp3306;}); break;
case 197: _LL3305: yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset);
break; case 198: _LL3309: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3311=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3311[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3312; _temp3312.tag= Cyc_AbstractDeclarator_tok; _temp3312.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3313=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3313->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)),( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)))->tms); _temp3313;}); _temp3312;});
_temp3311;}); break; case 199: _LL3310: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 200: _LL3314: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3316=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3316[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3317; _temp3317.tag= Cyc_AbstractDeclarator_tok; _temp3317.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3318=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3318->tms=({ struct Cyc_List_List* _temp3319=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3319->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3319->tl= 0; _temp3319;}); _temp3318;}); _temp3317;}); _temp3316;}); break;
case 201: _LL3315: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3321=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3321[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3322; _temp3322.tag= Cyc_AbstractDeclarator_tok; _temp3322.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3323=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3323->tms=({ struct Cyc_List_List* _temp3324=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3324->hd=( void*)(( void*) Cyc_Absyn_Carray_mod);
_temp3324->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)))->tms; _temp3324;}); _temp3323;}); _temp3322;});
_temp3321;}); break; case 202: _LL3320: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3326=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3326[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3327; _temp3327.tag= Cyc_AbstractDeclarator_tok; _temp3327.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3328=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3328->tms=({ struct Cyc_List_List* _temp3329=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3329->hd=( void*)(( void*)({
struct Cyc_Absyn_ConstArray_mod_struct* _temp3330=( struct Cyc_Absyn_ConstArray_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct)); _temp3330[ 0]=({
struct Cyc_Absyn_ConstArray_mod_struct _temp3331; _temp3331.tag= Cyc_Absyn_ConstArray_mod;
_temp3331.f1= Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3331;}); _temp3330;})); _temp3329->tl= 0; _temp3329;}); _temp3328;});
_temp3327;}); _temp3326;}); break; case 203: _LL3325: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3333=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3333[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3334; _temp3334.tag= Cyc_AbstractDeclarator_tok;
_temp3334.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3335=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3335->tms=({
struct Cyc_List_List* _temp3336=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3336->hd=( void*)(( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3337=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3337[ 0]=({ struct Cyc_Absyn_ConstArray_mod_struct
_temp3338; _temp3338.tag= Cyc_Absyn_ConstArray_mod; _temp3338.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3338;});
_temp3337;})); _temp3336->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)))->tms; _temp3336;}); _temp3335;});
_temp3334;}); _temp3333;}); break; case 204: _LL3332: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3340=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3340[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3341; _temp3341.tag= Cyc_AbstractDeclarator_tok;
_temp3341.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3342=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3342->tms=({
struct Cyc_List_List* _temp3343=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3343->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3344=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3344[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3345; _temp3345.tag=
Cyc_Absyn_Function_mod; _temp3345.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3346=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3346[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3347; _temp3347.tag= Cyc_Absyn_WithTypes;
_temp3347.f1= 0; _temp3347.f2= 0; _temp3347.f3= 0; _temp3347;}); _temp3346;}));
_temp3345;}); _temp3344;})); _temp3343->tl= 0; _temp3343;}); _temp3342;});
_temp3341;}); _temp3340;}); break; case 205: _LL3339: yyval=( void*)({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3349=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3349[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3350; _temp3350.tag= Cyc_AbstractDeclarator_tok;
_temp3350.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3351=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3351->tms=({
struct Cyc_List_List* _temp3352=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3352->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3353=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3353[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3354; _temp3354.tag=
Cyc_Absyn_Function_mod; _temp3354.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3355=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3355[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3356; _temp3356.tag= Cyc_Absyn_WithTypes;
_temp3356.f1= 0; _temp3356.f2= 0; _temp3356.f3=({ struct Cyc_Core_Opt* _temp3357=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3357->v=(
void*)(( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3358=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3358[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3359; _temp3359.tag= Cyc_Absyn_JoinEff; _temp3359.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3359;}); _temp3358;})); _temp3357;}); _temp3356;}); _temp3355;}));
_temp3354;}); _temp3353;})); _temp3352->tl= 0; _temp3352;}); _temp3351;});
_temp3350;}); _temp3349;}); break; case 206: _LL3348: { struct _tuple17
_temp3363; struct Cyc_Core_Opt* _temp3364; int _temp3366; struct Cyc_List_List*
_temp3368; struct _tuple17* _temp3361= Cyc_yyget_ParamDeclListBool_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3363=* _temp3361; _LL3369:
_temp3368= _temp3363.f1; goto _LL3367; _LL3367: _temp3366= _temp3363.f2; goto
_LL3365; _LL3365: _temp3364= _temp3363.f3; goto _LL3362; _LL3362: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3370=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3370[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3371; _temp3371.tag= Cyc_AbstractDeclarator_tok;
_temp3371.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3372=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3372->tms=({
struct Cyc_List_List* _temp3373=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3373->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3374=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3374[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3375; _temp3375.tag=
Cyc_Absyn_Function_mod; _temp3375.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3376=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3376[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3377; _temp3377.tag= Cyc_Absyn_WithTypes;
_temp3377.f1= _temp3368; _temp3377.f2= _temp3366; _temp3377.f3= _temp3364;
_temp3377;}); _temp3376;})); _temp3375;}); _temp3374;})); _temp3373->tl= 0;
_temp3373;}); _temp3372;}); _temp3371;}); _temp3370;}); break;} case 207:
_LL3360: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3379=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3379[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3380; _temp3380.tag=
Cyc_AbstractDeclarator_tok; _temp3380.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3381=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3381->tms=({ struct Cyc_List_List* _temp3382=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3382->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3383=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3383[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3384; _temp3384.tag= Cyc_Absyn_Function_mod;
_temp3384.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3385=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3385[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3386; _temp3386.tag= Cyc_Absyn_WithTypes;
_temp3386.f1= 0; _temp3386.f2= 0; _temp3386.f3= 0; _temp3386;}); _temp3385;}));
_temp3384;}); _temp3383;})); _temp3382->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)))->tms;
_temp3382;}); _temp3381;}); _temp3380;}); _temp3379;}); break; case 208: _LL3378: {
struct Cyc_Core_Opt* _temp3391=({ struct Cyc_Core_Opt* _temp3388=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3388->v=( void*)(( void*)({
struct Cyc_Absyn_JoinEff_struct* _temp3389=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3389[ 0]=({ struct
Cyc_Absyn_JoinEff_struct _temp3390; _temp3390.tag= Cyc_Absyn_JoinEff; _temp3390.f1=
Cyc_yyget_TypeList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3390;}); _temp3389;})); _temp3388;}); goto _LL3392; _LL3392: yyval=( void*)({
struct Cyc_AbstractDeclarator_tok_struct* _temp3393=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp3393[ 0]=({
struct Cyc_AbstractDeclarator_tok_struct _temp3394; _temp3394.tag= Cyc_AbstractDeclarator_tok;
_temp3394.f1=({ struct Cyc_Parse_Abstractdeclarator* _temp3395=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3395->tms=({
struct Cyc_List_List* _temp3396=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3396->hd=( void*)(( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3397=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3397[ 0]=({ struct Cyc_Absyn_Function_mod_struct _temp3398; _temp3398.tag=
Cyc_Absyn_Function_mod; _temp3398.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3399=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3399[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3400; _temp3400.tag= Cyc_Absyn_WithTypes;
_temp3400.f1= 0; _temp3400.f2= 0; _temp3400.f3= _temp3391; _temp3400;});
_temp3399;})); _temp3398;}); _temp3397;})); _temp3396->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)))->tms;
_temp3396;}); _temp3395;}); _temp3394;}); _temp3393;}); break;} case 209:
_LL3387: { struct _tuple17 _temp3404; struct Cyc_Core_Opt* _temp3405; int
_temp3407; struct Cyc_List_List* _temp3409; struct _tuple17* _temp3402= Cyc_yyget_ParamDeclListBool_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3404=*
_temp3402; _LL3410: _temp3409= _temp3404.f1; goto _LL3408; _LL3408: _temp3407=
_temp3404.f2; goto _LL3406; _LL3406: _temp3405= _temp3404.f3; goto _LL3403;
_LL3403: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3411=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3411[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3412; _temp3412.tag=
Cyc_AbstractDeclarator_tok; _temp3412.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3413=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3413->tms=({ struct Cyc_List_List* _temp3414=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3414->hd=( void*)(( void*)({
struct Cyc_Absyn_Function_mod_struct* _temp3415=( struct Cyc_Absyn_Function_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct)); _temp3415[ 0]=({
struct Cyc_Absyn_Function_mod_struct _temp3416; _temp3416.tag= Cyc_Absyn_Function_mod;
_temp3416.f1=( void*)(( void*)({ struct Cyc_Absyn_WithTypes_struct* _temp3417=(
struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3417[ 0]=({ struct Cyc_Absyn_WithTypes_struct _temp3418; _temp3418.tag= Cyc_Absyn_WithTypes;
_temp3418.f1= _temp3409; _temp3418.f2= _temp3407; _temp3418.f3= _temp3405;
_temp3418;}); _temp3417;})); _temp3416;}); _temp3415;})); _temp3414->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3414;}); _temp3413;}); _temp3412;}); _temp3411;}); break;} case 210:
_LL3401: { struct Cyc_List_List* _temp3420=(( struct Cyc_List_List*(*)( struct
Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL3421; _LL3421: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3422=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3422[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3423; _temp3423.tag= Cyc_AbstractDeclarator_tok; _temp3423.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3424=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3424->tms=({ struct Cyc_List_List* _temp3425=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3425->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3426=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3426[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3427; _temp3427.tag= Cyc_Absyn_TypeParams_mod;
_temp3427.f1= _temp3420; _temp3427.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3427.f3= 0; _temp3427;}); _temp3426;})); _temp3425->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3425;}); _temp3424;}); _temp3423;}); _temp3422;}); break;} case 211:
_LL3419:(( struct Cyc_Lexing_lexbuf*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Parse_lbuf))->v)->lex_curr_pos
-= 1;{ struct Cyc_List_List* _temp3429=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(*
f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line),((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)))); goto
_LL3430; _LL3430: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3431=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp3431[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct
_temp3432; _temp3432.tag= Cyc_AbstractDeclarator_tok; _temp3432.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3433=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3433->tms=({ struct Cyc_List_List* _temp3434=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3434->hd=( void*)(( void*)({
struct Cyc_Absyn_TypeParams_mod_struct* _temp3435=( struct Cyc_Absyn_TypeParams_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct)); _temp3435[ 0]=({
struct Cyc_Absyn_TypeParams_mod_struct _temp3436; _temp3436.tag= Cyc_Absyn_TypeParams_mod;
_temp3436.f1= _temp3429; _temp3436.f2= Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3436.f3= 0; _temp3436;}); _temp3435;})); _temp3434->tl=( Cyc_yyget_AbstractDeclarator_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)))->tms;
_temp3434;}); _temp3433;}); _temp3432;}); _temp3431;}); break;} case 212:
_LL3428: yyval=( void*)({ struct Cyc_AbstractDeclarator_tok_struct* _temp3438=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp3438[ 0]=({ struct Cyc_AbstractDeclarator_tok_struct _temp3439; _temp3439.tag=
Cyc_AbstractDeclarator_tok; _temp3439.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3440=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3440->tms=({ struct Cyc_List_List* _temp3441=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3441->hd=( void*)(( void*)({
struct Cyc_Absyn_Attributes_mod_struct* _temp3442=( struct Cyc_Absyn_Attributes_mod_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct)); _temp3442[ 0]=({
struct Cyc_Absyn_Attributes_mod_struct _temp3443; _temp3443.tag= Cyc_Absyn_Attributes_mod;
_temp3443.f1= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3443.f2= Cyc_yyget_AttributeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3443;});
_temp3442;})); _temp3441->tl=( Cyc_yyget_AbstractDeclarator_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)))->tms; _temp3441;}); _temp3440;});
_temp3439;}); _temp3438;}); break; case 213: _LL3437: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 214: _LL3444: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 215: _LL3445: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 216: _LL3446: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 217: _LL3447: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 218: _LL3448: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 219: _LL3449: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),(
struct _tagged_string)({ char* _temp3451=( char*)"`H"; struct _tagged_string
_temp3452; _temp3452.curr= _temp3451; _temp3452.base= _temp3451; _temp3452.last_plus_one=
_temp3451 + 3; _temp3452;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp3453=( char*)"bad occurrence of heap region `H"; struct
_tagged_string _temp3454; _temp3454.curr= _temp3453; _temp3454.base= _temp3453;
_temp3454.last_plus_one= _temp3453 + 33; _temp3454;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp3465=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3465->name=({ struct
_tagged_string* _temp3466=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3466[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 3)); _temp3466;}); _temp3465->kind=(( struct Cyc_Absyn_Conref*(*)(
void* x)) Cyc_Absyn_new_conref)(( void*) Cyc_Absyn_RgnKind); _temp3465;}); void*
t=( void*)({ struct Cyc_Absyn_VarType_struct* _temp3463=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp3463[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp3464; _temp3464.tag= Cyc_Absyn_VarType; _temp3464.f1=
tv; _temp3464;}); _temp3463;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3455=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3455[ 0]=({ struct Cyc_Stmt_tok_struct _temp3456; _temp3456.tag= Cyc_Stmt_tok;
_temp3456.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3457=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3457[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3458; _temp3458.tag= Cyc_Absyn_Region_s;
_temp3458.f1= tv; _temp3458.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3459=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3459->f1=(
void*) Cyc_Absyn_Loc_n; _temp3459->f2=({ struct _tagged_string* _temp3460=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3460[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3460;}); _temp3459;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3461=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3461[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3462; _temp3462.tag= Cyc_Absyn_RgnHandleType; _temp3462.f1=( void*) t;
_temp3462;}); _temp3461;}), 0); _temp3458.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3458;}); _temp3457;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3456;}); _temp3455;}); break;} case 220: _LL3450: if( Cyc_String_zstrcmp(
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),(
struct _tagged_string)({ char* _temp3468=( char*)"H"; struct _tagged_string
_temp3469; _temp3469.curr= _temp3468; _temp3469.base= _temp3468; _temp3469.last_plus_one=
_temp3468 + 2; _temp3469;})) == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp3470=( char*)"bad occurrence of heap region `H"; struct
_tagged_string _temp3471; _temp3471.curr= _temp3470; _temp3471.base= _temp3470;
_temp3471.last_plus_one= _temp3470 + 33; _temp3471;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));}{
struct Cyc_Absyn_Tvar* tv=({ struct Cyc_Absyn_Tvar* _temp3482=( struct Cyc_Absyn_Tvar*)
GC_malloc( sizeof( struct Cyc_Absyn_Tvar)); _temp3482->name=({ struct
_tagged_string* _temp3483=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string)); _temp3483[ 0]=({ struct _tagged_string _temp3484= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); xprintf("`%.*s",
_temp3484.last_plus_one - _temp3484.curr, _temp3484.curr);}); _temp3483;});
_temp3482->kind=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_RgnKind); _temp3482;}); void* t=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp3480=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp3480[ 0]=({ struct Cyc_Absyn_VarType_struct _temp3481; _temp3481.tag= Cyc_Absyn_VarType;
_temp3481.f1= tv; _temp3481;}); _temp3480;}); yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3472=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3472[ 0]=({ struct Cyc_Stmt_tok_struct _temp3473; _temp3473.tag= Cyc_Stmt_tok;
_temp3473.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Region_s_struct*
_temp3474=( struct Cyc_Absyn_Region_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Region_s_struct));
_temp3474[ 0]=({ struct Cyc_Absyn_Region_s_struct _temp3475; _temp3475.tag= Cyc_Absyn_Region_s;
_temp3475.f1= tv; _temp3475.f2= Cyc_Absyn_new_vardecl(({ struct _tuple1*
_temp3476=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3476->f1=(
void*) Cyc_Absyn_Loc_n; _temp3476->f2=({ struct _tagged_string* _temp3477=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string)); _temp3477[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3477;}); _temp3476;}),( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp3478=( struct Cyc_Absyn_RgnHandleType_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp3478[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp3479; _temp3479.tag= Cyc_Absyn_RgnHandleType; _temp3479.f1=( void*) t;
_temp3479;}); _temp3478;}), 0); _temp3475.f3= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3475;}); _temp3474;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3473;}); _temp3472;}); break;} case 221: _LL3467: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3486=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3486[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3487; _temp3487.tag= Cyc_Stmt_tok; _temp3487.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Cut_s_struct* _temp3488=( struct Cyc_Absyn_Cut_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct)); _temp3488[ 0]=({ struct Cyc_Absyn_Cut_s_struct
_temp3489; _temp3489.tag= Cyc_Absyn_Cut_s; _temp3489.f1= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3489;});
_temp3488;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3487;});
_temp3486;}); break; case 222: _LL3485: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3491=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3491[ 0]=({ struct Cyc_Stmt_tok_struct _temp3492; _temp3492.tag= Cyc_Stmt_tok;
_temp3492.f1= Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Splice_s_struct*
_temp3493=( struct Cyc_Absyn_Splice_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct));
_temp3493[ 0]=({ struct Cyc_Absyn_Splice_s_struct _temp3494; _temp3494.tag= Cyc_Absyn_Splice_s;
_temp3494.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3494;}); _temp3493;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3492;}); _temp3491;}); break; case 223: _LL3490: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3496=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3496[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3497; _temp3497.tag= Cyc_Stmt_tok; _temp3497.f1= Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Label_s_struct* _temp3498=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp3498[ 0]=({ struct
Cyc_Absyn_Label_s_struct _temp3499; _temp3499.tag= Cyc_Absyn_Label_s; _temp3499.f1=({
struct _tagged_string* _temp3500=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp3500[ 0]= Cyc_yyget_String_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3500;}); _temp3499.f2= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3499;});
_temp3498;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3497;});
_temp3496;}); break; case 224: _LL3495: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3502=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3502[ 0]=({ struct Cyc_Stmt_tok_struct _temp3503; _temp3503.tag= Cyc_Stmt_tok;
_temp3503.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3503;});
_temp3502;}); break; case 225: _LL3501: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3505=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3505[ 0]=({ struct Cyc_Stmt_tok_struct _temp3506; _temp3506.tag= Cyc_Stmt_tok;
_temp3506.f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3506;});
_temp3505;}); break; case 226: _LL3504: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3508=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3508[ 0]=({ struct Cyc_Stmt_tok_struct _temp3509; _temp3509.tag= Cyc_Stmt_tok;
_temp3509.f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3509;});
_temp3508;}); break; case 227: _LL3507: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 228: _LL3510:{ void* _temp3512= Cyc_yyget_BlockItem_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); struct Cyc_List_List*
_temp3520; struct Cyc_Absyn_Fndecl* _temp3522; struct Cyc_Absyn_Stmt* _temp3524;
_LL3514: if(*(( int*) _temp3512) == Cyc_Parse_TopDecls_bl){ _LL3521: _temp3520=(
struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*) _temp3512)->f1;
goto _LL3515;} else{ goto _LL3516;} _LL3516: if(*(( int*) _temp3512) == Cyc_Parse_FnDecl_bl){
_LL3523: _temp3522=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp3512)->f1; goto _LL3517;} else{ goto _LL3518;} _LL3518: if(*(( int*)
_temp3512) == Cyc_Parse_Stmt_bl){ _LL3525: _temp3524=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp3512)->f1; goto _LL3519;} else{ goto
_LL3513;} _LL3515: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3526=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3526[ 0]=({ struct Cyc_Stmt_tok_struct _temp3527; _temp3527.tag= Cyc_Stmt_tok;
_temp3527.f1= Cyc_Parse_flatten_declarations( _temp3520, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line))); _temp3527;}); _temp3526;}); goto _LL3513; _LL3517:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3528=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3528[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3529; _temp3529.tag= Cyc_Stmt_tok; _temp3529.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct* _temp3530=( struct
Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3530[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3531; _temp3531.tag= Cyc_Absyn_Fn_d;
_temp3531.f1= _temp3522; _temp3531;}); _temp3530;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)),
Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line))); _temp3529;});
_temp3528;}); goto _LL3513; _LL3519: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3532=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3532[ 0]=({ struct Cyc_Stmt_tok_struct _temp3533; _temp3533.tag= Cyc_Stmt_tok;
_temp3533.f1= _temp3524; _temp3533;}); _temp3532;}); goto _LL3513; _LL3513:;}
break; case 229: _LL3511:{ void* _temp3535= Cyc_yyget_BlockItem_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); struct Cyc_List_List* _temp3543;
struct Cyc_Absyn_Fndecl* _temp3545; struct Cyc_Absyn_Stmt* _temp3547; _LL3537:
if(*(( int*) _temp3535) == Cyc_Parse_TopDecls_bl){ _LL3544: _temp3543=( struct
Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*) _temp3535)->f1; goto
_LL3538;} else{ goto _LL3539;} _LL3539: if(*(( int*) _temp3535) == Cyc_Parse_FnDecl_bl){
_LL3546: _temp3545=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*)
_temp3535)->f1; goto _LL3540;} else{ goto _LL3541;} _LL3541: if(*(( int*)
_temp3535) == Cyc_Parse_Stmt_bl){ _LL3548: _temp3547=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp3535)->f1; goto _LL3542;} else{ goto
_LL3536;} _LL3538: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3549=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3549[ 0]=({ struct Cyc_Stmt_tok_struct _temp3550; _temp3550.tag= Cyc_Stmt_tok;
_temp3550.f1= Cyc_Parse_flatten_declarations( _temp3543, Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))); _temp3550;});
_temp3549;}); goto _LL3536; _LL3540: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3551=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3551[ 0]=({ struct Cyc_Stmt_tok_struct _temp3552; _temp3552.tag= Cyc_Stmt_tok;
_temp3552.f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Fn_d_struct*
_temp3553=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp3553[ 0]=({ struct Cyc_Absyn_Fn_d_struct _temp3554; _temp3554.tag= Cyc_Absyn_Fn_d;
_temp3554.f1= _temp3545; _temp3554;}); _temp3553;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3552;}); _temp3551;}); goto _LL3536; _LL3542: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3555=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3555[ 0]=({ struct Cyc_Stmt_tok_struct _temp3556; _temp3556.tag= Cyc_Stmt_tok;
_temp3556.f1= Cyc_Absyn_seq_stmt( _temp3547, Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3556;}); _temp3555;}); goto _LL3536; _LL3536:;} break; case 230: _LL3534:
yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3558=( struct Cyc_BlockItem_tok_struct*)
GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct)); _temp3558[ 0]=({ struct
Cyc_BlockItem_tok_struct _temp3559; _temp3559.tag= Cyc_BlockItem_tok; _temp3559.f1=(
void*)(( void*)({ struct Cyc_Parse_TopDecls_bl_struct* _temp3560=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp3560[ 0]=({
struct Cyc_Parse_TopDecls_bl_struct _temp3561; _temp3561.tag= Cyc_Parse_TopDecls_bl;
_temp3561.f1= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3561;}); _temp3560;})); _temp3559;}); _temp3558;}); break;
case 231: _LL3557: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3563=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3563[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3564; _temp3564.tag= Cyc_BlockItem_tok;
_temp3564.f1=( void*)(( void*)({ struct Cyc_Parse_Stmt_bl_struct* _temp3565=(
struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp3565[ 0]=({ struct Cyc_Parse_Stmt_bl_struct _temp3566; _temp3566.tag= Cyc_Parse_Stmt_bl;
_temp3566.f1= Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3566;}); _temp3565;})); _temp3564;}); _temp3563;}); break;
case 232: _LL3562: yyval=( void*)({ struct Cyc_BlockItem_tok_struct* _temp3568=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp3568[ 0]=({ struct Cyc_BlockItem_tok_struct _temp3569; _temp3569.tag= Cyc_BlockItem_tok;
_temp3569.f1=( void*)(( void*)({ struct Cyc_Parse_FnDecl_bl_struct* _temp3570=(
struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp3570[ 0]=({ struct Cyc_Parse_FnDecl_bl_struct _temp3571; _temp3571.tag= Cyc_Parse_FnDecl_bl;
_temp3571.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3571;}); _temp3570;})); _temp3569;}); _temp3568;}); break;
case 233: _LL3567: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3573=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3573[ 0]=({ struct Cyc_Stmt_tok_struct _temp3574; _temp3574.tag= Cyc_Stmt_tok;
_temp3574.f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3574;}); _temp3573;}); break; case 234: _LL3572: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3576=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3576[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3577; _temp3577.tag= Cyc_Stmt_tok; _temp3577.f1= Cyc_Absyn_ifthenelse_stmt(
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
_temp3577;}); _temp3576;}); break; case 235: _LL3575: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3579=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3579[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3580; _temp3580.tag= Cyc_Stmt_tok; _temp3580.f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3580;}); _temp3579;}); break; case 236: _LL3578: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3582=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3582[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3583; _temp3583.tag= Cyc_Stmt_tok; _temp3583.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)),
Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3583;}); _temp3582;}); break; case 237: _LL3581:
yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3585=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3585[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3586; _temp3586.tag= Cyc_SwitchClauseList_tok;
_temp3586.f1= 0; _temp3586;}); _temp3585;}); break; case 238: _LL3584: yyval=(
void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3588=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3588[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3589; _temp3589.tag= Cyc_SwitchClauseList_tok;
_temp3589.f1=({ struct Cyc_List_List* _temp3590=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3590->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3591=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3591->pattern= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp3591->pat_vars= 0; _temp3591->where_clause= 0; _temp3591->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3591->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3591;}); _temp3590->tl= 0; _temp3590;});
_temp3589;}); _temp3588;}); break; case 239: _LL3587: yyval=( void*)({ struct
Cyc_SwitchClauseList_tok_struct* _temp3593=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp3593[ 0]=({
struct Cyc_SwitchClauseList_tok_struct _temp3594; _temp3594.tag= Cyc_SwitchClauseList_tok;
_temp3594.f1=({ struct Cyc_List_List* _temp3595=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3595->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp3596=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp3596->pattern= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)); _temp3596->pat_vars= 0; _temp3596->where_clause= 0;
_temp3596->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3596->loc= Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line); _temp3596;});
_temp3595->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3595;}); _temp3594;}); _temp3593;}); break; case 240:
_LL3592: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct* _temp3598=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp3598[ 0]=({ struct Cyc_SwitchClauseList_tok_struct _temp3599; _temp3599.tag=
Cyc_SwitchClauseList_tok; _temp3599.f1=({ struct Cyc_List_List* _temp3600=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3600->hd=(
void*)({ struct Cyc_Absyn_Switch_clause* _temp3601=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3601->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3601->pat_vars=
0; _temp3601->where_clause= 0; _temp3601->body= Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3601->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line);
_temp3601;}); _temp3600->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3600;}); _temp3599;}); _temp3598;});
break; case 241: _LL3597: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3603=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3603[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3604; _temp3604.tag= Cyc_SwitchClauseList_tok; _temp3604.f1=({ struct Cyc_List_List*
_temp3605=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3605->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3606=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3606->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3606->pat_vars=
0; _temp3606->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 2)); _temp3606->body= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3606->loc= Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line);
_temp3606;}); _temp3605->tl= Cyc_yyget_SwitchClauseList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)); _temp3605;}); _temp3604;}); _temp3603;});
break; case 242: _LL3602: yyval=( void*)({ struct Cyc_SwitchClauseList_tok_struct*
_temp3608=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp3608[ 0]=({ struct Cyc_SwitchClauseList_tok_struct
_temp3609; _temp3609.tag= Cyc_SwitchClauseList_tok; _temp3609.f1=({ struct Cyc_List_List*
_temp3610=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3610->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp3611=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp3611->pattern= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); _temp3611->pat_vars=
0; _temp3611->where_clause=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3611->body= Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3611->loc=
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line); _temp3611;}); _temp3610->tl= Cyc_yyget_SwitchClauseList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3610;});
_temp3609;}); _temp3608;}); break; case 243: _LL3607: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3613=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3613[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3614; _temp3614.tag= Cyc_Stmt_tok; _temp3614.f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3614;}); _temp3613;}); break; case 244: _LL3612: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3616=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3616[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3617; _temp3617.tag= Cyc_Stmt_tok; _temp3617.f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3617;}); _temp3616;}); break; case 245: _LL3615: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3619=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3619[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3620; _temp3620.tag= Cyc_Stmt_tok; _temp3620.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3620;}); _temp3619;}); break; case 246: _LL3618: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3622=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3622[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3623; _temp3623.tag= Cyc_Stmt_tok; _temp3623.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3623;});
_temp3622;}); break; case 247: _LL3621: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3625=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3625[ 0]=({ struct Cyc_Stmt_tok_struct _temp3626; _temp3626.tag= Cyc_Stmt_tok;
_temp3626.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3626;}); _temp3625;}); break; case 248: _LL3624:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3628=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3628[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3629; _temp3629.tag= Cyc_Stmt_tok; _temp3629.f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
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
_temp3629;}); _temp3628;}); break; case 249: _LL3627: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3631=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3631[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3632; _temp3632.tag= Cyc_Stmt_tok; _temp3632.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)), Cyc_Absyn_true_exp(
0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3632;});
_temp3631;}); break; case 250: _LL3630: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3634=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3634[ 0]=({ struct Cyc_Stmt_tok_struct _temp3635; _temp3635.tag= Cyc_Stmt_tok;
_temp3635.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*(( void**(*)( struct
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
_temp3635;}); _temp3634;}); break; case 251: _LL3633: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3637=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3637[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3638; _temp3638.tag= Cyc_Stmt_tok; _temp3638.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
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
yylsp_offset)).last_line)); _temp3638;}); _temp3637;}); break; case 252: _LL3636:
yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3640=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3640[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3641; _temp3641.tag= Cyc_Stmt_tok; _temp3641.f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(*((
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
_temp3641;}); _temp3640;}); break; case 253: _LL3639: { struct Cyc_List_List*
_temp3643= Cyc_yyget_DeclList_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); goto _LL3644; _LL3644: { struct Cyc_Absyn_Stmt* _temp3645=
Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 5)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); goto _LL3646; _LL3646: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3647=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3647[ 0]=({ struct Cyc_Stmt_tok_struct _temp3648; _temp3648.tag= Cyc_Stmt_tok;
_temp3648.f1= Cyc_Parse_flatten_declarations( _temp3643, _temp3645); _temp3648;});
_temp3647;}); break;}} case 254: _LL3642: { struct Cyc_List_List* _temp3650= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); goto
_LL3651; _LL3651: { struct Cyc_Absyn_Stmt* _temp3652= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL3653;
_LL3653: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3654=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3654[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3655; _temp3655.tag= Cyc_Stmt_tok; _temp3655.f1= Cyc_Parse_flatten_declarations(
_temp3650, _temp3652); _temp3655;}); _temp3654;}); break;}} case 255: _LL3649: {
struct Cyc_List_List* _temp3657= Cyc_yyget_DeclList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); goto _LL3658; _LL3658: { struct Cyc_Absyn_Stmt*
_temp3659= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0),
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Stmt_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
goto _LL3660; _LL3660: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3661=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3661[ 0]=({ struct Cyc_Stmt_tok_struct _temp3662; _temp3662.tag= Cyc_Stmt_tok;
_temp3662.f1= Cyc_Parse_flatten_declarations( _temp3657, _temp3659); _temp3662;});
_temp3661;}); break;}} case 256: _LL3656: { struct Cyc_List_List* _temp3664= Cyc_yyget_DeclList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 5)); goto
_LL3665; _LL3665: { struct Cyc_Absyn_Stmt* _temp3666= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
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
goto _LL3667; _LL3667: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3668=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3668[ 0]=({ struct Cyc_Stmt_tok_struct _temp3669; _temp3669.tag= Cyc_Stmt_tok;
_temp3669.f1= Cyc_Parse_flatten_declarations( _temp3664, _temp3666); _temp3669;});
_temp3668;}); break;}} case 257: _LL3663: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3671=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3671[ 0]=({ struct Cyc_Stmt_tok_struct _temp3672; _temp3672.tag= Cyc_Stmt_tok;
_temp3672.f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp3673=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp3673[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3673;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3672;});
_temp3671;}); break; case 258: _LL3670: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3675=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3675[ 0]=({ struct Cyc_Stmt_tok_struct _temp3676; _temp3676.tag= Cyc_Stmt_tok;
_temp3676.f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs((*(( struct
Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3676;}); _temp3675;}); break; case 259: _LL3674: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3678=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3678[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3679; _temp3679.tag= Cyc_Stmt_tok; _temp3679.f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3679;}); _temp3678;}); break; case 260: _LL3677: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3681=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3681[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3682; _temp3682.tag= Cyc_Stmt_tok; _temp3682.f1= Cyc_Absyn_return_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).last_line)); _temp3682;}); _temp3681;}); break; case 261:
_LL3680: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3684=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3684[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3685; _temp3685.tag= Cyc_Stmt_tok; _temp3685.f1= Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)); _temp3685;});
_temp3684;}); break; case 262: _LL3683: yyval=( void*)({ struct Cyc_Stmt_tok_struct*
_temp3687=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp3687[ 0]=({ struct Cyc_Stmt_tok_struct _temp3688; _temp3688.tag= Cyc_Stmt_tok;
_temp3688.f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line));
_temp3688;}); _temp3687;}); break; case 263: _LL3686: yyval=( void*)({ struct
Cyc_Stmt_tok_struct* _temp3690=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct)); _temp3690[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3691; _temp3691.tag= Cyc_Stmt_tok; _temp3691.f1= Cyc_Absyn_fallthru_stmt( 0,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).last_line)); _temp3691;}); _temp3690;}); break; case 264:
_LL3689: yyval=( void*)({ struct Cyc_Stmt_tok_struct* _temp3693=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp3693[ 0]=({ struct Cyc_Stmt_tok_struct
_temp3694; _temp3694.tag= Cyc_Stmt_tok; _temp3694.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 4)).last_line)); _temp3694;}); _temp3693;}); break; case 265:
_LL3692: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3696=( struct Cyc_Pattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pattern_tok_struct)); _temp3696[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3697; _temp3697.tag= Cyc_Pattern_tok; _temp3697.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3697;});
_temp3696;}); break; case 266: _LL3695: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1); break; case 267: _LL3698: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3700=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3700[ 0]=({ struct Cyc_Pattern_tok_struct _temp3701; _temp3701.tag= Cyc_Pattern_tok;
_temp3701.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp3702=( struct Cyc_Absyn_Int_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp3702[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp3703; _temp3703.tag= Cyc_Absyn_Int_p;
_temp3703.f1=( void*)(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f1; _temp3703.f2=(* Cyc_yyget_Int_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset))).f2; _temp3703;}); _temp3702;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3701;}); _temp3700;}); break; case 268: _LL3699: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3705=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3705[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3706; _temp3706.tag= Cyc_Pattern_tok; _temp3706.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Int_p_struct* _temp3707=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp3707[ 0]=({ struct Cyc_Absyn_Int_p_struct
_temp3708; _temp3708.tag= Cyc_Absyn_Int_p; _temp3708.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp3708.f2= -(* Cyc_yyget_Int_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset))).f2; _temp3708;}); _temp3707;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3706;}); _temp3705;}); break; case 269: _LL3704: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3710=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3710[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3711; _temp3711.tag= Cyc_Pattern_tok; _temp3711.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Float_p_struct* _temp3712=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp3712[ 0]=({ struct
Cyc_Absyn_Float_p_struct _temp3713; _temp3713.tag= Cyc_Absyn_Float_p; _temp3713.f1=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp3713;}); _temp3712;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3711;});
_temp3710;}); break; case 270: _LL3709: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3715=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3715[ 0]=({ struct Cyc_Pattern_tok_struct _temp3716; _temp3716.tag= Cyc_Pattern_tok;
_temp3716.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Char_p_struct*
_temp3717=( struct Cyc_Absyn_Char_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_p_struct));
_temp3717[ 0]=({ struct Cyc_Absyn_Char_p_struct _temp3718; _temp3718.tag= Cyc_Absyn_Char_p;
_temp3718.f1= Cyc_yyget_Char_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3718;}); _temp3717;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3716;}); _temp3715;}); break; case 271: _LL3714: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3720=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3720[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3721; _temp3721.tag= Cyc_Pattern_tok; _temp3721.f1= Cyc_Absyn_new_pat((
void*) Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3721;});
_temp3720;}); break; case 272: _LL3719: yyval=( void*)({ struct Cyc_Pattern_tok_struct*
_temp3723=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3723[ 0]=({ struct Cyc_Pattern_tok_struct _temp3724; _temp3724.tag= Cyc_Pattern_tok;
_temp3724.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownId_p_struct*
_temp3725=( struct Cyc_Absyn_UnknownId_p_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct));
_temp3725[ 0]=({ struct Cyc_Absyn_UnknownId_p_struct _temp3726; _temp3726.tag=
Cyc_Absyn_UnknownId_p; _temp3726.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp3726;}); _temp3725;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3724;}); _temp3723;}); break; case 273: _LL3722: { struct Cyc_List_List*
_temp3728=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3729; _LL3729: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3730=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3730[ 0]=({ struct Cyc_Pattern_tok_struct _temp3731; _temp3731.tag= Cyc_Pattern_tok;
_temp3731.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownCall_p_struct*
_temp3732=( struct Cyc_Absyn_UnknownCall_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownCall_p_struct)); _temp3732[ 0]=({ struct Cyc_Absyn_UnknownCall_p_struct
_temp3733; _temp3733.tag= Cyc_Absyn_UnknownCall_p; _temp3733.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3733.f2=
_temp3728; _temp3733.f3= Cyc_yyget_PatternList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3733;}); _temp3732;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3731;}); _temp3730;}); break;} case 274: _LL3727: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3735=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3735[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3736; _temp3736.tag= Cyc_Pattern_tok; _temp3736.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Tuple_p_struct* _temp3737=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp3737[ 0]=({ struct
Cyc_Absyn_Tuple_p_struct _temp3738; _temp3738.tag= Cyc_Absyn_Tuple_p; _temp3738.f1=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1));
_temp3738;}); _temp3737;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3736;});
_temp3735;}); break; case 275: _LL3734: { struct Cyc_List_List* _temp3740=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))); goto
_LL3741; _LL3741: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3742=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3742[ 0]=({ struct Cyc_Pattern_tok_struct _temp3743; _temp3743.tag= Cyc_Pattern_tok;
_temp3743.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3744=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3744[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3745; _temp3745.tag= Cyc_Absyn_UnknownFields_p; _temp3745.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)); _temp3745.f2=
_temp3740; _temp3745.f3= 0; _temp3745;}); _temp3744;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3743;}); _temp3742;}); break;} case 276: _LL3739: { struct Cyc_List_List*
_temp3747=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).last_line), Cyc_yyget_TypeList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3))); goto
_LL3748; _LL3748: yyval=( void*)({ struct Cyc_Pattern_tok_struct* _temp3749=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp3749[ 0]=({ struct Cyc_Pattern_tok_struct _temp3750; _temp3750.tag= Cyc_Pattern_tok;
_temp3750.f1= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_UnknownFields_p_struct*
_temp3751=( struct Cyc_Absyn_UnknownFields_p_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnknownFields_p_struct)); _temp3751[ 0]=({ struct Cyc_Absyn_UnknownFields_p_struct
_temp3752; _temp3752.tag= Cyc_Absyn_UnknownFields_p; _temp3752.f1= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 4)); _temp3752.f2=
_temp3747; _temp3752.f3= Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp3752;}); _temp3751;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3750;}); _temp3749;}); break;} case 277: _LL3746: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3754=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3754[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3755; _temp3755.tag= Cyc_Pattern_tok; _temp3755.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Pointer_p_struct* _temp3756=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp3756[ 0]=({ struct
Cyc_Absyn_Pointer_p_struct _temp3757; _temp3757.tag= Cyc_Absyn_Pointer_p;
_temp3757.f1= Cyc_yyget_Pattern_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp3757;}); _temp3756;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3755;}); _temp3754;}); break; case 278: _LL3753: yyval=( void*)({ struct
Cyc_Pattern_tok_struct* _temp3759=( struct Cyc_Pattern_tok_struct*) GC_malloc(
sizeof( struct Cyc_Pattern_tok_struct)); _temp3759[ 0]=({ struct Cyc_Pattern_tok_struct
_temp3760; _temp3760.tag= Cyc_Pattern_tok; _temp3760.f1= Cyc_Absyn_new_pat((
void*)({ struct Cyc_Absyn_Reference_p_struct* _temp3761=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp3761[ 0]=({
struct Cyc_Absyn_Reference_p_struct _temp3762; _temp3762.tag= Cyc_Absyn_Reference_p;
_temp3762.f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp3763=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3763->f1=( void*) Cyc_Absyn_Loc_n;
_temp3763->f2=({ struct _tagged_string* _temp3764=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3764[ 0]= Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3764;});
_temp3763;}),( void*) Cyc_Absyn_VoidType, 0); _temp3762;}); _temp3761;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3760;}); _temp3759;}); break; case 279: _LL3758: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3766=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3766[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3767; _temp3767.tag= Cyc_PatternList_tok;
_temp3767.f1= 0; _temp3767;}); _temp3766;}); break; case 280: _LL3765: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3769=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3769[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3770; _temp3770.tag= Cyc_PatternList_tok;
_temp3770.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3770;}); _temp3769;}); break; case 281: _LL3768: yyval=( void*)({ struct
Cyc_PatternList_tok_struct* _temp3772=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3772[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3773; _temp3773.tag= Cyc_PatternList_tok;
_temp3773.f1=({ struct Cyc_List_List* _temp3774=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3774->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3774->tl=
0; _temp3774;}); _temp3773;}); _temp3772;}); break; case 282: _LL3771: yyval=(
void*)({ struct Cyc_PatternList_tok_struct* _temp3776=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct)); _temp3776[ 0]=({ struct
Cyc_PatternList_tok_struct _temp3777; _temp3777.tag= Cyc_PatternList_tok;
_temp3777.f1=({ struct Cyc_List_List* _temp3778=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3778->hd=( void*) Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3778->tl=
Cyc_yyget_PatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3778;}); _temp3777;}); _temp3776;}); break; case 283: _LL3775: yyval=( void*)({
struct Cyc_FieldPattern_tok_struct* _temp3780=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3780[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3781; _temp3781.tag= Cyc_FieldPattern_tok;
_temp3781.f1=({ struct _tuple14* _temp3782=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3782->f1= 0; _temp3782->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3782;});
_temp3781;}); _temp3780;}); break; case 284: _LL3779: yyval=( void*)({ struct
Cyc_FieldPattern_tok_struct* _temp3784=( struct Cyc_FieldPattern_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct)); _temp3784[ 0]=({ struct
Cyc_FieldPattern_tok_struct _temp3785; _temp3785.tag= Cyc_FieldPattern_tok;
_temp3785.f1=({ struct _tuple14* _temp3786=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3786->f1= Cyc_yyget_DesignatorList_tok(*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1)); _temp3786->f2= Cyc_yyget_Pattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3786;});
_temp3785;}); _temp3784;}); break; case 285: _LL3783: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3788=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3788[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3789; _temp3789.tag= Cyc_FieldPatternList_tok;
_temp3789.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp3789;}); _temp3788;}); break; case 286: _LL3787: yyval=( void*)({ struct
Cyc_FieldPatternList_tok_struct* _temp3791=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3791[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3792; _temp3792.tag= Cyc_FieldPatternList_tok;
_temp3792.f1=({ struct Cyc_List_List* _temp3793=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3793->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3793->tl=
0; _temp3793;}); _temp3792;}); _temp3791;}); break; case 287: _LL3790: yyval=(
void*)({ struct Cyc_FieldPatternList_tok_struct* _temp3795=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp3795[ 0]=({
struct Cyc_FieldPatternList_tok_struct _temp3796; _temp3796.tag= Cyc_FieldPatternList_tok;
_temp3796.f1=({ struct Cyc_List_List* _temp3797=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3797->hd=( void*) Cyc_yyget_FieldPattern_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp3797->tl=
Cyc_yyget_FieldPatternList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp3797;}); _temp3796;}); _temp3795;}); break; case 288: _LL3794: yyval=*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; case 289:
_LL3798: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3800=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3800[ 0]=({ struct Cyc_Exp_tok_struct
_temp3801; _temp3801.tag= Cyc_Exp_tok; _temp3801.f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3801;}); _temp3800;}); break; case 290: _LL3799: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 291: _LL3802: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3804=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3804[ 0]=({ struct Cyc_Exp_tok_struct
_temp3805; _temp3805.tag= Cyc_Exp_tok; _temp3805.f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(*((
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
_temp3805;}); _temp3804;}); break; case 292: _LL3803: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3807=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3807[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3808; _temp3808.tag= Cyc_Primopopt_tok; _temp3808.f1=
0; _temp3808;}); _temp3807;}); break; case 293: _LL3806: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3810=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3810[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3811; _temp3811.tag= Cyc_Primopopt_tok; _temp3811.f1=({
struct Cyc_Core_Opt* _temp3812=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3812->v=( void*)(( void*) Cyc_Absyn_Times); _temp3812;});
_temp3811;}); _temp3810;}); break; case 294: _LL3809: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3814=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3814[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3815; _temp3815.tag= Cyc_Primopopt_tok; _temp3815.f1=({
struct Cyc_Core_Opt* _temp3816=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3816->v=( void*)(( void*) Cyc_Absyn_Div); _temp3816;});
_temp3815;}); _temp3814;}); break; case 295: _LL3813: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3818=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3818[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3819; _temp3819.tag= Cyc_Primopopt_tok; _temp3819.f1=({
struct Cyc_Core_Opt* _temp3820=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3820->v=( void*)(( void*) Cyc_Absyn_Mod); _temp3820;});
_temp3819;}); _temp3818;}); break; case 296: _LL3817: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3822=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3822[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3823; _temp3823.tag= Cyc_Primopopt_tok; _temp3823.f1=({
struct Cyc_Core_Opt* _temp3824=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3824->v=( void*)(( void*) Cyc_Absyn_Plus); _temp3824;});
_temp3823;}); _temp3822;}); break; case 297: _LL3821: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3826=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3826[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3827; _temp3827.tag= Cyc_Primopopt_tok; _temp3827.f1=({
struct Cyc_Core_Opt* _temp3828=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3828->v=( void*)(( void*) Cyc_Absyn_Minus); _temp3828;});
_temp3827;}); _temp3826;}); break; case 298: _LL3825: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3830=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3830[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3831; _temp3831.tag= Cyc_Primopopt_tok; _temp3831.f1=({
struct Cyc_Core_Opt* _temp3832=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3832->v=( void*)(( void*) Cyc_Absyn_Bitlshift); _temp3832;});
_temp3831;}); _temp3830;}); break; case 299: _LL3829: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3834=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3834[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3835; _temp3835.tag= Cyc_Primopopt_tok; _temp3835.f1=({
struct Cyc_Core_Opt* _temp3836=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3836->v=( void*)(( void*) Cyc_Absyn_Bitlrshift); _temp3836;});
_temp3835;}); _temp3834;}); break; case 300: _LL3833: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3838=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3838[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3839; _temp3839.tag= Cyc_Primopopt_tok; _temp3839.f1=({
struct Cyc_Core_Opt* _temp3840=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3840->v=( void*)(( void*) Cyc_Absyn_Bitand); _temp3840;});
_temp3839;}); _temp3838;}); break; case 301: _LL3837: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3842=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3842[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3843; _temp3843.tag= Cyc_Primopopt_tok; _temp3843.f1=({
struct Cyc_Core_Opt* _temp3844=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3844->v=( void*)(( void*) Cyc_Absyn_Bitxor); _temp3844;});
_temp3843;}); _temp3842;}); break; case 302: _LL3841: yyval=( void*)({ struct
Cyc_Primopopt_tok_struct* _temp3846=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct)); _temp3846[ 0]=({ struct
Cyc_Primopopt_tok_struct _temp3847; _temp3847.tag= Cyc_Primopopt_tok; _temp3847.f1=({
struct Cyc_Core_Opt* _temp3848=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3848->v=( void*)(( void*) Cyc_Absyn_Bitor); _temp3848;});
_temp3847;}); _temp3846;}); break; case 303: _LL3845: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 304: _LL3849: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3851=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3851[ 0]=({ struct Cyc_Exp_tok_struct
_temp3852; _temp3852.tag= Cyc_Exp_tok; _temp3852.f1= Cyc_Absyn_conditional_exp(
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
_temp3852;}); _temp3851;}); break; case 305: _LL3850: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3854=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3854[ 0]=({ struct Cyc_Exp_tok_struct
_temp3855; _temp3855.tag= Cyc_Exp_tok; _temp3855.f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3855;}); _temp3854;}); break; case 306: _LL3853: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3857=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3857[ 0]=({ struct Cyc_Exp_tok_struct
_temp3858; _temp3858.tag= Cyc_Exp_tok; _temp3858.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3858;}); _temp3857;}); break; case 307: _LL3856: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3860=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3860[ 0]=({ struct Cyc_Exp_tok_struct
_temp3861; _temp3861.tag= Cyc_Exp_tok; _temp3861.f1= Cyc_Absyn_New_exp( 0, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset + 1)).last_line));
_temp3861;}); _temp3860;}); break; case 308: _LL3859: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3863=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3863[ 0]=({ struct Cyc_Exp_tok_struct
_temp3864; _temp3864.tag= Cyc_Exp_tok; _temp3864.f1= Cyc_Absyn_New_exp(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3864;});
_temp3863;}); break; case 309: _LL3862: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3866=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3866[ 0]=({ struct Cyc_Exp_tok_struct _temp3867; _temp3867.tag= Cyc_Exp_tok;
_temp3867.f1= Cyc_Absyn_New_exp(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3867;}); _temp3866;}); break; case 310: _LL3865: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 311: _LL3868: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 312: _LL3869: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3871=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3871[ 0]=({ struct Cyc_Exp_tok_struct
_temp3872; _temp3872.tag= Cyc_Exp_tok; _temp3872.f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3872;}); _temp3871;}); break; case 313: _LL3870: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 314: _LL3873: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3875=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3875[ 0]=({ struct Cyc_Exp_tok_struct
_temp3876; _temp3876.tag= Cyc_Exp_tok; _temp3876.f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3876;}); _temp3875;}); break; case 315: _LL3874: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 316: _LL3877: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3879=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3879[ 0]=({ struct Cyc_Exp_tok_struct
_temp3880; _temp3880.tag= Cyc_Exp_tok; _temp3880.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3880;});
_temp3879;}); break; case 317: _LL3878: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 318: _LL3881: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3883=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3883[ 0]=({ struct Cyc_Exp_tok_struct _temp3884; _temp3884.tag= Cyc_Exp_tok;
_temp3884.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3884;}); _temp3883;}); break; case 319: _LL3882: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 320: _LL3885: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3887=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3887[ 0]=({ struct Cyc_Exp_tok_struct
_temp3888; _temp3888.tag= Cyc_Exp_tok; _temp3888.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3888;});
_temp3887;}); break; case 321: _LL3886: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 322: _LL3889: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3891=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3891[ 0]=({ struct Cyc_Exp_tok_struct _temp3892; _temp3892.tag= Cyc_Exp_tok;
_temp3892.f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3892;});
_temp3891;}); break; case 323: _LL3890: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3894=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3894[ 0]=({ struct Cyc_Exp_tok_struct _temp3895; _temp3895.tag= Cyc_Exp_tok;
_temp3895.f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3895;});
_temp3894;}); break; case 324: _LL3893: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 325: _LL3896: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3898=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3898[ 0]=({ struct Cyc_Exp_tok_struct _temp3899; _temp3899.tag= Cyc_Exp_tok;
_temp3899.f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3899;});
_temp3898;}); break; case 326: _LL3897: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3901=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3901[ 0]=({ struct Cyc_Exp_tok_struct _temp3902; _temp3902.tag= Cyc_Exp_tok;
_temp3902.f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3902;});
_temp3901;}); break; case 327: _LL3900: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3904=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3904[ 0]=({ struct Cyc_Exp_tok_struct _temp3905; _temp3905.tag= Cyc_Exp_tok;
_temp3905.f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3905;});
_temp3904;}); break; case 328: _LL3903: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3907=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3907[ 0]=({ struct Cyc_Exp_tok_struct _temp3908; _temp3908.tag= Cyc_Exp_tok;
_temp3908.f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3908;});
_temp3907;}); break; case 329: _LL3906: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 330: _LL3909: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3911=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3911[ 0]=({ struct Cyc_Exp_tok_struct _temp3912; _temp3912.tag= Cyc_Exp_tok;
_temp3912.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3912;}); _temp3911;}); break; case 331: _LL3910: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3914=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3914[ 0]=({ struct Cyc_Exp_tok_struct
_temp3915; _temp3915.tag= Cyc_Exp_tok; _temp3915.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Bitlrshift, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3915;});
_temp3914;}); break; case 332: _LL3913: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 333: _LL3916: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3918=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3918[ 0]=({ struct Cyc_Exp_tok_struct _temp3919; _temp3919.tag= Cyc_Exp_tok;
_temp3919.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3919;}); _temp3918;}); break; case 334: _LL3917: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3921=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3921[ 0]=({ struct Cyc_Exp_tok_struct
_temp3922; _temp3922.tag= Cyc_Exp_tok; _temp3922.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Minus, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3922;});
_temp3921;}); break; case 335: _LL3920: yyval=*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset); break; case 336: _LL3923: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3925=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3925[ 0]=({ struct Cyc_Exp_tok_struct _temp3926; _temp3926.tag= Cyc_Exp_tok;
_temp3926.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Times, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3926;}); _temp3925;}); break; case 337: _LL3924: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3928=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3928[ 0]=({ struct Cyc_Exp_tok_struct
_temp3929; _temp3929.tag= Cyc_Exp_tok; _temp3929.f1= Cyc_Absyn_prim2_exp(( void*)
Cyc_Absyn_Div, Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3929;});
_temp3928;}); break; case 338: _LL3927: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3931=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3931[ 0]=({ struct Cyc_Exp_tok_struct _temp3932; _temp3932.tag= Cyc_Exp_tok;
_temp3932.f1= Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3932;}); _temp3931;}); break; case 339: _LL3930: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 340: _LL3933: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3935=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3935[ 0]=({ struct Cyc_Exp_tok_struct
_temp3936; _temp3936.tag= Cyc_Exp_tok; _temp3936.f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f3, Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3936;}); _temp3935;}); break; case 341: _LL3934: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 342: _LL3937: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3939=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3939[ 0]=({ struct Cyc_Exp_tok_struct
_temp3940; _temp3940.tag= Cyc_Exp_tok; _temp3940.f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3940;}); _temp3939;}); break; case 343: _LL3938: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3942=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3942[ 0]=({ struct Cyc_Exp_tok_struct
_temp3943; _temp3943.tag= Cyc_Exp_tok; _temp3943.f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3943;}); _temp3942;}); break; case 344: _LL3941: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3945=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3945[ 0]=({ struct Cyc_Exp_tok_struct
_temp3946; _temp3946.tag= Cyc_Exp_tok; _temp3946.f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3946;}); _temp3945;}); break; case 345: _LL3944: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3948=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3948[ 0]=({ struct Cyc_Exp_tok_struct
_temp3949; _temp3949.tag= Cyc_Exp_tok; _temp3949.f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3949;}); _temp3948;}); break; case 346: _LL3947: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 347: _LL3950: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp3952=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp3952[ 0]=({ struct Cyc_Exp_tok_struct
_temp3953; _temp3953.tag= Cyc_Exp_tok; _temp3953.f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3953;}); _temp3952;}); break; case 348: _LL3951: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3955=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3955[ 0]=({ struct Cyc_Exp_tok_struct
_temp3956; _temp3956.tag= Cyc_Exp_tok; _temp3956.f1= Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))).f3,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp3956;}); _temp3955;}); break; case 349: _LL3954:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp3958=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp3958[ 0]=({ struct Cyc_Exp_tok_struct
_temp3959; _temp3959.tag= Cyc_Exp_tok; _temp3959.f1= Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3959;}); _temp3958;}); break; case 350: _LL3957: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3961=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3961[ 0]=({ struct Cyc_Exp_tok_struct
_temp3962; _temp3962.tag= Cyc_Exp_tok; _temp3962.f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3962;}); _temp3961;}); break; case 351: _LL3960: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp3964=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp3964[ 0]=({ struct Cyc_Exp_tok_struct
_temp3965; _temp3965.tag= Cyc_Exp_tok; _temp3965.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Malloc_e_struct* _temp3966=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp3966[ 0]=({ struct
Cyc_Absyn_Malloc_e_struct _temp3967; _temp3967.tag= Cyc_Absyn_Malloc_e;
_temp3967.f1= 0; _temp3967.f2=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).last_line));
_temp3967;}); _temp3966;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp3965;});
_temp3964;}); break; case 352: _LL3963: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp3969=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp3969[ 0]=({ struct Cyc_Exp_tok_struct _temp3970; _temp3970.tag= Cyc_Exp_tok;
_temp3970.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Malloc_e_struct*
_temp3971=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp3971[ 0]=({ struct Cyc_Absyn_Malloc_e_struct _temp3972; _temp3972.tag= Cyc_Absyn_Malloc_e;
_temp3972.f1=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 6)); _temp3972.f2=( void*) Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2))).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).last_line)); _temp3972;}); _temp3971;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 8)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp3970;}); _temp3969;}); break; case 353: _LL3968: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3974=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3974[ 0]=({ struct Cyc_Primop_tok_struct
_temp3975; _temp3975.tag= Cyc_Primop_tok; _temp3975.f1=( void*)(( void*) Cyc_Absyn_Printf);
_temp3975;}); _temp3974;}); break; case 354: _LL3973: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3977=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3977[ 0]=({ struct Cyc_Primop_tok_struct
_temp3978; _temp3978.tag= Cyc_Primop_tok; _temp3978.f1=( void*)(( void*) Cyc_Absyn_Fprintf);
_temp3978;}); _temp3977;}); break; case 355: _LL3976: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3980=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3980[ 0]=({ struct Cyc_Primop_tok_struct
_temp3981; _temp3981.tag= Cyc_Primop_tok; _temp3981.f1=( void*)(( void*) Cyc_Absyn_Xprintf);
_temp3981;}); _temp3980;}); break; case 356: _LL3979: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3983=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3983[ 0]=({ struct Cyc_Primop_tok_struct
_temp3984; _temp3984.tag= Cyc_Primop_tok; _temp3984.f1=( void*)(( void*) Cyc_Absyn_Scanf);
_temp3984;}); _temp3983;}); break; case 357: _LL3982: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3986=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3986[ 0]=({ struct Cyc_Primop_tok_struct
_temp3987; _temp3987.tag= Cyc_Primop_tok; _temp3987.f1=( void*)(( void*) Cyc_Absyn_Fscanf);
_temp3987;}); _temp3986;}); break; case 358: _LL3985: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3989=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3989[ 0]=({ struct Cyc_Primop_tok_struct
_temp3990; _temp3990.tag= Cyc_Primop_tok; _temp3990.f1=( void*)(( void*) Cyc_Absyn_Sscanf);
_temp3990;}); _temp3989;}); break; case 359: _LL3988: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3992=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3992[ 0]=({ struct Cyc_Primop_tok_struct
_temp3993; _temp3993.tag= Cyc_Primop_tok; _temp3993.f1=( void*)(( void*) Cyc_Absyn_Bitnot);
_temp3993;}); _temp3992;}); break; case 360: _LL3991: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3995=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3995[ 0]=({ struct Cyc_Primop_tok_struct
_temp3996; _temp3996.tag= Cyc_Primop_tok; _temp3996.f1=( void*)(( void*) Cyc_Absyn_Not);
_temp3996;}); _temp3995;}); break; case 361: _LL3994: yyval=( void*)({ struct
Cyc_Primop_tok_struct* _temp3998=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp3998[ 0]=({ struct Cyc_Primop_tok_struct
_temp3999; _temp3999.tag= Cyc_Primop_tok; _temp3999.f1=( void*)(( void*) Cyc_Absyn_Minus);
_temp3999;}); _temp3998;}); break; case 362: _LL3997: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 363: _LL4000: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4002=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4002[ 0]=({ struct Cyc_Exp_tok_struct
_temp4003; _temp4003.tag= Cyc_Exp_tok; _temp4003.f1= Cyc_Absyn_subscript_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4003;}); _temp4002;}); break; case 364: _LL4001: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4005=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4005[ 0]=({ struct Cyc_Exp_tok_struct
_temp4006; _temp4006.tag= Cyc_Exp_tok; _temp4006.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)), 0, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4006;}); _temp4005;}); break; case 365: _LL4004: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4008=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4008[ 0]=({ struct Cyc_Exp_tok_struct
_temp4009; _temp4009.tag= Cyc_Exp_tok; _temp4009.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 3)), Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4009;}); _temp4008;}); break; case 366: _LL4007: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4011=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4011[ 0]=({ struct Cyc_Exp_tok_struct
_temp4012; _temp4012.tag= Cyc_Exp_tok; _temp4012.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4013=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4013[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4013;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4012;}); _temp4011;}); break; case 367: _LL4010: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4015=(* q).f1; struct Cyc_List_List* _temp4025; struct Cyc_List_List*
_temp4027; _LL4017: if( _temp4015 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4018;}
else{ goto _LL4019;} _LL4019: if(( unsigned int) _temp4015 > 1u?*(( int*)
_temp4015) == Cyc_Absyn_Rel_n: 0){ _LL4026: _temp4025=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp4015)->f1; if( _temp4025 == 0){ goto
_LL4020;} else{ goto _LL4021;}} else{ goto _LL4021;} _LL4021: if(( unsigned int)
_temp4015 > 1u?*(( int*) _temp4015) == Cyc_Absyn_Abs_n: 0){ _LL4028: _temp4027=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp4015)->f1; if(
_temp4027 == 0){ goto _LL4022;} else{ goto _LL4023;}} else{ goto _LL4023;}
_LL4023: goto _LL4024; _LL4018: goto _LL4016; _LL4020: goto _LL4016; _LL4022:
goto _LL4016; _LL4024: Cyc_Parse_err(( struct _tagged_string)({ char* _temp4029=(
char*)"struct field name is qualified"; struct _tagged_string _temp4030;
_temp4030.curr= _temp4029; _temp4030.base= _temp4029; _temp4030.last_plus_one=
_temp4029 + 31; _temp4030;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL4016;
_LL4016:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4031=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4031[ 0]=({ struct Cyc_Exp_tok_struct
_temp4032; _temp4032.tag= Cyc_Exp_tok; _temp4032.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(* q).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4032;}); _temp4031;}); break;} case 368:
_LL4014: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4034=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4034[ 0]=({ struct Cyc_Exp_tok_struct
_temp4035; _temp4035.tag= Cyc_Exp_tok; _temp4035.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),({ struct
_tagged_string* _temp4036=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4036[ 0]= Cyc_yyget_String_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset)); _temp4036;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 2)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4035;}); _temp4034;}); break; case 369: _LL4033: { struct _tuple1* q= Cyc_yyget_QualId_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));{ void*
_temp4038=(* q).f1; struct Cyc_List_List* _temp4048; struct Cyc_List_List*
_temp4050; _LL4040: if( _temp4038 ==( void*) Cyc_Absyn_Loc_n){ goto _LL4041;}
else{ goto _LL4042;} _LL4042: if(( unsigned int) _temp4038 > 1u?*(( int*)
_temp4038) == Cyc_Absyn_Rel_n: 0){ _LL4049: _temp4048=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp4038)->f1; if( _temp4048 == 0){ goto
_LL4043;} else{ goto _LL4044;}} else{ goto _LL4044;} _LL4044: if(( unsigned int)
_temp4038 > 1u?*(( int*) _temp4038) == Cyc_Absyn_Abs_n: 0){ _LL4051: _temp4050=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp4038)->f1; if(
_temp4050 == 0){ goto _LL4045;} else{ goto _LL4046;}} else{ goto _LL4046;}
_LL4046: goto _LL4047; _LL4041: goto _LL4039; _LL4043: goto _LL4039; _LL4045:
goto _LL4039; _LL4047: Cyc_Parse_err(( struct _tagged_string)({ char* _temp4052=(
char*)"struct field is qualified with module name"; struct _tagged_string
_temp4053; _temp4053.curr= _temp4052; _temp4053.base= _temp4052; _temp4053.last_plus_one=
_temp4052 + 43; _temp4053;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); goto _LL4039;
_LL4039:;} yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4054=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4054[ 0]=({ struct Cyc_Exp_tok_struct
_temp4055; _temp4055.tag= Cyc_Exp_tok; _temp4055.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2)),(* q).f2,
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 2)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4055;}); _temp4054;}); break;} case 370:
_LL4037: yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4057=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4057[ 0]=({ struct Cyc_Exp_tok_struct
_temp4058; _temp4058.tag= Cyc_Exp_tok; _temp4058.f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4058;}); _temp4057;}); break; case 371: _LL4056: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4060=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4060[ 0]=({ struct Cyc_Exp_tok_struct
_temp4061; _temp4061.tag= Cyc_Exp_tok; _temp4061.f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4061;}); _temp4060;}); break; case 372: _LL4059: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4063=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4063[ 0]=({ struct Cyc_Exp_tok_struct
_temp4064; _temp4064.tag= Cyc_Exp_tok; _temp4064.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4065=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4065[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4066; _temp4066.tag= Cyc_Absyn_CompoundLit_e;
_temp4066.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 4)); _temp4066.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1))); _temp4066;}); _temp4065;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 5)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4064;}); _temp4063;}); break; case 373: _LL4062: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4068=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4068[ 0]=({ struct Cyc_Exp_tok_struct
_temp4069; _temp4069.tag= Cyc_Exp_tok; _temp4069.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_CompoundLit_e_struct* _temp4070=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp4070[ 0]=({
struct Cyc_Absyn_CompoundLit_e_struct _temp4071; _temp4071.tag= Cyc_Absyn_CompoundLit_e;
_temp4071.f1= Cyc_yyget_ParamDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 5)); _temp4071.f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2))); _temp4071;}); _temp4070;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 6)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4069;}); _temp4068;}); break; case 374: _LL4067: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4073=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4073[ 0]=({ struct Cyc_Exp_tok_struct
_temp4074; _temp4074.tag= Cyc_Exp_tok; _temp4074.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Fill_e_struct* _temp4075=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp4075[ 0]=({ struct Cyc_Absyn_Fill_e_struct
_temp4076; _temp4076.tag= Cyc_Absyn_Fill_e; _temp4076.f1= Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)); _temp4076;});
_temp4075;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4074;});
_temp4073;}); break; case 375: _LL4072: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4078=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4078[ 0]=({ struct Cyc_Exp_tok_struct _temp4079; _temp4079.tag= Cyc_Exp_tok;
_temp4079.f1= Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Codegen_e_struct*
_temp4080=( struct Cyc_Absyn_Codegen_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct));
_temp4080[ 0]=({ struct Cyc_Absyn_Codegen_e_struct _temp4081; _temp4081.tag= Cyc_Absyn_Codegen_e;
_temp4081.f1= Cyc_yyget_FnDecl_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 1)); _temp4081;}); _temp4080;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4079;}); _temp4078;}); break; case 376: _LL4077: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4083=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4083[ 0]=({ struct Cyc_Exp_tok_struct
_temp4084; _temp4084.tag= Cyc_Exp_tok; _temp4084.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_UnknownId_e_struct* _temp4085=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp4085[ 0]=({
struct Cyc_Absyn_UnknownId_e_struct _temp4086; _temp4086.tag= Cyc_Absyn_UnknownId_e;
_temp4086.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4086;}); _temp4085;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4084;}); _temp4083;}); break; case 377: _LL4082: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset); break; case 378: _LL4087: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4089=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4089[ 0]=({ struct Cyc_Exp_tok_struct
_temp4090; _temp4090.tag= Cyc_Exp_tok; _temp4090.f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4090;}); _temp4089;}); break; case 379: _LL4088: yyval=*(( void**(*)(
struct _tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)(
yyvs, sizeof( void*), yyvsp_offset - 1); break; case 380: _LL4091: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4093=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4093[ 0]=({ struct Cyc_Exp_tok_struct
_temp4094; _temp4094.tag= Cyc_Exp_tok; _temp4094.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4095=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4095[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4096; _temp4096.tag=
Cyc_Absyn_UnknownId_e; _temp4096.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 1)); _temp4096;}); _temp4095;}), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 1)).last_line)),
Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset - 1)).first_line,(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_line)); _temp4094;}); _temp4093;}); break; case 381: _LL4092:
yyval=( void*)({ struct Cyc_Exp_tok_struct* _temp4098=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4098[ 0]=({ struct Cyc_Exp_tok_struct
_temp4099; _temp4099.tag= Cyc_Exp_tok; _temp4099.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct* _temp4100=(
struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp4100[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp4101; _temp4101.tag=
Cyc_Absyn_UnknownId_e; _temp4101.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 4)); _temp4101;}); _temp4100;}), Cyc_Position_segment_of_abs((*((
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
_temp4099;}); _temp4098;}); break; case 382: _LL4097: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4103=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4103[ 0]=({ struct Cyc_Exp_tok_struct
_temp4104; _temp4104.tag= Cyc_Exp_tok; _temp4104.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4104;}); _temp4103;}); break; case 383: _LL4102: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4106=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4106[ 0]=({ struct Cyc_Exp_tok_struct
_temp4107; _temp4107.tag= Cyc_Exp_tok; _temp4107.f1= Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Struct_e_struct* _temp4108=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp4108[ 0]=({ struct
Cyc_Absyn_Struct_e_struct _temp4109; _temp4109.tag= Cyc_Absyn_Struct_e;
_temp4109.f1= Cyc_yyget_QualId_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset - 3)); _temp4109.f2= 0; _temp4109.f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 1))); _temp4109.f4=
0; _temp4109;}); _temp4108;}), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 3)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4107;});
_temp4106;}); break; case 384: _LL4105: yyval=( void*)({ struct Cyc_Exp_tok_struct*
_temp4111=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4111[ 0]=({ struct Cyc_Exp_tok_struct _temp4112; _temp4112.tag= Cyc_Exp_tok;
_temp4112.f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), yyvsp_offset - 2)), Cyc_Position_segment_of_abs((*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset - 4)).first_line,(*(( struct Cyc_Yyltype*(*)(
struct _tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)(
yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line)); _temp4112;});
_temp4111;}); break; case 385: _LL4110: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4114=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4114[ 0]=({ struct Cyc_ExpList_tok_struct _temp4115; _temp4115.tag= Cyc_ExpList_tok;
_temp4115.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)));
_temp4115;}); _temp4114;}); break; case 386: _LL4113: yyval=( void*)({ struct
Cyc_ExpList_tok_struct* _temp4117=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct)); _temp4117[ 0]=({ struct Cyc_ExpList_tok_struct
_temp4118; _temp4118.tag= Cyc_ExpList_tok; _temp4118.f1=({ struct Cyc_List_List*
_temp4119=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4119->hd=( void*) Cyc_yyget_Exp_tok(*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*),
yyvsp_offset)); _temp4119->tl= 0; _temp4119;}); _temp4118;}); _temp4117;});
break; case 387: _LL4116: yyval=( void*)({ struct Cyc_ExpList_tok_struct*
_temp4121=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp4121[ 0]=({ struct Cyc_ExpList_tok_struct _temp4122; _temp4122.tag= Cyc_ExpList_tok;
_temp4122.f1=({ struct Cyc_List_List* _temp4123=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4123->hd=( void*) Cyc_yyget_Exp_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)); _temp4123->tl=
Cyc_yyget_ExpList_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset - 2));
_temp4123;}); _temp4122;}); _temp4121;}); break; case 388: _LL4120: yyval=( void*)({
struct Cyc_Exp_tok_struct* _temp4125=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp4125[ 0]=({ struct Cyc_Exp_tok_struct
_temp4126; _temp4126.tag= Cyc_Exp_tok; _temp4126.f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f1,(* Cyc_yyget_Int_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset))).f2, Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4126;}); _temp4125;}); break; case 389: _LL4124: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4128=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4128[ 0]=({ struct Cyc_Exp_tok_struct
_temp4129; _temp4129.tag= Cyc_Exp_tok; _temp4129.f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4129;}); _temp4128;}); break; case 390: _LL4127: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4131=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4131[ 0]=({ struct Cyc_Exp_tok_struct
_temp4132; _temp4132.tag= Cyc_Exp_tok; _temp4132.f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(*((
void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset)), Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4132;}); _temp4131;}); break; case 391: _LL4130: yyval=( void*)({ struct
Cyc_Exp_tok_struct* _temp4134=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct)); _temp4134[ 0]=({ struct Cyc_Exp_tok_struct
_temp4135; _temp4135.tag= Cyc_Exp_tok; _temp4135.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs((*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).first_line,(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line));
_temp4135;}); _temp4134;}); break; case 392: _LL4133: yyval=( void*)({ struct
Cyc_QualId_tok_struct* _temp4137=( struct Cyc_QualId_tok_struct*) GC_malloc(
sizeof( struct Cyc_QualId_tok_struct)); _temp4137[ 0]=({ struct Cyc_QualId_tok_struct
_temp4138; _temp4138.tag= Cyc_QualId_tok; _temp4138.f1=({ struct _tuple1*
_temp4139=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4139->f1=(
void*)({ struct Cyc_Absyn_Rel_n_struct* _temp4141=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp4141[ 0]=({ struct Cyc_Absyn_Rel_n_struct
_temp4142; _temp4142.tag= Cyc_Absyn_Rel_n; _temp4142.f1= 0; _temp4142;});
_temp4141;}); _temp4139->f2=({ struct _tagged_string* _temp4140=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4140[ 0]=
Cyc_yyget_String_tok(*(( void**(*)( struct _tagged_ptr2, unsigned int,
unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset));
_temp4140;}); _temp4139;}); _temp4138;}); _temp4137;}); break; case 393: _LL4136:
yyval=*(( void**(*)( struct _tagged_ptr2, unsigned int, unsigned int))
_check_unknown_subscript)( yyvs, sizeof( void*), yyvsp_offset); break; default:
_LL4143:( void) _throw(( void*) Cyc_Yyimpossible);} yyvsp_offset -= yylen;
yyssp_offset -= yylen; yylsp_offset -= yylen;*(( void**(*)( struct _tagged_ptr2,
unsigned int, unsigned int)) _check_unknown_subscript)( yyvs, sizeof( void*), ++
yyvsp_offset)= yyval; yylsp_offset ++; if( yylen == 0){(*(( struct Cyc_Yyltype*(*)(
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
yylsp_offset)).text=( struct _tagged_string)({ char* _temp4145=( char*)"";
struct _tagged_string _temp4146; _temp4146.curr= _temp4145; _temp4146.base=
_temp4145; _temp4146.last_plus_one= _temp4145 + 1; _temp4146;});} else{(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype), yylsp_offset)).last_line=(*((
struct Cyc_Yyltype*(*)( struct _tagged_ptr3, unsigned int, unsigned int))
_check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),( yylsp_offset +
yylen) - 1)).last_line;(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),
yylsp_offset)).last_column=(*(( struct Cyc_Yyltype*(*)( struct _tagged_ptr3,
unsigned int, unsigned int)) _check_unknown_subscript)( yyls, sizeof( struct Cyc_Yyltype),(
yylsp_offset + yylen) - 1)).last_column;} yyn=( int) Cyc_yyr1[
_check_known_subscript_notnull( 394u, yyn)]; yystate= Cyc_yypgoto[
_check_known_subscript_notnull( 108u, yyn - 124)] +*(( short*(*)( struct
_tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( yyss,
sizeof( short), yyssp_offset); if(( yystate >= 0? yystate <= 4222: 0)? Cyc_yycheck[
_check_known_subscript_notnull( 4223u, yystate)] ==*(( short*(*)( struct
_tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( yyss,
sizeof( short), yyssp_offset): 0){ yystate=( int) Cyc_yytable[
_check_known_subscript_notnull( 4223u, yystate)];} else{ yystate=( int) Cyc_yydefgoto[
_check_known_subscript_notnull( 108u, yyn - 124)];} goto yynewstate; yyerrlab:
if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 751u, yystate)]; if( yyn > - 32768? yyn < 4222:
0){ int sze= 0; struct _tagged_string msg; int x; int count; count= 0; for( x=
yyn < 0? - yyn: 0; x < 232u / sizeof( char*); x ++){ if( Cyc_yycheck[
_check_known_subscript_notnull( 4223u, x + yyn)] == x){( sze += Cyc_String_strlen(
Cyc_yytname[ _check_known_subscript_notnull( 232u, x)]) + 15, count ++);}} msg=({
unsigned int _temp4147=( unsigned int)( sze + 15); char* _temp4148=( char*)
GC_malloc_atomic( sizeof( char) * _temp4147); struct _tagged_string _temp4150={
_temp4148, _temp4148, _temp4148 + _temp4147};{ unsigned int _temp4149= _temp4147;
unsigned int i; for( i= 0; i < _temp4149; i ++){ _temp4148[ i]='\000';}};
_temp4150;}); Cyc_String_strcpy( msg,( struct _tagged_string)({ char* _temp4151=(
char*)"parse error"; struct _tagged_string _temp4152; _temp4152.curr= _temp4151;
_temp4152.base= _temp4151; _temp4152.last_plus_one= _temp4151 + 12; _temp4152;}));
if( count < 5){ count= 0; for( x= yyn < 0? - yyn: 0; x < 232u / sizeof( char*);
x ++){ if( Cyc_yycheck[ _check_known_subscript_notnull( 4223u, x + yyn)] == x){
Cyc_String_strcat( msg, count == 0?( struct _tagged_string)({ char* _temp4153=(
char*)", expecting `"; struct _tagged_string _temp4154; _temp4154.curr=
_temp4153; _temp4154.base= _temp4153; _temp4154.last_plus_one= _temp4153 + 14;
_temp4154;}):( struct _tagged_string)({ char* _temp4155=( char*)" or `"; struct
_tagged_string _temp4156; _temp4156.curr= _temp4155; _temp4156.base= _temp4155;
_temp4156.last_plus_one= _temp4155 + 6; _temp4156;})); Cyc_String_strcat( msg,
Cyc_yytname[ _check_known_subscript_notnull( 232u, x)]); Cyc_String_strcat( msg,(
struct _tagged_string)({ char* _temp4157=( char*)"'"; struct _tagged_string
_temp4158; _temp4158.curr= _temp4157; _temp4158.base= _temp4157; _temp4158.last_plus_one=
_temp4157 + 2; _temp4158;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror((
struct _tagged_string)({ char* _temp4159=( char*)"parse error"; struct
_tagged_string _temp4160; _temp4160.curr= _temp4159; _temp4160.base= _temp4159;
_temp4160.last_plus_one= _temp4159 + 12; _temp4160;}));}} goto yyerrlab1;
yyerrlab1: if( yyerrstatus == 3){ if( Cyc_yychar == 0){ return 1;} Cyc_yychar= -
2;} yyerrstatus= 3; goto yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset
== 0){ return 1;} yyvsp_offset --; yystate=( int)*(( short*(*)( struct
_tagged_ptr1, unsigned int, unsigned int)) _check_unknown_subscript)( yyss,
sizeof( short), -- yyssp_offset); yylsp_offset --; yyerrhandle: yyn=( int) Cyc_yypact[
_check_known_subscript_notnull( 751u, yystate)]; if( yyn == - 32768){ goto
yyerrdefault;} yyn += 1; if(( yyn < 0? 1: yyn > 4222)? 1: Cyc_yycheck[
_check_known_subscript_notnull( 4223u, yyn)] != 1){ goto yyerrdefault;} yyn=(
int) Cyc_yytable[ _check_known_subscript_notnull( 4223u, yyn)]; if( yyn < 0){
if( yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn
== 0){ goto yyerrpop;}} if( yyn == 750){ return 0;}*(( void**(*)( struct
_tagged_ptr2, unsigned int, unsigned int)) _check_unknown_subscript)( yyvs,
sizeof( void*), ++ yyvsp_offset)= Cyc_yylval;*(( struct Cyc_Yyltype*(*)( struct
_tagged_ptr3, unsigned int, unsigned int)) _check_unknown_subscript)( yyls,
sizeof( struct Cyc_Yyltype), ++ yylsp_offset)= Cyc_yylloc; goto yynewstate;}
void Cyc_yyprint( int i, void* v){ void* _temp4176= v; struct _tuple16*
_temp4196; struct _tuple16 _temp4198; int _temp4199; void* _temp4201; char
_temp4203; short _temp4205; struct _tagged_string _temp4207; struct Cyc_Core_Opt*
_temp4209; struct Cyc_Core_Opt* _temp4211; struct Cyc_Core_Opt _temp4213; struct
_tagged_string* _temp4214; struct _tuple1* _temp4216; struct _tuple1 _temp4218;
struct _tagged_string* _temp4219; void* _temp4221; _LL4178: if( _temp4176 == Cyc_Okay_tok){
goto _LL4179;} else{ goto _LL4180;} _LL4180: if(*(( void**) _temp4176) == Cyc_Int_tok){
_LL4197: _temp4196=( struct _tuple16*)(( struct Cyc_Int_tok_struct*) _temp4176)->f1;
_temp4198=* _temp4196; _LL4202: _temp4201= _temp4198.f1; goto _LL4200; _LL4200:
_temp4199= _temp4198.f2; goto _LL4181;} else{ goto _LL4182;} _LL4182: if(*((
void**) _temp4176) == Cyc_Char_tok){ _LL4204: _temp4203=( char)(( struct Cyc_Char_tok_struct*)
_temp4176)->f1; goto _LL4183;} else{ goto _LL4184;} _LL4184: if(*(( void**)
_temp4176) == Cyc_Short_tok){ _LL4206: _temp4205=( short)(( struct Cyc_Short_tok_struct*)
_temp4176)->f1; goto _LL4185;} else{ goto _LL4186;} _LL4186: if(*(( void**)
_temp4176) == Cyc_String_tok){ _LL4208: _temp4207=( struct _tagged_string)((
struct Cyc_String_tok_struct*) _temp4176)->f1; goto _LL4187;} else{ goto _LL4188;}
_LL4188: if(*(( void**) _temp4176) == Cyc_Stringopt_tok){ _LL4210: _temp4209=(
struct Cyc_Core_Opt*)(( struct Cyc_Stringopt_tok_struct*) _temp4176)->f1; if(
_temp4209 == 0){ goto _LL4189;} else{ goto _LL4190;}} else{ goto _LL4190;}
_LL4190: if(*(( void**) _temp4176) == Cyc_Stringopt_tok){ _LL4212: _temp4211=(
struct Cyc_Core_Opt*)(( struct Cyc_Stringopt_tok_struct*) _temp4176)->f1; if(
_temp4211 == 0){ goto _LL4192;} else{ _temp4213=* _temp4211; _LL4215: _temp4214=(
struct _tagged_string*) _temp4213.v; goto _LL4191;}} else{ goto _LL4192;}
_LL4192: if(*(( void**) _temp4176) == Cyc_QualId_tok){ _LL4217: _temp4216=(
struct _tuple1*)(( struct Cyc_QualId_tok_struct*) _temp4176)->f1; _temp4218=*
_temp4216; _LL4222: _temp4221= _temp4218.f1; goto _LL4220; _LL4220: _temp4219=
_temp4218.f2; goto _LL4193;} else{ goto _LL4194;} _LL4194: goto _LL4195; _LL4179:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL4177; _LL4181: fprintf( Cyc_Stdio_stderr,"%d",
_temp4199); goto _LL4177; _LL4183: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp4203); goto _LL4177; _LL4185: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp4205); goto _LL4177; _LL4187:({ struct _tagged_string _temp4223= _temp4207;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp4223.last_plus_one - _temp4223.curr,
_temp4223.curr);}); goto _LL4177; _LL4189: fprintf( Cyc_Stdio_stderr,"null");
goto _LL4177; _LL4191:({ struct _tagged_string _temp4224=* _temp4214; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp4224.last_plus_one - _temp4224.curr, _temp4224.curr);});
goto _LL4177; _LL4193: { struct Cyc_List_List* _temp4225= 0; goto _LL4226;
_LL4226:{ void* _temp4227= _temp4221; struct Cyc_List_List* _temp4235; struct
Cyc_List_List* _temp4237; _LL4229: if(( unsigned int) _temp4227 > 1u?*(( int*)
_temp4227) == Cyc_Absyn_Rel_n: 0){ _LL4236: _temp4235=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp4227)->f1; goto _LL4230;} else{ goto
_LL4231;} _LL4231: if(( unsigned int) _temp4227 > 1u?*(( int*) _temp4227) == Cyc_Absyn_Abs_n:
0){ _LL4238: _temp4237=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp4227)->f1; goto _LL4232;} else{ goto _LL4233;} _LL4233: if( _temp4227 ==(
void*) Cyc_Absyn_Loc_n){ goto _LL4234;} else{ goto _LL4228;} _LL4230: _temp4225=
_temp4235; goto _LL4228; _LL4232: _temp4225= _temp4237; goto _LL4228; _LL4234:
goto _LL4228; _LL4228:;} for( 0; _temp4225 != 0; _temp4225=(( struct Cyc_List_List*)
_check_null( _temp4225))->tl){({ struct _tagged_string _temp4239=*(( struct
_tagged_string*)(( struct Cyc_List_List*) _check_null( _temp4225))->hd); fprintf(
Cyc_Stdio_stderr,"%.*s::", _temp4239.last_plus_one - _temp4239.curr, _temp4239.curr);});}({
struct _tagged_string _temp4240=* _temp4219; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp4240.last_plus_one - _temp4240.curr, _temp4240.curr);}); goto _LL4177;}
_LL4195: fprintf( Cyc_Stdio_stderr,"?"); goto _LL4177; _LL4177:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp4241=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4241->v=( void*) Cyc_Lexing_from_file(
f); _temp4241;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}